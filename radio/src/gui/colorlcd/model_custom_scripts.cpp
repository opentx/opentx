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
#include "model_custom_scripts.h"
#include "opentx.h"
#include "libopenui.h"

#define SET_DIRTY()     storageDirty(EE_MODEL)

class CostomScriptsEditWindow: public Page {
  public:
    CostomScriptsEditWindow(uint8_t cs):
      Page(ICON_MODEL_LUA_SCRIPTS),
      cs(cs)
    {
      buildBody(&body);
      buildHeader(&header);
    }

    void checkEvents() override
    {
      invalidate();

      if (refreshDelay) {
        if (--refreshDelay == 0) {
              body.clear();
              buildBody(&body);
        }
      }
    }

  protected:
    uint8_t cs;
    bool active = false;
    uint32_t  refreshDelay = 0;
    StaticText * headerLuaName = nullptr;


    void buildHeader(PageHeader * window) {
      char str[20];
      new StaticText(window, { 70, 4, LCD_W - 100, 20 }, STR_MENUCUSTOMSCRIPTS, MENU_COLOR_INDEX);
      strAppendUnsigned(strAppend(str, "LUA"), cs + 1);
      headerLuaName = new StaticText(window, { 70, 28, LCD_W - 100, 20 }, str, MENU_COLOR_INDEX);
    }

    void buildBody(FormWindow * window) {

      ScriptData *sd = &(g_model.scriptsData[cs]);

      FormGridLayout grid;
      grid.spacer(10);


      new StaticText(window, grid.getLabelSlot(), STR_SCRIPT);
      new FileChoice(window, grid.getFieldSlot(),
                     SCRIPTS_MIXES_PATH,
                     SCRIPTS_EXT,
                     sizeof(sd->file),
                     [=]() {
                       return std::string(sd->file, ZLEN(sd->file));
                     },
                     [=](std::string newValue) {
                       memset(sd->file, 0, sizeof(sd->file));
                       strncpy(sd->file, newValue.c_str(), sizeof(sd->file));
                       SET_DIRTY();
                       if (ZEXIST(sd->file)) {
                          memset(sd->inputs, 0, sizeof(sd->inputs));
                          storageDirty(EE_MODEL);
                          LUA_LOAD_MODEL_SCRIPT(cs);
                          refreshDelay = 4;
                       }
                     });
      grid.nextLine();

      new StaticText(window, grid.getLabelSlot(), TR_NAME);
      new TextEdit(window, grid.getFieldSlot(), sd->name, sizeof(sd->name));
      grid.nextLine();

      new StaticText(window, grid.getLabelSlot(), STR_INPUTS);
      grid.nextLine();

      for (int i = 0; i < scriptInputsOutputs[cs].inputsCount; i++) {
          new StaticText(window, grid.getLabelSlot(), scriptInputsOutputs[cs].inputs[i].name);
          if (scriptInputsOutputs[cs].inputs[i].type == INPUT_TYPE_VALUE) {
              new NumberEdit(window, grid.getFieldSlot(), scriptInputsOutputs[cs].inputs[i].min-scriptInputsOutputs[cs].inputs[i].def,
                                    scriptInputsOutputs[cs].inputs[i].max-scriptInputsOutputs[cs].inputs[i].def,
                                   [=]() -> int32_t {
                                     return g_model.scriptsData[cs].inputs[i].value + scriptInputsOutputs[cs].inputs[i].def;
                                   },
                                   [=](int32_t newValue) {
                                     g_model.scriptsData[cs].inputs[i].value = newValue;
                                   });
          }
          else {
            new SourceChoice(window, grid.getFieldSlot(), 0, MIXSRC_LAST_TELEM, GET_SET_DEFAULT(g_model.scriptsData[cs].inputs[i].source));
          }
          grid.nextLine();
      }

      new StaticText(window, grid.getLineSlot(), "---------------------------------------------------");
      grid.nextLine();

      if (scriptInputsOutputs[cs].outputsCount > 0) {
          for (int i=0; i<scriptInputsOutputs[cs].outputsCount; i++) {
             char s[16];
             getSourceString(s, MIXSRC_FIRST_LUA + (cs * MAX_SCRIPT_OUTPUTS) + i);
             new StaticText(window, grid.getLabelSlot(), s);
             new NumberEdit(window, grid.getFieldSlot(), -127, 127, GET_VALUE(calcRESXto1000(scriptInputsOutputs[cs].outputs[i].value)),NULL, PREC1);
          }
      }

    }
};

static constexpr coord_t line1 = 2;
static constexpr coord_t line2 = 22;
static constexpr coord_t line3 = 42;
static constexpr coord_t col1 = 20;
static constexpr coord_t col2 = (LCD_W - 100) / 3 + col1;
static constexpr coord_t col3 = ((LCD_W - 100) / 3) * 2 + col1;

class CustomScriptsButton : public Button {
  public:
    CustomScriptsButton(FormWindow * parent, const rect_t & rect, int csIndex, std::function<uint8_t(void)> onPress):
      Button(parent, rect, onPress),
      csIndex(csIndex)
    {
    }

    bool isActive()
    {
      return getSwitch(SWSRC_FIRST_LOGICAL_SWITCH + csIndex);
    }

    void checkEvents() override
    {
        invalidate();
    }

    virtual void paint(BitmapBuffer * dc) override
    {

      LogicalSwitchData * cs = lswAddress(csIndex);
      uint8_t lsFamily = lswFamily(cs->func);
      ScriptData &sd = g_model.scriptsData[csIndex];

      if (strlen(sd.file)) {
         dc->drawSizedText(col1, line1, sd.file, sizeof(sd.file), 0);
         dc->drawSizedText(col2, line1, sd.name, sizeof(sd.name), 0);

        switch (scriptInternalData[csIndex].state) {
          case SCRIPT_SYNTAX_ERROR:
            dc->drawText(col3, line1, "(error)");
            break;
          case SCRIPT_KILLED:
            dc->drawText(col3, line1, "(killed)");
            break;
          default:
            dc->drawNumber(col3, line1, luaGetCpuUsed(csIndex), LEFT|TEXT_STATUSBAR_COLOR, 0, NULL, "%");
            break;
        }
      }
      else {
          dc->drawTextAtIndex(col1, line1, STR_VCSWFUNC, 0, 0);
      }

    }

  protected:
    uint8_t csIndex;
};

ModelCustomScriptsPage::ModelCustomScriptsPage():
  PageTab(STR_MENUCUSTOMSCRIPTS, ICON_MODEL_LUA_SCRIPTS)
{
}

void ModelCustomScriptsPage::rebuild(FormWindow * window, int8_t focusIndex)
{
  coord_t scrollPosition = window->getScrollPositionY();
  window->clear();
  build(window, focusIndex);
  window->setScrollPositionY(scrollPosition);
}

void ModelCustomScriptsPage::editCustomScript(FormWindow * window, uint8_t lsIndex)
{
  CostomScriptsEditWindow * lsWindow = new CostomScriptsEditWindow(lsIndex);
  lsWindow->setCloseHandler([=]() {
    rebuild(window, lsIndex);
  });
}

#define SCRIPTS_COLUMN_FILE  70
#define SCRIPTS_COLUMN_NAME  160
#define SCRIPTS_COLUMN_STATE 300

void ModelCustomScriptsPage::build(FormWindow * window, int8_t focusIndex)
{

    FormGridLayout grid;
    grid.spacer(8);
    grid.setLabelWidth(70);

    for (uint8_t i=0; i<MAX_SCRIPTS; i++) {
      char str[20];
      strAppendUnsigned(strAppend(str, "LUA"), i+1);
      new TextButton(window, grid.getLabelSlot(), str);

      Button * button = new CustomScriptsButton(window, grid.getFieldSlot(), i,
                                                [=]() -> uint8_t {
                                                  Menu * menu = new Menu(window );
                                                  ScriptData *sd = &g_model.scriptsData[i];
                                                  menu->addLine(STR_EDIT, [=]() {
                                                    editCustomScript(window, i);
                                                  });

                                                  if (strlen(sd->file))
                                                    menu->addLine(STR_COPY, [=]() {
                                                      clipboard.type = CLIPBOARD_TYPE_CUSTOM_SCRIPT;
                                                      clipboard.data.csd = *sd;
                                                    });
                                                  if (clipboard.type == CLIPBOARD_TYPE_CUSTOM_SCRIPT)
                                                    menu->addLine(STR_PASTE, [=]() {
                                                      *sd = clipboard.data.csd;
                                                      storageDirty(EE_MODEL);
                                                      LUA_LOAD_MODEL_SCRIPT(i);
                                                      rebuild(window, i);
                                                    });
                                                  menu->addLine(STR_CLEAR, [=]() {
                                                      memset(sd, 0, sizeof(ScriptData));
                                                      storageDirty(EE_MODEL);
                                                      LUA_LOAD_MODEL_SCRIPTS();
                                                      rebuild(window, i);
                                                  });
                                                  return 0;
                                                });
      if (focusIndex == i) {
        button->setFocus();
      }

      grid.spacer(button->height() + 15);
    }

    grid.nextLine();

    window->setInnerHeight(grid.getWindowHeight());

}
