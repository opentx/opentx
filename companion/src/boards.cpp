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

#include "boards.h"
#include "macros.h"

int getEEpromSize(BoardEnum board)
{
  switch (board) {
    case BOARD_STOCK:
      return EESIZE_STOCK;
    case BOARD_M128:
      return EESIZE_M128;
    case BOARD_MEGA2560:
    case BOARD_GRUVIN9X:
      return EESIZE_GRUVIN9X;
    case BOARD_SKY9X:
      return EESIZE_SKY9X;
    case BOARD_9XRPRO:
    case BOARD_AR9X:
      return EESIZE_9XRPRO;
    case BOARD_TARANIS_X7:
    case BOARD_TARANIS_X9D:
    case BOARD_TARANIS_X9DP:
    case BOARD_TARANIS_X9E:
    case BOARD_FLAMENCO:
      return EESIZE_TARANIS;
    default:
      return 0; // unlimited
  }
}

SwitchInfo getSwitchInfo(BoardEnum board, unsigned index)
{
  if (IS_TARANIS_X7(board)) {
    const SwitchInfo switches[] = {{SWITCH_3POS,   "SA"},
                                   {SWITCH_3POS,   "SB"},
                                   {SWITCH_3POS,   "SC"},
                                   {SWITCH_3POS,   "SD"},
                                   {SWITCH_2POS,   "SF"},
                                   {SWITCH_TOGGLE, "SH"}};
    if (index < DIM(switches))
      return switches[index];
  }
  else if (IS_HORUS_OR_TARANIS(board)) {
    const SwitchInfo switches[] = {{SWITCH_3POS,   "SA"},
                                   {SWITCH_3POS,   "SB"},
                                   {SWITCH_3POS,   "SC"},
                                   {SWITCH_3POS,   "SD"},
                                   {SWITCH_3POS,   "SE"},
                                   {SWITCH_2POS,   "SF"},
                                   {SWITCH_3POS,   "SG"},
                                   {SWITCH_TOGGLE, "SH"},
                                   {SWITCH_3POS,   "SI"},
                                   {SWITCH_3POS,   "SJ"},
                                   {SWITCH_3POS,   "SK"},
                                   {SWITCH_3POS,   "SL"},
                                   {SWITCH_3POS,   "SM"},
                                   {SWITCH_3POS,   "SN"},
                                   {SWITCH_3POS,   "SO"},
                                   {SWITCH_3POS,   "SP"},
                                   {SWITCH_3POS,   "SQ"},
                                   {SWITCH_3POS,   "SR"}};
    if (index < DIM(switches))
      return switches[index];
  }
  else {
    const SwitchInfo switches[] = {{SWITCH_2POS,   "THR"},
                                   {SWITCH_2POS,   "RUD"},
                                   {SWITCH_2POS,   "ELE"},
                                   {SWITCH_3POS,   "3POS"},
                                   {SWITCH_2POS,   "AIL"},
                                   {SWITCH_2POS,   "GEA"},
                                   {SWITCH_TOGGLE, "TRN"}};
    if (index < DIM(switches))
      return switches[index];
  }

  return {SWITCH_NOT_AVAILABLE, "???"};
}