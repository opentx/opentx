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

#include "afhds2.h"
#include "flysky.h"

afhds2::~afhds2() {
}

void afhds2::setupFrame() {
  setupPulsesAFHDS2(this->index);
}
void afhds2::init(bool resetFrameCount) {
  resetPulsesAFHDS2(this->index);
}
void afhds2::beginBind(::asyncOperationCallback_t callback) {
  bindReceiverAFHDS2(this->index);
}
void afhds2::beginRangeTest(::asyncOperationCallback_t callback) {

}
void afhds2::cancelOperations() {}
void afhds2::stop() {
}
void afhds2::setModuleSettingsToDefault() {

}
void afhds2::onDataReceived(uint8_t data, uint8_t* rxBuffer, uint8_t& rxBufferCount, uint8_t maxSize) {
//Response magic to be implemented
}
const char* afhds2::getState() {
  return "Unknown";
}
