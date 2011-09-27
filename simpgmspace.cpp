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
volatile int8_t eeprom_buffer_size = 0;

uint8_t eeprom[EESIZE];

void eeWriteBlockCmp(const void *i_pointer_ram, void *pointer_eeprom, size_t size)
{
#if 0
  printf(" eeWriteBlockCmp(%d %d)", size, (int)pointer_eeprom);
  for(uint8_t i=0; i<size; i++)
    printf(" %02X", ((const char*)i_pointer_ram)[i]);
  printf("\n");fflush(stdout);
#endif

  if (eepromFile) {
    FILE *fp = fopen(eepromFile, "r+");
    long ofs = (long) pointer_eeprom;
    const char* pointer_ram= (const char*)i_pointer_ram;
    //printf("eeWr p=%10p blk%3d ofs=%2d l=%d",pointer_ram,
    //       (int)pointer_eeprom/16,
    //       (int)pointer_eeprom%16,
    //       (int)size);
    while(size) {
      if(fseek(fp, ofs , SEEK_SET)==-1) perror("error in seek");
      char buf[1];
      if (fread(buf, 1, 1, fp) != 1) perror("error in read");

      if (buf[0] != pointer_ram[0]){
        //printf("X");
        g_tmr10ms++;
        if(fseek(fp, ofs , SEEK_SET)==-1) perror("error in seek");
        fwrite(pointer_ram, 1, 1,fp);
      }
      else{
        //printf(".");
      }

      size--;
      ofs++;
      (const char*)pointer_ram++;
    }
    fclose(fp);
  }
  else {
    memcpy(&eeprom[(int64_t)pointer_eeprom], i_pointer_ram, size);
  }
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
