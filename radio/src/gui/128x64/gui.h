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

#ifndef _GUI_H_
#define _GUI_H_

#include "gui_common.h"
#include "menus.h"
#include "popups.h"

#define MENUS_SCROLLBAR_WIDTH          0

#if defined(PCBX7)
  #define HEADER_LINE                  0
  #define HEADER_LINE_COLUMNS
#else
  #define HEADER_LINE                  1
  #define HEADER_LINE_COLUMNS          0,
#endif

#define COLUMN_X                       0
#define drawFieldLabel(x, y, str)      lcdDrawTextAlignedLeft(y, str)

#define NUM_BODY_LINES                 (LCD_LINES-1)
#define MENU_HEADER_HEIGHT             FH
#define MENU_INIT_VPOS                 0

#define CURVE_SIDE_WIDTH               (LCD_H/2)
#define CURVE_CENTER_X                 (LCD_W-CURVE_SIDE_WIDTH-2)
#define CURVE_CENTER_Y                 (LCD_H/2)

#define MIXES_2ND_COLUMN               (12*FW)

// Temporary no highlight
extern uint8_t noHighlightCounter;
#define NO_HIGHLIGHT()        (noHighlightCounter > 0)
#define START_NO_HIGHLIGHT()  do { noHighlightCounter = 25; } while(0)


#if !defined(CPUM64)
void drawSlider(coord_t x, coord_t y, uint8_t value, uint8_t max, uint8_t attr);
#elif defined(GRAPHICS)
void display5posSlider(coord_t x, coord_t y, uint8_t value, uint8_t attr);
  #define drawSlider(x, y, value, max, attr) lcdDrawNumber(x, y, value, attr|LEFT)
#else
  #define drawSlider(x, y, value, max, attr) lcdDrawNumber(x, y, value, attr|LEFT)
#endif

#if defined(NAVIGATION_POT1)
extern int16_t p1valdiff;
#else
  #define p1valdiff 0
#endif

#if defined(NAVIGATION_POT2)
extern int8_t p2valdiff;
#else
  #define p2valdiff 0
#endif

extern int8_t checkIncDec_Ret;  // global helper vars

#define EDIT_SELECT_MENU               -1
#define EDIT_SELECT_FIELD              0
#define EDIT_MODIFY_FIELD              1
#define EDIT_MODIFY_STRING             2
extern int8_t s_editMode;       // global editmode

// checkIncDec flags
#define EE_GENERAL                     0x01
#define EE_MODEL                       0x02
#define NO_INCDEC_MARKS                0x04
#define INCDEC_SWITCH                  0x08
#define INCDEC_SOURCE                  0x10
#define INCDEC_REP10                   0x40
#define NO_DBLKEYS                     0x80

#if defined(CPUARM)
  #define INCDEC_DECLARE_VARS(f)       uint8_t incdecFlag = (f); IsValueAvailable isValueAvailable = NULL
  #define INCDEC_SET_FLAG(f)           incdecFlag = (f)
  #define INCDEC_ENABLE_CHECK(fn)      isValueAvailable = fn
  #define CHECK_INCDEC_PARAM(event, var, min, max) checkIncDec(event, var, min, max, incdecFlag, isValueAvailable)
#elif defined(CPUM64)
#define INCDEC_DECLARE_VARS(f)
  #define INCDEC_SET_FLAG(f)
  #define INCDEC_ENABLE_CHECK(fn)
  #define CHECK_INCDEC_PARAM(event, var, min, max) checkIncDec(event, var, min, max, EE_MODEL)
#else
  #define INCDEC_DECLARE_VARS(f)       uint8_t incdecFlag = (f)
  #define INCDEC_SET_FLAG(f)           incdecFlag = (f)
  #define INCDEC_ENABLE_CHECK(fn)
  #define CHECK_INCDEC_PARAM(event, var, min, max) checkIncDec(event, var, min, max, incdecFlag)
#endif

// mawrow special values
#define TITLE_ROW      ((uint8_t)-1)
#define HIDDEN_ROW     ((uint8_t)-2)

#if defined(CPUARM)
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
#else
int16_t checkIncDec(event_t event, int16_t i_pval, int16_t i_min, int16_t i_max, uint8_t i_flags=0);
#endif

#if defined(CPUM64)
int8_t checkIncDecModel(event_t event, int8_t i_val, int8_t i_min, int8_t i_max);
int8_t checkIncDecModelZero(event_t event, int8_t i_val, int8_t i_max);
int8_t checkIncDecGen(event_t event, int8_t i_val, int8_t i_min, int8_t i_max);
#else
#define checkIncDecModel(event, i_val, i_min, i_max) checkIncDec(event, i_val, i_min, i_max, EE_MODEL)
#define checkIncDecModelZero(event, i_val, i_max) checkIncDec(event, i_val, 0, i_max, EE_MODEL)
#define checkIncDecGen(event, i_val, i_min, i_max) checkIncDec(event, i_val, i_min, i_max, EE_GENERAL)
#endif

#define CHECK_INCDEC_MODELVAR(event, var, min, max) \
  var = checkIncDecModel(event, var, min, max)

#define CHECK_INCDEC_MODELVAR_ZERO(event, var, max) \
  var = checkIncDecModelZero(event, var, max)

#if defined(CPUARM)
#define CHECK_INCDEC_MODELVAR_CHECK(event, var, min, max, check) \
    var = checkIncDec(event, var, min, max, EE_MODEL, check)
  #define CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, var, max, check) \
    var = checkIncDec(event, var, 0, max, EE_MODEL, check)
#else
  #define CHECK_INCDEC_MODELVAR_CHECK(event, var, min, max, check) \
    var = checkIncDec(event, var, min, max, EE_MODEL)
  #define CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, var, max, check) \
    CHECK_INCDEC_MODELVAR_ZERO(event, var, max)
#endif

#if defined(CPUARM)
#define AUTOSWITCH_ENTER_LONG() (attr && event==EVT_KEY_LONG(KEY_ENTER))
  #define CHECK_INCDEC_SWITCH(event, var, min, max, flags, available) \
    var = checkIncDec(event, var, min, max, (flags)|INCDEC_SWITCH, available)
  #define CHECK_INCDEC_MODELSWITCH(event, var, min, max, available) \
    CHECK_INCDEC_SWITCH(event, var, min, max, EE_MODEL, available)
#elif defined(AUTOSWITCH)
#define AUTOSWITCH_ENTER_LONG() (attr && event==EVT_KEY_LONG(KEY_ENTER))
  #define CHECK_INCDEC_SWITCH(event, var, min, max, flags, available) \
    var = checkIncDec(event, var, min, max, (flags)|INCDEC_SWITCH)
  #define CHECK_INCDEC_MODELSWITCH(event, var, min, max, available) \
    CHECK_INCDEC_SWITCH(event, var, min, max, EE_MODEL, available)
#else
  #define AUTOSWITCH_ENTER_LONG() (0)
  #define CHECK_INCDEC_SWITCH(event, var, min, max, flags, available) \
    CHECK_INCDEC_MODELVAR(event, var, min, max)
  #define CHECK_INCDEC_MODELSWITCH(event, var, min, max, available) \
    CHECK_INCDEC_MODELVAR(event, var, min, max)
#endif

#if defined(CPUARM)
#define CHECK_INCDEC_MODELSOURCE(event, var, min, max) \
    var = checkIncDec(event,var,min,max,EE_MODEL|INCDEC_SOURCE|NO_INCDEC_MARKS, isSourceAvailable)
#elif defined(AUTOSOURCE)
#define CHECK_INCDEC_MODELSOURCE(event, var, min, max) \
    var = checkIncDec(event,var,min,max,EE_MODEL|INCDEC_SOURCE|NO_INCDEC_MARKS)
#else
  #define CHECK_INCDEC_MODELSOURCE CHECK_INCDEC_MODELVAR
#endif

#define CHECK_INCDEC_GENVAR(event, var, min, max) \
  var = checkIncDecGen(event, var, min, max)

#if defined(PCBTARANIS)
#define CURSOR_ON_LINE()               (menuHorizontalPosition < 0)
#else
#define CURSOR_ON_LINE()               (0)
#endif

void check(event_t event, uint8_t curr, const MenuHandlerFunc *menuTab, uint8_t menuTabSize, const pm_uint8_t *horTab, uint8_t horTabMax, vertpos_t maxrow);
void check_simple(event_t event, uint8_t curr, const MenuHandlerFunc *menuTab, uint8_t menuTabSize, vertpos_t maxrow);
void check_submenu_simple(event_t event, uint8_t maxrow);

void title(const pm_char * s);
#define TITLE(str) title(str)

#if defined(CPUARM)
  #define MENU_TAB(...) const uint8_t mstate_tab[] = __VA_ARGS__
#else
  #define MENU_TAB(...) static const pm_uint8_t mstate_tab[] PROGMEM = __VA_ARGS__
#endif

#if defined(PCBX7)
#define MENU_CHECK(tab, menu, lines_count) \
  check(event, menu, tab, DIM(tab), mstate_tab, DIM(mstate_tab)-1, lines_count)
#else
#define MENU_CHECK(tab, menu, lines_count) \
  check(event, menu, tab, DIM(tab), mstate_tab, DIM(mstate_tab)-1, (lines_count)-1)
#endif

#define MENU(title, tab, menu, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  MENU_CHECK(tab, menu, lines_count); \
  TITLE(title)

#if defined(PCBX7)
#define SIMPLE_MENU_NOTITLE(tab, menu, lines_count) \
  check_simple(event, menu, tab, DIM(tab), lines_count);
#define SUBMENU_NOTITLE(lines_count, ...) { \
  MENU_TAB(__VA_ARGS__); \
  check(event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, lines_count); \
  }
#define SUBMENU(title, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  check(event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, lines_count); \
  TITLE(title)
#define SIMPLE_SUBMENU_NOTITLE(lines_count) \
  check_submenu_simple(event, lines_count);
#else
#define SIMPLE_MENU_NOTITLE(tab, menu, lines_count) \
  check_simple(event, menu, tab, DIM(tab), (lines_count)-1);
#define SUBMENU_NOTITLE(lines_count, ...) { \
  MENU_TAB(__VA_ARGS__); \
  check(event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, (lines_count)-1); \
  }

#define SUBMENU(title, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  check(event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, (lines_count)-1); \
  TITLE(title)

#define SIMPLE_SUBMENU_NOTITLE(lines_count) \
  check_submenu_simple(event, (lines_count)-1);



#endif

#define SIMPLE_MENU(title, tab, menu, lines_count) \
  SIMPLE_MENU_NOTITLE(tab, menu, lines_count); \
  TITLE(title)

#define SIMPLE_SUBMENU(title, lines_count) \
  SIMPLE_SUBMENU_NOTITLE(lines_count); \
  TITLE(title)

#if defined(CPUARM)
typedef int choice_t;
#else
typedef int8_t choice_t;
#endif

choice_t editChoice(coord_t x, coord_t y, const pm_char * label, const pm_char *values, choice_t value, choice_t min, choice_t max, LcdFlags attr, event_t event);
uint8_t editCheckBox(uint8_t value, coord_t x, coord_t y, const pm_char * label, LcdFlags attr, event_t event);
int8_t editSwitch(coord_t x, coord_t y, int8_t value, LcdFlags attr, event_t event);

#define ON_OFF_MENU_ITEM(value, x, y, label, attr, event) value = editCheckBox(value, x, y, label, attr, event)

#if defined(CPUARM) && defined(GVARS)
  #define GVAR_MENU_ITEM(x, y, v, min, max, attr, editflags, event) editGVarFieldValue(x, y, v, min, max, attr, editflags, event)
#else
  #define GVAR_MENU_ITEM(x, y, v, min, max, attr, editflags, event) editGVarFieldValue(x, y, v, min, max, attr, event)
#endif

#if defined(GVARS) && defined(CPUARM)
int16_t editGVarFieldValue(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t editflags, event_t event);
void drawGVarName(coord_t x, coord_t y, int8_t index, LcdFlags flags=0);
void drawGVarValue(coord_t x, coord_t y, uint8_t gvar, gvar_t value, LcdFlags flags=0);
void editGVarValue(coord_t x, coord_t y, event_t event, uint8_t gvar, uint8_t flightMode, LcdFlags flags);
#elif defined(GVARS)
int16_t editGVarFieldValue(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, event_t event);
#else
int16_t editGVarFieldValue(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, event_t event);
#endif

void gvarWeightItem(coord_t x, coord_t y, MixData * md, LcdFlags attr, event_t event);

#if defined(GVARS)
#define displayGVar(x, y, v, min, max) GVAR_MENU_ITEM(x, y, v, min, max, 0, 0, 0)
#else
#define displayGVar(x, y, v, min, max) lcdDraw8bitsNumber(x, y, v)
#endif

#if defined(CPUARM)
void editName(coord_t x, coord_t y, char * name, uint8_t size, event_t event, uint8_t active, LcdFlags attr=ZCHAR);
#else
void editName(coord_t x, coord_t y, char * name, uint8_t size, event_t event, uint8_t active);
#endif

#if defined(CPUM64)
#define editSingleName(x, y, label, name, size, event, active) editName(x, y, name, size, event, active)
#else
void editSingleName(coord_t x, coord_t y, const pm_char * label, char * name, uint8_t size, event_t event, uint8_t active);
#endif

uint8_t editDelay(coord_t y, event_t event, uint8_t attr, const pm_char * str, uint8_t delay);
#define EDIT_DELAY(x, y, event, attr, str, delay) editDelay(y, event, attr, str, delay)

#define WARNING_TYPE_ASTERISK          0
#define WARNING_TYPE_CONFIRM           1
#define WARNING_TYPE_INPUT             2

extern const pm_char * warningText;
extern const pm_char * warningInfoText;
extern uint8_t         warningInfoLength;
extern uint8_t         warningResult;
extern uint8_t         warningType;

#define COPY_MODE                      1
#define MOVE_MODE                      2
extern uint8_t s_copyMode;
extern int8_t s_copySrcRow;
extern int8_t s_copyTgtOfs;
extern uint8_t s_currIdx;
extern uint8_t s_curveChan;
extern uint8_t s_copySrcIdx;
extern uint8_t s_copySrcCh;
extern int8_t s_currCh;
extern uint8_t s_maxLines;

#if defined(SDCARD)
#define STATUS_LINE_LENGTH           32
extern char statusLineMsg[STATUS_LINE_LENGTH];
void showStatusLine();
void drawStatusLine();
#else
#define drawStatusLine()
#endif

#if defined(CPUARM)
#define TEXT_FILENAME_MAXLEN         40
  extern char s_text_file[TEXT_FILENAME_MAXLEN];
  void menuTextView(event_t event);
  void pushMenuTextView(const char *filename);
  void pushModelNotes();
  void readModelNotes();
#endif

#define LABEL(...)                     (uint8_t)-1

#define CURSOR_MOVED_LEFT(event)       (IS_ROTARY_LEFT(event) || EVT_KEY_MASK(event) == KEY_LEFT)
#define CURSOR_MOVED_RIGHT(event)      (IS_ROTARY_RIGHT(event) || EVT_KEY_MASK(event) == KEY_RIGHT)

#if defined(ROTARY_ENCODERS)
#define CASE_EVT_ROTARY_BREAK          case EVT_ROTARY_BREAK:
#define CASE_EVT_ROTARY_LONG           case EVT_ROTARY_LONG:
#else
#define CASE_EVT_ROTARY_BREAK
#define CASE_EVT_ROTARY_LONG
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
  #define IS_ROTARY_LEFT(evt)          (evt == EVT_ROTARY_LEFT)
  #define IS_ROTARY_RIGHT(evt)         (evt == EVT_ROTARY_RIGHT)
  #define IS_ROTARY_BREAK(evt)         (evt == EVT_ROTARY_BREAK)
  #define IS_ROTARY_LONG(evt)          (evt == EVT_ROTARY_LONG)
  #define IS_ROTARY_EVENT(evt)         (EVT_KEY_MASK(evt) >= 0x0e)
  void repeatLastCursorMove(event_t event);
  #define REPEAT_LAST_CURSOR_MOVE()    { if (EVT_KEY_MASK(event) >= 0x0e) putEvent(event); else repeatLastCursorMove(event); }
  #define MOVE_CURSOR_FROM_HERE()      if (menuHorizontalPosition > 0) REPEAT_LAST_CURSOR_MOVE()
#else
  #define IS_ROTARY_LEFT(evt)          (0)
  #define IS_ROTARY_RIGHT(evt)         (0)
  #define IS_ROTARY_BREAK(evt)         (0)
  #define IS_ROTARY_LONG(evt)          (0)
  #define IS_ROTARY_EVENT(evt)         (0)
  void repeatLastCursorMove(event_t event);
  #define REPEAT_LAST_CURSOR_MOVE()    repeatLastCursorMove(event)
  #define MOVE_CURSOR_FROM_HERE()      REPEAT_LAST_CURSOR_MOVE()
#endif

// TODO enum
#if defined(PCBX7)
#define EDIT_MODE_INIT                 0
#else
#define EDIT_MODE_INIT                 -1
#endif

#if defined(CPUM64)
  #define editNameCursorPos menuHorizontalPosition
#else
  extern uint8_t editNameCursorPos;
#endif

#if defined(VIRTUAL_INPUTS)
uint8_t getExposCount();
void insertExpo(uint8_t idx);
void deleteExpo(uint8_t idx);
uint8_t getMixesCount();
void insertMix(uint8_t idx);
void deleteMix(uint8_t idx);
#endif

typedef int (*FnFuncP) (int x);
void drawFunction(FnFuncP fn, uint8_t offset=0);

void onSourceLongEnterPress(const char *result);

uint8_t switchToMix(uint8_t source);

void deleteExpoMix(uint8_t expo, uint8_t idx);

void drawCheckBox(coord_t x, coord_t y, uint8_t value, LcdFlags attr);

extern const pm_uchar sticks[] PROGMEM;

void drawSplash();
void drawSecondSplash();
void drawScreenIndex(uint8_t index, uint8_t count, uint8_t attr);
void drawStick(coord_t centrex, int16_t xval, int16_t yval);
void drawPotsBars();
void doMainScreenGraphics();

void drawProgressBar(const char * label, int num, int den);
void drawSleepBitmap();

#if !defined(CPUM64)
void drawVerticalScrollbar(coord_t x, coord_t y, coord_t h, uint16_t offset, uint16_t count, uint8_t visible);
#endif

#if defined(PCBTARANIS)
void drawAlertBox(const pm_char * title, const pm_char * text, const char * action);
#endif

#if defined(VOICE)
 #define ALERT_SOUND_ARG , uint8_t sound
#else
 #define ALERT_SOUND_ARG
#endif

void showAlertBox(const pm_char * title, const pm_char * text, const char * action ALERT_SOUND_ARG);

#define SET_SCROLLBAR_X(x)
#define LOAD_MODEL_BITMAP()

#define IS_MAIN_VIEW_DISPLAYED()       menuHandlers[0] == menuMainView
#if defined(TELEMETRY_FRSKY)
#define IS_TELEMETRY_VIEW_DISPLAYED()  menuHandlers[0] == menuViewTelemetryFrsky
#else
#define IS_TELEMETRY_VIEW_DISPLAYED()  false
#endif
#define IS_OTHER_VIEW_DISPLAYED()      false

#if defined(CPUARM)
void editCurveRef(coord_t x, coord_t y, CurveRef & curve, event_t event, LcdFlags flags);
#endif

#if defined(FLIGHT_MODES)
void displayFlightModes(coord_t x, coord_t y, FlightModesType value);
FlightModesType editFlightModes(coord_t x, coord_t y, event_t event, FlightModesType value, uint8_t attr);
#else
#define displayFlightModes(...)
#endif

#endif // _GUI_H_
