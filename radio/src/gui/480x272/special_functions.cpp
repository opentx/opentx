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

#include "special_functions.h"
#include "opentx.h"
#include "libopenui.h"

#define SET_DIRTY()     storageDirty(functions == g_model.customFn ? EE_MODEL : EE_GENERAL)

class SpecialFunctionEditWindow : public Page {
  public:
    SpecialFunctionEditWindow(CustomFunctionData * functions, uint8_t index) :
      Page(),
      functions(functions),
      index(index)
    {
      buildBody(&body);
      buildHeader((FormWindow *) &header);
    }

  protected:
    CustomFunctionData * functions;
    uint8_t index;
    FormWindow * specialFunctionOneWindow = nullptr;
    StaticText * headerSF = nullptr;
    bool active = false;

    bool isActive() {
      return ((functions == g_model.customFn ? modelFunctionsContext.activeSwitches : globalFunctionsContext.activeSwitches) & ((MASK_CFN_TYPE)1 << index) ? 1 : 0);
    }

    void checkEvents() override
    {
      if (active != isActive()) {
        invalidate();
        headerSF->setFlags(isActive() ? BOLD|WARNING_COLOR : MENU_TITLE_COLOR);
        active = !active;
      }
    }

    void buildHeader(FormWindow * window)
    {
      new StaticText(window, {70, 4, 200, 20}, functions == g_model.customFn ? STR_MENUCUSTOMFUNC : STR_MENUSPECIALFUNCS, MENU_TITLE_COLOR);
      headerSF = new StaticText(window, {70, 28, 100, 20}, (functions == g_model.customFn ? "SF" : "GF" ) + std::to_string(index), MENU_TITLE_COLOR);
    }

    void updateSpecialFunctionOneWindow()
    {
      // SF.one variable part
      GridLayout grid;
      specialFunctionOneWindow->clear();

      CustomFunctionData * cfn = &functions[index];
      uint8_t func = CFN_FUNC(cfn);

      // Func param
      switch (func) {
        case FUNC_OVERRIDE_CHANNEL:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_CH);
          new NumberEdit(specialFunctionOneWindow, grid.getFieldSlot(), 1, MAX_OUTPUT_CHANNELS, GET_SET_VALUE_WITH_OFFSET(CFN_CH_INDEX(cfn), 1));
          grid.nextLine();

          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new NumberEdit(specialFunctionOneWindow, grid.getFieldSlot(), -100, 100, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_TRAINER:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_TIMER);
          new SourceChoice(specialFunctionOneWindow, grid.getFieldSlot(), 0, 4, GET_SET_DEFAULT(CFN_TIMER_INDEX(cfn)));
          grid.nextLine();
          break;

        case FUNC_RESET:
          if (CFN_PARAM(cfn) < FUNC_RESET_PARAM_FIRST_TELEM) {
            new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_RESET);
            auto choice = new Choice(specialFunctionOneWindow, grid.getFieldSlot(), nullptr, 0,
                                     FUNC_RESET_PARAM_FIRST_TELEM + lastUsedTelemetryIndex(),
                                     GET_SET_DEFAULT(CFN_PARAM(cfn)));
            choice->setAvailableHandler(isSourceAvailableInResetSpecialFunction);
            choice->setTextHandler([=](int32_t value) {
              if (value < FUNC_RESET_PARAM_FIRST_TELEM)
                return TEXT_AT_INDEX(STR_VFSWRESET, value);
              else
                return std::string(g_model.telemetrySensors[value - FUNC_RESET_PARAM_FIRST_TELEM].label, TELEM_LABEL_LEN);
            });
            grid.nextLine();
          }
          break;

        case FUNC_VOLUME:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_SPEAKER_VOLUME);
          new SourceChoice(specialFunctionOneWindow, grid.getFieldSlot(), 0, MIXSRC_LAST_CH, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_PLAY_SOUND:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new Choice(specialFunctionOneWindow, grid.getFieldSlot(), STR_FUNCSOUNDS, 0,
                     AU_SPECIAL_SOUND_LAST - AU_SPECIAL_SOUND_FIRST - 1, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_PLAY_TRACK:
        case FUNC_BACKGND_MUSIC:
        case FUNC_PLAY_SCRIPT:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new FileChoice(specialFunctionOneWindow, grid.getFieldSlot(),
                         func == FUNC_PLAY_SCRIPT ? SCRIPTS_FUNCS_PATH : std::string(SOUNDS_PATH, SOUNDS_PATH_LNG_OFS) +
                                                                         std::string(currentLanguagePack->id, 2),
                         func == FUNC_PLAY_SCRIPT ? SCRIPTS_EXT : SOUNDS_EXT,
                         sizeof(cfn->play.name),
                         [=]() {
                           return std::string(cfn->play.name, ZLEN(cfn->play.name));
                         },
                         [=](std::string newValue) {
                           strncpy(cfn->play.name, newValue.c_str(), sizeof(cfn->play.name));
                           SET_DIRTY();
                         });
          grid.nextLine();
          break;

        case FUNC_SET_TIMER: {
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_TIMER);
          auto timerchoice = new Choice(specialFunctionOneWindow, grid.getFieldSlot(), nullptr, 0, TIMERS - 1, GET_SET_DEFAULT(CFN_TIMER_INDEX(cfn)));
          timerchoice->setTextHandler([](int32_t value) {
            return std::string(STR_TIMER) + std::to_string(value + 1);
          });
          grid.nextLine();

          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new TimeEdit(specialFunctionOneWindow, grid.getFieldSlot(), 0, 9 * 60 * 60 - 1, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;
        }

        case FUNC_SET_FAILSAFE:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_MODULE);
          new Choice(specialFunctionOneWindow, grid.getFieldSlot(), "\004Int.Ext.", 0, NUM_MODULES - 1, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_PLAY_VALUE:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new SourceChoice(specialFunctionOneWindow, grid.getFieldSlot(), 0, MIXSRC_LAST_TELEM, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_HAPTIC:
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          new NumberEdit(specialFunctionOneWindow, grid.getFieldSlot(), 0, 3, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          grid.nextLine();
          break;

        case FUNC_LOGS: {
          new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_VALUE);
          auto edit = new NumberEdit(specialFunctionOneWindow, grid.getFieldSlot(), 0, 255, GET_SET_DEFAULT(CFN_PARAM(cfn)));
          edit->setDisplayHandler([=](BitmapBuffer * dc, LcdFlags flags, int32_t value) {
            lcdDrawNumber(2, 2, CFN_PARAM(cfn), PREC1, sizeof(CFN_PARAM(cfn)), nullptr, "s");
          });
          break;
        }
      }

      if (HAS_ENABLE_PARAM(func)) {
        new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_ENABLE);
        new CheckBox(specialFunctionOneWindow, grid.getFieldSlot(), GET_SET_DEFAULT(CFN_ACTIVE(cfn)));
        grid.nextLine();
      }
      else if (HAS_REPEAT_PARAM(func)) { // !1x 1x 1s 2s 3s ...
        new StaticText(specialFunctionOneWindow, grid.getLabelSlot(), STR_REPEAT);
        auto repeat = new NumberEdit(specialFunctionOneWindow, grid.getFieldSlot(2, 1), -1, 60/CFN_PLAY_REPEAT_MUL, GET_SET_DEFAULT(CFN_PLAY_REPEAT(cfn)));
        repeat->setDisplayHandler([](BitmapBuffer * dc, LcdFlags flags, int32_t value) {
          if (value == 0)
            lcdDrawText(2, 2, "1x", flags);
          else if (value == CFN_PLAY_REPEAT_NOSTART)
            lcdDrawText(2, 2, "!1x", flags);
          else
            drawNumber(dc, 2, 2, value * CFN_PLAY_REPEAT_MUL, flags, 0, nullptr, "s");
        });
      }
    }

    void buildBody(Window * window)
    {
      // SF.one
      GridLayout grid;
      grid.spacer(8);

      CustomFunctionData * cfn = &functions[index];

      // Switch
      new StaticText(window, grid.getLabelSlot(), STR_SWITCH);
      auto swicthchoice = new SwitchChoice(window, grid.getFieldSlot(), SWSRC_FIRST, SWSRC_LAST, GET_SET_DEFAULT(CFN_SWITCH(cfn)));
      swicthchoice->setAvailableHandler([=](int value) {
        return (functions == g_model.customFn ? isSwitchAvailable(value, ModelCustomFunctionsContext) : isSwitchAvailable(value,
                                                                                                                          GeneralCustomFunctionsContext));
      });
      grid.nextLine();

      // Function
      new StaticText(window, grid.getLabelSlot(), STR_FUNC);
      auto choice = new Choice(window, grid.getFieldSlot(), STR_VFSWFUNC, 0, FUNC_MAX - 1, GET_DEFAULT(CFN_FUNC(cfn)),
                               [=](int32_t newValue) {
                                 CFN_FUNC(cfn) = newValue;
                                 CFN_RESET(cfn);
                                 SET_DIRTY();
                                 updateSpecialFunctionOneWindow();
                               });
      choice->setAvailableHandler([=](int value) {
        return isAssignableFunctionAvailable(value, functions);
      });
      grid.nextLine();

      specialFunctionOneWindow = new FormWindow(window, {0, grid.getWindowHeight(), LCD_W, 0});
      updateSpecialFunctionOneWindow();
      grid.addWindow(specialFunctionOneWindow);
    }
};

static constexpr coord_t line1 = 0;
static constexpr coord_t line2 = 22;
static constexpr coord_t col1 = 20;
static constexpr coord_t col2 = (LCD_W - 100) / 3 + col1;
static constexpr coord_t col3 = ((LCD_W - 100) / 3) * 2 + col1 + 20;

class SpecialFunctionButton : public Button {
  public:
    SpecialFunctionButton(FormWindow * parent, const rect_t &rect, CustomFunctionData * functions, uint8_t index) :
      Button(parent, rect),
      functions(functions),
      index(index)
    {
      const CustomFunctionData * cfn = &functions[index];
      uint8_t func = CFN_FUNC(cfn);
      if (!CFN_EMPTY(cfn) &&
          (HAS_ENABLE_PARAM(func) || HAS_REPEAT_PARAM(func) || (func == FUNC_PLAY_TRACK || func == FUNC_BACKGND_MUSIC || func == FUNC_PLAY_SCRIPT))) {
        setHeight(getHeight() + 20);
      }
    }

    bool isActive()
    {
      return ((functions == g_model.customFn ? modelFunctionsContext.activeSwitches : globalFunctionsContext.activeSwitches) & ((MASK_CFN_TYPE)1 << index) ? 1 : 0);
    }

    void checkEvents() override
    {
      Button::checkEvents();
      if (active != isActive()) {
        invalidate();
        active = !active;
      }
    }

    void paintSpecialFunctionLine(BitmapBuffer * dc)
    {
      // SF.all
      const CustomFunctionData * cfn = &functions[index];
      if (functions[index].func == FUNC_OVERRIDE_CHANNEL && functions != g_model.customFn) {
        functions[index].func = FUNC_OVERRIDE_CHANNEL + 1;
      }
      uint8_t func = CFN_FUNC(cfn);

      drawSwitch(col1, line1, CFN_SWITCH(cfn), 0);
      if (CFN_EMPTY(cfn))
        return;

      lcdDrawTextAtIndex(col2, line1, STR_VFSWFUNC, func, 0);
      int16_t val_min = 0;
      int16_t val_max = 255;

      switch (func) {
        case FUNC_OVERRIDE_CHANNEL:
          drawChn(dc, col1, line2, CFN_CH_INDEX(cfn) + 1, 0);
          getMixSrcRange(MIXSRC_FIRST_CH, val_min, val_max);
          lcdDrawNumber(col2, line2, CFN_PARAM(cfn));
          break;

        case FUNC_TRAINER:
          drawSource(dc, col1, line2, CFN_CH_INDEX(cfn) == 0 ? 0 : MIXSRC_Rud + CFN_CH_INDEX(cfn) - 1);
          break;

        case FUNC_RESET:
          if (CFN_PARAM(cfn) < FUNC_RESET_PARAM_FIRST_TELEM) {
            lcdDrawTextAtIndex(col1, line2, STR_VFSWRESET, CFN_PARAM(cfn));
          }
          else {
            TelemetrySensor * sensor = &g_model.telemetrySensors[CFN_PARAM(cfn) - FUNC_RESET_PARAM_FIRST_TELEM];
            lcdDrawSizedText(col1, line2, sensor->label, TELEM_LABEL_LEN, ZCHAR);
          }
          break;

        case FUNC_VOLUME:
          drawSource(dc, col1, line2, CFN_PARAM(cfn), 0);
          break;

        case FUNC_PLAY_SOUND:
          lcdDrawTextAtIndex(col1, line2, STR_FUNCSOUNDS, CFN_PARAM(cfn));
          break;

        case FUNC_PLAY_TRACK:
        case FUNC_BACKGND_MUSIC:
        case FUNC_PLAY_SCRIPT:
          if (ZEXIST(cfn->play.name))
            lcdDrawSizedText(col1, line2, cfn->play.name, sizeof(cfn->play.name), 0);
          else
            lcdDrawTextAtIndex(col1, line2, STR_VCSWFUNC, 0, 0);
          break;

        case FUNC_SET_TIMER:
          drawStringWithIndex(col1, line2, STR_TIMER, CFN_TIMER_INDEX(cfn) + 1, 0);
          break;

        case FUNC_SET_FAILSAFE:
          lcdDrawTextAtIndex(col1, line2, "\004Int.Ext.", CFN_PARAM(cfn), 0);
          break;

        case FUNC_PLAY_VALUE:
          drawSource(dc, col1, line2, CFN_PARAM(cfn), 0);
          break;

        case FUNC_HAPTIC:
          lcdDrawNumber(col1, line2, CFN_PARAM(cfn), 0);
          break;

        case FUNC_LOGS:
          lcdDrawNumber(col3, line1, CFN_PARAM(cfn), PREC1, sizeof(CFN_PARAM(cfn)), nullptr, "s");
          break;
      }
      if (HAS_ENABLE_PARAM(func)) {
        drawCheckBox(col3, line2, CFN_ACTIVE(cfn), 0);
      }
      else if (HAS_REPEAT_PARAM(func)) {
        if (CFN_PLAY_REPEAT(cfn) == 0) {
          lcdDrawText(col3, line2, "1x", 0);
        }
        else if (CFN_PLAY_REPEAT(cfn) == CFN_PLAY_REPEAT_NOSTART) {
          lcdDrawText(col3, line2, "!1x", 0);
        }
        else {
          lcdDrawNumber(col3 + 12, line2, CFN_PLAY_REPEAT(cfn) * CFN_PLAY_REPEAT_MUL, 0 | RIGHT, 0, NULL, "s");
        }
      }
    }

    virtual void paint(BitmapBuffer * dc) override
    {
      if (active)
        dc->drawSolidFilledRect(2, 2, rect.w - 4, rect.h - 4, WARNING_COLOR);
      paintSpecialFunctionLine(dc);
      drawSolidRect(dc, 0, 0, rect.w, rect.h, 2, hasFocus() ? SCROLLBOX_COLOR : CURVE_AXIS_COLOR);
    }

  protected:
    CustomFunctionData * functions;
    uint8_t index;
    bool active = false;
};

SpecialFunctionsPage::SpecialFunctionsPage(CustomFunctionData * functions) :
  PageTab(functions == g_model.customFn ? STR_MENUCUSTOMFUNC : STR_MENUSPECIALFUNCS,
          functions == g_model.customFn ? ICON_MODEL_SPECIAL_FUNCTIONS : ICON_RADIO_GLOBAL_FUNCTIONS),
  functions(functions)
{
}

void SpecialFunctionsPage::rebuild(FormWindow * window, int8_t focusSpecialFunctionIndex)
{
  coord_t scrollPosition = window->getScrollPositionY();
  window->clear();
  build(window, focusSpecialFunctionIndex);
  window->setScrollPositionY(scrollPosition);
}

void SpecialFunctionsPage::editSpecialFunction(FormWindow * window, uint8_t index)
{
  Window * editWindow = new SpecialFunctionEditWindow(functions, index);
  editWindow->setCloseHandler([=]() {
    rebuild(window, index);
  });
}

void SpecialFunctionsPage::build(FormWindow * window, int8_t focusIndex)
{
  GridLayout grid;
  grid.spacer(2);
  grid.setLabelWidth(66);

  Window::clearFocus();

  char s[5] = "SF";
  if (functions == g_eeGeneral.customFn)
    s[0] = 'G';

  FormField * first = nullptr;

  for (uint8_t i = 0; i < MAX_SPECIAL_FUNCTIONS; i++) {
    CustomFunctionData * cfn = &functions[i];

    strAppendUnsigned(&s[2], i);
    new StaticText(window, grid.getLabelSlot(), s, BUTTON_BACKGROUND | CENTERED);

    Button * button = new SpecialFunctionButton(window, grid.getFieldSlot(), functions, i);
    if (i == 0)
      first = button;

    if (focusIndex == i)
      button->setFocus();
    button->setPressHandler([=]() -> uint8_t {
      button->bringToTop();
      Menu * menu = new Menu();
      menu->addLine(STR_EDIT, [=]() {
        editSpecialFunction(window, i);
      });
      if (!CFN_EMPTY(cfn)) {
        menu->addLine(STR_COPY, [=]() {
          clipboard.type = CLIPBOARD_TYPE_CUSTOM_FUNCTION;
          clipboard.data.cfn = *cfn;
        });
      }
      if (clipboard.type == CLIPBOARD_TYPE_CUSTOM_FUNCTION) {
        menu->addLine(STR_PASTE, [=]() {
          *cfn = clipboard.data.cfn;
          SET_DIRTY();
          rebuild(window, i);
        });
      }
      if (!CFN_EMPTY(cfn) && CFN_EMPTY(&functions[MAX_SPECIAL_FUNCTIONS - 1])) {
        menu->addLine(STR_INSERT, [=]() {
          memmove(cfn + 1, cfn, (MAX_SPECIAL_FUNCTIONS - i - 1) * sizeof(CustomFunctionData));
          memset(cfn, 0, sizeof(CustomFunctionData));
          SET_DIRTY();
          rebuild(window, i);
        });
      }
      if (!CFN_EMPTY(cfn)) {
        menu->addLine(STR_CLEAR, [=]() {
          memset(cfn, 0, sizeof(CustomFunctionData));
          SET_DIRTY();
        });
      }
      for (int j = i; j < MAX_SPECIAL_FUNCTIONS; j++) {
        if (!CFN_EMPTY(&functions[j])) {
          menu->addLine(STR_DELETE, [=]() {
            memmove(cfn, cfn + 1, (MAX_SPECIAL_FUNCTIONS - i - 1) * sizeof(CustomFunctionData));
            memset(&functions[MAX_SPECIAL_FUNCTIONS - 1], 0, sizeof(CustomFunctionData));
            SET_DIRTY();
            rebuild(window, i);
          });
          break;
        }
      }
      return 0;
    });

    grid.spacer(button->height() + 5);
  }

  FormField::link(FormField::getCurrentField(), first);

  grid.nextLine();

  window->setInnerHeight(grid.getWindowHeight());
}
