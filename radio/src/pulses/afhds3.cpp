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

#define FAILSAFE_HOLD 1
#define FAILSAFE_CUSTOM 2
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

const char* afhds3::getState() {
  if(data->state <= ModuleState::STATE_READY) return moduleStateText[data->state];
  return "Unknown";
}

void afhds3::putByte(uint8_t byte) {
  this->putBytes(&byte, 1);
}

void afhds3::putBytes(uint8_t* data, int length) {
  for (int i = 0; i < length; i++) {
    uint8_t byte = data[i];
    this->data->crc += byte;
    if (END == byte) {
      *this->data->ptr++ = ESC;
      *this->data->ptr++ = ESC_END;
    }
    else if (ESC == byte) {
      *this->data->ptr++ = ESC;
      *this->data->ptr++ = ESC_ESC;
    }
    else {
      *this->data->ptr++ = byte;
    }
  }
}

void afhds3::putHeader(COMMAND command, FRAME_TYPE frame) {
  operationState = State::SENDING_COMMAND;
  data->ptr = this->data->pulses;
  data->crc = 0;
  *data->ptr++ = END;
  uint8_t buffer[] = { FrameAddress, data->frame_index, frame, command};
  putBytes(buffer, 4);
}


void afhds3::putFooter() {
  putByte(data->crc ^ 0xff);
  *data->ptr++ = END;
  data->frame_index++;

  switch((FRAME_TYPE)data->pulses[3])
  {
    case FRAME_TYPE::REQUEST_GET_DATA:
    case FRAME_TYPE::REQUEST_SET_EXPECT_ACK:
    case FRAME_TYPE::REQUEST_SET_EXPECT_DATA:
      operationState = State::AWAITING_RESPONSE;
      break;
    default:
      operationState = State::IDLE;
  }
}

void afhds3::putFrame(COMMAND command, FRAME_TYPE frame, uint8_t* data, uint8_t dataLength){
  putHeader(command, frame);
  if(dataLength > 0) putBytes(data, dataLength);
  putFooter();
}

void afhds3::addAckToQueue(COMMAND command, uint8_t frameNumber) {
  request* r = new request(command, FRAME_TYPE::RESPONSE_ACK, nullptr, 0);
  r->frameNumber = frameNumber;
  commandQueue.push(r);
}

void afhds3::addToQueue(COMMAND command, FRAME_TYPE frameType, uint8_t* data, uint8_t dataLength) {

  request* r = new request(command, frameType, data, dataLength);
  commandQueue.push(r);
}

void afhds3::clearQueue() {
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

void afhds3::setState(uint8_t state) {
  if(state == data->state) return;
  uint8_t oldState = data->state;
  data->state = state;
  if(oldState == ModuleState::STATE_BINDING) {
    if(operationCallback != nullptr) {
      operationCallback(false);
      operationCallback = nullptr;
    }
  }
}

void afhds3::parseData(uint8_t* rxBuffer, uint8_t rxBufferCount) {

  if(!checkCRC(rxBuffer, rxBufferCount - 2)) {
    TRACE("AFHDS3 [INVALID CRC]");
    return;
  }
  AfhdsFrame* responseFrame = reinterpret_cast<AfhdsFrame*>(rxBuffer);
  AfhdsFrame* requestFrame = reinterpret_cast<AfhdsFrame*>(data->pulses);
  uint8_t oldState = data->state;
  if(containsData((enum FRAME_TYPE)responseFrame->frameType)) {
    switch(responseFrame->command)
    {
      case COMMAND::MODULE_READY:
        TRACE("AFHDS3 [MODULE_READY] %02X", responseFrame->value);
        if(responseFrame->value == MODULE_STATUS_READY) {
          setState(ModuleState::STATE_READY);
          requestedModuleMode = MODULE_MODE_E::RUN;
          addToQueue(COMMAND::MODULE_MODE, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, &requestedModuleMode, 1);
        }
        break;
      case COMMAND::MODULE_GET_CONFIG:
          std::memcpy((void*)cfg.buffer, &responseFrame->value, sizeof(cfg.buffer));
          TRACE("AFHDS3 [MODULE_GET_CONFIG] telemetry %d", cfg.config.telemetry);
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
            addToQueue(COMMAND::MODULE_VERSION, FRAME_TYPE::REQUEST_GET_DATA);
            addToQueue(COMMAND::MODULE_POWER_STATUS, FRAME_TYPE::REQUEST_GET_DATA);
            addToQueue(COMMAND::MODULE_GET_CONFIG, FRAME_TYPE::REQUEST_GET_DATA);
            addToQueue(COMMAND::MODULE_STATE, FRAME_TYPE::REQUEST_GET_DATA);
          }
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
              processSensor(data, 0xAA);
            }
            if(length == 5) {
              if(id == 0xFA) telemetry[1] = 0xF8; //remap to afhds3 snr
              processSensor(telemetry + 1, 0xAA);
            }
            else if(length == 6 && id == FRM302_STATUS) {
              //convert to ibus
              uint16_t t = (uint16_t)(((int16_t)telemetry[3] *10) + 400);
              uint8_t dataTemp[] = { ++id, telemetry[2], (uint8_t)(t & 0xFF), (uint8_t)(t >> 8)};
              processSensor(dataTemp, 0xAA);
              uint8_t dataVoltage[] = { ++id, telemetry[2], telemetry[4], telemetry[5] };
              processSensor(dataVoltage, 0xAA);
            }
            else if(length == 7) processSensor(telemetry + 1, 0xAC);
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

void afhds3::onDataReceived(uint8_t byte, uint8_t* rxBuffer, uint8_t& rxBufferCount, uint8_t maxSize) {
  if (rxBufferCount == 0 && byte != AfhdsSpecialChars::START) {
    TRACE("AFHDS3 [SKIP] %02X", byte);
    data->esc_state = 0;
    return;
  }

  if (byte == AfhdsSpecialChars::ESC) {
     data->esc_state = rxBufferCount;
     return;
  }

  if (rxBufferCount > 1 && byte == AfhdsSpecialChars::END) {
      rxBuffer[rxBufferCount++] = byte;
      parseData(rxBuffer, rxBufferCount);
      rxBufferCount = 0;
      return;
  }

  if (data->esc_state && byte == AfhdsSpecialChars::ESC_END) byte = AfhdsSpecialChars::END;
  else if (data->esc_state &&  byte == AfhdsSpecialChars::ESC_ESC) byte = AfhdsSpecialChars::ESC;
  //reset esc index
  data->esc_state = 0;

  if (rxBufferCount >= maxSize) {
    TRACE("AFHDS3 [BUFFER OVERFLOW]");
    rxBufferCount = 0;
  }
  rxBuffer[rxBufferCount++] = byte;
}
void afhds3::trace(const char* message) {
  char buffer[256];
  char *pos = buffer;
  for (int i = 0; i < data->ptr - data->pulses; i++) {
    pos += std::snprintf(pos, buffer + sizeof(buffer) - pos, "%02X ", data->pulses[i]);
  }
  (*pos) = 0;
  TRACE("%s size = %d data %s", message, data->ptr - data->pulses, buffer);
}
void afhds3::setupFrame() {
  //TRACE("%d state %d repeatCount %d", (int)operationState, this->data->state, repeatCount);
  if(operationState == State::AWAITING_RESPONSE) {
    if(repeatCount++ < 5) return; //re-send
    else init(false);
  }
  else if(operationState == State::UNKNOWN){
    data->state = ModuleState::STATE_NOT_READY;
  }
  data->ptr = data->pulses;
  repeatCount = 0;
  if (operationState == State::UNKNOWN || data->state == ModuleState::STATE_NOT_READY) {
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
    uint8_t frameIndexBackup = data->frame_index;
    if(r->frameNumber >= 0) {
      data->frame_index = r->frameNumber;
    }
    putFrame(r->command, r->frameType, r->payload, r->payloadSize);
    trace("AFHDS3 [CMD QUEUE] data");
    if(r->frameNumber >= 0) {
      data->frame_index = frameIndexBackup;
    }
    delete r;
    return;
  }

  //config should be loaded already
  if(syncSettings()) return;

  switch(data->state) {
    case ModuleState::STATE_READY:
        init();
        break;
    case ModuleState::STATE_HW_ERROR:
    case ModuleState::STATE_BINDING:
    case ModuleState::STATE_UPDATING_RX:
    case ModuleState::STATE_UPDATING_WAIT:
    case ModuleState::STATE_UPDATING_MOD:
    case ModuleState::STATE_SYNC_RUNNING:
    case ModuleState::STATE_HW_TEST:
        if(idleCount++ % 100 == 0) putFrame(COMMAND::MODULE_STATE, FRAME_TYPE::REQUEST_GET_DATA);
        break;
     case ModuleState::STATE_SYNC_DONE:
       if(idleCount++ % 800 == 0) {
         TRACE("AFHDS FAILSAFE");
         uint8_t failSafe[3+MAX_CHANNELS*2] = {0x11, 0x60 };
         uint8_t channels = setFailSafe((int16_t*)(failSafe + 3));
         failSafe[2] = channels *2;
         putFrame(COMMAND::SEND_COMMAND, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, failSafe, 3 + channels*2);
         //trace("AFHDS3 [AFHDS3 SET FAILSAFE] data");
       }
       else sendChannelsData();
      break;
  }
}

bool afhds3::syncSettings() {
  if (moduleData->afhds3.runPower != cfg.config.runPower) {
    cfg.config.runPower = moduleData->afhds3.runPower;
    uint8_t data[] = { 0x13, 0x20, 0x02, moduleData->afhds3.runPower, 0 };
    TRACE("AFHDS3 SET TX POWER %d", moduleData->afhds3.runPower);
    putFrame(COMMAND::SEND_COMMAND, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, data, sizeof(data));
    return true;
  }
  if(moduleData->afhds3.rxFreq() != cfg.config.pwmFreq) {
    cfg.config.pwmFreq = moduleData->afhds3.rxFreq();
    uint8_t data[] = {0x17, 0x70, 0x02, (uint8_t)(moduleData->afhds3.rxFreq() & 0xFF), (uint8_t)(moduleData->afhds3.rxFreq() >> 8)};
    TRACE("AFHDS3 SET RX FREQ");
    putFrame(COMMAND::SEND_COMMAND, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, data, sizeof(data));
    return true;
  }
  PULSE_MODE modelPulseMode = moduleData->afhds3.isPWM() ? PULSE_MODE::PWM: PULSE_MODE::PPM_MODE;
  if(modelPulseMode != cfg.config.pulseMode) {
    cfg.config.pulseMode = modelPulseMode;
    TRACE("AFHDS3 PWM/PPM %d", modelPulseMode);
    uint8_t data[] = {0x16, 0x70, 0x01, (uint8_t)(modelPulseMode)};
    putFrame(COMMAND::SEND_COMMAND, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, data, sizeof(data));
    return true;
  }

  SERIAL_MODE modelSerialMode = moduleData->afhds3.isSbus() ? SERIAL_MODE::SBUS_MODE : SERIAL_MODE::IBUS;
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

void afhds3::sendChannelsData() {
  uint8_t channels_start = moduleData->channelsStart;
  uint8_t channelsCount = 8 + moduleData->channelsCount;
  uint8_t channels_last = channels_start + channelsCount;

  uint8_t channels[2*((8 + moduleData->channelsCount) + 1)];
  channels[0] = 0x01;
  channels[1] = channelsCount;

  for(uint8_t channel = channels_start; channel < channels_last; channel++) {
    int16_t channelValue = convert(getChannelValue(channel));
    *((int16_t*)(channels + (channel * 2) + 2)) = channelValue;
  }
  putFrame(COMMAND::CHANNELS_FAILSAFE_DATA, FRAME_TYPE::REQUEST_SET_NO_RESP, channels, sizeof(channels));
}

void afhds3::beginBind(::asyncOperationCallback_t callback) {
  operationCallback = callback;
  TRACE("AFHDS3 [BIND]");
  setModelSettingsFromModule();
  addToQueue(COMMAND::MODULE_SET_CONFIG, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, cfg.buffer, sizeof(cfg.buffer));
  requestedModuleMode = MODULE_MODE_E::BIND;
  addToQueue(COMMAND::MODULE_MODE, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, &requestedModuleMode, 1);
}

void afhds3::beginRangeTest(::asyncOperationCallback_t callback) {
  TRACE("AFHDS3 [RANGE CHECK] NOT IMPLEMENTED");
}

void afhds3::cancelOperations() {
  if(operationCallback!=nullptr) operationCallback(false);
  init(false);
}
void afhds3::stop() {
  TRACE("AFHDS3 STOP");
  requestedModuleMode = MODULE_MODE_E::STANDBY;
  putFrame(COMMAND::MODULE_MODE, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, &requestedModuleMode, 1);
  init(true);
}


void afhds3::setModuleSettingsToDefault() {
  moduleData->afhds3.bindPower = BIND_POWER::MIN_0dbm;
  moduleData->afhds3.runPower = RUN_POWER::PLUS_15bBm;
  moduleData->afhds3.emi = EMI_STANDARD::FCC;
  moduleData->afhds3.telemetry = TELEMETRY::TELEMETRY_ENABLED;
  moduleData->afhds3.rx_freq[0] = 50;
  moduleData->afhds3.rx_freq[1] = 0;
  moduleData->afhds3.failsafeTimeout = 1000;
  moduleData->channelsCount = 14 - 8;
  moduleData->failsafeMode = FAILSAFE_HOLD;
  //" PWM+i"" PWM+s"" PPM+i"" PPM+s"
  moduleData->afhds3.mode = 0;
  for (uint8_t channel = 0; channel < MAX_OUTPUT_CHANNELS; channel++) {
    failsafeChannels[channel] = 0;
  }
}
int16_t afhds3::convert(int channelValue) {
  //pulseValue = limit<uint16_t>(0, 988 + ((channelValue + 1024) / 2), 0xfff);
  //988 - 750 = 238
  //238 * 20 = 4760
  //2250 - 2012 = 238
  //238 * 20 = 4760
  // 988   ---- 2012
  //-10240 ---- 10240
  //-1024  ---- 1024
  return limit<int16_t>(FAILSAFE_MIN, channelValue*10, FAILSAFE_MAX);
}
void afhds3::setModelSettingsFromModule() {
  cfg.config.bindPower = moduleData->afhds3.bindPower;
  cfg.config.runPower = moduleData->afhds3.runPower;
  cfg.config.emiStandard = moduleData->afhds3.emi;
  cfg.config.telemetry = 1; //moduleData->afhds3.telemetry; always use bidirectional mode
  cfg.config.pwmFreq = moduleData->afhds3.rxFreq();
  cfg.config.serialMode = moduleData->afhds3.isSbus() ? SERIAL_MODE::SBUS_MODE : SERIAL_MODE::IBUS;
  cfg.config.pulseMode = moduleData->afhds3.isPWM() ? PULSE_MODE::PWM: PULSE_MODE::PPM_MODE;
  cfg.config.channelCount = 8 + moduleData->channelsCount;
  cfg.config.failSafeTimout = moduleData->afhds3.failsafeTimeout;
  setFailSafe(cfg.config.failSafeMode);

}
uint8_t afhds3::setFailSafe(int16_t* target) {
  int16_t pulseValue = 0;
  uint8_t channels_start = moduleData->channelsStart;
  uint8_t channels_last = channels_start + 8 + moduleData->channelsCount;;

  for (uint8_t channel = channels_start; channel < channels_last; channel++) {
     if (moduleData->failsafeMode == FAILSAFE_CUSTOM) pulseValue = convert(failsafeChannels[channel]);
     else if (moduleData->failsafeMode == FAILSAFE_HOLD) pulseValue = FAILSAFE_KEEP_LAST;
     else pulseValue = convert(getChannelValue(channel));
     target[channel-channels_start] = pulseValue;
   }
  return (uint8_t)(channels_last - channels_start);
}


void afhds3::onModelSwitch() {
  //uint8_t cmd = MODULE_MODE_E::STANDBY;
  //putFrame(COMMAND::MODULE_MODE, FRAME_TYPE::REQUEST_SET_EXPECT_DATA, &cmd, 1);
  //setModelData();
}

void afhds3::init(bool resetFrameCount) {
  TRACE("AFHDS3 RESET");
  clearQueue();
  repeatCount = 0;
  idleCount = 0;
  this->data->state = ModuleState::STATE_NOT_READY;
  this->data->frame_index = 1;
  this->data->timeout = 0;
  this->data->esc_state = 0;
}

}
