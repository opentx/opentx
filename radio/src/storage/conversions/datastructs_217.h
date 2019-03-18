
enum Mix216Sources {
    MIXSRC216_NONE,

#if defined(PCBTARANIS)
    MIXSRC216_FIRST_INPUT,
    MIXSRC216_LAST_INPUT = MIXSRC216_FIRST_INPUT+MAX_INPUTS-1,

    MIXSRC216_FIRST_LUA,
    MIXSRC216_LAST_LUA = MIXSRC216_FIRST_LUA+(MAX_SCRIPTS*MAX_SCRIPT_OUTPUTS)-1,
#endif

    MIXSRC216_Rud,
    MIXSRC216_Ele,
    MIXSRC216_Thr,
    MIXSRC216_Ail,

    MIXSRC216_FIRST_POT,
#if defined(PCBTARANIS)
    MIXSRC216_POT1 = MIXSRC216_FIRST_POT,
    MIXSRC216_POT2,
    MIXSRC216_POT3,
    MIXSRC216_SLIDER1,
    MIXSRC216_SLIDER2,
    MIXSRC216_LAST_POT = MIXSRC216_SLIDER2,
#else
    MIXSRC216_P1 = MIXSRC216_FIRST_POT,
  MIXSRC216_P2,
  MIXSRC216_P3,
  MIXSRC216_LAST_POT = MIXSRC216_P3,
#endif

#if defined(PCBSKY9X)
    MIXSRC216_REa,
  MIXSRC216_LAST_ROTARY_ENCODER = MIXSRC216_REa,
#endif

    MIXSRC216_MAX,

    MIXSRC216_CYC1,
    MIXSRC216_CYC2,
    MIXSRC216_CYC3,

    MIXSRC216_TrimRud,
    MIXSRC216_TrimEle,
    MIXSRC216_TrimThr,
    MIXSRC216_TrimAil,

    MIXSRC216_FIRST_SWITCH,

#if defined(PCBTARANIS)
    MIXSRC216_SA = MIXSRC216_FIRST_SWITCH,
    MIXSRC216_SB,
    MIXSRC216_SC,
    MIXSRC216_SD,
    MIXSRC216_SE,
    MIXSRC216_SF,
    MIXSRC216_SG,
    MIXSRC216_SH,
#else
    MIXSRC216_3POS = MIXSRC216_FIRST_SWITCH,
  MIXSRC216_THR,
  MIXSRC216_RUD,
  MIXSRC216_ELE,
  MIXSRC216_AIL,
  MIXSRC216_GEA,
  MIXSRC216_TRN,
#endif
    MIXSRC216_FIRST_LOGICAL_SWITCH,
    MIXSRC216_SW1 = MIXSRC216_FIRST_LOGICAL_SWITCH,
    MIXSRC216_SW9 = MIXSRC216_SW1 + 8,
    MIXSRC216_SWA,
    MIXSRC216_SWB,
    MIXSRC216_SWC,
    MIXSRC216_LAST_LOGICAL_SWITCH = MIXSRC216_FIRST_LOGICAL_SWITCH+32-1,

    MIXSRC216_FIRST_TRAINER,
    MIXSRC216_LAST_TRAINER = MIXSRC216_FIRST_TRAINER+MAX_TRAINER_CHANNELS-1,

    MIXSRC216_FIRST_CH,
    MIXSRC216_CH1 = MIXSRC216_FIRST_CH,
    MIXSRC216_CH2,
    MIXSRC216_CH3,
    MIXSRC216_CH4,
    MIXSRC216_CH5,
    MIXSRC216_CH6,
    MIXSRC216_CH7,
    MIXSRC216_CH8,
    MIXSRC216_CH9,
    MIXSRC216_CH10,
    MIXSRC216_CH11,
    MIXSRC216_CH12,
    MIXSRC216_CH13,
    MIXSRC216_CH14,
    MIXSRC216_CH15,
    MIXSRC216_CH16,
    MIXSRC216_LAST_CH = MIXSRC216_CH1+MAX_OUTPUT_CHANNELS-1,

    MIXSRC216_GVAR1,
    MIXSRC216_LAST_GVAR = MIXSRC216_GVAR1+MAX_GVARS-1,

    MIXSRC216_FIRST_TELEM,
};

enum Telemetry216Source {
    TELEM216_NONE,
    TELEM216_TX_VOLTAGE,
    TELEM216_TX_TIME,
    TELEM216_RESERVE1,
    TELEM216_RESERVE2,
    TELEM216_RESERVE3,
    TELEM216_RESERVE4,
    TELEM216_RESERVE5,
    TELEM216_TIMER1,
    TELEM216_TIMER2,
    TELEM216_SWR,
    TELEM216_RSSI_TX,
    TELEM216_RSSI_RX,
    TELEM216_RESERVE0,
    TELEM216_A1,
    TELEM216_A2,
    TELEM216_A3,
    TELEM216_A4,
    TELEM216_ALT,
    TELEM216_RPM,
    TELEM216_FUEL,
    TELEM216_T1,
    TELEM216_T2,
    TELEM216_SPEED,
    TELEM216_DIST,
    TELEM216_GPSALT,
    TELEM216_CELL,
    TELEM216_CELLS_SUM,
    TELEM216_VFAS,
    TELEM216_CURRENT,
    TELEM216_CONSUMPTION,
    TELEM216_POWER,
    TELEM216_ACCx,
    TELEM216_ACCy,
    TELEM216_ACCz,
    TELEM216_HDG,
    TELEM216_VSPEED,
    TELEM216_ASPEED,
    TELEM216_DTE,
    TELEM216_RESERVE6,
    TELEM216_RESERVE7,
    TELEM216_RESERVE8,
    TELEM216_RESERVE9,
    TELEM216_RESERVE10,
    TELEM216_MIN_A1,
    TELEM216_MIN_A2,
    TELEM216_MIN_A3,
    TELEM216_MIN_A4,
    TELEM216_MIN_ALT,
    TELEM216_MAX_ALT,
    TELEM216_MAX_RPM,
    TELEM216_MAX_T1,
    TELEM216_MAX_T2,
    TELEM216_MAX_SPEED,
    TELEM216_MAX_DIST,
    TELEM216_MAX_ASPEED,
    TELEM216_MIN_CELL,
    TELEM216_MIN_CELLS_SUM,
    TELEM216_MIN_VFAS,
    TELEM216_MAX_CURRENT,
    TELEM216_MAX_POWER,
    TELEM216_RESERVE11,
    TELEM216_RESERVE12,
    TELEM216_RESERVE13,
    TELEM216_RESERVE14,
    TELEM216_RESERVE15,
    TELEM216_ACC,
    TELEM216_GPS_TIME,
};

#if defined(PCBTARANIS)
PACK(typedef struct {
    uint32_t srcRaw:10;
    uint32_t scale:14;
    uint32_t chn:8;
    int8_t   swtch;
    uint16_t flightModes;
    int8_t   weight;
    int8_t   carryTrim:6;
    uint8_t  mode:2;
    char     name[LEN_EXPOMIX_NAME];
    int8_t   offset;
    CurveRef curve;
}) ExpoData_v217;
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
typedef ExpoData_v217;
#endif

#if defined(PCBTARANIS)
PACK(typedef struct {
    uint8_t  destCh;
    uint16_t flightModes:9;
    uint16_t mltpx:2;         // multiplex method: 0 means +=, 1 means *=, 2 means :=
    uint16_t carryTrim:1;
    uint16_t mixWarn:4;       // mixer warning
    int16_t  weight;
    uint32_t srcRaw:10;
    int32_t  offset:14;
    int32_t  swtch:8;
    CurveRef curve;
    uint8_t  delayUp;
    uint8_t  delayDown;
    uint8_t  speedUp;
    uint8_t  speedDown;
    char     name[LEN_EXPOMIX_NAME];
}) MixData_v217;
#else
PACK(struct CurveRef {
  uint8_t type;
  int8_t  value;
});

PACK(struct MixData {
  int16_t  weight:11;       // GV1=-1024, -GV1=1023
  uint16_t destCh:5;
  uint16_t srcRaw:10;       // srcRaw=0 means not used
  uint16_t carryTrim:1;
  uint16_t mixWarn:2;       // mixer warning
  uint16_t mltpx:2;         // multiplex method: 0 means +=, 1 means *=, 2 means :=
  uint16_t spare:1;
  int32_t  offset:14;
  int32_t  swtch:9;
  uint32_t flightModes:9;
  CurveRef curve;
  uint8_t  delayUp;
  uint8_t  delayDown;
  uint8_t  speedUp;
  uint8_t  speedDown;
  NOBACKUP(char name[LEN_EXPOMIX_NAME]);
});
#endif

PACK(typedef struct {
    int32_t  mode:8;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
    uint32_t start:24;
    int32_t  value:24;
    uint32_t countdownBeep:2;
    uint32_t minuteBeep:1;
    uint32_t persistent:2;
    uint32_t spare:3;
    char     name[LEN_TIMER_NAME];
}) TimerData_v217;

PACK(typedef struct { // Logical Switches data
    uint16_t func:6;
    int16_t  v1:10;
    int16_t  v2;
    int16_t  v3;
    uint8_t  delay;
    uint8_t  duration;
    int8_t   andsw;
}) LogicalSwitchData_v217;

PACK(typedef struct {
  uint8_t type:4;
  int8_t  rfProtocol:4;
  uint8_t channelsStart;
  int8_t  channelsCount; // 0=8 channels
  union {
    struct {
      uint8_t failsafeMode:4;  // only 3 bits used
      uint8_t subType:3;
      uint8_t invertedSerial:1; // telemetry serial inverted from standard
    };
    struct {
      uint8_t failsafeMode:4;  // only 3 bits used
      uint8_t freq:2;
      uint8_t region:2;
    } r9m;
  };
  int16_t failsafeChannels[MAX_OUTPUT_CHANNELS];
  union {
    struct {
      int8_t  delay:6;
      uint8_t pulsePol:1;
      uint8_t outputType:1;    // false = open drain, true = push pull
      int8_t  frameLength;
    } ppm;
  };
}) ModuleData_v217;


PACK(typedef struct {
  char    name[6];
  uint8_t popup:1;
  uint8_t spare:7;
}) GVarData_v217;

PACK(typedef struct {
  ModelHeader header;
  TimerData_v217 timers[MAX_TIMERS];
  uint8_t   telemetryProtocol:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  uint8_t   noGlobalFunctions:1;
  uint8_t   displayTrims:2;
  uint8_t   ignoreSensorIds:1;
  int8_t    trimInc:3;            // Trim Increments
  uint8_t   disableThrottleWarning:1;
  uint8_t displayChecklist:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   throttleReversed:1;
  BeepANACenter beepANACenter;
  MixData_v217 mixData[MAX_MIXERS];
  LimitData limitData[MAX_OUTPUT_CHANNELS];
  ExpoData_v217  expoData[MAX_EXPOS];

  CurveData_v216 curves[MAX_CURVES];
  int8_t    points[MAX_CURVE_POINTS];

  LogicalSwitchData_v217 logicalSw[32];
  CustomFunctionData_v216 customFn[MAX_SPECIAL_FUNCTIONS];
  SwashRingData swashR;
  FlightModeData_v216 flightModeData[MAX_FLIGHT_MODES];

  uint8_t thrTraceSrc;

  swarnstate_t  switchWarningState;
  swarnenable_t switchWarningEnable;

  GVarData_v217 gvars[MAX_GVARS];

  FrSkyTelemetryData frsky;
  RssiAlarmData rssiAlarms;

  uint8_t spare:3;
  uint8_t trainerMode:3;
  uint8_t potsWarnMode:2;
  ModuleData_v217 moduleData[NUM_MODULES+1];

#if defined(PCBTARANIS)
  char curveNames[MAX_CURVES][6];
  ScriptData scriptsData[MAX_SCRIPTS];
  char inputNames[MAX_INPUTS][LEN_INPUT_NAME];
#endif

  uint8_t potsWarnEnabled;
  int8_t potsWarnPosition[NUM_POTS+NUM_SLIDERS];

#if defined(PCBTARANIS)
  TelemetrySensor telemetrySensors[MAX_TELEMETRY_SENSORS];
#else
  uint8_t rxBattAlarms[2];
#endif

  TARANIS_PCBX9E_FIELD(uint8_t toplcdTimer)
}) ModelData_v217;
