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
#define X0     (128-WCHART-2)
#define Y0     32

enum EnumTabModel {
  e_ModelSelect,
  e_Model,
#ifdef HELI
  e_Heli,
#endif
#ifdef FLIGHT_PHASES
  e_PhasesAll,
#endif
  e_ExposAll,
  e_MixAll,
  e_Limits,
  e_CurvesAll,
  e_CustomSwitches,
  e_FunctionSwitches,
#ifdef FRSKY
  e_Telemetry,
#endif
#ifdef TEMPLATES
  e_Templates
#endif
};

void menuProcModelSelect(uint8_t event);
void menuProcModel(uint8_t event);
#ifdef HELI
void menuProcHeli(uint8_t event);
#endif
#ifdef FLIGHT_PHASES
void menuProcPhasesAll(uint8_t event);
#endif
void menuProcExposAll(uint8_t event);
void menuProcMixAll(uint8_t event);
void menuProcLimits(uint8_t event);
void menuProcCurvesAll(uint8_t event);
void menuProcCustomSwitches(uint8_t event);
void menuProcFunctionSwitches(uint8_t event);
#ifdef FRSKY
void menuProcTelemetry(uint8_t event);
#endif
#ifdef TEMPLATES
void menuProcTemplates(uint8_t event);
#endif
void menuProcExpoOne(uint8_t event);

const MenuFuncP_PROGMEM menuTabModel[] PROGMEM = {
  menuProcModelSelect,
  menuProcModel,
#ifdef HELI
  menuProcHeli,
#endif
#ifdef FLIGHT_PHASES
  menuProcPhasesAll,
#endif
  menuProcExposAll,
  menuProcMixAll,
  menuProcLimits,
  menuProcCurvesAll,
  menuProcCustomSwitches,
  menuProcFunctionSwitches,
#ifdef FRSKY
  menuProcTelemetry,
#endif
#ifdef TEMPLATES
  menuProcTemplates
#endif
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
// TODO to be elsewhere if common to many menus
const pm_char * s_sdcard_error = NULL;

bool listSdFiles(const char *path, const char *extension)
{
  FILINFO fno;
  DIR dir;
  char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif

  s_menu_count = 0;
  s_menu_more = false;
  s_menu_flags = BSS;
  uint8_t offset = 0;

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
      if (len < 5 || strcmp(fn+len-4, extension)) continue;

      if (~fno.fattrib & AM_DIR) {                   /* It is a file. */
        if (offset < s_menu_offset) {
          offset++;
          continue;
        }
        if (s_menu_count == MENU_MAX_LINES) {
          s_menu_more = true;
          break;
        }
        char *menu_entry = &s_bss_menu[s_menu_count*MENU_LINE_LENGTH];
        memset(menu_entry, 0, MENU_LINE_LENGTH);
        for (uint8_t i=0; i<MENU_LINE_LENGTH-1; i++) {
          if (fn[i] == '.')
            break;
          menu_entry[i] = fn[i];
        }
        s_menu[s_menu_count++] = menu_entry;
      }
    }
  }

  return s_menu_count;
}
#endif

void menuProcModelSelect(uint8_t event)
{
  TITLE(STR_MENUMODELSEL);

#if defined(PCBARM)
  #define REFRESH(x)
#elif defined(PCBV4) && defined(SDCARD)
  static bool refresh = true;
  #define REFRESH(x) refresh = (x)
#else
#define refresh event
#endif

  if (s_confirmation) {
    eeDeleteModel(m_posVert); // delete file
    s_confirmation = 0;
    s_copyMode = 0;
    event = EVT_ENTRY_UP;
  }

#if defined(SDCARD)
  uint8_t _event = event;
  if (s_warning || s_sdcard_error || s_menu_count) {
    _event = 0;
  }
#else
  uint8_t _event = s_warning ? 0 : event;
#endif
  uint8_t _event_ = (IS_RE_NAVIGATION_EVT(_event) ? 0 : _event);

  if (s_copyMode || !eeModelExists(g_eeGeneral.currModel)) {
    if ((_event & 0x1f) == KEY_EXIT)
      _event_ -= KEY_EXIT;
  }

  int8_t oldSub = m_posVert;
  if (!check_submenu_simple(_event_, MAX_MODELS-1)) return;
#if defined(ROTARY_ENCODERS)
  if (m_posVert < 0) m_posVert = 0;
#endif
  if (s_editMode > 0) s_editMode = 0;

#if !defined(PCBARM)
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
    chainMenu(menuProcModel);
    return;
  }
#endif

  switch(_event)
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
          killEvents(_event);
          break;
        }
        // no break
      case EVT_KEY_BREAK(KEY_EXIT):
        if (s_copyMode) {
          sub = m_posVert = (s_copyMode == MOVE_MODE || s_copySrcRow<0) ? (MAX_MODELS+sub+s_copyTgtOfs) % MAX_MODELS : s_copySrcRow;
          s_copyMode = 0;
          killEvents(_event);
        }
        break;
#if defined(ROTARY_ENCODERS)
      case EVT_KEY_BREAK(BTN_REa):
      case EVT_KEY_BREAK(BTN_REb):
        if (navigationRotaryEncoder(_event))
          s_editMode = (s_editMode == 0 && sub == g_eeGeneral.currModel) ? -1 : 0;
        break;
      case EVT_KEY_LONG(BTN_REa):
      case EVT_KEY_LONG(BTN_REb):
        if (!navigationRotaryEncoder(_event))
          break;
#endif
      case EVT_KEY_LONG(KEY_MENU):
      case EVT_KEY_BREAK(KEY_MENU):
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
        else if (_event == EVT_KEY_LONG(KEY_MENU) || IS_RE_NAVIGATION_EVT_TYPE(_event, EVT_KEY_LONG)) {
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
          chainMenu(_event == EVT_KEY_FIRST(KEY_RIGHT) ? menuProcModel : menuTabModel[DIM(menuTabModel)-1]);
          return;
        }
        AUDIO_WARNING2();
        break;
      case EVT_KEY_FIRST(KEY_UP):
      case EVT_KEY_FIRST(KEY_DOWN):
        if (s_copyMode) {
          int8_t next_ofs = (_event == EVT_KEY_FIRST(KEY_UP) ? s_copyTgtOfs+1 : s_copyTgtOfs-1);
          if (next_ofs == MAX_MODELS || next_ofs == -MAX_MODELS)
            next_ofs = 0;

          if (s_copySrcRow < 0 && s_copyMode==COPY_MODE) {
            s_copySrcRow = oldSub;
            // find a hole (in the first empty slot above / below)
            m_posVert = eeFindEmptyModel(s_copySrcRow, _event==EVT_KEY_FIRST(KEY_DOWN));
            if (m_posVert == 0xff) {
              // no free room for duplicating the model
              AUDIO_ERROR();
              m_posVert = oldSub;
              s_copyMode = 0;
            }
            next_ofs = 0;
            sub = m_posVert;
          }
          s_copyTgtOfs = next_ofs;
          killEvents(_event);
        }
        break;
  }

#if !defined(PCBARM)
  lcd_puts(9*FW-(LEN_FREE-4)*FW, 0, STR_FREE);
  if (refresh) reusableBuffer.models.eepromfree = EeFsGetFree();
  lcd_outdezAtt(17*FW, 0, reusableBuffer.models.eepromfree, 0);
#endif

  DisplayScreenIndex(e_ModelSelect, DIM(menuTabModel), (sub == g_eeGeneral.currModel) ? INVERS : 0);

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
          lcd_putc(20*FW+2, y, '+');
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
#if defined(PCBARM)
      putsModelName(4*FW, y, ModelNames[k], k, 0);
#else
      uint16_t & size = reusableBuffer.models.listsizes[i];
      char * name = reusableBuffer.models.listnames[i];
      if (refresh) size = eeLoadModelName(k, name);
      putsModelName(4*FW, y, name, k, 0);
      lcd_outdezAtt(20*FW, y, size, 0);
#endif
      if (k==g_eeGeneral.currModel && (s_copyMode!=COPY_MODE || s_copySrcRow<0 || i+s_pgOfs!=sub)) lcd_putc(1, y, '*');
    }

    if (s_copyMode && sub==i+s_pgOfs) {
      lcd_filled_rect(9, y, DISPLAY_W-1-9, 7);
      lcd_rect(8, y-1, DISPLAY_W-1-7, 9, s_copyMode == COPY_MODE ? SOLID : DOTTED);
    }
  }

  if (s_warning) {
#if defined(PCBARM)
    s_warning_info = ModelNames[sub];
#else
    char * name = reusableBuffer.models.mainname;
    if (refresh) eeLoadModelName(sub, name);
    s_warning_info = name;
#endif
    s_warning_info_len = sizeof(g_model.name);
    displayConfirmation(event);
  }

#if defined(SDCARD)
  REFRESH(false);
  if (s_sdcard_error) {
    s_warning = s_sdcard_error;
    displayWarning(event);
    if (s_warning)
      s_warning = NULL;
    else
      s_sdcard_error = NULL;
  }

  if (s_menu_count) {
    const char * result = displayMenu(event);
    if (result) {
      REFRESH(true);
      if (result == STR_SELECT_MODEL || result == STR_CREATE_MODEL) {
        displayPopup(STR_LOADINGMODEL);
        eeCheck(true); // force writing of current model data before this is changed
        if (g_eeGeneral.currModel != sub) {
          g_eeGeneral.currModel = sub;
          STORE_GENERALVARS;
          eeLoadModel(sub);
        }
      }
      else if (result == STR_BACKUP_MODEL) {
        eeCheck(true); // force writing of current model data before this is changed
        s_sdcard_error = eeBackupModel(sub);
      }
      else if (result == STR_RESTORE_MODEL || result == STR_UPDATE_LIST) {
        if (!listSdFiles(MODELS_PATH, MODELS_EXT)) {
          s_sdcard_error = PSTR("No Models on SD");
          s_menu_flags = 0;
        }
      }
      else if (result == STR_DELETE_MODEL) {
        s_warning = STR_DELETEMODEL;
      }
      else {
        // The user choosed a file on SD to restore
        s_sdcard_error = eeRestoreModel(sub, (char *)result);
        if (!s_sdcard_error && g_eeGeneral.currModel == sub)
          eeLoadModel(sub);
      }
    }
  }
#endif
}

void EditName(uint8_t x, uint8_t y, char *name, uint8_t size, uint8_t event, bool active, uint8_t & cur)
{
  lcd_putsnAtt(x, y, name, size, ZCHAR | (active ? ((s_editMode>0) ? 0 : INVERS) : 0));

  if (active) {
    if (s_editMode>0) {
      uint8_t next = cur;
      char c = name[next];
      char v = c;
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
          if (v==0) {
            s_editMode = 0;
            killEvents(BTN_REa);
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

enum menuProcModelItems {
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

#define MODEL_PARAM_OFS (10*FW+2)
void menuProcModel(uint8_t event)
{
  lcd_outdezNAtt(7*FW,0,g_eeGeneral.currModel+1,INVERS+LEADING0,2);

  uint8_t protocol = g_model.protocol;
  MENU(STR_MENUSETUP, menuTabModel, e_Model, ((protocol<=PROTO_PPMSIM||IS_DSM2_PROTOCOL(protocol)||IS_PXX_PROTOCOL(protocol)) ? 14 : 13), {0,ZCHAR|(sizeof(g_model.name)-1),2,2,0,0,0,0,0,0,0,NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1,1,2});

  uint8_t  sub = m_posVert - 1;

  for (uint8_t i=0; i<7; i++) {
    uint8_t y = 1*FH + i*FH;
    uint8_t k = i+s_pgOfs;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_MODEL_NAME:
        lcd_putsLeft(y, STR_NAME);
        EditName(MODEL_PARAM_OFS, y, g_model.name, sizeof(g_model.name), event, attr, m_posHorz);
#if defined(PCBARM)
        memcpy(ModelNames[g_eeGeneral.currModel], g_model.name, sizeof(g_model.name));
#endif
        break;

      case ITEM_MODEL_TIMER1:
      case ITEM_MODEL_TIMER2:
      {
        TimerData *timer = &g_model.timers[k-ITEM_MODEL_TIMER1];
        putsStrIdx(0*FW, y, STR_TIMER, k-ITEM_MODEL_TIMER1+1); // TODO keep that?
        putsTmrMode(MODEL_PARAM_OFS, y, timer->mode, (attr && m_posHorz==0) ? blink : 0);
        putsTime(15*FW, y, timer->val,
            (attr && m_posHorz==1 ? blink:0),
            (attr && m_posHorz==2 ? blink:0) );
        if (attr && (s_editMode>0 || p1valdiff)) {
          div_t qr = div(timer->val, 60);
          switch (m_posHorz) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, timer->mode, -2*(MAX_PSWITCH+NUM_CSW), TMR_VAROFS-1+2*(MAX_PSWITCH+NUM_CSW));
              break;
            case 1:
            {
              CHECK_INCDEC_MODELVAR(event, qr.quot, 0, 59);
              timer->val = qr.rem + qr.quot*60;
              break;
            }
            case 2:
            {
              qr.rem -= checkIncDecModel(event, qr.rem+2, 1, 62)-2;
              timer->val -= qr.rem ;
              if ((int16_t)timer->val < 0) timer->val=0;
              break;
            }
          }
        }
        break;
      }

      case ITEM_MODEL_EXTENDED_LIMITS:
        lcd_putsLeft(y, STR_ELIMITS);
        menu_lcd_onoff( MODEL_PARAM_OFS, y, g_model.extendedLimits, attr ) ;
        if(attr) CHECK_INCDEC_MODELVAR(event,g_model.extendedLimits,0,1);
        break;

      case ITEM_MODEL_EXTENDED_TRIMS:
        lcd_putsLeft(y, STR_ETRIMS);
        menu_lcd_onoff( MODEL_PARAM_OFS, y, g_model.extendedTrims, attr ) ;
        if(attr) CHECK_INCDEC_MODELVAR(event,g_model.extendedTrims,0,1);
        break;

      case ITEM_MODEL_TRIM_INC:
        lcd_putsLeft(y, STR_TRIMINC);
        lcd_putsiAtt(MODEL_PARAM_OFS, y, STR_VTRIMINC, g_model.trimInc, attr);
        if(attr) CHECK_INCDEC_MODELVAR(event,g_model.trimInc,0,4);
        break;

      case ITEM_MODEL_THROTTLE_TRACE:
      {
        lcd_putsLeft(y, STR_TTRACE);
        int8_t idx = 3;
        if (g_model.thrTraceSrc > NUM_POTS) idx = NUM_STICKS+NUM_ROTARY_ENCODERS+NUM_STICKS+2+3+NUM_PPM+g_model.thrTraceSrc;
        else if (g_model.thrTraceSrc > 0) idx = NUM_STICKS+g_model.thrTraceSrc;
        putsChnRaw(MODEL_PARAM_OFS, y, idx, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.thrTraceSrc, 0, NUM_POTS+NUM_CHNOUT);
        break;
      }

      case ITEM_MODEL_THROTTLE_TRIM:
        lcd_putsLeft(y, STR_TTRIM);
        menu_lcd_onoff(MODEL_PARAM_OFS, y, g_model.thrTrim, attr) ;
        if (attr) CHECK_INCDEC_MODELVAR(event,g_model.thrTrim,0,1);
        break;

      case ITEM_MODEL_THROTTLE_WARNING:
      {
        lcd_putsLeft(y, STR_THROTTLEWARNING);
        menu_lcd_onoff(MODEL_PARAM_OFS, y, !g_model.disableThrottleWarning, attr) ;
        if (attr) g_model.disableThrottleWarning = !checkIncDecModel(event, !g_model.disableThrottleWarning, 0, 1);
        break;
      }

      case ITEM_MODEL_SWITCHES_WARNING:
      {
        lcd_putsLeft(y, STR_SWITCHWARNING);
        uint8_t states = g_model.switchWarningStates;
        char c = !(states & 1);
        menu_lcd_onoff(MODEL_PARAM_OFS, y, c, attr);
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
            lcd_putcAtt(MODEL_PARAM_OFS+2*FW+i*FW, y, c, attr);
          }
        }
        break;
      }

      case ITEM_MODEL_BEEP_CENTER:
        lcd_putsLeft(y, STR_BEEPCTR);
        for (uint8_t i=0;i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS;i++)
          lcd_putsiAtt(MODEL_PARAM_OFS+i*FW, y, STR_RETA123, i, ((m_posHorz==i) && attr) ? BLINK|INVERS : ((g_model.beepANACenter & ((BeepANACenter)1<<i)) ? INVERS : 0 ) );
        if (attr) {
          if((event==EVT_KEY_FIRST(KEY_MENU)) || p1valdiff) {
            killEvents(event);
            s_editMode = 0;
            g_model.beepANACenter ^= ((BeepANACenter)1<<m_posHorz);
            STORE_MODELVARS;
          }
        }
        break;

      case ITEM_MODEL_PROTOCOL:
        lcd_putsLeft(y, STR_PROTO);
        lcd_putsiAtt(MODEL_PARAM_OFS, y, STR_VPROTOS, protocol,
            (attr && m_posHorz==0 ? (s_editMode>0 ? BLINK|INVERS : INVERS):0));
        if (protocol <= PROTO_PPMSIM) {
          lcd_putsiAtt(MODEL_PARAM_OFS+7*FW, y, STR_NCHANNELS, g_model.ppmNCH+2, (attr && m_posHorz==1) ? blink : 0);
        }
#ifdef DSM2
        else if (protocol == PROTO_DSM2) {
          if (m_posHorz > 1) m_posHorz = 1;
          int8_t x = limit((int8_t)0, (int8_t)g_model.ppmNCH, (int8_t)2);
          g_model.ppmNCH = x;
          lcd_putsiAtt(MODEL_PARAM_OFS+5*FW, y, STR_DSM2MODE, x, (attr && m_posHorz==1) ? blink : 0);
        }
#endif
        else if (attr) {
          m_posHorz = 0;
        }
        if (attr && (s_editMode>0 || p1valdiff || (protocol>PROTO_PPMSIM && !IS_DSM2_PROTOCOL(protocol)))) {
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
          lcd_puts(MODEL_PARAM_OFS+3*FW, y, STR_MS);
          lcd_outdezAtt(MODEL_PARAM_OFS, y, (int16_t)g_model.ppmFrameLength*5 + 225, ((attr && m_posHorz==0) ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0) | PREC1|LEFT);
          lcd_putc(MODEL_PARAM_OFS+8*FW+2, y, 'u');
          lcd_outdezAtt(MODEL_PARAM_OFS+8*FW+2, y, (g_model.ppmDelay*50)+300, ((attr && m_posHorz==1) ? blink : 0));
          lcd_putcAtt(MODEL_PARAM_OFS+10*FW, y, g_model.pulsePol ? '-' : '+', (attr && m_posHorz==2) ? INVERS : 0);

          if(attr && (s_editMode>0 || p1valdiff)) {
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
        // TODO port PPM16 ppmDelay from er9x
#if defined(DSM2) || defined(PXX)
        else if (IS_DSM2_PROTOCOL(protocol) || IS_PXX_PROTOCOL(protocol)) {
          if (attr && m_posHorz > 0 && IS_DSM2_PROTOCOL(protocol))
            m_posHorz = 0;

          lcd_putsLeft(y, STR_RXNUM);
          lcd_outdezNAtt(MODEL_PARAM_OFS-(IS_DSM2_PROTOCOL(protocol) ? 0 : 3*FW), y, g_model.modelId, ((attr && m_posHorz==0) ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0) | LEADING0|LEFT, 2);

          if (attr && (IS_DSM2_PROTOCOL(protocol) || (m_posHorz==0 && (s_editMode>0 || p1valdiff))))
            CHECK_INCDEC_MODELVAR(event, g_model.modelId, 0, 99);

#if defined(PXX)
          if (protocol == PROTO_PXX) {
            lcd_putsAtt(MODEL_PARAM_OFS, y, STR_SYNCMENU, (m_posHorz==1 ? attr : 0));
            if (attr && m_posHorz==1) {
              s_editMode = false;
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

#ifdef FLIGHT_PHASES

#if defined(TRANSLATIONS_CZ)
#define PHASES_EDIT_2ND_COLUMN 12*FW
#else
#define PHASES_EDIT_2ND_COLUMN 10*FW
#endif

void menuProcPhaseOne(uint8_t event)
{
  PhaseData *phase = phaseaddress(s_currIdx);
  putsFlightPhase(13*FW, 0, s_currIdx+1, (getFlightPhase()==s_currIdx ? BOLD : 0));

#define MAX_TRIM_LINE (3+NUM_ROTARY_ENCODERS)

  SUBMENU(STR_MENUFLIGHTPHASE, (s_currIdx==0 ? 3 : 5), {ZCHAR|(sizeof(phase->name)-1), 0, MAX_TRIM_LINE, 0/*, 0*/});

  int8_t sub = m_posVert;

  for (uint8_t i=0, k=0, y=2*FH; i<5; i++, k++, y+=FH) {
    if (s_currIdx == 0 && i==1) i = 3;
    uint8_t attr = (sub==k ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i) {
      case 0:
        lcd_putsLeft( y, STR_NAME);
        EditName(PHASES_EDIT_2ND_COLUMN, y, phase->name, sizeof(phase->name), event, attr, m_posHorz);
        break;
      case 1:
        lcd_putsLeft( y, STR_SWITCH);
        putsSwitches(PHASES_EDIT_2ND_COLUMN,  y, phase->swtch, attr);
        if (attr) CHECK_INCDEC_MODELSWITCH(event, phase->swtch, -MAX_DRSWITCH, MAX_DRSWITCH);
        break;
      case 2:
        lcd_putsLeft( y, STR_TRIMS);
        for (uint8_t t=0; t<NUM_STICKS; t++) {
          putsTrimMode(PHASES_EDIT_2ND_COLUMN+(t*FW), y, s_currIdx, t, (attr && m_posHorz==t) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
          if (attr && m_posHorz==t && ((s_editMode>0) || p1valdiff)) {
            int16_t v = getRawTrimValue(s_currIdx, t);
            if (v < TRIM_EXTENDED_MAX) v = TRIM_EXTENDED_MAX;
            v = checkIncDec(event, v, TRIM_EXTENDED_MAX, TRIM_EXTENDED_MAX+MAX_PHASES-1, EE_MODEL);
            if (checkIncDec_Ret) {
              if (v == TRIM_EXTENDED_MAX) v = 0;
              setTrimValue(s_currIdx, t, v);
            }
          }
        }
#if defined(PCBV4)
        for (uint8_t t=0; t<NUM_ROTARY_ENCODERS; t++) {
          putsRotaryEncoderMode(PHASES_EDIT_2ND_COLUMN+((4+t)*FW)+2, y, s_currIdx, t, (attr && m_posHorz==4+t) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
          if (attr && m_posHorz==4+t && ((s_editMode>0) || p1valdiff)) {
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
#endif
        break;
      case 3:
        lcd_putsLeft( y, STR_FADEIN);
        lcd_outdezAtt(PHASES_EDIT_2ND_COLUMN, y, phase->fadeIn*5, attr|PREC1|LEFT);
        if(attr) CHECK_INCDEC_MODELVAR(event, phase->fadeIn, 0, 15);
        break;
      case 4:
        lcd_putsLeft( y, STR_FADEOUT);
        lcd_outdezAtt(PHASES_EDIT_2ND_COLUMN, y, phase->fadeOut*5, attr|PREC1|LEFT);
        if(attr) CHECK_INCDEC_MODELVAR(event, phase->fadeOut, 0, 15);
        break;
    }
  }
}

void menuProcPhasesAll(uint8_t event)
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
    case EVT_KEY_FIRST(KEY_MENU):
      if (sub == MAX_PHASES) {
        s_editMode = 0;
        trimsCheckTimer = 200; // 2 seconds
      }
      // no break
    case EVT_KEY_FIRST(KEY_RIGHT):
      if (sub >= 0 && sub < MAX_PHASES) {
        s_currIdx = sub;
        pushMenu(menuProcPhaseOne);
      }
      break;
  }

  uint8_t att;
  for (uint8_t i=0; i<MAX_PHASES; i++) {
#if defined(PCBARM)
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
#if defined PCBV4
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
      lcd_puts(11*FW+SWITCH_OFS, y, STR_DEFAULT);
    }
    else {
      putsSwitches(11*FW+SWITCH_OFS, y, p->swtch, 0);
      for (uint8_t t=0; t<NUM_STICKS; t++) {
        putsTrimMode((15+t)*FW+TRIMS_OFS, y, i, t, 0);
      }
#if defined PCBV4
      for (uint8_t t=0; t<NUM_ROTARY_ENCODERS; t++) {
        putsRotaryEncoderMode((19+t)*FW+TRIMS_OFS+ROTARY_ENC_OFS, y, i, t, 0);
      }
#endif
    }
    if (p->fadeIn || p->fadeOut) 
      lcd_putc(20*FW+2, y, (p->fadeIn && p->fadeOut) ? '*' : (p->fadeIn ? 'I' : 'O'));
  }

#if defined(PCBARM)
  if (s_pgOfs != MAX_PHASES-6) return;
#endif

  att = (sub==MAX_PHASES && !trimsCheckTimer) ? INVERS : 0;
  lcd_putsAtt(0, 7*FH, STR_CHECKTRIMS, att);
  putsFlightPhase(6*FW, 7*FH, s_perout_flight_phase+1, att);
}

#endif

#ifdef HELI

void menu_lcd_HYPHINV( uint8_t x,uint8_t y, uint8_t value, uint8_t attr)
{
  lcd_putsiAtt(x, y, STR_MMMINV, value, attr) ;
}

enum menuProcHeliItems {
  ITEM_HELI_SWASHTYPE,
  ITEM_HELI_COLLECTIVE,
  ITEM_HELI_SWASHRING,
  ITEM_HELI_ELEDIRECTION,
  ITEM_HELI_AILDIRECTION,
  ITEM_HELI_COLDIRECTION
};

void menuProcHeli(uint8_t event)
{
  SIMPLE_MENU(STR_MENUHELISETUP, menuTabModel, e_Heli, 7);

  uint8_t sub = m_posVert - 1;

  for (uint8_t i=0; i<6; i++) {
    uint8_t y = 1*FH + i*FH;
    uint8_t attr = (sub == i ? INVERS : 0);

    switch(i) {
      case ITEM_HELI_SWASHTYPE:
        lcd_putsLeft(y, STR_SWASHTYPE);
        lcd_putsiAtt(14*FW, y, STR_VSWASHTYPE, g_model.swashR.type, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.swashR.type, 0, SWASH_TYPE_NUM);
        break;

      case ITEM_HELI_COLLECTIVE:
        lcd_putsLeft(y, STR_COLLECTIVE);
        putsChnRaw(14*FW, y, g_model.swashR.collectiveSource, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.swashR.collectiveSource, 0, NUM_XCHNRAW);
        break;

      case ITEM_HELI_SWASHRING:
        lcd_putsLeft(y, STR_SWASHRING);
        lcd_outdezAtt(14*FW, y, g_model.swashR.value,  LEFT|attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.swashR.value, 0, 100);
        break;

      case ITEM_HELI_ELEDIRECTION:
        lcd_putsLeft(y, STR_ELEDIRECTION);
        menu_lcd_HYPHINV(14*FW, y, g_model.swashR.invertELE, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.swashR.invertELE, 0, 1);
        break;

      case ITEM_HELI_AILDIRECTION:
        lcd_putsLeft(y, STR_AILDIRECTION);
        menu_lcd_HYPHINV(14*FW, y, g_model.swashR.invertAIL, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.swashR.invertAIL, 0, 1);
        break;

      case ITEM_HELI_COLDIRECTION:
        lcd_putsLeft(y, STR_COLDIRECTION);
        menu_lcd_HYPHINV(14*FW, y, g_model.swashR.invertCOL, attr) ;
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.swashR.invertCOL, 0, 1);
        break;
    }
  }
}
#endif

static uint8_t s_curveChan;

typedef int16_t (*FnFuncP) (int16_t x);

int16_t expoFn(int16_t x)
{
  ExpoData *ed = expoaddress(s_currIdx);
  int16_t anas[NUM_STICKS] = {0};
  anas[ed->chn] = x;
  applyExpos(anas);
  return anas[ed->chn];
}

int16_t curveFn(int16_t x)
{
  return intpol(x, s_curveChan);
}

void DrawCurve(FnFuncP fn)
{
  lcd_vlineStip(X0, 0, DISPLAY_H, 0xee);
  lcd_hlineStip(X0-WCHART, Y0, WCHART*2, 0xee);

  for (int8_t xv=-WCHART+1; xv<WCHART; xv++) {
    uint16_t yv = (RESX + fn(xv * (RESX/WCHART))) / 2;
    yv = (DISPLAY_H-1) - yv * (DISPLAY_H-1) / RESX;
    lcd_plot(X0+xv, yv, BLACK);
  }
}

void menuProcCurveOne(uint8_t event)
{
  uint8_t points;
  int8_t *crv;
  static int8_t dfltCrv;

  TITLE(STR_MENUCURVE);
  lcd_outdezAtt(5*FW+1, 0, s_curveChan+1, INVERS|LEFT);
  DISPLAY_PROGRESS_BAR(20*FW+1);

  if (s_curveChan >= MAX_CURVE5) {
    points = 9;
    crv = g_model.curves9[s_curveChan-MAX_CURVE5];
  }
  else {
    points = 5;
    crv = g_model.curves5[s_curveChan];
  }

  switch(event) {
    case EVT_ENTRY:
      dfltCrv = 0;
#if defined(ROTARY_ENCODERS)
      s_editMode = -1;
#endif
      break;
#if defined(ROTARY_ENCODERS)
    case EVT_KEY_BREAK(BTN_REa):
    case EVT_KEY_BREAK(BTN_REb):
      if (!navigationRotaryEncoder(event))
        break;
#endif
    case EVT_KEY_FIRST(KEY_MENU):
      if (s_editMode<=0) {
        switch (m_posHorz) {
          case 0:
            s_editMode = 1;
            break;
          case 1:
            if (++dfltCrv > 4)
              dfltCrv = -4;
            for (uint8_t i=0; i<points; i++)
              crv[i] = (i-(points/2)) * dfltCrv * 50 / (points-1);
            break;
        }
      }
      break;
    case EVT_KEY_FIRST(KEY_EXIT):
      killEvents(event);
      if (s_editMode>0) {
        m_posHorz = 0;
        s_editMode = 0;
      }
      else {
        popMenu();
      }
      break;
    case EVT_KEY_REPT(KEY_LEFT):
    case EVT_KEY_FIRST(KEY_LEFT):
      if (m_posHorz>0) m_posHorz--;
      break;
    case EVT_KEY_REPT(KEY_RIGHT):
    case EVT_KEY_FIRST(KEY_RIGHT):
      if (m_posHorz<((s_editMode>0) ? points-1 : 1)) m_posHorz++;
      break;
  }

  for (uint8_t i = 0; i < points; i++) {
    uint8_t x, y;
    if (i>4) {
      x = 8*FW; y = (i-4) * FH;
    }
    else {
      x = 4*FW; y = (i+1) * FH;
    }
    uint8_t attr = (s_editMode>0 && m_posHorz==i) ? INVERS : 0;
    lcd_outdezAtt(x, y, crv[i], attr);
  }

  lcd_putsLeft(7*FH, STR_MODE);
  lcd_putsiAtt(5*FW-2, 7*FH, STR_CURVMODES, (s_editMode<=0)*m_posHorz, s_editMode>0 ? 0 : INVERS);

  if (s_editMode>0) {
    for (uint8_t i=0; i<points; i++) {
      uint8_t xx = X0-1-WCHART+i*WCHART/(points/2);
      uint8_t yy = (DISPLAY_H-1) - (100 + crv[i]) * (DISPLAY_H-1) / 200;

      if (m_posHorz==i) {
        lcd_filled_rect(xx-1, yy-2, 5, 5); // do selection square
        if (p1valdiff || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP))
          CHECK_INCDEC_MODELVAR( event, crv[i], -100,100);  // edit on up/down
      }
      else {
        lcd_filled_rect(xx, yy-1, 3, 3); // do markup square
      }
    }
  }

  DrawCurve(curveFn);
}

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

#if defined(PCBV4)
inline void pauseMixerCalculations()
{
  cli();
  TIMSK5 &= ~(1<<OCIE5A);
  sei();
}

inline void resumeMixerCalculations()
{
  cli();
  TIMSK5 |= (1<<OCIE5A);
  sei();
}
#else
#define pauseMixerCalculations()
#define resumeMixerCalculations()
#endif

void deleteExpoMix(uint8_t expo, uint8_t idx)
{
  pauseMixerCalculations();
  if (expo) {
    ExpoData *expo = expoaddress(idx);
    memmove(expo, expo+1, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
    memset(&g_model.expoData[MAX_EXPOS-1], 0, sizeof(ExpoData));
  }
  else {
    MixData *mix = mixaddress(idx);
    memmove(mix, mix+1, (MAX_MIXERS-(idx+1))*sizeof(MixData));
    memset(&g_model.mixData[MAX_MIXERS-1], 0, sizeof(MixData));
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
    memset(expo, 0, sizeof(ExpoData));
    expo->mode = 3; // pos&neg
    expo->chn = s_currCh - 1;
    expo->weight = 100;
  }
  else {
    MixData *mix = mixaddress(idx);
    memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
    memset(mix, 0, sizeof(MixData));
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

  pauseMixerCalculations();
  while (size--) {
    temp = *x;
    *x++ = *y;
    *y++ = temp;
  }
  resumeMixerCalculations();
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
    if(((MixData *)x)->destCh != ((MixData *)y)->destCh) {
      if (up) {
        if (((MixData *)x)->destCh>0) ((MixData *)x)->destCh--;
        else return false;
      }
      else {
        if (((MixData *)x)->destCh<NUM_CHNOUT-1) ((MixData *)x)->destCh++;
        else return false;
      }
      return true;
    }

    size = sizeof(MixData);
  }

  memswap(x, y, size);
  idx = tgt_idx;
  return true;
}

enum ExposFields {
#ifdef PCBARM
  EXPO_FIELD_NAME,
#endif
  EXPO_FIELD_WIDTH,
  EXPO_FIELD_EXPO,
  EXPO_FIELD_CURVE,
#ifdef FLIGHT_PHASES
  EXPO_FIELD_FLIGHT_PHASE,
#endif
  EXPO_FIELD_SWITCH,
  EXPO_FIELD_WHEN,
  EXPO_FIELD_MAX
};

void menuProcExpoOne(uint8_t event)
{
  ExpoData *ed = expoaddress(s_currIdx);
  putsChnRaw(7*FW+FW/2,0,ed->chn+1,0);

  SIMPLE_SUBMENU(STR_MENUDREXPO, EXPO_FIELD_MAX);

  int8_t sub = m_posVert;

  uint8_t y = FH;

  for (uint8_t i=0; i<EXPO_FIELD_MAX+1; i++) {
    lcd_putsiAtt(0, y, STR_EXPLABELS, i, 0);
    uint8_t attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i)
    {
#if defined(PCBARM)
      case EXPO_FIELD_NAME:
        EditName(6*FW+5, y, ed->name, sizeof(ed->name), event, attr, m_posHorz);
        break;
#endif

      case EXPO_FIELD_WIDTH:
        {
#if defined(PCBARM)
          lcd_outdezAtt(9*FW+5, y, ed->weight, attr|INFLIGHT(ed->weight));
          if (attr) CHECK_INFLIGHT_INCDEC_MODELVAR(event, ed->weight, 0, 100, 0, STR_DRWEIGHT);
#else
          PREPARE_INFLIGHT_BITFIELD(&ed->expo - 1);
          lcd_outdezAtt(9*FW+5, y, ed->weight, attr|INFLIGHT(*bitfield));
          if (attr) CHECK_INFLIGHT_INCDEC_MODELVAR_BITFIELD(event, ed->weight, 0, 100, 0, STR_DRWEIGHT, 1);
#endif
        }
        break;
      case EXPO_FIELD_EXPO:
        lcd_outdezAtt(9*FW+5, y, ed->expo, attr|INFLIGHT(ed->expo));
        if (attr) CHECK_INFLIGHT_INCDEC_MODELVAR(event, ed->expo, -100, 100, 0, STR_DREXPO);
        break;
      case EXPO_FIELD_CURVE:
        putsCurve(6*FW+5, y, ed->curve+(ed->curve >= CURVE_BASE+4 ? 4 : 0), attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, ed->curve, 0, 15);
        if (attr && ed->curve>=CURVE_BASE && event==EVT_KEY_FIRST(KEY_MENU)) {
          s_curveChan = ed->curve - (ed->curve >= CURVE_BASE+4 ? CURVE_BASE-4 : CURVE_BASE);
          pushMenu(menuProcCurveOne);
        }
        break;
#ifdef FLIGHT_PHASES
      case EXPO_FIELD_FLIGHT_PHASE:
        {
#if defined(PCBARM)
          putsFlightPhase(6*FW+5, y, ed->phase, attr);
          if (attr) { ed->phase = checkIncDecModel(event, ed->phase, -MAX_PHASES, MAX_PHASES); }
#else
          int8_t phase = ed->negPhase ? -ed->phase : +ed->phase;
          putsFlightPhase(6*FW+5, y, phase, attr);
          if (attr) { phase = checkIncDecModel(event, phase, -MAX_PHASES, MAX_PHASES); ed->negPhase = (phase < 0); ed->phase = abs(phase); }
#endif
        }
        break;
#endif
      case EXPO_FIELD_SWITCH:
        putsSwitches(6*FW+5, y, ed->swtch, attr);
        if (attr) CHECK_INCDEC_MODELSWITCH(event, ed->swtch, -MAX_DRSWITCH, MAX_DRSWITCH);
        break;
      case EXPO_FIELD_WHEN:
        lcd_putsiAtt(6*FW+5, y, STR_VWHEN, 3-ed->mode, attr);
        if (attr) ed->mode = 4 - checkIncDecModel(event, 4-ed->mode, 1, 3);
        break;
    }
    y+=FH;
  }

  DrawCurve(expoFn);

  int16_t x512 = calibratedStick[ed->chn];
  int16_t y512 = expoFn(x512);

  lcd_outdezAtt(20*FW, 6*FH, x512*25/256, 0);
  lcd_outdezAtt(14*FW, 1*FH, y512*25/256, 0);

  x512 = X0+x512/(RESXu/WCHART);
  y512 = (DISPLAY_H-1) - (uint16_t)((y512+RESX)/2) * (DISPLAY_H-1) / RESX;

  lcd_vline(x512, y512-3,3*2+1);
  lcd_hline(x512-3, y512,3*2+1);
}

enum MixFields {
#if defined(PCBARM)
  MIX_FIELD_NAME,
#endif
  MIX_FIELD_SOURCE,
  MIX_FIELD_WEIGHT,
  MIX_FIELD_DIFFERENTIAL,
  MIX_FIELD_OFFSET,
  MIX_FIELD_TRIM,
  MIX_FIELD_CURVE,
  MIX_FIELD_SWITCH,
#ifdef FLIGHT_PHASES
  MIX_FIELD_FLIGHT_PHASE,
#endif
  MIX_FIELD_WARNING,
  MIX_FIELD_MLTPX,
  MIX_FIELD_DELAY_UP,
  MIX_FIELD_DELAY_DOWN,
  MIX_FIELD_SLOW_UP,
  MIX_FIELD_SLOW_DOWN,
  MIX_FIELD_COUNT
};

#if defined(TRANSLATIONS_FR)
#define MIXES_2ND_COLUMN (13*FW)
#elif defined(TRANSLATIONS_CZ)
#define MIXES_2ND_COLUMN (13*FW)
#else
#define MIXES_2ND_COLUMN (9*FW)
#endif
void menuProcMixOne(uint8_t event)
{
  TITLEP(s_currCh ? STR_INSERTMIX : STR_EDITMIX);
  MixData *md2 = mixaddress(s_currIdx) ;
  putsChn(lcdLastPos+1*FW,0,md2->destCh+1,0);
  SIMPLE_SUBMENU_NOTITLE(MIX_FIELD_COUNT);

  int8_t  sub = m_posVert;

  for (uint8_t k=0; k<7; k++) {
    uint8_t y = (k+1) * FH;
    uint8_t i = k + s_pgOfs;
    uint8_t attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i) {
#if defined(PCBARM)
      case MIX_FIELD_NAME:
        lcd_putsLeft(y, STR_NAME);
        EditName(MIXES_2ND_COLUMN, y, md2->name, sizeof(md2->name), event, attr, m_posHorz);
        break;
#endif
      case MIX_FIELD_SOURCE:
        lcd_putsLeft(y, STR_SOURCE);
        putsMixerSource(MIXES_2ND_COLUMN, y, md2->srcRaw, attr);
        if(attr) CHECK_INCDEC_MODELVAR(event, md2->srcRaw, 1, NUM_XCHNMIX);
        break;
      case MIX_FIELD_WEIGHT:
        lcd_putsLeft(y, STR_WEIGHT);
        lcd_outdezAtt(MIXES_2ND_COLUMN, y, md2->weight, attr|LEFT|INFLIGHT(md2->weight));
        if (attr) CHECK_INFLIGHT_INCDEC_MODELVAR(event, md2->weight, -125, 125, 0, STR_MIXERWEIGHT);
        break;
      case MIX_FIELD_DIFFERENTIAL:
        // TODO INFLIGHT
        lcd_putsLeft(y, STR_DIFFERENTIAL);
        lcd_outdezAtt(MIXES_2ND_COLUMN, y, md2->differential*2, attr|LEFT);
        if (attr) CHECK_INCDEC_MODELVAR(event, md2->differential, -50, 50);
        break;
      case MIX_FIELD_OFFSET:
        lcd_putsLeft(y, STR_OFFSET);
        lcd_outdezAtt(MIXES_2ND_COLUMN, y, md2->sOffset, attr|LEFT|INFLIGHT(md2->sOffset));
        if (attr) CHECK_INFLIGHT_INCDEC_MODELVAR(event, md2->sOffset, -125, 125, 0, STR_MIXEROFFSET);
        break;
      case MIX_FIELD_TRIM:
      {
        uint8_t not_stick = (md2->srcRaw > NUM_STICKS);
        int8_t carryTrim = -md2->carryTrim;
        lcd_putsLeft(y, STR_TRIM);
        lcd_putsiAtt(MIXES_2ND_COLUMN, y, STR_VMIXTRIMS, (not_stick && carryTrim == 0) ? 0 : carryTrim+1, attr);
        if (attr) md2->carryTrim = -checkIncDecModel(event, carryTrim, not_stick ? TRIM_ON : -TRIM_OFF, -TRIM_AIL);
        break;
      }
      case MIX_FIELD_CURVE:
        lcd_putsLeft(y, STR_CURVES);
        putsCurve(MIXES_2ND_COLUMN, y, md2->curve, attr);
        if(attr) CHECK_INCDEC_MODELVAR( event, md2->curve, -MAX_CURVE5-MAX_CURVE9, MAX_CURVE5+MAX_CURVE9+7-1);
        if(attr && event==EVT_KEY_FIRST(KEY_MENU) && (md2->curve<0 || md2->curve>=CURVE_BASE)){
          s_curveChan = (md2->curve<0 ? -md2->curve-1 : md2->curve-CURVE_BASE);
          pushMenu(menuProcCurveOne);
        }
        break;
      case MIX_FIELD_SWITCH:
        lcd_putsLeft(y, STR_SWITCH);
        putsSwitches(MIXES_2ND_COLUMN,  y,md2->swtch,attr);
        if(attr) CHECK_INCDEC_MODELSWITCH( event, md2->swtch, -MAX_SWITCH, MAX_SWITCH);
        break;
#ifdef FLIGHT_PHASES
      case MIX_FIELD_FLIGHT_PHASE:
        lcd_putsLeft(y, STR_FPHASE);
        putsFlightPhase(MIXES_2ND_COLUMN, y, md2->phase, attr);
        if(attr) CHECK_INCDEC_MODELVAR( event, md2->phase, -MAX_PHASES, MAX_PHASES);
        break;
#endif
      case MIX_FIELD_WARNING:
        lcd_putsLeft(y, STR_MIXWARNING);
        if(md2->mixWarn)
          lcd_outdezAtt(MIXES_2ND_COLUMN,y,md2->mixWarn,attr|LEFT);
        else
          lcd_putsAtt(MIXES_2ND_COLUMN, y, STR_OFF, attr);
        if(attr) CHECK_INCDEC_MODELVAR( event, md2->mixWarn, 0,3);
        break;
      case MIX_FIELD_MLTPX:
        lcd_putsLeft(y, STR_MULTPX);
        lcd_putsiAtt(MIXES_2ND_COLUMN, y, STR_VMLTPX, md2->mltpx, attr);
        if(attr) CHECK_INCDEC_MODELVAR( event, md2->mltpx, 0, 2);
        break;
      case MIX_FIELD_DELAY_UP:
        lcd_putsLeft(y, STR_DELAYUP);
        lcd_outdezAtt(MIXES_2ND_COLUMN,y,5*md2->delayUp,attr|PREC1|LEFT);
        if(attr)  CHECK_INCDEC_MODELVAR( event, md2->delayUp, 0, MAX_DELAY);
        break;
      case MIX_FIELD_DELAY_DOWN:
        lcd_putsLeft(y, STR_DELAYDOWN);
        lcd_outdezAtt(MIXES_2ND_COLUMN,y,5*md2->delayDown,attr|PREC1|LEFT);
        if(attr)  CHECK_INCDEC_MODELVAR( event, md2->delayDown, 0, MAX_DELAY);
        break;
      case MIX_FIELD_SLOW_UP:
        lcd_putsLeft(y, STR_SLOWUP);
        lcd_outdezAtt(MIXES_2ND_COLUMN,y,5*md2->speedUp,attr|PREC1|LEFT);
        if(attr)  CHECK_INCDEC_MODELVAR( event, md2->speedUp, 0, MAX_SLOW);
        break;
      case MIX_FIELD_SLOW_DOWN:
        lcd_putsLeft(y, STR_SLOWDOWN);
        lcd_outdezAtt(MIXES_2ND_COLUMN,y,5*md2->speedDown,attr|PREC1|LEFT);
        if(attr)  CHECK_INCDEC_MODELVAR( event, md2->speedDown, 0, MAX_SLOW);
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

void menuProcExpoMix(uint8_t expo, uint8_t _event_)
{
  uint8_t _event = (s_warning ? 0 : _event_);
  uint8_t event = _event;
  uint8_t key = (event & 0x1f);

  if (s_copyMode) {
    if (key == KEY_EXIT)
      event -= KEY_EXIT;
  }

  TITLEP(expo ? STR_MENUDREXPO : STR_MIXER);
  lcd_outdezAtt(lcdLastPos+2*FW+FW/2, 0, getExpoMixCount(expo));
  lcd_puts(lcdLastPos, 0, expo ? STR_MAX(MAX_EXPOS) : STR_MAX(MAX_MIXERS));
  SIMPLE_MENU_NOTITLE(menuTabModel, expo ? e_ExposAll : e_MixAll, s_maxLines);

#if defined(ROTARY_ENCODERS)
  int8_t sub = m_posVert;
#else
  uint8_t sub = m_posVert;
#endif

  if (s_editMode > 0)
    s_editMode = 0;

  switch(_event)
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
      killEvents(_event);
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
    case EVT_KEY_BREAK(KEY_MENU):
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
    case EVT_KEY_LONG(BTN_REa):
    case EVT_KEY_LONG(BTN_REb):
      if (sub == 0)
        break;
#endif
    case EVT_KEY_LONG(KEY_MENU):
      killEvents(_event);
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
        pushMenu(expo ? menuProcExpoOne : menuProcMixOne);
        s_copyMode = 0;
        return;
      }
      break;
    case EVT_KEY_LONG(KEY_LEFT):
    case EVT_KEY_LONG(KEY_RIGHT):
      if (s_copyMode && !s_copyTgtOfs) {
        if (reachExpoMixCountLimit(expo)) break;
        s_currCh = (expo ? expoaddress(s_currIdx)->chn+1 : mixaddress(s_currIdx)->destCh+1);
        if (_event == EVT_KEY_LONG(KEY_RIGHT)) s_currIdx++;
        insertExpoMix(expo, s_currIdx);
        pushMenu(expo ? menuProcExpoOne : menuProcMixOne);
        s_copyMode = 0;
        killEvents(_event);
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
            uint8_t y = (cur-s_pgOfs)*FH;
            lcd_rect(expo ? 18 : 22, y-1, expo ? DISPLAY_W-18 : DISPLAY_W-22, 9, DOTTED);
            cur++;
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
          uint8_t y = (cur-s_pgOfs)*FH;
          uint8_t attr = ((s_copyMode || sub != cur) ? 0 : INVERS);         
          if (expo) {
            lcd_outdezAtt(EXPO_LINE_WEIGHT_POS, y, ed->weight, attr | (isExpoActive(i) ? BOLD : 0));
            if (attr != 0)
              CHECK_INCDEC_MODELVAR(_event, ed->weight, 0, 100);
            lcd_outdezAtt(EXPO_LINE_EXPO_POS, y, ed->expo, 0);

#if defined(PCBARM)
            if (ed->name[0]) {
              putsSwitches(11*FW, y, ed->swtch, 0);
              lcd_putsnAtt(15*FW+2, y, ed->name, sizeof(ed->name), ZCHAR | (isExpoActive(i) ? BOLD : 0));
            }
            else
#endif
            {
#if defined(FLIGHT_PHASES)
#if defined(PCBARM)
              putsFlightPhase(EXPO_LINE_PHASE_POS, y, ed->phase);
#else
              putsFlightPhase(EXPO_LINE_PHASE_POS, y, ed->negPhase ? -ed->phase : +ed->phase);
#endif
#endif
              putsSwitches(EXPO_LINE_SWITCH_POS, y, ed->swtch, 0); // normal switches
              if (ed->mode!=3) lcd_putc(17*FW, y, ed->mode == 2 ? 126 : 127);//'|' : (stkVal[i] ? '<' : '>'),0);*/
              if (ed->curve) putsCurve(18*FW+2, y, ed->curve+(ed->curve >= CURVE_BASE+4 ? 4 : 0));
            }
          }
          else {
            if (mixCnt > 0)
              lcd_putsiAtt(1*FW+0, y, STR_VMLTPX2, md->mltpx, 0);

            putsMixerSource(4*FW+0, y, md->srcRaw, isMixActive(i) ? BOLD : 0);

            lcd_outdezAtt(11*FW+3, y, md->weight, attr);
            if (attr != 0)
              CHECK_INCDEC_MODELVAR(_event, md->weight, -125, 125);

#if defined(PCBARM)
            if (md->name[0]) {
              lcd_putsnAtt(15*FW+2, y, md->name, sizeof(md->name), ZCHAR | (isMixActive(i) ? BOLD : 0));
            }
            else
#endif
            {
              if (md->curve) putsCurve(12*FW+2, y, md->curve);
              if (md->swtch) putsSwitches(15*FW+5, y, md->swtch);

              char cs = ' ';
              if (md->speedDown || md->speedUp)
                cs = 'S';
              if ((md->delayUp || md->delayDown))
                cs = (cs =='S' ? '*' : 'D');
              lcd_putcAtt(18*FW+7, y, cs, 0);

#ifdef FLIGHT_PHASES
              if (md->phase)
                putsFlightPhase(20*FW+2, y, md->phase, CONDENSED);
#endif
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
        cur++; mixCnt++; i++; if (expo) ed++; else md++;
      } while (expo ? (i<MAX_EXPOS && ed->chn+1 == ch && ed->mode) : (i<MAX_MIXERS && md->srcRaw && md->destCh+1 == ch));
      if (s_copyMode == MOVE_MODE && s_pgOfs < cur && cur-s_pgOfs < 8 && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
        uint8_t y = (cur-s_pgOfs)*FH;
        lcd_rect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? DISPLAY_W-EXPO_LINE_SELECT_POS : DISPLAY_W-22, 9, DOTTED);
        cur++;
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
          putsChnRaw(0, (cur-s_pgOfs)*FH, ch, attr);
        else
          putsChn(0, (cur-s_pgOfs)*FH, ch, attr); // show CHx
        if (s_copyMode == MOVE_MODE && s_copySrcCh == ch) {
          uint8_t y = (cur-s_pgOfs)*FH;
          lcd_rect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? (DISPLAY_W-EXPO_LINE_SELECT_POS) : (DISPLAY_W-22), 9, DOTTED);
        }
      }
      cur++;
    }
  }
  s_maxLines = cur;
  if (sub >= s_maxLines-1) m_posVert = s_maxLines-1;
  displayWarning(_event_);
}

void menuProcExposAll(uint8_t event)
{
  return menuProcExpoMix(1, event);
}

void menuProcMixAll(uint8_t event)
{
  return menuProcExpoMix(0, event);
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

void menuProcLimits(uint8_t _event)
{
#ifdef PPM_CENTER_ADJUSTABLE
#define LIMITS_ITEMS_COUNT 4
#else
#define LIMITS_ITEMS_COUNT 3
#endif

  uint8_t event = (s_warning ? 0 : _event);

  MENU(STR_MENULIMITS, menuTabModel, e_Limits, 1+NUM_CHNOUT+1, {0, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, LIMITS_ITEMS_COUNT, 0});

  uint8_t sub = m_posVert - 1;

#ifdef PPM_CENTER_ADJUSTABLE
  if (sub < NUM_CHNOUT) {
    lcd_outdezAtt(12*FW, 0, PPM_CENTER+g_model.servoCenter[sub]+g_chans512[sub]/2, 0);
    lcd_puts(12*FW, 0, STR_US);
  }
#endif

  if (s_confirmation) {
    LimitData *ld = limitaddress(sub);
    ld->revert = !ld->revert;
    s_confirmation = 0;
    AUDIO_WARNING2();
  }

  for (uint8_t i=0; i<7; i++) {
    uint8_t y = (i+1)*FH;
    uint8_t k = i+s_pgOfs;

    if (k==NUM_CHNOUT) {
      // last line available - add the "copy trim menu" line
      uint8_t attr = (sub==NUM_CHNOUT) ? INVERS : 0;
      lcd_putsAtt(3*FW, y, STR_TRIMS2OFFSETS, s_noHi ? 0 : attr);
      if (attr) {
        s_editMode = 0;
        if (event==EVT_KEY_LONG(KEY_MENU)) {
          s_noHi = NO_HI_LEN;
          killEvents(event);
          pauseMixerCalculations();
          moveTrimsToOffsets(); // if highlighted and menu pressed - move trims to offsets
          resumeMixerCalculations();
        }
      }
      return;
    }

    LimitData *ld = limitaddress(k) ;

#ifdef PPM_CENTER_ADJUSTABLE
#define LIMITS_MAX_POS 16*FW
    int8_t limit = ((g_model.extendedLimits && !g_model.servoCenter[k]) ? 125 : 100);
#else
#define LIMITS_MAX_POS 17*FW
    int16_t v = (ld->revert) ? -ld->offset : ld->offset;

    char swVal = '-';  // '-', '<', '>'
    if((g_chans512[k] - v) > 50) swVal = (ld->revert ? 127 : 126); // Switch to raw inputs?  - remove trim!
    if((g_chans512[k] - v) < -50) swVal = (ld->revert ? 126 : 127);
    putsChn(0, y, k+1, 0);
    lcd_putcAtt(12*FW+5, y, swVal, 0);

    int8_t limit = (g_model.extendedLimits ? 125 : 100);
#endif

    putsChn(0, y, k+1, 0);

    for (uint8_t j=0; j<=LIMITS_ITEMS_COUNT; j++) {
      uint8_t attr = ((sub==k && m_posHorz==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      uint8_t active = (attr && (s_editMode>0 || p1valdiff)) ;
      switch(j)
      {
        case 0:
#ifdef PPM_LIMITS_UNIT_US
          lcd_outdezAtt(  8*FW, y,  ((int32_t)ld->offset*128) / 25, attr|PREC1);
#else
          lcd_outdezAtt(  8*FW, y,  ld->offset, attr|PREC1);
#endif
          if (active) {
            int16_t new_offset = checkIncDec(event, ld->offset, -1000, 1000, EE_MODEL|NO_INCDEC_MARKS);
#if defined(PCBV4)
            if (checkIncDec_Ret)
#endif
            {
              pauseMixerCalculations();
              ld->offset = new_offset;
              resumeMixerCalculations();
            }
          }
          else if (attr && event==EVT_KEY_LONG(KEY_MENU)) {
            int16_t zero = g_chans512[k];
            pauseMixerCalculations();
            ld->offset = (ld->revert) ? -zero : zero;
            resumeMixerCalculations();
            s_editMode = 0;
            STORE_MODELVARS;
          }
          break;
        case 1:
#ifdef PPM_LIMITS_UNIT_US
          lcd_outdezAtt(12*FW+1, y, (((int16_t)ld->min-100)*128) / 25, attr | INFLIGHT(ld->min));
#else
          lcd_outdezAtt(12*FW, y, (int8_t)(ld->min-100), attr | INFLIGHT(ld->min));
#endif
          if (active) {
            CHECK_INFLIGHT_INCDEC_MODELVAR(event, ld->min, -limit, 25, +100, STR_MINLIMIT);
          }
          break;
        case 2:
#ifdef PPM_LIMITS_UNIT_US
          lcd_outdezAtt(LIMITS_MAX_POS, y, (((int16_t)ld->max+100)*128) / 25, attr | INFLIGHT(ld->max));
#else
          lcd_outdezAtt(LIMITS_MAX_POS, y, (int8_t)(ld->max+100), attr | INFLIGHT(ld->max));
#endif
          if (active) {
            CHECK_INFLIGHT_INCDEC_MODELVAR(event, ld->max, -25, limit, -100, STR_MAXLIMIT);
          }
          break;
        case 3:
#ifdef PPM_CENTER_ADJUSTABLE
          lcd_putcAtt(17*FW-2, y, ld->revert ? 127 : 126, attr);
#else
          lcd_putsiAtt(18*FW, y, STR_MMMINV, ld->revert, attr);
#endif
          if (active) {
            bool revert_new = checkIncDecModel(event, ld->revert, 0, 1);
            if (checkIncDec_Ret && thrOutput(k)) {
              s_warning = STR_INVERT_THR;
              killEvents(event);
              _event = 0;
            }
            else
              ld->revert = revert_new;
          }
          break;
#ifdef PPM_CENTER_ADJUSTABLE
        case 4:
          lcd_outdezAtt(21*FW+2, y, PPM_CENTER+g_model.servoCenter[k], attr);
          if (active && ld->max <= 0 && ld->min >= 0) {
            CHECK_INCDEC_MODELVAR(event, g_model.servoCenter[k], -125, +125);
          }
          break;
#endif
      }
    }
  }

  if (s_warning) {
    displayConfirmation(_event);
  }
}

void menuProcCurvesAll(uint8_t event)
{
  SIMPLE_MENU(STR_MENUCURVES, menuTabModel, e_CurvesAll, 1+MAX_CURVE5+MAX_CURVE9);

  int8_t  sub    = m_posVert - 1;

  switch (event) {
#if defined(ROTARY_ENCODERS)
    case EVT_KEY_BREAK(BTN_REa):
    case EVT_KEY_BREAK(BTN_REb):
      if (!navigationRotaryEncoder(event))
        break;
      // no break
#endif
    case EVT_KEY_FIRST(KEY_RIGHT):
    case EVT_KEY_FIRST(KEY_MENU):
      if (sub >= 0) {
        s_curveChan = sub;
        pushMenu(menuProcCurveOne);
      }
      break;
  }

  uint8_t y    = 1*FH;
  uint8_t yd   = 1;
  uint8_t m    = 0;
  for (uint8_t i = 0; i < 7; i++) {
    uint8_t k = i + s_pgOfs;
    uint8_t attr = sub == k ? INVERS : 0;
    bool    cv9 = k >= MAX_CURVE5;

    if(cv9 && (yd>6)) break;
    if(yd>7) break;
    if(!m) m = attr;
    putsStrIdx(0, y, STR_CV, k+1, attr);
    int8_t *crv = cv9 ? g_model.curves9[k-MAX_CURVE5] : g_model.curves5[k];
    for (uint8_t j = 0; j < (5); j++) {
      lcd_outdezAtt( j*(3*FW+3) + 7*FW + 2, y, crv[j], 0);
    }
    y += FH;yd++;
    if(cv9){
      for (uint8_t j = 0; j < 4; j++) {
        lcd_outdezAtt( j*(3*FW+3) + 7*FW + 2, y, crv[j+5], 0);
      }
      y += FH;yd++;
    }
  }

  if(!m) s_pgOfs++;
}

#if defined(PCBARM)
enum CustomSwitchFields {
  CSW_FIELD_FUNCTION,
  CSW_FIELD_V1,
  CSW_FIELD_V2,
  CSW_FIELD_DURATION,
  CSW_FIELD_DELAY,
  CSW_FIELD_COUNT
};
#define CSW_2ND_COLUMN (9*FW)
void menuProcCustomSwitchOne(uint8_t event)
{
  TITLEP("CUSTOM SWITCH");
  CustomSwData &cs = g_model.customSw[s_currIdx];
  uint8_t sw = DSW_SW1+s_currIdx;
  putsSwitches(14*FW, 0, sw, (getSwitch(sw, 0) ? BOLD : 0));
  SIMPLE_SUBMENU_NOTITLE(CSW_FIELD_COUNT);

  int8_t  sub = m_posVert;

  for (uint8_t k=0; k<7; k++) {
    uint8_t y = (k+2) * FH;
    uint8_t i = k + s_pgOfs;
    uint8_t attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    uint8_t cstate = CS_STATE(cs.func);
    switch(i) {
      case CSW_FIELD_FUNCTION:
        lcd_putsLeft(y, "Func");
        lcd_putsiAtt(CSW_2ND_COLUMN, y, STR_VCSWFUNC, cs.func, attr);
        if (attr) {
          CHECK_INCDEC_MODELVAR(event, cs.func, 0, CS_MAXF);
          if (cstate != CS_STATE(cs.func)) {
            cs.v1 = 0;
            cs.v2 = 0;
          }
        }
        break;
      case CSW_FIELD_V1:
      {
        lcd_putsLeft(y, "V1");
        int8_t v1_min=0, v1_max=NUM_XCHNCSW;
        if (cstate == CS_VBOOL) {
          putsSwitches(CSW_2ND_COLUMN, y, cs.v1, attr);
          v1_min = SWITCH_OFF; v1_max = SWITCH_ON;
        }
        else {
          putsChnRaw(CSW_2ND_COLUMN, y, cs.v1, attr);
        }
        if (attr) {
          CHECK_INCDEC_MODELVAR(event, cs.v1, v1_min, v1_max);
        }
        break;
      }
      case CSW_FIELD_V2:
      {
        lcd_putsLeft(y, "V2");
        int8_t v2_min=0, v2_max=NUM_XCHNCSW;
        if (cstate == CS_VBOOL) {
          putsSwitches(CSW_2ND_COLUMN, y, cs.v2, attr);
          v2_min = SWITCH_OFF; v2_max = SWITCH_ON;
        }
        else if (cstate == CS_VOFS) {
#if defined(FRSKY)
          if (cs.v1 > NUM_XCHNCSW-NUM_TELEMETRY) {
            putsTelemetryChannel(6*FW, y, cs.v1 - (NUM_XCHNCSW-NUM_TELEMETRY+1), convertTelemValue(cs.v1 - (NUM_XCHNCSW-NUM_TELEMETRY), 128+cs.v2), attr|LEFT);
            v2_min = -128; v2_max = maxTelemValue(cs.v1 - (NUM_XCHNCSW-NUM_TELEMETRY)) - 128;
            if (cs.v2 > v2_max) {
              cs.v2 = v2_max;
              eeDirty(EE_MODEL);
            }
          }
          else
#endif
          {
            v2_min = -125; v2_max = 125;
            lcd_outdezAtt(CSW_2ND_COLUMN, y, CSW_2ND_COLUMN, attr|LEFT);
          }
        }
        else {
          putsChnRaw(CSW_2ND_COLUMN, y, cs.v2, attr);
        }
        if (attr) {
          CHECK_INCDEC_MODELVAR(event, cs.v2, v2_min, v2_max);
        }
        break;
      }
      case CSW_FIELD_DURATION:
        lcd_putsLeft(y, "Duration");
        lcd_outdezAtt(CSW_2ND_COLUMN, y, 5*cs.duration, attr|PREC1|LEFT);
        if (attr) CHECK_INCDEC_MODELVAR(event, cs.duration, 0, MAX_CSW_DURATION);
        break;
      case CSW_FIELD_DELAY:
        lcd_putsLeft(y, "Delay");
        lcd_outdezAtt(CSW_2ND_COLUMN, y, 5*cs.delay, attr|PREC1|LEFT);
        if (attr) CHECK_INCDEC_MODELVAR(event, cs.delay, 0, MAX_CSW_DELAY);
        break;
    }
  }
}

void menuProcCustomSwitches(uint8_t event)
{
  MENU(STR_MENUCUSTOMSWITCHES, menuTabModel, e_CustomSwitches, NUM_CSW+1, {0, 2/*repeated...*/});

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
    case EVT_KEY_FIRST(KEY_RIGHT):
    case EVT_KEY_FIRST(KEY_MENU):
      if (sub >= 0) {
        s_currIdx = sub;
        pushMenu(menuProcCustomSwitchOne);
      }
      break;
  }

  for (uint8_t i=0; i<7; i++) {
    y = (i+1)*FH;
    k = i+s_pgOfs;
    CustomSwData &cs = g_model.customSw[k];

    // CSW name
    uint8_t sw = DSW_SW1+k;
    putsSwitches(0, y, sw, (sub==k ? INVERS : 0) | (getSwitch(sw, 0) ? BOLD : 0));

    if (cs.func > 0) {
      // CSW func
      lcd_putsiAtt(4*FW - 2, y, STR_VCSWFUNC, cs.func, 0);

      // CSW params
      uint8_t cstate = CS_STATE(cs.func);

      if (cstate == CS_VOFS)
      {
          putsChnRaw(12*FW-6, y, cs.v1, 0);

#if defined(FRSKY)
          if (cs.v1 > NUM_XCHNCSW-NUM_TELEMETRY) {
            putsTelemetryChannel(19*FW, y, cs.v1 - (NUM_XCHNCSW-NUM_TELEMETRY+1), convertTelemValue(cs.v1 - (NUM_XCHNCSW-NUM_TELEMETRY), 128+cs.v2), 0);
            int8_t v2_max = maxTelemValue(cs.v1 - (NUM_XCHNCSW-NUM_TELEMETRY)) - 128;
            if (cs.v2 > v2_max) {
              cs.v2 = v2_max;
              eeDirty(EE_MODEL);
            }
          }
          else
#endif
          {
            lcd_outdezAtt(19*FW, y, cs.v2, 0);
          }
      }
      else if (cstate == CS_VBOOL)
      {
          putsSwitches(12*FW-6, y, cs.v1, 0);
          putsSwitches(17*FW, y, cs.v2, 0);
      }
      else // cstate == CS_COMP
      {
          putsChnRaw(12*FW-6, y, cs.v1, 0);
          putsChnRaw(17*FW, y, cs.v2, 0);
      }
    }
  }
}
#else
void menuProcCustomSwitches(uint8_t event)
{
  MENU(STR_MENUCUSTOMSWITCHES, menuTabModel, e_CustomSwitches, NUM_CSW+1, {0, 2/*repeated...*/});

  uint8_t y = 0;
  uint8_t k = 0;
  int8_t sub = m_posVert - 1;

  for (uint8_t i=0; i<7; i++) {
    y = (i+1)*FH;
    k = i+s_pgOfs;
    uint8_t attr = (sub==k ? ((s_editMode>0) ? BLINK|INVERS : INVERS)  : 0);
    CustomSwData &cs = g_model.customSw[k];

    // CSW name
    uint8_t sw = DSW_SW1+k;
    putsSwitches(0, y, sw, getSwitch(sw, 0) ? BOLD : 0);

    // CSW func
    lcd_putsiAtt(4*FW - 2, y, STR_VCSWFUNC, cs.func, m_posHorz==0 ? attr : 0);

    // CSW params
    uint8_t cstate = CS_STATE(cs.func);
    int8_t v1_min=0, v1_max=NUM_XCHNCSW, v2_min=0, v2_max=NUM_XCHNCSW;

    if (cstate == CS_VOFS)
    {
        putsChnRaw(12*FW-6, y, cs.v1, (m_posHorz==1 ? attr : 0));

#if defined(FRSKY)
        if (cs.v1 > NUM_XCHNCSW-NUM_TELEMETRY) {
          putsTelemetryChannel(19*FW, y, cs.v1 - (NUM_XCHNCSW-NUM_TELEMETRY+1), convertTelemValue(cs.v1 - (NUM_XCHNCSW-NUM_TELEMETRY), 128+cs.v2), m_posHorz==2 ? attr : 0);
          v2_min = -128; v2_max = maxTelemValue(cs.v1 - (NUM_XCHNCSW-NUM_TELEMETRY)) - 128;
          if (cs.v2 > v2_max) {
            cs.v2 = v2_max;
            eeDirty(EE_MODEL);
          }
        }
        else
#endif
        {
          lcd_outdezAtt(19*FW, y, cs.v2, m_posHorz==2 ? attr : 0);
          v2_min = -125; v2_max = 125;
        }
    }
    else if (cstate == CS_VBOOL)
    {
        putsSwitches(12*FW-6, y, cs.v1, m_posHorz==1 ? attr : 0);
        putsSwitches(17*FW, y, cs.v2, m_posHorz==2 ? attr : 0);
        v1_min = SWITCH_OFF; v1_max = SWITCH_ON;
        v2_min = SWITCH_OFF; v2_max = SWITCH_ON;
    }
    else // cstate == CS_COMP
    {
        putsChnRaw(12*FW-6, y, cs.v1, m_posHorz==1 ? attr : 0);
        putsChnRaw(17*FW, y, cs.v2, m_posHorz==2 ? attr : 0);
    }

    if ((s_editMode>0 || p1valdiff) && attr) {
      switch (m_posHorz) {
        case 0:
          CHECK_INCDEC_MODELVAR(event, cs.func, 0,CS_MAXF);
          if (cstate != CS_STATE(cs.func)) {
            cs.v1 = 0;
            cs.v2 = 0;
          }
          break;
        case 1:
          CHECK_INCDEC_MODELVAR(event, cs.v1, v1_min, v1_max);
          break;
        case 2:
          CHECK_INCDEC_MODELVAR(event, cs.v2, v2_min, v2_max);
          break;
      }
    }
  }
}
#endif

void menuProcFunctionSwitches(uint8_t event)
{
#if defined(PCBARM) && defined(SDCARD)
  uint8_t _event = event;
  if (s_warning || s_menu_count) {
    event = 0;
  }
#endif

  MENU(STR_MENUFUNCSWITCHES, menuTabModel, e_FunctionSwitches, NUM_FSW+1, {0, 3/*repeated*/});

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
          putsSwitches(3, y, sd->swtch, attr | ((abs(sd->swtch) <= MAX_SWITCH && getSwitch(sd->swtch, 0) && (sd->func >= 16 || (FSW_PARAM(sd) & 1))) ? BOLD : 0));
          if (active) {
            CHECK_INCDEC_MODELSWITCH( event, sd->swtch, SWITCH_OFF-MAX_SWITCH, SWITCH_ON+MAX_SWITCH);
          }
          break;
        case 1:
          if (sd->swtch) {
            uint8_t func_displayed;
            if (sd->func < 16) {
              func_displayed = 0;
              putsChnRaw(11*FW-2, y, NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+NUM_STICKS+2+3+NUM_PPM+sd->func+1, attr);
            }
            else if (sd->func < 16 + NUM_STICKS + 1) {
              func_displayed = 1;
              if (sd->func != FUNC_TRAINER)
                putsChnRaw(13*FW-2, y, sd->func-FUNC_TRAINER, attr);
            }
            else
              func_displayed = 2 + sd->func - 16 - NUM_STICKS - 1;
            lcd_putsiAtt(5*FW-2, y, STR_VFSWFUNC, func_displayed, attr);
            if (active) {
              CHECK_INCDEC_MODELVAR( event, sd->func, 0, FUNC_MAX-1);
            }
          }
          else if (attr) {
            m_posHorz = 0;
          }
          break;
        case 2:
          if (sd->swtch) {
            int16_t val_displayed = FSW_PARAM(sd);
            int16_t val_min = 0;
            int16_t val_max = 255;
            if (sd->func == FUNC_PLAY_SOUND) {
#if defined(AUDIO)
              val_max = AU_FRSKY_LAST-AU_FRSKY_FIRST-1;
              lcd_putsiAtt(15*FW, y, STR_FUNCSOUNDS, val_displayed, attr);
#else
              break;
#endif
            }
#if defined(HAPTIC)
            else if (sd->func == FUNC_HAPTIC) {
              val_max = 3;
              lcd_outdezAtt(21*FW, y, val_displayed, attr);
            }
#endif
#if defined(PCBARM) && defined(SDCARD)
            else if (sd->func == FUNC_PLAY_TRACK) {
              if (sd->param[0] && sd->param[1])
                lcd_putsnAtt(15*FW, y, sd->param, sizeof(sd->param), attr);
              else
                lcd_putsiAtt(15*FW, y, STR_VCSWFUNC, 0, attr);
              if (active && event==EVT_KEY_BREAK(KEY_MENU)) {
                s_editMode = 0;
                if (!listSdFiles(SOUNDS_PATH, SOUNDS_EXT)) {
                  s_warning = PSTR("No Sounds on SD");
                  s_menu_flags = 0;
                }
              }
              break;
            }
            else if (sd->func == FUNC_PLAY_VALUE) {
              val_max = NUM_XCHNPLAY-1;
              putsChnRaw(17*FW, y, val_displayed+1, attr);
            }
#endif
#if defined(PCBARM)
            else if (sd->func == FUNC_VOLUME) {
              val_max = NUM_XCHNRAW-1;
              putsChnRaw(17*FW, y, val_displayed+1, attr);
            }
#endif
#if defined(SOMO)
            else if (sd->func == FUNC_PLAY_TRACK) {
              lcd_outdezAtt(21*FW, y, val_displayed, attr);
            }
            else if (sd->func == FUNC_PLAY_VALUE) {
              val_max = NUM_XCHNPLAY-1;
              putsChnRaw(17*FW, y, val_displayed+1, attr);
            }
#endif
            else if (sd->func == FUNC_RESET) {
              val_max = 3;
              lcd_putsiAtt(15*FW, y, STR_VFSWRESET, FSW_PARAM(sd), attr);
            }
            else if (sd->func <= FUNC_SAFETY_CH16) {
              int8_t value = ((int8_t)FSW_PARAM(sd) >> 1);
              lcd_outdezAtt(18*FW, y, limit((int8_t)-125, (int8_t)(value * 2), (int8_t)125), attr);
              if (active) FSW_PARAM(sd) = (FSW_PARAM(sd) & 1) + (checkIncDecModel(event, value, -63, 63) << 1);
              break;
            }
            else {
              if (attr) m_posHorz = 0;
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
          if (sd->swtch && sd->func <= FUNC_SAFETY_CH16) {
#if defined(GRAPHICS)
            menu_lcd_onoff(20*FW, y, (FSW_PARAM(sd) & 1), attr ) ;
#else
            menu_lcd_onoff(18*FW+2, y, (FSW_PARAM(sd) & 1), attr ) ;
#endif
            if (active) FSW_PARAM(sd) = (FSW_PARAM(sd) & 0xfe) + checkIncDecModel(event, FSW_PARAM(sd) & 1, 0, 1);
          }
          else if (attr) {
            m_posHorz = 0;
          }
          break;
      }
    }
  }

#if defined(PCBARM) && defined(SDCARD)
  if (s_warning) {
    displayConfirmation(_event);
  }

  if (s_menu_count) {
    const char * result = displayMenu(_event);
    if (result) {
      if (result == STR_UPDATE_LIST) {
        if (!listSdFiles(SOUNDS_PATH, SOUNDS_EXT)) {
          s_warning = PSTR("No Sounds on SD");
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

enum menuProcTelemetryItems {
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
#if defined(VARIO)
  ITEM_TELEMETRY_VARIO_LABEL,
  ITEM_TELEMETRY_VARIO_SOURCE,
  ITEM_TELEMETRY_VARIO_SPEED,
#endif
  ITEM_TELEMETRY_CUSTOM_SCREEN_LABEL,
  ITEM_TELEMETRY_CUSTOM_SCREEN_LINE1,
  ITEM_TELEMETRY_CUSTOM_SCREEN_LINE2,
  ITEM_TELEMETRY_CUSTOM_SCREEN_LINE3,
  ITEM_TELEMETRY_CUSTOM_SCREEN_LINE4,
  ITEM_TELEMETRY_GAUGES_LABEL,
  ITEM_TELEMETRY_GAUGES_LINE1,
  ITEM_TELEMETRY_GAUGES_LINE2,
  ITEM_TELEMETRY_GAUGES_LINE3,
  ITEM_TELEMETRY_GAUGES_LINE4,
  ITEM_TELEMETRY_MAX
};

#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
#define USRDATA_LINES (uint8_t)-1, 0, 0,
#else
#define USRDATA_LINES
#endif
#if defined(VARIO)
#define VARIO_LINES   (uint8_t)-1, 0, 1,
#else
#define VARIO_LINES
#endif

#ifdef FRSKY
#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_CZ)
#define TELEM_COL2 (9*FW)
#else
#define TELEM_COL2 (8*FW)
#endif

void menuProcTelemetry(uint8_t event)
{
  MENU(STR_MENUTELEMETRY, menuTabModel, e_Telemetry, ITEM_TELEMETRY_MAX+1, {0, (uint8_t)-1, 1, 0, 2, 2, (uint8_t)-1, 1, 0, 2, 2, (uint8_t)-1, 1, 1, USRDATA_LINES 0, 0, VARIO_LINES (uint8_t)-1, 1, 1, 1, 1, (uint8_t)-1, 2, 2, 2, 2});

  uint8_t sub = m_posVert - 1;

  switch (event) {
    case EVT_KEY_BREAK(KEY_DOWN):
    case EVT_KEY_BREAK(KEY_UP):
    case EVT_KEY_BREAK(KEY_LEFT):
    case EVT_KEY_BREAK(KEY_RIGHT):
      frskyEvalCurrentConsumptionBoundary();
      if (s_editMode>0 && sub<=ITEM_TELEMETRY_RSSI_ALARM2)
        FRSKY_setModelAlarms(); // update Fr-Sky module when edit mode exited
      break;
  }

  for (uint8_t i=0; i<7; i++) {
    uint8_t y = 1*FH + i*FH;
    uint8_t k = i + s_pgOfs;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);
    uint8_t ch = (k >= ITEM_TELEMETRY_A2_LABEL) ? 1 : 0;
    FrSkyChannelData & channel = g_model.frsky.channels[ch];

    switch(k) {
      case ITEM_TELEMETRY_A1_LABEL:
      case ITEM_TELEMETRY_A2_LABEL:
        lcd_putsLeft(y, STR_ACHANNEL);
        lcd_outdezAtt(2*FW, y, ch+1, 0);
        putsTelemetryChannel(TELEM_COL2+6*FW, y, ch+MAX_TIMERS, frskyData.analog[ch].value, LEFT);
        break;

      case ITEM_TELEMETRY_A1_RANGE:
      case ITEM_TELEMETRY_A2_RANGE:
        lcd_puts(4, y, STR_RANGE);
        putsTelemetryChannel(TELEM_COL2, y, ch+MAX_TIMERS, 255-channel.offset, ((attr && m_posHorz==0) ? blink : 0)|NO_UNIT|LEFT);
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
            CHECK_INCDEC_MODELVAR(event, channel.type, 0, UNIT_MAX-1);
          }
        }
        break;

      case ITEM_TELEMETRY_A1_OFFSET:
      case ITEM_TELEMETRY_A2_OFFSET:
        lcd_puts(4, y, STR_OFFSET);
        putsTelemetryChannel(TELEM_COL2, y, ch+MAX_TIMERS, 0, LEFT|attr);
        if (attr) channel.offset = checkIncDec(event, channel.offset, -256, 256, EE_MODEL);
        break;

      case ITEM_TELEMETRY_A1_ALARM1:
      case ITEM_TELEMETRY_A1_ALARM2:
      case ITEM_TELEMETRY_A2_ALARM1:
      case ITEM_TELEMETRY_A2_ALARM2:
      {
        uint8_t j = ((k==ITEM_TELEMETRY_A1_ALARM1 || k==ITEM_TELEMETRY_A2_ALARM1) ? 0 : 1);
        lcd_puts(4, y, STR_ALARM);
        lcd_putsiAtt(TELEM_COL2, y, STR_VALARM, ALARM_LEVEL(ch, j), (attr && m_posHorz==0) ? blink : 0);
        lcd_putsiAtt(TELEM_COL2+4*FW, y, STR_VALARMFN, ALARM_GREATER(ch, j), (attr && m_posHorz==1) ? blink : 0);
        putsTelemetryChannel(TELEM_COL2+6*FW, y, ch+MAX_TIMERS, channel.alarms_value[j], (attr && m_posHorz==2 ? blink : 0) | LEFT);

        if (attr && (s_editMode>0 || p1valdiff)) {
          uint8_t t;
          switch (m_posHorz) {
           case 0:
             t = ALARM_LEVEL(ch, j);
             channel.alarms_level = (channel.alarms_level & ~(3<<(2*j))) + (checkIncDec(event, t, 0, 3, EE_MODEL) << (2*j));
             break;
           case 1:
             t = ALARM_GREATER(ch, j);
             channel.alarms_greater = (channel.alarms_greater & ~(1<<j)) + (checkIncDec(event, t, 0, 1, EE_MODEL) << j);
             if(checkIncDec_Ret)
               FRSKY_setModelAlarms();
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
        lcd_puts(4, y, STR_ALARM);
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
        lcd_puts(4, y, STR_PROTO);
        lcd_putsiAtt(TELEM_COL2, y, STR_VTELPROTO, g_model.frsky.usrProto, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.frsky.usrProto, 0, 2);
        break;

      case ITEM_TELEMETRY_USR_BLADES:
        lcd_puts(4, y, STR_BLADES);
        lcd_outdezAtt(TELEM_COL2+FWNUM, y, 2+g_model.frsky.blades, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.frsky.blades, 0, 3);
        break;
#endif

      case ITEM_TELEMETRY_USR_VOLTAGE_SOURCE:
        lcd_puts(4, y, STR_VOLTAGE);
        lcd_putsiAtt(TELEM_COL2, y, STR_VOLTSRC, g_model.frsky.voltsSource+1, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.frsky.voltsSource, 0, 3);
        break;

      case ITEM_TELEMETRY_USR_CURRENT_SOURCE:
        lcd_puts(4, y, STR_CURRENT);
        lcd_putsiAtt(TELEM_COL2, y, STR_VOLTSRC, g_model.frsky.currentSource, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.frsky.currentSource, 0, 3);
        break;

#if defined(VARIO)
      case ITEM_TELEMETRY_VARIO_LABEL:
        lcd_putsLeft(y, STR_VARIO);
        break;

      case ITEM_TELEMETRY_VARIO_SOURCE:
        lcd_puts(4, y, STR_SOURCE);
        lcd_putsiAtt(TELEM_COL2, y, STR_VARIOSRC, g_model.varioSource, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.varioSource, VARIO_SOURCE_FIRST, VARIO_SOURCE_LAST-1);
        break;

      case ITEM_TELEMETRY_VARIO_SPEED:
        lcd_puts(4, y, STR_LIMIT);
        if (!g_model.varioSpeedDownMin)
          lcd_putsAtt(TELEM_COL2, y, STR_OFF, ((attr && m_posHorz==0) ? blink : 0));
        else
          lcd_outdezAtt(TELEM_COL2, y, -VARIO_SPEED_LIMIT_MUL*(VARIO_SPEED_LIMIT_DOWN_OFF - g_model.varioSpeedDownMin), ((attr && m_posHorz==0) ? blink : 0)|PREC2|LEFT);
        lcd_outdezAtt(TELEM_COL2+6*FW, y, VARIO_SPEED_LIMIT_MUL*(g_model.varioSpeedUpMin - VARIO_SPEED_LIMIT_UP_CENTER), ((attr && m_posHorz==1) ? blink : 0)|PREC2|LEFT);

        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (m_posHorz) {
            case 0:
              g_model.varioSpeedDownMin = checkIncDec(event, g_model.varioSpeedDownMin, 0, VARIO_SPEED_LIMIT_DOWN_OFF, EE_MODEL);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.varioSpeedUpMin, 0, VARIO_SPEED_LIMIT_UP_MAX);
              break;
          }
        }
        break;
#endif

      case ITEM_TELEMETRY_CUSTOM_SCREEN_LABEL:
        lcd_putsLeft(y, STR_DISPLAY);
        break;

      case ITEM_TELEMETRY_CUSTOM_SCREEN_LINE1:
      case ITEM_TELEMETRY_CUSTOM_SCREEN_LINE2:
      case ITEM_TELEMETRY_CUSTOM_SCREEN_LINE3:
      case ITEM_TELEMETRY_CUSTOM_SCREEN_LINE4:
        for (uint8_t c=0; c<2; c++) {
          uint8_t j = k - ITEM_TELEMETRY_CUSTOM_SCREEN_LINE1;
          uint8_t value = getTelemCustomField(j, c);
          lcd_putsiAtt(c==0?4:TELEM_COL2, y, STR_VTELEMCHNS, value, (attr && m_posHorz==c) ? blink : 0);
          if (attr && m_posHorz==c && (s_editMode>0 || p1valdiff)) {
            CHECK_INCDEC_MODELVAR(event, value, 0, (j==3 && c==0) ? TELEM_STATUS_MAX : TELEM_DISPLAY_MAX);
            if (checkIncDec_Ret) {
#if defined (PCBARM)
              g_model.frsky.lines[2*j+c] = value;
#else
              g_model.frskyLines[j] = (c==0 ? ((g_model.frskyLines[j] & 0xf0) + (value & 0x0f)) : (g_model.frskyLines[j] & 0x0f) + ((value & 0x0f) << 4));
              uint16_t mask = 0x3 << (4*j+2*c);
              g_model.frskyLinesXtra &= ~mask;
              mask = (value / 16) << (4*j+2*c);
              g_model.frskyLinesXtra |= mask;
#endif
            }
          }
        }
        break;

      case ITEM_TELEMETRY_GAUGES_LABEL:
        lcd_putsLeft(y, STR_BARS);
        break;

      case ITEM_TELEMETRY_GAUGES_LINE1:
      case ITEM_TELEMETRY_GAUGES_LINE2:
      case ITEM_TELEMETRY_GAUGES_LINE3:
      case ITEM_TELEMETRY_GAUGES_LINE4:
      {
        FrSkyBarData & bar = g_model.frsky.bars[k-ITEM_TELEMETRY_GAUGES_LINE1];
        uint8_t barSource = bar.source;
        lcd_putsiAtt(4, y, STR_VTELEMCHNS, barSource, (attr && m_posHorz==0) ? blink : 0);
        if (barSource) {
          putsTelemetryChannel(56-3*FW, y, barSource-1, convertTelemValue(barSource, bar.barMin*5), (attr && m_posHorz==1 ? blink : 0) | LEFT);
          putsTelemetryChannel(14*FW-3, y, barSource-1, convertTelemValue(barSource, (51-bar.barMax)*5), (attr && m_posHorz==2 ? blink : 0) | LEFT);
        }
        else {
          if (attr) m_posHorz = 0;
        }
        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (m_posHorz) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, barSource, 0, g_model.frsky.usrProto ? TELEM_BAR_MAX : TELEM_NOUSR_BAR_MAX);
              if (checkIncDec_Ret) {
                bar.source = barSource;
                bar.barMin = 0;
                bar.barMax = 51 - (maxTelemValue(barSource) / 5);
              }
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, bar.barMin, 0, 50-bar.barMax);
              break;
            case 2:
              bar.barMax = 51 - checkIncDec(event, 51 - bar.barMax, bar.barMin+1, maxTelemValue(barSource) / 5, EE_MODEL);
              break;
          }
        }
        break;
      }
    }
  }
}
#endif

#ifdef TEMPLATES
void menuProcTemplates(uint8_t _event)
{
  uint8_t event = (s_warning ? 0 : _event);

  SIMPLE_MENU(STR_MENUTEMPLATES, menuTabModel, e_Templates, 1+TMPL_COUNT);

  uint8_t sub = m_posVert - 1;

  if (s_confirmation) {
    if (sub<TMPL_COUNT)
      applyTemplate(sub);
    s_confirmation = 0;
    AUDIO_WARNING2();
  }

  switch(event)
  {
    case EVT_KEY_FIRST(KEY_MENU):
      if (sub!=255) {
        s_warning = STR_VTEMPLATES+1 + (sub * LEN2_VTEMPLATES);
      }
      killEvents(event);
      _event = 0;
      s_editMode = 0;
      break;
  }

  uint8_t y = 1*FH;
  uint8_t k = 0;
  for (uint8_t i=0; i<7 && k<TMPL_COUNT; i++) {
    k=i+s_pgOfs;
    lcd_outdezNAtt(3*FW, y, k, (sub==k ? INVERS : 0)|LEADING0, 2);
    lcd_putsiAtt(4*FW, y, STR_VTEMPLATES, k, (sub==k ? INVERS  : 0));
    y+=FH;
  }

  if (s_warning) {
    displayConfirmation(_event);
  }
}
#endif

