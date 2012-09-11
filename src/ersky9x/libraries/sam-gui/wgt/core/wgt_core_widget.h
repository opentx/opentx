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

#ifndef _SAMGUI_WGT_CORE_WIDGET_
#define _SAMGUI_WGT_CORE_WIDGET_

#include "disp/disp_backend.h"

typedef enum _WGT_Type
{
    WGT_TYPE_BUTTON,
    WGT_TYPE_TEXT,
    WGT_TYPE_BITMAP,
    WGT_TYPE_SLIDER,
    WGT_TYPE_MAX
} WGT_Type ;

typedef enum _WGT_Status
{
    WGT_STATUS_NORMAL,
    WGT_STATUS_SELECTED,
    WGT_STATUS_HIDDEN,
    WGT_STATUS_MAX
} WGT_Status ;

typedef enum _WGT_Style
{
    WGT_STYLE_NONE,
    WGT_STYLE_TEXT_LEFT,
    WGT_STYLE_TEXT_RIGHT,
    WGT_STYLE_TEXT_JUSTIFIED,
    WGT_STYLE_MAX
} WGT_Style ;

typedef struct _SWGT_Widget
{
    WGT_Type dwType ; // Widget type

    uint32_t dwID ; // Widget unique ID

    struct _SWGT_Widget* pParent ;
    struct _SWGT_Widget* pSibling ;
    struct _SWGT_Widget* pChild ;

    uint32_t dwX ;
    uint32_t dwY ;
    uint32_t dwWidth ;
    uint32_t dwHeight ;
    uint32_t dwClrText ;
    uint32_t dwClrBackground ;

    WGT_Style dwStyle ;

    void* pvBitmap ;
    void* pvFont ;
    void* pvText ;

    WGT_Status dwStatus ;
} SWGT_Widget ;

// ------------------------------------------------------------------------------------------------
// Widget functions
extern uint32_t WGT_CreateWidget( SWGT_Widget* pWidget, WGT_Type eType, uint32_t dwX, uint32_t dwY, uint32_t dwWidth, uint32_t dwHeight ) ;

extern uint32_t WGT_SetBkgndColor( SWGT_Widget* pWidget, uint32_t dwColor ) ;
extern uint32_t WGT_SetTextColor( SWGT_Widget* pWidget, uint32_t dwColor ) ;
extern uint32_t WGT_SetText( SWGT_Widget* pWidget, char* pszText ) ;
extern uint32_t WGT_SetBitmap( SWGT_Widget* pWidget, uint8_t* pucBitmap ) ;

//extern uint32_t WGT_DrawBMPFile( SWGT_Widget* pWidget, SDISPBackend* pBE ) ;
extern uint32_t WGT_Draw( SWGT_Widget* pWidget, SDISPBackend* pBE ) ;

#endif // _SAMGUI_WGT_CORE_WIDGET_
