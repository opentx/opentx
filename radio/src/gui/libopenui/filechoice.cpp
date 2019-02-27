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

#include "filechoice.h"
#include "menu.h"
#include "draw_functions.h"
#include "sdcard.h"

const uint8_t LBM_FOLDER[] = {
#include "mask_folder.lbm"
};

extern bool compare_nocase (const std::string& first, const std::string& second);

FileChoice::FileChoice(Window * parent, const rect_t & rect, std::string folder, const char * extension, int maxlen, std::function<std::string()> getValue, std::function<void(std::string)> setValue):
  Window(parent, rect),
  folder(std::move(folder)),
  extension(extension),
  maxlen(maxlen),
  getValue(std::move(getValue)),
  setValue(std::move(setValue))
{
}

void FileChoice::paint(BitmapBuffer * dc)
{
  bool hasFocus = this->hasFocus();
  LcdFlags textColor = 0;
  LcdFlags lineColor = CURVE_AXIS_COLOR;
  if (hasFocus) {
    textColor = TEXT_INVERTED_BGCOLOR;
    lineColor = TEXT_INVERTED_BGCOLOR;
  }
  dc->drawText(3, 2, getValue().c_str(), textColor);
  drawSolidRect(dc, 0, 0, rect.w, rect.h, 1, lineColor);
  dc->drawBitmapPattern(rect.w - 20, (rect.h - 11) / 2, LBM_FOLDER, lineColor);
}

#if defined(TOUCH_HARDWARE)
bool FileChoice::onTouchEnd(coord_t, coord_t)
{
  FILINFO fno;
  DIR dir;
  std::list<std::string> files;
  const char * fnExt;
  uint8_t fnLen, extLen;

  FRESULT res = f_opendir(&dir, folder.c_str()); // Open the directory
  if (res == FR_OK) {
    bool firstTime = true;
    for (;;) {
      res = sdReadDir(&dir, &fno, firstTime);
      if (res != FR_OK || fno.fname[0] == 0)
        break; // break on error or end of dir
      if (fno.fattrib & AM_DIR)
        continue; // skip subfolders
      if (fno.fattrib & AM_HID)
        continue; // skip hidden files
      if (fno.fattrib & AM_SYS)
        continue; // skip system files

      fnExt = getFileExtension(fno.fname, 0, 0, &fnLen, &extLen);
      fnLen -= extLen;

      if (!fnLen || fnLen > maxlen)
        continue; // wrong size
      if (extension && !isExtensionMatching(fnExt, extension))
        continue; // wrong extension

      files.emplace_back(fno.fname, fnLen);
    }

    // sort files
    files.sort(compare_nocase);

    auto menu = new Menu();
    int count = 0;
    int current = -1;
    std::string value = getValue();
    for (const auto &file: files) {
      menu->addLine(file, [=]() {
        setValue(file);
      });
      TRACE("%s %d %s %d", value.c_str(), value.size(), file.c_str(), file.size());
      if (value.compare(file) == 0) {
        TRACE("OK");
        current = count;
      }
      ++count;
    }

    if (current >= 0) {
      menu->select(current);
    }
  }
  setFocus();
  return true;
}
#endif
