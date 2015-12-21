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

#ifndef _MENUS_H_
#define _MENUS_H_

#if defined(TRANSLATIONS_FR)
  #define MENU_COLUMNS                 1
#else
  #define MENU_COLUMNS                 2
#endif

#define lcd_putsColumnLeft(x, y, str)  lcdDrawText((x > (LCD_W-10*FW-MENUS_SCROLLBAR_WIDTH)) ? MENU_COLUMN2_X : 0, y, str)

// Menus related stuff ...
typedef uint16_t vertpos_t;

typedef uint8_t & check_event_t;
#define horzpos_t int8_t

extern tmr10ms_t menuEntryTime;

extern vertpos_t menuVerticalPosition;
extern horzpos_t menuHorizontalPosition;
extern vertpos_t menuVerticalOffset;
extern uint8_t calibrationState;

// Temporary no highlight
extern uint8_t noHighlightCounter;
#define NO_HIGHLIGHT()        (noHighlightCounter > 0)
#define START_NO_HIGHLIGHT()  do { noHighlightCounter = 25; } while(0)

void menu_lcd_onoff(coord_t x, coord_t y, uint8_t value, LcdFlags attr);

typedef void (*MenuHandlerFunc)(uint8_t event);

extern MenuHandlerFunc menuHandlers[5];
extern uint8_t menuVerticalPositions[4];
extern uint8_t menuLevel;
extern uint8_t menuEvent;

/// goto given Menu, but substitute current menu in menuStack
void chainMenu(MenuHandlerFunc newMenu);
/// goto given Menu, store current menu in menuStack
void pushMenu(MenuHandlerFunc newMenu);
/// return to last menu in menustack
void popMenu();

void doMainScreenGraphics();
void menuFirstCalib(uint8_t event);

void onMainViewMenu(const char *result);

void menuModelSelect(uint8_t event);
void menuModelSetup(uint8_t event);
void menuModelHeli(uint8_t event);
void menuModelFlightModesAll(uint8_t event);
void menuModelExposAll(uint8_t event);
void menuModelMixAll(uint8_t event);
void menuModelLimits(uint8_t event);
void menuModelCurvesAll(uint8_t event);
void menuModelCurveOne(uint8_t event);
void menuModelGVars(uint8_t event);
void menuModelLogicalSwitches(uint8_t event);
void menuModelCustomFunctions(uint8_t event);
void menuModelCustomScripts(uint8_t event);
void menuModelTelemetry(uint8_t event);
void menuModelExpoOne(uint8_t event);

void menuGeneralSetup(uint8_t event);
void menuGeneralSdManager(uint8_t event);
void menuGeneralCustomFunctions(uint8_t event);
void menuGeneralTrainer(uint8_t event);
void menuGeneralVersion(uint8_t event);
void menuGeneralDiagKeys(uint8_t event);
void menuGeneralDiagAna(uint8_t event);
void menuGeneralHardware(uint8_t event);
void menuGeneralCalib(uint8_t event);

void menuMainViewChannelsMonitor(uint8_t event);
void menuChannelsView(uint8_t event);
void menuMainView(uint8_t event);
#if defined(FRSKY)
void menuTelemetryFrsky(uint8_t event);
#endif
void menuCustomFunctions(uint8_t event, CustomFunctionData * functions, CustomFunctionsContext * functionsContext);
void menuModelNotes(uint8_t event);
void menuStatisticsView(uint8_t event);
void menuStatisticsDebug(uint8_t event);
void menuAboutView(uint8_t event);
#if defined(DEBUG_TRACE_BUFFER)
void menuTraceBuffer(uint8_t event);
#endif

enum EnumTabDiag {
  e_Setup,
  e_Sd,
  e_GeneralCustomFunctions,
  e_Trainer,
  e_Vers,
  e_Keys,
  e_Ana,
  e_Hardware,
  e_Calib
};

const MenuHandlerFunc menuTabGeneral[] = {
  menuGeneralSetup,
  menuGeneralSdManager,
  menuGeneralCustomFunctions,
  menuGeneralTrainer,
  menuGeneralVersion,
  menuGeneralDiagKeys,
  menuGeneralDiagAna,
  menuGeneralHardware,
  menuGeneralCalib
};

enum EnumTabModel {
  e_ModelSelect,
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
  CASE_MAVLINK(e_MavSetup)
  CASE_TEMPLATES(e_Templates)
};

const MenuHandlerFunc menuTabModel[] = {
  menuModelSelect,
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

void drawSlider(coord_t x, coord_t y, uint8_t value, uint8_t max, uint8_t attr);

typedef uint16_t FlightModesType;

extern int8_t checkIncDec_Ret;  // global helper vars

#define EDIT_SELECT_MENU               -1
#define EDIT_SELECT_FIELD              0
#define EDIT_MODIFY_FIELD              1
#define EDIT_MODIFY_STRING             2
extern int8_t s_editMode; // global editmode

// checkIncDec flags
#define EE_GENERAL                     0x01
#define EE_MODEL                       0x02
#define NO_INCDEC_MARKS                0x04
#define INCDEC_SWITCH                  0x08
#define INCDEC_SOURCE                  0x10
#define INCDEC_REP10                   0x40
#define NO_DBLKEYS                     0x80

#define INCDEC_DECLARE_VARS(f)         uint8_t incdecFlag = (f); IsValueAvailable isValueAvailable = NULL
#define INCDEC_SET_FLAG(f)             incdecFlag = (f)
#define INCDEC_ENABLE_CHECK(fn)        isValueAvailable = fn
#define CHECK_INCDEC_PARAM(event, var, min, max) checkIncDec(event, var, min, max, incdecFlag, isValueAvailable)

// mawrow special values
#define TITLE_ROW                      ((uint8_t)-1)
#define HIDDEN_ROW                     ((uint8_t)-2)

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

int checkIncDec(unsigned int event, int val, int i_min, int i_max, unsigned int i_flags=0, IsValueAvailable isValueAvailable=NULL, const CheckIncDecStops &stops=stops100);
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
  var = checkIncDec(event,var,min,max,EE_MODEL|INCDEC_SOURCE|NO_INCDEC_MARKS, isSourceAvailable)

#define CHECK_INCDEC_GENVAR(event, var, min, max) \
  var = checkIncDecGen(event, var, min, max)

#define NAVIGATION_LINE_BY_LINE  0x40
#define CURSOR_ON_LINE()         (menuHorizontalPosition<0)

#define CHECK_FLAG_NO_SCREEN_INDEX   1
void check(const char *title, check_event_t event, uint8_t curr, const MenuHandlerFunc *menuTab, uint8_t menuTabSize, const pm_uint8_t *horTab, uint8_t horTabMax, vertpos_t maxrow, uint8_t flags=0);
void check_simple(const char *title, check_event_t event, uint8_t curr, const MenuHandlerFunc *menuTab, uint8_t menuTabSize, vertpos_t maxrow);
void check_submenu_simple(const char *title, check_event_t event, uint8_t maxrow);

void title(const pm_char * s);
#define TITLE(str) title(str)

#define MENU_TAB(...) const uint8_t mstate_tab[] = __VA_ARGS__

#define MENU_CHECK(title, tab, menu, lines_count) \
  check(title, event, menu, tab, DIM(tab), mstate_tab, DIM(mstate_tab)-1, lines_count)

#define MENU_CHECK_FLAGS(title, tab, menu, flags, lines_count) \
  check(title, event, menu, tab, DIM(tab), mstate_tab, DIM(mstate_tab)-1, lines_count, flags)

#define MENU(title, tab, menu, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  MENU_CHECK(title, tab, menu, lines_count)

#define MENU_FLAGS(title, tab, menu, flags, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  MENU_CHECK_FLAGS(title, tab, menu, flags, lines_count)

#define SIMPLE_MENU(title, tab, menu, lines_count) \
  check_simple(title, event, menu, tab, DIM(tab), lines_count)

#define SUBMENU_NOTITLE(lines_count, ...) { \
  MENU_TAB(__VA_ARGS__); \
  check(NULL, event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, lines_count); \
  }

#define SUBMENU(title, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  check(title, event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, lines_count)

#define SIMPLE_SUBMENU_NOTITLE(lines_count) \
  check_submenu_simple(NULL, event, lines_count);

#define SIMPLE_SUBMENU(title, lines_count) \
  check_submenu_simple(title, event, lines_count)

typedef int select_menu_value_t;

select_menu_value_t selectMenuItem(coord_t x, coord_t y, const pm_char *label, const pm_char *values, select_menu_value_t value, select_menu_value_t min, select_menu_value_t max, LcdFlags attr, uint8_t event);
uint8_t onoffMenuItem(uint8_t value, coord_t x, coord_t y, const pm_char *label, LcdFlags attr, uint8_t event);
swsrc_t switchMenuItem(coord_t x, coord_t y, swsrc_t value, LcdFlags attr, uint8_t event);

#define ON_OFF_MENU_ITEM(value, x, y, label, attr, event) value = onoffMenuItem(value, x, y, label, attr, event)

#if defined(GVARS)
  #define GVAR_MENU_ITEM(x, y, v, min, max, lcdattr, editflags, event) gvarMenuItem(x, y, v, min, max, lcdattr, editflags, event)
#else
  #define GVAR_MENU_ITEM(x, y, v, min, max, lcdattr, editflags, event) gvarMenuItem(x, y, v, min, max, lcdattr, event)
#endif

#if defined(GVARS)
  int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t editflags, uint8_t event);
  #define displayGVar(x, y, v, min, max) GVAR_MENU_ITEM(x, y, v, min, max, 0, 0, 0)
#else
  int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event);
  #define displayGVar(x, y, v, min, max) lcdDraw8bitsNumber(x, y, v)
#endif

extern uint8_t s_curveChan;
void editCurveRef(coord_t x, coord_t y, CurveRef & curve, uint8_t event, uint8_t attr);

extern uint8_t editNameCursorPos;
void editName(coord_t x, coord_t y, char *name, uint8_t size, uint8_t event, uint8_t active, uint8_t attr=ZCHAR);
void editSingleName(coord_t x, coord_t y, const pm_char *label, char *name, uint8_t size, uint8_t event, uint8_t active);

#if MENU_COLUMNS > 1
uint8_t editDelay(const coord_t x, const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay);
#define EDIT_DELAY(x, y, event, attr, str, delay) editDelay(x, y, event, attr, str, delay)
#else
uint8_t editDelay(const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay);
#define EDIT_DELAY(x, y, event, attr, str, delay) editDelay(y, event, attr, str, delay)
#endif

#define WARNING_TYPE_ASTERISK          0
#define WARNING_TYPE_CONFIRM           1
#define WARNING_TYPE_INPUT             2

void copySelection(char * dst, const char * src, uint8_t size);

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

#define MENU_X                         30
#define MENU_Y                         16
#define MENU_W                         LCD_W-(2*MENU_X)
#define WARNING_LINE_LEN               32
#define WARNING_LINE_X                 16
#define WARNING_LINE_Y                 3*FH

void displayBox(const char *title);
void displayPopup(const char *title);
void displayWarning(uint8_t event);

extern void (*popupFunc)(uint8_t event);
extern int16_t warningInputValue;
extern int16_t warningInputValueMin;
extern int16_t warningInputValueMax;
extern uint8_t warningInfoFlags;

#define DISPLAY_WARNING                (*popupFunc)
#define POPUP_WARNING(s)               (warningText = s, warningInfoText = 0, popupFunc = displayWarning)
#define POPUP_CONFIRMATION(s)          (warningText = s, warningType = WARNING_TYPE_CONFIRM, warningInfoText = 0, popupFunc = displayWarning)
#define POPUP_INPUT(s, func, start, min, max) (warningText = s, warningType = WARNING_TYPE_INPUT, popupFunc = func, warningInputValue = start, warningInputValueMin = min, warningInputValueMax = max)
#define WARNING_INFO_FLAGS             warningInfoFlags
#define SET_WARNING_INFO(info, len, flags) (warningInfoText = info, warningInfoLength = len, warningInfoFlags = flags)

#define NAVIGATION_MENUS
#define POPUP_MENU_ADD_ITEM(s)         do { popupMenuOffsetType = MENU_OFFSET_INTERNAL; if (popupMenuNoItems < POPUP_MENU_MAX_LINES) popupMenuItems[popupMenuNoItems++] = s; } while (0)
#define POPUP_MENU_MAX_LINES           12
#define MENU_MAX_DISPLAY_LINES         6
#define POPUP_MENU_ADD_SD_ITEM(s)      POPUP_MENU_ADD_ITEM(s)
#define MENU_LINE_LENGTH               (LEN_MODEL_NAME+12)
#define POPUP_MENU_ITEMS_FROM_BSS()
extern const char *popupMenuItems[POPUP_MENU_MAX_LINES];
extern uint16_t popupMenuNoItems;
extern uint16_t popupMenuOffset;
enum {
  MENU_OFFSET_INTERNAL,
  MENU_OFFSET_EXTERNAL
};
extern uint8_t popupMenuOffsetType;
const char * displayPopupMenu(uint8_t event);
extern void (*popupMenuHandler)(const char *result);

#define STATUS_LINE_LENGTH             32
extern char statusLineMsg[STATUS_LINE_LENGTH];
void showStatusLine();
void drawStatusLine();

#define TEXT_FILENAME_MAXLEN           40
extern char s_text_file[TEXT_FILENAME_MAXLEN];
void menuTextView(uint8_t event);
void pushMenuTextView(const char *filename);
void pushModelNotes();

void menuChannelsView(uint8_t event);

#define LABEL(...) (uint8_t)-1

#if defined(REV9E) && !defined(SIMU)
  #define KEY_UP                       KEY_MINUS
  #define KEY_DOWN                     KEY_PLUS
  #define KEY_RIGHT                    KEY_PLUS
  #define KEY_LEFT                     KEY_MINUS
  #define CURSOR_MOVED_LEFT(event)     (EVT_KEY_MASK(event) == KEY_LEFT)
  #define CURSOR_MOVED_RIGHT(event)    (EVT_KEY_MASK(event) == KEY_RIGHT)
#else
  #define KEY_UP                       KEY_PLUS
  #define KEY_DOWN                     KEY_MINUS
  #define KEY_RIGHT                    KEY_MINUS
  #define KEY_LEFT                     KEY_PLUS
  #define CURSOR_MOVED_LEFT(event)     (EVT_KEY_MASK(event) == KEY_LEFT)
  #define CURSOR_MOVED_RIGHT(event)    (EVT_KEY_MASK(event) == KEY_RIGHT)
#endif

#define REPEAT_LAST_CURSOR_MOVE()      { if (CURSOR_MOVED_LEFT(event) || CURSOR_MOVED_RIGHT(event)) putEvent(event); else menuHorizontalPosition = 0; }
#define MOVE_CURSOR_FROM_HERE()        if (menuHorizontalPosition > 0) REPEAT_LAST_CURSOR_MOVE()

#define MENU_FIRST_LINE_EDIT           (menuTab ? (MAXCOL((uint16_t)0) >= HIDDEN_ROW ? (MAXCOL((uint16_t)1) >= HIDDEN_ROW ? 2 : 1) : 0) : 0)
#define POS_HORZ_INIT(posVert)         ((COLATTR(posVert) & NAVIGATION_LINE_BY_LINE) ? -1 : 0)
#define EDIT_MODE_INIT                 0 // TODO enum

typedef int16_t (*FnFuncP) (int16_t x);
void drawFunction(FnFuncP fn, uint8_t offset=0);

void onSourceLongEnterPress(const char *result);

uint8_t switchToMix(uint8_t source);

#endif // _MENUS_H_
