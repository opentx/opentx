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

#include "sam-gui/wgt/core/wgt_core_widget.h"
#include "sam-gui/common/sam_gui_errors.h"
#include "sam-gui/file/file_fs.h"
#include "drivers/utility/bmp.h"
#include "freertos.h"
#include <string.h>
#include <stdio.h>

// ------------------------------------------------------------------------------------------------
// Widget functions
// ------------------------------------------------------------------------------------------------
extern uint32_t WGT_CreateWidget( SWGT_Widget* pWidget, WGT_Type eType, uint32_t dwX, uint32_t dwY, uint32_t dwWidth, uint32_t dwHeight )
{
    if ( pWidget == NULL )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    memset( pWidget, 0, sizeof( SWGT_Widget ) ) ;
    pWidget->dwType=eType ;
    pWidget->dwX=dwX ;
    pWidget->dwY=dwY ;
    pWidget->dwWidth=dwWidth ;
    pWidget->dwHeight=dwHeight ;

    return SAMGUI_E_OK ;
}

extern uint32_t WGT_SetBkgndColor( SWGT_Widget* pWidget, uint32_t dwColor )
{
    if ( pWidget == NULL )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    pWidget->dwClrBackground=dwColor ;

    return SAMGUI_E_OK ;
}

extern uint32_t WGT_SetTextColor( SWGT_Widget* pWidget, uint32_t dwColor )
{
    if ( pWidget == NULL )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    pWidget->dwClrText=dwColor ;

    return SAMGUI_E_OK ;
}

extern uint32_t WGT_SetText( SWGT_Widget* pWidget, char* pszText )
{
    if ( (pWidget == NULL) || (pszText == NULL) )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    pWidget->pvText=pszText ;

    return SAMGUI_E_OK ;
}

extern uint32_t WGT_SetBitmap( SWGT_Widget* pWidget, uint8_t* pucBitmap )
{
    if ( (pWidget == NULL) || (pucBitmap == NULL) )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    pWidget->pvBitmap=pucBitmap ;

    return SAMGUI_E_OK ;
}

//static uint32_t WGT_DrawBMPFile( SWGT_Widget* pWidget, SDISPBackend* pBE )
//{
//    uint32_t dwLine ;
//    uint8_t* pucLine=NULL ;
//    FIL fp ;
//    struct BMPHeader sHeader ;
//    uint32_t dwLineLength ;
//    uint32_t dwLength ;
//    uint32_t dwTemp ;
//
//    if ( (pWidget == NULL) || (pBE == NULL) )
//    {
//        return SAMGUI_E_BAD_POINTER ;
//    }
//
//    // Read first 256 bytes to obtain header
//    if ( f_open( &fp, pWidget->pvBitmap, FA_OPEN_EXISTING|FA_READ ) == FR_OK )
//    {
//        if ( f_read( &fp, &sHeader, sizeof( sHeader ), &dwLength ) == FR_OK )
//        {
////            printf( "read %c%c\r\n", (char)(sHeader.type&0xff), (char)(sHeader.type>>8) ) ;
////            printf( " -> .compression = %u\n\r", sHeader.compression ) ;
////            printf( " -> .width = %u\n\r", sHeader.width ) ;
////            printf( " -> .height = %u\n\r", sHeader.height ) ;
////            printf( " -> .bits = %u\n\r", sHeader.bits ) ;
//
//            dwLineLength=(((sHeader.width*sHeader.bits)+31)/32)*4 ;
//            printf( "free heap %u, needed %u\r\n", xPortGetFreeHeapSize(), dwLineLength ) ;
//
//            // if header is Windows BMP 24bpp, alloc line size bytes on HEAP
//            if ( (sHeader.bits == 24) ) //&& (dwLineLength >= xPortGetFreeHeapSize()) )
//            {
//                // Allocate line buffer
//                pucLine=pvPortMalloc( dwLineLength ) ;
//                if ( pucLine )
//                {
//                    // Move file pointer to begin of BMP data
//                    f_lseek( &fp, sHeader.offset ) ;
//
//                    // Read each bitmap line and write it on backend
//                    for ( dwLine=0 ; dwLine < pWidget->dwHeight ; dwLine++ )
//                    {
//                        if ( f_read( &fp, pucLine, dwLineLength, &dwLength ) == FR_OK )
//                        {
//                            for ( dwLength=0 ; dwLength < dwLineLength ; dwLength+=3 )
//                            {
//                                dwTemp=pucLine[dwLength] ; ;
//                                pucLine[dwLength]=pucLine[dwLength+2] ;
//                                pucLine[dwLength+2]=dwTemp ; ;
//                            }
//
//                            pBE->DrawBitmap( pWidget->dwX, pWidget->dwY+pWidget->dwHeight-dwLine, pWidget->dwWidth, 1, pucLine ) ;
//                        }
//                    }
//
//                    // Free line buffer
//                    vPortFree( pucLine ) ;
//                }
//            }
//        }
//        else
//        {
//            printf( "failed to read\r\n" ) ;
//        }
//        f_close( &fp ) ;
//        return SAMGUI_E_OK ;
//    }
//    else
//    {
//        printf( "failed to open %s\r\n", pWidget->pvBitmap ) ;
//    }
//
//    return SAMGUI_E_FILE_OPEN ;
//}

extern uint32_t WGT_Draw( SWGT_Widget* pWidget, SDISPBackend* pBE )
{
    SGUIColor clr ;

    if ( (pWidget == NULL) || (pBE == NULL) )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    // if bitmap declared, draw it
    if ( pWidget->pvBitmap != NULL )
    {
        // Check whether if bitmap is on external FS or internal RAM
//        if ( ((uint8_t*)(pWidget->pvBitmap))[0] == '/' )
//        {
//            if ( WGT_DrawBMPFile( pWidget, pBE ) != SAMGUI_E_OK )
//            {
//                clr.u.dwRGBA=pWidget->dwClrBackground ;
//
//                pBE->DrawFilledRectangle( pWidget->dwX, pWidget->dwY, pWidget->dwX+pWidget->dwWidth, pWidget->dwY+pWidget->dwHeight, NULL, &clr ) ;
//            }
//        }
//        else
//        {
            pBE->DrawBitmap( pWidget->dwX, pWidget->dwY, pWidget->dwWidth, pWidget->dwHeight, pWidget->pvBitmap ) ;
//        }
    }
    // else if colors, draw background color filled rectangle
    else
    {
        clr.u.dwRGBA=pWidget->dwClrBackground ;

        pBE->DrawFilledRectangle( pWidget->dwX, pWidget->dwY, pWidget->dwX+pWidget->dwWidth, pWidget->dwY+pWidget->dwHeight, NULL, &clr ) ;
    }

    // if label declared, draw text
    if ( (pWidget->pvText != NULL) && (pWidget->dwType == WGT_TYPE_TEXT) )
    {
        clr.u.dwRGBA=pWidget->dwClrText ;

        pBE->DrawText( pWidget->dwX, pWidget->dwY, pWidget->pvText, &clr, (void*)&g_Font10x14, 0 ) ;
    }

    return SAMGUI_E_OK ;
}
