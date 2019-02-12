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

const ZoneOption OPTIONS_THEME_DEFAULT[] = {
  { STR_BACKGROUND_COLOR, ZoneOption::Color, OPTION_VALUE_UNSIGNED(WHITE) },
  { STR_MAIN_COLOR, ZoneOption::Color, OPTION_VALUE_UNSIGNED(BLUE) },
  { NULL, ZoneOption::Bool }
};

class DefaultTheme: public Theme
{
  public:
    DefaultTheme():
      Theme("Default", OPTIONS_THEME_DEFAULT)
    {
      loadColors();
    }

    void loadColors() const
    {
      lcdColorTable[TEXT_COLOR_INDEX] = BLACK;
      lcdColorTable[TEXT_BGCOLOR_INDEX] = WHITE;
      lcdColorTable[TEXT_INVERTED_COLOR_INDEX] = WHITE;
      lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = RED;
      lcdColorTable[TEXT_STATUSBAR_COLOR_INDEX] = WHITE;
      lcdColorTable[LINE_COLOR_INDEX] = GREY;
      lcdColorTable[SCROLLBOX_COLOR_INDEX] = RED;
      lcdColorTable[MENU_TITLE_BGCOLOR_INDEX] = DARKGREY;
      lcdColorTable[MENU_TITLE_COLOR_INDEX] = WHITE;
      lcdColorTable[MENU_TITLE_DISABLE_COLOR_INDEX] = RGB(0, 71, 157);
      lcdColorTable[HEADER_COLOR_INDEX] = DARKGREY;
      lcdColorTable[ALARM_COLOR_INDEX] = RED;
      lcdColorTable[WARNING_COLOR_INDEX] = YELLOW;
      lcdColorTable[TEXT_DISABLE_COLOR_INDEX] = GREY;
      lcdColorTable[CURVE_AXIS_COLOR_INDEX] = LIGHTGREY;
      lcdColorTable[CURVE_COLOR_INDEX] = RED;
      lcdColorTable[CURVE_CURSOR_COLOR_INDEX] = RED;
      lcdColorTable[TRIM_BGCOLOR_INDEX] = RED;
      lcdColorTable[TRIM_SHADOW_COLOR_INDEX] = BLACK;
      lcdColorTable[MAINVIEW_PANES_COLOR_INDEX] = WHITE;
      lcdColorTable[MAINVIEW_GRAPHICS_COLOR_INDEX] = RED;
      lcdColorTable[BATTERY_CHARGE_COLOR_INDEX] = GREEN;

      // The header (top bar)
      lcdColorTable[HEADER_LOGO_BGCOLOR_INDEX] = RGB(0, 71, 157);
      lcdColorTable[HEADER_BGCOLOR_INDEX] = DARKBLUE;
      lcdColorTable[HEADER_CURRENT_BGCOLOR_INDEX] = RGB(2, 114, 250);

      // The menu title
      lcdColorTable[TITLE_BGCOLOR_INDEX] = RGB(0, 71, 157);

      lcdColorTable[OVERLAY_COLOR_INDEX] = BLACK;
      lcdColorTable[BARGRAPH1_COLOR_INDEX] = RED;
      lcdColorTable[BARGRAPH2_COLOR_INDEX] = RGB(167, 167, 167);
      lcdColorTable[BARGRAPH_BGCOLOR_INDEX] = RGB(222, 222, 222);
    }

    void loadMenuIcon(uint8_t index, const char * filename, uint32_t color=MENU_TITLE_COLOR) const
    {
      BitmapBuffer * mask = BitmapBuffer::loadMask(getThemePath(filename));
      if (mask) {
        uint8_t width = mask->getWidth();
        uint8_t height = mask->getHeight();

        // TODO delete iconMask[index];
        iconMask[index] = mask;

        // TODO delete menuIconNormal[index];
        menuIconNormal[index] = new BitmapBuffer(BMP_RGB565, TOPBAR_BUTTON_WIDTH, TOPBAR_BUTTON_WIDTH);
        if (menuIconNormal[index]) {
          menuIconNormal[index]->clear(HEADER_BGCOLOR);
          menuIconNormal[index]->drawMask((TOPBAR_BUTTON_WIDTH-width)/2, (TOPBAR_BUTTON_WIDTH-width)/2, mask, color);
        }

        // TODO delete menuIconSelected[index];
        menuIconSelected[index] = new BitmapBuffer(BMP_RGB565, TOPBAR_BUTTON_WIDTH, TOPBAR_BUTTON_WIDTH);
        if (menuIconSelected[index]) {
          if (index <= ICON_BACK)
            menuIconSelected[index]->clear(HEADER_LOGO_BGCOLOR);
          else
            menuIconSelected[index]->clear(HEADER_CURRENT_BGCOLOR);
          menuIconSelected[index]->drawMask((TOPBAR_BUTTON_WIDTH-width)/2, (TOPBAR_BUTTON_WIDTH-height)/2, mask, color);
        }
      }
    }

    void loadIcons() const
    {
      TRACE("Theme default -> loadIcons");
#if defined(LOG_TELEMETRY) || defined(WATCHDOG_DISABLED)
      loadMenuIcon(ICON_OPENTX, "mask_opentx_testmode.png", TEXT_COLOR);
#else
      loadMenuIcon(ICON_OPENTX, "mask_opentx.png");
#endif

      loadMenuIcon(ICON_NEXT, "mask_next.png");
      loadMenuIcon(ICON_BACK, "mask_back.png");
      loadMenuIcon(ICON_RADIO, "mask_menu_radio.png");
      loadMenuIcon(ICON_RADIO_SETUP, "mask_radio_setup.png");
      loadMenuIcon(ICON_RADIO_SD_BROWSER, "mask_radio_sd_browser.png");
      loadMenuIcon(ICON_RADIO_GLOBAL_FUNCTIONS, "mask_radio_global_functions.png");
      loadMenuIcon(ICON_RADIO_TRAINER, "mask_radio_trainer.png");
      loadMenuIcon(ICON_RADIO_HARDWARE, "mask_radio_hardware.png");
      loadMenuIcon(ICON_RADIO_CALIBRATION, "mask_radio_calibration.png");
      loadMenuIcon(ICON_RADIO_VERSION, "mask_radio_version.png");
      loadMenuIcon(ICON_MODEL_SELECT, "mask_modelselect.png");
      loadMenuIcon(ICON_MODEL, "mask_menu_model.png");
      loadMenuIcon(ICON_MODEL_SETUP, "mask_model_setup.png");
      loadMenuIcon(ICON_MODEL_HELI, "mask_model_heli.png");
      loadMenuIcon(ICON_MODEL_FLIGHT_MODES, "mask_model_flight_modes.png");
      loadMenuIcon(ICON_MODEL_INPUTS, "mask_model_inputs.png");
      loadMenuIcon(ICON_MODEL_MIXER, "mask_model_mixer.png");
      loadMenuIcon(ICON_MODEL_OUTPUTS, "mask_model_outputs.png");
      loadMenuIcon(ICON_MODEL_CURVES, "mask_model_curves.png");
      loadMenuIcon(ICON_MODEL_GVARS, "mask_model_gvars.png");
      loadMenuIcon(ICON_MODEL_LOGICAL_SWITCHES, "mask_model_logical_switches.png");
      loadMenuIcon(ICON_MODEL_SPECIAL_FUNCTIONS, "mask_model_special_functions.png");
      loadMenuIcon(ICON_MODEL_LUA_SCRIPTS, "mask_model_lua_scripts.png");
      loadMenuIcon(ICON_MODEL_TELEMETRY, "mask_model_telemetry.png");
      loadMenuIcon(ICON_STATS, "mask_menu_stats.png");
      loadMenuIcon(ICON_STATS_THROTTLE_GRAPH, "mask_stats_throttle_graph.png");
      loadMenuIcon(ICON_STATS_TIMERS, "mask_stats_timers.png");
      loadMenuIcon(ICON_STATS_ANALOGS, "mask_stats_analogs.png");
      loadMenuIcon(ICON_STATS_DEBUG, "mask_stats_debug.png");
      loadMenuIcon(ICON_THEME, "mask_menu_theme.png");
      loadMenuIcon(ICON_THEME_SETUP, "mask_theme_setup.png");
      loadMenuIcon(ICON_THEME_VIEW1, "mask_theme_view1.png");
      loadMenuIcon(ICON_THEME_VIEW2, "mask_theme_view2.png");
      loadMenuIcon(ICON_THEME_VIEW3, "mask_theme_view3.png");
      loadMenuIcon(ICON_THEME_VIEW4, "mask_theme_view4.png");
      loadMenuIcon(ICON_THEME_VIEW5, "mask_theme_view5.png");
      loadMenuIcon(ICON_THEME_ADD_VIEW, "mask_theme_add_view.png");
      loadMenuIcon(ICON_MONITOR, "mask_monitor.png");
      loadMenuIcon(ICON_MONITOR_CHANNELS1, "mask_monitor_channels1.png");
      loadMenuIcon(ICON_MONITOR_CHANNELS2, "mask_monitor_channels2.png");
      loadMenuIcon(ICON_MONITOR_CHANNELS3, "mask_monitor_channels3.png");
      loadMenuIcon(ICON_MONITOR_CHANNELS4, "mask_monitor_channels4.png");
      loadMenuIcon(ICON_MONITOR_LOGICAL_SWITCHES, "mask_monitor_logsw.png");

      BitmapBuffer * background = BitmapBuffer::loadMask(getThemePath("mask_currentmenu_bg.png"));
      BitmapBuffer * shadow = BitmapBuffer::loadMask(getThemePath("mask_currentmenu_shadow.png"));
      BitmapBuffer * dot = BitmapBuffer::loadMask(getThemePath("mask_currentmenu_dot.png"));

      if (!currentMenuBackground) {
        currentMenuBackground = new BitmapBuffer(BMP_RGB565, 36, 53);
      }
      if (currentMenuBackground) {
        currentMenuBackground->drawSolidFilledRect(0, 0, currentMenuBackground->getWidth(), MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
        currentMenuBackground->drawSolidFilledRect(0, MENU_HEADER_HEIGHT, currentMenuBackground->getWidth(), MENU_TITLE_TOP - MENU_HEADER_HEIGHT, TEXT_BGCOLOR);
        currentMenuBackground->drawSolidFilledRect(0, MENU_TITLE_TOP, currentMenuBackground->getWidth(), currentMenuBackground->getHeight() - MENU_TITLE_TOP, TITLE_BGCOLOR);
        currentMenuBackground->drawMask(0, 0, background, HEADER_CURRENT_BGCOLOR);
        currentMenuBackground->drawMask(0, 0, shadow, TRIM_SHADOW_COLOR);
        currentMenuBackground->drawMask(10, 39, dot, MENU_TITLE_COLOR);
      }

      // TODO delete topleftBitmap;
      topleftBitmap = BitmapBuffer::loadMaskOnBackground("topleft.png", TITLE_BGCOLOR, HEADER_BGCOLOR);

      // TODO delete background;
      // TODO delete shadow;
      // TODO delete dot;
    }

    void loadThemeBitmaps() const
    {
      TRACE("Theme default -> loadThemeBitmaps");
      // Calibration screen
      // TODO delete calibStick;
      calibStick = BitmapBuffer::load(getThemePath("stick_pointer.png"));

      // TODO delete calibStickBackground;
      calibStickBackground = BitmapBuffer::load(getThemePath("stick_background.png"));

      // TODO delete calibTrackpBackground;
      calibTrackpBackground = BitmapBuffer::load(getThemePath("trackp_background.png"));

      // TODO delete calibHorus;
#if defined(PCBX10)
      if(ANALOGS_PWM_ENABLED()) {
        calibHorus = BitmapBuffer::load(getThemePath("X10S.bmp"));
      }
      else {
        calibHorus = BitmapBuffer::load(getThemePath("X10.bmp"));
      }
#else
      calibHorus = BitmapBuffer::load(getThemePath("horus.bmp"));
#endif

      // Model Selection screen
      // TODO delete modelselIconBitmap;
      modelselIconBitmap = BitmapBuffer::loadMaskOnBackground("modelsel/mask_iconback.png", TITLE_BGCOLOR, TEXT_BGCOLOR);
      if (modelselIconBitmap) {
        BitmapBuffer * bitmap = BitmapBuffer::load(getThemePath("modelsel/icon_default.png"));
        modelselIconBitmap->drawBitmap(20, 8, bitmap);
        // TODO delete bitmap;
      }

      // TODO delete modelselSdFreeBitmap;
      modelselSdFreeBitmap = BitmapBuffer::loadMaskOnBackground("modelsel/mask_sdfree.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete modelselModelQtyBitmap;
      modelselModelQtyBitmap = BitmapBuffer::loadMaskOnBackground("modelsel/mask_modelqty.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete modelselModelNameBitmap;
      modelselModelNameBitmap = BitmapBuffer::loadMaskOnBackground("modelsel/mask_modelname.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete modelselModelMoveBackground;
      modelselModelMoveBackground = BitmapBuffer::loadMask(getThemePath("modelsel/mask_moveback.png"));

      // TODO delete modelselModelMoveIcon;
      modelselModelMoveIcon = BitmapBuffer::loadMask(getThemePath("modelsel/mask_moveico.png"));

      // TODO delete modelselWizardBackground;
      modelselWizardBackground = BitmapBuffer::load(getThemePath("wizard/background.png"));

      // Channels monitor screen
      // TODO delete chanMonLockedBitmap;
      chanMonLockedBitmap = BitmapBuffer::loadMaskOnBackground("mask_monitor_lockch.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete chanMonInvertedBitmap;
      chanMonInvertedBitmap = BitmapBuffer::loadMaskOnBackground("mask_monitor_inver.png", TEXT_COLOR, TEXT_BGCOLOR);

      // Mixer setup screen
      // TODO delete mixerSetupMixerBitmap;
      mixerSetupMixerBitmap = BitmapBuffer::loadMaskOnBackground("mask_sbar_mixer.png", MENU_TITLE_COLOR, HEADER_BGCOLOR);

      // TODO delete mixerSetupToBitmap;
      mixerSetupToBitmap = BitmapBuffer::loadMaskOnBackground("mask_sbar_to.png", MENU_TITLE_COLOR, HEADER_BGCOLOR);

      // TODO delete mixerSetupOutputBitmap;
      mixerSetupOutputBitmap = BitmapBuffer::loadMaskOnBackground("mask_sbar_output.png", MENU_TITLE_COLOR, HEADER_BGCOLOR);

      // TODO delete mixerSetupAddBitmap;
      mixerSetupAddBitmap = BitmapBuffer::loadMaskOnBackground("mask_mplex_add.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete mixerSetupMultiBitmap;
      mixerSetupMultiBitmap = BitmapBuffer::loadMaskOnBackground("mask_mplex_multi.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete mixerSetupReplaceBitmap;
      mixerSetupReplaceBitmap = BitmapBuffer::loadMaskOnBackground("mask_mplex_replace.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete mixerSetupLabelBitmap;
      mixerSetupLabelBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_label.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete mixerSetupCurveBitmap;
      mixerSetupCurveBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_curve.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete mixerSetupSwitchBitmap;
      mixerSetupSwitchBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_switch.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete mixerSetupFlightmodeBitmap;
      mixerSetupFlightmodeBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_fm.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete mixerSetupSlowBitmap;
      mixerSetupSlowBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_slow.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete mixerSetupDelayBitmap;
      mixerSetupDelayBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_delay.png", TEXT_COLOR, TEXT_BGCOLOR);

      // TODO delete mixerSetupDelaySlowBitmap;
      mixerSetupDelaySlowBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_delayslow.png", TEXT_COLOR, TEXT_BGCOLOR);
    }

    void load() const override
    {
      TRACE("Theme default -> load");
      loadColors();
      Theme::load();
      if (!backgroundBitmap) {
        TRACE("Theme default -> load backgroundBitmap");
        backgroundBitmap = BitmapBuffer::load(getThemePath("background.png"));
      }
      update();
    }

    void update() const override
    {
#if defined(PCBHORUS)
      uint32_t color = g_eeGeneral.themeData.options[1].unsignedValue;
      uint32_t bg_color = UNEXPECTED_SHUTDOWN() ? WHITE : g_eeGeneral.themeData.options[0].unsignedValue;

      lcdColorTable[TEXT_BGCOLOR_INDEX] = bg_color;
      lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = color;
      lcdColorTable[SCROLLBOX_COLOR_INDEX] = color;
      lcdColorTable[CURVE_COLOR_INDEX] = color;
      lcdColorTable[CURVE_CURSOR_COLOR_INDEX] = color;
      lcdColorTable[TITLE_BGCOLOR_INDEX] = color;
      lcdColorTable[MENU_TITLE_DISABLE_COLOR_INDEX] =
          RGB(GET_RED(color)>>1, GET_GREEN(color)>>1, GET_BLUE(color)>>1);
      lcdColorTable[TRIM_BGCOLOR_INDEX] = color;
      lcdColorTable[MAINVIEW_GRAPHICS_COLOR_INDEX] = color;
      #define DARKER(x)     ((x * 70) / 100)
      lcdColorTable[HEADER_BGCOLOR_INDEX] = RGB(DARKER(GET_RED(color)), DARKER(GET_GREEN(color)), DARKER(GET_BLUE(color)));
      lcdColorTable[HEADER_LOGO_BGCOLOR_INDEX] = color;
      lcdColorTable[HEADER_CURRENT_BGCOLOR_INDEX] = color;
#endif
      loadIcons();
      loadThemeBitmaps();
    }

    void drawBackground() const override
    {
      if (!backgroundBitmap || backgroundBitmap->getFormat() == BMP_ARGB4444 || backgroundBitmap->getWidth() != LCD_W || backgroundBitmap->getHeight() != LCD_H) {
        lcdSetColor(g_eeGeneral.themeData.options[0].unsignedValue);
        lcdDrawSolidFilledRect(0, 0, LCD_W, LCD_H, CUSTOM_COLOR);
      }
      if (backgroundBitmap) {
        lcd->drawBitmap(0, 0, backgroundBitmap);
      }
    }

    bool drawTopbarBackground(uint32_t options) const override
    {
      bool result = false;
      if (options & OPTION_TOPBAR_LOGO) {
        lcd->drawBitmap(0, 0, menuIconSelected[ICON_OPENTX]);
        lcd->drawSolidFilledRect(TOPBAR_BUTTON_WIDTH, 0, LCD_W-TOPBAR_BUTTON_WIDTH, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
      }
      else {
        lcd->drawSolidFilledRect(0, 0, LCD_W, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
      }

      if (options & OPTION_TOPBAR_DATETIME) {
        drawTopbarDatetime();
      }

      return result;
    }

    void drawIcon(coord_t x, coord_t y, uint8_t index, LcdFlags flags) const override
    {
      lcd->drawMask(x, y, iconMask[index], flags);
    }

    const BitmapBuffer * getIconBitmap(uint8_t index, bool selected) const override
    {
      return selected ? menuIconSelected[index] : menuIconNormal[index];
    }

    virtual const BitmapBuffer * getIconMask(uint8_t index) const override
    {
      return iconMask[index];
    }

  protected:
    static const BitmapBuffer * backgroundBitmap;
    static BitmapBuffer * keyboardBitmap;
    static BitmapBuffer * topleftBitmap;
    static BitmapBuffer * menuIconNormal[MENUS_ICONS_COUNT];
    static BitmapBuffer * menuIconSelected[MENUS_ICONS_COUNT];
    static BitmapBuffer * iconMask[MENUS_ICONS_COUNT];
    static BitmapBuffer * currentMenuBackground;
};

const BitmapBuffer * DefaultTheme::backgroundBitmap = NULL;
BitmapBuffer * keyboardBitmap = NULL;
BitmapBuffer * DefaultTheme::topleftBitmap = NULL;
BitmapBuffer * DefaultTheme::iconMask[MENUS_ICONS_COUNT] = { NULL };
BitmapBuffer * DefaultTheme::menuIconNormal[MENUS_ICONS_COUNT] = { NULL };
BitmapBuffer * DefaultTheme::menuIconSelected[MENUS_ICONS_COUNT] = { NULL };
BitmapBuffer * DefaultTheme::currentMenuBackground = NULL;

DefaultTheme defaultTheme;
Theme * theme = &defaultTheme;
