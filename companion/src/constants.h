#ifndef _constants_h_
#define _constants_h_

enum BoardEnum {
  BOARD_STOCK,
  BOARD_M128,
  BOARD_MEGA2560,
  BOARD_GRUVIN9X,
  BOARD_SKY9X,
  BOARD_9XRPRO,
  BOARD_AR9X,
  BOARD_TARANIS,
  BOARD_TARANIS_PLUS,
  BOARD_TARANIS_X9E
};

#define C9X_MAX_MODELS            60
#define C9X_MAX_TIMERS            3
#define C9X_MAX_FLIGHT_MODES      9
#define C9X_MAX_MIXERS            64
#define C9X_MAX_INPUTS            32
#define C9X_MAX_EXPOS             64
#define C9X_MAX_CURVES            32
#define C9X_MAX_POINTS            17
#define C9X_MAX_GVARS             9
#define C9X_MAX_ENCODERS          2
#define C9X_NUM_CHNOUT            32 // number of real output channels
#define C9X_NUM_CSW               64 // number of custom switches
#define C9X_MAX_CUSTOM_FUNCTIONS  64 // number of functions assigned to switches
#define C9X_NUM_MODULES           2
#define NUM_STICKS                4
#define BOARD_9X_NUM_POTS         3
#define C9X_NUM_POTS              8
#define NUM_CAL_PPM               4
#define NUM_CYC                   3
#define C9X_NUM_SWITCHES          32
#define C9X_NUM_KEYS              32

#endif
