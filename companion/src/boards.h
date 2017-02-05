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

#ifndef _BOARDS_H_
#define _BOARDS_H_

// TODO create a Board class with all these functions

namespace Board {

  enum Type
  {
    BOARD_STOCK,
    BOARD_M128,
    BOARD_MEGA2560,
    BOARD_GRUVIN9X,
    BOARD_SKY9X,
    BOARD_9XRPRO,
    BOARD_AR9X,
    BOARD_TARANIS_X7,
    BOARD_TARANIS_X9D,
    BOARD_TARANIS_X9DP,
    BOARD_TARANIS_X9E,
    BOARD_FLAMENCO,
    BOARD_X12S,
    BOARD_X10,
    BOARD_UNKNOWN = -1
  };

  enum PotType
  {
    POT_NONE,
    POT_WITH_DETENT,
    POT_MULTIPOS_SWITCH,
    POT_WITHOUT_DETENT
  };

  enum SliderType
  {
    SLIDER_NONE,
    SLIDER_WITH_DETENT
  };

  enum SwitchType
  {
    SWITCH_NOT_AVAILABLE,
    SWITCH_TOGGLE,
    SWITCH_2POS,
    SWITCH_3POS
  };

  struct SwitchInfo
  {
    SwitchType config;
    const char * name;
  };

  struct SwitchPosition {
    SwitchPosition(unsigned int index, unsigned int position):
      index(index),
      position(position)
      {
      }
      unsigned int index;
      unsigned int position;
  };

  enum Capability {
    Pots,
    Sliders,
    Switches,
    FactoryInstalledSwitches
  };
}

// TODO remove all those constants
#define EESIZE_STOCK                   2048
#define EESIZE_M128                    4096
#define EESIZE_GRUVIN9X                4096
#define EESIZE_TARANIS                 (32*1024)
#define EESIZE_SKY9X                   (128*4096)
#define EESIZE_9XRPRO                  (128*4096)
#define EESIZE_MAX                     EESIZE_9XRPRO

// TODO remove all those constants
#define FSIZE_STOCK                    (64*1024)
#define FSIZE_M128                     (128*1024)
#define FSIZE_GRUVIN9X                 (256*1024)
#define FSIZE_TARANIS                  (512*1024)
#define FSIZE_SKY9X                    (256*1024)
#define FSIZE_9XRPRO                   (512*1024)
#define FSIZE_HORUS                    (2048*1024)
#define FSIZE_MAX                      FSIZE_HORUS

int getEEpromSize(Board::Type board);
Board::SwitchInfo getSwitchInfo(Board::Type board, unsigned index);

int getBoardCapability(Board::Type board, Board::Capability capability);

#define IS_9X(board)                   (board==Board::BOARD_STOCK || board==Board::BOARD_M128)
#define IS_STOCK(board)                (board==Board::BOARD_STOCK)
#define IS_2560(board)                 (board==Board::BOARD_GRUVIN9X || board==Board::BOARD_MEGA2560)
#define IS_SKY9X(board)                (board==Board::BOARD_SKY9X || board==Board::BOARD_9XRPRO || board==Board::BOARD_AR9X)
#define IS_9XRPRO(board)               (board==Board::BOARD_9XRPRO)
#define IS_TARANIS_X7(board)           (board==Board::BOARD_TARANIS_X7)
#define IS_TARANIS_X9(board)           (board==Board::BOARD_TARANIS_X9D || board==Board::BOARD_TARANIS_X9DP || board==Board::BOARD_TARANIS_X9E)
#define IS_TARANIS_X9D(board)          (board==Board::BOARD_TARANIS_X9D || board==Board::BOARD_TARANIS_X9DP)
#define IS_TARANIS_PLUS(board)         (board==Board::BOARD_TARANIS_X9DP || board==Board::BOARD_TARANIS_X9E)
#define IS_TARANIS_X9E(board)          (board==Board::BOARD_TARANIS_X9E)
#define IS_TARANIS(board)              (IS_TARANIS_X9(board) || IS_TARANIS_X7(board))
#define IS_HORUS_X12S(board)           (board==Board::BOARD_X12S)
#define IS_HORUS_X10(board)            (board==Board::BOARD_X10)
#define IS_HORUS(board)                (IS_HORUS_X12S(board) || IS_HORUS_X10(board))
#define IS_HORUS_OR_TARANIS(board)     (IS_HORUS(board) || IS_TARANIS(board))
#define IS_FLAMENCO(board)             (board==Board::BOARD_FLAMENCO)
#define IS_STM32(board)                (IS_TARANIS(board) || IS_HORUS(board) || IS_FLAMENCO(board))
#define IS_ARM(board)                  (IS_STM32(board) || IS_SKY9X(board))
#define HAS_LARGE_LCD(board)           (IS_HORUS(board) || (IS_TARANIS(board) && !IS_TARANIS_X7(board)))

#endif // _BOARDS_H_
