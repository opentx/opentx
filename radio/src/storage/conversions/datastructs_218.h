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


#ifndef OPENTX_DATASTRUCTS_218_H
#define OPENTX_DATASTRUCTS_218_H

PACK(typedef struct {
  int32_t  mode:9;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint32_t start:23;
  int32_t  value:24;
  uint32_t countdownBeep:2;
  uint32_t minuteBeep:1;
  uint32_t persistent:2;
  int32_t  countdownStart:2;
  uint32_t direction:1;
  char     name[LEN_TIMER_NAME];
}) TimerData_v218;

PACK(typedef struct {
  trim_t trim[NUM_STICKS];
  char name[LEN_FLIGHT_MODE_NAME];
  int16_t swtch:9;       // swtch of phase[0] is not used
  int16_t spare:7;
  uint8_t fadeIn;
  uint8_t fadeOut;
  int16_t rotaryEncoders[1];
  gvar_t gvars[9];
}) FlightModeData_v218;

PACK(typedef struct {
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
  char     name[LEN_EXPOMIX_NAME];
}) MixData_v218;

PACK(typedef struct {
  uint16_t mode:2;
  uint16_t scale:14;
  uint16_t srcRaw:10;
  int16_t  carryTrim:6;
  uint32_t chn:5;
  int32_t  swtch:9;
  uint32_t flightModes:9;
  int32_t  weight:8;
  int32_t  spare:1;
  char     name[LEN_EXPOMIX_NAME];
  int8_t   offset;
  CurveRef curve;
}) ExpoData_v218;

PACK(typedef struct {
  uint8_t type:1;
  uint8_t smooth:1;
  int8_t  points:6;   // describes number of points - 5
  char name[LEN_CURVE_NAME];
}) CurveData_v218;

PACK(typedef struct { // Logical Switches data
  uint8_t  func;
  int32_t  v1:10;
  int32_t  v3:10;
  int32_t  andsw:9;
  uint32_t andswtype:1;
  uint32_t spare:2;
  int16_t  v2;
  uint8_t  delay;
  uint8_t  duration;
}) LogicalSwitchData_v218;

PACK(typedef struct {
  int16_t  swtch:9;
  uint16_t func:7;
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
}) CustomFunctionData_v218;

PACK(typedef struct {
  char    name[6];
  uint32_t min:12;
  uint32_t max:12;
  uint32_t popup:1;
  uint32_t prec:1;
  uint32_t unit:2;
  uint32_t spare:4;
}) GVarData_v218;

#define MODELDATA_EXTRA_218 \
uint8_t spare:3; \
uint8_t trainerMode:3; \
uint8_t potsWarnMode:2; \
ModuleData moduleData[NUM_MODULES+1]; \
ScriptData scriptsData[MAX_SCRIPTS]; \
char inputNames[MAX_INPUTS][LEN_INPUT_NAME]; \
uint8_t potsWarnEnabled; \
int8_t potsWarnPosition[NUM_POTS+NUM_SLIDERS];

PACK(typedef struct {
  ModelHeader header;
  TimerData_v218 timers[MAX_TIMERS];
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
  MixData_v218 mixData[MAX_MIXERS];
  LimitData limitData[MAX_OUTPUT_CHANNELS];
  ExpoData_v218  expoData[MAX_EXPOS];

  CurveData_v218 curves[MAX_CURVES];
  int8_t    points[MAX_CURVE_POINTS];

  LogicalSwitchData_v218 logicalSw[32];
  CustomFunctionData_v218 customFn[MAX_SPECIAL_FUNCTIONS];
  SwashRingData swashR;
  FlightModeData_v218 flightModeData[MAX_FLIGHT_MODES];

  uint8_t thrTraceSrc;

  swarnstate_t  switchWarningState;
  swarnenable_t switchWarningEnable;

  GVarData_v218 gvars[MAX_GVARS];

  FrSkyTelemetryData frsky;
  RssiAlarmData rssiAlarms;

  MODELDATA_EXTRA_218

    TelemetrySensor telemetrySensors[MAX_TELEMETRY_SENSORS];

  TARANIS_PCBX9E_FIELD(uint8_t toplcdTimer)
}) ModelData_v218;

#if defined(BUZZER)
#define BUZZER_FIELD int8_t buzzerMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all (only used on AVR radios without audio hardware)
#else
#define BUZZER_FIELD int8_t spareRadio:2;
#endif


PACK(typedef struct {
  uint8_t version;
  uint16_t variant;
  CalibData calib[NUM_STICKS+NUM_POTS+NUM_SLIDERS+NUM_MOUSE_ANALOGS+NUM_DUMMY_ANAS];
  uint16_t chkSum;
  uint8_t vBatWarn;
  int8_t txVoltageCalibration;
  int8_t backlightMode;
  TrainerData trainer;
  uint8_t view;            // index of view in main screen
  BUZZER_FIELD
  uint8_t fai:1;
  int8_t beepMode:2;      // -2=quiet, -1=only alarms, 0=no keys, 1=all
  uint8_t alarmsFlash:1;
  uint8_t disableMemoryWarning:1;
  uint8_t disableAlarmWarning:1;
  uint8_t stickMode:2;
  int8_t timezone:5;
  uint8_t adjustRTC:1;
  uint8_t inactivityTimer;
  uint8_t telemetryBaudrate:3;
  int8_t splashMode:3;
  int8_t hapticMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all
  int8_t switchesDelay;
  uint8_t lightAutoOff;
  uint8_t templateSetup;   // RETA order for receiver channels
  int8_t PPM_Multiplier;
  int8_t hapticLength;
  int8_t beepLength:3);
  int8_t hapticStrength:3;
  uint8_t gpsFormat:1;
  uint8_t unexpectedShutdown:1;
  uint8_t speakerPitch;
  int8_t speakerVolume;
  int8_t vBatMin;
  int8_t vBatMax;
  uint8_t  backlightBright;
  uint32_t globalTimer;
  uint8_t  bluetoothBaudrate:4;
  uint8_t  bluetoothMode:4;
  uint8_t  countryCode;
  uint8_t  imperial:1;
  uint8_t  jitterFilter:1; /* 0 - active */
  uint8_t  disableRssiPoweroffAlarm:1;
  uint8_t  USBMode:2;
  uint8_t  spareExtraArm:3;
  char     ttsLanguage[2];
  int8_t   beepVolume:4;
  int8_t   wavVolume:4;
  int8_t   varioVolume:4;
  int8_t   backgroundVolume:4;
  int8_t   varioPitch;
  int8_t   varioRange;
  int8_t   varioRepeat;
  CustomFunctionData_v218 customFn[MAX_SPECIAL_FUNCTIONS];
}) RadioData_v218;

#endif //OPENTX_DATASTRUCTS_218_H
