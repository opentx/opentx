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

#include "keyboard_curve.h"
#include "curveedit.h"
#include "button.h"
#include "lcd.h"

constexpr coord_t KEYBOARD_HEIGHT = 110;

CurveKeyboard * CurveKeyboard::_instance = nullptr;

CurveKeyboard::CurveKeyboard():
  Keyboard<CurveEdit>(KEYBOARD_HEIGHT)
{
  // up
  new TextButton(this, {LCD_W/2 - 20, 5, 40, 40}, "\200",
                 [=]() -> uint8_t {
                   if (field) {
                     field->up();
                   }
                   return 0;
                 }, BUTTON_BACKGROUND | BUTTON_NOFOCUS);

  // down
  new TextButton(this, {LCD_W/2 - 20, 65, 40, 40}, "\201",
                 [=]() -> uint8_t {
                   field->down();
                   return 0;
                 }, BUTTON_BACKGROUND | BUTTON_NOFOCUS);

  // left
  left = new TextButton(this, {LCD_W / 2 - 70, 35, 40, 40}, "\177",
                        [=]() -> uint8_t {
                          if (field) {
                            field->left();
                          }
                          return 0;
                        }, BUTTON_BACKGROUND | BUTTON_NOFOCUS);

  // right
  right = new TextButton(this, {LCD_W / 2 + 30, 35, 40, 40}, "\176",
                         [=]() -> uint8_t {
                           if (field) {
                             field->right();
                           }
                           return 0;
                         }, BUTTON_BACKGROUND | BUTTON_NOFOCUS);

  // next
  new TextButton(this, {LCD_W/2 + 80, 35, 60, 40}, "Next",
                 [=]() -> uint8_t {
                   if (field) {
                     field->next();
                   }
                   return 0;
                 }, BUTTON_BACKGROUND | BUTTON_NOFOCUS);

  // previous
  new TextButton(this, {LCD_W / 2 - 140, 35, 60, 40}, "Prev",
                 [=]() -> uint8_t {
                   if (field) {
                     field->previous();
                   }
                   return 0;
                 }, BUTTON_BACKGROUND | BUTTON_NOFOCUS);

}

CurveKeyboard::~CurveKeyboard()
{
  _instance = nullptr;
}

void CurveKeyboard::setField(CurveEdit * field)
{
  Keyboard<CurveEdit>::setField(field);
  bool custom = field->isCustomCurve();
  left->enable(custom);
  right->enable(custom);
}

void CurveKeyboard::paint(BitmapBuffer * dc)
{
  lcdSetColor(RGB(0xE0, 0xE0, 0xE0));
  dc->clear(CUSTOM_COLOR);
}
