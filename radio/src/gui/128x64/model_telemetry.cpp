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

#include "opentx.h"

enum MenuModelTelemetryFrskyItems {
  CASE_CPUARM(ITEM_TELEMETRY_PROTOCOL_TYPE)
#if defined(REVX)
  ITEM_TELEMETRY_INVERTED_SERIAL,
#endif
#if !defined(CPUARM)
  ITEM_TELEMETRY_A1_LABEL,
  ITEM_TELEMETRY_A1_RANGE,
  ITEM_TELEMETRY_A1_OFFSET,
  ITEM_TELEMETRY_A1_ALARM1,
  ITEM_TELEMETRY_A1_ALARM2,
  ITEM_TELEMETRY_A2_LABEL,
  ITEM_TELEMETRY_A2_RANGE,
  ITEM_TELEMETRY_A2_OFFSET,
  ITEM_TELEMETRY_A2_ALARM1,
  ITEM_TELEMETRY_A2_ALARM2,
#endif
  ITEM_TELEMETRY_RSSI_LABEL,
  ITEM_TELEMETRY_RSSI_ALARM1,
  ITEM_TELEMETRY_RSSI_ALARM2,
#if defined(CPUARM)
  ITEM_TELEMETRY_DISABLE_ALARMS,
  ITEM_TELEMETRY_SENSORS_LABEL,
  ITEM_TELEMETRY_SENSOR1,
  ITEM_TELEMETRY_SENSOR2,
  ITEM_TELEMETRY_SENSOR3,
  ITEM_TELEMETRY_SENSOR4,
  ITEM_TELEMETRY_SENSOR5,
  ITEM_TELEMETRY_SENSOR6,
  ITEM_TELEMETRY_SENSOR7,
  ITEM_TELEMETRY_SENSOR8,
  ITEM_TELEMETRY_SENSOR9,
  ITEM_TELEMETRY_SENSOR10,
  ITEM_TELEMETRY_SENSOR11,
  ITEM_TELEMETRY_SENSOR12,
  ITEM_TELEMETRY_SENSOR13,
  ITEM_TELEMETRY_SENSOR14,
  ITEM_TELEMETRY_SENSOR15,
  ITEM_TELEMETRY_SENSOR16,
  ITEM_TELEMETRY_SENSOR17,
  ITEM_TELEMETRY_SENSOR18,
  ITEM_TELEMETRY_SENSOR19,
  ITEM_TELEMETRY_SENSOR20,
  ITEM_TELEMETRY_SENSOR21,
  ITEM_TELEMETRY_SENSOR22,
  ITEM_TELEMETRY_SENSOR23,
  ITEM_TELEMETRY_SENSOR24,
  ITEM_TELEMETRY_SENSOR25,
  ITEM_TELEMETRY_SENSOR26,
  ITEM_TELEMETRY_SENSOR27,
  ITEM_TELEMETRY_SENSOR28,
  ITEM_TELEMETRY_SENSOR29,
  ITEM_TELEMETRY_SENSOR30,
  ITEM_TELEMETRY_SENSOR31,
  ITEM_TELEMETRY_SENSOR32,
  ITEM_TELEMETRY_DISCOVER_SENSORS,
  ITEM_TELEMETRY_NEW_SENSOR,
  ITEM_TELEMETRY_DELETE_ALL_SENSORS,
  ITEM_TELEMETRY_IGNORE_SENSOR_INSTANCE,
#endif
#if !defined(CPUARM)
#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
  ITEM_TELEMETRY_USR_LABEL,
  ITEM_TELEMETRY_USR_PROTO,
  ITEM_TELEMETRY_USR_BLADES,
#endif
  ITEM_TELEMETRY_USR_VOLTAGE_SOURCE,
  ITEM_TELEMETRY_USR_CURRENT_SOURCE,
#if defined(FAS_OFFSET) || !defined(CPUM64)
  ITEM_TELEMETRY_FAS_OFFSET,
#endif
#endif
#if defined(VARIO)
  ITEM_TELEMETRY_VARIO_LABEL,
  ITEM_TELEMETRY_VARIO_SOURCE,
  ITEM_TELEMETRY_VARIO_RANGE,
  ITEM_TELEMETRY_VARIO_CENTER,
#endif
#if !defined(CPUARM)
  // TODO check the cost of moving them to a new screen on the 9X
  ITEM_TELEMETRY_SCREEN_LABEL1,
  ITEM_TELEMETRY_SCREEN_LINE1,
  ITEM_TELEMETRY_SCREEN_LINE2,
  ITEM_TELEMETRY_SCREEN_LINE3,
  ITEM_TELEMETRY_SCREEN_LINE4,
  ITEM_TELEMETRY_SCREEN_LABEL2,
  ITEM_TELEMETRY_SCREEN_LINE5,
  ITEM_TELEMETRY_SCREEN_LINE6,
  ITEM_TELEMETRY_SCREEN_LINE7,
  ITEM_TELEMETRY_SCREEN_LINE8,
#endif
  ITEM_TELEMETRY_MAX
};

#if defined(TELEMETRY_FRSKY)
  #define TELEM_COL1                   INDENT_WIDTH
  #if defined(TRANSLATIONS_CZ)
    #define TELEM_COL2                 (9*FW)
  #else
    #define TELEM_COL2                 (8*FW)
  #endif
  #define TELEM_BARS_COLMIN            (56-3*FW)
  #define TELEM_BARS_COLMAX            (14*FW-3)
  #define TELEM_SCRTYPE_COL            TELEM_COL2

#define IS_RANGE_DEFINED(k)            (g_model.frsky.channels[k].ratio > 0)

#if defined(CPUARM)
  #define CHANNELS_ROWS
  #define SENSOR_ROWS(x)               (isTelemetryFieldAvailable(x) ? (uint8_t)0 : HIDDEN_ROW)
  #define SENSORS_ROWS                 LABEL(Sensors), SENSOR_ROWS(0), SENSOR_ROWS(1), SENSOR_ROWS(2), SENSOR_ROWS(3), SENSOR_ROWS(4), SENSOR_ROWS(5), SENSOR_ROWS(6), SENSOR_ROWS(7), SENSOR_ROWS(8), SENSOR_ROWS(9), SENSOR_ROWS(10), SENSOR_ROWS(11), SENSOR_ROWS(12), SENSOR_ROWS(13), SENSOR_ROWS(14), SENSOR_ROWS(15), SENSOR_ROWS(16), SENSOR_ROWS(17), SENSOR_ROWS(18), SENSOR_ROWS(19), SENSOR_ROWS(20), SENSOR_ROWS(21), SENSOR_ROWS(22), SENSOR_ROWS(23), SENSOR_ROWS(24), SENSOR_ROWS(25), SENSOR_ROWS(26), SENSOR_ROWS(27), SENSOR_ROWS(28), SENSOR_ROWS(29), SENSOR_ROWS(30), SENSOR_ROWS(31), 0, 0, 0, 0,
#else
  #define CHANNEL_ROWS(x)              LABEL(CHANNEL), 1, 0, 2, 2
  #define CHANNELS_ROWS                CHANNEL_ROWS(0), CHANNEL_ROWS(1),
  #define SENSORS_ROWS
#endif

#if defined(FAS_OFFSET) || !defined(CPUM64)
  #define IF_FAS_OFFSET(x)             x,
#else
  #define IF_FAS_OFFSET(x)
#endif

#if defined(CPUARM)
  #define USRDATA_ROWS
#elif defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
  #define USRDATA_ROWS                 LABEL(UsrData), 0, 0, 0, 0, IF_FAS_OFFSET(0)
#else
  #define USRDATA_ROWS                 0, 0, IF_FAS_OFFSET(0)
#endif

#if defined(CPUARM)
  #define RSSI_ROWS                      LABEL(RSSI), 0, 0, 1,
#else
  #define RSSI_ROWS                      LABEL(RSSI), 1, 1,
#endif

#if defined(CPUARM) || defined(GAUGES)
  #define SCREEN_TYPE_ROWS             0
#else
  #define SCREEN_TYPE_ROWS             LABEL(SCREEN)
#endif

#if defined(PCBSTD)
  #define VARIO_ROWS             1,
#elif defined(CPUARM)
  #define VARIO_ROWS             LABEL(Vario), 0, 1, 2,
#else
  #define VARIO_ROWS             LABEL(Vario), 0, 1, 1,
#endif


#if defined (PCBTARANIS)
  #define TELEMETRY_TYPE_SHOW_TELEMETRY  (! IS_INTERNAL_MODULE_ENABLED() && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM) ? (uint8_t)0 : HIDDEN_ROW
#elif defined (CPUARM)
  #define TELEMETRY_TYPE_SHOW_TELEMETRY  (g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM) ? (uint8_t)0 : HIDDEN_ROW
#endif


#if defined(REVX)
  #define TELEMETRY_TYPE_ROWS          TELEMETRY_TYPE_SHOW_TELEMETRY, TELEMETRY_TYPE_SHOW_TELEMETRY,
#elif defined(CPUARM)
  #define TELEMETRY_TYPE_ROWS          TELEMETRY_TYPE_SHOW_TELEMETRY,
#else
  #define TELEMETRY_TYPE_ROWS
#endif

#if defined(CPUARM)
  #define TELEMETRY_SCREENS_ROWS
#else
  #define TELEMETRY_SCREEN_ROWS(x)     SCREEN_TYPE_ROWS, 2, 2, 2, 2
  #define TELEMETRY_CURRENT_CHANNEL(k) (k >= ITEM_TELEMETRY_A2_LABEL ? TELEM_ANA_A2 : TELEM_ANA_A1)
  #define TELEMETRY_SCREENS_ROWS       TELEMETRY_SCREEN_ROWS(0), TELEMETRY_SCREEN_ROWS(1)
#endif

#if defined(CPUARM)
enum SensorFields {
  SENSOR_FIELD_NAME,
  SENSOR_FIELD_TYPE,
  SENSOR_FIELD_ID,
  SENSOR_FIELD_FORMULA=SENSOR_FIELD_ID,
  SENSOR_FIELD_UNIT,
  SENSOR_FIELD_PRECISION,
  SENSOR_FIELD_PARAM1,
  SENSOR_FIELD_PARAM2,
  SENSOR_FIELD_PARAM3,
  SENSOR_FIELD_PARAM4,
  SENSOR_FIELD_AUTOOFFSET,
  SENSOR_FIELD_ONLYPOSITIVE,
  SENSOR_FIELD_FILTER,
  SENSOR_FIELD_PERSISTENT,
  SENSOR_FIELD_LOGS,
  SENSOR_FIELD_MAX
};

#define SENSOR_2ND_COLUMN (12*FW)
#define SENSOR_3RD_COLUMN (18*FW)

#define SENSOR_UNIT_ROWS       ((sensor->type == TELEM_TYPE_CALCULATED && (sensor->formula == TELEM_FORMULA_DIST)) || sensor->isConfigurable() ? (uint8_t)0 : HIDDEN_ROW)
#define SENSOR_PREC_ROWS       (sensor->isPrecConfigurable() && sensor->unit != UNIT_FAHRENHEIT  ? (uint8_t)0 : HIDDEN_ROW)
#define SENSOR_PARAM1_ROWS     (sensor->unit >= UNIT_FIRST_VIRTUAL ? HIDDEN_ROW : (uint8_t)0)
#define SENSOR_PARAM2_ROWS     (sensor->unit == UNIT_GPS || sensor->unit == UNIT_DATETIME || sensor->unit == UNIT_CELLS || (sensor->type==TELEM_TYPE_CALCULATED && (sensor->formula==TELEM_FORMULA_CONSUMPTION || sensor->formula==TELEM_FORMULA_TOTALIZE)) ? HIDDEN_ROW : (uint8_t)0)
#define SENSOR_PARAM3_ROWS     (sensor->type == TELEM_TYPE_CALCULATED && sensor->formula < TELEM_FORMULA_MULTIPLY) ? (uint8_t)0 : HIDDEN_ROW
#define SENSOR_PARAM4_ROWS     (sensor->type == TELEM_TYPE_CALCULATED && sensor->formula < TELEM_FORMULA_MULTIPLY) ? (uint8_t)0 : HIDDEN_ROW
#define SENSOR_AUTOOFFSET_ROWS (sensor->unit != UNIT_RPMS && sensor->isConfigurable() ? (uint8_t)0 : HIDDEN_ROW)
#define SENSOR_ONLYPOS_ROWS    (sensor->isConfigurable() ? (uint8_t)0 : HIDDEN_ROW)
#define SENSOR_FILTER_ROWS     (sensor->isConfigurable() ? (uint8_t)0 : HIDDEN_ROW)
#define SENSOR_PERSISTENT_ROWS (sensor->type == TELEM_TYPE_CALCULATED ? (uint8_t)0 : HIDDEN_ROW)

void menuModelSensor(event_t event)
{
  TelemetrySensor * sensor = & g_model.telemetrySensors[s_currIdx];

  SUBMENU(STR_MENUSENSOR, SENSOR_FIELD_MAX, {0, 0, sensor->type == TELEM_TYPE_CALCULATED ? (uint8_t)0 : (uint8_t)1, SENSOR_UNIT_ROWS, SENSOR_PREC_ROWS, SENSOR_PARAM1_ROWS, SENSOR_PARAM2_ROWS, SENSOR_PARAM3_ROWS, SENSOR_PARAM4_ROWS, SENSOR_AUTOOFFSET_ROWS, SENSOR_ONLYPOS_ROWS, SENSOR_FILTER_ROWS, SENSOR_PERSISTENT_ROWS, 0 });
  lcdDrawNumber(PSIZE(TR_MENUSENSOR)*FW+1, 0, s_currIdx+1, INVERS|LEFT);

  if (!isGPSSensor(s_currIdx+1)) drawSensorCustomValue(SENSOR_2ND_COLUMN, 0, s_currIdx, getValue(MIXSRC_FIRST_TELEM+3*s_currIdx), LEFT);

  int8_t sub = menuVerticalPosition;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i + menuVerticalOffset;

    for (int j=0; j<k; j++) {
      if (mstate_tab[j+1] == HIDDEN_ROW)
        k++;
    }

    LcdFlags attr = (sub==k ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);

    switch (k) {

      case SENSOR_FIELD_NAME:
        editSingleName(SENSOR_2ND_COLUMN, y, STR_NAME, sensor->label, TELEM_LABEL_LEN, event, attr);
        break;

      case SENSOR_FIELD_TYPE:
        sensor->type = editChoice(SENSOR_2ND_COLUMN, y, NO_INDENT(STR_TYPE), STR_VSENSORTYPES, sensor->type, 0, 1, attr, event);
        if (attr && checkIncDec_Ret) {
          sensor->instance = 0;
          if (sensor->type == TELEM_TYPE_CALCULATED) {
            sensor->param = 0;
            sensor->autoOffset = 0;
            sensor->filter = 0;
          }
        }
        break;

      case SENSOR_FIELD_ID:
        if (sensor->type == TELEM_TYPE_CUSTOM) {
          lcdDrawTextAlignedLeft(y, STR_ID);
          lcdDrawHexNumber(SENSOR_2ND_COLUMN, y, sensor->id, LEFT|(menuHorizontalPosition==0 ? attr : 0));
          lcdDrawNumber(SENSOR_3RD_COLUMN, y, sensor->instance, LEFT|(menuHorizontalPosition==1 ? attr : 0));
          if (attr) {
            switch (menuHorizontalPosition) {
              case 0:
                CHECK_INCDEC_MODELVAR_ZERO(event, sensor->id, 0xffff);
                break;

              case 1:
                CHECK_INCDEC_MODELVAR_ZERO(event, sensor->instance, 0xff);
                break;
            }
          }
        }
        else {
          sensor->formula = editChoice(SENSOR_2ND_COLUMN, y, STR_FORMULA, STR_VFORMULAS, sensor->formula, 0, TELEM_FORMULA_LAST, attr, event);
          if (attr && checkIncDec_Ret) {
            sensor->param = 0;
            if (sensor->formula == TELEM_FORMULA_CELL) {
              sensor->unit = UNIT_VOLTS;
              sensor->prec = 2;
            }
            else if (sensor->formula == TELEM_FORMULA_DIST) {
              sensor->unit = UNIT_DIST;
              sensor->prec = 0;
            }
            else if (sensor->formula == TELEM_FORMULA_CONSUMPTION) {
              sensor->unit = UNIT_MAH;
              sensor->prec = 0;
            }
          }
        }
        break;

      case SENSOR_FIELD_UNIT:
        lcdDrawTextAlignedLeft(y, "Unit");
        // TODO flash saving with editChoice where I copied those 2 lines?
        lcdDrawTextAtIndex(SENSOR_2ND_COLUMN, y, STR_VTELEMUNIT, sensor->unit, attr);
        if (attr) {
          CHECK_INCDEC_MODELVAR_ZERO(event, sensor->unit, UNIT_MAX);
          if (checkIncDec_Ret) {
            telemetryItems[s_currIdx].clear();
          }
        }
        break;

      case SENSOR_FIELD_PRECISION:
        sensor->prec = editChoice(SENSOR_2ND_COLUMN, y, STR_PRECISION, STR_VPREC, sensor->prec, 0, 2, attr, event);
        if (attr && checkIncDec_Ret) {
          telemetryItems[s_currIdx].clear();
        }
        break;

      case SENSOR_FIELD_PARAM1:
        if (sensor->type == TELEM_TYPE_CALCULATED) {
          if (sensor->formula == TELEM_FORMULA_CELL) {
            lcdDrawTextAlignedLeft(y, STR_CELLSENSOR);
            drawSource(SENSOR_2ND_COLUMN, y, sensor->cell.source ? MIXSRC_FIRST_TELEM+3*(sensor->cell.source-1) : 0, attr);
            if (attr) {
              sensor->cell.source = checkIncDec(event, sensor->cell.source, 0, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isCellsSensor);
            }
            break;
          }
          else if (sensor->formula == TELEM_FORMULA_DIST) {
            lcdDrawTextAlignedLeft(y, STR_GPSSENSOR);
            drawSource(SENSOR_2ND_COLUMN, y, sensor->dist.gps ? MIXSRC_FIRST_TELEM+3*(sensor->dist.gps-1) : 0, attr);
            if (attr) {
              sensor->dist.gps = checkIncDec(event, sensor->dist.gps, 0, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isGPSSensor);
            }
            break;
          }
          else if (sensor->formula == TELEM_FORMULA_CONSUMPTION) {
            lcdDrawTextAlignedLeft(y, STR_CURRENTSENSOR);
            drawSource(SENSOR_2ND_COLUMN, y, sensor->consumption.source ? MIXSRC_FIRST_TELEM+3*(sensor->consumption.source-1) : 0, attr);
            if (attr) {
              sensor->consumption.source = checkIncDec(event, sensor->consumption.source, 0, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isCurrentSensor);
            }
            break;
          }
        }
        else {
          if (sensor->unit == UNIT_RPMS) {
            lcdDrawTextAlignedLeft(y, NO_INDENT(STR_BLADES));
            if (attr) CHECK_INCDEC_MODELVAR(event, sensor->custom.ratio, 1, 30000);
            lcdDrawNumber(SENSOR_2ND_COLUMN, y, sensor->custom.ratio, LEFT|attr);
            break;
          }
          else {
            lcdDrawTextAlignedLeft(y, STR_RATIO);
            if (attr) CHECK_INCDEC_MODELVAR(event, sensor->custom.ratio, 0, 30000);
            if (sensor->custom.ratio == 0)
              lcdDrawChar(SENSOR_2ND_COLUMN, y, '-', attr);
            else
              lcdDrawNumber(SENSOR_2ND_COLUMN, y, sensor->custom.ratio, LEFT|attr|PREC1);
            break;
          }
        }
        // no break

      case SENSOR_FIELD_PARAM2:
        if (sensor->type == TELEM_TYPE_CALCULATED) {
          if (sensor->formula == TELEM_FORMULA_CELL) {
            sensor->cell.index = editChoice(SENSOR_2ND_COLUMN, y, STR_CELLINDEX, STR_VCELLINDEX, sensor->cell.index, 0, 8, attr, event);
            break;
          }
          else if (sensor->formula == TELEM_FORMULA_DIST) {
            lcdDrawTextAlignedLeft(y, STR_ALTSENSOR);
            drawSource(SENSOR_2ND_COLUMN, y, sensor->dist.alt ? MIXSRC_FIRST_TELEM+3*(sensor->dist.alt-1) : 0, attr);
            if (attr) {
              sensor->dist.alt = checkIncDec(event, sensor->dist.alt, 0, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isAltSensor);
            }
            break;
          }
        }
        else if (sensor->unit == UNIT_RPMS) {
          lcdDrawTextAlignedLeft(y, STR_MULTIPLIER);
          if (attr) sensor->custom.offset = checkIncDec(event, sensor->custom.offset, 1, 30000, EE_MODEL|NO_INCDEC_MARKS|INCDEC_REP10);
          lcdDrawNumber(SENSOR_2ND_COLUMN, y, sensor->custom.offset, LEFT|attr);
          break;
        }
        else {
          lcdDrawTextAlignedLeft(y, NO_INDENT(STR_OFFSET));
          if (attr) CHECK_INCDEC_MODELVAR(event, sensor->custom.offset, -30000, +30000);
          if (sensor->prec > 0) attr |= (sensor->prec == 2 ? PREC2 : PREC1);
          lcdDrawNumber(SENSOR_2ND_COLUMN, y, sensor->custom.offset, LEFT|attr);
          break;
        }
        // no break

      case SENSOR_FIELD_PARAM3:
        // no break

      case SENSOR_FIELD_PARAM4:
      {
        drawStringWithIndex(0, y, NO_INDENT(STR_SOURCE), k-SENSOR_FIELD_PARAM1+1);
        int8_t & source = sensor->calc.sources[k-SENSOR_FIELD_PARAM1];
        if (attr) {
          source = checkIncDec(event, source, -MAX_TELEMETRY_SENSORS, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isSensorAvailable);
        }
        if (source < 0) {
          lcdDrawChar(SENSOR_2ND_COLUMN, y, '-', attr);
          drawSource(lcdNextPos, y, MIXSRC_FIRST_TELEM+3*(-1-source), attr);
        }
        else {
          drawSource(SENSOR_2ND_COLUMN, y, source ? MIXSRC_FIRST_TELEM+3*(source-1) : 0, attr);
        }
        break;
      }

      case SENSOR_FIELD_AUTOOFFSET:
        ON_OFF_MENU_ITEM(sensor->autoOffset, SENSOR_2ND_COLUMN, y, STR_AUTOOFFSET, attr, event);
        break;

      case SENSOR_FIELD_ONLYPOSITIVE:
        ON_OFF_MENU_ITEM(sensor->onlyPositive, SENSOR_2ND_COLUMN, y, STR_ONLYPOSITIVE, attr, event);
        break;

      case SENSOR_FIELD_FILTER:
        ON_OFF_MENU_ITEM(sensor->filter, SENSOR_2ND_COLUMN, y, STR_FILTER, attr, event);
        break;

      case SENSOR_FIELD_PERSISTENT:
        ON_OFF_MENU_ITEM(sensor->persistent, SENSOR_2ND_COLUMN, y, NO_INDENT(STR_PERSISTENT), attr, event);
        if (checkIncDec_Ret && !sensor->persistent) {
          sensor->persistentValue = 0;
        }
        break;

      case SENSOR_FIELD_LOGS:
        ON_OFF_MENU_ITEM(sensor->logs, SENSOR_2ND_COLUMN, y, STR_LOGS, attr, event);
        if (attr && checkIncDec_Ret) {
          logsClose();
        }
        break;

    }
  }
}

void onSensorMenu(const char * result)
{
  uint8_t index = menuVerticalPosition - HEADER_LINE - ITEM_TELEMETRY_SENSOR1;

  if (index < MAX_TELEMETRY_SENSORS) {
    if (result == STR_EDIT) {
      pushMenu(menuModelSensor);
    }
    else if (result == STR_DELETE) {
      delTelemetryIndex(index);
      index += 1;
      if (index<MAX_TELEMETRY_SENSORS && isTelemetryFieldAvailable(index))
        menuVerticalPosition += 1;
      else
        menuVerticalPosition = HEADER_LINE + ITEM_TELEMETRY_NEW_SENSOR;
    }
    else if (result == STR_COPY) {
      int newIndex = availableTelemetryIndex();
      if (newIndex >= 0) {
        TelemetrySensor & sourceSensor = g_model.telemetrySensors[index];
        TelemetrySensor & newSensor = g_model.telemetrySensors[newIndex];
        newSensor = sourceSensor;
        TelemetryItem & sourceItem = telemetryItems[index];
        TelemetryItem & newItem = telemetryItems[newIndex];
        newItem = sourceItem;
        storageDirty(EE_MODEL);
      }
      else {
        POPUP_WARNING(STR_TELEMETRYFULL);
      }
    }
  }
}
#endif

void menuModelTelemetryFrsky(event_t event)
{
#if defined(CPUARM)
  if (warningResult) {
    warningResult = 0;
    for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
      delTelemetryIndex(i);
    }
  }
#endif

  MENU(STR_MENUTELEMETRY, menuTabModel, MENU_MODEL_TELEMETRY_FRSKY, HEADER_LINE+ITEM_TELEMETRY_MAX, { HEADER_LINE_COLUMNS TELEMETRY_TYPE_ROWS CHANNELS_ROWS RSSI_ROWS SENSORS_ROWS USRDATA_ROWS VARIO_ROWS TELEMETRY_SCREENS_ROWS });

  uint8_t sub = menuVerticalPosition - HEADER_LINE;

  switch (event) {
    case EVT_KEY_BREAK(KEY_DOWN):
    case EVT_KEY_BREAK(KEY_UP):
#if !defined(PCBX7)
    case EVT_KEY_BREAK(KEY_LEFT):
    case EVT_KEY_BREAK(KEY_RIGHT):
#endif
      if (s_editMode>0 && sub<=ITEM_TELEMETRY_RSSI_ALARM2)
        frskySendAlarms(); // update FrSky module when edit mode exited
      break;
  }

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i + menuVerticalOffset;
#if defined(CPUARM)
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j+HEADER_LINE] == HIDDEN_ROW) {
        if (++k >= (int)DIM(mstate_tab)) {
          return;
        }
      }
    }
#endif

    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

#if !defined(CPUARM)
    uint8_t ch = TELEMETRY_CURRENT_CHANNEL(k);
    FrSkyChannelData & channel = g_model.frsky.channels[ch];
    uint8_t dest = TELEM_A1-1+ch;
#endif

#if defined(CPUARM)
    if (k>=ITEM_TELEMETRY_SENSOR1 && k<ITEM_TELEMETRY_SENSOR1+MAX_TELEMETRY_SENSORS) {
      int index = k - ITEM_TELEMETRY_SENSOR1;
      lcdDrawNumber(INDENT_WIDTH, y, index+1, LEFT|attr);
      lcdDrawChar(lcdLastRightPos, y, ':', attr);
      lcdDrawSizedText(3*FW, y, g_model.telemetrySensors[index].label, TELEM_LABEL_LEN, ZCHAR);
      if (telemetryItems[index].isFresh()) {
        lcdDrawChar(17*FW, y, '*');
      }
      TelemetryItem & telemetryItem = telemetryItems[index];
      if (telemetryItem.isAvailable()) {
        bool isOld = telemetryItem.isOld();
        lcdNextPos = TELEM_COL2;
        if (isOld) lcdDrawChar(lcdNextPos, y, '[');
        drawSensorCustomValue(lcdNextPos, y, index, getValue(MIXSRC_FIRST_TELEM+3*index), LEFT);
        if (isOld) lcdDrawChar(lcdLastRightPos, y, ']');
      }
      else {
        lcdDrawText(TELEM_COL2, y, "---", 0); // TODO shortcut
      }
      if (attr) {
        s_editMode = 0;
        s_currIdx = index;
        if (event == EVT_KEY_LONG(KEY_ENTER)) {
          killEvents(event);
          POPUP_MENU_ADD_ITEM(STR_EDIT);
          POPUP_MENU_ADD_ITEM(STR_COPY);
          POPUP_MENU_ADD_ITEM(STR_DELETE);
          POPUP_MENU_START(onSensorMenu);
        }
        else if (event == EVT_KEY_BREAK(KEY_ENTER)) {
          pushMenu(menuModelSensor);
        }
      }
    }
    else
#endif

    switch (k) {
#if defined(CPUARM)
      case ITEM_TELEMETRY_PROTOCOL_TYPE:
        lcdDrawTextAlignedLeft(y, STR_TELEMETRY_TYPE);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_TELEMETRY_PROTOCOLS, g_model.telemetryProtocol, attr);
        if (attr) {
          g_model.telemetryProtocol = checkIncDec(event, g_model.telemetryProtocol, PROTOCOL_TELEMETRY_FIRST, PROTOCOL_TELEMETRY_LAST, EE_MODEL, isTelemetryProtocolAvailable);
        }
        break;

#if defined(REVX)
      case ITEM_TELEMETRY_INVERTED_SERIAL:
        ON_OFF_MENU_ITEM(g_model.moduleData[EXTERNAL_MODULE].invertedSerial, TELEM_COL2, y, STR_INVERTED_SERIAL, attr, event);
        break;
#endif
#endif

#if defined(CPUARM)
      case ITEM_TELEMETRY_SENSORS_LABEL:
        lcdDrawTextAlignedLeft(y, STR_TELEMETRY_SENSORS);
        break;

      case ITEM_TELEMETRY_DISCOVER_SENSORS:
        lcdDrawText(0, y, allowNewSensors ? STR_STOP_DISCOVER_SENSORS : STR_DISCOVER_SENSORS, attr);
        if (attr && event==EVT_KEY_BREAK(KEY_ENTER)) {
          s_editMode = 0;
          allowNewSensors = !allowNewSensors;
        }
        break;

      case ITEM_TELEMETRY_NEW_SENSOR:
        lcdDrawText(0, y, STR_TELEMETRY_NEWSENSOR, attr);
        if (attr && event==EVT_KEY_BREAK(KEY_ENTER)) {
          s_editMode = 0;
          int res = availableTelemetryIndex();
          if (res >= 0) {
            s_currIdx = res;
            pushMenu(menuModelSensor);
          }
          else {
            POPUP_WARNING(STR_TELEMETRYFULL);
          }
        }
        break;

      case ITEM_TELEMETRY_DELETE_ALL_SENSORS:
        lcdDrawText(0, y, STR_DELETE_ALL_SENSORS, attr);
        if (attr) {
          s_editMode = 0;
          if (event==EVT_KEY_LONG(KEY_ENTER)) {
            killEvents(KEY_ENTER);
            POPUP_CONFIRMATION(STR_CONFIRMDELETE);
          }
        }
        break;

      case ITEM_TELEMETRY_IGNORE_SENSOR_INSTANCE:
        ON_OFF_MENU_ITEM(g_model.ignoreSensorIds, TELEM_COL2, y, STR_IGNORE_INSTANCE, attr, event);
        break;
#endif

#if !defined(CPUARM)
      case ITEM_TELEMETRY_A1_LABEL:
      case ITEM_TELEMETRY_A2_LABEL:
        lcdDrawTextAlignedLeft(y, STR_ACHANNEL);
        lcdDrawNumber(2*FW, y, ch+1, 0);
        drawTelemetryValue(TELEM_COL2+6*FW, y, dest, telemetryData.analog[ch].value, LEFT);
        break;

      case ITEM_TELEMETRY_A1_RANGE:
      case ITEM_TELEMETRY_A2_RANGE:
        lcdDrawTextAlignedLeft(y, STR_RANGE);
        drawTelemetryValue(TELEM_COL2, y, dest, 255-channel.offset, (menuHorizontalPosition<=0 ? attr : 0)|NO_UNIT|LEFT);
        lcdDrawTextAtIndex(lcdLastRightPos, y, STR_VTELEMUNIT, channel.type, menuHorizontalPosition!=0 ? attr : 0);
        if (attr && (s_editMode>0 || p1valdiff)) {
          if (menuHorizontalPosition == 0) {
            uint16_t ratio = checkIncDec(event, channel.ratio, 0, 256, EE_MODEL);
            if (checkIncDec_Ret) {
              if (ratio == 127 && channel.multiplier > 0) {
                channel.multiplier--; channel.ratio = 255;
              }
              else if (ratio == 256) {
                if (channel.multiplier < FRSKY_MULTIPLIER_MAX) { channel.multiplier++; channel.ratio = 128; }
              }
              else {
                channel.ratio = ratio;
              }
            }
          }
          else {
            CHECK_INCDEC_MODELVAR_ZERO(event, channel.type, UNIT_A1A2_MAX);
          }
        }
        break;

      case ITEM_TELEMETRY_A1_OFFSET:
      case ITEM_TELEMETRY_A2_OFFSET:
        lcdDrawTextAlignedLeft(y, STR_OFFSET);
        drawTelemetryValue(TELEM_COL2, y, dest, 0, LEFT|attr);
        if (attr) channel.offset = checkIncDec(event, channel.offset, -256, 256, EE_MODEL);
        break;

      case ITEM_TELEMETRY_A1_ALARM1:
      case ITEM_TELEMETRY_A1_ALARM2:
      case ITEM_TELEMETRY_A2_ALARM1:
      case ITEM_TELEMETRY_A2_ALARM2:
      {
        uint8_t alarm = ((k==ITEM_TELEMETRY_A1_ALARM1 || k==ITEM_TELEMETRY_A2_ALARM1) ? 0 : 1);
        lcdDrawTextAlignedLeft(y, STR_ALARM);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_VALARM, ALARM_LEVEL(ch, alarm), menuHorizontalPosition<=0 ? attr : 0);
        lcdDrawTextAtIndex(TELEM_COL2+4*FW, y, STR_VALARMFN, ALARM_GREATER(ch, alarm), (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);
        drawTelemetryValue(TELEM_COL2+6*FW, y, dest, channel.alarms_value[alarm], ((CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0) | LEFT);

        if (attr && (s_editMode>0 || p1valdiff)) {
          uint8_t t;
          switch (menuHorizontalPosition) {
           case 0:
             t = ALARM_LEVEL(ch, alarm);
             channel.alarms_level = (channel.alarms_level & ~(3<<(2*alarm))) + (checkIncDecModel(event, t, 0, 3) << (2*alarm));
             break;
           case 1:
             t = ALARM_GREATER(ch, alarm);
             if (t != checkIncDecModel(event, t, 0, 1)) {
               channel.alarms_greater ^= (1 << alarm);
               frskySendAlarms();
             }
             break;
           case 2:
             channel.alarms_value[alarm] = checkIncDec(event, channel.alarms_value[alarm], 0, 255, EE_MODEL);
             break;
          }
        }
        break;
      }
#endif

      case ITEM_TELEMETRY_RSSI_LABEL:
#if defined(MULTIMODULE)
        if (telemetryProtocol == PROTOCOL_MULTIMODULE &&
          g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false) == MM_RF_PROTO_FS_AFHDS2A)
          lcdDrawTextAlignedLeft(y, PSTR("RSNR"));
        else
#endif
        lcdDrawTextAlignedLeft(y, PSTR("RSSI"));
        break;

      case ITEM_TELEMETRY_RSSI_ALARM1:
      case ITEM_TELEMETRY_RSSI_ALARM2: {
#if defined(CPUARM)
        bool warning = (k==ITEM_TELEMETRY_RSSI_ALARM1);
        lcdDrawTextAlignedLeft(y, (warning ? STR_LOWALARM : STR_CRITICALALARM));
        lcdDrawNumber(LCD_W, y, warning? g_model.rssiAlarms.getWarningRssi() : g_model.rssiAlarms.getCriticalRssi(), RIGHT | attr, 3);
        if (attr && s_editMode>0) {
          if (warning)
            CHECK_INCDEC_MODELVAR(event, g_model.rssiAlarms.warning, -30, 30);
          else
            CHECK_INCDEC_MODELVAR(event, g_model.rssiAlarms.critical, -30, 30);
     }
#else // CPUARM
        uint8_t alarm = (k-ITEM_TELEMETRY_RSSI_ALARM1);
        lcdDrawTextAlignedLeft(y, STR_ALARM);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_VALARM, ((2+alarm+g_model.frsky.rssiAlarms[alarm].level)%4), menuHorizontalPosition<=0 ? attr : 0);
        lcdDrawChar(TELEM_COL2+4*FW, y, '<');
        lcdDrawNumber(TELEM_COL2+6*FW, y, getRssiAlarmValue(alarm), LEFT|(menuHorizontalPosition!=0 ? attr : 0), 3);
        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.rssiAlarms[alarm].level, -3, 2); // circular (saves flash)
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.rssiAlarms[alarm].value, -30, 30);
              break;
          }
       }
#endif // CPUARM
        break;
      }
#if defined(CPUARM)
      case ITEM_TELEMETRY_DISABLE_ALARMS:
        g_model.rssiAlarms.disabled = editCheckBox(g_model.rssiAlarms.disabled, LCD_W - 10, y, STR_DISABLE_ALARM, attr, event);
        break;
#endif
#if !defined(CPUARM)
#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
      case ITEM_TELEMETRY_USR_LABEL:
        lcdDrawTextAlignedLeft(y, STR_USRDATA);
        break;

      case ITEM_TELEMETRY_USR_PROTO:
        lcdDrawTextAlignedLeft(y, STR_PROTO);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_VTELPROTO, g_model.frsky.usrProto, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.frsky.usrProto, USR_PROTO_LAST);
        break;

      case ITEM_TELEMETRY_USR_BLADES:
        lcdDrawTextAlignedLeft(y, STR_BLADES);
        lcdDrawNumber(TELEM_COL2+FWNUM, y, 2+g_model.frsky.blades, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.frsky.blades, MAX_BLADES);
        break;
#endif

      case ITEM_TELEMETRY_USR_VOLTAGE_SOURCE:
        lcdDrawTextAlignedLeft(y, STR_VOLTAGE);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_AMPSRC, g_model.frsky.voltsSource+1, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.frsky.voltsSource, FRSKY_VOLTS_SOURCE_LAST);
        break;

      case ITEM_TELEMETRY_USR_CURRENT_SOURCE:
        lcdDrawTextAlignedLeft(y, STR_CURRENT);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_AMPSRC, g_model.frsky.currentSource, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.frsky.currentSource, FRSKY_CURRENT_SOURCE_LAST);
        break;

#if defined(FAS_OFFSET) || !defined(CPUM64)
      case ITEM_TELEMETRY_FAS_OFFSET:
        lcdDrawTextAlignedLeft(y, STR_FAS_OFFSET);
        lcdDrawNumber(TELEM_COL2, y, g_model.frsky.fasOffset, attr|LEFT|PREC1);
        lcdDrawNumber(TELEM_COL2+6*FW, y, telemetryData.hub.current, LEFT|PREC1);
        lcdDrawChar(TELEM_COL2+8*FW, y, 'A');
        if (attr) g_model.frsky.fasOffset = checkIncDec(event, g_model.frsky.fasOffset, -120, 120, EE_MODEL);
        break;
#endif
#endif

#if defined(VARIO)
      case ITEM_TELEMETRY_VARIO_LABEL:
        lcdDrawTextAlignedLeft(y, STR_VARIO);
        break;

      case ITEM_TELEMETRY_VARIO_SOURCE:
        lcdDrawTextAlignedLeft(y, STR_SOURCE);
#if defined(CPUARM)
        drawSource(TELEM_COL2, y, g_model.frsky.varioSource ? MIXSRC_FIRST_TELEM+3*(g_model.frsky.varioSource-1) : 0, attr);
        if (attr) {
          g_model.frsky.varioSource = checkIncDec(event, g_model.frsky.varioSource, 0, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isSensorAvailable);
        }
#else
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_VARIOSRC, g_model.frsky.varioSource, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioSource, 0, VARIO_SOURCE_LAST);
#endif
        break;

#if defined(PCBSTD)
      case ITEM_TELEMETRY_VARIO_RANGE:
        lcdDrawTextAlignedLeft(y, STR_RANGE);

        lcdDrawNumber(TELEM_COL2, y, 5+g_model.frsky.varioCenterMax, (menuHorizontalPosition==0 ? attr : 0)|PREC1|LEFT);
        lcdDrawNumber(TELEM_COL2+8*FW, y, 10+g_model.frsky.varioMax, (menuHorizontalPosition==1 ? attr : 0));
        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioCenterMax, -15, +15);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioMax, -7, 7);
              break;
          }
        }
        break;
#else
      case ITEM_TELEMETRY_VARIO_RANGE:
        lcdDrawTextAlignedLeft(y, STR_RANGE);
        if (attr && CURSOR_ON_LINE()) {
          lcdDrawSolidFilledRect(TELEM_COL2-1, y-1, LCD_W-TELEM_COL2+1, FH+1);
        }
        lcdDrawNumber(TELEM_COL2, y, -10+g_model.frsky.varioMin, ((CURSOR_ON_LINE() || menuHorizontalPosition==0) ? attr : 0)|LEFT);
        lcdDrawNumber(TELEM_COL2+4*FW, y, 10+g_model.frsky.varioMax, ((CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0)|LEFT);
        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioMin, -7, 7);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioMax, -7, 7);
              break;
          }
        }
        break;

      case ITEM_TELEMETRY_VARIO_CENTER:
        lcdDrawTextAlignedLeft(y, STR_CENTER);
        lcdDrawNumber(TELEM_COL2, y, -5+g_model.frsky.varioCenterMin, ((CURSOR_ON_LINE() || menuHorizontalPosition==0) ? attr : 0)|PREC1|LEFT);
        lcdDrawNumber(TELEM_COL2+4*FW, y, 5+g_model.frsky.varioCenterMax, ((CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0)|PREC1|LEFT);
#if defined(CPUARM)
        lcdDrawTextAtIndex(TELEM_COL2+8*FW, y, STR_VVARIOCENTER, g_model.frsky.varioCenterSilent, (menuHorizontalPosition==2 ? attr : 0));
#endif
        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioCenterMin, -16, 5+min<int8_t>(10, g_model.frsky.varioCenterMax+5));
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioCenterMax, -5+max<int8_t>(-10, g_model.frsky.varioCenterMin-5), +15);
              break;
#if defined(CPUARM)
            case 2:
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.frsky.varioCenterSilent, 1);
              break;
#endif
          }
        }
        break;
#endif
#endif

#if !defined(CPUARM)
      case ITEM_TELEMETRY_SCREEN_LABEL1:
      case ITEM_TELEMETRY_SCREEN_LABEL2:
      {
        uint8_t screenIndex = (k < ITEM_TELEMETRY_SCREEN_LABEL2 ? 1 : 2);
        drawStringWithIndex(0*FW, y, STR_SCREEN, screenIndex);
#if defined(GAUGES)
        bool screenType = g_model.frsky.screensType & screenIndex;
        if (screenType != (bool)editChoice(TELEM_SCRTYPE_COL, y, PSTR(""), STR_VTELEMSCREENTYPE, screenType, 0, 1, attr, event))
          g_model.frsky.screensType ^= screenIndex;
#endif
        break;
      }

      case ITEM_TELEMETRY_SCREEN_LINE1:
      case ITEM_TELEMETRY_SCREEN_LINE2:
      case ITEM_TELEMETRY_SCREEN_LINE3:
      case ITEM_TELEMETRY_SCREEN_LINE4:
      case ITEM_TELEMETRY_SCREEN_LINE5:
      case ITEM_TELEMETRY_SCREEN_LINE6:
      case ITEM_TELEMETRY_SCREEN_LINE7:
      case ITEM_TELEMETRY_SCREEN_LINE8:
      {
        uint8_t screenIndex, lineIndex;
        if (k < ITEM_TELEMETRY_SCREEN_LABEL2) {
          screenIndex = 0;
          lineIndex = k-ITEM_TELEMETRY_SCREEN_LINE1;
        }
        else {
          screenIndex = 1;
          lineIndex = k-ITEM_TELEMETRY_SCREEN_LINE5;
        }

#if defined(GAUGES)
        if (IS_BARS_SCREEN(screenIndex)) {
          FrSkyBarData & bar = g_model.frsky.screens[screenIndex].bars[lineIndex];
          source_t barSource = bar.source;
          lcdDrawTextAtIndex(TELEM_COL1, y, STR_VTELEMCHNS, barSource, menuHorizontalPosition==0 ? attr : 0);
          if (barSource) {
            drawTelemetryValue(TELEM_BARS_COLMIN, y, barSource-1, convertBarTelemValue(barSource, bar.barMin), (menuHorizontalPosition==1 ? attr : 0) | LEFT);
            drawTelemetryValue(TELEM_BARS_COLMAX, y, barSource-1, convertBarTelemValue(barSource, 255-bar.barMax), (menuHorizontalPosition==2 ? attr : 0) | LEFT);
          }
          else if (attr && menuHorizontalPosition>0) {
            menuHorizontalPosition = 0;
          }
          if (attr && (s_editMode>0 || p1valdiff)) {
            switch (menuHorizontalPosition) {
              case 0:
                bar.source = CHECK_INCDEC_MODELVAR_ZERO(event, barSource, TELEM_DISPLAY_MAX);
                if (checkIncDec_Ret) {
                  bar.barMin = 0;
                  bar.barMax = 255 - maxBarTelemValue(bar.source);
                }
                break;
              case 1:
                bar.barMin = checkIncDec(event, bar.barMin, 0, 254-bar.barMax, EE_MODEL|NO_INCDEC_MARKS);
                break;
              case 2:
                bar.barMax = 255 - checkIncDec(event, 255-bar.barMax, bar.barMin+1, maxBarTelemValue(barSource), EE_MODEL|NO_INCDEC_MARKS);
                break;
            }
          }
        }
        else
#endif
        {
          for (uint8_t c=0; c<NUM_LINE_ITEMS; c++) {
            uint8_t cellAttr = (menuHorizontalPosition==c ? attr : 0);
            source_t & value = g_model.frsky.screens[screenIndex].lines[lineIndex].sources[c];
            uint8_t pos[] = {INDENT_WIDTH, TELEM_COL2};
            lcdDrawTextAtIndex(pos[c], y, STR_VTELEMCHNS, value, cellAttr);
            if (cellAttr && (s_editMode>0 || p1valdiff)) {
              CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, value, (lineIndex==3 && c==0) ? TELEM_STATUS_MAX : TELEM_DISPLAY_MAX, isTelemetrySourceAvailable);
            }
          }
          if (attr && menuHorizontalPosition == NUM_LINE_ITEMS) {
            REPEAT_LAST_CURSOR_MOVE();
          }
        }
        break;
      }
#endif // !defined(CPUARM)
    }
  }
}
#endif
