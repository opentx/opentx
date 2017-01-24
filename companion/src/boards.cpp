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

using namespace Board;

int getEEpromSize(Board::Type board)
{
  switch (board) {
    case Board::BOARD_STOCK:
      return EESIZE_STOCK;
    case Board::BOARD_M128:
      return EESIZE_M128;
    case Board::BOARD_MEGA2560:
    case Board::BOARD_GRUVIN9X:
      return EESIZE_GRUVIN9X;
    case Board::BOARD_SKY9X:
      return EESIZE_SKY9X;
    case Board::BOARD_9XRPRO:
    case Board::BOARD_AR9X:
      return EESIZE_9XRPRO;
    case Board::BOARD_TARANIS_X7:
    case Board::BOARD_TARANIS_X9D:
    case Board::BOARD_TARANIS_X9DP:
    case Board::BOARD_TARANIS_X9E:
    case Board::BOARD_FLAMENCO:
      return EESIZE_TARANIS;
    default:
      return 0; // unlimited
  }
}

Board::SwitchInfo getSwitchInfo(Board::Type board, unsigned index)
{
  if (IS_TARANIS_X7(board)) {
    const Board::SwitchInfo switches[] = {{Board::SWITCH_3POS,   "SA"},
                                          {Board::SWITCH_3POS,   "SB"},
                                          {Board::SWITCH_3POS,   "SC"},
                                          {Board::SWITCH_3POS,   "SD"},
                                          {Board::SWITCH_2POS,   "SF"},
                                          {Board::SWITCH_TOGGLE, "SH"}};
    if (index < DIM(switches))
      return switches[index];
  }
  else if (IS_HORUS_OR_TARANIS(board)) {
    const Board::SwitchInfo switches[] = {{Board::SWITCH_3POS,   "SA"},
                                          {Board::SWITCH_3POS,   "SB"},
                                          {Board::SWITCH_3POS,   "SC"},
                                          {Board::SWITCH_3POS,   "SD"},
                                          {Board::SWITCH_3POS,   "SE"},
                                          {Board::SWITCH_2POS,   "SF"},
                                          {Board::SWITCH_3POS,   "SG"},
                                          {Board::SWITCH_TOGGLE, "SH"},
                                          {Board::SWITCH_3POS,   "SI"},
                                          {Board::SWITCH_3POS,   "SJ"},
                                          {Board::SWITCH_3POS,   "SK"},
                                          {Board::SWITCH_3POS,   "SL"},
                                          {Board::SWITCH_3POS,   "SM"},
                                          {Board::SWITCH_3POS,   "SN"},
                                          {Board::SWITCH_3POS,   "SO"},
                                          {Board::SWITCH_3POS,   "SP"},
                                          {Board::SWITCH_3POS,   "SQ"},
                                          {Board::SWITCH_3POS,   "SR"}};
    if (index < DIM(switches))
      return switches[index];
  }
  else {
    const Board::SwitchInfo switches[] = {{Board::SWITCH_2POS,   "THR"},
                                          {Board::SWITCH_2POS,   "RUD"},
                                          {Board::SWITCH_2POS,   "ELE"},
                                          {Board::SWITCH_3POS,   "3POS"},
                                          {Board::SWITCH_2POS,   "AIL"},
                                          {Board::SWITCH_2POS,   "GEA"},
                                          {Board::SWITCH_TOGGLE, "TRN"}};
    if (index < DIM(switches))
      return switches[index];
  }

  return {Board::SWITCH_NOT_AVAILABLE, "???"};
}