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

#include "../../opentx.h"
#include "../../timers.h"

#define BIGSIZE       MIDSIZE
#define LBOX_CENTERX  (BOX_WIDTH/2 + 17)
#define RBOX_CENTERX  (LCD_W-LBOX_CENTERX)
#define MODELNAME_X   (15)
#define MODELNAME_Y   (11)
#define VBATT_X       (MODELNAME_X+26)
#define VBATT_Y       (FH+3)
#define VBATTUNIT_X   (VBATT_X-2)
#define VBATTUNIT_Y   VBATT_Y
#define BITMAP_X      ((LCD_W-64)/2)
#define BITMAP_Y      (LCD_H/2)
#define PHASE_X       BITMAP_X
#define PHASE_Y       (3*FH)
#define PHASE_FLAGS   (0)
#define TIMERS_X      145
#define TIMERS_Y      20
#define TIMERS_H      25
#define TIMERS_R      193
#define REBOOT_X      (LCD_W-FW)
#define VSWITCH_X(i)  (((i>=NUM_LOGICAL_SWITCH*3/4) ? BITMAP_X+28 : ((i>=NUM_LOGICAL_SWITCH/2) ? BITMAP_X+25 : ((i>=NUM_LOGICAL_SWITCH/4) ? 21 : 18))) + 3*i)
#define VSWITCH_Y     (LCD_H-9)
#define TRIM_LH_X     (32+9)
#define TRIM_LV_X     10
#define TRIM_RV_X     (LCD_W-11)
#define TRIM_RH_X     (LCD_W-32-9)

#define TRIM_LEN 27
#define MARKER_WIDTH  5

const pm_uchar logo_taranis[] PROGMEM = {
#include "../../bitmaps/Taranis/logo.lbm"
};

const pm_uchar icons[] PROGMEM = {
#include "../../bitmaps/Taranis/icons.lbm"
};

#define ICON_RSSI     0, 9
#define ICON_SPEAKER0 9, 8
#define ICON_SPEAKER1 17, 8
#define ICON_SPEAKER2 25, 8
#define ICON_SPEAKER3 33, 8
#define ICON_SD       41, 11
#define ICON_LOGS     51, 11
#define ICON_TRAINER  61, 11
#define ICON_TRAINEE  71, 11
#define ICON_USB      81, 11
#define ICON_REBOOT   91, 11
#define ICON_ALTITUDE 102, 9

void doMainScreenGraphics()
{
  int16_t calibStickVert = calibratedStick[CONVERT_MODE(1)];
  if (g_model.throttleReversed && CONVERT_MODE(1) == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(LBOX_CENTERX, calibratedStick[CONVERT_MODE(0)], calibStickVert);

  calibStickVert = calibratedStick[CONVERT_MODE(2)];
  if (g_model.throttleReversed && CONVERT_MODE(2) == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(RBOX_CENTERX, calibratedStick[CONVERT_MODE(3)], calibStickVert);
}

void displayTrims(uint8_t phase)
{
  for (unsigned int i=0; i<NUM_STICKS; i++) {
    coord_t x[4] = { TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X };
    uint8_t vert[4] = { 0, 1, 1, 0 };
    coord_t xm, ym;
    unsigned int stickIndex = CONVERT_MODE(i);
    xm = x[stickIndex];

    uint32_t att = ROUND;
    int32_t trim = getTrimValue(phase, i);
    int32_t val = trim;
    bool exttrim = false;
    if (val < TRIM_MIN || val > TRIM_MAX) {
      exttrim = true;
    }
    if (val < -(TRIM_LEN+1)*4) {
      val = -(TRIM_LEN+1);
    }
    else if (val > (TRIM_LEN+1)*4) {
      val = TRIM_LEN+1;
    }
    else {
      val /= 4;
    }

    if (vert[i]) {
      ym = 31;
      lcd_vline(xm, ym-TRIM_LEN, TRIM_LEN*2);
      if (i!=2 || !g_model.thrTrim) {
        lcd_vline(xm-1, ym-1,  3);
        lcd_vline(xm+1, ym-1,  3);
      }
      ym -= val;
      drawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (trim >= 0) {
        lcd_hline(xm-1, ym-1,  3);
      }
      if (trim <= 0) {
        lcd_hline(xm-1, ym+1,  3);
      }
      if (exttrim) {
        lcd_hline(xm-1, ym,  3);
      }
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          lcd_outdezAtt(trim>0 ? 22 : 54, xm-2, -abs(trim), TINSIZE|VERTICAL);
        }
      }
    }
    else {
      ym = 60;
      lcd_hline(xm-TRIM_LEN, ym, TRIM_LEN*2);
      lcd_hline(xm-1, ym-1,  3);
      lcd_hline(xm-1, ym+1,  3);
      xm += val;
      drawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (trim >= 0) {
        lcd_vline(xm+1, ym-1,  3);
      }
      if (trim <= 0) {
        lcd_vline(xm-1, ym-1,  3);
      }
      if (exttrim) {
        lcd_vline(xm, ym-1,  3);
      }
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          lcd_outdezAtt((stickIndex==0 ? TRIM_LH_X : TRIM_RH_X)+(trim>0 ? -11 : 20), ym-2, -abs(trim), TINSIZE);
        }
      }
    }
    lcd_square(xm-3, ym-3, 7, att);
  }
}

void displaySliders()
{
  for (uint8_t i=NUM_STICKS; i<NUM_STICKS+NUM_POTS; i++) {
#if defined(REV9E)
    if (i < SLIDER1) continue;  // TODO change and display more values
    coord_t x = ((i==SLIDER1 || i==SLIDER3) ? 3 : LCD_W-5);
    int8_t y = (i<SLIDER3 ? LCD_H/2+1 : 1);
#else
    if (i == POT3) continue;
    coord_t x = ((i==POT1 || i==SLIDER1) ? 3 : LCD_W-5);
    int8_t y = (i>=SLIDER1 ? LCD_H/2+1 : 1);
#endif
    lcd_vline(x, y, LCD_H/2-2);
    lcd_vline(x+1, y, LCD_H/2-2);
    y += LCD_H/2-4;
    y -= ((calibratedStick[i]+RESX)*(LCD_H/2-4)/(RESX*2));  // calculate once per loop
    lcd_vline(x-1, y, 2);
    lcd_vline(x+2, y, 2);
  }
}

#define BAR_X        14
#define BAR_Y        1
#define BAR_W        184
#define BAR_H        9
#define BAR_NOTIFS_X BAR_X+133
#define BAR_VOLUME_X BAR_X+147
#define BAR_TIME_X   BAR_X+159

void displayTopBarGauge(coord_t x, int count, bool blinking=false)
{
  if (!blinking || BLINK_ON_PHASE)
    drawFilledRect(x+1, BAR_Y+2, 11, 5, SOLID, ERASE);
  for (int i=0; i<count; i+=2)
    lcd_vline(x+2+i, BAR_Y+3, 3);
}

#define LCD_NOTIF_ICON(x, icon) \
 lcd_bmp(x, BAR_Y, icons, icon); \
 lcd_hline(x, BAR_Y+8, 11)

void displayTopBar()
{
  uint8_t batt_icon_x;
  uint8_t altitude_icon_x;

  /* Tx voltage */
  putsVBat(BAR_X+2, BAR_Y+1, LEFT);
  batt_icon_x = lcdLastPos;
  lcd_rect(batt_icon_x+FW, BAR_Y+1, 13, 7);
  lcd_vline(batt_icon_x+FW+13, BAR_Y+2, 5);

  if (TELEMETRY_STREAMING()) {
    /* RSSI */
    LCD_ICON(batt_icon_x+3*FW+3, BAR_Y, ICON_RSSI);
    lcd_rect(batt_icon_x+5*FW, BAR_Y+1, 13, 7);

    /* Rx voltage */
    altitude_icon_x = batt_icon_x+7*FW+3;
    if (g_model.frsky.voltsSource) {
      TelemetryItem & voltsItem = telemetryItems[g_model.frsky.voltsSource-1];
      if (voltsItem.isAvailable()) {
        putsTelemetryChannelValue(batt_icon_x+7*FW+2, BAR_Y+1, g_model.frsky.voltsSource-1, voltsItem.value, LEFT);
        altitude_icon_x = lcdLastPos+1;
      }
    }

    /* Altitude */
    if (g_model.frsky.altitudeSource) {
      TelemetryItem & altitudeItem = telemetryItems[g_model.frsky.altitudeSource-1];
      if (altitudeItem.isAvailable()) {
        LCD_ICON(altitude_icon_x, BAR_Y, ICON_ALTITUDE);
        int32_t value = altitudeItem.value;
        TelemetrySensor & sensor = g_model.telemetrySensors[g_model.frsky.altitudeSource-1];
        if (sensor.prec) value /= sensor.prec == 2 ? 100 : 10;
        putsValueWithUnit(altitude_icon_x+2*FW-1, BAR_Y+1, value, UNIT_METERS, LEFT);
      }
    }
  }

  /* Notifs icons */
  coord_t x = BAR_NOTIFS_X;
  if (unexpectedShutdown) {
    LCD_NOTIF_ICON(x, ICON_REBOOT);
    x -= 12;
  }

  if (usbPlugged()) {
    LCD_NOTIF_ICON(x, ICON_USB);
    x -= 12;
  }

  if (TRAINER_CONNECTED() && SLAVE_MODE()) {
    LCD_NOTIF_ICON(x, ICON_TRAINEE);
    x -= 12;
  }
  else if (TRAINER_CONNECTED() && !SLAVE_MODE()) {
    LCD_NOTIF_ICON(x, ICON_TRAINER);
    x -= 12;
  }

  if (isFunctionActive(FUNCTION_LOGS)) {
    LCD_NOTIF_ICON(x, ICON_LOGS);
    x -= 12;
  }

  /* Audio volume */
  if (requiredSpeakerVolume == 0 || g_eeGeneral.beepMode == e_mode_quiet)
    LCD_ICON(BAR_VOLUME_X, BAR_Y, ICON_SPEAKER0);
  else if (requiredSpeakerVolume < 10)
    LCD_ICON(BAR_VOLUME_X, BAR_Y, ICON_SPEAKER1);
  else if (requiredSpeakerVolume < 20)
    LCD_ICON(BAR_VOLUME_X, BAR_Y, ICON_SPEAKER2);
  else
    LCD_ICON(BAR_VOLUME_X, BAR_Y, ICON_SPEAKER3);

  /* RTC time */
  putsRtcTime(BAR_TIME_X, BAR_Y+1, LEFT|TIMEBLINK);

  /* The background */
  drawFilledRect(BAR_X, BAR_Y, BAR_W, BAR_H, SOLID, FILL_WHITE|GREY(12)|ROUND);

  /* The inside of the Batt gauge */
  displayTopBarGauge(batt_icon_x+FW, GET_TXBATT_BARS(), IS_TXBATT_WARNING());

  /* The inside of the RSSI gauge */
  if (TELEMETRY_RSSI() > 0) {
    displayTopBarGauge(batt_icon_x+5*FW, TELEMETRY_RSSI() / 10, TELEMETRY_RSSI() < getRssiAlarmValue(0));
  }
}

void displayTimers()
{
  // Main and Second timer
  for (unsigned int i=0; i<2; i++) {
    if (g_model.timers[i].mode) {
      TimerState & timerState = timersStates[i];
      TimerData & timerData = g_model.timers[i];
      uint8_t y = TIMERS_Y + i*TIMERS_H;
      if (zlen(timerData.name, LEN_TIMER_NAME) > 0) {
        lcd_putsnAtt(TIMERS_X, y-7, timerData.name, LEN_TIMER_NAME, ZCHAR|SMLSIZE);
      }
      else {
        putsTimerMode(TIMERS_X, y-7, timerData.mode, SMLSIZE);
      }
      putsTimer(TIMERS_X, y, timerState.val, TIMEHOUR|MIDSIZE|LEFT, TIMEHOUR|MIDSIZE|LEFT);
      if (timerData.persistent) {
        lcd_putcAtt(TIMERS_R, y+1, 'P', SMLSIZE);
      }
      if (timerState.val < 0) {
        if (BLINK_ON_PHASE) {
          drawFilledRect(TIMERS_X-7, y-8, 60, 20);
        }
      }
    }
  }
}

void menuMainViewChannelsMonitor(uint8_t event)
{
  switch(event) {
    case EVT_KEY_BREAK(KEY_PAGE):
    case EVT_KEY_BREAK(KEY_EXIT):
      chainMenu(menuMainView);
      event = 0;
      break;
  }

  return menuChannelsView(event);
}

void onMainViewMenu(const char *result)
{
  if (result == STR_RESET_TIMER1) {
    timerReset(0);
  }
  else if (result == STR_RESET_TIMER2) {
    timerReset(1);
  }
#if TIMERS > 2
  else if (result == STR_RESET_TIMER3) {
    timerReset(2);
  }
#endif
  else if (result == STR_VIEW_NOTES) {
    pushModelNotes();
  }
  else if (result == STR_RESET_SUBMENU) {
    MENU_ADD_ITEM(STR_RESET_FLIGHT);
    MENU_ADD_ITEM(STR_RESET_TIMER1);
    MENU_ADD_ITEM(STR_RESET_TIMER2);
    MENU_ADD_ITEM(STR_RESET_TIMER3);
    MENU_ADD_ITEM(STR_RESET_TELEMETRY);
  }
  else if (result == STR_RESET_TELEMETRY) {
    telemetryReset();
  }
  else if (result == STR_RESET_FLIGHT) {
    flightReset();
  }
  else if (result == STR_STATISTICS) {
    chainMenu(menuStatisticsView);
  }
  else if (result == STR_ABOUT_US) {
    chainMenu(menuAboutView);
  }
}

void displaySwitch(coord_t x, coord_t y, int width, unsigned int index)
{
  if (SWITCH_EXISTS(index)) {
    int val = getValue(MIXSRC_FIRST_SWITCH+index);

    if (val >= 0) {
      lcd_hline(x, y, width);
      lcd_hline(x, y+2, width);
      y += 4;
      if (val > 0) {
        lcd_hline(x, y, width);
        lcd_hline(x, y+2, width);
        y += 4;
      }
    }

    lcd_putcAtt(width==5 ? x+1 : x, y, 'A'+index, TINSIZE);
    y += 6;

    if (val <= 0) {
      lcd_hline(x, y, width);
      lcd_hline(x, y+2, width);
      if (val < 0) {
        lcd_hline(x, y+4, width);
        lcd_hline(x, y+6, width);
      }
    }
  }
}

const MenuItem MAIN_MENU[] = {
  { "RADIO SETTINGS", menuGeneralSetup },
  { "MODEL SELECT", menuModelSelect },
  { "MODEL SETTINGS", menuModelSetup },
  { "CHECKLIST", menuModelNotes },
  { "SD MANAGER", menuGeneralSdManager },
  { "VERSION", menuGeneralVersion }
};

bool isMenuAvailable(int index)
{
  if (index == 4) {
    return modelHasNotes();
  }
  else {
    return true;
  }
}

#define GRAPHICAL_MENUS 0

#if GRAPHICAL_MENUS == 1
  #define DECLARE_MAIN_MENU()       static int currentMenuIndex = -1
  #define INIT_MAIN_MENU()          currentMenuIndex = -1
  #define IS_MAIN_MENU_DISPLAYED()  currentMenuIndex > 0
  #define TOGGLE_MAIN_MENU()        currentMenuIndex = -currentMenuIndex
#else
  #define DECLARE_MAIN_MENU()
  #define INIT_MAIN_MENU()
  #define IS_MAIN_MENU_DISPLAYED()  0
  #define TOGGLE_MAIN_MENU()
#endif

int getSwitchCount()
{
  int count = 0;
  for (int i=0; i<NUM_SWITCHES; ++i) {
    if (SWITCH_EXISTS(i)) {
      ++count;
    }
  }
  return count;
}

void menuMainView(uint8_t event)
{
  DECLARE_MAIN_MENU();

  STICK_SCROLL_DISABLE();

  switch(event) {

    case EVT_ENTRY:
      INIT_MAIN_MENU();
      killEvents(KEY_EXIT);
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      // no break

    case EVT_ENTRY_UP:
      LOAD_MODEL_BITMAP();
      break;

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(event);
      if (modelHasNotes()) {
        MENU_ADD_ITEM(STR_VIEW_NOTES);
      }
      MENU_ADD_ITEM(STR_RESET_SUBMENU);
      MENU_ADD_ITEM(STR_STATISTICS);
      MENU_ADD_ITEM(STR_ABOUT_US);
      menuHandler = onMainViewMenu;
      break;

#if MENUS_LOCK != 2/*no menus*/
    case EVT_KEY_BREAK(KEY_MENU):
#if GRAPHICAL_MENUS == 1
      TOGGLE_MAIN_MENU();
#else
      pushMenu(menuModelSelect);
#endif
      break;

    case EVT_KEY_LONG(KEY_MENU):
#if GRAPHICAL_MENUS == 1
      pushMenu(lastPopMenu());
#else
      pushMenu(menuGeneralSetup);
      killEvents(event);
#endif
      break;
#endif

    case EVT_KEY_BREAK(KEY_PAGE):
      eeDirty(EE_GENERAL);
      g_eeGeneral.view += 1;
      if (g_eeGeneral.view >= VIEW_COUNT) {
        g_eeGeneral.view = 0;
        chainMenu(menuMainViewChannelsMonitor);
      }
      break;

    case EVT_KEY_LONG(KEY_PAGE):
      if (!IS_FAI_ENABLED())
        chainMenu(menuTelemetryFrsky);
      killEvents(event);
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
      if (IS_MAIN_MENU_DISPLAYED()) {
        TOGGLE_MAIN_MENU();
      }
#if defined(GVARS)
      else if (s_gvar_timer > 0) {
        s_gvar_timer = 0;
      }
#endif
      AUDIO_KEYPAD_UP();
      break;
  }

  // Flight Mode Name
  int mode = mixerCurrentFlightMode;
  lcd_putsnAtt(PHASE_X, PHASE_Y, g_model.flightModeData[mode].name, sizeof(g_model.flightModeData[mode].name), ZCHAR|PHASE_FLAGS);

  // Model Name
  putsModelName(MODELNAME_X, MODELNAME_Y, g_model.header.name, g_eeGeneral.currModel, BIGSIZE);

  // Trims sliders
  displayTrims(mode);

  // Top bar
  displayTopBar();

  // Sliders (Pots / Sliders)
  displaySliders();

  lcd_bmp(BITMAP_X, BITMAP_Y, modelBitmap);

  // Switches
  if (getSwitchCount() > 8) {
    for (int i=0; i<NUM_SWITCHES; ++i) {
      div_t qr = div(i, 9);
      if (g_eeGeneral.view == VIEW_INPUTS) {
        div_t qr2 = div(qr.rem, 5);
        if (i >= 14) qr2.rem += 1;
        const coord_t x[4] = { 50, 144 };
        const coord_t y[4] = { 25, 42, 25, 42 };
        displaySwitch(x[qr.quot]+qr2.rem*4, y[qr2.quot], 3, i);
      }
      else {
        displaySwitch(15+qr.rem*6, 25+qr.quot*17, 5, i);
      }
    }
  }
  else {
    int index = 0;
    for (int i=0; i<NUM_SWITCHES; ++i) {
      if (SWITCH_EXISTS(i)) {
        getvalue_t val = getValue(MIXSRC_FIRST_SWITCH+i);
        getvalue_t sw = ((val < 0) ? 3*i+1 : ((val == 0) ? 3*i+2 : 3*i+3));
        putsSwitches((g_eeGeneral.view == VIEW_INPUTS) ? (index<4 ? 8*FW+3 : 24*FW+1) : (index<4 ? 3*FW+2 : 8*FW-1), (index%4)*FH+3*FH, sw, 0);
        index++;
      }
    }
  }

  if (g_eeGeneral.view == VIEW_TIMERS) {
    displayTimers();
  }
  else if (g_eeGeneral.view == VIEW_INPUTS) {
    // Sticks
    doMainScreenGraphics();
  }
  else {
    // Logical Switches
    lcd_puts(TRIM_RH_X - TRIM_LEN/2 + 5, 6*FH-1, "LS 1-32");
    for (int sw=0; sw<NUM_LOGICAL_SWITCH; ++sw) {
      div_t qr = div(sw, 10);
      uint8_t y = 13 + 11 * qr.quot;
      uint8_t x = TRIM_RH_X - TRIM_LEN + qr.rem*5 + (qr.rem >= 5 ? 3 : 0);
      LogicalSwitchData * cs = lswAddress(sw);
      if (cs->func == LS_FUNC_NONE) {
        lcd_hline(x, y+6, 4);
        lcd_hline(x, y+7, 4);
      }
      else if (getSwitch(SWSRC_SW1+sw)) {
        drawFilledRect(x, y, 4, 8);
      }
      else {
        lcd_rect(x, y, 4, 8);
      }
    }
  }

#if defined(GVARS)
  if (s_gvar_timer > 0) {
    s_gvar_timer--;
    drawFilledRect(BITMAP_X, BITMAP_Y, 64, 32, SOLID, ERASE);
    lcd_rect(BITMAP_X, BITMAP_Y, 64, 32);
    putsStrIdx(BITMAP_X+FW, BITMAP_Y+FH-1, STR_GV, s_gvar_last+1);
    lcd_putsnAtt(BITMAP_X+4*FW+FW/2, BITMAP_Y+FH-1, g_model.gvars[s_gvar_last].name, LEN_GVAR_NAME, ZCHAR);
    lcd_putsAtt(BITMAP_X+FW, BITMAP_Y+2*FH+3, PSTR("[\010]"), BOLD);
    lcd_outdezAtt(BITMAP_X+5*FW+FW/2, BITMAP_Y+2*FH+3, GVAR_VALUE(s_gvar_last, getGVarFlightPhase(mixerCurrentFlightMode, s_gvar_last)), BOLD);
  }
#endif

#if GRAPHICAL_MENUS > 0
  if (IS_MAIN_MENU_DISPLAYED()) {
    displayMenuBar(MAIN_MENU, currentMenuIndex-1);
    switch (event) {
      case EVT_KEY_FIRST(KEY_MINUS):
        currentMenuIndex = circularIncDec(currentMenuIndex, +1, 1, DIM(MAIN_MENU), isMenuAvailable);
        break;

      case EVT_KEY_FIRST(KEY_PLUS):
        currentMenuIndex = circularIncDec(currentMenuIndex, -1, 1, DIM(MAIN_MENU), isMenuAvailable);
        break;

      case EVT_KEY_FIRST(KEY_ENTER):
        killEvents(event);
        pushMenu(MAIN_MENU[currentMenuIndex-1].action);
        TOGGLE_MAIN_MENU();
        break;
    }
  }
#endif
}
