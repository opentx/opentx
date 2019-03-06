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

uint8_t THEME_ICONS[] = {
  ICON_THEME,
  ICON_THEME_SETUP,
  ICON_THEME_VIEW1,
  ICON_THEME_VIEW2,
  ICON_THEME_VIEW3,
  ICON_THEME_VIEW4,
  ICON_THEME_VIEW5,
  ICON_THEME_ADD_VIEW,
};

Layout * currentScreen;
WidgetsContainerInterface * currentContainer;
Widget * currentWidget;
uint8_t currentZone;
bool widgetNeedsSettings;

#define SCREENS_SETUP_2ND_COLUMN        200

char fileSelection[LEN_ZONE_OPTION_STRING];
uint8_t fileSelectionDone;

int updateMainviewsMenu();
bool menuScreenAdd(event_t event);
void onScreenSetupMenu(const char * result);

void onZoneOptionFileSelectionMenu(const char * result)
{
  if (result == STR_UPDATE_LIST) {
    if (!sdListFiles(BITMAPS_PATH, BITMAPS_EXT, LEN_ZONE_OPTION_STRING, NULL)) {
      POPUP_WARNING(STR_NO_BITMAPS_ON_SD);
    }
  }
  else {
    fileSelectionDone = true;
    memcpy(fileSelection, result, sizeof(fileSelection));
  }
}

uint8_t getZoneOptionColumns(const ZoneOption * option)
{
  if (option->type == ZoneOption::Color) {
    return uint8_t(2 | NAVIGATION_LINE_BY_LINE);
  }
  else {
    return 0;
  }
}

uint8_t editColorPart(coord_t x, coord_t y, event_t event, uint8_t part, uint8_t value, LcdFlags attr, uint32_t i_flags)
{
  const char * STR_COLOR_PARTS = "\002" "R:" "G:" "B:";
  uint8_t PART_BITS[] = { 5, 6, 5 };
  lcdDrawTextAtIndex(x, y, STR_COLOR_PARTS, part, (attr && menuHorizontalPosition < 0) ? TEXT_INVERTED_COLOR : TEXT_COLOR);
  lcdDrawNumber(x + 20, y, value << (8-PART_BITS[part]), LEFT|TEXT_COLOR|((attr && (menuHorizontalPosition < 0 || menuHorizontalPosition == part)) ? attr : TEXT_COLOR));
  if (attr && menuHorizontalPosition == part) {
    value = checkIncDec(event, value, 0, (1 << PART_BITS[part])-1, i_flags);
  }
  return value;
}

bool editZoneOption(coord_t y, const ZoneOption * option, ZoneOptionValue * value, LcdFlags attr, uint32_t i_flags, event_t event)
{
  lcdDrawText(MENUS_MARGIN_LEFT, y, option->name);

  if (option->type == ZoneOption::Bool) {
    value->boolValue = editCheckBox(value->boolValue, SCREENS_SETUP_2ND_COLUMN, y, attr, event); // TODO always does storageDirty(EE_MODEL)
  }
  else if (option->type == ZoneOption::Integer) {
    lcdDrawNumber(SCREENS_SETUP_2ND_COLUMN, y, value->signedValue, attr | LEFT);
    if (attr) {
      CHECK_INCDEC_MODELVAR(event, value->signedValue, option->min.signedValue, option->max.signedValue); // TODO i_flags
    }
  }
  else if (option->type == ZoneOption::String) {
    editName(SCREENS_SETUP_2ND_COLUMN, y, value->stringValue, sizeof(value->stringValue), event, attr); // TODO i_flags?
  }
  else if (option->type == ZoneOption::File) {
    if (ZEXIST(value->stringValue))
      lcdDrawSizedText(SCREENS_SETUP_2ND_COLUMN, y, value->stringValue, sizeof(value->stringValue), attr);
    else
      lcdDrawTextAtIndex(SCREENS_SETUP_2ND_COLUMN, y, STR_VCSWFUNC, 0, attr); // TODO define
    if (attr) {
      if (event==EVT_KEY_FIRST(KEY_ENTER)) {
        s_editMode = 0;
        if (sdListFiles(BITMAPS_PATH, BITMAPS_EXT, sizeof(value->stringValue), value->stringValue, LIST_NONE_SD_FILE)) {
          fileSelectionDone = false;
          POPUP_MENU_START(onZoneOptionFileSelectionMenu);
        }
        else {
          POPUP_WARNING(STR_NO_BITMAPS_ON_SD);
        }
      }
      else if (fileSelectionDone) {
        memcpy(value->stringValue, fileSelection, sizeof(fileSelection));
        fileSelectionDone = false;
        storageDirty(i_flags);
      }
    }
  }
  else if (option->type == ZoneOption::TextSize) {
    lcdDrawTextAtIndex(SCREENS_SETUP_2ND_COLUMN, y, "\010StandardTiny\0   Small\0  Mid\0    Double", value->unsignedValue, attr);
    if (attr) {
      value->unsignedValue = checkIncDec(event, value->unsignedValue, 0, 4, i_flags);
    }
  }
  else if (option->type == ZoneOption::Timer) {
    drawStringWithIndex(SCREENS_SETUP_2ND_COLUMN, y, STR_TIMER, value->unsignedValue + 1, attr);
    if (attr) {
      value->unsignedValue = checkIncDec(event, value->unsignedValue, 0, MAX_TIMERS - 1, i_flags);
    }
  }
  else if (option->type == ZoneOption::Source) {
    drawSource(SCREENS_SETUP_2ND_COLUMN, y, value->unsignedValue, attr);
    if (attr) {
      CHECK_INCDEC_MODELSOURCE(event, value->unsignedValue, 1, MIXSRC_LAST_TELEM);
    }
  }
  else if (option->type == ZoneOption::Color) {
    RGB_SPLIT(value->unsignedValue, r, g, b);

    if (attr && menuHorizontalPosition < 0) {
      lcdDrawSolidFilledRect(SCREENS_SETUP_2ND_COLUMN-3, y-1, 230, FH+1, TEXT_INVERTED_BGCOLOR);
    }

    lcdSetColor(value->unsignedValue);
    lcdDrawSolidFilledRect(SCREENS_SETUP_2ND_COLUMN-1, y+1, 42, 17, TEXT_COLOR);
    lcdDrawSolidFilledRect(SCREENS_SETUP_2ND_COLUMN, y+2, 40, 15, CUSTOM_COLOR);

    r = editColorPart(SCREENS_SETUP_2ND_COLUMN + 50, y, event, 0, r, attr, i_flags);
    g = editColorPart(SCREENS_SETUP_2ND_COLUMN + 110, y, event, 1, g, attr, i_flags);
    b = editColorPart(SCREENS_SETUP_2ND_COLUMN + 170, y, event, 2, b, attr, i_flags);

    if (attr && checkIncDec_Ret) {
      value->unsignedValue = RGB_JOIN(r, g, b);
    }
  }

  return (attr && checkIncDec_Ret);
}

int getOptionsCount(const ZoneOption * options)
{
  if (options == NULL) {
    return 0;
  }
  else {
    int count = 0;
    for (const ZoneOption * option = options; option->name; option++) {
      count++;
    }
    return count;
  }
}

template <class T>
bool menuSettings(const char * title, T * object, uint32_t i_flags, event_t event)
{

  if (object->getErrorMessage()) {
    // display error instead of widget settings
    // TODO nicer display (proper word-wrap)
    SIMPLE_SUBMENU("Widget Error", ICON_MODEL_LUA_SCRIPTS, 1);
    int len = strlen(object->getErrorMessage());
    int y = 3*FH;
    const char * p = object->getErrorMessage();
    while (len > 0) {
      lcdDrawSizedText(MENUS_MARGIN_LEFT, y, p, 30);
      p += 30;
      y += FH;
      len -= 30;
    }
    return true;
  }

  const ZoneOption * options = object->getOptions();
  linesCount = getOptionsCount(options);
  uint8_t mstate_tab[MAX_WIDGET_OPTIONS];
  for (int i=0; i<linesCount; i++) {
    mstate_tab[i] = getZoneOptionColumns(&options[i]);
  }

  CUSTOM_SUBMENU_WITH_OPTIONS(title, ICON_THEME, linesCount, OPTION_MENU_TITLE_BAR);

  for (int i=0; i<NUM_BODY_LINES+1; i++) {
    coord_t y = MENU_CONTENT_TOP + i * FH;
    int k = i + menuVerticalOffset;
    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (menuVerticalPosition == k ? blink : 0);
    if (k < linesCount) {
      const ZoneOption * option = &options[k];
      ZoneOptionValue * value = object->getOptionValue(k);
      if (editZoneOption(y, option, value, attr, i_flags, event)) {
        object->update();
      }
    }
  }

  return true;
}

bool menuWidgetSettings(event_t event)
{
  return menuSettings<Widget>(STR_WIDGET_SETTINGS, currentWidget, EE_MODEL, event);
}

bool menuWidgetChoice(event_t event)
{
  static Widget * previousWidget = NULL;
  static Widget * currentWidget = NULL;
  static std::list<const WidgetFactory *>::const_iterator iterator;
  static Widget::PersistentData tempData;

  switch (event) {
    case EVT_ENTRY:
    {
      previousWidget = currentContainer->getWidget(currentZone);
      currentContainer->setWidget(currentZone, NULL);
      iterator = getRegisteredWidgets().cbegin();
      if (previousWidget) {
        const WidgetFactory * factory = previousWidget->getFactory();
        std::list<const WidgetFactory *>::const_iterator it = getRegisteredWidgets().cbegin();
        for (; it != getRegisteredWidgets().cend(); ++it) {
          if (factory->getName() == (*it)->getName()) {
            iterator = it;
            break;
          }
        }
      }
      if (iterator != getRegisteredWidgets().cend())
        currentWidget = (*iterator)->create(currentContainer->getZone(currentZone), &tempData);
      break;
    }

    case EVT_KEY_FIRST(KEY_EXIT):
      if (previousWidget) {
        if (currentWidget)
          delete currentWidget;
        currentContainer->setWidget(currentZone, previousWidget);
      }
      popMenu();
      return false;

    case EVT_KEY_FIRST(KEY_ENTER):
      if (iterator != getRegisteredWidgets().cend()) {
        if (previousWidget)
          delete previousWidget;
        currentContainer->createWidget(currentZone, *iterator);
        widgetNeedsSettings = currentContainer->getWidget(currentZone)->getFactory()->getOptions();
        storageDirty(EE_MODEL);
      }
      popMenu();
      return false;

    case EVT_ROTARY_RIGHT:
      if (iterator != getRegisteredWidgets().cend() && iterator != --getRegisteredWidgets().cend()) {
        ++iterator;
        if (currentWidget)
          delete currentWidget;
        currentWidget = (*iterator)->create(currentContainer->getZone(currentZone), &tempData);
      }
      break;

    case EVT_ROTARY_LEFT:
      if (iterator != getRegisteredWidgets().cbegin()) {
        --iterator;
        if (currentWidget)
          delete currentWidget;
        currentWidget = (*iterator)->create(currentContainer->getZone(currentZone), &tempData);
      }
      break;
  }

  currentScreen->refresh();

  Zone zone = currentContainer->getZone(currentZone);
  lcdDrawFilledRect(0, 0, zone.x-2, LCD_H, SOLID, OVERLAY_COLOR | (8<<24));
  lcdDrawFilledRect(zone.x+zone.w+2, 0, LCD_W-zone.x-zone.w-2, LCD_H, SOLID, OVERLAY_COLOR | (8<<24));
  lcdDrawFilledRect(zone.x-2, 0, zone.w+4, zone.y-2, SOLID, OVERLAY_COLOR | (8<<24));
  lcdDrawFilledRect(zone.x-2, zone.y+zone.h+2, zone.w+4, LCD_H-zone.y-zone.h-2, SOLID, OVERLAY_COLOR | (8<<24));

  if (currentWidget)
    currentWidget->refresh();

  if (iterator != getRegisteredWidgets().cbegin()) {
    lcdDrawBitmapPattern(zone.x-10, zone.y+zone.h/2-10, LBM_SWIPE_CIRCLE, TEXT_INVERTED_BGCOLOR);
    lcdDrawBitmapPattern(zone.x-10, zone.y+zone.h/2-10, LBM_SWIPE_LEFT, TEXT_INVERTED_COLOR);
  }
  if (iterator != --getRegisteredWidgets().cend()) {
    lcdDrawBitmapPattern(zone.x+zone.w-9, zone.y+zone.h/2-10, LBM_SWIPE_CIRCLE, TEXT_INVERTED_BGCOLOR);
    lcdDrawBitmapPattern(zone.x+zone.w-9, zone.y+zone.h/2-10, LBM_SWIPE_RIGHT, TEXT_INVERTED_COLOR);
  }
  if (currentWidget)
    lcdDrawText(zone.x + zone.w, zone.y-1, currentWidget->getFactory()->getName(), RIGHT | TEXT_COLOR | SMLSIZE | INVERS);

  return true;
}

void onZoneMenu(const char * result)
{
  if (result == STR_SELECT_WIDGET) {
    pushMenu(menuWidgetChoice);
  }
  else if (result == STR_WIDGET_SETTINGS) {
    pushMenu(menuWidgetSettings);
  }
  else if (result == STR_REMOVE_WIDGET) {
    currentContainer->createWidget(currentZone, NULL);
    storageDirty(EE_MODEL);
  }
}

bool menuWidgetsSetup(event_t event)
{
  switch (event) {
    case EVT_ENTRY:
      menuVerticalPosition = 0;
      break;
    case EVT_KEY_FIRST(KEY_EXIT):
      killEvents(KEY_EXIT);
      popMenu();
      return false;
  }

  currentScreen->refresh();

  for (int i=currentContainer->getZonesCount()-1; i>=0; i--) {
    Zone zone = currentContainer->getZone(i);
    LcdFlags color;
    int padding, thickness;
    if (currentContainer == topbar) {
      color = MENU_TITLE_COLOR;
      padding = 2;
      thickness = 1;
    }
    else {
      color = TEXT_INVERTED_BGCOLOR;
      padding = 4;
      thickness = 2;
    }
    if (menuVerticalPosition == i) {
      lcdDrawSolidRect(zone.x-padding, zone.y-padding, zone.w+2*padding, zone.h+2*padding, thickness, color);
      if (event == EVT_KEY_FIRST(KEY_ENTER)) {
        killEvents(KEY_ENTER);
        currentZone = menuVerticalPosition;
        currentWidget = currentContainer->getWidget(menuVerticalPosition);
        if (currentWidget) {
          POPUP_MENU_ADD_ITEM(STR_SELECT_WIDGET);
          if (currentWidget->getFactory()->getOptions())
            POPUP_MENU_ADD_ITEM(STR_WIDGET_SETTINGS);
          POPUP_MENU_ADD_ITEM(STR_REMOVE_WIDGET);
          POPUP_MENU_START(onZoneMenu);
        }
        else {
          onZoneMenu(STR_SELECT_WIDGET);
        }
      }
    }
    else {
      if (widgetNeedsSettings) {
        currentWidget = currentContainer->getWidget(menuVerticalPosition);
        if (currentWidget) {
          widgetNeedsSettings = false;
          onZoneMenu(STR_WIDGET_SETTINGS);
        }
      }
      lcdDrawRect(zone.x-padding, zone.y-padding, zone.w+2*padding, zone.h+2*padding, thickness, 0x3F, color);
    }
  }
  navigate(event, currentContainer->getZonesCount(), currentContainer->getZonesCount(), 1);
  return true;
}

template <class T>
T * editThemeChoice(coord_t x, coord_t y, std::list<T *> & elList, T * current, bool needsOffsetCheck, LcdFlags attr, event_t event)
{
  static uint8_t menuHorizontalOffset = 0;
  uint8_t elCount = elList.size(), last, idx;
  coord_t pos;
  int currentIndex = 0;
  typename std::list<T *>::const_iterator elItr = elList.cbegin();

  if (!elCount)
    return NULL;

  for (; elItr != elList.cend(); ++elItr, ++currentIndex) {
    if ((*elItr) == current)
      break;
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
      lcdDrawSolidFilledRect(x-3, y-1, min<uint8_t>(4, elCount)*56+1, 2*FH-5, TEXT_INVERTED_BGCOLOR);
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
  last = min<uint8_t>(menuHorizontalOffset + 4, elCount);
  idx = menuHorizontalOffset;
  pos = x;
  elItr = elList.cbegin();
  std::advance(elItr, min<uint8_t>(menuHorizontalOffset, elCount - 1));
  for (; idx < last && elItr != elList.cend(); ++idx, ++elItr, pos += 56) {
    (*elItr)->drawThumb(pos, y+1, current == (*elItr) ? ((attr && menuHorizontalPosition < 0) ? TEXT_INVERTED_COLOR : TEXT_INVERTED_BGCOLOR) : LINE_COLOR);
  }
  if (elCount > 4) {
    lcdDrawBitmapPattern(x - 12, y+1, LBM_CARROUSSEL_LEFT, menuHorizontalOffset > 0 ? LINE_COLOR : CURVE_AXIS_COLOR);
    lcdDrawBitmapPattern(x + 4 * 56, y+1, LBM_CARROUSSEL_RIGHT, last < getRegisteredLayouts().size() ? LINE_COLOR : CURVE_AXIS_COLOR);
  }
  if (attr && menuHorizontalPosition >= 0) {
    lcdDrawSolidRect(x + (menuHorizontalPosition - menuHorizontalOffset) * 56 - 3, y - 1, 57, 35, 1, TEXT_INVERTED_BGCOLOR);
    if (event == EVT_KEY_FIRST(KEY_ENTER)) {
      s_editMode = 0;
      elItr = elList.cbegin();
      if (menuHorizontalPosition < (int)elList.size())
        std::advance(elItr, menuHorizontalPosition);
      if (elItr != elList.cend())
        return (*elItr);
      else
        return NULL;
    }
  }
  return NULL;
}

enum menuScreensThemeItems {
  ITEM_SCREEN_SETUP_THEME,
  ITEM_SCREEN_SETUP_THEME_OPTION1 = ITEM_SCREEN_SETUP_THEME+2
};

bool menuScreensTheme(event_t event)
{
  bool needsOffsetCheck = (menuVerticalPosition != 0 || menuHorizontalPosition < 0);
  const ZoneOption * options = theme->getOptions();
  int optionsCount = getOptionsCount(options);
  linesCount = ITEM_SCREEN_SETUP_THEME_OPTION1 + optionsCount + 1;

  menuPageCount = updateMainviewsMenu();
  uint8_t mstate_tab[2 + MAX_THEME_OPTIONS + 1] = { uint8_t(NAVIGATION_LINE_BY_LINE | uint8_t(getRegisteredThemes().size()-1)), ORPHAN_ROW };
  for (int i=0; i<optionsCount; i++) {
    mstate_tab[2+i] = getZoneOptionColumns(&options[i]);
  }
  mstate_tab[2+optionsCount] = 0; // The button for the Topbar setup
  CUSTOM_MENU_WITH_OPTIONS(STR_USER_INTERFACE, THEME_ICONS, menuTabScreensSetup, menuPageCount, 0, linesCount);

  for (int i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i * FH;
    int k = i + menuVerticalOffset;
    LcdFlags blink = ((s_editMode > 0) ? BLINK | INVERS : INVERS);
    LcdFlags attr = (menuVerticalPosition == k ? blink : 0);
    switch (k) {
      case ITEM_SCREEN_SETUP_THEME: {
        lcdDrawText(MENUS_MARGIN_LEFT, y + FH / 2, STR_THEME);
        Theme * new_theme = editThemeChoice<Theme>(SCREENS_SETUP_2ND_COLUMN, y, getRegisteredThemes(), theme, needsOffsetCheck, attr, event);
        if (new_theme) {
          new_theme->init();
          loadTheme(new_theme);
          strncpy(g_eeGeneral.themeName, new_theme->getName(), sizeof(g_eeGeneral.themeName));
          killEvents(KEY_ENTER);
          storageDirty(EE_GENERAL);
        }
        break;
      }

      case ITEM_SCREEN_SETUP_THEME+1:
        break;

      default:
      {
        uint8_t index = k - ITEM_SCREEN_SETUP_THEME_OPTION1;
        if (index < optionsCount) {
          const ZoneOption * option = &options[index];
          ZoneOptionValue * value = theme->getOptionValue(index);
          bool ret = editZoneOption(y, option, value, attr, EE_GENERAL, event);
          if (option->type == ZoneOption::Color) {
            if (attr && event == EVT_KEY_FIRST(KEY_EXIT)) {
              theme->update();
            }
          }
          else if (ret) {
            theme->update();
          }
        }
        else if (index == optionsCount) {
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TOP_BAR);
          drawButton(SCREENS_SETUP_2ND_COLUMN, y, STR_SETUP, attr);
          if (attr && event == EVT_KEY_FIRST(KEY_ENTER)) {
            currentScreen = customScreens[0];
            currentContainer = topbar;
            pushMenu(menuWidgetsSetup);
          }
        }
        break;
      }
    }
  }

  return true;
}

enum MenuScreenSetupItems {
  ITEM_SCREEN_SETUP_LAYOUT,
  ITEM_SCREEN_SETUP_WIDGETS_SETUP = ITEM_SCREEN_SETUP_LAYOUT+2,
  ITEM_SCREEN_SETUP_LAYOUT_OPTION1,
};

bool menuScreenSetup(int index, event_t event)
{
  if (customScreens[index] == NULL) {
    return menuScreenAdd(event);
  }

  currentScreen = customScreens[index];
  currentContainer = currentScreen;
  bool needsOffsetCheck = (menuVerticalPosition != 0 || menuHorizontalPosition < 0);

  char title[] = "Main view X";
  title[sizeof(title)-2] = '1' + index;
  menuPageCount = updateMainviewsMenu();

  const ZoneOption * options = currentScreen->getFactory()->getOptions();
  int optionsCount = getOptionsCount(options);
  linesCount = ITEM_SCREEN_SETUP_LAYOUT_OPTION1 + optionsCount;
  if (menuPageCount > 3)
    ++linesCount;

  uint8_t mstate_tab[2 + MAX_LAYOUT_OPTIONS + 1] = { uint8_t(NAVIGATION_LINE_BY_LINE | uint8_t(getRegisteredLayouts().size()-1)), ORPHAN_ROW };
  for (int i=0; i<optionsCount; i++) {
    mstate_tab[3+i] = getZoneOptionColumns(&options[i]);
  }
  mstate_tab[3+optionsCount] = 0; // The remove button

  CUSTOM_MENU_WITH_OPTIONS(title, THEME_ICONS, menuTabScreensSetup, menuPageCount, index+1, linesCount);

  for (int i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i * FH;
    int k = i + menuVerticalOffset;
    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (menuVerticalPosition == k ? blink : 0);
    switch(k) {
      case ITEM_SCREEN_SETUP_LAYOUT:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y + FH / 2, STR_LAYOUT);
        const LayoutFactory * factory = editThemeChoice<const LayoutFactory>(SCREENS_SETUP_2ND_COLUMN, y, getRegisteredLayouts(), currentScreen->getFactory(), needsOffsetCheck, attr, event);
        if (factory) {
          delete customScreens[index];
          currentScreen = customScreens[index] = factory->create(&g_model.screenData[index].layoutData);
          strncpy(g_model.screenData[index].layoutName, factory->getName(), sizeof(g_model.screenData[index].layoutName));
          killEvents(KEY_ENTER);
          storageDirty(EE_MODEL);
        }
        break;
      }

      case ITEM_SCREEN_SETUP_LAYOUT+1:
        break;

      case ITEM_SCREEN_SETUP_WIDGETS_SETUP:
        drawButton(SCREENS_SETUP_2ND_COLUMN, y, STR_SETUP_WIDGETS, attr);
        if (attr && event == EVT_KEY_FIRST(KEY_ENTER)) {
          pushMenu(menuWidgetsSetup);
        }
        break;

      default:
      {
        uint8_t o = k - ITEM_SCREEN_SETUP_LAYOUT_OPTION1;
        if (o < optionsCount) {
          const ZoneOption * option = &options[o];
          ZoneOptionValue * value = currentScreen->getOptionValue(o);
          if (editZoneOption(y, option, value, attr, EE_MODEL, event)) {
            currentScreen->update();
          }
        }
        else if (menuPageCount > 3 && o == optionsCount) {
          drawButton(SCREENS_SETUP_2ND_COLUMN, y, STR_REMOVE_SCREEN, attr);
          if (attr && event == EVT_KEY_LONG(KEY_ENTER)) {
            delete currentScreen;
            if (index != MAX_CUSTOM_SCREENS-1) {
              memmove(&g_model.screenData[index], &g_model.screenData[index + 1], sizeof(CustomScreenData) * (MAX_CUSTOM_SCREENS - index - 1));
              memmove(&customScreens[index], &customScreens[index + 1], sizeof(Layout *) * (MAX_CUSTOM_SCREENS - index - 1));
            }
            memset(&g_model.screenData[MAX_CUSTOM_SCREENS-1], 0, sizeof(CustomScreenData));
            customScreens[MAX_CUSTOM_SCREENS-1] = NULL;
            loadCustomScreens();
            killEvents(KEY_ENTER);
            chainMenu(menuTabScreensSetup[index > 0 ? index : 1]);
            return false;
          }
        }
        break;
      }
    }
  }

  return true;
}

template<int N>
bool menuCustomScreenSetup(event_t event)
{
  return menuScreenSetup(N, event);
}

const MenuHandlerFunc menuTabScreensSetup[1+MAX_CUSTOM_SCREENS] = {
  menuScreensTheme,
  menuCustomScreenSetup<0>,
  menuCustomScreenSetup<1>,
  menuCustomScreenSetup<2>,
  menuCustomScreenSetup<3>,
  menuCustomScreenSetup<4>
};

int updateMainviewsMenu()
{
  for (int index=1; index<MAX_CUSTOM_SCREENS; index++) {
    if (customScreens[index]) {
      THEME_ICONS[2+index] = ICON_THEME_VIEW1+index;
    }
    else {
      THEME_ICONS[2+index] = ICON_THEME_ADD_VIEW;
      return 2+index;
    }
  }
  return 1+MAX_CUSTOM_SCREENS;
}

bool menuScreenAdd(event_t event)
{
  menuPageCount = updateMainviewsMenu();

  if (event == EVT_KEY_FIRST(KEY_ENTER) && getRegisteredLayouts().size()) {
    const LayoutFactory * lf = getRegisteredLayouts().front();
    customScreens[menuPageCount-2] = lf->create(&g_model.screenData[menuPageCount-2].layoutData);
    strncpy(g_model.screenData[menuPageCount-2].layoutName, lf->getName(), sizeof(g_model.screenData[menuPageCount-2].layoutName));
    s_editMode = 0;
    menuHorizontalPosition = -1;
    killEvents(KEY_ENTER);
    storageDirty(EE_MODEL);
    return false;
  }

  SIMPLE_MENU_WITH_OPTIONS(STR_ADDMAINVIEW, THEME_ICONS, menuTabScreensSetup, menuPageCount, menuPageCount-1, 0);
  return true;
}
