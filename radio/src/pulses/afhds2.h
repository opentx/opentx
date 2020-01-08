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


#ifndef PULSES_AFHDS2_H_
#define PULSES_AFHDS2_H_
#include <functional>
#include <map>
#include <list>
#include "opentx.h"
#include "flysky.h"
#include "modules_helpers.h"


enum AfhdsSpecialChars {
  END = 0xC0,             //Frame end
  START = END,
  ESC_END = 0xDC,         //Escaped frame end - in case END occurs in fame then ESC ESC_END must be used
  ESC = 0xDB,             //Escaping character
  ESC_ESC = 0xDD,         //Escaping character in case ESC occurs in fame then ESC ESC_ESC  must be used
};


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


#define AFHDS_COMMAND_TIMEOUT 5
class afhds2 : public AbstractSerialModule {
public:

  virtual ~afhds2();

  afhds2(AbstractModule** moduleCollection, uint8_t index, uint8_t protocol, AfhdsPulsesData* data) :
    AbstractSerialModule(moduleCollection, index, protocol, AFHDS_COMMAND_TIMEOUT, AFHDS2_BAUDRATE) {
      this->data = data;
      this->moduleData = moduleData;
      this->getChannelValue = getChannelValue;
      this->failsafeChannels = failsafeChannels;
      init();
    }

    void setupFrame();
    void init(bool resetFrameCount = true);
    void beginBind(::asyncOperationCallback_t callback);
    void beginRangeTest(::asyncOperationCallback_t callback);
    void cancelOperations();
    void stop();
    void setModuleSettingsToDefault();
    void onDataReceived(uint8_t data, uint8_t* rxBuffer, uint8_t& rxBufferCount, uint8_t maxSize);
    const char* getState();
protected:
    AfhdsPulsesData* data;
};

#endif /* PULSES_AFHDS2_H_ */
