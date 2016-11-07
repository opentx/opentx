/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>

#if defined(_MSC_VER)
  #include <direct.h>
  #define mkdir(s, f) _mkdir(s)
#else
  #include <sys/time.h>
#endif

#if defined(SIMU_DISKIO)
  FILE * diskImage = 0;
#endif

#if defined(SIMU_AUDIO) && defined(CPUARM)
  #include <SDL.h>
#endif

uint8_t MCUCSR, MCUSR, MCUCR;
volatile uint8_t pina=0xff, pinb=0xff, pinc=0xff, pind, pine=0xff, pinf=0xff, ping=0xff, pinh=0xff, pinj=0, pinl=0;
uint8_t portb, portc, porth=0, dummyport;
uint16_t dummyport16;
int g_snapshot_idx = 0;

pthread_t main_thread_pid;
uint8_t main_thread_running = 0;
char * main_thread_error = NULL;

#if defined(STM32)
uint32_t Peri1_frequency, Peri2_frequency;
GPIO_TypeDef gpioa, gpiob, gpioc, gpiod, gpioe, gpiof, gpiog, gpioh, gpioi, gpioj;
TIM_TypeDef tim1, tim2, tim3, tim4, tim5, tim6, tim7, tim8, tim9, tim10;
RCC_TypeDef rcc;
DMA_Stream_TypeDef dma1_stream2, dma1_stream5, dma1_stream7, dma2_stream1, dma2_stream2, dma2_stream5, dma2_stream6;
DMA_TypeDef dma2;
USART_TypeDef Usart0, Usart1, Usart2, Usart3, Usart4;
SysTick_Type systick;
#elif defined(CPUARM)
Pio Pioa, Piob, Pioc;
Pmc pmc;
Ssc ssc;
Pwm pwm;
Twi Twio;
Usart Usart0;
Dacc dacc;
Adc Adc0;
#endif

#if defined(SDCARD) && !defined(SKIP_FATFS_DECLARATION)
char simuSdDirectory[1024] = "";
#endif

void lcdInit()
{
}

void toplcdOff()
{
}

uint16_t getTmr16KHz()
{
#if defined(_MSC_VER)
  return get_tmr10ms() * 16;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec * 2 / 125;
#endif
}

uint16_t getTmr2MHz()
{
#if defined(_MSC_VER)
  return get_tmr10ms() * 125;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec * 2;
#endif
}

void simuInit()
{
#if defined(STM32)
  RCC->CSR = 0;
#endif

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
  #if defined(PCBTARANIS) && !defined(PCBX9E)
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
  // if (state) TRACE("simuSetKey(%d, %d)", key, state);

  switch (key) {
#if !defined(PCBHORUS)
    KEY_CASE(KEY_MENU, KEYS_GPIO_REG_MENU, KEYS_GPIO_PIN_MENU)
    KEY_CASE(KEY_EXIT, KEYS_GPIO_REG_EXIT, KEYS_GPIO_PIN_EXIT)
#endif
#if defined(PCBHORUS)
    KEY_CASE(KEY_PGUP, KEYS_GPIO_REG_MENU, KEYS_GPIO_PIN_MENU)
    KEY_CASE(KEY_PGDN, KEYS_GPIO_REG_EXIT, KEYS_GPIO_PIN_EXIT)
    KEY_CASE(KEY_ENTER, KEYS_GPIO_REG_ENTER, KEYS_GPIO_PIN_ENTER)
    KEY_CASE(KEY_TELEM, KEYS_GPIO_REG_RIGHT, KEYS_GPIO_PIN_RIGHT)
    KEY_CASE(KEY_RADIO, KEYS_GPIO_REG_LEFT, KEYS_GPIO_PIN_LEFT)
    KEY_CASE(KEY_MODEL, KEYS_GPIO_REG_UP, KEYS_GPIO_PIN_UP)
    KEY_CASE(KEY_EXIT, KEYS_GPIO_REG_DOWN, KEYS_GPIO_PIN_DOWN)
#elif defined(PCBFLAMENCO)
    KEY_CASE(KEY_ENTER, KEYS_GPIO_REG_ENTER, KEYS_GPIO_PIN_ENTER)
    KEY_CASE(KEY_PAGE, KEYS_GPIO_REG_PAGE, KEYS_GPIO_PIN_PAGE)
#elif defined(PCBTARANIS)
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
#if defined(PCBSKY9X) && !defined(REVX) && !defined(AR9X)
    KEY_CASE(BTN_REa, PIOB->PIO_PDSR, 0x40)
#elif defined(PCBGRUVIN9X) || defined(PCBMEGA2560)
    KEY_CASE(BTN_REa, pind, 0x20)
#elif defined(PCB9X) && defined(ROTARY_ENCODER_NAVIGATION)
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
#if defined(PCBHORUS)
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
#elif defined(PCBTARANIS) || defined(PCBHORUS)
    SWITCH_3_CASE(0,  SWITCHES_GPIO_REG_A_L, SWITCHES_GPIO_REG_A_H, SWITCHES_GPIO_PIN_A_L, SWITCHES_GPIO_PIN_A_H)
    SWITCH_3_CASE(1,  SWITCHES_GPIO_REG_B_L, SWITCHES_GPIO_REG_B_H, SWITCHES_GPIO_PIN_B_L, SWITCHES_GPIO_PIN_B_H)
    SWITCH_3_CASE(2,  SWITCHES_GPIO_REG_C_L, SWITCHES_GPIO_REG_C_H, SWITCHES_GPIO_PIN_C_L, SWITCHES_GPIO_PIN_C_H)
    SWITCH_3_CASE(3,  SWITCHES_GPIO_REG_D_L, SWITCHES_GPIO_REG_D_H, SWITCHES_GPIO_PIN_D_L, SWITCHES_GPIO_PIN_D_H)
#if !defined(PCBX7D)
    SWITCH_3_CASE(4,  SWITCHES_GPIO_REG_E_L, SWITCHES_GPIO_REG_E_H, SWITCHES_GPIO_PIN_E_L, SWITCHES_GPIO_PIN_E_H)
#endif
    SWITCH_CASE(5, SWITCHES_GPIO_REG_F, SWITCHES_GPIO_PIN_F)
#if !defined(PCBX7D)
    SWITCH_3_CASE(6,  SWITCHES_GPIO_REG_G_L, SWITCHES_GPIO_REG_G_H, SWITCHES_GPIO_PIN_G_L, SWITCHES_GPIO_PIN_G_H)
#endif
    SWITCH_CASE(7, SWITCHES_GPIO_REG_H, SWITCHES_GPIO_PIN_H)
#if defined(PCBX9E)
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
#endif
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
#if defined(TELEMETRY_JETI) || defined(TELEMETRY_FRSKY) || defined(TELEMETRY_NMEA) || defined(TELEMETRY_ARDUPILOT) || defined(TELEMETRY_MAVLINK)
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

void StartSimu(bool tests)
{
  s_current_protocol[0] = 255;
  menuLevel = 0;

  main_thread_running = (tests ? 1 : 2); // TODO rename to simu_run_mode with #define

#if defined(SDCARD)
  if (strlen(simuSdDirectory) == 0) {
    getcwd(simuSdDirectory, 1024);
  }
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

#if defined(SIMU_EXCEPTIONS)
  signal(SIGFPE, sig);
  signal(SIGSEGV, sig);
  try {
#endif

  pthread_create(&main_thread_pid, NULL, &simuMain, NULL);

#if defined(SIMU_EXCEPTIONS)
  }
  catch (...) {
  }
#endif
}

void StopSimu()
{
  main_thread_running = 0;

#if defined(CPUARM)
  pthread_join(mixerTaskId, NULL);
  pthread_join(menusTaskId, NULL);
#endif
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
#endif

void audioConsumeCurrentBuffer()
{
}

#if defined(MASTER_VOLUME)
void setScaledVolume(uint8_t volume)
{
  simuAudio.currentVolume = 127 * volume * simuAudio.volumeGain / VOLUME_LEVEL_MAX / 10;
  // TRACE("setVolume(): in: %u, out: %u", volume, simuAudio.currentVolume);
}

void setVolume(uint8_t volume)
{
}

int32_t getVolume()
{
  return 0;
}
#endif

#if defined(SIMU_AUDIO) && defined(CPUARM)
void copyBuffer(uint8_t * dest, const uint16_t * buff, unsigned int samples)
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
    int len1 = min(len/2, simuAudio.leftoverLen);
    copyBuffer(stream, simuAudio.leftoverData, len1);
    len -= len1*2;
    stream += len1*2;
    simuAudio.leftoverLen -= len1;
    // putchar('l');
    if (simuAudio.leftoverLen) return;		// buffer fully filled
  }

  if (audioQueue.buffersFifo.filledAtleast(len/(AUDIO_BUFFER_SIZE*2)+1) ) {
    while(true) {
      const AudioBuffer * nextBuffer = audioQueue.buffersFifo.getNextFilledBuffer();
      if (nextBuffer) {
        if (len >= nextBuffer->size*2) {
          copyBuffer(stream, nextBuffer->data, nextBuffer->size);
          stream += nextBuffer->size*2;
          len -= nextBuffer->size*2;
          // putchar('+');
          audioQueue.buffersFifo.freeNextFilledBuffer();
        }
        else {
          //partial
          copyBuffer(stream, nextBuffer->data, len/2);
          simuAudio.leftoverLen = (nextBuffer->size-len/2);
          memcpy(simuAudio.leftoverData, &nextBuffer->data[len/2], simuAudio.leftoverLen*2);
          len = 0;
          // putchar('p');
          audioQueue.buffersFifo.freeNextFilledBuffer();
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
  setScaledVolume(VOLUME_LEVEL_DEF);

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

uint16_t stackAvailable()
{
  return 500;
}

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

char * convertSimuPath(const char *path)
{
  static char result[1024];
  if (((path[0] == '/') || (path[0] == '\\')) && (strcmp(simuSdDirectory, "/") != 0))
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
            simu::closedir(dir);
            return result;
          }
        }
      }
      simu::closedir(dir);
    }
#endif
  }
  TRACE("\tnot found");
  strcpy(result, path);
  return result;
}

FRESULT f_stat (const TCHAR * name, FILINFO *fno)
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
    if (fno) {
      fno->fattrib = (tmp.st_mode & S_IFDIR) ? AM_DIR : 0;
    }
    return FR_OK;
  }
}

FRESULT f_mount (FATFS* ,const TCHAR*, BYTE opt)
{
  return FR_OK;
}

FRESULT f_open (FIL * fil, const TCHAR *name, BYTE flag)
{
  char * path = convertSimuPath(name);
  char * realPath = findTrueFileName(path);
  fil->obj.fs = 0;
  if (!(flag & FA_WRITE)) {
    struct stat tmp;
    if (stat(realPath, &tmp)) {
      TRACE("f_open(%s) = INVALID_NAME (FIL %p)", path, fil);
      return FR_INVALID_NAME;
    }
    fil->obj.objsize = tmp.st_size;
    fil->fptr = 0;
  }
  fil->obj.fs = (FATFS*)fopen(realPath, (flag & FA_WRITE) ? ((flag & FA_CREATE_ALWAYS) ? "wb+" : "ab+") : "rb+");
  fil->fptr = 0;
  if (fil->obj.fs) {
    TRACE("f_open(%s, %x) = %p (FIL %p)", path, flag, fil->obj.fs, fil);
    return FR_OK;
  }
  TRACE("f_open(%s) = error %d (%s) (FIL %p)", path, errno, strerror(errno), fil);
  return FR_INVALID_NAME;
}

FRESULT f_read (FIL* fil, void* data, UINT size, UINT* read)
{
  if (fil && fil->obj.fs) {
    *read = fread(data, 1, size, (FILE*)fil->obj.fs);
    fil->fptr += *read;
    // TRACE("fread(%p) %u, %u", fil->obj.fs, size, *read);
  }
  return FR_OK;
}

FRESULT f_write (FIL* fil, const void* data, UINT size, UINT* written)
{
  if (fil && fil->obj.fs) {
    *written = fwrite(data, 1, size, (FILE*)fil->obj.fs);
    fil->fptr += size;
    // TRACE("fwrite(%p) %u, %u", fil->obj.fs, size, *written);
  }
  return FR_OK;
}

TCHAR * f_gets (TCHAR* buff, int len, FIL* fil)
{
  if (fil && fil->obj.fs) {
    buff = fgets(buff, len, (FILE*)fil->obj.fs);
    if (buff != NULL) {
      fil->fptr = *buff;
    }
    // TRACE("fgets(%p) %u, %s", fil->obj.fs, len, buff);
  }
  return buff;
}

FRESULT f_lseek (FIL* fil, DWORD offset)
{
  if (fil && fil->obj.fs) fseek((FILE*)fil->obj.fs, offset, SEEK_SET);
  fil->fptr = offset;
  return FR_OK;
}

UINT f_size(FIL* fil)
{
  if (fil && fil->obj.fs) {
    long curr = ftell((FILE*)fil->obj.fs);
    fseek((FILE*)fil->obj.fs, 0, SEEK_END);
    long size = ftell((FILE*)fil->obj.fs);
    fseek((FILE*)fil->obj.fs, curr, SEEK_SET);
    TRACE("f_size(%p) %u", fil->obj.fs, size);
    return size;
  }
  return 0;
}

FRESULT f_close (FIL * fil)
{
  TRACE("f_close(%p) (FIL:%p)", fil->obj.fs, fil);
  if (fil->obj.fs) {
    fclose((FILE*)fil->obj.fs);
    fil->obj.fs = NULL;
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
  rep->obj.fs = (FATFS *)simu::opendir(path);
  if ( rep->obj.fs ) {
    TRACE("f_opendir(%s) = OK", path);
    return FR_OK;
  }
  TRACE("f_opendir(%s) = error %d (%s)", path, errno, strerror(errno));
  return FR_NO_PATH;
}

FRESULT f_closedir (DIR * rep)
{
  TRACE("f_closedir(%p)", rep);
  if (rep->obj.fs) simu::closedir((simu::DIR *)rep->obj.fs);
  return FR_OK;
}

FRESULT f_readdir (DIR * rep, FILINFO * fil)
{
  simu::dirent * ent;
  if (!rep->obj.fs) return FR_NO_FILE;
  for(;;) {
    ent = simu::readdir((simu::DIR *)rep->obj.fs);
    if (!ent) return FR_NO_FILE;
    if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..") ) break;
  }

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

  memset(fil->fname, 0, SD_SCREEN_FILE_LENGTH);
  strcpy(fil->fname, ent->d_name);
  // TRACE("f_readdir(): %s", fil->fname);
  return FR_OK;
}

FRESULT f_mkfs (const TCHAR* path, BYTE opt, DWORD au, void* work, UINT len)
{
  TRACE("Format SD...");
  return FR_OK;
}

FRESULT f_mkdir (const TCHAR * name)
{
  char * path = convertSimuPath(name);
  if (mkdir(path, 0777)) {
    TRACE("mkdir(%s) = error %d (%s)", path, errno, strerror(errno));
    return FR_INVALID_NAME;
  }
  else {
    TRACE("mkdir(%s) = OK", path);
    return FR_OK;
  }
  return FR_OK;
}

FRESULT f_unlink (const TCHAR * name)
{
  char * path = convertSimuPath(name);
  if (unlink(path)) {
    TRACE("f_unlink(%s) = error %d (%s)", path, errno, strerror(errno));
    return FR_INVALID_NAME;
  }
  else {
    TRACE("f_unlink(%s) = OK", path);
    return FR_OK;
  }
}

FRESULT f_rename(const TCHAR *oldname, const TCHAR *newname)
{
  char old[1024];
  strcpy(old, convertSimuPath(oldname));
  char * path = convertSimuPath(newname);

  if (rename(old, path) < 0) {
    TRACE("f_rename(%s, %s) = error %d (%s)", old, path, errno, strerror(errno));
    return FR_INVALID_NAME;
  }
  TRACE("f_rename(%s, %s) = OK", old, path);
  return FR_OK;
}

int f_putc (TCHAR c, FIL * fil)
{
  if (fil && fil->obj.fs) fwrite(&c, 1, 1, (FILE*)fil->obj.fs);
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
  if (fil && fil->obj.fs) vfprintf((FILE*)fil->obj.fs, format, arglist);
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

  if (sz_path < (strlen(cwd) - strlen(simuSdDirectory))) {
    TRACE("f_getcwd(): buffer too short");
    return FR_NOT_ENOUGH_CORE;
  }

  // remove simuSdDirectory from the cwd
  strcpy(path, cwd + strlen(simuSdDirectory));

  if (strlen(path) == 0) {
    strcpy(path, "/");    // fix for the root directory
  }

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

pthread_mutex_t ioMutex;

int ff_cre_syncobj (BYTE vol, _SYNC_t* sobj) /* Create a sync object */
{
  pthread_mutex_init(&ioMutex, 0);
  return 1;
}

int ff_req_grant (_SYNC_t sobj)        /* Lock sync object */
{
  pthread_mutex_lock(&ioMutex);
  return 1;
}

void ff_rel_grant (_SYNC_t sobj)        /* Unlock sync object */
{
  pthread_mutex_unlock(&ioMutex);
}

int ff_del_syncobj (_SYNC_t sobj)        /* Delete a sync object */
{
  pthread_mutex_destroy(&ioMutex);
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

DRESULT __disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
  if (diskImage == 0) return RES_NOTRDY;
  traceDiskStatus();
  TRACE("disk_read(%u, %p, %u, %u)", pdrv, buff, sector, count);
  fseek(diskImage, sector*512, SEEK_SET);
  fread(buff, count, 512, diskImage);
  return RES_OK;
}

DRESULT __disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
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

void sdInit(void)
{
  // ioMutex = CoCreateMutex();
  // if (ioMutex >= CFG_MAX_MUTEX ) {
  //   // sd error
  //   return;
  // }

  if (f_mount(&g_FATFS_Obj, "", 1) == FR_OK) {
    // call sdGetFreeSectors() now because f_getfree() takes a long time first time it's called
    sdGetFreeSectors();

#if defined(LOG_TELEMETRY)
    f_open(&g_telemetryFile, LOGS_PATH "/telemetry.log", FA_OPEN_ALWAYS | FA_WRITE);
    if (f_size(&g_telemetryFile) > 0) {
      f_lseek(&g_telemetryFile, f_size(&g_telemetryFile)); // append
    }
#endif
  }
  else {
    TRACE("f_mount() failed");
  }
}

void sdDone()
{
  if (sdMounted()) {
    audioQueue.stopSD();
#if defined(LOG_TELEMETRY)
    f_close(&g_telemetryFile);
#endif
    f_mount(NULL, "", 0); // unmount SD
  }
}

uint32_t sdMounted()
{
  return g_FATFS_Obj.fs_type != 0;
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

bool simuLcdRefresh = true;
display_t simuLcdBuf[DISPLAY_BUFFER_SIZE];

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

  memcpy(simuLcdBuf, displayBuf, sizeof(simuLcdBuf));
  simuLcdRefresh = true;
}

void telemetryPortInit()
{
}

#if !defined(PCBFLAMENCO)
display_t simuLcdBackupBuf[DISPLAY_BUFFER_SIZE];
void lcdStoreBackupBuffer()
{
  memcpy(simuLcdBackupBuf, displayBuf, sizeof(simuLcdBackupBuf));
}

int lcdRestoreBackupBuffer()
{
  memcpy(displayBuf, simuLcdBackupBuf, sizeof(displayBuf));
  return 1;
}
#endif

uint32_t pwroffPressed() { return false; }

#if defined(CPUARM) && !defined(PWR_BUTTON_DELAY)
uint32_t pwrCheck() { return true; }
#endif

#if defined(CPUARM)
void pwrOff() { }
#endif

#if defined(STM32)
void pwrInit() { }
int usbPlugged() { return false; }
void USART_DeInit(USART_TypeDef* ) { }
ErrorStatus RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct) { return SUCCESS; }
ErrorStatus RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct) { return SUCCESS; }
void RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef * RTC_TimeStruct)
{
  time_t tme;
  time(&tme);
  struct tm * timeinfo = localtime(&tme);
  RTC_TimeStruct->RTC_Hours = timeinfo->tm_hour;
  RTC_TimeStruct->RTC_Minutes = timeinfo->tm_min;
  RTC_TimeStruct->RTC_Seconds = timeinfo->tm_sec;
}

void RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef * RTC_DateStruct)
{
  time_t tme;
  time(&tme);
  struct tm * timeinfo = localtime(&tme);
  RTC_DateStruct->RTC_Year = timeinfo->tm_year - TM_YEAR_BASE;
  RTC_DateStruct->RTC_Month = timeinfo->tm_mon + 1;
  RTC_DateStruct->RTC_Date = timeinfo->tm_mday;
}

void RTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct) { }
void RTC_DateStructInit(RTC_DateTypeDef* RTC_DateStruct) { }
void PWR_BackupAccessCmd(FunctionalState NewState) { }
void PWR_BackupRegulatorCmd(FunctionalState NewState) { }
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource) { }
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState) { }
void RCC_RTCCLKCmd(FunctionalState NewState) { }
ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct) { return SUCCESS; }
void USART_SendData(USART_TypeDef* USARTx, uint16_t Data) { }
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG) { return SET; }
void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF) { }
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct) { }
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState) { }
void USART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState) { }
void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState) { }
// void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct) { }
// void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct) { }
void TIM_DMAConfig(TIM_TypeDef* TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength) { }
void TIM_DMACmd(TIM_TypeDef* TIMx, uint16_t TIM_DMASource, FunctionalState NewState) { }
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState) { }
void RCC_PLLI2SConfig(uint32_t PLLI2SN, uint32_t PLLI2SR) { }
void RCC_PLLI2SCmd(FunctionalState NewState) { }
void RCC_I2SCLKConfig(uint32_t RCC_I2SCLKSource) { }
void SPI_I2S_DeInit(SPI_TypeDef* SPIx) { }
void I2S_Init(SPI_TypeDef* SPIx, I2S_InitTypeDef* I2S_InitStruct) { }
void I2S_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState) { }
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState) { }
void RCC_LSEConfig(uint8_t RCC_LSE) { }
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks) { };
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG) { return SET; }
ErrorStatus RTC_WaitForSynchro(void) { return SUCCESS; }
void unlockFlash() { }
void lockFlash() { }
void flashWrite(uint32_t *address, uint32_t *buffer) { SIMU_SLEEP(100); }
uint32_t isBootloaderStart(const uint8_t * block) { return 1; }
#endif // defined(PCBTARANIS)

#if defined(PCBFLAMENCO)
void i2cWriteTW8823(unsigned char, unsigned char) { }
uint8_t i2cReadBQ24195(uint8_t) { return 0; }
void i2cWriteBQ24195(uint8_t, uint8_t) { }
#endif

#if defined(PCBHORUS)
void LCD_ControlLight(uint16_t dutyCycle) { }
#endif

void serialPrintf(const char * format, ...) { }
void serialCrlf() { }
void serialPutc(char c) { }
uint16_t stackSize() { return 0; }

void * start_routine(void * attr)
{
  FUNCPtr task = (FUNCPtr)attr;
  task(NULL);
  return NULL;
}

OS_TID CoCreateTask(FUNCPtr task, void *argv, uint32_t parameter, void * stk, uint32_t stksize)
{
  pthread_t tid;
  pthread_create(&tid, NULL, start_routine, (void *)task);
  return tid;
}
