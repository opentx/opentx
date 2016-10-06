#include <stdlib.h>
#include <algorithm>
#include "helpers.h"
#include "opentxeeprom.h"
#include <QObject>
#include "customdebug.h"

#define IS_DBLEEPROM(board, version)          ((IS_2560(board) || board==BOARD_M128) && version >= 213)
// Macro used for Gruvin9x board and M128 board between versions 213 and 214 (when there were stack overflows!)
#define IS_DBLRAM(board, version)             ((IS_2560(board) && version >= 213) || (board==BOARD_M128 && version >= 213 && version <= 214))

#define HAS_PERSISTENT_TIMERS(board)          (IS_ARM(board) || IS_2560(board))
#define HAS_LARGE_LCD(board)                  IS_TARANIS(board)
#define MAX_VIEWS(board)                      (HAS_LARGE_LCD(board) ? 2 : 256)
#define MAX_POTS(board, version)              (IS_TARANIS(board) ? (IS_TARANIS_X9E(board) ? 4 : (version >= 216 ? 3 : 2)) : 3)
#define MAX_SLIDERS(board)                    (IS_TARANIS(board) ? (IS_TARANIS_X9E(board) ? 4 : 2) : 0)
#define MAX_SWITCHES(board, version)          (IS_TARANIS(board) ? (IS_TARANIS_X9E(board) ? 18 : 8) : 7)
#define MAX_SWITCHES_POSITION(board, version) (IS_TARANIS_X9E(board) ? 18*3 : (IS_TARANIS(board) ? 8*3 : 9))
#define MAX_ROTARY_ENCODERS(board)            (IS_2560(board) ? 2 : (IS_SKY9X(board) ? 1 : 0))
#define MAX_FLIGHT_MODES(board, version)      (IS_ARM(board) ? 9 :  (IS_DBLRAM(board, version) ? 6 :  5))
#define MAX_TIMERS(board, version)            ((IS_ARM(board) && version >= 217) ? 3 : 2)
#define MAX_MIXERS(board, version)            (IS_ARM(board) ? 64 : 32)
#define MAX_CHANNELS(board, version)          (IS_ARM(board) ? 32 : 16)
#define MAX_EXPOS(board, version)             (IS_ARM(board) ? ((IS_TARANIS(board) && version >= 216) ? 64 : 32) : (IS_DBLRAM(board, version) ? 16 : 14))
#define MAX_LOGICAL_SWITCHES(board, version)  (IS_ARM(board) ? (version >= 218 ? 64 : 32) : ((IS_DBLEEPROM(board, version) && version<217) ? 15 : 12))
#define MAX_CUSTOM_FUNCTIONS(board, version)  (IS_ARM(board) ? (version >= 216 ? 64 : 32) : (IS_DBLEEPROM(board, version) ? 24 : 16))
#define MAX_CURVES(board, version)            (IS_ARM(board) ? ((IS_TARANIS(board) && version >= 216) ? 32 : 16) : 8)
#define MAX_GVARS(board, version)             ((IS_ARM(board) && version >= 216) ? 9 : 5)
#define MAX_TELEMETRY_SENSORS(board, version) (32)
#define NUM_PPM_INPUTS(board, version)        ((IS_ARM(board) && version >= 216) ? 16 : 8)
#define ROTENC_COUNT(board, version)          (IS_ARM(board) ? ((IS_TARANIS(board) && version >= 218) ? 0 : 1) : (IS_2560(board) ? 2 : 0))

#define IS_AFTER_RELEASE_21_MARCH_2013(board, version) (version >= 214 || (!IS_ARM(board) && version >= 213))
#define IS_AFTER_RELEASE_23_MARCH_2013(board, version) (version >= 214 || (board==BOARD_STOCK && version >= 213))

inline int switchIndex(int i, BoardEnum board, unsigned int version)
{
  bool afterrelease21March2013 = IS_AFTER_RELEASE_21_MARCH_2013(board, version);
  if (!IS_TARANIS(board) && afterrelease21March2013)
    return (i<=3 ? i+3 : (i<=6 ? i-3 : i));
  else
    return i;
}

class SwitchesConversionTable: public ConversionTable {

  public:
    SwitchesConversionTable(BoardEnum board, unsigned int version, bool timer=false)
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

      if (IS_TARANIS(board) && version >= 216) {
        for (int i=1; i<=MAX_POTS(board, version)*6; i++) {
          addConversion(RawSwitch(SWITCH_TYPE_MULTIPOS_POT, -i), -val+offset);
          addConversion(RawSwitch(SWITCH_TYPE_MULTIPOS_POT, i), val++);
        }
      }

      if (version >= 216) {
        for (int i=1; i<=8; i++) {
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
        Cache(BoardEnum board, unsigned int version, unsigned long flags, SwitchesConversionTable * table):
          board(board),
          version(version),
          flags(flags),
          table(table)
        {
        }
        BoardEnum board;
        unsigned int version;
        unsigned long flags;
        SwitchesConversionTable * table;
    };

    static std::list<Cache> internalCache;

  public:

    static SwitchesConversionTable * getInstance(BoardEnum board, unsigned int version, unsigned long flags=0)
    {
      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache element = *it;
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
        Cache element = *it;
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
    SourcesConversionTable(BoardEnum board, unsigned int version, unsigned int variant, unsigned long flags=0)
    {
      bool afterrelease21March2013 = IS_AFTER_RELEASE_21_MARCH_2013(board, version);

      int val=0;

      if (!(flags & FLAG_NONONE)) {
        addConversion(RawSource(SOURCE_TYPE_NONE), val++);
      }

      if (IS_TARANIS(board) && version >= 216) {
        for (int i=0; i<32; i++) {
          addConversion(RawSource(SOURCE_TYPE_VIRTUAL_INPUT, i), val++);
        }
        for (int i=0; i<7; i++) {
          for (int j=0; j<6; j++) {
            addConversion(RawSource(SOURCE_TYPE_LUA_OUTPUT, i*16+j), val++);
          }
        }
      }

      for (int i=0; i<NUM_STICKS+MAX_POTS(board, version)+MAX_SLIDERS(board); i++) {
        addConversion(RawSource(SOURCE_TYPE_STICK, i), val++);
      }

      for (int i=0; i<MAX_ROTARY_ENCODERS(board); i++) {
        addConversion(RawSource(SOURCE_TYPE_ROTARY_ENCODER, 0), val++);
      }

      if (!afterrelease21March2013) {
        for (int i=0; i<NUM_STICKS; i++) {
          addConversion(RawSource(SOURCE_TYPE_TRIM, i), val++);
        }
      }

      addConversion(RawSource(SOURCE_TYPE_MAX), val++);

      if (afterrelease21March2013) {
        for (int i=0; i<3; i++)
          addConversion(RawSource(SOURCE_TYPE_CYC, i), val++);
      }

      if (afterrelease21March2013) {
        for (int i=0; i<NUM_STICKS; i++)
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
          for (int i=0; i<C9X_MAX_SENSORS*3; ++i) {
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
              if (i==TELEMETRY_SOURCE_TX_TIME || i==TELEMETRY_SOURCE_SWR || i==TELEMETRY_SOURCE_A3 || i==TELEMETRY_SOURCE_A4 || i==TELEMETRY_SOURCE_ASPEED || i==TELEMETRY_SOURCE_DTE || i==TELEMETRY_SOURCE_CELL_MIN || i==TELEMETRY_SOURCE_CELLS_MIN || i==TELEMETRY_SOURCE_VFAS_MIN)
                continue;
            }
            if (!IS_ARM(board)) {
              if (i==TELEMETRY_SOURCE_TX_TIME || i==TELEMETRY_SOURCE_SWR || i==TELEMETRY_SOURCE_A3 || i==TELEMETRY_SOURCE_A4 || i==TELEMETRY_SOURCE_A3_MIN || i==TELEMETRY_SOURCE_A4_MIN)
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
        Cache(BoardEnum board, unsigned int version, unsigned int variant, unsigned long flags, SourcesConversionTable * table):
          board(board),
          version(version),
          variant(variant),
          flags(flags),
          table(table)
        {
        }
        BoardEnum board;
        unsigned int version;
        unsigned int variant;
        unsigned long flags;
        SourcesConversionTable * table;
    };
    static std::list<Cache> internalCache;

  public:

    static SourcesConversionTable * getInstance(BoardEnum board, unsigned int version, unsigned int variant, unsigned long flags=0)
    {
      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache element = *it;
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
        Cache element = *it;
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
    SwitchField(RawSwitch & sw, BoardEnum board, unsigned int version, unsigned long flags=0):
      ConversionField< SignedField<N> >(_switch, SwitchesConversionTable::getInstance(board, version, flags),  QObject::tr("Switch").toLatin1(),
          QObject::tr("Switch ").toLatin1()+ sw.toString()+  QObject::tr(" cannot be exported on this board!").toLatin1()),
      sw(sw),
      _switch(0)
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
      eepromImportDebug() << QString("imported %1: %2").arg(ConversionField< SignedField<N> >::internalField.getName()).arg(sw.toString());
    }

  protected:
    RawSwitch & sw;
    int _switch;
};

class TelemetrySourcesConversionTable: public ConversionTable {

  public:
    TelemetrySourcesConversionTable(BoardEnum board, unsigned int version)
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
        addConversion(1+TELEMETRY_SOURCE_SWR, val++);
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
    TelemetrySourceField(RawSource & source, BoardEnum board, unsigned int version):
      ConversionField< UnsignedField<N> >(_source, &conversionTable, "Telemetry source"),
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
      eepromImportDebug() << QString("imported %1: %2").arg(ConversionField< UnsignedField<N> >::internalField.getName()).arg(source.toString());
    }

  protected:
    TelemetrySourcesConversionTable conversionTable;
    RawSource & source;
    BoardEnum board;
    unsigned int version;
    unsigned int _source;
};

template <int N>
class SourceField: public ConversionField< UnsignedField<N> > {
  public:
    SourceField(RawSource & source, BoardEnum board, unsigned int version, unsigned int variant, unsigned long flags=0):
      ConversionField< UnsignedField<N> >(_source, SourcesConversionTable::getInstance(board, version, variant, flags),
            "Source", QObject::tr("Source %1 cannot be exported on this board!").arg(source.toString())),
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
      eepromImportDebug() << QString("imported %1: %2").arg(ConversionField< UnsignedField<N> >::internalField.getName()).arg(source.toString());
    }

  protected:
    RawSource & source;
    unsigned int _source;
};


int smallGvarToEEPROM(int gvar)
{
  if (gvar < -10000) {
    gvar = 128 + gvar + 10000;
  }
  else if (gvar > 10000) {
    gvar = -128 +gvar - 10001;
  }
  return gvar;
}

int smallGvarToC9x(int gvar)
{
  if (gvar>110) {
    gvar = gvar-128 - 10000;
  }
  else if (gvar<-110) {
    gvar = gvar+128 + 10001;
  }
  return gvar;
}

void splitGvarParam(const int gvar, int & _gvar, unsigned int & _gvarParam, const BoardEnum board, const int version)
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

void concatGvarParam(int & gvar, const int _gvar, const unsigned int _gvarParam, const BoardEnum board, const int version)
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
    CurveReferenceField(CurveReference & curve, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      curve(curve),
      _curve_type(0),
      _curve_value(0)
    {
      internalField.Append(new UnsignedField<8>(_curve_type));
      internalField.Append(new SignedField<8>(_curve_value));
    }

    virtual void beforeExport()
    {
      if (curve.value != 0) {
        _curve_type = (unsigned int)curve.type;
        _curve_value = smallGvarToEEPROM(curve.value);
      }
      else {
        _curve_type = 0;
        _curve_value = 0;
      }
    }

    virtual void afterImport()
    {
      curve.type = (CurveReference::CurveRefType)_curve_type;
      curve.value = smallGvarToC9x(_curve_value);
      eepromImportDebug() << QString("imported CurveReference(%1)").arg(curve.toString());
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
    HeliField(SwashRingData & heli, BoardEnum board, unsigned int version, unsigned int variant)
    {
      if (IS_TARANIS(board) && version >= 217) {
        Append(new UnsignedField<8>(heli.type));
        Append(new UnsignedField<8>(heli.value));
        Append(new SourceField<8>(heli.collectiveSource, board, version, variant));
        Append(new SourceField<8>(heli.aileronSource, board, version, variant));
        Append(new SourceField<8>(heli.elevatorSource, board, version, variant));
        Append(new SignedField<8>(heli.collectiveWeight));
        Append(new SignedField<8>(heli.aileronWeight));
        Append(new SignedField<8>(heli.elevatorWeight));
      }
      else {
        Append(new ConversionField< SignedField<1> >(heli.elevatorWeight, exportHeliInversionWeight, importHeliInversionWeight));
        Append(new ConversionField< SignedField<1> >(heli.aileronWeight, exportHeliInversionWeight, importHeliInversionWeight));
        Append(new ConversionField< SignedField<1> >(heli.collectiveWeight, exportHeliInversionWeight, importHeliInversionWeight));
        Append(new UnsignedField<5>(heli.type));
        Append(new SourceField<8>(heli.collectiveSource, board, version, variant));
        //, FLAG_NOSWITCHES)); Fix shift in collective
        Append(new UnsignedField<8>(heli.value));
      }
    }
};

class FlightModeField: public TransformedField {
  public:
    FlightModeField(FlightModeData & phase, int index, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField("Phase"),
      phase(phase),
      index(index),
      board(board),
      version(version),
      rotencCount(ROTENC_COUNT(board, version))
    {
      if (board == BOARD_STOCK || (board==BOARD_M128 && version>=215)) {
        // On stock we use 10bits per trim
        for (int i=0; i<NUM_STICKS; i++)
          internalField.Append(new SignedField<8>(trimBase[i]));
        for (int i=0; i<NUM_STICKS; i++)
          internalField.Append(new SignedField<2>(trimExt[i]));
      }
      else if (IS_TARANIS(board) && version >= 216) {
        for (int i=0; i<NUM_STICKS; i++) {
          internalField.Append(new SignedField<11>(phase.trim[i]));
          internalField.Append(new UnsignedField<5>(trimMode[i]));
        }
      }
      else {
        for (int i=0; i<NUM_STICKS; i++) {
          internalField.Append(new SignedField<16>(trimBase[i]));
        }
      }
      if (IS_ARM(board) && version >= 218) {
        if (HAS_LARGE_LCD(board))
          internalField.Append(new ZCharField<10>(phase.name));
        else
          internalField.Append(new ZCharField<6>(phase.name));
        internalField.Append(new SwitchField<9>(phase.swtch, board, version));
        internalField.Append(new SpareBitsField<7>());
      }
      else {
        internalField.Append(new SwitchField<8>(phase.swtch, board, version));
        if (HAS_LARGE_LCD(board))
          internalField.Append(new ZCharField<10>(phase.name));
        else
          internalField.Append(new ZCharField<6>(phase.name));
      }
      if (IS_ARM(board) && version >= 214) {
        internalField.Append(new UnsignedField<8>(phase.fadeIn));
        internalField.Append(new UnsignedField<8>(phase.fadeOut));
      }
      else {
        internalField.Append(new UnsignedField<4>(phase.fadeIn));
        internalField.Append(new UnsignedField<4>(phase.fadeOut));
      }

      for (int i=0; i<rotencCount; i++) {
        internalField.Append(new SignedField<16>(phase.rotaryEncoders[i]));
      }

      if (board != BOARD_STOCK && (board != BOARD_M128 || version < 215)) {
        for (int i=0; i<MAX_GVARS(board, version); i++) {
          internalField.Append(new SignedField<16>(phase.gvars[i]));
        }
      }
    }

    virtual void beforeExport()
    {
      for (int i=0; i<NUM_STICKS; i++) {
        if (IS_TARANIS(board) && version >= 216) {
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
      for (int i=0; i<NUM_STICKS; i++) {
        if (IS_TARANIS(board) && version >= 216) {
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
      eepromImportDebug() << QString("imported %1: '%2'").arg(internalField.getName()).arg(phase.name);
    }

  protected:
    StructField internalField;
    FlightModeData & phase;
    int index;
    BoardEnum board;
    unsigned int version;
    int rotencCount;
    int trimBase[NUM_STICKS];
    int trimExt[NUM_STICKS];
    unsigned int trimMode[NUM_STICKS];
};

class MixField: public TransformedField {
  public:
    MixField(MixData & mix, BoardEnum board, unsigned int version, ModelData * model):
      TransformedField(internalField),
      internalField("Mix"),
      mix(mix),
      board(board),
      version(version),
      model(model)
    {
      if (IS_TARANIS(board) && version >= 218) {
        internalField.Append(new SignedField<11>(_weight));
        internalField.Append(new UnsignedField<5>(_destCh));
        internalField.Append(new SourceField<10>(mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new UnsignedField<1>((unsigned int &)mix.carryTrim));
        internalField.Append(new UnsignedField<2>(mix.mixWarn));
        internalField.Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
        internalField.Append(new SpareBitsField<1>());
        internalField.Append(new SignedField<14>(_offset));
        internalField.Append(new SwitchField<9>(mix.swtch, board, version));
        internalField.Append(new UnsignedField<9>(mix.flightModes));
        internalField.Append(new CurveReferenceField(mix.curve, board, version));
        internalField.Append(new UnsignedField<8>(mix.delayUp));
        internalField.Append(new UnsignedField<8>(mix.delayDown));
        internalField.Append(new UnsignedField<8>(mix.speedUp));
        internalField.Append(new UnsignedField<8>(mix.speedDown));
        internalField.Append(new ZCharField<8>(mix.name));
      }
      else if (IS_TARANIS(board) && version >= 217) {
        internalField.Append(new UnsignedField<8>(_destCh));
        internalField.Append(new UnsignedField<9>(mix.flightModes));
        internalField.Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
        internalField.Append(new UnsignedField<1>((unsigned int &)mix.carryTrim));
        internalField.Append(new UnsignedField<4>(mix.mixWarn));
        internalField.Append(new SignedField<16>(_weight));
        internalField.Append(new SourceField<10>(mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<14>(_offset));
        internalField.Append(new SwitchField<8>(mix.swtch, board, version));
        internalField.Append(new CurveReferenceField(mix.curve, board, version));
        internalField.Append(new UnsignedField<8>(mix.delayUp));
        internalField.Append(new UnsignedField<8>(mix.delayDown));
        internalField.Append(new UnsignedField<8>(mix.speedUp));
        internalField.Append(new UnsignedField<8>(mix.speedDown));
        internalField.Append(new ZCharField<8>(mix.name));
      }
      else if (IS_ARM(board) && version >= 217) {
        internalField.Append(new UnsignedField<5>(_destCh));
        internalField.Append(new UnsignedField<3>(mix.mixWarn));
        internalField.Append(new UnsignedField<9>(mix.flightModes));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new BoolField<1>(mix.noExpo));
        internalField.Append(new SignedField<3>(mix.carryTrim));
        internalField.Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
        internalField.Append(new SignedField<16>(_weight));
        internalField.Append(new SwitchField<8>(mix.swtch, board, version));
        internalField.Append(new SignedField<8>(_curveParam));
        internalField.Append(new UnsignedField<8>(mix.delayUp));
        internalField.Append(new UnsignedField<8>(mix.delayDown));
        internalField.Append(new UnsignedField<8>(mix.speedUp));
        internalField.Append(new UnsignedField<8>(mix.speedDown));
        internalField.Append(new SourceField<8>(mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<16>(_offset));
        internalField.Append(new ZCharField<6>(mix.name));
      }
      else if (IS_TARANIS(board) && version == 216) {
        internalField.Append(new UnsignedField<8>(_destCh));
        internalField.Append(new UnsignedField<16>(mix.flightModes));
        internalField.Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
        internalField.Append(new UnsignedField<1>((unsigned int &)mix.carryTrim));
        internalField.Append(new SpareBitsField<5>());
        internalField.Append(new SignedField<16>(_weight));
        internalField.Append(new SwitchField<8>(mix.swtch, board, version));
        internalField.Append(new CurveReferenceField(mix.curve, board, version));
        internalField.Append(new UnsignedField<4>(mix.mixWarn));
        internalField.Append(new SpareBitsField<4>());
        internalField.Append(new UnsignedField<8>(mix.delayUp));
        internalField.Append(new UnsignedField<8>(mix.delayDown));
        internalField.Append(new UnsignedField<8>(mix.speedUp));
        internalField.Append(new UnsignedField<8>(mix.speedDown));
        internalField.Append(new SourceField<8>(mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<16>(_offset));
        internalField.Append(new ZCharField<8>(mix.name));
        internalField.Append(new SpareBitsField<8>());
      }
      else if (IS_ARM(board) && version == 216) {
        internalField.Append(new UnsignedField<5>(_destCh));
        internalField.Append(new UnsignedField<3>(mix.mixWarn));
        internalField.Append(new UnsignedField<16>(mix.flightModes));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new BoolField<1>(mix.noExpo));
        internalField.Append(new SignedField<3>(mix.carryTrim));
        internalField.Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
        internalField.Append(new SpareBitsField<1>());
        internalField.Append(new SignedField<16>(_weight));
        internalField.Append(new SwitchField<8>(mix.swtch, board, version));
        internalField.Append(new SignedField<8>(_curveParam));
        internalField.Append(new UnsignedField<8>(mix.delayUp));
        internalField.Append(new UnsignedField<8>(mix.delayDown));
        internalField.Append(new UnsignedField<8>(mix.speedUp));
        internalField.Append(new UnsignedField<8>(mix.speedDown));
        internalField.Append(new SourceField<8>(mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<16>(_offset));
        internalField.Append(new ZCharField<6>(mix.name));
      }
      else if (IS_ARM(board)) {
        internalField.Append(new UnsignedField<8>(_destCh));
        internalField.Append(new UnsignedField<16>(mix.flightModes));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new BoolField<1>(mix.noExpo));
        internalField.Append(new SignedField<3>(mix.carryTrim));
        internalField.Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
        if (version >= 214)
          internalField.Append(new SpareBitsField<1>());
        else
          internalField.Append(new UnsignedField<1>(_offsetMode));
        internalField.Append(new SignedField<16>(_weight));
        internalField.Append(new SwitchField<8>(mix.swtch, board, version));
        internalField.Append(new SignedField<8>(_curveParam));
        if (version >= 214) {
          internalField.Append(new UnsignedField<4>(mix.mixWarn));
          internalField.Append(new SpareBitsField<4>());
        }
        else {
          internalField.Append(new UnsignedField<8>(mix.mixWarn));
        }
        internalField.Append(new UnsignedField<8>(mix.delayUp));
        internalField.Append(new UnsignedField<8>(mix.delayDown));
        internalField.Append(new UnsignedField<8>(mix.speedUp));
        internalField.Append(new UnsignedField<8>(mix.speedDown));
        internalField.Append(new SourceField<8>(mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        if (version >= 214)
          internalField.Append(new SignedField<16>(_offset));
        else
          internalField.Append(new SignedField<8>(_offset));
        if (HAS_LARGE_LCD(board)) {
          internalField.Append(new ZCharField<8>(mix.name));
          internalField.Append(new SpareBitsField<16>());
        }
        else {
          internalField.Append(new ZCharField<6>(mix.name));
        }
      }
      else if (IS_DBLRAM(board, version) && IS_AFTER_RELEASE_23_MARCH_2013(board, version)) {
        internalField.Append(new UnsignedField<4>(_destCh));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new BoolField<1>(mix.noExpo));
        internalField.Append(new UnsignedField<1>(_weightMode));
        internalField.Append(new UnsignedField<1>(_offsetMode));
        internalField.Append(new SourceField<8>(mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<8>(_weight));
        internalField.Append(new SwitchField<8>(mix.swtch, board, version));
        internalField.Append(new UnsignedField<8>(mix.flightModes));
        internalField.Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
        internalField.Append(new SignedField<3>(mix.carryTrim));
        internalField.Append(new UnsignedField<2>(mix.mixWarn));
        internalField.Append(new SpareBitsField<1>());
        internalField.Append(new UnsignedField<4>(mix.delayUp));
        internalField.Append(new UnsignedField<4>(mix.delayDown));
        internalField.Append(new UnsignedField<4>(mix.speedUp));
        internalField.Append(new UnsignedField<4>(mix.speedDown));
        internalField.Append(new SignedField<8>(_curveParam));
        internalField.Append(new SignedField<8>(_offset));
      }
      else {
        internalField.Append(new UnsignedField<4>(_destCh));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new BoolField<1>(mix.noExpo));
        internalField.Append(new UnsignedField<1>(_weightMode));
        internalField.Append(new UnsignedField<1>(_offsetMode));
        internalField.Append(new SignedField<8>(_weight));
        internalField.Append(new SwitchField<6>(mix.swtch, board, version));
        internalField.Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
        internalField.Append(new UnsignedField<5>(mix.flightModes));
        internalField.Append(new SignedField<3>(mix.carryTrim));
        internalField.Append(new SourceField<6>(mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new UnsignedField<2>(mix.mixWarn));
        internalField.Append(new UnsignedField<4>(mix.delayUp));
        internalField.Append(new UnsignedField<4>(mix.delayDown));
        internalField.Append(new UnsignedField<4>(mix.speedUp));
        internalField.Append(new UnsignedField<4>(mix.speedDown));
        internalField.Append(new SignedField<8>(_curveParam));
        internalField.Append(new SignedField<8>(_offset));
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
          _curveParam = smallGvarToEEPROM(mix.curve.value);
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
        if (mix.srcRaw.type == SOURCE_TYPE_STICK && mix.srcRaw.index < NUM_STICKS) {
          if (!mix.noExpo) {
            mix.srcRaw = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, mix.srcRaw.index);
          }
        }
      }

      if (mix.srcRaw.type != SOURCE_TYPE_NONE) {
        mix.destCh = _destCh + 1;
        if (!IS_TARANIS(board) || version < 216) {
          if (!_curveMode)
            mix.curve = CurveReference(CurveReference::CURVE_REF_DIFF, smallGvarToC9x(_curveParam));
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
      eepromImportDebug() << QString("imported %1: ch %2, name '%3'").arg(internalField.getName()).arg(mix.destCh).arg(mix.name);
    }

  protected:
    StructField internalField;
    MixData & mix;
    BoardEnum board;
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
    InputField(ExpoData & expo, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField("Input"),
      expo(expo),
      board(board),
      version(version)
    {
      if (IS_TARANIS(board) && version >= 218) {
        internalField.Append(new UnsignedField<2>(expo.mode, "Mode"));
        internalField.Append(new UnsignedField<14>(expo.scale, "Scale"));
        internalField.Append(new SourceField<10>(expo.srcRaw, board, version, 0));
        internalField.Append(new SignedField<6>(expo.carryTrim, "CarryTrim"));
        internalField.Append(new UnsignedField<5>(expo.chn, "Channel"));
        internalField.Append(new SwitchField<9>(expo.swtch, board, version));
        internalField.Append(new UnsignedField<9>(expo.flightModes));
        internalField.Append(new SignedField<8>(_weight, "Weight"));
        internalField.Append(new SpareBitsField<1>());
        internalField.Append(new ZCharField<8>(expo.name));
        internalField.Append(new SignedField<8>(_offset, "Offset"));
        internalField.Append(new CurveReferenceField(expo.curve, board, version));
      }
      else if (IS_TARANIS(board) && version >= 217) {
        internalField.Append(new SourceField<10>(expo.srcRaw, board, version, 0));
        internalField.Append(new UnsignedField<14>(expo.scale, "Scale"));
        internalField.Append(new UnsignedField<8>(expo.chn, "Channel"));
        internalField.Append(new SwitchField<8>(expo.swtch, board, version));
        internalField.Append(new UnsignedField<16>(expo.flightModes));
        internalField.Append(new SignedField<8>(_weight, "Weight"));
        internalField.Append(new SignedField<6>(expo.carryTrim, "CarryTrim"));
        internalField.Append(new UnsignedField<2>(expo.mode, "Mode"));
        internalField.Append(new ZCharField<8>(expo.name));
        internalField.Append(new SignedField<8>(_offset, "Offset"));
        internalField.Append(new CurveReferenceField(expo.curve, board, version));
      }
      else if (IS_ARM(board) && version >= 217) {
        internalField.Append(new UnsignedField<2>(expo.mode, "Mode"));
        internalField.Append(new UnsignedField<3>(expo.chn, "Channel"));
        internalField.Append(new BoolField<2>(_curveMode));
        internalField.Append(new UnsignedField<9>(expo.flightModes, "Phases"));
        internalField.Append(new SwitchField<8>(expo.swtch, board, version));
        internalField.Append(new SignedField<8>(_weight, "Weight"));
        internalField.Append(new ZCharField<6>(expo.name));
        internalField.Append(new SignedField<8>(_curveParam));
      }
      else if (IS_TARANIS(board) && version >= 216) {
        internalField.Append(new SourceField<8>(expo.srcRaw, board, version, 0));
        internalField.Append(new UnsignedField<16>(expo.scale, "Scale"));
        internalField.Append(new UnsignedField<8>(expo.chn, "Channel"));
        internalField.Append(new SwitchField<8>(expo.swtch, board, version));
        internalField.Append(new UnsignedField<16>(expo.flightModes));
        internalField.Append(new SignedField<8>(_weight, "Weight"));
        internalField.Append(new SignedField<6>(expo.carryTrim, "CarryTrim"));
        internalField.Append(new UnsignedField<2>(expo.mode, "Mode"));
        internalField.Append(new ZCharField<8>(expo.name));
        internalField.Append(new SignedField<8>(_offset, "Offset"));
        internalField.Append(new CurveReferenceField(expo.curve, board, version));
        internalField.Append(new SpareBitsField<8>());
      }
      else if (IS_ARM(board) && version >= 216) {
        internalField.Append(new UnsignedField<2>(expo.mode, "Mode"));
        internalField.Append(new UnsignedField<4>(expo.chn, "Channel"));
        internalField.Append(new BoolField<2>(_curveMode));
        internalField.Append(new SwitchField<8>(expo.swtch, board, version));
        internalField.Append(new UnsignedField<16>(expo.flightModes, "Phases"));
        internalField.Append(new SignedField<8>(_weight, "Weight"));
        internalField.Append(new ZCharField<6>(expo.name));
        internalField.Append(new SignedField<8>(_curveParam));
      }
      else if (IS_ARM(board)) {
        internalField.Append(new UnsignedField<8>(expo.mode, "Mode"));
        internalField.Append(new UnsignedField<8>(expo.chn, "Channel"));
        internalField.Append(new SwitchField<8>(expo.swtch, board, version));
        internalField.Append(new UnsignedField<16>(expo.flightModes, "Phases"));
        internalField.Append(new SignedField<8>(_weight, "Weight"));
        internalField.Append(new BoolField<8>(_curveMode));
        if (HAS_LARGE_LCD(board)) {
          internalField.Append(new ZCharField<8>(expo.name));
          internalField.Append(new SpareBitsField<16>());
        }
        else {
          internalField.Append(new ZCharField<6>(expo.name));
        }
        internalField.Append(new SignedField<8>(_curveParam));
      }
      else if (IS_DBLRAM(board, version) && IS_AFTER_RELEASE_23_MARCH_2013(board, version)) {
        internalField.Append(new UnsignedField<2>(expo.mode));
        internalField.Append(new UnsignedField<2>(expo.chn));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new SpareBitsField<3>());
        internalField.Append(new UnsignedField<8>(expo.flightModes));
        internalField.Append(new SwitchField<8>(expo.swtch, board, version));
        internalField.Append(new SignedField<8>(_weight));
        internalField.Append(new SignedField<8>(_curveParam));
      }
      else {
        internalField.Append(new UnsignedField<2>(expo.mode));
        internalField.Append(new SwitchField<6>(expo.swtch, board, version));
        internalField.Append(new UnsignedField<2>(expo.chn));
        internalField.Append(new UnsignedField<5>(expo.flightModes));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new SignedField<8>(_weight));
        internalField.Append(new SignedField<8>(_curveParam));
      }
    }

    virtual void beforeExport()
    {
      _weight = smallGvarToEEPROM(expo.weight);

      if (IS_TARANIS(board) && version >= 216) {
        _offset = smallGvarToEEPROM(expo.offset);
      }

      if (!IS_TARANIS(board) || version < 216) {
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
          _curveParam = smallGvarToEEPROM(expo.curve.value);
        }
      }
    }

    virtual void afterImport()
    {
      if (IS_TARANIS(board)) {
        if (version < 216) {
          if (expo.mode) {
            expo.srcRaw = RawSource(SOURCE_TYPE_STICK, expo.chn);
          }
        }
      }
      else if (expo.mode) {
        expo.srcRaw = RawSource(SOURCE_TYPE_STICK, expo.chn);
      }

      expo.weight = smallGvarToC9x(_weight);

      if (IS_TARANIS(board) && version >= 216) {
        expo.offset = smallGvarToC9x(_offset);
      }

      if (!IS_TARANIS(board) || version < 216) {
        if (!_curveMode)
          expo.curve = CurveReference(CurveReference::CURVE_REF_EXPO, smallGvarToC9x(_curveParam));
        else if (_curveParam > 6)
          expo.curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, _curveParam-6);
        else
          expo.curve = CurveReference(CurveReference::CURVE_REF_FUNC, _curveParam);
      }
      eepromImportDebug() << QString("imported %1: ch %2 name '%3'").arg(internalField.getName()).arg(expo.chn).arg(expo.name);
    }

  protected:
    StructField internalField;
    ExpoData & expo;
    BoardEnum board;
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

    LimitField(LimitData & limit, BoardEnum board, unsigned int version):
      StructField("Limit")
    {
      if (IS_TARANIS(board) && version >= 217) {
        Append(new ConversionField< SignedField<11> >(limit.min, exportLimitValue<1000, 1024>, importLimitValue<1000, 1024>));
        Append(new ConversionField< SignedField<11> >(limit.max, exportLimitValue<-1000, 1024>, importLimitValue<-1000, 1024>));
        Append(new SignedField<10>(limit.ppmCenter));
        Append(new ConversionField< SignedField<11> >(limit.offset, exportLimitValue<0, 1024>, importLimitValue<0, 1024>));
        Append(new BoolField<1>(limit.symetrical));
        Append(new BoolField<1>(limit.revert));
        Append(new SpareBitsField<3>());
        Append(new SignedField<8>(limit.curve.value));
        if (HAS_LARGE_LCD(board)) {
          Append(new ZCharField<6>(limit.name));
        }
      }
      else {
        if (IS_TARANIS(board) && version >= 216) {
          Append(new ConversionField< SignedField<16> >(limit.min, exportLimitValue<1000, 4096>, importLimitValue<1000, 4096>));
          Append(new ConversionField< SignedField<16> >(limit.max, exportLimitValue<-1000, 4096>, importLimitValue<-1000, 4096>));
          Append(new SignedField<8>(limit.ppmCenter));
          Append(new ConversionField< SignedField<14> >(limit.offset, exportLimitValue<0, 4096>, importLimitValue<0, 4096>));
        }
        else {
          Append(new ConversionField< SignedField<8> >(limit.min, +100, 10));
          Append(new ConversionField< SignedField<8> >(limit.max, -100, 10));
          Append(new SignedField<8>(limit.ppmCenter));
          Append(new SignedField<14>(limit.offset));
        }
        Append(new BoolField<1>(limit.symetrical));
        Append(new BoolField<1>(limit.revert));
        if (HAS_LARGE_LCD(board)) {
          Append(new ZCharField<6>(limit.name));
        }
        if (IS_TARANIS(board) && version >= 216) {
          Append(new SignedField<8>(limit.curve.value));
        }
      }
    }
};

class CurvesField: public TransformedField {
  public:
    CurvesField(CurveData * curves, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField("Curves"),
      curves(curves),
      board(board),
      version(version),
      maxCurves(MAX_CURVES(board, version)),
      maxPoints(IS_ARM(board) ? 512 : 112-8)
    {
      for (int i=0; i<maxCurves; i++) {
        if (IS_TARANIS(board) && version >= 218) {
          internalField.Append(new UnsignedField<1>((unsigned int &)curves[i].type));
          internalField.Append(new BoolField<1>(curves[i].smooth));
          internalField.Append(new ConversionField< SignedField<6> >(curves[i].count, -5));
          internalField.Append(new ZCharField<3>(curves[i].name));
        }
        else if (IS_TARANIS(board) && version >= 216) {
          internalField.Append(new UnsignedField<3>((unsigned int &)curves[i].type));
          internalField.Append(new BoolField<1>(curves[i].smooth));
          internalField.Append(new SpareBitsField<4>());
          internalField.Append(new ConversionField< SignedField<8> >(curves[i].count, -5));
        }
        else if (IS_ARM(board)) {
          internalField.Append(new SignedField<16>(_curves[i]));
        }
        else {
          internalField.Append(new SignedField<8>(_curves[i]));
        }
      }

      for (int i=0; i<maxPoints; i++) {
        internalField.Append(new SignedField<8>(_points[i]));
      }
    }

    virtual void beforeExport()
    {
      memset(_points, 0, sizeof(_points));

      int * cur = &_points[0];
      int offset = 0;

      for (int i=0; i<maxCurves; i++) {
        CurveData *curve = &curves[i];
        if (IS_TARANIS(board) && version >= 216) {
          offset += (curve->type == CurveData::CURVE_TYPE_CUSTOM ? curve->count * 2 - 2 : curve->count);
          if (offset > maxPoints) {
            EEPROMWarnings.push_back(::QObject::tr("OpenTX only accepts %1 points in all curves").arg(maxPoints));
            break;
          }
        }
        else {
          offset += (curve->type == CurveData::CURVE_TYPE_CUSTOM ? curve->count * 2 - 2 : curve->count) - 5;
          if (offset > maxPoints - 5 * maxCurves) {
            EEPROMWarnings.push_back(::QObject::tr("OpenTx only accepts %1 points in all curves").arg(maxPoints));
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
        CurveData *curve = &curves[i];
        if (!IS_TARANIS(board) || version < 216) {
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
        eepromImportDebug() << QString("imported curve: %3 points").arg(curve->count);
      }
    }

  protected:
    StructField internalField;
    CurveData *curves;
    BoardEnum board;
    unsigned int version;
    int maxCurves;
    int maxPoints;
    int _curves[C9X_MAX_CURVES];
    int _points[C9X_MAX_CURVES*C9X_MAX_POINTS*2];
};

class LogicalSwitchesFunctionsTable: public ConversionTable {

  public:
    LogicalSwitchesFunctionsTable(BoardEnum board, unsigned int version)
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
    AndSwitchesConversionTable(BoardEnum board, unsigned int version)
    {
      int val=0;
      addConversion(RawSwitch(SWITCH_TYPE_NONE), val++);

      if (IS_TARANIS(board)) {
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

    static ConversionTable * getInstance(BoardEnum board, unsigned int version)
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
    LogicalSwitchField(LogicalSwitchData & csw, BoardEnum board, unsigned int version, unsigned int variant, ModelData * model=NULL):
      TransformedField(internalField),
      internalField("LogicalSwitch"),
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
        internalField.Append(new ConversionField< UnsignedField<8> >(csw.func, &functionsConversionTable, "Function"));
        internalField.Append(new SignedField<10>(v1));
        internalField.Append(new SignedField<10>(v3));
        internalField.Append(new ConversionField< SignedField<9> >((int &)csw.andsw, andswitchesConversionTable, "AND switch"));
        internalField.Append(new SpareBitsField<1>()); // TODO extra switch mode
        internalField.Append(new SpareBitsField<2>());
        internalField.Append(new SignedField<16>(v2));
      }
      else if (IS_ARM(board) && version >= 217) {
        internalField.Append(new ConversionField< UnsignedField<6> >(csw.func, &functionsConversionTable, "Function"));
        internalField.Append(new SignedField<10>(v1));
        internalField.Append(new SignedField<16>(v2));
        internalField.Append(new SignedField<16>(v3));
      }
      else if (IS_ARM(board) && version >= 216) {
        internalField.Append(new SignedField<8>(v1));
        internalField.Append(new SignedField<16>(v2));
        internalField.Append(new SignedField<16>(v3));
        internalField.Append(new ConversionField< UnsignedField<8> >(csw.func, &functionsConversionTable, "Function"));
      }
      else if (IS_ARM(board) && version >= 215) {
        internalField.Append(new SignedField<16>(v1));
        internalField.Append(new SignedField<16>(v2));
        internalField.Append(new ConversionField< UnsignedField<8> >(csw.func, &functionsConversionTable, "Function"));
      }
      else if (IS_ARM(board)) {
        internalField.Append(new SignedField<8>(v1));
        internalField.Append(new SignedField<8>(v2));
        internalField.Append(new ConversionField< UnsignedField<8> >(csw.func, &functionsConversionTable, "Function"));
      }
      else {
        internalField.Append(new SignedField<8>(v1));
        internalField.Append(new SignedField<8>(v2));
        if (version >= 213)
          internalField.Append(new ConversionField< UnsignedField<4> >(csw.func, &functionsConversionTable, "Function"));
        else
          internalField.Append(new ConversionField< UnsignedField<8> >(csw.func, &functionsConversionTable, "Function"));
      }

      if (IS_ARM(board)) {
        int scale = (version >= 216 ? 0 : 5);
        internalField.Append(new ConversionField< UnsignedField<8> >(csw.delay, 0, scale));
        internalField.Append(new ConversionField< UnsignedField<8> >(csw.duration, 0, scale));
        if (version >= 214 && version <= 217) {
          internalField.Append(new ConversionField< SignedField<8> >((int &)csw.andsw, andswitchesConversionTable, "AND switch"));
        }
      }
      else {
        if (version >= 213) {
          internalField.Append(new ConversionField< UnsignedField<4> >((unsigned int &)csw.andsw, andswitchesConversionTable, "AND switch"));
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
          if (val1.type == SOURCE_TYPE_STICK && val1.index < NUM_STICKS) {
            csw.val1 = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, val1.index).toValue();
          }
          RawSource val2(csw.val2);
          if (val2.type == SOURCE_TYPE_STICK && val2.index < NUM_STICKS) {
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
          if (val1.type == SOURCE_TYPE_STICK && val1.index < NUM_STICKS) {
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
      eepromImportDebug() << QString("imported %1: %2").arg(internalField.getName()).arg(csw.funcToString());
    }

  protected:
    StructField internalField;
    LogicalSwitchData & csw;
    BoardEnum board;
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
    CustomFunctionsConversionTable(BoardEnum board, unsigned int version)
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
          if (IS_TARANIS(board))
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
        if (IS_TARANIS(board))
          addConversion(FuncScreenshot, val++);
      }
      else {
        addConversion(FuncPlaySound, val++);
        if (!IS_TARANIS(board))
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
    SwitchesWarningField(uint64_t & sw, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField(_sw, "SwitchesWarning"),
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
      eepromImportDebug() << QString("imported %1").arg(internalField.getName());
    }

  protected:
    BaseUnsignedField<uint64_t, N> internalField;
    uint64_t &sw;
    uint64_t _sw;
    BoardEnum board;
    unsigned int version;
};

class ArmCustomFunctionField: public TransformedField {
  public:
    ArmCustomFunctionField(CustomFunctionData & fn, BoardEnum board, unsigned int version, unsigned int variant):
      TransformedField(internalField),
      internalField("CustomFunction"),
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
        internalField.Append(new SwitchField<9>(fn.swtch, board, version));
        internalField.Append(new ConversionField< UnsignedField<7> >(_func, &functionsConversionTable, "Function", ::QObject::tr("OpenTX on this board doesn't accept this function")));
      }
      else {
        internalField.Append(new SwitchField<8>(fn.swtch, board, version));
        internalField.Append(new ConversionField< UnsignedField<8> >(_func, &functionsConversionTable, "Function", ::QObject::tr("OpenTX on this board doesn't accept this function")));
      }

      if (IS_TARANIS(board) && version >= 216)
        internalField.Append(new CharField<8>(_param, false));
      else if (IS_TARANIS(board))
        internalField.Append(new CharField<10>(_param, false));
      else
        internalField.Append(new CharField<6>(_param, false));

      if (version >= 216) {
        internalField.Append(new SignedField<8>(_active));
      }
      else if (version >= 214) {
        internalField.Append(new UnsignedField<2>(_mode));
        internalField.Append(new UnsignedField<6>((unsigned int &)_active));
      }
      else {
        internalField.Append(new UnsignedField<8>((unsigned int &)_active));
        internalField.Append(new SpareBitsField<8>());
      }
    }

    virtual void beforeExport()
    {
      if (fn.swtch.type != SWITCH_TYPE_NONE) {
        _func = fn.func;

        if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayValue || fn.func == FuncPlayHaptic || (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGVLast))
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

      if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayValue || fn.func == FuncPlayHaptic || (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGVLast))
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
      eepromImportDebug() << QString("imported %1").arg(internalField.getName());
    }

  protected:
    StructField internalField;
    CustomFunctionData & fn;
    BoardEnum board;
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
    AvrCustomFunctionField(CustomFunctionData & fn, BoardEnum board, unsigned int version, unsigned int variant):
      TransformedField(internalField),
      internalField("CustomFunction"),
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
        internalField.Append(new SwitchField<8>(fn.swtch, board, version));
        internalField.Append(new ConversionField< UnsignedField<8> >((unsigned int &)fn.func, &functionsConversionTable, "Function", ::QObject::tr("OpenTX on this board doesn't accept this function")));
        internalField.Append(new UnsignedField<2>(fn.adjustMode));
        internalField.Append(new UnsignedField<4>(_union_param));
        internalField.Append(new UnsignedField<1>(_active));
        internalField.Append(new SpareBitsField<1>());
      }
      else if (version >= 216) {
        internalField.Append(new SwitchField<6>(fn.swtch, board, version));
        internalField.Append(new ConversionField< UnsignedField<4> >((unsigned int &)fn.func, &functionsConversionTable, "Function", ::QObject::tr("OpenTX on this board doesn't accept this function")));
        internalField.Append(new UnsignedField<5>(_union_param));
        internalField.Append(new UnsignedField<1>(_active));
      }
      else if (version >= 213) {
        internalField.Append(new SwitchField<8>(fn.swtch, board, version));
        internalField.Append(new UnsignedField<3>(_union_param));
        internalField.Append(new ConversionField< UnsignedField<5> >((unsigned int &)fn.func, &functionsConversionTable, "Function", ::QObject::tr("OpenTX on this board doesn't accept this function")));
      }
      else {
        internalField.Append(new SwitchField<8>(fn.swtch, board, version));
        internalField.Append(new ConversionField< UnsignedField<7> >((unsigned int &)fn.func, &functionsConversionTable, "Function", ::QObject::tr("OpenTX on this board doesn't accept this function")));
        internalField.Append(new BoolField<1>((bool &)fn.enabled));
      }
      internalField.Append(new UnsignedField<8>(_param));
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
      eepromImportDebug() << QString("imported %1").arg(internalField.getName());
    }

  protected:
    StructField internalField;
    CustomFunctionData & fn;
    BoardEnum board;
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
    FrskyScreenField(FrSkyScreenData & screen, BoardEnum board, unsigned int version, unsigned int variant):
      DataField("Frsky Screen"),
      screen(screen),
      board(board),
      version(version)
    {
      for (int i=0; i<4; i++) {
        if (IS_ARM(board) && version >= 217) {
          if (IS_TARANIS(board))
            bars.Append(new SourceField<16>(screen.body.bars[i].source, board, version, variant));
          else
            bars.Append(new SourceField<8>(screen.body.bars[i].source, board, version, variant));
          bars.Append(new UnsignedField<16>(screen.body.bars[i].barMin));
          bars.Append(new UnsignedField<16>(screen.body.bars[i].barMax));
        }
        else {
          bars.Append(new TelemetrySourceField<8>(screen.body.bars[i].source, board, version));
          bars.Append(new UnsignedField<8>(screen.body.bars[i].barMin));
          bars.Append(new UnsignedField<8>(screen.body.bars[i].barMax));
        }
      }

      int columns = (IS_TARANIS(board) ? 3 : 2);
      for (int i=0; i<4; i++) {
        for (int j=0; j<columns; j++) {
          if (IS_TARANIS(board) && version >= 217)
            numbers.Append(new SourceField<16>(screen.body.lines[i].source[j], board, version, variant));
          else if (IS_ARM(board) && version >= 217)
            numbers.Append(new SourceField<8>(screen.body.lines[i].source[j], board, version, variant));
          else
            numbers.Append(new TelemetrySourceField<8>(screen.body.lines[i].source[j], board, version));
        }
      }

      if (!IS_TARANIS(board)) {
        if (IS_ARM(board))
          numbers.Append(new SpareBitsField<12*8>());
        else
          numbers.Append(new SpareBitsField<4*8>());
      }

      if (IS_TARANIS(board) && version >= 217) {
        script.Append(new CharField<8>(screen.body.script.filename));
        script.Append(new SpareBitsField<16*8>());
      }

      if (IS_ARM(board) && version >= 217) {
        if (IS_TARANIS(board))
          none.Append(new SpareBitsField<24*8>());
        else
          none.Append(new SpareBitsField<20*8>());
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

    virtual void ImportBits(QBitArray & input)
    {
      eepromImportDebug() << QString("importing %1: type: %2").arg(name).arg(screen.type);

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
    BoardEnum board;
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
    TelemetryVarioSourceConversionTable(BoardEnum board, unsigned int version)
    {
      int val = 0;
      if (!IS_TARANIS(board)) {
        addConversion(TELEMETRY_VARIO_SOURCE_ALTI, val++);
        addConversion(TELEMETRY_VARIO_SOURCE_ALTI_PLUS, val++);
      }
      addConversion(TELEMETRY_VARIO_SOURCE_VSPEED, val++);
      addConversion(TELEMETRY_VARIO_SOURCE_A1, val++);
      addConversion(TELEMETRY_VARIO_SOURCE_A2, val++);
      if (IS_TARANIS(board)) {
        addConversion(TELEMETRY_VARIO_SOURCE_DTE, val++);
      }
    }
};

class TelemetryVoltsSourceConversionTable: public ConversionTable
{
  public:
    TelemetryVoltsSourceConversionTable(BoardEnum board, unsigned int version)
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
    ScreenTypesConversionTable(BoardEnum board, unsigned int version)
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
    TelemetryCurrentSourceConversionTable(BoardEnum board, unsigned int version)
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
    FrskyField(FrSkyData & frsky, BoardEnum board, unsigned int version, unsigned int variant):
      StructField("FrSky"),
      telemetryVarioSourceConversionTable(board, version),
      screenTypesConversionTable(board, version),
      telemetryVoltsSourceConversionTable(board, version),
      telemetryCurrentSourceConversionTable(board, version)
    {
      rssiConversionTable[0] = RSSIConversionTable(0);
      rssiConversionTable[1] = RSSIConversionTable(1);

      if (IS_ARM(board)) {
        if (version >= 217) {
          Append(new UnsignedField<8>(frsky.voltsSource, "Volts Source"));
          Append(new UnsignedField<8>(frsky.altitudeSource, "Altitude Source"));
        }
        else {
          for (int i=0; i<(version >= 216 ? 4 : 2); i++) {
            Append(new UnsignedField<8>(frsky.channels[i].ratio, "Ratio"));
            Append(new SignedField<12>(frsky.channels[i].offset, "Offset"));
            Append(new UnsignedField<4>(frsky.channels[i].type, "Type"));
            for (int j=0; j<2; j++)
              Append(new UnsignedField<8>(frsky.channels[i].alarms[j].value, "Alarm value"));
            for (int j=0; j<2; j++)
              Append(new UnsignedField<2>(frsky.channels[i].alarms[j].level));
            for (int j=0; j<2; j++)
              Append(new UnsignedField<1>(frsky.channels[i].alarms[j].greater));
            Append(new SpareBitsField<2>());
            Append(new UnsignedField<8>(frsky.channels[i].multiplier, 0, 5, "Multiplier"));
          }
          Append(new UnsignedField<8>(frsky.usrProto));
          if (version >= 216) {
            Append(new ConversionField< UnsignedField<7> >(frsky.voltsSource, &telemetryVoltsSourceConversionTable, "Volts Source"));
            Append(new SpareBitsField<1>());
          }
          else {
            Append(new ConversionField< UnsignedField<8> >(frsky.voltsSource, &telemetryVoltsSourceConversionTable, "Volts Source"));
          }
          Append(new ConversionField< SignedField<8> >(frsky.blades, -2));
          Append(new ConversionField< UnsignedField<8> >(frsky.currentSource, &telemetryCurrentSourceConversionTable, "Current Source"));
        }

        if (version >= 217) {
          for (int i=0; i<4; i++) {
            Append(new UnsignedField<2>(frsky.screens[i].type));
          }
          for (int i=0; i<4; i++) {
            Append(new FrskyScreenField(frsky.screens[i], board, version, variant));
          }
        }
        else {
          Append(new UnsignedField<1>(frsky.screens[0].type));
          Append(new UnsignedField<1>(frsky.screens[1].type));
          Append(new UnsignedField<1>(frsky.screens[2].type));
          Append(new SpareBitsField<5>());
          for (int i=0; i<3; i++) {
            Append(new FrskyScreenField(frsky.screens[i], board, version, variant));
          }
        }

        if (version >= 217) {
          Append(new UnsignedField<7>(frsky.varioSource, "Vario Source"));
          Append(new BoolField<1>(frsky.varioCenterSilent));
        }
        else {
          Append(new ConversionField< UnsignedField<8> >(frsky.varioSource, &telemetryVarioSourceConversionTable, "Vario Source"));
        }
        Append(new SignedField<8>(frsky.varioCenterMax));
        Append(new SignedField<8>(frsky.varioCenterMin));
        Append(new SignedField<8>(frsky.varioMin));
        Append(new SignedField<8>(frsky.varioMax));
        for (int i=0; i<2; i++) {
          Append(new ConversionField< UnsignedField<2> >(frsky.rssiAlarms[i].level, &rssiConversionTable[i], "RSSI"));
          Append(new ConversionField< SignedField<6> >(frsky.rssiAlarms[i].value, -45+i*3));
        }
        if (version == 216) {
          Append(new BoolField<1>(frsky.mAhPersistent));
          Append(new UnsignedField<15>(frsky.storedMah));
          Append(new SignedField<8>(frsky.fasOffset));
        }
      }
      else {
        for (int i=0; i<2; i++) {
          Append(new UnsignedField<8>(frsky.channels[i].ratio, "Ratio"));
          Append(new SignedField<12>(frsky.channels[i].offset, "Offset"));
          Append(new UnsignedField<4>(frsky.channels[i].type, "Type"));
          for (int j=0; j<2; j++)
            Append(new UnsignedField<8>(frsky.channels[i].alarms[j].value, "Alarm value"));
          for (int j=0; j<2; j++)
            Append(new UnsignedField<2>(frsky.channels[i].alarms[j].level));
          for (int j=0; j<2; j++)
            Append(new UnsignedField<1>(frsky.channels[i].alarms[j].greater));
          Append(new UnsignedField<2>(frsky.channels[i].multiplier, 0, 3, "Multiplier"));
        }
        Append(new UnsignedField<2>(frsky.usrProto, "USR Proto"));
        Append(new ConversionField< UnsignedField<2> >((unsigned int &)frsky.blades, -2));
        for (int i=0; i<2; i++) {
          Append(new ConversionField< UnsignedField<1> >(frsky.screens[i].type, &screenTypesConversionTable, "Screen Type"));
        }
        Append(new ConversionField< UnsignedField<2> >(frsky.voltsSource, &telemetryVoltsSourceConversionTable, "Volts Source"));
        Append(new SignedField<4>(frsky.varioMin, "Vario Min"));
        Append(new SignedField<4>(frsky.varioMax));
        for (int i=0; i<2; i++) {
          Append(new ConversionField< UnsignedField<2> >(frsky.rssiAlarms[i].level, &rssiConversionTable[i], "RSSI level"));
          Append(new ConversionField< SignedField<6> >(frsky.rssiAlarms[i].value, -45+i*3, 0, 0, 100, "RSSI value"));
        }
        for (int i=0; i<2; i++) {
          Append(new FrskyScreenField(frsky.screens[i], board, version, variant));
        }
        Append(new UnsignedField<3>(frsky.varioSource));
        Append(new SignedField<5>(frsky.varioCenterMin));
        Append(new ConversionField< UnsignedField<3> >(frsky.currentSource, &telemetryCurrentSourceConversionTable, "Current Source"));
        Append(new SignedField<5>(frsky.varioCenterMax));
        if (version >= 216) {
          Append(new SignedField<8>(frsky.fasOffset));
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
    MavlinkField(MavlinkData & mavlink, BoardEnum board, unsigned int version):
      StructField("MavLink")
    {
      Append(new UnsignedField<4>(mavlink.rc_rssi_scale, "Rc_rssi_scale"));
      Append(new UnsignedField<1>(mavlink.pc_rssi_en, "Pc_rssi_en"));
      Append(new SpareBitsField<3>());
      Append(new SpareBitsField<8>());
      Append(new SpareBitsField<8>());
      Append(new SpareBitsField<8>());
    }
};

class SensorField: public TransformedField {
  public:
    SensorField(SensorData & sensor, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField("Sensor"),
      sensor(sensor),
      _param(0)
    {
      internalField.Append(new UnsignedField<16>(_id, "id/persistentValue"));
      internalField.Append(new UnsignedField<8>(_instance, "instance/formula"));
      internalField.Append(new ZCharField<4>(sensor.label));
      internalField.Append(new UnsignedField<1>(sensor.type, "type"));
      internalField.Append(new UnsignedField<5>(sensor.unit, "unit"));
      internalField.Append(new UnsignedField<2>(sensor.prec, "prec"));
      internalField.Append(new BoolField<1>(sensor.autoOffset));
      internalField.Append(new BoolField<1>(sensor.filter));
      internalField.Append(new BoolField<1>(sensor.logs));
      internalField.Append(new BoolField<1>(sensor.persistent));
      internalField.Append(new BoolField<1>(sensor.onlyPositive));
      internalField.Append(new SpareBitsField<3>());
      internalField.Append(new UnsignedField<32>(_param, "param"));
    }

    virtual void beforeExport()
    {
      if (sensor.type == SensorData::TELEM_TYPE_CUSTOM) {
        _id = sensor.id;
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
      eepromImportDebug() << QString("imported %1").arg(internalField.getName());
    }

  protected:
    StructField internalField;
    SensorData & sensor;
    unsigned int _id;
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

OpenTxModelData::OpenTxModelData(ModelData & modelData, BoardEnum board, unsigned int version, unsigned int variant):
  TransformedField(internalField),
  internalField("ModelData"),
  modelData(modelData),
  board(board),
  version(version),
  variant(variant),
  protocolsConversionTable(board)
{
  sprintf(name, "Model %s", modelData.name);

  eepromImportDebug() << QString("OpenTxModelData::OpenTxModelData(name: %1, board: %2, ver: %3, var: %4)").arg(name).arg(board).arg(version).arg(variant);

  if (HAS_LARGE_LCD(board))
    internalField.Append(new ZCharField<12>(modelData.name));
  else
    internalField.Append(new ZCharField<10>(modelData.name));

  bool afterrelease21March2013 = IS_AFTER_RELEASE_21_MARCH_2013(board, version);

  if (afterrelease21March2013) {
    internalField.Append(new UnsignedField<8>(modelData.moduleData[0].modelId));
  }
  if (IS_ARM(board) && version >= 217) {
    internalField.Append(new UnsignedField<8>(modelData.moduleData[1].modelId));
  }

  if (IS_TARANIS(board) && version >= 215) {
    internalField.Append(new CharField<10>(modelData.bitmap));
  }

  for (int i=0; i<MAX_TIMERS(board, version); i++) {
    if (IS_ARM(board) && version >= 218) {
      internalField.Append(new SwitchField<9>(modelData.timers[i].mode, board, version, true));
      internalField.Append(new UnsignedField<23>(modelData.timers[i].val));
      internalField.Append(new SignedField<24>(modelData.timers[i].pvalue));
      internalField.Append(new UnsignedField<2>(modelData.timers[i].countdownBeep));
      internalField.Append(new BoolField<1>(modelData.timers[i].minuteBeep));
      internalField.Append(new UnsignedField<2>(modelData.timers[i].persistent));
      internalField.Append(new SpareBitsField<3>());
      if (IS_TARANIS(board))
        internalField.Append(new ZCharField<8>(modelData.timers[i].name));
      else
        internalField.Append(new ZCharField<3>(modelData.timers[i].name));
    }
    else if (IS_ARM(board) && version >= 217) {
      internalField.Append(new SwitchField<8>(modelData.timers[i].mode, board, version, true));
      internalField.Append(new UnsignedField<24>(modelData.timers[i].val));
      internalField.Append(new SignedField<24>(modelData.timers[i].pvalue));
      internalField.Append(new UnsignedField<2>(modelData.timers[i].countdownBeep));
      internalField.Append(new BoolField<1>(modelData.timers[i].minuteBeep));
      internalField.Append(new UnsignedField<2>(modelData.timers[i].persistent));
      internalField.Append(new SpareBitsField<3>());
      if (IS_TARANIS(board))
        internalField.Append(new ZCharField<8>(modelData.timers[i].name));
      else
        internalField.Append(new ZCharField<3>(modelData.timers[i].name));
    }
    else if ((IS_ARM(board) || IS_2560(board)) && version >= 216) {
      internalField.Append(new SwitchField<8>(modelData.timers[i].mode, board, version, true));
      internalField.Append(new UnsignedField<16>(modelData.timers[i].val));
      internalField.Append(new UnsignedField<2>(modelData.timers[i].countdownBeep));
      internalField.Append(new BoolField<1>(modelData.timers[i].minuteBeep));
      internalField.Append(new UnsignedField<2>(modelData.timers[i].persistent));
      internalField.Append(new SpareBitsField<3>());
      internalField.Append(new SignedField<16>(modelData.timers[i].pvalue));
    }
    else if (afterrelease21March2013) {
      internalField.Append(new SwitchField<8>(modelData.timers[i].mode, board, version, true));
      internalField.Append(new UnsignedField<12>(modelData.timers[i].val));
      internalField.Append(new BoolField<1>((bool &)modelData.timers[i].countdownBeep));
      internalField.Append(new BoolField<1>(modelData.timers[i].minuteBeep));
      if (HAS_PERSISTENT_TIMERS(board)) {
        internalField.Append(new UnsignedField<1>(modelData.timers[i].persistent));
        internalField.Append(new SpareBitsField<1>());
        internalField.Append(new SignedField<16>(modelData.timers[i].pvalue));
      }
      else {
        internalField.Append(new SpareBitsField<2>());
      }
    }
    else {
      internalField.Append(new SwitchField<8>(modelData.timers[i].mode, board, version, true));
      internalField.Append(new UnsignedField<16>(modelData.timers[i].val));
      if (HAS_PERSISTENT_TIMERS(board)) {
        internalField.Append(new UnsignedField<1>(modelData.timers[i].persistent));
        internalField.Append(new SpareBitsField<15>());
      }
    }
  }

  if (IS_ARM(board))
    internalField.Append(new UnsignedField<3>(modelData.telemetryProtocol));
  else
    internalField.Append(new ConversionField< UnsignedField<3> >((unsigned int &)modelData.moduleData[0].protocol, &protocolsConversionTable, "Protocol", ::QObject::tr("OpenTX doesn't accept this telemetry protocol")));

  internalField.Append(new BoolField<1>(modelData.thrTrim));

  if (IS_ARM(board) && version >= 217) {
    internalField.Append(new BoolField<1>(modelData.noGlobalFunctions));
    internalField.Append(new UnsignedField<2>(modelData.trimsDisplay));
    internalField.Append(new BoolField<1>(modelData.frsky.ignoreSensorIds));
  }
  else if (IS_TARANIS(board) || (IS_ARM(board) && version >= 216)) {
    internalField.Append(new SpareBitsField<4>());
  }
  else {
    internalField.Append(new ConversionField< SignedField<4> >(modelData.moduleData[0].channelsCount, &channelsConversionTable, "Channels number", ::QObject::tr("OpenTX doesn't allow this number of channels")));
  }

  if (version >= 216)
    internalField.Append(new SignedField<3>(modelData.trimInc));
  else
    internalField.Append(new ConversionField< SignedField<3> >(modelData.trimInc, +2));

  internalField.Append(new BoolField<1>(modelData.disableThrottleWarning));

  if (IS_TARANIS(board) || (IS_ARM(board) && version >= 216))
    internalField.Append(new BoolField<1>(modelData.displayChecklist));
  else
    internalField.Append(new BoolField<1>(modelData.moduleData[0].ppm.pulsePol));

  internalField.Append(new BoolField<1>(modelData.extendedLimits));
  internalField.Append(new BoolField<1>(modelData.extendedTrims));
  internalField.Append(new BoolField<1>(modelData.throttleReversed));

  if (!IS_ARM(board) || version < 216) {
    internalField.Append(new ConversionField< SignedField<8> >(modelData.moduleData[0].ppm.delay, exportPpmDelay, importPpmDelay));
  }

  if (IS_ARM(board) || IS_2560(board))
    internalField.Append(new UnsignedField<16>(modelData.beepANACenter));
  else
    internalField.Append(new UnsignedField<8>(modelData.beepANACenter));

  for (int i=0; i<MAX_MIXERS(board, version); i++)
    internalField.Append(new MixField(modelData.mixData[i], board, version, &modelData));
  for (int i=0; i<MAX_CHANNELS(board, version); i++)
    internalField.Append(new LimitField(modelData.limitData[i], board, version));
  for (int i=0; i<MAX_EXPOS(board, version); i++)
    internalField.Append(new InputField(modelData.expoData[i], board, version));
  internalField.Append(new CurvesField(modelData.curves, board, version));
  for (int i=0; i<MAX_LOGICAL_SWITCHES(board, version); i++)
    internalField.Append(new LogicalSwitchField(modelData.logicalSw[i], board, version, variant, &modelData));
  for (int i=0; i<MAX_CUSTOM_FUNCTIONS(board, version); i++) {
    if (IS_ARM(board))
      internalField.Append(new ArmCustomFunctionField(modelData.customFn[i], board, version, variant));
    else
      internalField.Append(new AvrCustomFunctionField(modelData.customFn[i], board, version, variant));
  }
  internalField.Append(new HeliField(modelData.swashRingData, board, version, variant));
  for (int i=0; i<MAX_FLIGHT_MODES(board, version); i++) {
    internalField.Append(new FlightModeField(modelData.flightModeData[i], i, board, version));
  }

  if (!IS_ARM(board) || version < 216) {
    internalField.Append(new SignedField<8>(modelData.moduleData[0].ppm.frameLength));
  }

  internalField.Append(new UnsignedField<8>(modelData.thrTraceSrc, "Throttle Source"));

  if (!afterrelease21March2013) {
    internalField.Append(new UnsignedField<8>(modelData.moduleData[0].modelId));
  }

  if (IS_TARANIS_X9E(board))
    internalField.Append(new SwitchesWarningField<64>(modelData.switchWarningStates, board, version));
  else if (IS_TARANIS(board))
    internalField.Append(new SwitchesWarningField<16>(modelData.switchWarningStates, board, version));
  else
    internalField.Append(new SwitchesWarningField<8>(modelData.switchWarningStates, board, version));


  if (IS_TARANIS_X9E(board))
    internalField.Append(new UnsignedField<32>(modelData.switchWarningEnable));
  else if (version >= 216)
    internalField.Append(new UnsignedField<8>(modelData.switchWarningEnable));

  if ((board == BOARD_STOCK || (board == BOARD_M128 && version >= 215)) && (variant & GVARS_VARIANT)) {
    for (int i=0; i<MAX_GVARS(board, version); i++) {
      // on M64 GVARS are common to all phases, and there is no name
      internalField.Append(new SignedField<16>(modelData.flightModeData[0].gvars[i]));
    }
  }

  if (board != BOARD_STOCK && (board != BOARD_M128 || version < 215)) {
    for (int i=0; i<MAX_GVARS(board, version); i++) {
      internalField.Append(new ZCharField<6>(modelData.gvars_names[i]));
      if (version >= 216) {
        internalField.Append(new BoolField<1>(modelData.gvars_popups[i]));
        internalField.Append(new SpareBitsField<7>());
      }
    }
  }

  if ((board != BOARD_STOCK && (board != BOARD_M128 || version < 215)) || (variant & FRSKY_VARIANT)) {
    internalField.Append(new FrskyField(modelData.frsky, board, version, variant));
  }
  else if ((board == BOARD_STOCK || board == BOARD_M128) && (variant & MAVLINK_VARIANT)) {
    internalField.Append(new MavlinkField(modelData.mavlink, board, version));
  }

  if (IS_TARANIS(board) && version < 215) {
    internalField.Append(new CharField<10>(modelData.bitmap));
  }

  int modulesCount = 2;

  if (IS_TARANIS(board)) {
    modulesCount = 3;
    if (version >= 217) {
      internalField.Append(new SpareBitsField<3>());
      internalField.Append(new UnsignedField<3>(modelData.trainerMode));
      internalField.Append(new UnsignedField<2>(modelData.potsWarningMode));
    }
    else {
      internalField.Append(new ConversionField< SignedField<8> >(modelData.moduleData[1].protocol, &protocolsConversionTable, "Protocol", ::QObject::tr("OpenTX doesn't accept this radio protocol")));
      internalField.Append(new UnsignedField<8>(modelData.trainerMode));
    }
  }
  else if (IS_ARM(board)) {
    if (version >= 217) {
      modulesCount = 3;
      internalField.Append(new SpareBitsField<6>());
      internalField.Append(new UnsignedField<2>(modelData.potsWarningMode));
    }
    else if (version >= 216) {
      modulesCount = 3;
      internalField.Append(new ConversionField< SignedField<8> >(modelData.moduleData[0].protocol, &protocolsConversionTable, "Protocol", ::QObject::tr("OpenTX doesn't accept this radio protocol")));
    }
  }

  if (IS_ARM(board) && version >= 215) {
    for (int module=0; module<modulesCount; module++) {
      if (version >= 217) {
        internalField.Append(new ConversionField<SignedField<4> >(modelData.moduleData[module].protocol, &protocolsConversionTable, "Protocol", ::QObject::tr("OpenTX doesn't accept this radio protocol")));
        internalField.Append(new SignedField<4>(subprotocols[module]));
      }
      else {
        internalField.Append(new SignedField<8>(subprotocols[module]));
      }
      internalField.Append(new UnsignedField<8>(modelData.moduleData[module].channelsStart));
      internalField.Append(new ConversionField<SignedField<8> >(modelData.moduleData[module].channelsCount, -8));
      if (version >= 217) {
        internalField.Append(new UnsignedField<4>(modelData.moduleData[module].failsafeMode));
        internalField.Append(new UnsignedField<3>(modelData.moduleData[module].subType));
        internalField.Append(new BoolField<1>(modelData.moduleData[module].invertedSerial));
      } else
        internalField.Append(new ConversionField< UnsignedField<8> >(modelData.moduleData[module].failsafeMode, -1));
      for (int i=0; i<32; i++) {
        internalField.Append(new SignedField<16>(modelData.moduleData[module].failsafeChannels[i]));
      }
      if (version >= 217) {
        internalField.Append(new ConversionField< SignedField<6> >(modelData.moduleData[module].ppm.delay, exportPpmDelay, importPpmDelay));
        internalField.Append(new BoolField<1>(modelData.moduleData[module].ppm.pulsePol));
        internalField.Append(new BoolField<1>(modelData.moduleData[module].ppm.outputType));
        internalField.Append(new SignedField<8>(modelData.moduleData[module].ppm.frameLength));
      }
      else {
        internalField.Append(new ConversionField< SignedField<8> >(modelData.moduleData[module].ppm.delay, exportPpmDelay, importPpmDelay));
        internalField.Append(new SignedField<8>(modelData.moduleData[module].ppm.frameLength));
        internalField.Append(new BoolField<8>(modelData.moduleData[module].ppm.pulsePol));
      }
    }
  }

  if (IS_TARANIS(board) && version < 218) {
    for (int i=0; i<MAX_CURVES(board, version); i++) {
      internalField.Append(new ZCharField<6>(modelData.curves[i].name));
    }
  }

  if (IS_TARANIS(board)) {
    if (version >= 217) {
      for (int i=0; i<7; i++) {
        ScriptData & script = modelData.scriptData[i];
        internalField.Append(new CharField<8>(script.filename));
        internalField.Append(new ZCharField<8>(script.name));
        for (int j=0; j<8; j++) {
          internalField.Append(new SignedField<8>(script.inputs[j]));
        }
      }
    }
    else if (version >= 216) {
      for (int i=0; i<7; i++) {
        ScriptData & script = modelData.scriptData[i];
        internalField.Append(new CharField<10>(script.filename));
        internalField.Append(new ZCharField<10>(script.name));
        for (int j=0; j<10; j++) {
          internalField.Append(new SignedField<8>(script.inputs[j]));
        }
      }
    }
  }

  if (IS_TARANIS(board) && version >= 216) {
    for (int i=0; i<32; i++) {
      internalField.Append(new ZCharField<4>(modelData.inputNames[i]));
    }
  }

  if (IS_ARM(board) && version >= 217) {
    for (int i=0; i<8; i++) {
      if (i < MAX_POTS(board, version)+MAX_SLIDERS(board))
        internalField.Append(new BoolField<1>(modelData.potsWarningEnabled[i]));
      else
        internalField.Append(new SpareBitsField<1>());
    }
  }
  else if (IS_ARM(board) && version >= 216) {
    for (int i=0; i<6; i++) {
      if (i < MAX_POTS(board, version)+MAX_SLIDERS(board))
        internalField.Append(new BoolField<1>(modelData.potsWarningEnabled[i]));
      else
        internalField.Append(new SpareBitsField<1>());
    }
    internalField.Append(new UnsignedField<2>(modelData.potsWarningMode));
  }

  if (IS_ARM(board) && version >= 216) {
    for (int i=0; i < MAX_POTS(board, version)+MAX_SLIDERS(board); i++) {
      internalField.Append(new SignedField<8>(modelData.potPosition[i]));
    }
  }

  if (IS_ARM(board) && version == 216) {
    internalField.Append(new SpareBitsField<16>());
  }

  if (IS_SKY9X(board) && version >= 217) {
    internalField.Append(new SpareBitsField<8>());
    internalField.Append(new SpareBitsField<8>());
  }

  if (IS_ARM(board) && version >= 217) {
    for (int i=0; i<MAX_TELEMETRY_SENSORS(board, version); ++i) {
      internalField.Append(new SensorField(modelData.sensorData[i], board, version));
    }
  }

  if (IS_TARANIS_X9E(board)) {
    internalField.Append(new UnsignedField<8>(modelData.toplcdTimer));
  }
}

void OpenTxModelData::beforeExport()
{
  // qDebug() << QString("before export model") << modelData.name;

  for (int module=0; module<3; module++) {
    if (modelData.moduleData[module].protocol >= PULSES_PXX_XJT_X16 && modelData.moduleData[module].protocol <= PULSES_PXX_XJT_LR12)
      subprotocols[module] = modelData.moduleData[module].protocol - PULSES_PXX_XJT_X16;
    else if (modelData.moduleData[module].protocol >= PULSES_LP45 && modelData.moduleData[module].protocol <= PULSES_DSMX)
      subprotocols[module] = modelData.moduleData[module].protocol - PULSES_LP45;
    else if (modelData.moduleData[module].protocol == PULSES_MULTIMODULE) {
      // copy multi settings to ppm settings to get them written to the eeprom
      // (reverse the int => ms logic of the ppm delay) since only ppm is written

      subprotocols[module] = modelData.moduleData[module].multi.rfProtocol & (0x1f);
      int multiByte = ((modelData.moduleData[module].multi.rfProtocol >> 4) & 0x03) | (modelData.moduleData[module].multi.customProto << 7);
      modelData.moduleData[module].ppm.delay = 300 + 50 * multiByte;
      modelData.moduleData[module].ppm.frameLength = modelData.moduleData[module].multi.optionValue;
      modelData.moduleData[module].ppm.outputType = modelData.moduleData[module].multi.lowPowerMode;
      modelData.moduleData[module].ppm.pulsePol = modelData.moduleData[module].multi.autoBindMode;
    } else
      subprotocols[module] = (module==0 ? -1 : 0);
  }
}

void OpenTxModelData::afterImport()
{
  eepromImportDebug() << QString("OpenTxModelData::afterImport()") << modelData.name;

  if (IS_TARANIS(board) && version < 216) {
    for (unsigned int i=0; i<NUM_STICKS; i++) {
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
      strncpy(modelData.inputNames[i], AnalogString(i).toLatin1().constData(), sizeof(modelData.inputNames[i])-1);
    }
  }

  for (int module=0; module<3; module++) {
    if (modelData.moduleData[module].protocol == PULSES_PXX_XJT_X16 || modelData.moduleData[module].protocol == PULSES_LP45) {
      if (subprotocols[module] >= 0)
        modelData.moduleData[module].protocol += subprotocols[module];
      else
        modelData.moduleData[module].protocol = PULSES_OFF;
    } else if (modelData.moduleData[module].protocol == PULSES_MULTIMODULE) {
      // Copy data from ppm struct to multi struct
      unsigned int multiByte = (unsigned  int)((modelData.moduleData[module].ppm.delay - 300) / 50);
      modelData.moduleData[module].multi.rfProtocol = subprotocols[module]  | ((multiByte & 0x3) << 4);

      modelData.moduleData[module].multi.customProto = (multiByte & 0x80) == 0x80;
      modelData.moduleData[module].multi.optionValue = modelData.moduleData[module].ppm.frameLength;
      modelData.moduleData[module].multi.lowPowerMode = modelData.moduleData[module].ppm.outputType;
      modelData.moduleData[module].multi.autoBindMode = modelData.moduleData[module].ppm.pulsePol;
    }
  }

  if (IS_TARANIS(board) && version < 217 && modelData.moduleData[1].protocol != PULSES_OFF) {
    modelData.moduleData[1].modelId = modelData.moduleData[0].modelId;
  }
}

OpenTxGeneralData::OpenTxGeneralData(GeneralSettings & generalData, BoardEnum board, unsigned int version, unsigned int variant):
  TransformedField(internalField),
  internalField("General Settings"),
  generalData(generalData),
  board(board),
  version(version),
  inputsCount(NUM_STICKS+MAX_POTS(board, version)+MAX_SLIDERS(board))
{
  eepromImportDebug() << QString("OpenTxGeneralData::OpenTxGeneralData(board: %1, version:%2, variant:%3)").arg(board).arg(version).arg(variant);

  generalData.version = version;
  generalData.variant = variant;

  internalField.Append(new UnsignedField<8>(generalData.version));
  if (version >= 213 || (!IS_ARM(board) && version >= 212))
    internalField.Append(new UnsignedField<16>(generalData.variant));

  if (version >= 216) {
    for (int i=0; i<inputsCount; i++) {
      internalField.Append(new SignedField<16>(generalData.calibMid[i]));
      internalField.Append(new SignedField<16>(generalData.calibSpanNeg[i]));
      internalField.Append(new SignedField<16>(generalData.calibSpanPos[i]));
    }
  }
  else {
    for (int i=0; i<inputsCount; i++) {
      if (!IS_TARANIS(board) || i!=6)
        internalField.Append(new SignedField<16>(generalData.calibMid[i]));
    }
    for (int i=0; i<inputsCount; i++) {
      if (!IS_TARANIS(board) || i!=6)
        internalField.Append(new SignedField<16>(generalData.calibSpanNeg[i]));
    }
    for (int i=0; i<inputsCount; i++) {
      if (!IS_TARANIS(board) || i!=6)
        internalField.Append(new SignedField<16>(generalData.calibSpanPos[i]));
    }
  }

  internalField.Append(new UnsignedField<16>(chkSum));
  internalField.Append(new UnsignedField<8>(generalData.currModel));
  internalField.Append(new UnsignedField<8>(generalData.contrast));
  internalField.Append(new UnsignedField<8>(generalData.vBatWarn));
  internalField.Append(new SignedField<8>(generalData.txVoltageCalibration));
  internalField.Append(new SignedField<8>(generalData.backlightMode));

  for (int i=0; i<NUM_STICKS; i++) {
    internalField.Append(new SignedField<16>(generalData.trainer.calib[i]));
  }
  for (int i=0; i<NUM_STICKS; i++) {
    internalField.Append(new UnsignedField<6>(generalData.trainer.mix[i].src));
    internalField.Append(new UnsignedField<2>(generalData.trainer.mix[i].mode));
    internalField.Append(new SignedField<8>(generalData.trainer.mix[i].weight));
  }

  internalField.Append(new UnsignedField<8>(generalData.view, 0, MAX_VIEWS(board)-1));

  internalField.Append(new SpareBitsField<2>()); // TODO buzzerMode?
  internalField.Append(new BoolField<1>(generalData.fai));
  internalField.Append(new SignedField<2>((int &)generalData.beeperMode));
  internalField.Append(new BoolField<1>(generalData.flashBeep));
  internalField.Append(new BoolField<1>(generalData.disableMemoryWarning));
  internalField.Append(new BoolField<1>(generalData.disableAlarmWarning));

  internalField.Append(new UnsignedField<2>(generalData.stickMode));
  internalField.Append(new SignedField<5>(generalData.timezone));
  if (version >= 217 && IS_TARANIS(board)) {
    internalField.Append(new BoolField<1>(generalData.adjustRTC));
  }
  else {
    internalField.Append(new SpareBitsField<1>());
  }

  internalField.Append(new UnsignedField<8>(generalData.inactivityTimer));
  if (IS_9X(board)) {
    if (version >= 215) {
      internalField.Append(new UnsignedField<3>(generalData.mavbaud));
    }
    else {
      internalField.Append(new SpareBitsField<1>());
      internalField.Append(new BoolField<1>(generalData.minuteBeep));
      internalField.Append(new BoolField<1>(generalData.preBeep));
    }
  }
  else {
    internalField.Append(new SpareBitsField<3>());
  }
  if (version >= 216 && IS_TARANIS(board))
    internalField.Append(new SignedField<3>(generalData.splashDuration));
  else if (version >= 213 || (!IS_ARM(board) && version >= 212))
    internalField.Append(new UnsignedField<3>(generalData.splashMode)); // TODO
  else
    internalField.Append(new SpareBitsField<3>());
  internalField.Append(new SignedField<2>((int &)generalData.hapticMode));

  if (IS_ARM(board))
    internalField.Append(new SignedField<8>(generalData.switchesDelay));
  else
    internalField.Append(new SpareBitsField<8>()); // TODO blOffBright + blOnBright

  internalField.Append(new UnsignedField<8>(generalData.backlightDelay));
  internalField.Append(new UnsignedField<8>(generalData.templateSetup));
  internalField.Append(new SignedField<8>(generalData.PPM_Multiplier));
  internalField.Append(new SignedField<8>(generalData.hapticLength));

  if (version < 216 || (version < 218 && !IS_9X(board)) || (!IS_9X(board) && !IS_TARANIS(board))) {
    internalField.Append(new UnsignedField<8>(generalData.reNavigation));
  }

  if (version >= 216 && !IS_TARANIS(board)) {
    internalField.Append(new UnsignedField<8>(generalData.stickReverse));
  }

  internalField.Append(new SignedField<3>(generalData.beeperLength));
  internalField.Append(new SignedField<3>(generalData.hapticStrength));
  internalField.Append(new UnsignedField<1>(generalData.gpsFormat));
  internalField.Append(new SpareBitsField<1>()); // unexpectedShutdown

  internalField.Append(new UnsignedField<8>(generalData.speakerPitch));

  if (IS_ARM(board))
    internalField.Append(new ConversionField< SignedField<8> >(generalData.speakerVolume, -12, 0, 0, 23, "Volume"));
  else
    internalField.Append(new ConversionField< SignedField<8> >(generalData.speakerVolume, -7, 0, 0, 7, "Volume"));

  if (version >= 214 || (!IS_ARM(board) && version >= 213)) {
    internalField.Append(new SignedField<8>(generalData.vBatMin));
    internalField.Append(new SignedField<8>(generalData.vBatMax));
  }

  if (IS_ARM(board)) {
    internalField.Append(new UnsignedField<8>(generalData.backlightBright));
    if (version < 218) internalField.Append(new SignedField<8>(generalData.txCurrentCalibration));
    if (version >= 213) {
      if (version < 218) internalField.Append(new SignedField<8>(generalData.temperatureWarn)); // TODO
      if (version < 218) internalField.Append(new UnsignedField<8>(generalData.mAhWarn));
      if (version < 218) internalField.Append(new UnsignedField<16>(generalData.mAhUsed));
      internalField.Append(new UnsignedField<32>(generalData.globalTimer));
      if (version < 218) internalField.Append(new SignedField<8>(generalData.temperatureCalib)); // TODO
      internalField.Append(new UnsignedField<8>(generalData.btBaudrate)); // TODO
      if (version < 218) internalField.Append(new BoolField<8>(generalData.optrexDisplay)); //TODO
      if (version < 218) internalField.Append(new UnsignedField<8>(generalData.sticksGain)); // TODO
    }
    if (version >= 214) {
      if (version < 218) internalField.Append(new UnsignedField<8>(generalData.rotarySteps)); // TODO
      internalField.Append(new UnsignedField<8>(generalData.countryCode));
      internalField.Append(new UnsignedField<8>(generalData.imperial));
    }
    if (version >= 215) {
      internalField.Append(new CharField<2>(generalData.ttsLanguage));
      if (version < 218) {
        internalField.Append(new SignedField<8>(generalData.beepVolume));
        internalField.Append(new SignedField<8>(generalData.wavVolume));
        internalField.Append(new SignedField<8>(generalData.varioVolume));
      }
      else {
        internalField.Append(new SignedField<4>(generalData.beepVolume));
        internalField.Append(new SignedField<4>(generalData.wavVolume));
        internalField.Append(new SignedField<4>(generalData.varioVolume));
        internalField.Append(new SignedField<4>(generalData.backgroundVolume));
      }
      if (version >= 216) {
        internalField.Append(new SignedField<8>(generalData.varioPitch));
        internalField.Append(new SignedField<8>(generalData.varioRange));
        internalField.Append(new SignedField<8>(generalData.varioRepeat));
      }
      if (version < 218) internalField.Append(new SignedField<8>(generalData.backgroundVolume));
    }
    if (version >= 218) {
      for (int i=0; i<MAX_CUSTOM_FUNCTIONS(board, version); i++) {
        internalField.Append(new ArmCustomFunctionField(generalData.customFn[i], board, version, variant));
      }
    }
    if (IS_TARANIS(board) && version >= 216) {
      if (version >= 217) {
        internalField.Append(new UnsignedField<6>(generalData.hw_uartMode));
        if (IS_TARANIS_X9E(board)) {
          internalField.Append(new UnsignedField<1>(generalData.sliderConfig[2]));
          internalField.Append(new UnsignedField<1>(generalData.sliderConfig[3]));
        }
        else {
          internalField.Append(new SpareBitsField<2>());
        }
      }
      else {
        internalField.Append(new UnsignedField<8>(generalData.hw_uartMode));
      }
      for (int i=0; i<4; i++) {
        if (i < MAX_POTS(board, version))
          internalField.Append(new UnsignedField<2>(generalData.potConfig[i]));
        else
          internalField.Append(new SpareBitsField<2>());
      }
      internalField.Append(new UnsignedField<8>(generalData.backlightColor));
    }

    if (IS_TARANIS_X9E(board))
      internalField.Append(new SpareBitsField<64>()); // switchUnlockStates
    else if (IS_TARANIS(board))
      internalField.Append(new SpareBitsField<16>()); // switchUnlockStates

    if (version == 217) {
      for (int i=0; i<MAX_CUSTOM_FUNCTIONS(board, version); i++) {
        internalField.Append(new ArmCustomFunctionField(generalData.customFn[i], board, version, variant));
      }
    }

    if (IS_TARANIS(board) && version >= 217) {
      for (int i=0; i<MAX_SWITCHES(board, version); i++) {
        internalField.Append(new UnsignedField<2>(generalData.switchConfig[i]));
      }
      if (IS_TARANIS_X9E(board)) {
        internalField.Append(new SpareBitsField<64-2*18>());
      }
      for (int i=0; i<MAX_SWITCHES(board, version); ++i) {
        internalField.Append(new ZCharField<3>(generalData.switchName[i]));
      }
      for (int i=0; i<NUM_STICKS; ++i) {
        internalField.Append(new ZCharField<3>(generalData.stickName[i]));
      }
      for (int i=0; i<MAX_POTS(board, version); ++i) {
        internalField.Append(new ZCharField<3>(generalData.potName[i]));
      }
      for (int i=0; i<MAX_SLIDERS(board); ++i) {
        internalField.Append(new ZCharField<3>(generalData.sliderName[i]));
      }
    }

    if (IS_TARANIS_X9E(board) && version >= 217) {
      internalField.Append(new BoolField<8>(generalData.bluetoothEnable));
      internalField.Append(new ZCharField<10>(generalData.bluetoothName));
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
    generalData.potConfig[0] = GeneralSettings::POT_WITH_DETENT;
    generalData.potConfig[1] = GeneralSettings::POT_WITH_DETENT;
  }
}
