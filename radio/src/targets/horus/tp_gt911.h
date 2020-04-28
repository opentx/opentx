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

#ifndef _FT5X06_H_
#define _FT5X06_H_

#include "opentx.h"
#include "touch.h"

#define HAS_TOUCH_PANEL()     touchGT911Flag == true

bool touchPanelInit();
void touchPanelDeInit();

extern uint8_t touchGT911Flag;
extern uint8_t touchPanelEvent;

uint8_t gt911WriteRegister(uint16_t reg, uint8_t * buf, uint8_t len);
void gt911ReadRegister(uint16_t reg, uint8_t * buf, uint8_t len);
void touchPanelRead();

#define GT911_MAX_TP      5
#define GT911_CFG_NUMER   0x6A

//GT911 param table
const uint8_t GT911_Cfg[] =
{
  GT911_CFG_NUMER,     // 0x8047 Config version
  0xE0,                // 0x8048 X output map : x 480
  0x01,
  0x10,                // 0x804A Y ouptut max : y 272
  0x01,
  GT911_MAX_TP,        // 0x804C Touch number
  0x3C,                // 0x804D Module switch 1 : bit4= xy change Int mode
  0x20,                // 0x804E Module switch 2
  0x22,                // 0x804F Shake_Count
  0x0A,                // 0x8050 Filter
  0x28,                // 0x8051 Larger touch
  0x0F,                // 0x8052 Noise reduction
  0x5A,                // 0x8053 Screen touch level
  0x3C,                // 0x8054 Screen touch leave
  0x03,                // 0x8055 Low power control
  0x05,                // 0x8056 Refresh rate
  0x00,                // 0x8057 X threshold
  0x00,                // 0x8058 Y threshold
  0x00,                // 0x8059 Reserved
  0x00,                // 0x805A Reserved
  0x11,                // 0x805B Space (top, bottom)
  0x11,                // 0x805C Space (left, right)
  0x08,                // 0x805D Mini filter
  0x18,                // 0x805E Strech R0
  0x1A,                // 0x805F Strech R1
  0x1E,                // 0x8060 Strech R2
  0x14,                // 0x8061 Strech RM
  0x87,                // 0x8062 Drv groupA num
  0x29,                // 0x8063 Drv groupB num
  0x0A,                // 0x8064 Sensor num
  0xCF,                // 0x8065 FreqA factor
  0xD1,                // 0x8066 FreqB factor
  0xB2,                // 0x8067 Panel bit freq
  0x04,
  0x00,                // 0x8069 Reserved
  0x00,
  0x00,                // 0x806B Panel tx gain
  0xD8,                // 0x806C Panel rx gain
  0x02,                // 0x806D Panel dump shift
  0x1D,                // 0x806E Drv frame control
  0x00,                // 0x806F Charging level up
  0x01,                // 0x8070 Module switch 3
  0x00,                // 0x8071 Gesture distance
  0x00,                // 0x8072 Gesture long press time
  0x00,                // 0x8073 X/Y slope adjust
  0x00,                // 0x8074 Gesture control
  0x00,                // 0x8075 Gesture switch 1
  0x00,                // 0x8076 Gesture switch 2
  0x00,                // 0x8077 Gesture refresh rate
  0x00,                // 0x8078 Gesture touch level
  0x00,                // 0x8079 New green wake up level
  0xB4,                // 0x807A Freq hopping start
  0xEF,                // 0x807B Freq hopping end
  0x94,                // 0x807C Noise detect time
  0xD5,                // 0x807D Hopping flag
  0x02,                // 0x807E Hopping flag
  0x07,                // 0x807F Noise threshold
  0x00,                // 0x8080 Nois min threshold old
  0x00,                // 0x8081 Reserved
  0x04,                // 0x8082 Hopping sensor group
  0x6E,                // 0x8083 Hopping Seg1 normalize
  0xB9,                // 0x8084 Hopping Seg1 factor
  0x00,                // 0x8085 Main clock adjust
  0x6A,                // 0x8086 Hopping Seg2 normalize
  0xC4,                // 0x8087 Hopping Seg2 factor
  0x00,                // 0x8088 Reserved
  0x66,                // 0x8089 Hopping Seg3 normalize
  0xCF,                // 0x808A Hopping Seg3 factor
  0x00,                // 0x808B Reserved
  0x62,                // 0x808C Hopping Seg4 normalize
  0xDB,                // 0x808D Hopping Seg4 factor
  0x00,                // 0x808E Reserved
  0x5E,                // 0x808F Hopping Seg5 normalize
  0xE8,                // 0x8090 Hopping Seg5 factor
  0x00,                // 0x8091 Reserved
  0x5E,                // 0x8092 Hopping Seg6 normalize
  0x00,                // 0x8093 Key1
  0x00,                // 0x8094 Key2
  0x00,                // 0x8095 Key3
  0x00,                // 0x8096 Key4
  0x00,                // 0x8097 Key area
  0x00,                // 0x8098 Key touch level
  0x00,                // 0x8099 Key leave level
  0x00,                // 0x809A Key sens
  0x00,                // 0x809B Key sens
  0x00,                // 0x809C Key restrain
  0x00,                // 0x809D Key restrain time
  0x00,                // 0x809E Large gesture touch
  0x00,                // 0x809F Reserved
  0x00,                // 0x80A0 Reserved
  0x00,                // 0x80A1 Hotknot noise map
  0x00,                // 0x80A2 Link threshold
  0x00,                // 0x80A3 Pxy threshold
  0x00,                // 0x80A4 GHot dump shift
  0x00,                // 0x80A5 GHot rx gain
  0x00,                // 0x80A6 Freg gain
  0x00,                // 0x80A7 Freg gain 1
  0x00,                // 0x80A8 Freg gain 2
  0x00,                // 0x80A9 Freg gain 3
  0x00,                // 0x80AA Reserved
  0x00,                // 0x80AB Reserved
  0x00,                // 0x80AC Reserved
  0x00,                // 0x80AD Reserved
  0x00,                // 0x80AE Reserved
  0x00,                // 0x80AF Reserved
  0x00,                // 0x80B0 Reserved
  0x00,                // 0x80B1 Reserved
  0x00,                // 0x80B2 Reserved
  0x00,                // 0x80B3 Combine dis
  0x00,                // 0x80B4 Split set
  0x00,                // 0x80B5 Reserved
  0x00,                // 0x80B6 Reserved
  0x14,                // 0x80B7 Sensor CH0
  0x12,                // 0x80B8 Sensor CH1
  0x10,                // 0x80B9 Sensor CH2
  0x0E,                // 0x80BA Sensor CH3
  0x0C,                // 0x80BB Sensor CH4
  0x0A,                // 0x80BC Sensor CH5
  0x08,                // 0x80BD Sensor CH6
  0x06,                // 0x80BE Sensor CH7
  0x04,                // 0x80BF Sensor CH8
  0x02,                // 0x80C0 Sensor CH9
  0xFF,                // 0x80C1 Sensor CH10
  0xFF,                // 0x80C2 Sensor CH11
  0xFF,                // 0x80C3 Sensor CH12
  0xFF,                // 0x80C4 Sensor CH13
  0x00,                // 0x80C5 Reserved
  0x00,                // 0x80C6 Reserved
  0x00,                // 0x80C7 Reserved
  0x00,                // 0x80C8 Reserved
  0x00,                // 0x80C9 Reserved
  0x00,                // 0x80CA Reserved
  0x00,                // 0x80CB Reserved
  0x00,                // 0x80CC Reserved
  0x00,                // 0x80CD Reserved
  0x00,                // 0x80CE Reserved
  0x00,                // 0x80CF Reserved
  0x00,                // 0x80D0 Reserved
  0x00,                // 0x80D1 Reserved
  0x00,                // 0x80D2 Reserved
  0x00,                // 0x80D3 Reserved
  0x00,                // 0x80D4 Reserved
  0x28,                // 0x80D5 Driver CH0
  0x26,                // 0x80D6 Driver CH1
  0x24,                // 0x80D7 Driver CH2
  0x22,                // 0x80D8 Driver CH3
  0x21,                // 0x80D9 Driver CH4
  0x20,                // 0x80DA Driver CH5
  0x1F,                // 0x80DB Driver CH6
  0x1E,                // 0x80DC Driver CH7
  0x1D,                // 0x80DD Driver CH8
  0x0C,                // 0x80DE Driver CH9
  0x0A,                // 0x80DF Driver CH10
  0x08,                // 0x80E0 Driver CH11
  0x06,                // 0x80E1 Driver CH12
  0x04,                // 0x80E2 Driver CH13
  0x02,                // 0x80E3 Driver CH14
  0x00,                // 0x80E4 Driver CH15
  0xFF,                // 0x80E5 Driver CH16
  0xFF,                // 0x80E6 Driver CH17
  0xFF,                // 0x80E7 Driver CH18
  0xFF,                // 0x80E8 Driver CH19
  0xFF,                // 0x80E9 Driver CH20
  0xFF,                // 0x80EA Driver CH21
  0xFF,                // 0x80EB Driver CH22
  0xFF,                // 0x80EC Driver CH23
  0xFF,                // 0x80ED Driver CH24
  0xFF,                // 0x80EE Driver CH25
  0x00,                // 0x80EF Reserved
  0x00,                // 0x80F0 Reserved
  0x00,                // 0x80F1 Reserved
  0x00,                // 0x80F2 Reserved
  0x00,                // 0x80F3 Reserved
  0x00,                // 0x80F4 Reserved
  0x00,                // 0x80F5 Reserved
  0x00,                // 0x80F6 Reserved
  0x00,                // 0x80F7 Reserved
  0x00,                // 0x80F8 Reserved
  0x00,                // 0x80F9 Reserved
  0x00,                // 0x80FA Reserved
  0x00,                // 0x80FB Reserved
  0x00,                // 0x80FC Reserved
  0x00,                // 0x80FD Reserved
  0x00                 // 0x80FE Reserved
};

//I2C
#define GT_CMD_WR 		0X28
#define GT_CMD_RD 		0X29

#define SLAVE_Write_Address 0X28

//GT9147
#define GT_CTRL_REG 	        0X8040
#define GT_CFGS_REG 	        0X8047
#define GT_CHECK_REG 	        0X80FF
#define GT_PID_REG 		0X8140

#define GT_GSTID_REG 	        0X814E
#define GT_TP1_REG 		0X8150
#define GT_TP2_REG 		0X8158
#define GT_TP3_REG 		0X8160
#define GT_TP4_REG 		0X8168
#define GT_TP5_REG 		0X8170

#define GT911_READ_XY_REG               0x814E
#define GT911_CLEARBUF_REG              0x814E
#define GT911_CONFIG_REG                0x8047
#define GT911_COMMAND_REG               0x8040
#define GT911_PRODUCT_ID_REG            0x8140
#define GT911_VENDOR_ID_REG             0x814A
#define GT911_CONFIG_VERSION_REG        0x8047
#define GT911_CONFIG_CHECKSUM_REG       0x80FF
#define GT911_FIRMWARE_VERSION_REG      0x8144

PACK(typedef struct
{
  uint8_t track;
  uint16_t x;
  uint16_t y;
  uint16_t size;
  uint8_t reserved;
}) TouchPoint;

PACK(struct TouchData
{
  uint8_t pointsCount;
  union
  {
    TouchPoint points[GT911_MAX_TP];
    uint8_t data[GT911_MAX_TP * sizeof(TouchPoint)];
  };
});

extern struct TouchData touchData;

#define TPRST_LOW()   do { TOUCH_RST_GPIO->BSRRH = TOUCH_RST_GPIO_PIN; } while(0)
#define TPRST_HIGH()  do { TOUCH_RST_GPIO->BSRRL = TOUCH_RST_GPIO_PIN; } while(0)

#define TPINT_LOW()   do { TOUCH_INT_GPIO->BSRRH = TOUCH_INT_GPIO_PIN; } while(0)
#define TPINT_HIGH()  do { TOUCH_INT_GPIO->BSRRL = TOUCH_INT_GPIO_PIN; } while(0)


#define I2C_SCL_L()   do { I2C_GPIO->BSRRH = I2C_SCL_GPIO_PIN; } while(0)
#define I2C_SCL_H()   do { I2C_GPIO->BSRRL = I2C_SCL_GPIO_PIN; } while(0)

#define I2C_SDA_L()   do { I2C_GPIO->BSRRH = I2C_SDA_GPIO_PIN; } while(0)
#define I2C_SDA_H()   do { I2C_GPIO->BSRRL = I2C_SDA_GPIO_PIN; } while(0)

#define READ_SDA GPIO_ReadInputDataBit(I2C_GPIO, I2C_SDA_GPIO_PIN)

#endif // _FT5X06_H_
