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

#ifndef menus_h
#define menus_h

#define NO_HI_LEN 25

#define RESX    1024
#define RESXu   1024u
#define RESXul  1024ul
#define RESXl   1024l
#define RESKul  100ul
#define RESX_PLUS_TRIM (RESX+128)

typedef void (*MenuFuncP)(uint8_t event);

void displayScreenIndex(uint8_t index, uint8_t count, uint8_t attr);

#if defined(PCBX9D)
#define MENUS_SCROLLBAR_WIDTH 2
#else
#define MENUS_SCROLLBAR_WIDTH 0
#endif

#if defined(SDCARD)
typedef uint16_t pgofs_t;
#else
typedef uint8_t pgofs_t;
#endif

extern pgofs_t s_pgOfs;
extern uint8_t s_noHi;
extern uint8_t s_noScroll;

void menu_lcd_onoff(uint8_t x, uint8_t y, uint8_t value, uint8_t mode);

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

void doMainScreenGrphics();
void menuMainView(uint8_t event);
void menuGeneralDiagAna(uint8_t event);
#ifdef FRSKY
void menuTelemetryFrsky(uint8_t event);
#endif
void menuGeneralSetup(uint8_t event);
void menuGeneralCalib(uint8_t event);

void menuModelSelect(uint8_t event);
void menuModelFunctionSwitches(uint8_t event);
void menuStatisticsView(uint8_t event);
void menuStatisticsDebug(uint8_t event);

#if defined(ROTARY_ENCODERS)
extern int8_t scrollRE;
extern int16_t p1valdiff;
#define IS_RE_NAVIGATION_EVT_TYPE(event, type) (g_eeGeneral.reNavigation && event==type(BTN_REa + g_eeGeneral.reNavigation - 1))
#define IS_RE_NAVIGATION_EVT(event) (g_eeGeneral.reNavigation && (event&EVT_KEY_MASK)==(BTN_REa + g_eeGeneral.reNavigation - 1))
#else
#define IS_RE_NAVIGATION_EVT_TYPE(event, type) (0)
#define IS_RE_NAVIGATION_EVT(event) (0)
#if defined(NAVIGATION_POT1)
extern int16_t p1valdiff;
#else
#define p1valdiff 0
#endif
#endif

extern int8_t checkIncDec_Ret;  // global helper vars
extern int8_t s_editMode;        // global editmode

// checkIncDec flags
#define EE_GENERAL      0x01
#define EE_MODEL        0x02
#define NO_INCDEC_MARKS 0x04
#define INCDEC_SWITCH   0x08
int16_t checkIncDec(uint8_t event, int16_t i_pval, int16_t i_min, int16_t i_max, uint8_t i_flags);
int8_t checkIncDecModel(uint8_t event, int8_t i_val, int8_t i_min, int8_t i_max);
int8_t checkIncDecGen(uint8_t event, int8_t i_val, int8_t i_min, int8_t i_max);

#define CHECK_INCDEC_MODELVAR(event, var, min, max) \
  var = checkIncDecModel(event,var,min,max)

#if defined(AUTOSWITCH)
#define AUTOSWITCH_MENU_LONG() (attr && event==EVT_KEY_LONG(KEY_MENU))
#define CHECK_INCDEC_MODELSWITCH(event, var, min, max) \
  var = checkIncDec(event,var,min,max,EE_MODEL|INCDEC_SWITCH)
#else
#define AUTOSWITCH_MENU_LONG() (0)
#define CHECK_INCDEC_MODELSWITCH CHECK_INCDEC_MODELVAR
#endif

#define CHECK_INCDEC_GENVAR(event, var, min, max) \
  var = checkIncDecGen(event,var,min,max)

#if defined(ROTARY_ENCODERS)
void check_rotary_encoder();
#endif

// Menus related stuff ...
#if defined(SDCARD)
#define maxrow_t int16_t
#elif defined(ROTARY_ENCODERS)
#define maxrow_t int8_t
#else
#define maxrow_t uint8_t
#endif

extern maxrow_t m_posVert;
extern uint8_t m_posHorz;

inline void minit(){m_posVert=m_posHorz=0;}
bool check(uint8_t event, uint8_t curr, const MenuFuncP *menuTab, uint8_t menuTabSize, const pm_uint8_t *subTab, uint8_t subTabMax, maxrow_t maxrow);
bool check_simple(uint8_t event, uint8_t curr, const MenuFuncP *menuTab, uint8_t menuTabSize, maxrow_t maxrow);
bool check_submenu_simple(uint8_t event, uint8_t maxrow);

typedef void (*MenuFuncP_PROGMEM)(uint8_t event);

void title(const pm_char * s);
#define TITLE(str) title(str)

#define MENU(title, tab, menu, lines_count, ...) \
static const pm_uint8_t mstate_tab[] PROGMEM = __VA_ARGS__; \
if (!check(event,menu,tab,DIM(tab),mstate_tab,DIM(mstate_tab)-1,(lines_count)-1)) return; \
TITLE(title)

#define SIMPLE_MENU_NOTITLE(tab, menu, lines_count) \
if (!check_simple(event,menu,tab,DIM(tab),(lines_count)-1)) return;

#define SIMPLE_MENU(title, tab, menu, lines_count) \
SIMPLE_MENU_NOTITLE(tab, menu, lines_count); \
TITLE(title)

#define SUBMENU_NOTITLE(lines_count, ...) \
static const pm_uint8_t mstate_tab[] PROGMEM = __VA_ARGS__; \
if (!check(event,0,NULL,0,mstate_tab,DIM(mstate_tab)-1,(lines_count)-1)) return;

#define SUBMENU(title, lines_count, ...) \
static const pm_uint8_t mstate_tab[] PROGMEM = __VA_ARGS__; \
if (!check(event,0,NULL,0,mstate_tab,DIM(mstate_tab)-1,(lines_count)-1)) return; \
TITLE(title)

#define SIMPLE_SUBMENU_NOTITLE(lines_count) \
if (!check_submenu_simple(event,(lines_count)-1)) return;

#define SIMPLE_SUBMENU(title, lines_count) \
SIMPLE_SUBMENU_NOTITLE(lines_count); \
TITLE(title)

int8_t selectMenuItem(uint8_t x, uint8_t y, const pm_char *label, const pm_char *values, int8_t value, int8_t min, int8_t max, uint8_t attr, uint8_t event);
uint8_t onoffMenuItem(uint8_t value, uint8_t x, uint8_t y, const pm_char *label, uint8_t attr, uint8_t event );
int8_t switchMenuItem(uint8_t x, uint8_t y, int8_t value, uint8_t attr, uint8_t event);
int8_t gvarMenuItem(uint8_t x, uint8_t y, int8_t value, int8_t min, int8_t max, uint8_t attr, uint8_t event);
#if defined(GVARS)
#define displayGVar(x, y, v) gvarMenuItem(x, y, v, 0, 0, 0, 0)
#else
#define displayGVar(x, y, v) lcd_outdez8(x, y, v)
#endif

#define WARNING_TYPE_ASTERISK  0
#define WARNING_TYPE_CONFIRM   1

extern const pm_char * s_warning;
extern const pm_char * s_warning_info;
extern uint8_t         s_warning_info_len;
extern uint8_t         s_warning_result;
extern uint8_t         s_warning_type;

extern const pm_char * s_global_warning;

void displayBox();
void displayPopup(const pm_char * pstr);
void displayWarning(uint8_t event);

#if defined(SDCARD)
#define MENU_MAX_LINES 4
#define MENU_LINE_LENGTH (LEN_MODEL_NAME+1)
extern const char *s_menu[MENU_MAX_LINES];
extern uint16_t s_menu_count;
extern uint8_t s_menu_flags;
extern uint16_t s_menu_offset;
const char * displayMenu(uint8_t event);

#define STATUS_LINE_LENGTH 32
extern char statusLineMsg[STATUS_LINE_LENGTH];
void showStatusLine();
void drawStatusLine();
#else
#define s_menu_count 0
#define drawStatusLine()
#endif

#endif
