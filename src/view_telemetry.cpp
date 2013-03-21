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

#include "opentx.h"
#include "menus.h"

#if defined(FRSKY_HUB)
uint8_t barsThresholds[THLD_MAX];
#endif

uint8_t maxTelemValue(uint8_t channel)
{
  switch (channel) {
    case TELEM_FUEL:
    case TELEM_RSSI_TX:
    case TELEM_RSSI_RX:
      return 100;
    case TELEM_HDG:
      return 180;
    default:
      return 255;
  }
}

int16_t convertTelemValue(uint8_t channel, uint8_t value)
{
  int16_t result;
  switch (channel) {
    case TELEM_TM1:
    case TELEM_TM2:
      result = value * 3;
      break;
    case TELEM_ALT:
    case TELEM_GPSALT:
    case TELEM_MAX_ALT:
    case TELEM_MIN_ALT:
      result = value * 8 - 500;
      break;
    case TELEM_RPM:
    case TELEM_MAX_RPM:
      result = value * 50;
      break;
    case TELEM_T1:
    case TELEM_T2:
    case TELEM_MAX_T1:
    case TELEM_MAX_T2:
      result = (int16_t)value - 30;
      break;
    case TELEM_CELL:
    case TELEM_HDG:
      result = value * 2;
      break;
    case TELEM_DIST:
    case TELEM_MAX_DIST:
      result = value * 8;
      break;
    case TELEM_CURRENT:
    case TELEM_POWER:
      result = value * 5;
      break;
    case TELEM_CONSUMPTION:
      result = value * 20;
      break;
    default:
      result = value;
      break;
  }
  return result;
}

int16_t convertCswTelemValue(CustomSwData * cs)
{
  int16_t val;
  if (CS_STATE(cs->func)==CS_VOFS)
    val = convertTelemValue(cs->v1 - MIXSRC_FIRST_TELEM + 1, 128+cs->v2);
  else
    val = convertTelemValue(cs->v1 - MIXSRC_FIRST_TELEM + 1, 128+cs->v2) - convertTelemValue(cs->v1 - MIXSRC_FIRST_TELEM + 1, 128);
  return val;
}

const pm_uint8_t bchunit_ar[] PROGMEM = {
  UNIT_METERS,  // Alt
  UNIT_RAW,     // Rpm
  UNIT_PERCENT, // Fuel
  UNIT_DEGREES, // T1
  UNIT_DEGREES, // T2
  UNIT_KTS,     // Speed
  UNIT_METERS,  // Dist
  UNIT_METERS,  // GPS Alt
};

void putsTelemetryChannel(xcoord_t x, uint8_t y, uint8_t channel, lcdint_t val, uint8_t att)
{
  switch (channel) {
    case TELEM_TM1-1:
    case TELEM_TM2-1:
      att &= ~NO_UNIT;
      putsTime(x, y, val, att, att);
      break;
    case TELEM_MIN_A1-1:
    case TELEM_MIN_A2-1:
      channel -= TELEM_MIN_A1-TELEM_A1;
      // no break
    case TELEM_A1-1:
    case TELEM_A2-1:
      channel -= TELEM_A1-1;
      // A1 and A2
    {
      lcdint_t converted_value = applyChannelRatio(channel, val);
      if (g_model.frsky.channels[channel].type >= UNIT_RAW) {
        converted_value /= 10;
      }
      else {
#if !defined(PCBTARANIS)
        if (abs(converted_value) < 1000) {
          att |= PREC2;
        }
        else {
          converted_value /= 10;
          att |= PREC1;
        }
#else
        att |= PREC2;
#endif
      }
      putsTelemetryValue(x, y, converted_value, g_model.frsky.channels[channel].type, att);
      break;
    }

    case TELEM_CELL-1:
      putsTelemetryValue(x, y, val, UNIT_VOLTS, att|PREC2);
      break;

    case TELEM_TX_VOLTAGE-1:
    case TELEM_VFAS-1:
    case TELEM_CELLS_SUM-1:
      putsTelemetryValue(x, y, val, UNIT_VOLTS, att|PREC1);
      break;

    case TELEM_CURRENT-1:
    case TELEM_MAX_CURRENT-1:
      putsTelemetryValue(x, y, val, UNIT_AMPS, att|PREC1);
      break;

    case TELEM_CONSUMPTION-1:
      putsTelemetryValue(x, y, val, UNIT_MAH, att);
      break;

    case TELEM_POWER-1:
      putsTelemetryValue(x, y, val, UNIT_WATTS, att);
      break;

    case TELEM_ACCx-1:
    case TELEM_ACCy-1:
    case TELEM_ACCz-1:
    case TELEM_VSPD-1:
      putsTelemetryValue(x, y, val, UNIT_RAW, att|PREC2);
      break;

    case TELEM_RSSI_TX-1:
    case TELEM_RSSI_RX-1:
      putsTelemetryValue(x, y, val, UNIT_RAW, att);
      break;

#if defined(IMPERIAL_UNITS)
    case TELEM_ALT-1:
    case TELEM_MIN_ALT-1:
    case TELEM_MAX_ALT-1:
      if (g_model.frsky.usrProto == USR_PROTO_WS_HOW_HIGH) {
        putsTelemetryValue(x, y, val, UNIT_FEET, att);
        break;
      }
      // no break
#endif

    default:
    {
      uint8_t unit = 1;
      if (channel >= TELEM_MAX_T1-1 && channel <= TELEM_MAX_DIST-1)
        channel -= TELEM_MAX_T1 - TELEM_T1;
      if (channel <= TELEM_GPSALT-1)
        unit = channel + 1 - TELEM_ALT;
      if (channel >= TELEM_MIN_ALT-1 && channel <= TELEM_MAX_ALT-1)
        unit = 0;
      if (channel == TELEM_HDG-1)
        unit = 3;
      putsTelemetryValue(x, y, val, pgm_read_byte(bchunit_ar+unit), att);
      break;
    }
  }
}

enum FrskyViews {
  e_frsky_custom_screen_1,
  e_frsky_custom_screen_2,
  IF_CPUARM(e_frsky_custom_screen_3)
  e_frsky_voltages,
  e_frsky_after_flight,
  FRSKY_VIEW_MAX = e_frsky_after_flight
};

static uint8_t s_frsky_view = e_frsky_custom_screen_1;

#if LCD_W == 212
void displayRssiLine()
{
  if (frskyStreaming > 0) {
    lcd_hline(0, 55, 212, 0); // separator
    uint8_t rssi = min((uint8_t)99, frskyData.rssi[1].value);
    lcd_putsLeft(7*FH+1, STR_TX); lcd_outdezNAtt(4*FW, 7*FH+1, rssi, LEADING0, 2);
    lcd_rect(25, 57, 78, 7);
    lcd_filled_rect(26, 58, 19*rssi/25, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
    rssi = min((uint8_t)99, frskyData.rssi[0].value);
    lcd_puts(190, 7*FH+1, STR_RX); lcd_outdezNAtt(189+4*FW-1, 7*FH+1, rssi, LEADING0, 2);
    lcd_rect(110, 57, 78, 7);
    uint8_t v = 19*rssi/25;
    lcd_filled_rect(111+76-v, 58, v, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
  }
  else {
    lcd_putsAtt(7*FW, 7*FH+1, STR_NODATA, BLINK);
    lcd_status_line();
  }
}
#else
void displayRssiLine()
{
  if (frskyStreaming > 0) {
    lcd_hline(0, 55, 128, 0); // separator
    uint8_t rssi = min((uint8_t)99, frskyData.rssi[1].value);
    lcd_putsLeft(7*FH+1, STR_TX); lcd_outdezNAtt(4*FW, 7*FH+1, rssi, LEADING0, 2);
    lcd_rect(25, 57, 38, 7);
    lcd_filled_rect(26, 58, 4*rssi/11, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
    rssi = min((uint8_t)99, frskyData.rssi[0].value);
    lcd_puts(105, 7*FH+1, STR_RX); lcd_outdezNAtt(105+4*FW-1, 7*FH+1, rssi, LEADING0, 2);
    lcd_rect(65, 57, 38, 7);
    uint8_t v = 4*rssi/11;
    lcd_filled_rect(66+36-v, 58, v, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
  }
  else {
    lcd_putsAtt(7*FW, 7*FH+1, STR_NODATA, BLINK);
    lcd_status_line();
  }
}
#endif

#if defined(FRSKY_HUB)
void displayGpsTime()
{
#define TIME_LINE (7*FH+1)
  uint8_t att = (frskyStreaming > 0 ? LEFT|LEADING0 : LEFT|LEADING0|BLINK);
  lcd_outdezNAtt(6*FW+5, TIME_LINE, frskyData.hub.hour, att, 2);
  lcd_putcAtt(8*FW+2, TIME_LINE, ':', att);
  lcd_outdezNAtt(9*FW+2, TIME_LINE, frskyData.hub.min, att, 2);
  lcd_putcAtt(11*FW-1, TIME_LINE, ':', att);
  lcd_outdezNAtt(12*FW-1, TIME_LINE, frskyData.hub.sec, att, 2);
  lcd_status_line();
}

void displayGpsCoord(uint8_t y, char direction, int16_t bp, int16_t ap)
{
  if (frskyData.hub.gpsFix >= 0) {
    if (!direction) direction = '-';
    lcd_outdezAtt(10*FW, y, bp / 100, LEFT); // ddd before '.'
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
    lcd_puts(10*FW, y, STR_VCSWFUNC+1/*----*/);
  }
}
#endif

NOINLINE uint8_t getRssiAlarmValue(uint8_t alarm)
{
  return (50 + g_model.frsky.rssiAlarms[alarm].value);
}

void displayVoltageScreenLine(uint8_t y, uint8_t index)
{
  putsStrIdx(0, y, STR_A, index+1, 0);
  putsTelemetryChannel(3*FW+6*FW+4, y, index+TELEM_A1-1, frskyData.analog[index].value, DBLSIZE);
  lcd_putc(12*FW-1, y-FH, '<'); putsTelemetryChannel(17*FW, y-FH, index+TELEM_A1-1, frskyData.analog[index].min, NO_UNIT);
  lcd_putc(12*FW, y, '>');      putsTelemetryChannel(17*FW, y, index+TELEM_A1-1, frskyData.analog[index].max, NO_UNIT);
}

uint8_t barCoord(int16_t value, int16_t min, int16_t max)
{
  return limit((uint8_t)0, (uint8_t)(((int32_t)99 * (value - min)) / (max - min)), (uint8_t)100);
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

    case EVT_KEY_BREAK(KEY_DOWN):
      if (s_frsky_view++ == FRSKY_VIEW_MAX)
        s_frsky_view = 0;
      break;

    case EVT_KEY_FIRST(KEY_MENU):
      resetTelemetry();
      break;
  }

  lcdDrawTelemetryTopBar();

  if (frskyStreaming >= 0) {
    if (s_frsky_view < MAX_FRSKY_SCREENS) {
      FrSkyScreenData & screen = g_model.frsky.screens[s_frsky_view];
      if (g_model.frsky.screensType & (1<<s_frsky_view)) {
        // Custom Screen with gauges
        uint8_t barHeight = 5;
        for (int8_t i=3; i>=0; i--) {
          FrSkyBarData & bar = screen.bars[i];
          uint8_t source = bar.source;
          int16_t barMin = convertTelemValue(source, bar.barMin);
          int16_t barMax = convertTelemValue(source, 255-bar.barMax);
          if (source && barMax > barMin) {
            uint8_t y = barHeight+6+i*(barHeight+6);
            lcd_putsiAtt(0, y+barHeight-5, STR_VTELEMCHNS, source, 0);
            lcd_rect(25, y, 101, barHeight+2);
            int16_t value = getValue(MIXSRC_FIRST_TELEM+source-2);
            int16_t threshold = 0;
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

            for (uint8_t j=50; j<125; j+=25)
              if (j>26+thresholdX || j>26+width) lcd_vline(j, y+1, barHeight);

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
      else {
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
              if (frskyStreaming > 0) {
#if defined(FRSKY_HUB)
                if (field == TELEM_ACC) {
                  lcd_putsLeft(7*FH+1, STR_ACCEL);
                  lcd_outdezNAtt(4*FW, 7*FH+1, frskyData.hub.accelX, LEFT|PREC2);
                  lcd_outdezNAtt(10*FW, 7*FH+1, frskyData.hub.accelY, LEFT|PREC2);
                  lcd_outdezNAtt(16*FW, 7*FH+1, frskyData.hub.accelZ, LEFT|PREC2);
                  break;
                }
                else if (field == TELEM_GPS_TIME) {
                  displayGpsTime();
                  return;
                }
#endif
              }
              else {
                displayRssiLine();
                return;
              }
            }
            if (field) {
              fields_count++;
              int16_t value = getValue(MIXSRC_FIRST_TELEM+field-2);
              uint8_t att = (i==3 ? NO_UNIT : DBLSIZE|NO_UNIT);
#if LCD_W >= 212
              xcoord_t pos[] = {0, 71, 143, 214};
#else
              xcoord_t pos[] = {0, 65, 130};
#endif
              putsTelemetryChannel(pos[j+1]-2, 1+FH+2*FH*i, field-1, value, att);
#if LCD_W < 212              
              if (field >= TELEM_TM1 && field <= TELEM_TM2 && i!=3) {
                // there is not enough space on LCD for displaying "Tmr1" or "Tmr2", we write "T1" or "T2" instead
                field = field-TELEM_TM1+TELEM_T1;
              }
#endif
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
      uint8_t other = 0;
      lcd_putsiAtt(0, 2*FH, STR_VOLTSRC, g_model.frsky.voltsSource+1, 0);
      switch(g_model.frsky.voltsSource) {
        case 0:
        case 1:
          displayVoltageScreenLine(2*FH, g_model.frsky.voltsSource);
          other = !g_model.frsky.voltsSource;
          break;
#if defined(FRSKY_HUB)
        case 2:
          putsTelemetryChannel(3*FW+6*FW+4, 2*FH, TELEM_VFAS-1, frskyData.hub.vfas, DBLSIZE);
          break;
        case 3:
          putsTelemetryChannel(3*FW+6*FW+4, 2*FH, TELEM_CELLS_SUM-1, frskyData.hub.cellsSum, DBLSIZE);
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
            putsTelemetryChannel(3*FW+6*FW+4, 4*FH, TELEM_CURRENT-1, frskyData.hub.current, DBLSIZE);
            break;
#endif
        }

        putsTelemetryChannel(4, 6*FH, TELEM_POWER-1, frskyData.power, LEFT|DBLSIZE);
        putsTelemetryChannel(3*FW+4+4*FW+6*FW+FW, 6*FH, TELEM_CONSUMPTION-1, frskyData.currentConsumption, DBLSIZE);
      }
      else {
        displayVoltageScreenLine(other ? 5*FH : 4*FH, other);
        if (!other) displayVoltageScreenLine(6*FH, 1);
      }

#if defined(FRSKY_HUB)
      // Cells voltage
      if (frskyData.hub.cellsCount > 0) {
        uint8_t y = 1*FH;
        for (uint8_t k=0; k<frskyData.hub.cellsCount && k<6; k++) {
          uint8_t attr = (barsThresholds[THLD_CELL] && frskyData.hub.cellVolts[k] < barsThresholds[THLD_CELL]) ? BLINK|PREC2 : PREC2;
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
      if (g_model.frsky.usrProto == USR_PROTO_FRSKY) {
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
      lcd_puts(10*FW, line, STR_TX);
      lcd_outdezNAtt(lcdLastPos, line, frskyData.rssi[1].min, LEFT|LEADING0, 2);
      lcd_puts(16*FW, line, STR_RX);
      lcd_outdezNAtt(lcdLastPos, line, frskyData.rssi[0].min, LEFT|LEADING0, 2);
    }
#endif    
  }
  else {
    lcd_putsAtt(22, 40, STR_NODATA, DBLSIZE);
  }
}
