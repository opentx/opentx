/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "../../opentx.h"

/*
 *  Header bitmaps
 */

const uint8_t LBM_TOP_POLYGON[] = {
#include "../../bitmaps/Horus/mask_top_polygon.lbm"
};

const uint8_t LBM_DOT[] = {
#include "../../bitmaps/Horus/mask_dot.lbm"
};

const uint8_t LBM_CURRENT_BG[] = {
#include "../../bitmaps/Horus/mask_current_bg.lbm"
};

const uint8_t LBM_CURRENT_SHADOW[] = {
#include "../../bitmaps/Horus/mask_current_shadow.lbm"
};

const uint8_t LBM_CURRENT_DOT[] = {
#include "../../bitmaps/Horus/mask_current_dot.lbm"
};

/*
 *  Model menu bitmaps
 */

const uint8_t LBM_MENU_MODEL[] = {
#include "../../bitmaps/Horus/mask_menu_model.lbm"
};

const uint8_t LBM_MODEL_SETUP_ICON[] = {
#include "../../bitmaps/Horus/mask_model_setup.lbm"
};

const uint8_t LBM_HELI_ICON[] = {
#include "../../bitmaps/Horus/mask_heli.lbm"
};

const uint8_t LBM_FLIGHT_MODES_ICON[] = {
#include "../../bitmaps/Horus/mask_flight_modes.lbm"
};

const uint8_t LBM_INPUTS_ICON[] = {
#include "../../bitmaps/Horus/mask_inputs.lbm"
};

const uint8_t LBM_MIXER_ICON[] = {
#include "../../bitmaps/Horus/mask_mixer.lbm"
};

const uint8_t LBM_OUTPUTS_ICON[] = {
#include "../../bitmaps/Horus/mask_outputs.lbm"
};

const uint8_t LBM_CURVES_ICON[] = {
#include "../../bitmaps/Horus/mask_curves.lbm"
};

const uint8_t LBM_GVARS_ICON[] = {
#include "../../bitmaps/Horus/mask_gvars.lbm"
};

const uint8_t LBM_LOGICAL_SWITCHES_ICON[] = {
#include "../../bitmaps/Horus/mask_logical_switches.lbm"
};

const uint8_t LBM_SPECIAL_FUNCTIONS_ICON[] = {
#include "../../bitmaps/Horus/mask_special_functions.lbm"
};

const uint8_t LBM_LUA_SCRIPTS_ICON[] = {
#include "../../bitmaps/Horus/mask_lua_scripts.lbm"
};

const uint8_t LBM_TELEMETRY_ICON[] = {
#include "../../bitmaps/Horus/mask_telemetry.lbm"
};

const uint8_t * const LBM_MODEL_ICONS[] = {
  LBM_MENU_MODEL,
  LBM_MODEL_SETUP_ICON,
  CASE_HELI(LBM_HELI_ICON)
  CASE_FLIGHT_MODES(LBM_FLIGHT_MODES_ICON)
  LBM_INPUTS_ICON,
  LBM_MIXER_ICON,
  LBM_OUTPUTS_ICON,
  CASE_CURVES(LBM_CURVES_ICON)
  CASE_GVARS(LBM_GVARS_ICON)
  LBM_LOGICAL_SWITCHES_ICON,
  LBM_SPECIAL_FUNCTIONS_ICON,
#if defined(LUA_MODEL_SCRIPTS)
  LBM_LUA_SCRIPTS_ICON,
#endif
  LBM_TELEMETRY_ICON
};

