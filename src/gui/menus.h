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

#ifndef menus_h
#define menus_h

#define NO_HI_LEN  25

void displayScreenIndex(uint8_t index, uint8_t count, uint8_t attr);
inline void displayColumnHeader(const char **headers, uint8_t index)
{
  lcd_putsAtt(17*FW, 0, headers[index], 0);
}

#if LCD_W >= 212
  #if defined(TRANSLATIONS_FR)
    #define MENU_COLUMNS         1
    #define COLUMN_X             0
  #else
    #define MENU_COLUMNS         2
  #endif
  #define MENUS_SCROLLBAR_WIDTH  2
  #define MENU_COLUMN2_X         (8 + LCD_W / 2)
  #define lcd_putsColumnLeft(x, y, str) lcd_puts((x > (LCD_W-10*FW-MENUS_SCROLLBAR_WIDTH)) ? MENU_COLUMN2_X : 0, y, str)
#else
  #define MENUS_SCROLLBAR_WIDTH  0
  #define MENU_COLUMNS           1
  #define COLUMN_X               0
  #define lcd_putsColumnLeft(x, y, str) lcd_putsLeft(y, str)
#endif

// Menus related stuff ...
#if defined(SDCARD)
  typedef uint16_t vertpos_t;
#else
  typedef uint8_t vertpos_t;
#endif

#if defined(PCBTARANIS)
  typedef uint8_t & check_event_t;
  #define horzpos_t int8_t
#else
  typedef uint8_t check_event_t;
  #define horzpos_t uint8_t
#endif

extern vertpos_t m_posVert;
extern horzpos_t m_posHorz;
extern vertpos_t s_pgOfs;
extern uint8_t s_noHi;
extern uint8_t s_noScroll;

void menu_lcd_onoff(uint8_t x, uint8_t y, uint8_t value, LcdFlags attr);

typedef void (*MenuFuncP)(uint8_t event);
typedef void (*MenuFuncP_PROGMEM)(uint8_t event);
extern const MenuFuncP_PROGMEM menuTabModel[];

extern MenuFuncP g_menuStack[5];
extern uint8_t g_menuStackPtr;

/// goto given Menu, but substitute current menu in menuStack
void chainMenu(MenuFuncP newMenu);
/// goto given Menu, store current menu in menuStack
void pushMenu(MenuFuncP newMenu);
/// return to last menu in menustack
void popMenu();
///deliver address of last menu which was popped from
inline MenuFuncP lastPopMenu()
{
  return g_menuStack[g_menuStackPtr+1];
}

void drawPotsBars();
void doMainScreenGraphics();
void menuFirstCalib(uint8_t event);

void onMainViewMenu(const char *result);
void menuMainView(uint8_t event);
void menuGeneralDiagAna(uint8_t event);
#if defined(FRSKY)
void menuTelemetryFrsky(uint8_t event);
#endif
void menuGeneralSetup(uint8_t event);
void menuGeneralCalib(uint8_t event);

void menuModelSelect(uint8_t event);
void menuModelCustomFunctions(uint8_t event);
void menuStatisticsView(uint8_t event);
void menuStatisticsDebug(uint8_t event);
void menuAboutView(uint8_t event);

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

// mawrow special values
#define TITLE_ROW      ((uint8_t)-1)
#define HIDDEN_ROW     ((uint8_t)-2)

int16_t checkIncDec(uint8_t event, int16_t i_pval, int16_t i_min, int16_t i_max, uint8_t i_flags=0);

#if defined(CPUM64)
int8_t checkIncDecModel(uint8_t event, int8_t i_val, int8_t i_min, int8_t i_max);
int8_t checkIncDecModelZero(uint8_t event, int8_t i_val, int8_t i_max);
int8_t checkIncDecGen(uint8_t event, int8_t i_val, int8_t i_min, int8_t i_max);
#else
#define checkIncDecModel(event, i_val, i_min, i_max) checkIncDec(event, i_val, i_min, i_max, EE_MODEL)
#define checkIncDecModelZero(event, i_val, i_max) checkIncDec(event, i_val, 0, i_max, EE_MODEL)
#define checkIncDecGen(event, i_val, i_min, i_max) checkIncDec(event, i_val, i_min, i_max, EE_GENERAL)
#endif

#define CHECK_INCDEC_MODELVAR(event, var, min, max) \
  var = checkIncDecModel(event,var,min,max)

#define CHECK_INCDEC_MODELVAR_ZERO(event, var, max) \
  var = checkIncDecModelZero(event,var,max)

#if defined(AUTOSWITCH)
#define AUTOSWITCH_ENTER_LONG() (attr && event==EVT_KEY_LONG(KEY_ENTER))
#define CHECK_INCDEC_MODELSWITCH(event, var, min, max) \
  var = checkIncDec(event,var,min,max,EE_MODEL|INCDEC_SWITCH)
#else
#define AUTOSWITCH_ENTER_LONG() (0)
#define CHECK_INCDEC_MODELSWITCH CHECK_INCDEC_MODELVAR
#endif

#if defined(AUTOSOURCE)
#define CHECK_INCDEC_MODELSOURCE(event, var, min, max) \
  var = checkIncDec(event,var,min,max,EE_MODEL|INCDEC_SOURCE)
#else
#define CHECK_INCDEC_MODELSOURCE CHECK_INCDEC_MODELVAR
#endif

#define CHECK_INCDEC_GENVAR(event, var, min, max) \
  var = checkIncDecGen(event,var,min,max)

#if defined(PCBTARANIS)
  #define NAVIGATION_LINE_BY_LINE  0x40
#else
  #define NAVIGATION_LINE_BY_LINE  0
#endif
bool check(check_event_t event, uint8_t curr, const MenuFuncP *menuTab, uint8_t menuTabSize, const pm_uint8_t *subTab, uint8_t subTabMax, vertpos_t maxrow);
bool check_simple(check_event_t event, uint8_t curr, const MenuFuncP *menuTab, uint8_t menuTabSize, vertpos_t maxrow);
bool check_submenu_simple(check_event_t event, uint8_t maxrow);

void title(const pm_char * s);
#define TITLE(str) title(str)

#if defined(CPUARM)
  #define MENU_TAB(...) const uint8_t mstate_tab[] = __VA_ARGS__
#else
  #define MENU_TAB(...) static const pm_uint8_t mstate_tab[] PROGMEM = __VA_ARGS__
#endif

#define MENU_CHECK(tab, menu, lines_count) \
  check(event, menu, tab, DIM(tab), mstate_tab, DIM(mstate_tab)-1, (lines_count)-1)

#define MENU(title, tab, menu, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  if (!MENU_CHECK(tab, menu, lines_count)) return; \
  TITLE(title)

#define SIMPLE_MENU_NOTITLE(tab, menu, lines_count) \
  if (!check_simple(event,menu,tab,DIM(tab),(lines_count)-1)) return;

#define SIMPLE_MENU(title, tab, menu, lines_count) \
  SIMPLE_MENU_NOTITLE(tab, menu, lines_count); \
  TITLE(title)

#define SUBMENU_NOTITLE(lines_count, ...) { \
  static const pm_uint8_t mstate_tab[] PROGMEM = __VA_ARGS__; \
  if (!check(event,0,NULL,0,mstate_tab,DIM(mstate_tab)-1,(lines_count)-1)) return; }

#define SUBMENU(title, lines_count, ...) \
  static const pm_uint8_t mstate_tab[] PROGMEM = __VA_ARGS__; \
  if (!check(event,0,NULL,0,mstate_tab,DIM(mstate_tab)-1,(lines_count)-1)) return; \
  TITLE(title)

#define SIMPLE_SUBMENU_NOTITLE(lines_count) \
  if (!check_submenu_simple(event,(lines_count)-1)) return;

#define SIMPLE_SUBMENU(title, lines_count) \
  SIMPLE_SUBMENU_NOTITLE(lines_count); \
  TITLE(title)

int8_t selectMenuItem(uint8_t x, uint8_t y, const pm_char *label, const pm_char *values, int8_t value, int8_t min, int8_t max, LcdFlags attr, uint8_t event);
uint8_t onoffMenuItem(uint8_t value, uint8_t x, uint8_t y, const pm_char *label, LcdFlags attr, uint8_t event );
int8_t switchMenuItem(uint8_t x, uint8_t y, int8_t value, LcdFlags attr, uint8_t event);

#if defined(GVARS)
int16_t gvarMenuItem(uint8_t x, uint8_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event); // @@@ open.20.fsguruh
#define displayGVar(x, y, v, min, max) gvarMenuItem(x, y, v, min, max, 0, 0)
#else
int8_t gvarMenuItem(uint8_t x, uint8_t y, int8_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event);
#define displayGVar(x, y, v, min, max) lcd_outdez8(x, y, v)
#endif

#define WARNING_TYPE_ASTERISK  0
#define WARNING_TYPE_CONFIRM   1
#define WARNING_TYPE_INPUT     2

extern const pm_char * s_warning;
extern const pm_char * s_warning_info;
extern uint8_t         s_warning_info_len;
extern uint8_t         s_warning_result;
extern uint8_t         s_warning_type;

extern const pm_char * s_global_warning;

#define WARNING_LINE_X 16
#define WARNING_LINE_Y 3*FH

void displayBox();
void displayPopup(const pm_char * pstr);
void displayWarning(uint8_t event);

#if defined(CPUARM)
  extern void (*popupFunc)(uint8_t event);
  extern int16_t s_warning_input_value;
  extern int16_t s_warning_input_min;
  extern int16_t s_warning_input_max;
#endif

#if defined(CPUARM)
  #define DISPLAY_WARNING       (*popupFunc)
  #define POPUP_WARNING(s)      do { s_warning = s; popupFunc = displayWarning; } while(0)
  #define POPUP_CONFIRMATION(s) do { s_warning = s; s_warning_type = WARNING_TYPE_CONFIRM; popupFunc = displayWarning; } while(0)
  #define POPUP_INPUT(s, func, start, min, max) do { s_warning = s; s_warning_type = WARNING_TYPE_INPUT; popupFunc = func; s_warning_input_value = start; s_warning_input_min = min; s_warning_input_max = max; } while(0)
#else
  #define DISPLAY_WARNING       displayWarning
  #define POPUP_WARNING(s)      s_warning = s
  #define POPUP_CONFIRMATION(s) do { s_warning = s; s_warning_type = WARNING_TYPE_CONFIRM; } while(0)
#endif

#if defined(SDCARD) || (defined(ROTARY_ENCODER_NAVIGATION) && !defined(CPUM64))
  #define NAVIGATION_MENUS
  #define MENU_ADD_ITEM(s) s_menu[s_menu_count++] = s
  #define MENU_MAX_LINES 6
  #if defined(SDCARD)
    #define MENU_ADD_SD_ITEM(s) MENU_ADD_ITEM(s)
  #else
    #define MENU_ADD_SD_ITEM(s)
  #endif
  #define MENU_LINE_LENGTH (LEN_MODEL_NAME+1)
  extern const char *s_menu[MENU_MAX_LINES];
  extern uint16_t s_menu_count;
  extern uint8_t s_menu_flags;
  extern uint16_t s_menu_offset;
  const char * displayMenu(uint8_t event);
  extern void (*menuHandler)(const char *result);
#else
  #define s_menu_count 0
#endif

#if defined(SDCARD)
  #define STATUS_LINE_LENGTH 32
  extern char statusLineMsg[STATUS_LINE_LENGTH];
  void showStatusLine();
  void drawStatusLine();
#else
  #define drawStatusLine()
#endif

void menuChannelsView(uint8_t event);

#define LABEL(...) (uint8_t)-1

#if defined(PCBTARANIS)
  #define KEY_MOVE_UP    KEY_PLUS
  #define KEY_MOVE_DOWN  KEY_MINUS
  #define CURSOR_MOVED_LEFT(event)  (EVT_KEY_MASK(event) == KEY_PLUS)
  #define CURSOR_MOVED_RIGHT(event) (EVT_KEY_MASK(event) == KEY_MINUS)
  #define CASE_EVT_ROTARY_MOVE_RIGHT CASE_EVT_ROTARY_LEFT
  #define CASE_EVT_ROTARY_MOVE_LEFT  CASE_EVT_ROTARY_RIGHT
  #define IS_ROTARY_MOVE_RIGHT       IS_ROTARY_LEFT
  #define IS_ROTARY_MOVE_LEFT        IS_ROTARY_RIGHT
#else
  #define KEY_MOVE_UP    KEY_UP
  #define KEY_MOVE_DOWN  KEY_DOWN
  #define CURSOR_MOVED_LEFT(event)  (IS_ROTARY_LEFT(event) || EVT_KEY_MASK(event) == KEY_LEFT)
  #define CURSOR_MOVED_RIGHT(event) (IS_ROTARY_RIGHT(event) || EVT_KEY_MASK(event) == KEY_RIGHT)
  #define CASE_EVT_ROTARY_MOVE_RIGHT CASE_EVT_ROTARY_RIGHT
  #define CASE_EVT_ROTARY_MOVE_LEFT  CASE_EVT_ROTARY_LEFT
  #define IS_ROTARY_MOVE_RIGHT       IS_ROTARY_RIGHT
  #define IS_ROTARY_MOVE_LEFT        IS_ROTARY_LEFT
#endif

#if defined(PCBTARANIS)
  #define REPEAT_LAST_CURSOR_MOVE() { if (CURSOR_MOVED_LEFT(event) || CURSOR_MOVED_RIGHT(event)) putEvent(event); else m_posHorz = 0; }
  #define MOVE_CURSOR_FROM_HERE()   if (m_posHorz > 0) REPEAT_LAST_CURSOR_MOVE()
#elif defined(ROTARY_ENCODER_NAVIGATION)
  #define REPEAT_LAST_CURSOR_MOVE() { if (EVT_KEY_MASK(event) >= 0x0e) putEvent(event); else m_posHorz = 0; }
  #define MOVE_CURSOR_FROM_HERE()   if (m_posHorz > 0) REPEAT_LAST_CURSOR_MOVE()
#else
  #define REPEAT_LAST_CURSOR_MOVE() m_posHorz = 0;
  #define MOVE_CURSOR_FROM_HERE()   REPEAT_LAST_CURSOR_MOVE()
#endif

#if defined(PCBTARANIS)
  #define POS_VERT_INIT   (menuTab ? (MAXCOL((uint16_t)1) == 255 ? 2 : 1) : 0)
  #define POS_HORZ_INIT(posVert)   ((COLATTR(posVert) & NAVIGATION_LINE_BY_LINE) ? -1 : 0)
  #define EDIT_MODE_INIT  0 // TODO enum
#else
  #define POS_VERT_INIT   0
  #define POS_HORZ_INIT(posVert)   0
  #define EDIT_MODE_INIT  -1
#endif

#endif
