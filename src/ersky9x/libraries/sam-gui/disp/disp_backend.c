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

#include "disp/disp_backend.h"
#include "disp/backends/ILI9325/backend_ILI9325.h"
#include "disp/backends/HX8347/backend_HX8347.h"
#include "sam-gui/common/sam_gui_errors.h"
#include <stdlib.h>
#include <stdio.h>

//#define TRACE( x ) x
#define TRACE( x )

static __no_init SDISPBackend* _aDISP_Backends[DISP_BACKEND_MAX] ;

extern uint32_t DISP_Initialize( void )
{
    _aDISP_Backends[DISP_BACKEND_HX8347]=NULL ; //&sDISP_Backend_HX8347 ;
    _aDISP_Backends[DISP_BACKEND_ILI9325]=&sDISP_Backend_ILI9325 ;

    for ( int i=0 ; i < DISP_BACKEND_MAX ; i++ )
    {
        TRACE( printf( "DISP_Initialize - [%d] %x\r\n", i, _aDISP_Backends[i] ) ; )
    }

    return SAMGUI_E_OK ;
}

extern uint32_t DISP_GetBackend( uint32_t dwIndex, SDISPBackend** ppBackend )
{
    // Check pointers
    if ( ppBackend == NULL )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    // Check parameters consistency
    if ( dwIndex >= DISP_BACKEND_MAX )
    {
        return SAMGUI_E_BAD_PARAMETER ;
    }

    if ( _aDISP_Backends[dwIndex]->sData.dwID != dwIndex )
    {
        TRACE( printf( "DISP_GetBackend - Inconsistent backend %x\r\n", dwIndex ) ; )
        return SAMGUI_E_WRONG_COMPONENT ;
    }

    *ppBackend=_aDISP_Backends[dwIndex] ;
    TRACE( printf( "DISP_GetBackend - %x %x\r\n", _aDISP_Backends[dwIndex], *ppBackend ) ; )

    return SAMGUI_E_OK ;
}

