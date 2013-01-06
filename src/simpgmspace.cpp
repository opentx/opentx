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

#if defined(PCBX9D) || defined(PCBACT)
GPIO_TypeDef gpioa;
GPIO_TypeDef gpiob;
GPIO_TypeDef gpioc;
GPIO_TypeDef gpiod;
GPIO_TypeDef gpioe;
#elif defined(PCBSKY9X)
Pio Pioa, Piob, Pioc;
Pwm pwm;
Twi Twio;
Usart Usart0;
Dacc dacc;
Adc Adc0;
#endif

#if defined(PCBSKY9X)
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

#if defined(CPUARM)
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
#if defined(PCBX9D) || defined(PCBACT)
    case DSW(SW_SA0):
      GPIOE->IDR |= PIN_SW_A_L;
      GPIOB->IDR &= ~PIN_SW_A_H;
      break;
    case DSW(SW_SA1):
      GPIOE->IDR |= PIN_SW_A_H;
      GPIOB->IDR |= PIN_SW_A_L;
      break;
    case DSW(SW_SA2):
      GPIOE->IDR &= ~PIN_SW_A_L;
      GPIOB->IDR |= PIN_SW_A_H;
      break;
    case DSW(SW_SB0):
      GPIOB->IDR |= PIN_SW_B_L;
      GPIOB->IDR &= ~PIN_SW_B_H;
      break;
    case DSW(SW_SB1):
      GPIOB->IDR |= PIN_SW_B_H;
      GPIOB->IDR |= PIN_SW_B_L;
      break;
    case DSW(SW_SB2):
      GPIOB->IDR &= ~PIN_SW_B_L;
      GPIOB->IDR |= PIN_SW_B_H;
      break;
    case DSW(SW_SC0):
      GPIOE->IDR |= PIN_SW_C_L;
      GPIOB->IDR &= ~PIN_SW_C_H;
      break;
    case DSW(SW_SC1):
      GPIOB->IDR |= PIN_SW_C_H;
      GPIOE->IDR |= PIN_SW_C_L;
      break;
    case DSW(SW_SC2):
      GPIOE->IDR &= ~PIN_SW_C_L;
      GPIOB->IDR |= PIN_SW_C_H;
      break;
    case DSW(SW_SD0):
      GPIOE->IDR |= PIN_SW_D_L;
      GPIOE->IDR &= ~PIN_SW_D_H;
      break;
    case DSW(SW_SD1):
      GPIOE->IDR |= PIN_SW_D_H;
      GPIOE->IDR |= PIN_SW_D_L;
      break;
    case DSW(SW_SD2):
      GPIOE->IDR &= ~PIN_SW_D_L;
      GPIOE->IDR |= PIN_SW_D_H;
      break;
    case DSW(SW_SE0):
      GPIOB->IDR |= PIN_SW_E_L;
      GPIOB->IDR &= ~PIN_SW_E_H;
      break;
    case DSW(SW_SE1):
      GPIOB->IDR |= PIN_SW_E_H;
      GPIOB->IDR |= PIN_SW_E_L;
      break;
    case DSW(SW_SE2):
      GPIOB->IDR &= ~PIN_SW_E_L;
      GPIOB->IDR |= PIN_SW_E_H;
      break;
    case DSW(SW_SF0):
      GPIOE->IDR &= ~PIN_SW_F;
      break;
    case DSW(SW_SF2):
      GPIOE->IDR |= PIN_SW_F;
      break;
    case DSW(SW_SG0):
      GPIOE->IDR |= PIN_SW_G_L;
      GPIOA->IDR &= ~PIN_SW_G_H;
      break;
    case DSW(SW_SG1):
      GPIOA->IDR |= PIN_SW_G_H;
      GPIOE->IDR |= PIN_SW_G_L;
      break;
    case DSW(SW_SG2):
      GPIOE->IDR &= ~PIN_SW_G_L;
      GPIOA->IDR |= PIN_SW_G_H;
      break;
    case DSW(SW_SH0):
      GPIOE->IDR &= ~PIN_SW_H;
      break;
    case DSW(SW_SH2):
      GPIOE->IDR |= PIN_SW_H;
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

#if !defined(PCBX9D)
bool eeprom_thread_running = true;
void *eeprom_write_function(void *)
{
  while (!sem_wait(eeprom_write_sem)) {
    if (!eeprom_thread_running)
      return NULL;
#if defined(CPUARM)
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

    eeReadAll(); //load general setup and selected model

    if (g_eeGeneral.backlightMode != e_backlight_mode_off) backlightOn(); // on Tx start turn the light on

    if (main_thread_running == 1) {
#if defined(SPLASH)
      doSplash();
#endif

#if !defined(CPUARM)
      checkLowEEPROM();
#endif

#if defined(CPUARM)
      eeLoadModel(g_eeGeneral.currModel);
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

#if !defined(PCBX9D)
  eeprom_thread_running = true;
  assert(!pthread_create(&eeprom_thread_pid, NULL, &eeprom_write_function, NULL));
#endif
}

void StopEepromThread()
{
#if !defined(PCBX9D)
  eeprom_thread_running = false;
  sem_post(eeprom_write_sem);
  pthread_join(eeprom_thread_pid, NULL);
#endif
}

#if defined(PCBX9D)
void eeprom_read_block (void *pointer_ram, uint16_t pointer_eeprom, size_t size)
#else
void eeprom_read_block (void *pointer_ram, const void *pointer_eeprom, size_t size)
#endif
{
  assert(size);

  if (fp) {
    // printf("EEPROM read (pos=%d, size=%d)\n", pointer_eeprom, size); fflush(stdout);
    if (fseek(fp, (long)pointer_eeprom, SEEK_SET)==-1) perror("error in fseek");
    if (fread(pointer_ram, size, 1, fp) <= 0) perror("error in fread");
  }
  else {
    memcpy(pointer_ram, &eeprom[(uint64_t)pointer_eeprom], size);
  }
}

#if defined(PCBX9D)
void eeWriteBlockCmp(const void *pointer_ram, uint16_t pointer_eeprom, size_t size)
{
  assert(size);

  if (fp) {
    // printf("EEPROM write (pos=%d, size=%d)\n", pointer_eeprom, size); fflush(stdout);
    if (fseek(fp, (long)pointer_eeprom, SEEK_SET)==-1) perror("error in fseek");
    if (fwrite(pointer_ram, size, 1, fp) <= 0) perror("error in fwrite");
  }
  else {
    memcpy(&eeprom[(uint64_t)pointer_eeprom], pointer_ram, size);
  }
}

#endif

#if defined(CPUARM)
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

#if defined(CPUARM)
FATFS g_FATFS_Obj;
#endif

FRESULT f_stat (const TCHAR * path, FILINFO *)
{
  struct stat tmp;
  printf("f_stat(%s)\n", path); fflush(stdout);
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
  if (fil->fs) {
    fclose((FILE*)fil->fs);
    fil->fs=NULL;
  }
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

#if defined(CPUARM)
int32_t Card_state = SD_ST_MOUNTED;
uint32_t Card_CSD[4]; // TODO elsewhere
#endif

#else // TODO no SD quick and dirty!
int32_t Card_state;
#endif

bool lcd_refresh = true;
uint8_t lcd_buf[DISPLAY_BUF_SIZE];

void lcdSetRefVolt(uint8_t val)
{
}

void lcdRefresh()
{
  memcpy(lcd_buf, displayBuf, DISPLAY_BUF_SIZE);
  lcd_refresh = true;
}
