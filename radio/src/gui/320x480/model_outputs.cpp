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

#include "model_outputs.h"
#include "opentx.h"
#include "libwindows.h"

#define SET_DIRTY()     storageDirty(EE_MODEL)

class OutputEditWindow : public Page {
  public:
    OutputEditWindow(uint8_t channel) :
      Page(),
      channel(channel)
    {
      buildBody(&body);
      buildHeader(&header);
    }

  protected:
    uint8_t channel;

    void buildHeader(Window * window)
    {
      new StaticText(window, {70, 4, 100, 20}, STR_MENULIMITS, MENU_TITLE_COLOR);
      new StaticText(window, {70, 28, 100, 20}, getSourceString(MIXSRC_CH1 + channel), MENU_TITLE_COLOR);
    }

    void buildBody(Window * window)
    {
      GridLayout grid;
      grid.spacer(8);

      int limit = (g_model.extendedLimits ? LIMIT_EXT_MAX : 1000);

      LimitData * output = limitAddress(channel);

      // Name
      new StaticText(window, grid.getLabelSlot(), STR_NAME);
      new TextEdit(window, grid.getFieldSlot(), output->name, sizeof(output->name));
      grid.nextLine();

      // Offset
      new StaticText(window, grid.getLabelSlot(), TR_LIMITS_HEADERS_SUBTRIM);
      new NumberEdit(window, grid.getFieldSlot(), -1000, +1000, GET_SET_DEFAULT(output->offset), PREC1);
      grid.nextLine();

      // Min
      new StaticText(window, grid.getLabelSlot(), TR_MIN);
      new NumberEdit(window, grid.getFieldSlot(), -limit, 0,
                     GET_VALUE(output->min - 1000),
                     SET_VALUE(output->min, newValue + 1000),
                     PREC1);
      grid.nextLine();

      // Max
      new StaticText(window, grid.getLabelSlot(), TR_MAX);
      new NumberEdit(window, grid.getFieldSlot(), 0, +limit,
                     GET_VALUE(output->max + 1000),
                     SET_VALUE(output->max, newValue - 1000),
                     PREC1);
      grid.nextLine();

      // Direction
      new StaticText(window, grid.getLabelSlot(), "Inverted"); // TODO translation
      new CheckBox(window, grid.getFieldSlot(), GET_SET_DEFAULT(output->revert));
      grid.nextLine();

      // Curve
      new StaticText(window, grid.getLabelSlot(), TR_CURVE);
      auto edit = new NumberEdit(window, grid.getFieldSlot(), -MAX_CURVES, +MAX_CURVES, GET_SET_DEFAULT(output->curve));
      edit->setDisplayHandler([](BitmapBuffer * dc, LcdFlags flags, int32_t value) {
        dc->drawText(2, 2, getCurveString(value));
      });
      grid.nextLine();

      // PPM center
      new StaticText(window, grid.getLabelSlot(), TR_LIMITS_HEADERS_PPMCENTER);
      new NumberEdit(window, grid.getFieldSlot(), PPM_CENTER - PPM_CENTER_MAX, PPM_CENTER + PPM_CENTER_MAX,
                     GET_VALUE(output->ppmCenter + PPM_CENTER),
                     SET_VALUE(output->ppmCenter, newValue - PPM_CENTER));
      grid.nextLine();

      // PPM center
      new StaticText(window, grid.getLabelSlot(), TR_LIMITS_HEADERS_SUBTRIMMODE);
      new Choice(window, grid.getFieldSlot(), "\001\306=", 0, 1, GET_SET_DEFAULT(output->symetrical));
      grid.nextLine();

      window->setInnerHeight(grid.getWindowHeight());
    }
};

static constexpr coord_t line1 = 3;
static constexpr coord_t line2 = 22;

class OutputLineButton : public Button {
  public:
    OutputLineButton(Window * parent, const rect_t &rect, LimitData * output) :
      Button(parent, rect),
      output(output)
    {
      if (output->revert || output->curve || output->name[0]) {
        setHeight(getHeight() + 20);
      }
    }

    virtual void paint(BitmapBuffer * dc) override
    {
      // first line
      drawNumber(dc, 4, line1, output->min - 1000, SMLSIZE | PREC1);
      drawNumber(dc, 68, line1, output->max + 1000, SMLSIZE | PREC1);
      drawNumber(dc, 132, line1, output->offset, SMLSIZE | PREC1);
      drawNumber(dc, 226, line1, PPM_CENTER + output->ppmCenter, SMLSIZE | RIGHT);
      dc->drawText(228, line1 - 3, output->symetrical ? "=" : "\306");

      // second line
      if (output->revert) {
        drawTextAtIndex(dc, 4, line2, STR_MMMINV, output->revert);
      }
      if (output->curve) {
        dc->drawBitmap(68, line2 + 2, mixerSetupCurveBitmap);
        dc->drawText(88, line2, getCurveString(output->curve));
      }
      if (output->name[0]) {
        dc->drawBitmap(146, line2 + 2, mixerSetupLabelBitmap);
        dc->drawSizedText(166, line2, output->name, sizeof(output->name), ZCHAR);
      }

      // bounding rect
      drawSolidRect(dc, 0, 0, rect.w, rect.h, 2, hasFocus() ? SCROLLBOX_COLOR : CURVE_AXIS_COLOR);
    }

  protected:
    LimitData * output;
};

ModelOutputsPage::ModelOutputsPage() :
  PageTab(STR_MENULIMITS, ICON_MODEL_OUTPUTS)
{
}

void ModelOutputsPage::rebuild(Window * window, int8_t focusChannel)
{
  coord_t scrollPosition = window->getScrollPositionY();
  window->clear();
  build(window, focusChannel);
  window->setScrollPositionY(scrollPosition);
}

void ModelOutputsPage::build(Window * window, int8_t focusChannel)
{
  GridLayout grid;
  grid.spacer(8);
  grid.setLabelWidth(66);

  for (uint8_t ch = 0; ch < MAX_OUTPUT_CHANNELS; ch++) {
    LimitData * output = limitAddress(ch);

    new TextButton(window, grid.getLabelSlot(), getSourceString(MIXSRC_CH1 + ch),
                   [=]() -> uint8_t {
                     return 0;
                   });

    Button * button = new OutputLineButton(window, grid.getFieldSlot(), output);
    button->setPressHandler([=]() -> uint8_t {
      Menu * menu = new Menu();
      menu->addLine(STR_EDIT, [=]() {
        editOutput(window, ch);
      });
      menu->addLine(STR_RESET, [=]() {
        output->min = 0;
        output->max = 0;
        output->offset = 0;
        output->ppmCenter = 0;
        output->revert = false;
        output->curve = 0;
        output->symetrical = 0;
        storageDirty(EE_MODEL);
        rebuild(window, ch);
      });
      menu->addLine(STR_COPY_STICKS_TO_OFS, [=]() {
        copySticksToOffset(ch);
        storageDirty(EE_MODEL);
        button->invalidate();
      });
      menu->addLine(STR_COPY_TRIMS_TO_OFS, [=]() {
        copyTrimsToOffset(ch);
        storageDirty(EE_MODEL);
        button->invalidate();
      });
      return 0;
    });

    if (focusChannel == ch) {
      button->setFocus();
    }

    grid.spacer(button->height() + 5);
  }

  grid.nextLine();

  window->setInnerHeight(grid.getWindowHeight());
}

void ModelOutputsPage::editOutput(Window * window, uint8_t channel)
{
  Window * editWindow = new OutputEditWindow(channel);
  editWindow->setCloseHandler([=]() {
    rebuild(window, channel);
  });
}
