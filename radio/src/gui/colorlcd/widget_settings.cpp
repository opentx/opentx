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
#include "widget_settings.h"

static const rect_t widgetSettingsDialogRect = {
  LCD_W / 10, // x
  LCD_H / 4,  // y
  LCD_W - 2 * LCD_W / 10, // width
  LCD_H - 2 * LCD_H / 4   // height
};

WidgetSettings::WidgetSettings(Window * parent, Widget * widget, std::function<void(void)> confirmHandler) :
  Dialog(parent, TR_WIDGET_SETTINGS, widgetSettingsDialogRect),
  confirmHandler(std::move(confirmHandler))
{
  auto form = &content->form;
  FormGridLayout grid(content->form.width());
  grid.setLabelWidth(width() / 5);
  form->clear();

  uint8_t optIdx = 0;
  auto optPtr = widget->getOptions();

  while (optPtr->name != nullptr) {

    auto option = *optPtr;
    new StaticText(form, grid.getLabelSlot(), option.name);

    switch (option.type) {

      case ZoneOption::Integer:
        new NumberEdit(form, grid.getFieldSlot(),
                       option.min.signedValue, // Minimum
                       option.max.signedValue, // Maximum
                       [=]() -> int {          // getValue
                           return widget->getOptionValue(optIdx)->signedValue;
                       },
                       [=](int32_t newValue) { // setValue
                           widget->setOptionValue(optIdx, OPTION_VALUE_SIGNED(newValue));
                       });
        break;

      case ZoneOption::Source:
        new SourceChoice(form, grid.getFieldSlot(),
                         option.min.unsignedValue, // min
                         option.max.unsignedValue, // max
                         [=]() -> int16_t {        // getValue
                             return (int16_t) widget->getOptionValue(optIdx)->unsignedValue;
                         },
                         [=](int16_t newValue) {   // setValue
                             widget->setOptionValue(optIdx, OPTION_VALUE_UNSIGNED((uint32_t) newValue));
                         });
        break;

      case ZoneOption::Bool:
        new CheckBox(form, grid.getFieldSlot(),
                     [=]() -> uint8_t {      // getValue
                         return (uint8_t) widget->getOptionValue(optIdx)->boolValue;
                     },
                     [=](int8_t newValue) {  // setValue
                         widget->setOptionValue(optIdx, OPTION_VALUE_BOOL((uint32_t) newValue));
                     });
        break;

      case ZoneOption::String:
        break;

      case ZoneOption::File:
        break;

      case ZoneOption::TextSize:
        break;

      case ZoneOption::Timer: // Unsigned
      {
        auto tmChoice = new Choice(form, grid.getFieldSlot(),
                                   0, TIMERS - 1,
                                   [=]() -> int {        // getValue
                                       return (int) widget->getOptionValue(optIdx)->unsignedValue;
                                   },
                                   [=](int newValue) {   // setValue
                                       widget->setOptionValue(optIdx, OPTION_VALUE_UNSIGNED((uint32_t) newValue));
                                   });

        tmChoice->setTextHandler([](int value) {
            return std::string(STR_TIMER) + std::to_string(value + 1);
        });
      }
        break;

      case ZoneOption::Switch:
        new SwitchChoice(form, grid.getFieldSlot(),
                         option.min.unsignedValue, // min
                         option.max.unsignedValue, // max
                         [=]() -> int16_t {        // getValue
                             return (uint8_t) widget->getOptionValue(optIdx)->unsignedValue;
                         },
                         [=](int16_t newValue) {   // setValue
                             widget->setOptionValue(optIdx, OPTION_VALUE_UNSIGNED((uint32_t) newValue));
                         });
        break;

      case ZoneOption::Color:
        new ColorEdit(form, grid.getFieldSlot(), [=]() -> int {        // getValue
                          return (int) widget->getOptionValue(optIdx)->unsignedValue;
                      },
                      [=](int newValue) {   // setValue
                          widget->setOptionValue(optIdx, OPTION_VALUE_UNSIGNED((uint32_t) newValue));
                      });
        break;
    }

    optIdx++;
    optPtr++;
    grid.nextLine();
  }
}
