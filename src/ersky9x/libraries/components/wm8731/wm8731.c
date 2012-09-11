/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */


/**
  * \file
  *
  * Implementation WM8731 driver.
  *
  */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "wm8731.h"
#include <board.h>

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
 
/**
 * \brief Read data from WM8731 Register.
 *
 * \param pTwid   Pointer to twi driver structure
 * \param device  Twi slave address.
 * \param regAddr Register address to read.
 * \return value in the given register.
 */
uint16_t WM8731_Read(Twid *pTwid,
                     uint32_t device,
                     uint32_t regAddr)
{
    uint16_t bitsDataRegister;
    uint8_t Tdata[2]={0,0};

    TWID_Read(pTwid, device, regAddr, 0, Tdata, 2, 0);
    bitsDataRegister = (Tdata[0] << 8) | Tdata[1];
    return bitsDataRegister;
}

/**
 * \brief  Write data to WM8731 Register.
 *
 * \param pTwid   Pointer to twi driver structure
 * \param device  Twi slave address.
 * \param regAddr Register address to read.
 * \param data    Data to write
 */
void WM8731_Write(Twid *pTwid,
                  uint32_t device,
                  uint32_t regAddr,
                  uint16_t data)
{
    uint8_t tmpData[2];
    uint16_t tmp;
    tmp = ((regAddr & 0x7f) << 9) | (data & 0x1ff);

    tmpData[0] = (tmp & 0xff00) >> 8;
    tmpData[1] = tmp & 0xff;
    TWID_Write(pTwid, device, regAddr, 0, tmpData, 2, 0);
}

/**
 * \brief  Init WM8731 to DAC mode.
 *
 * \param pTwid   Pointer to twi driver structure
 * \param device  Twi slave address.
 * \return 0.
 */
uint8_t WM8731_DAC_Init(Twid *pTwid,
                   uint32_t device)
{
    /* reset */
    WM8731_Write(pTwid, device, WM8731_REG_RESET, 0);

    /* analogue audio path control */
    WM8731_Write(pTwid, device, WM8731_REG_ANALOGUE_PATH_CTRL, 0x12);

    /* digital audio path control*/
    WM8731_Write(pTwid, device, WM8731_REG_DIGITAL_PATH_CTRL, 0x00);

    /* power down control */
    WM8731_Write(pTwid, device, WM8731_REG_PWDOWN_CTRL, 0x7);

    /* Active control*/
    WM8731_Write(pTwid, device, WM8731_REG_ACTIVE_CTRL, 0x01);

    return 0;
}

/**
 * \brief  Set WM8731 volume
 *
 * \param pTwid   Pointer to twi driver structure
 * \param device  Twi slave address.
 * \param register Register address
 * \param value Register value, valid value is between 0x30 to 0x7f
 * \return 0.
 */
uint8_t WM8731_VolumeSet(Twid *pTwid,
                   uint32_t device,
                   uint16_t value)
{
    uint16_t regValue;

    value &= WM8731_LHPVOL_BITS;
    regValue = WM8731_LRHPBOTH_BIT | WM8731_LZCEN_BIT | value;
    WM8731_Write(pTwid, device, WM8731_REG_LEFT_HPOUT, regValue);
    regValue = WM8731_RZCEN_BIT | WM8731_RLHPBOTH_BIT | value;
    WM8731_Write(pTwid, device, WM8731_REG_RIGHT_HPOUT, value); 
    return 0;
}

