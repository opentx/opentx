#include "../open9x.h"

void Buttons_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* GPIOC GPIOD clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOBUTTON, ENABLE);
  
    /* GPIO  Configuration*/
    GPIO_InitStructure.GPIO_Pin = PIN_BUTTON_UP | PIN_BUTTON_ENTER | PIN_BUTTON_DOWN | PIN_TRIM3_UP | PIN_TRIM3_DN 
                                  | PIN_TRIM4_UP |PIN_TRIM4_DN | PIN_SW_A_L | PIN_SW_D_L | PIN_SW_F_H | PIN_SW_G_H
                                    | PIN_SW_G_L | PIN_SW_H_L;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    GPIO_InitStructure.GPIO_Pin = PIN_HOME | PIN_ESC | PIN_PAGE;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//复位状态，IN时应该忽略
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = PIN_TRIM1_UP | PIN_TRIM1_DN | PIN_TRIM2_UP | PIN_TRIM2_DN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = PIN_SW_B_H | PIN_SW_B_L | PIN_SW_C_H | PIN_SW_C_L | PIN_SW_D_H | PIN_SW_E_H | PIN_SW_F_L;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = PIN_SW_E_L;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
}

//short pin_status=0;
//short pin_status1=0;

void detect_buttons()
{
				/*short t_pin;short t_pin1;
				static short key_down_interval;
				key_down_interval++;
                
				t_pin = (~(GPIOC->IDR)) &(PIN_BUTTON_UP | PIN_BUTTON_ENTER | PIN_BUTTON_DOWN);
				t_pin1=(~(GPIOB->IDR)) &(PIN_POWER | PIN_BIND); 
                
                  if( (t_pin1 & PIN_BIND)  && (!(pin_status1 & PIN_BIND))&&(!(t_pin1 & PIN_POWER)) )
                  {
                  }
                  if( (t_pin1 & PIN_POWER)  && (!(pin_status1 & PIN_POWER)))
                  {
                                 
                  }
                  if( (pin_status1 & PIN_BIND)  && (pin_status1 & PIN_POWER ))
                  {
                      static char t_pin_bind;
                      t_pin_bind++;//这个变量
                      if( (t_pin_bind ==39) || (t_pin_bind==69)){
						t_pin_bind =40;
						//SetIDProc(0,0,0,t_pin_bind);
                      }
                  }
                
				if( (t_pin & PIN_BUTTON_UP)  && (!(pin_status & PIN_BUTTON_UP )) )
				{
                 
				}
				if( (t_pin & PIN_BUTTON_ENTER)  && (!(pin_status & PIN_BUTTON_ENTER )) )
				{
                  
				}
				if( (t_pin & PIN_BUTTON_DOWN)  && (!(pin_status & PIN_BUTTON_DOWN )) )
				{
                }
        //////////////////////////////////////////////////////////////////////////////////////                        
 				if( (pin_status & PIN_BUTTON_UP))
				{
					static char t_pin_up;
					t_pin_up++;//这个变量
					if( (t_pin_up ==9) || (t_pin_up==22)){
						t_pin_up =10;
						
					}else if((!(t_pin & PIN_BUTTON_UP)))
					{
					}
				}

				if( (pin_status & PIN_BUTTON_ENTER))
				{
					static char t_pin_enter;
					t_pin_enter++;
					if( (t_pin_enter ==9) || (t_pin_enter ==22)){
						t_pin_enter =10;
						
					}else if((!(t_pin & PIN_BUTTON_ENTER))){
						if(t_pin_enter<9)
							
						t_pin_enter =0;
					}
				}

				if( (pin_status & PIN_BUTTON_DOWN))
				{
					static char t_pin_down;
					t_pin_down++;
					if( (t_pin_down ==9) || (t_pin_down==22) ){
						t_pin_down =10;
						
					}else if((!(t_pin & PIN_BUTTON_DOWN))){
						if(t_pin_down<9)
							
						t_pin_down =0;
					}
				}
				
				pin_status = t_pin;
                pin_status1=t_pin1;*/
}
