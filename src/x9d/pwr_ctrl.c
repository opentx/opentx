#include "hal.h"
#include "stm32f2xx.h"
#include "pwr_ctrl.h"

void pwr_ctrl_down()
{
  GPIO_SetBits(GPIOPWR,PIN_MCU_PWR);
  //ee_save_data();
  GPIO_ResetBits(GPIOPWR,PIN_MCU_PWR);
}

void pwr_ctrl_on()
{

}

void check_pwr() //定时抑或中断，再测试
{
  if( GPIO_ReadInputDataBit(GPIOPWR,PIN_PWR_STATUS) == Bit_RESET)
  {
    pwr_ctrl_down();   
  }
}


void pwr_io_init()  
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* GPIOC GPIOD clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOPWR, ENABLE);
  
    /* GPIO  Configuration*/
    GPIO_InitStructure.GPIO_Pin = PIN_MCU_PWR | PIN_PWR_ON;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOPWR, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin = PIN_PWR_STATUS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOPWR, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = PIN_PWR_LED;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOPWRLED, &GPIO_InitStructure);
}
