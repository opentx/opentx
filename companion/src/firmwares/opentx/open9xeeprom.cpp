#include <stdlib.h>
#include <algorithm>
#include "helpers.h"
#include "open9xeeprom.h"
#include <QObject>

#define IS_DBLEEPROM(board, version)         ((board==BOARD_GRUVIN9X || board==BOARD_M128) && version >= 213)
// Macro used for Gruvin9x board and M128 board between versions 213 and 214 (when there were stack overflows!)
#define IS_DBLRAM(board, version)            ((board==BOARD_GRUVIN9X && version >= 213) || (board==BOARD_M128 && version >= 213 && version <= 214))

#define HAS_PERSISTENT_TIMERS(board)         (IS_ARM(board) || board == BOARD_GRUVIN9X)
#define HAS_LARGE_LCD(board)                 IS_TARANIS(board)
#define MAX_VIEWS(board)                     (HAS_LARGE_LCD(board) ? 2 : 256)
#define MAX_POTS(board)                      (IS_TARANIS(board) ? 4 : 3)
#define MAX_SWITCHES(board)                  (IS_TARANIS(board) ? 8 : 7)
#define MAX_SWITCHES_POSITION(board)         (IS_TARANIS(board) ? 22 : 9)
#define MAX_ROTARY_ENCODERS(board)           (board==BOARD_GRUVIN9X ? 2 : (board==BOARD_SKY9X ? 1 : 0))
#define MAX_PHASES(board, version)           (IS_ARM(board) ? 9 :  (IS_DBLRAM(board, version) ? 6 :  5))
#define MAX_MIXERS(board, version)           (IS_ARM(board) ? 64 : 32)
#define MAX_CHANNELS(board, version)         (IS_ARM(board) ? 32 : 16)
#define MAX_EXPOS(board, version)            (IS_ARM(board) ? 32 : (IS_DBLRAM(board, version) ? 16 : 14))
#define MAX_CUSTOM_SWITCHES(board, version)  (IS_ARM(board) ? 32 : (IS_DBLEEPROM(board, version) ? 15 : 12))
#define MAX_CUSTOM_FUNCTIONS(board, version) (IS_ARM(board) ? 32 : (IS_DBLEEPROM(board, version) ? 24 : 16))
#define MAX_CURVES(board)                    (IS_ARM(board) ? O9X_ARM_MAX_CURVES : O9X_MAX_CURVES)

#define IS_RELEASE_21_MARCH_2013(board, version) (version >= 214 || (!IS_ARM(board) && version >= 213))
#define IS_RELEASE_23_MARCH_2013(board, version) (version >= 214 || (board==BOARD_STOCK && version >= 213))

inline int switchIndex(int i, BoardEnum board, unsigned int version)
{
  bool release21March2013 = IS_RELEASE_21_MARCH_2013(board, version);
  if (!IS_TARANIS(board) && release21March2013)
    return (i<=3 ? i+3 : (i<=6 ? i-3 : i));
  else
    return i;
}

class SwitchesConversionTable: public ConversionTable {

  public:
    SwitchesConversionTable(BoardEnum board, unsigned int version, unsigned long flags=0)
    {
      int val=0;

      addConversion(RawSwitch(SWITCH_TYPE_NONE), val++);

      for (int i=1; i<=MAX_SWITCHES_POSITION(board); i++) {
        int s = switchIndex(i, board, version);
        addConversion(RawSwitch(SWITCH_TYPE_SWITCH, -s), -val);
        addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val++);
      }

      for (int i=1; i<=MAX_CUSTOM_SWITCHES(board, version); i++) {
        addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, -i), -val);
        addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
      }

      addConversion(RawSwitch(SWITCH_TYPE_OFF), -val);
      addConversion(RawSwitch(SWITCH_TYPE_ON), val++);

      for (int i=1; i<=MAX_SWITCHES_POSITION(board); i++) {
        int s = switchIndex(i, board, version);
        addConversion(RawSwitch(SWITCH_TYPE_MOMENT_SWITCH, -s), -val);
        addConversion(RawSwitch(SWITCH_TYPE_MOMENT_SWITCH, s), val++);
      }

      for (int i=1; i<=MAX_CUSTOM_SWITCHES(board, version); i++) {
        addConversion(RawSwitch(SWITCH_TYPE_MOMENT_VIRTUAL, -i), -val);
        addConversion(RawSwitch(SWITCH_TYPE_MOMENT_VIRTUAL, i), val++);
      }

      addConversion(RawSwitch(SWITCH_TYPE_ONM, 1 ), -val);
      addConversion(RawSwitch(SWITCH_TYPE_ONM, 0 ), val++);
      addConversion(RawSwitch(SWITCH_TYPE_TRN, 0), val++);
      addConversion(RawSwitch(SWITCH_TYPE_TRN, 1), val++);        
      addConversion(RawSwitch(SWITCH_TYPE_REA, 0), val++);
      addConversion(RawSwitch(SWITCH_TYPE_REA, 1), val++);        
    }

  protected:

    void addConversion(const RawSwitch & sw, const int b)
    {
      ConversionTable::addConversion(sw.toValue(), b);
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

  public:

    static SwitchesConversionTable * getInstance(BoardEnum board, unsigned int version, unsigned long flags=0)
    {
      static std::list<Cache> internalCache;

      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache element = *it;
        if (element.board == board && element.version == version && element.flags == flags)
          return element.table;
      }

      Cache element(board, version, flags, new SwitchesConversionTable(board, version, flags));
      internalCache.push_back(element);
      return element.table;
    }
};

#define FLAG_NONONE       0x01
#define FLAG_NOSWITCHES   0x02
#define FLAG_NOTELEMETRY  0x04

class SourcesConversionTable: public ConversionTable {

  public:
    SourcesConversionTable(BoardEnum board, unsigned int version, unsigned int variant, unsigned long flags=0)
    {
      bool release21March2013 = IS_RELEASE_21_MARCH_2013(board, version);

      int val=0;

      if (!(flags & FLAG_NONONE)) {
        addConversion(RawSource(SOURCE_TYPE_NONE), val++);
      }

      for (int i=0; i<4+MAX_POTS(board); i++)
        addConversion(RawSource(SOURCE_TYPE_STICK, i), val++);

      for (int i=0; i<MAX_ROTARY_ENCODERS(board); i++)
        addConversion(RawSource(SOURCE_TYPE_ROTARY_ENCODER, 0), val++);

      if (!release21March2013) {
        for (int i=0; i<NUM_STICKS; i++)
          addConversion(RawSource(SOURCE_TYPE_TRIM, i), val++);
      }

      addConversion(RawSource(SOURCE_TYPE_MAX), val++);

      if (release21March2013) {
        for (int i=0; i<3; i++)
          addConversion(RawSource(SOURCE_TYPE_CYC, i), val++);
      }

      if (release21March2013) {
        for (int i=0; i<NUM_STICKS; i++)
          addConversion(RawSource(SOURCE_TYPE_TRIM, i), val++);
      }

      addConversion(RawSource(SOURCE_TYPE_SWITCH, 0), val++);

      if (!(flags & FLAG_NOSWITCHES)) {
        if (release21March2013) {
          for (int i=1; i<MAX_SWITCHES(board); i++)
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
        for (int i=0; i<MAX_CUSTOM_SWITCHES(board, version); i++)
          addConversion(RawSource(SOURCE_TYPE_CUSTOM_SWITCH, i), val++);
      }

      if (!release21March2013) {
        for (int i=0; i<3; i++)
          addConversion(RawSource(SOURCE_TYPE_CYC, i), val++);
      }

      for (int i=0; i<8; i++)
        addConversion(RawSource(SOURCE_TYPE_PPM, i), val++);

      for (int i=0; i<MAX_CHANNELS(board, version); i++)
        addConversion(RawSource(SOURCE_TYPE_CH, i), val++);

      if (!(flags & FLAG_NOTELEMETRY)) {
        if (release21March2013) {
          if ((board != BOARD_STOCK && (board!=BOARD_M128 || version<215)) || (variant & GVARS_VARIANT)) {
            for (int i=0; i<5; i++)
              addConversion(RawSource(SOURCE_TYPE_GVAR, i), val++);
          }
        }

        if (release21March2013)
          addConversion(RawSource(SOURCE_TYPE_TELEMETRY, 0), val++);

        for (int i=1; i<TELEMETRY_SOURCE_ACC; i++)
          addConversion(RawSource(SOURCE_TYPE_TELEMETRY, i), val++);
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

  public:

    static SourcesConversionTable * getInstance(BoardEnum board, unsigned int version, unsigned int variant, unsigned long flags=0)
    {
      static std::list<Cache> internalCache;

      for (std::list<Cache>::iterator it=internalCache.begin(); it!=internalCache.end(); it++) {
        Cache element = *it;
        if (element.board == board && element.version == version && element.variant == variant && element.flags == flags)
          return element.table;
      }

      Cache element(board, version, variant, flags, new SourcesConversionTable(board, version, variant, flags));
      internalCache.push_back(element);
      return element.table;
    }
};

class TimerModeConversionTable: public ConversionTable {

  public:
    TimerModeConversionTable(BoardEnum board, unsigned int version, unsigned long flags=0)
    {
      int val=0;

      addConversion(TMRMODE_OFF, val++);
      addConversion(TMRMODE_ABS, val++);
      addConversion(TMRMODE_THs, val++);
      addConversion(TMRMODE_THp, val++);
      addConversion(TMRMODE_THt, val++);

      int swCount = MAX_SWITCHES_POSITION(board) + MAX_CUSTOM_SWITCHES(board, version);

      for (int i=0; i<swCount; i++) {
        int s = switchIndex(i+1, board, version) - 1;
        addConversion(TMRMODE_FIRST_SWITCH+i, val+s);
        addConversion(TMRMODE_FIRST_MOMENT_SWITCH+i, val+s+swCount);
        addConversion(TMRMODE_FIRST_NEG_SWITCH-i, -1-s);
        addConversion(TMRMODE_FIRST_NEG_MOMENT_SWITCH-i, -1-s-swCount);
      }
    }
};

class TimerModeField: public ConversionField< SignedField<8> > {
  public:
    TimerModeField(TimerMode & mode, BoardEnum board, unsigned int version, unsigned long flags=0):
      ConversionField< SignedField<8> >(_mode, &conversionTable, "TimerMode"),
      conversionTable(board, version, flags),
      mode(mode),
      _mode(0)
    {
    }

  virtual void beforeExport()
  {
    _mode = mode;
    ConversionField< SignedField<8> >::beforeExport();
  }

  virtual void afterImport()
  {
    ConversionField< SignedField<8> >::afterImport();
    mode = (TimerMode)_mode;
  }

  protected:
    TimerModeConversionTable conversionTable;
    TimerMode & mode;
    int _mode;
};

template <int N>
class SwitchField: public ConversionField< SignedField<N> > {
  public:
    SwitchField(RawSwitch & sw, BoardEnum board, unsigned int version, unsigned long flags=0):
      ConversionField< SignedField<N> >(_switch, SwitchesConversionTable::getInstance(board, version, flags), "Switch"),
      sw(sw),
      _switch(0)
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
    }    
    
  protected:
    RawSwitch & sw;
    int _switch;
};

template <int N>
class SourceField: public ConversionField< UnsignedField<N> > {
  public:
    SourceField(RawSource & source, BoardEnum board, unsigned int version, unsigned int variant, unsigned long flags=0):
      ConversionField< UnsignedField<N> >(_source, SourcesConversionTable::getInstance(board, version, variant, flags), "Source"),
      source(source),
      _source(0)
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
    }    

  protected:
    RawSource & source;
    unsigned int _source;
};

class HeliField: public StructField {
  public:
    HeliField(SwashRingData & heli, BoardEnum board, unsigned int version, unsigned int variant)
    {
      Append(new BoolField<1>(heli.invertELE));
      Append(new BoolField<1>(heli.invertAIL));
      Append(new BoolField<1>(heli.invertCOL));
      Append(new UnsignedField<5>(heli.type));
      Append(new SourceField<8>(heli.collectiveSource, board, version, variant));
      //, FLAG_NOSWITCHES)); Fix shift in collective
      Append(new UnsignedField<8>(heli.value));
    }
};

class PhaseField: public TransformedField {
  public:
    PhaseField(PhaseData & phase, int index, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField("Phase"),
      phase(phase),
      index(index),
      board(board),
      version(version),
      rotencCount(IS_ARM(board) ? 1 : (board == BOARD_GRUVIN9X ? 2 : 0))
    {
      if (board == BOARD_STOCK || (board==BOARD_M128 && version>=215)) {
        // On stock we use 10bits per trim
        for (int i=0; i<NUM_STICKS; i++)
          internalField.Append(new SignedField<8>(trimBase[i]));
        for (int i=0; i<NUM_STICKS; i++)
          internalField.Append(new SignedField<2>(trimExt[i]));
      }
      else {
        for (int i=0; i<NUM_STICKS; i++)
          internalField.Append(new SignedField<16>(trimBase[i]));
      }

      internalField.Append(new SwitchField<8>(phase.swtch, board, version));
      if (HAS_LARGE_LCD(board))
        internalField.Append(new ZCharField<10>(phase.name));
      else
        internalField.Append(new ZCharField<6>(phase.name));

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
        for (int i=0; i<O9X_MAX_GVARS; i++) {
          internalField.Append(new SignedField<16>(phase.gvars[i]));
        }
      }
    }

    virtual void beforeExport()
    {
      for (int i=0; i<NUM_STICKS; i++) {
        int trim;
        if (phase.trimRef[i] >= 0) {
          trim = 501 + phase.trimRef[i] - (phase.trimRef[i] >= index ? 1 : 0);
        }
        else {
          trim = std::max(-500, std::min(500, phase.trim[i]));
        }
        if (board == BOARD_STOCK || (board == BOARD_M128 && version >= 215)) {
          trimBase[i] = trim >> 2;
          trimExt[i] = (trim & 0x03);
        }
        else {
          trimBase[i] = trim;
        }
      }
    }

    virtual void afterImport()
    {
      for (int i=0; i<NUM_STICKS; i++) {
        int trim;
        if (board == BOARD_STOCK || (board == BOARD_M128 && version >= 215))
          trim = ((trimBase[i]) << 2) + (trimExt[i] & 0x03);
        else
          trim = trimBase[i];
        if (trim > 500) {
          phase.trimRef[i] = trim - 501;
          if (phase.trimRef[i] >= index)
            phase.trimRef[i] += 1;
          phase.trim[i] = 0;
        }
        else {
          phase.trim[i] = trim;
        }

      }
    }

  protected:
    StructField internalField;
    PhaseData & phase;
    int index;
    BoardEnum board;
    unsigned int version;
    int rotencCount;
    int trimBase[NUM_STICKS];
    int trimExt[NUM_STICKS];
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

void exportGvarParam(const int gvar, int & _gvar)
{
  if (gvar < -10000) {
    _gvar = 512 + gvar + 10000;
  }
  else if (gvar > 10000) {
    _gvar = 512 + gvar - 10001;
  }
  else {
    _gvar = gvar;
  }
}

void importGvarParam(int & gvar, const int _gvar)
{
  if (_gvar >= 512) {
    gvar = 10001 + _gvar - 512;
  }
  else if (_gvar >= 512-5) {
    gvar = -10000 + _gvar - 512;
  }
  else if (_gvar < -512) {
    gvar = -10000 + _gvar + 513;
  }
  else if (_gvar < -512+5) {
    gvar = 10000 + _gvar + 513;
  }
  else {
    gvar = _gvar;
  }

  // qDebug() << QString("import") << _gvar << gvar;
}

class MixField: public TransformedField {
  public:
    MixField(MixData & mix, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField("Mix"),
      mix(mix),
      board(board),
      version(version)
    {
      if (IS_ARM(board)) {
        internalField.Append(new UnsignedField<8>(_destCh));
        internalField.Append(new UnsignedField<16>(mix.phases));
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
          internalField.Append(new UnsignedField<4>(mix.srcVariant));
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
      else if (IS_DBLRAM(board, version) && IS_RELEASE_23_MARCH_2013(board, version)) {
        internalField.Append(new UnsignedField<4>(_destCh));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new BoolField<1>(mix.noExpo));
        internalField.Append(new UnsignedField<1>(_weightMode));
        internalField.Append(new UnsignedField<1>(_offsetMode));
        internalField.Append(new SourceField<8>(mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new SignedField<8>(_weight));
        internalField.Append(new SwitchField<8>(mix.swtch, board, version));
        internalField.Append(new UnsignedField<8>(mix.phases));
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
        internalField.Append(new UnsignedField<5>(mix.phases));
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
        _curveMode = mix.curve;
        _curveParam = (mix.curve ? mix.curve : smallGvarToEEPROM(mix.differential));
      }
      else {
        mix.clear();
        _destCh = 0;
        _curveMode = 0;
        _curveParam = 0;
      }

      if (IS_ARM(board)) {
        exportGvarParam(mix.weight, _weight);
        if (version >= 214)
          exportGvarParam(mix.sOffset, _offset);
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
      if (mix.srcRaw.type != SOURCE_TYPE_NONE) {
        mix.destCh = _destCh + 1;
        mix.curve = 0;
        mix.differential = 0;
        if (_curveMode)
          mix.curve = _curveParam;
        else
          mix.differential = smallGvarToC9x(_curveParam);
      }

      if (IS_ARM(board)) {
        importGvarParam(mix.weight, _weight);
        if (version >= 214)
          importGvarParam(mix.sOffset, _offset);
        else
          concatGvarParam(mix.sOffset, _offset, _offsetMode, board, version);
      }
      else {
        concatGvarParam(mix.weight, _weight, _weightMode, board, version);
        concatGvarParam(mix.sOffset, _offset, _offsetMode, board, version);
      }
    }

  protected:
    StructField internalField;
    MixData & mix;
    BoardEnum board;
    unsigned int version;
    unsigned int _destCh;
    bool _curveMode;
    int _curveParam;
    int _weight;
    int _offset;
    unsigned int _weightMode;
    unsigned int _offsetMode;
};

class ExpoField: public TransformedField {
  public:
    ExpoField(ExpoData & expo, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField("Expo"),
      expo(expo),
      board(board)
    {
      if (IS_ARM(board)) {
        internalField.Append(new UnsignedField<8>(expo.mode));
        internalField.Append(new UnsignedField<8>(expo.chn));
        internalField.Append(new SwitchField<8>(expo.swtch, board, version));
        internalField.Append(new UnsignedField<16>(expo.phases));
        internalField.Append(new SignedField<8>(_weight));
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
      else if (IS_DBLRAM(board, version) && IS_RELEASE_23_MARCH_2013(board, version)) {
        internalField.Append(new UnsignedField<2>(expo.mode));
        internalField.Append(new UnsignedField<2>(expo.chn));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new SpareBitsField<3>());
        internalField.Append(new UnsignedField<8>(expo.phases));
        internalField.Append(new SwitchField<8>(expo.swtch, board, version));
        internalField.Append(new SignedField<8>(_weight));
        internalField.Append(new SignedField<8>(_curveParam));
      }
      else {
        internalField.Append(new UnsignedField<2>(expo.mode));
        internalField.Append(new SwitchField<6>(expo.swtch, board, version));
        internalField.Append(new UnsignedField<2>(expo.chn));
        internalField.Append(new UnsignedField<5>(expo.phases));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new SignedField<8>(_weight));
        internalField.Append(new SignedField<8>(_curveParam));
      }
    }

    virtual void beforeExport()
    {
      _curveMode = (expo.curveMode && expo.curveParam);
      _weight    = smallGvarToEEPROM(expo.weight);
      _curveParam = smallGvarToEEPROM(expo.curveParam);
    }

    virtual void afterImport()
    {
      expo.curveMode  = _curveMode;
      expo.weight     = smallGvarToC9x(_weight);
      expo.curveParam = smallGvarToC9x(_curveParam);
    }

  protected:
    StructField internalField;
    ExpoData & expo;
    BoardEnum board;
    bool _curveMode;
    int  _weight;
    int  _curveParam;
};

class LimitField: public StructField {
  public:
    LimitField(LimitData & limit, BoardEnum board):
      StructField("Limit")
    {
      Append(new ConversionField< SignedField<8> >(limit.min, +100));
      Append(new ConversionField< SignedField<8> >(limit.max, -100));
      Append(new SignedField<8>(limit.ppmCenter));
      Append(new SignedField<14>(limit.offset));
      Append(new BoolField<1>(limit.symetrical));
      Append(new BoolField<1>(limit.revert));
      if (HAS_LARGE_LCD(board))
        Append(new ZCharField<6>(limit.name));
    }
};

class CurvesField: public TransformedField {
  public:
    CurvesField(CurveData * curves, BoardEnum board):
      TransformedField(internalField),
      internalField("Curves"),
      curves(curves),
      board(board),
      maxCurves(MAX_CURVES(board)),
      maxPoints(IS_ARM(board) ? O9X_ARM_NUM_POINTS : O9X_NUM_POINTS)
    {
      for (int i=0; i<maxCurves; i++) {
        if (IS_ARM(board))
          internalField.Append(new SignedField<16>(_curves[i]));
        else
          internalField.Append(new SignedField<8>(_curves[i]));
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
        offset += (curve->custom ? curve->count * 2 - 2 : curve->count) - 5;
        if (offset > maxPoints - 5 * maxCurves) {
          EEPROMWarnings += ::QObject::tr("openTx only accepts %1 points in all curves").arg(maxPoints) + "\n";
          break;
        }
        _curves[i] = offset;
        for (int j=0; j<curve->count; j++) {
          *cur++ = curve->points[j].y;
        }
        if (curve->custom) {
          for (int j=1; j<curve->count-1; j++) {
            *cur++ = curve->points[j].x;
          }
        }
      }
    }

    virtual void afterImport()
    {
      for (int i=0; i<maxCurves; i++) {
        CurveData *curve = &curves[i];
        int * cur = &_points[i==0 ? 0 : 5*i + _curves[i-1]];
        int * next = &_points[5*(i+1) + _curves[i]];
        int size = next - cur;
        if (size % 2 == 0) {
          curve->count = (size / 2) + 1;
          curve->custom = true;
        }
        else {
          curve->count = size;
          curve->custom = false;
        }
        for (int j=0; j<curve->count; j++)
          curve->points[j].y = cur[j];
        if (curve->custom) {
          curve->points[0].x = -100;
          for (int j=1; j<curve->count-1; j++)
            curve->points[j].x = cur[curve->count+j-1];
          curve->points[curve->count-1].x = +100;
        }
        else {
          for (int j=0; j<curve->count; j++)
            curve->points[j].x = -100 + (200*i) / (curve->count-1);
        }
      }
    }

  protected:
    StructField internalField;
    CurveData *curves;
    BoardEnum board;
    int maxCurves;
    int maxPoints;
    int _curves[O9X_ARM_MAX_CURVES];
    int _points[O9X_ARM_NUM_POINTS];
};

class CustomSwitchesFunctionsTable: public ConversionTable {

  public:
    CustomSwitchesFunctionsTable(BoardEnum board, unsigned int version)
    {
      int val=0;
      bool release21March2013 = IS_RELEASE_21_MARCH_2013(board, version);
      addConversion(CS_FN_OFF, val++);
      if (release21March2013)
        addConversion(CS_FN_VEQUAL, val++);
      addConversion(CS_FN_VPOS, val++);
      addConversion(CS_FN_VNEG, val++);
      addConversion(CS_FN_APOS, val++);
      addConversion(CS_FN_ANEG, val++);
      addConversion(CS_FN_AND, val++);
      addConversion(CS_FN_OR, val++);
      addConversion(CS_FN_XOR, val++);
      addConversion(CS_FN_EQUAL, val++);
      if (!release21March2013)
        addConversion(CS_FN_NEQUAL, val++);
      addConversion(CS_FN_GREATER, val++);
      addConversion(CS_FN_LESS, val++);
      if (!release21March2013) {
        addConversion(CS_FN_EGREATER, val++);
        addConversion(CS_FN_ELESS, val++);
      }
      addConversion(CS_FN_DPOS, val++);
      addConversion(CS_FN_DAPOS, val++);
      addConversion(CS_FN_TIM, val++);
    }
};

class CustomSwitchesAndSwitchesConversionTable: public ConversionTable {

  public:
    CustomSwitchesAndSwitchesConversionTable(BoardEnum board, unsigned int version)
    {
      int val=0;
      addConversion(RawSwitch(SWITCH_TYPE_NONE), val++);
      
      if (IS_TARANIS(board)) {
        for (int i=1; i<=MAX_SWITCHES_POSITION(board); i++) {
          int s = switchIndex(i, board, version);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, -s), -val);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val++);
        }
        for (int i=1; i<=MAX_CUSTOM_SWITCHES(board, version); i++) {
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, -i), -val);
          addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
        }
      }
      else if (board == BOARD_SKY9X) {
        for (int i=1; i<=8; i++) {
          int s = switchIndex(i, board, version);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, -s), -val);
          addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val++);
        }
        for (int i=1; i<=MAX_CUSTOM_SWITCHES(board, version); i++) {
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

  protected:

    void addConversion(const RawSwitch & sw, const int b)
    {
      ConversionTable::addConversion(sw.toValue(), b);
    }
};

class CustomSwitchField: public TransformedField {
  public:
    CustomSwitchField(CustomSwData & csw, BoardEnum board, unsigned int version, unsigned int variant):
      TransformedField(internalField),
      internalField("CustomSwitch"),
      csw(csw),
      board(board),
      version(version),
      variant(variant),
      functionsConversionTable(board, version),
      sourcesConversionTable(SourcesConversionTable::getInstance(board, version, variant, (version >= 214 || (!IS_ARM(board) && version >= 213)) ? 0 : FLAG_NOSWITCHES)),
      switchesConversionTable(SwitchesConversionTable::getInstance(board, version)),
      andswitchesConversionTable(board, version)
    {
      if (IS_ARM(board) && version >= 215) {
        internalField.Append(new SignedField<16>(v1));
        internalField.Append(new SignedField<16>(v2));
      }
      else {
        internalField.Append(new SignedField<8>(v1));
        internalField.Append(new SignedField<8>(v2));
      }

      if (IS_ARM(board)) {
        internalField.Append(new ConversionField< UnsignedField<8> >(csw.func, &functionsConversionTable, "Function"));
        internalField.Append(new UnsignedField<8>(csw.delay));
        internalField.Append(new UnsignedField<8>(csw.duration));
        if (version >= 214) {
          internalField.Append(new ConversionField< SignedField<8> >((int &)csw.andsw, &andswitchesConversionTable, "AND switch"));
        }
      }
      else {
        if (version >= 213) {
          internalField.Append(new ConversionField< UnsignedField<4> >(csw.func, &functionsConversionTable, "Function"));
          internalField.Append(new ConversionField< UnsignedField<4> >(csw.andsw, &andswitchesConversionTable, "AND switch"));
        }
        else {
          internalField.Append(new ConversionField< UnsignedField<8> >(csw.func, &functionsConversionTable, "Function"));
        }
      }
    }

    virtual void beforeExport()
    {
      v1 = csw.val1;
      v2 = csw.val2;

      if ((csw.func >= CS_FN_VPOS && csw.func <= CS_FN_ANEG) || (csw.func >= CS_FN_EQUAL && csw.func!=CS_FN_TIM)) {
        sourcesConversionTable->exportValue(csw.val1, v1);
      }

      if (csw.func >= CS_FN_EQUAL && csw.func <= CS_FN_ELESS) {
        sourcesConversionTable->exportValue(csw.val2, v2);
      }

      if (csw.func >= CS_FN_AND && csw.func <= CS_FN_XOR) {
        switchesConversionTable->exportValue(csw.val1, v1);
        switchesConversionTable->exportValue(csw.val2, v2);
      }
    }

    virtual void afterImport()
    {
      csw.val1 = v1;
      csw.val2 = v2;

      if ((csw.func >= CS_FN_VPOS && csw.func <= CS_FN_ANEG) || (csw.func >= CS_FN_EQUAL && csw.func!=CS_FN_TIM)) {
        sourcesConversionTable->importValue(v1, csw.val1);
      }

      if (csw.func >= CS_FN_EQUAL && csw.func <= CS_FN_ELESS) {
        sourcesConversionTable->importValue(v2, csw.val2);
      }

      if (csw.func >= CS_FN_AND && csw.func <= CS_FN_XOR) {
        switchesConversionTable->importValue(v1, csw.val1);
        switchesConversionTable->importValue(v2, csw.val2);
      }
    }

  protected:
    StructField internalField;
    CustomSwData & csw;
    BoardEnum board;
    unsigned int version;
    unsigned int variant;
    CustomSwitchesFunctionsTable functionsConversionTable;
    SourcesConversionTable * sourcesConversionTable;
    SwitchesConversionTable * switchesConversionTable;
    CustomSwitchesAndSwitchesConversionTable andswitchesConversionTable;
    int v1;
    int v2;
};

class CustomFunctionsConversionTable: public ConversionTable {

  public:
    CustomFunctionsConversionTable(BoardEnum board, unsigned int version)
    {
      int val=0;

      if (IS_ARM(board) || version < 213) {
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
      addConversion(FuncTrainer, val++);
      addConversion(FuncTrainerRUD, val++);
      addConversion(FuncTrainerELE, val++);
      addConversion(FuncTrainerTHR, val++);
      addConversion(FuncTrainerAIL, val++);
      addConversion(FuncInstantTrim, val++);
      addConversion(FuncPlaySound, val++);
      if (!IS_TARANIS(board))
        addConversion(FuncPlayHaptic, val++);
      addConversion(FuncReset, val++);
      addConversion(FuncVario, val++);
      addConversion(FuncPlayPrompt, val++);
      if (version >= 213 && !IS_ARM(board))
        addConversion(FuncPlayBoth, val++);
      addConversion(FuncPlayValue, val++);
      if (board == BOARD_GRUVIN9X || IS_ARM(board) )
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
};

template <int N>
class SwitchesWarningField: public TransformedField {
  public:
    SwitchesWarningField(unsigned int & sw, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField(_sw, "SwitchesWarning"),
      sw(sw),
      board(board),
      version(version)
    {
    }

    virtual void beforeExport()
    {
      bool release21March2013 = IS_RELEASE_21_MARCH_2013(board, version);
      if (release21March2013) {
        _sw = (sw & 0xC1) + ((sw & 0x30) >> 3) + ((sw & 0x0E) << 2);
      }
      else {
        _sw = sw;
      }
    }

    virtual void afterImport()
    {
      bool release21March2013 = IS_RELEASE_21_MARCH_2013(board, version);
      if (release21March2013) {
        sw = (_sw & 0xC1) + ((_sw & 0x38) >> 2) + ((_sw & 0x06) << 3);
      }
      else {
        sw = _sw;
      }
    }

  protected:
    UnsignedField<N> internalField;
    unsigned int &sw;
    unsigned int _sw;
    BoardEnum board;
    unsigned int version;
};

class CustomFunctionField: public TransformedField {
  public:
    CustomFunctionField(FuncSwData & fn, BoardEnum board, unsigned int version, unsigned int variant):
      TransformedField(internalField),
      internalField("CustomFunction"),
      fn(fn),
      board(board),
      version(version),
      variant(variant),
      functionsConversionTable(board, version),
      sourcesConversionTable(SourcesConversionTable::getInstance(board, version, variant, FLAG_NONONE)),
      _param(0),
      _delay(0),
      _union_param(0)
    {
      memset(_arm_param, 0, sizeof(_arm_param));

      internalField.Append(new SwitchField<8>(fn.swtch, board, version));
      if (IS_ARM(board)) {
        internalField.Append(new ConversionField< UnsignedField<8> >((unsigned int &)fn.func, &functionsConversionTable, "Function", ::QObject::tr("OpenTX on this board doesn't accept this function")));
        if (IS_TARANIS(board))
          internalField.Append(new CharField<10>(_arm_param));
        else
          internalField.Append(new CharField<6>(_arm_param));
        if (version >= 214) {
          internalField.Append(new UnsignedField<2>(_mode));
          internalField.Append(new UnsignedField<6>(_delay));
        }
        else {
          internalField.Append(new UnsignedField<8>(_delay));
        }
        if (version < 214)
          internalField.Append(new SpareBitsField<8>());
      }
      else {
        if (version >= 213) {
          internalField.Append(new UnsignedField<3>(_union_param));
          internalField.Append(new ConversionField< UnsignedField<5> >((unsigned int &)fn.func, &functionsConversionTable, "Function", ::QObject::tr("OpenTX on this board doesn't accept this function")));
        }
        else {
          internalField.Append(new ConversionField< UnsignedField<7> >((unsigned int &)fn.func, &functionsConversionTable, "Function", ::QObject::tr("OpenTX on this board doesn't accept this function")));
          internalField.Append(new BoolField<1>((bool &)fn.enabled));
        }
        internalField.Append(new UnsignedField<8>(_param));
      }
    }

    virtual void beforeExport()
    {
      if (IS_ARM(board)) {
        _mode = 0;
        if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayValue)
          _delay = fn.repeatParam / 5;
        else
          _delay = (fn.enabled ? 1 : 0);
        if (fn.func <= FuncInstantTrim) {
          *((uint32_t *)_arm_param) = fn.param;
        }
        else if (fn.func == FuncPlayPrompt || fn.func == FuncBackgroundMusic) {
          memcpy(_arm_param, fn.paramarm, sizeof(_arm_param));
        }
        else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGV5) {
          unsigned int value;
          if (version >= 214) {
            _mode = fn.adjustMode;
            if (fn.adjustMode == 1)
              sourcesConversionTable->exportValue(fn.param, (int &)value);
            else if (fn.adjustMode == 2)
              value = RawSource(fn.param).index;
            else
              value = fn.param;
          }
          else {
            unsigned int value;
            sourcesConversionTable->exportValue(fn.param, (int &)value);
          }
          *((uint32_t *)_arm_param) = value;
        }
        else if (fn.func == FuncPlayValue || fn.func == FuncVolume) {
          unsigned int value;
          sourcesConversionTable->exportValue(fn.param, (int &)value);
          *((uint32_t *)_arm_param) = value;
        }
        else {
          *((uint32_t *)_arm_param) = fn.param;
        }
      }
      else {
        /* the default behavior */
        _param = fn.param;
        _union_param = (fn.enabled ? 1 : 0);
        if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGV5) {
          if (version >= 213) {
            _union_param += (fn.adjustMode << 1);
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
          if (version >= 213) {
            _union_param = fn.repeatParam / 10;
            sourcesConversionTable->exportValue(fn.param, (int &)_param);
          }
          else {
            SourcesConversionTable::getInstance(board, version, variant, FLAG_NONONE|FLAG_NOSWITCHES)->exportValue(fn.param, (int &)_param);
          }
        }
        else if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayBoth) {
          if (version >= 213)
            _union_param = fn.repeatParam / 10;
        }
        else if (fn.func <= FuncSafetyCh16) {
          if (version >= 213)
            _union_param += ((fn.func % 4) << 1);
        }
      }
    }

    virtual void afterImport()
    {
      if (IS_ARM(board)) {
        if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayValue)
          fn.repeatParam = _delay * 5;
        else
          fn.enabled = (_delay & 0x01);

        unsigned int value = *((uint32_t *)_arm_param);
        if (fn.func <= FuncTrainer) {
            fn.param = (int)value;
        }
        else if (fn.func <= FuncInstantTrim) {
          fn.param = value;
        }
        else if (fn.func == FuncPlayPrompt || fn.func == FuncBackgroundMusic) {
          memcpy(fn.paramarm, _arm_param, sizeof(fn.paramarm));
        }
        else if (fn.func == FuncVolume) {
          sourcesConversionTable->importValue(value, (int &)fn.param);
        }
        else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGV5) {
          if (version >= 214) {
            fn.adjustMode = _mode;
            if (fn.adjustMode == 1)
              sourcesConversionTable->importValue(value, (int &)fn.param);
            else if (fn.adjustMode == 2)
              fn.param = RawSource(SOURCE_TYPE_GVAR, value).toValue();
            else
              fn.param = value;
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
        else {
          fn.param = value;
        }
      }
      else {
        fn.param = _param;
        if (version >= 213) {
          fn.enabled = (_union_param & 0x01);
        }
        if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGV5) {
          if (version >= 213) {
            fn.adjustMode = ((_union_param >> 1) & 0x03);
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
          if (version >= 213) {
            fn.repeatParam = _union_param * 10;
            sourcesConversionTable->importValue(_param, (int &)fn.param);
          }
          else {
            SourcesConversionTable::getInstance(board, version, variant, FLAG_NONONE|FLAG_NOSWITCHES)->importValue(_param, (int &)fn.param);
          }
        }
        else if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayBoth) {
          if (version >= 213)
            fn.repeatParam = _union_param * 10;
        }
        else if (fn.func <= FuncSafetyCh16) {
          if (version >= 213) {
            fn.func = AssignFunc(((fn.func >> 2) << 2) + ((_union_param >> 1) & 0x03));
          }
          fn.param = (int8_t)fn.param;
        }
      }
    }

  protected:
    StructField internalField;
    FuncSwData & fn;
    BoardEnum board;
    unsigned int version;
    unsigned int variant;
    CustomFunctionsConversionTable functionsConversionTable;
    SourcesConversionTable * sourcesConversionTable;
    char _arm_param[10];
    unsigned int _param;
    unsigned int _delay;
    unsigned int _mode;
    unsigned int _union_param;
};

class FrskyScreenField: public DataField {
  public:
    FrskyScreenField(FrSkyScreenData & screen, BoardEnum board, unsigned int version):
      DataField("Frsky Screen"),
      screen(screen),
      board(board),
      version(version)
    {
      for (int i=0; i<4; i++) {
        bars.Append(new UnsignedField<8>(_screen.body.bars[i].source));
        bars.Append(new UnsignedField<8>(_screen.body.bars[i].barMin));
        bars.Append(new UnsignedField<8>(_screen.body.bars[i].barMax));
      }

      int columns=(IS_TARANIS(board) ? 3:2);
      for (int i=0; i<4; i++) {
        for (int j=0; j<columns; j++) {
          numbers.Append(new UnsignedField<8>(_screen.body.lines[i].source[j]));
        }
      }
      if (!IS_TARANIS(board)) {
        for (int i=0; i<4; i++) {
          numbers.Append(new SpareBitsField<8>());
        }
      }
    }

    virtual void ExportBits(QBitArray & output)
    {
      _screen = screen;

      bool release21March2013 = IS_RELEASE_21_MARCH_2013(board, version);
      if (!release21March2013) {
        for (int i=0; i<4; i++) {
          if (_screen.body.bars[i].source > 0)
            _screen.body.bars[i].source--;
        }
        int columns=(IS_TARANIS(board) ? 3:2);
        for (int i=0; i<4; i++) {
          for (int j=0; j<columns;j++) {
            if (_screen.body.lines[i].source[j] > 0)
              _screen.body.lines[i].source[j]--;
          }
        }
      }

      if (screen.type == 0)
        numbers.ExportBits(output);
      else
        bars.ExportBits(output);
    }

    virtual void ImportBits(QBitArray & input)
    {
      _screen = screen;

      bool release21March2013 = IS_RELEASE_21_MARCH_2013(board, version);

      // NOTA: screen.type should have been imported first!
      if (screen.type == 0) {
        numbers.ImportBits(input);
        if (!release21March2013) {
          int columns=(IS_TARANIS(board) ? 3:2);
          for (int i=0; i<4; i++) {
            for (int j=0; j<columns;j++) {
              if (_screen.body.lines[i].source[j] > 0)
                _screen.body.lines[i].source[j]++;
            }
          }
        }
      }
      else {
        bars.ImportBits(input);
        if (!release21March2013) {
          for (int i=0; i<4; i++) {
            if (_screen.body.bars[i].source > 0)
              _screen.body.bars[i].source++;
          }
        }
      }

      screen = _screen;
    }

    virtual unsigned int size()
    {
      // NOTA: screen.type should have been imported first!
      if (screen.type == 0)
        return numbers.size();
      else
        return bars.size();
    }

  protected:
    FrSkyScreenData & screen;
    FrSkyScreenData _screen;
    BoardEnum board;
    unsigned int version;
    StructField bars;
    StructField numbers;
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

class VarioConversionTable: public ConversionTable
{
  public:
    VarioConversionTable()
    {
      addConversion(2, 0); // Vario
      addConversion(3, 1); // A1
      addConversion(4, 2); // A2
    }
};

class FrskyField: public StructField {
  public:
    FrskyField(FrSkyData & frsky, BoardEnum board, unsigned int version):
      StructField("FrSky")
    {
      rssiConversionTable[0] = RSSIConversionTable(0);
      rssiConversionTable[1] = RSSIConversionTable(1);

      if (IS_ARM(board)) {
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
          Append(new SpareBitsField<2>());
          Append(new UnsignedField<8>(frsky.channels[i].multiplier, 0, 5, "Multiplier"));
        }
        Append(new UnsignedField<8>(frsky.usrProto));
        Append(new UnsignedField<8>(frsky.voltsSource));
        Append(new UnsignedField<8>(frsky.blades));
        Append(new UnsignedField<8>(frsky.currentSource));

        Append(new UnsignedField<1>(frsky.screens[0].type));
        Append(new UnsignedField<1>(frsky.screens[1].type));
        Append(new UnsignedField<1>(frsky.screens[2].type));
        Append(new SpareBitsField<5>());

        for (int i=0; i<3; i++) {
          Append(new FrskyScreenField(frsky.screens[i], board, version));
        }
        if (IS_TARANIS(board))
          Append(new ConversionField< UnsignedField<8> >(frsky.varioSource, &varioConversionTable, "Vario Source"));
        else
          Append(new UnsignedField<8>(frsky.varioSource));
        Append(new SignedField<8>(frsky.varioCenterMax));
        Append(new SignedField<8>(frsky.varioCenterMin));
        Append(new SignedField<8>(frsky.varioMin));
        Append(new SignedField<8>(frsky.varioMax));
        for (int i=0; i<2; i++) {
          Append(new ConversionField< UnsignedField<2> >(frsky.rssiAlarms[i].level, &rssiConversionTable[i], "RSSI"));
          Append(new ConversionField< SignedField<6> >(frsky.rssiAlarms[i].value, -45+i*3));
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
        Append(new UnsignedField<2>(frsky.usrProto));
        Append(new UnsignedField<2>(frsky.blades));
        Append(new UnsignedField<1>(frsky.screens[0].type));
        Append(new UnsignedField<1>(frsky.screens[1].type));
        Append(new UnsignedField<2>(frsky.voltsSource));
        Append(new SignedField<4>(frsky.varioMin));
        Append(new SignedField<4>(frsky.varioMax));
        for (int i=0; i<2; i++) {
          Append(new ConversionField< UnsignedField<2> >(frsky.rssiAlarms[i].level, &rssiConversionTable[i], "RSSI level"));
          Append(new ConversionField< SignedField<6> >(frsky.rssiAlarms[i].value, -45+i*3, 0, 100, "RSSI value"));
        }
        for (int i=0; i<2; i++) {
          Append(new FrskyScreenField(frsky.screens[i], board, version));
        }
        Append(new UnsignedField<3>(frsky.varioSource));
        Append(new SignedField<5>(frsky.varioCenterMin));
        Append(new UnsignedField<3>(frsky.currentSource));
        Append(new SignedField<8>(frsky.varioCenterMax));
      }
    }

  protected:
    RSSIConversionTable rssiConversionTable[2];
    VarioConversionTable varioConversionTable;
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

int exportPpmDelay(int delay) { return (delay - 300) / 50; }
int importPpmDelay(int delay) { return 300 + 50 * delay; }

Open9xModelDataNew::Open9xModelDataNew(ModelData & modelData, BoardEnum board, unsigned int version, unsigned int variant):
  TransformedField(internalField),
  internalField("ModelData"),
  modelData(modelData),
  board(board),
  variant(variant),
  protocolsConversionTable(board)
{
  sprintf(name, "Model %s", modelData.name);

  if (HAS_LARGE_LCD(board))
    internalField.Append(new ZCharField<12>(modelData.name));
  else
    internalField.Append(new ZCharField<10>(modelData.name));

  bool release21March2013 = IS_RELEASE_21_MARCH_2013(board, version);

  if (release21March2013)
    internalField.Append(new UnsignedField<8>(modelData.modelId));

  if (IS_TARANIS(board) && version >= 215) {
    internalField.Append(new CharField<10>(modelData.bitmap));
  }

  for (int i=0; i<O9X_MAX_TIMERS; i++) {
    internalField.Append(new TimerModeField(modelData.timers[i].mode, board, version));
    if (release21March2013) {
      internalField.Append(new UnsignedField<12>(modelData.timers[i].val));

      internalField.Append(new BoolField<1>(modelData.timers[i].countdownBeep));
      internalField.Append(new BoolField<1>(modelData.timers[i].minuteBeep));
      if (HAS_PERSISTENT_TIMERS(board)) {
        internalField.Append(new BoolField<1>(modelData.timers[i].persistent));
        internalField.Append(new SpareBitsField<1>());
        internalField.Append(new SignedField<16>(modelData.timers[i].pvalue));
      }
      else {
        internalField.Append(new SpareBitsField<2>());
      }
    }
    else {
      internalField.Append(new UnsignedField<16>(modelData.timers[i].val));
      if (HAS_PERSISTENT_TIMERS(board)) {
        internalField.Append(new BoolField<1>(modelData.timers[i].persistent));
        internalField.Append(new SpareBitsField<15>());
      }
    }
  }

  if (IS_TARANIS(board))
    internalField.Append(new SpareBitsField<3>());
  else
    internalField.Append(new ConversionField< SignedField<3> >(modelData.moduleData[0].protocol, &protocolsConversionTable, "Protocol", ::QObject::tr("OpenTX doesn't accept this protocol")));

  internalField.Append(new BoolField<1>(modelData.thrTrim));

  if (IS_TARANIS(board))
    internalField.Append(new SpareBitsField<4>());
  else
    internalField.Append(new ConversionField< SignedField<4> >(modelData.moduleData[0].channelsCount, &channelsConversionTable, "Channels number", ::QObject::tr("OpenTX doesn't allow this number of channels")));

  internalField.Append(new UnsignedField<3>(modelData.trimInc));
  internalField.Append(new BoolField<1>(modelData.disableThrottleWarning));

  if (IS_TARANIS(board))
    internalField.Append(new SpareBitsField<1>());
  else
    internalField.Append(new BoolField<1>(modelData.moduleData[0].ppmPulsePol));

  internalField.Append(new BoolField<1>(modelData.extendedLimits));
  internalField.Append(new BoolField<1>(modelData.extendedTrims));
  internalField.Append(new BoolField<1>(modelData.throttleReversed));
  internalField.Append(new ConversionField< SignedField<8> >(modelData.moduleData[0].ppmDelay, exportPpmDelay, importPpmDelay));

  if (IS_ARM(board) || board==BOARD_GRUVIN9X)
    internalField.Append(new UnsignedField<16>(modelData.beepANACenter));
  else
    internalField.Append(new UnsignedField<8>(modelData.beepANACenter));

  for (int i=0; i<MAX_MIXERS(board, version); i++)
    internalField.Append(new MixField(modelData.mixData[i], board, version));
  for (int i=0; i<MAX_CHANNELS(board, version); i++)
    internalField.Append(new LimitField(modelData.limitData[i], board));
  for (int i=0; i<MAX_EXPOS(board, version); i++)
    internalField.Append(new ExpoField(modelData.expoData[i], board, version));
  internalField.Append(new CurvesField(modelData.curves, board));
  for (int i=0; i<MAX_CUSTOM_SWITCHES(board, version); i++)
    internalField.Append(new CustomSwitchField(modelData.customSw[i], board, version, variant));
  for (int i=0; i<MAX_CUSTOM_FUNCTIONS(board, version); i++)
    internalField.Append(new CustomFunctionField(modelData.funcSw[i], board, version, variant));
  internalField.Append(new HeliField(modelData.swashRingData, board, version, variant));
  for (int i=0; i<MAX_PHASES(board, version); i++)
    internalField.Append(new PhaseField(modelData.phaseData[i], i, board, version));
  internalField.Append(new SignedField<8>(modelData.moduleData[0].ppmFrameLength));
  internalField.Append(new UnsignedField<8>(modelData.thrTraceSrc));

  if (!release21March2013)
    internalField.Append(new UnsignedField<8>(modelData.modelId));

  if (IS_TARANIS(board))
    internalField.Append(new UnsignedField<16>(modelData.switchWarningStates));
  else
    internalField.Append(new SwitchesWarningField<8>(modelData.switchWarningStates, board, version));

  if ((board == BOARD_STOCK || (board == BOARD_M128 && version >= 215)) && (variant & GVARS_VARIANT)) {
    for (int i=0; i<O9X_MAX_GVARS; i++) {
      // on M64 GVARS are common to all phases, and there is no name
      internalField.Append(new SignedField<16>(modelData.phaseData[0].gvars[i]));
    }
  }

  if (board != BOARD_STOCK && (board != BOARD_M128 || version < 215)) {
    for (int i=0; i<O9X_MAX_GVARS; i++) {
      internalField.Append(new ZCharField<6>(modelData.gvars_names[i]));
    }
  }

  if ((board != BOARD_STOCK && (board != BOARD_M128 || version < 215)) || (variant & FRSKY_VARIANT)) {
    internalField.Append(new FrskyField(modelData.frsky, board, version));
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
    internalField.Append(new ConversionField< SignedField<8> >(modelData.moduleData[1].protocol, &protocolsConversionTable, "Protocol", ::QObject::tr("OpenTX doesn't accept this protocol")));
    internalField.Append(new UnsignedField<8>(modelData.trainerMode));
  }

  if (IS_ARM(board) && version >= 215) {
    for (int module=0; module<modulesCount; module++) {
      internalField.Append(new SignedField<8>(subprotocols[module]));
      internalField.Append(new UnsignedField<8>(modelData.moduleData[module].channelsStart));
      internalField.Append(new ConversionField< SignedField<8> >(modelData.moduleData[module].channelsCount, -8));
      internalField.Append(new UnsignedField<8>(modelData.moduleData[module].failsafeMode));
      for (int i=0; i<32; i++)
        internalField.Append(new SignedField<16>(modelData.moduleData[module].failsafeChannels[i]));
      internalField.Append(new ConversionField< SignedField<8> >(modelData.moduleData[module].ppmDelay, exportPpmDelay, importPpmDelay));
      internalField.Append(new SignedField<8>(modelData.moduleData[module].ppmFrameLength));
      internalField.Append(new BoolField<8>(modelData.moduleData[module].ppmPulsePol));
    }
  }

  if (IS_TARANIS(board)) {
    for (int i=0; i<MAX_CURVES(board); i++) {
      internalField.Append(new ZCharField<6>(modelData.curves[i].name));
    }
  }
}

void Open9xModelDataNew::beforeExport()
{
  // qDebug() << QString("before export model") << modelData.name;
  for (int module=0; module<3; module++) {
    if (modelData.moduleData[module].protocol >= PXX_XJT_X16 && modelData.moduleData[module].protocol <= PXX_XJT_LR12)
      subprotocols[module] = modelData.moduleData[module].protocol - PXX_XJT_X16;
    else
      subprotocols[module] = (module==0 ? -1:0);
  }
}

void Open9xModelDataNew::afterImport()
{
  // qDebug() << QString("after import model") << modelData.name;

  for (int module=0; module<3; module++) {
    if (modelData.moduleData[module].protocol == PXX_XJT_X16) {
      if (subprotocols[module]>=0)
        modelData.moduleData[module].protocol = PXX_XJT_X16 + subprotocols[module];
      else
        modelData.moduleData[module].protocol = OFF;
    }
  }
}

Open9xGeneralDataNew::Open9xGeneralDataNew(GeneralSettings & generalData, BoardEnum board, unsigned int version, unsigned int variant):
  TransformedField(internalField),
  internalField("General Settings"),
  generalData(generalData),
  board(board),
  inputsCount(IS_TARANIS(board) ? 8 : 7)
{
  generalData.version = version;
  generalData.variant = variant;

  internalField.Append(new UnsignedField<8>(generalData.version));
  if (version >= 213 || (!IS_ARM(board) && version >= 212))
    internalField.Append(new UnsignedField<16>(generalData.variant));

  for (int i=0; i<inputsCount; i++)
    internalField.Append(new SignedField<16>(generalData.calibMid[i]));
  for (int i=0; i<inputsCount; i++)
    internalField.Append(new SignedField<16>(generalData.calibSpanNeg[i]));
  for (int i=0; i<inputsCount; i++)
    internalField.Append(new SignedField<16>(generalData.calibSpanPos[i]));

  internalField.Append(new UnsignedField<16>(chkSum));
  internalField.Append(new UnsignedField<8>(generalData.currModel));
  internalField.Append(new UnsignedField<8>(generalData.contrast));
  internalField.Append(new UnsignedField<8>(generalData.vBatWarn));
  internalField.Append(new SignedField<8>(generalData.vBatCalib));
  internalField.Append(new SignedField<8>(generalData.backlightMode));

  for (int i=0; i<NUM_STICKS; i++)
    internalField.Append(new SignedField<16>(generalData.trainer.calib[i]));
  for (int i=0; i<NUM_STICKS; i++) {
    internalField.Append(new UnsignedField<6>(generalData.trainer.mix[i].src));
    internalField.Append(new UnsignedField<2>(generalData.trainer.mix[i].mode));
    internalField.Append(new SignedField<8>(generalData.trainer.mix[i].weight));
  }

  internalField.Append(new UnsignedField<8>(generalData.view, 0, MAX_VIEWS(board)-1));

  internalField.Append(new SpareBitsField<2>());
  internalField.Append(new BoolField<1>(generalData.fai));
  internalField.Append(new SignedField<2>((int &)generalData.beeperMode));
  internalField.Append(new BoolField<1>(generalData.flashBeep));
  internalField.Append(new BoolField<1>(generalData.disableMemoryWarning));
  internalField.Append(new BoolField<1>(generalData.disableAlarmWarning));

  internalField.Append(new UnsignedField<2>(generalData.stickMode));
  internalField.Append(new SignedField<5>(generalData.timezone));
  internalField.Append(new SpareBitsField<1>());

  internalField.Append(new UnsignedField<8>(generalData.inactivityTimer));
  if (IS_STOCK(board) && version >= 215) {
    internalField.Append(new UnsignedField<3>(generalData.mavbaud));
  } else {
    internalField.Append(new BoolField<1>(generalData.throttleReversed));
    internalField.Append(new BoolField<1>(generalData.minuteBeep));
    internalField.Append(new BoolField<1>(generalData.preBeep));
  }
  if (version >= 213 || (!IS_ARM(board) && version >= 212))
    internalField.Append(new UnsignedField<3>(generalData.splashMode)); // TODO
  else
    internalField.Append(new SpareBitsField<3>());
  internalField.Append(new SignedField<2>((int &)generalData.hapticMode));

  internalField.Append(new SpareBitsField<8>());
  internalField.Append(new UnsignedField<8>(generalData.backlightDelay));
  internalField.Append(new UnsignedField<8>(generalData.templateSetup));
  internalField.Append(new SignedField<8>(generalData.PPM_Multiplier));
  internalField.Append(new SignedField<8>(generalData.hapticLength));
  internalField.Append(new UnsignedField<8>(generalData.reNavigation));

  internalField.Append(new SignedField<3>(generalData.beeperLength));
  internalField.Append(new UnsignedField<3>(generalData.hapticStrength));
  internalField.Append(new UnsignedField<1>(generalData.gpsFormat));
  internalField.Append(new SpareBitsField<1>()); // unexpectedShutdown

  internalField.Append(new UnsignedField<8>(generalData.speakerPitch));

  if (IS_ARM(board))
    internalField.Append(new ConversionField< SignedField<8> >(generalData.speakerVolume, -12, 0, 23, "Volume"));
  else
    internalField.Append(new ConversionField< SignedField<8> >(generalData.speakerVolume, -7, 0, 7, "Volume"));

  if (version >= 214 || (!IS_ARM(board) && version >= 213)) {
    internalField.Append(new SignedField<8>(generalData.vBatMin));
    internalField.Append(new SignedField<8>(generalData.vBatMax));
  }

  if (IS_ARM(board)) {
    internalField.Append(new UnsignedField<8>(generalData.backlightBright));
    internalField.Append(new SignedField<8>(generalData.currentCalib));
    if (version >= 213) {
      internalField.Append(new SignedField<8>(generalData.temperatureWarn)); // TODO
      internalField.Append(new UnsignedField<8>(generalData.mAhWarn));
      internalField.Append(new SpareBitsField<16>()); // mAhUsed
      internalField.Append(new SpareBitsField<32>()); // globalTimer
      internalField.Append(new SignedField<8>(generalData.temperatureCalib)); // TODO
      internalField.Append(new UnsignedField<8>(generalData.btBaudrate)); // TODO
      internalField.Append(new BoolField<8>(generalData.optrexDisplay)); //TODO
      internalField.Append(new UnsignedField<8>(generalData.sticksGain)); // TODO
    }
    if (version >= 214) {
      internalField.Append(new UnsignedField<8>(generalData.rotarySteps)); // TODO
      internalField.Append(new UnsignedField<8>(generalData.countryCode));
      internalField.Append(new UnsignedField<8>(generalData.imperial));
    }
    if (version >= 215) {
      internalField.Append(new CharField<2>(generalData.ttsLanguage));
      internalField.Append(new SignedField<8>(generalData.beepVolume));
      internalField.Append(new SignedField<8>(generalData.wavVolume));
      internalField.Append(new SignedField<8>(generalData.varioVolume));
      internalField.Append(new SignedField<8>(generalData.backgroundVolume));
    }
  }
}

void Open9xGeneralDataNew::beforeExport()
{
  uint16_t sum = 0;
  for (int i=0; i<inputsCount; i++)
    sum += generalData.calibMid[i];
  for (int i=0; i<5; i++)
    sum += generalData.calibSpanNeg[i];
  chkSum = sum;
}

void Open9xGeneralDataNew::afterImport()
{
}
