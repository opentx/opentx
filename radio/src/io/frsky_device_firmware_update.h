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

#ifndef _FRSKY_DEVICE_FIRMWARE_UPDATE_H_
#define _FRSKY_DEVICE_FIRMWARE_UPDATE_H_

#include <functional>
#include "dataconstants.h"
#include "frsky_pxx2.h"
#include "popups.h"

class DeviceFirmwareUpdate {
    enum State {
      SPORT_IDLE,
      SPORT_POWERUP_REQ,
      SPORT_POWERUP_ACK,
      SPORT_VERSION_REQ,
      SPORT_VERSION_ACK,
      SPORT_DATA_TRANSFER,
      SPORT_DATA_REQ,
      SPORT_COMPLETE,
      SPORT_FAIL
    };

  public:
    DeviceFirmwareUpdate(ModuleIndex module):
      module(module) {
    }

    void flashFile(const char * filename, ProgressHandler progressHandler);

  protected:
    uint8_t state = SPORT_IDLE;
    uint32_t address = 0;
    ModuleIndex module;
    uint8_t frame[12];

    void startup();

    void startFrame(uint8_t command);
    void sendFrame();

    const uint8_t * readFullDuplexFrame(ModuleFifo & fifo, uint32_t timeout);
    const uint8_t * readHalfDuplexFrame(uint32_t timeout);
    const uint8_t * readFrame(uint32_t timeout);
    bool waitState(State state, uint32_t timeout);
    void processFrame(const uint8_t * frame);

    const char * sendPowerOn();
    const char * sendReqVersion();
    const char * uploadFile(const char * filename, ProgressHandler progressHandler);
    const char * endTransfer();
};

#endif // _FRSKY_DEVICE_FIRMWARE_UPDATE_H_
