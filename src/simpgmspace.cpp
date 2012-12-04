/*
 * Authors (alphabetical order)
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
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>

volatile uint8_t pinb=0xff, pinc=0xff, pind, pine=0xff, ping=0xff, pinh=0xff, pinj=0xff, pinl=0;
uint8_t portb, portc, porth=0, dummyport;
uint16_t dummyport16;
const char *eepromFile = NULL;
FILE *fp = NULL;

#if defined(PCBSKY9X)
Pio Pioa, Piob, Pioc;
Pwm pwm;
Twi Twio;
Usart Usart0;
Dacc dacc;
uint32_t eeprom_pointer;
char* eeprom_buffer_data;
volatile int32_t eeprom_buffer_size;
bool eeprom_read_operation;
#define EESIZE (128*4096)
void configure_pins( uint32_t pins, uint16_t config ) { }
#else
extern uint16_t eeprom_pointer;
extern const char* eeprom_buffer_data;
#endif

uint8_t eeprom[EESIZE];
sem_t *eeprom_write_sem;

#if defined(PCBSKY9X)
#define SWITCH_CASE(swtch, pin, bit) \
    case -DSW(swtch): \
      pin &= ~(1<<bit); \
      break; \
    case DSW(swtch): \
      pin |= (1<<bit); \
      break;
#else
#define SWITCH_CASE(swtch, pin, bit) \
    case DSW(swtch): \
      pin &= ~(1<<bit); \
      break; \
    case -DSW(swtch): \
      pin |= (1<<bit); \
      break;
#endif

void setSwitch(int8_t swtch)
{
  switch (swtch) {
#if defined(PCBX9D)
    case DSW(SW_SA0):
      PIOC->PIO_PDSR &= ~(1<<31);
      break;
    case DSW(SW_SA2):
      PIOC->PIO_PDSR |= (1<<31);
      break;
    case DSW(SW_SB0):
      PIOC->PIO_PDSR &= ~(1<<26);
      PIOC->PIO_PDSR &= ~(1<<27);
      break;
    case DSW(SW_SB1):
      PIOC->PIO_PDSR |= (1<<27);
      PIOC->PIO_PDSR &= ~(1<<26);
      break;
    case DSW(SW_SB2):
      PIOC->PIO_PDSR |= (1<<26);
      PIOC->PIO_PDSR &= ~(1<<27);
      break;
    case DSW(SW_SC0):
      PIOC->PIO_PDSR &= ~(1<<22);
      PIOC->PIO_PDSR &= ~(1<<23);
      break;
    case DSW(SW_SC1):
      PIOC->PIO_PDSR |= (1<<23);
      PIOC->PIO_PDSR &= ~(1<<22);
      break;
    case DSW(SW_SC2):
      PIOC->PIO_PDSR |= (1<<22);
      PIOC->PIO_PDSR &= ~(1<<23);
      break;
    case DSW(SW_SD0):
      PIOC->PIO_PDSR &= ~(1<<20);
      PIOC->PIO_PDSR &= ~(1<<21);
      break;
    case DSW(SW_SD1):
      PIOC->PIO_PDSR |= (1<<21);
      PIOC->PIO_PDSR &= ~(1<<20);
      break;
    case DSW(SW_SD2):
      PIOC->PIO_PDSR |= (1<<20);
      PIOC->PIO_PDSR &= ~(1<<21);
      break;
    case DSW(SW_SE0):
      PIOC->PIO_PDSR &= ~(1<<18);
      PIOC->PIO_PDSR &= ~(1<<19);
      break;
    case DSW(SW_SE1):
      PIOC->PIO_PDSR |= (1<<19);
      PIOC->PIO_PDSR &= ~(1<<18);
      break;
    case DSW(SW_SE2):
      PIOC->PIO_PDSR |= (1<<18);
      PIOC->PIO_PDSR &= ~(1<<19);
      break;
    case DSW(SW_SF0):
      PIOC->PIO_PDSR &= ~(1<<16);
      PIOC->PIO_PDSR &= ~(1<<17);
      break;
    case DSW(SW_SF1):
      PIOC->PIO_PDSR |= (1<<17);
      PIOC->PIO_PDSR &= ~(1<<16);
      break;
    case DSW(SW_SF2):
      PIOC->PIO_PDSR |= (1<<16);
      PIOC->PIO_PDSR &= ~(1<<17);
      break;
    case DSW(SW_SG0):
      PIOC->PIO_PDSR &= ~(1<<14);
      PIOC->PIO_PDSR &= ~(1<<15);
      break;
    case DSW(SW_SG1):
      PIOC->PIO_PDSR |= (1<<15);
      PIOC->PIO_PDSR &= ~(1<<14);
      break;
    case DSW(SW_SG2):
      PIOC->PIO_PDSR |= (1<<14);
      PIOC->PIO_PDSR &= ~(1<<15);
      break;
    case DSW(SW_SH0):
      PIOC->PIO_PDSR &= ~(1<<13);
      break;
    case DSW(SW_SH2):
      PIOC->PIO_PDSR |= (1<<13);
      break;
#elif defined(PCBSKY9X)
    case DSW(SW_ID0):
      PIOC->PIO_PDSR &= ~0x00004000;
      PIOC->PIO_PDSR |= 0x00000800;
      break;
    case DSW(SW_ID1):
      PIOC->PIO_PDSR |= 0x00004800;
      break;
    case DSW(SW_ID2):
      PIOC->PIO_PDSR &= ~0x00000800;
      PIOC->PIO_PDSR |= 0x00004000;
      break;

    SWITCH_CASE(SW_THR, PIOC->PIO_PDSR, 20)
    SWITCH_CASE(SW_RUD, PIOA->PIO_PDSR, 15)
    SWITCH_CASE(SW_ELE, PIOC->PIO_PDSR, 31)
    SWITCH_CASE(SW_AIL, PIOA->PIO_PDSR, 2)
    SWITCH_CASE(SW_GEA, PIOC->PIO_PDSR, 16)
    SWITCH_CASE(SW_TRN, PIOC->PIO_PDSR, 8)
#elif defined(PCBGRUVIN9X)
    SWITCH_CASE(SW_THR, ping, INP_G_ThrCt)
    SWITCH_CASE(SW_RUD, ping, INP_G_RuddDR)
    SWITCH_CASE(SW_ELE, pinc, INP_C_ElevDR)

    case DSW(SW_ID0):
      ping |=  (1<<INP_G_ID1);
      pinb &= ~(1<<INP_B_ID2);
      break;
    case DSW(SW_ID1):
      ping &= ~(1<<INP_G_ID1);
      pinb &= ~(1<<INP_B_ID2);
      break;
    case DSW(SW_ID2):
      ping &= ~(1<<INP_G_ID1);
      pinb |=  (1<<INP_B_ID2);
      break;

    SWITCH_CASE(SW_AIL, pinc, INP_C_AileDR)
    SWITCH_CASE(SW_GEA, ping, INP_G_Gear)
    SWITCH_CASE(SW_TRN, pinb, INP_B_Trainer)
#else // STOCK
#if defined(JETI) || defined(FRSKY) || defined(NMEA) || defined(ARDUPILOT)
    SWITCH_CASE(SW_THR, pinc, INP_C_ThrCt)
    SWITCH_CASE(SW_AIL, pinc, INP_C_AileDR)
#else
    SWITCH_CASE(SW_THR, pine, INP_E_ThrCt)
    SWITCH_CASE(SW_AIL, pine, INP_E_AileDR)
#endif
    case DSW(SW_ID0):
      ping |=  (1<<INP_G_ID1);
      pine &= ~(1<<INP_E_ID2);
      break;
    case DSW(SW_ID1):
      ping &= ~(1<<INP_G_ID1);
      pine &= ~(1<<INP_E_ID2);
      break;
    case DSW(SW_ID2):
      ping &= ~(1<<INP_G_ID1);
      pine |=  (1<<INP_E_ID2);
      break;

    SWITCH_CASE(SW_RUD, ping, INP_G_RuddDR)
    SWITCH_CASE(SW_ELE, pine, INP_E_ElevDR)
    SWITCH_CASE(SW_GEA, pine, INP_E_Gear)
    SWITCH_CASE(SW_TRN, pine, INP_E_Trainer)
#endif

    default:
      break;
  }
}

uint16_t getTmr16KHz()
{
  return get_tmr10ms() * 160;
}

bool eeprom_thread_running = true;
void *eeprom_write_function(void *)
{
  while (!sem_wait(eeprom_write_sem)) {
    if (!eeprom_thread_running)
      return NULL;
#if defined(PCBSKY9X)
    if (eeprom_read_operation) {
      assert(eeprom_buffer_size);
      eeprom_read_block(eeprom_buffer_data, (const void *)(int64_t)eeprom_pointer, eeprom_buffer_size);
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
#if !defined(PCBSKY9X)
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
#if defined(PCBSKY9X)
    }
    Spi_complete = 1;
#endif
  }
  return 0;
}

uint8_t main_thread_running = 0;
char * main_thread_error = NULL;
void *main_thread(void *)
{
#ifdef SIMU_EXCEPTIONS
  signal(SIGFPE, sig);
  signal(SIGSEGV, sig);

  try {
#endif

    s_current_protocol = 255;

    g_menuStackPtr = 0;
    g_menuStack[0] = menuMainView;
    g_menuStack[1] = menuModelSelect;

#ifdef PCBSKY9X
    eeprom_init();
#endif

    eeReadAll(); //load general setup and selected model

    if (g_eeGeneral.backlightMode != e_backlight_mode_off) backlightOn(); // on Tx start turn the light on

    if (main_thread_running == 1) {
#if defined(SPLASH)
      doSplash();
#endif

#if !defined(PCBSKY9X)
      checkLowEEPROM();
#endif

      checkTHR();
      checkSwitches();
      checkAlarm();
    }

    s_current_protocol = 0;

    while (main_thread_running) {
      perMain();
      sleep(1/*ms*/);
    }
#ifdef SIMU_EXCEPTIONS
  }
  catch (...) {
    main_thread_running = 0;
  }
#endif

  return NULL;
}

pthread_t main_thread_pid;
void StartMainThread(bool tests)
{
  main_thread_running = (tests ? 1 : 2);
  pthread_create(&main_thread_pid, NULL, &main_thread, NULL);
}

void StopMainThread()
{
  main_thread_running = 0;
  pthread_join(main_thread_pid, NULL);
}

pthread_t eeprom_thread_pid;
void StartEepromThread(const char *filename)
{
  eepromFile = filename;
  if (eepromFile) {
    fp = fopen(eepromFile, "r+");
    if (!fp)
      fp = fopen(eepromFile, "w+");
    if (!fp) perror("error in fopen");
  }
#ifdef __APPLE__
  eeprom_write_sem = sem_open("eepromsem", O_CREAT, S_IRUSR | S_IWUSR, 0);
#else
  eeprom_write_sem = (sem_t *)malloc(sizeof(sem_t));
  sem_init(eeprom_write_sem, 0, 0);
#endif
  eeprom_thread_running = true;
  assert(!pthread_create(&eeprom_thread_pid, NULL, &eeprom_write_function, NULL));
}

void StopEepromThread()
{
  eeprom_thread_running = false;
  sem_post(eeprom_write_sem);
  pthread_join(eeprom_thread_pid, NULL);
}

void eeprom_read_block (void *pointer_ram,
    const void *pointer_eeprom,
    size_t size)
{
  assert(size);

  if (fp) {
    memset(pointer_ram, 0, size);
    if (fseek(fp, (long) pointer_eeprom, SEEK_SET)==-1) perror("error in seek");
    if (fread(pointer_ram, size, 1, fp) <= 0) perror("error in read");
  }
  else {
    memcpy(pointer_ram, &eeprom[(uint64_t)pointer_eeprom], size);
  }
}

#if defined(PCBSKY9X)
uint16_t stack_free(uint8_t)
#else
uint16_t stack_free()
#endif
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

#if defined(SDCARD)
namespace simu {
#include <dirent.h>
}
#include "FatFs/ff.h"

#if defined WIN32 || !defined __GNUC__
#include <direct.h>
#endif

#if defined(PCBSKY9X)
FATFS g_FATFS_Obj;
#endif

FRESULT f_stat (const TCHAR * path, FILINFO *)
{
  struct stat tmp;
  // printf("f_stat(%s)\n", path); fflush(stdout);
  return stat(path, &tmp) ? FR_INVALID_NAME : FR_OK;
}

FRESULT f_mount (BYTE, FATFS*)
{
  return FR_OK;
}

FRESULT f_open (FIL * fil, const TCHAR *name, BYTE flag)
{
  if (!(flag & FA_WRITE)) {
    struct stat tmp;
    if (stat(name, &tmp))
      return FR_INVALID_NAME;
    fil->fsize = tmp.st_size;
  }
  fil->fs = (FATFS*)fopen(name, (flag & FA_WRITE) ? "w+" : "r+");
  return FR_OK;
}

FRESULT f_read (FIL* fil, void* data, UINT size, UINT* read)
{
  fread(data, size, 1, (FILE*)fil->fs);
  *read = size;
  return FR_OK;
}

FRESULT f_write (FIL* fil, const void* data, UINT size, UINT* written)
{
  fwrite(data, size, 1, (FILE*)fil->fs);
  *written = size;
  return FR_OK;
}

FRESULT f_lseek (FIL* fil, DWORD offset)
{
  fseek((FILE*)fil->fs, offset, SEEK_SET);
  return FR_OK;
}

FRESULT f_close (FIL * fil)
{
  if (fil->fs)
    fclose((FILE*)fil->fs);
  return FR_OK;
}

FRESULT f_chdir (const TCHAR *name)
{
#if defined WIN32 || !defined __GNUC__
  _chdir(name);
#else
  chdir(name);
#endif
  return FR_OK;
}

FRESULT f_opendir (DIR * rep, const TCHAR * name)
{
  rep->fs = (FATFS *)simu::opendir(name);
  return FR_OK;
}

FRESULT f_readdir (DIR * rep, FILINFO * fil)
{
  simu::dirent * ent = simu::readdir((simu::DIR *)rep->fs);
  if (!ent) return FR_NO_FILE;

#if defined(WIN32) || !defined(__GNUC__) || defined(__APPLE__)
  if (ent->d_type == DT_DIR)
#else
  if (ent->d_type == simu::DT_DIR)
#endif
    fil->fattrib = AM_DIR;
  else
    fil->fattrib = 0;
  memset(fil->fname, 0, 13);
  memset(fil->lfname, 0, SD_SCREEN_FILE_LENGTH);
  strncpy(fil->fname, ent->d_name, 13-1);
  strcpy(fil->lfname, ent->d_name);
  return FR_OK;
}

FRESULT f_mkfs (unsigned char, unsigned char, unsigned int)
{
  printf("Format SD...\n"); fflush(stdout);
  return FR_OK;
}

FRESULT f_mkdir (const TCHAR*)
{
  return FR_OK;
}

FRESULT f_unlink (const TCHAR*)
{
  return FR_OK;
}

int f_putc (TCHAR c, FIL * fil)
{
  fwrite(&c, 1, 1, (FILE*)fil->fs);
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

int f_printf (FIL *f, const TCHAR * format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  vfprintf((FILE*)f->fs, format, arglist);
  va_end(arglist);
  return 0;
}

FRESULT f_getcwd (TCHAR *path, UINT sz_path)
{
  strcpy(path, ".");
  return FR_OK;
}

#if defined(PCBSKY9X)
int32_t Card_state = SD_ST_MOUNTED;
uint32_t Card_CSD[4]; // TODO elsewhere
#endif

#endif

bool lcd_refresh = true;
uint8_t lcd_buf[DISPLAY_W*DISPLAY_H/8];

void lcdSetRefVolt(uint8_t val)
{
}

void refreshDisplay()
{
  memcpy(lcd_buf, displayBuf, sizeof(displayBuf));
  lcd_refresh = true;
}
