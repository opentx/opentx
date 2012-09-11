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

#ifndef _SAMGUI_WIDGET_CORE_SCREEN_
#define _SAMGUI_WIDGET_CORE_SCREEN_

#include "sam-gui/wgt/core/wgt_core_widget.h"
#include "sam-gui/wgt/core/wgt_core_message.h"
//#include "sam-gui/wgt/core/wgt_core.h"

#define WGT_MAX_WIDGETS      16

typedef struct _SWGTScreen
{
    // Data members
    SWGT_Widget* apWidgets[WGT_MAX_WIDGETS] ;        // array of widgets
    uint32_t dwWidgets ;                             // number of widgets
    uint32_t dwClrBackground ;                       // screen background color
    uint8_t* pucBmpBackground ;                      // screen background bitmap
    SWGT_Widget* pWidgetOld ;                        // previously selected widget
    SWGT_Widget* pWidgetCurrent ;                    // current selected widget

    // Callbacks
    uint32_t (*ProcessMessage)( struct _SWGTScreen* pScreen, SWGTCoreMessage* pMsg ) ;

    uint32_t (*OnInitialize)( struct _SWGTScreen* pScreen ) ;
    uint32_t (*OnEraseBackground)( struct _SWGTScreen* pScreen ) ;
    uint32_t (*OnPaint)( struct _SWGTScreen* pScreen ) ;
} SWGTScreen ;


// ------------------------------------------------------------------------------------------------
// Screen functions
extern uint32_t WGT_Screen_Initialize( SWGTScreen* pScreen, uint32_t dwClrBackground, uint8_t* pvBmpBackground ) ;
extern uint32_t WGT_Screen_AddWidget( SWGTScreen* pScreen, SWGT_Widget* pWidget ) ;
extern uint32_t WGT_Screen_GetPointedWidget( SWGTScreen* pScreen, uint32_t dwX, uint32_t dwY, SWGT_Widget** ppWidget ) ;

extern uint32_t WGT_Screen_GetSelectedWidget( SWGTScreen* pScreen, SWGT_Widget** ppWidget ) ;
extern uint32_t WGT_Screen_SetSelectedWidget( SWGTScreen* pScreen, SWGT_Widget* pWidget ) ;
//extern uint32_t WGT_Screen_SelectNextWidget( SWGTScreen* pScreen ) ;
//extern uint32_t WGT_Screen_SelectPreviousWidget( SWGTScreen* pScreen ) ;

//
extern uint32_t WGT_Screen_OnPaint( SWGTScreen* pScreen ) ;
extern uint32_t WGT_Screen_OnEraseBackground( SWGTScreen* pScreen ) ;

#if 0
extern uint32_t WGT_Screen_SendMessageID( SWGTScreen* pScreen, uint32_t dwID, uint32_t dwMsgID, uint32_t dwParam1, uint32_t dwParam2 ) ;
extern uint32_t WGT_Screen_SendMessageHdl( SWGTScreen* pScreen, SWGT_Widget* pWidget, uint32_t dwMsgID, uint32_t dwParam1, uint32_t dwParam2 ) ;

extern uint32_t WGT_Screen_PostMessageID( SWGTScreen* pScreen, uint32_t dwID, uint32_t dwMsgID, uint32_t dwParam1, uint32_t dwParam2 ) ;
extern uint32_t WGT_Screen_PostMessageHdl( SWGTScreen* pScreen, SWGT_Widget* pWidget, uint32_t dwMsgID, uint32_t dwParam1, uint32_t dwParam2 ) ;
#endif // 0

#endif // _SAMGUI_WIDGET_CORE_SCREEN_
