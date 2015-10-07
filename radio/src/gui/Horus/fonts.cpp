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

const uint16_t font_tinsize_specs[] PROGMEM =
#include "fonts/font_tinsize.specs"
;

const pm_uchar font_tinsize[] PROGMEM = {
#include "fonts/font_tinsize.lbm"
};

const uint16_t font_smlsize_specs[] PROGMEM =
#include "fonts/font_smlsize.specs"
;

const pm_uchar font_smlsize[] PROGMEM = {
#include "fonts/font_smlsize.lbm"
};

const uint16_t font_stdsize_specs[] PROGMEM =
#include "fonts/font_stdsize.specs"
;

const pm_uchar font_stdsize[] PROGMEM = {
#include "fonts/font_stdsize.lbm"
};

const uint16_t font_midsize_specs[] PROGMEM =
#include "fonts/font_midsize.specs"
;

const pm_uchar font_midsize[] PROGMEM = {
#include "fonts/font_midsize.lbm"
};

const uint16_t font_dblsize_specs[] PROGMEM =
#include "fonts/font_dblsize.specs"
;

const pm_uchar font_dblsize[] PROGMEM = {
#include "fonts/font_dblsize.lbm"
};

const uint16_t font_xxlsize_specs[] PROGMEM =
#include "fonts/font_xxlsize.specs"
;

const pm_uchar font_xxlsize[] PROGMEM = {
#include "fonts/font_xxlsize.lbm"
};

const uint16_t *fontspecsTable[16] = { font_stdsize_specs, font_tinsize_specs, font_smlsize_specs, font_midsize_specs, font_dblsize_specs, font_xxlsize_specs };
const pm_uchar *fontsTable[16] = { font_stdsize, font_tinsize, font_smlsize, font_midsize, font_dblsize, font_xxlsize };
