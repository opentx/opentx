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

#if defined(SPLASH)

#include "bmp_splash.lbm"

void drawSplash()
{
    static bool     loadImgFromSD  = true;
    static BitmapBuffer* splashImg = NULL;

    if (loadImgFromSD && splashImg == NULL) {
        bool sd_mounted = sdMounted();
        if (!sd_mounted) sdInit();
        splashImg = BitmapBuffer::load(BITMAPS_PATH "/" SPLASH_FILE);
        loadImgFromSD = false;
        if (!sd_mounted) sdDone();
    }

    lcd->clear();

    if (splashImg) {
        lcd->drawBitmap((LCD_W - splashImg->getWidth())/2,
                        (LCD_H - splashImg->getHeight())/2,
                        splashImg);
    }
    else {
        lcd->drawBitmap((LCD_W - BMP_SPLASH.getWidth())/2,
                        (LCD_H - BMP_SPLASH.getHeight())/2,
                        &BMP_SPLASH);
    }

#if MENUS_LOCK == 1
  if (readonly == false) {
    lcdDrawSolidFilledRect((LCD_W-(sizeof(TR_UNLOCKED)-1)*FW)/2 - 9, 50, (sizeof(TR_UNLOCKED)-1)*FW+16, 11, SOLID|ROUND);
    lcdDrawText((LCD_W-(sizeof(TR_UNLOCKED)-1)*FW)/2 , 53, STR_UNLOCKED);
  }
#endif

  lcdRefresh();
}
#endif
