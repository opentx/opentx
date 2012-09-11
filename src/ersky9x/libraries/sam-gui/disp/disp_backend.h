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

#ifndef _SAM_GUI_DISP_BACKEND_
#define _SAM_GUI_DISP_BACKEND_

#include "sam-gui/common/sam_gui_color.h"
#include "sam-gui/common/sam_gui_font.h"

typedef struct _SDISPBackendData
{
    uint32_t dwID ;
    uint32_t dwOrientation ; // Portrait, Landscape
    uint32_t dwMode ; // RGB, BGR
} sDISPBackendData ;

typedef struct _SDISPBackend
{
    sDISPBackendData sData ;

    uint32_t (*Reset)( void ) ;
    uint32_t (*Initialize)( void ) ;
    uint32_t (*GetPixel)( uint32_t dwX, uint32_t dwY, SGUIColor* pclrResult ) ;
    uint32_t (*DrawPixel)( uint32_t dwX, uint32_t dwY, SGUIColor* pclrIn ) ;
    uint32_t (*DrawLine)( uint32_t dwX1, uint32_t dwY1, uint32_t dwX2, uint32_t dwY2, SGUIColor* pclrIn ) ;
    uint32_t (*DrawCircle)( uint32_t dwX, uint32_t dwY, uint32_t dwRadius, SGUIColor* pclrBorder ) ;
    uint32_t (*DrawFilledCircle)( uint32_t dwX, uint32_t dwY, uint32_t dwRadius, SGUIColor* pclrBorder, SGUIColor* pclrInside ) ;
    uint32_t (*DrawRectangle)( uint32_t dwX1, uint32_t dwY1, uint32_t dwX2, uint32_t dwY2, SGUIColor* pclrFrame ) ;
    uint32_t (*DrawFilledRectangle)( uint32_t dwX1, uint32_t dwY1, uint32_t dwX2, uint32_t dwY2, SGUIColor* pclrFrame, SGUIColor* pclrInside ) ;
    uint32_t (*DrawBitmap)( uint32_t dwX, uint32_t dwY, uint32_t dwWidth, uint32_t dwHeight, uint8_t* pucData ) ;
    uint32_t (*DrawText)( uint32_t dwX, uint32_t dwY, uint8_t* pszText, SGUIColor* pclrText, SGUIFont* pFont, uint32_t dwSize ) ;
    uint32_t (*Fill)( uint32_t dwX, uint32_t dwY, SGUIColor* pclrIn ) ;
    uint32_t (*IOCtl)( uint32_t dwCommand, uint32_t* pdwValue, uint32_t* pdwValueLength ) ;
} SDISPBackend ;

#define DISP_BACKEND_IOCTL_POWER_ON              0x01L
#define DISP_BACKEND_IOCTL_POWER_OFF             0x02L
#define DISP_BACKEND_IOCTL_SET_BACKLIGHT         0x03L
#define DISP_BACKEND_IOCTL_SET_MODE_PORTRAIT     0x04L
#define DISP_BACKEND_IOCTL_SET_MODE_LANDSCAPE    0x05L

typedef enum _DISP_eBackend
{
    DISP_BACKEND_HX8347,
    DISP_BACKEND_ILI9325,
    DISP_BACKEND_MAX
} DISP_eBackend ;

extern uint32_t DISP_Initialize( void ) ;
extern uint32_t DISP_GetBackend( uint32_t dwIndex, SDISPBackend** ppBackend ) ;

#endif // _SAM_GUI_DISP_BACKEND_
