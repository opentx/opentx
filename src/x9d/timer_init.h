#ifndef	_TIMER_INIT_H_
#define	_TIMER_INIT_H_

#include "frgui.h"

#define	MAX_TIMER				(5)			//×î´óµÄtimerID

#define	TIMER_1					(0)
#define	TIMER_2					(1)
#define	TIMER_3					(2)
#define	TIMER_4					(3)
#define	TIMER_5                 (4)

struct TimerStruct
{
	char valid;
	char expires;
	PMSG pMsg;
};

void initTimer(void);
void setTimer(char timerID,char expires,PMSG pMsg);
void cancelTimer(char timerID);
void timer_interrupt(void);
void timer_tick(void);
void TIM10_CH1_BL_Init();

#define BKL_DEC                 (0)
#define BKL_INC                 (1)
#define BKL_OFF                 (2)
#define BKL_ON                  (3)


#endif
