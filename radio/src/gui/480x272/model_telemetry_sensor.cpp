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
  drawSensorCustomValue(50, 3 + FH, s_currIdx, getValue(MIXSRC_FIRST_TELEM+3*s_currIdx), MENU_TITLE_COLOR|LEFT);

  for (uint8_t i=0; i<NUM_BODY_LINES + 1/*plus one line in submenus*/; i++) {
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
            lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SOURCE);
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
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_OFFSET);
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
        drawStringWithIndex(MENUS_MARGIN_LEFT, y, STR_SOURCE, k-SENSOR_FIELD_PARAM1+1);
        int8_t * source = &sensor->calc.sources[k-SENSOR_FIELD_PARAM1];
        if (attr) {
          *source = checkIncDec(event, *source, -MAX_TELEMETRY_SENSORS, MAX_TELEMETRY_SENSORS, EE_MODEL|NO_INCDEC_MARKS, isSensorAvailable);
        }
        if (*source < 0) {
          lcdDrawText(SENSOR_2ND_COLUMN, y, "-", attr);
          drawSource(SENSOR_2ND_COLUMN+5, y, MIXSRC_FIRST_TELEM+3*(-1-*source), attr);
        }
        else {
          drawSource(SENSOR_2ND_COLUMN, y, *source ? MIXSRC_FIRST_TELEM+3*(*source-1) : 0, attr);
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
