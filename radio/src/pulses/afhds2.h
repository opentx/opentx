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
