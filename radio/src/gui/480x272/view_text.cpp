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

bool menuTextView(event_t event)
{
  static int lines_count;

  drawMenuTemplate(STR_TEXT_VIEWER, ICON_OPENTX);

  switch (event) {
    case EVT_ENTRY:
      menuVerticalOffset = 0;
      lines_count = 0;
      sdReadTextFile(reusableBuffer.viewText.filename, reusableBuffer.viewText.lines, lines_count);
      break;

    case EVT_ROTARY_LEFT:
      if (menuVerticalOffset == 0)
        break;
      else
        menuVerticalOffset--;
      sdReadTextFile(reusableBuffer.viewText.filename, reusableBuffer.viewText.lines, lines_count);
      break;

    case EVT_ROTARY_RIGHT:
      if (menuVerticalOffset+NUM_BODY_LINES >= lines_count)
        break;
      else
        ++menuVerticalOffset;
      sdReadTextFile(reusableBuffer.viewText.filename, reusableBuffer.viewText.lines, lines_count);
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
      popMenu();
      break;
  }

  for (int i=0; i<NUM_BODY_LINES; i++) {
    lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + i*FH, reusableBuffer.viewText.lines[i]);
  }

#if 0
  char * title = s_text_file;
#if defined(SIMU)
  if (!strncmp(title, "./", 2)) title += 2;
#endif
  lcdDrawTextAlignedCenter(MENU_FOOTER_TOP, title, HEADER_COLOR);
#endif

  drawVerticalScrollbar(LCD_W-5, 50, 195, menuVerticalOffset, lines_count, NUM_BODY_LINES);

  return true;
}

void pushMenuTextView(const char *filename)
{
  if (strlen(filename) < TEXT_FILENAME_MAXLEN) {
    strcpy(reusableBuffer.viewText.filename, filename);
    pushMenu(menuTextView);
  }
}
