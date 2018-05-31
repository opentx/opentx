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
#include "helpers.h"
#include "opentxeeprom.h"
#include "customdebug.h"
#include <stdlib.h>
#include <algorithm>

using namespace Board;

#define IS_DBLEEPROM(board, version)          ((IS_2560(board) || board==BOARD_M128) && version >= 213)
// Macro used for Gruvin9x board and M128 board between versions 213 and 214 (when there were stack overflows!)
#define IS_DBLRAM(board, version)             ((IS_2560(board) && version >= 213) || (board==BOARD_M128 && version >= 213 && version <= 214))

#define HAS_PERSISTENT_TIMERS(board)          (IS_ARM(board) || IS_2560(board))
#define MAX_VIEWS(board)                      (HAS_LARGE_LCD(board) ? 2 : 256)
#define MAX_POTS(board, version)              (IS_TARANIS_NOT_X9E(board) && version < 216 ? 2 : Boards::getCapability(board, Board::Pots))
#define MAX_SLIDERS(board)                    (IS_HORUS_X10(board) ? 4 : (Boards::getCapability(board, Board::Sliders))) //TODO need to be remove when x10 eeprom gets fixed
#define MAX_MOUSE_ANALOGS(board)              (IS_HORUS_X10(board) ? 2 : (Boards::getCapability(board, Board::MouseAnalogs))) //TODO need to be remove when x10 eeprom gets fixed
#define MAX_SWITCHES(board, version)          (Boards::getCapability(board, Board::Switches))
#define MAX_SWITCH_SLOTS(board, version)      (IS_TARANIS_X9E(board) ? 32 : 8)  // bitsize of swconfig_t / 2 (see radio/src/datastructs.h)
#define MAX_SWITCHES_POSITION(board, version) (Boards::getCapability(board, Board::SwitchPositions))
#define MAX_ROTARY_ENCODERS(board)            (IS_2560(board) ? 2 : (IS_SKY9X(board) ? 1 : 0))
#define MAX_FLIGHT_MODES(board, version)      (IS_ARM(board) ? 9 :  (IS_DBLRAM(board, version) ? 6 :  5))
#define MAX_TIMERS(board, version)            ((IS_ARM(board) && version >= 217) ? 3 : 2)
#define MAX_MIXERS(board, version)            (IS_ARM(board) ? 64 : 32)
#define MAX_CHANNELS(board, version)          (IS_ARM(board) ? 32 : 16)
#define MAX_TRIMS(board)                      (Boards::getCapability(board, Board::NumTrims))
#define MAX_EXPOS(board, version)             (IS_ARM(board) ? ((IS_HORUS_OR_TARANIS(board) && version >= 216) ? 64 : 32) : (IS_DBLRAM(board, version) ? 16 : 14))
#define MAX_LOGICAL_SWITCHES(board, version)  (IS_ARM(board) ? (version >= 218 ? 64 : 32) : ((IS_DBLEEPROM(board, version) && version<217) ? 15 : 12))
#define MAX_CUSTOM_FUNCTIONS(board, version)  (IS_ARM(board) ? (version >= 216 ? 64 : 32) : (IS_DBLEEPROM(board, version) ? 24 : 16))
#define MAX_CURVES(board, version)            (IS_ARM(board) ? ((HAS_LARGE_LCD(board) && version >= 216) ? 32 : 16) : 8)
#define MAX_GVARS(board, version)             ((IS_ARM(board) && version >= 216) ? 9 : 5)
#define MAX_SCRIPTS(board)                    (IS_HORUS(board) ? 9 : 7)
#define MAX_TELEMETRY_SENSORS(board, version) (32)
#define NUM_PPM_INPUTS(board, version)        ((IS_ARM(board) && version >= 216) ? 16 : 8)
#define ROTENC_COUNT(board, version)          (IS_ARM(board) ? ((IS_STM32(board) && version >= 218) ? 0 : 1) : (IS_2560(board) ? 2 : 0))
#define MAX_AUX_TRIMS(board)                  (IS_HORUS(board) ? 2 : 0)

#define IS_AFTER_RELEASE_21_MARCH_2013(board, version) (version >= 214 || (!IS_ARM(board) && version >= 213))
#define IS_AFTER_RELEASE_23_MARCH_2013(board, version) (version >= 214 || (board==BOARD_STOCK && version >= 213))

inline int switchIndex(int i, Board::Type board, unsigned int version)
{
  bool afterrelease21March2013 = IS_AFTER_RELEASE_21_MARCH_2013(board, version);
  if (!IS_HORUS_OR_TARANIS(board) && afterrelease21March2013)
    return (i<=3 ? i+3 : (i<=6 ? i-3 : i));
  else
    return i;
}

class SwitchesConversionTable: public ConversionTable {

  public:
    SwitchesConversionTable(Board::Type board, unsigned int version, bool timer=false)
    {
      int val=0;
      int offset=0;
      if (timer) {
        offset = 4;
        for (int i=0; i<5; i++) {
          addConversion(RawSwitch(SWITCH_TYPE_TIMER_MODE, i), val++);
        }
      }
      else {
        addConversion(RawSwitch(SWITCH_TYPE_NONE), val++);
      }

      for (int i=1; i<=MAX_SWITCHES_POSITION(board, version); i++) {
        int s = switchIndex(i, board, version);
        if (IS_TARANIS(board) && version < 217) {
          // SF positions 16 and 17 => 16 and 18
          // SH positions 21 and 22 => 22 and 24
          if (s == 17 || s == 23) {
            continue;
          }
          if (s >= 22) {
            addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val);
            addImportConversion(RawSwitch(SWITCH_TYPE_SWITCH, 21+22-s), -val+offset);
            addExportConversion(RawSwitch(SWITCH_TYPE_SWITCH, -s), -val+offset);
            val++;
            continue;
          }
        }
        addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val);
        addConversion(RawSwitch(SWITCH_TYPE_SWITCH, -s), -val+offset);
        val++;
      }

      if (IS_HORUS_OR_TARANIS(board) && version >= 216) {
        for (int i=1; i<=MAX_POTS(board, version)*6; i++) {
          addConversion(RawSwitch(SWITCH_TYPE_MULTIPOS_POT, -i), -val+offset);
          addConversion(RawSwitch(SWITCH_TYPE_MULTIPOS_POT, i), val++);
        }
      }

      if (version >= 216) {
        for (int i=1; i<=2*MAX_TRIMS(board); i++) {
          addConversion(RawSwitch(SWITCH_TYPE_TRIM, -i), -val+offset);
          addConversion(RawSwitch(SWITCH_TYPE_TRIM, i), val++);
        }
      }

      if (version >= 216) {
        for (int i=1; i<=MAX_ROTARY_ENCODERS(board); i++) {
          addConversion(RawSwitch(SWITCH_TYPE_ROTARY_ENCODER, -i), -val+offset);
          addConversion(RawSwitch(SWITCH_TYPE_ROTARY_ENCODER, i), val++);
        }
      }

      for (int i=1; i<=MAX_LOGICAL_SWITCHES(board, version); i++) {
        addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, -i), -val+offset);
        addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
      }

      addConversion(RawSwitch(SWITCH_TYPE_OFF), -val+offset);
      addConversion(RawSwitch(SWITCH_TYPE_ON), val++);

      if (version >= 216) {
        addConversion(RawSwitch(SWITCH_TYPE_ONE, -1), -val+offset);
        addConversion(RawSwitch(SWITCH_TYPE_ONE, 1), val++);
        if (IS_ARM(board)) {
          for (int i=1; i<=MAX_FLIGHT_MODES(board, version); i++) {
            addConversion(RawSwitch(SWITCH_TYPE_FLIGHT_MODE, -i), -val+offset);
            addConversion(RawSwitch(SWITCH_TYPE_FLIGHT_MODE, i), val++);
          }
        }
      }

      if (IS_ARM(board) && version >= 218) {
        addConversion(RawSwitch(SWITCH_TYPE_TELEMETRY, -1), -val+offset);
        addConversion(RawSwitch(SWITCH_TYPE_TELEMETRY, 1), val++);
        for (int i=1; i<=CPN_MAX_SENSORS; i++) {
          addConversion(RawSwitch(SWITCH_TYPE_SENSOR, -i), -val+offset);
          addConversion(RawSwitch(SWITCH_TYPE_SENSOR, i), val++);
        }
      }

      if (version < 216) {
        // previous "moment" switches
        for (int i=1; i<=MAX_SWITCHES_POSITION(board, version); i++) {
          int s = switchIndex(i, board, version);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val++);
        }

        for (int i=1; i<=MAX_LOGICAL_SWITCHES(board, version); i++) {
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
        }

        // previous "One" switch
        addConversion(RawSwitch(SWITCH_TYPE_ON), val++);
      }
    }


  protected:

    void addConversion(const RawSwitch & sw, const int b)
    {
      ConversionTable::addConversion(sw.toValue(), b);
    }

    void addImportConversion(const RawSwitch & sw, const int b)
    {
      ConversionTable::addImportConversion(sw.toValue(), b);
    }

    void addExportConversion(const RawSwitch & sw, const int b)
    {
      ConversionTable::addExportConversion(sw.toValue(), b);
    }

    class Cache {
      public:
        Cache(Board::Type board, unsigned int version, unsigned long flags, SwitchesConversionTable * table):
          board(board),
          version(version),
          flags(flags),
          table(table)
        {
        }
        Board::Type board;
        unsigned int version;
        unsigned long flags;
        SwitchesConversionTable * table;
    };

    static std::list<Cache> internalCache;

  public:

    static SwitchesConversionTable * getInstance(Board::Type board, unsigned int version, unsigned long flags=0)
    {
      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache & element = *it;
        if (element.board == board && element.version == version && element.flags == flags)
          return element.table;
      }

      Cache element(board, version, flags, new SwitchesConversionTable(board, version, flags));
      internalCache.push_back(element);
      return element.table;
    }
    static void Cleanup()
    {
      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache & element = *it;
        if (element.table)
          delete element.table;
      }
      internalCache.clear();
    }
};

std::list<SwitchesConversionTable::Cache> SwitchesConversionTable::internalCache;

#define FLAG_NONONE       0x01
#define FLAG_NOSWITCHES   0x02
#define FLAG_NOTELEMETRY  0x04

class SourcesConversionTable: public ConversionTable {

  public:
    SourcesConversionTable(Board::Type board, unsigned int version, unsigned int variant, unsigned long flags=0)
    {
      bool afterrelease21March2013 = IS_AFTER_RELEASE_21_MARCH_2013(board, version);

      int val=0;

      if (!(flags & FLAG_NONONE)) {
        addConversion(RawSource(SOURCE_TYPE_NONE), val++);
      }

      if ((IS_ARM(board) && version >= 218) || (IS_STM32(board) && version >= 216)) {
        for (int i=0; i<32; i++) {
          addConversion(RawSource(SOURCE_TYPE_VIRTUAL_INPUT, i), val++);
        }
      }

      if (IS_STM32(board) && version >= 216) {
        for (int i = 0; i < MAX_SCRIPTS(board); i++) {
          for (int j = 0; j < 6; j++) {
            addConversion(RawSource(SOURCE_TYPE_LUA_OUTPUT, i * 16 + j), val++);
          }
        }
      }

      for (int i=0; i<CPN_MAX_STICKS+MAX_POTS(board, version)+MAX_SLIDERS(board)+MAX_MOUSE_ANALOGS(board); i++) {
        addConversion(RawSource(SOURCE_TYPE_STICK, i), val++);
      }

      for (int i=0; i<MAX_ROTARY_ENCODERS(board); i++) {
        addConversion(RawSource(SOURCE_TYPE_ROTARY_ENCODER, 0), val++);
      }

      if (!afterrelease21March2013) {
        for (int i=0; i<CPN_MAX_STICKS; i++) {
          addConversion(RawSource(SOURCE_TYPE_TRIM, i), val++);
        }
      }

      addConversion(RawSource(SOURCE_TYPE_MAX), val++);

      if (afterrelease21March2013) {
        for (int i=0; i<3; i++)
          addConversion(RawSource(SOURCE_TYPE_CYC, i), val++);
      }

      if (afterrelease21March2013) {
        for (int i=0; i<MAX_TRIMS(board); i++)
          addConversion(RawSource(SOURCE_TYPE_TRIM, i), val++);
      }

      addConversion(RawSource(SOURCE_TYPE_SWITCH, 0), val++);

      if (!(flags & FLAG_NOSWITCHES)) {
        if (afterrelease21March2013) {
          for (int i=1; i<MAX_SWITCHES(board, version); i++)
            addConversion(RawSource(SOURCE_TYPE_SWITCH, i), val++);
        }
        else {
          for (int i=1; i<=9; i++) {
            if (i>=4 && i<=6)
              addConversion(RawSource(SOURCE_TYPE_SWITCH, 0), val++);
            else
              addConversion(RawSource(SOURCE_TYPE_SWITCH, i), val++);
          }
        }
        for (int i=0; i<MAX_LOGICAL_SWITCHES(board, version); i++) {
          addConversion(RawSource(SOURCE_TYPE_CUSTOM_SWITCH, i), val++);
        }
      }

      if (!afterrelease21March2013) {
        for (int i=0; i<3; i++) {
          addConversion(RawSource(SOURCE_TYPE_CYC, i), val++);
        }
      }

      for (int i=0; i<NUM_PPM_INPUTS(board, version); i++) {
        addConversion(RawSource(SOURCE_TYPE_PPM, i), val++);
      }

      for (int i=0; i<MAX_CHANNELS(board, version); i++) {
        addConversion(RawSource(SOURCE_TYPE_CH, i), val++);
      }

      if (!(flags & FLAG_NOTELEMETRY)) {
        if (IS_ARM(board) && version >= 217) {
          for (int i=0; i<MAX_GVARS(board, version); i++)
            addConversion(RawSource(SOURCE_TYPE_GVAR, i), val++);
          addConversion(RawSource(SOURCE_TYPE_SPECIAL, 0), val++); // BATT
          addConversion(RawSource(SOURCE_TYPE_SPECIAL, 1), val++); // TIME
          val += 5;
          addConversion(RawSource(SOURCE_TYPE_SPECIAL, 2), val++); // Timer1
          addConversion(RawSource(SOURCE_TYPE_SPECIAL, 3), val++); // Timer2
          addConversion(RawSource(SOURCE_TYPE_SPECIAL, 4), val++); // Timer3
          for (int i=0; i<CPN_MAX_SENSORS*3; ++i) {
            addConversion(RawSource(SOURCE_TYPE_TELEMETRY, i), val++);
          }
        }
        else  {
          if (afterrelease21March2013) {
            if ((board != BOARD_STOCK && (board!=BOARD_M128 || version<215)) || (variant & GVARS_VARIANT)) {
              for (int i=0; i<MAX_GVARS(board, version); i++) {
                addConversion(RawSource(SOURCE_TYPE_GVAR, i), val++);
              }
            }
          }

          for (int i=0; i<TELEMETRY_SOURCE_ACC; i++) {
            if (version < 216) {
              if (i==TELEMETRY_SOURCE_TX_TIME || i==TELEMETRY_SOURCE_RAS || i==TELEMETRY_SOURCE_A3 || i==TELEMETRY_SOURCE_A4 || i==TELEMETRY_SOURCE_ASPEED || i==TELEMETRY_SOURCE_DTE || i==TELEMETRY_SOURCE_CELL_MIN || i==TELEMETRY_SOURCE_CELLS_MIN || i==TELEMETRY_SOURCE_VFAS_MIN)
                continue;
            }
            if (!IS_ARM(board)) {
              if (i==TELEMETRY_SOURCE_TX_TIME || i==TELEMETRY_SOURCE_RAS || i==TELEMETRY_SOURCE_A3 || i==TELEMETRY_SOURCE_A4 || i==TELEMETRY_SOURCE_A3_MIN || i==TELEMETRY_SOURCE_A4_MIN)
                continue;
            }
            addConversion(RawSource(SOURCE_TYPE_TELEMETRY, i), val++);
            if (version >= 216 && IS_ARM(board)) {
              if (i==TELEMETRY_SOURCE_RSSI_RX)
                val += 1;
              if (i==TELEMETRY_SOURCE_TX_TIME)
                val += 5;
              if (i==TELEMETRY_SOURCE_DTE)
                val += 5;
              if (i==TELEMETRY_SOURCE_POWER_MAX)
                val += 5;
            }
          }
        }
      }
    }

  protected:

    void addConversion(const RawSource & source, const int b)
    {
      ConversionTable::addConversion(source.toValue(), b);
    }

    class Cache {
      public:
        Cache(Board::Type board, unsigned int version, unsigned int variant, unsigned long flags, SourcesConversionTable * table):
          board(board),
          version(version),
          variant(variant),
          flags(flags),
          table(table)
        {
        }
        Board::Type board;
        unsigned int version;
        unsigned int variant;
        unsigned long flags;
        SourcesConversionTable * table;
    };
    static std::list<Cache> internalCache;

  public:

    static SourcesConversionTable * getInstance(Board::Type board, unsigned int version, unsigned int variant, unsigned long flags=0)
    {
      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache & element = *it;
        if (element.board == board && element.version == version && element.variant == variant && element.flags == flags)
          return element.table;
      }

      Cache element(board, version, variant, flags, new SourcesConversionTable(board, version, variant, flags));
      internalCache.push_back(element);
      return element.table;
    }
    static void Cleanup()
    {
      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache & element = *it;
        if (element.table)
          delete element.table;
      }
      internalCache.clear();
    }
};

std::list<SourcesConversionTable::Cache> SourcesConversionTable::internalCache;

void OpenTxEepromCleanup(void)
{
  SourcesConversionTable::Cleanup();
  SwitchesConversionTable::Cleanup();
}

template <int N>
class SwitchField: public ConversionField< SignedField<N> > {
  public:
    SwitchField(DataField * parent, RawSwitch & sw, Board::Type board, unsigned int version, unsigned long flags=0):
      ConversionField< SignedField<N> >(parent, _switch, SwitchesConversionTable::getInstance(board, version, flags), DataField::tr("Switch").toLatin1(),
          DataField::tr("Switch ").toLatin1() + sw.toString(board) + DataField::tr(" cannot be exported on this board!").toLatin1()),
      sw(sw),
      _switch(0),
      board(board)
    {
    }

    virtual ~SwitchField()
    {
    }

    virtual void beforeExport()
    {
      _switch = sw.toValue();
      ConversionField< SignedField<N> >::beforeExport();
    }

    virtual void afterImport()
    {
      ConversionField< SignedField<N> >::afterImport();
      sw = RawSwitch(_switch);
      qCDebug(eepromImport) << QString("imported %1: %2").arg(ConversionField< SignedField<N> >::internalField.getName()).arg(sw.toString(board));
    }

  protected:
    RawSwitch & sw;
    int _switch;
    Board::Type board;
};

class TelemetrySourcesConversionTable: public ConversionTable {

  public:
    TelemetrySourcesConversionTable(Board::Type board, unsigned int version)
    {
      int val = 0;

      if (IS_AFTER_RELEASE_21_MARCH_2013(board, version)) {
        addConversion(0, val++);
      }

      addConversion(1+TELEMETRY_SOURCE_TX_BATT, val++);
      if (IS_ARM(board) && version >= 216) {
        addConversion(1+TELEMETRY_SOURCE_TX_TIME, val++);
        for (int i=0; i<5; i++)
          addConversion(1+TELEMETRY_SOURCE_RESERVE, val++);
      }
      addConversion(1+TELEMETRY_SOURCE_TIMER1, val++);
      addConversion(1+TELEMETRY_SOURCE_TIMER2, val++);
      if (IS_ARM(board) && version >= 217)
        addConversion(1+TELEMETRY_SOURCE_TIMER3, val++);
      if (IS_ARM(board) && version >= 216)
        addConversion(1+TELEMETRY_SOURCE_RAS, val++);
      addConversion(1+TELEMETRY_SOURCE_RSSI_TX, val++);
      addConversion(1+TELEMETRY_SOURCE_RSSI_RX, val++);
      if (IS_ARM(board) && version >= 216)
        addConversion(1+TELEMETRY_SOURCE_RESERVE, val++);
      addConversion(1+TELEMETRY_SOURCE_A1, val++);
      addConversion(1+TELEMETRY_SOURCE_A2, val++);
      if (IS_ARM(board) && version >= 216) {
        addConversion(1+TELEMETRY_SOURCE_A3, val++);
        addConversion(1+TELEMETRY_SOURCE_A4, val++);
      }
      addConversion(1+TELEMETRY_SOURCE_ALT, val++);
      addConversion(1+TELEMETRY_SOURCE_RPM, val++);
      addConversion(1+TELEMETRY_SOURCE_FUEL, val++);
      addConversion(1+TELEMETRY_SOURCE_T1, val++);
      addConversion(1+TELEMETRY_SOURCE_T2, val++);
      addConversion(1+TELEMETRY_SOURCE_SPEED, val++);
      addConversion(1+TELEMETRY_SOURCE_DIST, val++);
      addConversion(1+TELEMETRY_SOURCE_GPS_ALT, val++);
      addConversion(1+TELEMETRY_SOURCE_CELL, val++);
      addConversion(1+TELEMETRY_SOURCE_CELLS_SUM, val++);
      addConversion(1+TELEMETRY_SOURCE_VFAS, val++);
      addConversion(1+TELEMETRY_SOURCE_CURRENT, val++);
      addConversion(1+TELEMETRY_SOURCE_CONSUMPTION, val++);
      addConversion(1+TELEMETRY_SOURCE_POWER, val++);
      addConversion(1+TELEMETRY_SOURCE_ACCX, val++);
      addConversion(1+TELEMETRY_SOURCE_ACCY, val++);
      addConversion(1+TELEMETRY_SOURCE_ACCZ, val++);
      addConversion(1+TELEMETRY_SOURCE_HDG, val++);
      addConversion(1+TELEMETRY_SOURCE_VERTICAL_SPEED, val++);
      if (version >= 216) {
        addConversion(1+TELEMETRY_SOURCE_ASPEED, val++);
        addConversion(1+TELEMETRY_SOURCE_DTE, val++);
      }
      if (IS_ARM(board) && version >= 216) {
        for (int i=0; i<5; i++)
          addConversion(1+TELEMETRY_SOURCE_RESERVE, val++);
      }
      addConversion(1+TELEMETRY_SOURCE_A1_MIN, val++);
      addConversion(1+TELEMETRY_SOURCE_A2_MIN, val++);
      if (IS_ARM(board) && version >= 216) {
        addConversion(1+TELEMETRY_SOURCE_A3_MIN, val++);
        addConversion(1+TELEMETRY_SOURCE_A4_MIN, val++);
      }
      addConversion(1+TELEMETRY_SOURCE_ALT_MIN, val++);
      addConversion(1+TELEMETRY_SOURCE_ALT_MAX, val++);
      addConversion(1+TELEMETRY_SOURCE_RPM_MAX, val++);
      addConversion(1+TELEMETRY_SOURCE_T1_MAX, val++);
      addConversion(1+TELEMETRY_SOURCE_T2_MAX, val++);
      addConversion(1+TELEMETRY_SOURCE_SPEED_MAX, val++);
      addConversion(1+TELEMETRY_SOURCE_DIST_MAX, val++);
      if (version >= 216) {
        addConversion(1+TELEMETRY_SOURCE_ASPEED_MAX, val++);
        addConversion(1+TELEMETRY_SOURCE_CELL_MIN, val++);
        addConversion(1+TELEMETRY_SOURCE_CELLS_MIN, val++);
        addConversion(1+TELEMETRY_SOURCE_VFAS_MIN, val++);
      }
      addConversion(1+TELEMETRY_SOURCE_CURRENT_MAX, val++);
      addConversion(1+TELEMETRY_SOURCE_POWER_MAX, val++);
      if (IS_ARM(board) && version >= 216) {
        for (int i=0; i<5; i++)
          addConversion(1+TELEMETRY_SOURCE_RESERVE, val++);
      }
      addConversion(1+TELEMETRY_SOURCE_ACC, val++);
      addConversion(1+TELEMETRY_SOURCE_GPS_TIME, val++);
    }
};

template <int N>
class TelemetrySourceField: public ConversionField< UnsignedField<N> > {
  public:
    TelemetrySourceField(DataField * parent, RawSource & source, Board::Type board, unsigned int version):
      ConversionField< UnsignedField<N> >(parent, _source, &conversionTable, "Telemetry source"),
      conversionTable(board, version),
      source(source),
      board(board),
      version(version),
      _source(0)
    {
    }

    virtual ~TelemetrySourceField()
    {
    }

    virtual void beforeExport()
    {
      _source = (source.type == SOURCE_TYPE_TELEMETRY ? source.index+1 : 0);
      ConversionField< UnsignedField<N> >::beforeExport();
    }

    virtual void afterImport()
    {
      ConversionField< UnsignedField<N> >::afterImport();
      source = (_source == 0 ? RawSource(0) : RawSource(SOURCE_TYPE_TELEMETRY, _source-1));
      qCDebug(eepromImport) << QString("imported %1: %2").arg(ConversionField< UnsignedField<N> >::internalField.getName()).arg(source.toString());
    }

  protected:
    TelemetrySourcesConversionTable conversionTable;
    RawSource & source;
    Board::Type board;
    unsigned int version;
    unsigned int _source;
};

template <int N>
class SourceField: public ConversionField< UnsignedField<N> > {
  public:
    SourceField(DataField * parent, RawSource & source, Board::Type board, unsigned int version, unsigned int variant, unsigned long flags=0):
      ConversionField< UnsignedField<N> >(parent, _source, SourcesConversionTable::getInstance(board, version, variant, flags),
            DataField::tr("Source").toLatin1(), DataField::tr("Source %1 cannot be exported on this board!").arg(source.toString())),
      source(source),
      _source(0)
    {
    }

    virtual ~SourceField()
    {
    }

    virtual void beforeExport()
    {
      _source = source.toValue();
      ConversionField< UnsignedField<N> >::beforeExport();
    }

    virtual void afterImport()
    {
      ConversionField< UnsignedField<N> >::afterImport();
      source = RawSource(_source);
      qCDebug(eepromImport) << QString("imported %1: %2").arg(ConversionField< UnsignedField<N> >::internalField.getName()).arg(source.toString());
    }

  protected:
    RawSource & source;
    unsigned int _source;
};


int smallGvarImport(int gvar)
{
  if (gvar < -10000) {
    gvar = 128 + gvar + 10000;
  }
  else if (gvar > 10000) {
    gvar = -128 +gvar - 10001;
  }
  return gvar;
}

int smallGvarExport(int gvar)
{
  if (gvar > 110) {
    gvar = gvar - 128 - 10000;
  }
  else if (gvar < -110) {
    gvar = gvar + 128 + 10001;
  }
  return gvar;
}

void splitGvarParam(const int gvar, int & _gvar, unsigned int & _gvarParam, const Board::Type board, const int version)
{
  if (version >= 214 || (!IS_ARM(board) && version >= 213)) {
    if (gvar < -10000) {
      _gvarParam = 0;
      _gvar = 256 + gvar + 10000;
    }
    else if (gvar > 10000) {
      _gvarParam = 1;
      _gvar = gvar - 10001;
    }
    else {
      if (gvar < 0) _gvarParam = 1;
      else          _gvarParam = 0;
      _gvar = gvar;  // save routine throws away all unused bits; therefore no 2er complement compensation needed here
    }
  }
  else {
    if (gvar < -10000) {
      _gvarParam = 1;
      _gvar = gvar + 10000;
    }
    else if (gvar > 10000) {
      _gvarParam = 1;
      _gvar = gvar - 10001;
    }
    else {
      _gvarParam = 0;
      _gvar = gvar;
    }
  }
}

void concatGvarParam(int & gvar, const int _gvar, const unsigned int _gvarParam, const Board::Type board, const int version)
{
  if (version >= 214 || (!IS_ARM(board) && version >= 213)) {
        gvar = _gvar;
    if (gvar<0) gvar+=256;  // remove 2er complement, because 8bit part is in this case unsigned
        if (_gvarParam) {  // here is the real sign bit
          gvar|=-256;   // set all higher bits to simulate negative value
        }

    if (gvar>245) {
        gvar = gvar-256 - 10000;
    } else if (gvar<-245) {
        gvar = gvar+256 + 10001;
    }
  }
  else {
    if (_gvarParam == 0) {
      gvar = _gvar;
    }
    else if (_gvar >= 0) {
      gvar = 10001 + _gvar;
    }
    else {
      gvar = -10000 + _gvar;
    }
  }
}

void exportGvarParam(const int gvar, int & _gvar, int version)
{
  int GV1 = (version >= 217 ? 1024 : (version >= 216 ? 4096 : 512));

  if (gvar < -10000) {
    _gvar = GV1 + gvar + 10000;
  }
  else if (gvar > 10000) {
    _gvar = GV1 + gvar - 10001;
  }
  else {
    _gvar = gvar;
  }
}

void importGvarParam(int & gvar, const int _gvar, int version)
{
  int GV1 = (version >= 217 ? 1024 : (version >= 216 ? 4096 : 512));

  if (_gvar >= GV1) {
    gvar = 10001 + _gvar - GV1;
  }
  else if (_gvar >= GV1-9) {
    gvar = -10000 + _gvar - GV1;
  }
  else if (_gvar < -GV1) {
    gvar = -10000 + _gvar + GV1 + 1;
  }
  else if (_gvar < -GV1+9) {
    gvar = 10000 + _gvar + GV1 + 1;
  }
  else {
    gvar = _gvar;
  }

  // qDebug() << QString("import") << _gvar << gvar;
}

class CurveReferenceField: public TransformedField {
  public:
    CurveReferenceField(DataField * parent, CurveReference & curve, Board::Type board, unsigned int version):
      TransformedField(parent, internalField),
      internalField(this),
      curve(curve),
      _curve_type(0),
      _curve_value(0)
    {
      internalField.Append(new UnsignedField<8>(this, _curve_type));
      internalField.Append(new SignedField<8>(this, _curve_value));
    }

    virtual void beforeExport()
    {
      if (curve.value != 0) {
        _curve_type = (unsigned int)curve.type;
        _curve_value = smallGvarImport(curve.value);
      }
      else {
        _curve_type = 0;
        _curve_value = 0;
      }
    }

    virtual void afterImport()
    {
      curve.type = (CurveReference::CurveRefType)_curve_type;
      curve.value = smallGvarExport(_curve_value);
      qCDebug(eepromImport) << QString("imported CurveReference(%1)").arg(curve.toString());
    }

  protected:
    StructField internalField;
    CurveReference & curve;
    unsigned int _curve_type;
    int _curve_value;
};

static int exportHeliInversionWeight(int source) {
  return (source < 0) ? 1 : 0;
}

static int importHeliInversionWeight(int source) {
  return source ? -100: 100;
}

class HeliField: public StructField {
  public:
    HeliField(DataField * parent, SwashRingData & heli, Board::Type board, unsigned int version, unsigned int variant):
      StructField(parent, "Heli")
    {
      if ((IS_ARM(board) && version >= 218) || (IS_STM32(board) && version >= 217)) {
        Append(new UnsignedField<8>(this, heli.type));
        Append(new UnsignedField<8>(this, heli.value));
        Append(new SourceField<8>(this, heli.collectiveSource, board, version, variant));
        Append(new SourceField<8>(this, heli.aileronSource, board, version, variant));
        Append(new SourceField<8>(this, heli.elevatorSource, board, version, variant));
        Append(new SignedField<8>(this, heli.collectiveWeight));
        Append(new SignedField<8>(this, heli.aileronWeight));
        Append(new SignedField<8>(this, heli.elevatorWeight));
      }
      else {
        Append(new ConversionField< SignedField<1> >(this, heli.elevatorWeight, exportHeliInversionWeight, importHeliInversionWeight));
        Append(new ConversionField< SignedField<1> >(this, heli.aileronWeight, exportHeliInversionWeight, importHeliInversionWeight));
        Append(new ConversionField< SignedField<1> >(this, heli.collectiveWeight, exportHeliInversionWeight, importHeliInversionWeight));
        Append(new UnsignedField<5>(this, heli.type));
        Append(new SourceField<8>(this, heli.collectiveSource, board, version, variant));
        //, FLAG_NOSWITCHES)); Fix shift in collective
        Append(new UnsignedField<8>(this, heli.value));
      }
    }
};

class FlightModeField: public TransformedField {
  public:
    FlightModeField(DataField * parent, FlightModeData & phase, int index, Board::Type board, unsigned int version):
      TransformedField(parent, internalField),
      internalField(this, "FlightMode"),
      phase(phase),
      index(index),
      board(board),
      version(version),
      rotencCount(ROTENC_COUNT(board, version))
    {
      if (board == BOARD_STOCK || (board==BOARD_M128 && version>=215)) {
        // On stock we use 10bits per trim
        for (int i=0; i<CPN_MAX_STICKS; i++)
          internalField.Append(new SignedField<8>(this, trimBase[i]));
        for (int i=0; i<CPN_MAX_STICKS; i++)
          internalField.Append(new SignedField<2>(this, trimExt[i]));
      }
      else if (IS_ARM(board) && version >= 218) {
        for (int i=0; i<CPN_MAX_STICKS+MAX_AUX_TRIMS(board); i++) {
          internalField.Append(new SignedField<11>(this, phase.trim[i]));
          internalField.Append(new UnsignedField<5>(this, trimMode[i]));
        }
      }
      else if (IS_TARANIS(board) && version >= 216) {
        for (int i=0; i<CPN_MAX_STICKS; i++) {
          internalField.Append(new SignedField<11>(this, phase.trim[i]));
          internalField.Append(new UnsignedField<5>(this, trimMode[i]));
        }
      }
      else {
        for (int i=0; i<CPN_MAX_STICKS; i++) {
          internalField.Append(new SignedField<16>(this, trimBase[i]));
        }
      }
      if (IS_ARM(board) && version >= 218) {
        if (HAS_LARGE_LCD(board))
          internalField.Append(new ZCharField<10>(this, phase.name, "Flight mode name"));
        else
          internalField.Append(new ZCharField<6>(this, phase.name, "Flight mode name"));
        internalField.Append(new SwitchField<9>(this, phase.swtch, board, version));
        internalField.Append(new SpareBitsField<7>(this));
      }
      else {
        internalField.Append(new SwitchField<8>(this, phase.swtch, board, version));
        if (HAS_LARGE_LCD(board))
          internalField.Append(new ZCharField<10>(this, phase.name));
        else
          internalField.Append(new ZCharField<6>(this, phase.name));
      }
      if (IS_ARM(board) && version >= 214) {
        internalField.Append(new UnsignedField<8>(this, phase.fadeIn));
        internalField.Append(new UnsignedField<8>(this, phase.fadeOut));
      }
      else {
        internalField.Append(new UnsignedField<4>(this, phase.fadeIn));
        internalField.Append(new UnsignedField<4>(this, phase.fadeOut));
      }

      for (int i=0; i<rotencCount; i++) {
        internalField.Append(new SignedField<16>(this, phase.rotaryEncoders[i]));
      }

      if (board != BOARD_STOCK && (board != BOARD_M128 || version < 215)) {
        for (int i=0; i<MAX_GVARS(board, version); i++) {
          internalField.Append(new SignedField<16>(this, phase.gvars[i]));
        }
      }
    }

    virtual void beforeExport()
    {
      for (int i=0; i<CPN_MAX_STICKS+MAX_AUX_TRIMS(board); i++) {
        if ((IS_HORUS_OR_TARANIS(board) && version >= 216) || (IS_ARM(board) && version >= 218)) {
          if (phase.trimMode[i] < 0)
            trimMode[i] = TRIM_MODE_NONE;
          else
            trimMode[i] = 2*phase.trimRef[i] + phase.trimMode[i];
        }
        else {
          int trim;
          if (phase.trimMode[i] < 0)
            trim = 0;
          else if (phase.trimRef[i] != index)
            trim = 501 + phase.trimRef[i] - (phase.trimRef[i] > index ? 1 : 0);
          else
            trim = std::max(-500, std::min(500, phase.trim[i]));
          if (board == BOARD_STOCK || (board == BOARD_M128 && version >= 215)) {
            trimBase[i] = trim >> 2;
            trimExt[i] = (trim & 0x03);
          }
          else {
            trimBase[i] = trim;
          }
        }
      }
    }

    virtual void afterImport()
    {
      for (int i=0; i<CPN_MAX_STICKS+MAX_AUX_TRIMS(board); i++) {
        if ((IS_HORUS_OR_TARANIS(board) && version >= 216) || (IS_ARM(board) && version >= 218)) {
          if (trimMode[i] == TRIM_MODE_NONE) {
            phase.trimMode[i] = -1;
          }
          else {
            phase.trimMode[i] = trimMode[i] % 2;
            phase.trimRef[i] = trimMode[i] / 2;
          }
        }
        else {
          if (phase.swtch == RawSwitch(SWITCH_TYPE_NONE) && index > 0) {
            phase.trimRef[i] = 0;
            phase.trimMode[i] = 0;
            phase.trim[i] = 0;
          }
          else {
            int trim;
            if (board == BOARD_STOCK || (board == BOARD_M128 && version >= 215))
              trim = ((trimBase[i]) << 2) + (trimExt[i] & 0x03);
            else
              trim = trimBase[i];
            if (trim > 500) {
              phase.trimRef[i] = trim - 501;
              if (phase.trimRef[i] >= index)
                phase.trimRef[i] += 1;
              phase.trimMode[i] = 0;
              phase.trim[i] = 0;
            }
            else {
              phase.trimRef[i] = index/*own trim*/;
              phase.trimMode[i] = 0;
              phase.trim[i] = trim;
            }
          }
        }
      }
      qCDebug(eepromImport) << QString("imported %1: '%2'").arg(internalField.getName()).arg(phase.name);
    }

  protected:
    StructField internalField;
    FlightModeData & phase;
    int index;
    Board::Type board;
    unsigned int version;
    int rotencCount;
    int trimBase[CPN_MAX_TRIMS];
    int trimExt[CPN_MAX_TRIMS];
    unsigned int trimMode[CPN_MAX_TRIMS];
};

class MixField: public TransformedField {
  public:
    MixField(DataField * parent, MixData & mix, Board::Type board, unsigned int version, ModelData * model):
      TransformedField(parent, internalField),
      internalField(this, "Mix"),
      mix(mix),
      board(board),
      version(version),
      model(model)
    {
      if (IS_ARM(board) && version >= 218) {
        internalField.Append(new SignedField<11>(this, _weight));
        internalField.Append(new UnsignedField<5>(this, _destCh));
        internalField.Append(new SourceField<10>(this, mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new UnsignedField<1>(this, (unsigned int &)mix.carryTrim));
        internalField.Append(new UnsignedField<2>(this, mix.mixWarn));
        internalField.Append(new UnsignedField<2>(this, (unsigned int &)mix.mltpx));
        internalField.Append(new SpareBitsField<1>(this));
        internalField.Append(new SignedField<14>(this, _offset));
        internalField.Append(new SwitchField<9>(this, mix.swtch, board, version));
        internalField.Append(new UnsignedField<9>(this, mix.flightModes));
        internalField.Append(new CurveReferenceField(this, mix.curve, board, version));
        internalField.Append(new UnsignedField<8>(this, mix.delayUp));
        internalField.Append(new UnsignedField<8>(this, mix.delayDown));
        internalField.Append(new UnsignedField<8>(this, mix.speedUp));
        internalField.Append(new UnsignedField<8>(this, mix.speedDown));
        if (IS_HORUS(board))
          internalField.Append(new ZCharField<6>(this, mix.name));
        else if (HAS_LARGE_LCD(board))
          internalField.Append(new ZCharField<8>(this, mix.name));
        else
          internalField.Append(new ZCharField<6>(this, mix.name));
      }
      else if (IS_TARANIS(board) && version == 217) {
        internalField.Append(new UnsignedField<8>(this, _destCh));
        internalField.Append(new UnsignedField<9>(this, mix.flightModes));
        internalField.Append(new UnsignedField<2>(this, (unsigned int &)mix.mltpx));
        internalField.Append(new UnsignedField<1>(this, (unsigned int &)mix.carryTrim));
        internalField.Append(new UnsignedField<4>(this, mix.mixWarn));
        internalField.Append(new SignedField<16>(this, _weight));
        internalField.Append(new SourceField<10>(this, mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<14>(this, _offset));
        internalField.Append(new SwitchField<8>(this, mix.swtch, board, version));
        internalField.Append(new CurveReferenceField(this, mix.curve, board, version));
        internalField.Append(new UnsignedField<8>(this, mix.delayUp));
        internalField.Append(new UnsignedField<8>(this, mix.delayDown));
        internalField.Append(new UnsignedField<8>(this, mix.speedUp));
        internalField.Append(new UnsignedField<8>(this, mix.speedDown));
        internalField.Append(new ZCharField<8>(this, mix.name));
      }
      else if (IS_ARM(board) && version == 217) {
        internalField.Append(new UnsignedField<5>(this, _destCh));
        internalField.Append(new UnsignedField<3>(this, mix.mixWarn));
        internalField.Append(new UnsignedField<9>(this, mix.flightModes));
        internalField.Append(new BoolField<1>(this, _curveMode));
        internalField.Append(new BoolField<1>(this, mix.noExpo));
        internalField.Append(new SignedField<3>(this, mix.carryTrim));
        internalField.Append(new UnsignedField<2>(this, (unsigned int &)mix.mltpx));
        internalField.Append(new SignedField<16>(this, _weight));
        internalField.Append(new SwitchField<8>(this, mix.swtch, board, version));
        internalField.Append(new SignedField<8>(this, _curveParam));
        internalField.Append(new UnsignedField<8>(this, mix.delayUp));
        internalField.Append(new UnsignedField<8>(this, mix.delayDown));
        internalField.Append(new UnsignedField<8>(this, mix.speedUp));
        internalField.Append(new UnsignedField<8>(this, mix.speedDown));
        internalField.Append(new SourceField<8>(this, mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<16>(this, _offset));
        internalField.Append(new ZCharField<6>(this, mix.name));
      }
      else if (IS_TARANIS(board) && version == 216) {
        internalField.Append(new UnsignedField<8>(this, _destCh));
        internalField.Append(new UnsignedField<16>(this, mix.flightModes));
        internalField.Append(new UnsignedField<2>(this, (unsigned int &)mix.mltpx));
        internalField.Append(new UnsignedField<1>(this, (unsigned int &)mix.carryTrim));
        internalField.Append(new SpareBitsField<5>(this));
        internalField.Append(new SignedField<16>(this, _weight));
        internalField.Append(new SwitchField<8>(this, mix.swtch, board, version));
        internalField.Append(new CurveReferenceField(this, mix.curve, board, version));
        internalField.Append(new UnsignedField<4>(this, mix.mixWarn));
        internalField.Append(new SpareBitsField<4>(this));
        internalField.Append(new UnsignedField<8>(this, mix.delayUp));
        internalField.Append(new UnsignedField<8>(this, mix.delayDown));
        internalField.Append(new UnsignedField<8>(this, mix.speedUp));
        internalField.Append(new UnsignedField<8>(this, mix.speedDown));
        internalField.Append(new SourceField<8>(this, mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<16>(this, _offset));
        internalField.Append(new ZCharField<8>(this, mix.name));
        internalField.Append(new SpareBitsField<8>(this));
      }
      else if (IS_ARM(board) && version == 216) {
        internalField.Append(new UnsignedField<5>(this, _destCh));
        internalField.Append(new UnsignedField<3>(this, mix.mixWarn));
        internalField.Append(new UnsignedField<16>(this, mix.flightModes));
        internalField.Append(new BoolField<1>(this, _curveMode));
        internalField.Append(new BoolField<1>(this, mix.noExpo));
        internalField.Append(new SignedField<3>(this, mix.carryTrim));
        internalField.Append(new UnsignedField<2>(this, (unsigned int &)mix.mltpx));
        internalField.Append(new SpareBitsField<1>(this));
        internalField.Append(new SignedField<16>(this, _weight));
        internalField.Append(new SwitchField<8>(this, mix.swtch, board, version));
        internalField.Append(new SignedField<8>(this, _curveParam));
        internalField.Append(new UnsignedField<8>(this, mix.delayUp));
        internalField.Append(new UnsignedField<8>(this, mix.delayDown));
        internalField.Append(new UnsignedField<8>(this, mix.speedUp));
        internalField.Append(new UnsignedField<8>(this, mix.speedDown));
        internalField.Append(new SourceField<8>(this, mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<16>(this, _offset));
        internalField.Append(new ZCharField<6>(this, mix.name));
      }
      else if (IS_ARM(board)) {
        internalField.Append(new UnsignedField<8>(this, _destCh));
        internalField.Append(new UnsignedField<16>(this, mix.flightModes));
        internalField.Append(new BoolField<1>(this, _curveMode));
        internalField.Append(new BoolField<1>(this, mix.noExpo));
        internalField.Append(new SignedField<3>(this, mix.carryTrim));
        internalField.Append(new UnsignedField<2>(this, (unsigned int &)mix.mltpx));
        if (version >= 214)
          internalField.Append(new SpareBitsField<1>(this));
        else
          internalField.Append(new UnsignedField<1>(this, _offsetMode));
        internalField.Append(new SignedField<16>(this, _weight));
        internalField.Append(new SwitchField<8>(this, mix.swtch, board, version));
        internalField.Append(new SignedField<8>(this, _curveParam));
        if (version >= 214) {
          internalField.Append(new UnsignedField<4>(this, mix.mixWarn));
          internalField.Append(new SpareBitsField<4>(this));
        }
        else {
          internalField.Append(new UnsignedField<8>(this, mix.mixWarn));
        }
        internalField.Append(new UnsignedField<8>(this, mix.delayUp));
        internalField.Append(new UnsignedField<8>(this, mix.delayDown));
        internalField.Append(new UnsignedField<8>(this, mix.speedUp));
        internalField.Append(new UnsignedField<8>(this, mix.speedDown));
        internalField.Append(new SourceField<8>(this, mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        if (version >= 214)
          internalField.Append(new SignedField<16>(this, _offset));
        else
          internalField.Append(new SignedField<8>(this, _offset));
        if (HAS_LARGE_LCD(board)) {
          internalField.Append(new ZCharField<8>(this, mix.name));
          internalField.Append(new SpareBitsField<16>(this));
        }
        else {
          internalField.Append(new ZCharField<6>(this, mix.name));
        }
      }
      else if (IS_DBLRAM(board, version) && IS_AFTER_RELEASE_23_MARCH_2013(board, version)) {
        internalField.Append(new UnsignedField<4>(this, _destCh));
        internalField.Append(new BoolField<1>(this, _curveMode));
        internalField.Append(new BoolField<1>(this, mix.noExpo));
        internalField.Append(new UnsignedField<1>(this, _weightMode));
        internalField.Append(new UnsignedField<1>(this, _offsetMode));
        internalField.Append(new SourceField<8>(this, mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<8>(this, _weight));
        internalField.Append(new SwitchField<8>(this, mix.swtch, board, version));
        internalField.Append(new UnsignedField<8>(this, mix.flightModes));
        internalField.Append(new UnsignedField<2>(this, (unsigned int &)mix.mltpx));
        internalField.Append(new SignedField<3>(this, mix.carryTrim));
        internalField.Append(new UnsignedField<2>(this, mix.mixWarn));
        internalField.Append(new SpareBitsField<1>(this));
        internalField.Append(new UnsignedField<4>(this, mix.delayUp));
        internalField.Append(new UnsignedField<4>(this, mix.delayDown));
        internalField.Append(new UnsignedField<4>(this, mix.speedUp));
        internalField.Append(new UnsignedField<4>(this, mix.speedDown));
        internalField.Append(new SignedField<8>(this, _curveParam));
        internalField.Append(new SignedField<8>(this, _offset));
      }
      else {
        internalField.Append(new UnsignedField<4>(this, _destCh));
        internalField.Append(new BoolField<1>(this, _curveMode));
        internalField.Append(new BoolField<1>(this, mix.noExpo));
        internalField.Append(new UnsignedField<1>(this, _weightMode));
        internalField.Append(new UnsignedField<1>(this, _offsetMode));
        internalField.Append(new SignedField<8>(this, _weight));
        internalField.Append(new SwitchField<6>(this, mix.swtch, board, version));
        internalField.Append(new UnsignedField<2>(this, (unsigned int &)mix.mltpx));
        internalField.Append(new UnsignedField<5>(this, mix.flightModes));
        internalField.Append(new SignedField<3>(this, mix.carryTrim));
        internalField.Append(new SourceField<6>(this, mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new UnsignedField<2>(this, mix.mixWarn));
        internalField.Append(new UnsignedField<4>(this, mix.delayUp));
        internalField.Append(new UnsignedField<4>(this, mix.delayDown));
        internalField.Append(new UnsignedField<4>(this, mix.speedUp));
        internalField.Append(new UnsignedField<4>(this, mix.speedDown));
        internalField.Append(new SignedField<8>(this, _curveParam));
        internalField.Append(new SignedField<8>(this, _offset));
      }
    }

    virtual void beforeExport()
    {
      if (mix.destCh && mix.srcRaw.type != SOURCE_TYPE_NONE) {
        _destCh = mix.destCh - 1;
        if (mix.curve.type == CurveReference::CURVE_REF_CUSTOM) {
          _curveMode = true;
          if (mix.curve.value > 0) {
            _curveParam = 6 + mix.curve.value;
          }
          else {
            _curveParam = mix.curve.value;
          }
        }
        else if (mix.curve.type == CurveReference::CURVE_REF_FUNC) {
          _curveMode = true;
          _curveParam = mix.curve.value;
        }
        else if (mix.curve.type == CurveReference::CURVE_REF_DIFF) {
          _curveMode = 0;
          _curveParam = smallGvarImport(mix.curve.value);
        }
      }
      else {
        mix.clear();
        _destCh = 0;
        _curveMode = 0;
        _curveParam = 0;
      }

      if (IS_ARM(board)) {
        exportGvarParam(mix.weight, _weight, version);
        if (version >= 214)
          exportGvarParam(mix.sOffset, _offset, version);
        else
          splitGvarParam(mix.sOffset, _offset, _offsetMode, board, version);
      }
      else {
        splitGvarParam(mix.weight, _weight, _weightMode, board, version);
        splitGvarParam(mix.sOffset, _offset, _offsetMode, board, version);
      }
    }

    virtual void afterImport()
    {
      if (IS_TARANIS(board) && version < 216) {
        if (mix.srcRaw.type == SOURCE_TYPE_STICK && mix.srcRaw.index < CPN_MAX_STICKS) {
          if (!mix.noExpo) {
            mix.srcRaw = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, mix.srcRaw.index);
          }
        }
      }

      if (mix.srcRaw.type != SOURCE_TYPE_NONE) {
        mix.destCh = _destCh + 1;
        if (!IS_ARM(board) || (!IS_STM32(board) && version < 218) || version < 216) {
          if (!_curveMode)
            mix.curve = CurveReference(CurveReference::CURVE_REF_DIFF, smallGvarExport(_curveParam));
          else if (_curveParam > 6)
            mix.curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, _curveParam-6);
          else if (_curveParam < 0)
            mix.curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, _curveParam);
          else
            mix.curve = CurveReference(CurveReference::CURVE_REF_FUNC, _curveParam);
        }
      }

      if (IS_ARM(board)) {
        importGvarParam(mix.weight, _weight, version);
        if (version >= 214)
          importGvarParam(mix.sOffset, _offset, version);
        else
          concatGvarParam(mix.sOffset, _offset, _offsetMode, board, version);
      }
      else {
        concatGvarParam(mix.weight, _weight, _weightMode, board, version);
        concatGvarParam(mix.sOffset, _offset, _offsetMode, board, version);
      }

      if (IS_TARANIS(board) && version < 216) {
        if (mix.sOffset >= -500 && mix.sOffset <= 500 && mix.weight >= -500 && mix.weight <= 500) {
          mix.sOffset = divRoundClosest(mix.sOffset * mix.weight, 100);
        }
        if (mix.carryTrim < 0) mix.carryTrim = 0;
      }
      qCDebug(eepromImport) << QString("imported %1: ch %2, name '%3'").arg(internalField.getName()).arg(mix.destCh).arg(mix.name);
    }

  protected:
    StructField internalField;
    MixData & mix;
    Board::Type board;
    unsigned int version;
    ModelData * model;
    unsigned int _destCh;
    bool _curveMode;
    int _curveParam;
    int _weight;
    int _offset;
    unsigned int _weightMode;
    unsigned int _offsetMode;
};

class InputField: public TransformedField {
  public:
    InputField(DataField * parent, ExpoData & expo, Board::Type board, unsigned int version):
      TransformedField(parent, internalField),
      internalField(this, "Input"),
      expo(expo),
      board(board),
      version(version)
    {
      if (IS_ARM(board) && version >= 218) {
        internalField.Append(new UnsignedField<2>(this, expo.mode, "Mode"));
        internalField.Append(new UnsignedField<14>(this, expo.scale, "Scale"));
        internalField.Append(new SourceField<10>(this, expo.srcRaw, board, version, 0));
        internalField.Append(new SignedField<6>(this, expo.carryTrim, "CarryTrim"));
        internalField.Append(new UnsignedField<5>(this, expo.chn, "Channel"));
        internalField.Append(new SwitchField<9>(this, expo.swtch, board, version));
        internalField.Append(new UnsignedField<9>(this, expo.flightModes));
        internalField.Append(new SignedField<8>(this, _weight, "Weight"));
        internalField.Append(new SpareBitsField<1>(this));
        if (IS_HORUS(board))
          internalField.Append(new ZCharField<6>(this, expo.name));
        else if (HAS_LARGE_LCD(board))
          internalField.Append(new ZCharField<8>(this, expo.name));
        else
          internalField.Append(new ZCharField<6>(this, expo.name));
        internalField.Append(new SignedField<8>(this, _offset, "Offset"));
        internalField.Append(new CurveReferenceField(this, expo.curve, board, version));
      }
      else if (IS_TARANIS(board) && version >= 217) {
        internalField.Append(new SourceField<10>(this, expo.srcRaw, board, version, 0));
        internalField.Append(new UnsignedField<14>(this, expo.scale, "Scale"));
        internalField.Append(new UnsignedField<8>(this, expo.chn, "Channel"));
        internalField.Append(new SwitchField<8>(this, expo.swtch, board, version));
        internalField.Append(new UnsignedField<16>(this, expo.flightModes));
        internalField.Append(new SignedField<8>(this, _weight, "Weight"));
        internalField.Append(new SignedField<6>(this, expo.carryTrim, "CarryTrim"));
        internalField.Append(new UnsignedField<2>(this, expo.mode, "Mode"));
        internalField.Append(new ZCharField<8>(this, expo.name));
        internalField.Append(new SignedField<8>(this, _offset, "Offset"));
        internalField.Append(new CurveReferenceField(this, expo.curve, board, version));
      }
      else if (IS_ARM(board) && version >= 217) {
        internalField.Append(new UnsignedField<2>(this, expo.mode, "Mode"));
        internalField.Append(new UnsignedField<3>(this, expo.chn, "Channel"));
        internalField.Append(new BoolField<2>(this, _curveMode));
        internalField.Append(new UnsignedField<9>(this, expo.flightModes, "Phases"));
        internalField.Append(new SwitchField<8>(this, expo.swtch, board, version));
        internalField.Append(new SignedField<8>(this, _weight, "Weight"));
        internalField.Append(new ZCharField<6>(this, expo.name));
        internalField.Append(new SignedField<8>(this, _curveParam));
      }
      else if (IS_TARANIS(board) && version >= 216) {
        internalField.Append(new SourceField<8>(this, expo.srcRaw, board, version, 0));
        internalField.Append(new UnsignedField<16>(this, expo.scale, "Scale"));
        internalField.Append(new UnsignedField<8>(this, expo.chn, "Channel"));
        internalField.Append(new SwitchField<8>(this, expo.swtch, board, version));
        internalField.Append(new UnsignedField<16>(this, expo.flightModes));
        internalField.Append(new SignedField<8>(this, _weight, "Weight"));
        internalField.Append(new SignedField<6>(this, expo.carryTrim, "CarryTrim"));
        internalField.Append(new UnsignedField<2>(this, expo.mode, "Mode"));
        internalField.Append(new ZCharField<8>(this, expo.name));
        internalField.Append(new SignedField<8>(this, _offset, "Offset"));
        internalField.Append(new CurveReferenceField(this, expo.curve, board, version));
        internalField.Append(new SpareBitsField<8>(this));
      }
      else if (IS_ARM(board) && version >= 216) {
        internalField.Append(new UnsignedField<2>(this, expo.mode, "Mode"));
        internalField.Append(new UnsignedField<4>(this, expo.chn, "Channel"));
        internalField.Append(new BoolField<2>(this, _curveMode));
        internalField.Append(new SwitchField<8>(this, expo.swtch, board, version));
        internalField.Append(new UnsignedField<16>(this, expo.flightModes, "Phases"));
        internalField.Append(new SignedField<8>(this, _weight, "Weight"));
        internalField.Append(new ZCharField<6>(this, expo.name));
        internalField.Append(new SignedField<8>(this, _curveParam));
      }
      else if (IS_ARM(board)) {
        internalField.Append(new UnsignedField<8>(this, expo.mode, "Mode"));
        internalField.Append(new UnsignedField<8>(this, expo.chn, "Channel"));
        internalField.Append(new SwitchField<8>(this, expo.swtch, board, version));
        internalField.Append(new UnsignedField<16>(this, expo.flightModes, "Phases"));
        internalField.Append(new SignedField<8>(this, _weight, "Weight"));
        internalField.Append(new BoolField<8>(this, _curveMode));
        if (HAS_LARGE_LCD(board)) {
          internalField.Append(new ZCharField<8>(this, expo.name));
          internalField.Append(new SpareBitsField<16>(this));
        }
        else {
          internalField.Append(new ZCharField<6>(this, expo.name));
        }
        internalField.Append(new SignedField<8>(this, _curveParam));
      }
      else if (IS_DBLRAM(board, version) && IS_AFTER_RELEASE_23_MARCH_2013(board, version)) {
        internalField.Append(new UnsignedField<2>(this, expo.mode));
        internalField.Append(new UnsignedField<2>(this, expo.chn));
        internalField.Append(new BoolField<1>(this, _curveMode));
        internalField.Append(new SpareBitsField<3>(this));
        internalField.Append(new UnsignedField<8>(this, expo.flightModes));
        internalField.Append(new SwitchField<8>(this, expo.swtch, board, version));
        internalField.Append(new SignedField<8>(this, _weight));
        internalField.Append(new SignedField<8>(this, _curveParam));
      }
      else {
        internalField.Append(new UnsignedField<2>(this, expo.mode));
        internalField.Append(new SwitchField<6>(this, expo.swtch, board, version));
        internalField.Append(new UnsignedField<2>(this, expo.chn));
        internalField.Append(new UnsignedField<5>(this, expo.flightModes));
        internalField.Append(new BoolField<1>(this, _curveMode));
        internalField.Append(new SignedField<8>(this, _weight));
        internalField.Append(new SignedField<8>(this, _curveParam));
      }
    }

    virtual void beforeExport()
    {
      _weight = smallGvarImport(expo.weight);

      if ((IS_HORUS_OR_TARANIS(board) && version >= 216) || (IS_ARM(board) && version >= 218)) {
        _offset = smallGvarImport(expo.offset);
      }

      if (!IS_ARM(board) || (!IS_TARANIS(board) && version < 218) || version < 216) {
        if (expo.curve.type==CurveReference::CURVE_REF_FUNC && expo.curve.value) {
          _curveMode = true;
          _curveParam = expo.curve.value;
        }
        else if (expo.curve.type==CurveReference::CURVE_REF_CUSTOM && expo.curve.value) {
          _curveMode = true;
          _curveParam = expo.curve.value+6;
        }
        else {
          _curveMode = false;
          _curveParam = smallGvarImport(expo.curve.value);
        }
      }
    }

    virtual void afterImport()
    {
      if ((IS_STM32(board) && version < 216 )|| (!IS_STM32(board) && expo.mode)) {
        expo.srcRaw = RawSource(SOURCE_TYPE_STICK, expo.chn);
      }

      expo.weight = smallGvarExport(_weight);

      if ((IS_STM32(board) && version >= 216) || (IS_ARM(board) && version >= 218)) {
        expo.offset = smallGvarExport(_offset);
      }

      if (!IS_ARM(board) || (!IS_TARANIS(board) && version < 218) || version < 216) {
        if (!_curveMode)
          expo.curve = CurveReference(CurveReference::CURVE_REF_EXPO, smallGvarExport(_curveParam));
        else if (_curveParam > 6)
          expo.curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, _curveParam-6);
        else
          expo.curve = CurveReference(CurveReference::CURVE_REF_FUNC, _curveParam);
      }
      qCDebug(eepromImport) << QString("imported %1: ch %2 name '%3'").arg(internalField.getName()).arg(expo.chn).arg(expo.name);
    }

  protected:
    StructField internalField;
    ExpoData & expo;
    Board::Type board;
    unsigned int version;
    bool _curveMode;
    int  _weight;
    int  _offset;
    int  _curveParam;
};

class LimitField: public StructField {
  public:
    template <int shift, int GV1>
    static int exportLimitValue(int value)
    {
      if (value > 10000) {
        return -GV1 + value - 10001;
      }
      if (value < -10000) {
        return GV1 + value + 10000;
      }
      return value + shift;
    }

    template <int shift, int GV1>
    static int importLimitValue(int value)
    {
      if (value >= GV1) {
        return 10001 + value - GV1;
      }
      if (value >= GV1-9) {
        return -10000 + value - GV1;
      }
      if (value < -GV1) {
        return -10000 + value + GV1 + 1;
      }
      if (value < -GV1+9) {
        return 10000 + value + GV1 + 1;
      }
      return value - shift;
    }

    LimitField(DataField * parent, LimitData & limit, Board::Type board, unsigned int version):
      StructField(parent, "Limit")
    {
      if (IS_ARM(board) && version >= 218) {
        Append(new ConversionField< SignedField<11> >(this, limit.min, exportLimitValue<1000, 1024>, importLimitValue<1000, 1024>));
        Append(new ConversionField< SignedField<11> >(this, limit.max, exportLimitValue<-1000, 1024>, importLimitValue<-1000, 1024>));
        Append(new SignedField<10>(this, limit.ppmCenter));
        Append(new ConversionField< SignedField<11> >(this, limit.offset, exportLimitValue<0, 1024>, importLimitValue<0, 1024>));
        Append(new BoolField<1>(this, limit.symetrical));
        Append(new BoolField<1>(this, limit.revert));
        Append(new SpareBitsField<3>(this));
        Append(new SignedField<8>(this, limit.curve.value));
        if (HAS_LARGE_LCD(board))
          Append(new ZCharField<6>(this, limit.name));
        else
          Append(new ZCharField<4>(this, limit.name));
      }
      else if (IS_TARANIS(board) && version >= 217) {
        Append(new ConversionField< SignedField<11> >(this, limit.min, exportLimitValue<1000, 1024>, importLimitValue<1000, 1024>));
        Append(new ConversionField< SignedField<11> >(this, limit.max, exportLimitValue<-1000, 1024>, importLimitValue<-1000, 1024>));
        Append(new SignedField<10>(this, limit.ppmCenter));
        Append(new ConversionField< SignedField<11> >(this, limit.offset, exportLimitValue<0, 1024>, importLimitValue<0, 1024>));
        Append(new BoolField<1>(this, limit.symetrical));
        Append(new BoolField<1>(this, limit.revert));
        Append(new SpareBitsField<3>(this));
        Append(new SignedField<8>(this, limit.curve.value));
        Append(new ZCharField<6>(this, limit.name));
      }
      else {
        if (IS_TARANIS(board) && version >= 216) {
          Append(new ConversionField< SignedField<16> >(this, limit.min, exportLimitValue<1000, 4096>, importLimitValue<1000, 4096>));
          Append(new ConversionField< SignedField<16> >(this, limit.max, exportLimitValue<-1000, 4096>, importLimitValue<-1000, 4096>));
          Append(new SignedField<8>(this, limit.ppmCenter));
          Append(new ConversionField< SignedField<14> >(this, limit.offset, exportLimitValue<0, 4096>, importLimitValue<0, 4096>));
        }
        else {
          Append(new ConversionField< SignedField<8> >(this, limit.min, +100, 10));
          Append(new ConversionField< SignedField<8> >(this, limit.max, -100, 10));
          Append(new SignedField<8>(this, limit.ppmCenter));
          Append(new SignedField<14>(this, limit.offset));
        }
        Append(new BoolField<1>(this, limit.symetrical));
        Append(new BoolField<1>(this, limit.revert));
        if (HAS_LARGE_LCD(board)) {
          Append(new ZCharField<6>(this, limit.name));
        }
        if (IS_TARANIS(board) && version >= 216) {
          Append(new SignedField<8>(this, limit.curve.value));
        }
      }
    }
};

class CurvesField: public TransformedField {
  public:
    CurvesField(DataField * parent, CurveData * curves, Board::Type board, unsigned int version):
      TransformedField(parent, internalField),
      internalField(this, "Curves"),
      curves(curves),
      board(board),
      version(version),
      maxCurves(MAX_CURVES(board, version)),
      maxPoints(IS_ARM(board) ? 512 : 112-8)
    {
      for (int i=0; i<maxCurves; i++) {
        if (IS_ARM(board) && version >= 218) {
          internalField.Append(new UnsignedField<1>(this, (unsigned int &)curves[i].type));
          internalField.Append(new BoolField<1>(this, curves[i].smooth));
          internalField.Append(new ConversionField< SignedField<6> >(this, curves[i].count, -5));
          internalField.Append(new ZCharField<3>(this, curves[i].name));
        }
        else if (IS_TARANIS(board) && version >= 216) {
          internalField.Append(new UnsignedField<3>(this, (unsigned int &)curves[i].type));
          internalField.Append(new BoolField<1>(this, curves[i].smooth));
          internalField.Append(new SpareBitsField<4>(this));
          internalField.Append(new ConversionField< SignedField<8> >(this, curves[i].count, -5));
        }
        else if (IS_ARM(board)) {
          internalField.Append(new SignedField<16>(this, _curves[i]));
        }
        else {
          internalField.Append(new SignedField<8>(this, _curves[i]));
        }
      }

      for (int i=0; i<maxPoints; i++) {
        internalField.Append(new SignedField<8>(this, _points[i]));
      }
    }

    virtual void beforeExport()
    {
      memset(_points, 0, sizeof(_points));

      int * cur = &_points[0];
      int offset = 0;

      for (int i=0; i<maxCurves; i++) {
        CurveData *curve = &curves[i];
        if (IS_STM32(board) && version >= 216) {
          offset += (curve->type == CurveData::CURVE_TYPE_CUSTOM ? curve->count * 2 - 2 : curve->count);
          if (offset > maxPoints) {
            setError(DataField::tr("OpenTX only accepts %1 points in all curves").arg(maxPoints));
            break;
          }
        }
        else {
          offset += (curve->type == CurveData::CURVE_TYPE_CUSTOM ? curve->count * 2 - 2 : curve->count) - 5;
          if (offset > maxPoints - 5 * maxCurves) {
            setError(DataField::tr("OpenTx only accepts %1 points in all curves").arg(maxPoints));
            break;
          }
          _curves[i] = offset;
        }
        for (int j=0; j<curve->count; j++) {
          *cur++ = curve->points[j].y;
        }
        if (curve->type == CurveData::CURVE_TYPE_CUSTOM) {
          for (int j=1; j<curve->count-1; j++) {
            *cur++ = curve->points[j].x;
          }
        }
      }
    }

    virtual void afterImport()
    {
      int * cur = &_points[0];

      for (int i=0; i<maxCurves; i++) {
        CurveData * curve = &curves[i];
        if (!IS_ARM(board) || (!IS_STM32(board) && version < 218) || version < 216) {
          int * next = &_points[5*(i+1) + _curves[i]];
          int size = next - cur;
          if (size % 2 == 0) {
            curve->count = (size / 2) + 1;
            curve->type = CurveData::CURVE_TYPE_CUSTOM;
          }
          else {
            curve->count = size;
            curve->type = CurveData::CURVE_TYPE_STANDARD;
          }
        }

        if (curve->count > 17) {
          qDebug() << "Wrong points count" << curve->count;
          curve->count = 0;
          for (int j=0; j<maxCurves; j++) {
            CurveData *curve = &curves[j];
            curve->clear(5);
          }
          return;
        }

        for (int j=0; j<curve->count; j++) {
          curve->points[j].y = *cur++;
        }

        if (curve->type == CurveData::CURVE_TYPE_CUSTOM) {
          curve->points[0].x = -100;
          for (int j=1; j<curve->count-1; j++)
            curve->points[j].x = *cur++;
          curve->points[curve->count-1].x = +100;
        }
        else {
          for (int j=0; j<curve->count; j++)
            curve->points[j].x = -100 + (200*i) / (curve->count-1);
        }
        qCDebug(eepromImport) << QString("imported curve: %3 points").arg(curve->count);
      }
    }

  protected:
    StructField internalField;
    CurveData * curves;
    Board::Type board;
    unsigned int version;
    int maxCurves;
    int maxPoints;
    int _curves[CPN_MAX_CURVES];
    int _points[CPN_MAX_CURVES*CPN_MAX_POINTS*2];
};

class LogicalSwitchesFunctionsTable: public ConversionTable {

  public:
    LogicalSwitchesFunctionsTable(Board::Type board, unsigned int version)
    {
      int val=0;
      bool afterrelease21March2013 = IS_AFTER_RELEASE_21_MARCH_2013(board, version);
      addConversion(LS_FN_OFF, val++);
      if (IS_ARM(board) && version >= 216)
        addConversion(LS_FN_VEQUAL, val++);
      if (afterrelease21March2013)
        addConversion(LS_FN_VALMOSTEQUAL, val++);
      addConversion(LS_FN_VPOS, val++);
      addConversion(LS_FN_VNEG, val++);
      if (IS_ARM(board) && version >= 216) val++; // later RANGE
      addConversion(LS_FN_APOS, val++);
      addConversion(LS_FN_ANEG, val++);
      addConversion(LS_FN_AND, val++);
      addConversion(LS_FN_OR, val++);
      addConversion(LS_FN_XOR, val++);
      if (IS_ARM(board) && version >= 216) addConversion(LS_FN_EDGE, val++);
      addConversion(LS_FN_EQUAL, val++);
      if (!afterrelease21March2013)
        addConversion(LS_FN_NEQUAL, val++);
      addConversion(LS_FN_GREATER, val++);
      addConversion(LS_FN_LESS, val++);
      if (!afterrelease21March2013) {
        addConversion(LS_FN_EGREATER, val++);
        addConversion(LS_FN_ELESS, val++);
      }
      addConversion(LS_FN_DPOS, val++);
      addConversion(LS_FN_DAPOS, val++);
      addConversion(LS_FN_TIMER, val++);
      if (version >= 216)
        addConversion(LS_FN_STICKY, val++);
    }
};

class AndSwitchesConversionTable: public ConversionTable {

  public:
    AndSwitchesConversionTable(Board::Type board, unsigned int version)
    {
      int val=0;
      addConversion(RawSwitch(SWITCH_TYPE_NONE), val++);

      if (IS_STM32(board)) {
        for (int i=1; i<=MAX_SWITCHES_POSITION(board, version); i++) {
          int s = switchIndex(i, board, version);
          if (IS_TARANIS(board) && version < 217) {
            // SF positions 16 and 17 => 16 and 18
            // SH positions 21 and 22 => 22 and 24
            if (s == 17 || s == 23) {
              continue;
            }
          }
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, -s), -val);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val++);
        }
        for (int i=1; i<=MAX_LOGICAL_SWITCHES(board, version); i++) {
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, -i), -val);
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
        }
      }
      else if (IS_SKY9X(board)) {
        for (int i=1; i<=8; i++) {
          int s = switchIndex(i, board, version);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, -s), -val);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val++);
        }
        for (int i=1; i<=MAX_LOGICAL_SWITCHES(board, version); i++) {
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, -i), -val);
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
        }
      }
      else {
        for (int i=1; i<=9; i++) {
          int s = switchIndex(i, board, version);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val++);
        }
        for (int i=1; i<=7; i++) {
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
        }
      }
    }

    static ConversionTable * getInstance(Board::Type board, unsigned int version)
    {
      if (IS_ARM(board) && version >= 216)
        return new SwitchesConversionTable(board, version);
      else
        return new AndSwitchesConversionTable(board, version);

#if 0
      static std::list<Cache> internalCache;

      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache element = *it;
        if (element.board == board && element.version == version && element.flags == flags)
          return element.table;
      }

      Cache element(board, version, flags, new SwitchesConversionTable(board, version, flags));
      internalCache.push_back(element);
      return element.table;
#endif
    }


  protected:

    void addConversion(const RawSwitch & sw, const int b)
    {
      ConversionTable::addConversion(sw.toValue(), b);
    }
};

class LogicalSwitchField: public TransformedField {
  public:
    LogicalSwitchField(DataField * parent, LogicalSwitchData & csw, Board::Type board, unsigned int version, unsigned int variant, ModelData * model=NULL):
      TransformedField(parent, internalField),
      internalField(this, "LogicalSwitch"),
      csw(csw),
      board(board),
      version(version),
      variant(variant),
      model(model),
      functionsConversionTable(board, version),
      sourcesConversionTable(SourcesConversionTable::getInstance(board, version, variant, (version >= 214 || (!IS_ARM(board) && version >= 213)) ? 0 : FLAG_NOSWITCHES)),
      switchesConversionTable(SwitchesConversionTable::getInstance(board, version)),
      andswitchesConversionTable(AndSwitchesConversionTable::getInstance(board, version)),
      v1(0),
      v2(0),
      v3(0)
    {
      if (IS_ARM(board) && version >= 218) {
        internalField.Append(new ConversionField< UnsignedField<8> >(this, csw.func, &functionsConversionTable, "Function"));
        internalField.Append(new SignedField<10>(this, v1));
        internalField.Append(new SignedField<10>(this, v3));
        internalField.Append(new ConversionField< SignedField<9> >(this, (int &)csw.andsw, andswitchesConversionTable, "AND switch"));
        internalField.Append(new SpareBitsField<1>(this)); // TODO extra switch mode
        internalField.Append(new SpareBitsField<2>(this));
        internalField.Append(new SignedField<16>(this, v2));
      }
      else if (IS_ARM(board) && version >= 217) {
        internalField.Append(new ConversionField< UnsignedField<6> >(this, csw.func, &functionsConversionTable, "Function"));
        internalField.Append(new SignedField<10>(this, v1));
        internalField.Append(new SignedField<16>(this, v2));
        internalField.Append(new SignedField<16>(this, v3));
      }
      else if (IS_ARM(board) && version >= 216) {
        internalField.Append(new SignedField<8>(this, v1));
        internalField.Append(new SignedField<16>(this, v2));
        internalField.Append(new SignedField<16>(this, v3));
        internalField.Append(new ConversionField< UnsignedField<8> >(this, csw.func, &functionsConversionTable, "Function"));
      }
      else if (IS_ARM(board) && version >= 215) {
        internalField.Append(new SignedField<16>(this, v1));
        internalField.Append(new SignedField<16>(this, v2));
        internalField.Append(new ConversionField< UnsignedField<8> >(this, csw.func, &functionsConversionTable, "Function"));
      }
      else if (IS_ARM(board)) {
        internalField.Append(new SignedField<8>(this, v1));
        internalField.Append(new SignedField<8>(this, v2));
        internalField.Append(new ConversionField< UnsignedField<8> >(this, csw.func, &functionsConversionTable, "Function"));
      }
      else {
        internalField.Append(new SignedField<8>(this, v1));
        internalField.Append(new SignedField<8>(this, v2));
        if (version >= 213)
          internalField.Append(new ConversionField< UnsignedField<4> >(this, csw.func, &functionsConversionTable, "Function"));
        else
          internalField.Append(new ConversionField< UnsignedField<8> >(this, csw.func, &functionsConversionTable, "Function"));
      }

      if (IS_ARM(board)) {
        int scale = (version >= 216 ? 0 : 5);
        internalField.Append(new ConversionField< UnsignedField<8> >(this, csw.delay, 0, scale));
        internalField.Append(new ConversionField< UnsignedField<8> >(this, csw.duration, 0, scale));
        if (version >= 214 && version <= 217) {
          internalField.Append(new ConversionField< SignedField<8> >(this, (int &)csw.andsw, andswitchesConversionTable, "AND switch"));
        }
      }
      else {
        if (version >= 213) {
          internalField.Append(new ConversionField< UnsignedField<4> >(this, (unsigned int &)csw.andsw, andswitchesConversionTable, "AND switch"));
        }
      }
    }

    ~LogicalSwitchField()
    {
      delete andswitchesConversionTable;
    }

    virtual void beforeExport()
    {
      if (csw.func == LS_FN_TIMER) {
        v1 = csw.val1;
        v2 = csw.val2;
      }
      else if (csw.func == LS_FN_EDGE) {
        switchesConversionTable->exportValue(csw.val1, v1);
        v2 = csw.val2;
        v3 = csw.val3;
      }
      else if ((csw.func >= LS_FN_AND && csw.func <= LS_FN_XOR) || csw.func == LS_FN_STICKY) {
        switchesConversionTable->exportValue(csw.val1, v1);
        switchesConversionTable->exportValue(csw.val2, v2);
      }
      else if (csw.func >= LS_FN_EQUAL && csw.func <= LS_FN_ELESS) {
        sourcesConversionTable->exportValue(csw.val1, v1);
        sourcesConversionTable->exportValue(csw.val2, v2);
      }
      else if (csw.func != LS_FN_OFF) {
        sourcesConversionTable->exportValue(csw.val1, v1);
        v2 = csw.val2;
      }
    }

    virtual void afterImport()
    {
      if (csw.func == LS_FN_TIMER) {
        csw.val1 = v1;
        csw.val2 = v2;
      }
      else if (csw.func == LS_FN_EDGE) {
        switchesConversionTable->importValue(v1, csw.val1);
        csw.val2 = v2;
        csw.val3 = v3;
      }
      else if ((csw.func >= LS_FN_AND && csw.func <= LS_FN_XOR) || csw.func == LS_FN_STICKY) {
        switchesConversionTable->importValue(v1, csw.val1);
        switchesConversionTable->importValue(v2, csw.val2);
      }
      else if (csw.func >= LS_FN_EQUAL && csw.func <= LS_FN_ELESS) {
        if (IS_ARM(board)) {
          sourcesConversionTable->importValue((uint32_t)v1, csw.val1);
          sourcesConversionTable->importValue((uint32_t)v2, csw.val2);
        }
        else {
          sourcesConversionTable->importValue((uint8_t)v1, csw.val1);
          sourcesConversionTable->importValue((uint8_t)v2, csw.val2);
        }
        if (IS_TARANIS(board) && version < 216) {
          RawSource val1(csw.val1);
          if (val1.type == SOURCE_TYPE_STICK && val1.index < CPN_MAX_STICKS) {
            csw.val1 = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, val1.index).toValue();
          }
          RawSource val2(csw.val2);
          if (val2.type == SOURCE_TYPE_STICK && val2.index < CPN_MAX_STICKS) {
            csw.val2 = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, val2.index).toValue();
          }
        }
      }
      else if (csw.func != LS_FN_OFF) {
        if (IS_ARM(board) && version >= 217) {
          sourcesConversionTable->importValue((uint32_t)v1, csw.val1);
        }
        else {
          sourcesConversionTable->importValue((uint8_t)v1, csw.val1);
        }
        if (IS_TARANIS(board) && version < 216) {
          RawSource val1(csw.val1);
          if (val1.type == SOURCE_TYPE_STICK && val1.index < CPN_MAX_STICKS) {
            csw.val1 = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, val1.index).toValue();
          }
        }
        csw.val2 = v2;
        RawSource val1(csw.val1);
        if (IS_ARM(board) && version < 216 && val1.type == SOURCE_TYPE_TELEMETRY) {
          switch (val1.index) {
            case TELEMETRY_SOURCE_TIMER1:
            case TELEMETRY_SOURCE_TIMER2:
            case TELEMETRY_SOURCE_TIMER3:
              csw.val2 = (csw.val2 + 128) * 3;
              break;
            case TELEMETRY_SOURCE_ALT:
            case TELEMETRY_SOURCE_GPS_ALT:
            case TELEMETRY_SOURCE_ALT_MIN:
            case TELEMETRY_SOURCE_ALT_MAX:
              csw.val2 = (csw.val2 + 128) * 8 - 500;
              break;
            case TELEMETRY_SOURCE_RPM:
            case TELEMETRY_SOURCE_RPM_MAX:
              csw.val2 = (csw.val2 + 128) * 50;
              break;
            case TELEMETRY_SOURCE_T1:
            case TELEMETRY_SOURCE_T2:
            case TELEMETRY_SOURCE_T1_MAX:
            case TELEMETRY_SOURCE_T2_MAX:
              csw.val2 = (csw.val2 + 128) + 30;
              break;
            case TELEMETRY_SOURCE_CELL:
            case TELEMETRY_SOURCE_HDG:
              csw.val2 = (csw.val2 + 128) * 2;
              break;
            case TELEMETRY_SOURCE_DIST:
            case TELEMETRY_SOURCE_DIST_MAX:
              csw.val2 = (csw.val2 + 128) * 8;
              break;
            case TELEMETRY_SOURCE_CURRENT:
            case TELEMETRY_SOURCE_POWER:
              csw.val2 = (csw.val2 + 128) * 5;
              break;
            case TELEMETRY_SOURCE_CONSUMPTION:
              csw.val2 = (csw.val2 + 128) * 20;
              break;
            default:
              csw.val2 += 128;
              break;
          }
        }
      }
      qCDebug(eepromImport) << QString("imported %1: %2").arg(internalField.getName()).arg(csw.funcToString());
    }

  protected:
    StructField internalField;
    LogicalSwitchData & csw;
    Board::Type board;
    unsigned int version;
    unsigned int variant;
    ModelData * model;
    LogicalSwitchesFunctionsTable functionsConversionTable;
    SourcesConversionTable * sourcesConversionTable;
    SwitchesConversionTable * switchesConversionTable;
    ConversionTable * andswitchesConversionTable;
    int v1;
    int v2;
    int v3;
};

class CustomFunctionsConversionTable: public ConversionTable {

  public:
    CustomFunctionsConversionTable(Board::Type board, unsigned int version)
    {
      int val=0;

      if (version >= 216) {
        for (int i=0; i<MAX_CHANNELS(board, version); i++) {
          addConversion(i, val);
        }
        val++;
      }
      else if (IS_ARM(board) || version < 213) {
        for (int i=0; i<16; i++) {
          addConversion(val, val);
          val++;
        }
      }
      else {
        for (int i=0; i<16; i++) {
          addConversion(i, i / 4);
        }
        val+=4;
      }

      if (version >= 216) {
        addConversion(FuncTrainer, val);
        addConversion(FuncTrainerRUD, val);
        addConversion(FuncTrainerELE, val);
        addConversion(FuncTrainerTHR, val);
        addConversion(FuncTrainerAIL, val);
        val++;
      }
      else {
        addConversion(FuncTrainer, val++);
        addConversion(FuncTrainerRUD, val++);
        addConversion(FuncTrainerELE, val++);
        addConversion(FuncTrainerTHR, val++);
        addConversion(FuncTrainerAIL, val++);
      }

      addConversion(FuncInstantTrim, val++);

      if (version >= 216) {
        addConversion(FuncReset, val++);
        if (IS_ARM(board)) {
          addConversion(FuncSetTimer1, val);
          addConversion(FuncSetTimer2, val);
          addConversion(FuncSetTimer3, val);
          val++;
        }
        for (int i=0; i<MAX_GVARS(board, version); i++)
          addConversion(FuncAdjustGV1+i, val);
        val++;
        if (IS_ARM(board)) {
          addConversion(FuncVolume, val++);
          addConversion(FuncSetFailsafeInternalModule, val);
          addConversion(FuncSetFailsafeExternalModule, val++);
          addConversion(FuncRangeCheckInternalModule, val);
          addConversion(FuncRangeCheckExternalModule, val++);
          addConversion(FuncBindInternalModule, val);
          addConversion(FuncBindExternalModule, val++);
        }
        addConversion(FuncPlaySound, val++);
        addConversion(FuncPlayPrompt, val++);
        if (version >= 213 && !IS_ARM(board))
          addConversion(FuncPlayBoth, val++);
        addConversion(FuncPlayValue, val++);
        if (IS_ARM(board)) {
          addConversion(FuncReserve, val++);
          if (IS_STM32(board))
            addConversion(FuncPlayScript, val++);
          else
            addConversion(FuncReserve, val++);
          addConversion(FuncReserve, val++);
          addConversion(FuncBackgroundMusic, val++);
          addConversion(FuncBackgroundMusicPause, val++);
        }
        addConversion(FuncVario, val++);
        addConversion(FuncPlayHaptic, val++);
        if (IS_2560(board) || IS_ARM(board) )
          addConversion(FuncLogs, val++);
        addConversion(FuncBacklight, val++);
        if (IS_STM32(board))
          addConversion(FuncScreenshot, val++);
      }
      else {
        addConversion(FuncPlaySound, val++);
        if (!IS_HORUS_OR_TARANIS(board))
          addConversion(FuncPlayHaptic, val++);
        addConversion(FuncReset, val++);
        addConversion(FuncVario, val++);
        addConversion(FuncPlayPrompt, val++);
        if (version >= 213 && !IS_ARM(board))
          addConversion(FuncPlayBoth, val++);
        addConversion(FuncPlayValue, val++);
        if (IS_2560(board) || IS_ARM(board) )
          addConversion(FuncLogs, val++);
        if (IS_ARM(board))
          addConversion(FuncVolume, val++);
        addConversion(FuncBacklight, val++);
        if (IS_ARM(board)) {
          addConversion(FuncBackgroundMusic, val++);
          addConversion(FuncBackgroundMusicPause, val++);
        }
        for (int i=0; i<5; i++)
          addConversion(FuncAdjustGV1+i, val++);
      }
    }
};

template <int N>
class SwitchesWarningField: public TransformedField {
  public:
    SwitchesWarningField(DataField * parent, uint64_t & sw, Board::Type board, unsigned int version):
      TransformedField(parent, internalField),
      internalField(this, _sw, "SwitchesWarning"),
      sw(sw),
      board(board),
      version(version)
    {
    }

    virtual void beforeExport()
    {
      _sw = sw;
    }

    virtual void afterImport()
    {
      bool afterrelease21March2013 = IS_AFTER_RELEASE_21_MARCH_2013(board, version);
      if (afterrelease21March2013 && version < 216) {
        sw = _sw >> 1;
      }
      else if (!afterrelease21March2013) {
        sw = ((_sw & 0x30) >> 4) + ((_sw & 0x0E) << 1) + ((_sw & 0xC0) >> 1);
      }
      else {
        sw = _sw;
      }
      qCDebug(eepromImport) << QString("imported %1").arg(internalField.getName());
    }

  protected:
    BaseUnsignedField<uint64_t, N> internalField;
    uint64_t & sw;
    uint64_t _sw;
    Board::Type board;
    unsigned int version;
};

class ArmCustomFunctionField: public TransformedField {
  public:
    ArmCustomFunctionField(DataField * parent, CustomFunctionData & fn, Board::Type board, unsigned int version, unsigned int variant):
      TransformedField(parent, internalField),
      internalField(this, "CustomFunction"),
      fn(fn),
      board(board),
      version(version),
      variant(variant),
      functionsConversionTable(board, version),
      sourcesConversionTable(SourcesConversionTable::getInstance(board, version, variant, version >= 216 ? 0 : FLAG_NONONE)),
      _func(0),
      _active(0),
      _mode(0)
    {
      memset(_param, 0, sizeof(_param));

      if (version >= 218) {
        internalField.Append(new SwitchField<9>(this, fn.swtch, board, version));
        internalField.Append(new ConversionField< UnsignedField<7> >(this, _func, &functionsConversionTable, "Function", DataField::tr("OpenTX on this board doesn't accept this function")));
      }
      else {
        internalField.Append(new SwitchField<8>(this, fn.swtch, board, version));
        internalField.Append(new ConversionField< UnsignedField<8> >(this, _func, &functionsConversionTable, "Function", DataField::tr("OpenTX on this board doesn't accept this function")));
      }

      if (IS_TARANIS(board) && version >= 216)
        internalField.Append(new CharField<8>(this, _param, false));
      else if (IS_TARANIS(board))
        internalField.Append(new CharField<10>(this, _param, false));
      else
        internalField.Append(new CharField<6>(this, _param, false));

      if (version >= 216) {
        internalField.Append(new SignedField<8>(this, _active));
      }
      else if (version >= 214) {
        internalField.Append(new UnsignedField<2>(this, _mode));
        internalField.Append(new UnsignedField<6>(this, (unsigned int &)_active));
      }
      else {
        internalField.Append(new UnsignedField<8>(this, (unsigned int &)_active));
        internalField.Append(new SpareBitsField<8>(this));
      }
    }

    static bool hasRepeatParam(const CustomFunctionData & fn)
    {
      return (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayValue || fn.func == FuncPlayHaptic);
    }

    virtual void beforeExport()
    {
      if (fn.swtch.type != SWITCH_TYPE_NONE) {
        _func = fn.func;

        if (hasRepeatParam(fn))
          _active = (version >= 216 ? fn.repeatParam : (fn.repeatParam/5));
        else
          _active = (fn.enabled ? 1 : 0);

        if (fn.func >= FuncOverrideCH1 && fn.func <= FuncOverrideCH32) {
          if (version >= 216) {
            *((uint16_t *)_param) = fn.param;
            *((uint8_t *)(_param+3)) = fn.func - FuncOverrideCH1;
          }
          else {
            *((uint32_t *)_param) = fn.param;
          }
        }
        else if (fn.func >= FuncTrainer && fn.func <= FuncTrainerAIL) {
          if (version >= 216)
            *((uint8_t *)(_param+3)) = fn.func - FuncTrainer;
        }
        else if (fn.func >= FuncSetTimer1 && fn.func <= FuncSetTimer3) {
          if (version >= 216) {
            *((uint16_t *)_param) = fn.param;
            *((uint8_t *)(_param+3)) = fn.func - FuncSetTimer1;
          }
        }
        else if (fn.func >= FuncSetFailsafeInternalModule && fn.func <= FuncSetFailsafeExternalModule) {
          if (version >= 216) {
            *((uint16_t *)_param) = fn.func - FuncSetFailsafeInternalModule;
          }
        }
        else if (fn.func >= FuncRangeCheckInternalModule && fn.func <= FuncRangeCheckExternalModule) {
          if (version >= 216) {
            *((uint16_t *)_param) = fn.func - FuncRangeCheckInternalModule;
          }
        }
        else if (fn.func >= FuncBindInternalModule && fn.func <= FuncBindExternalModule) {
          if (version >= 216) {
            *((uint16_t *)_param) = fn.func - FuncBindInternalModule;
          }
        }
        else if (fn.func == FuncPlayPrompt || fn.func == FuncBackgroundMusic || fn.func == FuncPlayScript) {
          memcpy(_param, fn.paramarm, sizeof(_param));
        }
        else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGVLast) {
          if (version >= 216) {
            *((uint8_t *)(_param+2)) = fn.adjustMode;
            *((uint8_t *)(_param+3)) = fn.func - FuncAdjustGV1;
            unsigned int value;
            if (fn.adjustMode == 1)
              sourcesConversionTable->exportValue(fn.param, (int &)value);
            else if (fn.adjustMode == 2)
              value = RawSource(fn.param).index;
            else
              value = fn.param;
            *((uint16_t *)_param) = value;
          }
          else if (version >= 214) {
            unsigned int value;
            _mode = fn.adjustMode;
            if (fn.adjustMode == 1)
              sourcesConversionTable->exportValue(fn.param, (int &)value);
            else if (fn.adjustMode == 2)
              value = RawSource(fn.param).index;
            else
              value = fn.param;
            *((uint32_t *)_param) = value;
          }
          else {
            unsigned int value;
            sourcesConversionTable->exportValue(fn.param, (int &)value);
            *((uint32_t *)_param) = value;
          }
        }
        else if (fn.func == FuncPlayValue || fn.func == FuncVolume) {
          unsigned int value;
          sourcesConversionTable->exportValue(fn.param, (int &)value);
          if (version >= 216)
            *((uint16_t *)_param) = value;
          else
            *((uint32_t *)_param) = value;
        }
        else if (fn.func == FuncReset) {
          if (version >= 217)
            *((uint32_t *)_param) = fn.param;
          else
            *((uint32_t *)_param) = (fn.param < 2 ? fn.param : fn.param-1);
        }
        else {
          *((uint32_t *)_param) = fn.param;
        }
      }
    }

    virtual void afterImport()
    {
      fn.func = (AssignFunc)_func;

      if (hasRepeatParam(fn))
        fn.repeatParam = (version >= 216 ? _active : (_active*5));
      else
        fn.enabled = (_active & 0x01);

      unsigned int value=0, mode=0, index=0;
      if (version >= 216) {
        value = *((uint16_t *)_param);
        mode = *((uint8_t *)(_param+2));
        index = *((uint8_t *)(_param+3));
      }
      else {
        value = *((uint32_t *)_param);
      }

      if (fn.func >= FuncOverrideCH1 && fn.func <= FuncOverrideCH32) {
        fn.func = AssignFunc(fn.func + index);
        fn.param = (int16_t)(uint16_t)value;
      }
      else if (fn.func >= FuncSetTimer1 && fn.func <= FuncSetTimer3) {
        fn.func = AssignFunc(fn.func + index);
        fn.param = (int)value;
      }
      else if (fn.func >= FuncTrainer && fn.func <= FuncTrainerAIL) {
        fn.func = AssignFunc(fn.func + index);
        fn.param = value;
      }
      else if (fn.func == FuncPlayPrompt || fn.func == FuncBackgroundMusic || fn.func == FuncPlayScript) {
        memcpy(fn.paramarm, _param, sizeof(fn.paramarm));
      }
      else if (fn.func == FuncVolume) {
        sourcesConversionTable->importValue(value, (int &)fn.param);
      }
      else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGVLast) {
        if (version >= 216) {
          fn.func = AssignFunc(fn.func + index);
          fn.adjustMode = mode;
          if (fn.adjustMode == 1)
            sourcesConversionTable->importValue(value, (int &)fn.param);
          else if (fn.adjustMode == 2)
            fn.param = RawSource(SOURCE_TYPE_GVAR, value).toValue();
          else
            fn.param = (int16_t)value;
        }
        else if (version >= 214) {
          fn.adjustMode = _mode;
          if (fn.adjustMode == 1)
            sourcesConversionTable->importValue(value, (int &)fn.param);
          else if (fn.adjustMode == 2)
            fn.param = RawSource(SOURCE_TYPE_GVAR, value).toValue();
          else
            fn.param = (int8_t)value;
        }
        else {
          sourcesConversionTable->importValue(value, (int &)fn.param);
        }
      }
      else if (fn.func == FuncPlayValue) {
        if (version >= 213)
          sourcesConversionTable->importValue(value, (int &)fn.param);
        else
          SourcesConversionTable::getInstance(board, version, variant, FLAG_NONONE|FLAG_NOSWITCHES)->importValue(value, (int &)fn.param);
      }
      else if (fn.func == FuncReset) {
        if (version >= 217)
          fn.param = value;
        else
          fn.param = (value < 2 ? value : value+1);
      }
      else {
        fn.param = value;
      }
      qCDebug(eepromImport) << QString("imported %1").arg(internalField.getName());
    }

  protected:
    StructField internalField;
    CustomFunctionData & fn;
    Board::Type board;
    unsigned int version;
    unsigned int variant;
    CustomFunctionsConversionTable functionsConversionTable;
    SourcesConversionTable * sourcesConversionTable;
    unsigned int _func;
    char _param[10];
    int _active;
    unsigned int _mode;
};

class AvrCustomFunctionField: public TransformedField {
  public:
    AvrCustomFunctionField(DataField * parent, CustomFunctionData & fn, Board::Type board, unsigned int version, unsigned int variant):
      TransformedField(parent, internalField),
      internalField(this, "CustomFunction"),
      fn(fn),
      board(board),
      version(version),
      variant(variant),
      functionsConversionTable(board, version),
      sourcesConversionTable(SourcesConversionTable::getInstance(board, version, variant, version >= 216 ? 0 : FLAG_NONONE)),
      _param(0),
      _mode(0),
      _union_param(0),
      _active(0)
    {
      if (version >= 217 && IS_2560(board)) {
        internalField.Append(new SwitchField<8>(this, fn.swtch, board, version));
        internalField.Append(new ConversionField< UnsignedField<8> >(this, (unsigned int &)fn.func, &functionsConversionTable, "Function", DataField::tr("OpenTX on this board doesn't accept this function")));
        internalField.Append(new UnsignedField<2>(this, fn.adjustMode));
        internalField.Append(new UnsignedField<4>(this, _union_param));
        internalField.Append(new UnsignedField<1>(this, _active));
        internalField.Append(new SpareBitsField<1>(this));
      }
      else if (version >= 216) {
        internalField.Append(new SwitchField<6>(this, fn.swtch, board, version));
        internalField.Append(new ConversionField< UnsignedField<4> >(this, (unsigned int &)fn.func, &functionsConversionTable, "Function", DataField::tr("OpenTX on this board doesn't accept this function")));
        internalField.Append(new UnsignedField<5>(this, _union_param));
        internalField.Append(new UnsignedField<1>(this, _active));
      }
      else if (version >= 213) {
        internalField.Append(new SwitchField<8>(this, fn.swtch, board, version));
        internalField.Append(new UnsignedField<3>(this, _union_param));
        internalField.Append(new ConversionField< UnsignedField<5> >(this, (unsigned int &)fn.func, &functionsConversionTable, "Function", DataField::tr("OpenTX on this board doesn't accept this function")));
      }
      else {
        internalField.Append(new SwitchField<8>(this, fn.swtch, board, version));
        internalField.Append(new ConversionField< UnsignedField<7> >(this, (unsigned int &)fn.func, &functionsConversionTable, "Function", DataField::tr("OpenTX on this board doesn't accept this function")));
        internalField.Append(new BoolField<1>(this, (bool &)fn.enabled));
      }
      internalField.Append(new UnsignedField<8>(this, _param));
    }

    virtual void beforeExport()
    {
      _param = fn.param;
      _active = (fn.enabled ? 1 : 0);

      if (fn.func >= FuncOverrideCH1 && fn.func <= FuncOverrideCH32) {
        if (version >= 216)
          _union_param = fn.func - FuncOverrideCH1;
        else if (version >= 213)
          _active += ((fn.func % 4) << 1);
      }
      else if (fn.func >= FuncTrainer && fn.func <= FuncTrainerAIL) {
        if (version >= 216)
          _union_param = fn.func - FuncTrainer;
      }
      else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGVLast) {
        if (version >= 217 && IS_2560(board)) {
          _union_param = (fn.func - FuncAdjustGV1);
          if (fn.adjustMode == 1)
            sourcesConversionTable->exportValue(fn.param, (int &)_param);
          else if (fn.adjustMode == 2)
            _param = RawSource(fn.param).index;
        }
        else if (version >= 216) {
          _union_param = fn.adjustMode;
          _union_param += (fn.func - FuncAdjustGV1) << 2;
          if (fn.adjustMode == 1)
            sourcesConversionTable->exportValue(fn.param, (int &)_param);
          else if (fn.adjustMode == 2)
            _param = RawSource(fn.param).index;
        }
        else if (version >= 213) {
          _active += (fn.adjustMode << 1);
          if (fn.adjustMode == 1)
            sourcesConversionTable->exportValue(fn.param, (int &)_param);
          else if (fn.adjustMode == 2)
            _param = RawSource(fn.param).index;
        }
        else {
          sourcesConversionTable->exportValue(fn.param, (int &)_param);
        }
      }
      else if (fn.func == FuncPlayValue) {
        if (version >= 216) {
          _union_param = fn.repeatParam / 10;
          sourcesConversionTable->exportValue(fn.param, (int &)_param);
        }
        else if (version >= 213) {
          _active = fn.repeatParam / 10;
          sourcesConversionTable->exportValue(fn.param, (int &)_param);
        }
        else {
          SourcesConversionTable::getInstance(board, version, variant, FLAG_NONONE|FLAG_NOSWITCHES)->exportValue(fn.param, (int &)_param);
        }
      }
      else if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayBoth) {
        if (version >= 216) {
          _union_param = fn.repeatParam / 10;
        }
        else if (version >= 213) {
          _active = fn.repeatParam / 10;
        }
      }
    }

    virtual void afterImport()
    {
      fn.param = _param;
      if (version >= 213) {
        fn.enabled = (_active & 0x01);
      }

      if (fn.func >= FuncOverrideCH1 && fn.func <= FuncOverrideCH32) {
        if (version >= 216)
          fn.func = AssignFunc(fn.func + _union_param);
        else if (version >= 213)
          fn.func = AssignFunc(((fn.func >> 2) << 2) + ((_active >> 1) & 0x03));
        fn.param = (int8_t)fn.param;
      }
      else if (fn.func >= FuncTrainer && fn.func <= FuncTrainerAIL) {
        if (version >= 216)
          fn.func = AssignFunc(fn.func + _union_param);
      }
      else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGVLast) {
        if (version >= 217 && IS_2560(board)) {
          fn.func = AssignFunc(fn.func + _union_param);
          if (fn.adjustMode == 1)
            sourcesConversionTable->importValue(_param, (int &)fn.param);
          else if (fn.adjustMode == 2)
            fn.param = RawSource(SOURCE_TYPE_GVAR, _param).toValue();
        }
        else if (version >= 216) {
          fn.func = AssignFunc(fn.func + (_union_param >> 2));
          fn.adjustMode = (_union_param & 0x03);
          if (fn.adjustMode == 1)
            sourcesConversionTable->importValue(_param, (int &)fn.param);
          else if (fn.adjustMode == 2)
            fn.param = RawSource(SOURCE_TYPE_GVAR, _param).toValue();
        }
        else if (version >= 213) {
          fn.adjustMode = ((_active >> 1) & 0x03);
          if (fn.adjustMode == 1)
            sourcesConversionTable->importValue(_param, (int &)fn.param);
          else if (fn.adjustMode == 2)
            fn.param = RawSource(SOURCE_TYPE_GVAR, _param).toValue();
        }
        else {
          sourcesConversionTable->importValue(_param, (int &)fn.param);
        }
      }
      else if (fn.func == FuncPlayValue) {
        if (version >= 216) {
          fn.repeatParam = _union_param * 10;
          sourcesConversionTable->importValue(_param, (int &)fn.param);
        }
        else if (version >= 213) {
          fn.repeatParam = _active * 10;
          sourcesConversionTable->importValue(_param, (int &)fn.param);
        }
        else {
          SourcesConversionTable::getInstance(board, version, variant, FLAG_NONONE|FLAG_NOSWITCHES)->importValue(_param, (int &)fn.param);
        }
      }
      else if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayBoth) {
        if (version >= 216)
          fn.repeatParam = _union_param * 10;
        else if (version >= 213)
          fn.repeatParam = _active * 10;
      }
      qCDebug(eepromImport) << QString("imported %1").arg(internalField.getName());
    }

  protected:
    StructField internalField;
    CustomFunctionData & fn;
    Board::Type board;
    unsigned int version;
    unsigned int variant;
    CustomFunctionsConversionTable functionsConversionTable;
    SourcesConversionTable * sourcesConversionTable;
    unsigned int _param;
    unsigned int _mode;
    unsigned int _union_param;
    unsigned int _active;
};

class FrskyScreenField: public DataField {
  public:
    FrskyScreenField(DataField * parent, FrSkyScreenData & screen, Board::Type board, unsigned int version, unsigned int variant):
      DataField(parent, "Frsky Screen"),
      screen(screen),
      board(board),
      version(version),
      none(this),
      bars(this),
      numbers(this),
      script(this)
    {
      for (int i=0; i<4; i++) {
        if (IS_ARM(board) && version >= 217) {
          if (IS_TARANIS(board))
            bars.Append(new SourceField<16>(this, screen.body.bars[i].source, board, version, variant));
          else
            bars.Append(new SourceField<8>(this, screen.body.bars[i].source, board, version, variant));
          bars.Append(new SignedField<16>(this, screen.body.bars[i].barMin));
          bars.Append(new SignedField<16>(this, screen.body.bars[i].barMax));
        }
        else {
          bars.Append(new TelemetrySourceField<8>(this, screen.body.bars[i].source, board, version));
          bars.Append(new UnsignedField<8>(this, (unsigned &)screen.body.bars[i].barMin));
          bars.Append(new UnsignedField<8>(this, (unsigned &)screen.body.bars[i].barMax));
        }
      }

      int columns = (HAS_LARGE_LCD(board) ? 3 : 2);
      for (int i=0; i<4; i++) {
        for (int j=0; j<columns; j++) {
          if (IS_TARANIS(board) && version >= 217)
            numbers.Append(new SourceField<16>(this, screen.body.lines[i].source[j], board, version, variant));
          else if (IS_ARM(board) && version >= 217)
            numbers.Append(new SourceField<8>(this, screen.body.lines[i].source[j], board, version, variant));
          else
            numbers.Append(new TelemetrySourceField<8>(this, screen.body.lines[i].source[j], board, version));
        }
      }
      for (unsigned i=numbers.size(); i<bars.size(); i++) {
        numbers.Append(new SpareBitsField<1>(this));
      }

      if (IS_TARANIS(board) && version >= 217) {
        script.Append(new CharField<8>(this, screen.body.script.filename, true, "Script name"));
        script.Append(new SpareBitsField<16*8>(this));
      }

      if (IS_ARM(board) && version >= 217) {
        if (IS_TARANIS(board))
          none.Append(new SpareBitsField<24*8>(this));
        else
          none.Append(new SpareBitsField<20*8>(this));
      }
    }

    virtual void ExportBits(QBitArray & output)
    {
      if (IS_ARM(board) && version >= 217) {
        if (screen.type == TELEMETRY_SCREEN_SCRIPT)
          script.ExportBits(output);
        else if (screen.type == TELEMETRY_SCREEN_NUMBERS)
          numbers.ExportBits(output);
        else if (screen.type == TELEMETRY_SCREEN_BARS)
          bars.ExportBits(output);
        else
          none.ExportBits(output);
      }
      else {
        if (screen.type == TELEMETRY_SCREEN_NUMBERS)
          numbers.ExportBits(output);
        else
          bars.ExportBits(output);
      }
    }

    virtual void ImportBits(const QBitArray & input)
    {
      qCDebug(eepromImport) << QString("importing %1: type: %2").arg(name).arg(screen.type);

      // NOTA: screen.type should have been imported first!
      if (IS_ARM(board) && version >= 217) {
        if (screen.type == TELEMETRY_SCREEN_SCRIPT)
          script.ImportBits(input);
        else if (screen.type == TELEMETRY_SCREEN_NUMBERS)
          numbers.ImportBits(input);
        else if (screen.type == TELEMETRY_SCREEN_BARS)
          bars.ImportBits(input);
        else
          none.ImportBits(input);
      }
      else {
        if (screen.type == TELEMETRY_SCREEN_NUMBERS)
          numbers.ImportBits(input);
        else
          bars.ImportBits(input);
      }
    }

    virtual unsigned int size()
    {
      // NOTA: screen.type should have been imported first!
      if (IS_ARM(board) && version >= 217) {
        if (screen.type == TELEMETRY_SCREEN_SCRIPT)
          return script.size();
        else if (screen.type == TELEMETRY_SCREEN_NUMBERS)
          return numbers.size();
        else if (screen.type == TELEMETRY_SCREEN_BARS)
          return bars.size();
        else
          return none.size();
      }
      else {
        if (screen.type == TELEMETRY_SCREEN_NUMBERS)
          return numbers.size();
        else
          return bars.size();
      }
    }

  protected:
    FrSkyScreenData & screen;
    Board::Type board;
    unsigned int version;
    StructField none;
    StructField bars;
    StructField numbers;
    StructField script;
};

class RSSIConversionTable: public ConversionTable
{
  public:
    RSSIConversionTable(int index)
    {
      addConversion(0, 2-index);
      addConversion(1, 3-index);
      addConversion(2, index ? 3 : 0);
      addConversion(3, 1-index);
    }

    RSSIConversionTable()
    {
    }
};

class TelemetryVarioSourceConversionTable: public ConversionTable
{
  public:
    TelemetryVarioSourceConversionTable(Board::Type board, unsigned int version)
    {
      int val = 0;
      if (!IS_HORUS_OR_TARANIS(board)) {
        addConversion(TELEMETRY_VARIO_SOURCE_ALTI, val++);
        addConversion(TELEMETRY_VARIO_SOURCE_ALTI_PLUS, val++);
      }
      addConversion(TELEMETRY_VARIO_SOURCE_VSPEED, val++);
      addConversion(TELEMETRY_VARIO_SOURCE_A1, val++);
      addConversion(TELEMETRY_VARIO_SOURCE_A2, val++);
      if (IS_HORUS_OR_TARANIS(board)) {
        addConversion(TELEMETRY_VARIO_SOURCE_DTE, val++);
      }
    }
};

class TelemetryVoltsSourceConversionTable: public ConversionTable
{
  public:
    TelemetryVoltsSourceConversionTable(Board::Type board, unsigned int version)
    {
      int val = 0;
      addConversion(TELEMETRY_VOLTS_SOURCE_A1, val++);
      addConversion(TELEMETRY_VOLTS_SOURCE_A2, val++);
      if (IS_ARM(board) && version >= 216) {
        addConversion(TELEMETRY_VOLTS_SOURCE_A3, val++);
        addConversion(TELEMETRY_VOLTS_SOURCE_A4, val++);
      }
      addConversion(TELEMETRY_VOLTS_SOURCE_FAS, val++);
      addConversion(TELEMETRY_VOLTS_SOURCE_CELLS, val++);
    }
};

class ScreenTypesConversionTable: public ConversionTable
{
  public:
    ScreenTypesConversionTable(Board::Type board, unsigned int version)
    {
      int val = 0;
      if (IS_ARM(board)) {
        addConversion(TELEMETRY_SCREEN_NONE, val++);
      }
      addConversion(TELEMETRY_SCREEN_NUMBERS, val++);
      addConversion(TELEMETRY_SCREEN_BARS, val++);
    }
};

class TelemetryCurrentSourceConversionTable: public ConversionTable
{
  public:
    TelemetryCurrentSourceConversionTable(Board::Type board, unsigned int version)
    {
      int val = 0;
      addConversion(TELEMETRY_CURRENT_SOURCE_NONE, val++);
      addConversion(TELEMETRY_CURRENT_SOURCE_A1, val++);
      addConversion(TELEMETRY_CURRENT_SOURCE_A2, val++);
      if (IS_ARM(board) && version >= 216) {
        addConversion(TELEMETRY_CURRENT_SOURCE_A3, val++);
        addConversion(TELEMETRY_CURRENT_SOURCE_A4, val++);
      }
      addConversion(TELEMETRY_CURRENT_SOURCE_FAS, val++);
    }
};

class FrskyField: public StructField {
  public:
    FrskyField(DataField * parent, FrSkyData & frsky, RSSIAlarmData & rssiAlarms, Board::Type board, unsigned int version, unsigned int variant):
      StructField(parent, "FrSky"),
      telemetryVarioSourceConversionTable(board, version),
      screenTypesConversionTable(board, version),
      telemetryVoltsSourceConversionTable(board, version),
      telemetryCurrentSourceConversionTable(board, version)
    {
      rssiConversionTable[0] = RSSIConversionTable(0);
      rssiConversionTable[1] = RSSIConversionTable(1);

      if (IS_ARM(board)) {
        if (!IS_HORUS(board)) {
          if (version >= 217) {
            Append(new UnsignedField<8>(this, frsky.voltsSource, "Volts Source"));
            Append(new UnsignedField<8>(this, frsky.altitudeSource, "Altitude Source"));
          }
          else {
            for (int i = 0; i < (version >= 216 ? 4 : 2); i++) {
              Append(new UnsignedField<8>(this, frsky.channels[i].ratio, "Ratio"));
              Append(new SignedField<12>(this, frsky.channels[i].offset, "Offset"));
              Append(new UnsignedField<4>(this, frsky.channels[i].type, "Type"));
              for (int j = 0; j < 2; j++)
                Append(new UnsignedField<8>(this, frsky.channels[i].alarms[j].value, "Alarm value"));
              for (int j = 0; j < 2; j++)
                Append(new UnsignedField<2>(this, frsky.channels[i].alarms[j].level));
              for (int j = 0; j < 2; j++)
                Append(new UnsignedField<1>(this, frsky.channels[i].alarms[j].greater));
              Append(new SpareBitsField<2>(this));
              Append(new UnsignedField<8>(this, frsky.channels[i].multiplier, 0, 5, "Multiplier"));
            }
            Append(new UnsignedField<8>(this, frsky.usrProto));
            if (version >= 216) {
              Append(new ConversionField<UnsignedField<7> >(this, frsky.voltsSource, &telemetryVoltsSourceConversionTable, "Volts Source"));
              Append(new SpareBitsField<1>(this));
            }
            else {
              Append(new ConversionField<UnsignedField<8> >(this, frsky.voltsSource, &telemetryVoltsSourceConversionTable, "Volts Source"));
            }
            Append(new ConversionField<SignedField<8> >(this, frsky.blades, -2));
            Append(new ConversionField<UnsignedField<8> >(this, frsky.currentSource, &telemetryCurrentSourceConversionTable, "Current Source"));
          }

          if (version >= 217) {
            for (int i = 0; i < 4; i++) {
              Append(new UnsignedField<2>(this, frsky.screens[i].type));
            }
            for (int i = 0; i < 4; i++) {
              Append(new FrskyScreenField(this, frsky.screens[i], board, version, variant));
            }
          }
          else {
            Append(new UnsignedField<1>(this, frsky.screens[0].type));
            Append(new UnsignedField<1>(this, frsky.screens[1].type));
            Append(new UnsignedField<1>(this, frsky.screens[2].type));
            Append(new SpareBitsField<5>(this));
            for (int i = 0; i < 3; i++) {
              Append(new FrskyScreenField(this, frsky.screens[i], board, version, variant));
            }
          }
        }

        if (version >= 217) {
          Append(new UnsignedField<7>(this, frsky.varioSource, "Vario Source"));
          Append(new BoolField<1>(this, frsky.varioCenterSilent));
        }
        else {
          Append(new ConversionField< UnsignedField<8> >(this, frsky.varioSource, &telemetryVarioSourceConversionTable, "Vario Source"));
        }
        Append(new SignedField<8>(this, frsky.varioCenterMax));
        Append(new SignedField<8>(this, frsky.varioCenterMin));
        Append(new SignedField<8>(this, frsky.varioMin));
        Append(new SignedField<8>(this, frsky.varioMax));
        Append(new BoolField<1>(this, rssiAlarms.disabled));
        Append(new SpareBitsField<1>(this));
        Append(new ConversionField<SignedField<6> >(this, rssiAlarms.warning, -45));
        Append(new SpareBitsField<2>(this));
        Append(new ConversionField<SignedField<6> >(this, rssiAlarms.critical, -42));

        if (version == 216) {
          Append(new BoolField<1>(this, frsky.mAhPersistent));
          Append(new UnsignedField<15>(this, frsky.storedMah));
          Append(new SignedField<8>(this, frsky.fasOffset));
        }
      }
      else {
        for (int i=0; i<2; i++) {
          Append(new UnsignedField<8>(this, frsky.channels[i].ratio, "Ratio"));
          Append(new SignedField<12>(this, frsky.channels[i].offset, "Offset"));
          Append(new UnsignedField<4>(this, frsky.channels[i].type, "Type"));
          for (int j=0; j<2; j++)
            Append(new UnsignedField<8>(this, frsky.channels[i].alarms[j].value, "Alarm value"));
          for (int j=0; j<2; j++)
            Append(new UnsignedField<2>(this, frsky.channels[i].alarms[j].level));
          for (int j=0; j<2; j++)
            Append(new UnsignedField<1>(this, frsky.channels[i].alarms[j].greater));
          Append(new UnsignedField<2>(this, frsky.channels[i].multiplier, 0, 3, "Multiplier"));
        }
        Append(new UnsignedField<2>(this, frsky.usrProto, "USR Proto"));
        Append(new ConversionField< UnsignedField<2> >(this, (unsigned int &)frsky.blades, -2));
        for (int i=0; i<2; i++) {
          Append(new ConversionField< UnsignedField<1> >(this, frsky.screens[i].type, &screenTypesConversionTable, "Screen Type"));
        }
        Append(new ConversionField< UnsignedField<2> >(this, frsky.voltsSource, &telemetryVoltsSourceConversionTable, "Volts Source"));
        Append(new SignedField<4>(this, frsky.varioMin, "Vario Min"));
        Append(new SignedField<4>(this, frsky.varioMax));
        for (int i=0; i<2; i++) {
          Append(new ConversionField< UnsignedField<2> >(this, rssiAlarms.level[i], &rssiConversionTable[i], "RSSI level"));
          Append(new ConversionField< SignedField<6> >(this, i ==0 ? rssiAlarms.warning : rssiAlarms.critical, -45+i*3, 0, 0, 100, "RSSI value"));
        }
        for (int i=0; i<2; i++) {
          Append(new FrskyScreenField(this, frsky.screens[i], board, version, variant));
        }
        Append(new UnsignedField<3>(this, frsky.varioSource));
        Append(new SignedField<5>(this, frsky.varioCenterMin));
        Append(new ConversionField< UnsignedField<3> >(this, frsky.currentSource, &telemetryCurrentSourceConversionTable, "Current Source"));
        Append(new SignedField<5>(this, frsky.varioCenterMax));
        if (version >= 216) {
          Append(new SignedField<8>(this, frsky.fasOffset));
        }
      }
    }

  protected:
    RSSIConversionTable rssiConversionTable[2];
    TelemetryVarioSourceConversionTable telemetryVarioSourceConversionTable;
    ScreenTypesConversionTable screenTypesConversionTable;
    TelemetryVoltsSourceConversionTable telemetryVoltsSourceConversionTable;
    TelemetryCurrentSourceConversionTable telemetryCurrentSourceConversionTable;
};

class MavlinkField: public StructField {
  public:
    MavlinkField(DataField * parent, MavlinkData & mavlink, Board::Type board, unsigned int version):
      StructField(parent, "MavLink")
    {
      Append(new UnsignedField<4>(this, mavlink.rc_rssi_scale, "Rc_rssi_scale"));
      Append(new UnsignedField<1>(this, mavlink.pc_rssi_en, "Pc_rssi_en"));
      Append(new SpareBitsField<3>(this));
      Append(new SpareBitsField<8>(this));
      Append(new SpareBitsField<8>(this));
      Append(new SpareBitsField<8>(this));
    }
};

/*
 * TODO
 */
#if 0
class CustomScreenField: public StructField {
  public:
    CustomScreenField(CustomScreenData & customScreen):
      StructField("Custom Screen"),
      customScreen(customScreen)
    {
    }

  protected:
    CustomScreenData & customScreen;
};
#endif

class SensorField: public TransformedField {
  public:
    SensorField(DataField * parent, SensorData & sensor, Board::Type board, unsigned int version):
      TransformedField(parent, internalField),
      internalField(this, "Sensor"),
      sensor(sensor),
      version(version),
      _param(0)
    {
      internalField.Append(new UnsignedField<16>(this, _id, "id/persistentValue"));
      internalField.Append(new UnsignedField<8>(this, _instance, "instance/formula"));
      internalField.Append(new ZCharField<4>(this, sensor.label));
      internalField.Append(new UnsignedField<1>(this, sensor.type, "type"));
      internalField.Append(new UnsignedField<5>(this, sensor.unit, "unit"));
      internalField.Append(new UnsignedField<2>(this, sensor.prec, "prec"));
      internalField.Append(new BoolField<1>(this, sensor.autoOffset));
      internalField.Append(new BoolField<1>(this, sensor.filter));
      internalField.Append(new BoolField<1>(this, sensor.logs));
      internalField.Append(new BoolField<1>(this, sensor.persistent));
      internalField.Append(new BoolField<1>(this, sensor.onlyPositive));
      internalField.Append(new UnsignedField<3>(this, _subid, "subid"));
      internalField.Append(new UnsignedField<32>(this, _param, "param"));
    }

    virtual void beforeExport()
    {
      if (sensor.type == SensorData::TELEM_TYPE_CUSTOM) {
        _id = sensor.id;
        _subid = sensor.subid;
        _instance = sensor.instance;
        _ratio = sensor.ratio;
        _offset = sensor.offset;
      }
      else {
        _id = sensor.persistentValue;
        _instance = sensor.formula;
        if (sensor.formula == SensorData::TELEM_FORMULA_CELL)
          _param = (sensor.source) + (sensor.index << 8);
        else if (sensor.formula == SensorData::TELEM_FORMULA_ADD || sensor.formula == SensorData::TELEM_FORMULA_AVERAGE || sensor.formula == SensorData::TELEM_FORMULA_MULTIPLY || sensor.formula == SensorData::TELEM_FORMULA_MIN || sensor.formula == SensorData::TELEM_FORMULA_MAX)
          _param = ((uint8_t)sensor.sources[0]) + ((uint8_t)sensor.sources[1] << 8) + ((uint8_t)sensor.sources[2] << 16) + ((uint8_t)sensor.sources[3] << 24);
        else if (sensor.formula == SensorData::TELEM_FORMULA_DIST)
          _param = (sensor.gps) + (sensor.alt << 8);
        else if (sensor.formula == SensorData::TELEM_FORMULA_CONSUMPTION || sensor.formula == SensorData::TELEM_FORMULA_TOTALIZE)
          _param = (sensor.amps);
      }
    }

    virtual void afterImport()
    {
      if (sensor.type == SensorData::TELEM_TYPE_CUSTOM) {
        sensor.id = _id;
        sensor.subid = _subid;
        sensor.instance = _instance;
        sensor.ratio = _ratio;
        sensor.offset = _offset;
      }
      else {
        sensor.persistentValue = _id;
        sensor.formula = _instance;
        if (sensor.formula == SensorData::TELEM_FORMULA_CELL)
          (sensor.source = _sources[0], sensor.index = _sources[1]);
        else if (sensor.formula == SensorData::TELEM_FORMULA_ADD || sensor.formula == SensorData::TELEM_FORMULA_AVERAGE || sensor.formula == SensorData::TELEM_FORMULA_MULTIPLY || sensor.formula == SensorData::TELEM_FORMULA_MIN || sensor.formula == SensorData::TELEM_FORMULA_MAX)
          for (int i=0; i<4; ++i)
            sensor.sources[i] = _sources[i];
        else if (sensor.formula == SensorData::TELEM_FORMULA_DIST)
          (sensor.gps = _sources[0], sensor.alt = _sources[1]);
        else if (sensor.formula == SensorData::TELEM_FORMULA_CONSUMPTION || sensor.formula == SensorData::TELEM_FORMULA_TOTALIZE)
          sensor.amps = _sources[0];
      }

      if (version < 218) {
        if (sensor.unit > SensorData::UNIT_WATTS)
          sensor.unit++;
        if (sensor.unit > SensorData::UNIT_DEGREE)
          sensor.unit++;
      }

      qCDebug(eepromImport) << QString("imported %1").arg(internalField.getName());
    }

  protected:
    StructField internalField;
    SensorData & sensor;
    unsigned int version;
    unsigned int _id;
    unsigned int _subid;
    unsigned int _instance;
    union {
      unsigned int _param;
      uint8_t _sources[4];
      struct {
        uint16_t _ratio;
        int16_t _offset;
      };
    };
};

int exportPpmDelay(int delay) { return (delay - 300) / 50; }
int importPpmDelay(int delay) { return 300 + 50 * delay; }

OpenTxModelData::OpenTxModelData(ModelData & modelData, Board::Type board, unsigned int version, unsigned int variant):
  TransformedField(NULL, internalField),
  internalField(this, "ModelData"),
  modelData(modelData),
  board(board),
  version(version),
  variant(variant),
  protocolsConversionTable(board)
{
  sprintf(name, "Model %s", modelData.name);

  qCDebug(eepromImport) << QString("OpenTxModelData::OpenTxModelData(name: %1, board: %2, ver: %3, var: %4)").arg(name).arg(board).arg(version).arg(variant);

  if (IS_HORUS(board))
    internalField.Append(new ZCharField<15>(this, modelData.name, "Model name"));
  else if (HAS_LARGE_LCD(board))
    internalField.Append(new ZCharField<12>(this, modelData.name, "Model name"));
  else
    internalField.Append(new ZCharField<10>(this, modelData.name, "Model name"));

  bool afterrelease21March2013 = IS_AFTER_RELEASE_21_MARCH_2013(board, version);

  if (afterrelease21March2013) {
    internalField.Append(new UnsignedField<8>(this, modelData.moduleData[0].modelId));
  }
  if (IS_ARM(board) && version >= 217) {
    internalField.Append(new UnsignedField<8>(this, modelData.moduleData[1].modelId));
  }

  if (HAS_LARGE_LCD(board) && version >= 215) {
    internalField.Append(new CharField<10>(this, modelData.bitmap, true, "Model bitmap"));
  }

  for (int i=0; i<MAX_TIMERS(board, version); i++) {
    if (IS_ARM(board) && version >= 218) {
      internalField.Append(new SwitchField<9>(this, modelData.timers[i].mode, board, version, true));
      internalField.Append(new UnsignedField<23>(this, modelData.timers[i].val));
      internalField.Append(new SignedField<24>(this, modelData.timers[i].pvalue));
      internalField.Append(new UnsignedField<2>(this, modelData.timers[i].countdownBeep));
      internalField.Append(new BoolField<1>(this, modelData.timers[i].minuteBeep));
      internalField.Append(new UnsignedField<2>(this, modelData.timers[i].persistent));
      internalField.Append(new SpareBitsField<3>(this));
      if (HAS_LARGE_LCD(board))
        internalField.Append(new ZCharField<8>(this, modelData.timers[i].name, "Timer name"));
      else
        internalField.Append(new ZCharField<3>(this, modelData.timers[i].name, "Timer name"));
    }
    else if (IS_ARM(board) && version >= 217) {
      internalField.Append(new SwitchField<8>(this, modelData.timers[i].mode, board, version, true));
      internalField.Append(new UnsignedField<24>(this, modelData.timers[i].val));
      internalField.Append(new SignedField<24>(this, modelData.timers[i].pvalue));
      internalField.Append(new UnsignedField<2>(this, modelData.timers[i].countdownBeep));
      internalField.Append(new BoolField<1>(this, modelData.timers[i].minuteBeep));
      internalField.Append(new UnsignedField<2>(this, modelData.timers[i].persistent));
      internalField.Append(new SpareBitsField<3>(this));
      if (IS_TARANIS(board))
        internalField.Append(new ZCharField<8>(this, modelData.timers[i].name, "Timer name"));
      else
        internalField.Append(new ZCharField<3>(this, modelData.timers[i].name, "Timer name"));
    }
    else if ((IS_ARM(board) || IS_2560(board)) && version >= 216) {
      internalField.Append(new SwitchField<8>(this, modelData.timers[i].mode, board, version, true));
      internalField.Append(new UnsignedField<16>(this, modelData.timers[i].val));
      internalField.Append(new UnsignedField<2>(this, modelData.timers[i].countdownBeep));
      internalField.Append(new BoolField<1>(this, modelData.timers[i].minuteBeep));
      internalField.Append(new UnsignedField<2>(this, modelData.timers[i].persistent));
      internalField.Append(new SpareBitsField<3>(this));
      internalField.Append(new SignedField<16>(this, modelData.timers[i].pvalue));
    }
    else if (afterrelease21March2013) {
      internalField.Append(new SwitchField<8>(this, modelData.timers[i].mode, board, version, true));
      internalField.Append(new UnsignedField<12>(this, modelData.timers[i].val));
      internalField.Append(new BoolField<1>(this, (bool &)modelData.timers[i].countdownBeep));
      internalField.Append(new BoolField<1>(this, modelData.timers[i].minuteBeep));
      if (HAS_PERSISTENT_TIMERS(board)) {
        internalField.Append(new UnsignedField<1>(this, modelData.timers[i].persistent));
        internalField.Append(new SpareBitsField<1>(this));
        internalField.Append(new SignedField<16>(this, modelData.timers[i].pvalue));
      }
      else {
        internalField.Append(new SpareBitsField<2>(this));
      }
    }
    else {
      internalField.Append(new SwitchField<8>(this, modelData.timers[i].mode, board, version, true));
      internalField.Append(new UnsignedField<16>(this, modelData.timers[i].val));
      if (HAS_PERSISTENT_TIMERS(board)) {
        internalField.Append(new UnsignedField<1>(this, modelData.timers[i].persistent));
        internalField.Append(new SpareBitsField<15>(this));
      }
    }
  }

  if (IS_ARM(board))
    internalField.Append(new UnsignedField<3>(this, modelData.telemetryProtocol));
  else
    internalField.Append(new ConversionField< UnsignedField<3> >(this, (unsigned int &)modelData.moduleData[0].protocol, &protocolsConversionTable, "Protocol", DataField::tr("OpenTX doesn't accept this telemetry protocol")));

  internalField.Append(new BoolField<1>(this, modelData.thrTrim));

  if (IS_ARM(board) && version >= 217) {
    internalField.Append(new BoolField<1>(this, modelData.noGlobalFunctions));
    internalField.Append(new UnsignedField<2>(this, modelData.trimsDisplay));
    internalField.Append(new BoolField<1>(this, modelData.frsky.ignoreSensorIds));
  }
  else if (IS_HORUS_OR_TARANIS(board) || (IS_ARM(board) && version >= 216)) {
    internalField.Append(new SpareBitsField<4>(this));
  }
  else {
    internalField.Append(new ConversionField< SignedField<4> >(this, modelData.moduleData[0].channelsCount, &channelsConversionTable, "Channels number", DataField::tr("OpenTX doesn't allow this number of channels")));
  }

  if (version >= 216)
    internalField.Append(new SignedField<3>(this, modelData.trimInc));
  else
    internalField.Append(new ConversionField< SignedField<3> >(this, modelData.trimInc, +2));

  internalField.Append(new BoolField<1>(this, modelData.disableThrottleWarning));

  if (IS_HORUS_OR_TARANIS(board) || (IS_ARM(board) && version >= 216))
    internalField.Append(new BoolField<1>(this, modelData.displayChecklist));
  else
    internalField.Append(new BoolField<1>(this, modelData.moduleData[0].ppm.pulsePol));

  internalField.Append(new BoolField<1>(this, modelData.extendedLimits));
  internalField.Append(new BoolField<1>(this, modelData.extendedTrims));
  internalField.Append(new BoolField<1>(this, modelData.throttleReversed));

  if (!IS_ARM(board) || version < 216) {
    internalField.Append(new ConversionField< SignedField<8> >(this, modelData.moduleData[0].ppm.delay, exportPpmDelay, importPpmDelay));
  }

  if (IS_ARM(board) || IS_2560(board))
    internalField.Append(new UnsignedField<16>(this, modelData.beepANACenter));
  else
    internalField.Append(new UnsignedField<8>(this, modelData.beepANACenter));

  for (int i=0; i<MAX_MIXERS(board, version); i++)
    internalField.Append(new MixField(this, modelData.mixData[i], board, version, &modelData));
  for (int i=0; i<MAX_CHANNELS(board, version); i++)
    internalField.Append(new LimitField(this, modelData.limitData[i], board, version));
  for (int i=0; i<MAX_EXPOS(board, version); i++)
    internalField.Append(new InputField(this, modelData.expoData[i], board, version));
  internalField.Append(new CurvesField(this, modelData.curves, board, version));
  for (int i=0; i<MAX_LOGICAL_SWITCHES(board, version); i++)
    internalField.Append(new LogicalSwitchField(this, modelData.logicalSw[i], board, version, variant, &modelData));
  for (int i=0; i<MAX_CUSTOM_FUNCTIONS(board, version); i++) {
    if (IS_ARM(board))
      internalField.Append(new ArmCustomFunctionField(this, modelData.customFn[i], board, version, variant));
    else
      internalField.Append(new AvrCustomFunctionField(this, modelData.customFn[i], board, version, variant));
  }
  internalField.Append(new HeliField(this, modelData.swashRingData, board, version, variant));
  for (int i=0; i<MAX_FLIGHT_MODES(board, version); i++) {
    internalField.Append(new FlightModeField(this, modelData.flightModeData[i], i, board, version));
  }

  if (!IS_ARM(board) || version < 216) {
    internalField.Append(new SignedField<8>(this, modelData.moduleData[0].ppm.frameLength));
  }

  internalField.Append(new UnsignedField<8>(this, modelData.thrTraceSrc, "Throttle Source"));

  if (!afterrelease21March2013) {
    internalField.Append(new UnsignedField<8>(this, modelData.moduleData[0].modelId));
  }

  if (IS_HORUS(board))
    internalField.Append(new SwitchesWarningField<32>(this, modelData.switchWarningStates, board, version));
  else if (IS_TARANIS_X9E(board))
    internalField.Append(new SwitchesWarningField<64>(this, modelData.switchWarningStates, board, version));
  else if (IS_TARANIS(board))
    internalField.Append(new SwitchesWarningField<16>(this, modelData.switchWarningStates, board, version));
  else
    internalField.Append(new SwitchesWarningField<8>(this, modelData.switchWarningStates, board, version));


  if (IS_TARANIS_X9E(board))
    internalField.Append(new UnsignedField<32>(this, modelData.switchWarningEnable));
  else if (!IS_HORUS(board) && version >= 216)
    internalField.Append(new UnsignedField<8>(this, modelData.switchWarningEnable));

  if (board != BOARD_STOCK && (board != BOARD_M128 || version < 215)) {
    for (int i=0; i<MAX_GVARS(board, version); i++) {
      if (version >= 218) {
        internalField.Append(new ZCharField<3>(this, modelData.gvarData[i].name, "GVar name"));
        internalField.Append(new UnsignedField<12>(this, (unsigned &)modelData.gvarData[i].min));
        internalField.Append(new UnsignedField<12>(this, (unsigned &)modelData.gvarData[i].max));
        internalField.Append(new BoolField<1>(this, modelData.gvarData[i].popup));
        internalField.Append(new UnsignedField<1>(this, modelData.gvarData[i].prec));
        internalField.Append(new UnsignedField<2>(this, modelData.gvarData[i].unit));
        internalField.Append(new SpareBitsField<4>(this));
      }
      else {
        internalField.Append(new ZCharField<3>(this, modelData.gvarData[i].name, "GVar name"));
        internalField.Append(new SpareBitsField<3*8>(this));
        if (version >= 216) {
          internalField.Append(new BoolField<1>(this, modelData.gvarData[i].popup));
          internalField.Append(new SpareBitsField<7>(this));
        }
      }
    }
  }
  else if (variant & GVARS_VARIANT) {
    for (int i=0; i<MAX_GVARS(board, version); i++) {
      // on M64 GVARS are common to all flight modes, and there is no name
      internalField.Append(new SignedField<16>(this, modelData.flightModeData[0].gvars[i]));
    }
  }

  if ((board != BOARD_STOCK && (board != BOARD_M128 || version < 215)) || (variant & FRSKY_VARIANT)) {
    internalField.Append(new FrskyField(this, modelData.frsky, modelData.rssiAlarms, board, version, variant));
  }
  else if ((board == BOARD_STOCK || board == BOARD_M128) && (variant & MAVLINK_VARIANT)) {
    internalField.Append(new MavlinkField(this, modelData.mavlink, board, version));
  }

  if (IS_TARANIS(board) && version < 215) {
    internalField.Append(new CharField<10>(this, modelData.bitmap, true, "Model bitmap"));
  }

  int modulesCount = 2;

  if (IS_STM32(board)) {
    modulesCount = 3;
    if (version >= 217) {
      internalField.Append(new SpareBitsField<3>(this));
      internalField.Append(new UnsignedField<3>(this, modelData.trainerMode));
      internalField.Append(new UnsignedField<2>(this, modelData.potsWarningMode));
    }
    else {
      internalField.Append(new ConversionField< SignedField<8> >(this, modelData.moduleData[1].protocol, &protocolsConversionTable, "Protocol", DataField::tr("OpenTX doesn't accept this radio protocol")));
      internalField.Append(new UnsignedField<8>(this, modelData.trainerMode));
    }
  }
  else if (IS_ARM(board)) {
    if (version >= 217) {
      modulesCount = 3;
      internalField.Append(new SpareBitsField<6>(this));
      internalField.Append(new UnsignedField<2>(this, modelData.potsWarningMode));
    }
    else if (version >= 216) {
      modulesCount = 3;
      internalField.Append(new ConversionField< SignedField<8> >(this, modelData.moduleData[0].protocol, &protocolsConversionTable, "Protocol", DataField::tr("OpenTX doesn't accept this radio protocol")));
    }
  }

  if (IS_ARM(board) && version >= 215) {
    for (int module=0; module<modulesCount; module++) {
      if (version >= 217) {
        internalField.Append(new ConversionField<SignedField<4> >(this, modelData.moduleData[module].protocol, &protocolsConversionTable, "Protocol", DataField::tr("OpenTX doesn't accept this radio protocol")));
        internalField.Append(new SignedField<4>(this, subprotocols[module]));
      }
      else {
        internalField.Append(new SignedField<8>(this, subprotocols[module]));
      }
      internalField.Append(new UnsignedField<8>(this, modelData.moduleData[module].channelsStart));
      internalField.Append(new ConversionField<SignedField<8> >(this, modelData.moduleData[module].channelsCount, -8));
      if (version >= 217) {
        internalField.Append(new UnsignedField<4>(this, modelData.moduleData[module].failsafeMode));
        internalField.Append(new UnsignedField<3>(this, modelData.moduleData[module].subType));
        internalField.Append(new BoolField<1>(this, modelData.moduleData[module].invertedSerial));
      }
      else {
        internalField.Append(new ConversionField<UnsignedField<8> >(this, modelData.moduleData[module].failsafeMode, -1));
      }
      for (int i=0; i<32; i++) {
        internalField.Append(new SignedField<16>(this, modelData.moduleData[module].failsafeChannels[i]));
      }
      if (version >= 217) {
        internalField.Append(new ConversionField< SignedField<6> >(this, modelData.moduleData[module].ppm.delay, exportPpmDelay, importPpmDelay));
        internalField.Append(new BoolField<1>(this, modelData.moduleData[module].ppm.pulsePol));
        internalField.Append(new BoolField<1>(this, modelData.moduleData[module].ppm.outputType));
        internalField.Append(new SignedField<8>(this, modelData.moduleData[module].ppm.frameLength));
      }
      else {
        internalField.Append(new ConversionField< SignedField<8> >(this, modelData.moduleData[module].ppm.delay, exportPpmDelay, importPpmDelay));
        internalField.Append(new SignedField<8>(this, modelData.moduleData[module].ppm.frameLength));
        internalField.Append(new BoolField<8>(this, modelData.moduleData[module].ppm.pulsePol));
      }
    }
  }

  if (IS_TARANIS(board) && version < 218) {
    for (int i=0; i<MAX_CURVES(board, version); i++) {
      internalField.Append(new ZCharField<6>(this, modelData.curves[i].name, "Curve name"));
    }
  }

  if (IS_STM32(board)) {
    if (version >= 218) {
      for (int i=0; i<MAX_SCRIPTS(board); i++) {
        ScriptData & script = modelData.scriptData[i];
        internalField.Append(new CharField<6>(this, script.filename, true, "Script filename"));
        internalField.Append(new ZCharField<6>(this, script.name, "Script name"));
        for (int j=0; j<6; j++) {
          internalField.Append(new SignedField<16>(this, script.inputs[j]));
        }
      }
    }
    else if (version >= 217) {
      for (int i=0; i<7; i++) {
        ScriptData & script = modelData.scriptData[i];
        internalField.Append(new CharField<8>(this, script.filename, true, "Script filename"));
        internalField.Append(new ZCharField<8>(this, script.name, "Script name"));
        for (int j=0; j<8; j++) {
          internalField.Append(new SignedField<8>(this, script.inputs[j]));
        }
      }
    }
    else if (version >= 216) {
      for (int i=0; i<7; i++) {
        ScriptData & script = modelData.scriptData[i];
        internalField.Append(new CharField<10>(this, script.filename, true, "Script filename"));
        internalField.Append(new ZCharField<10>(this, script.name, "Script name"));
        for (int j=0; j<10; j++) {
          internalField.Append(new SignedField<8>(this, script.inputs[j]));
        }
      }
    }
  }

  if (IS_ARM(board) && version >= 216) {
    for (int i=0; i<32; i++) {
      if (HAS_LARGE_LCD(board))
        internalField.Append(new ZCharField<4>(this, modelData.inputNames[i], "Input name"));
      else
        internalField.Append(new ZCharField<3>(this, modelData.inputNames[i], "Input name"));
    }
  }

  if (IS_ARM(board) && version >= 217) {
    for (int i=0; i<8; i++) {
      if (i < MAX_POTS(board, version)+MAX_SLIDERS(board))
        internalField.Append(new BoolField<1>(this, modelData.potsWarningEnabled[i]));
      else
        internalField.Append(new SpareBitsField<1>(this));
    }
  }
  else if (IS_ARM(board) && version >= 216) {
    for (int i=0; i<6; i++) {
      if (i < MAX_POTS(board, version)+MAX_SLIDERS(board))
        internalField.Append(new BoolField<1>(this, modelData.potsWarningEnabled[i]));
      else
        internalField.Append(new SpareBitsField<1>(this));
    }
    internalField.Append(new UnsignedField<2>(this, modelData.potsWarningMode));
  }

  if (IS_ARM(board) && version >= 216) {
    for (int i=0; i < MAX_POTS(board, version)+MAX_SLIDERS(board); i++) {
      internalField.Append(new SignedField<8>(this, modelData.potPosition[i]));
    }
  }

  if (IS_ARM(board) && version == 216) {
    internalField.Append(new SpareBitsField<16>(this));
  }

  if (IS_SKY9X(board) && version >= 217) {
    internalField.Append(new SpareBitsField<8>(this));
    internalField.Append(new SpareBitsField<8>(this));
  }

  if (IS_ARM(board) && version >= 217) {
    for (int i=0; i<MAX_TELEMETRY_SENSORS(board, version); ++i) {
      internalField.Append(new SensorField(this, modelData.sensorData[i], board, version));
    }
  }

  if (IS_TARANIS_X9E(board)) {
    internalField.Append(new UnsignedField<8>(this, modelData.toplcdTimer));
  }

  if (IS_HORUS(board)) {
    for (int i = 0; i < 5; i++) {
      internalField.Append(new CharField<610>(this, modelData.customScreenData[i], false, "Custom screen blob"));
    }
    internalField.Append(new CharField<216>(this, modelData.topbarData, false, "Top bar blob"));
    internalField.Append(new SpareBitsField<8>(this)); // current view
  }
}

void OpenTxModelData::beforeExport()
{
  // qDebug() << QString("before export model") << modelData.name;

  for (int module=0; module<3; module++) {
    if ((modelData.moduleData[module].protocol >= PULSES_PXX_XJT_X16 && modelData.moduleData[module].protocol <= PULSES_PXX_XJT_LR12) ||
      modelData.moduleData[module].protocol == PULSES_PXX_R9M) {
      if (! (modelData.moduleData[module].protocol == PULSES_PXX_R9M)) {
        subprotocols[module] = modelData.moduleData[module].protocol - PULSES_PXX_XJT_X16;
      }
      int pxxByte = (modelData.moduleData[module].pxx.power & 0x03)
                    | modelData.moduleData[module].pxx.receiver_telem_off << 4
                    | modelData.moduleData[module].pxx.receiver_channel_9_16 << 5;
      modelData.moduleData[module].ppm.delay = 300 + 50 * pxxByte;
      modelData.moduleData[module].ppm.pulsePol = modelData.moduleData[module].pxx.external_antenna;
      modelData.moduleData[module].ppm.outputType = modelData.moduleData[module].pxx.sport_out;

    }
    else if (modelData.moduleData[module].protocol >= PULSES_LP45 && modelData.moduleData[module].protocol <= PULSES_DSMX) {
      subprotocols[module] = modelData.moduleData[module].protocol - PULSES_LP45;
    }
    else if (modelData.moduleData[module].protocol == PULSES_MULTIMODULE) {
      // copy multi settings to ppm settings to get them written to the eeprom
      // (reverse the int => ms logic of the ppm delay) since only ppm is written
      subprotocols[module] = modelData.moduleData[module].multi.rfProtocol & (0x1f);
      int multiByte = ((modelData.moduleData[module].multi.rfProtocol >> 4) & 0x03) | (modelData.moduleData[module].multi.customProto << 7);
      modelData.moduleData[module].ppm.delay = 300 + 50 * multiByte;
      modelData.moduleData[module].ppm.frameLength = modelData.moduleData[module].multi.optionValue;
      modelData.moduleData[module].ppm.outputType = modelData.moduleData[module].multi.lowPowerMode;
      modelData.moduleData[module].ppm.pulsePol = modelData.moduleData[module].multi.autoBindMode;
    }
    else {
      subprotocols[module] = (module == 0 ? -1 : 0);
    }
  }

  if (IS_HORUS(board)) {
    uint32_t newSwitchWarningStates = 0;
    for (int i = 0; i < MAX_SWITCHES(board, version); i++) {
      uint8_t value = (modelData.switchWarningStates >> (2*i)) & 0x03;
      if (!(modelData.switchWarningEnable & (1 << i))) {
        newSwitchWarningStates |= (value + 1) << (3*i);
      }
    }
    modelData.switchWarningStates = newSwitchWarningStates;
  }
}

void OpenTxModelData::afterImport()
{
  qCDebug(eepromImport) << QString("OpenTxModelData::afterImport()") << modelData.name;

  if (IS_TARANIS(board) && version < 216) {
    for (unsigned int i=0; i<CPN_MAX_STICKS; i++) {
      for (int j=0; j<64; j++) {
        ExpoData * expo = &modelData.expoData[j];
        if (expo->mode == INPUT_MODE_BOTH && expo->chn == i && expo->flightModes == 0 && expo->swtch.type == SWITCH_TYPE_NONE)
          break;
        if (expo->mode == 0 || expo->chn > i) {
          ExpoData * newExpo = modelData.insertInput(j);
          newExpo->mode = INPUT_MODE_BOTH;
          newExpo->srcRaw = RawSource(SOURCE_TYPE_STICK, i);
          newExpo->chn = i;
          newExpo->weight = 100;
          break;
        }
      }
      strncpy(modelData.inputNames[i], getCurrentFirmware()->getAnalogInputName(i).toLatin1().constData(), sizeof(modelData.inputNames[i])-1);
    }
  }

  if (IS_HORUS(board)) {
    modelData.moduleData[0].protocol = PULSES_PXX_XJT_X16;
  }

  for (int module=0; module<3; module++) {
    if (modelData.moduleData[module].protocol == PULSES_PXX_XJT_X16 || modelData.moduleData[module].protocol == PULSES_LP45) {
      if (subprotocols[module] >= 0)
        modelData.moduleData[module].protocol += subprotocols[module];
      else
        modelData.moduleData[module].protocol = PULSES_OFF;
    }
    else if (modelData.moduleData[module].protocol == PULSES_MULTIMODULE) {
      // Copy data from ppm struct to multi struct
      unsigned int multiByte = (unsigned  int)((modelData.moduleData[module].ppm.delay - 300) / 50);
      modelData.moduleData[module].multi.rfProtocol = (subprotocols[module] & 0x0f) | ((multiByte & 0x3) << 4);
      modelData.moduleData[module].multi.customProto = (multiByte & 0x80) == 0x80;
      modelData.moduleData[module].multi.optionValue = modelData.moduleData[module].ppm.frameLength;
      modelData.moduleData[module].multi.lowPowerMode = modelData.moduleData[module].ppm.outputType;
      modelData.moduleData[module].multi.autoBindMode = modelData.moduleData[module].ppm.pulsePol;
    }

    if ((modelData.moduleData[module].protocol >= PULSES_PXX_XJT_X16 && modelData.moduleData[module].protocol <= PULSES_PXX_XJT_LR12) ||
        modelData.moduleData[module].protocol == PULSES_PXX_R9M) {
      // Do the same for pxx
      unsigned int pxxByte = (unsigned  int)((modelData.moduleData[module].ppm.delay - 300) / 50);
      modelData.moduleData[module].pxx.power = pxxByte & 0x03;
      modelData.moduleData[module].pxx.receiver_telem_off = static_cast<bool>(pxxByte & (1 << 4));
      modelData.moduleData[module].pxx.receiver_channel_9_16 = static_cast<bool>(pxxByte & (1 << 5));
      modelData.moduleData[module].pxx.sport_out = modelData.moduleData[module].ppm.outputType;
      modelData.moduleData[module].pxx.external_antenna = modelData.moduleData[module].ppm.pulsePol;
    }
  }

  if (IS_TARANIS(board) && version < 217 && modelData.moduleData[1].protocol != PULSES_OFF) {
    modelData.moduleData[1].modelId = modelData.moduleData[0].modelId;
  }

  if (IS_HORUS(board)) {
    uint32_t newSwitchWarningStates = 0;
    for (int i = 0; i < MAX_SWITCHES(board, version); i++) {
      uint8_t value = (modelData.switchWarningStates >> (3*i)) & 0x07;
      if (value == 0)
        modelData.switchWarningEnable |= (1 << i);
      else
        newSwitchWarningStates |= ((value & 0x03) - 1) << (2*i);
    }
    modelData.switchWarningStates = newSwitchWarningStates;
  }
}

OpenTxGeneralData::OpenTxGeneralData(GeneralSettings & generalData, Board::Type board, unsigned int version, unsigned int variant):
  TransformedField(NULL, internalField),
  internalField(this, "General Settings"),
  generalData(generalData),
  board(board),
  version(version),
  inputsCount(CPN_MAX_STICKS+MAX_POTS(board, version)+MAX_SLIDERS(board)+MAX_MOUSE_ANALOGS(board))
{
  qCDebug(eepromImport) << QString("OpenTxGeneralData::OpenTxGeneralData(board: %1, version:%2, variant:%3)").arg(board).arg(version).arg(variant);

  generalData.version = version;
  generalData.variant = variant;

  internalField.Append(new UnsignedField<8>(this, generalData.version));
  if (version >= 213 || (!IS_ARM(board) && version >= 212))
    internalField.Append(new UnsignedField<16>(this, generalData.variant));

  if (version >= 216) {
    for (int i=0; i<inputsCount; i++) {
      internalField.Append(new SignedField<16>(this, generalData.calibMid[i]));
      internalField.Append(new SignedField<16>(this, generalData.calibSpanNeg[i]));
      internalField.Append(new SignedField<16>(this, generalData.calibSpanPos[i]));
    }
  }
  else {
    for (int i=0; i<inputsCount; i++) {
      if (!IS_TARANIS(board) || i!=6)
        internalField.Append(new SignedField<16>(this, generalData.calibMid[i]));
    }
    for (int i=0; i<inputsCount; i++) {
      if (!IS_TARANIS(board) || i!=6)
        internalField.Append(new SignedField<16>(this, generalData.calibSpanNeg[i]));
    }
    for (int i=0; i<inputsCount; i++) {
      if (!IS_TARANIS(board) || i!=6)
        internalField.Append(new SignedField<16>(this, generalData.calibSpanPos[i]));
    }
  }

  internalField.Append(new UnsignedField<16>(this, chkSum));
  if (!IS_HORUS(board)) {
    internalField.Append(new UnsignedField<8>(this, generalData.currModelIndex));
    internalField.Append(new UnsignedField<8>(this, generalData.contrast));
  }
  internalField.Append(new UnsignedField<8>(this, generalData.vBatWarn));
  internalField.Append(new SignedField<8>(this, generalData.txVoltageCalibration));
  internalField.Append(new SignedField<8>(this, generalData.backlightMode));

  for (int i=0; i<CPN_MAX_STICKS; i++) {
    internalField.Append(new SignedField<16>(this, generalData.trainer.calib[i]));
  }
  for (int i=0; i<CPN_MAX_STICKS; i++) {
    internalField.Append(new UnsignedField<6>(this, generalData.trainer.mix[i].src));
    internalField.Append(new UnsignedField<2>(this, generalData.trainer.mix[i].mode));
    internalField.Append(new SignedField<8>(this, generalData.trainer.mix[i].weight));
  }

  internalField.Append(new UnsignedField<8>(this, generalData.view, 0, MAX_VIEWS(board)-1));

  internalField.Append(new SpareBitsField<2>(this)); // TODO buzzerMode?
  internalField.Append(new BoolField<1>(this, generalData.fai));
  internalField.Append(new SignedField<2>(this, (int &)generalData.beeperMode));
  internalField.Append(new BoolField<1>(this, generalData.flashBeep));
  internalField.Append(new BoolField<1>(this, generalData.disableMemoryWarning));
  internalField.Append(new BoolField<1>(this, generalData.disableAlarmWarning));

  internalField.Append(new UnsignedField<2>(this, generalData.stickMode));
  internalField.Append(new SignedField<5>(this, generalData.timezone));
  if (version >= 217 && IS_HORUS_OR_TARANIS(board)) {
    internalField.Append(new BoolField<1>(this, generalData.adjustRTC));
  }
  else {
    internalField.Append(new SpareBitsField<1>(this));
  }

  internalField.Append(new UnsignedField<8>(this, generalData.inactivityTimer));
  if (IS_9X(board)) {
    if (version >= 215) {
      internalField.Append(new UnsignedField<3>(this, generalData.mavbaud));
    }
    else {
      internalField.Append(new SpareBitsField<1>(this));
      internalField.Append(new BoolField<1>(this, generalData.minuteBeep));
      internalField.Append(new BoolField<1>(this, generalData.preBeep));
    }
  }
  else {
    internalField.Append(new SpareBitsField<3>(this));
  }
  if (version >= 216 && IS_HORUS(board))
    internalField.Append(new SpareBitsField<3>(this));
  else if (version >= 216 && IS_TARANIS(board))
    internalField.Append(new SignedField<3>(this, generalData.splashDuration));
  else if (version >= 213 || (!IS_ARM(board) && version >= 212))
    internalField.Append(new UnsignedField<3>(this, generalData.splashMode)); // TODO
  else
    internalField.Append(new SpareBitsField<3>(this));
  internalField.Append(new SignedField<2>(this, (int &)generalData.hapticMode));

  if (IS_ARM(board))
    internalField.Append(new SignedField<8>(this, generalData.switchesDelay));
  else
    internalField.Append(new SpareBitsField<8>(this)); // TODO blOffBright + blOnBright

  internalField.Append(new UnsignedField<8>(this, generalData.backlightDelay));
  internalField.Append(new UnsignedField<8>(this, generalData.templateSetup));
  internalField.Append(new SignedField<8>(this, generalData.PPM_Multiplier));
  internalField.Append(new SignedField<8>(this, generalData.hapticLength));

  if (version < 216 || (version < 218 && !IS_9X(board)) || (!IS_9X(board) && !IS_TARANIS(board) && !IS_HORUS(board))) {
    internalField.Append(new UnsignedField<8>(this, generalData.reNavigation));
  }

  if (version >= 216 && !IS_TARANIS(board) && !IS_HORUS(board)) {
    internalField.Append(new UnsignedField<8>(this, generalData.stickReverse));
  }

  internalField.Append(new SignedField<3>(this, generalData.beeperLength));
  internalField.Append(new SignedField<3>(this, generalData.hapticStrength));
  internalField.Append(new UnsignedField<1>(this, generalData.gpsFormat));
  internalField.Append(new SpareBitsField<1>(this)); // unexpectedShutdown

  internalField.Append(new UnsignedField<8>(this, generalData.speakerPitch));

  if (IS_ARM(board))
    internalField.Append(new ConversionField< SignedField<8> >(this, generalData.speakerVolume, -12, 0, 0, 23, "Volume"));
  else
    internalField.Append(new ConversionField< SignedField<8> >(this, generalData.speakerVolume, -7, 0, 0, 7, "Volume"));

  if (version >= 214 || (!IS_ARM(board) && version >= 213)) {
    internalField.Append(new SignedField<8>(this, generalData.vBatMin));
    internalField.Append(new SignedField<8>(this, generalData.vBatMax));
  }

  if (IS_ARM(board)) {
    internalField.Append(new UnsignedField<8>(this, generalData.backlightBright));
    if (version < 218) internalField.Append(new SignedField<8>(this, generalData.txCurrentCalibration));
    if (version >= 213) {
      if (version < 218) internalField.Append(new SignedField<8>(this, generalData.temperatureWarn));
      if (version < 218) internalField.Append(new UnsignedField<8>(this, generalData.mAhWarn));
      if (version < 218) internalField.Append(new UnsignedField<16>(this, generalData.mAhUsed));
      internalField.Append(new UnsignedField<32>(this, generalData.globalTimer));
      if (version < 218) internalField.Append(new SignedField<8>(this, generalData.temperatureCalib));
      internalField.Append(new UnsignedField<4>(this, generalData.bluetoothBaudrate));
      internalField.Append(new UnsignedField<4>(this, generalData.bluetoothMode));
      if (version < 218) internalField.Append(new BoolField<8>(this, generalData.optrexDisplay));
      if (version < 218) internalField.Append(new UnsignedField<8>(this, generalData.sticksGain));
    }
    if (version >= 214) {
      if (version < 218) internalField.Append(new UnsignedField<8>(this, generalData.rotarySteps));
      internalField.Append(new UnsignedField<8>(this, generalData.countryCode));
      internalField.Append(new UnsignedField<1>(this, generalData.imperial));
      if (version >= 218) {
        internalField.Append(new BoolField<1>(this, generalData.jitterFilter));
        internalField.Append(new BoolField<1>(this, generalData.disableRssiPoweroffAlarm));
        internalField.Append(new UnsignedField<2>(this, generalData.usbMode));
        internalField.Append(new SpareBitsField<3>(this));
      }
      else {
        internalField.Append(new SpareBitsField<7>(this));
      }
    }
    if (version >= 215) {
      internalField.Append(new CharField<2>(this, generalData.ttsLanguage, true, "TTS language"));
      if (version >= 218) {
        internalField.Append(new SignedField<4>(this, generalData.beepVolume));
        internalField.Append(new SignedField<4>(this, generalData.wavVolume));
        internalField.Append(new SignedField<4>(this, generalData.varioVolume));
        internalField.Append(new SignedField<4>(this, generalData.backgroundVolume));
      }
      else {
        internalField.Append(new SignedField<8>(this, generalData.beepVolume));
        internalField.Append(new SignedField<8>(this, generalData.wavVolume));
        internalField.Append(new SignedField<8>(this, generalData.varioVolume));
      }
      if (version >= 216) {
        internalField.Append(new SignedField<8>(this, generalData.varioPitch));
        internalField.Append(new SignedField<8>(this, generalData.varioRange));
        internalField.Append(new SignedField<8>(this, generalData.varioRepeat));
      }
      if (version < 218) internalField.Append(new SignedField<8>(this, generalData.backgroundVolume));
    }
    if (version >= 218) {
      for (int i=0; i<MAX_CUSTOM_FUNCTIONS(board, version); i++) {
        internalField.Append(new ArmCustomFunctionField(this, generalData.customFn[i], board, version, variant));
      }
    }

    if (IS_STM32(board) && version >= 216) {
      if (version >= 218) {
        internalField.Append(new UnsignedField<4>(this, generalData.hw_uartMode));
        for (uint8_t i=0; i<4; i++) {
          internalField.Append(new UnsignedField<1>(this, generalData.sliderConfig[i]));
        }
      }
      else if (version >= 217) {
        internalField.Append(new UnsignedField<6>(this, generalData.hw_uartMode));
        if (IS_TARANIS_X9E(board)) {
          internalField.Append(new UnsignedField<1>(this, generalData.sliderConfig[2]));
          internalField.Append(new UnsignedField<1>(this, generalData.sliderConfig[3]));
        }
        else {
          internalField.Append(new SpareBitsField<2>(this));
        }
      }
      else {
        internalField.Append(new UnsignedField<8>(this, generalData.hw_uartMode));
      }
      if (IS_HORUS(board)) {
        for (int i=0; i<16; i++) {
          if (i < MAX_SWITCHES(board, version))
            internalField.Append(new UnsignedField<2>(this, generalData.switchConfig[i]));
          else
            internalField.Append(new SpareBitsField<2>(this));
        }
      }
      for (int i=0; i<4; i++) {
        if (i < MAX_POTS(board, version))
          internalField.Append(new UnsignedField<2>(this, generalData.potConfig[i]));
        else
          internalField.Append(new SpareBitsField<2>(this));
      }
      if (!IS_HORUS(board)) {
        internalField.Append(new UnsignedField<8>(this, generalData.backlightColor));
      }
    }
    else if (IS_SKY9X(board) && version >= 218) {
      internalField.Append(new SignedField<8>(this, generalData.txCurrentCalibration));
      internalField.Append(new SignedField<8>(this, generalData.temperatureWarn));
      internalField.Append(new UnsignedField<8>(this, generalData.mAhWarn));
      internalField.Append(new UnsignedField<16>(this, generalData.mAhUsed));
      internalField.Append(new SignedField<8>(this, generalData.temperatureCalib));
      internalField.Append(new BoolField<8>(this, generalData.optrexDisplay));
      internalField.Append(new UnsignedField<8>(this, generalData.sticksGain));
      internalField.Append(new UnsignedField<8>(this, generalData.rotarySteps));
    }

    if (IS_TARANIS_X9E(board))
      internalField.Append(new SpareBitsField<64>(this)); // switchUnlockStates
    else if (IS_TARANIS(board))
      internalField.Append(new SpareBitsField<16>(this)); // switchUnlockStates

    if (version == 217) {
      for (int i=0; i<MAX_CUSTOM_FUNCTIONS(board, version); i++) {
        internalField.Append(new ArmCustomFunctionField(this, generalData.customFn[i], board, version, variant));
      }
    }

    if (IS_HORUS(board)) {
      for (int i=0; i<MAX_SWITCHES(board, version); ++i) {
        internalField.Append(new ZCharField<3>(this, generalData.switchName[i], "Switch name"));
      }
      for (int i=0; i<CPN_MAX_STICKS; ++i) {
        internalField.Append(new ZCharField<3>(this, generalData.stickName[i], "Stick name"));
      }
      for (int i=0; i<MAX_POTS(board, version); ++i) {
        internalField.Append(new ZCharField<3>(this, generalData.potName[i], "Pot name"));
      }
      for (int i=0; i<MAX_SLIDERS(board); ++i) {
        internalField.Append(new ZCharField<3>(this, generalData.sliderName[i], "Slider name"));
      }
      internalField.Append(new CharField<17>(this, generalData.currModelFilename, true, "Current model filename"));
    }
    else if (IS_TARANIS(board) && version >= 217) {
      for (int i=0; i<MAX_SWITCH_SLOTS(board, version); i++) {
        if (i < MAX_SWITCHES(board, version))
          internalField.Append(new UnsignedField<2>(this, generalData.switchConfig[i]));
        else
          internalField.Append(new SpareBitsField<2>(this));
      }
      for (int i=0; i<MAX_SWITCHES(board, version); ++i) {
        internalField.Append(new ZCharField<3>(this, generalData.switchName[i], "Switch name"));
      }
      for (int i=0; i<CPN_MAX_STICKS; ++i) {
        internalField.Append(new ZCharField<3>(this, generalData.stickName[i], "Stick name"));
      }
      for (int i=0; i<MAX_POTS(board, version); ++i) {
        internalField.Append(new ZCharField<3>(this, generalData.potName[i], "Pot name"));
      }
      for (int i=0; i<MAX_SLIDERS(board); ++i) {
        internalField.Append(new ZCharField<3>(this, generalData.sliderName[i], "Slider name"));
      }
    }

    if (IS_HORUS(board)) {
      internalField.Append(new SpareBitsField<1>(this));
      internalField.Append(new UnsignedField<7>(this, generalData.backlightOffBright));
      internalField.Append(new ZCharField<10>(this, generalData.bluetoothName, "Bluetooth name"));
    }
    else if (IS_TARANIS_X9E(board) && version >= 217) {
      internalField.Append(new UnsignedField<8>(this, generalData.bluetoothMode));
      internalField.Append(new ZCharField<10>(this, generalData.bluetoothName, "Bluetooth name"));
    }

    if (IS_HORUS(board)) {
      internalField.Append(new CharField<8>(this, generalData.themeName, true, "Theme name"));
      for (int i=0; i<5; i++) {
        internalField.Append(new CharField<8>(this, (char *)generalData.themeOptionValue[i], true, "Theme blob"));
      }
    }
  }
}

void OpenTxGeneralData::beforeExport()
{
  uint16_t sum = 0;
  if (version >= 216) {
    int count = 0;
    for (int i=0; i<inputsCount; i++) {
      sum += generalData.calibMid[i];
      if (++count == 12) break;
      sum += generalData.calibSpanNeg[i];
      if (++count == 12) break;
      sum += generalData.calibSpanPos[i];
      if (++count == 12) break;
    }
  }
  else {
    for (int i=0; i<inputsCount; i++)
      sum += generalData.calibMid[i];
    for (int i=0; i<5; i++)
      sum += generalData.calibSpanNeg[i];
  }
  chkSum = sum;
}

void OpenTxGeneralData::afterImport()
{
  if (IS_TARANIS(board) && version < 217) {
    generalData.potConfig[0] = POT_WITH_DETENT;
    generalData.potConfig[1] = POT_WITH_DETENT;
  }
}
