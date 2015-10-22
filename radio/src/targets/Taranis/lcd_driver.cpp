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
  #define RESET_WAIT_DELAY_MS      300        //wait time after LCD reset before first command
  #define WAIT_FOR_DMA_END()       { while(lcd_busy) {}; }
#else
  #define CONTRAST_OFS 5
  #define RESET_WAIT_DELAY_MS     1300        //wait time after LCD reset before first command
  #define WAIT_FOR_DMA_END()
#endif

bool lcdInitFinished = false;
void lcdInitFinish();

/*
  delaysInit() must be called before the first call to this function!
*/
static void Delay(uint32_t ms)
{
  while(ms--) {
    delay_01us(10000);
  }
}

#if defined(REVPLUS)

// New hardware SPI driver for LCD
void initLcdSpi()
{
  // APB1 clock / 2 = 133nS per clock
  SPI3->CR1 = 0 ;		// Clear any mode error
  SPI3->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_CPOL | SPI_CR1_CPHA ;
  SPI3->CR2 = 0 ;
  SPI3->CR1 |= SPI_CR1_MSTR ;	// Make sure in case SSM/SSI needed to be set first
  SPI3->CR1 |= SPI_CR1_SPE ;

  configure_pins( LCD_GPIO_PIN_NCS, PIN_OUTPUT | PIN_PORTA | PIN_OS25) ;
  configure_pins( LCD_GPIO_PIN_RST, PIN_OUTPUT | PIN_PORTD | PIN_OS25) ;
  configure_pins( LCD_GPIO_PIN_A0,  PIN_OUTPUT | PIN_PORTC | PIN_OS50) ;
  configure_pins( LCD_GPIO_PIN_MOSI|LCD_GPIO_PIN_CLK, PIN_PORTC | PIN_OS50 | PIN_PER_6 | PIN_PERIPHERAL ) ;

  // NVIC_SetPriority( DMA1_Stream7_IRQn, 8 ) ;
  NVIC_EnableIRQ(DMA1_Stream7_IRQn) ;
  DMA1->HIFCR |= DMA_HIFCR_CTCIF7; //clear interrupt flag
  DMA1->LISR |= DMA_HISR_TCIF7;    //enable DMA TX end interrupt

  DMA1_Stream7->CR &= ~DMA_SxCR_EN ;    // Disable DMA
  DMA1->HIFCR = DMA_HIFCR_CTCIF7 | DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CFEIF7 ; // Write ones to clear bits
  DMA1_Stream7->CR =  DMA_SxCR_PL_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 ;
  DMA1_Stream7->PAR = (uint32_t) &SPI3->DR ;
  DMA1_Stream7->M0AR = (uint32_t)displayBuf;
  DMA1_Stream7->FCR = 0x05 ; //DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
  DMA1_Stream7->NDTR = LCD_W*LCD_H/8*4 ;
}

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
  Delay(20);
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
  LCD_CLK_LOW(); \
  LCD_CLK_LOW(); \
  LCD_CLK_HIGH(); \
  LCD_CLK_HIGH();

#if defined(REVPLUS)

volatile bool lcd_busy;

#if !defined(LCD_DUAL_BUFFER)
void lcdRefreshWait() 
{
  WAIT_FOR_DMA_END();
}
#endif

void lcdRefresh(bool wait)
{
  if (!lcdInitFinished) {
    lcdInitFinish();
  }

  //wait if previous DMA transfer still active
  WAIT_FOR_DMA_END();
  lcd_busy = true;

  Set_Address(0, 0);
	
  LCD_NCS_LOW();
  LCD_A0_HIGH();

  DMA1_Stream7->CR &= ~DMA_SxCR_EN ;    // Disable DMA
  DMA1->HIFCR = DMA_HIFCR_CTCIF7 | DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CFEIF7 ; // Write ones to clear bits

#if defined(LCD_DUAL_BUFFER)
  //switch LCD buffer
  DMA1_Stream7->M0AR = (uint32_t)displayBuf;
  displayBuf = (displayBuf == displayBuf1) ? displayBuf2 : displayBuf1;
#endif

  DMA1_Stream7->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE;		// Enable DMA & tXe interrupt
  SPI3->CR2 |= SPI_CR2_TXDMAEN ;
}

extern "C" void DMA1_Stream7_IRQHandler()
{
  //clear interrupt flag
  DMA1_Stream7->CR &= ~DMA_SxCR_TCIE ;  // Stop interrupt
  DMA1->HIFCR |= DMA_HIFCR_CTCIF7;      // Clear interrupt flag
  SPI3->CR2 &= ~SPI_CR2_TXDMAEN ;
  DMA1_Stream7->CR &= ~DMA_SxCR_EN ;    // Disable DMA

  while ( SPI3->SR & SPI_SR_BSY ) {
    /* Wait for SPI to finish sending data 
    The DMA TX End interrupt comes two bytes before the end of SPI transmission,
    therefore we have to wait here.
    */
  }
  LCD_NCS_HIGH();
  lcd_busy = false;
}

#else     // #if defined(REVPLUS)
void lcdRefresh()
{  
  if (!lcdInitFinished) {
    lcdInitFinish();
  }

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
      LCD_WRITE_BIT(b & 0x08);
      LCD_WRITE_BIT(b & 0x04);
      LCD_WRITE_BIT(b & 0x02);
      LCD_WRITE_BIT(b & 0x01);
    }

    LCD_NCS_HIGH();
    LCD_A0_HIGH();

    WriteData(0);
  }
}
#endif

void backlightInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
#if defined(REV9E)
  GPIO_InitStructure.GPIO_Pin = BACKLIGHT_GPIO_PIN_1|BACKLIGHT_GPIO_PIN_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(BACKLIGHT_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(BACKLIGHT_GPIO, BACKLIGHT_GPIO_PinSource_1, BACKLIGHT_GPIO_AF_1);
  GPIO_PinAFConfig(BACKLIGHT_GPIO, BACKLIGHT_GPIO_PinSource_2, BACKLIGHT_GPIO_AF_1);
  BACKLIGHT_TIMER->ARR = 100 ;
  BACKLIGHT_TIMER->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 50000 - 1;  // 20us * 100 = 2ms => 500Hz
  BACKLIGHT_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 ; // PWM
  BACKLIGHT_TIMER->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E ;
  BACKLIGHT_TIMER->CCR2 = 0 ;
  BACKLIGHT_TIMER->CCR1 = 100 ;
  BACKLIGHT_TIMER->EGR = 0 ;
  BACKLIGHT_TIMER->CR1 = TIM_CR1_CEN ;            // Counter enable
#elif defined(REVPLUS)
  GPIO_InitStructure.GPIO_Pin = BACKLIGHT_GPIO_PIN_1|BACKLIGHT_GPIO_PIN_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(BACKLIGHT_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(BACKLIGHT_GPIO, BACKLIGHT_GPIO_PinSource_1, BACKLIGHT_GPIO_AF_1);
  GPIO_PinAFConfig(BACKLIGHT_GPIO, BACKLIGHT_GPIO_PinSource_2, BACKLIGHT_GPIO_AF_1);
  BACKLIGHT_TIMER->ARR = 100 ;
  BACKLIGHT_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 50000 - 1;  // 20us * 100 = 2ms => 500Hz
  BACKLIGHT_TIMER->CCMR1 = TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 ; // PWM
  BACKLIGHT_TIMER->CCMR2 = TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 ; // PWM
  BACKLIGHT_TIMER->CCER = TIM_CCER_CC4E | TIM_CCER_CC2E ;
  BACKLIGHT_TIMER->CCR2 = 0 ;
  BACKLIGHT_TIMER->CCR4 = 100 ;
  BACKLIGHT_TIMER->EGR = 0 ;
  BACKLIGHT_TIMER->CR1 = TIM_CR1_CEN ;            // Counter enable
#else
  GPIO_InitStructure.GPIO_Pin = BACKLIGHT_GPIO_PIN_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(BACKLIGHT_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(BACKLIGHT_GPIO, BACKLIGHT_GPIO_PinSource_1, BACKLIGHT_GPIO_AF_1);
  BACKLIGHT_TIMER->ARR = 100 ;
  BACKLIGHT_TIMER->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 50000 - 1;  // 20us * 100 = 2ms => 500Hz
  BACKLIGHT_TIMER->CCMR1 = 0x60 ;    // PWM
  BACKLIGHT_TIMER->CCER = 1 ;
  BACKLIGHT_TIMER->CCR1 = 80;
  BACKLIGHT_TIMER->EGR = 0 ;
  BACKLIGHT_TIMER->CR1 = 1 ;
#endif
}

/** Init the analog SPI GPIO
*/
static void LCD_Hardware_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /*!< Configure lcd CLK\ MOSI\ A0pin in output push-pull mode *************/
  GPIO_InitStructure.GPIO_Pin = LCD_GPIO_PIN_MOSI | LCD_GPIO_PIN_CLK | LCD_GPIO_PIN_A0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(LCD_GPIO_SPI, &GPIO_InitStructure);
  
  LCD_NCS_HIGH();
  
  /*!< Configure lcd NCS pin in output push-pull mode ,PULLUP *************/
  GPIO_InitStructure.GPIO_Pin = LCD_GPIO_PIN_NCS; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(LCD_GPIO_NCS, &GPIO_InitStructure);
  
  /*!< Configure lcd RST pin in output pushpull mode ,PULLUP *************/
  GPIO_InitStructure.GPIO_Pin = LCD_GPIO_PIN_RST; 
  GPIO_Init(LCD_GPIO_RST, &GPIO_InitStructure);
}

/*
  Proper method for turning of LCD module. It must be used,
  otherwise we might damage LCD crystals in the long run!
*/
void lcdOff()
{
  WAIT_FOR_DMA_END();
  /* 
  LCD Sleep mode is also good for draining capacitors and enables us
  to re-init LCD without any delay
  */
  AspiCmd(0xAE);    //LCD sleep
  Delay(3);	        //wait for caps to drain
}

/*
  Starts LCD initialization routine. It should be called as
  soon as possible after the reset because LCD takes a lot of
  time to properly power-on.

  Make sure that Delay() is functional before calling this function!
*/
void lcdInit()
{
  LCD_Hardware_Init();

  if (WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()) return;    //no need to reset LCD module

  //reset LCD module
  LCD_RST_LOW();
  Delay(1);       // only 3 us needed according to data-sheet, we use 1 ms
  LCD_RST_HIGH();
}

/*
  Finishes LCD initialization. It is called auto-magically when first LCD command is 
  issued by the other parts of the code.
*/
void lcdInitFinish()
{
  lcdInitFinished = true;

#if defined(REVPLUS)
  initLcdSpi();
#endif
  
  /*
    LCD needs longer time to initialize in low temperatures. The data-sheet 
    mentions a time of at least 150 ms. The delay of 1300 ms was obtained 
    experimentally. It was tested down to -10 deg Celsius.

    The longer initialization time seems to only be needed for regular Taranis, 
    the Taranis Plus (9XE) has been tested to work without any problems at -18 deg Celsius.
    Therefore the delay for T+ is lower.
    
    If radio is reset by watchdog or boot-loader the wait is skipped, but the LCD
    is initialized in any case. 

    This initialization is needed in case the user moved power switch to OFF and 
    then immediately to ON position, because lcdOff() was called. In any case the LCD 
    initialization (without reset) is also recommended by the data sheet.
  */

  if (!WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()) {
#if !defined(BOOT)
    while(g_tmr10ms < (RESET_WAIT_DELAY_MS/10)) {};    //wait measured from the power-on
#else
    Delay(RESET_WAIT_DELAY_MS);
#endif
  }
  
  LCD_Init();
  AspiCmd(0xAF);	//dc2=1, IC into exit SLEEP MODE, dc3=1 gray=ON, dc4=1 Green Enhanc mode disabled
  Delay(20);      //needed for internal DC-DC converter startup
}

void lcdSetRefVolt(uint8_t val)
{
  if (!lcdInitFinished) {
    lcdInitFinish();
  }
  WAIT_FOR_DMA_END();
  AspiCmd(0x81);	//Set Vop
  AspiCmd(val+CONTRAST_OFS);		//0--255
}

#if defined(REV9E)
void turnBacklightOn(uint8_t level, uint8_t color)
{
  BACKLIGHT_TIMER->CCR1 = ((100-level)*(20-color))/20;
  BACKLIGHT_TIMER->CCR2 = ((100-level)*color)/20;
}

void turnBacklightOff(void)
{
  BACKLIGHT_TIMER->CCR1 = 0;
  BACKLIGHT_TIMER->CCR2 = 0;
}
#elif defined(REVPLUS)
void turnBacklightOn(uint8_t level, uint8_t color)
{
  BACKLIGHT_TIMER->CCR4 = ((100-level)*(20-color))/20;
  BACKLIGHT_TIMER->CCR2 = ((100-level)*color)/20;
}

void turnBacklightOff(void)
{
  BACKLIGHT_TIMER->CCR4 = 0;
  BACKLIGHT_TIMER->CCR2 = 0;
}
#endif
