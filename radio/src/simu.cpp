/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "fx.h"
#include "FXExpression.h"
#include "FXPNGImage.h"
#include <unistd.h>
#include "fxkeys.h"
#include "opentx.h"
#include <time.h>
#include <ctype.h>
#if defined(SIMU_AUDIO)
  #include <SDL.h>
#endif

#if LCD_W > 212
  #define LCD_ZOOM 1
#else
  #define LCD_ZOOM 2
#endif

#define W2 LCD_W*LCD_ZOOM
#define H2 LCD_H*LCD_ZOOM

class Open9xSim: public FXMainWindow
{
  FXDECLARE(Open9xSim)

  public:
    Open9xSim(){};
    Open9xSim(FXApp* a);
    ~Open9xSim();
    void updateKeysAndSwitches(bool start=false);
    long onKeypress(FXObject*,FXSelector,void*);
    long onTimeout(FXObject*,FXSelector,void*);
    void createBitmap(int index, uint16_t *data, int x, int y, int w, int h);
    void makeSnapshot(const FXDrawable* drawable);
    void doEvents();
    void refreshDisplay();
    void setPixel(int x, int y, FXColor color);

  private:
    FXImage       *bmp;
    FXImageFrame  *bmf;
    bool           firstTime;

  public:
    FXSlider      *sliders[NUM_STICKS];
    FXKnob        *knobs[NUM_POTS];
};

// Message Map
FXDEFMAP(Open9xSim) Open9xSimMap[] = {
//Message_Type   _________ ID____Message_Handler_______
  FXMAPFUNC(SEL_TIMEOUT,   2,    Open9xSim::onTimeout),
  FXMAPFUNC(SEL_KEYPRESS,  0,    Open9xSim::onKeypress),
};

FXIMPLEMENT(Open9xSim,FXMainWindow,Open9xSimMap,ARRAYNUMBER(Open9xSimMap))

Open9xSim::Open9xSim(FXApp* a):
  FXMainWindow(a, "OpenTX Simu", NULL, NULL, DECOR_ALL, 20, 90, 0, 0)
{
  firstTime = true;
  memset(displayBuf, 0, DISPLAY_BUFFER_SIZE);
  bmp = new FXPPMImage(getApp(),NULL,IMAGE_OWNED|IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP, W2, H2);

#if defined(SIMU_AUDIO)
  SDL_Init(SDL_INIT_AUDIO);
#endif

  FXHorizontalFrame *hf11=new FXHorizontalFrame(this,LAYOUT_CENTER_X);
  FXHorizontalFrame *hf1=new FXHorizontalFrame(this,LAYOUT_FILL_X);

  //rh lv rv lh
  for (int i=0; i<4; i++) {
    switch (i) {
#define L LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|LAYOUT_FIX_X|LAYOUT_FIX_Y
#undef X0
#define X0 10
#define Y0 10
      case 0:
        sliders[i]=new FXSlider(hf1,NULL,0,L|SLIDER_HORIZONTAL,X0+0,Y0+100,100,20);
        break;
      case 1:
        sliders[i]=new FXSlider(hf1,NULL,0,L|SLIDER_VERTICAL,X0+100,Y0+0,20,100);
        break;
      case 2:
        sliders[i]=new FXSlider(hf1,NULL,0,L|SLIDER_VERTICAL,X0+120,Y0+0,20,100);
        break;
      case 3:
        sliders[i]=new FXSlider(hf1,NULL,0,L|SLIDER_HORIZONTAL,X0+140,Y0+100,100,20);
        break;
      default:;
    }
    sliders[i]->setRange(-1024, 1024);
    sliders[i]->setTickDelta(7);
    sliders[i]->setValue(0);
  }

  for(int i=0; i<NUM_POTS; i++){
    knobs[i]= new FXKnob(hf11,NULL,0,KNOB_TICKS|LAYOUT_LEFT);
    knobs[i]->setRange(-1024, 1024);
    knobs[i]->setValue(0);
  }
  
  bmf = new FXImageFrame(this,bmp);

  updateKeysAndSwitches(true);

  getApp()->addTimeout(this, 2, 100);
}

Open9xSim::~Open9xSim()
{
  StopMainThread();
  StopAudioThread();
  StopEepromThread();
  
  delete bmp;
  delete sliders[0];
  delete sliders[1];
  delete sliders[2];
  delete sliders[3];

  for(int i=0; i<NUM_POTS; i++){
    delete knobs[i];
  }

  delete bmf;

#if defined(SIMU_AUDIO)
  SDL_Quit();
#endif
}

void Open9xSim::createBitmap(int index, uint16_t *data, int x, int y, int w, int h)
{
  FXPNGImage snapshot(getApp(), NULL, IMAGE_OWNED, w, h);

  for (int i=0; i<w; i++) {
    for (int j=0; j<h; j++) {
      display_t z = data[(y+j) * LCD_W + (x+i)];
      FXColor color = FXRGB(255*((z&0xF00)>>8)/0x0f, 255*((z&0x0F0)>>4)/0x0f, 255*(z&0x00F)/0x0f);
      snapshot.setPixel(i, j, color);
    }
  }

  FXFileStream stream;
  char buf[32];
  sprintf(buf,"%02d.png", index);
  if (stream.open(buf, FXStreamSave)) {
    snapshot.savePixels(stream);
    stream.close();
    TRACE("Bitmap %d (w=%d, h=%d) created", index, w, h);
  }
  else {
    TRACE("Bitmap %d (w=%d, h=%d) error", index, w, h);
  }
}

void Open9xSim::makeSnapshot(const FXDrawable* drawable)
{
     // Construct and create an FXImage object
     FXPNGImage snapshot(getApp(), NULL, 0, drawable->getWidth(), drawable->getHeight());
     snapshot.create();

     // Create a window device context and lock it onto the image
     FXDCWindow dc(&snapshot);

     // Draw from the widget to this
     dc.drawArea(drawable, 0, 0, drawable->getWidth(), drawable->getHeight(), 0, 0);

     // Release lock
     dc.end();

     // Grab pixels from server side back to client side
     snapshot.restore();

     // Save recovered pixels to a file
     FXFileStream stream;
     char buf[100];

     do {
       stream.close();
       sprintf(buf,"snapshot_%02d.png", ++g_snapshot_idx);
     } while (stream.open(buf, FXStreamLoad));

     if (stream.open(buf, FXStreamSave)) {
         snapshot.savePixels(stream);
         stream.close();
         printf("Snapshot written: %s\n", buf);
     }
     else {
       printf("Cannot create snapshot %s\n", buf);
     }
}

void Open9xSim::doEvents()
{
  getApp()->runOneEvent(false);
}

long Open9xSim::onKeypress(FXObject*,FXSelector,void*v)
{
  FXEvent *evt=(FXEvent*)v;
  // printf("keypress %x\n", evt->code);
  if (evt->code=='s') {
    makeSnapshot(bmf);
  }
  return 0;
}

void Open9xSim::updateKeysAndSwitches(bool start)
{
  static int keys1[] = {
#if defined(PCBHORUS)
    KEY_Page_Up,   KEY_MENU,
    KEY_Return,    KEY_ENTER,
    KEY_BackSpace, KEY_EXIT,
    KEY_Up,        KEY_UP,
    KEY_Down,      KEY_DOWN,
    KEY_Right,     KEY_RIGHT,
    KEY_Left,      KEY_LEFT,
#elif defined(PCBTARANIS) || defined(PCBFLAMENCO)
    KEY_Page_Up,   KEY_MENU,
    KEY_Page_Down, KEY_PAGE,
    KEY_Return,    KEY_ENTER,
    KEY_BackSpace, KEY_EXIT,
    KEY_Up,        KEY_PLUS,
    KEY_Down,      KEY_MINUS,
#else
    KEY_Return,    KEY_MENU,
    KEY_BackSpace, KEY_EXIT,
    KEY_Right,     KEY_RIGHT,
    KEY_Left,      KEY_LEFT,
    KEY_Up,        KEY_UP,
    KEY_Down,      KEY_DOWN,
#endif
#if defined(ROTARY_ENCODER_NAVIGATION)
    KEY_F,         BTN_REa,
#endif
  };

  for (unsigned int i=0; i<DIM(keys1); i+=2) {
    simuSetKey(keys1[i+1], start ? false : getApp()->getKeyState(keys1[i]));
  }

#ifdef __APPLE__
  // gruvin: Can't use Function keys on the Mac -- too many other app conflicts.
  //         The ordering of these keys, Q/W,E/R,T/Y,U/I matches the on screen 
  //         order of trim sliders
  static FXuint trimKeys[] = { KEY_E, KEY_R, KEY_U, KEY_I, KEY_R, KEY_E, KEY_Y, KEY_T, KEY_Q, KEY_W };
#else
  static FXuint trimKeys[] = { KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8 };
#endif

  for (unsigned i=0; i<DIM(trimKeys); i++) {
    simuSetTrim(i, getApp()->getKeyState(trimKeys[i]));
  }

#define SWITCH_KEY(key, swtch, states) \
  static bool state##key = 0; \
  static int8_t state_##swtch = 2; \
  static int8_t inc_##swtch = 1; \
  if (getApp()->getKeyState(KEY_##key)) { \
    if (!state##key) { \
      state_##swtch = (state_##swtch+inc_##swtch); \
      if (state_##swtch == 1+states) inc_##swtch = -1; \
      else if (state_##swtch == 2) inc_##swtch = 1; \
      state##key = true; \
    } \
  } \
  else { \
    state##key = false; \
  } \
  simuSetSwitch(swtch, state_##swtch-states);

#if defined(PCBFLAMENCO)
  SWITCH_KEY(A, 0, 3);
  SWITCH_KEY(B, 1, 2);
  // SWITCH_KEY(C, 2, 3);
  // SWITCH_KEY(D, 3, 3);
  SWITCH_KEY(E, 4, 2);
  SWITCH_KEY(F, 5, 3);
#elif defined(PCBTARANIS) && defined(REV9E)
  SWITCH_KEY(A, 0, 3);
  SWITCH_KEY(B, 1, 3);
  SWITCH_KEY(C, 2, 3);
  SWITCH_KEY(D, 3, 3);
  SWITCH_KEY(E, 4, 3);
  SWITCH_KEY(F, 5, 3);
  SWITCH_KEY(G, 6, 3);
  SWITCH_KEY(H, 7, 3);
  SWITCH_KEY(I, 8, 3);
  SWITCH_KEY(J, 9, 3);
  SWITCH_KEY(K, 10, 3);
  SWITCH_KEY(L, 11, 3);
  SWITCH_KEY(M, 12, 3);
  SWITCH_KEY(N, 13, 3);
  SWITCH_KEY(O, 14, 3);
  SWITCH_KEY(P, 15, 3);
  SWITCH_KEY(Q, 16, 3);
  SWITCH_KEY(R, 17, 3);
#elif defined(PCBTARANIS)
  SWITCH_KEY(A, 0, 3);
  SWITCH_KEY(B, 1, 3);
  SWITCH_KEY(C, 2, 3);
  SWITCH_KEY(D, 3, 3);
  SWITCH_KEY(E, 4, 3);
  SWITCH_KEY(F, 5, 2);
  SWITCH_KEY(G, 6, 3);
  SWITCH_KEY(H, 7, 2);
#else
  SWITCH_KEY(1, 0, 2);
  SWITCH_KEY(2, 1, 2);
  SWITCH_KEY(3, 2, 2);
  SWITCH_KEY(4, 3, 3);
  SWITCH_KEY(5, 4, 2);
  SWITCH_KEY(6, 5, 2);
  SWITCH_KEY(7, 6, 2);
#endif
}

long Open9xSim::onTimeout(FXObject*, FXSelector, void*)
{
  if (hasFocus()) {
#if defined(PCBSKY9X) && !defined(REVX)
    Coproc_temp = 23;
    Coproc_maxtemp = 28;
#endif

#if defined(PCBSKY9X)
    temperature = 31;
    maxTemperature = 42;
#endif

    updateKeysAndSwitches();
    
#if defined(ROTARY_ENCODER_NAVIGATION)
    static bool rotencAction = false;
    if (getApp()->getKeyState(KEY_G)) {
      if (!rotencAction) g_rotenc[0] += ROTARY_ENCODER_GRANULARITY;
      rotencAction = true;
    }
    else if (getApp()->getKeyState(KEY_D)) {
      if (!rotencAction) g_rotenc[0] -= ROTARY_ENCODER_GRANULARITY;
      rotencAction = true;
    }
    else {
      rotencAction = false;
    }
#endif

#if defined(PCBTARANIS) && defined(REV9E)
    SWITCH_KEY(A, 0, 3);
    SWITCH_KEY(B, 1, 3);
    SWITCH_KEY(C, 2, 3);
    SWITCH_KEY(D, 3, 3);
    SWITCH_KEY(E, 4, 3);
    SWITCH_KEY(F, 5, 2);
    SWITCH_KEY(G, 6, 3);
    SWITCH_KEY(H, 7, 2);
    SWITCH_KEY(I, 8, 3);
    SWITCH_KEY(J, 9, 3);
    SWITCH_KEY(K, 10, 3);
    SWITCH_KEY(L, 11, 3);
    SWITCH_KEY(M, 12, 3);
    SWITCH_KEY(N, 13, 3);
    SWITCH_KEY(O, 14, 3);
    SWITCH_KEY(P, 15, 3);
    SWITCH_KEY(Q, 16, 3);
    SWITCH_KEY(R, 17, 3);
#elif defined(PCBTARANIS)
    SWITCH_KEY(A, 0, 3);
    SWITCH_KEY(B, 1, 3);
    SWITCH_KEY(C, 2, 3);
    SWITCH_KEY(D, 3, 3);
    SWITCH_KEY(E, 4, 3);
    SWITCH_KEY(F, 5, 2);
    SWITCH_KEY(G, 6, 3);
    SWITCH_KEY(H, 7, 2);
#else
    SWITCH_KEY(1, 0, 2);
    SWITCH_KEY(2, 1, 2);
    SWITCH_KEY(3, 2, 2);
    SWITCH_KEY(4, 3, 3);
    SWITCH_KEY(5, 4, 2);
    SWITCH_KEY(6, 5, 2);
    SWITCH_KEY(7, 6, 2);
#endif
  }
  
  per10ms();
  refreshDisplay();
  getApp()->addTimeout(this, 2, 10);
  return 0;
}

#if defined(PCBTARANIS)
  #define BL_COLOR FXRGB(47, 123, 227)
#else
  #define BL_COLOR FXRGB(150, 200, 152)
#endif

void Open9xSim::setPixel(int x, int y, FXColor color)
{
  for (int i=0; i<LCD_ZOOM; ++i) {
    for (int j=0; j<LCD_ZOOM; ++j) {
      bmp->setPixel(LCD_ZOOM*x+i, LCD_ZOOM*y+j, color);
    }
  }
}

void Open9xSim::refreshDisplay()
{
  if (lcd_refresh) {
    lcd_refresh = false;
    FXColor offColor = isBacklightEnable() ? BL_COLOR : FXRGB(200, 200, 200);
#if LCD_W == 128
    FXColor onColor = FXRGB(0, 0, 0);
#endif
    for (int x=0; x<LCD_W; x++) {
      for (int y=0; y<LCD_H; y++) {
#if defined(PCBHORUS)
    	display_t z = lcd_buf[y * LCD_W + x];
    	if (1) {
          FXColor color = FXRGB(255*((z&0xF800)>>11)/0x1f, 255*((z&0x07E0)>>5)/0x3F, 255*(z&0x001F)/0x01F);
          setPixel(x, y, color);
    	}
#elif defined(PCBFLAMENCO)
        display_t z = lcd_buf[y * LCD_W + x];
        if (1) {
          FXColor color = FXRGB(255*((z&0xF00)>>8)/0x0f, 255*((z&0x0F0)>>4)/0x0f, 255*(z&0x00F)/0x0f);
          setPixel(x, y, color);
        }
#elif defined(PCBTARANIS)
        display_t * p = &lcd_buf[y / 2 * LCD_W + x];
        uint8_t z = (y & 1) ? (*p >> 4) : (*p & 0x0F);
        if (z) {
          FXColor color;
          if (isBacklightEnable())
            color = FXRGB(47-(z*47)/15, 123-(z*123)/15, 227-(z*227)/15);
          else
            color = FXRGB(200-(z*200)/15, 200-(z*200)/15, 200-(z*200)/15);
          setPixel(x, y, color);
        }
#else
        if (lcd_buf[x+(y/8)*LCD_W] & (1<<(y%8))) {
          setPixel(x, y, onColor);
        }
#endif
        else {
          setPixel(x, y, offColor);
        }
      }
    }

    bmp->render();
    bmf->setImage(bmp);
  }
}

Open9xSim *th9xSim;
void doFxEvents()
{
  //puts("doFxEvents");
  th9xSim->getApp()->runOneEvent(false);
  th9xSim->refreshDisplay();
}

int main(int argc,char **argv)
{
  // Each FOX GUI program needs one, and only one, application object.
  // The application objects coordinates some common stuff shared between
  // all the widgets; for example, it dispatches events, keeps track of
  // all the windows, and so on.
  // We pass the "name" of the application, and its "vendor", the name
  // and vendor are used to search the registry database (which stores
  // persistent information e.g. fonts and colors).
  FXApp application("OpenTX Simu", "OpenTX");

  // Here we initialize the application.  We pass the command line arguments
  // because FOX may sometimes need to filter out some of the arguments.
  // This opens up the display as well, and reads the registry database
  // so that persistent settings are now available.
  application.init(argc,argv);

  // This creates the main window. We pass in the title to be displayed
  // above the window, and possibly some icons for when its iconified.
  // The decorations determine stuff like the borders, close buttons,
  // drag handles, and so on the Window Manager is supposed to give this
  // window.
  //FXMainWindow *main=new FXMainWindow(&application,"Hello",NULL,NULL,DECOR_ALL);
  th9xSim = new Open9xSim(&application);
  application.create();

  // Pretty self-explanatory:- this shows the window, and places it in the
  // middle of the screen.
#ifndef __APPLE__
  th9xSim->show(PLACEMENT_SCREEN);
#else
  th9xSim->show(); // Otherwise the main window gets centred across my two monitors, split down the middle.
#endif

#if defined(FRSKY) && !defined(FRSKY_SPORT)
  frskyStreaming = 1;
#endif

  printf("Model size = %d\n", (int)sizeof(g_model));

  simuInit();

  StartEepromThread(argc >= 2 ? argv[1] : "eeprom.bin");
  StartAudioThread();
  StartMainThread();

  return application.run();
}

uint16_t anaIn(uint8_t chan)
{
  if (chan<NUM_STICKS)
    return th9xSim->sliders[chan]->getValue();
  else if (chan<NUM_STICKS+NUM_POTS)
    return th9xSim->knobs[chan-NUM_STICKS]->getValue();
#if defined(PCBTARANIS) || defined(PCBFLAMENCO)
  else if (chan == TX_VOLTAGE)
    return 1000;
#elif defined(PCBSKY9X)
  else if (chan == TX_VOLTAGE)
    return 5.1*1500/11.3;
  else if (chan == TX_CURRENT)
    return 100;
#elif defined(PCBGRUVIN9X)
  else if (chan == TX_VOLTAGE)
    return 150;
#else
  else if (chan == TX_VOLTAGE)
    return 1500;
#endif
  else
    return 0;
}

void createBitmap(int index, uint16_t *data, int x, int y, int w, int h)
{
  th9xSim->createBitmap(index, data, x, y, w, h);
}
