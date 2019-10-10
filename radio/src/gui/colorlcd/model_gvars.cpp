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
#include "model_gvars.h"
#include "libopenui.h"


#define SET_DIRTY()     storageDirty(EE_MODEL)

#define TEXT_OFFSET_TOP 2
#define LINE_HEIGHT 20
#define TEXT_LEFT_MARGIN 2
#define GVAR_NAME_SIZE 50

GVarButton::GVarButton(Window * parent, const rect_t & rect, uint8_t gvar):
  Button(parent, rect),
  gvar(gvar)
{

  int perRow = (width() - GVAR_NAME_SIZE + TEXT_LEFT_MARGIN * 2) / GVAR_NAME_SIZE;
  lines = MAX_FLIGHT_MODES / perRow;
  if (MAX_FLIGHT_MODES % perRow != 0) lines++;
  setHeight(LINE_HEIGHT * (lines << 1) + TEXT_OFFSET_TOP * 2);
}


void GVarButton::checkEvents() {
  Button::checkEvents();
  if (currentFlightMode != getFlightMode()) {
    invalidate();
  }
  else {
    int32_t sum = 0;
    for (int flightMode = 0; flightMode < MAX_FLIGHT_MODES; flightMode++) {
      FlightModeData * fmData = &g_model.flightModeData[flightMode];
      sum += fmData->gvars[gvar];
    }
    if (sum != gvarSum) invalidate();
  }
}

void GVarButton::paint(BitmapBuffer * dc) {
  GVarData * gvariable = &g_model.gvars[gvar];
  // The bounding rect
  int nameRectW = TEXT_LEFT_MARGIN + GVAR_NAME_SIZE;
  coord_t x = TEXT_LEFT_MARGIN;
  coord_t y = TEXT_OFFSET_TOP;
  y += LINE_HEIGHT;
  currentFlightMode = getFlightMode();
  gvarSum = 0;

  dc->drawSolidFilledRect(0, 0, nameRectW, rect.h, CURVE_AXIS_COLOR);
  dc->drawText(2, TEXT_OFFSET_TOP, getGVarString(gvar), 0);
  dc->drawSizedText(x, y, gvariable->name, LEN_GVAR_NAME, 0);
  //values are right aligned
  x += GVAR_NAME_SIZE;
  bool bgFilled = false;

  coord_t startX = x;
  int line = 0;
  for (int flightMode = 0; flightMode < MAX_FLIGHT_MODES; flightMode++) {
    FlightModeData * fmData = &g_model.flightModeData[flightMode];
    gvar_t v = fmData->gvars[gvar];
    gvarSum += v;

    LcdFlags attr = RIGHT;
    if (flightMode == currentFlightMode) attr |= BOLD;

    x += GVAR_NAME_SIZE;
    if (x > width()) {
      bgFilled = false;
      x = nameRectW + GVAR_NAME_SIZE;
      y += LINE_HEIGHT * 2;
      startX = nameRectW;
      line += 2;
    }

    y = TEXT_OFFSET_TOP + line * LINE_HEIGHT;
    if (!bgFilled) {
      dc->drawSolidFilledRect(startX, y, width() - startX, LINE_HEIGHT, CURVE_AXIS_COLOR);
      bgFilled = true;
    }

    //Flight mode
    drawFlightMode(dc, x, y, flightMode, attr);

    coord_t yval = TEXT_OFFSET_TOP + (line + 1) * LINE_HEIGHT;

    if (v <= GVAR_MAX && (gvariable->prec > 0 || abs(v) >= 1000 || (abs(v) >= 100 && gvariable->unit > 0))) {
      attr |= SMLSIZE;
      yval += 3;
    }

    if (v > GVAR_MAX) {
      uint8_t fm = v - GVAR_MAX - 1;
      if (fm >= flightMode) fm++;
      drawFlightMode(dc, x, yval, fm, attr);
    }
    else {
      drawGVarValue(dc, x, yval, gvar, v, attr);
    }
  }

  dc->drawSolidRect(0, 0, rect.w, rect.h, 2, hasFocus() ? SCROLLBOX_COLOR : CURVE_AXIS_COLOR);
}

void GVarButton::drawFlightMode(BitmapBuffer * dc, coord_t x, coord_t y, int fm, LcdFlags attr) {
  char label[16];
  getFlightModeString(label, fm + 1);
  dc->drawSizedText(x, y, label, strlen(label), attr);
}

void GVarRenderer::paint(BitmapBuffer * dc) {
  lastFlightMode = getFlightMode();
  FlightModeData * fmData = &g_model.flightModeData[lastFlightMode];
  lastGVar = fmData->gvars[index];
  drawStringWithIndex(0, TEXT_OFFSET_TOP, TR_GV, index + 1, MENU_COLOR);
  if (lastGVar > GVAR_MAX) {
    uint8_t fm = lastGVar - GVAR_MAX - 1;
    if (fm >= lastFlightMode) fm++;
    char label[16];
    getFlightModeString(label, fm + 1);
    dc->drawSizedText(GVAR_NAME_SIZE, TEXT_OFFSET_TOP, label, strlen(label), MENU_COLOR);
  }
  else {
    drawGVarValue(dc, GVAR_NAME_SIZE, TEXT_OFFSET_TOP, index, lastGVar, MENU_COLOR);
  }
}

void GVarRenderer::checkEvents() {
  if (lastFlightMode != getFlightMode()) {
    invalidate();
    updated = true;
  }
  if (lastGVar != g_model.flightModeData[getFlightMode()].gvars[index]) {
    invalidate();
    updated = true;
  }
}
bool GVarRenderer::isUpdated() {
  if (!updated) return false;
  updated = false;
  return true;
}

const std::string GVarEditWindow::unitPercent = "%";

void GVarEditWindow::buildHeader(Window * window)
{
  new StaticText(window, {70, 4, window->width() - 70, 20}, STR_GLOBAL_VAR, MENU_COLOR);
  gVarInHeader = new GVarRenderer(window, {70, 28, window->width() - 70, 20}, index);
}
void GVarEditWindow::checkEvents() {
  Page::checkEvents();
  if (gVarInHeader && gVarInHeader->isUpdated()) {
    for (int fm = 0; fm < MAX_FLIGHT_MODES; fm++) {
      if (!values[fm]) continue;
      values[fm]->invalidate();
    }
  }
}

void GVarEditWindow::setProperties(int onlyForFlightMode) {
  GVarData * gvar = &g_model.gvars[index];
  int32_t minValue = GVAR_MIN + gvar->min;
  int32_t maxValue = GVAR_MAX - gvar->max;
  std::string suffix;
  LcdFlags prec = gvar->prec ? PREC1 : 0;
  if (gvar->unit) suffix = GVarEditWindow::unitPercent;
  if (min && max) {
    min->setMax(maxValue);
    max->setMin(minValue);

    min->setSuffix(suffix);
    max->setSuffix(suffix);

    //TODO min->setLcdFlags(prec);
    //TODO max->setLcdFlags(prec);

    min->invalidate();
    max->invalidate();
  }
  FlightModeData * fmData;
  for (int fm = 0; fm < MAX_FLIGHT_MODES; fm++) {
    if (onlyForFlightMode >= 0 && fm != onlyForFlightMode) continue;
    if (!values[fm]) continue;
    fmData = &g_model.flightModeData[fm];

    //custom value
    if (fmData->gvars[index] <= GVAR_MAX || fm == 0) {
      values[fm]->setMin(GVAR_MIN + gvar->min);
      values[fm]->setMax(GVAR_MAX - gvar->max);
      values[fm]->setDisplayHandler(nullptr);
    }
    else {
      values[fm]->setMin(GVAR_MAX + 1);
      values[fm]->setMax(GVAR_MAX + MAX_FLIGHT_MODES - 1);
      values[fm]->setDisplayHandler([=](BitmapBuffer * dc, LcdFlags flags, int32_t value) {
          uint8_t targetFlightMode = value - GVAR_MAX - 1;
          if (targetFlightMode >= fm) targetFlightMode++;
          char label[16];
          getFlightModeString(label, targetFlightMode + 1);
          dc->drawSizedText(2, 2, label, strlen(label), flags);
      });
    }

    values[fm]->setSuffix(suffix);
    //TODO values[fm]->setLcdFlags(prec);
    values[fm]->invalidate();
  }
  if (gVarInHeader) gVarInHeader->invalidate();
}

void GVarEditWindow::buildBody(Window * window) {
  GVarData * gvar = &g_model.gvars[index];

  FormGridLayout grid;
  grid.spacer(8);

  new StaticText(window, grid.getLabelSlot(), STR_NAME);
  new TextEdit(window, grid.getFieldSlot(), gvar->name, LEN_GVAR_NAME);

  grid.nextLine();

  new StaticText(window, grid.getLabelSlot(), STR_UNIT);
  new Choice(window, grid.getFieldSlot(), "\001-%", 0, 1, GET_DEFAULT(gvar->unit),
             [=](int16_t newValue) {
                 gvar->unit = newValue;
                 SET_DIRTY();
                 setProperties();
             }
  );

  grid.nextLine();

  new StaticText(window, grid.getLabelSlot(), STR_PRECISION);
  new Choice(window, grid.getFieldSlot(), STR_VPREC, 0, 1, GET_DEFAULT(gvar->prec),
             [=](int16_t newValue) {
                 gvar->prec = newValue;
                 SET_DIRTY();
                 setProperties();
             }
  );

  grid.nextLine();

  new StaticText(window, grid.getLabelSlot(), STR_MIN);
  min = new NumberEdit(window, grid.getFieldSlot(), GVAR_MIN, GVAR_MAX - gvar->max,
                       [=] { return gvar->min + GVAR_MIN; },
                       [=](int32_t newValue) {
                           gvar->min = newValue - GVAR_MIN;
                           SET_DIRTY();
                           setProperties();
                       }
  );
  grid.nextLine();

  new StaticText(window, grid.getLabelSlot(), STR_MAX);
  max = new NumberEdit(window, grid.getFieldSlot(), GVAR_MIN + gvar->min, GVAR_MAX,
                       [=] { return GVAR_MAX - gvar->max; },
                       [=](int32_t newValue) {
                           gvar->max = GVAR_MAX - newValue;
                           SET_DIRTY();
                           setProperties();
                       }
  );
  grid.nextLine();
  char flightModeName[16];
  FlightModeData * fmData;

  for (int flightMode = 0; flightMode < MAX_FLIGHT_MODES; flightMode++) {
    fmData = &g_model.flightModeData[flightMode];
    getFlightModeString(flightModeName, flightMode + 1);

    int userNameLen = zlen(fmData->name, LEN_FLIGHT_MODE_NAME);

    if (userNameLen > 0) {
      strcpy(flightModeName, fmData->name);
    }
    new StaticText(window, grid.getLabelSlot(), flightModeName);
    if (flightMode > 0) {
      CheckBox * cb = new CheckBox(window, grid.getFieldSlot(2, 0),
                                   [=] { return fmData->gvars[index] <= GVAR_MAX; }, [=](uint8_t checked) {
            if (!values[flightMode]) return;
            fmData->gvars[index] = checked ? 0 : GVAR_MAX + 1;
            setProperties(flightMode);
        });
      //TODO cb->setLabel(STR_OWN);
    }
    values[flightMode] = new NumberEdit(window, grid.getFieldSlot(2, 1), GVAR_MIN + gvar->min, GVAR_MAX + MAX_FLIGHT_MODES - 1,
                                        GET_SET_DEFAULT(fmData->gvars[index]));
    grid.nextLine();
  }

  setProperties();
  window->setInnerHeight(grid.getWindowHeight());
}

void ModelGVarsPage::rebuild(FormWindow * window) {
  coord_t scrollPosition = window->getScrollPositionY();
  window->clear();
  build(window);
  window->setScrollPositionY(scrollPosition);
}

void ModelGVarsPage::build(FormWindow * window) {
  FormGridLayout grid;
  grid.spacer(8);
  grid.setLabelWidth(70);
  for (uint8_t index = 0; index < MAX_GVARS; index++) {
    Button * button = new GVarButton(window, grid.getLineSlot(), index);
    button->setPressHandler([=]() -> uint8_t {
        Menu * menu = new Menu();
        menu->addLine(STR_EDIT, [=]() {
            Window * editWindow = new GVarEditWindow(index);
            editWindow->setCloseHandler([=]() {
                rebuild(window);
            });
        });
        menu->addLine(STR_CLEAR, [=]() {
            for (int i = 0; i < MAX_FLIGHT_MODES; i++) {
              g_model.flightModeData[i].gvars[index] = 0;
            }
            storageDirty(EE_MODEL);
        });
        return 0;
    });
    grid.nextLine(button->getHeight());
  }
  window->setInnerHeight(grid.getWindowHeight());
}