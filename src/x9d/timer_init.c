#include "stm32f2xx.h"
#include "timer_init.h"
#include "hal.h"
#include "msg_que.h"
//Hardware Init Of Timer

//TIM10_CH1 LCD backlight
//PWM OUTPUT MODE 500HZ,400HZ闪动。修改TIM10-> CCR1（1-4000）改变占空比，正比。
void TIM10_CH1_BL_Init()
{
    uint16_t PrescalerValue = 0;
  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    GPIO_InitTypeDef GPIO_InitStructure;
    /* TIM10 clock enable ,GPIOBL clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOBL, ENABLE);
  
    /* GPIOC Configuration: TIM10_CH1 (PB.08)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_BL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOBL, &GPIO_InitStructure); 
    /* Connect TIM10 pins to AF3 */  
    GPIO_PinAFConfig(GPIOBL, GPIO_PinSource_BL, GPIO_AF_TIM10);
    
    /*
    APB2 = 120M,分频系数6，TIM10计数频率2M。TIM10频率为2M/4000=500HZ.
     so the TIM10 Channel 1 generates a PWM signal with a frequency equal to 
    500Hz and a duty cycle = (TIM10_CCR1/ TIM10_ARR + 1)* 100 = 50%
    */
    
    /* Compute the prescaler value */
    PrescalerValue = (uint16_t) ((SystemCoreClock ) / 2000000) - 1;
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 3999; //ARR=19999
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);

    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse =2000; //TIM10-> CCR1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM10, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);

    /* TIM10 enable counter */
    TIM_Cmd(TIM10, ENABLE);
}

void TIM10_CH1_BL_Adjust(char mode)
{
  switch(mode)
  {
  case BKL_DEC: 
    TIM10->CCR1 += 250; 
    if(TIM10->CCR1>4000) 
        TIM10->CCR1 = 4000;
    break;
  case BKL_INC: 
    if(TIM10->CCR1 < 250)
        TIM10->CCR1 = 0;
              else 
          TIM10->CCR1 -= 250;    
    break;
  case BKL_OFF: TIM_Cmd(TIM10, DISABLE);break;
  case BKL_ON:  TIM_Cmd(TIM10, ENABLE);break;
  
  default:break;  
  }
}     

//TIM1_CH1:CPPM_OUTPUT
void TIM1_CH1_CPPM_Init()
{
}


static struct TimerStruct timer_struct[MAX_TIMER];
char led_lights;

/*******************************************
	初始化应用层定时器
*******************************************/
static void initTimerStruct(void)
{
	char i;
	for(i=0;i<MAX_TIMER;i++)
	{
		timer_struct[i].valid=0;
	}
}

/*****************************************
* timer hardware init
*****************************************/
void initTimer(void)
{
	initTimerStruct();
}
/********************************************
	定时器 tick 处理
********************************************/
void timer_tick(void)
{
	{
		short i;
		struct TimerStruct *p=timer_struct;

		for(i=0;i<MAX_TIMER;i++)
		{
			if(p->valid){
				p->expires--;
				if(p->expires==0)
				{
					p->valid=0;
					//raiseEvent(p->event);
					postMessage(p->pMsg->hWnd, p->pMsg->iMsg, p->pMsg->wParam, p->pMsg->lParam);
				}
			}
			p++;
		}
	}
}



/****************************************
* 设置定时器
****************************************/
void setTimer(char timerID,char expires,PMSG pMsg)
{
	struct TimerStruct *p=&timer_struct[timerID];
	p->expires=expires;
	p->pMsg =pMsg;
	p->valid=1;
}
/****************************************
* 取消定时器
****************************************/
void cancelTimer(char timerID)
{
	timer_struct[timerID].valid=0;
}































/**************************************************************************
*****************************************************************************
******************************************************************************/

static struct TimerStruct timer_struct[MAX_TIMER];

/*******************************************
	初始化应用层定时器
*******************************************/
static void initTimerStruct(void)
{
	char i;
	for(i=0;i<MAX_TIMER;i++)
	{
		timer_struct[i].valid=0;
	}
}

/*****************************************
* timer hardware init
*****************************************/
void initTimer(void)
{
	initTimerStruct();
}
/********************************************
	定时器 tick 处理
********************************************/
void timer_tick(void)
{
	{
		short i;
		struct TimerStruct *p=timer_struct;

		for(i=0;i<MAX_TIMER;i++)
		{
			if(p->valid){
				p->expires--;
				if(p->expires==0)
				{
					p->valid=0;
					//raiseEvent(p->event);
					postMessage(p->pMsg->hWnd, p->pMsg->iMsg, p->pMsg->wParam, p->pMsg->lParam);
				}
			}
			p++;
		}
	}
}



/****************************************
* 设置定时器
****************************************/
void setTimer(char timerID,char expires,PMSG pMsg)
{
	struct TimerStruct *p=&timer_struct[timerID];
	p->expires=expires;
	p->pMsg =pMsg;
	p->valid=1;
}
/****************************************
* 取消定时器
****************************************/
void cancelTimer(char timerID)
{
	timer_struct[timerID].valid=0;
}

/****************************************************************************
****************************************************************************/