/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 *
 * gruvin9x is based on code named er9x by
 * Author - Erez Raviv <erezraviv@gmail.com>, which is in turn
 * was based on the original (and ongoing) project by Thomas Husterer,
 * th9x -- http://code.google.com/p/th9x/
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

#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include "simpgmspace.h"
#include "lcd.h"
#include "gruvin9x.h"
#include "menus.h"

volatile uint8_t pinb=0, pinc=0xff, pind, pine=0xff, ping=0xff, pinj=0xff, pinl=0;
uint8_t portb, dummyport;
uint16_t dummyport16;
const char *eepromFile;

extern uint16_t eeprom_pointer;
extern const char* eeprom_buffer_data;
uint8_t eeprom[EESIZE];
sem_t eeprom_write_sem;

bool eeprom_thread_running = true;
void *eeprom_write_function(void *)
{
  while (!sem_wait(&eeprom_write_sem)) {

    if (!eeprom_thread_running)
      return NULL;

    FILE *fp = NULL;
    
    if (eepromFile) {
      fp = fopen(eepromFile, "r+");
      assert(fp);
    }

    while (--eeprom_buffer_size) {
      assert(eeprom_buffer_size > 0);
      if (fp) {
        if (fseek(fp, eeprom_pointer, SEEK_SET) == -1)
          perror("error in fseek");
        if (fwrite(eeprom_buffer_data, 1, 1, fp) != 1)
          perror("error in fwrite");
        usleep(5000/*5ms*/);
      }
      else {
        memcpy(&eeprom[eeprom_pointer], eeprom_buffer_data, 1);
      }
      eeprom_pointer++;
      eeprom_buffer_data++;
      
      if (fp && eeprom_buffer_size == 1) {
        fclose(fp);
      }
    }
  }

  return 0;
}

uint8_t main_thread_running = 0;
void *main_thread(void *)
{
  g_menuStack[0] = menuMainView;
  g_menuStack[1] = menuProcModelSelect;

  eeReadAll(); //load general setup and selected model

  if (main_thread_running == 1) {
    doSplash();
    checkLowEEPROM();
    checkTHR();
    checkSwitches();
    checkAlarm();
  }

  while (main_thread_running) {
    perMain();
    usleep(1000);
  }
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
  sem_init(&eeprom_write_sem, 0, 0);
  eeprom_thread_running = true;
  assert(!pthread_create(&eeprom_thread_pid, NULL, &eeprom_write_function, NULL));
}


void StopEepromThread()
{
  eeprom_thread_running = false;
  sem_post(&eeprom_write_sem);
  pthread_join(eeprom_thread_pid, NULL);
}

void eeprom_read_block (void *pointer_ram,
    const void *pointer_eeprom,
    size_t size)
{
  if (eepromFile) {
    FILE *fp=fopen(eepromFile, "r");
    if (!fp) { perror("error in fopen"); return; }
    if (fseek(fp, (long) pointer_eeprom, SEEK_SET)==-1) perror("error in seek");
    if (fread(pointer_ram, size, 1, fp) <= 0) perror("error in read");
    fclose(fp);
  }
  else {
    memcpy(pointer_ram, &eeprom[(uint64_t)pointer_eeprom], size);
  }
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
