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

#include "model_flightmodes.h"
#include "opentx.h"

#define SET_DIRTY()     storageDirty(EE_MODEL)

ModelFlightModesPage::ModelFlightModesPage():
  PageTab(STR_MENUFLIGHTMODES, ICON_MODEL_FLIGHT_MODES)
{
}

bool isTrimModeAvailable(int mode)
{
  return (mode < 0 || (mode%2) == 0 || (mode/2) != menuVerticalPosition);
}

void ModelFlightModesPage::build(Window * window)
{
  NumberEdit * edit;
  GridLayout grid;
  grid.spacer();

  for (int i = 0; i < MAX_FLIGHT_MODES; i++) {
    // Flight mode index
    char label[16];
    getFlightModeString(label, i+1);
    new Subtitle(window, grid.getLabelSlot(), label); // TODO (getFlightMode()==k ? BOLD : 0)
    grid.nextLine();

    // Flight mode name
    new StaticText(window, grid.getLabelSlot(true), STR_NAME);
    new TextEdit(window, grid.getFieldSlot(), g_model.flightModeData[i].name, LEN_FLIGHT_MODE_NAME);
    grid.nextLine();

    // Flight mode switch
    if (i > 0) {
      new StaticText(window, grid.getLabelSlot(true), STR_SWITCH);
      new SwitchChoice(window, grid.getFieldSlot(), SWSRC_FIRST_IN_MIXES, SWSRC_LAST_IN_MIXES, GET_SET_DEFAULT(g_model.flightModeData[i].swtch));
      grid.nextLine();
    }

    // Flight mode trims
    new StaticText(window, grid.getLabelSlot(true), STR_TRIMS);

    const char * STR_VTRIMS_MODES = "\002"
      "--"
      "=0"
      "+0"
      "=1"
      "+1"
      "=2"
      "+2"
      "=3"
      "+3"
      "=4"
      "+4"
      "=5"
      "+5"
      "=6"
      "+6"
      "=7"
      "+7"
      "=8"
      "+8";
    for (int t=0; t<NUM_TRIMS; t++) {
      // TODO isTrimModeAvailable to avoid +{{CURRENT}}
      new Choice(window, grid.getFieldSlot(NUM_TRIMS, t), STR_VTRIMS_MODES, -1, 2*MAX_FLIGHT_MODES-1,
                     GET_DEFAULT(g_model.flightModeData[i].trim[t].mode==TRIM_MODE_NONE ? -1 : g_model.flightModeData[i].trim[t].mode),
                     SET_DEFAULT(g_model.flightModeData[i].trim[t].mode),
                     STDSIZE);
    }
    grid.nextLine();

    // Flight mode fade in / out
    new StaticText(window, grid.getLabelSlot(true), "Fade in/out");
    edit = new NumberEdit(window, grid.getFieldSlot(2, 0), 0, DELAY_MAX,
                   GET_DEFAULT(g_model.flightModeData[i].fadeIn * (10 / DELAY_STEP)),
                   SET_VALUE(g_model.flightModeData[i].fadeIn, newValue / (10 / DELAY_STEP)),
                   PREC1);
    edit->setStep(10 / DELAY_STEP);
    edit = new NumberEdit(window, grid.getFieldSlot(2, 1), 0, DELAY_MAX,
                   GET_DEFAULT(g_model.flightModeData[i].fadeOut * (10 / DELAY_STEP)),
                   SET_VALUE(g_model.flightModeData[i].fadeOut, newValue / (10 / DELAY_STEP)),
                   PREC1);
    edit->setStep(10 / DELAY_STEP);
    grid.nextLine();
  }

  char label[32];
  sprintf(label, "Check %d Trims", mixerCurrentFlightMode);
  new TextButton(window, { 60, grid.getWindowHeight() + 5, LCD_W - 120, 30 }, label,
                 [&]() -> uint8_t {
                   if (trimsCheckTimer)
                     trimsCheckTimer = 0;
                   else
                     trimsCheckTimer = 200; // 2 seconds trims cancelled
                   return trimsCheckTimer;
                 });

  grid.nextLine();

  window->setInnerHeight(grid.getWindowHeight());
}
