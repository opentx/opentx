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

#include "keys.h"

#define MENU_COLUMN2_X         280
#define MIXES_2ND_COLUMN       140
#define COLUMN_HEADER_X        150

extern uint8_t menuPageIndex;
extern uint8_t menuPageCount;
extern uint16_t linesCount;
extern uint8_t linesDisplayed;
extern int menuVerticalPosition;
extern int menuHorizontalPosition;
extern int menuVerticalOffset;
extern uint8_t menuCalibrationState;

// Temporary no highlight
extern uint8_t noHighlightCounter;
#define NO_HIGHLIGHT()        (noHighlightCounter > 0)
#define START_NO_HIGHLIGHT()  do { noHighlightCounter = 25; } while(0)

typedef bool (*MenuHandlerFunc)(event_t event);

extern MenuHandlerFunc menuHandlers[5];
extern uint8_t menuVerticalPositions[4];
extern uint8_t menuLevel;
extern event_t menuEvent;

/// goto given Menu, but substitute current menu in menuStack
void chainMenu(MenuHandlerFunc newMenu);
/// goto given Menu, store current menu in menuStack
void pushMenu(MenuHandlerFunc newMenu);
/// return to last menu in menuStack
void popMenu();

enum MenuIcons {
  ICON_OPENTX,
  ICON_RADIO,
  ICON_RADIO_SETUP,
  ICON_RADIO_SD_BROWSER,
  ICON_RADIO_GLOBAL_FUNCTIONS,
  ICON_RADIO_TRAINER,
  ICON_RADIO_HARDWARE,
  ICON_RADIO_CALIBRATION,
  ICON_RADIO_VERSION,
  ICON_MODEL,
  ICON_MODEL_SETUP,
  ICON_MODEL_HELI,
  ICON_MODEL_FLIGHT_MODES,
  ICON_MODEL_INPUTS,
  ICON_MODEL_MIXER,
  ICON_MODEL_OUTPUTS,
  ICON_MODEL_CURVES,
  ICON_MODEL_GVARS,
  ICON_MODEL_LOGICAL_SWITCHES,
  ICON_MODEL_SPECIAL_FUNCTIONS,
  ICON_MODEL_LUA_SCRIPTS,
  ICON_MODEL_TELEMETRY,
  ICON_THEME,
  ICON_THEME_SETUP,
  ICON_THEME_VIEW1,
  ICON_THEME_VIEW2,
  ICON_THEME_VIEW3,
  ICON_THEME_VIEW4,
  ICON_THEME_VIEW5,
  ICON_THEME_ADD_VIEW,
  ICON_STATS,
  ICON_STATS_THROTTLE_GRAPH,
  ICON_STATS_TIMERS,
  ICON_STATS_ANALOGS,
  ICON_STATS_DEBUG,
  ICON_MONITOR,
  ICON_MONITOR_CHANNELS1,
  ICON_MONITOR_CHANNELS2,
  ICON_MONITOR_CHANNELS3,
  ICON_MONITOR_CHANNELS4,
  ICON_MONITOR_LOGICAL_SWITCHES,
  MENUS_ICONS_COUNT
};

enum EnumTabModel {
  MENU_MODEL_SETUP,
  CASE_HELI(MENU_MODEL_HELI)
  CASE_FLIGHT_MODES(MENU_MODEL_FLIGHT_MODES)
  MENU_MODEL_INPUTS,
  MENU_MODEL_MIXES,
  MENU_MODEL_OUTPUTS,
  CASE_CURVES(MENU_MODEL_CURVES)
  CASE_GVARS(MENU_MODEL_GVARS)
  MENU_MODEL_LOGICAL_SWITCHES,
  MENU_MODEL_SPECIAL_FUNCTIONS,
#if defined(LUA_MODEL_SCRIPTS)
  MENU_MODEL_CUSTOM_SCRIPTS,
#endif
  CASE_FRSKY(MENU_MODEL_TELEMETRY_FRSKY)
  MENU_MODEL_PAGES_COUNT
};

const uint8_t RADIO_ICONS[] = {
  ICON_RADIO,
  ICON_RADIO_SETUP,
  ICON_RADIO_SD_BROWSER,
  ICON_RADIO_GLOBAL_FUNCTIONS,
  ICON_RADIO_TRAINER,
  ICON_RADIO_HARDWARE,
  ICON_RADIO_VERSION
};

const uint8_t MODEL_ICONS[] = {
  ICON_MODEL,
  ICON_MODEL_SETUP,
  CASE_HELI(ICON_MODEL_HELI)
  CASE_FLIGHT_MODES(ICON_MODEL_FLIGHT_MODES)
  ICON_MODEL_INPUTS,
  ICON_MODEL_MIXER,
  ICON_MODEL_OUTPUTS,
  CASE_CURVES(ICON_MODEL_CURVES)
  CASE_GVARS(ICON_MODEL_GVARS)
  ICON_MODEL_LOGICAL_SWITCHES,
  ICON_MODEL_SPECIAL_FUNCTIONS,
#if defined(LUA_MODEL_SCRIPTS)
  ICON_MODEL_LUA_SCRIPTS,
#endif
  ICON_MODEL_TELEMETRY
};

const uint8_t STATS_ICONS[] = {
  ICON_STATS,
  ICON_STATS_THROTTLE_GRAPH,
  ICON_STATS_DEBUG,
  ICON_STATS_ANALOGS,
#if defined(DEBUG_TRACE_BUFFER)
  ICON_STATS_TIMERS
#endif
};

const uint8_t MONITOR_ICONS[] = {
  ICON_MONITOR,
  ICON_MONITOR_CHANNELS1,
  ICON_MONITOR_CHANNELS2,
  ICON_MONITOR_CHANNELS3,
  ICON_MONITOR_CHANNELS4,
  ICON_MONITOR_LOGICAL_SWITCHES
};

bool menuModelSetup(event_t event);
bool menuModelFailsafe(event_t event);
bool menuModelHeli(event_t event);
bool menuModelFlightModesAll(event_t event);
bool menuModelExposAll(event_t event);
bool menuModelMixAll(event_t event);
bool menuModelLimits(event_t event);
bool menuModelCurvesAll(event_t event);
bool menuModelCurveOne(event_t event);
bool menuModelGVars(event_t event);
bool menuModelLogicalSwitches(event_t event);
bool menuModelSpecialFunctions(event_t event);
bool menuModelCustomScripts(event_t event);
bool menuModelTelemetryFrsky(event_t event);
bool menuModelExpoOne(event_t event);

extern const MenuHandlerFunc menuTabModel[MENU_MODEL_PAGES_COUNT];

enum EnumTabRadio {
  MENU_RADIO_SETUP,
  MENU_RADIO_SD_MANAGER,
  MENU_RADIO_SPECIAL_FUNCTIONS,
  MENU_RADIO_TRAINER,
  MENU_RADIO_HARDWARE,
  MENU_RADIO_VERSION,
  MENU_RADIO_PAGES_COUNT
};

bool menuRadioSetup(event_t event);
bool menuRadioSdManager(event_t event);
bool menuRadioSpecialFunctions(event_t event);
bool menuRadioTrainer(event_t event);
bool menuRadioVersion(event_t event);
bool menuRadioHardware(event_t event);
bool menuRadioCalibration(event_t event);

extern const MenuHandlerFunc menuTabGeneral[MENU_RADIO_PAGES_COUNT];

enum MenuRadioIndexes
{
  e_StatsGraph,
  e_StatsDebug,
  e_StatsAnalogs,
#if defined(DEBUG_TRACE_BUFFER)
  e_StatsTraces,
#endif
};

bool menuStatsGraph(event_t event);
bool menuStatsDebug(event_t event);
bool menuStatsAnalogs(event_t event);
bool menuStatsTraces(event_t event);

static const MenuHandlerFunc menuTabStats[] PROGMEM = {
  menuStatsGraph,
  menuStatsDebug,
  menuStatsAnalogs,
#if defined(DEBUG_TRACE_BUFFER)
  menuStatsTraces,
#endif
};

enum EnumTabMonitors
{
  e_MonChannelsFirst,
  e_MonChannels1 = e_MonChannelsFirst,
  e_MonChannels2,
  e_MonChannels3,
  e_MonChannels4,
  e_MonLogicalSwitches,
  e_MonTabChannelsPagesCount
};

extern const MenuHandlerFunc menuTabMonitors[e_MonTabChannelsPagesCount];
extern uint8_t lastMonitorPage;
extern void drawSingleMixerBar(coord_t, coord_t, coord_t, coord_t, uint8_t);
extern void drawSingleOutputBar(coord_t, coord_t, coord_t, coord_t, uint8_t);

extern const MenuHandlerFunc menuTabScreensSetup[1+MAX_CUSTOM_SCREENS] PROGMEM;

bool menuFirstCalib(event_t event);
bool menuMainView(event_t event);
bool menuSpecialFunctions(event_t event, CustomFunctionData * functions, CustomFunctionsContext & functionsContext);
bool menuModelSelect(event_t event);
bool menuAboutView(event_t event);
bool menuMainViewChannelsMonitor(event_t event);
bool menuTextView(event_t event);
bool menuScreensTheme(event_t event);

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

int checkIncDec(event_t event, int val, int i_min, int i_max, unsigned int i_flags=0, IsValueAvailable isValueAvailable=NULL, const CheckIncDecStops &stops=stops100);
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

int8_t navigate(event_t event, int count, int rows, int columns=1, bool loop=true);
bool check(event_t event, uint8_t curr, const MenuHandlerFunc * menuTab, uint8_t menuTabSize, const uint8_t * horTab, uint8_t horTabMax, int maxrow, uint8_t flags=0);
bool check_simple(event_t event, uint8_t curr, const MenuHandlerFunc * menuTab, uint8_t menuTabSize, int maxrow);
bool check_submenu_simple(event_t event, uint8_t maxrow);

#define MENU_TAB(...) const uint8_t mstate_tab[] = __VA_ARGS__

#define MENU_WITH_OPTIONS(title, icons, tab, tabCount, menu, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  if (event == EVT_ENTRY || event == EVT_ENTRY_UP) TRACE("Menu %s displayed ...", title); \
  if (!check(event, menu, tab, tabCount, mstate_tab, DIM(mstate_tab)-1, lines_count)) return false; \
  drawMenuTemplate(title, 0, icons, OPTION_MENU_TITLE_BAR);

#define CUSTOM_MENU_WITH_OPTIONS(title, icons, tab, tabCount, menu, lines_count) \
    if (event == EVT_ENTRY || event == EVT_ENTRY_UP) TRACE("Menu %s displayed ...", title); \
    if (!check(event, menu, tab, tabCount, mstate_tab, DIM(mstate_tab)-1, lines_count)) return false; \
    drawMenuTemplate(title, 0, icons, OPTION_MENU_TITLE_BAR);

#define MENU(title, icons, tab, menu, lines_count, ...) \
  MENU_WITH_OPTIONS(title, icons, tab, DIM(tab), menu, lines_count, __VA_ARGS__)

#define SIMPLE_MENU_WITH_OPTIONS(title, icons, tab, tabCount, menu, lines_count) \
  if (event == EVT_ENTRY || event == EVT_ENTRY_UP) TRACE("Menu %s displayed ...", title); \
  if (!check_simple(event, menu, tab, tabCount, lines_count)) return false; \
  drawMenuTemplate(title, 0, icons, OPTION_MENU_TITLE_BAR);

#define SIMPLE_MENU(title, icons, tab, menu, lines_count) \
  SIMPLE_MENU_WITH_OPTIONS(title, icons, tab, DIM(tab), menu, lines_count)

#define SUBMENU(title, icon, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  if (!check(event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, lines_count)) return false; \
  drawMenuTemplate(title, icon);

#define SUBMENU_WITH_OPTIONS(title, icon, lines_count, options, ...) \
  MENU_TAB(__VA_ARGS__); \
  if (!check(event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, lines_count)) return false; \
  drawMenuTemplate(title, icon, NULL, options);

#define CUSTOM_SUBMENU_WITH_OPTIONS(title, icon, lines_count, options) \
  if (!check(event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, lines_count)) return false; \
  drawMenuTemplate(title, icon, NULL, options);

#define SIMPLE_SUBMENU(title, icon, lines_count) \
  if (!check_submenu_simple(event, lines_count)) return false; \
  drawMenuTemplate(title, icon, NULL)

#define SIMPLE_SUBMENU_WITH_OPTIONS(title, icon, lines_count, options) \
  if (!check_submenu_simple(event, lines_count)) return false; \
  drawMenuTemplate(title, icon, NULL, options)

#if defined(GVARS)
  #define GVAR_MENU_ITEM(x, y, v, min, max, lcdattr, editflags, event) editGVarFieldValue(x, y, v, min, max, lcdattr, editflags, event)
#else
  #define GVAR_MENU_ITEM(x, y, v, min, max, lcdattr, editflags, event) editGVarFieldValue(x, y, v, min, max, lcdattr, event)
#endif

#if defined(GVARS)
  int16_t editGVarFieldValue(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t editflags, event_t event);
  #define displayGVar(x, y, v, min, max) GVAR_MENU_ITEM(x, y, v, min, max, 0, 0, 0)
#else
  int16_t editGVarFieldValue(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, event_t event);
  #define displayGVar(x, y, v, min, max) lcdDrawNumber(x, y, v)
#endif

extern uint8_t editNameCursorPos;
void editName(coord_t x, coord_t y, char *name, uint8_t size, event_t event, uint8_t active, LcdFlags flags=ZCHAR);

uint8_t editDelay(coord_t x, coord_t y, event_t event, uint8_t attr, uint8_t delay);
void editCurveRef(coord_t x, coord_t y, CurveRef & curve, event_t event, LcdFlags flags);

extern uint8_t s_curveChan;

#define WARNING_TYPE_ALERT     0
#define WARNING_TYPE_ASTERISK  1
#define WARNING_TYPE_CONFIRM   2
#define WARNING_TYPE_INPUT     3
#define WARNING_TYPE_INFO      4

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
#define POPUP_X                120
#define POPUP_Y                70
#define POPUP_W                300
#define POPUP_H                130
#define WARNING_LINE_LEN       32
#define WARNING_LINE_X         (POPUP_X+66)
#define WARNING_LINE_Y         (POPUP_Y+9)
#define WARNING_INFOLINE_Y     (WARNING_LINE_Y+68)

void copySelection(char * dst, const char * src, uint8_t size);

void showMessageBox(const char * title);
void runPopupWarning(event_t event);

extern void (* popupFunc)(event_t event);
extern int16_t warningInputValue;
extern int16_t warningInputValueMin;
extern int16_t warningInputValueMax;
extern uint8_t warningInfoFlags;

#define DISPLAY_WARNING                (*popupFunc)
#define POPUP_WARNING(s)               (warningType = WARNING_TYPE_ASTERISK, warningText = s, warningInfoText = 0, popupFunc = runPopupWarning)
#define POPUP_CONFIRMATION(s)          (warningText = s, warningType = WARNING_TYPE_CONFIRM, warningInfoText = 0, popupFunc = runPopupWarning)
#define POPUP_INPUT(s, func, start, min, max) (warningText = s, warningType = WARNING_TYPE_INPUT, popupFunc = func, warningInputValue = start, warningInputValueMin = min, warningInputValueMax = max)
#define WARNING_INFO_FLAGS             warningInfoFlags
#define SET_WARNING_INFO(info, len, flags)    (warningInfoText = info, warningInfoLength = len, warningInfoFlags = flags)

#define NAVIGATION_MENUS
#define POPUP_MENU_ADD_ITEM(s)         do { popupMenuOffsetType = MENU_OFFSET_INTERNAL; if (popupMenuNoItems < POPUP_MENU_MAX_LINES) popupMenuItems[popupMenuNoItems++] = s; } while (0)
#define POPUP_MENU_SELECT_ITEM(s)      s_menu_item =  (s > 0 ? (s < popupMenuNoItems ? s : popupMenuNoItems) : 0)
#define POPUP_MENU_START(func)         do { popupMenuHandler = (func); AUDIO_KEY_PRESS(); } while(0)
#define POPUP_MENU_MAX_LINES           12
#define MENU_MAX_DISPLAY_LINES         9
#define MENU_LINE_LENGTH               (LEN_MODEL_NAME+12)
#define POPUP_MENU_SET_BSS_FLAG()
extern const char * popupMenuItems[POPUP_MENU_MAX_LINES];
extern uint16_t popupMenuNoItems;
extern uint16_t popupMenuOffset;
enum {
  MENU_OFFSET_INTERNAL,
  MENU_OFFSET_EXTERNAL
};
extern uint8_t popupMenuOffsetType;
extern uint8_t s_menu_item;
const char * runPopupMenu(event_t event);
extern void (*popupMenuHandler)(const char * result);

#define TEXT_FILENAME_MAXLEN           40
extern char s_text_file[TEXT_FILENAME_MAXLEN];
void pushMenuTextView(const char * filename);
void pushModelNotes();
void readModelNotes();

#define LABEL(...)                     (uint8_t)-1

#define CURSOR_MOVED_LEFT(event)       (event==EVT_ROTARY_LEFT || EVT_KEY_MASK(event) == KEY_LEFT)
#define CURSOR_MOVED_RIGHT(event)      (event==EVT_ROTARY_RIGHT || EVT_KEY_MASK(event) == KEY_RIGHT)
#define REPEAT_LAST_CURSOR_MOVE(last, refresh) { if (CURSOR_MOVED_RIGHT(event)) menuHorizontalPosition = (menuHorizontalPosition >= (last) ? 0 : menuHorizontalPosition + 1); else menuHorizontalPosition = (menuHorizontalPosition <= (0) ? last : menuHorizontalPosition - 1); if (refresh) putEvent(EVT_REFRESH); }

#define MENU_FIRST_LINE_EDIT           (MAXCOL((uint16_t)0) >= HIDDEN_ROW ? (MAXCOL((uint16_t)1) >= HIDDEN_ROW ? 2 : 1) : 0)
#define POS_HORZ_INIT(posVert)         ((COLATTR(posVert) & NAVIGATION_LINE_BY_LINE) ? -1 : 0)
#define EDIT_MODE_INIT                 0 // TODO enum

typedef int (*FnFuncP) (int x);
void drawFunction(FnFuncP fn, int x, int y, int width);

void onSourceLongEnterPress(const char *result);

uint8_t switchToMix(uint8_t source);

#endif // _MENUS_H_
