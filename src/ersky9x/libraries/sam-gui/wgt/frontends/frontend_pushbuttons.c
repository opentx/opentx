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
/* PIO functions */
#include <pio/pio.h>
#include <pio/pio_it.h>

#include "sam-gui/wgt/frontends/frontend_pushbuttons.h"
#include "sam-gui/wgt/core/wgt_core_message.h"
#include "sam-gui/common/sam_gui_errors.h"

/** VBus pin instance. */
const Pin gPinButton1 = PIN_PUSHBUTTON_1;
const Pin gPinButton2 = PIN_PUSHBUTTON_2;

static uint32_t _WFE_PushButtons_Initialize( void )
{
    /* Configure PIO */
    PIO_Configure(&gPinButton1, 1);
    PIO_Configure(&gPinButton2, 1);
    
    /* Enable PIO interrupt */
    PIO_EnableIt(&gPinButton1);
    PIO_EnableIt(&gPinButton2);

    return SAMGUI_E_OK ;
}

static uint32_t _WFE_PushButtons_IOCtl( uint32_t dwCommand, uint32_t* pdwValue, uint32_t* pdwValueLength )
{
    return SAMGUI_E_OK ;
}

const SWGTFrontend sWGT_Frontend_PushButtons=
{
    .sData={ .dwId=WGT_FRONTEND_PUSHBUTTONS },

    .Initialize=_WFE_PushButtons_Initialize,

    .IOCtl=_WFE_PushButtons_IOCtl
} ;
