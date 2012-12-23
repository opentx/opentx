/**
  ******************************************************************************
  * @file    Project/lcd/lcd.c 
  * @author  X9D Application Team
  * @Hardware version V0.2
  * @date    11-July-2012
  * @brief   This file provides LCD Init and botom drivers.
  * *
  ******************************************************************************
*/

#include "../open9x.h"

#define	WriteData(x)	 AspiData(x)
#define	WriteCommand(x)	 AspiCmd(x)

void Set_Address(u8 x, u8 y)
{
  WriteCommand(x&0x0F);	//Set Column Address LSB CA[3:0]
  WriteCommand((x>>4)|0x10);	//Set Column Address MSB CA[7:4]
    
  WriteCommand((y&0x0F)|0x60);	//Set Row Address LSB RA [3:0]
  WriteCommand(((y>>4)&0x0F)|0x70);    //Set Row Address MSB RA [7:4]
}

void lcdRefresh()
{  
  for (uint8_t y=0; y<DISPLAY_H; y++) {
    uint8_t *p = &displayBuf[y*DISPLAY_W];
    uint8_t mask = (1 << (y%8));
    Set_Address(0, y);
    AspiCmd(0xAF);
    for (uint8_t x=0; x<DISPLAY_W; x+=2) {
      uint8_t data = (p[x] & mask ? 0x0F : 0) + (p[x+1] & mask ? 0xF0 : 0);
      WriteData(data);
    }
  }
}

/**Init the Backlight GPIO */
static void LCD_BL_Config()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOBL, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_BL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOBL, &GPIO_InitStructure);
}

/** Init the anolog spi gpio
*/
static void LCD_Hardware_Init()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_LCD, ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /*!< Configure lcd CLK\ MOSI\ A0pin in output pushpull mode *************/
  GPIO_InitStructure.GPIO_Pin =PIN_LCD_MOSI | PIN_LCD_CLK | PIN_LCD_A0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_LCD, &GPIO_InitStructure);
  
  /*!< Configure lcd NCS pin in output pushpull mode ,PULLUP *************/
  GPIO_InitStructure.GPIO_Pin = PIN_LCD_NCS; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_Init(GPIO_LCD, &GPIO_InitStructure);
}

static void LCD_Init()
{
  /*Hardware Reset need delay*/
  /*LCD_RST_LOW();
    Delay(50);    
    LCD_RST_HIGH();*/
    
  AspiCmd(0x25);   //Temperature compensation curve definition: 0x25 = -0.05%/oC
  AspiCmd(0x2b);   //Panel loading set ,Internal VLCD.
  AspiCmd(0xEA);	//set bias=1/10 :Command table NO.27
  AspiCmd(0x81);	//Set Vop
  AspiCmd(50);		//0--255
  AspiCmd(0xA6);	//inverse display off
  AspiCmd(0xD1);	//SET RGB:Command table NO.21 .SET RGB or BGR.  D1=RGB
  AspiCmd(0xD5);	//set color mode 4K and 12bits  :Command table NO.22
  AspiCmd(0xA0);	//line rates,25.2 Klps
  AspiCmd(0xC8);	//SET N-LINE INVERSION
  AspiCmd(0x00);	//Disable NIV
  AspiCmd(0xF1);	//Set CEN
  AspiCmd(0x3F);	// 1/64DUTY
  AspiCmd(0x84);	//Disable Partial Display
  AspiCmd(0xC4);	//MY=1,MX=0
  AspiCmd(0x89);	//WA=1,column (CA) increment (+1) first until CA reaches CA boundary, then RA will increment by (+1).

  AspiCmd(0xF8);	//Set Window Program Enable  ,inside modle
  AspiCmd(0xF4);   //starting column address of RAM program window.
  AspiCmd(0x00);
  AspiCmd(0xF5);   //starting row address of RAM program window.
  AspiCmd(0x60);
  AspiCmd(0xF6);   //ending column address of RAM program window.
  AspiCmd(0x47);
  AspiCmd(0xF7);   //ending row address of RAM program window.
  AspiCmd(0x9F);

  AspiCmd(0xAF);	//dc2=1,IC into exit SLEEP MODE,	 dc3=1  gray=ON 开灰阶	,dc4=1  Green Enhanc mode disabled	  绿色增强模式关
	
}

void lcdInit()
{
  LCD_BL_Config();
  LCD_Hardware_Init();
  LCD_Init();
}

void lcdSetRefVolt(uint8_t val)
{
}
