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

Layout * currentScreen;
Widget * currentWidget;

#define SCREENS_SETUP_2ND_COLUMN        200

void onWidgetChoiceMenu(const char * result)
{
  customScreens[0]->setWidget(menuVerticalPosition, result);
}

ZoneOptionValue editZoneOption(coord_t y, const ZoneOption * option, ZoneOptionValue value, LcdFlags attr, evt_t event)
{
  lcdDrawText(MENUS_MARGIN_LEFT + INDENT_WIDTH, y, option->name);

  if (option->type == ZoneOption::Bool) {
    value.boolValue = editCheckBox(value.boolValue, SCREENS_SETUP_2ND_COLUMN, y, attr, event);
  }
  else if (option->type == ZoneOption::Integer) {
    lcdDrawNumber(SCREENS_SETUP_2ND_COLUMN, y, value.signedValue, attr | LEFT);
    if (attr) {
      CHECK_INCDEC_MODELVAR(event, value.signedValue, -30000, 30000);
    }
  }
  else if (option->type == ZoneOption::String) {
    editName(SCREENS_SETUP_2ND_COLUMN, y, value.stringValue, sizeof(value.stringValue), event, attr|EE_MODEL);
  }
  else if (option->type == ZoneOption::TextSize) {
    lcdDrawTextAtIndex(SCREENS_SETUP_2ND_COLUMN, y, "\010StandardTiny\0   Small\0  Mid\0    Double", value.unsignedValue, attr);
    if (attr) {
      value.unsignedValue = checkIncDec(event, value.unsignedValue, 0, 4, EE_MODEL);
    }
  }
  else if (option->type == ZoneOption::Timer) {
    drawStringWithIndex(SCREENS_SETUP_2ND_COLUMN, y, STR_TIMER, value.unsignedValue + 1, attr);
    if (attr) {
      value.unsignedValue = checkIncDec(event, value.unsignedValue, 0, MAX_TIMERS - 1, EE_MODEL);
    }
  }
  else if (option->type == ZoneOption::Source) {
    putsMixerSource(SCREENS_SETUP_2ND_COLUMN, y, value.unsignedValue, attr);
    if (attr) {
      CHECK_INCDEC_MODELSOURCE(event, value.unsignedValue, 1, MIXSRC_LAST);
    }
  }
  else if (option->type == ZoneOption::Color) {
    lcdSetColor(value.unsignedValue);
    lcdDrawSolidRect(SCREENS_SETUP_2ND_COLUMN, y, 40, 15, attr ? TEXT_INVERTED_BGCOLOR : TEXT_COLOR);
    lcdDrawSolidFilledRect(SCREENS_SETUP_2ND_COLUMN + 1, y + 1, 38, 13, CUSTOM_COLOR);
    if (attr) {
      CHECK_INCDEC_MODELVAR(event, value.unsignedValue, 0, 65535);
    }
  }
  return value;
}

bool menuWidgetSettings(evt_t event)
{
  linesCount = 0;
  const ZoneOption * options = currentWidget->getFactory()->getOptions();
  for (const ZoneOption * option = options; option->name; option++) {
    linesCount++;
  }

  SUBMENU_WITH_OPTIONS("Widget settings", LBM_MAINVIEWS_ICON, linesCount, OPTION_MENU_TITLE_BAR, { 0, 0, 0 });

  for (int i=0; i<NUM_BODY_LINES+1; i++) {
    coord_t y = MENU_CONTENT_TOP + i * FH;
    int k = i + menuVerticalOffset;
    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (menuVerticalPosition == k ? blink : 0);
    if (k < linesCount) {
      const ZoneOption * option = &options[k];
      ZoneOptionValue value = currentWidget->getOptionValue(k);
      value = editZoneOption(y, option, value, attr, event);
      if (attr) {
        currentWidget->setOptionValue(k, value);
      }
    }
  }

  return true;
}

void onZoneMenu(const char * result)
{
  if (result == STR_SELECT_WIDGET) {
    for (unsigned int i=0; i<countRegisteredWidgets; i++) {
      POPUP_MENU_ADD_ITEM(registeredWidgets[i]->getName());
    }
    popupMenuHandler = onWidgetChoiceMenu;
  }
  else if (result == STR_WIDGET_SETTINGS) {
    currentWidget = currentScreen->getWidget(menuVerticalPosition);
    pushMenu(menuWidgetSettings);
  }
  else if (result == STR_REMOVE_WIDGET) {
    currentScreen->setWidget(menuVerticalPosition, NULL);
  }
}

bool menuSetupWidgets(evt_t event)
{
  switch (event) {
    case EVT_ENTRY:
      menuVerticalPosition = 0;
      break;
    case EVT_KEY_BREAK(KEY_EXIT):
      popMenu();
      return false;
  }

  currentScreen->refresh(true);

  for (int i=currentScreen->getZonesCount()-1; i>=0; i--) {
    Zone zone = currentScreen->getZone(i);
    if (menuVerticalPosition == i) {
      lcdDrawSolidRect(zone.x, zone.y, zone.w, zone.h, TEXT_INVERTED_COLOR);
      lcdDrawSolidRect(zone.x-1, zone.y-1, zone.w+2, zone.h+2, TEXT_INVERTED_COLOR);
      if (event == EVT_KEY_BREAK(KEY_ENTER)) {
        Widget * widget = currentScreen->getWidget(i);
        if (widget) {
          POPUP_MENU_ADD_ITEM(STR_SELECT_WIDGET);
          if (widget->getFactory()->getOptions())
            POPUP_MENU_ADD_ITEM(STR_WIDGET_SETTINGS);
          POPUP_MENU_ADD_ITEM(STR_REMOVE_WIDGET);
          popupMenuHandler = onZoneMenu;
        }
        else {
          onZoneMenu(STR_SELECT_WIDGET);
        }
      }
    }
    else {
      lcdDrawRect(zone.x, zone.y, zone.w, zone.h, DOTTED, TEXT_INVERTED_COLOR);
    }
  }
  navigate(event, currentScreen->getZonesCount(), currentScreen->getZonesCount(), 1);
  return true;
}

bool menuSetupScreensView(evt_t event)
{
  currentScreen = customScreens[0];

  unsigned int layoutIndex = 0;
  for (unsigned int i=0; i<countRegisteredLayouts; i++) {
    if (registeredLayouts[i] == currentScreen->getFactory()) {
      layoutIndex = i;
      break;
    }
  }

  linesCount = 1;
  const ZoneOption * options = currentScreen->getFactory()->getOptions();
  for (const ZoneOption * option = options; option->name; option++) {
    linesCount++;
  }

  SUBMENU_WITH_OPTIONS("Main views setup", LBM_MAINVIEWS_ICON, linesCount, OPTION_MENU_TITLE_BAR, { 0, 0, 0 });

  for (int i=0; i<NUM_BODY_LINES+1; i++) {
    coord_t y = MENU_CONTENT_TOP + i * FH;
    int k = i + menuVerticalOffset;
    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (menuVerticalPosition == k ? blink : 0);
    if (k == 0) {
      lcdDrawText(MENUS_MARGIN_LEFT, y, "Layout");
      lcdDrawText(SCREENS_SETUP_2ND_COLUMN, y, currentScreen->getFactory()->getName(), attr);
      if (attr) {
        layoutIndex = checkIncDec(event, layoutIndex, 0, countRegisteredLayouts-1, EE_MODEL);
        if (checkIncDec_Ret) {
          customScreens[0] = registeredLayouts[layoutIndex]->create(&g_model.screenData[0].layoutData);
          strncpy(g_model.screenData[0].layoutName, customScreens[0]->getFactory()->getName(), sizeof(g_model.screenData[0].layoutName));
        }
        else if (event == EVT_KEY_LONG(KEY_ENTER)) {
          pushMenu(menuSetupWidgets);
        }
      }
    }
    else if (k < linesCount) {
      const ZoneOption * option = &options[k-1];
      ZoneOptionValue value = currentScreen->getOptionValue(k-1);
      value = editZoneOption(y, option, value, attr, event);
      if (attr) {
        currentScreen->setOptionValue(k-1, value);
      }
    }
  }

  return true;
}
