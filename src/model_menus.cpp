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

#define WCHART 32
#define X0     (DISPLAY_W-WCHART-2)
#define Y0     32

enum EnumTabModel {
  e_ModelSelect,
  e_Model,
  IF_HELI(e_Heli)
  IF_FLIGHT_PHASES(e_PhasesAll)
  e_ExposAll,
  e_MixAll,
  e_Limits,
  IF_CURVES(e_CurvesAll)
  e_CustomSwitches,
  e_CustomFunctions,
  IF_FRSKY(e_Telemetry)
  IF_TEMPLATES(e_Templates)
};

void menuModelSelect(uint8_t event);
void menuModelSetup(uint8_t event);
#ifdef HELI
void menuModelHeli(uint8_t event);
#endif
#ifdef FLIGHT_PHASES
void menuModelPhasesAll(uint8_t event);
#endif
void menuModelExposAll(uint8_t event);
void menuModelMixAll(uint8_t event);
void menuModelLimits(uint8_t event);
void menuModelCurvesAll(uint8_t event);
void menuModelCustomSwitches(uint8_t event);
void menuModelCustomFunctions(uint8_t event);
#ifdef FRSKY
void menuModelTelemetry(uint8_t event);
#endif
#ifdef TEMPLATES
void menuModelTemplates(uint8_t event);
#endif
void menuModelExpoOne(uint8_t event);

const MenuFuncP_PROGMEM menuTabModel[] PROGMEM = {
  menuModelSelect,
  menuModelSetup,
  IF_HELI(menuModelHeli)
  IF_FLIGHT_PHASES(menuModelPhasesAll)
  menuModelExposAll,
  menuModelMixAll,
  menuModelLimits,
  IF_CURVES(menuModelCurvesAll)
  menuModelCustomSwitches,
  menuModelCustomFunctions,
  IF_FRSKY(menuModelTelemetry)
  IF_TEMPLATES(menuModelTemplates)
};

#define COPY_MODE 1
#define MOVE_MODE 2
static uint8_t s_copyMode = 0;
static int8_t s_copySrcRow;
static int8_t s_copyTgtOfs;

inline uint8_t eeFindEmptyModel(uint8_t id, bool down)
{
  uint8_t i = id;
  for (;;) {
    i = (MAX_MODELS + (down ? i+1 : i-1)) % MAX_MODELS;
    if (!eeModelExists(i)) break;
    if (i == id) return 0xff; // no free space in directory left
  }
  return i;
}

#if defined(SDCARD)
bool listSdFiles(const char *path, const char *extension, const uint8_t maxlen, const char *selection)
{
  FILINFO fno;
  DIR dir;
  char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif

  static uint16_t s_last_menu_offset = 0;

#if defined(PCBSKY9X)
  if (selection) {
    memset(reusableBuffer.models.menu_bss, 0, sizeof(reusableBuffer.models.menu_bss));
    strcpy(reusableBuffer.models.menu_bss[0], path);
    strcat(reusableBuffer.models.menu_bss[0], "/");
    strncat(reusableBuffer.models.menu_bss[0], selection, maxlen);
    strcat(reusableBuffer.models.menu_bss[0], SOUNDS_EXT);
    if (f_stat(reusableBuffer.models.menu_bss[0], &fno) != FR_OK) {
      selection = NULL;
    }
  }
#endif

  if (s_menu_offset == 0) {
    s_last_menu_offset = 0;
    memset(reusableBuffer.models.menu_bss, 0, sizeof(reusableBuffer.models.menu_bss));
  }
  else if (s_menu_offset == s_last_menu_offset) {
    return true; // should not happen, only there because of Murphy's law
  }
  else if (s_menu_offset > s_last_menu_offset) {
    memmove(reusableBuffer.models.menu_bss[0], reusableBuffer.models.menu_bss[1], (MENU_MAX_LINES-1)*MENU_LINE_LENGTH);
    memset(reusableBuffer.models.menu_bss[MENU_MAX_LINES-1], 0xff, MENU_LINE_LENGTH);
  }
  else {
    memmove(reusableBuffer.models.menu_bss[1], reusableBuffer.models.menu_bss[0], (MENU_MAX_LINES-1)*MENU_LINE_LENGTH);
    memset(reusableBuffer.models.menu_bss[0], 0, MENU_LINE_LENGTH);
  }

  s_menu_count = 0;
  s_menu_flags = BSS;

  FRESULT res = f_opendir(&dir, path);        /* Open the directory */
  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */

#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif

      uint8_t len = strlen(fn);
      if (len < 5 || len > maxlen+4 || strcmp(fn+len-4, extension) || (fno.fattrib & AM_DIR)) continue;

      s_menu_count++;
      fn[len-4] = '\0';

      if (s_menu_offset == 0) {
        if (selection && strncmp(fn, selection, maxlen) < 0) {
          s_last_menu_offset++;
        }
        else {
          for (uint8_t i=0; i<MENU_MAX_LINES; i++) {
            char *line = reusableBuffer.models.menu_bss[i];
            if (line[0] == '\0' || strcmp(fn, line) < 0) {
              if (i < MENU_MAX_LINES-1) memmove(reusableBuffer.models.menu_bss[i+1], line, sizeof(reusableBuffer.models.menu_bss[i]) * (MENU_MAX_LINES-1-i));
              memset(line, 0, MENU_LINE_LENGTH);
              strcpy(line, fn);
              break;
            }
          }
        }
        for (uint8_t i=0; i<min(s_menu_count, (uint16_t)MENU_MAX_LINES); i++)
          s_menu[i] = reusableBuffer.models.menu_bss[i];
      }
      else if (s_menu_offset > s_last_menu_offset) {
        if (strcmp(fn, reusableBuffer.models.menu_bss[MENU_MAX_LINES-2]) > 0 && strcmp(fn, reusableBuffer.models.menu_bss[MENU_MAX_LINES-1]) < 0) {
          memset(reusableBuffer.models.menu_bss[MENU_MAX_LINES-1], 0, MENU_LINE_LENGTH);
          strcpy(reusableBuffer.models.menu_bss[MENU_MAX_LINES-1], fn);
        }
      }
      else {
        if (strcmp(fn, reusableBuffer.models.menu_bss[1]) < 0 && strcmp(fn, reusableBuffer.models.menu_bss[0]) > 0) {
          memset(reusableBuffer.models.menu_bss[0], 0, MENU_LINE_LENGTH);
          strcpy(reusableBuffer.models.menu_bss[0], fn);
        }
      }
    }
  }

  if (s_menu_offset > 0)
    s_last_menu_offset = s_menu_offset;
  else
    s_menu_offset = s_last_menu_offset;

  return s_menu_count;
}
#endif

void menuModelSelect(uint8_t event)
{
  TITLE(STR_MENUMODELSEL);

#if defined(PCBSKY9X)
  #define REFRESH(x)
#elif defined(PCBGRUVIN9X) && defined(SDCARD)
  static bool refresh = true;
  #define REFRESH(x) refresh = (x)
#else
#define refresh event
#endif

  if (s_warning_result) {
    eeDeleteModel(m_posVert); // delete file
    s_warning_result = 0;
    s_copyMode = 0;
    event = EVT_ENTRY_UP;
  }

#if defined(SDCARD)
  uint8_t _event = event;
  if (s_menu_count) {
    event = 0;
  }
#endif
  uint8_t _event_ = (IS_RE_NAVIGATION_EVT(event) ? 0 : event);

  if (s_copyMode || !eeModelExists(g_eeGeneral.currModel)) {
    if ((event & 0x1f) == KEY_EXIT)
      _event_ -= KEY_EXIT;
  }

  int8_t oldSub = m_posVert;
  if (!check_submenu_simple(_event_, MAX_MODELS-1)) return;
#if defined(ROTARY_ENCODERS)
  if (m_posVert < 0) m_posVert = 0;
#endif
  if (s_editMode > 0) s_editMode = 0;

#if !defined(PCBSKY9X)
  if (event
#if defined(ROTARY_ENCODERS)
      || oldSub != m_posVert
#endif
      ) {
    eeFlush(); // flush eeprom write
#if defined(SDCARD)
    REFRESH(true);
#endif
  }
#endif

  int8_t sub = m_posVert;

#if defined(ROTARY_ENCODERS)
  if (scrollRE > 0 && s_editMode < 0) {
    chainMenu(menuModelSetup);
    return;
  }
#endif

  switch(event)
  {
      case EVT_ENTRY:
        m_posVert = sub = g_eeGeneral.currModel;
        s_copyMode = 0;
        s_editMode = -1;
        eeCheck(true);
        break;
      case EVT_KEY_LONG(KEY_EXIT):
        if (s_copyMode && s_copyTgtOfs == 0 && g_eeGeneral.currModel != sub && eeModelExists(sub)) {
          s_warning = STR_DELETEMODEL;
          s_warning_type = WARNING_TYPE_CONFIRM;
          killEvents(event);
          break;
        }
        // no break
      case EVT_KEY_BREAK(KEY_EXIT):
        if (s_copyMode) {
          sub = m_posVert = (s_copyMode == MOVE_MODE || s_copySrcRow<0) ? (MAX_MODELS+sub+s_copyTgtOfs) % MAX_MODELS : s_copySrcRow;
          s_copyMode = 0;
        }
        break;
#if defined(ROTARY_ENCODERS)
      case EVT_KEY_BREAK(BTN_REa):
      case EVT_KEY_BREAK(BTN_REb):
        if (navigationRotaryEncoder(event))
          s_editMode = (s_editMode == 0 && sub == g_eeGeneral.currModel) ? -1 : 0;
        break;
      case EVT_KEY_LONG(BTN_REa):
      case EVT_KEY_LONG(BTN_REb):
        if (!navigationRotaryEncoder(event))
          break;
        // no break
#endif
      case EVT_KEY_LONG(KEY_ENTER):
      case EVT_KEY_BREAK(KEY_ENTER):
        s_editMode = 0;
        if (s_copyMode && (s_copyTgtOfs || s_copySrcRow>=0)) {
          displayPopup(s_copyMode==COPY_MODE ? STR_COPYINGMODEL : STR_MOVINGMODEL);
          eeCheck(true); // force writing of current model data before this is changed

          uint8_t cur = (MAX_MODELS + sub + s_copyTgtOfs) % MAX_MODELS;

          if (s_copyMode == COPY_MODE) {
            if (!eeCopyModel(cur, s_copySrcRow))
              cur = sub;
          }

          s_copySrcRow = g_eeGeneral.currModel; // to update the currModel value
          while (sub != cur) {
            uint8_t src = cur;
            cur = (s_copyTgtOfs > 0 ? cur+MAX_MODELS-1 : cur+1) % MAX_MODELS;
            eeSwapModels(src, cur);
            if (src == s_copySrcRow)
              s_copySrcRow = cur;
            else if (cur == s_copySrcRow)
              s_copySrcRow = src;
          }

          if (s_copySrcRow != g_eeGeneral.currModel) {
            g_eeGeneral.currModel = s_copySrcRow;
            STORE_GENERALVARS;
          }

          s_copyMode = 0;
          event = EVT_ENTRY_UP;
        }
        else if (event == EVT_KEY_LONG(KEY_MENU) || IS_RE_NAVIGATION_EVT_TYPE(event, EVT_KEY_LONG)) {
          s_copyMode = 0;
          killEvents(event);
#if defined(SDCARD)
          if (g_eeGeneral.currModel != sub) {
            if (eeModelExists(sub)) {
              s_menu[s_menu_count++] = STR_SELECT_MODEL;
              s_menu[s_menu_count++] = STR_BACKUP_MODEL;
              s_menu[s_menu_count++] = STR_RESTORE_MODEL;
              s_menu[s_menu_count++] = STR_DELETE_MODEL;
            }
            else {
              s_menu[s_menu_count++] = STR_CREATE_MODEL;
              s_menu[s_menu_count++] = STR_RESTORE_MODEL;
            }
          }
          else {
            s_menu[s_menu_count++] = STR_BACKUP_MODEL;
            s_menu[s_menu_count++] = STR_RESTORE_MODEL;
          }
#else
          if (g_eeGeneral.currModel != sub) {
            displayPopup(STR_LOADINGMODEL);
            saveTimers();
            eeCheck(true); // force writing of current model data before this is changed
            g_eeGeneral.currModel = sub;
            STORE_GENERALVARS;
            eeLoadModel(sub);
          }
#endif
        }
        else if (eeModelExists(sub)) {
          s_copyMode = (s_copyMode == COPY_MODE ? MOVE_MODE : COPY_MODE);
          s_copyTgtOfs = 0;
          s_copySrcRow = -1;
        }
        break;
      case EVT_KEY_FIRST(KEY_LEFT):
      case EVT_KEY_FIRST(KEY_RIGHT):
        if (sub == g_eeGeneral.currModel) {
          chainMenu(event == EVT_KEY_FIRST(KEY_RIGHT) ? menuModelSetup : menuTabModel[DIM(menuTabModel)-1]);
          return;
        }
        AUDIO_WARNING2();
        break;
#if !defined(PCBX9D)
      case EVT_KEY_FIRST(KEY_UP):
      case EVT_KEY_FIRST(KEY_DOWN):
        if (s_copyMode) {
          int8_t next_ofs = (event == EVT_KEY_FIRST(KEY_UP) ? s_copyTgtOfs+1 : s_copyTgtOfs-1);
          if (next_ofs == MAX_MODELS || next_ofs == -MAX_MODELS)
            next_ofs = 0;

          if (s_copySrcRow < 0 && s_copyMode==COPY_MODE) {
            s_copySrcRow = oldSub;
            // find a hole (in the first empty slot above / below)
            m_posVert = eeFindEmptyModel(s_copySrcRow, event==EVT_KEY_FIRST(KEY_DOWN));
            if ((uint8_t)m_posVert == 0xff) {
              // no free room for duplicating the model
              AUDIO_ERROR();
              m_posVert = oldSub;
              s_copyMode = 0;
            }
            next_ofs = 0;
            sub = m_posVert;
          }
          s_copyTgtOfs = next_ofs;
        }
        break;
#endif
  }

#if !defined(PCBSKY9X)
  lcd_puts(9*FW-(LEN_FREE-4)*FW, 0, STR_FREE);
  if (refresh) reusableBuffer.models.eepromfree = EeFsGetFree();
  lcd_outdezAtt(17*FW, 0, reusableBuffer.models.eepromfree, 0);
#endif

#if defined(ROTARY_ENCODERS)
  displayScreenIndex(e_ModelSelect, DIM(menuTabModel), (sub == g_eeGeneral.currModel) ? ((g_eeGeneral.reNavigation && s_editMode < 0) ? INVERS|BLINK : INVERS) : 0);
#else
  displayScreenIndex(e_ModelSelect, DIM(menuTabModel), (sub == g_eeGeneral.currModel) ? INVERS : 0);
#endif

  if (sub-s_pgOfs < 1) s_pgOfs = max(0, sub-1);
  else if (sub-s_pgOfs > 5)  s_pgOfs = min(MAX_MODELS-7, sub-4);

  for (uint8_t i=0; i<7; i++) {
    uint8_t y=(i+1)*FH;
    uint8_t k=i+s_pgOfs;
    lcd_outdezNAtt(3*FW+2, y, k+1, LEADING0+((!s_copyMode && sub==k) ? INVERS : 0), 2);

    if (s_copyMode == MOVE_MODE || (s_copyMode == COPY_MODE && s_copySrcRow >= 0)) {
      if (k == sub) {
        if (s_copyMode == COPY_MODE) {
          k = s_copySrcRow;
          lcd_putc(DISPLAY_W-FW, y, '+');
        }
        else {
          k = sub + s_copyTgtOfs;
        }
      }
      else if (s_copyTgtOfs < 0 && ((k < sub && k >= sub+s_copyTgtOfs) || (k-MAX_MODELS < sub && k-MAX_MODELS >= sub+s_copyTgtOfs)))
        k += 1;
      else if (s_copyTgtOfs > 0 && ((k > sub && k <= sub+s_copyTgtOfs) || (k+MAX_MODELS > sub && k+MAX_MODELS <= sub+s_copyTgtOfs)))
        k += MAX_MODELS-1;
    }

    k %= MAX_MODELS;

    if (eeModelExists(k)) {
#if defined(PCBSKY9X)
      putsModelName(4*FW, y, ModelNames[k], k, 0);
#else
      uint16_t & size = reusableBuffer.models.listsizes[i];
      char * name = reusableBuffer.models.listnames[i];
      if (refresh) size = eeLoadModelName(k, name);
      putsModelName(4*FW, y, name, k, 0);
      lcd_outdezAtt(20*FW, y, size, 0);
#endif
      if (k==g_eeGeneral.currModel && (s_copyMode!=COPY_MODE || s_copySrcRow<0 || i+s_pgOfs!=(pgofs_t)sub)) lcd_putc(1, y, '*');
    }

    if (s_copyMode && (pgofs_t)sub==i+s_pgOfs) {
      lcd_filled_rect(9, y, DISPLAY_W-1-9, 7);
      lcd_rect(8, y-1, DISPLAY_W-1-7, 9, s_copyMode == COPY_MODE ? SOLID : DOTTED);
    }
  }

  if (s_warning) {
#if defined(PCBSKY9X)
    s_warning_info = ModelNames[sub];
#else
    char * name = reusableBuffer.models.mainname;
    if (refresh) eeLoadModelName(sub, name);
    s_warning_info = name;
#endif
    s_warning_info_len = sizeof(g_model.name);
    displayWarning(event);
  }

#if defined(SDCARD)
  REFRESH(false);

  if (s_menu_count) {
    const char * result = displayMenu(_event);
    if (result) {
      REFRESH(true);
      if (result == STR_SELECT_MODEL || result == STR_CREATE_MODEL) {
        displayPopup(STR_LOADINGMODEL);
        saveTimers();
        eeCheck(true); // force writing of current model data before this is changed
        if (g_eeGeneral.currModel != sub) {
          g_eeGeneral.currModel = sub;
          STORE_GENERALVARS;
          eeLoadModel(sub);
        }
      }
      else if (result == STR_BACKUP_MODEL) {
        eeCheck(true); // force writing of current model data before this is changed
        s_warning = eeBackupModel(sub);
      }
      else if (result == STR_RESTORE_MODEL || result == STR_UPDATE_LIST) {
        if (!listSdFiles(MODELS_PATH, MODELS_EXT, 10, NULL)) {
          s_warning = STR_NO_MODELS_ON_SD;
          s_menu_flags = 0;
        }
      }
      else if (result == STR_DELETE_MODEL) {
        s_warning = STR_DELETEMODEL;
        s_warning_type = WARNING_TYPE_CONFIRM;
      }
      else {
        // The user choosed a file on SD to restore
        s_warning = eeRestoreModel(sub, (char *)result);
        if (!s_warning && g_eeGeneral.currModel == sub)
          eeLoadModel(sub);
      }
    }
  }
#endif
}

void editName(uint8_t x, uint8_t y, char *name, uint8_t size, uint8_t event, bool active, uint8_t & cur)
{
  lcd_putsLeft(y, STR_NAME);

  lcd_putsnAtt(x, y, name, size, ZCHAR | (active ? ((s_editMode>0) ? 0 : INVERS) : 0));

  if (active) {
    if (s_editMode>0) {
      uint8_t next = cur;
      char c = name[next];
      int8_t v = c;
      if (p1valdiff || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP)
          || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP)) {
         v = checkIncDec(event, abs(v), 0, ZCHAR_MAX, 0);
         if (c < 0) v = -v;
      }

      switch (event) {
        case EVT_KEY_BREAK(KEY_LEFT):
          if (next>0) next--;
          break;
        case EVT_KEY_BREAK(KEY_RIGHT):
          if (next<size-1) next++;
          break;
#if defined(ROTARY_ENCODERS)
        case EVT_KEY_LONG(BTN_REa):
        case EVT_KEY_LONG(BTN_REb):
          if (!navigationRotaryEncoder(event))
            break;
          // no break
#if defined(PCBX9D)
        case EVT_KEY_LONG(KEY_ENTER):
#endif
          if (v==0) {
            s_editMode = 0;
            killEvents(event);
            break;
          }
          // no break
#endif
        case EVT_KEY_LONG(KEY_LEFT):
        case EVT_KEY_LONG(KEY_RIGHT):
          if (v>=-26 && v<=26) {
            v = -v; // toggle case
            if (event==EVT_KEY_LONG(KEY_LEFT))
              killEvents(KEY_LEFT);
          }
          break;
      }

      if (c != v) {
        name[cur] = v;
        STORE_MODELVARS;
      }
      lcd_putcAtt(x+cur*FW, y, idx2char(v), INVERS);
      cur = next;
    }
    else {
      cur = 0;
    }
  }
}

enum menuModelSetupItems {
  ITEM_MODEL_NAME,
  ITEM_MODEL_TIMER1,
  ITEM_MODEL_TIMER2,
  ITEM_MODEL_EXTENDED_LIMITS,
  ITEM_MODEL_EXTENDED_TRIMS,
  ITEM_MODEL_TRIM_INC,
  ITEM_MODEL_THROTTLE_TRACE,
  ITEM_MODEL_THROTTLE_TRIM,
  ITEM_MODEL_THROTTLE_WARNING,
  ITEM_MODEL_SWITCHES_WARNING,
  ITEM_MODEL_BEEP_CENTER,
  ITEM_MODEL_PROTOCOL,
  ITEM_MODEL_PROTOCOL_PARAMS
};

#if defined(PCBSKY9X) || defined(PCBGRUVIN9X)
#define FIELD_TIMER_MAX 3
#else
#define FIELD_TIMER_MAX 2
#endif

#if defined(LCD212)
#define MODEL_SETUP_2ND_COLUMN (DISPLAY_W-15*FW-MENUS_SCROLLBAR_WIDTH)
#else
#define MODEL_SETUP_2ND_COLUMN (DISPLAY_W-11*FW-MENUS_SCROLLBAR_WIDTH)
#endif

void menuModelSetup(uint8_t event)
{
  lcd_outdezNAtt(7*FW,0,g_eeGeneral.currModel+1,INVERS+LEADING0,2);

  uint8_t protocol = g_model.protocol;

#ifdef DSM2
  if (event == EVT_KEY_LONG(KEY_EXIT))
    s_rangecheck_mode = 0;
#endif

  MENU(STR_MENUSETUP, menuTabModel, e_Model, ((protocol<=PROTO_PPMSIM||IS_DSM2_PROTOCOL(protocol)||IS_PXX_PROTOCOL(protocol)) ? 14 : 13), {0,ZCHAR|(sizeof(g_model.name)-1),FIELD_TIMER_MAX,FIELD_TIMER_MAX,0,0,0,0,0,0,0,NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1,1,2});

  uint8_t  sub = m_posVert - 1;
  int8_t editMode = s_editMode;

  for (uint8_t i=0; i<7; i++) {
    uint8_t y = 1*FH + i*FH;
    uint8_t k = i+s_pgOfs;
    uint8_t blink = ((editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_MODEL_NAME:
        editName(MODEL_SETUP_2ND_COLUMN, y, g_model.name, sizeof(g_model.name), event, attr, m_posHorz);
#if defined(PCBSKY9X)
        memcpy(ModelNames[g_eeGeneral.currModel], g_model.name, sizeof(g_model.name));
#endif
        break;

      case ITEM_MODEL_TIMER1:
      case ITEM_MODEL_TIMER2:
      {
        TimerData *timer = &g_model.timers[k-ITEM_MODEL_TIMER1];
        putsStrIdx(0*FW, y, STR_TIMER, k-ITEM_MODEL_TIMER1+1);
        putsTmrMode(MODEL_SETUP_2ND_COLUMN, y, timer->mode, (attr && m_posHorz==0) ? blink : 0);
        putsTime(MODEL_SETUP_2ND_COLUMN+5*FW-2, y, timer->start,
            (attr && m_posHorz==1 ? blink:0),
            (attr && m_posHorz==2 ? blink:0) );
#if defined(PCBSKY9X) || defined(PCBGRUVIN9X)
        lcd_putcAtt(MODEL_SETUP_2ND_COLUMN+10*FW-1, y, g_model.timers[k-ITEM_MODEL_TIMER1].remanent ? 'R' : '-', (attr && m_posHorz==3) ? blink : 0);
#endif
        if (attr && (editMode>0 || p1valdiff)) {
          div_t qr = div(timer->start, 60);
          switch (m_posHorz) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, timer->mode, -2*(MAX_PSWITCH+NUM_CSW), TMR_VAROFS-1+2*(MAX_PSWITCH+NUM_CSW));
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, qr.quot, 0, 59);
              timer->start = qr.rem + qr.quot*60;
              break;
            case 2:
              qr.rem -= checkIncDecModel(event, qr.rem+2, 1, 62)-2;
              timer->start -= qr.rem ;
              if ((int16_t)timer->start < 0) timer->start=0;
              break;
#if defined(PCBSKY9X) || defined(PCBGRUVIN9X)
            case 3:
              CHECK_INCDEC_MODELVAR(event, g_model.timers[k-ITEM_MODEL_TIMER1].remanent, 0, 1);
              break;
#endif
          }
        }
        break;
      }

      case ITEM_MODEL_EXTENDED_LIMITS:
        g_model.extendedLimits = onoffMenuItem(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, STR_ELIMITS, attr, event);
        break;

      case ITEM_MODEL_EXTENDED_TRIMS:
        g_model.extendedTrims = onoffMenuItem(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, STR_ETRIMS, attr, event);
        break;

      case ITEM_MODEL_TRIM_INC:
        g_model.trimInc = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_TRIMINC, STR_VTRIMINC, g_model.trimInc, 0, 4, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_TRACE:
      {
        lcd_putsLeft(y, STR_TTRACE);
        int8_t idx = 3;
        if (g_model.thrTraceSrc > NUM_POTS) idx = NUM_STICKS+NUM_ROTARY_ENCODERS+NUM_STICKS+2+3+NUM_PPM+g_model.thrTraceSrc;
        else if (g_model.thrTraceSrc > 0) idx = NUM_STICKS+g_model.thrTraceSrc;
        putsChnRaw(MODEL_SETUP_2ND_COLUMN, y, idx, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.thrTraceSrc, 0, NUM_POTS+NUM_CHNOUT);
        break;
      }

      case ITEM_MODEL_THROTTLE_TRIM:
        g_model.thrTrim = onoffMenuItem(g_model.thrTrim, MODEL_SETUP_2ND_COLUMN, y, STR_TTRIM, attr, event);
        break;

      case ITEM_MODEL_THROTTLE_WARNING:
        g_model.disableThrottleWarning = !onoffMenuItem(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEWARNING, attr, event);
        break;

      case ITEM_MODEL_SWITCHES_WARNING:
      {
        lcd_putsLeft(y, STR_SWITCHWARNING);
        uint8_t states = g_model.switchWarningStates;
        char c = !(states & 1);
        menu_lcd_onoff(MODEL_SETUP_2ND_COLUMN, y, c, attr);
        if (attr) {
          s_editMode = 0;
          switch(event) {
            case EVT_KEY_LONG(KEY_MENU):
              killEvents(event);
              getMovedSwitch();
              g_model.switchWarningStates = 0x01 + (switches_states << 1);
              // no break
            case EVT_KEY_BREAK(KEY_MENU):
            case EVT_KEY_BREAK(KEY_LEFT):
            case EVT_KEY_BREAK(KEY_RIGHT):
              g_model.switchWarningStates ^= 0x01;
              STORE_MODELVARS;
              break;
          }
        }
        if (c) {
          states >>= 1;
          for (uint8_t i=1; i<7; i++) {
            attr = 0;
            if (i == 4) {
              c = '0'+(states & 0x03);
              states >>= 2;
            }
            else {
              if (states & 0x01)
                attr = INVERS;
              c = pgm_read_byte(STR_VSWITCHES - 2 + (3*i) + (i>=5 ? 6 : 0));
              states >>= 1;
            }
            lcd_putcAtt(MODEL_SETUP_2ND_COLUMN+2*FW+i*FW, y, c, attr);
          }
        }
        break;
      }

      case ITEM_MODEL_BEEP_CENTER:
        lcd_putsLeft(y, STR_BEEPCTR);
        for (uint8_t i=0;i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS;i++)
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+i*FW, y, STR_RETA123, i, ((m_posHorz==i) && attr) ? BLINK|INVERS : ((g_model.beepANACenter & ((BeepANACenter)1<<i)) ? INVERS : 0 ) );
        if (attr) {
          if((event==EVT_KEY_BREAK(KEY_MENU)) || p1valdiff) {
            s_editMode = 0;
            g_model.beepANACenter ^= ((BeepANACenter)1<<m_posHorz);
            STORE_MODELVARS;
          }
        }
        break;

      case ITEM_MODEL_PROTOCOL:
        lcd_putsLeft(y, NO_INDENT(STR_PROTO));
        lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN, y, STR_VPROTOS, protocol,
            (attr && m_posHorz==0 ? (editMode>0 ? BLINK|INVERS : INVERS):0));
        if (protocol <= PROTO_PPMSIM) {
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+7*FW, y, STR_NCHANNELS, g_model.ppmNCH+2, (attr && m_posHorz==1) ? blink : 0);
        }
#ifdef DSM2
        else if (protocol == PROTO_DSM2) {
          if (attr && m_posHorz > 1) m_posHorz = 1;
          int8_t x = limit((int8_t)0, (int8_t)g_model.ppmNCH, (int8_t)2);
          g_model.ppmNCH = x;
          lcd_putsiAtt(MODEL_SETUP_2ND_COLUMN+5*FW, y, STR_DSM2MODE, x, (attr && m_posHorz==1) ? blink : 0);
        }
#endif
        else if (attr) {
          m_posHorz = 0;
        }
        if (attr && (editMode>0 || p1valdiff || (protocol>PROTO_PPMSIM && !IS_DSM2_PROTOCOL(protocol)))) {
          switch (m_posHorz) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.protocol,0, PROTO_MAX-1);
              break;
            case 1:
#ifdef DSM2
              if (protocol == PROTO_DSM2)
                CHECK_INCDEC_MODELVAR(event, g_model.ppmNCH, 0, 2);
              else
#endif
                CHECK_INCDEC_MODELVAR(event, g_model.ppmNCH, -2, 4);
              break;
          }
        }
        break;

      case ITEM_MODEL_PROTOCOL_PARAMS:
        if (protocol <= PROTO_PPMSIM) {
          lcd_putsLeft( y, STR_PPMFRAME);
          lcd_puts(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_MS);
          lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN, y, (int16_t)g_model.ppmFrameLength*5 + 225, ((attr && m_posHorz==0) ? (editMode>0 ? BLINK|INVERS : INVERS) : 0) | PREC1|LEFT);
          lcd_putc(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, 'u');
          lcd_outdezAtt(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, (g_model.ppmDelay*50)+300, ((attr && m_posHorz==1) ? blink : 0));
          lcd_putcAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, g_model.pulsePol ? '+' : '-', (attr && m_posHorz==2) ? INVERS : 0);

          if(attr && (editMode>0 || p1valdiff)) {
            switch (m_posHorz) {
              case 0:
                CHECK_INCDEC_MODELVAR(event, g_model.ppmFrameLength, -20, 35);
                break;
              case 1:
                CHECK_INCDEC_MODELVAR(event, g_model.ppmDelay, -4, 10);
                break;
              case 2:
                CHECK_INCDEC_MODELVAR(event, g_model.pulsePol, 0, 1);
                break;
            }
          }
        }
#if defined(DSM2) || defined(PXX)
        else if (IS_DSM2_PROTOCOL(protocol) || IS_PXX_PROTOCOL(protocol)) {
          if (attr && m_posHorz > 1 && IS_DSM2_PROTOCOL(protocol))
            m_posHorz = 0; // limit 3 column row to 2 colums for DSM2 protocol (Rx_Num and RANGE fields)

          lcd_putsLeft(y, STR_RXNUM);
          lcd_outdezNAtt(MODEL_SETUP_2ND_COLUMN-(IS_DSM2_PROTOCOL(protocol) ? 0 : 3*FW), y, g_model.modelId, ((attr && m_posHorz==0) ? (editMode>0 ? BLINK|INVERS : INVERS) : 0) | LEADING0|LEFT, 2);

#if defined(DSM2)
          if (IS_DSM2_PROTOCOL(protocol)) { // RANGE CHECK
            lcd_putsnAtt(MODEL_SETUP_2ND_COLUMN+5*FW, y, PSTR("RANGErange")+5*(s_rangecheck_mode), 5, (m_posHorz==1 ? attr : 0));
            s_rangecheck_mode = (attr && m_posHorz==1 && editMode>0); // [MENU] key toggles range check mode
          }
#endif

          if (attr && (m_posHorz==0 && (editMode>0 || p1valdiff)))
            CHECK_INCDEC_MODELVAR(event, g_model.modelId, 0, 99);

#if defined(PXX)
          if (protocol == PROTO_PXX) {
            lcd_putsAtt(MODEL_SETUP_2ND_COLUMN, y, STR_SYNCMENU, (m_posHorz==1 ? attr : 0));
            if (attr && m_posHorz==1) {
              s_editMode = 0;
              if (event==EVT_KEY_LONG(KEY_MENU)) {
                // send reset code
                pxxFlag = PXX_SEND_RXNUM;
              }
            }
          }
#endif
        }
#endif
        break;
    }
  }
}

static uint8_t s_currIdx;

#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_CZ) || defined(TRANSLATIONS_SE) || defined(TRANSLATIONS_DE)
#define MIXES_2ND_COLUMN    (13*FW)
#else
#define MIXES_2ND_COLUMN    (11*FW)
#endif

#if defined(LCD212)
#define EXPO_ONE_2ND_COLUMN (DISPLAY_W - 88)
#define EXPO_ONE_FP_WIDTH   (9*FW)
#else
#define EXPO_ONE_2ND_COLUMN (7*FW+2)
#define EXPO_ONE_FP_WIDTH   (5*FW)
#endif

uint8_t editDelay(const uint8_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay)
{
  lcd_putsLeft(y, str);
  lcd_outdezAtt(MIXES_2ND_COLUMN, y, 5*delay, attr|PREC1|LEFT);
  if (attr) CHECK_INCDEC_MODELVAR(event, delay, 0, MAX_DELAY);
  return delay;
}

#if defined(FLIGHT_PHASES)

#if defined(PCBSKY9X)
#define PhasesType uint16_t
#else
#define PhasesType uint8_t
#endif

PhasesType editPhases(uint8_t x, uint8_t y, uint8_t event, PhasesType value, uint8_t attr)
{
  lcd_putsLeft(y, STR_FPHASE);

  uint8_t posHorz = m_posHorz;

#if defined(PCBSKY9X) && !defined(LCD212)
  bool expoMenu = (x==EXPO_ONE_2ND_COLUMN-2*FW);
#endif

  for (uint8_t p=0; p<MAX_PHASES; p++) {
#if defined(PCBSKY9X) && !defined(LCD212)
    if (expoMenu && ((attr && p < posHorz-4) || (x > EXPO_ONE_2ND_COLUMN+2*FW)))
      continue;
#endif
    lcd_putcAtt(x, y, '0'+p, ((posHorz==p) && attr) ? BLINK|INVERS : ((value & (1<<p)) ? 0 : INVERS));
    x += FW;
  }

  if (attr) {
    if ((event==EVT_KEY_BREAK(KEY_MENU)) || p1valdiff) {
      s_editMode = 0;
      value ^= (1<<posHorz);
      STORE_MODELVARS;
    }
  }

  return value;
}

enum menuModelPhaseItems {
  ITEM_MODEL_PHASE_NAME,
  ITEM_MODEL_PHASE_SWITCH,
  ITEM_MODEL_PHASE_TRIMS,
  IF_ROTARY_ENCODERS(ITEM_MODEL_PHASE_ROTARY_ENCODERS)
  ITEM_MODEL_PHASE_FADE_IN,
  ITEM_MODEL_PHASE_FADE_OUT,
#if defined(GVARS) && !defined(M64)
  ITEM_MODEL_PHASE_GVARS_LABEL,
  ITEM_MODEL_PHASE_GV1,
  ITEM_MODEL_PHASE_GV2,
  ITEM_MODEL_PHASE_GV3,
  ITEM_MODEL_PHASE_GV4,
  ITEM_MODEL_PHASE_GV5,
#endif
  ITEM_MODEL_PHASE_MAX
};

void menuModelPhaseOne(uint8_t event)
{
  PhaseData *phase = phaseaddress(s_currIdx);
  putsFlightPhase(13*FW, 0, s_currIdx+1, (getFlightPhase()==s_currIdx ? BOLD : 0));

#if defined(GVARS) && !defined(M64)
  static bool editingName = false;
  if (editingName && s_editMode <= 0) editingName = false;
  if (!editingName && s_editMode > 0 && m_posHorz == 0) editingName = true;

  static const pm_uint8_t mstate_tab_phase1[] PROGMEM = {ZCHAR|(sizeof(phase->name)-1), 0, 0, (uint8_t)-1, 2, 2, 2, 2, 2};
  static const pm_uint8_t mstate_tab_others[] PROGMEM = {ZCHAR|(sizeof(phase->name)-1), 0, 3, IF_ROTARY_ENCODERS(NUM_ROTARY_ENCODERS-1) 0, 0, (uint8_t)-1, 2, 2, 2, 2, 2};

  if (!check(event, 0, NULL, 0, (s_currIdx == 0) ? mstate_tab_phase1 : mstate_tab_others, DIM(mstate_tab_others)-1, ITEM_MODEL_PHASE_MAX - 1 - (s_currIdx==0 ? (ITEM_MODEL_PHASE_FADE_IN-ITEM_MODEL_PHASE_SWITCH) : 0))) return;

  TITLE(STR_MENUFLIGHTPHASE);

  #define PHASE_ONE_FIRST_LINE (1*FH)
#else
  SUBMENU(STR_MENUFLIGHTPHASE, 3 + (s_currIdx==0 ? 0 : 2 + (bool)NUM_ROTARY_ENCODERS), {ZCHAR|(sizeof(phase->name)-1), 0, 3, IF_ROTARY_ENCODERS(NUM_ROTARY_ENCODERS-1) 0/*, 0*/});
  #define PHASE_ONE_FIRST_LINE (2*FH)
#endif

  int8_t sub = m_posVert;
  int8_t editMode = s_editMode;

#if defined(GVARS) && !defined(M64)
  if (s_currIdx == 0 && sub>=ITEM_MODEL_PHASE_SWITCH) sub += ITEM_MODEL_PHASE_FADE_IN-ITEM_MODEL_PHASE_SWITCH;

  for (uint8_t k=0; k<7; k++) {
    uint8_t y = (k+1) * FH;
    int8_t i = k + s_pgOfs;
    if (s_currIdx == 0 && i>=ITEM_MODEL_PHASE_SWITCH) i += ITEM_MODEL_PHASE_FADE_IN-ITEM_MODEL_PHASE_SWITCH;
    uint8_t attr = (sub==i ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
#else
  for (uint8_t i=0, k=0, y=PHASE_ONE_FIRST_LINE; i<ITEM_MODEL_PHASE_MAX; i++, k++, y+=FH) {
    if (s_currIdx == 0 && i==ITEM_MODEL_PHASE_SWITCH) i = ITEM_MODEL_PHASE_FADE_IN;
    uint8_t attr = (sub==k ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
#endif
    switch(i) {
      case ITEM_MODEL_PHASE_NAME:
        editName(MIXES_2ND_COLUMN, y, phase->name, sizeof(phase->name), event, attr, m_posHorz);
        break;
      case ITEM_MODEL_PHASE_SWITCH:
        phase->swtch = switchMenuItem(MIXES_2ND_COLUMN, y, phase->swtch, attr, event);
        break;
      case ITEM_MODEL_PHASE_TRIMS:
        lcd_putsLeft(y, STR_TRIMS);
        for (uint8_t t=0; t<NUM_STICKS; t++) {
          putsTrimMode(MIXES_2ND_COLUMN+(t*FW), y, s_currIdx, t, (attr && m_posHorz==t) ? ((editMode>0) ? BLINK|INVERS : INVERS) : 0);
          if (attr && m_posHorz==t && ((editMode>0) || p1valdiff)) {
            int16_t v = getRawTrimValue(s_currIdx, t);
            if (v < TRIM_EXTENDED_MAX) v = TRIM_EXTENDED_MAX;
            v = checkIncDec(event, v, TRIM_EXTENDED_MAX, TRIM_EXTENDED_MAX+MAX_PHASES-1, EE_MODEL);
            if (checkIncDec_Ret) {
              if (v == TRIM_EXTENDED_MAX) v = 0;
              setTrimValue(s_currIdx, t, v);
            }
          }
        }
        break;

#if defined(ROTARY_ENCODERS)
      case ITEM_MODEL_PHASE_ROTARY_ENCODERS:
        lcd_putsLeft(y, STR_ROTARY_ENCODER);
        for (uint8_t t=0; t<NUM_ROTARY_ENCODERS; t++) {
          putsRotaryEncoderMode(MIXES_2ND_COLUMN+(t*FW), y, s_currIdx, t, (attr && m_posHorz==t) ? ((editMode>0) ? BLINK|INVERS : INVERS) : 0);
          if (attr && m_posHorz==t && ((editMode>0) || p1valdiff)) {
#if defined(EXTRA_ROTARY_ENCODERS)
            int16_t v;
            if(t < (NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA))
              v = phaseaddress(s_currIdx)->rotaryEncoders[t];
            else
              v = g_model.rotaryEncodersExtra[s_currIdx][t-(NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA)];
#else
            int16_t v = phaseaddress(s_currIdx)->rotaryEncoders[t];
#endif
            if (v < ROTARY_ENCODER_MAX) v = ROTARY_ENCODER_MAX;
            v = checkIncDec(event, v, ROTARY_ENCODER_MAX, ROTARY_ENCODER_MAX+MAX_PHASES-1, EE_MODEL);
            if (checkIncDec_Ret) {
              if (v == ROTARY_ENCODER_MAX) v = 0;
#if defined(EXTRA_ROTARY_ENCODERS)
              if (t < (NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA))
                phaseaddress(s_currIdx)->rotaryEncoders[t] = v;
              else
                g_model.rotaryEncodersExtra[s_currIdx][t-(NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA)] = v;
#else
              phaseaddress(s_currIdx)->rotaryEncoders[t] = v;
#endif
            }
          }
        }
        break;
#endif

      case ITEM_MODEL_PHASE_FADE_IN:
        phase->fadeIn = editDelay(y, event, attr, STR_FADEIN, phase->fadeIn);
        break;
      case ITEM_MODEL_PHASE_FADE_OUT:
        phase->fadeOut = editDelay(y, event, attr, STR_FADEOUT, phase->fadeOut);
        break;

#if defined(GVARS) && !defined(M64)

      case ITEM_MODEL_PHASE_GVARS_LABEL:
        lcd_putsLeft(y, STR_GLOBAL_VARS);
        break;

      default:
      {
        uint8_t idx = i-ITEM_MODEL_PHASE_GV1;

        if (editingName)
          editName(4*FW, y, g_model.gvarsNames[idx], sizeof(gvar_name_t), event, attr, m_posHorz);
        else
          lcd_putsnAtt(4*FW, y, g_model.gvarsNames[idx], sizeof(gvar_name_t), ZCHAR|(m_posHorz==0 ? attr : 0));

        lcd_putsLeft(y, PSTR("    "));
        putsStrIdx(INDENT_WIDTH, y, STR_GV, idx+1);

        int16_t v = phase->gvars[idx];
        if (v > GVAR_MAX) {
          uint8_t p = v - GVAR_MAX - 1;
          if (p >= s_currIdx) p++;
          putsFlightPhase(MIXES_2ND_COLUMN, y, p+1, !editingName && m_posHorz==1 ? attr : 0);
        }
        else {
          lcd_putsAtt(MIXES_2ND_COLUMN, y, STR_OWN, !editingName && m_posHorz==1 ? attr : 0);
        }
        if (attr && !editingName && s_currIdx>0 && m_posHorz==1 && ((editMode>0) || p1valdiff)) {
          if (v < GVAR_MAX) v = GVAR_MAX;
          v = checkIncDec(event, v, GVAR_MAX, GVAR_MAX+MAX_PHASES-1, EE_MODEL);
          if (checkIncDec_Ret) {
            if (v == GVAR_MAX) v = 0;
            phase->gvars[idx] = v;
          }
        }

        uint8_t p = getGVarFlightPhase(s_currIdx, idx);
        lcd_outdezAtt(21*FW, y, GVAR_VALUE(idx, p), !editingName && m_posHorz==2 ? attr : 0);
        if (attr && !editingName && m_posHorz==2 && ((editMode>0) || p1valdiff))
          GVAR_VALUE(idx, p) = checkIncDec(event, GVAR_VALUE(idx, p), -1024, +1024, EE_MODEL);

        break;
      }
#endif
    }
  }
}

void menuModelPhasesAll(uint8_t event)
{
  SIMPLE_MENU(STR_MENUFLIGHTPHASES, menuTabModel, e_PhasesAll, 1+MAX_PHASES+1);

  int8_t sub = m_posVert - 1;

  switch (event) {
#if defined(ROTARY_ENCODERS)
    case EVT_KEY_BREAK(BTN_REa):
    case EVT_KEY_BREAK(BTN_REb):
      if (!navigationRotaryEncoder(event))
        break;
      // no break
#endif
    case EVT_KEY_FIRST(KEY_ENTER):
      if (sub == MAX_PHASES) {
        s_editMode = 0;
        trimsCheckTimer = 200; // 2 seconds
      }
      // no break
#if !defined(PCBX9D)
    case EVT_KEY_FIRST(KEY_RIGHT):
#endif
      if (sub >= 0 && sub < MAX_PHASES) {
        s_currIdx = sub;
        pushMenu(menuModelPhaseOne);
      }
      break;
  }

  uint8_t att;
  for (uint8_t i=0; i<MAX_PHASES; i++) {
#if defined(PCBSKY9X)
    int8_t y = (1+i-s_pgOfs)*FH;
    if (y<1*FH || y>7*FH) continue;
#else
    uint8_t y=(i+1)*FH;
#endif
    att = i==sub ? INVERS : 0;
    PhaseData *p = phaseaddress(i);
#if defined(EXTRA_ROTARY_ENCODERS)
    putsFlightPhase(0, y, i+1, att|CONDENSED|(getFlightPhase()==i ? BOLD : 0));
#else //EXTRA_ROTARY_ENCODERS
    putsFlightPhase(0, y, i+1, att|(getFlightPhase()==i ? BOLD : 0));
#endif //EXTRA_ROTARY_ENCODERS
#if defined(ROTARY_ENCODERS)
#if defined(EXTRA_ROTARY_ENCODERS)
#define NAME_OFS (-4-12)
#define SWITCH_OFS (-FW/2-2-13)
#define TRIMS_OFS  (-FW/2-4-15)
#define ROTARY_ENC_OFS (0)
#else //!EXTRA_ROTARY_ENCODERS
#define NAME_OFS (-4)
#define SWITCH_OFS (-FW/2-2)
#define TRIMS_OFS  (-FW/2-4)
#define ROTARY_ENC_OFS (2)
#endif //EXTRA_ROTARY_ENCODERS
#else
#define NAME_OFS 0
#define SWITCH_OFS (FW/2)
#define TRIMS_OFS  (FW/2)
#endif
    lcd_putsnAtt(4*FW+NAME_OFS, y, p->name, sizeof(p->name), ZCHAR);
    if (i == 0) {
      lcd_puts((5+LEN_FP_NAME)*FW+SWITCH_OFS, y, STR_DEFAULT);
    }
    else {
      putsSwitches((5+LEN_FP_NAME)*FW+SWITCH_OFS, y, p->swtch, 0);
      for (uint8_t t=0; t<NUM_STICKS; t++) {
        putsTrimMode((9+LEN_FP_NAME+t)*FW+TRIMS_OFS, y, i, t, 0);
      }
#if defined(PCBGRUVIN9X)
      for (uint8_t t=0; t<NUM_ROTARY_ENCODERS; t++) {
        putsRotaryEncoderMode((13+LEN_FP_NAME+t)*FW+TRIMS_OFS+ROTARY_ENC_OFS, y, i, t, 0);
      }
#endif
    }
#if defined(LCD212)
    if (p->fadeIn || p->fadeOut) {
      lcd_outdezAtt(24*FW, y, 5*p->fadeIn, PREC1|LEFT);
      lcd_putc(lcdLastPos, y, '/');
      lcd_outdezAtt(lcdLastPos+FW, y, 5*p->fadeOut, PREC1|LEFT);
    }
#else
    if (p->fadeIn || p->fadeOut) 
      lcd_putc(DISPLAY_W-FW-MENUS_SCROLLBAR_WIDTH, y, (p->fadeIn && p->fadeOut) ? '*' : (p->fadeIn ? 'I' : 'O'));
#endif
  }

#if defined(PCBSKY9X)
  if (s_pgOfs != MAX_PHASES-6) return;
#endif

  lcd_putsLeft(7*FH+1, STR_CHECKTRIMS);
  putsFlightPhase(OFS_CHECKTRIMS, 7*FH+1, s_perout_flight_phase+1);
  if (sub==MAX_PHASES && !trimsCheckTimer) {
    lcd_status_line();
  }
}

#endif

#ifdef HELI

enum menuModelHeliItems {
  ITEM_HELI_SWASHTYPE,
  ITEM_HELI_COLLECTIVE,
  ITEM_HELI_SWASHRING,
  ITEM_HELI_ELEDIRECTION,
  ITEM_HELI_AILDIRECTION,
  ITEM_HELI_COLDIRECTION
};

#define HELI_PARAM_OFS (14*FW)

void menuModelHeli(uint8_t event)
{
  SIMPLE_MENU(STR_MENUHELISETUP, menuTabModel, e_Heli, 7);

  uint8_t sub = m_posVert - 1;

  for (uint8_t i=0; i<6; i++) {
    uint8_t y = 1*FH + i*FH;
    uint8_t attr = (sub == i ? INVERS : 0);

    switch(i) {
      case ITEM_HELI_SWASHTYPE:
        g_model.swashR.type = selectMenuItem(HELI_PARAM_OFS, y, STR_SWASHTYPE, STR_VSWASHTYPE, g_model.swashR.type, 0, SWASH_TYPE_NUM, attr, event);
        break;

      case ITEM_HELI_COLLECTIVE:
        g_model.swashR.collectiveSource = selectMenuItem(HELI_PARAM_OFS, y, STR_COLLECTIVE, NULL, g_model.swashR.collectiveSource, 0, NUM_XCHNRAW, attr, event);
        putsChnRaw(HELI_PARAM_OFS, y, g_model.swashR.collectiveSource, attr);
        break;

      case ITEM_HELI_SWASHRING:
        lcd_putsLeft(y, STR_SWASHRING);
        lcd_outdezAtt(HELI_PARAM_OFS, y, g_model.swashR.value,  LEFT|attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.swashR.value, 0, 100);
        break;

      case ITEM_HELI_ELEDIRECTION:
        g_model.swashR.invertELE = selectMenuItem(HELI_PARAM_OFS, y, STR_ELEDIRECTION, STR_MMMINV, g_model.swashR.invertELE, 0, 1, attr, event);
        break;

      case ITEM_HELI_AILDIRECTION:
        g_model.swashR.invertAIL = selectMenuItem(HELI_PARAM_OFS, y, STR_AILDIRECTION, STR_MMMINV, g_model.swashR.invertAIL, 0, 1, attr, event);
        break;

      case ITEM_HELI_COLDIRECTION:
        g_model.swashR.invertCOL = selectMenuItem(HELI_PARAM_OFS, y, STR_COLDIRECTION, STR_MMMINV, g_model.swashR.invertCOL, 0, 1, attr, event);
        break;
    }
  }
}
#endif

typedef int16_t (*FnFuncP) (int16_t x);

int16_t expoFn(int16_t x)
{
  ExpoData *ed = expoaddress(s_currIdx);
  int16_t anas[NUM_STICKS] = {0};
  anas[ed->chn] = x;
  applyExpos(anas);
  return anas[ed->chn];
}

#if defined(CURVES)
static uint8_t s_curveChan;
int16_t curveFn(int16_t x)
{
  return intpol(x, s_curveChan);
}
#endif

void DrawCurve(FnFuncP fn, uint8_t offset=0)
{
  lcd_vlineStip(X0-offset, 0, DISPLAY_H, 0xee);
  lcd_hlineStip(X0-WCHART-offset, Y0, WCHART*2, 0xee);

  uint8_t prev_yv = 255;

  for (int8_t xv=-WCHART+1; xv<=WCHART; xv++) {
    uint16_t yv = (RESX + fn(xv * (RESX/WCHART))) / 2;
    yv = (DISPLAY_H-1) - yv * (DISPLAY_H-1) / RESX;
    if (prev_yv != 255) {
      if (abs((int8_t)yv-prev_yv) <= 1)
        lcd_plot(X0+xv-offset, prev_yv, BLACK);
      else
        lcd_vline(X0+xv-offset, prev_yv < yv ? yv : yv+1, prev_yv-yv);
    }
    prev_yv = yv;
  }
}

#if defined(CURVES)
bool moveCurve(uint8_t index, int8_t shift, int8_t custom=0)
{
  if (g_model.curves[MAX_CURVES-1] + shift > NUM_POINTS-5*MAX_CURVES) {
    AUDIO_WARNING2();
    return false;
  }

  int8_t *crv = curveaddress(index);
  if (shift < 0) {
    for (uint8_t i=0; i<custom; i++)
      crv[i] = crv[2*i];
  }

  int8_t *nextCrv = curveaddress(index+1);
  memmove(nextCrv+shift, nextCrv, 5*(MAX_CURVES-index-1)+g_model.curves[MAX_CURVES-1]-g_model.curves[index]);
  if (shift < 0) memclear(&g_model.points[NUM_POINTS-1] + shift, -shift);
  while (index<MAX_CURVES)
    g_model.curves[index++] += shift;

  for (uint8_t i=0; i<custom-2; i++)
    crv[custom+i] = -100 + ((200 * (i+1) + custom/2) / (custom-1)) ;

  eeDirty(EE_MODEL);
  return true;
}

void menuModelCurveOne(uint8_t event)
{
  TITLE(STR_MENUCURVE);
  lcd_outdezAtt(5*FW+1, 0, s_curveChan+1, INVERS|LEFT);
  DISPLAY_PROGRESS_BAR(20*FW+1);

  CurveInfo crv = curveinfo(s_curveChan);

  switch(event) {
    case EVT_ENTRY:
      s_editMode = 1;
      break;
#if defined(ROTARY_ENCODERS)
    case EVT_KEY_BREAK(BTN_REa):
    case EVT_KEY_BREAK(BTN_REb):
      if (!navigationRotaryEncoder(event))
        break;
      // no break
#endif
    case EVT_KEY_BREAK(KEY_ENTER):
      if (s_editMode <= 0)
        m_posHorz = 0;
#if defined(PCBX9D)
      else if (s_editMode == 1)
        s_editMode = (crv.custom && m_posHorz>0 && m_posHorz<crv.points-1) ? 2 : 3;
      else if (s_editMode == 2)
        s_editMode = 3;
#else
      if (s_editMode == 1 && crv.custom)
        s_editMode = 2;
#endif
      else
        s_editMode = 1;
      break;
    case EVT_KEY_LONG(KEY_ENTER):
      if (s_editMode <= 0) {
        if (++m_posHorz > 4)
          m_posHorz = -4;
        for (uint8_t i=0; i<crv.points; i++)
          crv.crv[i] = (i-(crv.points/2)) * m_posHorz * 50 / (crv.points-1);
        eeDirty(EE_MODEL);
        killEvents(event);
      }
      break;
    case EVT_KEY_FIRST(KEY_EXIT):
      if (s_editMode > 0) {
        if (--s_editMode == 0)
          m_posHorz = 0;
      }
      else
        popMenu();
      break;
    case EVT_KEY_REPT(KEY_LEFT):
    case EVT_KEY_FIRST(KEY_LEFT):
      if (s_editMode==1 && m_posHorz>0) m_posHorz--;
      if (s_editMode <= 0) {
        if (crv.custom) {
          moveCurve(s_curveChan, -crv.points+2);
        }
        else if (crv.points > MIN_POINTS) {
          moveCurve(s_curveChan, -1, (crv.points+1)/2);
        }
        else {
          AUDIO_WARNING2();
        }
        return;
      }
      break;
    case EVT_KEY_REPT(KEY_RIGHT):
    case EVT_KEY_FIRST(KEY_RIGHT):
      if (s_editMode==1 && m_posHorz<(crv.points-1)) m_posHorz++;
      if (s_editMode <= 0) {
        if (!crv.custom) {
          moveCurve(s_curveChan, crv.points-2, crv.points);
        }
        else if (crv.points < MAX_POINTS) {
          if (moveCurve(s_curveChan, 1)) {
            for (int8_t i=crv.points+crv.points-2; i>=0; i--) {
              if (i%2)
                crv.crv[i] = (crv.crv[i/2] + crv.crv[1+i/2]) / 2;
              else
                crv.crv[i] = crv.crv[i/2];
            }
          }
        }
        else {
          AUDIO_WARNING2();
        }
      }
      break;
  }

  lcd_putsLeft(7*FH, STR_CURVE_TYPE);
  uint8_t attr = (s_editMode <= 0 ? INVERS : 0);
  lcd_outdezAtt(5*FW-2, 7*FH, crv.points, LEFT|attr);
  lcd_putsAtt(lcdLastPos, 7*FH, crv.custom ? PSTR("pt'") : PSTR("pt"), attr);

  DrawCurve(curveFn);

  for (uint8_t i=0; i<crv.points; i++) {
    uint8_t xx = X0-1-WCHART+i*WCHART/(crv.points/2);
    uint8_t yy = (DISPLAY_H-1) - (100 + crv.crv[i]) * (DISPLAY_H-1) / 200;
    if (crv.custom && i>0 && i<crv.points-1)
      xx = X0-1-WCHART + (100 + (100 + crv.crv[crv.points+i-1]) * (2*WCHART)) / 200;

    lcd_filled_rect(xx, yy-1, 3, 3, SOLID, BLACK); // do markup square

    if (s_editMode>0 && m_posHorz==i) {
      if (s_editMode==1 || !BLINK_ON_PHASE) {
        // do selection square
        lcd_filled_rect(xx-1, yy-2, 5, 5, SOLID, BLACK);
        lcd_filled_rect(xx, yy-1, 3, 3, SOLID);
      }

      int8_t x = -100 + 200*i/(crv.points-1);
      if (crv.custom && i>0 && i<crv.points-1) x = crv.crv[crv.points+i-1];
#if defined(PCBX9D)
      lcd_puts(7, 2*FH, PSTR("x=")); lcd_outdezAtt(7+2*FW, 2*FH, x, LEFT|(s_editMode==2 ? BLINK|INVERS : 0));
      lcd_puts(7, 3*FH, PSTR("y=")); lcd_outdezAtt(7+2*FW, 3*FH, crv.crv[i], LEFT|(s_editMode==3 ? BLINK|INVERS : 0));
#else
      lcd_puts(7, 2*FH, PSTR("x=")); lcd_outdezAtt(7+2*FW, 2*FH, x, LEFT);
      lcd_puts(7, 3*FH, PSTR("y=")); lcd_outdezAtt(7+2*FW, 3*FH, crv.crv[i], LEFT);
#endif
      lcd_rect(3, 1*FH+4, 7*FW-2, 3*FH-2);

#if defined(PCBX9D)
      if (event==EVT_KEY_FIRST(KEY_MINUS) || event==EVT_KEY_FIRST(KEY_PLUS) || event==EVT_KEY_REPT(KEY_MINUS) || event==EVT_KEY_REPT(KEY_PLUS)) {
        if (s_editMode == 2)
          CHECK_INCDEC_MODELVAR(event, crv.crv[crv.points+i-1], i==1 ? -99 : crv.crv[crv.points+i-2]+1, i==crv.points-2 ? 99 : crv.crv[crv.points+i]-1);  // edit X
        else if (s_editMode == 3)
          CHECK_INCDEC_MODELVAR(event, crv.crv[i], -100, 100);  // edit Y
      }
#else
      if (p1valdiff || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP))
        CHECK_INCDEC_MODELVAR(event, crv.crv[i], -100, 100);  // edit Y on up/down


      if (i>0 && i<crv.points-1 && s_editMode==2 && (event==EVT_KEY_FIRST(KEY_LEFT) || event==EVT_KEY_FIRST(KEY_RIGHT) || event==EVT_KEY_REPT(KEY_LEFT) || event==EVT_KEY_REPT(KEY_RIGHT)))
        CHECK_INCDEC_MODELVAR(event, crv.crv[crv.points+i-1], i==1 ? -99 : crv.crv[crv.points+i-2]+1, i==crv.points-2 ? 99 : crv.crv[crv.points+i]-1);  // edit X on left/right
#endif
    }
  }
}
#endif

uint8_t getExpoMixCount(uint8_t expo)
{
  uint8_t count = 0;
  uint8_t ch ;

  for(int8_t i=(expo ? MAX_EXPOS-1 : MAX_MIXERS-1); i>=0; i--) {
    ch = (expo ? expoaddress(i)->mode : mixaddress(i)->srcRaw);
    if (ch != 0) {
      count++;
    }
  }
  return count;
}

bool reachExpoMixCountLimit(uint8_t expo)
{
  // check mixers count limit
  if (getExpoMixCount(expo) >= (expo ? MAX_EXPOS : MAX_MIXERS)) {
    s_warning = (expo ? STR_NOFREEEXPO : STR_NOFREEMIXER);
    return true;
  }
  return false;
}

void deleteExpoMix(uint8_t expo, uint8_t idx)
{
  pauseMixerCalculations();
  if (expo) {
    ExpoData *expo = expoaddress(idx);
    memmove(expo, expo+1, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
    memclear(&g_model.expoData[MAX_EXPOS-1], sizeof(ExpoData));
  }
  else {
    MixData *mix = mixaddress(idx);
    memmove(mix, mix+1, (MAX_MIXERS-(idx+1))*sizeof(MixData));
    memclear(&g_model.mixData[MAX_MIXERS-1], sizeof(MixData));
  }
  resumeMixerCalculations();
  STORE_MODELVARS;
}

static int8_t s_currCh;
void insertExpoMix(uint8_t expo, uint8_t idx)
{
  pauseMixerCalculations();
  if (expo) {
    ExpoData *expo = expoaddress(idx);
    memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
    memclear(expo, sizeof(ExpoData));
    expo->mode = 3; // pos&neg
    expo->chn = s_currCh - 1;
    expo->weight = 100;
  }
  else {
    MixData *mix = mixaddress(idx);
    memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
    memclear(mix, sizeof(MixData));
    mix->destCh = s_currCh-1;
    mix->srcRaw = (s_currCh > 4 ? s_currCh : channel_order(s_currCh));
    mix->weight = 100;
  }
  resumeMixerCalculations();
  STORE_MODELVARS;
}

void copyExpoMix(uint8_t expo, uint8_t idx)
{
  pauseMixerCalculations();
  if (expo) {
    ExpoData *expo = expoaddress(idx);
    memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
  }
  else {
    MixData *mix = mixaddress(idx);
    memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
  }
  resumeMixerCalculations();
  STORE_MODELVARS;
}

void memswap(void *a, void *b, uint8_t size)
{
  uint8_t *x = (uint8_t*)a;
  uint8_t *y = (uint8_t*)b;
  uint8_t temp ;

  while (size--) {
    temp = *x;
    *x++ = *y;
    *y++ = temp;
  }
}

bool swapExpoMix(uint8_t expo, uint8_t &idx, uint8_t up)
{
  void *x, *y;
  uint8_t size;
  int8_t tgt_idx = (up ? idx-1 : idx+1);

  if (expo) {
    x = (ExpoData *)expoaddress(idx);

    if (tgt_idx < 0) {
      if (((ExpoData *)x)->chn == 0)
        return false;
      ((ExpoData *)x)->chn--;
      return true;
    }

    if (tgt_idx == MAX_EXPOS) {
      if (((ExpoData *)x)->chn == NUM_STICKS-1)
        return false;
      ((ExpoData *)x)->chn++;
      return true;
    }

    y = (ExpoData *)expoaddress(tgt_idx);
    if(((ExpoData *)x)->chn != ((ExpoData *)y)->chn || !((ExpoData *)y)->mode) {
      if (up) {
        if (((ExpoData *)x)->chn>0) ((ExpoData *)x)->chn--;
        else return false;
      }
      else {
        if (((ExpoData *)x)->chn<NUM_STICKS-1) ((ExpoData *)x)->chn++;
        else return false;
      }
      return true;
    }

    size = sizeof(ExpoData);
  }
  else {
    x = (MixData *)mixaddress(idx);

    if (tgt_idx < 0) {
      if (((MixData *)x)->destCh == 0)
        return false;
      ((MixData *)x)->destCh--;
      return true;
    }

    if (tgt_idx == MAX_MIXERS) {
      if (((MixData *)x)->destCh == NUM_CHNOUT-1)
        return false;
      ((MixData *)x)->destCh++;
      return true;
    }

    y = (MixData *)mixaddress(tgt_idx);
    uint8_t destCh = ((MixData *)x)->destCh;
    if(!((MixData *)y)->srcRaw || destCh != ((MixData *)y)->destCh) {
      if (up) {
        if (destCh>0) ((MixData *)x)->destCh--;
        else return false;
      }
      else {
        if (destCh<NUM_CHNOUT-1) ((MixData *)x)->destCh++;
        else return false;
      }
      return true;
    }

    size = sizeof(MixData);
  }

  pauseMixerCalculations();
  memswap(x, y, size);
  resumeMixerCalculations();

  idx = tgt_idx;
  return true;
}

enum ExposFields {
  IF_PCBSKY9X(EXPO_FIELD_NAME)
  EXPO_FIELD_WIDTH,
  EXPO_FIELD_EXPO,
  IF_CURVES(EXPO_FIELD_CURVE)
  IF_FLIGHT_PHASES(EXPO_FIELD_FLIGHT_PHASE)
  EXPO_FIELD_SWITCH,
  EXPO_FIELD_SIDE,
  EXPO_FIELD_MAX
};

void menuModelExpoOne(uint8_t event)
{
  ExpoData *ed = expoaddress(s_currIdx);
  putsChnRaw(7*FW+FW/2,0,ed->chn+1,0);

  SUBMENU(STR_MENUDREXPO, EXPO_FIELD_MAX, {IF_PCBSKY9X(ZCHAR|(sizeof(ed->name)-1)) 0, 0, IF_CURVES(1) IF_FLIGHT_PHASES(MAX_PHASES-1) 0 /*, ...*/});

  int8_t sub = m_posVert;

  uint8_t y = FH;

  for (uint8_t i=0; i<EXPO_FIELD_MAX+1; i++) {
    uint8_t attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i)
    {
#if defined(PCBSKY9X)
      case EXPO_FIELD_NAME:
        editName(EXPO_ONE_2ND_COLUMN+3*FW-sizeof(ed->name)*FW, y, ed->name, sizeof(ed->name), event, attr, m_posHorz);
        break;
#endif
      case EXPO_FIELD_WIDTH:
        lcd_putsLeft(y, STR_WEIGHT);
        ed->weight = gvarMenuItem(EXPO_ONE_2ND_COLUMN+3*FW, y, ed->weight, 0, 100, attr, event);
        break;
      case EXPO_FIELD_EXPO:
        lcd_putsLeft(y, STR_EXPO);
        if (ed->curveMode==MODE_EXPO || ed->curveParam==0) {
          ed->curveMode = MODE_EXPO;
          ed->curveParam = gvarMenuItem(EXPO_ONE_2ND_COLUMN+3*FW, y, ed->curveParam, -100, 100, attr, event);
        }
        else {
          lcd_putsAtt(EXPO_ONE_2ND_COLUMN, y, STR_NA, attr);
        }
        break;
#if defined(CURVES)
      case EXPO_FIELD_CURVE:
        lcd_putsLeft(y, STR_CURVE);
        if (ed->curveMode!=MODE_EXPO || ed->curveParam==0) {
          putsCurve(EXPO_ONE_2ND_COLUMN, y, ed->curveParam, attr);
          if (attr) {
            CHECK_INCDEC_MODELVAR(event, ed->curveParam, 0, CURVE_BASE+MAX_CURVES-1);
            if (ed->curveParam) ed->curveMode = MODE_CURVE;
            if (ed->curveParam>=CURVE_BASE && event==EVT_KEY_FIRST(KEY_MENU)) {
              s_curveChan = ed->curveParam - CURVE_BASE;
              pushMenu(menuModelCurveOne);
            }
          }
        }
        else {
          lcd_putsAtt(EXPO_ONE_2ND_COLUMN, y, STR_NA, attr);
        }
        break;
#endif
#if defined(FLIGHT_PHASES)
      case EXPO_FIELD_FLIGHT_PHASE:
        ed->phases = editPhases(EXPO_ONE_2ND_COLUMN+3*FW-EXPO_ONE_FP_WIDTH, y, event, ed->phases, attr);
        break;
#endif
      case EXPO_FIELD_SWITCH:
        ed->swtch = switchMenuItem(EXPO_ONE_2ND_COLUMN, y, ed->swtch, attr, event);
        break;
      case EXPO_FIELD_SIDE:
        ed->mode = 4 - selectMenuItem(EXPO_ONE_2ND_COLUMN, y, STR_SIDE, STR_VSIDE, 4-ed->mode, 1, 3, attr, event);
        break;
    }
    y+=FH;
  }

  DrawCurve(expoFn);

  int16_t x512 = calibratedStick[ed->chn];
  int16_t y512 = expoFn(x512);

  lcd_outdezAtt(DISPLAY_W-8, 6*FH, x512*25/256, 0);
  lcd_outdezAtt(DISPLAY_W-8-6*FW, 1*FH, y512*25/256, 0);

  x512 = X0+x512/(RESXu/WCHART);
  y512 = (DISPLAY_H-1) - (uint16_t)((y512+RESX)/2) * (DISPLAY_H-1) / RESX;

  lcd_vline(x512, y512-3,3*2+1);
  lcd_hline(x512-3, y512,3*2+1);
}

enum MixFields {
  IF_PCBSKY9X(MIX_FIELD_NAME)
  MIX_FIELD_SOURCE,
  MIX_FIELD_WEIGHT,
  MIX_FIELD_OFFSET,
  MIX_FIELD_TRIM,
  IF_CURVES(MIX_FIELD_CURVE)
  MIX_FIELD_SWITCH,
  IF_FLIGHT_PHASES(MIX_FIELD_FLIGHT_PHASE)
  MIX_FIELD_WARNING,
  MIX_FIELD_MLTPX,
  MIX_FIELD_DELAY_UP,
  MIX_FIELD_DELAY_DOWN,
  MIX_FIELD_SLOW_UP,
  MIX_FIELD_SLOW_DOWN,
  MIX_FIELD_COUNT
};

void menuModelMixOne(uint8_t event)
{
  TITLE(s_currCh ? STR_INSERTMIX : STR_EDITMIX);
  MixData *md2 = mixaddress(s_currIdx) ;
  putsChn(lcdLastPos+1*FW,0,md2->destCh+1,0);

#if defined(ROTARY_ENCODERS)
  if (m_posVert == MIX_FIELD_TRIM && md2->srcRaw > NUM_STICKS)
    SUBMENU_NOTITLE(MIX_FIELD_COUNT, {IF_PCBSKY9X(ZCHAR|(sizeof(md2->name)-1)) 0, 0, 0, 0, IF_CURVES(1) 0, IF_FLIGHT_PHASES(MAX_PHASES-1) 0 /*, ...*/})
  else
    SUBMENU_NOTITLE(MIX_FIELD_COUNT, {IF_PCBSKY9X(ZCHAR|(sizeof(md2->name)-1)) 0, 0, 0, 1, IF_CURVES(1) 0, IF_FLIGHT_PHASES(MAX_PHASES-1) 0 /*, ...*/});
#else
  SUBMENU_NOTITLE(MIX_FIELD_COUNT, {IF_PCBSKY9X(ZCHAR|(sizeof(md2->name)-1)) 0, 0, 0, 1, IF_CURVES(1) 0, IF_FLIGHT_PHASES(MAX_PHASES-1) 0 /*, ...*/});
#endif

  int8_t sub = m_posVert;
  int8_t editMode = s_editMode;

  for (uint8_t k=0; k<7; k++) {
    uint8_t y = (k+1) * FH;
    int8_t i = k + s_pgOfs;
    uint8_t attr = (sub==i ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i) {
#if defined(PCBSKY9X)
      case MIX_FIELD_NAME:
        editName(MIXES_2ND_COLUMN, y, md2->name, sizeof(md2->name), event, attr, m_posHorz);
        break;
#endif
      case MIX_FIELD_SOURCE:
        lcd_putsLeft(y, NO_INDENT(STR_SOURCE));
        putsMixerSource(MIXES_2ND_COLUMN, y, md2->srcRaw, attr);
        if(attr) CHECK_INCDEC_MODELVAR(event, md2->srcRaw, 1, NUM_XCHNMIX);
        break;
      case MIX_FIELD_WEIGHT:
        lcd_putsLeft(y, STR_WEIGHT);
        md2->weight = gvarMenuItem(MIXES_2ND_COLUMN, y, md2->weight, -125, 125, attr|LEFT, event);
        break;
      case MIX_FIELD_OFFSET:
        lcd_putsLeft(y, NO_INDENT(STR_OFFSET));
        md2->offset = gvarMenuItem(MIXES_2ND_COLUMN, y, md2->offset, -125, 125, attr|LEFT, event);
        break;
      case MIX_FIELD_TRIM:
      {
        uint8_t not_stick = (md2->srcRaw > NUM_STICKS);
        int8_t carryTrim = -md2->carryTrim;
        lcd_putsLeft(y, STR_TRIM);
        lcd_putsiAtt((not_stick ? MIXES_2ND_COLUMN : MIXES_2ND_COLUMN-5*FW), y, STR_VMIXTRIMS, (not_stick && carryTrim == 0) ? 0 : carryTrim+1, m_posHorz==0 ? attr : 0);
        if (attr && m_posHorz==0 && (not_stick || editMode>0)) md2->carryTrim = -checkIncDecModel(event, carryTrim, not_stick ? TRIM_ON : -TRIM_OFF, -TRIM_AIL);
        if (!not_stick) {
          lcd_puts(MIXES_2ND_COLUMN, y, STR_DREX);
          menu_lcd_onoff(MIXES_2ND_COLUMN+5*FW, y, !md2->noExpo, m_posHorz==1 ? attr : 0);
          if (attr && m_posHorz==1 && editMode>0) md2->noExpo = !checkIncDecModel(event, !md2->noExpo, 0, 1);
        }
        else if (attr) {
          m_posHorz = 0;
        }
        break;
      }
#if defined(CURVES)
      case MIX_FIELD_CURVE:
      {
        int8_t curveParam = md2->curveParam;
        lcd_putsLeft(y, STR_CURVE);
        if (md2->curveMode == MODE_CURVE) {
          putsCurve(MIXES_2ND_COLUMN, y, curveParam, attr);
          if (attr) {
            if (event==EVT_KEY_FIRST(KEY_MENU) && (curveParam<0 || curveParam>=CURVE_BASE)){
              s_curveChan = (curveParam<0 ? -curveParam-1 : curveParam-CURVE_BASE);
              pushMenu(menuModelCurveOne);
            }
            else {
              CHECK_INCDEC_MODELVAR(event, md2->curveParam, -MAX_CURVES, CURVE_BASE+MAX_CURVES-1);
              if (md2->curveParam == 0)
                md2->curveMode = MODE_DIFFERENTIAL;
              m_posHorz = 0;
            }
          }
        }
        else {
          lcd_putsAtt(MIXES_2ND_COLUMN, y, PSTR("Diff"), m_posHorz==0 ? attr : 0);
          md2->curveParam = gvarMenuItem(MIXES_2ND_COLUMN+5*FW, y, curveParam, -100, 100, LEFT|(m_posHorz==1 ? attr : 0), editMode>0 ? event : 0);
          if (attr && editMode>0 && m_posHorz==0) {
            int8_t tmp = 0;
            CHECK_INCDEC_MODELVAR(event, tmp, -1, 1);
            if (tmp != 0) {
              md2->curveMode = MODE_CURVE;
              md2->curveParam = tmp;
            }
          }
        }
        break;
      }
#endif
      case MIX_FIELD_SWITCH:
        md2->swtch = switchMenuItem(MIXES_2ND_COLUMN, y, md2->swtch, attr, event);
        break;
#if defined(FLIGHT_PHASES)
      case MIX_FIELD_FLIGHT_PHASE:
        md2->phases = editPhases(MIXES_2ND_COLUMN, y, event, md2->phases, attr);
        break;
#endif
      case MIX_FIELD_WARNING:
        lcd_putsLeft(y, STR_MIXWARNING);
        if (md2->mixWarn)
          lcd_outdezAtt(MIXES_2ND_COLUMN, y, md2->mixWarn, attr|LEFT);
        else
          lcd_putsAtt(MIXES_2ND_COLUMN, y, STR_OFF, attr);
        if(attr) CHECK_INCDEC_MODELVAR( event, md2->mixWarn, 0, 3);
        break;
      case MIX_FIELD_MLTPX:
        md2->mltpx = selectMenuItem(MIXES_2ND_COLUMN, y, STR_MULTPX, STR_VMLTPX, md2->mltpx, 0, 2, attr, event);
        break;
      case MIX_FIELD_DELAY_UP:
        md2->delayUp = editDelay(y, event, attr, STR_DELAYUP, md2->delayUp);
        break;
      case MIX_FIELD_DELAY_DOWN:
        md2->delayDown = editDelay(y, event, attr, STR_DELAYDOWN, md2->delayDown);
        break;
      case MIX_FIELD_SLOW_UP:
        md2->speedUp = editDelay(y, event, attr, STR_SLOWUP, md2->speedUp);
        break;
      case MIX_FIELD_SLOW_DOWN:
        md2->speedDown = editDelay(y, event, attr, STR_SLOWDOWN, md2->speedDown);
        break;
    }
  }
}

static uint8_t s_maxLines = 8;
static uint8_t s_copySrcIdx;
static uint8_t s_copySrcCh;

#define _STR_MAX(x) PSTR("/" #x)
#define STR_MAX(x) _STR_MAX(x)

#if defined(TRANSLATIONS_CZ)
#define EXPO_LINE_WEIGHT_POS 7*FW-2
#define EXPO_LINE_EXPO_POS   10*FW+2
#define EXPO_LINE_PHASE_POS  10*FW+4
#define EXPO_LINE_SWITCH_POS 13*FW+5
#define EXPO_LINE_SELECT_POS 24
#else
#define EXPO_LINE_WEIGHT_POS 6*FW-2
#define EXPO_LINE_EXPO_POS   9*FW+1
#define EXPO_LINE_PHASE_POS  10*FW
#define EXPO_LINE_SWITCH_POS 13*FW+4
#define EXPO_LINE_SELECT_POS 18
#endif

void menuModelExpoMix(uint8_t expo, uint8_t event)
{
  uint8_t key = (event & 0x1f);

  if (s_copyMode && key==KEY_EXIT)
    event -= KEY_EXIT;

  TITLE(expo ? STR_MENUDREXPO : STR_MIXER);
  lcd_outdezAtt(lcdLastPos+2*FW+FW/2, 0, getExpoMixCount(expo));
  lcd_puts(lcdLastPos, 0, expo ? STR_MAX(MAX_EXPOS) : STR_MAX(MAX_MIXERS));
  SIMPLE_MENU_NOTITLE(menuTabModel, expo ? e_ExposAll : e_MixAll, s_maxLines);

  if (s_copyMode && key==KEY_EXIT)
    event += KEY_EXIT;

#if defined(ROTARY_ENCODERS)
  int8_t sub = m_posVert;
#else
  uint8_t sub = m_posVert;
#endif

  if (s_editMode > 0)
    s_editMode = 0;

  switch(event)
  {
    case EVT_ENTRY:
    case EVT_ENTRY_UP:
      s_copyMode = 0;
      s_copyTgtOfs = 0;
      break;
    case EVT_KEY_LONG(KEY_EXIT):
      if (s_copyMode && s_copyTgtOfs == 0) {
        deleteExpoMix(expo, s_currIdx);
      }
      killEvents(event);
      // no break
    case EVT_KEY_BREAK(KEY_EXIT):
      if (s_copyTgtOfs) {
        // cancel the current copy / move operation
        if (s_copyMode == COPY_MODE) {
          deleteExpoMix(expo, s_currIdx);
        }
        else {
          do {
            swapExpoMix(expo, s_currIdx, s_copyTgtOfs > 0);
            if (s_copyTgtOfs < 0)
              s_copyTgtOfs++;
            else
              s_copyTgtOfs--;
          } while (s_copyTgtOfs != 0);
          STORE_MODELVARS;
        }
        sub = m_posVert = s_copySrcRow;
      }
      s_copyMode = 0;
      s_copyTgtOfs = 0;
      break;
    case EVT_KEY_BREAK(KEY_ENTER):
      if (sub == 0) break;
      if (!s_currCh || (s_copyMode && !s_copyTgtOfs)) {
        s_copyMode = (s_copyMode == COPY_MODE ? MOVE_MODE : COPY_MODE);
        s_copySrcIdx = s_currIdx;
        s_copySrcCh = expo ? expoaddress(s_currIdx)->chn+1 : mixaddress(s_currIdx)->destCh+1;
        s_copySrcRow = sub;
        break;
      }
      // no break
#if defined(ROTARY_ENCODERS)
    case EVT_KEY_BREAK(BTN_REa):
    case EVT_KEY_BREAK(BTN_REb):
      if (event!=EVT_KEY_BREAK(KEY_ENTER) && !navigationRotaryEncoder(event))
        break;
      // no break
#endif
    case EVT_KEY_LONG(KEY_ENTER):
      if (sub == 0)
        break;
      killEvents(event);
      if (s_copyTgtOfs) {
        s_copyMode = 0;
        s_copyTgtOfs = 0;
      }
      else {
        if (s_copyMode) s_currCh = 0;
        if (s_currCh) {
          if (reachExpoMixCountLimit(expo)) break;
          insertExpoMix(expo, s_currIdx);
        }
        pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
        s_copyMode = 0;
        return;
      }
      break;
    case EVT_KEY_LONG(KEY_LEFT):
    case EVT_KEY_LONG(KEY_RIGHT):
      if (s_copyMode && !s_copyTgtOfs) {
        if (reachExpoMixCountLimit(expo)) break;
        s_currCh = (expo ? expoaddress(s_currIdx)->chn+1 : mixaddress(s_currIdx)->destCh+1);
        if (event == EVT_KEY_LONG(KEY_RIGHT)) s_currIdx++;
        insertExpoMix(expo, s_currIdx);
        pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
        s_copyMode = 0;
        killEvents(event);
        return;
      }
      break;
    case EVT_KEY_REPT(KEY_UP):
    case EVT_KEY_FIRST(KEY_UP):
    case EVT_KEY_REPT(KEY_DOWN):
    case EVT_KEY_FIRST(KEY_DOWN):
      if (s_copyMode) {
        uint8_t next_ofs = (key == KEY_UP ? s_copyTgtOfs - 1 : s_copyTgtOfs + 1);

        if (s_copyTgtOfs==0 && s_copyMode==COPY_MODE) {
          // insert a mix on the same channel (just above / just below)
          if (reachExpoMixCountLimit(expo)) break;
          copyExpoMix(expo, s_currIdx);
          if (key==KEY_DOWN) s_currIdx++;
          else if (sub-s_pgOfs >= 6) s_pgOfs++;
        }
        else if (next_ofs==0 && s_copyMode==COPY_MODE) {
          // delete the mix
          deleteExpoMix(expo, s_currIdx);
          if (key==KEY_UP) s_currIdx--;
        }
        else {
          // only swap the mix with its neighbor
          if (!swapExpoMix(expo, s_currIdx, key==KEY_UP)) break;
          STORE_MODELVARS;
        }

        s_copyTgtOfs = next_ofs;
      }
      break;
  }

  s_currCh = 0;
  uint8_t cur = 1;
  uint8_t i = 0;

  for (uint8_t ch=1; ch<=(expo ? NUM_STICKS : NUM_CHNOUT); ch++) {
    void *pointer = NULL; MixData * &md = (MixData * &)pointer; ExpoData * &ed = (ExpoData * &)pointer;
    uint8_t y = (cur-s_pgOfs)*FH;
    if (expo ? (i<MAX_EXPOS && (ed=expoaddress(i))->chn+1 == ch && ed->mode) : (i<MAX_MIXERS && (md=mixaddress(i))->srcRaw && md->destCh+1 == ch)) {
      if (s_pgOfs < cur && cur-s_pgOfs < 8) {
        if (expo)
          putsChnRaw(0, (cur-s_pgOfs)*FH, ch, 0);
        else
          putsChn(0, (cur-s_pgOfs)*FH, ch, 0); // show CHx
      }
      uint8_t mixCnt = 0;
      do {
        if (s_copyMode) {
          if (s_copyMode == MOVE_MODE && s_pgOfs < cur && cur-s_pgOfs < 8 && s_copySrcCh == ch && s_copyTgtOfs != 0 && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
            lcd_rect(expo ? 18 : 22, y-1, expo ? DISPLAY_W-18 : DISPLAY_W-22, 9, DOTTED);
            cur++; y+=FH;
          }
          if (s_currIdx == i) {
            sub = m_posVert = cur;
            s_currCh = ch;
          }
        }
        else if (sub == cur) {
          s_currIdx = i;
        }
        if (s_pgOfs < cur && cur-s_pgOfs < 8) {
          uint8_t attr = ((s_copyMode || sub != cur) ? 0 : INVERS);         
          if (expo) {
            ed->weight = gvarMenuItem(EXPO_LINE_WEIGHT_POS, y, ed->weight, 0, 100, attr | (isExpoActive(i) ? BOLD : 0), event);

            if (ed->curveMode == MODE_CURVE)
              putsCurve(EXPO_LINE_EXPO_POS-3*FW, y, ed->curveParam);
            else
              displayGVar(EXPO_LINE_EXPO_POS, y, ed->curveParam);

#if defined(PCBSKY9X)
            if (ed->name[0]) {
              putsSwitches(11*FW, y, ed->swtch, 0);
              lcd_putsnAtt(DISPLAY_W-sizeof(ed->name)*FW-MENUS_SCROLLBAR_WIDTH, y, ed->name, sizeof(ed->name), ZCHAR | (isExpoActive(i) ? BOLD : 0));
            }
#if !defined(LCD212)
            else
#endif
#endif
            {
              putsSwitches(EXPO_LINE_SWITCH_POS, y, ed->swtch, 0); // normal switches
              if (ed->mode!=3) lcd_putc(17*FW, y, ed->mode == 2 ? 126 : 127);//'|' : (stkVal[i] ? '<' : '>'),0);*/
            }
          }
          else {
            if (mixCnt > 0)
              lcd_putsiAtt(1*FW+0, y, STR_VMLTPX2, md->mltpx, 0);

            putsMixerSource(4*FW+0, y, md->srcRaw, isMixActive(i) ? BOLD : 0);

            md->weight = gvarMenuItem(11*FW+3, y, md->weight, -125, 125, attr, event);

#if defined(PCBSKY9X)
            if (md->name[0]) {
              lcd_putsnAtt(DISPLAY_W-sizeof(md->name)*FW-MENUS_SCROLLBAR_WIDTH, y, md->name, sizeof(md->name), ZCHAR | (isMixActive(i) ? BOLD : 0));
            }
#if !defined(LCD212)
            else
#endif
#endif
            {
              if (md->curveParam) {
                if (md->curveMode == MODE_CURVE)
                  putsCurve(12*FW+2, y, md->curveParam);
                else
                  displayGVar(15*FW+2, y, md->curveParam);
              }
              if (md->swtch) putsSwitches(16*FW, y, md->swtch);

              char cs = ' ';
              if (md->speedDown || md->speedUp)
                cs = 'S';
              if ((md->delayUp || md->delayDown))
                cs = (cs =='S' ? '*' : 'D');
              lcd_putcAtt(19*FW+7, y, cs, 0);
            }
          }
          if (s_copyMode) {
            if ((s_copyMode==COPY_MODE || s_copyTgtOfs == 0) && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
              /* draw a border around the raw on selection mode (copy/move) */
              lcd_rect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? (DISPLAY_W-EXPO_LINE_SELECT_POS) : (DISPLAY_W-22), 9, s_copyMode == COPY_MODE ? SOLID : DOTTED);
            }
            if (cur == sub) {
              /* invert the raw when it's the current one */
              lcd_filled_rect(expo ? EXPO_LINE_SELECT_POS+1 : 23, y, expo ? (DISPLAY_W-EXPO_LINE_SELECT_POS-2) : (DISPLAY_W-24), 7);
            }
          }
        }
        cur++; y+=FH; mixCnt++; i++; if (expo) ed++; else md++;
      } while (expo ? (i<MAX_EXPOS && ed->chn+1 == ch && ed->mode) : (i<MAX_MIXERS && md->srcRaw && md->destCh+1 == ch));
      if (s_copyMode == MOVE_MODE && s_pgOfs < cur && cur-s_pgOfs < 8 && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
        lcd_rect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? DISPLAY_W-EXPO_LINE_SELECT_POS : DISPLAY_W-22, 9, DOTTED);
        cur++; y+=FH;
      }
    }
    else {
      uint8_t attr = 0;
      if (sub == cur) {
        s_currIdx = i;
        s_currCh = ch;
        if (!s_copyMode) {
          attr = INVERS;
        }
      }
      if (s_pgOfs < cur && cur-s_pgOfs < 8) {
        if (expo)
          putsChnRaw(0, y, ch, attr);
        else
          putsChn(0, y, ch, attr); // show CHx
        if (s_copyMode == MOVE_MODE && s_copySrcCh == ch) {
          lcd_rect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? (DISPLAY_W-EXPO_LINE_SELECT_POS) : (DISPLAY_W-22), 9, DOTTED);
        }
      }
      cur++; y+=FH;
    }
  }
  s_maxLines = cur;
  if (sub >= s_maxLines-1) m_posVert = s_maxLines-1;
}

void menuModelExposAll(uint8_t event)
{
  return menuModelExpoMix(1, event);
}

void menuModelMixAll(uint8_t event)
{
  return menuModelExpoMix(0, event);
}

bool thrOutput(uint8_t ch)
{
  for (uint8_t i=0; i<MAX_MIXERS; i++) {
    MixData *mix = mixaddress(i);
    if (mix->destCh==ch && mix->srcRaw==MIXSRC_Thr)
      return true;
  }
  return false;
}

enum LimitsItems {
  ITEM_LIMITS_OFFSET,
  ITEM_LIMITS_MIN,
  ITEM_LIMITS_MAX,
  ITEM_LIMITS_DIRECTION,
#ifdef PPM_CENTER_ADJUSTABLE
  ITEM_LIMITS_PPM_CENTER,
#endif
#ifdef PPM_LIMITS_SYMETRICAL
  ITEM_LIMITS_SYMETRICAL,
#endif
  ITEM_LIMITS_COUNT,
  ITEM_LIMITS_MAXROW = ITEM_LIMITS_COUNT-1
};

#if defined(PCBX9D)
#define LIMITS_DIRECTION_POS  12*FW+4
#define LIMITS_MAX_POS        16*FW+4
#define LIMITS_REVERT_POS     18*FW
#define LIMITS_PPM_CENTER_POS 25*FW
#else
#ifdef PPM_LIMITS_SYMETRICAL
#ifdef PPM_CENTER_ADJUSTABLE
#define LIMITS_MAX_POS        15*FW
#define LIMITS_REVERT_POS     16*FW-3
#define LIMITS_PPM_CENTER_POS 20*FW+1
#else
#define LIMITS_DIRECTION_POS  12*FW+4
#define LIMITS_MAX_POS        16*FW+4
#define LIMITS_REVERT_POS     17*FW
#endif
#else
#ifdef PPM_CENTER_ADJUSTABLE
#define LIMITS_MAX_POS        16*FW
#define LIMITS_REVERT_POS     17*FW-2
#define LIMITS_PPM_CENTER_POS 21*FW+2
#else
#define LIMITS_MAX_POS        17*FW
#define LIMITS_REVERT_POS     18*FW
#define LIMITS_DIRECTION_POS  12*FW+5
#endif
#endif
#endif

void menuModelLimits(uint8_t event)
{
#if defined(PCBSKY9X)
  MENU(STR_MENULIMITS, menuTabModel, e_Limits, 1+NUM_CHNOUT+1, {0, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, 0});
#else
  MENU(STR_MENULIMITS, menuTabModel, e_Limits, 1+NUM_CHNOUT+1, {0, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, 0});
#endif

  uint8_t sub = m_posVert - 1;

  if (sub < NUM_CHNOUT) {
#if defined(PPM_CENTER_ADJUSTABLE)
    lcd_outdezAtt(13*FW, 0, PPM_CENTER+limitaddress(sub)->ppmCenter+g_chans512[sub]/2, 0);
    lcd_puts(13*FW, 0, STR_US);
#else
    lcd_outdezAtt(13*FW, 0, calcRESXto1000(g_chans512[sub]), PREC1);
#endif
  }

  if (s_warning_result) {
    LimitData *ld = limitaddress(sub);
    ld->revert = !ld->revert;
    s_warning_result = 0;
    AUDIO_WARNING2();
  }

  for (uint8_t i=0; i<7; i++) {
    uint8_t y = (i+1)*FH;
    uint8_t k = i+s_pgOfs;

    if (k==NUM_CHNOUT) {
      // last line available - add the "copy trim menu" line
      uint8_t attr = (sub==NUM_CHNOUT) ? INVERS : 0;
      lcd_putsAtt(3*FW+CENTER_OFS, y, STR_TRIMS2OFFSETS, s_noHi ? 0 : attr);
      if (attr) {
        s_editMode = 0;
        if (event==EVT_KEY_LONG(KEY_MENU)) {
          s_noHi = NO_HI_LEN;
          killEvents(event);
          moveTrimsToOffsets(); // if highlighted and menu pressed - move trims to offsets
        }
      }
      return;
    }

    LimitData *ld = limitaddress(k) ;

#if defined(LCD212) || !defined(PPM_CENTER_ADJUSTABLE)
    int16_t v = (ld->revert) ? -ld->offset : ld->offset;

    char swVal = '-';  // '-', '<', '>'
    if((g_chans512[k] - v) > 50) swVal = (ld->revert ? 127 : 126); // Switch to raw inputs?  - remove trim!
    if((g_chans512[k] - v) < -50) swVal = (ld->revert ? 126 : 127);
    putsChn(0, y, k+1, 0);
    lcd_putcAtt(LIMITS_DIRECTION_POS, y, swVal, 0);
#endif

#if defined(PPM_CENTER_ADJUSTABLE)
    int8_t limit = ((g_model.extendedLimits && !limitaddress(k)->ppmCenter) ? 125 : 100);
#else
    int8_t limit = (g_model.extendedLimits ? 125 : 100);
#endif

    putsChn(0, y, k+1, 0);

    for (uint8_t j=0; j<ITEM_LIMITS_COUNT; j++) {
      uint8_t attr = ((sub==k && m_posHorz==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      uint8_t active = (attr && (s_editMode>0 || p1valdiff)) ;
      switch(j)
      {
        case ITEM_LIMITS_OFFSET:
#if defined(PPM_LIMITS_UNIT_US)
          lcd_outdezAtt(  8*FW, y,  ((int32_t)ld->offset*128) / 25, attr|PREC1);
#else
          lcd_outdezAtt(  8*FW, y,  ld->offset, attr|PREC1);
#endif
          if (active) {
            ld->offset = checkIncDec(event, ld->offset, -1000, 1000, EE_MODEL|NO_INCDEC_MARKS);
          }
          else if (attr && event==EVT_KEY_LONG(KEY_MENU)) {
            pauseMixerCalculations();
            int32_t zero = (int32_t)g_chans512[k];
            perOut(e_perout_mode_nosticks+e_perout_mode_notrainer, 0);
            int32_t chan = chans[k];
            int8_t lim = ld->max+100;
            if (chan < 0) {
              chan = -chan;
              lim = ld->min-100;
            }
            zero = (zero*100000 - 10*chan*lim) / (102400 - chan);
            ld->offset = (ld->revert) ? -zero : zero;
            resumeMixerCalculations();
            s_editMode = 0;
            STORE_MODELVARS;
          }
          break;
        case ITEM_LIMITS_MIN:
#if defined(PPM_LIMITS_UNIT_US)
          lcd_outdezAtt(12*FW+1, y, (((int16_t)ld->min-100)*128) / 25, attr);
#else
          lcd_outdezAtt(12*FW, y, (int8_t)(ld->min-100), attr);
#endif
          if (active) ld->min = 100 + checkIncDecModel(event, ld->min-100, -limit, 25);
          break;
        case ITEM_LIMITS_MAX:
#if defined(PPM_LIMITS_UNIT_US)
          lcd_outdezAtt(LIMITS_MAX_POS, y, (((int16_t)ld->max+100)*128) / 25, attr);
#else
          lcd_outdezAtt(LIMITS_MAX_POS, y, (int8_t)(ld->max+100), attr);
#endif
          if (active) ld->max = -100 + checkIncDecModel(event, ld->max+100, -25, limit);
          break;
        case ITEM_LIMITS_DIRECTION:
#if defined(PPM_CENTER_ADJUSTABLE) && !defined(LCD212)
          lcd_putcAtt(LIMITS_REVERT_POS, y, ld->revert ? 127 : 126, attr);
#else
          lcd_putsiAtt(LIMITS_REVERT_POS, y, STR_MMMINV, ld->revert, attr);
#endif
          if (active) {
            bool revert_new = checkIncDecModel(event, ld->revert, 0, 1);
            if (checkIncDec_Ret && thrOutput(k)) {
              s_warning = STR_INVERT_THR;
              s_warning_type = WARNING_TYPE_CONFIRM;
            }
            else
              ld->revert = revert_new;
          }
          break;
#if defined(PPM_CENTER_ADJUSTABLE)
        case ITEM_LIMITS_PPM_CENTER:
          lcd_outdezAtt(LIMITS_PPM_CENTER_POS, y, PPM_CENTER+ld->ppmCenter, attr);
          if (active && ld->max <= 0 && ld->min >= 0) {
            CHECK_INCDEC_MODELVAR(event, ld->ppmCenter, -125, +125);
          }
          break;
#endif
#if defined(PPM_LIMITS_SYMETRICAL)
        case ITEM_LIMITS_SYMETRICAL:
          lcd_putcAtt(DISPLAY_W-FW, y, ld->symetrical ? '=' : '^', attr);
          if (active) {
            CHECK_INCDEC_MODELVAR(event, ld->symetrical, 0, 1);
          }
          break;
#endif
      }
    }
  }
}

#if defined(CURVES)

#if defined(GVARS)
#define CURVE_SELECTED() (sub >= 0 && sub < MAX_CURVES)
#define GVAR_SELECTED() (sub >= MAX_CURVES)
#else
#define CURVE_SELECTED() (sub >= 0)
#endif

void menuModelCurvesAll(uint8_t event)
{
#if defined(GVARS) && defined(M64)
  SIMPLE_MENU(STR_MENUCURVES, menuTabModel, e_CurvesAll, 1+MAX_CURVES+MAX_GVARS);
#else
  SIMPLE_MENU(STR_MENUCURVES, menuTabModel, e_CurvesAll, 1+MAX_CURVES);
#endif

  int8_t  sub    = m_posVert - 1;

  switch (event) {
#if defined(ROTARY_ENCODERS)
    case EVT_KEY_BREAK(BTN_REa):
    case EVT_KEY_BREAK(BTN_REb):
      if (!navigationRotaryEncoder(event))
        break;
      // no break
#endif
#if !defined(PCBX9D)
    case EVT_KEY_FIRST(KEY_RIGHT):
#endif
    case EVT_KEY_FIRST(KEY_ENTER):
      if (CURVE_SELECTED()) {
        s_curveChan = sub;
        pushMenu(menuModelCurveOne);
      }
      break;
  }

  for (uint8_t i = 0; i < 7; i++) {
    uint8_t y = FH + i*FH;
    uint8_t k = i + s_pgOfs;
    uint8_t attr = (sub == k ? INVERS : 0);
#if defined(GVARS) && defined(M64)
    if (k < MAX_CURVES) {
      putsStrIdx(0, y, STR_CV, k+1, attr);
    }
    else {
      putsStrIdx(0, y, STR_GV, k-MAX_CURVES+1);
      if (GVAR_SELECTED()) {
        if (attr && s_editMode>0) attr |= BLINK;
        lcd_outdezAtt(10*FW, y, GVAR_VALUE(k-MAX_CURVES, -1), attr);
        if (attr) g_model.gvars[k-MAX_CURVES] = checkIncDec(event, g_model.gvars[k-MAX_CURVES], -1000, 1000, EE_MODEL);
      }
    }
#else
    putsStrIdx(0, y, STR_CV, k+1, attr);
#endif
  }

  if (CURVE_SELECTED()) {
    s_curveChan = sub;
    DrawCurve(curveFn, 25);
  }
}
#endif

#if defined(PCBSKY9X)
enum CustomSwitchFields {
  CSW_FIELD_FUNCTION,
  CSW_FIELD_V1,
  CSW_FIELD_V2,
  CSW_FIELD_DURATION,
  CSW_FIELD_DELAY,
  CSW_FIELD_COUNT
};
#define CSW_2ND_COLUMN (9*FW)
void menuModelCustomSwitchOne(uint8_t event)
{
  TITLE(STR_MENUCUSTOMSWITCH);

  CustomSwData * cs = cswaddress(s_currIdx);
  uint8_t sw = DSW_SW1+s_currIdx;
  putsSwitches(14*FW, 0, sw, (getSwitch(sw, 0) ? BOLD : 0));
  SIMPLE_SUBMENU_NOTITLE(CSW_FIELD_COUNT);

  int8_t  sub = m_posVert;

  for (uint8_t k=0; k<7; k++) {
    uint8_t y = (k+2) * FH;
    uint8_t i = k + s_pgOfs;
    uint8_t attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    uint8_t cstate = CS_STATE(cs->func);
    switch(i) {
      case CSW_FIELD_FUNCTION:
        lcd_putsLeft(y, STR_FUNC);
        lcd_putsiAtt(CSW_2ND_COLUMN, y, STR_VCSWFUNC, cs->func, attr);
        if (attr) {
          CHECK_INCDEC_MODELVAR(event, cs->func, 0, CS_MAXF);
          if (cstate != CS_STATE(cs->func)) {
            cs->v1 = 0;
            cs->v2 = 0;
          }
        }
        break;
      case CSW_FIELD_V1:
      {
        lcd_putsLeft(y, STR_V1);
        int8_t v1_min=0, v1_max=NUM_XCHNCSW;
        if (cstate == CS_VBOOL) {
          putsSwitches(CSW_2ND_COLUMN, y, cs->v1, attr);
          v1_min = SWITCH_OFF; v1_max = SWITCH_ON;
        }
        else {
          putsChnRaw(CSW_2ND_COLUMN, y, cs->v1, attr);
        }
        if (attr) {
          CHECK_INCDEC_MODELVAR(event, cs->v1, v1_min, v1_max);
        }
        break;
      }
      case CSW_FIELD_V2:
      {
        lcd_putsLeft(y, STR_V2);
        int8_t v2_min=0, v2_max=NUM_XCHNCSW;
        if (cstate == CS_VBOOL) {
          putsSwitches(CSW_2ND_COLUMN, y, cs->v2, attr);
          v2_min = SWITCH_OFF; v2_max = SWITCH_ON;
        }
        else if (cstate == CS_VCOMP) {
          putsChnRaw(CSW_2ND_COLUMN, y, cs->v2, attr);
        }
        else {
#if defined(FRSKY)
          if (cs->v1 > NUM_XCHNCSW-NUM_TELEMETRY) {
            putsTelemetryChannel(CSW_2ND_COLUMN, y, cs->v1 - (NUM_XCHNCSW-NUM_TELEMETRY+1), cstate==CS_VOFS ? convertTelemValue(cs->v1 - (NUM_XCHNCSW-NUM_TELEMETRY), 128+cs->v2) : cs->v2, attr|LEFT);
            v2_max = maxTelemValue(cs->v1 - (NUM_XCHNCSW-NUM_TELEMETRY));
            if (cstate == CS_VOFS) {
              v2_min = -128;
              v2_max -= 128;
            }
            else {
              v2_max = min((uint8_t)127, (uint8_t)v2_max);
              v2_min = -v2_max;
            }
            if (cs->v2 > v2_max) {
              cs->v2 = v2_max;
              eeDirty(EE_MODEL);
            }
          }
          else
#endif
          {
            v2_min = -125; v2_max = 125;
            lcd_outdezAtt(CSW_2ND_COLUMN, y, cs->v2, attr|LEFT);
          }
        }

        if (attr) {
          CHECK_INCDEC_MODELVAR(event, cs->v2, v2_min, v2_max);
        }
        break;
      }
      case CSW_FIELD_DURATION:
        lcd_putsLeft(y, STR_DURATION);
        if (cs->duration > 0)
          lcd_outdezAtt(CSW_2ND_COLUMN, y, 5*cs->duration, attr|PREC1|LEFT);
        else
          lcd_putsiAtt(CSW_2ND_COLUMN, y, STR_MMMINV, 0, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, cs->duration, 0, MAX_CSW_DURATION);
        break;
      case CSW_FIELD_DELAY:
        lcd_putsLeft(y, STR_DELAY);
        if (cs->delay > 0)
          lcd_outdezAtt(CSW_2ND_COLUMN, y, 5*cs->delay, attr|PREC1|LEFT);
        else
          lcd_putsiAtt(CSW_2ND_COLUMN, y, STR_MMMINV, 0, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, cs->delay, 0, MAX_CSW_DELAY);
        break;
    }
  }
}

void menuModelCustomSwitches(uint8_t event)
{
  SIMPLE_MENU(STR_MENUCUSTOMSWITCHES, menuTabModel, e_CustomSwitches, NUM_CSW+1);

  uint8_t y = 0;
  uint8_t k = 0;
  int8_t sub = m_posVert - 1;

  switch (event) {
#if defined(ROTARY_ENCODERS)
    case EVT_KEY_BREAK(BTN_REa):
    case EVT_KEY_BREAK(BTN_REb):
      if (!navigationRotaryEncoder(event))
        break;
      // no break
#endif
#if !defined(PCBX9D)
    case EVT_KEY_FIRST(KEY_RIGHT):
#endif
    case EVT_KEY_FIRST(KEY_ENTER):
      if (sub >= 0) {
        s_currIdx = sub;
        pushMenu(menuModelCustomSwitchOne);
      }
      break;
  }

  for (uint8_t i=0; i<7; i++) {
    y = (i+1)*FH;
    k = i+s_pgOfs;
    CustomSwData * cs = cswaddress(k);

    // CSW name
    uint8_t sw = DSW_SW1+k;
    putsSwitches(0, y, sw, (sub==k ? INVERS : 0) | (getSwitch(sw, 0) ? BOLD : 0));

    if (cs->func > 0) {
      // CSW func
      lcd_putsiAtt(4*FW - 4, y, STR_VCSWFUNC, cs->func, 0);

      // CSW params
      uint8_t cstate = CS_STATE(cs->func);

      if (cstate == CS_VBOOL) {
        putsSwitches(12*FW-4, y, cs->v1, 0);
        putsSwitches(17*FW, y, cs->v2, 0);
      }
      else if (cstate == CS_VCOMP) {
        putsChnRaw(12*FW-4, y, cs->v1, 0);
        putsChnRaw(17*FW, y, cs->v2, 0);
      }
      else {
        putsChnRaw(12*FW-4, y, cs->v1, 0);

#if defined(FRSKY)
        if (cs->v1 > NUM_XCHNCSW-NUM_TELEMETRY) {
          putsTelemetryChannel(19*FW, y, cs->v1 - (NUM_XCHNCSW-NUM_TELEMETRY+1), cstate==CS_VOFS ? convertTelemValue(cs->v1 - (NUM_XCHNCSW-NUM_TELEMETRY), 128+cs->v2) : cs->v2, 0);
        }
        else
#endif
        {
          lcd_outdezAtt(19*FW, y, cs->v2, 0);
        }
      }
    }
  }
}
#else
void menuModelCustomSwitches(uint8_t event)
{
  MENU(STR_MENUCUSTOMSWITCHES, menuTabModel, e_CustomSwitches, NUM_CSW+1, {0, 2/*repeated...*/});

  uint8_t y = 0;
  uint8_t k = 0;
  int8_t sub = m_posVert - 1;

  for (uint8_t i=0; i<7; i++) {
    y = (i+1)*FH;
    k = i+s_pgOfs;
    uint8_t attr = (sub==k ? ((s_editMode>0) ? BLINK|INVERS : INVERS)  : 0);
    CustomSwData * cs = cswaddress(k);

    // CSW name
    uint8_t sw = DSW_SW1+k;
    putsSwitches(0, y, sw, getSwitch(sw, 0) ? BOLD : 0);

    // CSW func
    lcd_putsiAtt(4*FW - 4, y, STR_VCSWFUNC, cs->func, m_posHorz==0 ? attr : 0);

    // CSW params
    uint8_t cstate = CS_STATE(cs->func);
    int8_t v1_min=0, v1_max=NUM_XCHNCSW, v2_min=0, v2_max=NUM_XCHNCSW;

    if (cstate == CS_VBOOL) {
      putsSwitches(12*FW-4, y, cs->v1, m_posHorz==1 ? attr : 0);
      putsSwitches(17*FW, y, cs->v2, m_posHorz==2 ? attr : 0);
      v1_min = SWITCH_OFF; v1_max = SWITCH_ON;
      v2_min = SWITCH_OFF; v2_max = SWITCH_ON;
    }
    else if (cstate == CS_VCOMP) {
      putsChnRaw(12*FW-4, y, cs->v1, m_posHorz==1 ? attr : 0);
      putsChnRaw(17*FW, y, cs->v2, m_posHorz==2 ? attr : 0);
    }
    else {
      putsChnRaw(12*FW-4, y, cs->v1, (m_posHorz==1 ? attr : 0));

#if defined(FRSKY)
      if (cs->v1 > NUM_XCHNCSW-NUM_TELEMETRY) {
        putsTelemetryChannel(19*FW, y, cs->v1 - (NUM_XCHNCSW-NUM_TELEMETRY+1), cstate==CS_VOFS ? convertTelemValue(cs->v1 - (NUM_XCHNCSW-NUM_TELEMETRY), 128+cs->v2) : cs->v2, m_posHorz==2 ? attr : 0);
        v2_max = maxTelemValue(cs->v1 - (NUM_XCHNCSW-NUM_TELEMETRY));
        if (cstate == CS_VOFS) {
          v2_min = -128;
          v2_max -= 128;
        }
        else {
          v2_max = min((uint8_t)127, (uint8_t)v2_max);
          v2_min = -v2_max;
        }
        if (cs->v2 > v2_max) {
          cs->v2 = v2_max;
          eeDirty(EE_MODEL);
        }
      }
      else
#endif
      {
        lcd_outdezAtt(19*FW, y, cs->v2, m_posHorz==2 ? attr : 0);
        v2_min = -125; v2_max = 125;
      }
    }

    if ((s_editMode>0 || p1valdiff) && attr) {
      switch (m_posHorz) {
        case 0:
          CHECK_INCDEC_MODELVAR(event, cs->func, 0, CS_MAXF);
          if (cstate != CS_STATE(cs->func)) {
            cs->v1 = 0;
            cs->v2 = 0;
          }
          break;
        case 1:
          CHECK_INCDEC_MODELVAR(event, cs->v1, v1_min, v1_max);
          break;
        case 2:
          CHECK_INCDEC_MODELVAR(event, cs->v2, v2_min, v2_max);
          break;
      }
    }
  }
}
#endif

#define MODEL_CUSTOM_FUNC_3RD_COLUMN (15*FW+2)

void menuModelCustomFunctions(uint8_t event)
{
#if defined(PCBSKY9X) && defined(SDCARD)
  uint8_t _event = event;
  if (s_menu_count) {
    event = 0;
  }
#endif

  MENU(STR_MENUCUSTOMFUNC, menuTabModel, e_CustomFunctions, NUM_FSW+1, {0, 3/*repeated*/});

  uint8_t y = 0;
  uint8_t k = 0;
  int8_t  sub = m_posVert - 1;

  for (uint8_t i=0; i<7; i++) {
    y=(i+1)*FH;
    k=i+s_pgOfs;
    if(k==NUM_CHNOUT) break;
    FuncSwData *sd = &g_model.funcSw[k];
    for (uint8_t j=0; j<4; j++) {
      uint8_t attr = ((sub==k && m_posHorz==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      uint8_t active = (attr && (s_editMode>0 || p1valdiff));
      switch (j) {
        case 0:
          if (sd->func <= FUNC_TRAINER_AIL && sd->active) {
            if (sd->swtch > MAX_SWITCH+1) sd->swtch -= (MAX_SWITCH+1);
            if (sd->swtch < -MAX_SWITCH-1) sd->swtch += (MAX_SWITCH+1);
          }
          putsSwitches(3, y, sd->swtch, SWONOFF | attr | ((abs(sd->swtch) <= (MAX_SWITCH+1) && getSwitch(sd->swtch, 0) && (sd->func > FUNC_INSTANT_TRIM || sd->active)) ? BOLD : 0));
          if (active || AUTOSWITCH_MENU_LONG()) {
#if defined(PCBSKY9X)
            CHECK_INCDEC_MODELSWITCH(event, sd->swtch, SWITCH_OFF-MAX_SWITCH, SWITCH_ON+MAX_SWITCH+1+2*MAX_PSWITCH);
#else
            CHECK_INCDEC_MODELSWITCH(event, sd->swtch, SWITCH_OFF-MAX_SWITCH, SWITCH_ON+MAX_SWITCH+1);            
#endif
          }
          break;
        case 1:
          if (sd->swtch) {
            uint8_t func_displayed;
            if (sd->func < FUNC_TRAINER) {
              func_displayed = 0;
              putsChnRaw(11*FW-2, y, NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+NUM_STICKS+2+3+NUM_PPM+sd->func+1, attr);
            }
            else if (sd->func < 16 + NUM_STICKS + 1) {
              func_displayed = 1;
              if (sd->func != FUNC_TRAINER)
                putsChnRaw(13*FW-2, y, sd->func-FUNC_TRAINER, attr);
            }
#if defined(DEBUG)
            else if (sd->func == FUNC_TEST) {
              func_displayed = FUNC_TEST - 16 - NUM_STICKS - MAX_GVARS + 2;
            }
#endif
#if defined(GVARS)
            else if (sd->func >= FUNC_ADJUST_GV1) {
              func_displayed = FUNC_ADJUST_GV1 - 16 - NUM_STICKS + 1;
              putsStrIdx(12*FW-2, y, STR_GV, sd->func-FUNC_ADJUST_GV1+1, attr);
            }
#endif
            else {
              func_displayed = 2 + sd->func - 16 - NUM_STICKS - 1;
            }
            lcd_putsiAtt(5*FW-2, y, STR_VFSWFUNC, func_displayed, attr);
            if (active) {
              CHECK_INCDEC_MODELVAR( event, sd->func, 0, FUNC_MAX-1);
              if (checkIncDec_Ret) FSW_PARAM(sd) = 0;
            }
          }
          else if (attr) {
            m_posHorz = 0;
          }
          break;
        case 2:
          if (sd->swtch) {
            int16_t val_displayed = FSW_PARAM(sd);
            int8_t val_min = 0;
            uint8_t val_max = 255;
            if (sd->func == FUNC_PLAY_SOUND) {
#if defined(AUDIO)
              val_max = AU_FRSKY_LAST-AU_FRSKY_FIRST-1;
              lcd_putsiAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_FUNCSOUNDS, val_displayed, attr);
#else
              break;
#endif
            }
#if defined(HAPTIC)
            else if (sd->func == FUNC_HAPTIC) {
              val_max = 3;
              lcd_outdezAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
            }
#endif
#if defined(PCBSKY9X) && defined(SDCARD)
            else if (sd->func == FUNC_PLAY_TRACK || sd->func == FUNC_BACKGND_MUSIC) {
              if (sd->param[0] && sd->param[1])
                lcd_putsnAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, sd->param, sizeof(sd->param), attr);
              else
                lcd_putsiAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_VCSWFUNC, 0, attr);
              if (active && event==EVT_KEY_BREAK(KEY_MENU)) {
                s_editMode = 0;
                if (!listSdFiles(SOUNDS_PATH, SOUNDS_EXT, 6, sd->param)) {
                  s_warning = STR_NO_SOUNDS_ON_SD;
                  s_menu_flags = 0;
                }
              }
              break;
            }
            else if (sd->func == FUNC_PLAY_VALUE) {
              val_max = NUM_XCHNPLAY-1;
              putsChnRaw(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed+1, attr);
            }
#endif
#if defined(PCBSKY9X)
            else if (sd->func == FUNC_VOLUME) {
              val_max = NUM_XCHNRAW-1;
              putsChnRaw(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed+1, attr);
            }
#elif defined(VOICE)
            else if (sd->func == FUNC_PLAY_TRACK) {
              lcd_outdezAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed+PROMPT_CUSTOM_BASE, attr|LEFT);
            }
            else if (sd->func == FUNC_PLAY_VALUE) {
              val_max = NUM_XCHNPLAY-1;
              putsChnRaw(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed+1, attr);
            }
#endif
#if defined(SDCARD)
            else if (sd->func == FUNC_LOGS) {
              if (val_displayed) {
                lcd_outdezAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|PREC1|LEFT);
                lcd_putc(lcdLastPos, y, 's');
              }
              else {
                lcd_putsiAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_MMMINV, 0, attr);
              }
            }
#endif
            else if (sd->func == FUNC_RESET) {
              val_max = 3;
              lcd_putsiAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_VFSWRESET, FSW_PARAM(sd), attr);
            }
            else if (sd->func <= FUNC_SAFETY_CH16) {
              val_displayed = (int8_t)FSW_PARAM(sd);
              val_min = -125; val_max = 125;
              lcd_outdezAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
            }
#if defined(GVARS)
            else if (sd->func >= FUNC_ADJUST_GV1
#if defined(DEBUG)
                && sd->func != FUNC_TEST
#endif
                ) {
              val_max = NUM_XCHNRAW-1;
              putsChnRaw(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed+1, attr);
            }
#endif
            else {
              if (attr) m_posHorz = ((event & EVT_KEY_MASK) == KEY_LEFT ? 1 : 3);
              break;
            }

            if (active) {
              FSW_PARAM(sd) = checkIncDec(event, val_displayed, val_min, val_max, EE_MODEL);
            }
          }
          else if (attr) {
            m_posHorz = 0;
          }
          break;

        case 3:
          if (sd->swtch && sd->func <= FUNC_INSTANT_TRIM) {
#if defined(GRAPHICS)
            menu_lcd_onoff(20*FW, y, sd->active, attr);
#else
            menu_lcd_onoff(18*FW+2, y, sd->active, attr) ;
#endif
            if (active) CHECK_INCDEC_MODELVAR(event, sd->active, 0, 1);
          }
          else if (attr) {
            m_posHorz = 0;
          }
          break;
      }
    }
  }

#if defined(PCBSKY9X) && defined(SDCARD)
  if (s_menu_count) {
    const char * result = displayMenu(_event);
    if (result) {
      if (result == STR_UPDATE_LIST) {
        if (!listSdFiles(SOUNDS_PATH, SOUNDS_EXT, 6, NULL)) {
          s_warning = STR_NO_SOUNDS_ON_SD;
          s_menu_flags = 0;
        }
      }
      else {
        // The user choosed a wav file in the list
        memcpy(g_model.funcSw[sub].param, result, sizeof(g_model.funcSw[sub].param));
        eeDirty(EE_MODEL);
      }
    }
  }
#endif
}

enum menuModelTelemetryItems {
  ITEM_TELEMETRY_A1_LABEL,
  ITEM_TELEMETRY_A1_RANGE,
  ITEM_TELEMETRY_A1_OFFSET,
  ITEM_TELEMETRY_A1_ALARM1,
  ITEM_TELEMETRY_A1_ALARM2,
  ITEM_TELEMETRY_A2_LABEL,
  ITEM_TELEMETRY_A2_RANGE,
  ITEM_TELEMETRY_A2_OFFSET,
  ITEM_TELEMETRY_A2_ALARM1,
  ITEM_TELEMETRY_A2_ALARM2,
  ITEM_TELEMETRY_RSSI_LABEL,
  ITEM_TELEMETRY_RSSI_ALARM1,
  ITEM_TELEMETRY_RSSI_ALARM2,
#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
  ITEM_TELEMETRY_USR_LABEL,
  ITEM_TELEMETRY_USR_PROTO,
  ITEM_TELEMETRY_USR_BLADES,
#endif
  ITEM_TELEMETRY_USR_VOLTAGE_SOURCE,
  ITEM_TELEMETRY_USR_CURRENT_SOURCE,
  IF_VARIO(ITEM_TELEMETRY_VARIO_LABEL)
  IF_VARIO(ITEM_TELEMETRY_VARIO_SOURCE)
  IF_VARIO(ITEM_TELEMETRY_VARIO_SPEED)
  ITEM_TELEMETRY_SCREEN_LABEL1,
  ITEM_TELEMETRY_SCREEN_LINE1,
  ITEM_TELEMETRY_SCREEN_LINE2,
  ITEM_TELEMETRY_SCREEN_LINE3,
  ITEM_TELEMETRY_SCREEN_LINE4,
  ITEM_TELEMETRY_SCREEN_LABEL2,
  ITEM_TELEMETRY_SCREEN_LINE5,
  ITEM_TELEMETRY_SCREEN_LINE6,
  ITEM_TELEMETRY_SCREEN_LINE7,
  ITEM_TELEMETRY_SCREEN_LINE8,
#if defined(PCBSKY9X)
  ITEM_TELEMETRY_SCREEN_LABEL3,
  ITEM_TELEMETRY_SCREEN_LINE9,
  ITEM_TELEMETRY_SCREEN_LINE10,
  ITEM_TELEMETRY_SCREEN_LINE11,
  ITEM_TELEMETRY_SCREEN_LINE12,
#endif
  ITEM_TELEMETRY_MAX
};

#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
#define USRDATA_LINES (uint8_t)-1, 0, 0,
#else
#define USRDATA_LINES
#endif

#ifdef FRSKY
#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_CZ)
#define TELEM_COL2 (9*FW)
#else
#define TELEM_COL2 (8*FW)
#endif

void menuModelTelemetry(uint8_t event)
{
  MENU(STR_MENUTELEMETRY, menuTabModel, e_Telemetry, ITEM_TELEMETRY_MAX+1, {0, (uint8_t)-1, 1, 0, 2, 2, (uint8_t)-1, 1, 0, 2, 2, (uint8_t)-1, 1, 1, USRDATA_LINES 0, 0, IF_VARIO((uint8_t)-1) IF_VARIO(0) IF_VARIO(1) 0, 2, 2, 2, 2, 0, 2, 2, 2, IF_PCBSKY9X(2) IF_PCBSKY9X(0) IF_PCBSKY9X(2) IF_PCBSKY9X(2) IF_PCBSKY9X(2) 2 });

  uint8_t sub = m_posVert - 1;

  switch (event) {
    case EVT_KEY_BREAK(KEY_DOWN):
    case EVT_KEY_BREAK(KEY_UP):
#if !defined(PCBX9D)
    case EVT_KEY_BREAK(KEY_LEFT):
    case EVT_KEY_BREAK(KEY_RIGHT):
#endif
      if (s_editMode>0 && sub<=ITEM_TELEMETRY_RSSI_ALARM2)
        FRSKY_setModelAlarms(); // update FrSky module when edit mode exited
      break;
  }

  for (uint8_t i=0; i<7; i++) {
    uint8_t y = 1*FH + i*FH;
    uint8_t k = i + s_pgOfs;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);
    uint8_t ch = (k >= ITEM_TELEMETRY_A2_LABEL) ? 1 : 0;
    FrSkyChannelData & channel = g_model.frsky.channels[ch];
    uint8_t dest=TELEM_A1-1+ch;
    switch(k) {
      case ITEM_TELEMETRY_A1_LABEL:
      case ITEM_TELEMETRY_A2_LABEL:
        lcd_putsLeft(y, STR_ACHANNEL);
        lcd_outdezAtt(2*FW, y, ch+1, 0);
        putsTelemetryChannel(TELEM_COL2+6*FW, y, dest, frskyData.analog[ch].value, LEFT);
        break;

      case ITEM_TELEMETRY_A1_RANGE:
      case ITEM_TELEMETRY_A2_RANGE:
        lcd_putsLeft(y, STR_RANGE);
        putsTelemetryChannel(TELEM_COL2, y, dest, 255-channel.offset, ((attr && m_posHorz==0) ? blink : 0)|NO_UNIT|LEFT);
        lcd_putsiAtt(lcdLastPos, y, STR_VTELEMUNIT, channel.type, (attr && m_posHorz==1 ? blink : 0));
        if (attr && (s_editMode>0 || p1valdiff)) {
          if (m_posHorz == 0) {
            uint16_t ratio = checkIncDec(event, channel.ratio, 0, 256, EE_MODEL);
            if (checkIncDec_Ret) {
              if (ratio == 127 && channel.multiplier > 0) {
                channel.multiplier--; channel.ratio = 255;
              }
              else if (ratio == 256) {
                if (channel.multiplier < 3) { channel.multiplier++; channel.ratio = 128; }
              }
              else {
                channel.ratio = ratio;
              }
            }
          }
          else {
            CHECK_INCDEC_MODELVAR(event, channel.type, 0, UNIT_A1A2_MAX);
          }
        }
        break;

      case ITEM_TELEMETRY_A1_OFFSET:
      case ITEM_TELEMETRY_A2_OFFSET:
        lcd_putsLeft(y, STR_OFFSET);
        putsTelemetryChannel(TELEM_COL2, y, dest, 0, LEFT|attr);
        if (attr) channel.offset = checkIncDec(event, channel.offset, -256, 256, EE_MODEL);
        break;

      case ITEM_TELEMETRY_A1_ALARM1:
      case ITEM_TELEMETRY_A1_ALARM2:
      case ITEM_TELEMETRY_A2_ALARM1:
      case ITEM_TELEMETRY_A2_ALARM2:
      {
        uint8_t j = ((k==ITEM_TELEMETRY_A1_ALARM1 || k==ITEM_TELEMETRY_A2_ALARM1) ? 0 : 1);
        lcd_putsLeft(y, STR_ALARM);
        lcd_putsiAtt(TELEM_COL2, y, STR_VALARM, ALARM_LEVEL(ch, j), (attr && m_posHorz==0) ? blink : 0);
        lcd_putsiAtt(TELEM_COL2+4*FW, y, STR_VALARMFN, ALARM_GREATER(ch, j), (attr && m_posHorz==1) ? blink : 0);
        putsTelemetryChannel(TELEM_COL2+6*FW, y, dest, channel.alarms_value[j], (attr && m_posHorz==2 ? blink : 0) | LEFT);

        if (attr && (s_editMode>0 || p1valdiff)) {
          uint8_t t;
          switch (m_posHorz) {
           case 0:
             t = ALARM_LEVEL(ch, j);
             channel.alarms_level = (channel.alarms_level & ~(3<<(2*j))) + (checkIncDecModel(event, t, 0, 3) << (2*j));
             break;
           case 1:
             t = ALARM_GREATER(ch, j);
             if (t != checkIncDecModel(event, t, 0, 1)) {
               channel.alarms_greater ^= (1 << j);
               FRSKY_setModelAlarms();
             }
             break;
           case 2:
             channel.alarms_value[j] = checkIncDec(event, channel.alarms_value[j], 0, 255, EE_MODEL);
             break;
          }
        }
        break;
      }

      case ITEM_TELEMETRY_RSSI_LABEL:
        lcd_putsLeft(y, PSTR("RSSI"));
        break;

      case ITEM_TELEMETRY_RSSI_ALARM1:
      case ITEM_TELEMETRY_RSSI_ALARM2: {
        uint8_t j = k-ITEM_TELEMETRY_RSSI_ALARM1;
        lcd_putsLeft(y, STR_ALARM);
        lcd_putsiAtt(TELEM_COL2, y, STR_VALARM, ((2+j+g_model.frsky.rssiAlarms[j].level)%4), (attr && m_posHorz==0) ? blink : 0);
        lcd_putc(TELEM_COL2+4*FW, y, '<');
        lcd_outdezNAtt(TELEM_COL2+6*FW, y, getRssiAlarmValue(j), LEFT|((attr && m_posHorz==1) ? blink : 0), 3);

        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (m_posHorz) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.rssiAlarms[j].level, -3, 2); // circular (saves flash)
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.rssiAlarms[j].value, -30, 30);
              break;
          }
        }
        break;
      }

#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
      case ITEM_TELEMETRY_USR_LABEL:
        lcd_putsLeft(y, STR_USRDATA);
        break;

      case ITEM_TELEMETRY_USR_PROTO:
        lcd_putsLeft(y, STR_PROTO);
        lcd_putsiAtt(TELEM_COL2, y, STR_VTELPROTO, g_model.frsky.usrProto, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.frsky.usrProto, 0, 2);
        break;

      case ITEM_TELEMETRY_USR_BLADES:
        lcd_putsLeft(y, STR_BLADES);
        lcd_outdezAtt(TELEM_COL2+FWNUM, y, 2+g_model.frsky.blades, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.frsky.blades, 0, 3);
        break;
#endif

      case ITEM_TELEMETRY_USR_VOLTAGE_SOURCE:
        lcd_putsLeft(y, STR_VOLTAGE);
        lcd_putsiAtt(TELEM_COL2, y, STR_VOLTSRC, g_model.frsky.voltsSource+1, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.frsky.voltsSource, 0, 3);
        break;

      case ITEM_TELEMETRY_USR_CURRENT_SOURCE:
        lcd_putsLeft(y, STR_CURRENT);
        lcd_putsiAtt(TELEM_COL2, y, STR_VOLTSRC, g_model.frsky.currentSource, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.frsky.currentSource, 0, 3);
        break;

#if defined(VARIO)
      case ITEM_TELEMETRY_VARIO_LABEL:
        lcd_putsLeft(y, STR_VARIO);
        break;

      case ITEM_TELEMETRY_VARIO_SOURCE:
        lcd_putsLeft(y, STR_SOURCE);
        lcd_putsiAtt(TELEM_COL2, y, STR_VARIOSRC, g_model.frsky.varioSource, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioSource, VARIO_SOURCE_FIRST, VARIO_SOURCE_LAST);
        break;

      case ITEM_TELEMETRY_VARIO_SPEED:
        lcd_putsLeft(y, STR_LIMIT);
        if (!g_model.frsky.varioSpeedDownMin)
          lcd_putsAtt(TELEM_COL2, y, STR_OFF, ((attr && m_posHorz==0) ? blink : 0));
        else
          lcd_outdezAtt(TELEM_COL2, y, -VARIO_SPEED_LIMIT_MUL*(VARIO_SPEED_LIMIT_DOWN_OFF - g_model.frsky.varioSpeedDownMin), ((attr && m_posHorz==0) ? blink : 0)|PREC2|LEFT);
        lcd_outdezAtt(TELEM_COL2+6*FW, y, VARIO_SPEED_LIMIT_MUL*(g_model.frsky.varioSpeedUpMin - VARIO_SPEED_LIMIT_UP_CENTER), ((attr && m_posHorz==1) ? blink : 0)|PREC2|LEFT);

        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (m_posHorz) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioSpeedDownMin, 0, VARIO_SPEED_LIMIT_DOWN_OFF);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioSpeedUpMin, 0, VARIO_SPEED_LIMIT_UP_MAX);
              break;
          }
        }
        break;
#endif

      case ITEM_TELEMETRY_SCREEN_LABEL1:
      case ITEM_TELEMETRY_SCREEN_LABEL2:

#if defined(PCBSKY9X)
      case ITEM_TELEMETRY_SCREEN_LABEL3:
      {
        uint8_t screenIndex = (k < ITEM_TELEMETRY_SCREEN_LABEL2 ? 0 : (k < ITEM_TELEMETRY_SCREEN_LABEL3 ? 1 : 2));
        bool screenType = IS_BARS_SCREEN(screenIndex);
        putsStrIdx(0*FW, y, STR_SCREEN, screenIndex+1);
        if (screenType != selectMenuItem(10*FW, y, PSTR(""), STR_VSCREEN, screenType, 0, 1, attr, event))
          g_model.frsky.screensType ^= (1 << screenIndex);
        break;
      }
#else
      {
        uint8_t screenIndex = (k < ITEM_TELEMETRY_SCREEN_LABEL2 ? 1 : 2);
        bool screenType = g_model.frsky.screensType & screenIndex;
        putsStrIdx(0*FW, y, STR_SCREEN, screenIndex);
        if (screenType != selectMenuItem(10*FW, y, PSTR(""), STR_VSCREEN, screenType, 0, 1, attr, event))
          g_model.frsky.screensType ^= screenIndex;
        break;
      }
#endif

      case ITEM_TELEMETRY_SCREEN_LINE1:
      case ITEM_TELEMETRY_SCREEN_LINE2:
      case ITEM_TELEMETRY_SCREEN_LINE3:
      case ITEM_TELEMETRY_SCREEN_LINE4:
      case ITEM_TELEMETRY_SCREEN_LINE5:
      case ITEM_TELEMETRY_SCREEN_LINE6:
      case ITEM_TELEMETRY_SCREEN_LINE7:
      case ITEM_TELEMETRY_SCREEN_LINE8:

#if defined(PCBSKY9X)
      case ITEM_TELEMETRY_SCREEN_LINE9:
      case ITEM_TELEMETRY_SCREEN_LINE10:
      case ITEM_TELEMETRY_SCREEN_LINE11:
      case ITEM_TELEMETRY_SCREEN_LINE12:
#endif

      {
        uint8_t screenIndex, lineIndex;
        if (k < ITEM_TELEMETRY_SCREEN_LABEL2) {
          screenIndex = 0;
          lineIndex = k-ITEM_TELEMETRY_SCREEN_LINE1;
        }
#if defined(PCBSKY9X)
        else if (k >= ITEM_TELEMETRY_SCREEN_LABEL3) {
          screenIndex = 2;
          lineIndex = k-ITEM_TELEMETRY_SCREEN_LINE9;
        }
#endif
        else {
          screenIndex = 1;
          lineIndex = k-ITEM_TELEMETRY_SCREEN_LINE5;
        }
        if (IS_BARS_SCREEN(screenIndex)) {
          FrSkyBarData & bar = g_model.frsky.screens[screenIndex].bars[lineIndex];
          uint8_t barSource = bar.source;
          lcd_putsiAtt(INDENT_WIDTH, y, STR_VTELEMCHNS, barSource, (attr && m_posHorz==0) ? blink : 0);
          if (barSource) {
            putsTelemetryChannel(56-3*FW, y, barSource-1, convertTelemValue(barSource, bar.barMin), (attr && m_posHorz==1 ? blink : 0) | LEFT);
            putsTelemetryChannel(14*FW-3, y, barSource-1, convertTelemValue(barSource, 255-bar.barMax), (attr && m_posHorz==2 ? blink : 0) | LEFT);
          }
          else {
            if (attr) m_posHorz = 0;
          }
          if (attr && (s_editMode>0 || p1valdiff)) {
            switch (m_posHorz) {
              case 0:
                bar.source = checkIncDecModel(event, barSource, 0, g_model.frsky.usrProto ? TELEM_DISPLAY_MAX : TELEM_NOUSR_MAX);
                if (checkIncDec_Ret) {
                  bar.barMin = 0;
                  bar.barMax = 255-maxTelemValue(bar.source);
                }
                break;
              case 1:
                bar.barMin = checkIncDec(event, bar.barMin, 0, 254-bar.barMax, EE_MODEL|NO_INCDEC_MARKS);
                break;
              case 2:
                bar.barMax = 255 - checkIncDec(event, 255-bar.barMax, bar.barMin+1, maxTelemValue(barSource), EE_MODEL|NO_INCDEC_MARKS);
                break;
            }
          }
        }
        else {
          for (uint8_t c=0; c<2; c++) {
            uint8_t & value = g_model.frsky.screens[screenIndex].lines[lineIndex].sources[c];
            lcd_putsiAtt(c==0?INDENT_WIDTH:TELEM_COL2, y, STR_VTELEMCHNS, value, (attr && m_posHorz==c) ? blink : 0);
            if (attr && m_posHorz==c && (s_editMode>0 || p1valdiff)) {
              CHECK_INCDEC_MODELVAR(event, value, 0, g_model.frsky.usrProto ? ((lineIndex==3 && c==0) ? TELEM_STATUS_MAX : TELEM_DISPLAY_MAX) : TELEM_NOUSR_MAX);
            }
          }
          if (attr && m_posHorz == 2) m_posHorz = 0;
        }
        break;
      }
    }
  }
}
#endif

#if defined(TEMPLATES)
void menuModelTemplates(uint8_t event)
{
  SIMPLE_MENU(STR_MENUTEMPLATES, menuTabModel, e_Templates, 1+TMPL_COUNT);

  uint8_t sub = m_posVert - 1;

  if (sub < TMPL_COUNT) {
    if (s_warning_result) {
      s_warning_result = 0;
      applyTemplate(sub);
      AUDIO_WARNING2();
    }
    if (event==EVT_KEY_BREAK(KEY_ENTER)) {
      s_warning = STR_VTEMPLATES+1 + (sub * LEN2_VTEMPLATES);
      s_warning_type = WARNING_TYPE_CONFIRM;
      s_editMode = 0;
    }
  }

  uint8_t y = 1*FH;
  uint8_t k = 0;
  for (uint8_t i=0; i<7 && k<TMPL_COUNT; i++) {
    k=i+s_pgOfs;
    lcd_outdezNAtt(3*FW, y, k, (sub==k ? INVERS : 0)|LEADING0, 2);
    lcd_putsiAtt(4*FW, y, STR_VTEMPLATES, k, (sub==k ? INVERS  : 0));
    y+=FH;
  }
}
#endif

