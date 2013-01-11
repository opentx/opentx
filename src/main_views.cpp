/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
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

#include "open9x.h"

#if LCD >= 212
#define BOX_WIDTH     31
#define LBOX_CENTERX  (BOX_WIDTH/2 + 17)
#define LBOX_CENTERY  (DISPLAY_H-BOX_WIDTH/2-10)
#define RBOX_CENTERX  (DISPLAY_W-LBOX_CENTERX)
#define RBOX_CENTERY  LBOX_CENTERY
#define MODELNAME_X   (13)
#define VBATT_X       (MODELNAME_X+26)
#define VBATT_Y       (FH+3)
#define VBATTUNIT_X   (VBATT_X-2)
#define VBATTUNIT_Y   VBATT_Y
#define BITMAP_X      ((DISPLAY_W-64)/2)
#define BITMAP_Y      (DISPLAY_H/2)
#define PHASE_X       BITMAP_X
#define PHASE_Y       (3*FH)
#define TIMERS_X      144
#define TIMERS_R      192
#define REBOOT_X      (DISPLAY_W-FW)
#define VSWITCH_X(i)  (((i>=NUM_CSW*3/4) ? BITMAP_X+28 : ((i>=NUM_CSW/2) ? BITMAP_X+25 : ((i>=NUM_CSW/4) ? 21 : 18))) + 3*i)
#define VSWITCH_Y     (DISPLAY_H-9)
#define BAR_HEIGHT    (31-9)
#define TRIM_LH_X     (32+9)
#define TRIM_LV_X     10
#define TRIM_RV_X     (DISPLAY_W-11)
#define TRIM_RH_X     (DISPLAY_W-32-9)
#else
#define BOX_WIDTH     23
#define LBOX_CENTERX  (DISPLAY_W/4 + 10)
#define LBOX_CENTERY  (DISPLAY_H-9-BOX_WIDTH/2)
#define RBOX_CENTERX  (3*DISPLAY_W/4 - 10)
#define RBOX_CENTERY  LBOX_CENTERY
#define MODELNAME_X   (2*FW-2)
#define PHASE_X       (6*FW)
#define PHASE_Y       (2*FH)
#define VBATT_X       (6*FW)
#define VBATT_Y       (2*FH)
#define VBATTUNIT_X   (VBATT_X-1)
#define VBATTUNIT_Y   (3*FH)
#define REBOOT_X      (20*FW-3)
#define VSWITCH_X(i)  (16 + 3*i)
#define VSWITCH_Y     (DISPLAY_H-8)
#define BAR_HEIGHT    (BOX_WIDTH-1l)
#define TRIM_LH_X     (DISPLAY_W*1/4+2)
#define TRIM_LV_X     3
#define TRIM_RV_X     (DISPLAY_W-4)
#define TRIM_RH_X     (DISPLAY_W*3/4-2)
#endif

#define TRIM_LEN 27
#define MARKER_WIDTH  5
#define BOX_LIMIT     (BOX_WIDTH-MARKER_WIDTH)

void drawPotsBars()
{
  // Optimization by Mike Blandford
  uint8_t x, y, len ;  // declare temporary variables
  for (x=DISPLAY_W/2-5, y=NUM_STICKS; y<NUM_STICKS+NUM_POTS; x+=5, y++) {
    len = ((calibratedStick[y]+RESX)*BAR_HEIGHT/(RESX*2))+1l;  // calculate once per loop
    V_BAR(x, DISPLAY_H-8, len)
  }
}

void doMainScreenGraphics()
{
  lcd_square(LBOX_CENTERX-BOX_WIDTH/2, LBOX_CENTERY-BOX_WIDTH/2, BOX_WIDTH);
  lcd_square(RBOX_CENTERX-BOX_WIDTH/2, RBOX_CENTERY-BOX_WIDTH/2, BOX_WIDTH);

  DO_CROSS(LBOX_CENTERX, LBOX_CENTERY, 3)
  DO_CROSS(RBOX_CENTERX, RBOX_CENTERY, 3)

  int16_t calibStickVert = calibratedStick[CONVERT_MODE(1+1)-1];
  if (g_eeGeneral.throttleReversed && CONVERT_MODE(1+1)-1 == THR_STICK)
    calibStickVert = -calibStickVert;
  lcd_square(LBOX_CENTERX+(calibratedStick[CONVERT_MODE(0+1)-1]*BOX_LIMIT/(2*RESX))-MARKER_WIDTH/2, LBOX_CENTERY-(calibStickVert*BOX_LIMIT/(2*RESX))-MARKER_WIDTH/2, MARKER_WIDTH, ROUND);
  calibStickVert = calibratedStick[CONVERT_MODE(2+1)-1];
  if (g_eeGeneral.throttleReversed && CONVERT_MODE(2+1)-1 == THR_STICK)
    calibStickVert = -calibStickVert;
  lcd_square(RBOX_CENTERX+(calibratedStick[CONVERT_MODE(3+1)-1]*BOX_LIMIT/(2*RESX))-MARKER_WIDTH/2, RBOX_CENTERY-(calibStickVert*BOX_LIMIT/(2*RESX))-MARKER_WIDTH/2, MARKER_WIDTH, ROUND);

#if !defined(PCBX9D)
  drawPotsBars();
#endif
}

void displayTrims(uint8_t phase)
{
  for (uint8_t i=0; i<4; i++) {
    static xcoord_t x[4] = {TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X};
    static uint8_t vert[4] = {0,1,1,0};
    uint8_t xm, ym;
    xm = x[CONVERT_MODE(i+1)-1];

    uint8_t att = ROUND;
    int16_t val = getTrimValue(phase, i);

    if (val < -125 || val > 125)
      att = BLINK|INVERS|ROUND;

    if (val < -(TRIM_LEN+1)*4)
      val = -(TRIM_LEN+1);
    else if (val > (TRIM_LEN+1)*4)
      val = TRIM_LEN+1;
    else
      val /= 4;

    if (vert[i]) {
      ym = 31;
      lcd_vline(xm, ym-TRIM_LEN, TRIM_LEN*2);
      if (i!=2 || !g_model.thrTrim) {
        lcd_vline(xm-1, ym-1,  3);
        lcd_vline(xm+1, ym-1,  3);
      }
      ym -= val;
    }
    else {
      ym = 60;
      lcd_hline(xm-TRIM_LEN, ym, TRIM_LEN*2);
      lcd_hline(xm-1, ym-1,  3);
      lcd_hline(xm-1, ym+1,  3);
      xm += val;
    }
    lcd_square(xm-3, ym-3, 7, att);
  }
}

#if defined(PCBX9D) || defined(PCBACT)
void displaySliders()
{
  for (uint8_t i=NUM_STICKS; i<NUM_STICKS+NUM_POTS; i++) {
    xcoord_t x = (i%2 ? DISPLAY_W-5 : 3);
    int8_t y = (i>NUM_STICKS+1 ? DISPLAY_H/2+1 : 1);
    lcd_vline(x, y, DISPLAY_H/2-2);
    lcd_vline(x+1, y, DISPLAY_H/2-2);
    y += DISPLAY_H/2-4;
    y -= ((calibratedStick[i]+RESX)*(DISPLAY_H/2-4)/(RESX*2));  // calculate once per loop
    lcd_vline(x-1, y, 2);
    lcd_vline(x+2, y, 2);
  }
}
#endif

#if LCD >= 212
void displayTimers()
{
  // Main timer
  if (g_model.timers[0].mode) {
    putsTime(TIMERS_X, 0, s_timerVal[0], MIDSIZE, MIDSIZE);
    putsTmrMode(TIMERS_X-16, 5, g_model.timers[0].mode, SWCONDENSED|SMLSIZE);
    if (g_model.timers[0].remanent) lcd_putcAtt(TIMERS_R, 1, 'R', SMLSIZE);
    if (s_timerState[0]==TMR_BEEPING) {
      lcd_hline(TIMERS_X-6, 2, 4);
      if (BLINK_ON_PHASE)
        lcd_filled_rect(TIMERS_X-17, 0, 69, 12);
    }
  }

  // Second timer
  if (g_model.timers[1].mode) {
    putsTime(TIMERS_X, FH+3, s_timerVal[1], MIDSIZE, MIDSIZE);
    putsTmrMode(TIMERS_X-16, FH+8, g_model.timers[1].mode, SWCONDENSED|SMLSIZE);
    if (g_model.timers[1].remanent) lcd_putcAtt(TIMERS_R, FH+4, 'R', SMLSIZE);
    if (s_timerState[1]==TMR_BEEPING) {
      lcd_hline(TIMERS_X-6, FH+5, 4);
      if (BLINK_ON_PHASE)
        lcd_filled_rect(TIMERS_X-17, FH+3, 69, 12);
    }
  }
}
#else
void displayTimers()
{
  // Main timer
  if (g_model.timers[0].mode) {
    uint8_t att = DBLSIZE | (s_timerState[0]==TMR_BEEPING ? BLINK|INVERS : 0);
    putsTime(12*FW+2, FH*2, s_timerVal[0], att, att);
    putsTmrMode(s_timerVal[0] >= 0 ? 9*FW-FW/2+3 : 9*FW-FW/2-4, FH*3, g_model.timers[0].mode, SWCONDENSED);
  }
}
#endif


#if defined(CPUARM)
void displayVoltage()
{
  if (g_vbat100mV > g_eeGeneral.vBatWarn && g_eeGeneral.temperatureWarn && getTemperature() >= g_eeGeneral.temperatureWarn) {
    putsTelemetryValue(6*FW-1, 3*FH, getTemperature(), UNIT_DEGREES, BLINK|INVERS|DBLSIZE);
  }
#if defined(PCBSKY9X)
  else if (g_vbat100mV > g_eeGeneral.vBatWarn && g_eeGeneral.mAhWarn && (g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.currentCalib)/8192/36) / 500 >= g_eeGeneral.mAhWarn) {
    putsTelemetryValue(7*FW-1, 3*FH, (g_eeGeneral.mAhUsed + Current_used*(488 + g_eeGeneral.currentCalib)/8192/36)/10, UNIT_MAH, BLINK|INVERS|DBLSIZE);
  }
#endif
  else {
    LcdFlags att = (g_vbat100mV <= g_eeGeneral.vBatWarn ? BLINK|INVERS : 0) | MIDSIZE;
    putsVBat(VBATT_X-1, VBATT_Y, att|NO_UNIT);
#if defined(PCBX9D) || defined(PCBACT)
    lcd_putcAtt(VBATTUNIT_X, VBATTUNIT_Y, 'v', MIDSIZE);
#else
    lcd_putc(VBATT_X, VBATTUNIT_Y, 'V');
#endif
  }
}
#else
void displayVoltage()
{
  LcdFlags att = (g_vbat100mV <= g_eeGeneral.vBatWarn ? BLINK|INVERS : 0) | MIDSIZE;
  putsVBat(VBATT_X-1, VBATT_Y, att|NO_UNIT);
  lcd_putc(VBATT_X, VBATTUNIT_Y, 'V');
}
#endif

#if defined(PCBX9D) || defined(PCBACT)
#define EVT_KEY_MODEL_MENU   EVT_KEY_BREAK(KEY_MENU)
#define EVT_KEY_GENERAL_MENU EVT_KEY_LONG(KEY_MENU)
#define EVT_KEY_TELEMETRY    EVT_KEY_LONG(KEY_PAGE)
#define EVT_KEY_STATISTICS   EVT_KEY_LONG(KEY_ENTER)
#else
#define EVT_KEY_MODEL_MENU   EVT_KEY_LONG(KEY_RIGHT)
#define EVT_KEY_GENERAL_MENU EVT_KEY_LONG(KEY_LEFT)
#define EVT_KEY_TELEMETRY    EVT_KEY_LONG(KEY_DOWN)
#define EVT_KEY_STATISTICS   EVT_KEY_LONG(KEY_UP)
#endif

#if defined(PCBX9D) || defined(PCBACT)
void menuMainViewChannelsMonitor(uint8_t event)
{
  switch(event) {
    case EVT_KEY_BREAK(KEY_PAGE):
    case EVT_KEY_BREAK(KEY_EXIT):
      chainMenu(menuMainView);
      return;
  }

  return menuChannelsMonitor(event);
}
#endif

void menuMainView(uint8_t event)
{
#if !defined(PCBX9D)
  uint8_t view = g_eeGeneral.view;
  uint8_t view_base = view & 0x0f;
#endif

  uint8_t _event = event;
  if (s_global_warning) {
    event = 0;
  }

  switch(event)
  {
    case EVT_ENTRY:
      killEvents(KEY_EXIT);
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      break;

#if !defined(PCBX9D)
    /* TODO if timer2 is OFF, it's possible to use this timer2 as in er9x...
    case EVT_KEY_BREAK(KEY_MENU):
      if (view_base == VIEW_TIMER2) {
        Timer2_running = !Timer2_running;
        AUDIO_KEYPAD_UP();
      }
    break;
    */

    case EVT_KEY_BREAK(KEY_RIGHT):
    case EVT_KEY_BREAK(KEY_LEFT):
      if (view_base <= VIEW_INPUTS) {
#if defined(PCBSKY9X)
        if (view_base == VIEW_INPUTS)
          g_eeGeneral.view ^= ALTERNATE_VIEW;
        else
          g_eeGeneral.view = (g_eeGeneral.view + (4*ALTERNATE_VIEW) + ((event==EVT_KEY_BREAK(KEY_LEFT)) ? -ALTERNATE_VIEW : ALTERNATE_VIEW)) % (4*ALTERNATE_VIEW);
#else
        g_eeGeneral.view ^= ALTERNATE_VIEW;
#endif
        eeDirty(EE_GENERAL);
        AUDIO_KEYPAD_UP();
      }
      break;
#endif

#if !defined(READONLY)
#if !defined(PCBX9D) && !defined(PCBACT)
    case EVT_KEY_LONG(KEY_MENU):// go to last menu
      pushMenu(lastPopMenu());
      killEvents(event);
      break;
#endif

    case EVT_KEY_MODEL_MENU:
      pushMenu(menuModelSelect);
      killEvents(event);
      break;

    case EVT_KEY_GENERAL_MENU:
      pushMenu(menuGeneralSetup);
      killEvents(event);
      break;
#endif

#if defined(PCBX9D) || defined(PCBACT)
    case EVT_KEY_BREAK(KEY_PAGE):
      eeDirty(EE_GENERAL);
      g_eeGeneral.view += 1;
      if (g_eeGeneral.view >= VIEW_COUNT) {
        g_eeGeneral.view = 0;
        chainMenu(menuMainViewChannelsMonitor);
        return;
      }
      break;
#else
    case EVT_KEY_BREAK(KEY_UP):
    case EVT_KEY_BREAK(KEY_DOWN):
      g_eeGeneral.view = (event == EVT_KEY_BREAK(KEY_UP) ? (view_base == VIEW_COUNT-1 ? 0 : view_base+1) : (view_base == 0 ? VIEW_COUNT-1 : view_base-1));
      eeDirty(EE_GENERAL);
      AUDIO_KEYPAD_UP();
      break;
#endif

    case EVT_KEY_STATISTICS:
      chainMenu(menuStatisticsView);
      killEvents(event);
      return;

    case EVT_KEY_TELEMETRY:
#if defined(FRSKY)
      chainMenu(menuTelemetryFrsky);
#elif defined(JETI)
      JETI_EnableRXD(); // enable JETI-Telemetry reception
      chainMenu(menuTelemetryJeti);
#elif defined(ARDUPILOT)
      ARDUPILOT_EnableRXD(); // enable ArduPilot-Telemetry reception
      chainMenu(menuTelemetryArduPilot);
#elif defined(NMEA)
      NMEA_EnableRXD(); // enable NMEA-Telemetry reception
      chainMenu(menuTelemetryNMEA);
#elif defined(MAVLINK)
      chainMenu(menuTelemetryMavlink);
#else
      chainMenu(menuStatisticsDebug);
#endif
      killEvents(event);
      return;

    case EVT_KEY_FIRST(KEY_EXIT):
      if (s_timerState[0]==TMR_BEEPING) {
        s_timerState[0] = TMR_STOPPED;
      }
      else if (s_global_warning) {
        s_global_warning = NULL;
      }
#if defined(GVARS)
      else if (s_gvar_timer > 0) {
        s_gvar_timer = 0;
      }
#endif
#if LCD < 212
      else if (view == VIEW_TIMER2) {
        resetTimer(1);
      }
#endif
      else {
        resetTimer(0);
      }
      AUDIO_KEYPAD_UP();
      break;

    case EVT_KEY_LONG(KEY_EXIT):
      resetAll();
      AUDIO_KEYPAD_UP();
      break;
  }

  {
    // Flight Phase Name
    uint8_t phase = s_perout_flight_phase;
    lcd_putsnAtt(PHASE_X, PHASE_Y, g_model.phaseData[phase].name, sizeof(g_model.phaseData[phase].name), ZCHAR);

    // Model Name
    putsModelName(MODELNAME_X, 0*FH, g_model.name, g_eeGeneral.currModel, MIDSIZE);

    // Main Voltage (or alarm if any)
    displayVoltage();

    // Timers
    displayTimers();

    // Trims sliders
    displayTrims(phase);
  }

#if defined(PCBX9D) || defined(PCBACT)
  // Sliders (Pots / Sliders)
  displaySliders();

#if defined(PCBX9D)
  // Model Bitmap
  lcd_img(BITMAP_X, BITMAP_Y, modelBitmap, 0, 0);
#endif

  // Switches
  for (uint8_t i=0; i<8; i++) {
    uint8_t sw;
    int16_t val;
    // TODO simplify this + reuse code in checkSwitches() + Menu MODELSETUP
    switch(i) {
      case 5:
        sw = getValue(MIXSRC_SF-1) > 0 ? 3*i+2 : 3*i+1;
        break;
      case 6:
        val = getValue(MIXSRC_SG-1);
        sw = ((val < 0) ? 3*i : ((val == 0) ? 3*i+1 : 3*i+2));
        break;     
      case 7:
        sw = getValue(MIXSRC_SH-1) > 0 ? 3*i+1 : 3*i;
        break;
      default:
      {
        val = getValue(MIXSRC_SA+i-1);
        sw = ((val < 0) ? 3*i+1 : ((val == 0) ? 3*i+2 : 3*i+3));
        break;
      }
    }
    putsSwitches((g_eeGeneral.view == VIEW_SWITCHES) ? (i<4 ? 3*FW+2 : 8*FW-1) : (i<4 ? 8*FW+3 : 24*FW+1), (i%4)*FH+3*FH, sw, 0);
  }

  if (g_eeGeneral.view == VIEW_INPUTS) {
    // Sticks
    doMainScreenGraphics();
  }
  else {
    // Custom Switches
    uint8_t sw = 0;
    for (uint8_t line=0; line<4; line++) {
      for (uint8_t col=0; col<8; col++) {
        uint8_t x = DISPLAY_W/2+7*FW+col*FW;
        uint8_t y = DISPLAY_H/2-7+line*8;
        lcd_putcAtt(x, y, sw>=9 ? 'A'+sw-9 : '1'+sw, SMLSIZE);
        if (getSwitch(DSW(SW_SW1+sw), 0))
          lcd_filled_rect(x-1, y-1, 6, 8);
        sw++;
      }
    }
  }
#else // PCBX9D
  if (view_base < VIEW_INPUTS) {
    // scroll bar
    lcd_hlineStip(38, 34, 54, DOTTED);
#if defined(PCBSKY9X)
    lcd_hline(38 + (g_eeGeneral.view / ALTERNATE_VIEW) * 13, 34, 13, SOLID);
#else
    lcd_hline((g_eeGeneral.view & ALTERNATE_VIEW) ? 64 : 38, 34, 26, SOLID);
#endif

    for (uint8_t i=0; i<8; i++) {
      uint8_t x0,y0;
#if defined(PCBSKY9X)
      uint8_t chan = 8*(g_eeGeneral.view / ALTERNATE_VIEW) + i;
#else
      uint8_t chan = (g_eeGeneral.view & ALTERNATE_VIEW) ? 8+i : i;
#endif

      int16_t val = g_chans512[chan];

      switch(view_base)
      {
        case VIEW_OUTPUTS_VALUES:
          x0 = (i%4*9+3)*FW/2;
          y0 = i/4*FH+40;
#if defined(PPM_UNIT_US)
          lcd_outdezAtt(x0+4*FW , y0, PPM_CH_CENTER(chan)+val/2, 0);
#elif defined(PPM_UNIT_PERCENT_PREC1)
          lcd_outdezAtt(x0+4*FW , y0, calcRESXto1000(val), PREC1);
#else
          lcd_outdezAtt(x0+4*FW , y0, calcRESXto1000(val)/10, 0); // G: Don't like the decimal part*
#endif
          break;

        case VIEW_OUTPUTS_BARS:
#define WBAR2 (50/2)
          x0       = i<4 ? DISPLAY_W/4+2 : DISPLAY_W*3/4-2;
          y0       = 38+(i%4)*5;

          uint16_t lim = g_model.extendedLimits ? 640*2 : 512*2;
          int8_t len = (abs(val) * WBAR2 + lim/2) / lim;

          if(len>WBAR2)  len = WBAR2;  // prevent bars from going over the end - comment for debugging
          lcd_hlineStip(x0-WBAR2, y0, WBAR2*2+1, DOTTED);
          lcd_vline(x0,y0-2,5);
          if (val>0)
            x0+=1;
          else
            x0-=len;
          lcd_hline(x0,y0+1,len);
          lcd_hline(x0,y0-1,len);
          break;
      }
    }
  }
  else if (view_base == VIEW_INPUTS) {
    if (view == VIEW_INPUTS) {
      // Sticks + Pots
      doMainScreenGraphics();

      // Switches
      for (uint8_t i=0; i<6; i++) {
        int8_t sw1 = (i<3 ? 1+i : 4+i);
        int8_t sw2 = (sw1 == 9 ? (getSwitch(4, 0) ? 4 : (getSwitch(5, 0) ? 5 : 6)) : sw1);
        putsSwitches(i<3 ? 2*FW-3: 17*FW+2, (i%3)*FH+4*FH+1, sw2, getSwitch(sw1, 0) ? INVERS : 0);
      }
    }
    else {
#if defined(PCBGRUVIN9X) && defined(ROTARY_ENCODERS)
      for (uint8_t i=0; i<NUM_ROTARY_ENCODERS; i++) {
        int16_t val = getRotaryEncoder(i);
        int8_t len = limit((int16_t)0, (int16_t)(((val+1024) * BAR_HEIGHT) / 2048), (int16_t)BAR_HEIGHT);
#if defined(EXTRA_ROTARY_ENCODERS)
#define V_BAR_W 5
        V_BAR(DISPLAY_W/2-8+V_BAR_W*i, DISPLAY_H-8, len)
#else //EXTRA_ROTARY_ENCODERS
#define V_BAR_W 5
        V_BAR(DISPLAY_W/2-3+V_BAR_W*i, DISPLAY_H-8, len)
#endif //EXTRA_ROTARY_ENCODERS
      }
#endif // PCBGRUVIN9X && ROTARY_ENCODERS

      // Curstom Switches
#if defined(PCBSKY9X)
      for (uint8_t i=0; i<NUM_CSW; i++) {
        int8_t len = getSwitch(DSW(SW_SW1)+i, 0) ? BAR_HEIGHT : 1;
        uint8_t x = VSWITCH_X(i);
        lcd_vline(x-1, VSWITCH_Y-len, len);
        lcd_vline(x,   VSWITCH_Y-len, len);
      }
#elif defined(PCBGRUVIN9X) && defined(EXTRA_ROTARY_ENCODERS)
      for (uint8_t i=0; i<NUM_CSW; i++)
        putsSwitches(2*FW-2 + (i/3)*(4*FW-2) + (i/3>1 ? 3*FW+6 : 0), 4*FH+1 + (i%3)*FH, 10+i, getSwitch(10+i, 0) ? INVERS : 0);
#elif defined(PCBGRUVIN9X)
      for (uint8_t i=0; i<NUM_CSW; i++)
        putsSwitches(2*FW-2 + (i/3)*(4*FW) + (i/3>1 ? 3*FW : 0), 4*FH+1 + (i%3)*FH, 10+i, getSwitch(10+i, 0) ? INVERS : 0);
#else
      for (uint8_t i=0; i<NUM_CSW; i++)
        putsSwitches(2*FW-2 + (i/3)*(5*FW), 4*FH+1 + (i%3)*FH, 10+i, getSwitch(10+i, 0) ? INVERS : 0);
#endif
    }
  }
  else { // timer2
    putsTime(33+FW+2, FH*5, s_timerVal[1], DBLSIZE, DBLSIZE);
    putsTmrMode(s_timerVal[1] >= 0 ? 20-FW/2+5 : 20-FW/2-2, FH*6, g_model.timers[1].mode, SWCONDENSED);
    // lcd_outdezNAtt(33+11*FW, FH*6, s_timerVal_10ms[1], LEADING0, 2); // 1/100s
  }
#endif // PCBX9D

  // And ! in case of unexpected shutdown
  if (unexpectedShutdown) {
    lcd_putcAtt(REBOOT_X, 0*FH, '!', INVERS);
  }

  if (s_global_warning) {
    s_warning = s_global_warning;
    displayWarning(_event);
    if (!s_warning) s_global_warning = NULL;
    s_warning = NULL;
  }

#if defined(GVARS)
  else if (s_gvar_timer > 0) {
    s_gvar_timer--;
    s_warning = PSTR("Global Variable");
    displayBox();
#if defined(CPUM64)
    lcd_putsAtt(16+4*FW, 5*FH, PSTR("[\010]"), BOLD);
    lcd_outdezAtt(16+4*FW+4*FW+FW/2, 5*FH, GVAR_VALUE(s_gvar_last, s_perout_flight_phase), BOLD);
#else
    lcd_putsnAtt(16, 5*FH, g_model.gvarsNames[s_gvar_last], sizeof(gvar_name_t), ZCHAR);
    lcd_putsAtt(16+7*FW, 5*FH, PSTR("[\010]"), BOLD);
    lcd_outdezAtt(16+7*FW+4*FW+FW/2, 5*FH, GVAR_VALUE(s_gvar_last, s_perout_flight_phase), BOLD);
#endif
    s_warning = NULL;
  }
#endif

#if defined(DSM2)
  if (s_bind_mode) // Issue 98
    lcd_putsAtt(15*FW, 0, PSTR("BIND"), 0);
#endif
}

