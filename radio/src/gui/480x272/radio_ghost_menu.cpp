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

bool menuGhostModuleConfig(event_t event)
{
  switch (event) {
    case EVT_ENTRY:
      memclear(&reusableBuffer.ghostMenu, sizeof(reusableBuffer.ghostMenu));
      strAppend((char *) &reusableBuffer.ghostMenu.line[1].menuText, STR_WAITING_FOR_MODULE, 0);
      reusableBuffer.ghostMenu.line[1].lineFlags = GHST_LINE_FLAGS_ValueEdit;
      reusableBuffer.ghostMenu.buttonAction = GHST_BTN_None;
      reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_Open;
      moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
      break;

    case EVT_ROTARY_LEFT:
      reusableBuffer.ghostMenu.buttonAction = GHST_BTN_JoyUp;
      reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_None;
      moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
      audioKeyPress();
      break;

    case EVT_ROTARY_RIGHT:
      reusableBuffer.ghostMenu.buttonAction = GHST_BTN_JoyDown;
      reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_None;
      moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
      audioKeyPress();
      break;

    case EVT_KEY_FIRST(KEY_ENTER):
      reusableBuffer.ghostMenu.buttonAction = GHST_BTN_JoyPress;
      reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_None;
      moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
      audioKeyPress();
      break;

    case EVT_KEY_BREAK(KEY_EXIT):
      reusableBuffer.ghostMenu.buttonAction = GHST_BTN_JoyLeft;
      reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_None;
      moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
      audioKeyPress();
      break;

    case EVT_KEY_LONG(KEY_EXIT):
      menuVerticalOffset = 0;
      memclear(&reusableBuffer.ghostMenu, sizeof(reusableBuffer.ghostMenu));
      reusableBuffer.ghostMenu.buttonAction = GHST_BTN_None;
      reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_Close;
      moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
      RTOS_WAIT_MS(10);
      popMenu();
      break;
  }

  if (reusableBuffer.ghostMenu.menuStatus == GHST_MENU_STATUS_UNOPENED) { // Handles situation where module is plugged after tools start
    reusableBuffer.ghostMenu.buttonAction = GHST_MENU_CTRL_None;
    reusableBuffer.ghostMenu.menuAction = GHST_MENU_CTRL_Open;
    moduleState[EXTERNAL_MODULE].counter = GHST_MENU_CONTROL;
  }
  else if (reusableBuffer.ghostMenu.menuStatus == GHST_MENU_STATUS_CLOSING) {
    popMenu();
  }


  drawMenuTemplate("GHOST MENU", ICON_OPENTX, nullptr, OPTION_MENU_NO_SCROLLBAR);

  constexpr coord_t xOffset = 140;
  constexpr coord_t xOffset2 = 260;
  constexpr coord_t yOffset = 55;
  constexpr coord_t lineSpacing = 25;
  LcdFlags flags = 0;

  for (uint8_t line = 0; line < GHST_MENU_LINES; line++) {
    flags = 0;
    if (reusableBuffer.ghostMenu.line[line].splitLine) {
      if (reusableBuffer.ghostMenu.line[line].lineFlags & GHST_LINE_FLAGS_LabelSelect)
        flags = INVERS;
      lcdDrawText(xOffset, yOffset + line * lineSpacing, reusableBuffer.ghostMenu.line[line].menuText, flags);

      flags = 0;
      if (reusableBuffer.ghostMenu.line[line].lineFlags & GHST_LINE_FLAGS_ValueSelect)
        flags |= INVERS;
      if (reusableBuffer.ghostMenu.line[line].lineFlags & GHST_LINE_FLAGS_ValueEdit)
        flags |= BLINK;
      lcdDrawText(xOffset2, yOffset + line * lineSpacing, &reusableBuffer.ghostMenu.line[line].menuText[reusableBuffer.ghostMenu.line[line].splitLine], flags);
    }
    else {
      if (reusableBuffer.ghostMenu.line[line].lineFlags & GHST_LINE_FLAGS_LabelSelect) {
        lcdDrawText(xOffset, yOffset + line * lineSpacing, reusableBuffer.ghostMenu.line[line].menuText, INVERS);
      }
      else if (reusableBuffer.ghostMenu.line[line].lineFlags & GHST_LINE_FLAGS_ValueEdit) {
        if (BLINK_ON_PHASE) {
          lcdDrawText(xOffset, yOffset + line * lineSpacing, reusableBuffer.ghostMenu.line[line].menuText, flags);
        }
      }
      else {
        lcdDrawText(xOffset, yOffset + line * lineSpacing, reusableBuffer.ghostMenu.line[line].menuText, flags);
      }
    }
  }

  return true;
}
