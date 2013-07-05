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

volatile uint8_t pina=0xff, pinb=0xff, pinc=0xff, pind, pine=0xff, ping=0xff, pinh=0xff, pinj=0xff, pinl=0;
uint8_t portb, portc, porth=0, dummyport;
uint16_t dummyport16;
const char *eepromFile = NULL;
FILE *fp = NULL;

#if defined(PCBTARANIS)
uint32_t Peri1_frequency ;
uint32_t Peri2_frequency ;
GPIO_TypeDef gpioa;
GPIO_TypeDef gpiob;
GPIO_TypeDef gpioc;
GPIO_TypeDef gpiod;
GPIO_TypeDef gpioe;
TIM_TypeDef tim1;
TIM_TypeDef tim3;
TIM_TypeDef tim8;
RCC_TypeDef rcc;
DMA_Stream_TypeDef dma2_stream2;
DMA_Stream_TypeDef dma2_stream6;
DMA_TypeDef dma2;
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
  #define EESIZE_SIMU (128*4096)
#else
  extern uint16_t eeprom_pointer;
  extern const char* eeprom_buffer_data;
#endif

#if !defined(EESIZE_SIMU)
  #define EESIZE_SIMU EESIZE
#endif

char simuSdDirectory[1024] = "";

uint8_t eeprom[EESIZE_SIMU];
sem_t *eeprom_write_sem;

#if defined(CPUARM)
#if defined(PCBTARANIS)
#define SWITCH_CASE(swtch, pin, mask) \
    case swtch: \
      if (state) pin &= ~(mask); else pin |= (mask); \
      break;
#else
#define SWITCH_CASE(swtch, pin, mask) \
    case swtch: \
      if (state) pin |= (mask); else pin &= ~(mask); \
      break;
#endif
#define SWITCH_3_CASE(swtch, pin1, pin2, mask1, mask2) \
    case swtch: \
      if (state < 0) pin1 &= ~(mask1); else pin1 |= (mask1); \
      if (state > 0) pin2 &= ~(mask2); else pin2 |= (mask2); \
      break;
#define KEY_CASE(key, pin, mask) \
    case key: \
      if (state) pin &= ~mask; else pin |= mask;\
      break;
#define TRIM_CASE KEY_CASE
#else
#define SWITCH_CASE(swtch, pin, mask) \
    case swtch: \
      if (state) pin &= ~(mask); else pin |= (mask); \
      break;
#define SWITCH_3_CASE(swtch, pin1, pin2, mask1, mask2) \
    case swtch: \
      if (state >= 0) pin1 &= ~(mask1); else pin1 |= (mask1); \
      if (state <= 0) pin2 &= ~(mask2); else pin2 |= (mask2); \
      break;
#define KEY_CASE(key, pin, mask) \
    case key: \
      if (state) pin |= (mask); else pin &= ~(mask);\
      break;
#define TRIM_CASE KEY_CASE
#endif

void simuSetKey(uint8_t key, bool state)
{
  switch (key) {
    KEY_CASE(KEY_MENU, GPIO_BUTTON_MENU, PIN_BUTTON_MENU)
    KEY_CASE(KEY_EXIT, GPIO_BUTTON_EXIT, PIN_BUTTON_EXIT)
#if defined(PCBTARANIS)
    KEY_CASE(KEY_ENTER, GPIO_BUTTON_ENTER, PIN_BUTTON_ENTER)
    KEY_CASE(KEY_PAGE, GPIO_BUTTON_PAGE, PIN_BUTTON_PAGE)
    KEY_CASE(KEY_MINUS, GPIO_BUTTON_MINUS, PIN_BUTTON_MINUS)
    KEY_CASE(KEY_PLUS, GPIO_BUTTON_PLUS, PIN_BUTTON_PLUS)
#else
    KEY_CASE(KEY_RIGHT, GPIO_BUTTON_RIGHT, PIN_BUTTON_RIGHT)
    KEY_CASE(KEY_LEFT, GPIO_BUTTON_LEFT, PIN_BUTTON_LEFT)
    KEY_CASE(KEY_UP, GPIO_BUTTON_UP, PIN_BUTTON_UP)
    KEY_CASE(KEY_DOWN, GPIO_BUTTON_DOWN, PIN_BUTTON_DOWN)
#endif
#if defined(PCBSKY9X)
    KEY_CASE(BTN_REa, PIOB->PIO_PDSR, 0x40)
#elif defined(PCBGRUVIN9X)
    KEY_CASE(BTN_REa, pind, 0x20)
#elif defined(ROTARY_ENCODER_NAVIGATION)
    KEY_CASE(BTN_REa, RotEncoder, 0x20)
#endif
  }
}

void simuSetTrim(uint8_t trim, bool state)
{
  // printf("trim=%d state=%d\n", trim, state); fflush(stdout);

  switch (trim) {
    TRIM_CASE(0, GPIO_TRIM_LH_L, PIN_TRIM_LH_L)
    TRIM_CASE(1, GPIO_TRIM_LH_R, PIN_TRIM_LH_R)
    TRIM_CASE(2, GPIO_TRIM_LV_DN, PIN_TRIM_LV_DN)
    TRIM_CASE(3, GPIO_TRIM_LV_UP, PIN_TRIM_LV_UP)
    TRIM_CASE(4, GPIO_TRIM_RV_DN, PIN_TRIM_RV_DN)
    TRIM_CASE(5, GPIO_TRIM_RV_UP, PIN_TRIM_RV_UP)
    TRIM_CASE(6, GPIO_TRIM_RH_L, PIN_TRIM_RH_L)
    TRIM_CASE(7, GPIO_TRIM_RH_R, PIN_TRIM_RH_R)
  }
}

void simuSetSwitch(uint8_t swtch, int8_t state)
{
  // printf("swtch=%d state=%d\n", swtch, state); fflush(stdout);
  switch (swtch) {
#if defined(PCBTARANIS)
    SWITCH_3_CASE(0, GPIO_PIN_SW_A_L, GPIO_PIN_SW_A_H, PIN_SW_A_L, PIN_SW_A_H)
    SWITCH_3_CASE(1, GPIO_PIN_SW_B_L, GPIO_PIN_SW_B_H, PIN_SW_B_L, PIN_SW_B_H)
    SWITCH_3_CASE(2, GPIO_PIN_SW_C_L, GPIO_PIN_SW_C_H, PIN_SW_C_L, PIN_SW_C_H)
    SWITCH_3_CASE(3, GPIO_PIN_SW_D_L, GPIO_PIN_SW_D_H, PIN_SW_D_L, PIN_SW_D_H)
    SWITCH_3_CASE(4, GPIO_PIN_SW_E_H, GPIO_PIN_SW_E_L, PIN_SW_E_H, PIN_SW_E_L)
    SWITCH_CASE(5, GPIO_PIN_SW_F, PIN_SW_F)
    SWITCH_3_CASE(6, GPIO_PIN_SW_G_L, GPIO_PIN_SW_G_H, PIN_SW_G_L, PIN_SW_G_H)
    SWITCH_CASE(7, GPIO_PIN_SW_H, PIN_SW_H)
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
#else // STOCK
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

#if !defined(PCBTARANIS)
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
extern void opentxStart();
void *main_thread(void *)
{
#ifdef SIMU_EXCEPTIONS
  signal(SIGFPE, sig);
  signal(SIGSEGV, sig);

  try {
#endif

    s_current_protocol[0] = 255;

    g_menuStackPtr = 0;
    g_menuStack[0] = menuMainView;
    g_menuStack[1] = menuModelSelect;

#if defined(CPUARM) && defined(SDCARD)
    refreshSystemAudioFiles();
#endif

    eeReadAll(); // load general setup and selected model

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

    while (main_thread_running) {
      perMain();
      sleep(10/*ms*/);
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
  if (strlen(simuSdDirectory) == 0)
    getcwd(simuSdDirectory, 1024);

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

#if !defined(PCBTARANIS)
  eeprom_thread_running = true;
  assert(!pthread_create(&eeprom_thread_pid, NULL, &eeprom_write_function, NULL));
#endif
}

void StopEepromThread()
{
#if !defined(PCBTARANIS)
  eeprom_thread_running = false;
  sem_post(eeprom_write_sem);
  pthread_join(eeprom_thread_pid, NULL);
#endif
}

#if defined(PCBTARANIS)
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

#if defined(PCBTARANIS)
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

FRESULT f_stat (const TCHAR * name, FILINFO *)
{
  char path[1024];
  if (name[0] == '/')
    sprintf(path, "%s%s", simuSdDirectory, name);
  else
    strcpy(path, name);

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
  char path[1024];
  if (name[0] == '/')
    sprintf(path, "%s%s", simuSdDirectory, name);
  else
    strcpy(path, name);

  if (!(flag & FA_WRITE)) {
    struct stat tmp;
    if (stat(path, &tmp))
      return FR_INVALID_NAME;
    fil->fsize = tmp.st_size;
  }
  fil->fs = (FATFS*)fopen(path, (flag & FA_WRITE) ? "w+" : "r+");
  return FR_OK;
}

FRESULT f_read (FIL* fil, void* data, UINT size, UINT* read)
{
  if (fil && fil->fs) fread(data, size, 1, (FILE*)fil->fs);
  *read = size;
  return FR_OK;
}

FRESULT f_write (FIL* fil, const void* data, UINT size, UINT* written)
{
  if (fil && fil->fs) fwrite(data, size, 1, (FILE*)fil->fs);
  *written = size;
  return FR_OK;
}

FRESULT f_lseek (FIL* fil, DWORD offset)
{
  if (fil && fil->fs) fseek((FILE*)fil->fs, offset, SEEK_SET);
  return FR_OK;
}

FRESULT f_close (FIL * fil)
{
  if (fil && fil->fs) {
    fclose((FILE*)fil->fs);
    fil->fs = NULL;
  }
  return FR_OK;
}

FRESULT f_chdir (const TCHAR *name)
{
  char path[1024];
  if (name[0] == '/')
    sprintf(path, "%s%s", simuSdDirectory, name);
  else
    strcpy(path, name);

#if defined WIN32 || !defined __GNUC__
  _chdir(path);
#else
  chdir(path);
#endif
  return FR_OK;
}

FRESULT f_opendir (DIR * rep, const TCHAR * name)
{
  char path[1024];
  if (name[0] == '/')
    sprintf(path, "%s%s", simuSdDirectory, name);
  else
    strcpy(path, name);

  rep->fs = (FATFS *)simu::opendir(path);
  return FR_OK;
}

FRESULT f_readdir (DIR * rep, FILINFO * fil)
{
  if (!rep->fs) return FR_NO_FILE;
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
  getcwd(path, sz_path);
  return FR_OK;
}

#if defined(PCBSKY9X)
int32_t Card_state = SD_ST_MOUNTED;
uint32_t Card_CSD[4]; // TODO elsewhere
#endif

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

#if defined(PCBTARANIS)
void usbStart() { }
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
#endif
