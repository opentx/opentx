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

#if defined(PCBHORUS) || defined(PCBX7) || defined(PCBXLITE) || defined(USEHORUSBT)
#define BLUETOOTH_COMMAND_NAME         "AT+NAME"
#define BLUETOOTH_ANSWER_NAME          "OK+"
#define BLUETOOTH_COMMAND_BAUD_115200  "AT+BAUD115200"
#else
#define BLUETOOTH_COMMAND_NAME         "TTM:REN-"
#define BLUETOOTH_ANSWER_NAME          "TTM:REN"
#define BLUETOOTH_COMMAND_BAUD_115200  "TTM:BPS-115200"
#endif

extern Fifo<uint8_t, 64> btTxFifo;
extern Fifo<uint8_t, 128> btRxFifo;

Bluetooth bluetooth;

void Bluetooth::write(const uint8_t * data, uint8_t length)
{
  TRACE_NOCRLF("BT>");
  for (int i=0; i<length; i++) {
    TRACE_NOCRLF(" %02X", data[i]);
    btTxFifo.push(data[i]);
  }
  TRACE_NOCRLF("\r\n");
  bluetoothWriteWakeup();
}

void Bluetooth::writeString(const char * str)
{
  TRACE("BT> %s", str);
  while (*str != 0) {
    btTxFifo.push(*str++);
  }
  bluetoothWriteWakeup();
}

char * Bluetooth::readline(bool error_reset)
{
  uint8_t byte;

  while (1) {
    if (!btRxFifo.pop(byte)) {
#if defined(PCBX9E) && !defined(USEHORUSBT)     // X9E BT module can get unresponsive
      TRACE("NO RESPONSE FROM BT MODULE");
#endif
      return nullptr;
    }

    TRACE_NOCRLF("%02X ", byte);

#if 0
    if (error_reset && byte == 'R' && bufferIndex == 4 && memcmp(buffer, "ERRO", 4)) {
#if defined(PCBX9E)  // X9E enter BT reset loop if following code is implemented
      TRACE("BT Error...");
#else
      TRACE("BT Reset...");
      bufferIndex = 0;
      bluetoothDone();
      state = BLUETOOTH_STATE_OFF;
      wakeupTime = get_tmr10ms() + 100; /* 1s */
#endif
      return NULL;
    }
    else
#endif

    if (byte == '\n') {
      if (bufferIndex > 2 && buffer[bufferIndex-1] == '\r') {
        buffer[bufferIndex-1] = '\0';
        bufferIndex = 0;
        TRACE("BT< %s", buffer);
        if (error_reset && !strcmp((char *)buffer, "ERROR")) {
#if defined(PCBX9E) // X9E enter BT reset loop if following code is implemented
          TRACE("BT Error...");
#else
          TRACE("BT Reset...");
          bluetoothDone();
          state = BLUETOOTH_STATE_OFF;
          wakeupTime = get_tmr10ms() + 100; /* 1s */
#endif
          return nullptr;
        }
        else {
          if (!memcmp(buffer, "Central:", 8))
            strcpy(localAddr, (char *) buffer + 8);
          else if (!memcmp(buffer, "Peripheral:", 11))
            strcpy(localAddr, (char *) buffer + 11);
          return (char *) buffer;
        }
      }
      else {
        bufferIndex = 0;
      }
    }
    else {
      buffer[bufferIndex++] = byte;
      bufferIndex &= (BLUETOOTH_LINE_LENGTH-1);
    }
  }
}

void Bluetooth::processTrainerFrame(const uint8_t * buffer)
{
  TRACE("");

  for (uint8_t channel=0, i=1; channel<8; channel+=2, i+=3) {
    // +-500 != 512, but close enough.
    ppmInput[channel] = buffer[i] + ((buffer[i+1] & 0xf0) << 4) - 1500;
    ppmInput[channel+1] = ((buffer[i+1] & 0x0f) << 4) + ((buffer[i+2] & 0xf0) >> 4) + ((buffer[i+2] & 0x0f) << 8) - 1500;
  }

  ppmInputValidityTimer = PPM_IN_VALID_TIMEOUT;
}

void Bluetooth::appendTrainerByte(uint8_t data)
{
  if (bufferIndex < BLUETOOTH_LINE_LENGTH) {
    buffer[bufferIndex++] = data;
    // we check for "DisConnected", but the first byte could be altered (if received in state STATE_DATA_XOR)
    if (data == '\n') {
      if (!strncmp((char *)&buffer[bufferIndex-13], "isConnected", 11)) {
        TRACE("BT< DisConnected");
        state = BLUETOOTH_STATE_DISCONNECTED;
        bufferIndex = 0;
        wakeupTime += 200; // 1s
      }
    }
  }
}

void Bluetooth::processTrainerByte(uint8_t data)
{
  static uint8_t dataState = STATE_DATA_IDLE;

  switch (dataState) {
    case STATE_DATA_START:
      if (data == START_STOP) {
        dataState = STATE_DATA_IN_FRAME ;
        bufferIndex = 0;
      }
      else {
        appendTrainerByte(data);
      }
      break;

    case STATE_DATA_IN_FRAME:
      if (data == BYTESTUFF) {
        dataState = STATE_DATA_XOR; // XOR next byte
      }
      else if (data == START_STOP) {
        dataState = STATE_DATA_IN_FRAME ;
        bufferIndex = 0;
      }
      else {
        appendTrainerByte(data);
      }
      break;

    case STATE_DATA_XOR:
      appendTrainerByte(data ^ STUFF_MASK);
      dataState = STATE_DATA_IN_FRAME;
      break;

    case STATE_DATA_IDLE:
      if (data == START_STOP) {
        bufferIndex = 0;
        dataState = STATE_DATA_START;
      }
      else {
        appendTrainerByte(data);
      }
      break;
  }

  if (bufferIndex >= BLUETOOTH_PACKET_SIZE) {
    uint8_t crc = 0x00;
    for (int i=0; i<13; i++) {
      crc ^= buffer[i];
    }
    if (crc == buffer[13]) {
      if (buffer[0] == 0x80) {
        processTrainerFrame(buffer);
      }
    }
    dataState = STATE_DATA_IDLE;
  }
}

void Bluetooth::pushByte(uint8_t byte)
{
  crc ^= byte;
  if (byte == START_STOP || byte == BYTESTUFF) {
    buffer[bufferIndex++] = 0x7d;
    byte ^= STUFF_MASK;
  }
  buffer[bufferIndex++] = byte;
}

void Bluetooth::sendTrainer()
{
  int16_t PPM_range = g_model.extendedLimits ? 640*2 : 512*2;

  int firstCh = g_model.trainerData.channelsStart;
  int lastCh = firstCh + 8;

  uint8_t * cur = buffer;
  bufferIndex = 0;
  crc = 0x00;

  buffer[bufferIndex++] = START_STOP; // start byte
  pushByte(0x80); // trainer frame type?
  for (int channel=0; channel<lastCh; channel+=2, cur+=3) {
    uint16_t channelValue1 = PPM_CH_CENTER(channel) + limit((int16_t)-PPM_range, channelOutputs[channel], (int16_t)PPM_range) / 2;
    uint16_t channelValue2 = PPM_CH_CENTER(channel+1) + limit((int16_t)-PPM_range, channelOutputs[channel+1], (int16_t)PPM_range) / 2;
    pushByte(channelValue1 & 0x00ff);
    pushByte(((channelValue1 & 0x0f00) >> 4) + ((channelValue2 & 0x00f0) >> 4));
    pushByte(((channelValue2 & 0x000f) << 4) + ((channelValue2 & 0x0f00) >> 8));
  }
  buffer[bufferIndex++] = crc;
  buffer[bufferIndex++] = START_STOP; // end byte

  write(buffer, bufferIndex);
  bufferIndex = 0;
}

void Bluetooth::forwardTelemetry(const uint8_t * packet)
{
  crc = 0x00;

  buffer[bufferIndex++] = START_STOP; // start byte
  for (uint8_t i=0; i<sizeof(SportTelemetryPacket); i++) {
    pushByte(packet[i]);
  }
  buffer[bufferIndex++] = crc;
  buffer[bufferIndex++] = START_STOP; // end byte

  if (bufferIndex >= 2*FRSKY_SPORT_PACKET_SIZE) {
    write(buffer, bufferIndex);
    bufferIndex = 0;
  }
}

void Bluetooth::receiveTrainer()
{
  uint8_t byte;

  while (1) {
    if (!btRxFifo.pop(byte)) {
      return;
    }

    TRACE_NOCRLF("%02X ", byte);

    processTrainerByte(byte);
  }
}

#if defined(PCBX9E) && !defined(USEHORUSBT)
void Bluetooth::wakeup(void)
{
#if !defined(SIMU)
  if (!g_eeGeneral.bluetoothMode) {
    if (state != BLUETOOTH_INIT) {
      bluetoothDone();
      state = BLUETOOTH_INIT;
    }
  }
  else {
    static tmr10ms_t waitEnd = 0;
    if (state != BLUETOOTH_STATE_IDLE) {
      if (state == BLUETOOTH_INIT) {
        bluetoothInit(BLUETOOTH_DEFAULT_BAUDRATE);
        char command[32];
        char * cur = strAppend(command, BLUETOOTH_COMMAND_NAME);
        uint8_t len = ZLEN(g_eeGeneral.bluetoothName);
        if (len > 0) {
          for (int i = 0; i < len; i++) {
            *cur++ = zchar2char(g_eeGeneral.bluetoothName[i]);
          }
        }
        else {
          cur = strAppend(cur, "Taranis-X9E");
        }
        strAppend(cur, "\r\n");
        writeString(command);
        state = BLUETOOTH_WAIT_TTM;
        waitEnd = get_tmr10ms() + 25; // 250ms
      }
      else if (state == BLUETOOTH_WAIT_TTM) {
        if (get_tmr10ms() > waitEnd) {
          char * line = readline();
          if (strncmp(line, "OK+", 3)) {
            state = BLUETOOTH_STATE_IDLE;
          }
          else {
            bluetoothInit(BLUETOOTH_FACTORY_BAUDRATE);
            const char btMessage[] = "TTM:BPS-115200";
            writeString(btMessage);
            state = BLUETOOTH_WAIT_BAUDRATE_CHANGE;
            waitEnd = get_tmr10ms() + 250; // 2.5s
          }
        }
      }
      else if (state == BLUETOOTH_WAIT_BAUDRATE_CHANGE) {
        if (get_tmr10ms() > waitEnd) {
          state = BLUETOOTH_INIT;
        }
      }
    }
    else if (IS_BLUETOOTH_TRAINER()){
      state = BLUETOOTH_STATE_CONNECTED;
      bluetoothWriteWakeup();
      sendTrainer();
    }
  }
#endif
}
#else // PCBX9E
void Bluetooth::wakeup()
{
  if (state != BLUETOOTH_STATE_OFF) {
    bluetoothWriteWakeup();
    if (bluetoothIsWriting()) {
      return;
    }
  }

  tmr10ms_t now = get_tmr10ms();

  if (now < wakeupTime)
    return;

  wakeupTime = now + 5; /* 50ms default */

  if (g_eeGeneral.bluetoothMode == BLUETOOTH_OFF || (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER && !IS_BLUETOOTH_TRAINER())) {
    if (state != BLUETOOTH_STATE_OFF) {
      bluetoothDone();
      state = BLUETOOTH_STATE_OFF;
    }
    wakeupTime = now + 10; /* 100ms */
  }
  else if (state == BLUETOOTH_STATE_OFF) {
    bluetoothInit(BLUETOOTH_FACTORY_BAUDRATE);
    state = BLUETOOTH_STATE_FACTORY_BAUDRATE_INIT;
  }

  if (state == BLUETOOTH_STATE_FACTORY_BAUDRATE_INIT) {
    writeString("AT+BAUD4\r\n");
    state = BLUETOOTH_STATE_BAUDRATE_SENT;
    wakeupTime = now + 10; /* 100ms */
  }
  else if (state == BLUETOOTH_STATE_BAUDRATE_SENT) {
    bluetoothInit(BLUETOOTH_DEFAULT_BAUDRATE);
    state = BLUETOOTH_STATE_BAUDRATE_INIT;
    readline(false);
    wakeupTime = now + 10; /* 100ms */
  }
  else if (state == BLUETOOTH_STATE_CONNECTED) {
    if (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER && g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH) {
      receiveTrainer();
    }
    else {
      if (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER && g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH) {
        sendTrainer();
        wakeupTime = now + 2; /* 20ms */
      }
      readline(); // to deal with "ERROR"
    }
  }
  else {
    char * line = readline();
    if (state == BLUETOOTH_STATE_BAUDRATE_INIT) {
      char command[32];
      char * cur = strAppend(command, BLUETOOTH_COMMAND_NAME);
      uint8_t len = ZLEN(g_eeGeneral.bluetoothName);
      if (len > 0) {
        for (int i = 0; i < len; i++) {
          *cur++ = zchar2char(g_eeGeneral.bluetoothName[i]);
        }
      }
      else {
#if defined(PCBHORUS)
        cur = strAppend(cur, "Horus");
#else
        cur = strAppend(cur, "taranis"); // TODO capital letter once allowed by BT module
#endif
      }
      strAppend(cur, "\r\n");
      writeString(command);
      state = BLUETOOTH_STATE_NAME_SENT;
    }
    else if (state == BLUETOOTH_STATE_NAME_SENT && (!strncmp(line, "OK+", 3) || !strncmp(line, "Central:", 8) || !strncmp(line, "Peripheral:", 11))) {
      writeString("AT+TXPW0\r\n");
      state = BLUETOOTH_STATE_POWER_SENT;
    }
    else if (state == BLUETOOTH_STATE_POWER_SENT && (!strncmp(line, "Central:", 8) || !strncmp(line, "Peripheral:", 11))) {
      if (g_eeGeneral.bluetoothMode == BLUETOOTH_TRAINER && g_model.trainerData.mode == TRAINER_MODE_MASTER_BLUETOOTH)
        writeString("AT+ROLE1\r\n");
      else
        writeString("AT+ROLE0\r\n");
      state = BLUETOOTH_STATE_ROLE_SENT;
    }
    else if (state == BLUETOOTH_STATE_ROLE_SENT && (!strncmp(line, "Central:", 8) || !strncmp(line, "Peripheral:", 11))) {
      state = BLUETOOTH_STATE_IDLE;
    }
    else if (state == BLUETOOTH_STATE_DISCOVER_REQUESTED) {
      writeString("AT+DISC?\r\n");
      state = BLUETOOTH_STATE_DISCOVER_SENT;
    }
    else if (state == BLUETOOTH_STATE_DISCOVER_SENT && !strcmp(line, "OK+DISCS")) {
      state = BLUETOOTH_STATE_DISCOVER_START;
    }
    else if (state == BLUETOOTH_STATE_DISCOVER_START && !strncmp(line, "OK+DISC:", 8)) {
      if (strlen(line) < 8 + LEN_BLUETOOTH_ADDR && reusableBuffer.moduleSetup.bt.devicesCount < MAX_BLUETOOTH_DISTANT_ADDR) {
        strncpy(reusableBuffer.moduleSetup.bt.devices[reusableBuffer.moduleSetup.bt.devicesCount], &line[8], LEN_BLUETOOTH_ADDR);
        ++reusableBuffer.moduleSetup.bt.devicesCount;
      }
    }
    /* else if (state == BLUETOOTH_STATE_DISCOVER_START && !strcmp(line, "OK+DISCE")) {
      state = BLUETOOTH_STATE_DISCOVER_END;
    } */
    else if (state == BLUETOOTH_STATE_BIND_REQUESTED) {
      char command[32];
      strAppend(strAppend(strAppend(command, "AT+CON"), distantAddr), "\r\n");
      writeString(command);
      state = BLUETOOTH_STATE_CONNECT_SENT;
    }
    else if ((state == BLUETOOTH_STATE_IDLE || state == BLUETOOTH_STATE_DISCONNECTED || state == BLUETOOTH_STATE_CONNECT_SENT) && !strncmp(line, "Connected:", 10)) {
      strcpy(distantAddr, &line[10]); // TODO quick & dirty
      state = BLUETOOTH_STATE_CONNECTED;
      if (g_model.trainerData.mode == TRAINER_MODE_SLAVE_BLUETOOTH) {
        wakeupTime += 500; // it seems a 5s delay is needed before sending the 1st frame
      }
    }
    else if (state == BLUETOOTH_STATE_DISCONNECTED && !line) {
      char command[32];
      strAppend(strAppend(strAppend(command, "AT+CON"), distantAddr), "\r\n");
      writeString(command);
      wakeupTime = now + 200; /* 2s */
    }
  }
}
#endif

void Bluetooth::flashFirmware(const char * filename)
{
  state = BLUETOOTH_STATE_FLASH_FIRMWARE;

  pausePulses();

  /* go to bootloader mode */
  bluetoothDone();

  drawProgressScreen(getBasename(filename), "Device reset...", 0, 0);

  /* wait 100ms */
  watchdogSuspend(100);
  RTOS_WAIT_MS(100);

  const char * result = nullptr;

  AUDIO_PLAY(AU_SPECIAL_SOUND_BEEP1 );
  BACKLIGHT_ENABLE();

  if (result) {
    POPUP_WARNING(STR_FIRMWARE_UPDATE_ERROR);
    SET_WARNING_INFO(result, strlen(result), 0);
  }
  else {
    POPUP_INFORMATION(STR_FIRMWARE_UPDATE_SUCCESS);
  }

  /* wait 2s off */
  watchdogSuspend(2000);
  RTOS_WAIT_MS(2000);

  state = BLUETOOTH_STATE_OFF;
  resumePulses();
}