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

#include "../../keys.h"

#define NO_HI_LEN  25

#if defined(TRANSLATIONS_FR)
  #define MENU_COLUMNS         1
  #define COLUMN_X             0
#else
  #define MENU_COLUMNS         2
#endif

#define MENU_COLUMN2_X         280

typedef int16_t vertpos_t;

typedef evt_t & check_event_t;
#define horzpos_t int8_t

extern uint8_t menuPageIndex;
extern uint8_t menuPageCount;
extern uint16_t linesCount;
extern vertpos_t m_posVert;
extern horzpos_t m_posHorz;
extern vertpos_t s_pgOfs;
extern uint8_t s_noHi;
extern uint8_t calibrationState;

void drawCheckBox(coord_t x, coord_t y, uint8_t value, LcdFlags attr);

typedef bool (*MenuFuncP)(evt_t event);
extern const MenuFuncP menuTabModel[];
extern const MenuFuncP menuTabGeneral[];

extern MenuFuncP g_menuStack[5];
extern uint8_t g_menuPos[4];
extern uint8_t g_menuStackPtr;
extern evt_t menuEvent;

/// goto given Menu, but substitute current menu in menuStack
void chainMenu(MenuFuncP newMenu);
/// goto given Menu, store current menu in menuStack
void pushMenu(MenuFuncP newMenu);
/// return to last menu in menustack
void popMenu();

bool menuFirstCalib(evt_t event);

bool menuMainView(evt_t event);
bool menuGeneralDiagAna(evt_t event);
bool menuGeneralSetup(evt_t event);
bool menuGeneralCalib(evt_t event);
bool menuCustomFunctions(evt_t event, CustomFunctionData * functions, CustomFunctionsContext & functionsContext);
bool menuModelSelect(evt_t event);
bool menuModelSetup(evt_t event);
bool menuModelCustomFunctions(evt_t event);
bool menuStatisticsView(evt_t event);
bool menuStatisticsDebug(evt_t event);
bool menuAboutView(evt_t event);
bool menuMainViewChannelsMonitor(evt_t event);
bool menuChannelsView(evt_t event);
bool menuChannelsView(evt_t event);
bool menuTextView(evt_t event);

#if defined(DEBUG_TRACE_BUFFER)
void menuTraceBuffer(evt_t event);
#endif

extern int8_t checkIncDec_Ret;  // global helper vars

#define EDIT_SELECT_MENU   -1
#define EDIT_SELECT_FIELD  0
#define EDIT_MODIFY_FIELD  1
#define EDIT_MODIFY_STRING 2
extern int8_t s_editMode;       // global editmode

// checkIncDec flags
#define EE_GENERAL      0x01
#define EE_MODEL        0x02
#define NO_INCDEC_MARKS 0x04
#define INCDEC_SWITCH   0x08
#define INCDEC_SOURCE   0x10
#define INCDEC_REP10    0x40
#define NO_DBLKEYS      0x80

// mawrow special values
#define TITLE_ROW      ((uint8_t)-1)
#define HIDDEN_ROW     ((uint8_t)-2)

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
int8_t checkIncDecMovedSwitch(int8_t val);
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
  var = checkIncDec(event, var,min,max,EE_MODEL|INCDEC_SOURCE|NO_INCDEC_MARKS, isSourceAvailable)

#define CHECK_INCDEC_GENVAR(event, var, min, max) \
  var = checkIncDecGen(event, var, min, max)

#define NAVIGATION_LINE_BY_LINE  0x40
#define CURSOR_ON_LINE()         (m_posHorz<0)

#define CHECK_FLAG_NO_SCREEN_INDEX   1
bool navigate(evt_t event, int count, int rows, int columns=1);
bool check(check_event_t event, uint8_t curr, const MenuFuncP * menuTab, uint8_t menuTabSize, const pm_uint8_t *horTab, uint8_t horTabMax, vertpos_t maxrow, uint8_t flags=0);
bool check_simple(check_event_t event, uint8_t curr, const MenuFuncP * menuTab, uint8_t menuTabSize, vertpos_t maxrow);
bool check_submenu_simple(check_event_t event, uint8_t maxrow);

#define MENU_TAB(...) const uint8_t mstate_tab[] = __VA_ARGS__

void drawShadow(coord_t x, coord_t y, coord_t w, coord_t h);
void drawScreenTemplate(const char * title);
void drawMenuTemplate(const char * title, uint16_t scrollbar_X=0);
void drawSlider(coord_t x, coord_t y, uint8_t value, uint8_t max, uint8_t attr);

#define MENU(title, tab, menu, lines_count, scrollbar_X, ...) \
  MENU_TAB(__VA_ARGS__); \
  if (event == EVT_ENTRY || event == EVT_ENTRY_UP) TRACE("Menu %s displayed ...", title); \
  if (!check(event, menu, tab, DIM(tab), mstate_tab, DIM(mstate_tab)-1, lines_count)) return false; \
  drawMenuTemplate(title, scrollbar_X); \

#define SIMPLE_MENU(title, tab, menu, lines_count, scrollbar_X) \
  if (event == EVT_ENTRY || event == EVT_ENTRY_UP) TRACE("Menu %s displayed ...", title); \
  if (!check_simple(event, menu, tab, DIM(tab), lines_count)) return false; \
  drawMenuTemplate(title, scrollbar_X); \

#define SUBMENU(title, lines_count, scrollbar_X, ...) \
  MENU_TAB(__VA_ARGS__); \
  if (!check(event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, lines_count)) return false; \
  drawSubmenuTemplate(title, scrollbar_X);

#define SIMPLE_SUBMENU_NOTITLE(lines_count) \
  if (!check_submenu_simple(event, lines_count)) return false

#define SIMPLE_SUBMENU(title, lines_count, scrollbar_X) \
  SIMPLE_SUBMENU_NOTITLE(lines_count); \
  drawSubmenuTemplate(title, scrollbar_X)

typedef int select_menu_value_t;

select_menu_value_t selectMenuItem(coord_t x, coord_t y, const pm_char * values, select_menu_value_t value, select_menu_value_t min, select_menu_value_t max, LcdFlags attr, evt_t event);
uint8_t onoffMenuItem(uint8_t value, coord_t x, coord_t y, LcdFlags attr, evt_t event);
int8_t switchMenuItem(coord_t x, coord_t y, int8_t value, LcdFlags attr, evt_t event);

#if defined(GVARS)
  #define GVAR_MENU_ITEM(x, y, v, min, max, lcdattr, editflags, event) gvarMenuItem(x, y, v, min, max, lcdattr, editflags, event)
#else
  #define GVAR_MENU_ITEM(x, y, v, min, max, lcdattr, editflags, event) gvarMenuItem(x, y, v, min, max, lcdattr, event)
#endif

#if defined(GVARS)
  int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t editflags, evt_t event);
  #define displayGVar(x, y, v, min, max) GVAR_MENU_ITEM(x, y, v, min, max, 0, 0, 0)
#else
  int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, evt_t event);
  #define displayGVar(x, y, v, min, max) lcdDrawNumber(x, y, v)
#endif

void editName(coord_t x, coord_t y, char *name, uint8_t size, evt_t event, uint8_t active, LcdFlags flags=ZCHAR);

#define WARNING_TYPE_ASTERISK  0
#define WARNING_TYPE_CONFIRM   1
#define WARNING_TYPE_INPUT     2

extern const pm_char * s_warning;
extern const pm_char * s_warning_info;
extern uint8_t         s_warning_info_len;
extern uint8_t         s_warning_result;
extern uint8_t         s_warning_type;

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

void displayBox();
void displayPopup(const char *title);
void displayWarning(evt_t event);

extern void (*popupFunc)(evt_t event);
extern int16_t s_warning_input_value;
extern int16_t s_warning_input_min;
extern int16_t s_warning_input_max;
extern uint8_t s_warning_info_flags;

#define DISPLAY_WARNING       (*popupFunc)
#define POPUP_WARNING(s)      (s_warning = s, s_warning_info = 0, popupFunc = displayWarning)
#define POPUP_CONFIRMATION(s) (s_warning = s, s_warning_type = WARNING_TYPE_CONFIRM, s_warning_info = 0, popupFunc = displayWarning)
#define POPUP_INPUT(s, func, start, min, max) (s_warning = s, s_warning_type = WARNING_TYPE_INPUT, popupFunc = func, s_warning_input_value = start, s_warning_input_min = min, s_warning_input_max = max)
#define WARNING_INFO_FLAGS    s_warning_info_flags
#define SET_WARNING_INFO(info, len, flags) (s_warning_info = info, s_warning_info_len = len, s_warning_info_flags = flags)

#define NAVIGATION_MENUS
#define MENU_ADD_ITEM(s) s_menu[s_menu_count++] = s
#define MENU_MAX_LINES 6
#define MENU_MAX_DISPLAY_LINES 6
#define MENU_ADD_SD_ITEM(s) MENU_ADD_ITEM(s)
#define MENU_LINE_LENGTH (LEN_MODEL_NAME+12)
extern const char *s_menu[MENU_MAX_LINES];
extern uint16_t s_menu_count;
extern uint8_t s_menu_flags;
extern uint16_t s_menu_offset;
const char * displayMenu(evt_t event);
extern void (*menuHandler)(const char *result);

#define TEXT_FILENAME_MAXLEN  40
extern char s_text_file[TEXT_FILENAME_MAXLEN];
void pushMenuTextView(const char *filename);
void pushModelNotes();

#define LABEL(...) (uint8_t)-1

#define CURSOR_MOVED_LEFT(event)       (event==EVT_ROTARY_LEFT || EVT_KEY_MASK(event) == KEY_LEFT)
#define CURSOR_MOVED_RIGHT(event)      (event==EVT_ROTARY_RIGHT || EVT_KEY_MASK(event) == KEY_RIGHT)

#define REPEAT_LAST_CURSOR_MOVE()      { if (CURSOR_MOVED_LEFT(event) || CURSOR_MOVED_RIGHT(event)) putEvent(event); else m_posHorz = 0; }
#define MOVE_CURSOR_FROM_HERE()        if (m_posHorz > 0) REPEAT_LAST_CURSOR_MOVE()

#define MENU_FIRST_LINE_EDIT           (MAXCOL((uint16_t)0) >= HIDDEN_ROW ? (MAXCOL((uint16_t)1) >= HIDDEN_ROW ? 2 : 1) : 0)
#define POS_HORZ_INIT(posVert)         ((COLATTR(posVert) & NAVIGATION_LINE_BY_LINE) ? -1 : 0)
#define EDIT_MODE_INIT                 0 // TODO enum

typedef int (*FnFuncP) (int x);
void drawFunction(FnFuncP fn, int offset=0);

#endif // _MENUS_H_
