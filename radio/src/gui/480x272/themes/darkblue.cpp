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

class DarkblueTheme: public Theme
{
  public:
    DarkblueTheme():
      Theme("Darkblue")
    {
    }

    void loadColors() const
    {
      lcdColorTable[TEXT_COLOR_INDEX] = WHITE;
      lcdColorTable[TEXT_BGCOLOR_INDEX] = RGB(10, 78, 121);
      lcdColorTable[TEXT_INVERTED_COLOR_INDEX] = WHITE;
      lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[TEXT_STATUSBAR_COLOR_INDEX] = WHITE;
      lcdColorTable[LINE_COLOR_INDEX] = GREY;
      lcdColorTable[SCROLLBOX_COLOR_INDEX] = WHITE;
      lcdColorTable[MENU_TITLE_BGCOLOR_INDEX] = DARKGREY;
      lcdColorTable[MENU_TITLE_COLOR_INDEX] = WHITE;
      lcdColorTable[MENU_TITLE_DISABLE_COLOR_INDEX] = BLACK;
      lcdColorTable[HEADER_COLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[ALARM_COLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[WARNING_COLOR_INDEX] = YELLOW;
      lcdColorTable[TEXT_DISABLE_COLOR_INDEX] = GREY;
      lcdColorTable[CURVE_AXIS_COLOR_INDEX] = LIGHTGREY;
      lcdColorTable[CURVE_COLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[CURVE_CURSOR_COLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[TITLE_BGCOLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[TRIM_BGCOLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[TRIM_SHADOW_COLOR_INDEX] = RGB(100, 100, 100);
      lcdColorTable[MAINVIEW_PANES_COLOR_INDEX] = GREY;
      lcdColorTable[MAINVIEW_GRAPHICS_COLOR_INDEX] = WHITE;
      lcdColorTable[HEADER_BGCOLOR_INDEX] = BLACK;
      lcdColorTable[HEADER_ICON_BGCOLOR_INDEX] = BLACK;
      lcdColorTable[HEADER_CURRENT_BGCOLOR_INDEX] = RGB(10, 78, 121);
      lcdColorTable[OVERLAY_COLOR_INDEX] = BLACK;
      lcdColorTable[BARGRAPH1_COLOR_INDEX] = RGB(15, 150, 250);
      lcdColorTable[BARGRAPH2_COLOR_INDEX] = RGB(167, 167, 167);
      lcdColorTable[BARGRAPH_BGCOLOR_INDEX] = RGB(97, 97, 102);
    }

    void loadMenuIcon(uint8_t index, const char * filename) const
    {
      BitmapBuffer * mask = BitmapBuffer::loadMask(getThemePath(filename));
      if (mask) {
        menuIconNormal[index] = new BitmapBuffer(BMP_RGB565, mask->getWidth(), mask->getHeight());
        if (menuIconNormal[index]) {
          menuIconNormal[index]->clear(HEADER_BGCOLOR);
          menuIconNormal[index]->drawMask(0, 0, mask, HEADER_CURRENT_BGCOLOR);
        }
        menuIconSelected[index] = new BitmapBuffer(BMP_RGB565, mask->getWidth(), mask->getHeight());
        if (menuIconSelected[index]) {
          menuIconSelected[index]->clear(HEADER_BGCOLOR);
          menuIconSelected[index]->drawMask(0, 0, mask, MENU_TITLE_COLOR);
        }
      }
    }

    void loadMenusIcons() const
    {
      loadMenuIcon(ICON_OPENTX, "mask_opentx.png");
      loadMenuIcon(ICON_RADIO, "mask_menu_radio.png");
      loadMenuIcon(ICON_RADIO_SETUP, "mask_radio_setup.png");
      loadMenuIcon(ICON_RADIO_SD_BROWSER, "mask_radio_sd_browser.png");
      loadMenuIcon(ICON_RADIO_GLOBAL_FUNCTIONS, "mask_radio_global_functions.png");
      loadMenuIcon(ICON_RADIO_TRAINER, "mask_radio_trainer.png");
      loadMenuIcon(ICON_RADIO_HARDWARE, "mask_radio_hardware.png");
      loadMenuIcon(ICON_RADIO_VERSION, "mask_radio_version.png");
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
      loadMenuIcon(ICON_MONITOR_LOGICAL_SWITCHES, "/mask_monitor_logsw.png");
    }

    void loadThemeBitmaps() const
    {
      // Calibration screen
      delete calibStick;
      calibStick = BitmapBuffer::load(getThemePath("stick_pointer.png"));

      delete calibStickBackground;
      calibStickBackground = BitmapBuffer::load(getThemePath("stick_background.png"));

      delete calibTrackpBackground;
      calibTrackpBackground = BitmapBuffer::load(getThemePath("trackp_background.png"));

      delete calibHorus;
#if defined(PCBX10)
      if(STICKS_PWM_ENABLED()) {
        calibHorus = BitmapBuffer::load(getThemePath("X10S.bmp"));
      }
      else {
        calibHorus = BitmapBuffer::load(getThemePath("X10.bmp"));
      }
#else
      calibHorus = BitmapBuffer::load(getThemePath("horus.bmp"));
#endif

      // Channels monitor screen
      delete chanMonLockedBitmap;
      chanMonLockedBitmap = BitmapBuffer::loadMaskOnBackground("mask_monitor_lockch.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete chanMonInvertedBitmap;
      chanMonInvertedBitmap = BitmapBuffer::loadMaskOnBackground("mask_monitor_inver.png", TEXT_COLOR, TEXT_BGCOLOR);

     // Model Selection screen
      delete modelselIconBitmap;
      modelselIconBitmap = BitmapBuffer::loadMaskOnBackground("modelsel/mask_iconback.png", TITLE_BGCOLOR, TEXT_BGCOLOR);
      if (modelselIconBitmap) {
        BitmapBuffer * bitmap = BitmapBuffer::load(getThemePath("modelsel/icon_default.png"));
        modelselIconBitmap->drawBitmap(20, 8, bitmap);
        delete bitmap;
      }

      delete modelselSdFreeBitmap;
      modelselSdFreeBitmap = BitmapBuffer::loadMaskOnBackground("modelsel/mask_sdfree.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete modelselModelQtyBitmap;
      modelselModelQtyBitmap = BitmapBuffer::loadMaskOnBackground("modelsel/mask_modelqty.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete modelselModelNameBitmap;
      modelselModelNameBitmap = BitmapBuffer::loadMaskOnBackground("modelsel/mask_modelname.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete modelselModelMoveBackground;
      modelselModelMoveBackground = BitmapBuffer::loadMask(getThemePath("modelsel/mask_moveback.png"));

      delete modelselModelMoveIcon;
      modelselModelMoveIcon = BitmapBuffer::loadMask(getThemePath("modelsel/mask_moveico.png"));

      delete modelselWizardBackground;
      modelselWizardBackground = BitmapBuffer::load(getThemePath("wizard/background.png"));


      // Mixer setup screen
      delete mixerSetupMixerBitmap;
      mixerSetupMixerBitmap = BitmapBuffer::loadMaskOnBackground("mask_sbar_mixer.png", MENU_TITLE_COLOR, HEADER_BGCOLOR);

      delete mixerSetupToBitmap;
      mixerSetupToBitmap = BitmapBuffer::loadMaskOnBackground("mask_sbar_to.png", TEXT_BGCOLOR, HEADER_BGCOLOR);

      delete mixerSetupOutputBitmap;
      mixerSetupOutputBitmap = BitmapBuffer::loadMaskOnBackground("mask_sbar_output.png", MENU_TITLE_COLOR, HEADER_BGCOLOR);

      delete mixerSetupAddBitmap;
      mixerSetupAddBitmap = BitmapBuffer::loadMaskOnBackground("mask_mplex_add.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupMultiBitmap;
      mixerSetupMultiBitmap = BitmapBuffer::loadMaskOnBackground("mask_mplex_multi.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupReplaceBitmap;
      mixerSetupReplaceBitmap = BitmapBuffer::loadMaskOnBackground("mask_mplex_replace.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupLabelBitmap;
      mixerSetupLabelBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_label.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupCurveBitmap;
      mixerSetupCurveBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_curve.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupSwitchBitmap;
      mixerSetupSwitchBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_switch.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupSlowBitmap;
      mixerSetupSlowBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_slow.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupDelayBitmap;
      mixerSetupDelayBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_delay.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupDelaySlowBitmap;
      mixerSetupDelaySlowBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_delayslow.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupFlightmodeBitmap;
      mixerSetupFlightmodeBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_fm.png", TEXT_COLOR, TEXT_BGCOLOR);
    }

    virtual void load() const
    {
      loadColors();
      loadMenusIcons();
      loadThemeBitmaps();
      loadFontCache();
    }

    void drawTopbarBackground(uint8_t icon) const
    {
      lcdDrawSolidFilledRect(0, 0, LCD_W, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
      lcdDrawSolidFilledRect(0, 0, 41, MENU_HEADER_HEIGHT, HEADER_ICON_BGCOLOR);

      if (icon == ICON_OPENTX) {
        static BitmapBuffer * thumb = BitmapBuffer::load(getFilePath("topmenu_opentx.bmp"));
        lcd->drawBitmap(5, 7, thumb);
      }
      else {
        lcd->drawBitmap(5, 7, menuIconSelected[icon]);
      }

      drawTopbarDatetime();
    }

    virtual void drawMenuIcon(uint8_t index, uint8_t position, bool selected) const
    {
      lcd->drawBitmap(50+position*MENU_ICONS_SPACING, 7, selected ? menuIconSelected[index] : menuIconNormal[index]);
    }

  protected:
    static BitmapBuffer * menuIconNormal[MENUS_ICONS_COUNT];
    static BitmapBuffer * menuIconSelected[MENUS_ICONS_COUNT];
};

BitmapBuffer * DarkblueTheme::menuIconNormal[MENUS_ICONS_COUNT] = { NULL };
BitmapBuffer * DarkblueTheme::menuIconSelected[MENUS_ICONS_COUNT] = { NULL };

const DarkblueTheme darkblueTheme;
