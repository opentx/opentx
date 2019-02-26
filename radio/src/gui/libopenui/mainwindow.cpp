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

#include "lcd.h"
#include "mainwindow.h"

#if defined(TOUCH_HARDWARE)
#include "touch_driver.h"
#endif

void DMACopy(void * src, void * dest, unsigned len);

MainWindow mainWindow;

#if defined(TOUCH_HARDWARE)
STRUCT_TOUCH touchState;
#endif

void MainWindow::emptyTrash()
{
  for (auto window: trash) {
    delete window;
  }
  trash.clear();
}


void MainWindow::checkEvents()
{
#if defined(TOUCH_HARDWARE)
  if (touchState.Event == TE_DOWN) {
    onTouchStart(touchState.X + scrollPositionX, touchState.Y + scrollPositionY);
    // touchState.Event = TE_NONE;
  }
  else if (touchState.Event == TE_UP) {
    touchState.Event = TE_NONE;
    onTouchEnd(touchState.startX + scrollPositionX, touchState.startY + scrollPositionY);
  }
  else if (touchState.Event == TE_SLIDE) {
    coord_t x = touchState.X - touchState.lastX;
    coord_t y = touchState.Y - touchState.lastY;
    onTouchSlide(touchState.X, touchState.Y, touchState.startX, touchState.startY, x, y);
    touchState.lastX = touchState.X;
    touchState.lastY = touchState.Y;
  }
#endif

  Window::checkEvents();

  emptyTrash();
}

void MainWindow::invalidate(const rect_t & rect)
{
  if (invalidatedRect.w) {
    coord_t left = min(invalidatedRect.left(), rect.left());
    coord_t right = max(invalidatedRect.right(), rect.right());
    coord_t top = min(invalidatedRect.top(), rect.top());
    coord_t bottom = max(invalidatedRect.bottom(), rect.bottom());
    invalidatedRect = {left, top, right - left, bottom - top};
  }
  else {
    invalidatedRect = rect;
  }
}

bool MainWindow::refresh()
{
  if (invalidatedRect.w) {
    if (invalidatedRect.x > 0 || invalidatedRect.y > 0 || invalidatedRect.w < LCD_W || invalidatedRect.h < LCD_H) {
      TRACE_WINDOWS("Refresh rect: left=%d top=%d width=%d height=%d", invalidatedRect.left(), invalidatedRect.top(), invalidatedRect.w, invalidatedRect.h);
      BitmapBuffer * previous = lcd;
      lcdNextLayer();
      DMACopy(previous->getData(), lcd->getData(), DISPLAY_BUFFER_SIZE);
    }
    else {
      TRACE_WINDOWS("Refresh full screen");
      lcdNextLayer();
    }
    lcd->setOffset(0, 0);
    lcd->setClippingRect(invalidatedRect.left(), invalidatedRect.right(), invalidatedRect.top(), invalidatedRect.bottom());
    fullPaint(lcd);
    invalidatedRect.w = 0;
    return true;
  }
  else {
    return false;
  }
}

void MainWindow::run()
{
  checkEvents();
  if (refresh()) {
    lcdRefresh();
  }
}
