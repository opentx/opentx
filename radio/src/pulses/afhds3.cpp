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


#include "afhds3.h"
#include "../debug.h"
#include "../definitions.h"
#include <cstdio>
#include <queue>
#define FAILSAFE_HOLD 1
#define FAILSAFE_CUSTOM 2
#define MAX_RETRIES_AFHDS3 5
extern int32_t getChannelValue(uint8_t channel);
extern void processFlySkySensor(const uint8_t * packet, uint8_t type);

namespace afhds3 {

static const char* const moduleStateText[] = {
   "Not ready",
   "HW Error",
   "Binding",
   "Connecting",
   "Connected",
   "Standby",
   "Waiting for update",
   "Updating",
   "Updating RX",
   "Updating RX failed",
   "Testing"
   "Ready",
   "HW test"
};

static const char* const powerSourceText[] = {
   "Unknown",
   "Internal",
   "External"
};

static const COMMAND periodicRequestCommands[] = {
    COMMAND::MODULE_STATE,
    COMMAND::MODULE_POWER_STATUS,
    COMMAND::MODULE_GET_CONFIG
};


//Command to be send, with the synchronous implementation probably can be removed - dynamic memory allocation!!!
//Because instance of the class is union member we can not use is as member
std::queue<request*> commandQueue;

//Address used in transmitted frames - it constrains of target address and source address
const uint8_t FrameAddress = DeviceAddress::TRANSMITTER | (DeviceAddress::MODULE << 4);

//Static collection of afhds3 object instances by module
PulsesData* AFHDS3PulsesData[EXTERNAL_MODULE + 1] = { nullptr, nullptr };

//friends function that can access telemetry parsing method
void processTelemetryData(uint8_t module, uint8_t data, uint8_t* rxBuffer, uint8_t& rxBufferCount,  uint8_t maxSize) {
  if(AFHDS3PulsesData[module]) {
    AFHDS3PulsesData[module]->processTelemetryData(data, rxBuffer, rxBufferCount, maxSize);
  }
}

void PulsesData::getStatusString(char * buffer) const {
  strcpy(buffer, this->state <= ModuleState::STATE_READY ? moduleStateText[this->state]: "Unknown");
}

void PulsesData::getPowerStatus(char* buffer) const {
  strcpy(buffer, this->powerSource <= MODULE_POWER_SOURCE::EXTERNAL ? powerSourceText[this->powerSource] : "Unknown");
}


void PulsesData::processTelemetryData(uint8_t byte, uint8_t* rxBuffer, uint8_t& rxBufferCount, uint8_t maxSize) {
  if (rxBufferCount == 0 && byte != AfhdsSpecialChars::START) {
    TRACE("AFHDS3 [SKIP] %02X", byte);
    this->esc_state = 0;
    return;
  }

  if (byte == AfhdsSpecialChars::ESC) {
     this->esc_state = rxBufferCount;
     return;
  }

  if (rxBufferCount > 1 && byte == AfhdsSpecialChars::END) {
      rxBuffer[rxBufferCount++] = byte;
      parseData(rxBuffer, rxBufferCount);
      rxBufferCount = 0;
      return;
  }

  if (this->esc_state && byte == AfhdsSpecialChars::ESC_END) byte = AfhdsSpecialChars::END;
  else if (esc_state &&  byte == AfhdsSpecialChars::ESC_ESC) byte = AfhdsSpecialChars::ESC;
  //reset esc index
  this->esc_state = 0;

  if (rxBufferCount >= maxSize) {
    TRACE("AFHDS3 [BUFFER OVERFLOW]");
    rxBufferCount = 0;
  }
  rxBuffer[rxBufferCount++] = byte;
}

void PulsesData::setupFrame()
{
  //TRACE("%d state %d repeatCount %d", (int)operationState, this->data->state, repeatCount);
  if(operationState == State::AWAITING_RESPONSE) {
    if (repeatCount++ < MAX_RETRIES_AFHDS3)
      return; //re-send
    else {
      TRACE("AFHDS3 [NO RESP]");
      clearFrameData();
      clearQueue();
    }
  }
  else if(operationState == State::UNKNOWN){
    this->state = ModuleState::STATE_NOT_READY;
  }
  repeatCount = 0;
  if (this->state == ModuleState::STATE_NOT_READY) {
    TRACE("AFHDS3 [GET MODULE READY]");
    putFrame(COMMAND::MODULE_READY, FRAME_TYPE::REQUEST_GET_DATA);
    return;
  }

  //not allow more than 10 commands
  if(commandQueue.size() > 10) {
    clearQueue();
  }


  //check waiting commands
  if(!commandQueue.empty()) {
    request* r = commandQueue.front();
    commandQueue.pop();
    uint8_t frameIndexBackup = this->frame_index;
    if(r->frameNumber >= 0) {
      this->frame_index = r->frameNumber;
    }
    putFrame(r->command, r->frameType, r->payload, r->payloadSize);
    trace("AFHDS3 [CMD QUEUE] data", r->payload, r->payloadSize);
    if(r->frameNumber >= 0) {
      this->frame_index = frameIndexBackup;
    }
    delete r;
    return;
  }

  //config should be loaded already
  if(syncSettings()) return;

  //if module is ready but not started
  if(this->state == ModuleState::STATE_READY) {
    cmdCount = 0;
    repeatCount = 0;
    requestInfoAndRun(true);
    return;
  }

  if(getModuleMode(module_index) == ::ModuleSettingsMode::MODULE_MODE_BIND) {
    if(state != STATE_BINDING)
    {
      TRACE("AFHDS3 [BIND]");
      setConfigFromModel();
      putFrame(COMMAND::MODULE_SET_CONFIG, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, cfg.buffer, sizeof(cfg.buffer));
      requestedModuleMode = MODULE_MODE_E::BIND;
      addToQueue(COMMAND::MODULE_MODE, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, &requestedModuleMode, 1);
      return;
    }
  }
  else if(getModuleMode(module_index) == ::ModuleSettingsMode::MODULE_MODE_RANGECHECK) {
    if(cfg.config.runPower != RUN_POWER::RUN_POWER_FIRST) {
      TRACE("AFHDS3 [RANGE CHECK]");
      cfg.config.runPower = RUN_POWER::RUN_POWER_FIRST;
      uint8_t data[] = { 0x13, 0x20, 0x02, cfg.config.runPower, 0 };
      TRACE("AFHDS3 SET TX POWER %d", moduleData->afhds3.runPower);
      putFrame(COMMAND::SEND_COMMAND, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, data, sizeof(data));
      return;
    }
  }
  else if(getModuleMode(module_index) == ::ModuleSettingsMode::MODULE_MODE_NORMAL){ //exit bind
    if(state == STATE_BINDING) {
      TRACE("AFHDS3 [EXIT BIND]");
      requestedModuleMode = MODULE_MODE_E::RUN;
      putFrame(COMMAND::MODULE_MODE, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, &requestedModuleMode, 1);
      return;
    }
  }


  cmdCount++;
  bool every128 = (cmdCount & 0x7F) == 0x7F;
  bool every512 = (cmdCount & 0x1FF) == 0x1FF;
  bool syncDone = this->state == ModuleState::STATE_SYNC_DONE;
  bool syncActive = this->state == ModuleState::STATE_SYNC_RUNNING;

  if (every128 && !every512)
  {
    uint32_t commandIndex = ((cmdCount & 0x1FF)>> 7);
    putFrame(periodicRequestCommands[commandIndex], FRAME_TYPE::REQUEST_GET_DATA);
    TRACE("AFHDS3 [periodic request index %d] %02X", commandIndex, (uint8_t)periodicRequestCommands[commandIndex]);
  }
  else if (syncActive || syncDone)
  {
    if (every512) {
      if (syncActive)
      {
        //one-way state is not synchronized
        TRACE("AFHDS ONE WAY FAILSAFE");
        uint16_t failSafe[MAX_CHANNELS + 1] = { 0 };
        uint8_t channels = setFailSafe((int16_t*) (&failSafe[1]));
        failSafe[0] = (int16_t) ((channels << 8) | CHANNELS_DATA_MODE::FAIL_SAFE);
        putFrame(COMMAND::CHANNELS_FAILSAFE_DATA, FRAME_TYPE::REQUEST_SET_NO_RESP, (uint8_t*) failSafe, channels * 2 + 2);
      }
      else
      {
        TRACE("AFHDS TWO WAYS FAILSAFE");
        uint8_t failSafe[3 + MAX_CHANNELS * 2] = { 0x11, 0x60 };
        uint8_t channels = setFailSafe((int16_t*) (failSafe + 3));
        failSafe[2] = channels * 2;
        putFrame(COMMAND::SEND_COMMAND, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, failSafe, 3 + channels * 2);
      }
    }
    else
    {
      sendChannelsData();
    }
  }
  else {
    //default frame - request state
    putFrame(MODULE_STATE, FRAME_TYPE::REQUEST_GET_DATA);
  }
}

void PulsesData::init(uint8_t moduleIndex, bool resetFrameCount) {
  module_index = moduleIndex;
  AFHDS3PulsesData[module_index] = this;
  //clear local vars because it is member of union
  moduleData = &g_model.moduleData[module_index];
  operationState = State::UNKNOWN;
  state = ModuleState::STATE_NOT_READY;
  clearFrameData();
}

void PulsesData::clearFrameData() {
  TRACE("AFHDS3 clearFrameData");
  reset();
  clearQueue();
  repeatCount = 0;
  cmdCount = 0;
  this->frame_index = 1;
  this->timeout = 0;
  this->esc_state = 0;
}

void PulsesData::putBytes(uint8_t* data, int length) {
  for (int i = 0; i < length; i++) {
    uint8_t byte = data[i];
    this->crc += byte;
    if (END == byte) {
      sendByte(ESC);
      sendByte(ESC_END);
    }
    else if (ESC == byte) {
      sendByte(ESC);
      sendByte(ESC_ESC);
    }
    else {
      sendByte(byte);
    }
  }
}


void PulsesData::putFrame(COMMAND command, FRAME_TYPE frame, uint8_t* data, uint8_t dataLength){
  /////////
  //header
  /////////
  operationState = State::SENDING_COMMAND;
  reset();
  this->crc = 0;
  sendByte(START);
  uint8_t buffer[] = { FrameAddress, this->frame_index, frame, command };
  putBytes(buffer, 4);
  /////////
  //payload
  /////////
  if(dataLength > 0) putBytes(data, dataLength);
  /////////
  //footer
  /////////
  uint8_t crcValue = this->crc ^ 0xff;
  putBytes(&crcValue, 1);
  sendByte(END);
  this->frame_index++;

  switch (frame) {
  case FRAME_TYPE::REQUEST_GET_DATA:
  case FRAME_TYPE::REQUEST_SET_EXPECT_ACK:
  case FRAME_TYPE::REQUEST_SET_EXPECT_DATA:
    operationState = State::AWAITING_RESPONSE;
    break;
  default:
    operationState = State::IDLE;
  }

  flush();
}

void PulsesData::addAckToQueue(COMMAND command, uint8_t frameNumber) {
  request* r = new request(command, FRAME_TYPE::RESPONSE_ACK, nullptr, 0);
  r->frameNumber = frameNumber;
  commandQueue.push(r);
}

void PulsesData::addToQueue(COMMAND command, FRAME_TYPE frameType, uint8_t* data, uint8_t dataLength) {
  request* r = new request(command, frameType, data, dataLength);
  commandQueue.push(r);
}

void PulsesData::clearQueue() {
  while(!commandQueue.empty()) {
    request* r = commandQueue.front();
    delete r;
    commandQueue.pop();
  }
}

bool checkCRC(const uint8_t* data, uint8_t size)
{
  uint8_t crc = 0;
  //skip start byte
  for (uint8_t i = 1; i < size; i++) {
    crc += data[i];
  }
  return (crc ^ 0xff) == data[size];
}

bool containsData(enum FRAME_TYPE frameType) {
  return frameType == FRAME_TYPE::RESPONSE_DATA || frameType == FRAME_TYPE::REQUEST_SET_EXPECT_DATA ||
      frameType == FRAME_TYPE::REQUEST_SET_EXPECT_ACK || frameType == FRAME_TYPE::REQUEST_SET_EXPECT_DATA ||
      frameType == FRAME_TYPE::REQUEST_SET_NO_RESP;
}

void PulsesData::setState(uint8_t state) {
  if(state == this->state) return;
  uint8_t oldState = this->state;
  this->state = state;
  if(oldState == ModuleState::STATE_BINDING) {
    setModuleMode(module_index, ::ModuleSettingsMode::MODULE_MODE_NORMAL);
  }
}

void PulsesData::requestInfoAndRun(bool send) {
  if(!send) addToQueue(COMMAND::MODULE_VERSION, FRAME_TYPE::REQUEST_GET_DATA);
  addToQueue(COMMAND::MODULE_POWER_STATUS, FRAME_TYPE::REQUEST_GET_DATA);
  requestedModuleMode = MODULE_MODE_E::RUN;
  addToQueue(COMMAND::MODULE_MODE, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, &requestedModuleMode, 1);
  if(send) putFrame(COMMAND::MODULE_VERSION, FRAME_TYPE::REQUEST_GET_DATA);
}

void PulsesData::parseData(uint8_t* rxBuffer, uint8_t rxBufferCount) {

  if(!checkCRC(rxBuffer, rxBufferCount - 2)) {
    TRACE("AFHDS3 [INVALID CRC]");
    return;
  }
  AfhdsFrame* responseFrame = reinterpret_cast<AfhdsFrame*>(rxBuffer);
  //AfhdsFrame* requestFrame = reinterpret_cast<AfhdsFrame*>(this->pulses);
  //uint8_t oldState = this->state;
  if(containsData((enum FRAME_TYPE)responseFrame->frameType)) {
    switch(responseFrame->command)
    {
      case COMMAND::MODULE_READY:
        TRACE("AFHDS3 [MODULE_READY] %02X", responseFrame->value);
        if(responseFrame->value == MODULE_STATUS_READY) {
          setState(ModuleState::STATE_READY);
          requestInfoAndRun();
        }
        break;
      case COMMAND::MODULE_GET_CONFIG:
          std::memcpy((void*)cfg.buffer, &responseFrame->value, sizeof(cfg.buffer));
          TRACE("AFHDS3 [MODULE_GET_CONFIG] bpow %d rpow %d tele %d pwm/ppm %d ibus/sbus %d", cfg.config.bindPower, cfg.config.runPower, cfg.config.telemetry, cfg.config.pulseMode, cfg.config.serialMode);
          break;
      case COMMAND::MODULE_VERSION:
        std::memcpy((void*)&version, &responseFrame->value, sizeof(version));
        TRACE("AFHDS3 [MODULE_VERSION] Product %d, HW %d, BOOT %d, FW %d", version.productNumber, version.hardwereVersion, version.bootloaderVersion, version.firmwareVersion);
        break;
      case COMMAND::MODULE_POWER_STATUS:
        powerSource = (enum MODULE_POWER_SOURCE)responseFrame->value;
        TRACE("AFHDS3 [MODULE_POWER_STATUS], %d", powerSource);
        break;
      case COMMAND::MODULE_STATE:
        TRACE("AFHDS3 [MODULE_STATE] %02X", responseFrame->value);
        setState(responseFrame->value);
        break;
      case COMMAND::MODULE_MODE:
        TRACE("AFHDS3 [MODULE_MODE] %02X", responseFrame->value);
        if(responseFrame->value != CMD_RESULT::SUCCESS) {
            setState(ModuleState::STATE_NOT_READY);
        }
        else {
          if(requestedModuleMode == MODULE_MODE_E::RUN) {
            addToQueue(COMMAND::MODULE_GET_CONFIG, FRAME_TYPE::REQUEST_GET_DATA);
            addToQueue(COMMAND::MODULE_STATE, FRAME_TYPE::REQUEST_GET_DATA);
          }
          requestedModuleMode = MODULE_MODE_UNKNOWN;
        }
        break;
      case COMMAND::MODULE_SET_CONFIG:
        if(responseFrame->value != CMD_RESULT::SUCCESS) {
          setState(ModuleState::STATE_NOT_READY);
        }
        TRACE("AFHDS3 [MODULE_SET_CONFIG], %02X", responseFrame->value);
        break;
      case COMMAND::TELEMETRY_DATA:
      {
        uint8_t* telemetry = &responseFrame->value;

        if(telemetry[0] == 0x22) {
          telemetry++;
          while(telemetry < rxBuffer + rxBufferCount) {
            uint8_t length = telemetry[0];
            uint8_t id = telemetry[1];
            if(id == 0xFE) id = 0xF7;  //use new id because format is different
            if(length == 0 || telemetry + length > rxBuffer + rxBufferCount) break;
            if(length == 4) { //one byte value fill missing byte
              uint8_t data[] = { id, telemetry[2], telemetry[3], 0};
              ::processFlySkySensor(data, 0xAA);
            }
            if(length == 5) {
              if(id == 0xFA) telemetry[1] = 0xF8; //remap to afhds3 snr
              ::processFlySkySensor(telemetry + 1, 0xAA);
            }
            else if(length == 6 && id == FRM302_STATUS) {
              //convert to ibus
              uint16_t t = (uint16_t)(((int16_t)telemetry[3] *10) + 400);
              uint8_t dataTemp[] = { ++id, telemetry[2], (uint8_t)(t & 0xFF), (uint8_t)(t >> 8)};
              ::processFlySkySensor(dataTemp, 0xAA);
              uint8_t dataVoltage[] = { ++id, telemetry[2], telemetry[4], telemetry[5] };
              ::processFlySkySensor(dataVoltage, 0xAA);
            }
            else if(length == 7) ::processFlySkySensor(telemetry + 1, 0xAC);
            telemetry += length;
          }
        }
      }
      break;
      case COMMAND::COMMAND_RESULT:
      {
          //AfhdsFrameData* respData = responseFrame->GetData();
          //TRACE("COMMAND RESULT %02X result %d datalen %d", respData->CommandResult.command, respData->CommandResult.result, respData->CommandResult.respLen);
      }
      break;
    }
  }

  if(responseFrame->frameType == FRAME_TYPE::REQUEST_GET_DATA || responseFrame->frameType == FRAME_TYPE::REQUEST_SET_EXPECT_DATA)
  {
    TRACE("Command %02X NOT IMPLEMENTED!", responseFrame->command);
  }
  else if (responseFrame->frameType == FRAME_TYPE::REQUEST_SET_EXPECT_ACK) {
    //we need to respond now - it may break messaging context
    //if(!commandQueue.empty()) { //check if such request is not queued
      //request* r = commandQueue.front();
      //if(r->command == (enum COMMAND)responseFrame->command && r->frameType == FRAME_TYPE::RESPONSE_ACK) return;
    //}
    TRACE("SEND ACK cmd %02X type %02X", responseFrame->command, responseFrame->frameType);
    addAckToQueue((enum COMMAND)responseFrame->command, responseFrame->frameNumber);
    //not tested danger function
    //::sendExtModuleNow();
  }
  else if(responseFrame->frameType == FRAME_TYPE::RESPONSE_DATA || responseFrame->frameType == FRAME_TYPE::RESPONSE_ACK) {
    if(operationState == State::AWAITING_RESPONSE /* && requestFrame->command == responseFrame->command*/) {
      operationState = State::IDLE;
    }
  }
}


void PulsesData::trace(const char* message, uint8_t * payload, uint8_t payloadSize) {
  char buffer[256];
  char *pos = buffer;
  if(payload == nullptr) {
    payload = (uint8_t *) pulses;
    payloadSize = getSize();
  }


  for (int i = 0; i < payloadSize; i++) {
    pos += snprintf(pos, buffer + sizeof(buffer) - pos, "%02X ", payload[i]);
  }
  (*pos) = 0;
  TRACE("%s size = %d data %s", message, payloadSize, buffer);
}

inline bool isSbus(uint8_t mode) {
  return (mode & 1);
}
inline bool isPWM(uint8_t mode) {
  return mode < 2;
}

RUN_POWER PulsesData::getMaxRunPower()
{
  if(powerSource == MODULE_POWER_SOURCE::EXTERNAL) return RUN_POWER::PLUS_33dBm;
  return RUN_POWER::PLUS_20bBm;
}

RUN_POWER PulsesData::actualRunPower()
{
  uint8_t actualRfPower = cfg.config.runPower;
  if(getMaxRunPower() < actualRfPower)
    actualRfPower = getMaxRunPower();
  return (RUN_POWER)actualRfPower;
}
RUN_POWER PulsesData::getRunPower() {
  RUN_POWER targetPower = (RUN_POWER)moduleData->afhds3.runPower;
  if(getMaxRunPower() < targetPower)
    targetPower = getMaxRunPower();
  return targetPower;
}

bool PulsesData::syncSettings() {
  RUN_POWER targetPower = getRunPower();

  /*not sure if we need to prevent them in bind mode*/
  if (getModuleMode(module_index) != ::ModuleSettingsMode::MODULE_MODE_BIND && targetPower != cfg.config.runPower) {
    cfg.config.runPower = moduleData->afhds3.runPower;
    uint8_t data[] = { 0x13, 0x20, 0x02, moduleData->afhds3.runPower, 0 };
    TRACE("AFHDS3 SET TX POWER %d", moduleData->afhds3.runPower);
    putFrame(COMMAND::SEND_COMMAND, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, data, sizeof(data));
    return true;
  }

  //other settings only in 2 way mode (state must be synchronized)
  if(this->state != ModuleState::STATE_SYNC_DONE) return false;

  if(moduleData->afhds3.rxFreq() != cfg.config.pwmFreq) {
    cfg.config.pwmFreq = moduleData->afhds3.rxFreq();
    uint8_t data[] = {0x17, 0x70, 0x02, (uint8_t)(moduleData->afhds3.rxFreq() & 0xFF), (uint8_t)(moduleData->afhds3.rxFreq() >> 8)};
    TRACE("AFHDS3 SET RX FREQ");
    putFrame(COMMAND::SEND_COMMAND, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, data, sizeof(data));
    return true;
  }
  PULSE_MODE modelPulseMode = isPWM(moduleData->rfProtocol) ? PULSE_MODE::PWM_MODE: PULSE_MODE::PPM_MODE;
  if(modelPulseMode != cfg.config.pulseMode) {
    cfg.config.pulseMode = modelPulseMode;
    TRACE("AFHDS3 PWM/PPM %d", modelPulseMode);
    uint8_t data[] = {0x16, 0x70, 0x01, (uint8_t)(modelPulseMode)};
    putFrame(COMMAND::SEND_COMMAND, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, data, sizeof(data));
    return true;
  }

  SERIAL_MODE modelSerialMode = isSbus(moduleData->rfProtocol) ? SERIAL_MODE::SBUS_MODE : SERIAL_MODE::IBUS;
  if(modelSerialMode != cfg.config.serialMode) {
    cfg.config.serialMode = modelSerialMode;
    TRACE("AFHDS3 IBUS/SBUS %d", modelSerialMode);
    uint8_t data[] = {0x18, 0x70, 0x01, (uint8_t)(modelSerialMode)};
    putFrame(COMMAND::SEND_COMMAND, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, data, sizeof(data));
    return true;
  }

  if(moduleData->afhds3.failsafeTimeout != cfg.config.failSafeTimout) {
    moduleData->afhds3.failsafeTimeout = cfg.config.failSafeTimout;
    uint8_t data[] = {0x12,  0x60, 0x02, (uint8_t)(moduleData->afhds3.failsafeTimeout & 0xFF), (uint8_t)(moduleData->afhds3.failsafeTimeout >> 8) };
    putFrame(COMMAND::SEND_COMMAND, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, data, sizeof(data));
    TRACE("AFHDS3 TRACE FAILSAFE TMEOUT");
    return true;
  }

  return false;
}

void PulsesData::sendChannelsData() {
  uint8_t channels_start = moduleData->channelsStart;
  uint8_t channelsCount = 8 + moduleData->channelsCount;
  uint8_t channels_last = channels_start + channelsCount;

  uint8_t channels[2*((8 + moduleData->channelsCount) + 1)];
  channels[0] = 0x01;
  channels[1] = channelsCount;

  for(uint8_t channel = channels_start; channel < channels_last; channel++) {
    int16_t channelValue = convert(::getChannelValue(channel));
    *((int16_t*)(channels + (channel * 2) + 2)) = channelValue;
  }
  putFrame(COMMAND::CHANNELS_FAILSAFE_DATA, FRAME_TYPE::REQUEST_SET_NO_RESP, channels, sizeof(channels));
}

void PulsesData::stop() {
  TRACE("AFHDS3 STOP");
  //requestedModuleMode = MODULE_MODE_E::STANDBY;
  //putFrame(COMMAND::MODULE_MODE, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, &requestedModuleMode, 1);
  //init(module_index, true);
}

void PulsesData::setConfigFromModel() {
  cfg.config.bindPower = moduleData->afhds3.bindPower;
  cfg.config.runPower = getRunPower();
  cfg.config.emiStandard = EMI_STANDARD::FCC;
  cfg.config.telemetry = moduleData->afhds3.telemetry; //always use bidirectional mode
  cfg.config.pwmFreq = moduleData->afhds3.rxFreq();
  cfg.config.serialMode = isSbus(moduleData->rfProtocol) ? SERIAL_MODE::SBUS_MODE : SERIAL_MODE::IBUS;
  cfg.config.pulseMode = isPWM(moduleData->rfProtocol) ? PULSE_MODE::PWM_MODE: PULSE_MODE::PPM_MODE;
  cfg.config.channelCount = 8 + moduleData->channelsCount;
  cfg.config.failSafeTimout = moduleData->afhds3.failsafeTimeout;
  setFailSafe(cfg.config.failSafeMode);

}

inline int16_t PulsesData::convert(int channelValue) {
    //pulseValue = limit<uint16_t>(0, 988 + ((channelValue + 1024) / 2), 0xfff);
    //988 - 750 = 238
    //238 * 20 = 4760
    //2250 - 2012 = 238
    //238 * 20 = 4760
    // 988   ---- 2012
    //-10240 ---- 10240
    //-1024  ---- 1024
    return ::limit<int16_t>(FAILSAFE_MIN, channelValue*10, FAILSAFE_MAX);
}

uint8_t PulsesData::setFailSafe(int16_t* target) {
  int16_t pulseValue = 0;
  uint8_t channels_start = moduleData->channelsStart;
  uint8_t channels_last = channels_start + 8 + moduleData->channelsCount;;

  for (uint8_t channel = channels_start; channel < channels_last; channel++) {
     if (moduleData->failsafeMode == FAILSAFE_CUSTOM) pulseValue = convert(g_model.failsafeChannels[channel]);
     else if (moduleData->failsafeMode == FAILSAFE_HOLD) pulseValue = FAILSAFE_KEEP_LAST;
     else pulseValue = convert(::getChannelValue(channel));
     target[channel-channels_start] = pulseValue;
   }
  return (uint8_t)(channels_last - channels_start);
}


void PulsesData::onModelSwitch() {
  //uint8_t cmd = MODULE_MODE_E::STANDBY;
  //putFrame(COMMAND::MODULE_MODE, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, &cmd, 1);
  //setModelData();
}

}
