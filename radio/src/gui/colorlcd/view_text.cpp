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

#define TEXT_FILE_MAXSIZE     2048

char s_text_file[TEXT_FILENAME_MAXLEN];
char s_text_screen[NUM_BODY_LINES][LCD_COLS+1];

void readTextFile(int & lines_count)
{
  FIL file;
  int result;
  char c;
  unsigned int sz;
  int line_length = 0;
  int escape = 0;
  char escape_chars[2];
  int current_line = 0;

  memset(s_text_screen, 0, sizeof(s_text_screen));

  result = f_open(&file, s_text_file, FA_OPEN_EXISTING | FA_READ);
  if (result == FR_OK) {
    for (int i=0; i<TEXT_FILE_MAXSIZE && f_read(&file, &c, 1, &sz)==FR_OK && sz==1 && (lines_count==0 || current_line-menuVerticalOffset<NUM_BODY_LINES); i++) {
      if (c == '\n') {
        ++current_line;
        line_length = 0;
        escape = 0;
      }
      else if (c!='\r' && current_line>=menuVerticalOffset && current_line-menuVerticalOffset<NUM_BODY_LINES && line_length<LCD_COLS) {
        if (c=='\\' && escape==0) {
          escape = 1;
          continue;
        }
        else if (c!='\\' && escape>0 && escape<4) {
          escape_chars[escape-1] = c;
          if (escape == 2 && !strncmp(escape_chars, "up", 2)) {
            c = '\300';
            escape = 0;
          }
          else if (escape == 2 && !strncmp(escape_chars, "dn", 2)) {
            c = '\301';
            escape = 0;
          }
          else if (escape == 3) {
            int val = atoi(escape_chars);
            if (val >= 200 && val < 225) {
              c = '\200' + val-200;
            }
            escape = 0;
          }
          else {
            escape++;
            continue;
          }
        }
        else if (c=='~') {
          c = 'z'+1;
        }
        else if (c=='\t') {
          c = 0x1D; //tab
        }
        escape = 0;
        s_text_screen[current_line-menuVerticalOffset][line_length++] = c;
      }
    }
    if (c != '\n') {
      current_line += 1;
    }
    f_close(&file);
  }

  if (lines_count == 0) {
    lines_count = current_line;
  }
}

bool menuTextView(event_t event)
{
  static int lines_count;

  drawMenuTemplate(STR_TEXT_VIEWER, ICON_OPENTX);

  switch (event) {
    case EVT_ENTRY:
      menuVerticalOffset = 0;
      lines_count = 0;
      readTextFile(lines_count);
      break;

    case EVT_ROTARY_LEFT:
      if (menuVerticalOffset == 0)
        break;
      else
        menuVerticalOffset--;
      readTextFile(lines_count);
      break;

    case EVT_ROTARY_RIGHT:
      if (menuVerticalOffset+NUM_BODY_LINES >= lines_count)
        break;
      else
        ++menuVerticalOffset;
      readTextFile(lines_count);
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
      popMenu();
      break;
  }

  for (int i=0; i<NUM_BODY_LINES; i++) {
    lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + i*FH, s_text_screen[i]);
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
    strcpy(s_text_file, filename);
    pushMenu(menuTextView);
  }
}
