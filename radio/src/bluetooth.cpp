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

#include "opentx.h"

#if defined(PCBX7) || defined(PCBHORUS)
#define BLUETOOTH_COMMAND_NAME         "AT+NAME"
#define BLUETOOTH_ANSWER_NAME          "OK+"
#define BLUETOOTH_COMMAND_BAUD_115200  "AT+BAUD115200"
#else
#define BLUETOOTH_COMMAND_NAME         "TTM:REN-"
#define BLUETOOTH_ANSWER_NAME          "TTM:REN"
#define BLUETOOTH_COMMAND_BAUD_115200  "TTM:BPS-115200"
#endif

#define BLUETOOTH_PACKET_SIZE          14
#define BLUETOOTH_LINE_LENGTH          32

extern Fifo<uint8_t, 64> btTxFifo;
extern Fifo<uint8_t, 64> btRxFifo;

volatile uint8_t bluetoothState;
char bluetoothFriend[LEN_BLUETOOTH_FRIEND+1];
uint8_t bluetoothBuffer[BLUETOOTH_LINE_LENGTH+1];
uint8_t bluetoothBufferIndex = 0;
tmr10ms_t bluetoothWakeupTime = 0;

void bluetoothWrite(const uint8_t * data, uint8_t length)
{
  for (int i=0; i<length; i++) {
    TRACE_NOCRLF("%02X ", data[i]);
    btTxFifo.push(data[i]);
  }
}

void bluetoothWriteString(const char * str)
{
  TRACE("BT> %s", str);
  while (*str != 0) {
    btTxFifo.push(*str++);
  }
}

char * bluetoothReadline()
{
  uint8_t byte;

  while (1) {
    if (!btRxFifo.pop(byte)) {
#if defined(PCBX9E)      // X9E BT module can get unresponsive
      TRACE("NO RESPONSE FROM BT MODULE");
#endif
      return NULL;
    }
    TRACE_NOCRLF("%02X ", byte);
    if (byte == '\n') {
      bluetoothBuffer[bluetoothBufferIndex] = '\0';
      bluetoothBufferIndex = 0;
      TRACE("BT< %s", bluetoothBuffer);
      return (char *)bluetoothBuffer;
    }
    else if (byte != '\r') {
      bluetoothBuffer[bluetoothBufferIndex++] = byte;
      bluetoothBufferIndex &= (BLUETOOTH_LINE_LENGTH-1);
    }
  }
}

void bluetoothProcessTrainerFrame(const uint8_t * bluetoothBuffer)
{
  TRACE("");

  for (uint8_t channel=0, i=1; channel<8; channel+=2, i+=3) {
    // +-500 != 512, but close enough.
    ppmInput[channel] = bluetoothBuffer[i] + ((bluetoothBuffer[i+1] & 0xf0) << 4) - 1500;
    ppmInput[channel+1] = ((bluetoothBuffer[i+1] & 0x0f) << 4) + ((bluetoothBuffer[i+2] & 0xf0) >> 4) + ((bluetoothBuffer[i+2] & 0x0f) << 8) - 1500;
  }

  ppmInputValidityTimer = PPM_IN_VALID_TIMEOUT;
}

void bluetoothProcessTrainerByte(uint8_t data)
{
  static uint8_t dataState = STATE_DATA_IDLE;

  switch (dataState) {
    case STATE_DATA_START:
      if (data == START_STOP) {
        dataState = STATE_DATA_IN_FRAME ;
        bluetoothBufferIndex = 0;
      }
      else {
        if (bluetoothBufferIndex < BLUETOOTH_PACKET_SIZE) {
          bluetoothBuffer[bluetoothBufferIndex++] = data;
        }
        if (data == '\n' && !strncmp((char *)bluetoothBuffer, "DisConnected", 12)) {
          bluetoothState = BLUETOOTH_STATE_IDLE;
          bluetoothBufferIndex = 0;
        }
      }
      break;

    case STATE_DATA_IN_FRAME:
      if (data == BYTESTUFF) {
        dataState = STATE_DATA_XOR; // XOR next byte
      }
      else if (data == START_STOP) {
        dataState = STATE_DATA_IN_FRAME ;
        bluetoothBufferIndex = 0;
      }
      else if (bluetoothBufferIndex < BLUETOOTH_PACKET_SIZE) {
        bluetoothBuffer[bluetoothBufferIndex++] = data;
      }
      break;

    case STATE_DATA_XOR:
      if (telemetryRxBufferCount < BLUETOOTH_PACKET_SIZE) {
        bluetoothBuffer[bluetoothBufferIndex++] = data ^ STUFF_MASK;
      }
      dataState = STATE_DATA_IN_FRAME;
      break;

    case STATE_DATA_IDLE:
      if (data == START_STOP) {
        bluetoothBufferIndex = 0;
        dataState = STATE_DATA_START;
      }
      break;
  }

  if (bluetoothBufferIndex >= BLUETOOTH_PACKET_SIZE) {
    uint8_t crc = 0x00;
    for (int i=0; i<13; i++) {
      crc ^= bluetoothBuffer[i];
    }
    if (crc == bluetoothBuffer[13]) {
      if (bluetoothBuffer[0] == 0x80) {
        bluetoothProcessTrainerFrame(bluetoothBuffer);
      }
    }
    dataState = STATE_DATA_IDLE;
  }
}

uint8_t bluetoothCrc;
void bluetoothPushByte(uint8_t byte)
{
  bluetoothCrc ^= byte;
  if (byte == START_STOP || byte == BYTESTUFF) {
    bluetoothBuffer[bluetoothBufferIndex++] = 0x7d;
    byte ^= STUFF_MASK;
  }
  bluetoothBuffer[bluetoothBufferIndex++] = byte;
}

void bluetoothSendTrainer()
{
  int16_t PPM_range = g_model.extendedLimits ? 640*2 : 512*2;

  int firstCh = g_model.moduleData[TRAINER_MODULE].channelsStart;
  int lastCh = firstCh + 8;

  uint8_t * cur = bluetoothBuffer;
  bluetoothBufferIndex = 0;
  bluetoothCrc = 0x00;

  bluetoothBuffer[bluetoothBufferIndex++] = START_STOP; // start byte
  bluetoothPushByte(0x80); // trainer frame type?
  for (int channel=0; channel<lastCh; channel+=2, cur+=3) {
    uint16_t channelValue1 = PPM_CH_CENTER(channel) + limit((int16_t)-PPM_range, channelOutputs[channel], (int16_t)PPM_range) / 2;
    uint16_t channelValue2 = PPM_CH_CENTER(channel+1) + limit((int16_t)-PPM_range, channelOutputs[channel+1], (int16_t)PPM_range) / 2;
    bluetoothPushByte(channelValue1 & 0x00ff);
    bluetoothPushByte(((channelValue1 & 0x0f00) >> 4) + ((channelValue2 & 0x00f0) >> 4));
    bluetoothPushByte(((channelValue2 & 0x000f) << 4) + ((channelValue2 & 0x0f00) >> 8));
  }
  bluetoothBuffer[bluetoothBufferIndex++] = bluetoothCrc;
  bluetoothBuffer[bluetoothBufferIndex++] = START_STOP; // end byte

  bluetoothWrite(bluetoothBuffer, bluetoothBufferIndex);
  bluetoothBufferIndex = 0;
}

void bluetoothForwardTelemetry(uint8_t data)
{
  bluetoothBuffer[bluetoothBufferIndex++] = data;
  if (bluetoothBufferIndex == BLUETOOTH_LINE_LENGTH || (data == START_STOP && bluetoothBufferIndex > 2*FRSKY_SPORT_PACKET_SIZE)) {
    bluetoothWrite(bluetoothBuffer, bluetoothBufferIndex);
    bluetoothBufferIndex = 0;
  }
}

void bluetoothReceiveTrainer()
{
  uint8_t byte;

  while (1) {
    if (!btRxFifo.pop(byte)) {
      return;
    }

    TRACE_NOCRLF("%02X ", byte);

    bluetoothProcessTrainerByte(byte);
  }
}

#if defined(PCBX9E) && defined(DEBUG)
void bluetoothWakeup(void)
{
  if (!g_eeGeneral.bluetoothMode) {
    if (bluetoothState != BLUETOOTH_INIT) {
      bluetoothDone();
      bluetoothState = BLUETOOTH_INIT;
    }
  }
  else {
    static tmr10ms_t waitEnd = 0;
    if (bluetoothState != BLUETOOTH_STATE_IDLE) {

      if (bluetoothState == BLUETOOTH_INIT) {
        bluetoothInit(BLUETOOTH_DEFAULT_BAUDRATE);
        char command[32];
        char * cur = strAppend(command, BLUETOOTH_COMMAND_NAME);
        uint8_t len = ZLEN(g_eeGeneral.bluetoothName);
        if (len > 0) {
          for (int i = 0; i < len; i++) {
            *cur++ = idx2char(g_eeGeneral.bluetoothName[i]);
          }
        }
        else {
          cur = strAppend(cur, "Taranis-X9E");
        }
        strAppend(cur, "\r\n");
        bluetoothWriteString(command);
        bluetoothState = BLUETOOTH_WAIT_TTM;
        waitEnd = get_tmr10ms() + 25; // 250ms
      }
      else if (bluetoothState == BLUETOOTH_WAIT_TTM) {
        if (get_tmr10ms() > waitEnd) {
            char * line = bluetoothReadline();
            if (strncmp(line, "OK+", 3)) {
            bluetoothState = BLUETOOTH_STATE_IDLE;
            }
            else {
              bluetoothInit(BLUETOOTH_FACTORY_BAUDRATE);
              const char btMessage[] = "TTM:BPS-115200";
              bluetoothWriteString(btMessage);
              bluetoothState = BLUETOOTH_WAIT_BAUDRATE_CHANGE;
              waitEnd = get_tmr10ms() + 250; // 2.5s
            }
          }
        }
      else if (bluetoothState == BLUETOOTH_WAIT_BAUDRATE_CHANGE) {
        if (get_tmr10ms() > waitEnd) {
          bluetoothState = BLUETOOTH_INIT;
        }
      }
    } else if (IS_BLUETOOTH_TRAINER()){
      bluetoothState = BLUETOOTH_STATE_CONNECTED;
      bluetoothWriteWakeup();
      bluetoothSendTrainer();
    }
  }
}
#else //PCBX9E
void bluetoothWakeup()
{
  tmr10ms_t now = get_tmr10ms();

  if (now < bluetoothWakeupTime)
    return;

  if (g_eeGeneral.bluetoothMode == BLUETOOTH_OFF || !IS_BLUETOOTH_TRAINER()) {
    if (bluetoothState != BLUETOOTH_STATE_OFF) {
      bluetoothDone();
      bluetoothState = BLUETOOTH_STATE_OFF;
    }
    bluetoothWakeupTime = now + 10;
  }
  else {
    bluetoothWakeupTime = now + 2;
    bluetoothWriteWakeup();
    if (bluetoothState == BLUETOOTH_STATE_CONNECTED) {
      if (g_model.trainerMode == TRAINER_MODE_MASTER_BLUETOOTH) {
        bluetoothReceiveTrainer();
      }
      else {
        bluetoothSendTrainer();
        char * line = bluetoothReadline();
        if (!strcmp(line, "ERROR")) {
          bluetoothState = BLUETOOTH_STATE_IDLE;
        }
      }
    }
    else {
      char * line = bluetoothReadline();
      if (bluetoothState == BLUETOOTH_STATE_OFF && IS_BLUETOOTH_TRAINER()) {
        char command[32];
        char * cur = strAppend(command, BLUETOOTH_COMMAND_NAME);
        uint8_t len = ZLEN(g_eeGeneral.bluetoothName);
        if (len > 0) {
          for (int i = 0; i < len; i++) {
            *cur++ = idx2char(g_eeGeneral.bluetoothName[i]);
          }
        }
        else {
#if defined(PCBHORUS)
          cur = strAppend(cur, "Horus");
#else
          cur = strAppend(cur, "Taranis");
#endif
        }
        strAppend(cur, "\r\n");
        bluetoothWriteString(command);
        bluetoothState = BLUETOOTH_STATE_NAME_SENT;
      }
      else if (bluetoothState == BLUETOOTH_STATE_NAME_SENT && !strncmp(line, "OK+", 3)) {
        bluetoothWriteString("AT+TXPW3\r\n");
        bluetoothState = BLUETOOTH_STATE_POWER_SENT;
      }
      else if (bluetoothState == BLUETOOTH_STATE_POWER_SENT && (!strncmp(line, "Central:", 8) || !strncmp(line, "Peripheral:", 11))) {
        if (g_model.trainerMode == TRAINER_MODE_MASTER_BLUETOOTH)
          bluetoothWriteString("AT+ROLE1\r\n");
        else
          bluetoothWriteString("AT+ROLE0\r\n");
        bluetoothState = BLUETOOTH_STATE_ROLE_SENT;
      }
      else if (bluetoothState == BLUETOOTH_STATE_ROLE_SENT && (!strncmp(line, "Central:", 8) || !strncmp(line, "Peripheral:", 11))) {
        bluetoothState = BLUETOOTH_STATE_IDLE;
      }
      else if (bluetoothState == BLUETOOTH_STATE_DISCOVER_REQUESTED) {
        bluetoothWriteString("AT+DISC?\r\n");
        bluetoothState = BLUETOOTH_STATE_DISCOVER_SENT;
      }
      else if (bluetoothState == BLUETOOTH_STATE_DISCOVER_SENT && !strcmp(line, "OK+DISCS")) {
        bluetoothState = BLUETOOTH_STATE_DISCOVER_START;
      }
      else if (bluetoothState == BLUETOOTH_STATE_DISCOVER_START && !strncmp(line, "OK+DISC:", 8)) {
        TRACE("STOP");
        strcpy(bluetoothFriend, &line[8]); // TODO quick & dirty
      }
      else if (bluetoothState == BLUETOOTH_STATE_DISCOVER_START && !strcmp(line, "OK+DISCE")) {
        bluetoothState = BLUETOOTH_STATE_DISCOVER_END;
      }
      else if (bluetoothState == BLUETOOTH_STATE_BIND_REQUESTED) {
        char command[32];
        strAppend(strAppend(strAppend(command, "AT+CON"), bluetoothFriend), "\r\n");
        bluetoothWriteString(command);
        bluetoothState = BLUETOOTH_STATE_BIND_SENT;
      }
      else if ((bluetoothState == BLUETOOTH_STATE_IDLE || bluetoothState == BLUETOOTH_STATE_BIND_SENT) && !strncmp(line, "Connected:", 10)) {
        strcpy(bluetoothFriend, &line[10]); // TODO quick & dirty
        bluetoothState = BLUETOOTH_STATE_CONNECTED;
        if (g_model.trainerMode == TRAINER_MODE_SLAVE_BLUETOOTH) {
          bluetoothWakeupTime += 500; // it seems a 5s delay is needed before sending the 1st frame
        }
      }
    }
  }
}
#endif
