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

enum menuModelHeliItems {
  ITEM_HELI_SWASHTYPE,
  ITEM_HELI_SWASHRING,
  ITEM_HELI_ELE,
  ITEM_HELI_ELE_WEIGHT,
  ITEM_HELI_AIL,
  ITEM_HELI_AIL_WEIGHT,
  ITEM_HELI_COL,
  ITEM_HELI_COL_WEIGHT,
  ITEM_HELI_MAX
};

#define MODEL_HELI_2ND_COLUMN (230)

void menuModelHeli(evt_t event)
{
  SIMPLE_MENU(STR_MENUHELISETUP, menuTabModel, e_Heli, ITEM_HELI_MAX, DEFAULT_SCROLLBAR_X);

  int sub = m_posVert;

  for (unsigned int i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    int k = i+s_pgOfs;
    LcdFlags blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_HELI_SWASHTYPE:
        g_model.swashR.type = selectMenuItem(MODEL_HELI_2ND_COLUMN, y, STR_SWASHTYPE, STR_VSWASHTYPE, g_model.swashR.type, 0, SWASH_TYPE_MAX, attr, event);
        break;

      case ITEM_HELI_SWASHRING:
        lcd_putsLeft(y, STR_SWASHRING);
        lcd_outdezAtt(MODEL_HELI_2ND_COLUMN, y, g_model.swashR.value, LEFT|attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.swashR.value, 100);
        break;

      case ITEM_HELI_ELE:
        lcd_putsLeft(y, STR_ELEVATOR);
        putsMixerSource(MODEL_HELI_2ND_COLUMN, y, g_model.swashR.elevatorSource, attr);
        if (attr) CHECK_INCDEC_MODELSOURCE(event, g_model.swashR.elevatorSource, 0, MIXSRC_LAST_CH);
        break;

      case ITEM_HELI_ELE_WEIGHT:
        lcd_puts(MENU_TITLE_LEFT+INDENT_WIDTH, y, STR_WEIGHT);
        lcd_outdezAtt(MODEL_HELI_2ND_COLUMN, y, g_model.swashR.elevatorWeight, LEFT|attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.swashR.elevatorWeight, -100, 100);
        break;

      case ITEM_HELI_AIL:
        lcd_putsLeft(y, STR_AILERON);
        putsMixerSource(MODEL_HELI_2ND_COLUMN, y, g_model.swashR.aileronSource, attr);
        if (attr) CHECK_INCDEC_MODELSOURCE(event, g_model.swashR.aileronSource, 0, MIXSRC_LAST_CH);
        break;

      case ITEM_HELI_AIL_WEIGHT:
        lcd_puts(MENU_TITLE_LEFT+INDENT_WIDTH, y, STR_WEIGHT);
        lcd_outdezAtt(MODEL_HELI_2ND_COLUMN, y, g_model.swashR.aileronWeight, LEFT|attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.swashR.aileronWeight, -100, 100);
        break;

      case ITEM_HELI_COL:
        lcd_putsLeft(y, STR_COLLECTIVE);
        putsMixerSource(MODEL_HELI_2ND_COLUMN, y, g_model.swashR.collectiveSource, attr);
        if (attr) CHECK_INCDEC_MODELSOURCE(event, g_model.swashR.collectiveSource, 0, MIXSRC_LAST_CH);
        break;

      case ITEM_HELI_COL_WEIGHT:
        lcd_puts(MENU_TITLE_LEFT+INDENT_WIDTH, y, STR_WEIGHT);
        lcd_outdezAtt(MODEL_HELI_2ND_COLUMN, y, g_model.swashR.collectiveWeight, LEFT|attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.swashR.collectiveWeight, -100, 100);
        break;

    }
  }
}
