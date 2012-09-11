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

#ifndef _SAMGUI_WIDGET_CORE_MESSAGE_
#define _SAMGUI_WIDGET_CORE_MESSAGE_

typedef enum _WGT_Message
{
    WGT_MSG_INIT,                 // 0
    WGT_MSG_ERASE_BKGND,          // 1
    WGT_MSG_PAINT,                // 2
    WGT_MSG_TIMER,                // 3
    WGT_MSG_POINTER_RAW,          // 4
    WGT_MSG_POINTER,              // 5
    WGT_MSG_POINTER_PRESSED,      // 6
    WGT_MSG_POINTER_RELEASED,     // 7
    WGT_MSG_WIDGET_SELECTED,      // 8
    WGT_MSG_KEY_PRESSED,          // 9
    WGT_MSG_KEY_RELEASED,         // 10
    WGT_MSG_USER1,                // 11
    WGT_MSG_USER2,
    WGT_MSG_USER3,
    WGT_MSG_USER4,
    WGT_MSG_USER5,
    WGT_MSG_MAX
} WGT_Message ;

typedef enum _SWGT_KeyCode
{
    WGT_KEY_NRST,
    WGT_KEY_PB1,
    WGT_KEY_PB2,
    WGT_KEY_K1,
    WGT_KEY_K2,
    WGT_KEY_K3,
    WGT_KEY_K4,
    WGT_KEY_K5,
    WGT_KEY_S1,
    WGT_KEY_VR1
} SWGT_KeyCode ;

typedef struct _SWGTCoreMessage
{
    uint32_t dwID ;
    uint32_t dwParam1 ;
    uint32_t dwParam2 ;
} SWGTCoreMessage ;

extern uint32_t WGT_SendMessage( uint32_t dwMsgID, uint32_t dwParam1, uint32_t dwParam2 ) ;
extern uint32_t WGT_PostMessage( uint32_t dwMsgID, uint32_t dwParam1, uint32_t dwParam2 ) ;
extern uint32_t WGT_SendMessageISR( uint32_t dwMsgID, uint32_t dwParam1, uint32_t dwParam2 ) ;
extern uint32_t WGT_PostMessageISR( uint32_t dwMsgID, uint32_t dwParam1, uint32_t dwParam2 ) ;

#endif // _SAMGUI_WIDGET_CORE_MESSAGE_
