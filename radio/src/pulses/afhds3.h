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


#ifndef PULSES_AFHDS3_H_
#define PULSES_AFHDS3_H_

#include "afhds2.h"
#include "modules_helpers.h"
#include <cstring>
#include <queue>

namespace afhds3 {

enum DeviceAddress {
  TRANSMITTER = 0x01, MODULE = 0x03,
};

enum FRAME_TYPE {
  REQUEST_GET_DATA = 0x01,  //Get data response: ACK + DATA
  REQUEST_SET_EXPECT_DATA = 0x02,  //Set data response: ACK + DATA
  REQUEST_SET_EXPECT_ACK = 0x03,  //Set data response: ACK
  REQUEST_SET_NO_RESP = 0x05,  //Set data response: none
  RESPONSE_DATA = 0x10,  //Response ACK + DATA
  RESPONSE_ACK = 0x20,  //Response ACK
  NOT_USED = 0xff
};

enum COMMAND {
  MODULE_READY = 0x01,
  MODULE_STATE = 0x02,
  MODULE_MODE = 0x03,
  MODULE_SET_CONFIG = 0x04,
  MODULE_GET_CONFIG = 0x06,
  CHANNELS_FAILSAFE_DATA = 0x07,
  TELEMETRY_DATA = 0x09,
  SEND_COMMAND = 0x0C,
  COMMAND_RESULT = 0x0D,
  MODULE_POWER_STATUS = 0x0F,
  MODULE_VERSION = 0x1F,
};

enum COMMAND_DIRECTION {
  RADIO_TO_MODULE = 0, MODULE_TO_RADIO = 1
};

enum DATA_TYPE {
  READY_DT,  // 8 bytes 0x01 Not ready 0x02 Ready
  STATE_DT,  // See MODULE_STATE
  MODE_DT,
  MOD_CONFIG_DT,
  CHANNELS_DT,
  TELEMETRY_DT,
  MODULE_POWER_DT,
  MODULE_VERSION_DT,
  EMPTY_DT,
};
//enum used by command response -> translate to ModuleState
enum MODULE_READY_E {
  MODULE_STATUS_UNKNOWN = 0x00,
  MODULE_STATUS_NOT_READY = 0x01,
  MODULE_STATUS_READY = 0x02
};

enum ModuleState {
  STATE_HW_ERROR = 0x01,
  STATE_BINDING = 0x02,
  STATE_SYNC_RUNNING = 0x03,
  STATE_SYNC_DONE = 0x04,
  STATE_STANDBY = 0x05,
  STATE_UPDATING_WAIT = 0x06,
  STATE_UPDATING_MOD = 0x07,
  STATE_UPDATING_RX = 0x08,
  STATE_UPDATING_RX_FAILED = 0x09,
  STATE_RF_TESTING = 0x0a,
  STATE_NOT_READY = 0x0b, //virtual
  STATE_READY = 0x0c,     //virtual
  STATE_HW_TEST = 0xff,
};

//used for set command
enum MODULE_MODE_E {
  STANDBY = 0x01,
  BIND = 0x02,  //after bind module will enter run mode
  RUN = 0x03,
  RX_UPDATE = 0x04, //after successful update module will enter standby mode, otherwise hw error will be raised
};

enum CMD_RESULT {
  FAILURE = 0x01,
  SUCCESS = 0x02,
};


#define MIN_FREQ 50
#define MAX_FREQ 400
#define MAX_CHANNELS 18
#define FAILSAFE_KEEP_LAST 0x8000
#define FAILSAFE_MIN -15000
#define FAILSAFE_MAX 15000

enum BIND_POWER {
  MIN_16dBm = 0x00,
  MIN_5dBm = 0x01,
  MIN_0dbm = 0x02,
  PLUS_5dBm = 0x03,
  PLUS_14dBm = 0x04
};

enum RUN_POWER {
  PLUS_15bBm = 0x00,
  PLUS_20bBm = 0x01,
  PLUS_27dbm = 0x02,
  PLUS_30dBm = 0x03,
  PLUS_33dBm = 0x04
};

enum EMI_STANDARD {
  FCC = 0x00, CE = 0x01
};

enum TELEMETRY {
  TELEMETRY_DISABLED = 0x00, TELEMETRY_ENABLED = 0x01
};

enum PULSE_MODE {
  PWM = 0x00, PPM_MODE = 0x01,
};

enum SERIAL_MODE {
  IBUS = 0x00, SBUS_MODE = 0x02
};

PACK(struct Config_s {
  uint8_t bindPower;
  uint8_t runPower;
  uint8_t emiStandard;
  uint8_t telemetry;
  uint16_t pwmFreq;
  uint8_t pulseMode;
  uint8_t serialMode;
  uint8_t channelCount;
  uint16_t failSafeTimout;
  int16_t failSafeMode[MAX_CHANNELS];
});

union Config_u {
  Config_s config;
  uint8_t buffer[sizeof(Config_s)];
};

enum CHANNELS_DATA_MODE {
  CHANNELS = 0x01, FAIL_SAFE = 0x02,
};

PACK(struct ChannelsData {
  uint8_t mode;
  uint8_t channelsNumber;
  int16_t data[MAX_CHANNELS];
});

union ChannelsData_u {
  ChannelsData data;
  uint8_t buffer[sizeof(ChannelsData)];
};

PACK(struct TelemetryData {
  uint8_t sensorType;
  uint8_t length;
  uint8_t type;
  uint8_t semsorID;
  uint8_t data[8];
});

enum MODULE_POWER_SOURCE {
  INTERNAL = 0x01, EXTERNAL = 0x02,
};

PACK(struct ModuleVersion {
  uint32_t productNumber;
  uint32_t hardwereVersion;
  uint32_t bootloaderVersion;
  uint32_t firmwareVersion;
  uint32_t rfVersion;
});


union AfhdsFrameData {
  uint8_t value;
  Config_s Config;
  ChannelsData Channels;
  TelemetryData Telemetry;
  ModuleVersion Version;
};

PACK(struct AfhdsFrame {
  uint8_t startByte;
  uint8_t address;
  uint8_t frameNumber;
  uint8_t frameType;
  uint8_t command;
  uint8_t value;

  AfhdsFrameData* GetData() {
    return reinterpret_cast<AfhdsFrameData*>(&value);
  }
});

#define FRM302_STATUS 0x56

enum State {
  UNKNOWN = 0,
  SENDING_COMMAND,
  AWAITING_RESPONSE,
  IDLE
};

class request {
  public:
  request(COMMAND command, FRAME_TYPE frameType, const uint8_t* data = nullptr, uint8_t length = 0) {
    this->command = command;
    this->frameType = frameType;
    if(data && length){
      payload = new uint8_t[length];
      std::memcpy(payload, data, length);
    }
    else payload = nullptr;
    payloadSize = length;
  }
  ~request() {
    if(payload != nullptr) {
      delete[] payload;
      payload = nullptr;
    }
  }
  enum COMMAND command;
  enum FRAME_TYPE frameType;
  uint8_t* payload;
  uint8_t payloadSize;
};
#define AFHDS3_BAUDRATE 1500000
#define AFHDS3_COMMAND_TIMEOUT 5
class afhds3 : public ::AbstractSerialModule {
public:
  afhds3(::AbstractModule** moduleCollection, uint8_t index, uint8_t protocol,
      ::AfhdsPulsesData* data, ::processSensor_t processSensor) :
     AbstractSerialModule(moduleCollection, index, protocol, AFHDS3_COMMAND_TIMEOUT, AFHDS3_BAUDRATE) {
    this->data = data;
    this->processSensor = processSensor;
    init();
  }

  virtual ~afhds3() {
    clearQueue();
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
  void setModelSettingsFromModule();
private:
  const uint8_t FrameAddress = DeviceAddress::TRANSMITTER | (DeviceAddress::MODULE << 4);
  const uint16_t commandRepeatCount = 5;
  void putByte(uint8_t byte);
  void putBytes(uint8_t* data, int length);
  void putHeader(COMMAND command, FRAME_TYPE frameType);
  void putFooter();
  void putFrame(COMMAND command, FRAME_TYPE frameType, uint8_t* data = nullptr, uint8_t dataLength = 0);
  void addToQueue(COMMAND command, FRAME_TYPE frameType, uint8_t* data = nullptr, uint8_t dataLength = 0);
  void parseData(uint8_t* rxBuffer, uint8_t rxBufferCount);
  void setState(uint8_t state);
  bool syncSettings();
  void trace(const char* message);
  uint8_t setFailSafe(int16_t* target);
  int16_t convert(int channelValue);
  void onModelSwitch();
  void sendChannelsData();
  void clearQueue();

  //external data
  ::AfhdsPulsesData* data;
  ::processSensor_t processSensor;
  //missing ppm center!

  //local config
  Config_u cfg;
  ModuleVersion version;
  enum MODULE_POWER_SOURCE powerSource;
  //buffer where the channels are
  State operationState;
  uint16_t repeatCount;
  uint32_t idleCount;
  std::queue<request*> commandQueue;
};

}
#endif /* PULSES_AFHDS3_H_ */
