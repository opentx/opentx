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

#include "../opentx.h"

#if LCD_W >= 212
#define BIGSIZE       MIDSIZE
#define BOX_WIDTH     31
#define BOX_CENTERY   (LCD_H-BOX_WIDTH/2-10)
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
#define TIMER1_Y      20
#define TIMER2_Y      45
#define TIMERS_R      193
#define REBOOT_X      (LCD_W-FW)
#define VSWITCH_X(i)  (((i>=NUM_CSW*3/4) ? BITMAP_X+28 : ((i>=NUM_CSW/2) ? BITMAP_X+25 : ((i>=NUM_CSW/4) ? 21 : 18))) + 3*i)
#define VSWITCH_Y     (LCD_H-9)
#define BAR_HEIGHT    (31-9)
#define TRIM_LH_X     (32+9)
#define TRIM_LV_X     10
#define TRIM_RV_X     (LCD_W-11)
#define TRIM_RH_X     (LCD_W-32-9)
#else
#define BIGSIZE       DBLSIZE
#define BOX_WIDTH     23
#define BOX_CENTERY   (LCD_H-9-BOX_WIDTH/2)
#define LBOX_CENTERX  (LCD_W/4 + 10)
#define RBOX_CENTERX  (3*LCD_W/4 - 10)
#define MODELNAME_X   (2*FW-2)
#define MODELNAME_Y   (0)
#define PHASE_X       (6*FW)
#define PHASE_Y       (2*FH)
#define PHASE_FLAGS   0
#define VBATT_X       (6*FW)
#define VBATT_Y       (2*FH)
#define VBATTUNIT_X   (VBATT_X-1)
#define VBATTUNIT_Y   (3*FH)
#define REBOOT_X      (20*FW-3)
#define VSWITCH_X(i)  (16 + 3*i)
#define VSWITCH_Y     (LCD_H-8)
#define BAR_HEIGHT    (BOX_WIDTH-1l)
#define TRIM_LH_X     (LCD_W*1/4+2)
#define TRIM_LV_X     3
#define TRIM_RV_X     (LCD_W-4)
#define TRIM_RH_X     (LCD_W*3/4-2)
#endif

#define TRIM_LEN 27
#define MARKER_WIDTH  5
#define BOX_LIMIT     (BOX_WIDTH-MARKER_WIDTH)

#if defined(PCBTARANIS)
  const pm_uchar logo_taranis[] PROGMEM = {
  #include "../bitmaps/logo_taranis.lbm"
  };

  const pm_uchar icons[] PROGMEM = {
  #include "../bitmaps/icons.lbm"
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
#endif

void drawPotsBars()
{
  // Optimization by Mike Blandford
  uint8_t x, y, len ;  // declare temporary variables
  for (x=LCD_W/2-5, y=NUM_STICKS; y<NUM_STICKS+NUM_POTS; x+=5, y++) {
    len = ((calibratedStick[y]+RESX)*BAR_HEIGHT/(RESX*2))+1l;  // calculate once per loop
    V_BAR(x, LCD_H-8, len)
  }
}

void drawStick(uint8_t centrex, int16_t xval, int16_t yval)
{
  lcd_square(centrex-BOX_WIDTH/2, BOX_CENTERY-BOX_WIDTH/2, BOX_WIDTH);
  DO_CROSS(centrex, BOX_CENTERY, 3);
  lcd_square(centrex + (xval/((2*RESX)/BOX_LIMIT)) - MARKER_WIDTH/2, BOX_CENTERY - (yval/((2*RESX)/BOX_LIMIT)) - MARKER_WIDTH/2, MARKER_WIDTH, ROUND);
}

void doMainScreenGraphics()
{
  int16_t calibStickVert = calibratedStick[CONVERT_MODE(1+1)-1];
  if (g_model.throttleReversed && CONVERT_MODE(1+1)-1 == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(LBOX_CENTERX, calibratedStick[CONVERT_MODE(0+1)-1], calibStickVert);

  calibStickVert = calibratedStick[CONVERT_MODE(2+1)-1];
  if (g_model.throttleReversed && CONVERT_MODE(2+1)-1 == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(RBOX_CENTERX, calibratedStick[CONVERT_MODE(3+1)-1], calibStickVert);

#if !defined(PCBTARANIS)
  drawPotsBars();
#endif
}

void displayTrims(uint8_t phase)
{
  for (uint8_t i=0; i<4; i++) {
    static xcoord_t x[4] = {TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X};
    static uint8_t vert[4] = {0,1,1,0};
    uint8_t xm, ym;
    xm = x[CONVERT_MODE(i+1)-1];

    uint8_t att = ROUND;
    int16_t val = getTrimValue(phase, i);

    if (val < -125 || val > 125)
      att = BLINK|INVERS|ROUND;

    if (val < -(TRIM_LEN+1)*4)
      val = -(TRIM_LEN+1);
    else if (val > (TRIM_LEN+1)*4)
      val = TRIM_LEN+1;
    else
      val /= 4;

    if (vert[i]) {
      ym = 31;
      lcd_vline(xm, ym-TRIM_LEN, TRIM_LEN*2);
      if (i!=2 || !g_model.thrTrim) {
        lcd_vline(xm-1, ym-1,  3);
        lcd_vline(xm+1, ym-1,  3);
      }
      ym -= val;
    }
    else {
      ym = 60;
      lcd_hline(xm-TRIM_LEN, ym, TRIM_LEN*2);
      lcd_hline(xm-1, ym-1,  3);
      lcd_hline(xm-1, ym+1,  3);
      xm += val;
    }
    lcd_square(xm-3, ym-3, 7, att);
  }
}

#if defined(PCBTARANIS)
void displaySliders()
{
  for (uint8_t i=NUM_STICKS; i<NUM_STICKS+NUM_POTS; i++) {
    xcoord_t x = (i%2 ? LCD_W-5 : 3);
    int8_t y = (i>NUM_STICKS+1 ? LCD_H/2+1 : 1);
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
#define BAR_BATT_X   BAR_X+18
#define BAR_RSSI_X   BAR_X+42
#define BAR_A1_X     BAR_X+69
#define BAR_NOTIFS_X BAR_X+133
#define BAR_VOLUME_X BAR_X+147
#define BAR_TIME_X   BAR_X+168

void displayTopBarGauge(xcoord_t x, int count, bool blinking=false)
{
  if (!blinking || BLINK_ON_PHASE)
    lcd_filled_rect(x+1, BAR_Y+2, 11, 5, SOLID, ERASE);
  count = min(10, count);
  for (int i=0; i<count; i+=2)
    lcd_vline(x+2+i, BAR_Y+3, 3);
}

#define LCD_NOTIF_ICON(x, icon) \
 lcd_bmp(x, BAR_Y, icons, icon); \
 lcd_hline(x, BAR_Y+8, 11)

void displayTopBar()
{
  /* Tx voltage */
  putsVBat(BAR_BATT_X, BAR_Y+1, 0);
  lcd_rect(BAR_BATT_X+FW, BAR_Y+1, 13, 7);
  lcd_vline(BAR_BATT_X+FW+13, BAR_Y+2, 5);

  if (frskyData.rssi[0].value > 0) {
    /* RSSI */
    LCD_ICON(BAR_RSSI_X, BAR_Y, ICON_RSSI);
    lcd_rect(BAR_RSSI_X+10, BAR_Y+1, 13, 7);

    /* Rx voltage */
    lcdint_t voltage = 0;
    uint8_t channel = 0;
    if (g_model.frsky.voltsSource <= 1) {
      channel = TELEM_A1+g_model.frsky.voltsSource-1;
      voltage = frskyData.analog[g_model.frsky.voltsSource].value;
    }
    else if (g_model.frsky.voltsSource == 2) {
      channel = TELEM_VFAS-1;
      voltage = frskyData.hub.vfas;
    }
    else if (g_model.frsky.voltsSource == 3) {
      channel = TELEM_CELLS_SUM-1;
      voltage = frskyData.hub.cellsSum;
    }
    if (voltage > 0) {
      putsTelemetryChannel(BAR_A1_X, BAR_Y+1, channel, voltage, LEFT);
    }
  }

  /* Notifs icons */
  xcoord_t x = BAR_NOTIFS_X;
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

  if (isFunctionActive(FUNC_LOGS)) {
    LCD_NOTIF_ICON(x, ICON_LOGS);
    x -= 12;
  }

  if (sdMounted()) {
    LCD_NOTIF_ICON(x, ICON_SD);
  }

  /* Audio volume */
  if (requiredSpeakerVolume == 0)
    LCD_ICON(BAR_VOLUME_X, BAR_Y, ICON_SPEAKER0);
  else if (requiredSpeakerVolume < 10)
    LCD_ICON(BAR_VOLUME_X, BAR_Y, ICON_SPEAKER1);
  else if (requiredSpeakerVolume < 20)
    LCD_ICON(BAR_VOLUME_X, BAR_Y, ICON_SPEAKER2);
  else
    LCD_ICON(BAR_VOLUME_X, BAR_Y, ICON_SPEAKER3);

  /* RTC time */
  struct gtm t;
  gettime(&t);
  lcd_putcAtt(BAR_TIME_X-1, BAR_Y+1, ':', BLINK);
  lcd_outdezNAtt(BAR_TIME_X+1, BAR_Y+1, t.tm_hour, LEADING0, 2);
  lcd_outdezNAtt(BAR_TIME_X+3*FWNUM-1, BAR_Y+1, t.tm_min, LEADING0, 2);

  /* The background */
  lcd_filled_rect(BAR_X, BAR_Y, BAR_W, BAR_H, SOLID, FILL_WHITE|GREY_DEFAULT|ROUND);

  /* The inside of the Batt gauge */
  int count = 10 * (g_vbat100mV - g_eeGeneral.vBatMin - 90) / (30 + g_eeGeneral.vBatMax - g_eeGeneral.vBatMin);
  displayTopBarGauge(BAR_BATT_X+FW, count, g_vbat100mV <= g_eeGeneral.vBatWarn);

  /* The inside of the RSSI gauge */
  if (frskyData.rssi[0].value > 0) {
    displayTopBarGauge(BAR_RSSI_X+10, frskyData.rssi[0].value / 10, frskyData.rssi[0].value < getRssiAlarmValue(0));
  }
}
#endif

#if LCD_W >= 212
void displayTimers()
{
  // Main timer
  if (g_model.timers[0].mode) {
    TimerState & timerState = timersStates[0];
    putsTime(TIMERS_X, TIMER1_Y, timerState.val, MIDSIZE|LEFT, MIDSIZE|LEFT);
    putsTmrMode(TIMERS_X, TIMER1_Y-6, g_model.timers[0].mode, STRCONDENSED|SMLSIZE);
    if (g_model.timers[0].persistent) lcd_putcAtt(TIMERS_R, TIMER1_Y+1, 'P', SMLSIZE);
    if (timerState.val < 0) {
      if (BLINK_ON_PHASE) {
        lcd_filled_rect(TIMERS_X-7, TIMER1_Y-7, 60, 19);
      }
    }
  }

  // Second timer
  if (g_model.timers[1].mode) {
    TimerState & timerState = timersStates[1];
    putsTime(TIMERS_X, TIMER2_Y, timerState.val, MIDSIZE|LEFT, MIDSIZE|LEFT);
    putsTmrMode(TIMERS_X, TIMER2_Y-6, g_model.timers[1].mode, STRCONDENSED|SMLSIZE);
    if (g_model.timers[1].persistent) lcd_putcAtt(TIMERS_R, TIMER2_Y+1, 'P', SMLSIZE);
    if (timerState.val < 0) {
      if (BLINK_ON_PHASE) {
        lcd_filled_rect(TIMERS_X-7, TIMER2_Y-7, 60, 19);
      }
    }
  }
}
#else
void displayTimers()
{
  // Main timer
  if (g_model.timers[0].mode) {
    TimerState & timerState = timersStates[0];
    uint8_t att = DBLSIZE | (timerState.state==TMR_BEEPING ? BLINK|INVERS : 0);
    putsTime(12*FW+2+10*FWNUM-4, FH*2, timerState.val, att, att);
    putsTmrMode(timerState.val >= 0 ? 9*FW-FW/2+3 : 9*FW-FW/2-4, FH*3, g_model.timers[0].mode, STRCONDENSED);
  }
}
#endif

void displayBattVoltage()
{
#if defined(BATTGRAPH)
  putsVBat(VBATT_X-8, VBATT_Y+1, 0);
  lcd_filled_rect(VBATT_X-25, VBATT_Y+9, 22, 5);
  lcd_vline(VBATT_X-3, VBATT_Y+10, 3);
  uint8_t count = limit<uint8_t>(2, 20 * (g_vbat100mV - g_eeGeneral.vBatMin - 90) / (30 + g_eeGeneral.vBatMax - g_eeGeneral.vBatMin), 20);
  for (uint8_t i=0; i<count; i+=2)
    lcd_vline(VBATT_X-24+i, VBATT_Y+10, 3);
  if (g_vbat100mV > g_eeGeneral.vBatWarn || BLINK_ON_PHASE)
    lcd_filled_rect(VBATT_X-26, VBATT_Y, 25, 15);
#else
  LcdFlags att = (g_vbat100mV <= g_eeGeneral.vBatWarn ? BLINK|INVERS : 0) | BIGSIZE;
  putsVBat(VBATT_X-1, VBATT_Y, att|NO_UNIT);
#if LCD_W >= 212
  lcd_putcAtt(VBATTUNIT_X, VBATTUNIT_Y, 'v', MIDSIZE);
#else
  lcd_putc(VBATT_X, VBATTUNIT_Y, 'V');
#endif
#endif
}

#if defined(PCBSKY9X)
void displayVoltageOrAlarm()
{
  if (g_vbat100mV > g_eeGeneral.vBatWarn && g_eeGeneral.temperatureWarn && getTemperature() >= g_eeGeneral.temperatureWarn) {
    putsTelemetryValue(6*FW-1, 3*FH, getTemperature(), UNIT_DEGREES, BLINK|INVERS|DBLSIZE);
  }
  else if (g_vbat100mV > g_eeGeneral.vBatWarn && g_eeGeneral.mAhWarn && (g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.currentCalib)/8192/36) / 500 >= g_eeGeneral.mAhWarn) {
    putsTelemetryValue(7*FW-1, 3*FH, (g_eeGeneral.mAhUsed + Current_used*(488 + g_eeGeneral.currentCalib)/8192/36)/10, UNIT_MAH, BLINK|INVERS|DBLSIZE);
  }
  else {
    displayBattVoltage();
  }
}
#else
  #define displayVoltageOrAlarm() displayBattVoltage()
#endif

#if defined(PCBTARANIS)
  #define EVT_KEY_MODEL_MENU   EVT_KEY_BREAK(KEY_MENU)
  #define EVT_KEY_GENERAL_MENU EVT_KEY_LONG(KEY_MENU)
  #define EVT_KEY_TELEMETRY    EVT_KEY_LONG(KEY_PAGE)
  #define EVT_KEY_CONTEXT_MENU EVT_KEY_LONG(KEY_ENTER)
#else
  #define EVT_KEY_MODEL_MENU   EVT_KEY_LONG(KEY_RIGHT)
  #define EVT_KEY_GENERAL_MENU EVT_KEY_LONG(KEY_LEFT)
  #define EVT_KEY_TELEMETRY    EVT_KEY_LONG(KEY_DOWN)
  #define EVT_KEY_STATISTICS   EVT_KEY_LONG(KEY_UP)
  #define EVT_KEY_CONTEXT_MENU EVT_KEY_BREAK(KEY_MENU)
#endif

#if defined(PCBTARANIS)
void menuMainViewChannelsMonitor(uint8_t event)
{
  switch(event) {
    case EVT_KEY_BREAK(KEY_PAGE):
    case EVT_KEY_BREAK(KEY_EXIT):
      chainMenu(menuMainView);
      return;
  }

  return menuChannelsView(event);
}
#endif

#if defined(NAVIGATION_MENUS)
void onMainViewMenu(const char *result)
{
  if (result == STR_RESET_TIMER1) {
    resetTimer(0);
  }
  else if (result == STR_RESET_TIMER2) {
    resetTimer(1);
  }
#if defined(FRSKY)
  else if (result == STR_RESET_TELEMETRY) {
    resetTelemetry();
  }
#endif
  else if (result == STR_RESET_FLIGHT) {
    resetAll();
  }
  else if (result == STR_STATISTICS) {
    chainMenu(menuStatisticsView);
  }
#if defined(CPUARM)
  else if (result == STR_ABOUT_US) {
    chainMenu(menuAboutView);
  }
#endif
}
#endif

void menuMainView(uint8_t event)
{
  STICK_SCROLL_DISABLE();

#if !defined(PCBTARANIS)
  uint8_t view = g_eeGeneral.view;
  uint8_t view_base = view & 0x0f;
#endif

  uint8_t _event = event;
  if (s_global_warning) {
    event = 0;
  }

  switch(event)
  {
    case EVT_ENTRY:
      killEvents(KEY_EXIT);
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      break;

#if !defined(PCBTARANIS)
    /* TODO if timer2 is OFF, it's possible to use this timer2 as in er9x...
    case EVT_KEY_BREAK(KEY_MENU):
      if (view_base == VIEW_TIMER2) {
        Timer2_running = !Timer2_running;
        AUDIO_KEYPAD_UP();
      }
    break;
    */

    case EVT_KEY_BREAK(KEY_RIGHT):
    case EVT_KEY_BREAK(KEY_LEFT):
      if (view_base <= VIEW_INPUTS) {
#if defined(PCBSKY9X)
        if (view_base == VIEW_INPUTS)
          g_eeGeneral.view ^= ALTERNATE_VIEW;
        else
          g_eeGeneral.view = (g_eeGeneral.view + (4*ALTERNATE_VIEW) + ((event==EVT_KEY_BREAK(KEY_LEFT)) ? -ALTERNATE_VIEW : ALTERNATE_VIEW)) % (4*ALTERNATE_VIEW);
#else
        g_eeGeneral.view ^= ALTERNATE_VIEW;
#endif
        eeDirty(EE_GENERAL);
        AUDIO_KEYPAD_UP();
      }
      break;
#endif

#if defined(NAVIGATION_MENUS)
    case EVT_KEY_CONTEXT_MENU:
      killEvents(event);
      MENU_ADD_ITEM(STR_RESET_FLIGHT);
      MENU_ADD_ITEM(STR_RESET_TIMER1);
      MENU_ADD_ITEM(STR_RESET_TIMER2);
#if defined(FRSKY)
      MENU_ADD_ITEM(STR_RESET_TELEMETRY);
#endif
      MENU_ADD_ITEM(STR_STATISTICS);
#if defined(CPUARM)
      MENU_ADD_ITEM(STR_ABOUT_US);
#endif
      menuHandler = onMainViewMenu;
      break;
#endif

#if !defined(READONLY)
#if !defined(PCBTARANIS)
    case EVT_KEY_LONG(KEY_MENU):// go to last menu
      pushMenu(lastPopMenu());
      killEvents(event);
      break;
#endif

    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_MODEL_MENU:
      pushMenu(menuModelSelect);
      killEvents(event);
      break;

    CASE_EVT_ROTARY_LONG
    case EVT_KEY_GENERAL_MENU:
      pushMenu(menuGeneralSetup);
      killEvents(event);
      break;
#endif

#if defined(PCBTARANIS)
    case EVT_KEY_BREAK(KEY_PAGE):
      eeDirty(EE_GENERAL);
      g_eeGeneral.view += 1;
      if (g_eeGeneral.view >= VIEW_COUNT) {
        g_eeGeneral.view = 0;
        chainMenu(menuMainViewChannelsMonitor);
        return;
      }
      break;
#else
    case EVT_KEY_BREAK(KEY_UP):
    case EVT_KEY_BREAK(KEY_DOWN):
      g_eeGeneral.view = (event == EVT_KEY_BREAK(KEY_UP) ? (view_base == VIEW_COUNT-1 ? 0 : view_base+1) : (view_base == 0 ? VIEW_COUNT-1 : view_base-1));
      eeDirty(EE_GENERAL);
      AUDIO_KEYPAD_UP();
      break;
#endif

#if !defined(PCBTARANIS)
    case EVT_KEY_STATISTICS:
      chainMenu(menuStatisticsView);
      killEvents(event);
      return;
#endif

    case EVT_KEY_TELEMETRY:
#if defined(FRSKY)
      if (!IS_FAI_ENABLED())
        chainMenu(menuTelemetryFrsky);
#elif defined(JETI)
      JETI_EnableRXD(); // enable JETI-Telemetry reception
      chainMenu(menuTelemetryJeti);
#elif defined(ARDUPILOT)
      ARDUPILOT_EnableRXD(); // enable ArduPilot-Telemetry reception
      chainMenu(menuTelemetryArduPilot);
#elif defined(NMEA)
      NMEA_EnableRXD(); // enable NMEA-Telemetry reception
      chainMenu(menuTelemetryNMEA);
#elif defined(MAVLINK)
      chainMenu(menuTelemetryMavlink);
#else
      chainMenu(menuStatisticsDebug);
#endif
      killEvents(event);
      return;

    case EVT_KEY_FIRST(KEY_EXIT):
      if (timersStates[0].state==TMR_BEEPING) {
        timersStates[0].state = TMR_STOPPED;
      }
      else if (s_global_warning) {
        s_global_warning = NULL;
      }
#if !defined(PCBTARANIS)
      else if (view == VIEW_TIMER2) {
        resetTimer(1);
      }
#endif
      AUDIO_KEYPAD_UP();
      break;

#if !defined(NAVIGATION_MENUS)
    case EVT_KEY_LONG(KEY_EXIT):
      resetAll();
      AUDIO_KEYPAD_UP();
      break;
#endif
  }

  {
    // Flight Phase Name
    uint8_t phase = s_perout_flight_phase;
    lcd_putsnAtt(PHASE_X, PHASE_Y, g_model.phaseData[phase].name, sizeof(g_model.phaseData[phase].name), ZCHAR|PHASE_FLAGS);

    // Model Name
    putsModelName(MODELNAME_X, MODELNAME_Y, g_model.header.name, g_eeGeneral.currModel, BIGSIZE);

#if !defined(PCBTARANIS)
    // Main Voltage (or alarm if any)
    displayVoltageOrAlarm();

    // Timers
    displayTimers();
#endif

    // Trims sliders
    displayTrims(phase);
  }

#if defined(PCBTARANIS)
  // Top bar
  displayTopBar();

  // Sliders (Pots / Sliders)
  displaySliders();

  lcd_bmp(BITMAP_X, BITMAP_Y, modelBitmap);

  // Switches
  for (uint8_t i=0; i<8; i++) {
    getvalue_t sw;
    getvalue_t val;
    // TODO simplify this + reuse code in checkSwitches() + Menu MODELSETUP
    switch(i) {
      case 5:
        sw = getValue(MIXSRC_SF-1) > 0 ? 3*i+2 : 3*i+1;
        break;
      case 6:
        val = getValue(MIXSRC_SG-1);
        sw = ((val < 0) ? 3*i : ((val == 0) ? 3*i+1 : 3*i+2));
        break;     
      case 7:
        sw = getValue(MIXSRC_SH-1) > 0 ? 3*i+1 : 3*i;
        break;
      default:
      {
        val = getValue(MIXSRC_SA+i-1);
        sw = ((val < 0) ? 3*i+1 : ((val == 0) ? 3*i+2 : 3*i+3));
        break;
      }
    }
    putsSwitches((g_eeGeneral.view == VIEW_INPUTS) ? (i<4 ? 8*FW+3 : 24*FW+1) : (i<4 ? 3*FW+2 : 8*FW-1), (i%4)*FH+3*FH, sw, 0);
  }

  if (g_eeGeneral.view == VIEW_TIMERS) {
    displayTimers();
  }
  else if (g_eeGeneral.view == VIEW_INPUTS) {
    // Sticks
    doMainScreenGraphics();
  }
  else {
    // Custom Switches
    uint8_t sw = 0;
    for (uint8_t line=0; line<4; line++) {
      for (uint8_t col=0; col<8; col++) {
        uint8_t x = LCD_W/2+7*FW+col*FW;
        uint8_t y = LCD_H/2-7+line*8;
        lcd_putcAtt(x, y, sw>=9 ? 'A'+sw-9 : '1'+sw, SMLSIZE);
        if (getSwitch(SWSRC_SW1+sw))
          lcd_filled_rect(x-1, y-1, 6, 8);
        sw++;
      }
    }
  }
#else // PCBTARANIS
  if (view_base < VIEW_INPUTS) {
    // scroll bar
    lcd_hlineStip(38, 34, 54, DOTTED);
#if defined(PCBSKY9X)
    lcd_hline(38 + (g_eeGeneral.view / ALTERNATE_VIEW) * 13, 34, 13, SOLID);
#else
    lcd_hline((g_eeGeneral.view & ALTERNATE_VIEW) ? 64 : 38, 34, 26, SOLID);
#endif

    for (uint8_t i=0; i<8; i++) {
      uint8_t x0,y0;
#if defined(PCBSKY9X)
      uint8_t chan = 8*(g_eeGeneral.view / ALTERNATE_VIEW) + i;
#else
      uint8_t chan = (g_eeGeneral.view & ALTERNATE_VIEW) ? 8+i : i;
#endif

      int16_t val = channelOutputs[chan];

      switch(view_base)
      {
        case VIEW_OUTPUTS_VALUES:
          x0 = (i%4*9+3)*FW/2;
          y0 = i/4*FH+40;
#if defined(PPM_UNIT_US)
          lcd_outdezAtt(x0+4*FW , y0, PPM_CH_CENTER(chan)+val/2, 0);
#elif defined(PPM_UNIT_PERCENT_PREC1)
          lcd_outdezAtt(x0+4*FW , y0, calcRESXto1000(val), PREC1);
#else
          lcd_outdezAtt(x0+4*FW , y0, calcRESXto1000(val)/10, 0); // G: Don't like the decimal part*
#endif
          break;

        case VIEW_OUTPUTS_BARS:
#define WBAR2 (50/2)
          x0       = i<4 ? LCD_W/4+2 : LCD_W*3/4-2;
          y0       = 38+(i%4)*5;

          uint16_t lim = g_model.extendedLimits ? 640*2 : 512*2;
          int8_t len = (abs(val) * WBAR2 + lim/2) / lim;

          if(len>WBAR2)  len = WBAR2;  // prevent bars from going over the end - comment for debugging
          lcd_hlineStip(x0-WBAR2, y0, WBAR2*2+1, DOTTED);
          lcd_vline(x0,y0-2,5);
          if (val>0)
            x0+=1;
          else
            x0-=len;
          lcd_hline(x0,y0+1,len);
          lcd_hline(x0,y0-1,len);
          break;
      }
    }
  }
  else if (view_base == VIEW_INPUTS) {
    if (view == VIEW_INPUTS) {
      // Sticks + Pots
      doMainScreenGraphics();

      // Switches
      for (uint8_t i=SWSRC_THR; i<=SWSRC_TRN; i++) {
        int8_t sw = (i == SWSRC_TRN ? (switchState(SW_ID0) ? SWSRC_ID0 : (switchState(SW_ID1) ? SWSRC_ID1 : SWSRC_ID2)) : i);
        uint8_t x = 2*FW-2, y = i*FH+1;
        if (i>=SWSRC_AIL) {
          x = 17*FW-1;
          y -= 3*FH;
        }
        putsSwitches(x, y, sw, getSwitch(i) ? INVERS : 0);
      }
    }
    else {
#if defined(PCBGRUVIN9X) && defined(ROTARY_ENCODERS)
      for (uint8_t i=0; i<NUM_ROTARY_ENCODERS; i++) {
        int16_t val = getRotaryEncoder(i);
        int8_t len = limit((int16_t)0, (int16_t)(((val+1024) * BAR_HEIGHT) / 2048), (int16_t)BAR_HEIGHT);
#if ROTARY_ENCODERS > 2
#define V_BAR_W 5
        V_BAR(LCD_W/2-8+V_BAR_W*i, LCD_H-8, len)
#else
#define V_BAR_W 5
        V_BAR(LCD_W/2-3+V_BAR_W*i, LCD_H-8, len)
#endif
      }
#endif // PCBGRUVIN9X && ROTARY_ENCODERS

      // Custom Switches
#if defined(PCBSKY9X)
      for (uint8_t i=0; i<NUM_CSW; i++) {
        int8_t len = getSwitch(SWSRC_SW1+i) ? BAR_HEIGHT : 1;
        uint8_t x = VSWITCH_X(i);
        lcd_vline(x-1, VSWITCH_Y-len, len);
        lcd_vline(x,   VSWITCH_Y-len, len);
      }
#elif defined(PCBGRUVIN9X) && ROTARY_ENCODERS > 2
      for (uint8_t i=0; i<NUM_CSW; i++)
        putsSwitches(2*FW-2 + (i/3)*(4*FW-2) + (i/3>1 ? 3*FW+6 : 0), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
#elif defined(PCBGRUVIN9X)
      for (uint8_t i=0; i<NUM_CSW; i++)
        putsSwitches(2*FW-2 + (i/3)*(4*FW-2) + (i/3>1 ? 3*FW : 0), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
#elif !defined(PCBSTD)
      for (uint8_t i=0; i<NUM_CSW; i++)
        putsSwitches(2*FW-2 + (i/3)*(4*FW-1), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
#else
      for (uint8_t i=0; i<NUM_CSW; i++)
        putsSwitches(2*FW-2 + (i/3)*(5*FW), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
#endif
    }
  }
  else { // timer2
    putsTime(33+FW+2+10*FWNUM-4, FH*5, timersStates[1].val, DBLSIZE, DBLSIZE);
    putsTmrMode(timersStates[1].val >= 0 ? 20-FW/2+5 : 20-FW/2-2, FH*6, g_model.timers[1].mode, STRCONDENSED);
    // lcd_outdezNAtt(33+11*FW, FH*6, s_timerVal_10ms[1], LEADING0, 2); // 1/100s
  }
#endif // PCBTARANIS

#if !defined(PCBTARANIS)
  // And ! in case of unexpected shutdown
  if (unexpectedShutdown) {
    lcd_putcAtt(REBOOT_X, 0*FH, '!', INVERS);
  }
#endif

  if (s_global_warning) {
    s_warning = s_global_warning;
    displayWarning(_event);
    if (!s_warning) s_global_warning = NULL;
    s_warning = NULL;
  }

#if defined(DSM2)
  if (dsm2Flag == DSM2_BIND_FLAG) // Issue 98
    lcd_putsAtt(15*FW, 0, PSTR("BIND"), 0);
#endif
}

