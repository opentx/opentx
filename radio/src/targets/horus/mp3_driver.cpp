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

#include "../../opentx.h"

void setSampleRate(uint32_t frequency)
{
}

uint32_t audioBufferCount = 0;

#define	 VS1001 	0
#define	 VS1011 	1
#define	 VS1002 	2
#define	 VS1003 	3
#define	 VS1053 	4
#define	 VS1033 	5
#define	 VS1103 	7
//extern u8 VS10XX_ID;	  		  //
//命令
#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03

#define SPI_MODE        	0x00  //
#define SPI_STATUS      	0x01  //
#define SPI_BASS        	0x02  //
#define SPI_CLOCKF      	0x03  //
#define SPI_DECODE_TIME 	0x04  //
#define SPI_AUDATA      	0x05  //
#define SPI_WRAM        	0x06  //
#define SPI_WRAMADDR    	0x07  //
#define SPI_HDAT0       	0x08  //
#define SPI_HDAT1       	0x09  //

#define SPI_AIADDR      	0x0a  //
#define SPI_VOL         	0x0b  //
#define SPI_AICTRL0     	0x0c  //
#define SPI_AICTRL1     	0x0d  //
#define SPI_AICTRL2     	0x0e  //
#define SPI_AICTRL3     	0x0f
#define SM_DIFF         	0x01
#define SM_LAYER12         	0x02  //
#define SM_RESET        	0x04  //
#define SM_CANCEL       	0x08  //
#define SM_EARSPEAKER_LO  	0x10  //
#define SM_TESTS        	0x20  //
#define SM_STREAM       	0x40  //
#define SM_EARSPEAKER_HI   	0x80  //
#define SM_DACT         	0x100 //
#define SM_SDIORD       	0x200 //
#define SM_SDISHARE     	0x400 //
#define SM_SDINEW       	0x800 //
#define SM_ADPCM        	0x1000//
#define SM_LINE1         	0x4000//
#define SM_CLK_RANGE     	0x8000//

#define GPIO_XDCS_PIN              GPIO_Pin_0                     /* PI.00 */
#define GPIO_XDCS_GPIO_PORT        GPIOI
#define GPIO_XDCS_GPIO_CLK         RCC_AHB1Periph_GPIOI
#define GPIO_XDCS_SOURCE           GPIO_PinSource0

#define SPI_SCK_PIN               GPIO_Pin_1                    /* PI.01 */
#define SPI_SCK_GPIO_PORT         GPIOI
#define SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOI
#define SPI_SCK_SOURCE            GPIO_PinSource1
#define SPI_SCK_AF                GPIO_AF_SPI2

#define SPI_MISO_PIN              GPIO_Pin_2                    /* PI.02 */
#define SPI_MISO_GPIO_PORT        GPIOI
#define SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOI
#define SPI_MISO_SOURCE           GPIO_PinSource2
#define SPI_MISO_AF               GPIO_AF_SPI2

#define SPI_MOSI_PIN              GPIO_Pin_3                     /* PI.03 */
#define SPI_MOSI_GPIO_PORT        GPIOI                          /* GPIOI */
#define SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOI
#define SPI_MOSI_SOURCE           GPIO_PinSource3
#define SPI_MOSI_AF               GPIO_AF_SPI2

#define SPI                       SPI2
#define SPI_CLK                   RCC_APB1Periph_SPI2

#define GPIO_CS_PIN                GPIO_Pin_13                     /* PH.13 */
#define GPIO_CS_GPIO_PORT          GPIOH
#define GPIO_CS_GPIO_CLK           RCC_AHB1Periph_GPIOH
#define GPIO_CS_SOURCE             GPIO_PinSource13
#define GPIO_CS_AF                 GPIO_AF_SPI2

#define GPIO_DREQ_PIN              GPIO_Pin_14                     /* PH.14 */
#define GPIO_DREQ_GPIO_PORT        GPIOH
#define GPIO_DREQ_GPIO_CLK         RCC_AHB1Periph_GPIOH
#define GPIO_DREQ_SOURCE           GPIO_PinSource14

#define GPIO_RST_PIN               GPIO_Pin_15                     /* PH.15 */
#define GPIO_RST_GPIO_PORT         GPIOH
#define GPIO_RST_GPIO_CLK          RCC_AHB1Periph_GPIOH
#define GPIO_RST_SOURCE            GPIO_PinSource15

#define CS_HIGH()         do{GPIOH->BSRRL =GPIO_CS_PIN;}while(0)
#define CS_LOW()          do{GPIOH->BSRRH =GPIO_CS_PIN;}while(0)

#define XDCS_HIGH()         do{GPIOI->BSRRL =GPIO_XDCS_PIN;}while(0)
#define XDCS_LOW()          do{GPIOI->BSRRH =GPIO_XDCS_PIN;}while(0)

#define RST_HIGH()         do{GPIOH->BSRRL =GPIO_RST_PIN;}while(0)
#define RST_LOW()          do{GPIOH->BSRRH =GPIO_RST_PIN;}while(0)

#define READ_DREQ()          (GPIO_ReadInputDataBit(GPIOH,GPIO_DREQ_PIN))
#define VS_WRITE_COMMAND     0x02  //

#define MP3_BUFFER_SIZE   32

#define SPI_SPEED_2   0
#define SPI_SPEED_4   1
#define SPI_SPEED_8   2
#define SPI_SPEED_16  3
#define SPI_SPEED_32  4
#define SPI_SPEED_64  5
#define SPI_SPEED_128 6
#define SPI_SPEED_256 7


//disable optimize
#define OPTIMIZE(level) __attribute__((optimize(level)))

u8 VS10XX_ID=0;

OPTIMIZE("O0") static void delay_spi(uint32_t nCount)
{
  __IO uint32_t index = 0;
  for(index = (100000 * nCount); index != 0; --index)
  {
    asm("nop\n");
  }
}

OPTIMIZE("O0") static void delay_us(uint32_t nCount)
{
  __IO uint32_t index = 0;
  for(index = (100 * nCount); index != 0; --index)
  {
    asm("nop\n");
  }
}


OPTIMIZE("O0") u8 SPIx_ReadWriteByte(uint8_t value)
{
  u8 ret_val = 0;

  unsigned short time_out = 0x0FFF;

  while( SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_TXE) == RESET )
  {
    --time_out;
    if (!time_out)
    {
      //reset SPI
      SPI_Cmd(SPI,DISABLE);

      SPI_I2S_ClearFlag(SPI, SPI_I2S_FLAG_OVR);
      SPI_I2S_ClearFlag(SPI, SPI_I2S_FLAG_BSY);
      SPI_I2S_ClearFlag(SPI, I2S_FLAG_UDR);
      SPI_I2S_ClearFlag(SPI, SPI_I2S_FLAG_TIFRFE);

      SPI_Cmd(SPI,ENABLE);

      break;
    }
  }
  SPI_I2S_SendData(SPI, value);

  time_out = 0x0FFF;

  while(SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
    --time_out;
    if (!time_out)
    {
      //reset SPI
      SPI_Cmd(SPI,DISABLE);

      SPI_I2S_ClearFlag(SPI, SPI_I2S_FLAG_OVR);
      SPI_I2S_ClearFlag(SPI, SPI_I2S_FLAG_BSY);
      SPI_I2S_ClearFlag(SPI, I2S_FLAG_UDR);
      SPI_I2S_ClearFlag(SPI, SPI_I2S_FLAG_TIFRFE);

      SPI_Cmd(SPI,ENABLE);

      break;
    }
  }
  ret_val = SPI_I2S_ReceiveData(SPI);

  return (ret_val);
}

void SPIx_SetSpeed(u8 SpeedSet)
{
  SPI->CR1&=0XFFC7;//Fsck=Fcpu/256
  switch(SpeedSet)
  {
    case SPI_SPEED_2:
      SPI->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz
      break;
    case SPI_SPEED_4://
      SPI->CR1|=1<<3;//Fsck=Fpclk/4=18Mhz
      break;
    case SPI_SPEED_8://
      SPI->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz
      break;
    case SPI_SPEED_16://
      SPI->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
      break;
    case SPI_SPEED_32:  //
      SPI->CR1|=4<<3;//Fsck=Fpclk/32=2.25Mhz
      break;
    case SPI_SPEED_64:  //
      SPI->CR1|=5<<3;//Fsck=Fpclk/16=1.125Mhz
      break;
    case SPI_SPEED_128: //
      SPI->CR1|=6<<3;//Fsck=Fpclk/16=562.5Khz
      break;
    case SPI_SPEED_256: //
      SPI->CR1|=7<<3;//Fsck=Fpclk/16=281.25Khz
      break;
  }
  SPI->CR1|=1<<6; //
}

u16 VS_RD_Reg(u8 address)
{
  u16 temp=0;
  u8 retry=0;

  while(READ_DREQ()==0&&(retry++)<0XFE);
  if(retry>=0XFE)return 0;
  SPIx_SetSpeed(SPI_SPEED_64);
  XDCS_HIGH();
  CS_LOW();
  SPIx_ReadWriteByte(VS_READ_COMMAND);
  SPIx_ReadWriteByte(address);
  temp=SPIx_ReadWriteByte(0xff);
  temp=temp<<8;
  temp+=SPIx_ReadWriteByte(0xff);
  delay_us(10);//10us
  CS_HIGH();
  SPIx_SetSpeed(SPI_SPEED_8);

  return temp;
}

void VS_WR_Cmd(u8 address,u16 data)
{
  u16 retry=0;
  while(READ_DREQ()==0&&(retry++)<0xfffe)retry++;
  SPIx_SetSpeed(SPI_SPEED_64);
  XDCS_HIGH();
  CS_LOW();
  SPIx_ReadWriteByte(VS_WRITE_COMMAND);
  SPIx_ReadWriteByte(address); //
  SPIx_ReadWriteByte(data>>8); //
  SPIx_ReadWriteByte(data);     //
  delay_us(5);//5us
  CS_HIGH();
  SPIx_SetSpeed(SPI_SPEED_8);  //
}

void VS_Rst_DecodeTime(void)
{
  VS_WR_Cmd(SPI_DECODE_TIME,0x0000);
  VS_WR_Cmd(SPI_DECODE_TIME,0x0000);
}

u8 VS_HD_Reset(void)
{
  u8 retry=0;

  XDCS_HIGH();
  CS_HIGH();
  RST_LOW();
  delay_spi(100);//100ms
  RST_HIGH();
  while((READ_DREQ() ==0)&&(retry<200))//
  {
    retry++;
    delay_us(50);
  }
  delay_spi(20);//20ms
  if(retry>=200)
    return 1;
  else
    return 0;
}

void VS_Soft_Reset(void)
{
  volatile u8 retry;

  SPIx_SetSpeed(SPI_SPEED_64);
  while(READ_DREQ()== 0);

  VS_WR_Cmd(SPI_MODE,0x0020); //SOFT RESET,new model

  return;

  SPIx_ReadWriteByte(0x00); //start the transfer
  retry=0;
  VS10XX_ID=VS_RD_Reg(SPI_STATUS);//Read the status register
  VS10XX_ID>>=4;//get vs10xx signal

  if(VS10XX_ID==VS1053)
    VS_WR_Cmd(SPI_MODE,0x0836); //SOFT RESET,new model
  else
    VS_WR_Cmd(SPI_MODE,0x0824);

  while((READ_DREQ()== 0)&&(retry<200))//waiting DREQ HIGH
  {
    retry++;
    delay_us(50);
  }
  retry=0;
  while(VS_RD_Reg(SPI_CLOCKF)!=0x9800) // Wait for set up successful
  {
    VS_WR_Cmd(SPI_CLOCKF,0x9800);
    if(retry++ >100)break;
  }

  VS_Rst_DecodeTime();    //Reset the decoding time
  SPIx_SetSpeed(SPI_SPEED_8);         //
  XDCS_LOW();
  SPIx_ReadWriteByte(0X0);
  SPIx_ReadWriteByte(0X0);
  SPIx_ReadWriteByte(0X0);
  SPIx_ReadWriteByte(0X0);
  delay_us(10);//10us
  XDCS_HIGH();
}

void mp3_spi_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI|RCC_AHB1Periph_GPIOH,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

  GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;
  GPIO_Init(SPI_SCK_GPIO_PORT,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;
  GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_CS_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_XDCS_PIN;
  GPIO_Init(GPIO_XDCS_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_RST_PIN;
  GPIO_Init(GPIO_RST_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_DREQ_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(GPIO_DREQ_GPIO_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(SPI_SCK_GPIO_PORT, SPI_SCK_SOURCE, GPIO_AF_SPI2);
  GPIO_PinAFConfig(SPI_MISO_GPIO_PORT, SPI_MISO_SOURCE, GPIO_AF_SPI2);
  GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT, SPI_MOSI_SOURCE, GPIO_AF_SPI2);

  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq( &RCC_Clocks);

  SPI_I2S_DeInit(SPI);

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI,&SPI_InitStructure);

  SPI_Cmd(SPI,ENABLE);

  SPI_I2S_ClearFlag(SPI, SPI_I2S_FLAG_RXNE);
  SPI_I2S_ClearFlag(SPI, SPI_I2S_FLAG_TXE);
}

int SpiMp3_read(const void * buffer, int len);

void audioInit()
{
  mp3_spi_init();
  VS_HD_Reset();
  VS_Soft_Reset();
  // SPIx_SetSpeed(SPI_SPEED_8);

  // delay_us(10000);//10ms
  // VS_WR_Cmd(SPI_BASS, data);
  //delay_us(10000);//10ms
 // VS_WR_Cmd(SPI_VOL, 0x2424);
 // delay_us(10000);//10ms


  const uint8_t sine[] = { 0x53, 0xEF, 0x6E, 126, 0, 0, 0, 0 };

  const uint8_t header[] = {
    0x52, 0x49, 0x46, 0x46, 0xff, 0xff, 0xff, 0xff, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20,
    0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x44, 0xac, 0x00, 0x00, 0x10, 0xb1, 0x02, 0x00,
    0x04, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };

  const int16_t sineValues[] =
          {
                  0, 196, 392, 588, 784, 980, 1175, 1370, 1564, 1758,
                  1951, 2143, 2335, 2525, 2715, 2904, 3091, 3278, 3463, 3647,
                  3829, 4011, 4190, 4369, 4545, 4720, 4894, 5065, 5235, 5403,
                  5569, 5733, 5895, 6055, 6213, 6369, 6522, 6673, 6822, 6969,
                  7113, 7255, 7395, 7532, 7667, 7799, 7929, 8056, 8180, 8302,
                  8422, 8539, 8653, 8765, 8874, 8980, 9084, 9185, 9283, 9379,
                  9472, 9562, 9650, 9735, 9818, 9898, 9975, 10050, 10123, 10192,
                  10260, 10324, 10387, 10447, 10504, 10559, 10612, 10663, 10711, 10757,
                  10801, 10843, 10882, 10920, 10955, 10989, 11020, 11050, 11078, 11104,
                  11128, 11151, 11172, 11191, 11209, 11225, 11240, 11254, 11266, 11277,
                  11287, 11296, 11303, 11310, 11316, 11320, 11324, 11327, 11330, 11331,
                  11332, 11333, 11333, 11333, 11332, 11331, 11329, 11328, 11326, 11324,
                  11323, 11321, 11319, 11318, 11316, 11315, 11314, 11313, 11313, 11313,
                  11314, 11315, 11317, 11319, 11323, 11326, 11331, 11336, 11342, 11349,
                  11356, 11365, 11375, 11385, 11397, 11409, 11423, 11437, 11453, 11470,
                  11488, 11507, 11527, 11548, 11571, 11595, 11620, 11646, 11673, 11702,
                  11732, 11763, 11795, 11828, 11863, 11899, 11936, 11974, 12013, 12054,
                  12095, 12138, 12182, 12227, 12273, 12320, 12368, 12417, 12467, 12518,
                  12570, 12623, 12676, 12731, 12786, 12842, 12898, 12956, 13014, 13072,
                  13131, 13191, 13251, 13311, 13372, 13433, 13495, 13556, 13618, 13680,
                  13743, 13805, 13867, 13929, 13991, 14053, 14115, 14177, 14238, 14299,
                  14359, 14419, 14479, 14538, 14597, 14655, 14712, 14768, 14824, 14879,
                  14933, 14986, 15039, 15090, 15140, 15189, 15237, 15284, 15330, 15375,
                  15418, 15460, 15500, 15539, 15577, 15614, 15648, 15682, 15714, 15744,
                  15772, 15799, 15825, 15849, 15871, 15891, 15910, 15927, 15942, 15955,
                  15967, 15977, 15985, 15991, 15996, 15999, 16000, 15999, 15996, 15991,
                  15985, 15977, 15967, 15955, 15942, 15927, 15910, 15891, 15871, 15849,
                  15825, 15799, 15772, 15744, 15714, 15682, 15648, 15614, 15577, 15539,
                  15500, 15460, 15418, 15375, 15330, 15284, 15237, 15189, 15140, 15090,
                  15039, 14986, 14933, 14879, 14824, 14768, 14712, 14655, 14597, 14538,
                  14479, 14419, 14359, 14299, 14238, 14177, 14115, 14053, 13991, 13929,
                  13867, 13805, 13743, 13680, 13618, 13556, 13495, 13433, 13372, 13311,
                  13251, 13191, 13131, 13072, 13014, 12956, 12898, 12842, 12786, 12731,
                  12676, 12623, 12570, 12518, 12467, 12417, 12368, 12320, 12273, 12227,
                  12182, 12138, 12095, 12054, 12013, 11974, 11936, 11899, 11863, 11828,
                  11795, 11763, 11732, 11702, 11673, 11646, 11620, 11595, 11571, 11548,
                  11527, 11507, 11488, 11470, 11453, 11437, 11423, 11409, 11397, 11385,
                  11375, 11365, 11356, 11349, 11342, 11336, 11331, 11326, 11323, 11319,
                  11317, 11315, 11314, 11313, 11313, 11313, 11314, 11315, 11316, 11318,
                  11319, 11321, 11323, 11324, 11326, 11328, 11329, 11331, 11332, 11333,
                  11333, 11333, 11332, 11331, 11330, 11327, 11324, 11320, 11316, 11310,
                  11303, 11296, 11287, 11277, 11266, 11254, 11240, 11225, 11209, 11191,
                  11172, 11151, 11128, 11104, 11078, 11050, 11020, 10989, 10955, 10920,
                  10882, 10843, 10801, 10757, 10711, 10663, 10612, 10559, 10504, 10447,
                  10387, 10324, 10260, 10192, 10123, 10050, 9975, 9898, 9818, 9735,
                  9650, 9562, 9472, 9379, 9283, 9185, 9084, 8980, 8874, 8765,
                  8653, 8539, 8422, 8302, 8180, 8056, 7929, 7799, 7667, 7532,
                  7395, 7255, 7113, 6969, 6822, 6673, 6522, 6369, 6213, 6055,
                  5895, 5733, 5569, 5403, 5235, 5065, 4894, 4720, 4545, 4369,
                  4190, 4011, 3829, 3647, 3463, 3278, 3091, 2904, 2715, 2525,
                  2335, 2143, 1951, 1758, 1564, 1370, 1175, 980, 784, 588,
                  392, 196, 0, -196, -392, -588, -784, -980, -1175, -1370,
                  -1564, -1758, -1951, -2143, -2335, -2525, -2715, -2904, -3091, -3278,
                  -3463, -3647, -3829, -4011, -4190, -4369, -4545, -4720, -4894, -5065,
                  -5235, -5403, -5569, -5733, -5895, -6055, -6213, -6369, -6522, -6673,
                  -6822, -6969, -7113, -7255, -7395, -7532, -7667, -7799, -7929, -8056,
                  -8180, -8302, -8422, -8539, -8653, -8765, -8874, -8980, -9084, -9185,
                  -9283, -9379, -9472, -9562, -9650, -9735, -9818, -9898, -9975, -10050,
                  -10123, -10192, -10260, -10324, -10387, -10447, -10504, -10559, -10612, -10663,
                  -10711, -10757, -10801, -10843, -10882, -10920, -10955, -10989, -11020, -11050,
                  -11078, -11104, -11128, -11151, -11172, -11191, -11209, -11225, -11240, -11254,
                  -11266, -11277, -11287, -11296, -11303, -11310, -11316, -11320, -11324, -11327,
                  -11330, -11331, -11332, -11333, -11333, -11333, -11332, -11331, -11329, -11328,
                  -11326, -11324, -11323, -11321, -11319, -11318, -11316, -11315, -11314, -11313,
                  -11313, -11313, -11314, -11315, -11317, -11319, -11323, -11326, -11331, -11336,
                  -11342, -11349, -11356, -11365, -11375, -11385, -11397, -11409, -11423, -11437,
                  -11453, -11470, -11488, -11507, -11527, -11548, -11571, -11595, -11620, -11646,
                  -11673, -11702, -11732, -11763, -11795, -11828, -11863, -11899, -11936, -11974,
                  -12013, -12054, -12095, -12138, -12182, -12227, -12273, -12320, -12368, -12417,
                  -12467, -12518, -12570, -12623, -12676, -12731, -12786, -12842, -12898, -12956,
                  -13014, -13072, -13131, -13191, -13251, -13311, -13372, -13433, -13495, -13556,
                  -13618, -13680, -13743, -13805, -13867, -13929, -13991, -14053, -14115, -14177,
                  -14238, -14299, -14359, -14419, -14479, -14538, -14597, -14655, -14712, -14768,
                  -14824, -14879, -14933, -14986, -15039, -15090, -15140, -15189, -15237, -15284,
                  -15330, -15375, -15418, -15460, -15500, -15539, -15577, -15614, -15648, -15682,
                  -15714, -15744, -15772, -15799, -15825, -15849, -15871, -15891, -15910, -15927,
                  -15942, -15955, -15967, -15977, -15985, -15991, -15996, -15999, -16000, -15999,
                  -15996, -15991, -15985, -15977, -15967, -15955, -15942, -15927, -15910, -15891,
                  -15871, -15849, -15825, -15799, -15772, -15744, -15714, -15682, -15648, -15614,
                  -15577, -15539, -15500, -15460, -15418, -15375, -15330, -15284, -15237, -15189,
                  -15140, -15090, -15039, -14986, -14933, -14879, -14824, -14768, -14712, -14655,
                  -14597, -14538, -14479, -14419, -14359, -14299, -14238, -14177, -14115, -14053,
                  -13991, -13929, -13867, -13805, -13743, -13680, -13618, -13556, -13495, -13433,
                  -13372, -13311, -13251, -13191, -13131, -13072, -13014, -12956, -12898, -12842,
                  -12786, -12731, -12676, -12623, -12570, -12518, -12467, -12417, -12368, -12320,
                  -12273, -12227, -12182, -12138, -12095, -12054, -12013, -11974, -11936, -11899,
                  -11863, -11828, -11795, -11763, -11732, -11702, -11673, -11646, -11620, -11595,
                  -11571, -11548, -11527, -11507, -11488, -11470, -11453, -11437, -11423, -11409,
                  -11397, -11385, -11375, -11365, -11356, -11349, -11342, -11336, -11331, -11326,
                  -11323, -11319, -11317, -11315, -11314, -11313, -11313, -11313, -11314, -11315,
                  -11316, -11318, -11319, -11321, -11323, -11324, -11326, -11328, -11329, -11331,
                  -11332, -11333, -11333, -11333, -11332, -11331, -11330, -11327, -11324, -11320,
                  -11316, -11310, -11303, -11296, -11287, -11277, -11266, -11254, -11240, -11225,
                  -11209, -11191, -11172, -11151, -11128, -11104, -11078, -11050, -11020, -10989,
                  -10955, -10920, -10882, -10843, -10801, -10757, -10711, -10663, -10612, -10559,
                  -10504, -10447, -10387, -10324, -10260, -10192, -10123, -10050, -9975, -9898,
                  -9818, -9735, -9650, -9562, -9472, -9379, -9283, -9185, -9084, -8980,
                  -8874, -8765, -8653, -8539, -8422, -8302, -8180, -8056, -7929, -7799,
                  -7667, -7532, -7395, -7255, -7113, -6969, -6822, -6673, -6522, -6369,
                  -6213, -6055, -5895, -5733, -5569, -5403, -5235, -5065, -4894, -4720,
                  -4545, -4369, -4190, -4011, -3829, -3647, -3463, -3278, -3091, -2904,
                  -2715, -2525, -2335, -2143, -1951, -1758, -1564, -1370, -1175, -980,
                  -784, -588, -392, -196,
          };

  delay_us(10000);//10ms

  while(SpiMp3_read(sine, sizeof(sine)) != 1) {
    delay_us(1000);//10ms
  }

  for (int i=0; i<100; i++)
    delay_us(1000);//1ms
}

unsigned short curIndex = 0;

bool dacQueue(AudioBuffer * buffer)
{
  SpiMp3_read(buffer->data, AUDIO_BUFFER_SIZE);
}

int SpiMp3_read(const void * buffer, int len)
{
  audioBufferCount++;

  int ret_val = 0;
  unsigned char n = 0;
  char temp_c = 0;

  char *pTmp = (char*)buffer;

  if( (READ_DREQ()!=0) )
  {
    if (curIndex >= len)
    {
      curIndex = 0;
      //send over
      ret_val = 1;
    }
    else
    {
      XDCS_LOW();
      //send data
      for( n = 0; n < 32; ++n)
      {
        if (curIndex >= len)
        {
          //none
          //temp_c = 0x00;
          // SPIx_ReadWriteByte(temp_c);
        }
        else
        {
          temp_c = pTmp[curIndex];
          SPIx_ReadWriteByte(temp_c);
          ++curIndex;
        }
      }
      //delay for send over
      delay_us(10);//10ms
      XDCS_HIGH();

      //continue
      ret_val = 2;
    }
  }
  else
  {
    //none , normal
  }

  return (ret_val);
}

#if 0
int SpiMp3_ioControl(Driver *driver, int io_type , int data)
{
  if (io_type == 0)
  {
    VS_WR_Cmd(SPI_BASS, data);
  }
  else if (io_type == 1)
  {
    VS_WR_Cmd(SPI_VOL, data);
  }
  else if (io_type == 2)
  {
    //clear the index
    curIndex = 0;
  }

  return 0;
}


int SpiMp3_close(Driver *driver)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3| GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  SPI_I2S_DeInit(SPI);

  return 0;
}

#endif