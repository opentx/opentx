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
char s_text_screen[LCD_LINES-1][LCD_COLS+1];

void readTextFile(int & lines_count)
{
  FIL file;
  int result;
  char c;
  unsigned int sz;
  int line_length = 0;
  int escape = 0;
  char escape_chars[4] = {0};
  int current_line = 0;

  memset(s_text_screen, 0, sizeof(s_text_screen));

  result = f_open(&file, s_text_file, FA_OPEN_EXISTING | FA_READ);
  if (result == FR_OK) {
    for (int i=0; i<TEXT_FILE_MAXSIZE && f_read(&file, &c, 1, &sz)==FR_OK && sz==1 && (lines_count==0 || current_line-menuVerticalOffset<LCD_LINES-1); i++) {
      if (c == '\n') {
        ++current_line;
        line_length = 0;
        escape = 0;
      }
      else if (c!='\r' && current_line>=menuVerticalOffset && current_line-menuVerticalOffset<LCD_LINES-1 && line_length<LCD_COLS) {
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

#if defined(PCBX7) || defined(PCBX9E)
#define EVT_KEY_NEXT_LINE              EVT_ROTARY_RIGHT
#define EVT_KEY_PREVIOUS_LINE          EVT_ROTARY_LEFT
#else
#define EVT_KEY_NEXT_LINE              EVT_KEY_FIRST(KEY_DOWN)
#define EVT_KEY_PREVIOUS_LINE          EVT_KEY_FIRST(KEY_UP)
#endif

void readModelNotes()
{
  LED_ERROR_BEGIN();

  strcpy(s_text_file, MODELS_PATH "/");
  char *buf = strcat_modelname(&s_text_file[sizeof(MODELS_PATH)], g_eeGeneral.currModel);
  strcpy(buf, TEXT_EXT);

  clearKeyEvents();
  event_t event = EVT_ENTRY;
  while (event != EVT_KEY_BREAK(KEY_EXIT)) {
    lcdRefreshWait();
    lcdClear();
    menuTextView(event);
    event = getEvent();
    lcdRefresh();
    wdt_reset();
  }

  LED_ERROR_END();
}

void menuTextView(event_t event)
{
  static int lines_count;

  switch (event) {
    case EVT_ENTRY:
      menuVerticalOffset = 0;
      lines_count = 0;
      readTextFile(lines_count);
      break;

    case EVT_KEY_PREVIOUS_LINE:
      if (menuVerticalOffset == 0)
        break;
      else
        menuVerticalOffset--;
      readTextFile(lines_count);
      break;

    case EVT_KEY_NEXT_LINE:
      if (menuVerticalOffset+LCD_LINES-1 >= lines_count)
        break;
      else
        ++menuVerticalOffset;
      readTextFile(lines_count);
      break;

    case EVT_KEY_BREAK(KEY_EXIT):
      popMenu();
      break;
  }

  for (int i=0; i<LCD_LINES-1; i++) {
    lcdDrawText(0, i*FH+FH+1, s_text_screen[i], FIXEDWIDTH);
  }

  char * title = s_text_file;
#if defined(SIMU)
  if (!strncmp(title, "./", 2)) title += 2;
#else
  // TODO?
#endif
  lcdDrawText(LCD_W/2-strlen(title)*FW/2, 0, title);
  lcdInvertLine(0);

  if (lines_count > LCD_LINES-1) {
    drawVerticalScrollbar(LCD_W-1, FH, LCD_H-FH, menuVerticalOffset, lines_count, LCD_LINES-1);
  }
}

void pushMenuTextView(const char *filename)
{
  if (strlen(filename) < TEXT_FILENAME_MAXLEN) {
    strcpy(s_text_file, filename);
    pushMenu(menuTextView);
  }
}

#undef EVT_KEY_NEXT_LINE
#undef EVT_KEY_PREVIOUS_LINE
