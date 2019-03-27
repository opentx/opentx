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

#include "opentx.h"

//#define ACC_LSB_VALUE	0.000488  // 0.488  mg/LSB
//#define GYRO_LSB_VALUE	0.07    // 70mDPS in 2000 deg/second

/* COMMON VALUES FOR ACCEL-GYRO SENSORS */
#define LSM6DS3_WHO_AM_I                        0x0f
#define LSM6DS3_WHO_AM_I_DEF                        0x69
#define LSM6DS3_AXIS_EN_MASK                        0x38
#define LSM6DS3_INT1_CTRL_ADDR                        0x0d
#define LSM6DS3_INT2_CTRL_ADDR                        0x0e
#define LSM6DS3_INT1_FULL                        0x20
#define LSM6DS3_INT1_FTH                        0x08
#define LSM6DS3_MD1_ADDR                        0x5e
#define LSM6DS3_ODR_LIST_NUM                        5
#define LSM6DS3_ODR_POWER_OFF_VAL                0x00
#define LSM6DS3_ODR_26HZ_VAL                        0x02
#define LSM6DS3_ODR_52HZ_VAL                        0x03
#define LSM6DS3_ODR_104HZ_VAL                        0x04
#define LSM6DS3_ODR_208HZ_VAL                        0x05
#define LSM6DS3_ODR_416HZ_VAL                        0x06
#define LSM6DS3_FS_LIST_NUM                        4
#define LSM6DS3_BDU_ADDR                        0x12
#define LSM6DS3_BDU_MASK                        0x40
#define LSM6DS3_EN_BIT                                0x01
#define LSM6DS3_DIS_BIT                                0x00
#define LSM6DS3_FUNC_EN_ADDR                        0x19
#define LSM6DS3_FUNC_EN_MASK                        0x04
#define LSM6DS3_FUNC_CFG_ACCESS_ADDR                0x01
#define LSM6DS3_FUNC_CFG_ACCESS_MASK                0x01
#define LSM6DS3_FUNC_CFG_ACCESS_MASK2                0x04
#define LSM6DS3_FUNC_CFG_REG2_MASK                0x80
#define LSM6DS3_FUNC_CFG_START1_ADDR                0x62
#define LSM6DS3_FUNC_CFG_START2_ADDR                0x63
#define LSM6DS3_SELFTEST_ADDR                        0x14
#define LSM6DS3_SELFTEST_ACCEL_MASK                0x03
#define LSM6DS3_SELFTEST_GYRO_MASK                0x0c
#define LSM6DS3_SELF_TEST_DISABLED_VAL                0x00
#define LSM6DS3_SELF_TEST_POS_SIGN_VAL                0x01
#define LSM6DS3_SELF_TEST_NEG_ACCEL_SIGN_VAL        0x02
#define LSM6DS3_SELF_TEST_NEG_GYRO_SIGN_VAL        0x03
#define LSM6DS3_LIR_ADDR                        0x58
#define LSM6DS3_LIR_MASK                        0x01
#define LSM6DS3_TIMER_EN_ADDR                        0x58
#define LSM6DS3_TIMER_EN_MASK                        0x80
#define LSM6DS3_PEDOMETER_EN_ADDR                0x58
#define LSM6DS3_PEDOMETER_EN_MASK                0x40
#define LSM6DS3_INT2_ON_INT1_ADDR                0x13
#define LSM6DS3_INT2_ON_INT1_MASK                0x20
#define LSM6DS3_MIN_DURATION_MS                        1638
#define LSM6DS3_ROUNDING_ADDR                        0x16
#define LSM6DS3_ROUNDING_MASK                        0x04
#define LSM6DS3_FIFO_MODE_ADDR                        0x0a
#define LSM6DS3_FIFO_MODE_MASK                        0x07
#define LSM6DS3_FIFO_MODE_BYPASS                0x00
#define LSM6DS3_FIFO_MODE_CONTINUOS                0x06
#define LSM6DS3_FIFO_THRESHOLD_IRQ_MASK                0x08
#define LSM6DS3_FIFO_ODR_ADDR                        0x0a
#define LSM6DS3_FIFO_ODR_MASK                        0x78
#define LSM6DS3_FIFO_ODR_MAX                        0x07
#define LSM6DS3_FIFO_ODR_MAX_HZ                        800
#define LSM6DS3_FIFO_ODR_OFF                        0x00
#define LSM6DS3_FIFO_CTRL3_ADDR                        0x08
#define LSM6DS3_FIFO_ACCEL_DECIMATOR_MASK        0x07
#define LSM6DS3_FIFO_GYRO_DECIMATOR_MASK        0x38
#define LSM6DS3_FIFO_CTRL4_ADDR                        0x09
#define LSM6DS3_FIFO_STEP_C_DECIMATOR_MASK        0x38
#define LSM6DS3_FIFO_THR_L_ADDR                        0x06
#define LSM6DS3_FIFO_THR_H_ADDR                        0x07
#define LSM6DS3_FIFO_THR_H_MASK                        0x0f
#define LSM6DS3_FIFO_THR_IRQ_MASK                0x08
#define LSM6DS3_FIFO_PEDO_E_ADDR                0x07
#define LSM6DS3_FIFO_PEDO_E_MASK                0x80
#define LSM6DS3_FIFO_STEP_C_FREQ                25

/* CUSTOM VALUES FOR ACCEL SENSOR */
#define LSM6DS3_ACCEL_ODR_ADDR                        0x10
#define LSM6DS3_ACCEL_ODR_MASK                        0xf0
#define LSM6DS3_ACCEL_FS_ADDR                        0x10
#define LSM6DS3_ACCEL_FS_MASK                        0x0c
#define LSM6DS3_ACCEL_FS_2G_VAL                        0x00
#define LSM6DS3_ACCEL_FS_4G_VAL                        0x02
#define LSM6DS3_ACCEL_FS_8G_VAL                        0x03
#define LSM6DS3_ACCEL_FS_16G_VAL                0x01
#define LSM6DS3_ACCEL_FS_2G_GAIN                61
#define LSM6DS3_ACCEL_FS_4G_GAIN                122
#define LSM6DS3_ACCEL_FS_8G_GAIN                244
#define LSM6DS3_ACCEL_FS_16G_GAIN                488
#define LSM6DS3_ACCEL_OUT_X_L_ADDR                0x28
#define LSM6DS3_ACCEL_OUT_Y_L_ADDR                0x2a
#define LSM6DS3_ACCEL_OUT_Z_L_ADDR                0x2c
#define LSM6DS3_ACCEL_AXIS_EN_ADDR                0x18
#define LSM6DS3_ACCEL_DRDY_IRQ_MASK                0x01
#define LSM6DS3_ACCEL_STD                        1
#define LSM6DS3_ACCEL_STD_FROM_PD                2

/* CUSTOM VALUES FOR GYRO SENSOR */
#define LSM6DS3_GYRO_ODR_ADDR                        0x11
#define LSM6DS3_GYRO_ODR_MASK                        0xf0
#define LSM6DS3_GYRO_FS_ADDR                        0x11
#define LSM6DS3_GYRO_FS_MASK                        0x0c
#define LSM6DS3_GYRO_FS_245_VAL                        0x00
#define LSM6DS3_GYRO_FS_500_VAL                        0x01
#define LSM6DS3_GYRO_FS_1000_VAL                0x02
#define LSM6DS3_GYRO_FS_2000_VAL                0x03
#define LSM6DS3_GYRO_FS_245_GAIN                8750
#define LSM6DS3_GYRO_FS_500_GAIN                17500
#define LSM6DS3_GYRO_FS_1000_GAIN                35000
#define LSM6DS3_GYRO_FS_2000_GAIN                70000
#define LSM6DS3_GYRO_OUT_X_L_ADDR                0x22
#define LSM6DS3_GYRO_OUT_Y_L_ADDR                0x24
#define LSM6DS3_GYRO_OUT_Z_L_ADDR                0x26
#define LSM6DS3_GYRO_AXIS_EN_ADDR                0x19
#define LSM6DS3_GYRO_DRDY_IRQ_MASK                0x02
#define LSM6DS3_GYRO_STD                        6
#define LSM6DS3_GYRO_STD_FROM_PD                2

#define LSM6DS3_OUT_XYZ_SIZE                        8

/* CUSTOM VALUES FOR SIGNIFICANT MOTION SENSOR */
#define LSM6DS3_SIGN_MOTION_EN_ADDR                0x19
#define LSM6DS3_SIGN_MOTION_EN_MASK                0x01
#define LSM6DS3_SIGN_MOTION_DRDY_IRQ_MASK        0x40

/* CUSTOM VALUES FOR STEP DETECTOR SENSOR */
#define LSM6DS3_STEP_DETECTOR_DRDY_IRQ_MASK        0x80

/* CUSTOM VALUES FOR STEP COUNTER SENSOR */
#define LSM6DS3_STEP_COUNTER_DRDY_IRQ_MASK        0x80
#define LSM6DS3_STEP_COUNTER_OUT_L_ADDR                0x4b
#define LSM6DS3_STEP_COUNTER_OUT_SIZE                2
#define LSM6DS3_STEP_COUNTER_RES_ADDR                0x19
#define LSM6DS3_STEP_COUNTER_RES_MASK                0x06
#define LSM6DS3_STEP_COUNTER_RES_ALL_EN                0x03
#define LSM6DS3_STEP_COUNTER_RES_FUNC_EN          0x02
#define LSM6DS3_STEP_COUNTER_DURATION_ADDR        0x15

/* CUSTOM VALUES FOR TILT SENSOR */
#define LSM6DS3_TILT_EN_ADDR                      0x58
#define LSM6DS3_TILT_EN_MASK                      0x20
#define LSM6DS3_TILT_DRDY_IRQ_MASK                0x02

#define LSM6DS3_ENABLE_AXIS                       0x07
#define LSM6DS3_FIFO_DIFF_L                       0x3a
#define LSM6DS3_FIFO_DIFF_MASK                    0x0fff
#define LSM6DS3_FIFO_DATA_OUT_L                   0x3e
#define LSM6DS3_FIFO_ELEMENT_LEN_BYTE             6
#define LSM6DS3_FIFO_BYTE_FOR_CHANNEL             2
#define LSM6DS3_FIFO_DATA_OVR_2REGS               0x4000
#define LSM6DS3_FIFO_DATA_OVR                     0x40

#define LSM6DS3_SRC_FUNC_ADDR                     0x53
#define LSM6DS3_FIFO_DATA_AVL_ADDR                0x3b

#define LSM6DS3_SRC_SIGN_MOTION_DATA_AVL          0x40
#define LSM6DS3_SRC_STEP_DETECTOR_DATA_AVL        0x10
#define LSM6DS3_SRC_TILT_DATA_AVL                 0x20
#define LSM6DS3_SRC_STEP_COUNTER_DATA_AVL         0x80
#define LSM6DS3_FIFO_DATA_AVL                     0x80
#define LSM6DS3_RESET_ADDR                        0x12
#define LSM6DS3_RESET_MASK                        0x01

#define LSM6DS3_ADDRESS                           0xD6
#define LSM6DSLTR_ID                              0x6A

static const char configure[][2] =
        {
                {LSM6DS3_ACCEL_AXIS_EN_ADDR, 0x38}, ///< xyz
                {LSM6DS3_ACCEL_ODR_ADDR,     (0x3 << 4) | (0x1 << 2) | (0x3 << 0)}, //ODR_52Hz|16G|BW_50Hz
                {LSM6DS3_GYRO_AXIS_EN_ADDR,  0x38},        ///< xyz
                {LSM6DS3_GYRO_ODR_ADDR,      (3 << 4) | (3 << 2) | (0 << 0)},   ///< ODR_52Hz|2000dps|
                {LSM6DS3_INT1_CTRL_ADDR,     0x3},   ///< GYRO_READY | ACCL_READY
                {LSM6DS3_INT2_CTRL_ADDR,     0x3},    ///< GYRO_READY | ACCL_READY
        };

volatile int lsm6ds3_state = 1;

static void i2c2Init()
{
  I2C_DeInit(I2CX);

  I2C_InitTypeDef I2C_InitStructure;
  I2C_InitStructure.I2C_ClockSpeed = I2CX_SPEED;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2CX, &I2C_InitStructure);
  I2C_Cmd(I2CX, ENABLE);

  GPIO_PinAFConfig(I2CX_SCL_GPIO, I2CX_SCL_GPIO_PinSource, I2CX_GPIO_AF);
  GPIO_PinAFConfig(I2CX_SDA_GPIO, I2CX_SDA_GPIO_PinSource, I2CX_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = I2CX_SCL_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2CX_SCL_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = I2CX_SDA_GPIO_PIN;
  GPIO_Init(I2CX_SDA_GPIO, &GPIO_InitStructure);
}

#define I2C_TIMEOUT_MAX 10000
bool I2C2_WaitEvent(uint32_t event)
{
  uint32_t timeout = I2C_TIMEOUT_MAX;
  while (!I2C_CheckEvent(I2CX, event)) {
    if ((timeout--) == 0) return false;
  }
  return true;
}

bool I2C2_WaitEventCleared(uint32_t event)
{
  uint32_t timeout = I2C_TIMEOUT_MAX;
  while (I2C_CheckEvent(I2CX, event)) {
    if ((timeout--) == 0) return false;
  }
  return true;
}

int setGyroRegister(uint8_t address, uint8_t value)
{
  if (!I2C2_WaitEventCleared(I2C_FLAG_BUSY))
    return -1;

  I2C_GenerateSTART(I2CX, ENABLE);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return -1;

  I2C_Send7bitAddress(I2CX, LSM6DS3_ADDRESS, I2C_Direction_Transmitter);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    return -1;

  I2C_SendData(I2CX, address);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // TODO TRANSMITTING
    return -1;

  I2C_SendData(I2CX, value);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return -1;

  I2C_GenerateSTOP(I2CX, ENABLE);

  return 0;
}

int readGyroRegister(uint8_t address)
{
  if (!I2C2_WaitEventCleared(I2C_FLAG_BUSY))
    return -1;

  I2C_GenerateSTART(I2CX, ENABLE);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return -1;

  I2C_Send7bitAddress(I2CX, LSM6DS3_ADDRESS, I2C_Direction_Transmitter);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    return -1;

  I2C_SendData(I2CX, address);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return -1;

  I2C_GenerateSTART(I2CX, ENABLE);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return -1;

  I2C_Send7bitAddress(I2CX, LSM6DS3_ADDRESS, I2C_Direction_Receiver);

  I2C_AcknowledgeConfig(I2CX, DISABLE);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
    return -1;

  uint8_t value = I2C_ReceiveData(I2CX);

  I2C_GenerateSTOP(I2CX, ENABLE);

  return value;
}

int gyroInit()
{
  i2c2Init();

  uint8_t id = readGyroRegister(LSM6DS3_WHO_AM_I);
  if (id != LSM6DSLTR_ID) {
    return -1;
  }

  TRACE("OK");

  setGyroRegister(0x12, 0x05);
  delay_ms(1);
  setGyroRegister(0x12, 0x04);
  delay_ms(1);

  TRACE("OK2");

  for (uint8_t i = 0; i < DIM(configure); i++) {
    setGyroRegister(configure[i][0], configure[i][1]);
  }

  TRACE("OK4");

  return 0;
}

#if 0
int gyroRead(void *buffer, int len, OFFSET offset1)
{
  char t_char[10];
  short t_short[7];

  short *p_short;
  I2C_transfer_block i2c_block[2];

  xSemaphoreTake(lsm6ds3_sema, portMAX_DELAY);

  t_char[0] = LSM6DS3_GYRO_OUT_X_L_ADDR;

  i2c_block[0].addr = LSM6DS3_ADDRESS;
  i2c_block[0].rw = I2C_WRITE;
  i2c_block[0].stop_mode = I2C_WRITE_BYPASS_STOP;
  i2c_block[0].len = 1;
  i2c_block[0].buf = t_char;

  i2c_block[1].addr = LSM6DS3_ADDRESS;
  i2c_block[1].rw = I2C_READ;
  i2c_block[1].stop_mode = I2C_WRITE_APPEND_STOP;

  i2c_block[1].len = len;
  i2c_block[1].buf = (char *) t_short;

  i2c_dma_transfer(i2c_block, 2);

  {
    p_short = (short *) buffer;
    *p_short++ = t_short[0];
    *p_short++ = -t_short[1];
    *p_short++ = t_short[2];

    *p_short++ = -t_short[3];
    *p_short++ = t_short[4];
    *p_short++ = -t_short[5];
  }

  return len;
}

#endif