/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef _SAMGUI_WIDGET_CORE_
#define _SAMGUI_WIDGET_CORE_

//#include "common/sam_gui_color.h"
#include "common/sam_gui_font.h"
//#include "common/sam_gui_point.h"
//#include "common/sam_gui_size.h"
#include "disp/disp_backend.h"
//#include "sam-gui/wgt/core/wgt_core_widget.h"
#include "sam-gui/wgt/core/wgt_core_screen.h"
#include "sam-gui/wgt/core/wgt_core_frontend.h"

#include "FreeRTOS.h"
#include "queue.h"

typedef struct _SWGTCoreData
{
    xQueueHandle hMessagesQueue ;

    SDISPBackend* pBE ;

    SWGTFrontend* pTSD ;
    SWGTFrontend* pQTouch ;
    SWGTFrontend* pPotentiometer ;
    SWGTFrontend* pPushbuttons ;

    SWGTScreen* pCurrentScreen ;
    uint32_t dwTimerDelay ;
} SWGTCoreData ;

extern SWGTCoreData g_WGT_CoreData ;

// ------------------------------------------------------------------------------------------------

extern uint32_t WGT_Initialize( void ) ;
extern uint32_t WGT_Start( void ) ;

extern uint32_t WGT_SetTimerPeriod( uint32_t dwDelay ) ;
extern uint32_t WGT_GetTimerPeriod( void ) ;

extern uint32_t WGT_SetCurrentScreen( SWGTScreen* pScreen ) ;

#endif // _SAMGUI_WIDGET_CORE_
