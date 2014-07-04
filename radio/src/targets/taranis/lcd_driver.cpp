/**
  ******************************************************************************
  * @file    Project/lcd/lcd.c 
  * @author  FrSky Application Team
  * @Hardware version V0.2
  * @date    11-July-2012
  * @brief   This file provides LCD Init and botom drivers.
  * *
  ******************************************************************************
*/

#include "../../opentx.h"

#define	WriteData(x)	 AspiData(x)
#define	WriteCommand(x)	 AspiCmd(x)

#if defined(REVPLUS)
  #define CONTRAST_OFS 160
#else
  #define CONTRAST_OFS 5
#endif

//275us
void Delay(volatile unsigned int ms)
{
  volatile u8 i;
  while(ms != 0) {
    for(i=0;i<250;i++) {}
    for(i=0;i<75;i++) {}
    ms--;
  }
}

#if defined(REVPLUS)
static void LCD_Init()
{
  WriteCommand(0x2F);   //Internal pump control
  Delay(20);
  WriteCommand(0x24);   //Temperature compensation
  WriteCommand(0xE9);   //set bias=1/10
  WriteCommand(0x81);   //Set Vop
#if defined(BOOT)
  AspiCmd(CONTRAST_OFS+25);
#else
  AspiCmd(CONTRAST_OFS+g_eeGeneral.contrast);
#endif
  WriteCommand(0xA2);   //set line rate:28KLPS
  WriteCommand(0x28);   //set pannel loading
  WriteCommand(0x40);   //scroll line LSB
  WriteCommand(0x50);   //SCROLL LINE MSB
  WriteCommand(0x89);   //ram address control
  WriteCommand(0xC0);   //LCD mapping control
  WriteCommand(0x04);   //MX=0,MY=1
  WriteCommand(0xD0);   //DISPLAY PATTERN = 16-SCALE GRAY
  WriteCommand(0xF1);   //SET COM end
  WriteCommand(0x3F);   //64

  WriteCommand(0xF8);   //Set Window Program Disable.

  WriteCommand(0xF5);   //starting row address of RAM program window.PAGE1
  WriteCommand(0x00);
  WriteCommand(0xF7);   //end row address of RAM program window.PAGE32
  WriteCommand(0x1F);
  WriteCommand(0xF4);   //start column address of RAM program window.
  WriteCommand(0x00);
  WriteCommand(0xF6);   //end column address of RAM program window.SEG212
  WriteCommand(0xD3);
}
#else
static void LCD_Init()
{	
  AspiCmd(0x2B);   //Panel loading set ,Internal VLCD.
  AspiCmd(0x25);   //Temperature compensation curve definition: 0x25 = -0.05%/oC
  AspiCmd(0xEA);	//set bias=1/10 :Command table NO.27
  AspiCmd(0x81);	//Set Vop
#if defined(BOOT)
  AspiCmd(CONTRAST_OFS+25);
#else
  AspiCmd(CONTRAST_OFS+g_eeGeneral.contrast);
#endif
  AspiCmd(0xA6);	//inverse display off
  AspiCmd(0xD1);	//SET RGB:Command table NO.21 .SET RGB or BGR.  D1=RGB
  AspiCmd(0xD5);	//set color mode 4K and 12bits  :Command table NO.22
  AspiCmd(0xA0);	//line rates,25.2 Klps
  AspiCmd(0xC8);	//SET N-LINE INVERSION
  AspiCmd(0x1D);	//Disable NIV
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
}
#endif

void Set_Address(u8 x, u8 y)
{
  WriteCommand(x&0x0F);	//Set Column Address LSB CA[3:0]
  WriteCommand((x>>4)|0x10);	//Set Column Address MSB CA[7:4]
    
  WriteCommand((y&0x0F)|0x60);	//Set Row Address LSB RA [3:0]
  WriteCommand(((y>>4)&0x0F)|0x70);    //Set Row Address MSB RA [7:4]
}

#define LCD_WRITE_BIT(bit) \
  if (bit) \
    LCD_MOSI_HIGH(); \
  else \
    LCD_MOSI_LOW(); \
  LCD_CLK_LOW(); \
  __no_operation(); \
  LCD_CLK_HIGH();

#if defined(REVPLUS)
void lcdRefresh()
{  
  for (uint32_t y=0; y<LCD_H; y+=2) {
    uint8_t *p = &displayBuf[y/2 * LCD_W];

    Set_Address(0, y/2);
    AspiCmd(0xAF);

    LCD_CLK_HIGH();
    LCD_A0_HIGH();
    LCD_NCS_LOW();

    for (uint32_t x=0; x<LCD_W; x++) {
      uint8_t a = p[x] ;
      LCD_WRITE_BIT(a & 0x80);
      LCD_WRITE_BIT(a & 0x40);
      LCD_WRITE_BIT(a & 0x20);
      LCD_WRITE_BIT(a & 0x10);
      LCD_WRITE_BIT(a & 0x08);
      LCD_WRITE_BIT(a & 0x04);
      LCD_WRITE_BIT(a & 0x02);
      LCD_WRITE_BIT(a & 0x01);
    }

    LCD_NCS_HIGH();
    LCD_A0_HIGH();

    WriteData(0);
  }
}
#else
void lcdRefresh()
{  
  for (uint32_t y=0; y<LCD_H; y++) {
    uint8_t *p = &displayBuf[y/2 * LCD_W];

    Set_Address(0, y);
    AspiCmd(0xAF);

    LCD_CLK_HIGH();
    LCD_A0_HIGH();
    LCD_NCS_LOW();

    for (uint32_t x=0; x<LCD_W; x++) {
      uint8_t b = p[x];
      if (y & 1)
        b >>= 4;
      LCD_WRITE_BIT(b & 0x01);
      LCD_WRITE_BIT(b & 0x02);
      LCD_WRITE_BIT(b & 0x04);
      LCD_WRITE_BIT(b & 0x08);
    }

    LCD_NCS_HIGH();
    LCD_A0_HIGH();

    WriteData(0);
  }
}
#endif

/**Init the Backlight GPIO */
static void LCD_BL_Config()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOBL, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  
#if defined(REVPLUS)
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_BL|GPIO_Pin_BLW;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOBL, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOBL, GPIO_PinSource_BL, Pin_BL_AF);
  GPIO_PinAFConfig(GPIOBL, GPIO_PinSource_BLW, Pin_BL_AF);

  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN ;        // Enable clock
  TIM4->ARR = 100 ;
  TIM4->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 10000 - 1;
  TIM4->CCMR1 = TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 ; // PWM
  TIM4->CCMR2 = TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 ; // PWM
  TIM4->CCER = TIM_CCER_CC4E | TIM_CCER_CC2E ;
  TIM4->CCR2 = 0 ;
  TIM4->CCR4 = 0 ;
  TIM4->EGR = 0 ;
  TIM4->CR1 = TIM_CR1_CEN ;            // Counter enable
#else
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_BL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOBL, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOBL, GPIO_PinSource_BL, Pin_BL_AF);

  RCC->APB2ENR |= RCC_APB2ENR_TIM10EN ;        // Enable clock
  TIM10->ARR = 100 ;
  TIM10->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 10000 - 1;
  TIM10->CCMR1 = 0x60 ;    // PWM
  TIM10->CCER = 1 ;
  TIM10->CCR1 = 80;
  TIM10->EGR = 0 ;
  TIM10->CR1 = 1 ;
#endif
}

/** Init the anolog spi gpio
*/
static void LCD_Hardware_Init()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_LCD, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  
  /*!< Configure lcd CLK\ MOSI\ A0pin in output pushpull mode *************/
  GPIO_InitStructure.GPIO_Pin = PIN_LCD_MOSI | PIN_LCD_CLK | PIN_LCD_A0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_LCD_SPI, &GPIO_InitStructure);
  
  LCD_NCS_HIGH();
  
  /*!< Configure lcd NCS pin in output pushpull mode ,PULLUP *************/
  GPIO_InitStructure.GPIO_Pin = PIN_LCD_NCS; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIO_LCD_NCS, &GPIO_InitStructure);
  
  /*!< Configure lcd RST pin in output pushpull mode ,PULLUP *************/
  GPIO_InitStructure.GPIO_Pin = PIN_LCD_RST; 
  GPIO_Init(GPIO_LCD_RST, &GPIO_InitStructure);
}

void LCD_OFF()
{
  AspiCmd(0xE2);
  Delay(20);	
}

void lcdInit()
{
  LCD_BL_Config();
  LCD_Hardware_Init();
  
  LCD_RST_HIGH();
  Delay(5);

  LCD_RST_LOW();
  Delay(120); //11ms

  LCD_RST_HIGH();
  Delay(2500);
 
  AspiCmd(0xE2);
  Delay(2500);
  LCD_Init();
  Delay(120);

  LCD_Init();
  Delay(120);
  AspiCmd(0xAF);	//dc2=1, IC into exit SLEEP MODE, dc3=1 gray=ON, dc4=1 Green Enhanc mode disabled
}

void lcdSetRefVolt(uint8_t val)
{
  AspiCmd(0x81);	//Set Vop
  AspiCmd(val+CONTRAST_OFS);		//0--255
}

#if defined(REVPLUS)
void turnBacklightOn(uint8_t level, uint8_t color)
{
  TIM4->CCR4 = (100-level)*color;
  TIM4->CCR2 = (100-level)*(100-color);
}

void turnBacklightOff(void)
{
  TIM4->CCR4 = 0;
  TIM4->CCR2 = 0;
}
#endif
