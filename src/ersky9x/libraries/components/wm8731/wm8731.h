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

#ifndef WM8731_H
#define WM8731_H

#include "board.h"
#include <twi/twid.h>

/*----------------------------------------------------------------------------
 *         Definitions
 *----------------------------------------------------------------------------*/
#define WM8731_CSB_STATE            (0x0 << 0)

/** Slave address */
//#define WM8731_SLAVE_ADDRESS        0x1a | WM8731_CSB_STATE*/
#define WM8731_SLAVE_ADDRESS        0x1b


/** Reset register*/
#define WM8731_REG_RESET                           0x0F

/** Left Line in register*/
#define WM8731_REG_LEFT_lINEIN                     0x0
/** Left line input volume control*/
#define WM8731_LINVOL_BITS                         (0x1f << 0)
/** Left line input mute to ADC*/
#define WM8731_LINMUTE_BIT                         (0x1 << 7)
/** Left to right channel line input volume and mute data load control*/
#define WM8731_LRINBOTH_BIT                        (0x0 << 8)

/** Right Line in register*/
#define WM8731_REG_RIGHT_lINEIN                     0x1
/** Right line input volume control*/
#define WM8731_RINVOL_BITS                         (0x1f << 0)
/** Right line input mute to ADC*/
#define WM8731_RINMUTE_BIT                         (0x1 << 7)
/** Right to right channel line input volume and mute data load control*/
#define WM8731_RLINBOTH_BIT                        (0x0 << 8)

/** Left Headphone out register*/
#define WM8731_REG_LEFT_HPOUT                      0x2
/** Left chnnel headphone output volume control*/
#define WM8731_LHPVOL_BITS                         (0x7f << 0)
/** Left channel zero cross detect enable*/
#define WM8731_LZCEN_BIT                           (0x1 << 7)
/** Left to right channel headphone volume, mute and zero cross data load control*/
#define WM8731_LRHPBOTH_BIT                        (0x1 << 8)

/** Right Headphone out register*/
#define WM8731_REG_RIGHT_HPOUT                      0x3
/** Right chnnel headphone output volume control*/
#define WM8731_RHPVOL_BITS                         (0x7f << 0)
/** Right channel zero cross detect enable*/
#define WM8731_RZCEN_BIT                           (0x1 << 7)
/** Right to right channel headphone volume, mute and zero cross data load control*/
#define WM8731_RLHPBOTH_BIT                        (0x1 << 8)

/** Analogue audio path control register*/
#define WM8731_REG_ANALOGUE_PATH_CTRL              0x4

/** Digital audio path control register*/
#define WM8731_REG_DIGITAL_PATH_CTRL               0x5

/** Power down control register*/
#define WM8731_REG_PWDOWN_CTRL                     0x6
/** LineIn power down*/
#define WM8731_LINEINPD_BIT                        (0x1 << 0)
#define WM8731_ENABLE_LINEINPD                     (0x1 << 0)
#define WM8731_DISABLE_LINEINPD                    (0x0 << 0)
/** Mic power down*/
#define WM8731_MICPD_BIT                           (0x1 << 1)
#define WM8731_ENABLE_MICPD                        (0x1 << 1)
#define WM8731_DISABLE_MICPD                       (0x0 << 1)
/** ADC power down*/
#define WM8731_ADCPD_BIT                           (0x1 << 2)
#define WM8731_ENABLE_ADCPD                        (0x1 << 2)
#define WM8731_DISABLE_ADCPD                       (0x0 << 2)
/** DAC power down*/
#define WM8731_DACPD_BIT                           (0x1 << 3)
#define WM8731_ENABLE_DACPD                        (0x1 << 3)
#define WM8731_DISABLE_DACPD                       (0x0 << 3)
/** OUT power down*/
#define WM8731_OUTPD_BIT                           (0x1 << 4)
#define WM8731_ENABLE_OUTPD                        (0x1 << 4)
#define WM8731_DISABLE_OUTPD                       (0x0 << 4)
/** OSC power down*/
#define WM8731_OSCBIT                              (0x1 << 5)
#define WM8731_ENABLE_OSCPD                        (0x1 << 5)
#define WM8731_DISABLE_OSCPD                       (0x0 << 5)
/** CLKOUT powerdown*/
#define WM8731_CLKOUTPD_BIT                        (0x1 << 6)
#define WM8731_ENABLE_CLKOUTPD                     (0x1 << 6)
#define WM8731_DISABLE_CLKOUTPD                    (0x0 << 6)
/** Power off device*/
#define WM8731_POWEROFF_BIT                        (0x1 << 7)
#define WM8731_DEV_POWER_OFF                       (0x1 << 7)
#define WM8731_DEV_POWER_ON                        (0x0 << 7)

/** Interface format register*/
#define WM8731_REG_DA_INTERFACE_FORMAT             0x7
/** Format*/
#define WM8731_FORMAT_BITS                         (0x3 << 0)
#define WM8731_FORMAT_MSB_LEFT_JUSTIFIED           (0x0 << 0)
#define WM8731_FORMAT_MSB_RIGHT_JUSTIFIED          (0x1 << 0)
#define WM8731_FORMAT_I2S                          (0x2 << 0)
#define WM8731_FORMAT_DSP                          (0x3 << 0)
/** Input audio data bit length select*/
#define WM8731_IWL_BITS                            (0x3 << 2)
#define WM8731_IWL_16_BIT                          (0x0 << 2)
#define WM8731_IWL_20_BIT                          (0x1 << 2)
#define WM8731_IWL_24_BIT                          (0x2 << 2)
#define WM8731_IWL_32_BIT                          (0x3 << 2)
/** DACLRC phase control*/
#define WM8731_IRP_BITS                            (0x1 << 4)
/** DAC Left right clock swap*/
#define WM8731_IRSWAP_BIT                          (0x1 << 5)
/** Master slave mode control*/
#define WM8731_MS_BIT                              (0x1 << 6)
#define WM8731_ENABLE_MASTER_MODE                  (0x1 << 6)
#define WM8731_ENABLE_SLAVE_MODE                   (0x0 << 6)
/** Bit clock invert*/
#define WM8731_BCLKINV_BIT                         (0x1 << 7)

/** Sampling control*/
#define WM8731_REG_SAMPLECTRL                      0x8
/** Mode select, usb mode, normal mode*/
#define WM8731_USBNORMAL_BIT                       (0x1 << 0)
#define WM8731_NORMAL_MODE                         (0x1 << 0)
#define WM8731_USB_MODE                            (0x1 << 1)
/** Base over-sampling rate*/
#define WM8731_BOSR_BIT                            (0x1 << 1)
#define WM8731_USB_250FS                           (0x0 << 1)
#define WM8731_USB_272FS                           (0x1 << 1)
#define WM8731_NORMAL_256FS                        (0x0 << 1)
#define WM8731_NORMAL_128_192_384_FS               (0x1 << 1)
/** Sample rate control*/
#define WM8731_SR_BITS                             (0xf << 2)
#define WM8731_CLKIDIV2_BIT                        (0x1 << 6)
#define WM8731_CLKODIV2_BIT                        (0x1 << 7)

/** Active control register*/
#define WM8731_REG_ACTIVE_CTRL                     0x9
/** Activate interface*/
#define WM8731_ACTIVE_BIT                          (0x1 << 0)
#define WM8731_ACTIVE_INTERFACE_ACTIVE             (0x1 << 1)
#define WM8731_ACTIVE_INTERFACE_INACTIVE           (0x0 << 1)


/*----------------------------------------------------------------------------
 *         Exported functions
 *----------------------------------------------------------------------------*/

extern uint16_t WM8731_Read(Twid *pTwid, uint32_t device, uint32_t regAddr);
extern void WM8731_Write(Twid *pTwid, uint32_t device, uint32_t regAddr, uint16_t data);
extern uint8_t WM8731_DAC_Init(Twid *pTwid, uint32_t device);
extern uint8_t WM8731_VolumeSet(Twid *pTwid,  uint32_t device, uint16_t value);

#endif // WM8731_H


