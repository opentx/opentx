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

#include "../../opentx.h"

void backupEeprom()
{
  char filename[60];
  uint8_t buffer[1024];
  FIL file;

  lcd_clear();
  displayProgressBar(STR_WRITING);

  // reset unexpectedShutdown to prevent warning when user restores EEPROM backup
  g_eeGeneral.unexpectedShutdown = 0;
  eeDirty(EE_GENERAL);
  eeCheck(true);

  // create the directory if needed...
  DIR folder;
  FRESULT result = f_opendir(&folder, EEPROMS_PATH);
  if (result != FR_OK) {
    if (result == FR_NO_PATH)
      result = f_mkdir(EEPROMS_PATH);
    if (result != FR_OK) {
      POPUP_WARNING(SDCARD_ERROR(result));
      return;
    }
  }

  // prepare the filename...
  char * tmp = strAppend(filename, EEPROMS_PATH "/eeprom");
  tmp = strAppendDate(tmp, true);
  strAppend(tmp, EEPROM_EXT);

  // open the file for writing...
  f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);

  for (int i=0; i<EESIZE; i+=1024) {
    UINT count;
    eepromReadBlock(buffer, i, 1024);
    f_write(&file, buffer, 1024, &count);
    updateProgressBar(i, EESIZE);
    SIMU_SLEEP(100/*ms*/);
  }

  f_close(&file);

  //set back unexpectedShutdown
  g_eeGeneral.unexpectedShutdown = 1;
  eeDirty(EE_GENERAL);
  eeCheck(true);
}

void menuGeneralVersion(uint8_t event)
{
  if (s_warning_result) {
    s_warning_result = 0;
    displayPopup(STR_EEPROMFORMATTING);
    eeErase(false);
#if !defined(SIMU)
    NVIC_SystemReset();
#else
    exit(0);
#endif
  }
  
  SIMPLE_MENU(STR_MENUVERSION, menuTabGeneral, e_Vers, 1);

  lcd_putsLeft(MENU_HEADER_HEIGHT+1, vers_stamp);

  lcd_putsLeft(MENU_HEADER_HEIGHT+5*FH+1, STR_EEBACKUP);
  lcd_putsLeft(MENU_HEADER_HEIGHT+6*FH+1, STR_FACTORYRESET);
  drawFilledRect(0, MENU_HEADER_HEIGHT+5*FH, LCD_W, 2*FH+1, SOLID);

  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    backupEeprom();
  }
  else if (event == EVT_KEY_LONG(KEY_MENU)) {
    POPUP_CONFIRMATION(STR_CONFIRMRESET);
  }
}
