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

#define STATUS_BAR_Y     (7*FH+1)
#if LCD_W >= 212
  #define TELEM_2ND_COLUMN (11*FW)
#else
  #define TELEM_2ND_COLUMN (10*FW)
#endif

#if defined(FRSKY_HUB) && defined(GAUGES)
uint8_t barsThresholds[THLD_MAX];
#endif

enum FrskyViews {
  e_frsky_custom_screen_1,
  e_frsky_custom_screen_2,
  IF_CPUARM(e_frsky_custom_screen_3)
  e_frsky_voltages,
  e_frsky_after_flight,
  FRSKY_VIEW_MAX = e_frsky_after_flight
};

static uint8_t s_frsky_view = e_frsky_custom_screen_1;

#if defined(PCBTARANIS)
void displayRssiLine()
{
  if (TELEMETRY_STREAMING()) {
    lcd_hline(0, 55, 212, 0); // separator
    uint8_t rssi = min((uint8_t)99, frskyData.rssi[0].value);
    lcd_putsLeft(STATUS_BAR_Y, STR_RX); lcd_outdezNAtt(4*FW, STATUS_BAR_Y, rssi, LEADING0, 2);
    lcd_rect(25, 57, 78, 7);
    lcd_filled_rect(26, 58, 19*rssi/25, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
  }
  else {
    lcd_putsAtt(7*FW, STATUS_BAR_Y, STR_NODATA, BLINK);
    lcd_status_line();
  }
}
#else
void displayRssiLine()
{
  if (TELEMETRY_STREAMING()) {
    lcd_hline(0, 55, 128, 0); // separator
    uint8_t rssi = min((uint8_t)99, frskyData.rssi[1].value);
    lcd_putsLeft(STATUS_BAR_Y, STR_TX); lcd_outdezNAtt(4*FW, STATUS_BAR_Y, rssi, LEADING0, 2);
    lcd_rect(25, 57, 38, 7);
    lcd_filled_rect(26, 58, 4*rssi/11, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
    rssi = min((uint8_t)99, frskyData.rssi[0].value);
    lcd_puts(105, STATUS_BAR_Y, STR_RX); lcd_outdezNAtt(105+4*FW-1, STATUS_BAR_Y, rssi, LEADING0, 2);
    lcd_rect(65, 57, 38, 7);
    uint8_t v = 4*rssi/11;
    lcd_filled_rect(66+36-v, 58, v, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
  }
  else {
    lcd_putsAtt(7*FW, STATUS_BAR_Y, STR_NODATA, BLINK);
    lcd_status_line();
  }
}
#endif

#if defined(FRSKY) && defined(FRSKY_HUB) && defined(GPS)
void displayGpsTime()
{
  uint8_t att = (TELEMETRY_STREAMING() ? LEFT|LEADING0 : LEFT|LEADING0|BLINK);
  lcd_outdezNAtt(CENTER_OFS+6*FW+5, STATUS_BAR_Y, frskyData.hub.hour, att, 2);
  lcd_putcAtt(CENTER_OFS+8*FW+2, STATUS_BAR_Y, ':', att);
  lcd_outdezNAtt(CENTER_OFS+9*FW+2, STATUS_BAR_Y, frskyData.hub.min, att, 2);
  lcd_putcAtt(CENTER_OFS+11*FW-1, STATUS_BAR_Y, ':', att);
  lcd_outdezNAtt(CENTER_OFS+12*FW-1, STATUS_BAR_Y, frskyData.hub.sec, att, 2);
  lcd_status_line();
}

void displayGpsCoord(uint8_t y, char direction, int16_t bp, int16_t ap)
{
  if (frskyData.hub.gpsFix >= 0) {
    if (!direction) direction = '-';
    lcd_outdezAtt(TELEM_2ND_COLUMN, y, bp / 100, LEFT); // ddd before '.'
    lcd_putc(lcdLastPos, y, '@');
    uint8_t mn = bp % 100;
    if (g_eeGeneral.gpsFormat == 0) {
      lcd_putc(lcdLastPos+FWNUM, y, direction);
      lcd_outdezNAtt(lcdLastPos+FW+FW+1, y, mn, LEFT|LEADING0, 2); // mm before '.'
      lcd_vline(lcdLastPos, y, 2);
      uint16_t ss = ap * 6;
      lcd_outdezAtt(lcdLastPos+3, y, ss / 1000, LEFT); // ''
      lcd_plot(lcdLastPos, y+FH-2, 0); // small decimal point
      lcd_outdezAtt(lcdLastPos+2, y, ss % 1000, LEFT); // ''
      lcd_vline(lcdLastPos, y, 2);
      lcd_vline(lcdLastPos+2, y, 2);
    }
    else {
      lcd_outdezNAtt(lcdLastPos+FW, y, mn, LEFT|LEADING0, 2); // mm before '.'
      lcd_plot(lcdLastPos, y+FH-2, 0); // small decimal point
      lcd_outdezNAtt(lcdLastPos+2, y, ap, LEFT|UNSIGN|LEADING0, 4); // after '.'
      lcd_putc(lcdLastPos+1, y, direction);
    }
  }
  else {
    // no fix
    lcd_puts(TELEM_2ND_COLUMN, y, STR_VCSWFUNC+1/*----*/);
  }
}
#else
#define displayGpsTime()
#define displayGpsCoord(...)
#endif

NOINLINE uint8_t getRssiAlarmValue(uint8_t alarm)
{
  return (45 - 3*alarm + g_model.frsky.rssiAlarms[alarm].value);
}

void displayVoltageScreenLine(uint8_t y, uint8_t index)
{
  putsStrIdx(0, y, STR_A, index+1, 0);
  if (TELEMETRY_STREAMING()) {
    putsTelemetryChannel(3*FW+6*FW+4, y-FH+1, index+TELEM_A1-1, frskyData.analog[index].value, DBLSIZE);
    lcd_putc(12*FW-1, y-FH, '<'); putsTelemetryChannel(17*FW, y-FH, index+TELEM_A1-1, frskyData.analog[index].min, NO_UNIT);
    lcd_putc(12*FW, y, '>');      putsTelemetryChannel(17*FW, y, index+TELEM_A1-1, frskyData.analog[index].max, NO_UNIT);
  }
}

#if LCD_W >= 212
  #define BAR_WIDTH   160
#else
  #define BAR_WIDTH   100
#endif

uint8_t barCoord(int16_t value, int16_t min, int16_t max)
{
  return limit((uint8_t)0, (uint8_t)(((int32_t)(BAR_WIDTH-1) * (value - min)) / (max - min)), (uint8_t)BAR_WIDTH);
}

void menuTelemetryFrsky(uint8_t event)
{
  if (event == EVT_KEY_FIRST(KEY_EXIT)) {
    chainMenu(menuMainView);
    return;
  }

  switch (event) {
    case EVT_KEY_BREAK(KEY_UP):
      if (s_frsky_view-- == 0)
        s_frsky_view = FRSKY_VIEW_MAX;
      break;

#if defined(PCBTARANIS)
    case EVT_KEY_BREAK(KEY_PAGE):
#endif
    case EVT_KEY_BREAK(KEY_DOWN):
      if (s_frsky_view++ == FRSKY_VIEW_MAX)
        s_frsky_view = 0;
      break;

#if defined(PCBTARANIS)
    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(event);
      MENU_ADD_ITEM(STR_RESET_TELEMETRY);
      MENU_ADD_ITEM(STR_RESET_FLIGHT);
      menuHandler = onMainViewMenu;
      break;
#else
    case EVT_KEY_FIRST(KEY_ENTER):
      resetTelemetry();
      break;
#endif
  }

  lcdDrawTelemetryTopBar();

  if (s_frsky_view < MAX_FRSKY_SCREENS) {
    FrSkyScreenData & screen = g_model.frsky.screens[s_frsky_view];
#if defined(GAUGES)
    if (g_model.frsky.screensType & (1<<s_frsky_view)) {
      // Custom Screen with gauges
      uint8_t barHeight = 5;
      for (int8_t i=3; i>=0; i--) {
        FrSkyBarData & bar = screen.bars[i];
        uint8_t source = bar.source;
        getvalue_t barMin = convertTelemValue(source, bar.barMin);
        getvalue_t barMax = convertTelemValue(source, 255-bar.barMax);
        if (source && barMax > barMin) {
          uint8_t y = barHeight+6+i*(barHeight+6);
          lcd_putsiAtt(0, y+barHeight-5, STR_VTELEMCHNS, source, 0);
          lcd_rect(25, y, BAR_WIDTH+1, barHeight+2);
          getvalue_t value = getValue(MIXSRC_FIRST_TELEM+source-2);
#if LCD_W >= 212
          putsTelemetryChannel(27+BAR_WIDTH, y+barHeight-6, source-1, value, LEFT);
#endif
          getvalue_t threshold = 0;
          uint8_t thresholdX = 0;
          if (source <= TELEM_TM2)
            threshold = 0;
          else if (source <= TELEM_RSSI_RX)
            threshold = getRssiAlarmValue(source-TELEM_RSSI_TX);
          else if (source <= TELEM_A2)
            threshold = g_model.frsky.channels[source-TELEM_A1].alarms_value[0];
#if defined(FRSKY_HUB)
          else
            threshold = convertTelemValue(source, barsThresholds[source-TELEM_ALT]);
#endif

          if (threshold) {
            thresholdX = barCoord(threshold, barMin, barMax);
            if (thresholdX == 100)
              thresholdX = 0;
          }

          uint8_t width = barCoord(value, barMin, barMax);

          // reversed barshade for T1/T2
          uint8_t barShade = ((threshold > value) ? DOTTED : SOLID);
          if (source == TELEM_T1 || source == TELEM_T2)
            barShade = -barShade;

          lcd_filled_rect(26, y+1, width, barHeight, barShade);

          for (uint8_t j=24; j<99; j+=25)
            if (j>thresholdX || j>width) lcd_vline(j*BAR_WIDTH/100+26, y+1, barHeight);

          if (thresholdX) {
            lcd_vlineStip(26+thresholdX, y-2, barHeight+3, DOTTED);
            lcd_hline(25+thresholdX, y-2, 3);
          }
        }
        else {
          barHeight += 2;
        }
      }
      displayRssiLine();
    }
    else
#endif
    {
      // Custom Screen with numbers
      uint8_t fields_count = 0;
      for (uint8_t i=0; i<4; i++) {
        for (uint8_t j=0; j<NUM_LINE_ITEMS; j++) {
          uint8_t field = screen.lines[i].sources[j];
          if (i==3 && j==0) {
#if LCD_W >= 212
            lcd_vline(69, 8, 48);
            lcd_vline(141, 8, 48);
#else
            lcd_vline(63, 8, 48);
#endif
            if (TELEMETRY_STREAMING()) {
#if defined(FRSKY_HUB)
              if (field == TELEM_ACC) {
                lcd_putsLeft(STATUS_BAR_Y, STR_ACCEL);
                lcd_outdezNAtt(4*FW, STATUS_BAR_Y, frskyData.hub.accelX, LEFT|PREC2);
                lcd_outdezNAtt(10*FW, STATUS_BAR_Y, frskyData.hub.accelY, LEFT|PREC2);
                lcd_outdezNAtt(16*FW, STATUS_BAR_Y, frskyData.hub.accelZ, LEFT|PREC2);
                break;
              }
#if defined(GPS)
              else if (field == TELEM_GPS_TIME) {
                displayGpsTime();
                return;
              }
#endif
#endif
            }
            else {
              displayRssiLine();
              return;
            }
          }
          if (field) {
            fields_count++;
            getvalue_t value = getValue(MIXSRC_FIRST_TELEM+field-2);
            uint8_t att = (i==3 ? NO_UNIT : DBLSIZE|NO_UNIT);
#if LCD_W >= 212
            xcoord_t pos[] = {0, 71, 143, 214};
#else
            xcoord_t pos[] = {0, 65, 130};
#endif
            putsTelemetryChannel(pos[j+1]-2, 1+FH+2*FH*i, field-1, value, att);
            
            if (field >= TELEM_TM1 && field <= TELEM_TM2 && i!=3) {
              // there is not enough space on LCD for displaying "Tmr1" or "Tmr2" and still see the - sign, we write "T1" or "T2" instead
              field = field-TELEM_TM1+TELEM_T1;
            }
            lcd_putsiAtt(pos[j], 1+FH+2*FH*i, STR_VTELEMCHNS, field, 0);
          }
        }
      }
      lcd_status_line();
      if (fields_count == 0)
        putEvent(event == EVT_KEY_BREAK(KEY_UP) ? event : EVT_KEY_BREAK(KEY_DOWN));
    }
  }
  else if (s_frsky_view == e_frsky_voltages) {
    // Volts / Amps / Watts / mAh
    uint8_t analog = 0;
    lcd_putsiAtt(0, 2*FH, STR_VOLTSRC, g_model.frsky.voltsSource+1, 0);
    switch(g_model.frsky.voltsSource) {
      case 0:
      case 1:
        displayVoltageScreenLine(2*FH, g_model.frsky.voltsSource);
        analog = 1+g_model.frsky.voltsSource;
        break;
#if defined(FRSKY_HUB)
      case 2:
        putsTelemetryChannel(3*FW+6*FW+4, FH+1, TELEM_VFAS-1, frskyData.hub.vfas, DBLSIZE);
        break;
      case 3:
        putsTelemetryChannel(3*FW+6*FW+4, FH+1, TELEM_CELLS_SUM-1, frskyData.hub.cellsSum, DBLSIZE);
        break;
#endif
    }

    if (g_model.frsky.currentSource) {
      lcd_putsiAtt(0, 4*FH, STR_VOLTSRC, g_model.frsky.currentSource, 0);
      switch(g_model.frsky.currentSource) {
        case 1:
        case 2:
          displayVoltageScreenLine(4*FH, g_model.frsky.currentSource-1);
          break;
#if defined(FRSKY_HUB)
        case 3:
          putsTelemetryChannel(3*FW+6*FW+4, 3*FH+1, TELEM_CURRENT-1, frskyData.hub.current, DBLSIZE);
          break;
#endif
      }

      putsTelemetryChannel(4, 5*FH+1, TELEM_POWER-1, frskyData.hub.power, LEFT|DBLSIZE);
      putsTelemetryChannel(3*FW+4+4*FW+6*FW+FW, 5*FH+1, TELEM_CONSUMPTION-1, frskyData.hub.currentConsumption, DBLSIZE);
    }
    else {
      displayVoltageScreenLine(analog > 0 ? 5*FH : 4*FH, analog ? 2-analog : 0);
      if (analog == 0) displayVoltageScreenLine(6*FH, 1);
    }

#if defined(FRSKY_HUB)
    // Cells voltage
    if (frskyData.hub.cellsCount > 0) {
      uint8_t y = 1*FH;
      for (uint8_t k=0; k<frskyData.hub.cellsCount && k<6; k++) {
#if defined(GAUGES)
        uint8_t attr = (barsThresholds[THLD_CELL] && frskyData.hub.cellVolts[k] < barsThresholds[THLD_CELL]) ? BLINK|PREC2 : PREC2;
#else
        uint8_t attr = PREC2;
#endif
        lcd_outdezNAtt(LCD_W, y, frskyData.hub.cellVolts[k] * 2, attr, 4);
        y += 1*FH;
      }
      lcd_vline(LCD_W-3*FW-2, 8, 47);
    }
#endif

    displayRssiLine();
  }
#if defined(FRSKY_HUB)
  else if (s_frsky_view == e_frsky_after_flight) {
    uint8_t line=1*FH+1;
    if (IS_GPS_AVAILABLE()) {
      // Latitude
      lcd_putsLeft(line, STR_LATITUDE);
      displayGpsCoord(line, frskyData.hub.gpsLatitudeNS, frskyData.hub.gpsLatitude_bp, frskyData.hub.gpsLatitude_ap);
      // Longitude
      line+=1*FH+1;
      lcd_putsLeft(line, STR_LONGITUDE);
      displayGpsCoord(line, frskyData.hub.gpsLongitudeEW, frskyData.hub.gpsLongitude_bp, frskyData.hub.gpsLongitude_ap);
      displayGpsTime();
      line+=1*FH+1;
    }
    // Rssi
    lcd_putsLeft(line, STR_MINRSSI);
#if defined(PCBTARANIS)
    lcd_outdezNAtt(TELEM_2ND_COLUMN, line, frskyData.rssi[0].min, LEFT|LEADING0, 2);
#else
    lcd_puts(TELEM_2ND_COLUMN, line, STR_TX);
    lcd_outdezNAtt(TELEM_2ND_COLUMN+3*FW, line, frskyData.rssi[1].min, LEFT|LEADING0, 2);
    lcd_puts(TELEM_2ND_COLUMN+6*FW, line, STR_RX);
    lcd_outdezNAtt(TELEM_2ND_COLUMN+9*FW, line, frskyData.rssi[0].min, LEFT|LEADING0, 2);
#endif
  }
#endif    
}
