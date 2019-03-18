#include "definitions.h"
#include "board.h"

#define NUM_STICKS                     4

#define XPOTS_MULTIPOS_COUNT 6

#define MAX_SCRIPT_INPUTS  10
#define MAX_SCRIPT_OUTPUTS 6
#define MAX_GVARS 9

#if defined(PCBHORUS)
  #define MAX_MODELS                   60
  #define MAX_OUTPUT_CHANNELS          32 // number of real output channels CH1-CH32
  #define MAX_FLIGHT_MODES             9
  #define MAX_MIXERS                   64
  #define MAX_EXPOS                    64
  #define MAX_LOGICAL_SWITCHES         64
  #define MAX_SPECIAL_FUNCTIONS        64 // number of functions assigned to switches
  #define MAX_SCRIPTS                  9
  #define MAX_INPUTS                   32
  #define MAX_TRAINER_CHANNELS         16
  #define MAX_TELEMETRY_SENSORS        32
  #define MAX_CUSTOM_SCREENS           5
#elif defined(PCBTARANIS)
  #define MAX_MODELS                   60
  #define MAX_OUTPUT_CHANNELS          32 // number of real output channels CH1-CH32
  #define MAX_FLIGHT_MODES             9
  #define MAX_MIXERS                   64
  #define MAX_EXPOS                    64
  #define MAX_LOGICAL_SWITCHES         64
  #define MAX_SPECIAL_FUNCTIONS        64 // number of functions assigned to switches
  #define MAX_SCRIPTS                  7
  #define MAX_INPUTS                   32
  #define MAX_TRAINER_CHANNELS         16
  #define MAX_TELEMETRY_SENSORS        32
#else
  #define MAX_MODELS                   60
  #define MAX_OUTPUT_CHANNELS          32 // number of real output channels CH1-CH32
  #define MAX_FLIGHT_MODES             9
  #define MAX_MIXERS                   64
  #define MAX_EXPOS                    32
  #define MAX_LOGICAL_SWITCHES         64
  #define MAX_SPECIAL_FUNCTIONS        64 // number of functions assigned to switches
  #define MAX_INPUTS                   32
  #define MAX_TRAINER_CHANNELS         16
  #define MAX_TELEMETRY_SENSORS        32
#endif


#if defined(PCBHORUS)
  #define LEN_SWITCH_NAME              3
  #define LEN_ANA_NAME                 3
  #define LEN_MODEL_FILENAME           16
  #define LEN_BLUETOOTH_NAME           10
#else
  #define LEN_SWITCH_NAME              3
  #define LEN_ANA_NAME                 3
  #define LEN_BLUETOOTH_NAME           10
#endif

#if defined(PCBHORUS)
  #define LEN_MODEL_NAME               15
  #define LEN_TIMER_NAME               8
  #define LEN_FLIGHT_MODE_NAME         10
  #define LEN_BITMAP_NAME              10  // TODO next EEPROM change: we need 14 here as we have to store the file extension
  #define LEN_EXPOMIX_NAME             6
  #define LEN_CHANNEL_NAME             6
  #define LEN_INPUT_NAME               4
  #define LEN_CURVE_NAME               3
  #define LEN_FUNCTION_NAME            6
  #define MAX_CURVES                   32
  #define MAX_CURVE_POINTS             512
#elif LCD_W == 212
  #define LEN_MODEL_NAME               12
  #define LEN_TIMER_NAME               8
  #define LEN_FLIGHT_MODE_NAME         10
  #define LEN_BITMAP_NAME              10
  #define LEN_EXPOMIX_NAME             8   // TODO next EEPROM change: 6 seem enough
  #define LEN_CHANNEL_NAME             6
  #define LEN_INPUT_NAME               4
  #define LEN_CURVE_NAME               3
  #define LEN_FUNCTION_NAME            8
  #define MAX_CURVES                   32
  #define MAX_CURVE_POINTS             512
#else
  #define LEN_MODEL_NAME               10
  #define LEN_TIMER_NAME               3
  #define LEN_FLIGHT_MODE_NAME         6
  #define LEN_EXPOMIX_NAME             6
  #define LEN_CHANNEL_NAME             4
  #define LEN_INPUT_NAME               3
  #define LEN_CURVE_NAME               3
  #define LEN_FUNCTION_NAME            6
  #define MAX_CURVES                   16   // TODO next EEPROM check if can be changed to 32 to have all ARM the same
  #define MAX_CURVE_POINTS             512
#endif

enum TelemetrySource {
    TELEM_NONE,
    TELEM_TX_VOLTAGE,
    TELEM_TX_TIME,
    TELEM_RESERVE1,
    TELEM_RESERVE2,
    TELEM_RESERVE3,
    TELEM_RESERVE4,
    TELEM_RESERVE5,
    TELEM_TIMER1,
    TELEM_TIMER2,
    TELEM_SWR,
    TELEM_RSSI_TX,
    TELEM_RSSI_RX,
    TELEM_RESERVE0,
    TELEM_A_FIRST,
    TELEM_A1=TELEM_A_FIRST,
    TELEM_A2,
    TELEM_A3,
    TELEM_A4,
    TELEM_A_LAST=TELEM_A4,
    TELEM_ALT,
    TELEM_RPM,
    TELEM_FUEL,
    TELEM_T1,
    TELEM_T2,
    TELEM_SPEED,
    TELEM_DIST,
    TELEM_GPSALT,
    TELEM_CELL,
    TELEM_CELLS_SUM,
    TELEM_VFAS,
    TELEM_CURRENT,
    TELEM_CONSUMPTION,
    TELEM_POWER,
    TELEM_ACCx,
    TELEM_ACCy,
    TELEM_ACCz,
    TELEM_HDG,
    TELEM_VSPEED,
    TELEM_ASPEED,
    TELEM_DTE,
    TELEM_RESERVE6,
    TELEM_RESERVE7,
    TELEM_RESERVE8,
    TELEM_RESERVE9,
    TELEM_RESERVE10,
    TELEM_MIN_A_FIRST,
    TELEM_MIN_A1=TELEM_MIN_A_FIRST,
    TELEM_MIN_A2,
    TELEM_MIN_A3,
    TELEM_MIN_A4,
    TELEM_MIN_A_LAST=TELEM_MIN_A4,
    // TODO: add A1-4 MAX
            TELEM_MIN_ALT,
    TELEM_MAX_ALT,
    TELEM_MAX_RPM,
    TELEM_MAX_T1,
    TELEM_MAX_T2,
    TELEM_MAX_SPEED,
    TELEM_MAX_DIST,
    TELEM_MAX_ASPEED,
    TELEM_MIN_CELL,
    TELEM_MIN_CELLS_SUM,
    TELEM_MIN_VFAS,
    TELEM_MAX_CURRENT,
    TELEM_MAX_POWER,
    TELEM_RESERVE11,
    TELEM_RESERVE12,
    TELEM_RESERVE13,
    TELEM_RESERVE14,
    TELEM_RESERVE15,
    TELEM_ACC,
    TELEM_GPS_TIME,
    TELEM_CSW_MAX = TELEM_MAX_POWER,
    TELEM_NOUSR_MAX = TELEM_A2,
    TELEM_DISPLAY_MAX = TELEM_MAX_POWER,
    TELEM_STATUS_MAX = TELEM_GPS_TIME,
    TELEM_FIRST_STREAMED_VALUE = TELEM_RSSI_TX,
};

#define NUM_TELEMETRY      TELEM_CSW_MAX

#if defined(PCBTARANIS)
enum CurveType {
    CURVE_TYPE_STANDARD,
    CURVE_TYPE_CUSTOM,
    CURVE_TYPE_LAST = CURVE_TYPE_CUSTOM
};
PACK(typedef struct {
    uint8_t type:3;
    uint8_t smooth:1;
    uint8_t spare:4;
    int8_t  points;
}) CurveInfo;
#define NUM_POINTS         512
#define CURVDATA           CurveInfo
#else
#define LEN_MODEL_NAME     10
  #define LEN_EXPOMIX_NAME   6
  #define LEN_FP_NAME        6
  #define NUM_POINTS         512
  #define CURVDATA           int16_t
#endif

#if defined(PCBTARANIS)
enum CurveRefType {
    CURVE_REF_DIFF,
    CURVE_REF_EXPO,
    CURVE_REF_FUNC,
    CURVE_REF_CUSTOM
};
PACK(typedef struct {
    uint8_t type;
    int8_t  value;
}) CurveRef;
#else
#define MODE_DIFFERENTIAL  0
  #define MODE_EXPO          0
  #define MODE_CURVE         1
#endif

#if defined(PCBTARANIS)
PACK(typedef struct {
    uint8_t  srcRaw;
    uint16_t scale;
    uint8_t  chn;
    int8_t   swtch;
    uint16_t flightModes;
    int8_t   weight;
    int8_t   carryTrim:6;
    uint8_t  mode:2;
    char     name[LEN_EXPOMIX_NAME];
    int8_t   offset;
    CurveRef curve;
    uint8_t  spare;
}) ExpoData_v216;
#else
PACK(typedef struct {
  uint8_t  mode:2;         // 0=end, 1=pos, 2=neg, 3=both
  uint8_t  chn:4;
  uint8_t  curveMode:2;
  int8_t   swtch;
  uint16_t flightModes;
  int8_t   weight;
  char     name[LEN_EXPOMIX_NAME];
  int8_t   curveParam;
}) ExpoData_v216;
typedef ExpoData_v216;
#endif

#if defined(PCBTARANIS)
PACK(typedef struct {
    int16_t min;
    int16_t max;
    int8_t  ppmCenter;
    int16_t offset:14;
    uint16_t symetrical:1;
    uint16_t revert:1;
    char name[LEN_CHANNEL_NAME];
    int8_t curve;
}) LimitData_v216;
#else
PACK(struct LimitData {
  int32_t min:11;
  int32_t max:11;
  int32_t ppmCenter:10;
  int16_t offset:11;
  uint16_t symetrical:1;
  uint16_t revert:1;
  uint16_t spare:3;
  int8_t curve;
  NOBACKUP(char name[LEN_CHANNEL_NAME]);
});
#endif

PACK(typedef struct {
    uint8_t type:3;
    uint8_t smooth:1;
    uint8_t spare:4;
    int8_t  points;
}) CurveData_v216;

#if defined(PCBTARANIS)
PACK(typedef struct {
    uint8_t  destCh;
    uint16_t flightModes;
    uint8_t  mltpx:2;         // multiplex method: 0 means +=, 1 means *=, 2 means :=
    uint8_t  carryTrim:1;
    uint8_t  spare1:5;
    int16_t  weight;
    int8_t   swtch;
    CurveRef curve;
    uint8_t  mixWarn:4;       // mixer warning
    uint8_t  spare2:4;
    uint8_t  delayUp;
    uint8_t  delayDown;
    uint8_t  speedUp;
    uint8_t  speedDown;
    uint8_t  srcRaw;
    int16_t  offset;
    char     name[LEN_EXPOMIX_NAME];
    uint8_t  spare3;
}) MixData_v216;
#else
PACK(typedef struct {
  uint8_t  destCh:5;
  uint8_t  mixWarn:3;         // mixer warning
  uint16_t flightModes;
  uint8_t  curveMode:1;
  uint8_t  noExpo:1;
  int8_t   carryTrim:3;
  uint8_t  mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  uint8_t  spare:1;
  int16_t  weight;
  int8_t   swtch;
  int8_t   curveParam;
  uint8_t  delayUp;
  uint8_t  delayDown;
  uint8_t  speedUp;
  uint8_t  speedDown;
  uint8_t  srcRaw;
  int16_t  offset;
  char     name[LEN_EXPOMIX_NAME];
}) MixData_v216;
#endif

PACK(typedef struct {
  int8_t   mode;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint16_t start;
  uint8_t  countdownBeep:2;
  uint8_t  minuteBeep:1;
  uint8_t  persistent:2;
  uint8_t  spare:3;
  uint16_t value;
}) TimerData_v216;

PACK(typedef struct {
  int16_t trim[NUM_STICKS];
  int8_t swtch;       // swtch of phase[0] is not used
  char name[LEN_FLIGHT_MODE_NAME];
  uint8_t fadeIn;
  uint8_t fadeOut;
  int16_t rotaryEncoders[1];
  int16_t gvars[9];
}) FlightModeData_v216;

PACK(typedef struct { // Logical Switches data
  int8_t  v1;
  int16_t v2;
  int16_t v3;
  uint8_t func;
  uint8_t delay;
  uint8_t duration;
  int8_t  andsw;
}) LogicalSwitchData_v216;

#if defined(PCBTARANIS)
PACK(typedef struct {
    int8_t  swtch;
    uint8_t func;
    PACK(union {
             PACK(struct {
                      char name[8];
                  }) play;

             PACK(struct {
                      int16_t val;
                      uint8_t mode;
                      uint8_t param;
                      int32_t spare2;
                  }) all;

             PACK(struct {
                      int32_t val1;
                      int32_t val2;
                  }) clear;
         });
    uint8_t active;
}) CustomFunctionData_v216;
#else
PACK(typedef struct {
  int8_t  swtch;
  uint8_t func;
  PACK(union {
    PACK(struct {
      char name[6];
    }) play;

    PACK(struct {
      int16_t val;
      uint8_t mode;
      uint8_t param;
      int16_t spare2;
    }) all;

    PACK(struct {
      int32_t val1;
      int16_t val2;
    }) clear;
  });
  uint8_t active;
}) CustomFunctionData_v216;
#endif


PACK(typedef struct {
  uint8_t    source;
  uint8_t    barMin;           // minimum for bar display
  uint8_t    barMax;           // ditto for max display (would usually = ratio)
}) FrSkyBarData_v216;


#if LCD_W >= 212
#define NUM_LINE_ITEMS 3
#else
#define NUM_LINE_ITEMS 2
#endif

PACK(typedef struct {
  uint8_t    sources[NUM_LINE_ITEMS];
}) FrSkyLineData_v216;

typedef union {
  FrSkyBarData_v216  bars[4];
  FrSkyLineData_v216 lines[4];
} FrSkyScreenData_v216;


PACK(struct FrSkyChannelData_v216 {
 uint8_t unused[7];
});



PACK(typedef struct {
  FrSkyChannelData_v216 channels[4];
  uint8_t usrProto; // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh, 3=Halcyon
  uint8_t voltsSource:7;
  uint8_t altitudeDisplayed:1;
  int8_t blades;    // How many blades for RPMs, 0=2 blades
  uint8_t currentSource;
  uint8_t screensType; // 2bits per screen (None/Gauges/Numbers/Script)
  FrSkyScreenData_v216 screens[3];
  uint8_t varioSource;
  int8_t  varioCenterMax;
  int8_t  varioCenterMin;
  int8_t  varioMin;
  int8_t  varioMax;
  uint8_t rssiAlarms[2];
  uint16_t mAhPersistent:1;
  uint16_t storedMah:15;
  int8_t   fasOffset;
}) FrSkyData_v216;

PACK(typedef struct {
  char    file[10];
  char    name[10];
  int8_t  inputs[10];
}) ScriptData_v216;

PACK(typedef struct { // Swash Ring data
  uint8_t   invertELE:1;
  uint8_t   invertAIL:1;
  uint8_t   invertCOL:1;
  uint8_t   type:5;
  uint8_t   collectiveSource;
  uint8_t   value;
}) SwashRingData_v216;

PACK(typedef struct {
  int8_t  rfProtocol;
  uint8_t channelsStart;
  int8_t  channelsCount; // 0=8 channels
  uint8_t failsafeMode;
  int16_t failsafeChannels[MAX_OUTPUT_CHANNELS];
  int8_t  ppmDelay;
  int8_t  ppmFrameLength;
  uint8_t ppmPulsePol;
}) ModuleData_v216;

#if defined(PCBTARANIS) && LCD_W >= 212
PACK(typedef struct {
  char name[LEN_MODEL_NAME];
  uint8_t modelId;
  char bitmap[LEN_BITMAP_NAME];
}) ModelHeader_v216;
#else
PACK(typedef struct {
  char      name[LEN_MODEL_NAME];
  uint8_t   modelId;
}) ModelHeader_v216;
#endif

PACK(struct GVarData {
  char name[3];
  uint32_t min:12;
  uint32_t max:12;
  uint32_t popup:1;
  uint32_t prec:1;
  uint32_t unit:2;
  uint32_t spare:4;
});

PACK(typedef struct {
  ModelHeader_v216 header;
  TimerData_v216 timers[2];
  uint8_t   telemetryProtocol:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  int8_t    spare2:4;
  int8_t    trimInc:3;            // Trim Increments
  uint8_t   disableThrottleWarning:1;
  uint8_t displayChecklist:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   throttleReversed:1;
  uint16_t beepANACenter;        // 1<<0->A1.. 1<<6->A7
  MixData_v216 mixData[MAX_MIXERS];
  LimitData_v216 limitData[MAX_OUTPUT_CHANNELS];
  ExpoData_v216  expoData[MAX_EXPOS];

  CurveData_v216 curves[MAX_CURVES];
  int8_t    points[MAX_CURVE_POINTS];

  LogicalSwitchData_v216 logicalSw[32];
  CustomFunctionData_v216 customFn[MAX_SPECIAL_FUNCTIONS];
  SwashRingData_v216 swashR;
  FlightModeData_v216 flightModeData[MAX_FLIGHT_MODES];

  uint8_t   thrTraceSrc;

  uint16_t switchWarningState;
  uint8_t  switchWarningEnable;

  GVarData gvars[MAX_GVARS];

  FrSkyData_v216 frsky;

  uint8_t externalModule;

#if defined(PCBTARANIS)
  uint8_t trainerMode;
#endif

  ModuleData_v216 moduleData[2 + 1];

#if defined(PCBTARANIS)
  char curveNames[MAX_CURVES][6];
  ScriptData_v216 scriptsData[MAX_SCRIPTS];
  char inputNames[MAX_INPUTS][LEN_INPUT_NAME];
#endif

  uint8_t nPotsToWarn;
  int8_t potPosition[NUM_POTS+NUM_SLIDERS];
  uint8_t rxBattAlarms[2];
}) ModelData_v216;


PACK(struct CalibData {
  int16_t mid;
  int16_t spanNeg;
  int16_t spanPos;
});


PACK(struct TrainerMix {
         uint8_t srcChn:6; // 0-7 = ch1-8
         uint8_t mode:2;   // off,add-mode,subst-mode
         int8_t  studWeight;
     });


PACK(struct TrainerData {
  int16_t        calib[4];
  TrainerMix mix[4];
});

#if defined(COLORLCD)
typedef uint16_t swconfig_t;
typedef uint32_t swarnstate_t;
#elif defined(PCBX9E)
typedef uint64_t swconfig_t;
typedef uint64_t swarnstate_t;
typedef uint32_t swarnenable_t;
#elif defined(PCBTARANIS)
typedef uint16_t swconfig_t;
typedef uint16_t swarnstate_t;
typedef uint8_t swarnenable_t;
#else
typedef uint8_t swarnstate_t;
typedef uint8_t swarnenable_t;
#endif

PACK(typedef struct {
    uint8_t   version;
    uint16_t  variant;
    CalibData calib[NUM_STICKS+NUM_POTS+NUM_SLIDERS];
    uint16_t  chkSum;
    int8_t    currModel;
    uint8_t   contrast;
    uint8_t   vBatWarn;
    int8_t    txVoltageCalibration;
    int8_t    backlightMode;
    TrainerData trainer;
    uint8_t   view;            // index of view in main screen
    int8_t    buzzerMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all
    uint8_t   fai:1;
    int8_t    beepMode:2;      // -2=quiet, -1=only alarms, 0=no keys, 1=all
    uint8_t   alarmsFlash:1;
    uint8_t   disableMemoryWarning:1;
    uint8_t   disableAlarmWarning:1;
    uint8_t   stickMode:2;
    int8_t    timezone:5;
    uint8_t   adjustRTC:1;
    uint8_t   inactivityTimer;
    uint8_t   telemetryBaudrate:3;
    int8_t    splashMode:3;
    int8_t    hapticMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all
    int8_t    switchesDelay;
    uint8_t   lightAutoOff;
    uint8_t   templateSetup;   // RETA order for receiver channels
    int8_t    PPM_Multiplier;
    int8_t    hapticLength;
    uint8_t   reNavigation;
#if !defined(PCBTARANIS)
    uint8_t   stickReverse;
#endif
    int8_t    beepLength:3;
    int8_t    hapticStrength:3;
    uint8_t   gpsFormat:1;
    uint8_t   unexpectedShutdown:1;
    uint8_t   speakerPitch;
    int8_t    speakerVolume;
    int8_t    vBatMin;
    int8_t    vBatMax;

    uint8_t  backlightBright;
    int8_t   txCurrentCalibration;
    int8_t   temperatureWarn;
    uint8_t  mAhWarn;
    uint16_t mAhUsed;
    uint32_t globalTimer;
    int8_t   temperatureCalib;
    uint8_t  bluetoothBaudrate;
    uint8_t  optrexDisplay;
    uint8_t  sticksGain;
    uint8_t  rotarySteps;
    uint8_t  countryCode;
    uint8_t  imperial;
    char     ttsLanguage[2];
    int8_t   beepVolume;
    int8_t   wavVolume;
    int8_t   varioVolume;
    int8_t   varioPitch;
    int8_t   varioRange;
    int8_t   varioRepeat;
    int8_t   backgroundVolume;

#if defined(PCBTARANIS)
    uint8_t serial2Mode:6;
    uint8_t slidersConfig:2;
    uint8_t potsConfig;
    uint8_t backlightColor;
    swarnstate_t switchUnlockStates;
    CustomFunctionData_v216 customFn[MAX_SPECIAL_FUNCTIONS];
    swconfig_t switchConfig;
    char switchNames[NUM_SWITCHES][LEN_SWITCH_NAME];
    char anaNames[NUM_STICKS+NUM_POTS+NUM_SLIDERS][LEN_ANA_NAME];
#else
    CustomFunctionData_v216 customFn[MAX_SPECIAL_FUNCTIONS];
#endif

#if defined(PCBX9E)
    uint8_t bluetoothEnable;
    char bluetoothName[LEN_BLUETOOTH_NAME];
#endif
}) RadioData_v216;
