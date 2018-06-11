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

// TODO remove all those constants
// Update: These are now all only used within this class.
//  External access is only via getEEpromSize() and getFlashSize()

#define EESIZE_STOCK                   2048
#define EESIZE_M128                    4096
#define EESIZE_GRUVIN9X                4096
#define EESIZE_TARANIS                 (32*1024)
#define EESIZE_SKY9X                   (128*4096)
#define EESIZE_9XRPRO                  (128*4096)
#define EESIZE_MAX                     EESIZE_9XRPRO

// getFlashSize() (and these macros) is only used by radiointerface::getDfuArgs (perhaps can find a better way?)
#define FSIZE_STOCK                    (64*1024)
#define FSIZE_M128                     (128*1024)
#define FSIZE_GRUVIN9X                 (256*1024)
#define FSIZE_TARANIS                  (512*1024)
#define FSIZE_SKY9X                    (256*1024)
#define FSIZE_9XRPRO                   (512*1024)
#define FSIZE_HORUS                    (2048*1024)
#define FSIZE_MAX                      FSIZE_HORUS

using namespace Board;

void Boards::setBoardType(const Type & board)
{
  if (board >= BOARD_UNKNOWN && board < BOARD_ENUM_COUNT)
    m_boardType = board;
  else
    m_boardType = BOARD_UNKNOWN;
}

uint32_t Boards::getFourCC(Type board)
{
  switch (board) {
    case BOARD_X12S:
      return 0x3478746F;
    case BOARD_X10:
      return 0x3778746F;
    case BOARD_TARANIS_XLITE:
      return 0x3978746F;
    case BOARD_TARANIS_X7:
      return 0x3678746F;
    case BOARD_TARANIS_X9E:
      return 0x3578746F;
    case BOARD_TARANIS_X9D:
    case BOARD_TARANIS_X9DP:
      return 0x3378746F;
    case BOARD_SKY9X:
    case BOARD_AR9X:
    case BOARD_9XRPRO:
      return 0x3278746F;
    case BOARD_MEGA2560:
    case BOARD_GRUVIN9X:
      return 0x3178746F;
    default:
      return 0;
  }
}

const int Boards::getEEpromSize(Board::Type board)
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
    case BOARD_TARANIS_XLITE:
    case BOARD_TARANIS_X7:
    case BOARD_TARANIS_X9D:
    case BOARD_TARANIS_X9DP:
    case BOARD_TARANIS_X9E:
      return EESIZE_TARANIS;
    case BOARD_UNKNOWN:
      return EESIZE_MAX;
    default:
      return 0; // unlimited
  }
}

const int Boards::getFlashSize(Type board)
{
  switch (board) {
    case BOARD_STOCK:
      return FSIZE_STOCK;
    case BOARD_M128:
      return FSIZE_M128;
    case BOARD_MEGA2560:
    case BOARD_GRUVIN9X:
      return FSIZE_GRUVIN9X;
    case BOARD_SKY9X:
      return FSIZE_SKY9X;
    case BOARD_9XRPRO:
    case BOARD_AR9X:
      return FSIZE_9XRPRO;
    case BOARD_TARANIS_XLITE:
    case BOARD_TARANIS_X7:
    case BOARD_TARANIS_X9D:
    case BOARD_TARANIS_X9DP:
    case BOARD_TARANIS_X9E:
      return FSIZE_TARANIS;
    case BOARD_X12S:
    case BOARD_X10:
      return FSIZE_HORUS;
    case BOARD_UNKNOWN:
      return FSIZE_MAX;
    default:
      return 0;
  }
}

const SwitchInfo Boards::getSwitchInfo(Board::Type board, unsigned index)
{
  if (IS_TARANIS_XLITE(board)) {
    const Board::SwitchInfo switches[] = {
      {SWITCH_3POS,   "SA"},
      {SWITCH_3POS,   "SB"},
      {SWITCH_2POS,   "SC"},
      {SWITCH_2POS,   "SD"}
    };
    if (index < DIM(switches))
      return switches[index];
  }
  if (IS_TARANIS_X7(board)) {
    const Board::SwitchInfo switches[] = {
      {SWITCH_3POS,   "SA"},
      {SWITCH_3POS,   "SB"},
      {SWITCH_3POS,   "SC"},
      {SWITCH_3POS,   "SD"},
      {SWITCH_2POS,   "SF"},
      {SWITCH_TOGGLE, "SH"}
    };
    if (index < DIM(switches))
      return switches[index];
  }
  else if (IS_HORUS_OR_TARANIS(board)) {
    const Board::SwitchInfo switches[] = {
      {SWITCH_3POS,   "SA"},
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
      {SWITCH_3POS,   "SR"}
    };
    if (index < DIM(switches))
      return switches[index];
  }
  else {
    const Board::SwitchInfo switches[] = {
      {SWITCH_3POS,   "3POS"},
      {SWITCH_2POS,   "THR"},
      {SWITCH_2POS,   "RUD"},
      {SWITCH_2POS,   "ELE"},
      {SWITCH_2POS,   "AIL"},
      {SWITCH_2POS,   "GEA"},
      {SWITCH_TOGGLE, "TRN"}
    };
    if (index < DIM(switches))
      return switches[index];
  }

  return {SWITCH_NOT_AVAILABLE, "???"};
}

const int Boards::getCapability(Board::Type board, Board::Capability capability)
{
  switch (capability) {
    case Sticks:
      return 4;

    case Pots:
      if (IS_TARANIS_SMALL(board))
        return 2;
      else if (IS_TARANIS_X9E(board))
        return 4;
      else
        return 3;

    case FactoryInstalledPots:
      if (IS_TARANIS_X9(board))
        return 2;
      else
        return getCapability(board, Pots);

    case Sliders:
      if (IS_HORUS_X12S(board) || IS_TARANIS_X9E(board))
        return 4;
      else if (IS_TARANIS_X9D(board) || IS_HORUS_X10(board))
        return 2;
      else
        return 0;

    case MouseAnalogs:
      if (IS_HORUS_X12S(board))
        return 2;
      else
        return 0;

    case MaxAnalogs:
      return getCapability(board, Board::Sticks) + getCapability(board, Board::Pots) + getCapability(board, Board::Sliders) +  getCapability(board, Board::MouseAnalogs);

    case MultiposPots:
      return IS_HORUS_OR_TARANIS(board) ? 3 : 0;

    case MultiposPotsPositions:
      return IS_HORUS_OR_TARANIS(board) ? 6 : 0;

    case Switches:
      if (IS_TARANIS_X9E(board))
        return 18;
      else if (IS_TARANIS_X7(board))
        return 6;
      else if (IS_TARANIS_XLITE(board))
        return 4;
      else if (IS_HORUS_OR_TARANIS(board))
        return 8;
      else
        return 7;

    case FactoryInstalledSwitches:
      if (IS_TARANIS_X9E(board))
        return 8;
      else
        return getCapability(board, Switches);

    case SwitchPositions:
      if (IS_HORUS_OR_TARANIS(board))
        return getCapability(board, Switches) * 3;
      else
        return 9;

    case NumTrims:
      if (IS_HORUS(board))
        return 6;
      else
        return 4;

    case NumTrimSwitches:
      return getCapability(board, NumTrims) * 2;
  }

  return 0;
}

const QString Boards::getAxisName(int index)
{
  const QString axes[] = {
    tr("Left Horizontal"),
    tr("Left Vertical"),
    tr("Right Vertical"),
    tr("Right Horizontal"),
    tr("Aux. 1"),
    tr("Aux. 2"),
  };
  if (index < (int)DIM(axes))
    return axes[index];
  else
    return tr("Unknown");
}

const QString Boards::getAnalogInputName(Board::Type board, unsigned index)
{
  if ((int)index < getBoardCapability(board, Board::Sticks)) {
    const QString sticks[] = {
      tr("Rud"),
      tr("Ele"),
      tr("Thr"),
      tr("Ail")
    };
    return sticks[index];
  }

  index -= getCapability(board, Board::Sticks);

  if (IS_9X(board) || IS_2560(board) || IS_SKY9X(board)) {
    const QString pots[] = {
      "P1",
      "P2",
      "P3"
    };
    if (index < DIM(pots))
      return pots[index];
  }
  else if (IS_TARANIS_X9E(board)) {
    const QString pots[] = {
      "F1",
      "F2",
      "F3",
      "F4",
      "S1",
      "S2",
      "LS",
      "RS"
    };
    if (index < DIM(pots))
      return pots[index];
  }
  else if (IS_TARANIS(board)) {
    const QString pots[] = {
      "S1",
      "S2",
      "S3",
      "LS",
      "RS"
    };
    if (index < DIM(pots))
      return pots[index];
  }
  else if (IS_HORUS_X12S(board)) {
    const QString pots[] = {
      "S1",
      "6P",
      "S2",
      "L1",
      "L2",
      "LS",
      "RS",
      "JSx",
      "JSy"
    };
    if (index < DIM(pots))
      return pots[index];
  }
  else if (IS_HORUS_X10(board)) {
    const QString pots[] = {
      "S1",
      "6P",
      "S2",
      "LS",
      "RS"
    };
    if (index < DIM(pots))
      return pots[index];
  }

  return "???";
}

const bool Boards::isBoardCompatible(Type board1, Type board2)
{
  return (getFourCC(board1) == getFourCC(board2));
}

const QString Boards::getBoardName(Board::Type board)
{
  switch (board) {
    case BOARD_STOCK:
      return "9X";
    case BOARD_M128:
      return "9X128";
    case BOARD_GRUVIN9X:
      return "Gruvin9x";
    case BOARD_MEGA2560:
      return "MEGA2560";
    case BOARD_TARANIS_X7:
      return "Taranis X7/X7S";
     case BOARD_TARANIS_XLITE:
      return "Taranis X-Lite";
    case BOARD_TARANIS_X9D:
      return "Taranis X9D";
    case BOARD_TARANIS_X9DP:
      return "Taranis X9D+";
    case BOARD_TARANIS_X9E:
      return "Taranis X9E";
    case BOARD_SKY9X:
      return "Sky9x";
    case BOARD_9XRPRO:
      return "9XR-PRO";
    case BOARD_AR9X:
      return "AR9X";
    case BOARD_X12S:
      return "Horus X12S";
    case BOARD_X10:
      return "Horus X10/X10S";
    default:
      return tr("Unknown");
  }
}
