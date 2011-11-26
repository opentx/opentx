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

volatile unsigned char pinb=0, pinc=0xff, pind, pine=0xff, ping=0xff;
unsigned char portb, dummyport;
const char *eepromFile = "eeprom.bin";

extern uint16_t eeprom_pointer;
extern const char* eeprom_buffer_data;
uint8_t eeprom[EESIZE];
sem_t eeprom_write_sem;
pthread_t write_thread_pid = 0;

void *eeprom_write_function(void *)
{
  while (!sem_wait(&eeprom_write_sem)) {

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

void InitEepromThread()
{
  sem_init(&eeprom_write_sem, 0, 0);
  assert(!pthread_create(&write_thread_pid, NULL, &eeprom_write_function, NULL));
}

void eeprom_read_block (void *pointer_ram,
    const void *pointer_eeprom,
    size_t size)
{
  if (eepromFile) {
    FILE *fp=fopen(eepromFile, "r");
    if(fseek(fp, (long) pointer_eeprom, SEEK_SET)==-1) perror("error in seek");
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
