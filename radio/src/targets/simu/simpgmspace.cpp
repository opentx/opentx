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

#include "opentx.h"
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#if defined(RTCLOCK)
  #include <time.h>
#endif

#if defined WIN32 || !defined __GNUC__
  #include <direct.h>
#endif

#if defined(SIMU_DISKIO)
  FILE * diskImage = 0;
#endif

#if defined(SIMU_AUDIO) && defined(CPUARM)
  #include <SDL.h>
#endif

volatile uint8_t pina=0xff, pinb=0xff, pinc=0xff, pind, pine=0xff, pinf=0xff, ping=0xff, pinh=0xff, pinj=0, pinl=0;
uint8_t portb, portc, porth=0, dummyport;
uint16_t dummyport16;
const char *eepromFile = NULL;
FILE *fp = NULL;
int g_snapshot_idx = 0;

#if defined(CPUSTM32)
uint32_t Peri1_frequency, Peri2_frequency;
GPIO_TypeDef gpioa, gpiob, gpioc, gpiod, gpioe, gpiof, gpiog, gpioh, gpioi, gpioj;
TIM_TypeDef tim1, tim2, tim3, tim4, tim5, tim6, tim7, tim8, tim9, tim10;
RCC_TypeDef rcc;
DMA_Stream_TypeDef dma2_stream2, dma2_stream6;
DMA_TypeDef dma2;
USART_TypeDef Usart0, Usart1, Usart2, Usart3, Usart4;
#elif defined(CPUARM)
Pio Pioa, Piob, Pioc;
Pwm pwm;
Twi Twio;
Usart Usart0;
Dacc dacc;
Adc Adc0;
#endif

#if defined(EEPROM_RLC)
  extern uint16_t eeprom_pointer;
  extern uint8_t * eeprom_buffer_data;
#else
  uint32_t eeprom_pointer;
  uint8_t * eeprom_buffer_data;
  volatile int32_t eeprom_buffer_size;
  bool eeprom_read_operation;
  #define EESIZE_SIMU (128*4096) // TODO why here?
#endif

#if !defined(EESIZE_SIMU)
  #define EESIZE_SIMU EESIZE
#endif

#if defined(SDCARD) && !defined(SKIP_FATFS_DECLARATION)
char simuSdDirectory[1024] = "";
#endif

uint8_t eeprom[EESIZE_SIMU];
sem_t *eeprom_write_sem;

void simuInit()
{
  for (int i = 0; i <= 17; i++) {
    simuSetSwitch(i, 0);
    simuSetKey(i, false);  // a little dirty, but setting keys that don't exist is perfectly OK here
  }
}

#define NEG_CASE(sw_or_key, pin, mask) \
    case sw_or_key: \
      if ((int)state > 0) pin &= ~(mask); else pin |= (mask); \
      break;
#define POS_CASE(sw_or_key, pin, mask) \
    case sw_or_key: \
      if ((int)state > 0) pin |= (mask); else pin &= ~(mask); \
      break;

#if defined(CPUARM)
  #if defined(PCBTARANIS) && !defined(REV9E)
    #define SWITCH_CASE NEG_CASE
  #else
    #define SWITCH_CASE POS_CASE
  #endif
  #define SWITCH_3_CASE(swtch, pin1, pin2, mask1, mask2) \
    case swtch: \
      if ((int)state < 0) pin1 &= ~(mask1); else pin1 |= (mask1); \
      if ((int)state > 0) pin2 &= ~(mask2); else pin2 |= (mask2); \
      break;
  #define KEY_CASE NEG_CASE
  #define TRIM_CASE NEG_CASE
#else
  #if defined(PCBMEGA2560)
    #define SWITCH_CASE POS_CASE
  #else
    #define SWITCH_CASE NEG_CASE
  #endif
  #define SWITCH_3_CASE(swtch, pin1, pin2, mask1, mask2) \
    case swtch: \
      if ((int)state >= 0) pin1 &= ~(mask1); else pin1 |= (mask1); \
      if ((int)state <= 0) pin2 &= ~(mask2); else pin2 |= (mask2); \
      break;
  #define KEY_CASE POS_CASE
  #define TRIM_CASE KEY_CASE
#endif

void simuSetKey(uint8_t key, bool state)
{
  // TRACE("simuSetKey(%d, %d)", key, state);
  switch (key) {
    KEY_CASE(KEY_MENU, KEYS_GPIO_REG_MENU, KEYS_GPIO_PIN_MENU)
    KEY_CASE(KEY_EXIT, KEYS_GPIO_REG_EXIT, KEYS_GPIO_PIN_EXIT)
#if defined(PCBHORUS)
    KEY_CASE(KEY_ENTER, KEYS_GPIO_REG_ENTER, KEYS_GPIO_PIN_ENTER)
    KEY_CASE(KEY_RIGHT, KEYS_GPIO_REG_RIGHT, KEYS_GPIO_PIN_RIGHT)
    KEY_CASE(KEY_LEFT, KEYS_GPIO_REG_LEFT, KEYS_GPIO_PIN_LEFT)
    KEY_CASE(KEY_UP, KEYS_GPIO_REG_UP, KEYS_GPIO_PIN_UP)
    KEY_CASE(KEY_DOWN, KEYS_GPIO_REG_DOWN, KEYS_GPIO_PIN_DOWN)
#elif defined(PCBTARANIS) || defined(PCBFLAMENCO)
    KEY_CASE(KEY_ENTER, KEYS_GPIO_REG_ENTER, KEYS_GPIO_PIN_ENTER)
    KEY_CASE(KEY_PAGE, KEYS_GPIO_REG_PAGE, KEYS_GPIO_PIN_PAGE)
    KEY_CASE(KEY_MINUS, KEYS_GPIO_REG_MINUS, KEYS_GPIO_PIN_MINUS)
    KEY_CASE(KEY_PLUS, KEYS_GPIO_REG_PLUS, KEYS_GPIO_PIN_PLUS)
#else
    KEY_CASE(KEY_RIGHT, KEYS_GPIO_REG_RIGHT, KEYS_GPIO_PIN_RIGHT)
    KEY_CASE(KEY_LEFT, KEYS_GPIO_REG_LEFT, KEYS_GPIO_PIN_LEFT)
    KEY_CASE(KEY_UP, KEYS_GPIO_REG_UP, KEYS_GPIO_PIN_UP)
    KEY_CASE(KEY_DOWN, KEYS_GPIO_REG_DOWN, KEYS_GPIO_PIN_DOWN)
#endif
#if defined(PCBSKY9X) && !defined(REVX)
    KEY_CASE(BTN_REa, PIOB->PIO_PDSR, 0x40)
#elif defined(PCBGRUVIN9X) || defined(PCBMEGA2560)
    KEY_CASE(BTN_REa, pind, 0x20)
#elif defined(ROTARY_ENCODER_NAVIGATION)
    KEY_CASE(BTN_REa, RotEncoder, 0x20)
#endif
  }
}

void simuSetTrim(uint8_t trim, bool state)
{
  // TRACE("trim=%d state=%d", trim, state);

  switch (trim) {
    TRIM_CASE(0, TRIMS_GPIO_REG_LHL, TRIMS_GPIO_PIN_LHL)
    TRIM_CASE(1, TRIMS_GPIO_REG_LHR, TRIMS_GPIO_PIN_LHR)
    TRIM_CASE(2, TRIMS_GPIO_REG_LVD, TRIMS_GPIO_PIN_LVD)
    TRIM_CASE(3, TRIMS_GPIO_REG_LVU, TRIMS_GPIO_PIN_LVU)
    TRIM_CASE(4, TRIMS_GPIO_REG_RVD, TRIMS_GPIO_PIN_RVD)
    TRIM_CASE(5, TRIMS_GPIO_REG_RVU, TRIMS_GPIO_PIN_RVU)
    TRIM_CASE(6, TRIMS_GPIO_REG_RHL, TRIMS_GPIO_PIN_RHL)
    TRIM_CASE(7, TRIMS_GPIO_REG_RHR, TRIMS_GPIO_PIN_RHR)
#if defined(HORUS)
    TRIM_CASE(8, TRIMS_GPIO_REG_LSU, TRIMS_GPIO_PIN_LSU)
    TRIM_CASE(9, TRIMS_GPIO_REG_LSD, TRIMS_GPIO_PIN_RVU)
    TRIM_CASE(10, TRIMS_GPIO_REG_RSU, TRIMS_GPIO_PIN_RHL)
    TRIM_CASE(11, TRIMS_GPIO_REG_RSD, TRIMS_GPIO_PIN_RHR)
#endif
  }
}

// TODO use a better numbering to allow google tests to work on Taranis
void simuSetSwitch(uint8_t swtch, int8_t state)
{
  // TRACE("simuSetSwitch(%d, %d)", swtch, state);
  switch (swtch) {
#if defined(PCBFLAMENCO)
    // SWITCH_3_CASE(0, SWITCHES_GPIO_REG_A_L, SWITCHES_GPIO_REG_A_H, SWITCHES_GPIO_PIN_A_L, SWITCHES_GPIO_PIN_A_H)
    // SWITCH_CASE(1, SWITCHES_GPIO_REG_B, SWITCHES_GPIO_PIN_B)
    // SWITCH_3_CASE(2, SWITCHES_GPIO_REG_C_L, SWITCHES_GPIO_REG_C_H, SWITCHES_GPIO_PIN_C_L, SWITCHES_GPIO_PIN_C_H)
    // SWITCH_3_CASE(3, SWITCHES_GPIO_REG_D_L, SWITCHES_GPIO_REG_D_H, SWITCHES_GPIO_PIN_D_L, SWITCHES_GPIO_PIN_D_H)
    // SWITCH_CASE(4, SWITCHES_GPIO_REG_E, SWITCHES_GPIO_PIN_E)
    // SWITCH_3_CASE(5, SWITCHES_GPIO_REG_F_H, SWITCHES_GPIO_REG_F_L, SWITCHES_GPIO_PIN_F_H, SWITCHES_GPIO_PIN_F_L)
#elif defined(PCBTARANIS) && defined(REV9E)
    SWITCH_3_CASE(0,  SWITCHES_GPIO_REG_A_L, SWITCHES_GPIO_REG_A_H, SWITCHES_GPIO_PIN_A_L, SWITCHES_GPIO_PIN_A_H)
    SWITCH_3_CASE(1,  SWITCHES_GPIO_REG_B_L, SWITCHES_GPIO_REG_B_H, SWITCHES_GPIO_PIN_B_L, SWITCHES_GPIO_PIN_B_H)
    SWITCH_3_CASE(2,  SWITCHES_GPIO_REG_C_L, SWITCHES_GPIO_REG_C_H, SWITCHES_GPIO_PIN_C_L, SWITCHES_GPIO_PIN_C_H)
    SWITCH_3_CASE(3,  SWITCHES_GPIO_REG_D_L, SWITCHES_GPIO_REG_D_H, SWITCHES_GPIO_PIN_D_L, SWITCHES_GPIO_PIN_D_H)
    SWITCH_3_CASE(4,  SWITCHES_GPIO_REG_E_L, SWITCHES_GPIO_REG_E_H, SWITCHES_GPIO_PIN_E_L, SWITCHES_GPIO_PIN_E_H)
    SWITCH_CASE(5, SWITCHES_GPIO_REG_F, SWITCHES_GPIO_PIN_F)
    SWITCH_3_CASE(6,  SWITCHES_GPIO_REG_G_L, SWITCHES_GPIO_REG_G_H, SWITCHES_GPIO_PIN_G_L, SWITCHES_GPIO_PIN_G_H)
    SWITCH_CASE(7, SWITCHES_GPIO_REG_H, SWITCHES_GPIO_PIN_H)
    SWITCH_3_CASE(8,  SWITCHES_GPIO_REG_I_L, SWITCHES_GPIO_REG_I_H, SWITCHES_GPIO_PIN_I_L, SWITCHES_GPIO_PIN_I_H)
    SWITCH_3_CASE(9,  SWITCHES_GPIO_REG_J_L, SWITCHES_GPIO_REG_J_H, SWITCHES_GPIO_PIN_J_L, SWITCHES_GPIO_PIN_J_H)
    SWITCH_3_CASE(10, SWITCHES_GPIO_REG_K_L, SWITCHES_GPIO_REG_K_H, SWITCHES_GPIO_PIN_K_L, SWITCHES_GPIO_PIN_K_H)
    SWITCH_3_CASE(11, SWITCHES_GPIO_REG_L_L, SWITCHES_GPIO_REG_L_H, SWITCHES_GPIO_PIN_L_L, SWITCHES_GPIO_PIN_L_H)
    SWITCH_3_CASE(12, SWITCHES_GPIO_REG_M_L, SWITCHES_GPIO_REG_M_H, SWITCHES_GPIO_PIN_M_L, SWITCHES_GPIO_PIN_M_H)
    SWITCH_3_CASE(13, SWITCHES_GPIO_REG_N_L, SWITCHES_GPIO_REG_N_H, SWITCHES_GPIO_PIN_N_L, SWITCHES_GPIO_PIN_N_H)
    SWITCH_3_CASE(14, SWITCHES_GPIO_REG_O_L, SWITCHES_GPIO_REG_O_H, SWITCHES_GPIO_PIN_O_L, SWITCHES_GPIO_PIN_O_H)
    SWITCH_3_CASE(15, SWITCHES_GPIO_REG_P_L, SWITCHES_GPIO_REG_P_H, SWITCHES_GPIO_PIN_P_L, SWITCHES_GPIO_PIN_P_H)
    SWITCH_3_CASE(16, SWITCHES_GPIO_REG_Q_L, SWITCHES_GPIO_REG_Q_H, SWITCHES_GPIO_PIN_Q_L, SWITCHES_GPIO_PIN_Q_H)
    SWITCH_3_CASE(17, SWITCHES_GPIO_REG_R_L, SWITCHES_GPIO_REG_R_H, SWITCHES_GPIO_PIN_R_L, SWITCHES_GPIO_PIN_R_H)
#elif defined(PCBTARANIS) || defined(PCBHORUS)
    SWITCH_3_CASE(0,  SWITCHES_GPIO_REG_A_L, SWITCHES_GPIO_REG_A_H, SWITCHES_GPIO_PIN_A_L, SWITCHES_GPIO_PIN_A_H)
    SWITCH_3_CASE(1,  SWITCHES_GPIO_REG_B_L, SWITCHES_GPIO_REG_B_H, SWITCHES_GPIO_PIN_B_L, SWITCHES_GPIO_PIN_B_H)
    SWITCH_3_CASE(2,  SWITCHES_GPIO_REG_C_L, SWITCHES_GPIO_REG_C_H, SWITCHES_GPIO_PIN_C_L, SWITCHES_GPIO_PIN_C_H)
    SWITCH_3_CASE(3,  SWITCHES_GPIO_REG_D_L, SWITCHES_GPIO_REG_D_H, SWITCHES_GPIO_PIN_D_L, SWITCHES_GPIO_PIN_D_H)
    SWITCH_3_CASE(4,  SWITCHES_GPIO_REG_E_H, SWITCHES_GPIO_REG_E_L, SWITCHES_GPIO_PIN_E_H, SWITCHES_GPIO_PIN_E_L)
    SWITCH_CASE(5, SWITCHES_GPIO_REG_F, SWITCHES_GPIO_PIN_F)
    SWITCH_3_CASE(6,  SWITCHES_GPIO_REG_G_L, SWITCHES_GPIO_REG_G_H, SWITCHES_GPIO_PIN_G_L, SWITCHES_GPIO_PIN_G_H)
    SWITCH_CASE(7, SWITCHES_GPIO_REG_H, SWITCHES_GPIO_PIN_H)
#elif defined(PCBSKY9X)
    SWITCH_CASE(0, PIOC->PIO_PDSR, 1<<20)
    SWITCH_CASE(1, PIOA->PIO_PDSR, 1<<15)
    SWITCH_CASE(2, PIOC->PIO_PDSR, 1<<31)
    SWITCH_3_CASE(3, PIOC->PIO_PDSR, PIOC->PIO_PDSR, 0x00004000, 0x00000800)
    SWITCH_CASE(4, PIOA->PIO_PDSR, 1<<2)
    SWITCH_CASE(5, PIOC->PIO_PDSR, 1<<16)
    SWITCH_CASE(6, PIOC->PIO_PDSR, 1<<8)
#elif defined(PCBGRUVIN9X)
    SWITCH_CASE(0, ping, 1<<INP_G_ThrCt)
    SWITCH_CASE(1, ping, 1<<INP_G_RuddDR)
    SWITCH_CASE(2, pinc, 1<<INP_C_ElevDR)
    SWITCH_3_CASE(3, ping, pinb, (1<<INP_G_ID1), (1<<INP_B_ID2))
    SWITCH_CASE(4, pinc, 1<<INP_C_AileDR)
    SWITCH_CASE(5, ping, 1<<INP_G_Gear)
    SWITCH_CASE(6, pinb, 1<<INP_B_Trainer)
#elif defined(PCBMEGA2560)
    SWITCH_CASE(0, ping, 1<<INP_G_ThrCt)
    SWITCH_CASE(1, ping, 1<<INP_G_RuddDR)
    SWITCH_CASE(2, pinc, 1<<INP_L_ElevDR)
    SWITCH_3_CASE(3, pinc, pinc, (1<<INP_C_ID1), (1<<INP_C_ID2))
    SWITCH_CASE(4, pinc, 1<<INP_C_AileDR)
    SWITCH_CASE(5, ping, 1<<INP_G_Gear)
    SWITCH_CASE(6, pinb, 1<<INP_L_Trainer)
#else // PCB9X
#if defined(JETI) || defined(FRSKY) || defined(NMEA) || defined(ARDUPILOT)
    SWITCH_CASE(0, pinc, 1<<INP_C_ThrCt)
    SWITCH_CASE(4, pinc, 1<<INP_C_AileDR)
#else
    SWITCH_CASE(0, pine, 1<<INP_E_ThrCt)
    SWITCH_CASE(4, pine, 1<<INP_E_AileDR)
#endif
    SWITCH_3_CASE(3, ping, pine, (1<<INP_G_ID1), (1<<INP_E_ID2))
    SWITCH_CASE(1, ping, 1<<INP_G_RuddDR)
    SWITCH_CASE(2, pine, 1<<INP_E_ElevDR)
    SWITCH_CASE(5, pine, 1<<INP_E_Gear)
    SWITCH_CASE(6, pine, 1<<INP_E_Trainer)
#endif

    default:
      break;
  }
}

uint16_t getTmr16KHz()
{
  return get_tmr10ms() * 160;
}

#if !defined(PCBTARANIS) && !defined(PCBHORUS)
bool eeprom_thread_running = true;
void *eeprom_write_function(void *)
{
  while (!sem_wait(eeprom_write_sem)) {
    if (!eeprom_thread_running)
      return NULL;
#if defined(CPUARM)
    if (eeprom_read_operation) {
      assert(eeprom_buffer_size);
      eepromReadBlock(eeprom_buffer_data, eeprom_pointer, eeprom_buffer_size);
    }
    else {
#endif
    if (fp) {
      if (fseek(fp, eeprom_pointer, SEEK_SET) == -1)
        perror("error in fseek");
    }
    while (--eeprom_buffer_size) {
      assert(eeprom_buffer_size > 0);
      if (fp) {
        if (fwrite(eeprom_buffer_data, 1, 1, fp) != 1)
          perror("error in fwrite");
#if !defined(CPUARM)
        sleep(5/*ms*/);
#endif
      }
      else {
        memcpy(&eeprom[eeprom_pointer], eeprom_buffer_data, 1);
      }
      eeprom_pointer++;
      eeprom_buffer_data++;
      
      if (fp && eeprom_buffer_size == 1) {
        fflush(fp);
      }
    }
#if defined(CPUARM)
    }
    Spi_complete = 1;
#endif
  }
  return 0;
}
#endif

uint8_t main_thread_running = 0;
char * main_thread_error = NULL;
extern void opentxStart();
void *main_thread(void *)
{
#ifdef SIMU_EXCEPTIONS
  signal(SIGFPE, sig);
  signal(SIGSEGV, sig);

  try {
#endif

#if defined(COLORLCD)
    lcdColorsInit();
#endif

#if defined(CPUARM)
    stackPaint();
#endif
    
    s_current_protocol[0] = 255;

    g_menuStackPtr = 0;
    g_menuStack[0] = menuMainView;
    g_menuStack[1] = menuModelSelect;

#if defined(EEPROM)
    eeReadAll(); // load general setup and selected model
#endif

#if defined(SIMU_DISKIO)
    f_mount(&g_FATFS_Obj, "", 1);
    // call sdGetFreeSectors() now because f_getfree() takes a long time first time it's called
    sdGetFreeSectors();
#endif

#if defined(CPUARM) && defined(SDCARD)
    referenceSystemAudioFiles();
#endif

    if (g_eeGeneral.backlightMode != e_backlight_mode_off) backlightOn(); // on Tx start turn the light on

    if (main_thread_running == 1) {
      opentxStart();
    }
    else {
#if defined(CPUARM)
      eeLoadModel(g_eeGeneral.currModel);
#endif
    }

    s_current_protocol[0] = 0;

#if defined(PCBFLAMENCO)
    menuEntryTime = get_tmr10ms() - 200;
#endif

    while (main_thread_running) {
#if defined(CPUARM)
      doMixerCalculations();
#if defined(FRSKY) || defined(MAVLINK)
      telemetryWakeup();
#endif
      checkTrims();
#endif
      perMain();
      sleep(10/*ms*/);
    }

#if defined(LUA)
    luaClose();
#endif

#ifdef SIMU_EXCEPTIONS
  }
  catch (...) {
    main_thread_running = 0;
  }
#endif

#if defined(SIMU_DISKIO)
  if (diskImage) {
    fclose(diskImage);
  }
#endif

  return NULL;
}

#if defined WIN32 || !defined __GNUC__
#define chdir  _chdir
#define getcwd _getcwd
#endif

pthread_t main_thread_pid;
void StartMainThread(bool tests)
{
#if defined(SDCARD)
  if (strlen(simuSdDirectory) == 0)
    getcwd(simuSdDirectory, 1024);
#endif

#if defined(CPUARM)
  pthread_mutex_init(&mixerMutex, NULL);
  pthread_mutex_init(&audioMutex, NULL);
#endif

  /*
    g_tmr10ms must be non-zero otherwise some SF functions (that use this timer as a marker when it was last executed) 
    will be executed twice on startup. Normal radio does not see this issue because g_tmr10ms is already a big number
    before the first call to the Special Functions. Not so in the simulator.

    There is another issue, some other function static variables depend on this value. If simulator is started 
    multiple times in one Companion session, they are set to their initial values only first time the simulator
    is started. Therefore g_tmr10ms must also be set to non-zero value only the first time, then it must be left
    alone to continue from the previous simulator session value. See the issue #2446

  */
  if (g_tmr10ms == 0) {
    g_tmr10ms = 1;
  }
  
#if defined(RTCLOCK)
  g_rtcTime = time(0);
#endif
  
  main_thread_running = (tests ? 1 : 2);
  pthread_create(&main_thread_pid, NULL, &main_thread, NULL);
}

void StopMainThread()
{
  main_thread_running = 0;
  pthread_join(main_thread_pid, NULL);
}

#if defined(CPUARM)

struct SimulatorAudio {
  int volumeGain;
  int currentVolume;
  uint16_t leftoverData[AUDIO_BUFFER_SIZE];
  int leftoverLen;
  bool threadRunning;
  pthread_t threadPid;
} simuAudio;

bool dacQueue(AudioBuffer *buffer)
{
  return false;
}

void setVolume(uint8_t volume)
{
  simuAudio.currentVolume = min<int>((volumeScale[min<uint8_t>(volume, VOLUME_LEVEL_MAX)] * simuAudio.volumeGain) / 10, 127);
  // TRACE("setVolume(): in: %u, out: %u", volume, simuAudio.currentVolume);
}
#endif

#if defined(SIMU_AUDIO) && defined(CPUARM)

void copyBuffer(uint8_t * dest, uint16_t * buff, unsigned int samples) 
{
  for(unsigned int i=0; i<samples; i++) {
    int sample = ((int32_t)(uint32_t)(buff[i]) - 0x8000);  // conversion from uint16_t 
    *((uint16_t*)dest) = (int16_t)((sample * simuAudio.currentVolume)/127);
    dest += 2;
  }
}

void fillAudioBuffer(void *udata, Uint8 *stream, int len)
{
  SDL_memset(stream, 0, len);

  if (simuAudio.leftoverLen) {
    copyBuffer(stream, simuAudio.leftoverData, simuAudio.leftoverLen);
    len -= simuAudio.leftoverLen*2;
    stream += simuAudio.leftoverLen*2;
    simuAudio.leftoverLen = 0;
    // putchar('l');
  }

  if (audioQueue.filledAtleast(len/(AUDIO_BUFFER_SIZE*2)+1) ) {
    while(true) {
      AudioBuffer *nextBuffer = audioQueue.getNextFilledBuffer();
      if (nextBuffer) {
        if (len >= nextBuffer->size*2) {
          copyBuffer(stream, nextBuffer->data, nextBuffer->size);
          stream += nextBuffer->size*2;
          len -= nextBuffer->size*2;
          // putchar('+');
        }
        else {
          //partial
          copyBuffer(stream, nextBuffer->data, len/2);
          simuAudio.leftoverLen = (nextBuffer->size-len/2);
          memcpy(simuAudio.leftoverData, &nextBuffer->data[len/2], simuAudio.leftoverLen*2);
          len = 0;
          // putchar('p');
          break;
        }
      }
      else {
        break;
      }
    }
  }

  //fill the rest of buffer with silence
  if (len > 0) {
    SDL_memset(stream, 0x8000, len);  // make sure this is silence.
    // putchar('.');
  }
}

void * audioThread(void *)
{
  /*
    Checking here if SDL audio was initialized is wrong, because
    the SDL_CloseAudio() de-initializes it.

    if ( !SDL_WasInit(SDL_INIT_AUDIO) ) {
      fprintf(stderr, "ERROR: couldn't initialize SDL audio support\n");
      return 0;
    }
  */

  SDL_AudioSpec wanted, have;

  /* Set the audio format */
  wanted.freq = AUDIO_SAMPLE_RATE;
  wanted.format = AUDIO_S16SYS;
  wanted.channels = 1;    /* 1 = mono, 2 = stereo */
  wanted.samples = AUDIO_BUFFER_SIZE*2;  /* Good low-latency value for callback */
  wanted.callback = fillAudioBuffer;
  wanted.userdata = NULL;

  /*
    SDL_OpenAudio() internally calls SDL_InitSubSystem(SDL_INIT_AUDIO),
    which initializes SDL Audio subsystem if necessary
  */
  if ( SDL_OpenAudio(&wanted, &have) < 0 ) {
    fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    return 0;
  }
  SDL_PauseAudio(0);

  while (simuAudio.threadRunning) {
    audioQueue.wakeup();
    sleep(1);
  }
  SDL_CloseAudio();
  return 0;
}

void StartAudioThread(int volumeGain)
{ 
  simuAudio.leftoverLen = 0;
  simuAudio.threadRunning = true;
  simuAudio.volumeGain = volumeGain;
  setVolume(VOLUME_LEVEL_DEF);

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  struct sched_param sp;
  sp.sched_priority = SCHED_RR;
  pthread_attr_setschedparam(&attr, &sp);
  pthread_create(&simuAudio.threadPid, &attr, &audioThread, NULL);
  return;
}

void StopAudioThread()
{
  simuAudio.threadRunning = false;
  pthread_join(simuAudio.threadPid, NULL);
}
#endif // #if defined(SIMU_AUDIO) && defined(CPUARM)

pthread_t eeprom_thread_pid;

void StartEepromThread(const char *filename)
{
  eepromFile = filename;
  if (eepromFile) {
    fp = fopen(eepromFile, "rb+");
    if (!fp)
      fp = fopen(eepromFile, "wb+");
    if (!fp) perror("error in fopen");
  }
#ifdef __APPLE__
  eeprom_write_sem = sem_open("eepromsem", O_CREAT, S_IRUSR | S_IWUSR, 0);
#else
  eeprom_write_sem = (sem_t *)malloc(sizeof(sem_t));
  sem_init(eeprom_write_sem, 0, 0);
#endif

#if !defined(PCBTARANIS) && !defined(PCBHORUS)
  eeprom_thread_running = true;
  assert(!pthread_create(&eeprom_thread_pid, NULL, &eeprom_write_function, NULL));
#endif
}

void StopEepromThread()
{
#if !defined(PCBTARANIS) && !defined(PCBFLAMENCO) && !defined(PCBHORUS)
  eeprom_thread_running = false;
  sem_post(eeprom_write_sem);
  pthread_join(eeprom_thread_pid, NULL);
#endif
#ifdef __APPLE__
  //TODO free semaphore eeprom_write_sem
#else
  sem_destroy(eeprom_write_sem);
  free(eeprom_write_sem);
#endif

  if (fp) fclose(fp);
}

void eepromReadBlock (uint8_t * pointer_ram, uint32_t pointer_eeprom, uint32_t size)
{
  assert(size);

  if (fp) {
    // TRACE("EEPROM read (pos=%d, size=%d)", pointer_eeprom, size);
    if (fseek(fp, (long)pointer_eeprom, SEEK_SET)==-1) perror("error in fseek");
    if (fread(pointer_ram, size, 1, fp) <= 0) perror("error in fread");
  }
  else {
    memcpy(pointer_ram, &eeprom[(uint64_t)pointer_eeprom], size);
  }
}

#if defined(PCBTARANIS) || defined(PCBFLAMENCO)
void eepromWriteBlock(uint8_t * pointer_ram, uint32_t pointer_eeprom, uint32_t size)
{
  assert(size);

  if (fp) {
    // TRACE("EEPROM write (pos=%d, size=%d)", pointer_eeprom, size);
    if (fseek(fp, (long)pointer_eeprom, SEEK_SET)==-1) perror("error in fseek");
    if (fwrite(pointer_ram, size, 1, fp) <= 0) perror("error in fwrite");
  }
  else {
    memcpy(&eeprom[(uint64_t)pointer_eeprom], pointer_ram, size);
  }
}

#endif

uint16_t stackAvailable()
{
  return 500;
}

#if 0
static void EeFsDump(){
  for(int i=0; i<EESIZE; i++)
  {
    printf("%02x ",eeprom[i]);
    if(i%16 == 15) puts("");
  }
  puts("");
}
#endif

#if defined(SDCARD) && !defined(SKIP_FATFS_DECLARATION) && !defined(SIMU_DISKIO)
namespace simu {
#include <dirent.h>
#if !defined WIN32
  #include <libgen.h>
#endif
}
#include "ff.h"

#if defined WIN32 || !defined __GNUC__
#include <direct.h>
#include <stdlib.h>
#endif

#include <map>
#include <string>

#if defined(CPUARM)
FATFS g_FATFS_Obj;
#endif

char *convertSimuPath(const char *path)
{
  static char result[1024];
  if (path[0] == '/' && strcmp(simuSdDirectory, "/") != 0)
    sprintf(result, "%s%s", simuSdDirectory, path);
  else
    strcpy(result, path);

  return result;
}

typedef std::map<std::string, std::string> filemap_t;

filemap_t fileMap;

char *findTrueFileName(const char *path)
{
  TRACE("findTrueFileName(%s)", path);
  static char result[1024];
  filemap_t::iterator i = fileMap.find(path);
  if (i != fileMap.end()) {
    strcpy(result, i->second.c_str());
    TRACE("\tfound in map: %s", result);
    return result;
  }
  else {
    //find file
    //add to map
#if defined WIN32 || !defined __GNUC__
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    _splitpath(path, drive, dir, fname, ext);
    std::string fileName = std::string(fname) + std::string(ext);
    std::string dirName = std::string(drive) + std::string(dir);
    std::string searchName = dirName + "*";
    // TRACE("\tsearching for: %s", fileName.c_str());
    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(searchName.c_str(), &ffd);
    if (INVALID_HANDLE_VALUE != hFind) {
      do {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
          //TRACE("comparing with: %s", ffd.cFileName);
          if (!strcasecmp(fileName.c_str(), ffd.cFileName)) {
            strcpy(result, dirName.c_str());
            strcat(result, ffd.cFileName);
            TRACE("\tfound: %s", ffd.cFileName);
            fileMap.insert(filemap_t:: value_type(path, result));
            return result;  
          }
        }
      }
      while (FindNextFile(hFind, &ffd) != 0);
    }
#else
    strcpy(result, path);
    std::string fileName = simu::basename(result);
    strcpy(result, path);
    std::string dirName = simu::dirname(result);
    simu::DIR * dir = simu::opendir(dirName.c_str());
    if (dir) {
      // TRACE("\tsearching for: %s", fileName.c_str());
      for (;;) {
        struct simu::dirent * res = simu::readdir(dir);
        if (res == 0) break;
#if defined(__APPLE__)
        if ((res->d_type == DT_REG) || (res->d_type == DT_LNK)) {
#else
        if ((res->d_type == simu::DT_REG) || (res->d_type == simu::DT_LNK)) {
#endif
          // TRACE("comparing with: %s", res->d_name);
          if (!strcasecmp(fileName.c_str(), res->d_name)) {
            strcpy(result, dirName.c_str());
            strcat(result, "/");
            strcat(result, res->d_name);
            TRACE("\tfound: %s", res->d_name);
            fileMap.insert(filemap_t:: value_type(path, result));
            return result;  
          }
        }
      }
    }
#endif
  }
  TRACE("\tnot found");
  strcpy(result, path);
  return result;
}

FRESULT f_stat (const TCHAR * name, FILINFO *)
{
  char *path = convertSimuPath(name);
  char * realPath = findTrueFileName(path);
  struct stat tmp;
  if (stat(realPath, &tmp)) {
    TRACE("f_stat(%s) = error %d (%s)", path, errno, strerror(errno));
    return FR_INVALID_NAME;
  }
  else {
    TRACE("f_stat(%s) = OK", path);
    return FR_OK;
  }
}

FRESULT f_mount (FATFS* ,const TCHAR*, BYTE opt)
{
  return FR_OK;
}

FRESULT f_open (FIL * fil, const TCHAR *name, BYTE flag)
{
  char *path = convertSimuPath(name);
  char * realPath = findTrueFileName(path);
  if (!(flag & FA_WRITE)) {
    struct stat tmp;
    if (stat(realPath, &tmp)) {
      TRACE("f_open(%s) = INVALID_NAME", path);
      return FR_INVALID_NAME;
    }
    fil->fsize = tmp.st_size;
    fil->fptr = 0;
  }
  fil->fs = (FATFS*)fopen(realPath, (flag & FA_WRITE) ? "wb+" : "rb+");
  fil->fptr = 0;
  if (fil->fs) {
    TRACE("f_open(%s) = %p", path, (FILE*)fil->fs);
    return FR_OK;
  }
  TRACE("f_open(%s) = error %d (%s)", path, errno, strerror(errno));
  return FR_INVALID_NAME;
}

FRESULT f_read (FIL* fil, void* data, UINT size, UINT* read)
{
  if (fil && fil->fs) {
    *read = fread(data, 1, size, (FILE*)fil->fs);
    fil->fptr += *read;
    // TRACE("fread(%p) %u, %u", fil->fs, size, *read);
  }
  return FR_OK;
}

FRESULT f_write (FIL* fil, const void* data, UINT size, UINT* written)
{
  if (fil && fil->fs) {
    *written = fwrite(data, 1, size, (FILE*)fil->fs);
    fil->fptr += size;
    // TRACE("fwrite(%p) %u, %u", fil->fs, size, *written);
  }
  return FR_OK;
}

FRESULT f_lseek (FIL* fil, DWORD offset)
{
  if (fil && fil->fs) fseek((FILE*)fil->fs, offset, SEEK_SET);
  fil->fptr = offset;
  return FR_OK;
}

FRESULT f_close (FIL * fil)
{
  if (fil && fil->fs) {
    TRACE("f_close(%p)", (FILE*)fil->fs);
    fclose((FILE*)fil->fs);
    fil->fs = NULL;
  }
  return FR_OK;
}

FRESULT f_chdir (const TCHAR *name)
{
  chdir(convertSimuPath(name));
  return FR_OK;
}

FRESULT f_opendir (DIR * rep, const TCHAR * name)
{
  char *path = convertSimuPath(name);
  rep->fs = (FATFS *)simu::opendir(path);
  if ( rep->fs ) {
    TRACE("f_opendir(%s) = OK", path);
    return FR_OK;
  }
  TRACE("f_opendir(%s) = error %d (%s)", path, errno, strerror(errno));
  return FR_NO_PATH;
}

FRESULT f_closedir (DIR * rep)
{
  TRACE("f_closedir(%p)", rep);
  simu::closedir((simu::DIR *)rep->fs);
  return FR_OK;
}

FRESULT f_readdir (DIR * rep, FILINFO * fil)
{
  if (!rep->fs) return FR_NO_FILE;
  simu::dirent * ent = simu::readdir((simu::DIR *)rep->fs);
  if (!ent) return FR_NO_FILE;

#if defined(WIN32) || !defined(__GNUC__) || defined(__APPLE__)
  fil->fattrib = (ent->d_type == DT_DIR ? AM_DIR : 0);
#else
  if (ent->d_type == simu::DT_UNKNOWN) {
    fil->fattrib = 0;
    struct stat buf;
    if (stat(ent->d_name, &buf) == 0) {
      fil->fattrib = (S_ISDIR(buf.st_mode) ? AM_DIR : 0);
    }
  }
  else {
    fil->fattrib = (ent->d_type == simu::DT_DIR ? AM_DIR : 0);
  }
#endif

  memset(fil->fname, 0, 13);
  memset(fil->lfname, 0, SD_SCREEN_FILE_LENGTH);
  strncpy(fil->fname, ent->d_name, 13-1);
  strcpy(fil->lfname, ent->d_name);
  // TRACE("f_readdir(): %s", fil->fname);
  return FR_OK;
}

FRESULT f_mkfs (const TCHAR *path, BYTE, UINT)
{
  TRACE("Format SD...");
  return FR_OK;
}

FRESULT f_mkdir (const TCHAR*)
{
  return FR_OK;
}

FRESULT f_unlink (const TCHAR* name)
{
  char *path = convertSimuPath(name);
  if (unlink(path)) {
    TRACE("f_unlink(%s) = error %d (%s)", path, errno, strerror(errno));
    return FR_INVALID_NAME;
  }
  TRACE("f_unlink(%s) = OK", path);
  return FR_OK;
}

FRESULT f_rename(const TCHAR *oldname, const TCHAR *newname)
{
  if (rename(oldname, newname) < 0) {
    TRACE("f_rename(%s, %s) = error %d (%s)", oldname, newname, errno, strerror(errno));
    return FR_INVALID_NAME;
  }
  TRACE("f_rename(%s, %s) = OK", oldname, newname);
  return FR_OK;
}

int f_putc (TCHAR c, FIL * fil)
{
  if (fil && fil->fs) fwrite(&c, 1, 1, (FILE*)fil->fs);
  return FR_OK;
}

int f_puts (const TCHAR * str, FIL * fil)
{
  int n;
  for (n = 0; *str; str++, n++) {
    if (f_putc(*str, fil) == EOF) return EOF;
  }
  return n;
}

int f_printf (FIL *fil, const TCHAR * format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  if (fil && fil->fs) vfprintf((FILE*)fil->fs, format, arglist);
  va_end(arglist);
  return 0;
}

FRESULT f_getcwd (TCHAR *path, UINT sz_path)
{
  char cwd[1024];
  if (!getcwd(cwd, 1024)) {
    TRACE("f_getcwd() = getcwd() error %d (%s)", errno, strerror(errno));
    strcpy(path, ".");
    return FR_NO_PATH;
  }

  if (strlen(cwd) < strlen(simuSdDirectory)) {
    TRACE("f_getcwd() = logic error strlen(cwd) < strlen(simuSdDirectory):  cwd: \"%s\",  simuSdDirectory: \"%s\"", cwd, simuSdDirectory);
    strcpy(path, ".");
    return FR_NO_PATH;
  }

  // remove simuSdDirectory from the cwd
  strcpy(path, cwd + strlen(simuSdDirectory));
  TRACE("f_getcwd() = %s", path);
  return FR_OK;
}

FRESULT f_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs)
{
  // just fake that we always have some clusters free
  *nclst = 10;
  return FR_OK;  
}

#if defined(PCBSKY9X)
int32_t Card_state = SD_ST_MOUNTED;
uint32_t Card_CSD[4]; // TODO elsewhere
#endif

#endif  // #if defined(SDCARD) && !defined(SKIP_FATFS_DECLARATION) && !defined(SIMU_DISKIO)


#if defined(SIMU_DISKIO)
#include "FatFs/diskio.h"
#include <time.h>
#include <stdio.h>

#if defined(CPUARM)
FATFS g_FATFS_Obj = { 0};
#endif

int ff_cre_syncobj (BYTE vol, _SYNC_t* sobj) /* Create a sync object */
{
  return 1;
}

int ff_req_grant (_SYNC_t sobj)        /* Lock sync object */
{
  return 1;
}

void ff_rel_grant (_SYNC_t sobj)        /* Unlock sync object */
{

}

int ff_del_syncobj (_SYNC_t sobj)        /* Delete a sync object */
{
  return 1;
}

DWORD get_fattime (void)
{
  time_t tim = time(0);
  const struct tm * t = gmtime(&tim);

  /* Pack date and time into a DWORD variable */
  return ((DWORD)(t->tm_year - 80) << 25)
    | ((uint32_t)(t->tm_mon+1) << 21)
    | ((uint32_t)t->tm_mday << 16)
    | ((uint32_t)t->tm_hour << 11)
    | ((uint32_t)t->tm_min << 5)
    | ((uint32_t)t->tm_sec >> 1);
}

unsigned int noDiskStatus = 0;

void traceDiskStatus()
{
  if (noDiskStatus > 0) {
    TRACE("disk_status() called %d times", noDiskStatus);
    noDiskStatus = 0;  
  }
}

DSTATUS disk_initialize (BYTE pdrv)
{
  traceDiskStatus();
  TRACE("disk_initialize(%u)", pdrv);
  diskImage = fopen("sdcard.image", "r+");
  return diskImage ? (DSTATUS)0 : (DSTATUS)STA_NODISK;
}

DSTATUS disk_status (BYTE pdrv)
{
  ++noDiskStatus;
  // TRACE("disk_status(%u)", pdrv);
  return (DSTATUS)0;
}

DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
  if (diskImage == 0) return RES_NOTRDY;
  traceDiskStatus();
  TRACE("disk_read(%u, %p, %u, %u)", pdrv, buff, sector, count);
  fseek(diskImage, sector*512, SEEK_SET);
  fread(buff, count, 512, diskImage);
  return RES_OK;
}

DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
  if (diskImage == 0) return RES_NOTRDY;
  traceDiskStatus();
  TRACE("disk_write(%u, %p, %u, %u)", pdrv, buff, sector, count);
  fseek(diskImage, sector*512, SEEK_SET);
  fwrite(buff, count, 512, diskImage);
  return RES_OK;
}

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
  if (diskImage == 0) return RES_NOTRDY;
  traceDiskStatus();
  TRACE("disk_ioctl(%u, %u, %p)", pdrv, cmd, buff);
  if (pdrv) return RES_PARERR;

  DRESULT res;
  BYTE *ptr = (BYTE *)buff;

  if (cmd == CTRL_POWER) {
    switch (*ptr) {
      case 0:         /* Sub control code == 0 (POWER_OFF) */
        res = RES_OK;
        break;
      case 1:         /* Sub control code == 1 (POWER_ON) */
        res = RES_OK;
        break;
      case 2:         /* Sub control code == 2 (POWER_GET) */
        *(ptr+1) = (BYTE)1;  /* fake powered */
        res = RES_OK;
        break;
      default :
        res = RES_PARERR;
    }
    return res;
  }

  switch(cmd) {
/* Generic command (Used by FatFs) */
    case CTRL_SYNC :     /* Complete pending write process (needed at _FS_READONLY == 0) */
      break;

    case GET_SECTOR_COUNT: /* Get media size (needed at _USE_MKFS == 1) */
      {
        struct stat buf;
        if (stat("sdcard.image", &buf) == 0) {
          DWORD noSectors  = buf.st_size / 512;
          *(DWORD*)buff = noSectors;
          TRACE("disk_ioctl(GET_SECTOR_COUNT) = %u", noSectors);
          return RES_OK; 
        }
        return RES_ERROR;
      }

    case GET_SECTOR_SIZE: /* Get sector size (needed at _MAX_SS != _MIN_SS) */
      TRACE("disk_ioctl(GET_SECTOR_SIZE) = 512");
      *(WORD*)buff = 512;
      res = RES_OK;
      break;

    case GET_BLOCK_SIZE : /* Get erase block size (needed at _USE_MKFS == 1) */
      *(WORD*)buff = 512 * 4;
      res = RES_OK;
      break;

    case CTRL_TRIM : /* Inform device that the data on the block of sectors is no longer used (needed at _USE_TRIM == 1) */
      break;

/* Generic command (Not used by FatFs) */
    case CTRL_LOCK : /* Lock/Unlock media removal */
    case CTRL_EJECT: /* Eject media */
    case CTRL_FORMAT: /* Create physical format on the media */
      return RES_PARERR;


/* MMC/SDC specific ioctl command */
    // case MMC_GET_TYPE    10  /* Get card type */
    // case MMC_GET_CSD     11  /* Get CSD */
    // case MMC_GET_CID     12  /* Get CID */
    // case MMC_GET_OCR     13  /* Get OCR */
    // case MMC_GET_SDSTAT    14  /* Get SD status */

/* ATA/CF specific ioctl command */
    // case ATA_GET_REV     20  /* Get F/W revision */
    // case ATA_GET_MODEL   21  /* Get model name */
    // case ATA_GET_SN      22  /* Get serial number */
    default:
      return RES_PARERR;
  }
  return RES_OK;
}

uint32_t sdIsHC()
{
  return sdGetSize() > 2000000;
}

uint32_t sdGetSpeed()
{
  return 330000;
}

#endif // #if defined(SIMU_DISKIO)



bool lcd_refresh = true;
display_t lcd_buf[DISPLAY_BUF_SIZE];

#if !defined(PCBFLAMENCO) && !defined(PCBHORUS)
void lcdSetRefVolt(uint8_t val)
{
}
#endif

void adcPrepareBandgap()
{
}

#if defined(PCBTARANIS)
void lcdOff()
{
}
#endif

void lcdRefresh()
{
#if defined(PCBFLAMENCO)
  TW8823_SendScreen();
#endif

  memcpy(lcd_buf, displayBuf, sizeof(lcd_buf));
  lcd_refresh = true;
}

#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
void pwrInit() { }
void pwrOff() { }
#if defined(REV9E)
uint32_t pwrPressed() { return false; }
#else
uint32_t pwrCheck() { return true; }
#endif
int usbPlugged() { return false; }
void USART_DeInit(USART_TypeDef* ) { }
ErrorStatus RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct) { return SUCCESS; }
ErrorStatus RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct) { return SUCCESS; }
void RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct) { }
void RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct) { }
void RTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct) { }
void RTC_DateStructInit(RTC_DateTypeDef* RTC_DateStruct) { }
void PWR_BackupAccessCmd(FunctionalState NewState) { }
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource) { }
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState) { }
void RCC_RTCCLKCmd(FunctionalState NewState) { }
ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct) { return SUCCESS; }
void USART_SendData(USART_TypeDef* USARTx, uint16_t Data) { }
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG) { return SET; }
void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF) { }
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct) { }
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState) { }
void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState) { }
void RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SR) { }
void RCC_PLLI2SCmd(FunctionalState NewState) { }
void RCC_I2SCLKConfig(uint32_t RCC_I2SCLKSource) { }
void SPI_I2S_DeInit(SPI_TypeDef* SPIx) { }
void I2S_Init(SPI_TypeDef* SPIx, I2S_InitTypeDef* I2S_InitStruct) { }
void I2S_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState) { }
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState) { }
void RCC_LSEConfig(uint8_t RCC_LSE) { }
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG) { return RESET; }
ErrorStatus RTC_WaitForSynchro(void) { return SUCCESS; }
void unlockFlash() { }
void lockFlash() { }
void writeFlash(uint32_t *address, uint32_t *buffer) { SIMU_SLEEP(100); }
uint32_t isBootloaderStart(const void *block) { return 1; }
#if defined(REVPLUS)
void turnBacklightOn(uint8_t level, uint8_t color)
{
  TIM4->CCR4 = (100-level)*color;
  TIM4->CCR2 = (100-level)*(100-color);
}

void turnBacklightOff(void)
{
  TIM4->CCR4 = 0;
  TIM4->CCR2 = 0;
}
#endif

#endif  // #if defined(PCBTARANIS)

#if defined(PCBFLAMENCO)
void i2cWriteTW8823(unsigned char, unsigned char) { }
uint8_t i2cReadBQ24195(uint8_t) { return 0; }
void i2cWriteBQ24195(uint8_t, uint8_t) { }
#endif
