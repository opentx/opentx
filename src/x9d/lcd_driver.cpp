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
#include <stdint.h>
#include "lcd.h"
#include "spi.h"
#include "hal.h"
#include "Macro_define.h" 

#define	WriteData(x)	 AspiData(x)
#define	WriteCommand(x)	 AspiCmd(x)

void Set_Address(u8 x, u8 y)
{
  WriteCommand(x&0x0F);	//Set Column Address LSB CA[3:0]
  WriteCommand((x>>4)|0x10);	//Set Column Address MSB CA[7:4]
    
  WriteCommand((y&0x0F)|0x60);	//Set Row Address LSB RA [3:0]
  WriteCommand(((y>>4)&0x0F)|0x70);    //Set Row Address MSB RA [7:4]
}

/**6 ponits in one line share the same address,3 bytes in the buffer.
3 bytes wrote in one time */
void refreshDisplay()
{  
  u8 _x1,_x0,temp_x0,temp_x00;
    
  _x1 =( DISPLAY_W>>1) + 1;
	
  temp_x0 = 0;
  temp_x00 = temp_x0/3;

  for(uint32_t y0=0; y0<=y1; y0++)
  {
    Set_Address(0, y0);//change the address every column
    AspiCmd(0xAF);
    for(uint_32_t x=0; x_x0<=_x1; _x0++)
    {
      WriteData(lcd_buffer[_x0][y0]);
      //WriteData(lcd_buffer[_x0][y0]);
      //WriteData(lcd_buffer[_x0][y0]);
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

#if 0
void Delay(uint32_t ms);
/* Initialize the LCD */
#endif

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

void lcd_init()
{
  LCD_BL_Config();
  LCD_Hardware_Init();
  LCD_Init();
}

#if 0
/***************************************************************
***********FOR Test
**********************************************************************/
void Delay(uint32_t ms)
{
  u8 i;
  while(ms != 0)
  {
    for(i=0;i<250;i++) {}
    for(i=0;i<75;i++) {}
    ms--;
  }
}


void TEST_LCD()
{
        //static u32 cnt;
        //CC中断
        GPIO_SetBits(GPIOB,GPIO_Pin_8);//BACKLIGHT
        
        //Delay(3000);
        //paintScreen();
        clearScreen();
        drawPoint(0,0,1);
        paintRect(0,0,0,0);
        reversePoint(0,0);
        paintRect(0,0,0,0);
        Delay(3000);
        drawPoint(1,0,1);
        paintRect(0,0,1,0);
        Delay(3000);
        reversePoint(1,0);
        paintRect(0,0,1,0);
        Delay(3000);
        drawPoint(0,1,1);
        paintRect(0,1,0,1);
        Delay(3000);
        reversePoint(0,1);
        paintRect(0,1,0,1);
        
        drawPoint(211,0,1);
        paintRect(0,0,211,0);
        Delay(3000);
        reversePoint(211,0);
        paintRect(0,0,211,0);   
                
        drawPoint(211,63,1);
        paintRect(211,63,211,63);
        Delay(3000);
        reversePoint(211,63);
        paintRect(211,63,211,63);
        
}
#endif
