/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _MENUS_H_
#define _MENUS_H_

#include "../../keys.h"

#if defined(TRANSLATIONS_FR)
  #define MENU_COLUMNS         1
  #define COLUMN_X             0
#else
  #define MENU_COLUMNS         2
#endif

#define MENU_COLUMN2_X         280
#define MIXES_2ND_COLUMN       100
#define COLUMN_HEADER_X        150

typedef evt_t & check_event_t;

extern uint8_t menuPageIndex;
extern uint8_t menuPageCount;
extern uint16_t linesCount;
extern int menuVerticalPosition;
extern int menuHorizontalPosition;
extern int menuVerticalOffset;
extern uint8_t calibrationState;

// Temporary no highlight
extern uint8_t noHighlightCounter;
#define NO_HIGHLIGHT()        (noHighlightCounter > 0)
#define START_NO_HIGHLIGHT()  do { noHighlightCounter = 25; } while(0)

typedef bool (*MenuHandlerFunc)(evt_t event);

extern MenuHandlerFunc menuHandlers[5];
extern uint8_t menuVerticalPositions[4];
extern uint8_t menuLevel;
extern evt_t menuEvent;

/// goto given Menu, but substitute current menu in menuStack
void chainMenu(MenuHandlerFunc newMenu);
/// goto given Menu, store current menu in menuStack
void pushMenu(MenuHandlerFunc newMenu);
/// return to last menu in menustack
void popMenu();

enum EnumTabModel {
  // e_ModelSelect,
  e_ModelSetup,
  CASE_HELI(e_Heli)
  CASE_FLIGHT_MODES(e_FlightModesAll)
  e_InputsAll,
  e_MixAll,
  e_Limits,
  CASE_CURVES(e_CurvesAll)
  CASE_GVARS(e_GVars)
  e_LogicalSwitches,
  e_CustomFunctions,
#if defined(LUA_MODEL_SCRIPTS)
  e_CustomScripts,
#endif
  CASE_FRSKY(e_Telemetry)
};

bool menuModelSetup(evt_t event);
bool menuModelHeli(evt_t event);
bool menuModelFlightModesAll(evt_t event);
bool menuModelExposAll(evt_t event);
bool menuModelMixAll(evt_t event);
bool menuModelLimits(evt_t event);
bool menuModelCurvesAll(evt_t event);
bool menuModelCurveOne(evt_t event);
bool menuModelGVars(evt_t event);
bool menuModelLogicalSwitches(evt_t event);
bool menuModelCustomFunctions(evt_t event);
bool menuModelCustomScripts(evt_t event);
bool menuModelTelemetry(evt_t event);
bool menuModelExpoOne(evt_t event);

const MenuHandlerFunc menuTabModel[] = {
//   menuModelSelect,
  menuModelSetup,
  CASE_HELI(menuModelHeli)
  CASE_FLIGHT_MODES(menuModelFlightModesAll)
  menuModelExposAll,
  menuModelMixAll,
  menuModelLimits,
  CASE_CURVES(menuModelCurvesAll)
#if defined(GVARS) && defined(FLIGHT_MODES)
  CASE_GVARS(menuModelGVars)
#endif
  menuModelLogicalSwitches,
  menuModelCustomFunctions,
#if defined(LUA_MODEL_SCRIPTS)
  menuModelCustomScripts,
#endif
  CASE_FRSKY(menuModelTelemetry)
  CASE_MAVLINK(menuTelemetryMavlinkSetup)
  CASE_TEMPLATES(menuModelTemplates)
};

enum EnumTabRadio {
  e_Setup,
  e_Sd,
  e_GeneralCustomFunctions,
  e_Trainer,
  e_Hardware,
  e_Vers,
};

bool menuGeneralSetup(evt_t event);
bool menuGeneralSdManager(evt_t event);
bool menuGeneralCustomFunctions(evt_t event);
bool menuGeneralTrainer(evt_t event);
bool menuGeneralVersion(evt_t event);
bool menuGeneralHardware(evt_t event);
bool menuGeneralCalib(evt_t event);

static const MenuHandlerFunc menuTabGeneral[] PROGMEM = {
  menuGeneralSetup,
  menuGeneralSdManager,
  menuGeneralCustomFunctions,
  menuGeneralTrainer,
  menuGeneralHardware,
  menuGeneralVersion,
};

extern const MenuHandlerFunc menuTabScreensSetup[1+MAX_CUSTOM_SCREENS] PROGMEM;

bool menuFirstCalib(evt_t event);
bool menuMainView(evt_t event);
bool menuCustomFunctions(evt_t event, CustomFunctionData * functions, CustomFunctionsContext & functionsContext);
bool menuModelSelect(evt_t event);
bool menuStatisticsView(evt_t event);
bool menuStatisticsDebug(evt_t event);
bool menuAboutView(evt_t event);
bool menuMainViewChannelsMonitor(evt_t event);
bool menuChannelsView(evt_t event);
bool menuChannelsView(evt_t event);
bool menuTextView(evt_t event);
bool menuScreensTheme(evt_t event);

#if defined(DEBUG_TRACE_BUFFER)
void menuTraceBuffer(evt_t event);
#endif

typedef uint16_t FlightModesType;

extern int8_t checkIncDec_Ret;  // global helper vars

#define EDIT_SELECT_MENU   -1
#define EDIT_SELECT_FIELD  0
#define EDIT_MODIFY_FIELD  1
#define EDIT_MODIFY_STRING 2
extern int8_t s_editMode;       // global editmode

// checkIncDec flags
#define EE_GENERAL                     0x01
#define EE_MODEL                       0x02
#define NO_INCDEC_MARKS                0x04
#define INCDEC_SWITCH                  0x08
#define INCDEC_SOURCE                  0x10
#define INCDEC_REP10                   0x40
#define NO_DBLKEYS                     0x80

// mawrow special values
#define TITLE_ROW                      ((uint8_t)-1)
#define ORPHAN_ROW                     ((uint8_t)-2)
#define HIDDEN_ROW                     ((uint8_t)-3)
#define NAVIGATION_LINE_BY_LINE        0x40
#define CURSOR_ON_LINE()               (menuHorizontalPosition<0)

struct CheckIncDecStops {
  const int count;
  const int stops[];
  int min() const
  {
    return stops[0];
  }
  int max() const
  {
    return stops[count-1];
  }
  bool contains(int value) const
  {
    for (int i=0; i<count; ++i) {
      int stop = stops[i];
      if (value == stop)
        return true;
      else if (value < stop)
        return false;
    }
    return false;
  }
};
extern const CheckIncDecStops &stops100;
extern const CheckIncDecStops &stops1000;
extern const CheckIncDecStops &stopsSwitch;

#define INIT_STOPS(var, ...)                                        \
  const int _ ## var[] = { __VA_ARGS__ };                           \
  const CheckIncDecStops &var  = (const CheckIncDecStops&)_ ## var;
#define CATEGORY_END(val)                                          \
  (val), (val+1)

int checkIncDec(evt_t event, int val, int i_min, int i_max, unsigned int i_flags=0, IsValueAvailable isValueAvailable=NULL, const CheckIncDecStops &stops=stops100);
swsrc_t checkIncDecMovedSwitch(swsrc_t val);
#define checkIncDecModel(event, i_val, i_min, i_max) checkIncDec(event, i_val, i_min, i_max, EE_MODEL)
#define checkIncDecModelZero(event, i_val, i_max) checkIncDec(event, i_val, 0, i_max, EE_MODEL)
#define checkIncDecGen(event, i_val, i_min, i_max) checkIncDec(event, i_val, i_min, i_max, EE_GENERAL)

#define CHECK_INCDEC_MODELVAR(event, var, min, max) \
  var = checkIncDecModel(event, var, min, max)

#define CHECK_INCDEC_MODELVAR_ZERO(event, var, max) \
  var = checkIncDecModelZero(event, var, max)

#define CHECK_INCDEC_MODELVAR_CHECK(event, var, min, max, check) \
  var = checkIncDec(event, var, min, max, EE_MODEL, check)

#define CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, var, max, check) \
  var = checkIncDec(event, var, 0, max, EE_MODEL, check)

#define AUTOSWITCH_ENTER_LONG() (attr && event==EVT_KEY_LONG(KEY_ENTER))
#define CHECK_INCDEC_SWITCH(event, var, min, max, flags, available) \
  var = checkIncDec(event, var, min, max, (flags)|INCDEC_SWITCH, available)
#define CHECK_INCDEC_MODELSWITCH(event, var, min, max, available) \
  CHECK_INCDEC_SWITCH(event, var, min, max, EE_MODEL, available)

#define CHECK_INCDEC_MODELSOURCE(event, var, min, max) \
  var = checkIncDec(event, var, min, max, EE_MODEL|INCDEC_SOURCE|NO_INCDEC_MARKS, isSourceAvailable)

#define CHECK_INCDEC_GENVAR(event, var, min, max) \
  var = checkIncDecGen(event, var, min, max)

#define INCDEC_DECLARE_VARS(f)         uint8_t incdecFlag = (f); IsValueAvailable isValueAvailable = NULL
#define INCDEC_SET_FLAG(f)             incdecFlag = (f)
#define INCDEC_ENABLE_CHECK(fn)        isValueAvailable = fn
#define CHECK_INCDEC_PARAM(event, var, min, max) \
                                       checkIncDec(event, var, min, max, incdecFlag, isValueAvailable)

bool navigate(evt_t event, int count, int rows, int columns=1);
bool check(check_event_t event, uint8_t curr, const MenuHandlerFunc * menuTab, uint8_t menuTabSize, const pm_uint8_t *horTab, uint8_t horTabMax, int maxrow, uint8_t flags=0);
bool check_simple(check_event_t event, uint8_t curr, const MenuHandlerFunc * menuTab, uint8_t menuTabSize, int maxrow);
bool check_submenu_simple(check_event_t event, uint8_t maxrow);

#define MENU_TAB(...) const uint8_t mstate_tab[] = __VA_ARGS__

#define MENU_WITH_OPTIONS(title, icons, tab, tabCount, menu, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  if (event == EVT_ENTRY || event == EVT_ENTRY_UP) TRACE("Menu %s displayed ...", title); \
  if (!check(event, menu, tab, tabCount, mstate_tab, DIM(mstate_tab)-1, lines_count)) return false; \
  drawMenuTemplate(title, icons); \

#define MENU(title, icons, tab, menu, lines_count, ...) \
  MENU_WITH_OPTIONS(title, icons, tab, DIM(tab), menu, lines_count, __VA_ARGS__)

#define SIMPLE_MENU_WITH_OPTIONS(title, icons, tab, tabCount, menu, lines_count) \
  if (event == EVT_ENTRY || event == EVT_ENTRY_UP) TRACE("Menu %s displayed ...", title); \
  if (!check_simple(event, menu, tab, tabCount, lines_count)) return false; \
  drawMenuTemplate(title, icons);

#define SIMPLE_MENU(title, icons, tab, menu, lines_count) \
  SIMPLE_MENU_WITH_OPTIONS(title, icons, tab, DIM(tab), menu, lines_count)

#define SUBMENU(title, icon, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  if (!check(event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, lines_count)) return false; \
  drawScreenTemplate(title, icon);

#define SUBMENU_WITH_OPTIONS(title, icon, lines_count, options, ...) \
  MENU_TAB(__VA_ARGS__); \
  if (!check(event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, lines_count)) return false; \
  drawScreenTemplate(title, icon, options);

#define SIMPLE_SUBMENU_NOTITLE(lines_count) \
  if (!check_submenu_simple(event, lines_count)) return false

#define SIMPLE_SUBMENU(title, icon, lines_count) \
  SIMPLE_SUBMENU_NOTITLE(lines_count); \
  drawScreenTemplate(title, icon)

typedef int select_menu_value_t;

select_menu_value_t selectMenuItem(coord_t x, coord_t y, const pm_char * values, select_menu_value_t value, select_menu_value_t min, select_menu_value_t max, LcdFlags attr, evt_t event);
uint8_t editCheckBox(uint8_t value, coord_t x, coord_t y, LcdFlags attr, evt_t event);
int8_t switchMenuItem(coord_t x, coord_t y, int8_t value, LcdFlags attr, evt_t event);

#if defined(GVARS)
  #define GVAR_MENU_ITEM(x, y, v, min, max, lcdattr, editflags, event) editGVarFieldValue(x, y, v, min, max, lcdattr, editflags, event)
#else
  #define GVAR_MENU_ITEM(x, y, v, min, max, lcdattr, editflags, event) editGVarFieldValue(x, y, v, min, max, lcdattr, event)
#endif

#if defined(GVARS)
  int16_t editGVarFieldValue(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t editflags, evt_t event);
  #define displayGVar(x, y, v, min, max) GVAR_MENU_ITEM(x, y, v, min, max, 0, 0, 0)
#else
  int16_t editGVarFieldValue(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, evt_t event);
  #define displayGVar(x, y, v, min, max) lcdDrawNumber(x, y, v)
#endif

extern uint8_t editNameCursorPos;
void editName(coord_t x, coord_t y, char *name, uint8_t size, evt_t event, uint8_t active, LcdFlags flags=ZCHAR);

uint8_t editDelay(const coord_t x, const coord_t y, const evt_t event, const uint8_t attr, uint8_t delay);
void editCurveRef(coord_t x, coord_t y, CurveRef & curve, evt_t event, uint8_t attr);

extern uint8_t s_curveChan;

#define WARNING_TYPE_ASTERISK  0
#define WARNING_TYPE_CONFIRM   1
#define WARNING_TYPE_INPUT     2

extern const pm_char * warningText;
extern const pm_char * warningInfoText;
extern uint8_t         warningInfoLength;
extern uint8_t         warningResult;
extern uint8_t         warningType;

#define COPY_MODE 1
#define MOVE_MODE 2
extern uint8_t s_copyMode;
extern int8_t s_copySrcRow;
extern int8_t s_copyTgtOfs;
extern uint8_t s_currIdx;
extern int8_t s_currCh;
extern uint8_t s_copySrcIdx;
extern uint8_t s_copySrcCh;

uint8_t getExposCount();
void deleteExpo(uint8_t idx);
void insertExpo(uint8_t idx);

uint8_t getMixesCount();
void deleteMix(uint8_t idx);
void insertMix(uint8_t idx);

#define MENU_X                 80
#define MENU_W                 LCD_W-(2*MENU_X)
#define POPUP_X                130
#define POPUP_Y                70
#define POPUP_W                300
#define POPUP_H                130
#define WARNING_LINE_LEN       32
#define WARNING_LINE_X         (POPUP_X+76)
#define WARNING_LINE_Y         (POPUP_Y+9)
#define WARNING_INFOLINE_Y     (WARNING_LINE_Y+68)

void copySelection(char * dst, const char * src, uint8_t size);

void displayPopup(const char * title);
void displayWarning(evt_t event);

extern void (* popupFunc)(evt_t event);
extern int16_t warningInputValue;
extern int16_t warningInputValueMin;
extern int16_t warningInputValueMax;
extern uint8_t warningInfoFlags;

#define DISPLAY_WARNING                (*popupFunc)
#define POPUP_WARNING(s)               (warningText = s, warningInfoText = 0, popupFunc = displayWarning)
#define POPUP_CONFIRMATION(s)          (warningText = s, warningType = WARNING_TYPE_CONFIRM, warningInfoText = 0, popupFunc = displayWarning)
#define POPUP_INPUT(s, func, start, min, max) (warningText = s, warningType = WARNING_TYPE_INPUT, popupFunc = func, warningInputValue = start, warningInputValueMin = min, warningInputValueMax = max)
#define WARNING_INFO_FLAGS             warningInfoFlags
#define SET_WARNING_INFO(info, len, flags)    (warningInfoText = info, warningInfoLength = len, warningInfoFlags = flags)

#define NAVIGATION_MENUS
#define POPUP_MENU_ADD_ITEM(s)         do { popupMenuOffsetType = MENU_OFFSET_INTERNAL; if (popupMenuNoItems < POPUP_MENU_MAX_LINES) popupMenuItems[popupMenuNoItems++] = s; } while (0)
#define POPUP_MENU_MAX_LINES           12
#define MENU_MAX_DISPLAY_LINES         9
#define MENU_LINE_LENGTH               (LEN_MODEL_NAME+12)
#define POPUP_MENU_ITEMS_FROM_BSS()
extern const char * popupMenuItems[POPUP_MENU_MAX_LINES];
extern uint16_t popupMenuNoItems;
extern uint16_t popupMenuOffset;
enum {
  MENU_OFFSET_INTERNAL,
  MENU_OFFSET_EXTERNAL
};
extern uint8_t popupMenuOffsetType;
const char * displayPopupMenu(evt_t event);
extern void (*popupMenuHandler)(const char * result);

#define TEXT_FILENAME_MAXLEN           40
extern char s_text_file[TEXT_FILENAME_MAXLEN];
void pushMenuTextView(const char * filename);
void pushModelNotes();

#define LABEL(...)                     (uint8_t)-1

#define CURSOR_MOVED_LEFT(event)       (event==EVT_ROTARY_LEFT || EVT_KEY_MASK(event) == KEY_LEFT)
#define CURSOR_MOVED_RIGHT(event)      (event==EVT_ROTARY_RIGHT || EVT_KEY_MASK(event) == KEY_RIGHT)

#define REPEAT_LAST_CURSOR_MOVE(defaultPosition)  { if (CURSOR_MOVED_LEFT(event) || CURSOR_MOVED_RIGHT(event)) putEvent(event); else menuHorizontalPosition = (defaultPosition); }
#define MOVE_CURSOR_FROM_HERE()        if (menuHorizontalPosition > 0) REPEAT_LAST_CURSOR_MOVE(0)

#define MENU_FIRST_LINE_EDIT           (MAXCOL((uint16_t)0) >= HIDDEN_ROW ? (MAXCOL((uint16_t)1) >= HIDDEN_ROW ? 2 : 1) : 0)
#define POS_HORZ_INIT(posVert)         ((COLATTR(posVert) & NAVIGATION_LINE_BY_LINE) ? -1 : 0)
#define EDIT_MODE_INIT                 0 // TODO enum

typedef int (*FnFuncP) (int x);
void drawFunction(FnFuncP fn, int x, int y, int width);

uint8_t switchToMix(uint8_t source);

#endif // _MENUS_H_
