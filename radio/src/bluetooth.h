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
#if defined(PCBX9E) && defined(DEBUG)
    BLUETOOTH_INIT,
    BLUETOOTH_WAIT_TTM,
    BLUETOOTH_WAIT_BAUDRATE_CHANGE,
#endif
    BLUETOOTH_STATE_OFF,
    BLUETOOTH_STATE_NAME_SENT,
    BLUETOOTH_STATE_POWER_SENT,
    BLUETOOTH_STATE_ROLE_SENT,
    BLUETOOTH_STATE_IDLE,
    BLUETOOTH_STATE_DISCOVER_REQUESTED,
    BLUETOOTH_STATE_DISCOVER_SENT,
    BLUETOOTH_STATE_DISCOVER_START,
    BLUETOOTH_STATE_DISCOVER_END,
    BLUETOOTH_STATE_BIND_REQUESTED,
    BLUETOOTH_STATE_BIND_SENT,
    BLUETOOTH_STATE_CONNECTED
};

#define LEN_BLUETOOTH_FRIEND           16

extern volatile uint8_t bluetoothState;
extern char bluetoothFriend[LEN_BLUETOOTH_FRIEND+1];

char * bluetoothReadline();
void bluetoothWriteString(const char * command);
void bluetoothForwardTelemetry(uint8_t data);
void bluetoothWakeup();
