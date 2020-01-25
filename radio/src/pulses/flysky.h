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

#ifndef _PULSES_FLYSKY_H_
#define _PULSES_FLYSKY_H_
<<<<<<< Updated upstream
#include "afhds2.h"

/*
#define END                             0xC0
#define ESC                             0xDB
#define ESC_END                         0xDC
#define ESC_ESC                         0xDD
*/

enum FlySkyModuleCommandID {
  COMMAND_ID_NONE,
  COMMAND_ID_RF_INIT,
  COMMAND_ID_BIND,
  COMMAND_ID_SET_RECEIVER_ID,
  COMMAND_ID_RF_GET_CONFIG,
  COMMAND_ID_SEND_CHANNEL_DATA,
  COMMAND_ID_RX_SENSOR_DATA,
  COMMAND_ID_SET_RX_PWM_PPM,
  COMMAND_ID_SET_RX_SERVO_FREQ,
  COMMAND_ID_GET_FIRMWARE_REV,
  COMMAND_ID_SET_RX_IBUS_SBUS,
  COMMAND_ID_SET_RX_IBUS_SERVO_EXT,
  COMMAND_ID0C_UPDATE_RF_FIRMWARE = 0x0C,
  COMMAND_ID0D_SET_TX_POWER = 0x0D,
  COMMAND_ID_SET_RF_PROTOCOL,
  COMMAND_ID_TEST_RANGE,
  COMMAND_ID_TEST_RF_RESERVED,
  COMMAND_ID20_UPDATE_RX_FIRMWARE = 0x20,
  COMMAND_ID_LAST
};

enum FlySkySensorType_E {
  FLYSKY_SENSOR_RX_VOLTAGE,
  FLYSKY_SENSOR_RX_SIGNAL,
  FLYSKY_SENSOR_RX_RSSI,
  FLYSKY_SENSOR_RX_NOISE,
  FLYSKY_SENSOR_RX_SNR,
  FLYSKY_SENSOR_TEMP,
  FLYSKY_SENSOR_EXT_VOLTAGE,
  FLYSKY_SENSOR_MOTO_RPM,
  FLYSKY_SENSOR_PRESURRE,
  FLYSKY_SENSOR_GPS
};

enum FlySkyModuleState_E {
  FLYSKY_MODULE_STATE_SET_TX_POWER,
  FLYSKY_MODULE_STATE_INIT,
  FLYSKY_MODULE_STATE_BIND,
  FLYSKY_MODULE_STATE_SET_RECEIVER_ID,
  FLYSKY_MODULE_STATE_SET_RX_PWM_PPM,
  FLYSKY_MODULE_STATE_SET_RX_IBUS_SBUS,
  FLYSKY_MODULE_STATE_SET_RX_FREQUENCY,
  FLYSKY_MODULE_STATE_UPDATE_RF_FIRMWARE,
  FLYSKY_MODULE_STATE_UPDATE_RX_FIRMWARE,
  FLYSKY_MODULE_STATE_UPDATE_HALL_FIRMWARE,
  FLYSKY_MODULE_STATE_UPDATE_RF_PROTOCOL,
  FLYSKY_MODULE_STATE_GET_RECEIVER_CONFIG,
  FLYSKY_MODULE_STATE_GET_RX_FIRMWARE_INFO,
  FLYSKY_MODULE_STATE_GET_RX_FW_VERSION,
  FLYSKY_MODULE_STATE_GET_RF_FIRMWARE_INFO,
  FLYSKY_MODULE_STATE_GET_RF_FW_VERSION,
  FLYSKY_MODULE_STATE_IDLE,
  FLYSKY_MODULE_STATE_DEFAULT,
};

enum FlySkyBindState_E {
  BIND_LOW_POWER,
  BIND_NORMAL_POWER,
  BIND_EXIT,
};

enum FlySkyRxPulse_E {
  FLYSKY_PWM,
  FLYSKY_PPM
};

enum FlySkyRxPort_E {
  FLYSKY_IBUS,
  FLYSKY_SBUS
};

enum FlySkyFirmwareType_E {
  FLYSKY_RX_FIRMWARE,
  FLYSKY_RF_FIRMWARE
};

enum FlySkyChannelDataType_E {
  FLYSKY_CHANNEL_DATA_NORMAL,
  FLYSKY_CHANNEL_DATA_FAILSAFE
};

enum FlySkyPulseModeValue_E {
  PWM_IBUS, PWM_SBUS,
  PPM_IBUS, PPM_SBUS
};

enum DEBUG_RF_FRAME_PRINT_E {
  FRAME_PRINT_OFF,// 0:OFF, 1:RF frame only, 2:TX frame only, 3:Both RF and TX frame
  RF_FRAME_ONLY,
  TX_FRAME_ONLY,
  BOTH_FRAME_PRINT
};


void setFlyskyState(uint8_t port, uint8_t state);
void onFlySkyBindReceiver(uint8_t port);
void onFlySkyModuleSetPower(uint8_t port, bool isPowerOn);
void onFlySkyGetVersionInfoStart(uint8_t port, uint8_t isRfTransfer);
=======

struct AfhdsPulsesData {
  uint8_t  pulses[64];
  uint8_t  * ptr;
  uint8_t  frame_index;
  uint8_t  crc;
  uint8_t  state;
  uint8_t  timeout;
  uint8_t  esc_state;
  uint8_t  telemetry[64];
  uint8_t  telemetry_index;
  const uint8_t * getData()
  {
  	return pulses;
  }
  uint8_t getSize()
  {
	  return ptr - pulses;
  }

} __attribute__((__packed__));

enum FlySkyModuleState_E {
  STATE_SET_TX_POWER = 0,
  STATE_INIT = 1,
  STATE_BIND = 2,
  STATE_SET_RECEIVER_ID = 3,
  STATE_SET_RX_PWM_PPM = 4,
  STATE_SET_RX_IBUS_SBUS = 5,
  STATE_SET_RX_FREQUENCY = 6,
  STATE_UPDATE_RF_FIRMWARE = 7,
  STATE_UPDATE_RX_FIRMWARE = 8,
  STATE_UPDATE_HALL_FIRMWARE = 9,
  STATE_UPDATE_RF_PROTOCOL = 10,
  STATE_GET_RECEIVER_CONFIG = 11,
  STATE_GET_RX_VERSION_INFO = 12,
  STATE_GET_RF_VERSION_INFO = 13,
  STATE_SET_RANGE_TEST = 14,
  STATE_RANGE_TEST_RUNNING = 15,
  STATE_IDLE = 16,
  STATE_DEFAULT_AFHDS2 = 17,
};
extern uint8_t tx_working_power;

void resetPulsesAFHDS2(uint8_t port, uint8_t targetMode);
void setupPulsesAFHDS2(uint8_t port);
void bindReceiverAFHDS2(uint8_t port);

void getVersionInfoAFHDS2(uint8_t port, uint8_t isRfTransfer);
//used to update config or force rx update mode
void setFlyskyState(uint8_t port, uint8_t state);
>>>>>>> Stashed changes
void usbDownloadTransmit(uint8_t *buffer, uint32_t size);
#endif
