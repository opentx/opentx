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
  ITEM_TELEMETRY_PROTOCOL_TYPE,
  ITEM_TELEMETRY_RSSI_LABEL,
  ITEM_TELEMETRY_RSSI_ALARM1,
  ITEM_TELEMETRY_RSSI_ALARM2,
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
#if defined(VARIO)
  ITEM_TELEMETRY_VARIO_LABEL,
  ITEM_TELEMETRY_VARIO_SOURCE,
  ITEM_TELEMETRY_VARIO_RANGE,
  ITEM_TELEMETRY_VARIO_CENTER,
#endif
  ITEM_TELEMETRY_MAX
};

#define TELEM_COL1                    10
#define TELEM_COL2                    200
#define TELEM_COL3                    275
#define TELEM_COL4                    350
#define TELEM_COL5                    425

#define IF_FAS_OFFSET(x)              x,
#define IS_RANGE_DEFINED(k)           (g_model.frsky.channels[k].ratio > 0)

#define SENSOR_ROWS(x)                (isTelemetryFieldAvailable(x) ? (uint8_t)0 : HIDDEN_ROW)
#define SENSORS_ROWS                  LABEL(Sensors), SENSOR_ROWS(0), SENSOR_ROWS(1), SENSOR_ROWS(2), SENSOR_ROWS(3), SENSOR_ROWS(4), SENSOR_ROWS(5), SENSOR_ROWS(6), SENSOR_ROWS(7), SENSOR_ROWS(8), SENSOR_ROWS(9), SENSOR_ROWS(10), SENSOR_ROWS(11), SENSOR_ROWS(12), SENSOR_ROWS(13), SENSOR_ROWS(14), SENSOR_ROWS(15), SENSOR_ROWS(16), SENSOR_ROWS(17), SENSOR_ROWS(18), SENSOR_ROWS(19), SENSOR_ROWS(20), SENSOR_ROWS(21), SENSOR_ROWS(22), SENSOR_ROWS(23), SENSOR_ROWS(24), SENSOR_ROWS(25), SENSOR_ROWS(26), SENSOR_ROWS(27), SENSOR_ROWS(28), SENSOR_ROWS(29), SENSOR_ROWS(30), SENSOR_ROWS(31), 0, 0, 0, 0,
#if defined(VARIO)
  #define VARIO_ROWS                  LABEL(Vario), 0, 1, 2,
#else
  #define VARIO_ROWS
#endif
#define RSSI_ROWS                     LABEL(RSSI), 0, 0, 0,
#define VARIO_RANGE_ROWS              3
#define TELEMETRY_TYPE_ROWS           (g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_PPM) ? (uint8_t)0 : HIDDEN_ROW,

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

#define SENSOR_2ND_COLUMN      140
#define SENSOR_3RD_COLUMN      280

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

bool menuModelSensor(event_t event)
{
  TelemetrySensor * sensor = &g_model.telemetrySensors[s_currIdx];

  SUBMENU("SENSOR", ICON_MODEL_TELEMETRY, SENSOR_FIELD_MAX, { 0, 0, sensor->type == TELEM_TYPE_CALCULATED ? (uint8_t)0 : (uint8_t)1, SENSOR_UNIT_ROWS, SENSOR_PREC_ROWS, SENSOR_PARAM1_ROWS, SENSOR_PARAM2_ROWS, SENSOR_PARAM3_ROWS, SENSOR_PARAM4_ROWS, SENSOR_AUTOOFFSET_ROWS, SENSOR_ONLYPOS_ROWS, SENSOR_FILTER_ROWS, SENSOR_PERSISTENT_ROWS, 0 });
  lcdDrawNumber(lcdNextPos, 3, s_currIdx+1, MENU_TITLE_COLOR|LEFT);
  drawSensorCustomValue(50, 3+FH, s_currIdx, getValue(MIXSRC_FIRST_TELEM+3*s_currIdx), MENU_TITLE_COLOR|LEFT);

  for (uint8_t i=0; i<NUM_BODY_LINES+1; i++) {
    coord_t y = MENU_CONTENT_TOP - FH - 2 + i*FH;
    int k = i + menuVerticalOffset;

    for (int j=0; j<k; j++) {
      if (mstate_tab[j+1] == HIDDEN_ROW) {
        if (++k >= (int)DIM(mstate_tab)) {
          return true;
        }
      }
    }

    LcdFlags attr = (menuVerticalPosition==k ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);

    switch (k) {
      case SENSOR_FIELD_NAME:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_NAME);
        editName(SENSOR_2ND_COLUMN, y, sensor->label, TELEM_LABEL_LEN, event, attr);
        break;

      case SENSOR_FIELD_TYPE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, NO_INDENT(STR_TYPE));
        sensor->type = editChoice(SENSOR_2ND_COLUMN, y, STR_VSENSORTYPES, sensor->type, 0, 1, attr, event);
        if (attr && checkIncDec_Ret) {
          sensor->instance = 0;
          if (sensor->type == TELEM_TYPE_CALCULATED) {
            sensor->param = 0;
            sensor->filter = 0;
            sensor->autoOffset = 0;
          }
        }
        break;

      case SENSOR_FIELD_ID:
        if (sensor->type == TELEM_TYPE_CUSTOM) {
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ID);
          lcdDrawHexNumber(SENSOR_2ND_COLUMN, y, sensor->id, LEFT|(menuHorizontalPosition==0 ? attr : 0));
          lcdDrawNumber(SENSOR_3RD_COLUMN, y, sensor->instance, LEFT|(menuHorizontalPosition==1 ? attr : 0));
          if (attr) {
            switch (menuHorizontalPosition) {
              case 0:
                sensor->id = checkIncDec(event, sensor->id, 0x0000, 0xffff, INCDEC_REP10|NO_INCDEC_MARKS);
                break;

              case 1:
                CHECK_INCDEC_MODELVAR_ZERO(event, sensor->instance, 0xff);
                break;
            }
          }
        }
        else {
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_FORMULA);
          sensor->formula = editChoice(SENSOR_2ND_COLUMN, y, STR_VFORMULAS, sensor->formula, 0, TELEM_FORMULA_LAST, attr, event);
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
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_UNIT);
        // TODO flash saving with editChoice where I copied those 2 lines?
        lcdDrawTextAtIndex(SENSOR_2ND_COLUMN, y, STR_VTELEMUNIT, sensor->unit, attr);
        if (attr) {
          CHECK_INCDEC_MODELVAR_ZERO(event, sensor->unit, UNIT_MAX);
          if (checkIncDec_Ret) {
            if (sensor->unit == UNIT_FAHRENHEIT) {
              sensor->prec = 0;
            }
            telemetryItems[s_currIdx].clear();
          }
        }
        break;

      case SENSOR_FIELD_PRECISION:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PRECISION);
        sensor->prec = editChoice(SENSOR_2ND_COLUMN, y, STR_VPREC, sensor->prec, 0, 2, attr, event);
        if (attr && checkIncDec_Ret) {
          telemetryItems[s_currIdx].clear();
        }
        break;

      case SENSOR_FIELD_PARAM1:
        if (sensor->type == TELEM_TYPE_CALCULATED) {
          if (sensor->formula == TELEM_FORMULA_CELL) {
            lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CELLSENSOR);
            drawSource(SENSOR_2ND_COLUMN, y, sensor->cell.source ? MIXSRC_FIRST_TELEM+3*(sensor->cell.source-1) : 0, attr);
            if (attr) {
              sensor->cell.source = checkIncDec(event, sensor->cell.source, 0, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isCellsSensor);
            }
            break;
          }
          else if (sensor->formula == TELEM_FORMULA_DIST) {
            lcdDrawText(MENUS_MARGIN_LEFT, y, STR_GPSSENSOR);
            drawSource(SENSOR_2ND_COLUMN, y, sensor->dist.gps ? MIXSRC_FIRST_TELEM+3*(sensor->dist.gps-1) : 0, attr);
            if (attr) {
              sensor->dist.gps = checkIncDec(event, sensor->dist.gps, 0, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isGPSSensor);
            }
            break;
          }
          else if (sensor->formula == TELEM_FORMULA_CONSUMPTION) {
            lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CURRENTSENSOR);
            drawSource(SENSOR_2ND_COLUMN, y, sensor->consumption.source ? MIXSRC_FIRST_TELEM+3*(sensor->consumption.source-1) : 0, attr);
            if (attr) {
              sensor->consumption.source = checkIncDec(event, sensor->consumption.source, 0, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isSensorAvailable);
            }
            break;
          }
          else if (sensor->formula == TELEM_FORMULA_TOTALIZE) {
            lcdDrawText(MENUS_MARGIN_LEFT, y, NO_INDENT(STR_SOURCE));
            drawSource(SENSOR_2ND_COLUMN, y, sensor->consumption.source ? MIXSRC_FIRST_TELEM+3*(sensor->consumption.source-1) : 0, attr);
            if (attr) {
              sensor->consumption.source = checkIncDec(event, sensor->consumption.source, 0, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isSensorAvailable);
            }
            break;
          }
        }
        else {
          if (sensor->unit == UNIT_RPMS) {
            lcdDrawText(MENUS_MARGIN_LEFT, y, NO_INDENT(STR_BLADES));
            if (attr) sensor->custom.ratio = checkIncDec(event, sensor->custom.ratio, 1, 30000, EE_MODEL|NO_INCDEC_MARKS|INCDEC_REP10);
            lcdDrawNumber(SENSOR_2ND_COLUMN, y, sensor->custom.ratio, LEFT|attr);
            break;
          }
          else {
            lcdDrawText(MENUS_MARGIN_LEFT, y, STR_RATIO);
            if (attr) sensor->custom.ratio = checkIncDec(event, sensor->custom.ratio, 0, 30000, EE_MODEL|NO_INCDEC_MARKS|INCDEC_REP10);
            if (sensor->custom.ratio == 0)
              lcdDrawText(SENSOR_2ND_COLUMN, y, "-", attr);
            else
              lcdDrawNumber(SENSOR_2ND_COLUMN, y, sensor->custom.ratio, LEFT|attr|PREC1);
            break;
          }
        }
        // no break

      case SENSOR_FIELD_PARAM2:
        if (sensor->type == TELEM_TYPE_CALCULATED) {
          if (sensor->formula == TELEM_FORMULA_CELL) {
            lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CELLINDEX);
            sensor->cell.index = editChoice(SENSOR_2ND_COLUMN, y, STR_VCELLINDEX, sensor->cell.index, 0, 8, attr, event);
            break;
          }
          else if (sensor->formula == TELEM_FORMULA_DIST) {
            lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ALTSENSOR);
            drawSource(SENSOR_2ND_COLUMN, y, sensor->dist.alt ? MIXSRC_FIRST_TELEM+3*(sensor->dist.alt-1) : 0, attr);
            if (attr) {
              sensor->dist.alt = checkIncDec(event, sensor->dist.alt, 0, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isAltSensor);
            }
            break;
          }
        }
        else if (sensor->unit == UNIT_RPMS) {
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MULTIPLIER);
          if (attr) sensor->custom.offset = checkIncDec(event, sensor->custom.offset, 1, 30000, EE_MODEL|NO_INCDEC_MARKS|INCDEC_REP10);
          lcdDrawNumber(SENSOR_2ND_COLUMN, y, sensor->custom.offset, LEFT|attr);
          break;
        }
        else {
          lcdDrawText(MENUS_MARGIN_LEFT, y, NO_INDENT(STR_OFFSET));
          if (attr) sensor->custom.offset = checkIncDec(event, sensor->custom.offset, -30000, +30000, EE_MODEL|NO_INCDEC_MARKS|INCDEC_REP10);
          if (sensor->prec > 0) attr |= (sensor->prec == 2 ? PREC2 : PREC1);
          lcdDrawNumber(SENSOR_2ND_COLUMN, y, sensor->custom.offset, LEFT|attr);
          break;
        }
        // no break

      case SENSOR_FIELD_PARAM3:
        // no break

      case SENSOR_FIELD_PARAM4:
      {
        drawStringWithIndex(MENUS_MARGIN_LEFT, y, NO_INDENT(STR_SOURCE), k-SENSOR_FIELD_PARAM1+1);
        int8_t & source = sensor->calc.sources[k-SENSOR_FIELD_PARAM1];
        if (attr) {
          source = checkIncDec(event, source, -MAX_TELEMETRY_SENSORS, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isSensorAvailable);
        }
        if (source < 0) {
          lcdDrawText(SENSOR_2ND_COLUMN, y, "-", attr);
          drawSource(SENSOR_2ND_COLUMN+5, y, MIXSRC_FIRST_TELEM+3*(-1-source), attr);
        }
        else {
          drawSource(SENSOR_2ND_COLUMN, y, source ? MIXSRC_FIRST_TELEM+3*(source-1) : 0, attr);
        }
        break;
      }

      case SENSOR_FIELD_AUTOOFFSET:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_AUTOOFFSET);
        sensor->autoOffset = editCheckBox(sensor->autoOffset, SENSOR_2ND_COLUMN, y, attr, event);
        break;

      case SENSOR_FIELD_ONLYPOSITIVE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ONLYPOSITIVE);
        sensor->onlyPositive = editCheckBox(sensor->onlyPositive, SENSOR_2ND_COLUMN, y, attr, event);
        break;

      case SENSOR_FIELD_FILTER:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_FILTER);
        sensor->filter = editCheckBox(sensor->filter, SENSOR_2ND_COLUMN, y, attr, event);
        break;

      case SENSOR_FIELD_PERSISTENT:
        lcdDrawText(MENUS_MARGIN_LEFT, y, NO_INDENT(STR_PERSISTENT));
        sensor->persistent = editCheckBox(sensor->persistent, SENSOR_2ND_COLUMN, y, attr, event);
        if (checkIncDec_Ret && !sensor->persistent) {
          sensor->persistentValue = 0;
        }
        break;

      case SENSOR_FIELD_LOGS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_LOGS);
        sensor->logs = editCheckBox(sensor->logs, SENSOR_2ND_COLUMN, y, attr, event);
        if (attr && checkIncDec_Ret) {
          logsClose();
        }
        break;
    }
  }
  return true;
}

void onSensorMenu(const char * result)
{
  uint8_t index = menuVerticalPosition - ITEM_TELEMETRY_SENSOR1;

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
        menuVerticalPosition = ITEM_TELEMETRY_NEW_SENSOR;
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

bool menuModelTelemetryFrsky(event_t event)
{
  if (warningResult) {
    warningResult = 0;
    for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
      delTelemetryIndex(i);
    }
  }

  MENU(STR_MENUTELEMETRY, MODEL_ICONS, menuTabModel, MENU_MODEL_TELEMETRY_FRSKY, ITEM_TELEMETRY_MAX, { TELEMETRY_TYPE_ROWS RSSI_ROWS SENSORS_ROWS VARIO_ROWS });

  for (uint8_t i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    int k = i + menuVerticalOffset;
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j] == HIDDEN_ROW)
        k++;
    }

    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (menuVerticalPosition == k ? blink : 0);

    if (k>=ITEM_TELEMETRY_SENSOR1 && k<ITEM_TELEMETRY_SENSOR1+MAX_TELEMETRY_SENSORS) {
      int index = k-ITEM_TELEMETRY_SENSOR1;
      lcdDrawNumber(MENUS_MARGIN_LEFT+INDENT_WIDTH, y, index+1, LEFT|attr, 0, NULL, ":");
      lcdDrawSizedText(60, y, g_model.telemetrySensors[index].label, TELEM_LABEL_LEN, ZCHAR);
      TelemetryItem & telemetryItem = telemetryItems[index];
      if (telemetryItem.isFresh()) {
        lcdDrawText(130, y, "*");
      }
      if (telemetryItem.isAvailable()) {
        LcdFlags color = telemetryItem.isOld() ? ALARM_COLOR : TEXT_COLOR;
        drawSensorCustomValue(TELEM_COL2, y, index, getValue(MIXSRC_FIRST_TELEM+3*index), LEFT|color);
      }
      else {
        lcdDrawText(TELEM_COL2, y, "---"); // TODO shortcut
      }
      TelemetrySensor * sensor = & g_model.telemetrySensors[index];
#if defined(MULTIMODULE)
      if (IS_SPEKTRUM_PROTOCOL()) {
          // Spektrum does not (yet?) really support multiple sensor of the same type. But a lot of
          // different sensor display the same information (e.g. voltage, capacity). Show the id
          // of the sensor in the overview to ease figuring out what sensors belong together
          lcdDrawHexNumber(TELEM_COL5, y, sensor->id, LEFT);
        } else
#endif
      if (sensor->type == TELEM_TYPE_CUSTOM && !g_model.ignoreSensorIds) {
        lcdDrawNumber(TELEM_COL5, y, sensor->instance, LEFT);
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

    switch (k) {
      case ITEM_TELEMETRY_PROTOCOL_TYPE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TELEMETRY_TYPE);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_TELEMETRY_PROTOCOLS, g_model.telemetryProtocol, attr);
        g_model.telemetryProtocol = checkIncDec(event, g_model.telemetryProtocol, PROTOCOL_TELEMETRY_FIRST, PROTOCOL_TELEMETRY_LAST, EE_MODEL, isTelemetryProtocolAvailable);
        break;

      case ITEM_TELEMETRY_SENSORS_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TELEMETRY_SENSORS);
        lcdDrawText(TELEM_COL2, y, STR_VALUE, 0);
        if (!g_model.ignoreSensorIds && !IS_SPEKTRUM_PROTOCOL()) {
          lcdDrawText(TELEM_COL5, y, STR_ID, 0);
        }
        break;

      case ITEM_TELEMETRY_DISCOVER_SENSORS:
        lcdDrawText(MENUS_MARGIN_LEFT+INDENT_WIDTH, y, allowNewSensors ? NO_INDENT(STR_STOP_DISCOVER_SENSORS) : NO_INDENT(STR_DISCOVER_SENSORS), attr);
        if (attr && event==EVT_KEY_BREAK(KEY_ENTER)) {
          s_editMode = 0;
          allowNewSensors = !allowNewSensors;
        }
        break;

      case ITEM_TELEMETRY_NEW_SENSOR:
        lcdDrawText(MENUS_MARGIN_LEFT+INDENT_WIDTH, y, NO_INDENT(STR_TELEMETRY_NEWSENSOR), attr);
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
        lcdDrawText(MENUS_MARGIN_LEFT+INDENT_WIDTH, y, NO_INDENT(STR_DELETE_ALL_SENSORS), attr);
        if (attr)
          s_editMode = 0;
        if (attr && event==EVT_KEY_LONG(KEY_ENTER)) {
          killEvents(KEY_ENTER);
          POPUP_CONFIRMATION(STR_CONFIRMDELETE);
        }
        break;

      case ITEM_TELEMETRY_IGNORE_SENSOR_INSTANCE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_IGNORE_INSTANCE);
        g_model.ignoreSensorIds = editCheckBox(g_model.ignoreSensorIds, TELEM_COL2, y, attr, event);
        break;

      case ITEM_TELEMETRY_RSSI_LABEL:
#if defined(MULTIMODULE)
        if (g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF &&
          g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_MULTIMODULE  &&
          g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false) == MM_RF_PROTO_FS_AFHDS2A)
          lcdDrawText(MENUS_MARGIN_LEFT, y, PSTR("RSNR"));
        else
#endif
        lcdDrawText(MENUS_MARGIN_LEFT, y, PSTR("RSSI"));
        break;

      case ITEM_TELEMETRY_RSSI_ALARM1:
      case ITEM_TELEMETRY_RSSI_ALARM2: {
        bool warning = (k==ITEM_TELEMETRY_RSSI_ALARM1);
        lcdDrawText(MENUS_MARGIN_LEFT, y, (warning ? STR_LOWALARM : STR_CRITICALALARM));
        lcdDrawNumber(TELEM_COL2, y, warning? g_model.rssiAlarms.getWarningRssi() : g_model.rssiAlarms.getCriticalRssi(), LEFT|attr, 3);

        if (attr && s_editMode>0) {
          if (warning)
            CHECK_INCDEC_MODELVAR(event, g_model.rssiAlarms.warning, -30, 30);
          else
            CHECK_INCDEC_MODELVAR(event, g_model.rssiAlarms.critical, -30, 30);
        }
        break;
      }
      case ITEM_TELEMETRY_DISABLE_ALARMS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_DISABLE_ALARM);
        g_model.rssiAlarms.disabled = editCheckBox(g_model.rssiAlarms.disabled, TELEM_COL3, y, attr, event);
        break;
#if defined(VARIO)
      case ITEM_TELEMETRY_VARIO_LABEL:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_VARIO);
        break;

      case ITEM_TELEMETRY_VARIO_SOURCE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SOURCE);
        drawSource(TELEM_COL2, y, g_model.frsky.varioSource ? MIXSRC_FIRST_TELEM+3*(g_model.frsky.varioSource-1) : 0, attr);
        if (attr) {
          g_model.frsky.varioSource = checkIncDec(event, g_model.frsky.varioSource, 0, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isSensorAvailable);
        }
        break;

      case ITEM_TELEMETRY_VARIO_RANGE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_RANGE);
        lcdDrawNumber(TELEM_COL2, y, -10+g_model.frsky.varioMin, (menuHorizontalPosition==0 ? attr : 0)|LEFT);
        lcdDrawNumber(TELEM_COL3, y, 10+g_model.frsky.varioMax, (menuHorizontalPosition==1 ? attr : 0)|LEFT);
        if (attr && s_editMode>0) {
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
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CENTER);
        lcdDrawNumber(TELEM_COL2, y, -5+g_model.frsky.varioCenterMin, (menuHorizontalPosition==0 ? attr : 0)|PREC1|LEFT);
        lcdDrawNumber(TELEM_COL3, y, 5+g_model.frsky.varioCenterMax, (menuHorizontalPosition==1 ? attr : 0)|PREC1|LEFT);
        lcdDrawTextAtIndex(TELEM_COL4, y, STR_VVARIOCENTER, g_model.frsky.varioCenterSilent, (menuHorizontalPosition==2 ? attr : 0));
        if (attr && s_editMode>0) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioCenterMin, -16, 5+min<int8_t>(10, g_model.frsky.varioCenterMax+5));
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioCenterMax, -5+max<int8_t>(-10, g_model.frsky.varioCenterMin-5), +15);
              break;
            case 2:
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.frsky.varioCenterSilent, 1);
              break;
          }
        }
        break;
#endif
    }
  }
  return true;
}
