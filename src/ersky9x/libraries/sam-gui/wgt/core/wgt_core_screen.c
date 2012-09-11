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

#include "sam-gui/wgt/core/wgt_core_screen.h"
#include "sam-gui/common/sam_gui_errors.h"
#include "common/sam_gui_color.h"
#include "sam-gui/wgt/core/wgt_core.h"
#include "sam-gui/common/sam_gui_colors.h"
#include "board.h"
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// ------------------------------------------------------------------------------------------------
// Screen functions
// ------------------------------------------------------------------------------------------------
extern uint32_t WGT_Screen_Initialize( SWGTScreen* pScreen, uint32_t dwClrBackground, uint8_t* pucBmpBackground )
{
    memset( pScreen->apWidgets, 0, sizeof( pScreen->apWidgets ) ) ;
    pScreen->dwWidgets=0 ;
    pScreen->pWidgetOld=NULL ;
    pScreen->pWidgetCurrent=NULL ;

    pScreen->dwClrBackground=dwClrBackground ;
    pScreen->pucBmpBackground=pucBmpBackground ;

    return SAMGUI_E_OK ;
}

extern uint32_t WGT_Screen_AddWidget( SWGTScreen* pScreen, SWGT_Widget* pWidget )
{
    if ( pScreen->dwWidgets == WGT_MAX_WIDGETS )
    {
        printf( "WGT_Screen_AddWidget - too much\r\n" ) ;
        return SAMGUI_E_TOO_MUCH_WIDGETS ;
    }

    pScreen->apWidgets[pScreen->dwWidgets]=pWidget ;
    pWidget->dwID=pScreen->dwWidgets ;

    pScreen->dwWidgets++ ;

    return SAMGUI_E_OK ;
}

extern uint32_t WGT_Screen_GetPointedWidget( SWGTScreen* pScreen, uint32_t dwX, uint32_t dwY, SWGT_Widget** ppWidget )
{
    uint32_t dw ;

    if ( pScreen == NULL )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    for ( dw=0 ; dw < pScreen->dwWidgets ; dw++ )
    {
        if ( pScreen->apWidgets[dw] )
        {
            // Check bounding box
            if ( (dwX >= pScreen->apWidgets[dw]->dwX) && (dwX < pScreen->apWidgets[dw]->dwX+pScreen->apWidgets[dw]->dwWidth) &&
                 (dwY >= pScreen->apWidgets[dw]->dwY) && (dwY < pScreen->apWidgets[dw]->dwY+pScreen->apWidgets[dw]->dwHeight) )
            {
                if ( pScreen->apWidgets[dw]->dwType == WGT_TYPE_BUTTON )
                {
                    if ( ppWidget != NULL )
                    {
                        *ppWidget=pScreen->apWidgets[dw] ;
                    }

                    return SAMGUI_E_OK ;
                }
            }
        }
    }

    return SAMGUI_E_NO_SELECTED_WIDGET ;
}

extern uint32_t WGT_Screen_GetSelectedWidget( SWGTScreen* pScreen, SWGT_Widget** ppWidget )
{
    if ( pScreen == NULL )
    {
        return SAMGUI_E_BAD_PARAMETER ;
    }

    if ( ppWidget != NULL )
    {
        *ppWidget=pScreen->pWidgetCurrent ;

        return SAMGUI_E_OK ;
    }

    return SAMGUI_E_BAD_POINTER ;
}

extern uint32_t WGT_Screen_SetSelectedWidget( SWGTScreen* pScreen, SWGT_Widget* pWidget )
{
    uint32_t dw ;
    SGUIColor clrSelection={ .u.dwRGBA=GUICLR_ATMEL_BLUE } ;

    if ( pScreen == NULL )
    {
        return SAMGUI_E_BAD_PARAMETER ;
    }

    for ( dw=0 ; dw < pScreen->dwWidgets ; dw++ )
    {
        if ( pScreen->apWidgets[dw] == pWidget )
        {
            pScreen->pWidgetCurrent=pWidget ;

            if ( pScreen->pWidgetOld != pScreen->pWidgetCurrent )
            {
                printf( "Widget changed\r\n" ) ;
                // Selected widget changed, repaint previous one
                if ( pScreen->pWidgetOld != NULL )
                {
                    WGT_Draw( pScreen->pWidgetOld, g_WGT_CoreData.pBE ) ;
                }

                g_WGT_CoreData.pBE->DrawRectangle( pWidget->dwX, pWidget->dwY, pWidget->dwX+pWidget->dwWidth-1, pWidget->dwY+pWidget->dwHeight-1, &clrSelection ) ;
                pScreen->pWidgetOld=pScreen->pWidgetCurrent ;
            }

            return SAMGUI_E_OK ;
        }
    }

    return SAMGUI_E_INVALID_WIDGET ;
}

//extern uint32_t WGT_Screen_SelectNextWidget( SWGTScreen* pScreen )
//{
//    uint32_t dw ;
//    uint32_t dwIndex=WGT_MAX_WIDGETS ;
//    uint32_t dwIndexNext=WGT_MAX_WIDGETS ;
//
//    SGUIColor clrSelection={ .u.dwRGBA=GUICLR_ATMEL_BLUE } ;
//
//    if ( pScreen == NULL )
//    {
//        return SAMGUI_E_BAD_PARAMETER ;
//    }
//
//    for ( dw=0 ; dw < pScreen->dwWidgets ; dw++ )
//    {
//        if ( pScreen->apWidgets[dw] == pScreen->pWidgetCurrent )
//        {
//            // Got selected
//            dwIndex=dw ;
//            break ;
//        }
//    }
//
//    // Did we find the current selected button?
//    if ( dwIndex != WGT_MAX_WIDGETS )
//    {
//        // Find next button
//        for ( dw=dwIndex ; dw < pScreen->dwWidgets ; dw++ )
//        {
//            if ( pScreen->apWidgets[dw]->dwType == WGT_TYPE_BUTTON )
//            {
//                // Got selected
//                dwIndexNext=dw ;
//                break ;
//            }
//        }
//
//    }
//
//    return SAMGUI_E_OK ;
//}
//
//extern uint32_t WGT_Screen_SelectPreviousWidget( SWGTScreen* pScreen )
//{
//    return SAMGUI_E_OK ;
//}

extern uint32_t WGT_Screen_OnPaint( SWGTScreen* pScreen )
{
    uint32_t dw ;

    for ( dw=0 ; dw < pScreen->dwWidgets ; dw++ )
    {
        if ( pScreen->apWidgets[dw] )
        {
            WGT_Draw( pScreen->apWidgets[dw], g_WGT_CoreData.pBE ) ;
        }
    }

    return SAMGUI_E_OK ;
}

extern uint32_t WGT_Screen_OnEraseBackground( SWGTScreen* pScreen )
{
    if ( pScreen->pucBmpBackground == NULL )
    {
        SGUIColor clr={ .u.dwRGBA=pScreen->dwClrBackground } ;

        g_WGT_CoreData.pBE->DrawFilledRectangle( 0, 0, BOARD_LCD_WIDTH, BOARD_LCD_HEIGHT, NULL, &clr ) ;
    }
    else
    {
        g_WGT_CoreData.pBE->DrawBitmap( 0, 0, BOARD_LCD_WIDTH, BOARD_LCD_HEIGHT, pScreen->pucBmpBackground ) ;
    }

    return SAMGUI_E_OK ;
}

#if 0
extern uint32_t WGT_Screen_SendMessageID( SWGTScreen* pScreen, uint32_t dwID, uint32_t dwMsgID, uint32_t dwParam1, uint32_t dwParam2 )
{
    SWGTCoreMessage sMsg ;

    sMsg.dwID=dwMsgID ;
    sMsg.dwParam1=dwParam1 ;
    sMsg.dwParam2=dwParam2 ;

    // Send message
    if ( xQueueSendToFront( g_WGT_CoreData.hMessagesQueue, &sMsg, ( portTickType )10 ) != pdPASS )
    {
        return SAMGUI_E_MSQUEUE ;
    }

    return SAMGUI_E_OK ;
}

extern uint32_t WGT_Screen_SendMessageHdl( SWGTScreen* pScreen, SWGT_Widget* pWidget, uint32_t dwMsgID, uint32_t dwParam1, uint32_t dwParam2 )
{
    SWGTCoreMessage sMsg ;

    sMsg.dwID=dwMsgID ;
    sMsg.dwParam1=dwParam1 ;
    sMsg.dwParam2=dwParam2 ;

    // Send message
    if ( xQueueSendToFront( g_WGT_CoreData.hMessagesQueue, &sMsg, ( portTickType )10 ) != pdPASS )
    {
        return SAMGUI_E_MSQUEUE ;
    }

    return SAMGUI_E_OK ;
}

extern uint32_t WGT_Screen_PostMessageID( SWGTScreen* pScreen, uint32_t dwID, uint32_t dwMsgID, uint32_t dwParam1, uint32_t dwParam2 )
{
    SWGTCoreMessage sMsg ;

    sMsg.dwID=dwMsgID ;
    sMsg.dwParam1=dwParam1 ;
    sMsg.dwParam2=dwParam2 ;

    // Post message
    if ( xQueueSendToBack( g_WGT_CoreData.hMessagesQueue, &sMsg, ( portTickType )0 ) != pdPASS )
    {
        return SAMGUI_E_MSQUEUE ;
    }

    return SAMGUI_E_OK ;
}

extern uint32_t WGT_Screen_PostMessageHdl( SWGTScreen* pScreen, SWGT_Widget* pWidget, uint32_t dwMsgID, uint32_t dwParam1, uint32_t dwParam2 )
{
    SWGTCoreMessage sMsg ;

    sMsg.dwID=dwMsgID ;
    sMsg.dwParam1=dwParam1 ;
    sMsg.dwParam2=dwParam2 ;

    // Post message
    if ( xQueueSendToBack( g_WGT_CoreData.hMessagesQueue, &sMsg, ( portTickType )0 ) != pdPASS )
    {
        return SAMGUI_E_MSQUEUE ;
    }

    return SAMGUI_E_OK ;
}
#endif // 0
