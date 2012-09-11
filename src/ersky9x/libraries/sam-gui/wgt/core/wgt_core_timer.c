#include "sam-gui/wgt/core/wgt_core_timer.h"
#include "porting/sam_gui_porting.h"
#include "sam-gui/common/sam_gui_errors.h"
#include <string.h>

static __no_init SWGTTimer* gs_apWGTTimers[WGT_TIMER_MAX] ;

extern uint32_t WGT_Timer_Create( SWGTTimer* pTimer, uint32_t dwID, uint32_t dwDelay )
{
    return SAMGUI_E_OK ;
}

extern uint32_t WGT_Timer_Start( SWGTTimer* pTimer )
{
    return SAMGUI_E_OK ;
}

extern uint32_t WGT_Timer_Stop( SWGTTimer* pTimer )
{
    return SAMGUI_E_OK ;
}

extern uint32_t WGT_Timer_Initialize( void )
{
    memset( gs_apWGTTimers, 0, sizeof( gs_apWGTTimers ) ) ;

    return SAMGUI_E_OK ;
}

extern uint32_t WGT_Timer_Process( void )
{
    return SAMGUI_E_OK ;
}
