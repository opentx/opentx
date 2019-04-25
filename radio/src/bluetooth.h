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

enum BluetoothStates {
#if defined(PCBX9E) && !defined(USEHORUSBT)
  BLUETOOTH_INIT,
  BLUETOOTH_WAIT_TTM,
  BLUETOOTH_WAIT_BAUDRATE_CHANGE,
#endif
  BLUETOOTH_STATE_OFF,
  BLUETOOTH_STATE_FACTORY_BAUDRATE_INIT,
  BLUETOOTH_STATE_BAUDRATE_SENT,
  BLUETOOTH_STATE_BAUDRATE_INIT,
  BLUETOOTH_STATE_NAME_SENT,
  BLUETOOTH_STATE_POWER_SENT,
  BLUETOOTH_STATE_ROLE_SENT,
  BLUETOOTH_STATE_IDLE,
  BLUETOOTH_STATE_DISCOVER_REQUESTED,
  BLUETOOTH_STATE_DISCOVER_SENT,
  BLUETOOTH_STATE_DISCOVER_START,
  BLUETOOTH_STATE_DISCOVER_END,
  BLUETOOTH_STATE_BIND_REQUESTED,
  BLUETOOTH_STATE_CONNECT_SENT,
  BLUETOOTH_STATE_CONNECTED,
  BLUETOOTH_STATE_DISCONNECTED,
  BLUETOOTH_STATE_FIRMWARE_UGRADE
};

#define LEN_BLUETOOTH_ADDR              16
#define MAX_BLUETOOTH_DISTANT_ADDR      6
#define BLUETOOTH_PACKET_SIZE          14
#define BLUETOOTH_LINE_LENGTH          32

class Bluetooth
{
  public:
    void write(const uint8_t * data, uint8_t length);
    void writeString(const char * str);
    char * readline(bool error_reset = true);

    void processTrainerFrame(const uint8_t * buffer);
    void appendTrainerByte(uint8_t data);
    void processTrainerByte(uint8_t data);
    void pushByte(uint8_t byte);
    void sendTrainer();
    void forwardTelemetry(const uint8_t * packet);
    void receiveTrainer();
    void wakeup();

    volatile uint8_t state;
    char localAddr[LEN_BLUETOOTH_ADDR+1];
    char distantAddr[LEN_BLUETOOTH_ADDR+1];

  protected:
    uint8_t buffer[BLUETOOTH_LINE_LENGTH+1];
    uint8_t bufferIndex = 0;
    tmr10ms_t wakeupTime = 0;
    uint8_t crc;
};

extern Bluetooth bluetooth;
