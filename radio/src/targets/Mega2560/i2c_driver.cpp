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

// From https://github.com/g4lvanix/I2C-master-lib

#include <avr/io.h>
#include <util/twi.h>

#include "i2c_driver.h"

//#define F_SCL 400000UL // SCL frequency
#define F_SCL 800000UL // SCL frequency
#define Prescaler 1
#define TWBR_val ((((F_CPU / F_SCL) / Prescaler) - 16 ) / 2)

void wait()
{
  while( !(TWCR & (1<<TWINT)) );
}

void i2c_init(void)
{
  TWBR = (uint8_t)TWBR_val;
}

uint8_t i2c_start(uint8_t address)
{
  // reset TWI control register
  TWCR = 0;
  // transmit START condition
  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
  // wait for end of transmission
  wait();

  // check if the start condition was successfully transmitted
  if((TWSR & 0xF8) != TW_START){ return 1; }

  // load slave address into data register
  TWDR = address;
  // start transmission of address
  TWCR = (1<<TWINT) | (1<<TWEN);
  // wait for end of transmission
  wait();
  // check if the device has acknowledged the READ / WRITE mode
  uint8_t twst = TW_STATUS & 0xF8;
  if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;

  return 0;
}

uint8_t i2c_write(uint8_t data)
{
  // load data into data register
  TWDR = data;
  // start transmission of data
  TWCR = (1<<TWINT) | (1<<TWEN);
  // wait for end of transmission
  wait();

  if( (TWSR & 0xF8) != TW_MT_DATA_ACK ){ return 1; }

  return 0;
}

uint8_t i2c_read_ack(void)
{

  // start TWI module and acknowledge data after reception
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
  // wait for end of transmission
  wait();
  // return received data from TWDR
  return TWDR;
}

uint8_t i2c_read_nack(void)
{

  // start receiving without acknowledging reception
  TWCR = (1<<TWINT) | (1<<TWEN);
  // wait for end of transmission
  wait();
  // return received data from TWDR
  return TWDR;
}

uint8_t i2c_transmit(uint8_t address, uint8_t* data, uint16_t length)
{
  if (i2c_start(address | I2C_WRITE)) return 1;

  for (uint16_t i = 0; i < length; i++)
  {
    if (i2c_write(data[i])) return 1;
  }

  i2c_stop();

  return 0;
}

uint8_t i2c_receive(uint8_t address, uint8_t* data, uint16_t length)
{
  if (i2c_start(address | I2C_READ)) return 1;

  for (uint16_t i = 0; i < (length-1); i++)
  {
    data[i] = i2c_read_ack();
  }
  data[(length-1)] = i2c_read_nack();

  i2c_stop();

  return 0;
}

uint8_t i2c_writeReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length)
{
  if (i2c_start(devaddr | 0x00)) return 1;

  i2c_write(regaddr);

  for (uint16_t i = 0; i < length; i++)
  {
    if (i2c_write(data[i])) return 1;
  }

  i2c_stop();

  return 0;
}

uint8_t i2c_readReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length)
{
  if (i2c_start(devaddr)) return 1;

  i2c_write(regaddr);

  if (i2c_start(devaddr | 0x01)) return 1;

  for (uint16_t i = 0; i < (length-1); i++)
  {
    data[i] = i2c_read_ack();
  }
  data[(length-1)] = i2c_read_nack();

  i2c_stop();

  return 0;
}

void i2c_stop(void)
{
  // transmit STOP condition
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

uint8_t iic_read (
uint8_t dev,    /* Device address */
uint8_t adr,    /* Read start address */
uint16_t cnt,   /* Read uint8_t count */
uint8_t *buff   /* Read data buffer */
)
{
  uint8_t ret;
  ret = i2c_readReg(dev, adr, buff, cnt);
  return (!ret);
}

uint8_t iic_write (
uint8_t dev,      /* Device address */
uint8_t adr,      /* Write start address */
uint16_t cnt,     /* Write uint8_t count */
const uint8_t *buff /* Data to be written */
)
{
  uint8_t ret;
  ret = i2c_writeReg(dev, adr, (uint8_t *)buff, cnt);
  return (!ret);
}
