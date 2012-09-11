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

#include "sam-gui/wgt/frontends/frontend_ADS7843.h"
#include "sam-gui/common/sam_gui_errors.h"
#include "board.h"
#include "pio/pio.h"
#include "pio/pio_it.h"
#include "tsd/tsd_ads7843.h"
#include "demo_parameters.h"
#include "sam-gui/porting/sam_gui_porting.h"
#include "sam-gui/wgt/core/wgt_core_message.h"
#include "components/ads7843/ads7843.h"

/* RTOS operations */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <stdio.h>
#include <string.h>

/** \addtogroup Demo_Touchscreen Touchscreen driver
 * The Touchscreen driver posts messages to the Widget message queue:
 * <ul>
 * <li>WGT_MSG_POINTER_RAW: pen pressed, provides uncalibrated-filtered pen position. </li>
 * <li>WGT_MSG_POINTER_PRESSED: pen pressed, provides calibrated-filtered pen position.</li>
 * <li>WGT_MSG_POINTER: pen moved, provides calibrated-filtered pen position.</li>
 * <li>WGT_MSG_POINTER_RELEASED: pen released, provides calibrated-filtered pen position.</li>
 * \n
 *
 * Touchscreen ADS7843 is connected to the microcontroller through SPI and an
 * a signal connected to a PIO corresponding to the pen press detection.
 * Each time a pen press is detected, the microcontroller is notified thrrough 
 * the interrupt line triggering a PIO interrupt.
 * \n
 * 
 * The touchscreen driver starts a task suspended until the PIO interrupt 
 * handler releases a semaphore. Then the task does measurements until the pen
 * is detected as released. 
 * Once the pen is released, the task enters in suspend mode, waiting to get the
 * semaphore ie. waiting for the next pen press detection.
 *
 * Related files :\n
 * \ref ads7843.c\n
 * \ref ads7843.h\n
*/

/*
 * ----------------------------------------------------------------------------
 *         Touchscreen driver parameters
 * ----------------------------------------------------------------------------
*/
#define NB_SAMPLES       3 /**< Number of succesive measurements required to get an accurate pen position */
#define SAMPLING_PERIOD 10 /**< Sampling period (in ms) when pen is pressed to detect pen mouvements */

/** Binary flag to check if the screen has been calibrated. Calibration calculus
  * is not applied since this flag is not true.
  * Only raw values are sent to the application until the calibration has been 
  * done */
uint8_t gTsCalibrated = 0;

static Ads7843CalibParameters gs_ADS7843CalibParameters;

/** Pins used by Interrupt Signal for Touch Screen Controller */
static const Pin _WFE_ADS7843_pinPenIRQ=PIN_TCS_IRQ ;

/** Touch screen semaphore used to resume touch screen task when pen is pressed */
static xSemaphoreHandle gTsSemaphore = NULL;

/**
  * \brief Returns Touchscreen pen position
  * Performs several (NB_SAMPLES succesive measurements and do a mean. Then 
  * calibration function is applied according to the calibration parameters.
*/
static void _WFE_ADS7843_GetPenPosition(uint32_t *pdwX, uint32_t *pdwY)
{
    uint32_t i;
    uint32_t dwX, dwY; /* Measured coordinates */
    uint32_t dwTrueX, dwTrueY; /* Filtered coordinates */
    

    /* Do several successive measurements and process a mean */
    dwTrueX = 0;
    dwTrueY = 0;
    for (i = 0; i < NB_SAMPLES; ++i) {
        ADS7843_GetPosition( &dwX, &dwY ) ;
        dwTrueX += dwX;
        dwTrueY += dwY;
    }
    dwTrueX /= NB_SAMPLES;
    dwTrueY /= NB_SAMPLES;
    
    /* If calibration has been done, apply calibration function */
    if (gTsCalibrated) {
        dwTrueX = gs_ADS7843CalibParameters.dwPointX
                - ((int32_t)(gs_ADS7843CalibParameters.dwMeasureX - (int32_t)dwTrueX) * 1024)
                / gs_ADS7843CalibParameters.lSlopeX ;
        dwTrueY =  gs_ADS7843CalibParameters.dwPointY
                - ((int32_t)(gs_ADS7843CalibParameters.dwMeasureY - (int32_t)dwTrueY) * 1024)
                / gs_ADS7843CalibParameters.lSlopeY ;
    
        if ((int32_t)dwTrueX < 0 ) /* Is pPoint[0] negative ? */
            dwTrueX = 0;
        if ( dwTrueX > BOARD_LCD_WIDTH ) /* Is pPoint[0] bigger than the LCD width ? */
            dwTrueX = BOARD_LCD_WIDTH ;
    
        if ((int32_t)dwTrueY < 0 ) /* Is pPoint[0] negative ? */
            dwTrueY = 0;
        if ( dwTrueY > BOARD_LCD_HEIGHT ) /* Is pPoint[0] bigger than the LCD width ? */
            dwTrueY = BOARD_LCD_HEIGHT ;
    }
    
    /* Update pen position coordinates */
    *pdwX = dwTrueX;
    *pdwY = dwTrueY;   
}

/**
  * \brief Touch screen sampling task
  *
  * This task is resumed when a PIO interrupt release the semaphore, that's to
  * say when the pen is pressed. Then this task reports pen activity each
  * SAMPLING_RATE period until the pen is released
*/
static void _WFE_ADS7843_Task( void* pParameter )
{
    uint8_t  isPenPressed;
    uint32_t dwX, dwPrevX;
    uint32_t dwY, dwPrevY;


    while (1) {
      
        /* Block waiting for the semaphore to become available: */
        /* A PIO interrupt has been triggered: a key has been pressed */
        if( xSemaphoreTake( gTsSemaphore, portMAX_DELAY ) != pdTRUE )
            continue;
        
        /* Add a delay between interrupt and data acquisition */
        vTaskDelay( 5 );

        isPenPressed = 1;
        
        /* Perform a first measurement corresponding to the key pressed */
        _WFE_ADS7843_GetPenPosition(&dwX, &dwY);
        
        /* If it is not calibrated send the raw value to the application */
        if (!gTsCalibrated) {
            WGT_SendMessageISR( WGT_MSG_POINTER_RAW, dwX, dwY);
            isPenPressed = 0;
        }
        else {
            /* Send the key pressed position */
            WGT_SendMessageISR( WGT_MSG_POINTER_PRESSED, dwX, dwY) ;
            dwPrevX = dwX;
            dwPrevY = dwY;
        }
        
        /* Perform measurements at SAMPLING_RATE while the key is pressed */
        while (isPenPressed) {
            vTaskDelay( SAMPLING_PERIOD/portTICK_RATE_MS ) ;

            /* Perform measurements at SAMPLING_RATE */
            _WFE_ADS7843_GetPenPosition(&dwX, &dwY);
            
            /* If the touch has been released exit the loop and post release message */
            if ( PIO_Get( &_WFE_ADS7843_pinPenIRQ ) == 1) {
                WGT_SendMessageISR( WGT_MSG_POINTER_RELEASED, dwPrevX, dwPrevY) ;
                isPenPressed = 0;
            }

            /* Else send the pen position if this one is different from the previous one */
            else if ( (dwX != dwPrevX) || (dwY != dwPrevY) ) {
                WGT_SendMessageISR( WGT_MSG_POINTER, dwX, dwY) ;
                dwPrevX = dwX;
                dwPrevY = dwY;
            }

        }  
        
        /* Loop and wait for PIO ISR to release the semaphore */
            /* Enable the PIO interrupt */
        PIO_EnableIt( &_WFE_ADS7843_pinPenIRQ ) ;
    }
}

/**
 * \brief Interrupt handler for Touchscreen.
 */
#ifdef BOARD_REV_A
void PIOA_Irq4Handler(unsigned char id)
#endif
#ifdef BOARD_REV_B
void PIOA_Irq16Handler(unsigned char id)
#endif
{
    portBASE_TYPE xHigherPriorityTaskWoken;
    
    /* A posedge has been received */
    xSemaphoreGiveFromISR( gTsSemaphore, &xHigherPriorityTaskWoken );
    PIO_DisableIt( &_WFE_ADS7843_pinPenIRQ ) ;
}


static uint32_t _WFE_ADS7843_Initialize( void )
{
    // Initialize touchscreen 
    ADS7843_Initialize() ;

    /* Configure pios */
    PIO_Configure( &_WFE_ADS7843_pinPenIRQ, PIO_LISTSIZE( _WFE_ADS7843_pinPenIRQ ) ) ;

    /* Create the semaphore required for pen detection */
    vSemaphoreCreateBinary( gTsSemaphore );
    if( gTsSemaphore == NULL )
        return SAMGUI_E_NOT_ENOUGH_MEMORY;
    
    /* Take the semaphore, this one will be release in the PIO ISR handler, once the pen will be pressed */
    if( xSemaphoreTake( gTsSemaphore, 0 ) != pdTRUE )
        return SAMGUI_E_NOT_ENOUGH_MEMORY; /* TODO: change return code, it shall correspond to initialization error */
    
    /* Get calibration parameters */
    gs_ADS7843CalibParameters.dwPointX = g_demo_parameters.sCalibration.dwPointX;
    gs_ADS7843CalibParameters.dwPointY = g_demo_parameters.sCalibration.dwPointY;
    gs_ADS7843CalibParameters.dwMeasureX = g_demo_parameters.sCalibration.dwMeasureX;
    gs_ADS7843CalibParameters.dwMeasureY = g_demo_parameters.sCalibration.dwMeasureY;
    gs_ADS7843CalibParameters.lSlopeX = g_demo_parameters.sCalibration.lSlopeX;
    gs_ADS7843CalibParameters.lSlopeY = g_demo_parameters.sCalibration.lSlopeY;

    /* Check if we already have calibration data */
    if (gs_ADS7843CalibParameters.dwPointX) {
        gTsCalibrated = 1;
    }

    /* Enable the PIO interrupt */
    PIO_EnableIt( &_WFE_ADS7843_pinPenIRQ ) ;

    /* Create the touch screen driver task */
    if (xTaskCreate( _WFE_ADS7843_Task, "WGT_Core_TS", 512, NULL, tskIDLE_PRIORITY+2, NULL ) != pdPASS)
        return SAMGUI_E_NOT_ENOUGH_MEMORY;
    
    return SAMGUI_E_OK ;
}

static uint32_t _WFE_ADS7843_IOCtl( uint32_t dwCommand, uint32_t* pdwValue, uint32_t* pdwValueLength )
{
    switch (dwCommand) {
        case WGT_FRONTEND_IOCTL_SET_DATA:
            if ( (pdwValueLength != NULL) && (*pdwValueLength == sizeof( Ads7843CalibParameters )) )
            {
                memcpy( &gs_ADS7843CalibParameters, pdwValue, sizeof( Ads7843CalibParameters ) ) ;
                if ( gs_ADS7843CalibParameters.lSlopeX != 0 )
                {
                    gTsCalibrated=1 ;
                }
                else
                {
                    gTsCalibrated=0 ;
                }
            }
        break;

        default:
        break;
    }
    return SAMGUI_E_OK ;
}

const SWGTFrontend sWGT_Frontend_ADS7843=
{
    .sData={ .dwId=WGT_FRONTEND_ADS7843 },

    .Initialize=_WFE_ADS7843_Initialize,

    .IOCtl=_WFE_ADS7843_IOCtl
} ;