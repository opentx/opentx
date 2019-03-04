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

#include "page.h"
#include "mainwindow.h"
#include "keyboard_number.h"
#include "keyboard_text.h"
#include "keyboard_curve.h"
#include "opentx.h" // TODO for constants

PageHeader::PageHeader(Page * parent, uint8_t icon):
  Window(parent, { 0, 0, LCD_W, MENU_HEADER_HEIGHT }, OPAQUE),
  icon(icon)
#if defined(TOUCH_HARDWARE)
  , back(this, { 0, 0, TOPBAR_BUTTON_WIDTH, TOPBAR_BUTTON_WIDTH }, ICON_BACK,
       [=]() -> uint8_t {
         parent->deleteLater();
         return 0;
       }, BUTTON_NOFOCUS)
#endif
{
}

void PageHeader::paint(BitmapBuffer * dc)
{
  theme->drawMenuBackground(dc, icon, "");
  dc->drawSolidFilledRect(MENU_HEADER_HEIGHT, 0, LCD_W - MENU_HEADER_HEIGHT, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
}

Page::Page(unsigned icon):
  Window(&mainWindow, {0, 0, LCD_W, LCD_H}, OPAQUE),
  header(this, icon),
  body(this, { 0, MENU_HEADER_HEIGHT, LCD_W, LCD_H - MENU_HEADER_HEIGHT })
{
  setFocus();
}

Page::~Page()
{
#if defined(TOUCH_HARDWARE)
  TextKeyboard::instance()->disable(false);
  NumberKeyboard::instance()->disable(false);
  CurveKeyboard::instance()->disable(false);
#endif
}

void Page::paint(BitmapBuffer * dc)
{
  dc->clear(TEXT_BGCOLOR);
}

void Page::onKeyEvent(event_t event)
{
  TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

  if (event == EVT_KEY_LONG(KEY_EXIT) || event == EVT_KEY_BREAK(KEY_EXIT)) {
    killEvents(event);
    deleteLater();
  }
}

#if defined(TOUCH_HARDWARE)
bool Page::onTouchEnd(coord_t x, coord_t y)
{
  if (Window::onTouchEnd(x, y))
    return true;

  TextKeyboard::instance()->disable(true);
  NumberKeyboard::instance()->disable(true);
  CurveKeyboard::instance()->disable(true);
  return true;
}
#endif
