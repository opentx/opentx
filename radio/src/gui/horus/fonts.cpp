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

#include "../../opentx.h"

const uint16_t font_tinsize_specs[] PROGMEM =
#include "font_tinsize.specs"
;

const pm_uchar font_tinsize[] PROGMEM = {
#include "font_tinsize.lbm"
};

const uint16_t font_smlsize_specs[] PROGMEM =
#include "font_smlsize.specs"
;

const pm_uchar font_smlsize[] PROGMEM = {
#include "font_smlsize.lbm"
};

const uint16_t font_stdsize_specs[] PROGMEM =
#include "font_stdsize.specs"
;

const pm_uchar font_stdsize[] PROGMEM = {
#include "font_stdsize.lbm"
};

const uint16_t font_midsize_specs[] PROGMEM =
#include "font_midsize.specs"
;

const pm_uchar font_midsize[] PROGMEM = {
#include "font_midsize.lbm"
};

const uint16_t font_dblsize_specs[] PROGMEM =
#include "font_dblsize.specs"
;

const pm_uchar font_dblsize[] PROGMEM = {
#include "font_dblsize.lbm"
};

const uint16_t font_xxlsize_specs[] PROGMEM =
#include "font_xxlsize.specs"
;

const pm_uchar font_xxlsize[] PROGMEM = {
#include "font_xxlsize.lbm"
};

const uint16_t *fontspecsTable[16] = { font_stdsize_specs, font_tinsize_specs, font_smlsize_specs, font_midsize_specs, font_dblsize_specs, font_xxlsize_specs };
const pm_uchar *fontsTable[16] = { font_stdsize, font_tinsize, font_smlsize, font_midsize, font_dblsize, font_xxlsize };
