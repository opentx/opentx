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

#include "../opentx.h"

const pm_uchar sticks[] PROGMEM = {
#include "../bitmaps/sticks.lbm"
};

#if defined(CPUARM)
extern LP_CONST LanguagePack czLanguagePack;
extern LP_CONST LanguagePack enLanguagePack;
extern LP_CONST LanguagePack esLanguagePack;
extern LP_CONST LanguagePack frLanguagePack;
extern LP_CONST LanguagePack deLanguagePack;
extern LP_CONST LanguagePack itLanguagePack;
extern LP_CONST LanguagePack plLanguagePack;
extern LP_CONST LanguagePack ptLanguagePack;
extern LP_CONST LanguagePack skLanguagePack;
extern LP_CONST LanguagePack seLanguagePack;
extern LP_CONST LanguagePack huLanguagePack;

const LanguagePack * LP_CONST languagePacks[] = {
  // alphabetical order
  &czLanguagePack,
  &deLanguagePack,
  &enLanguagePack,
  &esLanguagePack,
  &frLanguagePack,
  &huLanguagePack,
  &itLanguagePack,
  &plLanguagePack,
  &ptLanguagePack,
  &seLanguagePack,
  &skLanguagePack,
  NULL
};
#endif

enum EnumTabDiag {
  e_Setup,
  CASE_SDCARD(e_Sd)
  CASE_CPUARM(e_GeneralCustomFunctions)
  e_Trainer,
  e_Vers,
  e_Keys,
  e_Ana,
  CASE_CPUARM(e_Hardware)
  e_Calib
};

void menuGeneralSetup(uint8_t event);
void menuGeneralSdManager(uint8_t event);
void menuGeneralCustomFunctions(uint8_t event);
void menuGeneralTrainer(uint8_t event);
void menuGeneralVersion(uint8_t event);
void menuGeneralDiagKeys(uint8_t event);
void menuGeneralDiagAna(uint8_t event);
void menuGeneralHardware(uint8_t event);
void menuGeneralCalib(uint8_t event);

const MenuFuncP_PROGMEM menuTabGeneral[] PROGMEM = {
  menuGeneralSetup,
  CASE_SDCARD(menuGeneralSdManager)
  CASE_CPUARM(menuGeneralCustomFunctions)
  menuGeneralTrainer,
  menuGeneralVersion,
  menuGeneralDiagKeys,
  menuGeneralDiagAna,
  CASE_CPUARM(menuGeneralHardware)
  menuGeneralCalib
};

#if defined(SDCARD)
void menuGeneralSdManagerInfo(uint8_t event)
{
  SIMPLE_SUBMENU(STR_SD_INFO_TITLE, 1);

  lcd_putsLeft(2*FH, STR_SD_TYPE);
  lcd_puts(10*FW, 2*FH, SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD);

  lcd_putsLeft(3*FH, STR_SD_SIZE);
  lcd_outdezAtt(10*FW, 3*FH, SD_GET_SIZE_MB(), LEFT);
  lcd_putc(lcdLastPos, 3*FH, 'M');

  lcd_putsLeft(4*FH, STR_SD_SECTORS);
  lcd_outdezAtt(10*FW, 4*FH, SD_GET_BLOCKNR()/1000, LEFT);
  lcd_putc(lcdLastPos, 4*FH, 'k');

  lcd_putsLeft(5*FH, STR_SD_SPEED);
  lcd_outdezAtt(10*FW, 5*FH, SD_GET_SPEED()/1000, LEFT);
  lcd_puts(lcdLastPos, 5*FH, "kb/s");
}

inline bool isFilenameGreater(bool isfile, const char * fn, const char * line)
{
  return (isfile && !line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcasecmp(fn, line) > 0);
}

inline bool isFilenameLower(bool isfile, const char * fn, const char * line)
{
  return (!isfile && line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcasecmp(fn, line) < 0);
}

#if defined(PCBTARANIS)
void backupEeprom()
{
  char filename[60];
  uint8_t buffer[1024];
  FIL file;

  lcd_clear();
  lcd_putsLeft(4*FH, STR_WRITING);
  lcd_rect(3, 6*FH+4, 204, 7);
  lcdRefresh();

  //reset unexpectedShutdown to prevent warning when user restores EEPROM backup
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
    eeprom_read_block(buffer, i, 1024);
    f_write(&file, buffer, 1024, &count);
    lcd_hline(5, 6*FH+6, (200*i)/EESIZE, FORCE);
    lcd_hline(5, 6*FH+7, (200*i)/EESIZE, FORCE);
    lcd_hline(5, 6*FH+8, (200*i)/EESIZE, FORCE);
    lcdRefresh();
    SIMU_SLEEP(100/*ms*/);
  }

  f_close(&file);

  //set back unexpectedShutdown
  g_eeGeneral.unexpectedShutdown = 1;
  eeDirty(EE_GENERAL);
  eeCheck(true);
}
#endif

#if defined(PCBTARANIS)
void flashBootloader(const char * filename)
{
  FIL file;
  f_open(&file, filename, FA_READ);
  uint8_t buffer[1024];
  UINT count;

  lcd_clear();
  lcd_putsLeft(4*FH, STR_WRITING);
  lcd_rect(3, 6*FH+4, 204, 7);
  lcdRefresh();

  static uint8_t unlocked = 0;
  if (!unlocked) {
    unlocked = 1;
    unlockFlash();
  }

  for (int i=0; i<BOOTLOADER_SIZE; i+=1024) {
    watchdogSetTimeout(100/*1s*/);
    if (f_read(&file, buffer, 1024, &count) != FR_OK || count != 1024) {
      POPUP_WARNING(STR_SDCARD_ERROR);
      break;
    }
    if (i==0 && !isBootloaderStart((uint32_t *)buffer)) {
      POPUP_WARNING(STR_INCOMPATIBLE);
      break;
    }
    for (int j=0; j<1024; j+=FLASH_PAGESIZE) {
      writeFlash(CONVERT_UINT_PTR(FIRMWARE_ADDRESS+i+j), (uint32_t *)(buffer+j));
      lcd_hline(5, 6*FH+6, (200*i)/BOOTLOADER_SIZE, FORCE);
      lcd_hline(5, 6*FH+7, (200*i)/BOOTLOADER_SIZE, FORCE);
      lcd_hline(5, 6*FH+8, (200*i)/BOOTLOADER_SIZE, FORCE);
      lcdRefresh();
      SIMU_SLEEP(30/*ms*/);
    }
  }

  if (unlocked) {
    lockFlash();
    unlocked = 0;
  }

  f_close(&file);
}
#endif

void onSdManagerMenu(const char *result)
{
  TCHAR lfn[_MAX_LFN+1];

  uint8_t index = m_posVert-1-s_pgOfs;
  if (result == STR_SD_INFO) {
    pushMenu(menuGeneralSdManagerInfo);
  }
  else if (result == STR_SD_FORMAT) {
    POPUP_CONFIRMATION(STR_CONFIRM_FORMAT);
  }
  else if (result == STR_DELETE_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat_P(lfn, PSTR("/"));
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    f_unlink(lfn);
    strncpy(statusLineMsg, reusableBuffer.sdmanager.lines[index], 13);
    strcpy_P(statusLineMsg+min((uint8_t)strlen(statusLineMsg), (uint8_t)13), STR_REMOVED);
    showStatusLine();
    if ((uint16_t)m_posVert == reusableBuffer.sdmanager.count) m_posVert--;
    reusableBuffer.sdmanager.offset = s_pgOfs-1;
  }
#if defined(CPUARM)
  /* TODO else if (result == STR_LOAD_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    POPUP_WARNING(eeLoadModelSD(lfn));
  } */
  else if (result == STR_PLAY_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    audioQueue.stopAll();
    audioQueue.playFile(lfn, 0, ID_PLAY_FROM_SD_MANAGER);
  }
#endif
#if defined(PCBTARANIS)
  else if (result == STR_ASSIGN_BITMAP) {
    strAppendFilename(g_model.header.bitmap, reusableBuffer.sdmanager.lines[index], sizeof(g_model.header.bitmap));
    LOAD_MODEL_BITMAP();
    memcpy(modelHeaders[g_eeGeneral.currModel].bitmap, g_model.header.bitmap, sizeof(g_model.header.bitmap));
    eeDirty(EE_MODEL);
  }
  else if (result == STR_VIEW_TEXT) {
    f_getcwd(lfn, _MAX_LFN);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    pushMenuTextView(lfn);
  }
  else if (result == STR_FLASH_BOOTLOADER) {
    f_getcwd(lfn, _MAX_LFN);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    flashBootloader(lfn);
  }
#endif
#if defined(LUA)
  else if (result == STR_EXECUTE_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    luaExec(lfn);
  }
#endif
}

void menuGeneralSdManager(uint8_t _event)
{
  FILINFO fno;
  DIR dir;
  char *fn;   /* This function is assuming non-Unicode cfg. */
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);

#if defined(SDCARD)
  if (s_warning_result) {
    s_warning_result = 0;
    displayPopup(STR_FORMATTING);
    closeLogs();
#if defined(PCBSKY9X)
    Card_state = SD_ST_DATA;
#endif
#if defined(CPUARM)
    audioQueue.stopSD();
#endif
    if (f_mkfs(0, 1, 0) == FR_OK) {
      f_chdir("/");
      reusableBuffer.sdmanager.offset = -1;
    }
    else {
      POPUP_WARNING(STR_SDCARD_ERROR);
    }
  }
#endif

  uint8_t event = ((READ_ONLY() && EVT_KEY_MASK(_event) == KEY_ENTER) ? 0 : _event);
  SIMPLE_MENU(SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD, menuTabGeneral, e_Sd, 1+reusableBuffer.sdmanager.count);

  if (s_editMode > 0)
    s_editMode = 0;

  switch(_event) {
    case EVT_ENTRY:
      f_chdir(ROOT_PATH);
      reusableBuffer.sdmanager.offset = 65535;
      break;

#if defined(PCBTARANIS)
    case EVT_KEY_LONG(KEY_MENU):
      if (!READ_ONLY()) {
        killEvents(_event);
        // MENU_ADD_ITEM(STR_SD_INFO);  TODO: Implement
        MENU_ADD_ITEM(STR_SD_FORMAT);
        menuHandler = onSdManagerMenu;
      }
      break;
#endif

#if defined(PCBTARANIS)
    case EVT_KEY_BREAK(KEY_ENTER):
#else
    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_FIRST(KEY_RIGHT):
    case EVT_KEY_FIRST(KEY_ENTER):
#endif
    {
      if (m_posVert > 0) {
        vertpos_t index = m_posVert-1-s_pgOfs;
        if (!reusableBuffer.sdmanager.lines[index][SD_SCREEN_FILE_LENGTH+1]) {
          f_chdir(reusableBuffer.sdmanager.lines[index]);
          s_pgOfs = 0;
          m_posVert = 1;
          reusableBuffer.sdmanager.offset = 65535;
          killEvents(_event);
          break;
        }
      }
      if (!IS_ROTARY_BREAK(_event) || m_posVert==0)
        break;
      // no break;
    }

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(_event);
#if !defined(PCBTARANIS)
      if (m_posVert == 0) {
        MENU_ADD_ITEM(STR_SD_INFO);
        MENU_ADD_ITEM(STR_SD_FORMAT);
      }
      else
#endif
      {
#if defined(CPUARM)
        uint8_t index = m_posVert-1-s_pgOfs;
        // TODO duplicated code for finding extension
        char * ext = reusableBuffer.sdmanager.lines[index];
        int len = strlen(ext) - 4;
        ext += len;
        /* TODO if (!strcasecmp(ext, MODELS_EXT)) {
          s_menu[s_menu_count++] = STR_LOAD_FILE;
        }
        else */ if (!strcasecmp(ext, SOUNDS_EXT)) {
          MENU_ADD_ITEM(STR_PLAY_FILE);
        }
#endif
#if defined(PCBTARANIS)
        else if (!strcasecmp(ext, BITMAPS_EXT) && !READ_ONLY() && len <= (int)sizeof(g_model.header.bitmap)) {
          MENU_ADD_ITEM(STR_ASSIGN_BITMAP);
        }
        else if (!strcasecmp(ext, TEXT_EXT)) {
          MENU_ADD_ITEM(STR_VIEW_TEXT);
        }
        else if (!strcasecmp(ext, FIRMWARE_EXT) && !READ_ONLY()) {
          MENU_ADD_ITEM(STR_FLASH_BOOTLOADER);
        }
#endif
#if defined(LUA)
        else if (!strcasecmp(ext, SCRIPTS_EXT)) {
          MENU_ADD_ITEM(STR_EXECUTE_FILE);
        }
#endif
        if (!READ_ONLY()) {
          MENU_ADD_ITEM(STR_DELETE_FILE);
          // MENU_ADD_ITEM(STR_RENAME_FILE);  TODO: Implement
          // MENU_ADD_ITEM(STR_COPY_FILE);    TODO: Implement
        }
      }
      menuHandler = onSdManagerMenu;
      break;
  }

  if (reusableBuffer.sdmanager.offset != s_pgOfs) {
    if (s_pgOfs == 0) {
      reusableBuffer.sdmanager.offset = 0;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    }
    else if (s_pgOfs == reusableBuffer.sdmanager.count-7) {
      reusableBuffer.sdmanager.offset = s_pgOfs;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    }
    else if (s_pgOfs > reusableBuffer.sdmanager.offset) {
      memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], 6*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[6], 0xff, SD_SCREEN_FILE_LENGTH);
      reusableBuffer.sdmanager.lines[6][SD_SCREEN_FILE_LENGTH+1] = 1;
    }
    else {
      memmove(reusableBuffer.sdmanager.lines[1], reusableBuffer.sdmanager.lines[0], 6*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
    }

    reusableBuffer.sdmanager.count = 0;

    FRESULT res = f_opendir(&dir, ".");        /* Open the directory */
    if (res == FR_OK) {
      for (;;) {
        res = f_readdir(&dir, &fno);                   /* Read a directory item */
        if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        if (fno.fname[0] == '.' && fno.fname[1] == '\0') continue;             /* Ignore dot entry */
#if _USE_LFN
        fn = *fno.lfname ? fno.lfname : fno.fname;
#else
        fn = fno.fname;
#endif
        if (strlen(fn) > SD_SCREEN_FILE_LENGTH) continue;

        reusableBuffer.sdmanager.count++;

        bool isfile = !(fno.fattrib & AM_DIR);

        if (s_pgOfs == 0) {
          for (uint8_t i=0; i<LCD_LINES-1; i++) {
            char *line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameLower(isfile, fn, line)) {
              if (i < 6) memmove(reusableBuffer.sdmanager.lines[i+1], line, sizeof(reusableBuffer.sdmanager.lines[i]) * (6-i));
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[i]));
              strcpy(line, fn);
              line[SD_SCREEN_FILE_LENGTH+1] = isfile;
              break;
            }
          }
        }
        else if (reusableBuffer.sdmanager.offset == s_pgOfs) {
          for (int8_t i=6; i>=0; i--) {
            char *line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameGreater(isfile, fn, line)) {
              if (i > 0) memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], sizeof(reusableBuffer.sdmanager.lines[0]) * i);
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[i]));
              strcpy(line, fn);
              line[SD_SCREEN_FILE_LENGTH+1] = isfile;
              break;
            }
          }
        }
        else if (s_pgOfs > reusableBuffer.sdmanager.offset) {
          if (isFilenameGreater(isfile, fn, reusableBuffer.sdmanager.lines[5]) && isFilenameLower(isfile, fn, reusableBuffer.sdmanager.lines[6])) {
            memset(reusableBuffer.sdmanager.lines[6], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[6], fn);
            reusableBuffer.sdmanager.lines[6][SD_SCREEN_FILE_LENGTH+1] = isfile;
          }
        }
        else {
          if (isFilenameLower(isfile, fn, reusableBuffer.sdmanager.lines[1]) && isFilenameGreater(isfile, fn, reusableBuffer.sdmanager.lines[0])) {
            memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[0], fn);
            reusableBuffer.sdmanager.lines[0][SD_SCREEN_FILE_LENGTH+1] = isfile;
          }
        }
      }
    }
  }

  reusableBuffer.sdmanager.offset = s_pgOfs;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_TITLE_HEIGHT + 1 + i*FH;
    lcdNextPos = 0;
    uint8_t attr = (m_posVert-1-s_pgOfs == i ? BSS|INVERS : BSS);
    if (reusableBuffer.sdmanager.lines[i][0]) {
      if (!reusableBuffer.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1]) { lcd_putcAtt(0, y, '[', attr); }
      lcd_putsAtt(lcdNextPos, y, reusableBuffer.sdmanager.lines[i], attr);
      if (!reusableBuffer.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1]) { lcd_putcAtt(lcdNextPos, y, ']', attr); }
    }
  }

#if defined(PCBTARANIS)
  static vertpos_t sdBitmapIdx = 0xFFFF;
  static uint8_t sdBitmap[MODEL_BITMAP_SIZE];
  vertpos_t index = m_posVert-1-s_pgOfs;
  if (m_posVert > 0) {
    char * ext = reusableBuffer.sdmanager.lines[index];
    ext += strlen(ext) - 4;
    if (!strcasecmp(ext, BITMAPS_EXT)) {
      if (sdBitmapIdx != m_posVert) {
        sdBitmapIdx = m_posVert;
        uint8_t *dest = sdBitmap;
        if (bmpLoad(dest, reusableBuffer.sdmanager.lines[index], MODEL_BITMAP_WIDTH, MODEL_BITMAP_HEIGHT)) {
          memcpy(sdBitmap, logo_taranis, MODEL_BITMAP_SIZE);
        }
      }
      lcd_bmp(22*FW+2, 2*FH+FH/2, sdBitmap);
    }
  }
#endif
}
#endif

#if defined(CPUARM)
void menuGeneralCustomFunctions(uint8_t event)
{
  MENU(STR_MENUGLOBALFUNCS, menuTabGeneral, e_GeneralCustomFunctions, NUM_CFN+1, {0, NAVIGATION_LINE_BY_LINE|4/*repeated*/});
  return menuCustomFunctions(event, g_eeGeneral.customFn, globalFunctionsContext);
}
#endif

#if LCD_W >= 212
  #define TRAINER_CALIB_POS 12
#else
  #define TRAINER_CALIB_POS 8
#endif

void menuGeneralTrainer(uint8_t event)
{
  uint8_t y;
  bool slave = SLAVE_MODE();

  MENU(STR_MENUTRAINER, menuTabGeneral, e_Trainer, (slave ? 1 : 7), {0, 2, 2, 2, 2, 0/*, 0*/});

  if (slave) {
    lcd_puts(7*FW, 4*FH, STR_SLAVE);
  }
  else {
    uint8_t attr;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);

    lcd_puts(3*FW, MENU_TITLE_HEIGHT+1, STR_MODESRC);

    y = MENU_TITLE_HEIGHT + 1 + FH;

    for (uint8_t i=1; i<=NUM_STICKS; i++) {
      uint8_t chan = channel_order(i);
      volatile TrainerMix *td = &g_eeGeneral.trainer.mix[chan-1];

      putsMixerSource(0, y, MIXSRC_Rud-1+chan, (m_posVert==i && CURSOR_ON_LINE()) ? INVERS : 0);

      for (uint8_t j=0; j<3; j++) {

        attr = ((m_posVert==i && m_posHorz==j) ? blink : 0);

        switch(j) {
          case 0:
            lcd_putsiAtt(4*FW, y, STR_TRNMODE, td->mode, attr);
            if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->mode, 0, 2);
            break;

          case 1:
            lcd_outdezAtt(11*FW, y, td->studWeight, attr);
            if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->studWeight, -125, 125);
            break;

          case 2:
            lcd_putsiAtt(12*FW, y, STR_TRNCHN, td->srcChn, attr);
            if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->srcChn, 0, 3);
            break;
        }
      }
      y += FH;
    }

    attr = (m_posVert==5) ? blink : 0;
    lcd_putsLeft(MENU_TITLE_HEIGHT+1+5*FH, STR_MULTIPLIER);
    lcd_outdezAtt(LEN_MULTIPLIER*FW+3*FW, MENU_TITLE_HEIGHT+1+5*FH, g_eeGeneral.PPM_Multiplier+10, attr|PREC1);
    if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.PPM_Multiplier, -10, 40);

    attr = (m_posVert==6) ? INVERS : 0;
    if (attr) s_editMode = 0;
    lcd_putsAtt(0*FW, MENU_TITLE_HEIGHT+1+6*FH, STR_CAL, attr);
    for (uint8_t i=0; i<4; i++) {
      uint8_t x = (i*TRAINER_CALIB_POS+16)*FW/2;
#if defined (PPM_UNIT_PERCENT_PREC1)
      lcd_outdezAtt(x, MENU_TITLE_HEIGHT+1+6*FH, (g_ppmIns[i]-g_eeGeneral.trainer.calib[i])*2, PREC1);
#else
      lcd_outdezAtt(x, MENU_TITLE_HEIGHT+1+6*FH, (g_ppmIns[i]-g_eeGeneral.trainer.calib[i])/5, 0);
#endif
    }

    if (attr) {
      if (event==EVT_KEY_LONG(KEY_ENTER)){
        memcpy(g_eeGeneral.trainer.calib, g_ppmIns, sizeof(g_eeGeneral.trainer.calib));
        eeDirty(EE_GENERAL);
        AUDIO_WARNING1();
      }
    }
  }
}

void menuGeneralVersion(uint8_t event)
{
  SIMPLE_MENU(STR_MENUVERSION, menuTabGeneral, e_Vers, 1);

  lcd_putsLeft(MENU_TITLE_HEIGHT+FH, vers_stamp);
  
#if defined(COPROCESSOR)
  if (Coproc_valid == 1) {
     lcd_putsLeft(6*FH, PSTR("CoPr:"));
     lcd_outdez8(10*FW, 6*FH, Coproc_read);
  }
  else {
     lcd_putsLeft(6*FH, PSTR("CoPr: ---"));
  }
#endif

#if defined(PCBTARANIS)
  lcd_putsLeft(MENU_TITLE_HEIGHT+6*FH, STR_EEBACKUP);
  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    backupEeprom();
  }
#endif
}

void displayKeyState(uint8_t x, uint8_t y, EnumKeys key)
{
  uint8_t t = switchState(key);
  lcd_putcAtt(x, y, t+'0', t ? INVERS : 0);
}

void menuGeneralDiagKeys(uint8_t event)
{
  SIMPLE_MENU(STR_MENUDIAG, menuTabGeneral, e_Keys, 1);

  lcd_puts(14*FW, MENU_TITLE_HEIGHT+2*FH, STR_VTRIM);

  for(uint8_t i=0; i<9; i++) {
    coord_t y;

    if (i<8) {
      y = MENU_TITLE_HEIGHT + FH*3 + FH*(i/2);
      if (i&1) lcd_img(14*FW, y, sticks, i/2, 0);
      displayKeyState(i&1? 20*FW : 18*FW, y, (EnumKeys)(TRM_BASE+i));
    }

    if (i<6) {
      y = i*FH+MENU_TITLE_HEIGHT+FH;
      lcd_putsiAtt(0, y, STR_VKEYS, (5-i), 0);
      displayKeyState(5*FW+2, y, (EnumKeys)(KEY_MENU+(5-i)));
    }

#if !defined(PCBTARANIS)
    if (i != SW_ID0-SW_BASE) {
      y = MENU_TITLE_HEIGHT+i*FH-2*FH;
      putsSwitches(8*FW, y, i+1, 0); //ohne off,on
      displayKeyState(11*FW+2, y, (EnumKeys)(SW_BASE+i));
    }
#endif
  }

#if defined(ROTARY_ENCODERS) || defined(ROTARY_ENCODER_NAVIGATION)
  for(uint8_t i=0; i<DIM(g_rotenc); i++) {
    coord_t y = MENU_TITLE_HEIGHT /* ??? + 1 ??? */ + i*FH;
    lcd_putsiAtt(14*FW, y, STR_VRENCODERS, i, 0);
    lcd_outdezNAtt(18*FW, y, g_rotenc[i], LEFT|(switchState((EnumKeys)(BTN_REa+i)) ? INVERS : 0));
  }
#endif

}

void menuGeneralDiagAna(uint8_t event)
{
#if defined(PCBSKY9X) && !defined(REVA)
  #define ANAS_ITEMS_COUNT 4
#elif defined(PCBSKY9X)
  #define ANAS_ITEMS_COUNT 3
#else
  #define ANAS_ITEMS_COUNT 2
#endif

  SIMPLE_MENU(STR_MENUANA, menuTabGeneral, e_Ana, ANAS_ITEMS_COUNT);

  STICK_SCROLL_DISABLE();

  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
    coord_t y = MENU_TITLE_HEIGHT + 1 + (i/2)*FH;
    uint8_t x = i&1 ? 64+5 : 0;
    putsStrIdx(x, y, PSTR("A"), i+1);
    lcd_putc(lcdNextPos, y, ':');
    lcd_outhex4(x+3*FW-1, y, anaIn(i));
    lcd_outdez8(x+10*FW-1, y, (int16_t)calibratedStick[CONVERT_MODE(i)]*25/256);
  }

#if !defined(CPUARM)
  // Display raw BandGap result (debug)
  lcd_puts(64+5, MENU_TITLE_HEIGHT+1+3*FH, STR_BG);
  lcd_outdezAtt(64+5+6*FW-3, 1+4*FH, BandGap, 0);
#endif

#if defined(PCBTARANIS)
  lcd_putsLeft(MENU_TITLE_HEIGHT+1+5*FH, STR_BATT_CALIB);
  static int32_t adcBatt;
  adcBatt = ((adcBatt * 7) + anaIn(TX_VOLTAGE)) / 8;
  uint32_t batCalV = (adcBatt + (adcBatt*g_eeGeneral.vBatCalib)/128) * BATT_SCALE;
  batCalV >>= 11;
  batCalV += 2; // because of the diode
  putsVolts(LEN_CALIB_FIELDS*FW+4*FW, MENU_TITLE_HEIGHT+1+5*FH, batCalV, (m_posVert==1 ? INVERS : 0));
#elif defined(PCBSKY9X)
  lcd_putsLeft(MENU_TITLE_HEIGHT+1+4*FH, STR_BATT_CALIB);
  static int32_t adcBatt;
  adcBatt = ((adcBatt * 7) + anaIn(TX_VOLTAGE)) / 8;
  uint32_t batCalV = (adcBatt + adcBatt*(g_eeGeneral.vBatCalib)/128) * 4191;
  batCalV /= 55296;
  putsVolts(LEN_CALIB_FIELDS*FW+4*FW, MENU_TITLE_HEIGHT+1+4*FH, batCalV, (m_posVert==1 ? INVERS : 0));
#elif defined(PCBGRUVIN9X)
  lcd_putsLeft(6*FH-2, STR_BATT_CALIB);
  // Gruvin wants 2 decimal places and instant update of volts calib field when button pressed
  static uint16_t adcBatt;
  adcBatt = ((adcBatt * 7) + anaIn(TX_VOLTAGE)) / 8; // running average, sourced directly (to avoid unending debate :P)
  uint32_t batCalV = ((uint32_t)adcBatt*1390 + (10*(int32_t)adcBatt*g_eeGeneral.vBatCalib)/8) / BandGap;
  lcd_outdezNAtt(LEN_CALIB_FIELDS*FW+4*FW, 6*FH-2, batCalV, PREC2|(m_posVert==1 ? INVERS : 0));
#else
  lcd_putsLeft(6*FH-2, STR_BATT_CALIB);
  putsVolts(LEN_CALIB_FIELDS*FW+4*FW, 6*FH-2, g_vbat100mV, (m_posVert==1 ? INVERS : 0));
#endif
  if (m_posVert==1) CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatCalib, -127, 127);

#if defined(PCBSKY9X) && !defined(REVA)
  lcd_putsLeft(6*FH+1, STR_CURRENT_CALIB);
  putsValueWithUnit(LEN_CALIB_FIELDS*FW+4*FW, 6*FH+1, getCurrent(), UNIT_MILLIAMPS, (m_posVert==2 ? INVERS : 0)) ;
  if (m_posVert==2) CHECK_INCDEC_GENVAR(event, g_eeGeneral.currentCalib, -49, 49);
#endif

#if defined(PCBSKY9X)
  lcd_putsLeft(7*FH+1, STR_TEMP_CALIB);
  putsValueWithUnit(LEN_CALIB_FIELDS*FW+4*FW, 7*FH+1, getTemperature(), UNIT_TEMPERATURE, (m_posVert==3 ? INVERS : 0)) ;
  if (m_posVert==3) CHECK_INCDEC_GENVAR(event, g_eeGeneral.temperatureCalib, -100, 100);
#endif
}

#define XPOT_DELTA 10
#define XPOT_DELAY 10 /* cycles */

void menuCommonCalib(uint8_t event)
{
  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) { // get low and high vals for sticks and trims
    int16_t vt = anaIn(i);
    reusableBuffer.calib.loVals[i] = min(vt, reusableBuffer.calib.loVals[i]);
    reusableBuffer.calib.hiVals[i] = max(vt, reusableBuffer.calib.hiVals[i]);
    if (i >= POT1 && i <= POT_LAST) {
      if (IS_POT_WITHOUT_DETENT(i)) {
        reusableBuffer.calib.midVals[i] = (reusableBuffer.calib.hiVals[i] + reusableBuffer.calib.loVals[i]) / 2;
      }
#if defined(PCBTARANIS)
      uint8_t idx = i - POT1;
      int count = reusableBuffer.calib.xpotsCalib[idx].stepsCount;
      if (IS_POT_MULTIPOS(i) && count <= XPOTS_MULTIPOS_COUNT) {
        if (reusableBuffer.calib.xpotsCalib[idx].lastCount == 0 || vt < reusableBuffer.calib.xpotsCalib[idx].lastPosition - XPOT_DELTA || vt > reusableBuffer.calib.xpotsCalib[idx].lastPosition + XPOT_DELTA) {
          reusableBuffer.calib.xpotsCalib[idx].lastPosition = vt;
          reusableBuffer.calib.xpotsCalib[idx].lastCount = 1;
        }
        else {
          if (reusableBuffer.calib.xpotsCalib[idx].lastCount < 255) reusableBuffer.calib.xpotsCalib[idx].lastCount++;
        }
        if (reusableBuffer.calib.xpotsCalib[idx].lastCount == XPOT_DELAY) {
          int16_t position = reusableBuffer.calib.xpotsCalib[idx].lastPosition;
          bool found = false;
          for (int j=0; j<count; j++) {
            int16_t step = reusableBuffer.calib.xpotsCalib[idx].steps[j];
            if (position >= step-XPOT_DELTA && position <= step+XPOT_DELTA) {
              found = true;
              break;
            }
          }
          if (!found) {
            if (count < XPOTS_MULTIPOS_COUNT) {
              reusableBuffer.calib.xpotsCalib[idx].steps[count] = position;
            }
            reusableBuffer.calib.xpotsCalib[idx].stepsCount += 1;
          }
        }
      }
#endif
    }
  }

  calibrationState = reusableBuffer.calib.state; // make sure we don't scroll while calibrating

  switch (event)
  {
    case EVT_ENTRY:
      reusableBuffer.calib.state = 0;
      break;

    case EVT_KEY_BREAK(KEY_ENTER):
      reusableBuffer.calib.state++;
      break;
  }

  switch (reusableBuffer.calib.state) {
    case 0:
      // START CALIBRATION
      if (!READ_ONLY()) {
        lcd_putsLeft(MENU_TITLE_HEIGHT+2*FH, STR_MENUTOSTART);
      }
      break;

    case 1:
      // SET MIDPOINT
      lcd_putsAtt(0*FW, MENU_TITLE_HEIGHT+FH, STR_SETMIDPOINT, INVERS);
      lcd_putsLeft(MENU_TITLE_HEIGHT+2*FH, STR_MENUWHENDONE);

      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
        reusableBuffer.calib.loVals[i] = 15000;
        reusableBuffer.calib.hiVals[i] = -15000;
        reusableBuffer.calib.midVals[i] = anaIn(i);
#if defined(PCBTARANIS)
        if (i<NUM_XPOTS) {
          reusableBuffer.calib.xpotsCalib[i].stepsCount = 0;
          reusableBuffer.calib.xpotsCalib[i].lastCount = 0;
        }
#endif
      }
      break;

    case 2:
      // MOVE STICKS/POTS
      STICK_SCROLL_DISABLE();
      lcd_putsAtt(0*FW, MENU_TITLE_HEIGHT+FH, STR_MOVESTICKSPOTS, INVERS);
      lcd_putsLeft(MENU_TITLE_HEIGHT+2*FH, STR_MENUWHENDONE);

      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
        if (abs(reusableBuffer.calib.loVals[i]-reusableBuffer.calib.hiVals[i]) > 50) {
          g_eeGeneral.calib[i].mid = reusableBuffer.calib.midVals[i];
          int16_t v = reusableBuffer.calib.midVals[i] - reusableBuffer.calib.loVals[i];
          g_eeGeneral.calib[i].spanNeg = v - v/STICK_TOLERANCE;
          v = reusableBuffer.calib.hiVals[i] - reusableBuffer.calib.midVals[i];
          g_eeGeneral.calib[i].spanPos = v - v/STICK_TOLERANCE;
        }
      }
      break;

    case 3:
#if defined(PCBTARANIS)
      for (uint8_t i=POT1; i<=POT_LAST; i++) {
        int idx = i - POT1;
        int count = reusableBuffer.calib.xpotsCalib[idx].stepsCount;
        if (IS_POT_MULTIPOS(i)) {
          if (count > 1 && count <= XPOTS_MULTIPOS_COUNT) {
            for (int j=0; j<count; j++) {
              for (int k=j+1; k<count; k++) {
                if (reusableBuffer.calib.xpotsCalib[idx].steps[k] < reusableBuffer.calib.xpotsCalib[idx].steps[j]) {
                  swap(reusableBuffer.calib.xpotsCalib[idx].steps[j], reusableBuffer.calib.xpotsCalib[idx].steps[k]);
                }
              }
            }
            StepsCalibData * calib = (StepsCalibData *) &g_eeGeneral.calib[i];
            calib->count = count - 1;
            for (int j=0; j<calib->count; j++) {
              calib->steps[j] = (reusableBuffer.calib.xpotsCalib[idx].steps[j+1] + reusableBuffer.calib.xpotsCalib[idx].steps[j]) >> 5;
            }
          }
          else {
            g_eeGeneral.potsType &= ~(0x03<<(2*idx));
          }
        }
      }
#endif
      g_eeGeneral.chkSum = evalChkSum();
      eeDirty(EE_GENERAL);
      reusableBuffer.calib.state = 4;
      break;

    default:
      reusableBuffer.calib.state = 0;
      break;
  }

  doMainScreenGraphics();

#if defined(PCBTARANIS)
  drawPotsBars();
  for (int i=POT1; i<=POT_LAST; i++) {
    uint8_t steps = 0;
    if (reusableBuffer.calib.state == 2) {
      steps = reusableBuffer.calib.xpotsCalib[i-POT1].stepsCount;
    }
    else if (IS_POT_MULTIPOS(i)) {
      StepsCalibData * calib = (StepsCalibData *) &g_eeGeneral.calib[i];
      steps = calib->count + 1;
    }
    if (steps > 0 && steps <= XPOTS_MULTIPOS_COUNT) {
      lcd_outdezAtt(LCD_W/2-2+(i-POT1)*5, LCD_H-6, steps, TINSIZE);
    }
  }
#endif
}

void menuGeneralCalib(uint8_t event)
{
  check_simple(event, e_Calib, menuTabGeneral, DIM(menuTabGeneral), 0);

  if (menuEvent) {
    calibrationState = 0;
  }

  TITLE(STR_MENUCALIBRATION);
  menuCommonCalib(READ_ONLY() ? 0 : event);
}

void menuFirstCalib(uint8_t event)
{
  if (event == EVT_KEY_BREAK(KEY_EXIT) || reusableBuffer.calib.state == 4) {
    calibrationState = 0;
    chainMenu(menuMainView);
  }
  else {
    lcd_putsCenter(0*FH, MENUCALIBRATION);
    lcd_invert_line(0);
    menuCommonCalib(event);
  }
}
