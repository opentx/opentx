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

#include "tabsgroup.h"
#include "mainwindow.h"

#if defined(HARDWARE_TOUCH)
#include "keyboard_number.h"
#include "keyboard_text.h"
#include "keyboard_curve.h"
#endif

#include "opentx.h" // TODO for constants...
#include "view_main.h"

TabsGroupHeader::TabsGroupHeader(TabsGroup * parent, uint8_t icon):
  Window(parent, { 0, 0, LCD_W, MENU_BODY_TOP }, OPAQUE),
#if defined(HARDWARE_TOUCH)
  back(this, { 0, 0, TOPBAR_BUTTON_WIDTH, TOPBAR_BUTTON_WIDTH }, ICON_OPENTX,
       [=]() -> uint8_t {
         parent->deleteLater();
         return 1;
       }, BUTTON_NOFOCUS),
#endif
  icon(icon),
  carousel(this, parent)
{
}

void TabsGroupHeader::paint(BitmapBuffer * dc)
{
  theme->drawMenuBackground(dc, icon, title);
}

TabsCarousel::TabsCarousel(Window * parent, TabsGroup * menu):
  Window(parent, { TOPBAR_BUTTON_WIDTH, 0, LCD_W - TOPBAR_BUTTON_WIDTH, MENU_HEADER_HEIGHT + 10 }, TRANSPARENT),
  menu(menu)
{
}

void TabsCarousel::updateInnerWidth()
{
  setInnerWidth(padding_left + TOPBAR_BUTTON_WIDTH * menu->tabs.size());
}

void TabsCarousel::paint(BitmapBuffer * dc)
{
  for (unsigned index = 0; index < menu->tabs.size(); index++) {
    if (index != currentIndex) {
      theme->drawMenuIcon(dc, menu->tabs[index]->icon, index, false);
    }
  }
  theme->drawMenuIcon(dc, menu->tabs[currentIndex]->icon, currentIndex, true);
}

#if defined(HARDWARE_TOUCH)
bool TabsCarousel::onTouchEnd(coord_t x, coord_t y)
{
  unsigned index = (x - padding_left) / TOPBAR_BUTTON_WIDTH;
  menu->setCurrentTab(index);
  setCurrentIndex(index);
  return true;
}
#endif

TabsGroup::TabsGroup(uint8_t icon):
  Window(&mainWindow, { 0, 0, LCD_W, LCD_H }, OPAQUE),
  header(this, icon),
  body(this, { 0, MENU_BODY_TOP, LCD_W, MENU_BODY_HEIGHT })
{
}

TabsGroup::~TabsGroup()
{
  for (auto tab: tabs) {
    delete tab;
  }

  body.deleteChildren();

#if defined(HARDWARE_TOUCH)
  TextKeyboard::instance()->disable(false);
  NumberKeyboard::instance()->disable(false);
  CurveKeyboard::instance()->disable(false);
#endif
}

void TabsGroup::addTab(PageTab * page)
{
  tabs.push_back(page);
  if (!currentTab) {
    setCurrentTab(page);
  }
  header.carousel.updateInnerWidth();
}

void TabsGroup::removeTab(unsigned index)
{
  if (currentTab == tabs[index]) {
    setCurrentTab(max<unsigned>(0, index - 1));
  }

  tabs.erase(tabs.begin() + index);
}

void TabsGroup::setCurrentTab(PageTab * tab)
{
  if (tab != currentTab) {
    clearFocus();
    body.clear();
#if defined(HARDWARE_TOUCH)
    TextKeyboard::instance()->disable(false);
    NumberKeyboard::instance()->disable(false);
    CurveKeyboard::instance()->disable(false);
#endif
    currentTab = tab;
    tab->build(&body);
    header.setTitle(tab->title.c_str());
    invalidate();
  }
}

void TabsGroup::checkEvents()
{
  Window::checkEvents();
  if (currentTab) {
    currentTab->checkEvents();
  }
}

#if defined(HARDWARE_KEYS)
void TabsGroup::onKeyEvent(event_t event)
{
  TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

  if (event == EVT_KEY_BREAK(KEY_PGDN)) {
    uint8_t current = header.carousel.getCurrentIndex() + 1;
    setCurrentTab(current >= tabs.size() ? 0 : current);
  }
  else if (event == EVT_KEY_LONG(KEY_PGDN)) {
    killEvents(event);
    uint8_t current = header.carousel.getCurrentIndex();
    setCurrentTab(current == 0 ? tabs.size() - 1 : current - 1);
  }
  else if (event == EVT_KEY_LONG(KEY_EXIT) || event == EVT_KEY_BREAK(KEY_EXIT)) {
    killEvents(event);
    ViewMain::instance->setFocus();
    deleteLater();
  }
  else if (parent) {
    parent->onKeyEvent(event);
  }
}
#endif

void TabsGroup::paint(BitmapBuffer * dc)
{
  dc->clear(TEXT_BGCOLOR);
}

#if defined(HARDWARE_TOUCH)
bool TabsGroup::onTouchEnd(coord_t x, coord_t y)
{
  if (Window::onTouchEnd(x, y))
    return true;

  TextKeyboard::instance()->disable(true);
  NumberKeyboard::instance()->disable(true);
  CurveKeyboard::instance()->disable(true);
  return true;
}
#endif
