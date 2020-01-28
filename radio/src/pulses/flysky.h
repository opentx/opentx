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


/*
#define END                             0xC0
#define ESC                             0xDB
#define ESC_END                         0xDC
#define ESC_ESC                         0xDD
*/


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
void usbDownloadTransmit(uint8_t *buffer, uint32_t size);
#endif
