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

#include <QtCore>
#include <QObject>
#include <QString>

// TODO create a Board class with all these functions

namespace Board {

  enum Type
  {
    BOARD_UNKNOWN = -1,
    BOARD_STOCK = 0,
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
    BOARD_X12S,
    BOARD_X10,
    BOARD_TARANIS_XLITE,
    BOARD_ENUM_COUNT
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

  enum StickAxes {
    STICK_AXIS_LH = 0,
    STICK_AXIS_LV,
    STICK_AXIS_RV,
    STICK_AXIS_RH,
    STICK_AXIS_COUNT
  };

  enum TrimAxes {
    TRIM_AXIS_LH = 0,
    TRIM_AXIS_LV,
    TRIM_AXIS_RV,
    TRIM_AXIS_RH,
    TRIM_AXIS_T5,
    TRIM_AXIS_T6,
    TRIM_AXIS_COUNT
  };

  enum TrimSwitches
  {
    TRIM_SW_LH_DEC,
    TRIM_SW_LH_INC,
    TRIM_SW_LV_DEC,
    TRIM_SW_LV_INC,
    TRIM_SW_RV_DEC,
    TRIM_SW_RV_INC,
    TRIM_SW_RH_DEC,
    TRIM_SW_RH_INC,
    TRIM_SW_T5_DEC,
    TRIM_SW_T5_INC,
    TRIM_SW_T6_DEC,
    TRIM_SW_T6_INC,
    TRIM_SW_COUNT
  };

  enum Capability {
    Sticks,
    Pots,
    FactoryInstalledPots,
    Sliders,
    MouseAnalogs,
    MaxAnalogs,
    MultiposPots,
    MultiposPotsPositions,
    Switches,
    SwitchPositions,
    FactoryInstalledSwitches,
    NumTrims,
    NumTrimSwitches
  };

  struct SwitchInfo
  {
    SwitchType config;
    QString name;
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

}

class Boards
{
  Q_DECLARE_TR_FUNCTIONS(Boards)

  public:

    Boards(Board::Type board)
    {
      setBoardType(board);
    }

    void setBoardType(const Board::Type & board);
    Board::Type getBoardType() const { return m_boardType; }

    const uint32_t getFourCC() const { return getFourCC(m_boardType); }
    const int getEEpromSize() const { return getEEpromSize(m_boardType); }
    const int getFlashSize() const { return getFlashSize(m_boardType); }
    const Board::SwitchInfo getSwitchInfo(unsigned index) const { return getSwitchInfo(m_boardType, index); }
    const int getCapability(Board::Capability capability) const { return getCapability(m_boardType, capability); }
    const QString getAnalogInputName(unsigned index) const { return getAnalogInputName(m_boardType, index); }
    const bool isBoardCompatible(Board::Type board2) const { return isBoardCompatible(m_boardType, board2); }

    static uint32_t getFourCC(Board::Type board);
    static const int getEEpromSize(Board::Type board);
    static const int getFlashSize(Board::Type board);
    static const Board::SwitchInfo getSwitchInfo(Board::Type board, unsigned index);
    static const int getCapability(Board::Type board, Board::Capability capability);
    static const QString getAxisName(int index);
    static const QString getAnalogInputName(Board::Type board, unsigned index);
    static const bool isBoardCompatible(Board::Type board1, Board::Type board2);
    static const QString getBoardName(Board::Type board);

  protected:

    Board::Type m_boardType;
};

// temporary aliases for transition period, use Boards class instead.
#define getBoardCapability(b__, c__)   Boards::getCapability(b__, c__)
#define getEEpromSize(b__)             Boards::getEEpromSize(b__)
#define getSwitchInfo(b__, i__)        Boards::getSwitchInfo(b__, i__)

#define IS_9X(board)                   (board==Board::BOARD_STOCK || board==Board::BOARD_M128)
#define IS_STOCK(board)                (board==Board::BOARD_STOCK)
#define IS_M128(board)                 (board==Board::BOARD_M128)
#define IS_2560(board)                 (board==Board::BOARD_GRUVIN9X || board==Board::BOARD_MEGA2560)
#define IS_SKY9X(board)                (board==Board::BOARD_SKY9X || board==Board::BOARD_9XRPRO || board==Board::BOARD_AR9X)
#define IS_9XRPRO(board)               (board==Board::BOARD_9XRPRO)
#define IS_TARANIS_XLITE(board)        (board==Board::BOARD_TARANIS_XLITE)
#define IS_TARANIS_X7(board)           (board==Board::BOARD_TARANIS_X7)
#define IS_TARANIS_X9(board)           (board==Board::BOARD_TARANIS_X9D || board==Board::BOARD_TARANIS_X9DP || board==Board::BOARD_TARANIS_X9E)
#define IS_TARANIS_X9D(board)          (board==Board::BOARD_TARANIS_X9D || board==Board::BOARD_TARANIS_X9DP)
#define IS_TARANIS_PLUS(board)         (board==Board::BOARD_TARANIS_X9DP || board==Board::BOARD_TARANIS_X9E)
#define IS_TARANIS_X9E(board)          (board==Board::BOARD_TARANIS_X9E)
#define IS_TARANIS(board)              (IS_TARANIS_X9(board) || IS_TARANIS_X7(board) || IS_TARANIS_XLITE(board))
#define IS_TARANIS_SMALL(board)        (board==Board::BOARD_TARANIS_X7 || board==Board::BOARD_TARANIS_XLITE)
#define IS_TARANIS_NOT_X9E(board)      (IS_TARANIS(board) && !IS_TARANIS_X9E(board))
#define IS_HORUS_X12S(board)           (board==Board::BOARD_X12S)
#define IS_HORUS_X10(board)            (board==Board::BOARD_X10)
#define IS_HORUS(board)                (IS_HORUS_X12S(board) || IS_HORUS_X10(board))
#define IS_HORUS_OR_TARANIS(board)     (IS_HORUS(board) || IS_TARANIS(board))
#define IS_STM32(board)                (IS_TARANIS(board) || IS_HORUS(board))
#define IS_ARM(board)                  (IS_STM32(board) || IS_SKY9X(board))
#define HAS_LARGE_LCD(board)           (IS_HORUS(board) || IS_TARANIS_X9(board))

#endif // _BOARDS_H_
