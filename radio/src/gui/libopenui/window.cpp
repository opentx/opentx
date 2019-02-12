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

#include <keys.h>
#include "window.h"

Window * Window::focusWindow = nullptr;
std::list<Window *> Window::trash;

Window::Window(Window * parent, const rect_t & rect, uint8_t flags):
  parent(parent),
  rect(rect),
  innerWidth(rect.w),
  innerHeight(rect.h),
  windowFlags(flags)
{
  focusWindow = this;

  if (parent) {
    parent->addChild(this);
    if (!(flags & TRANSPARENT)) {
      invalidate();
    }
  }
}

Window::~Window()
{
  if (focusWindow == this) {
    focusWindow = nullptr;
  }
}

void Window::attach(Window * window)
{
  if (parent)
    detach();
  parent = window;
  parent->addChild(this);
}

void Window::detach()
{
  if (parent) {
    parent->removeChild(this);
    parent = nullptr;
  }
}

void Window::deleteLater(bool detach)
{
#if defined(DEBUG_WINDOWS)
  TRACE("Delete %s", getWindowDebugString().c_str());
#endif

  if (focusWindow == this) {
    focusWindow = nullptr;
  }

  if (detach)
    this->detach();
  else
    parent = nullptr;

  if (onClose) {
    onClose();
  }

  trash.push_back(this);
}

void Window::clear()
{
  scrollPositionX = 0;
  scrollPositionY = 0;
  innerWidth = rect.w;
  innerHeight = rect.h;
  deleteChildren();
  invalidate();
}

void Window::deleteChildren()
{
  for (auto window: children) {
    window->deleteLater(false);
  }
  children.clear();
}

void Window::clearFocus()
{
  if (focusWindow) {
    focusWindow->onFocusLost();
    focusWindow = nullptr;
  }
}

void Window::setFocus()
{
#if defined(DEBUG_WINDOWS)
  TRACE("%s setFocus()", getWindowDebugString().c_str());
#endif

  if (focusWindow != this) {
    clearFocus();
    focusWindow = this;
  }
  parent->scrollTo(this);
  invalidate();
}

void Window::setScrollPositionX(coord_t value)
{
  coord_t newScrollPosition = max<coord_t>(0, min<coord_t>(innerWidth - width(), value));
  if (newScrollPosition != scrollPositionX) {
    scrollPositionX = newScrollPosition;
    invalidate();
  }
}

void Window::setScrollPositionY(coord_t value)
{
  coord_t newScrollPosition = max<coord_t>(0, min<coord_t>(innerHeight - height(), value));
  if (newScrollPosition != scrollPositionY) {
    scrollPositionY = newScrollPosition;
    invalidate();
  }
}

void Window::scrollTo(Window * child)
{
  /* TODO if (child->top() < scrollPositionY) {
    scrollPositionY = child->top();
  }*/

  coord_t bottom = child->bottom();
  Window * parent = child->getParent();
  while (parent && parent != this) {
    bottom += parent->top();
    parent = parent->getParent();
  }
  if (bottom > scrollPositionY + height() - 5) {
    setScrollPositionY(bottom - height() + 5);
  }
}

void Window::fullPaint(BitmapBuffer * dc)
{
#if defined(DEBUG_WINDOWS)
  TRACE("%s", getWindowDebugString().c_str());
#endif
  paint(dc);
  drawVerticalScrollbar(dc);
  paintChildren(dc);
}

bool Window::isChildFullSize(Window * child)
{
  return child->top() == 0 && child->height() == height() && child->left() == 0 && child->width() == width();
}

bool Window::isChildVisible(Window * window)
{
  for (auto rit = children.rbegin(); rit != children.rend(); rit++) {
    auto child = *rit;
    if (child == window) {
      return true;
    }
    if ((child->getWindowFlags() & OPAQUE) & isChildFullSize(child)) {
      return false;
    }
  }
  return false;
}

void Window::paintChildren(BitmapBuffer * dc)
{
  coord_t x = dc->getOffsetX();
  coord_t y = dc->getOffsetY();
  coord_t xmin, xmax, ymin, ymax;
  dc->getClippingRect(xmin, xmax, ymin, ymax);

  auto it = children.end();

  while(it != children.begin()) {
    auto child = *(--it);
    if ((child->windowFlags & OPAQUE) && isChildFullSize(child)) {
      break;
    }
  }

  for (; it != children.end(); it++) {
    auto child = *it;

    coord_t child_xmin = x + child->rect.x;
    if (child_xmin >= xmax)
      continue;
    coord_t child_ymin = y + child->rect.y;
    if (child_ymin >= ymax)
      continue;
    coord_t child_xmax = child_xmin + child->rect.w;
    if (child_xmax <= xmin)
      continue;
    coord_t child_ymax = child_ymin + child->rect.h;
    if (child_ymax <= ymin)
      continue;

    dc->setOffset(x + child->rect.x - child->scrollPositionX, y + child->rect.y - child->scrollPositionY);
    dc->setClippingRect(max(xmin, x + child->rect.left()),
                        min(xmax, x + child->rect.right()),
                        max(ymin, y + child->rect.top()),
                        min(ymax, y + child->rect.bottom()));
    child->fullPaint(dc);
  }
}

void Window::checkEvents()
{
  for (auto child: children) {
    child->checkEvents();
    if (child->getParent() != this) {
      // the child asked to be deleted, we can't continue, next children will be checked on next round
      break;
    }
  }

  if (this == focusWindow) {
    event_t event = getEvent(false);
    if (event) {
      TRACE("Event 0x%x received ...", event);
      this->onKeyEvent(event);
    }
  }
}

void Window::onKeyEvent(event_t event)
{
  TRACE("%s received event 0x%X", getWindowDebugString().c_str(), event);
  if (parent) {
    parent->onKeyEvent(event);
  }
}

#if defined(TOUCH_HARDWARE)
bool Window::onTouchStart(coord_t x, coord_t y)
{
  for (auto it = children.rbegin(); it != children.rend(); ++it) {
    auto child = *it;
    if (pointInRect(x, y, child->rect)) {
      if (child->onTouchStart(x - child->rect.x + child->scrollPositionX, y - child->rect.y + child->scrollPositionY)) {
        return true;
      }
    }
  }

  return false;
}

bool Window::onTouchEnd(coord_t x, coord_t y)
{
  for (auto it = children.rbegin(); it != children.rend(); ++it) {
    auto child = *it;
    if (pointInRect(x, y, child->rect)) {
      if (child->onTouchEnd(x - child->rect.x + child->scrollPositionX, y - child->rect.y + child->scrollPositionY)) {
        return true;
      }
    }
  }

  return false;
}

bool Window::onTouchSlide(coord_t x, coord_t y, coord_t startX, coord_t startY, coord_t slideX, coord_t slideY)
{
  for (auto it = children.rbegin(); it != children.rend(); ++it) {
    auto child = *it;
    if (pointInRect(startX, startY, child->rect)) {
      if (child->onTouchSlide(x - child->rect.x, y - child->rect.y, startX - child->rect.x, startY - child->rect.y, slideX, slideY)) {
        return true;
      }
    }
  }

  if (slideY && innerHeight > rect.h) {
    setScrollPositionY(scrollPositionY - slideY);
    return true;
  }

  if (slideX && innerWidth > rect.w) {
    setScrollPositionX(scrollPositionX - slideX);
    return true;
  }

  return false;
}
#endif

void Window::adjustInnerHeight()
{
  innerHeight = 0;
  for (auto child: children) {
    innerHeight = max(innerHeight, child->rect.y + child->rect.h);
  }
}

coord_t Window::adjustHeight()
{
  adjustInnerHeight();
  coord_t old = rect.h;
  rect.h = innerHeight;
  return rect.h - old;
}

void Window::moveWindowsTop(coord_t y, coord_t delta)
{
  for (auto child: children) {
    if (child->rect.y > y) {
      child->rect.y += delta;
    }
  }
  innerHeight += delta;
}

void Window::invalidate(const rect_t & rect)
{
  if (isVisible()) {
    parent->invalidate({this->rect.x + rect.x - parent->scrollPositionX, this->rect.y + rect.y - parent->scrollPositionY, rect.w, rect.h});
  }
}

void Window::drawVerticalScrollbar(BitmapBuffer * dc)
{
  if (innerHeight > rect.h) {
    coord_t x = rect.w - 3;
    coord_t y = scrollPositionY + 3;
    coord_t h = rect.h - 6;
    lcd->drawSolidFilledRect(x, y, 1, h, LINE_COLOR);
    coord_t yofs = (h*scrollPositionY + innerHeight/2) / innerHeight;
    coord_t yhgt = (h*rect.h + innerHeight/2) / innerHeight;
    if (yhgt + yofs > h)
      yhgt = h - yofs;
    dc->drawSolidFilledRect(x-1, y + yofs, 3, yhgt, scrollbarColor);
  }
}
