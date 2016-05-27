#ifndef _I2C_DRIVER_H_
#define _I2C_DRIVER_H_

#include "stdint.h"

uint32_t fromBCD(uint8_t bcd_value);
uint8_t toBCD(uint32_t value);

bool i2cInit();
uint32_t i2cReadBuffer(uint8_t chip, uint8_t addr[3], uint8_t addr_len, uint8_t *buffer, uint32_t length);
uint32_t i2cWriteBuffer(uint8_t chip, uint8_t addr[3], uint8_t addr_len, uint8_t *buffer, uint32_t length);

#endif
