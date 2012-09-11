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

#include "sam-gui/wgt/core/wgt_core.h"
#include "sam-gui/common/sam_gui_errors.h"
#include "sam-gui/porting/sam_gui_porting.h"
#include "disp/disp_backend.h"
#include "sam-gui/common/sam_gui_font.h"
#include "tsd/tsd_ads7843.h"
#include "sam-gui/common/sam_gui_colors.h"
#include "sam-gui/wgt/core/wgt_core_behaviour.h"

#include <string.h>
#include <stdio.h>

#include "board.h"
#include "lcd/color.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define WGT_CORE_MSG_QUEUE_SIZE    32

SWGTCoreData g_WGT_CoreData ;

// ------------------------------------------------------------------------------------------------

/** \brief
*/
static void _WGT_TaskMessageLoop( void* pParameter )
{
    SWGTCoreData* pData=(SWGTCoreData*)pParameter ;
    SWGTCoreMessage xMessage ;

    WGT_Start() ;

    for ( ; ; )
    {
        // Wait for a widget message

//        if ( xQueueReceive( pData->hMessagesQueue, &xMessage, portMAX_DELAY ) == pdPASS )
        if ( xQueueReceive( pData->hMessagesQueue, &xMessage, g_WGT_CoreData.dwTimerDelay/portTICK_RATE_MS ) == pdTRUE )
        {
            if ( g_WGT_CoreData.pCurrentScreen != NULL )
            {
                PreProcessMessage_Default( g_WGT_CoreData.pCurrentScreen, &xMessage ) ;
            }
        }
        else // delay, send timer
        {
            if ( g_WGT_CoreData.pCurrentScreen != NULL )
            {
                xMessage.dwID=WGT_MSG_TIMER ;
                if ( g_WGT_CoreData.pCurrentScreen->ProcessMessage( g_WGT_CoreData.pCurrentScreen, &xMessage ) == SAMGUI_E_OK )
                {
                }
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------

/** \brief
*/
extern uint32_t WGT_Initialize( void )
{
    uint32_t dwError ;

    // Set Timer Delay to 1s
    g_WGT_CoreData.dwTimerDelay=1000 ;

	// Create the queue used by the Messages task.
	g_WGT_CoreData.hMessagesQueue=xQueueCreate( WGT_CORE_MSG_QUEUE_SIZE, sizeof( SWGTCoreMessage ) ) ;

	// Create the Messages task.
	xTaskCreate( _WGT_TaskMessageLoop, "WGT_ML", 512, &g_WGT_CoreData, tskIDLE_PRIORITY+1, NULL ) ;

    // Initialize LCD backend.
    dwError=DISP_GetBackend( DISP_BACKEND_ILI9325, &g_WGT_CoreData.pBE ) ;
    if ( dwError != SAMGUI_E_OK )
    {
        printf( "_TaskLCD - Error getting Backend (%x)\r\n", dwError ) ;
        return dwError ;
    }

    // Initialize frontends.
    WGT_Frontend_Initialize() ;

    return SAMGUI_E_OK ;
}

/** \brief
*/
extern uint32_t WGT_Start( void )
{
    SGUIColor clrWhite={ .u.dwRGBA=COLOR_WHITE } ;

    // Initialize backend.
    g_WGT_CoreData.pBE->IOCtl( DISP_BACKEND_IOCTL_POWER_OFF, NULL, NULL ) ;
    g_WGT_CoreData.pBE->Initialize() ;

//    g_WGT_CoreData.pBE->IOCtl( DISP_BACKEND_IOCTL_SET_BACKLIGHT, (uint32_t*)1, NULL ) ;

	g_WGT_CoreData.pBE->DrawFilledRectangle( 0, 0, BOARD_LCD_WIDTH, BOARD_LCD_HEIGHT, NULL, &clrWhite ) ;
    g_WGT_CoreData.pBE->IOCtl( DISP_BACKEND_IOCTL_POWER_ON, NULL, NULL ) ;

    // Initialize frontends.
    if ( WGT_Frontend_GetFrontend( WGT_FRONTEND_ADS7843, &g_WGT_CoreData.pTSD ) == SAMGUI_E_OK )
    {
        g_WGT_CoreData.pTSD->Initialize() ;
    }

    if ( WGT_Frontend_GetFrontend( WGT_FRONTEND_PUSHBUTTONS, &g_WGT_CoreData.pPushbuttons ) == SAMGUI_E_OK )
    {
        g_WGT_CoreData.pPushbuttons->Initialize() ;
    }

    if ( WGT_Frontend_GetFrontend( WGT_FRONTEND_POTENTIOMETER, &g_WGT_CoreData.pPotentiometer ) == SAMGUI_E_OK )
    {
        g_WGT_CoreData.pPotentiometer->Initialize() ;
    }

    if ( WGT_Frontend_GetFrontend( WGT_FRONTEND_QTOUCH, &g_WGT_CoreData.pQTouch ) == SAMGUI_E_OK )
    {
        g_WGT_CoreData.pQTouch->Initialize() ;
    }

    return SAMGUI_E_OK ;
}

/** \brief
*/
extern uint32_t WGT_SetTimerPeriod( uint32_t dwDelay )
{
    uint32_t dw ;

    dw=g_WGT_CoreData.dwTimerDelay ;
    g_WGT_CoreData.dwTimerDelay=dwDelay ;

    return dw ;
}

/** \brief
*/
extern uint32_t WGT_GetTimerPeriod( void )
{
    return g_WGT_CoreData.dwTimerDelay ;
}

/** \brief
*/
extern uint32_t WGT_SetCurrentScreen( SWGTScreen* pScreen )
{
    if ( pScreen == NULL )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    g_WGT_CoreData.pCurrentScreen=pScreen ;

    WGT_PostMessage( WGT_MSG_INIT, 0, 0 ) ;
    WGT_PostMessage( WGT_MSG_ERASE_BKGND, 0, 0 ) ;
    WGT_PostMessage( WGT_MSG_PAINT, 0, 0 ) ;

    return SAMGUI_E_OK ;
}




