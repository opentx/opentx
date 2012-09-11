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

#include "sam-gui/wgt/core/wgt_core_frontend.h"
#include "sam-gui/common/sam_gui_errors.h"
#include "sam-gui/wgt/frontends/frontend_ADS7843.h"
#include "sam-gui/wgt/frontends/frontend_pushbuttons.h"
#include "sam-gui/wgt/frontends/frontend_potentiometer.h"
#include "sam-gui/wgt/frontends/frontend_qtouch.h"
#include <stdlib.h>
#include <stdio.h>

#define TRACE( x ) x
//#define TRACE( x )

static __no_init SWGTFrontend* _aWGT_Frontends[WGT_FRONTEND_MAX] ;

extern uint32_t WGT_Frontend_Initialize( void )
{
    _aWGT_Frontends[WGT_FRONTEND_ADS7843]=(void*)&sWGT_Frontend_ADS7843 ;
    _aWGT_Frontends[WGT_FRONTEND_PUSHBUTTONS]=(void*)&sWGT_Frontend_PushButtons ;
    _aWGT_Frontends[WGT_FRONTEND_POTENTIOMETER]=(void*)&sWGT_Frontend_Potentiometer ;
    _aWGT_Frontends[WGT_FRONTEND_QTOUCH]=(void*)&sWGT_Frontend_QTouch ;

//    for ( int i=0 ; i < WGT_FRONTEND_MAX ; i++ )
//    {
//        TRACE( printf( "WGT_Frontend_Initialize - [%d] %x\r\n", i, _aWGT_Frontends[i] ) ; )
//    }

    return SAMGUI_E_OK ;
}

extern uint32_t WGT_Frontend_GetFrontend( uint32_t dwIndex, SWGTFrontend** ppFrontend )
{
    // Check pointers
    if ( ppFrontend == NULL )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    // Check parameters consistency
    if ( dwIndex >= WGT_FRONTEND_MAX )
    {
        return SAMGUI_E_BAD_PARAMETER ;
    }

    if ( _aWGT_Frontends[dwIndex]->sData.dwId != dwIndex )
    {
        TRACE( printf( "WGT_Frontend_GetFrontend - Inconsistent frontend %x/%x\r\n", dwIndex, _aWGT_Frontends[dwIndex]->sData.dwId ) ; )
        return SAMGUI_E_WRONG_COMPONENT ;
    }

    *ppFrontend=_aWGT_Frontends[dwIndex] ;
//    TRACE( printf( "WGT_Frontend_GetFrontend - %x %x\r\n", _aWGT_Frontends[dwIndex], *ppFrontend ) ; )

    return SAMGUI_E_OK ;
}

