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
  storageDirty(EE_MODEL);
}

ZoneOptionValue editZoneOption(coord_t y, const ZoneOption * option, ZoneOptionValue value, LcdFlags attr, evt_t event)
{
  lcdDrawText(MENUS_MARGIN_LEFT, y, option->name);

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
    editName(SCREENS_SETUP_2ND_COLUMN, y, value.stringValue, sizeof(value.stringValue), event, attr);
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
    storageDirty(EE_MODEL);
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

extern MenuHandlerFunc menuTabMainviews[1+MAX_CUSTOM_SCREENS];
extern int updateMainviewsMenu();

template <class T>
T * editThemeChoice(coord_t x, coord_t y, T * array[], uint8_t count, T * current, bool needsOffsetCheck, LcdFlags attr, evt_t event)
{
  static uint8_t menuHorizontalOffset;

  int currentIndex = 0;
  for (unsigned int i=0; i<count; i++) {
    if (array[i] == current) {
      currentIndex = i;
      break;
    }
  }

  if (event == EVT_ENTRY) {
    menuHorizontalOffset = 0;
    needsOffsetCheck = true;
  }

  if (needsOffsetCheck) {
    if (currentIndex < menuHorizontalOffset) {
      menuHorizontalOffset = currentIndex;
    }
    else if (currentIndex > menuHorizontalOffset + 3) {
      menuHorizontalOffset = currentIndex - 3;
    }
  }
  if (attr) {
    if (menuHorizontalPosition < 0) {
      lcdDrawSolidFilledRect(x-3, y-2, min<uint8_t>(4, count)*56+1, 2*FH-5, TEXT_INVERTED_BGCOLOR);
    }
    else {
      if (needsOffsetCheck) {
        menuHorizontalPosition = currentIndex;
      }
      else if (menuHorizontalPosition < menuHorizontalOffset) {
        menuHorizontalOffset = menuHorizontalPosition;
      }
      else if (menuHorizontalPosition > menuHorizontalOffset + 3) {
        menuHorizontalOffset = menuHorizontalPosition - 3;
      }
    }
  }
  unsigned int last = min<int>(menuHorizontalOffset + 4, count);
  for (unsigned int i=menuHorizontalOffset, pos=x; i<last; i++, pos += 56) {
    T * element = array[i];
    element->drawThumb(pos, y, current == element ? ((attr && menuHorizontalPosition < 0) ? TEXT_INVERTED_COLOR : TEXT_INVERTED_BGCOLOR) : LINE_COLOR);
  }
  if (count > 4) {
    lcdDrawBitmapPattern(x - 12, y, LBM_CARROUSSEL_LEFT, menuHorizontalOffset > 0 ? LINE_COLOR : CURVE_AXIS_COLOR);
    lcdDrawBitmapPattern(x + 4 * 56, y, LBM_CARROUSSEL_RIGHT, last < countRegisteredLayouts ? LINE_COLOR : CURVE_AXIS_COLOR);
  }
  if (attr && menuHorizontalPosition >= 0) {
    lcdDrawSolidRect(x + (menuHorizontalPosition - menuHorizontalOffset) * 56 - 3, y - 2, 57, 35, TEXT_INVERTED_BGCOLOR);
    if (menuHorizontalPosition != currentIndex && event == EVT_KEY_BREAK(KEY_ENTER)) {
      s_editMode = 0;
      return array[menuHorizontalPosition];
    }
  }
  return NULL;
}

bool menuScreensTheme(evt_t event)
{
  bool needsOffsetCheck = (menuVerticalPosition != 0 || menuHorizontalPosition < 0);

  int menuItemsCount = updateMainviewsMenu();
  MENU_WITH_OPTIONS("User interface", LBM_MAINVIEWS_ICONS, menuTabMainviews, menuItemsCount, 0, 1, { uint8_t(NAVIGATION_LINE_BY_LINE|uint8_t(countRegisteredLayouts-1)), ORPHAN_ROW, 0, 0, 0, 0 });

  for (int i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i * FH;
    int k = i + menuVerticalOffset;
    LcdFlags blink = ((s_editMode > 0) ? BLINK | INVERS : INVERS);
    LcdFlags attr = (menuVerticalPosition == k ? blink : 0);
    switch (k) {
      case 0: {
        lcdDrawText(MENUS_MARGIN_LEFT, y + FH / 2, "Theme");
        const Theme * new_theme = editThemeChoice<const Theme>(SCREENS_SETUP_2ND_COLUMN, y, registeredThemes, countRegisteredThemes, theme, needsOffsetCheck, attr, event);
        if (new_theme) {
          theme = new_theme;
          theme->load();
          // strncpy(g_model.screenData[T].layoutName, factory->getName(), sizeof(g_model.screenData[T].layoutName));
          storageDirty(EE_MODEL);
        }
        break;
      }

      case 1:
        break;
    }
  }
}

bool menuScreenAdd(evt_t event)
{
  int menuItemsCount = updateMainviewsMenu();
  MENU_WITH_OPTIONS("Add main view", LBM_MAINVIEWS_ICONS, menuTabMainviews, menuItemsCount, menuItemsCount-1, 0, { uint8_t(NAVIGATION_LINE_BY_LINE|uint8_t(countRegisteredLayouts-1)), ORPHAN_ROW, 0, 0, 0, 0 });
}

template <int T>
bool menuScreenSetup(evt_t event)
{
  currentScreen = customScreens[T];

  bool needsOffsetCheck = (menuVerticalPosition != 0 || menuHorizontalPosition < 0);

  linesCount = 3;
  const ZoneOption * options = currentScreen->getFactory()->getOptions();
  for (const ZoneOption * option = options; option->name; option++) {
    linesCount++;
  }

  char title[] = "Main view X";
  title[sizeof(title)-2] = '1' + T;
  int menuItemsCount = updateMainviewsMenu();
  MENU_WITH_OPTIONS(title, LBM_MAINVIEWS_ICONS, menuTabMainviews, menuItemsCount, T+1, linesCount, { uint8_t(NAVIGATION_LINE_BY_LINE|uint8_t(countRegisteredLayouts-1)), ORPHAN_ROW, 0, 0, 0, 0 });

  for (int i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i * FH;
    int k = i + menuVerticalOffset;
    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (menuVerticalPosition == k ? blink : 0);
    switch(k) {
      case 0:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y + FH / 2, "Layout");
        const LayoutFactory * factory = editThemeChoice<const LayoutFactory>(SCREENS_SETUP_2ND_COLUMN, y, registeredLayouts, countRegisteredLayouts, currentScreen->getFactory(), needsOffsetCheck, attr, event);
        if (factory) {
          customScreens[T] = factory->create(&g_model.screenData[T].layoutData);
          strncpy(g_model.screenData[T].layoutName, factory->getName(), sizeof(g_model.screenData[T].layoutName));
          storageDirty(EE_MODEL);
        }
        break;
      }

      case 1:
        break;

      case 2:
        drawButton(SCREENS_SETUP_2ND_COLUMN, y, "Setup widgets", attr);
        if (attr && event == EVT_KEY_BREAK(KEY_ENTER)) {
          pushMenu(menuSetupWidgets);
        }
        break;

      default:
        if (k < linesCount) {
          uint8_t index = k - 3;
          const ZoneOption *option = &options[index];
          ZoneOptionValue value = currentScreen->getOptionValue(index);
          value = editZoneOption(y, option, value, attr, event);
          if (attr) {
            currentScreen->setOptionValue(index, value);
          }
        }
        break;
    }
  }

  return true;
}

MenuHandlerFunc menuTabMainviews[1+MAX_CUSTOM_SCREENS] = {
  menuScreensTheme,
  menuScreenSetup<0>,
  menuScreenSetup<1>,
  menuScreenSetup<2>,
  menuScreenSetup<3>,
  menuScreenSetup<4>
};

const MenuHandlerFunc menuMainviews[MAX_CUSTOM_SCREENS] = {
  menuScreenSetup<0>,
  menuScreenSetup<1>,
  menuScreenSetup<2>,
  menuScreenSetup<3>,
  menuScreenSetup<4>
};

int updateMainviewsMenu()
{
  for (int index=1; index<MAX_CUSTOM_SCREENS; index++) {
    if (customScreens[index]) {
      menuTabMainviews[1+index] = menuMainviews[index];
      LBM_MAINVIEWS_ICONS[2+index] = LBM_MAINVIEWS_ITEM_OUT_ICON;
    }
    else {
      menuTabMainviews[1+index] = menuScreenAdd;
      LBM_MAINVIEWS_ICONS[2+index] = LBM_MAINVIEWS_ADD_ICON;
      return 2+index;
    }
  }
  return 1+MAX_CUSTOM_SCREENS;
}