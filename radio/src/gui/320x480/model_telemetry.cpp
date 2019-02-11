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

#include "model_telemetry.h"
#include "opentx.h"
#include "libopenui.h"

#define SET_DIRTY() storageDirty(EE_MODEL)

static constexpr coord_t SENSOR_COL1 = 30;
static constexpr coord_t SENSOR_COL2 = SENSOR_COL1 + 70;
static constexpr coord_t SENSOR_COL3 = LCD_W - 50;

class SensorSourceChoice : public SourceChoice {
  public:
    SensorSourceChoice(Window * window, const rect_t &rect, uint8_t * source, IsValueAvailable isValueAvailable) :
      SourceChoice(window, rect, MIXSRC_NONE, MIXSRC_LAST_TELEM,
                   GET_DEFAULT(*source ? MIXSRC_FIRST_TELEM + 3 * (*source - 1) : MIXSRC_NONE),
                   [=](uint8_t newValue) {
                     *source = newValue == MIXSRC_NONE ? 0 : (newValue - MIXSRC_FIRST_TELEM) / 3 + 1;
                   })
    {
      setAvailableHandler([=](int16_t value) {
        if (value == MIXSRC_NONE)
          return true;
        if (value < MIXSRC_FIRST_TELEM)
          return false;
        auto qr = div(value - MIXSRC_FIRST_TELEM, 3);
        return qr.rem == 0 && isValueAvailable(qr.quot + 1);
      });
    }
};

class SensorButton : public Button {
  public:
    SensorButton(Window * parent, const rect_t &rect, uint8_t index) :
      Button(parent, rect),
      index(index)
    {
    }

    static constexpr coord_t line1 = 1;

    void checkEvents() override
    {
      TelemetryItem &telemetryItem = telemetryItems[index];
      if (telemetryItem.isFresh()) {
        invalidate();
      }
    }

    void paint(BitmapBuffer * dc) override
    {
      TelemetryItem &telemetryItem = telemetryItems[index];

      if (telemetryItem.isFresh()) {
        dc->drawSolidFilledRect(2, 2, rect.w - 4, rect.h - 4, WARNING_COLOR);
      }

      lcdDrawNumber(2, 1, index + 1, LEFT, 0, NULL, ":");

      lcdDrawSizedText(SENSOR_COL1, line1, g_model.telemetrySensors[index].label, TELEM_LABEL_LEN, ZCHAR);

      if (telemetryItem.isAvailable()) {
        LcdFlags color = telemetryItem.isOld() ? ALARM_COLOR : TEXT_COLOR;
        drawSensorCustomValue(SENSOR_COL2, line1, index, getValue(MIXSRC_FIRST_TELEM + 3 * index), LEFT | color);
      }
      else {
        lcdDrawText(SENSOR_COL2, line1, "---", CURVE_COLOR);
      }

      TelemetrySensor * sensor = & g_model.telemetrySensors[index];
      if (IS_SPEKTRUM_PROTOCOL()) {
        lcdDrawHexNumber(SENSOR_COL3, line1, sensor->id, LEFT);
      }
      else if (sensor->type == TELEM_TYPE_CUSTOM && !g_model.ignoreSensorIds) {
        lcdDrawNumber(SENSOR_COL3, line1, sensor->instance, LEFT);
      }
      drawSolidRect(dc, 0, 0, rect.w, rect.h, 2, hasFocus() ? SCROLLBOX_COLOR : CURVE_AXIS_COLOR);
    }

  protected:
    uint8_t index;
};

class SensorEditWindow : public Page {
  public:
    SensorEditWindow(uint8_t index) :
      Page(),
      index(index)
    {
      buildBody(&body);
      buildHeader(&header);
    }

    ~SensorEditWindow()
    {
      body.deleteChildren();
    }

  protected:
    uint8_t index;
    Window * sensorOneWindow = nullptr;

    void buildHeader(Window * window)
    {
      new StaticText(window, {70, 4, 200, 20}, STR_SENSOR + std::to_string(index + 1), MENU_TITLE_COLOR);
      // dynamic display of sensor value ?
      //new StaticText(window, {70, 28, 100, 20}, "SF" + std::to_string(index), MENU_TITLE_COLOR);
    }

    void updateSensorOneWindow()
    {
      // Sensor variable part
      GridLayout grid;
      sensorOneWindow->clear();
      TelemetrySensor * sensor = &g_model.telemetrySensors[index];

      if (sensor->type == TELEM_TYPE_CALCULATED) {
        // Formula
        new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_FORMULA);
        new Choice(sensorOneWindow, grid.getFieldSlot(), STR_VFORMULAS, 0, TELEM_FORMULA_LAST, GET_DEFAULT(sensor->formula),
                   [=](uint8_t newValue) {
                     sensor->formula = newValue;
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
                     SET_DIRTY();
                     telemetryItems[index].clear();
                     updateSensorOneWindow();
                   });
        grid.nextLine();
      }
      else {
        new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_ID);
        auto hex = new NumberEdit(sensorOneWindow, grid.getFieldSlot(2, 0), 0, 0xffff, GET_SET_DEFAULT(sensor->id));
        hex->setDisplayHandler([](BitmapBuffer * dc, LcdFlags flags, int32_t value) {
          lcdDrawHexNumber(2, 2, value, 0);
        });
        new NumberEdit(sensorOneWindow, grid.getFieldSlot(2, 1), 0, 0xff, GET_SET_DEFAULT(sensor->instance));
        grid.nextLine();
      }

      // Unit
      if ((sensor->type == TELEM_TYPE_CALCULATED && (sensor->formula == TELEM_FORMULA_DIST)) || sensor->isConfigurable()) {
        new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_UNIT);
        new Choice(sensorOneWindow, grid.getFieldSlot(), STR_VTELEMUNIT, 0, UNIT_MAX, GET_DEFAULT(sensor->unit),
                   [=](uint8_t newValue) {
                     sensor->unit = newValue;
                     if (sensor->unit == UNIT_FAHRENHEIT) {
                       sensor->prec = 0;
                     }
                     SET_DIRTY();
                     telemetryItems[index].clear();
                     updateSensorOneWindow();
                   });
        grid.nextLine();
      }

      // Precision
      if (sensor->isPrecConfigurable() && sensor->unit != UNIT_FAHRENHEIT) {
        new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_PRECISION);
        new Choice(sensorOneWindow, grid.getFieldSlot(), STR_VPREC, 0, 2, GET_DEFAULT(sensor->prec),
                   [=](uint8_t newValue) {
                     sensor->prec = newValue;
                     SET_DIRTY();
                     telemetryItems[index].clear();
                     updateSensorOneWindow();
                   });
        grid.nextLine();
      }

      // Params
      if (sensor->unit < UNIT_FIRST_VIRTUAL) {
        if (sensor->type == TELEM_TYPE_CALCULATED) {
          if (sensor->formula == TELEM_FORMULA_CELL) {
            new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_CELLSENSOR);
            new SensorSourceChoice(sensorOneWindow, grid.getFieldSlot(), &sensor->cell.source, isCellsSensor);
          }
          else if (sensor->formula == TELEM_FORMULA_DIST) {
            new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_GPSSENSOR);
            new SensorSourceChoice(sensorOneWindow, grid.getFieldSlot(), &sensor->dist.gps, isGPSSensor);
          }
          else if (sensor->formula == TELEM_FORMULA_CONSUMPTION) {
            new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_CURRENTSENSOR);
            new SensorSourceChoice(sensorOneWindow, grid.getFieldSlot(), &sensor->consumption.source, isSensorAvailable);
          }
          else if (sensor->formula == TELEM_FORMULA_TOTALIZE) {
            new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_SOURCE);
            new SensorSourceChoice(sensorOneWindow, grid.getFieldSlot(), &sensor->consumption.source, isSensorAvailable);
          }
          else {
            new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_SOURCE + std::to_string(1));
            new SensorSourceChoice(sensorOneWindow, grid.getFieldSlot(), (uint8_t *) &sensor->calc.sources[0], isSensorAvailable);
          }
        }
        else {
          if (sensor->unit == UNIT_RPMS) {
            new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_BLADES);
            new NumberEdit(sensorOneWindow, grid.getFieldSlot(), 1, 30000, GET_SET_DEFAULT(sensor->custom.ratio));
          }
          else {
            new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_RATIO);
            auto edit = new NumberEdit(sensorOneWindow, grid.getFieldSlot(), 0, 30000, GET_SET_DEFAULT(sensor->custom.ratio));
            edit->setZeroText("-");
          }
        }
        grid.nextLine();
      }

      if (!(sensor->unit == UNIT_GPS || sensor->unit == UNIT_DATETIME || sensor->unit == UNIT_CELLS ||
            (sensor->type == TELEM_TYPE_CALCULATED && (sensor->formula == TELEM_FORMULA_CONSUMPTION || sensor->formula == TELEM_FORMULA_TOTALIZE)))) {
        if (sensor->type == TELEM_TYPE_CALCULATED) {
          if (sensor->formula == TELEM_FORMULA_CELL) {
            new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_CELLINDEX);
            new Choice(sensorOneWindow, grid.getFieldSlot(), STR_VCELLINDEX, 0, 8, GET_SET_DEFAULT(sensor->cell.index));
          }
          else if (sensor->formula == TELEM_FORMULA_DIST) {
            new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_ALTSENSOR);
            new SensorSourceChoice(sensorOneWindow, grid.getFieldSlot(), &sensor->dist.alt, isAltSensor);
          }
          else {
            new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_SOURCE + std::to_string(2));
            new SensorSourceChoice(sensorOneWindow, grid.getFieldSlot(), (uint8_t *) &sensor->calc.sources[1], isSensorAvailable);
          }
        }
        else if (sensor->unit == UNIT_RPMS) {
          new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_MULTIPLIER);
          new NumberEdit(sensorOneWindow, grid.getFieldSlot(), 1, 30000, GET_SET_DEFAULT(sensor->custom.offset));
        }
        else {
          new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_OFFSET);
          new NumberEdit(sensorOneWindow, grid.getFieldSlot(), -30000, 30000, GET_SET_DEFAULT(sensor->custom.offset),
                         (sensor->prec > 0) ? (sensor->prec == 2 ? PREC2 : PREC1) : 0);
        }
        grid.nextLine();
      }

      if ((sensor->type == TELEM_TYPE_CALCULATED && sensor->formula < TELEM_FORMULA_MULTIPLY)) {
        new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_SOURCE + std::to_string(3));
        new SensorSourceChoice(sensorOneWindow, grid.getFieldSlot(), (uint8_t *) &sensor->calc.sources[2], isSensorAvailable);
        grid.nextLine();

        new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_SOURCE + std::to_string(4));
        new SensorSourceChoice(sensorOneWindow, grid.getFieldSlot(), (uint8_t *) &sensor->calc.sources[3], isSensorAvailable);
        grid.nextLine();
      }

      // Auto Offset
      if (sensor->unit != UNIT_RPMS && sensor->isConfigurable()) {
        new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_AUTOOFFSET);
        new CheckBox(sensorOneWindow, grid.getFieldSlot(), GET_SET_DEFAULT(sensor->autoOffset));
        grid.nextLine();
      }

      if (sensor->isConfigurable()) {
        // Only positive
        new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_ONLYPOSITIVE);
        new CheckBox(sensorOneWindow, grid.getFieldSlot(), GET_SET_DEFAULT(sensor->onlyPositive));
        grid.nextLine();

        // Filter
        new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_FILTER);
        new CheckBox(sensorOneWindow, grid.getFieldSlot(), GET_SET_DEFAULT(sensor->filter));
        grid.nextLine();
      }

      if (sensor->type == TELEM_TYPE_CALCULATED) {
        new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_PERSISTENT);
        new CheckBox(sensorOneWindow, grid.getFieldSlot(), GET_DEFAULT(sensor->persistent), [=](int32_t newValue) {
          sensor->persistent = newValue;
          if (!sensor->persistent)
            sensor->persistentValue = 0;
          SET_DIRTY();
        });
        grid.nextLine();
      }

      new StaticText(sensorOneWindow, grid.getLabelSlot(), STR_LOGS);
      new CheckBox(sensorOneWindow, grid.getFieldSlot(), GET_DEFAULT(sensor->logs), [=](int32_t newValue) {
        sensor->logs = newValue;
        logsClose();
        SET_DIRTY();
      });

      coord_t delta = sensorOneWindow->adjustHeight();
      Window * parent = sensorOneWindow->getParent();
      parent->moveWindowsTop(sensorOneWindow->top(), delta);
    }

    void buildBody(Window * window)
    {
      // Sensor one
      GridLayout grid;
      grid.spacer(8);

      TelemetrySensor * sensor = &g_model.telemetrySensors[index];

      // Name
      new StaticText(window, grid.getLabelSlot(), STR_NAME);
      new TextEdit(window, grid.getFieldSlot(), sensor->label, TELEM_LABEL_LEN);
      grid.nextLine();

      // Type
      new StaticText(window, grid.getLabelSlot(), STR_TYPE);
      new Choice(window, grid.getFieldSlot(), STR_VSENSORTYPES, 0, 1, GET_DEFAULT(sensor->type),
                 [=](uint8_t newValue) {
                   sensor->type = newValue;
                   sensor->instance = 0;
                   if (sensor->type == TELEM_TYPE_CALCULATED) {
                     sensor->param = 0;
                     sensor->filter = 0;
                     sensor->autoOffset = 0;
                   }
                   SET_DIRTY();
                   updateSensorOneWindow();
                 });
      grid.nextLine();

      sensorOneWindow = new Window(window, {0, grid.getWindowHeight(), LCD_W, 0});
      updateSensorOneWindow();
      grid.addWindow(sensorOneWindow);

      window->setInnerHeight(grid.getWindowHeight());
    }
};

ModelTelemetryPage::ModelTelemetryPage() :
  PageTab(STR_MENUTELEMETRY, ICON_MODEL_TELEMETRY)
{
}

void ModelTelemetryPage::checkEvents()
{
  if (lastKnownIndex != availableTelemetryIndex()) {
    lastKnownIndex = availableTelemetryIndex();
    rebuild(window);
  }
}

void ModelTelemetryPage::rebuild(Window * window, int8_t focusSensorIndex)
{
  coord_t scrollPosition = window->getScrollPositionY();
  window->clear();
  build(window, focusSensorIndex);
  window->setScrollPositionY(scrollPosition);
}


void ModelTelemetryPage::editSensor(Window * window, uint8_t index)
{
  Window * editWindow = new SensorEditWindow(index);
  editWindow->setCloseHandler([=]() {
    rebuild(window, index);
  });
}

void ModelTelemetryPage::build(Window * window, int8_t focusSensorIndex)
{
  GridLayout grid;
  grid.spacer(8);
  grid.setLabelWidth(180);

  this->window = window;

  // RSSI
  if (g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF &&
      g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_MULTIMODULE &&
      g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(false) == MM_RF_PROTO_FS_AFHDS2A)
    new Subtitle(window, grid.getLineSlot(), "RSNR");
  else
    new Subtitle(window, grid.getLineSlot(), "RSSI");
  grid.nextLine();

  new StaticText(window, grid.getLabelSlot(true), STR_LOWALARM);
  auto edit = new NumberEdit(window, grid.getFieldSlot(), -30, 30, GET_SET_DEFAULT(g_model.rssiAlarms.warning));
  edit->setDisplayHandler([](BitmapBuffer * dc, LcdFlags flags, int32_t value) {
    drawNumber(dc, 2, 2, g_model.rssiAlarms.getWarningRssi(), flags);
  });
  grid.nextLine();

  new StaticText(window, grid.getLabelSlot(true), STR_CRITICALALARM);
  edit = new NumberEdit(window, grid.getFieldSlot(), -30, 30, GET_SET_DEFAULT(g_model.rssiAlarms.critical));
  edit->setDisplayHandler([](BitmapBuffer * dc, LcdFlags flags, int32_t value) {
    drawNumber(dc, 2, 2, g_model.rssiAlarms.getCriticalRssi(), flags);
  });
  grid.nextLine();

  new StaticText(window, grid.getLabelSlot(true), STR_DISABLE_ALARM);
  new CheckBox(window, grid.getFieldSlot(), GET_SET_DEFAULT(g_model.rssiAlarms.disabled));
  grid.nextLine();

  // Sensors
  grid.setLabelWidth(140);
  new Subtitle(window, grid.getLineSlot(), STR_TELEMETRY_SENSORS);
  new StaticText(window, {SENSOR_COL2, grid.getWindowHeight() + 3, SENSOR_COL3 - SENSOR_COL2, lineHeight}, STR_VALUE, SMLSIZE | TEXT_DISABLE_COLOR);
  if (!g_model.ignoreSensorIds && !IS_SPEKTRUM_PROTOCOL())
    new StaticText(window, {SENSOR_COL3, grid.getWindowHeight() + 3, LCD_W - SENSOR_COL3, lineHeight}, STR_ID, SMLSIZE | TEXT_DISABLE_COLOR);
  grid.nextLine();

  for (uint8_t idx = 0; idx < MAX_TELEMETRY_SENSORS; idx++) {
    if (g_model.telemetrySensors[idx].isAvailable()) {
      Button * button = new SensorButton(window, grid.getLineSlot(), idx);
      button->setPressHandler([=]() -> uint8_t {
        button->bringToTop();
        Menu * menu = new Menu();
        menu->addLine(STR_EDIT, [=]() {
          editSensor(window, idx);
        });
        menu->addLine(STR_COPY, [=]() {
          auto newIndex = availableTelemetryIndex();
          if (newIndex >= 0) {
            TelemetrySensor &sourceSensor = g_model.telemetrySensors[idx];
            TelemetrySensor &newSensor = g_model.telemetrySensors[newIndex];
            newSensor = sourceSensor;
            TelemetryItem &sourceItem = telemetryItems[idx];
            TelemetryItem &newItem = telemetryItems[newIndex];
            newItem = sourceItem;
            SET_DIRTY();
            rebuild(window, newIndex);
          }
          else {
            new Dialog(WARNING_TYPE_ALERT, "", STR_TELEMETRYFULL);
          }
        });
        menu->addLine(STR_DELETE, [=]() {
          delTelemetryIndex(idx); // calls setDirty internally
          rebuild(window);
        });
        return 0;
      });
      if (focusSensorIndex == idx) {
        button->setFocus();
      }
      grid.nextLine();
    }
  }

  // Autodiscover button
  auto discover = new TextButton(window, grid.getLineSlot(), STR_DISCOVER_SENSORS);
  discover->setPressHandler([=]() {
    allowNewSensors = !allowNewSensors;
    if (allowNewSensors) {
      discover->setText(STR_STOP_DISCOVER_SENSORS);
      return 1;
    }
    else {
      discover->setText(STR_DISCOVER_SENSORS);
      return 0;
    }
  });
  grid.nextLine();

  // New sensor button
  new TextButton(window, grid.getLineSlot(), STR_TELEMETRY_NEWSENSOR,
                 [=]() -> uint8_t {
                   int res = availableTelemetryIndex();
                   if (res >= 0)
                     editSensor(window, res);
                   else
                     new Dialog(WARNING_TYPE_ALERT, "", STR_TELEMETRYFULL);
                   return 0;
                 });
  grid.nextLine();

  // Delete all sensors button
  new TextButton(window, grid.getLineSlot(), STR_DELETE_ALL_SENSORS,
                 []() -> uint8_t {
                   new Dialog(WARNING_TYPE_CONFIRM, STR_CONFIRMDELETE, "", [=]() {
                     for (int i = 0; i < MAX_TELEMETRY_SENSORS; i++) {
                       delTelemetryIndex(i);
                     }
                   });
                   return 0;
                 });
  grid.nextLine();

  // Ignore instance button
  new StaticText(window, grid.getLabelSlot(true), STR_IGNORE_INSTANCE);
  new CheckBox(window, grid.getFieldSlot(), GET_SET_DEFAULT(g_model.ignoreSensorIds));
  grid.nextLine();

  // Vario
  grid.setLabelWidth(100);
  new Subtitle(window, grid.getLineSlot(), STR_VARIO);
  grid.nextLine();
  new StaticText(window, grid.getLabelSlot(true), STR_SOURCE);
  auto choice = new SourceChoice(window, grid.getFieldSlot(), MIXSRC_NONE, MIXSRC_LAST_TELEM,
                                 GET_DEFAULT(g_model.frsky.varioSource ? MIXSRC_FIRST_TELEM + 3 * (g_model.frsky.varioSource - 1) : MIXSRC_NONE),
                                 SET_VALUE(g_model.frsky.varioSource, newValue == MIXSRC_NONE ? 0 : (newValue - MIXSRC_FIRST_TELEM) / 3 + 1));
  choice->setAvailableHandler([=](int16_t value) {
    if (value == MIXSRC_NONE)
      return true;
    if (value < MIXSRC_FIRST_TELEM)
      return false;
    auto qr = div(value - MIXSRC_FIRST_TELEM, 3);
    return qr.rem == 0 && isSensorAvailable(qr.quot + 1);
  });
  grid.nextLine();
  new StaticText(window, grid.getLabelSlot(true), STR_RANGE);
  new NumberEdit(window, grid.getFieldSlot(2, 0), -7, 7, GET_SET_WITH_OFFSET(g_model.frsky.varioMin, -10));
  new NumberEdit(window, grid.getFieldSlot(2, 1), -7, 7, GET_SET_WITH_OFFSET(g_model.frsky.varioMax, 10));
  grid.nextLine();
  new StaticText(window, grid.getLabelSlot(true), STR_CENTER);
  new NumberEdit(window, grid.getFieldSlot(3, 0), -7, 7, GET_SET_WITH_OFFSET(g_model.frsky.varioCenterMin, -5), PREC1);
  new NumberEdit(window, grid.getFieldSlot(3, 1), -7, 7, GET_SET_WITH_OFFSET(g_model.frsky.varioCenterMax, 5), PREC1);
  new Choice(window, grid.getFieldSlot(3, 2), STR_VVARIOCENTER, 0, 1, GET_SET_DEFAULT(g_model.frsky.varioCenterSilent));
  grid.nextLine();

  grid.nextLine();

  window->setInnerHeight(grid.getWindowHeight());
}
