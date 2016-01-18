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


OPTIMIZE("O0") u8 SPIy_ReadWriteByte(uint8_t value)
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
  SPIy_ReadWriteByte(VS_READ_COMMAND);
  SPIy_ReadWriteByte(address);
  temp=SPIy_ReadWriteByte(0xff);
  temp=temp<<8;
  temp+=SPIy_ReadWriteByte(0xff);
  delay_us(10);//10us
  CS_HIGH();
  SPIx_SetSpeed(SPI_SPEED_8);

  return temp;
}

u16 VS_RD_Cmd(u8 address)
{
  u16 retry=0;
	u16 x ;
  while(READ_DREQ()==0&&(retry++)<0xfffe)retry++;
  SPIx_SetSpeed(SPI_SPEED_64);
  XDCS_HIGH();
  CS_LOW();
  SPIy_ReadWriteByte(VS_READ_COMMAND);
  SPIy_ReadWriteByte(address); //
  x = SPIy_ReadWriteByte(0); //
	x <<= 8 ;
  x |= SPIy_ReadWriteByte(0);     //
  delay_us(5);//5us
  CS_HIGH();
  SPIx_SetSpeed(SPI_SPEED_8);  //
	return x ;
}

void VS_WR_Cmd(u8 address,u16 data)
{
  u16 retry=0;
  while(READ_DREQ()==0&&(retry++)<0xfffe)retry++;
  SPIx_SetSpeed(SPI_SPEED_64);
  XDCS_HIGH();
  CS_LOW();
  SPIy_ReadWriteByte(VS_WRITE_COMMAND);
  SPIy_ReadWriteByte(address); //
  SPIy_ReadWriteByte(data>>8); //
  SPIy_ReadWriteByte(data);     //
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

uint16_t Mp3ReadValue ;

void VS_Soft_Reset(void)
{
  volatile u8 retry;

  SPIx_SetSpeed(SPI_SPEED_64);
  while(READ_DREQ()== 0);
  SPIy_ReadWriteByte(0x00); //start the transfer
  retry=0;
  VS10XX_ID=VS_RD_Reg(SPI_STATUS);//Read the status register
  VS10XX_ID>>=4;//get vs10xx signal

  if(VS10XX_ID==VS1053) VS_WR_Cmd(SPI_MODE,0x0816);//SOFT RESET,new model
  else VS_WR_Cmd(SPI_MODE,0x0804);

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
  SPIy_ReadWriteByte(0X0);
  SPIy_ReadWriteByte(0X0);
  SPIy_ReadWriteByte(0X0);
  SPIy_ReadWriteByte(0X0);
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

const uint8_t header[] = {
        0x52, 0x49, 0x46, 0x46, 0xff, 0xff, 0xff, 0xff, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20,
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x7d, 0x00, 0x00, 0x00, 0xfa, 0x00, 0x00,
        0x02, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void audioInit()
{
  mp3_spi_init();
  VS_HD_Reset();
  VS_Soft_Reset();
  SPIx_SetSpeed(SPI_SPEED_8);

  // SPIx_SetSpeed(SPI_SPEED_8);

  // delay_us(10000);//10ms
  // VS_WR_Cmd(SPI_BASS, data);
  //delay_us(10000);//10ms
  // VS_WR_Cmd(SPI_VOL, 0x2424);
  // delay_us(10000);//10ms



  delay_us(10000);//10ms
/*
  XDCS_LOW();
  SPIy_ReadWriteByte(0x53);
  SPIy_ReadWriteByte(0xEF);
  SPIy_ReadWriteByte(0x6E);
  SPIy_ReadWriteByte(0x63);
  SPIy_ReadWriteByte(0X0);
  SPIy_ReadWriteByte(0X0);
  SPIy_ReadWriteByte(0X0);
  SPIy_ReadWriteByte(0X0);
  delay_us(10);//10ms
  XDCS_HIGH();
*/

  XDCS_LOW();

  while (SpiMp3_read(header, sizeof(header)) != 1) {
    // delay_us(10);//10ms
  }
  /*for (int h = 0; h < 10; h++) {

    while (SpiMp3_read(sineValues, sizeof(sineValues)) != 1) {
      delay_us(1000);//10ms
    }

    for (int i = 0; i < 100; i++)
      delay_us(1000);//1ms
  }*/

}

unsigned short curIndex = 0;

bool dacQueue(AudioBuffer * buffer)
{
  while (SpiMp3_read(buffer->data, buffer->size) != 1) {
    // delay_us(10);//10ms
  }

  return true;
  // SpiMp3_read(buffer->data, AUDIO_BUFFER_SIZE);
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
      // XDCS_LOW();
      //send data
      for( n = 0; n < 32; ++n)
      {
        if (curIndex >= len)
        {
          curIndex = 0;
          return 1;
          //none
          //temp_c = 0x00;
          // SPIy_ReadWriteByte(temp_c);
        }
        else
        {
          temp_c = pTmp[curIndex];
          SPIy_ReadWriteByte(temp_c);
          ++curIndex;
        }
      }
      //delay for send over
      // delay_us(10);//10ms
      //XDCS_HIGH();

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