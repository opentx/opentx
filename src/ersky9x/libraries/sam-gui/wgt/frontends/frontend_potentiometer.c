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
/** \addtogroup Demo_Potentiomer Potentiometer driver
 * The Potentiometer driver posts messages to the Widget message queue.
 * \n
 *
 * Board pontentiometer is connected to the 12 bit ADC analog line. Each time the
 * user turns the potentiometer, it triggers an interrupt which posts a message
 * with a value from 0 to 255 corresponding to the measurement.
 *
 * The SAMPLING_PERIOD definition controls the sampling frequency. A timer
 * counter in waveform mode is used to trigger acquisitions. It is configured to
 * count from the Slow clock frequency.
 *
 * The window mode of the ADC is used to trigger an ADC interrupt only when the
 * measure goes outside a window controlled by the NB_INTERVALS definition.
 *
 * Each time a new ADC interrupt occurs, a new window is calculated from the current
 * measurement.
 *
 * During the initialization a first measure is done to get the initial value.
 * Application can have access to the current measure using ioctl command.
 *
 * For more accurate information, please look at the ADC section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref adc.c\n
 * \ref adc.h\n
*/

#include <stdint.h>

#include "sam-gui/wgt/frontends/frontend_potentiometer.h"
#include "sam-gui/common/sam_gui_errors.h"
#include "sam-gui/wgt/core/wgt_core_message.h"

#include <cmsis/core_cm3.h>
#include <board.h>
#include <pmc/pmc.h>
#include <adc/adc.h>
#include <tc/tc.h>

/* A message is posted each time the values goes outside the [-5,+5] interval around the previous measure */
#define NB_INTERVALS 5
/** ADC clock */
#define BOARD_ADC_FREQ (6000000)

/** ADC sampling period: 10 times/sec */
#define SAMPLING_PERIOD 32768/10

static uint32_t gdwPotentiometerValue;

/**
 *  \brief TC0 configuration
 *
 * Configures Timer Counter 0 (TC0) to generate an interrupt every second.
 *
 */
static void ConfigureTc0(void)
{

    /* Enable TC0 peripheral clock*/
    PMC_EnablePeripheral(ID_TC0);

    /* Configure TC for a 1s (= 1Hz) tick*/
    TC_Configure(TC0,0, 0x4 | TC_CMR0_ACPC_SET | TC_CMR0_WAVE
            | TC_CMR0_ACPA_CLEAR | (0x2 << 13));

    /* 50% duty ,1s frequency*/
    TC0->TC_CHANNEL[0].TC_RA = SAMPLING_PERIOD/2;
    TC0->TC_CHANNEL[0].TC_RC = SAMPLING_PERIOD;
}


void ADC_IrqHandler(void)
{
    Adc *pAdc = ADC;
    int32_t  dwLowPotThreshold, dwHighPotThreshold;
    volatile uint32_t dummy = 0;

    /* Read the status to ack the IT */
    dummy = ADC_GetStatus(ADC);

    /* Get the potentiometer initial value */
    gdwPotentiometerValue = ADC_GetConvertedData(pAdc, ADC_CHANNEL_5);

    /* Read LCDR to clear DRDY bit */
    dummy = ADC_GetLastConvertedData(pAdc);

    /* Set Window threshold according to the initial values */
    dwLowPotThreshold  = gdwPotentiometerValue - (NB_INTERVALS * (0x1000 / 256));
    if (dwLowPotThreshold < 0)
        dwLowPotThreshold = 0;
    dwHighPotThreshold = gdwPotentiometerValue + (NB_INTERVALS * (0x1000 / 256));
    if (dwHighPotThreshold >= 0x1000)
        dwHighPotThreshold = 0x1000 - 1;

    /* Normalize the value 0 -> 255 */
    gdwPotentiometerValue = (gdwPotentiometerValue * 256) / 0xFFF;

    /* Post Message */
    WGT_SendMessageISR( WGT_MSG_KEY_PRESSED, WGT_KEY_VR1, 255 - gdwPotentiometerValue ) ;

    /* Setup Threshold*/
    ADC_SetComparisonWindow(pAdc,((dwHighPotThreshold<<16)|dwLowPotThreshold));

}

static uint32_t _WFE_Potentiometer_Initialize( void )
{
    Adc *pAdc = ADC;
    int32_t  dwLowPotThreshold, dwHighPotThreshold;

    /* STEP1: Realize a first measure to get potentiometer's initial position */
    /* Initialize ADC*/
    ADC_Initialize( pAdc,ID_ADC, ADC_MR_TRGEN_DIS,/*HARDWARE trigger*/
            0,ADC_MR_SLEEP_NORMAL,ADC_MR_LOWRES_12_BIT,
            BOARD_MCK,BOARD_ADC_FREQ,10,20);

    /*Enable  channel 5 (potentiometer) */
    ADC_EnableChannel(pAdc, ADC_CHANNEL_5);

    /* Start convrsion */
    ADC_StartConversion(pAdc);

    /* Wait for the end of conversion */
    while ( !(ADC_GetStatus(pAdc) & ADC_ISR_EOC5) ) {}

    /* Get the potentiometer initial value */
    gdwPotentiometerValue = ADC_GetConvertedData(pAdc, ADC_CHANNEL_5);

    /* Set Window threshold according to the initial values */
    dwLowPotThreshold  = gdwPotentiometerValue - (NB_INTERVALS * (0x1000 / 256));
    if (dwLowPotThreshold < 0)
        dwLowPotThreshold = 0;
    dwHighPotThreshold = gdwPotentiometerValue + (NB_INTERVALS * (0x1000 / 256));
    if (dwHighPotThreshold >= 0x1000)
        dwHighPotThreshold = 0x1000 - 1;

    /* Normalize the value 0 -> 255 */
    gdwPotentiometerValue = (gdwPotentiometerValue * 256) / 0xFFF;

    /* STEP2: Re configure ADC to use windowing */
    /* Initialize ADC*/
    ADC_Initialize( pAdc,ID_ADC, ADC_MR_TRGEN_EN,/*HARDWARE trigger*/
            ADC_MR_TRGSEL_TIOA0,ADC_MR_SLEEP_NORMAL,ADC_MR_LOWRES_12_BIT,
            BOARD_MCK,BOARD_ADC_FREQ,10,20);

    /*Enable  channel 5 (potentiometer) */
    ADC_EnableChannel(pAdc, ADC_CHANNEL_5);

    /* Configure TC*/
    ConfigureTc0();

    /*Channel 5 has to be compared*/
    ADC_SetCompareChannel(pAdc, ADC_CHANNEL_5);
    /*Compare mode, in the window*/
    ADC_SetCompareMode(pAdc, ADC_EMR_CMPMODE_OUT);

    /* Setup Threshold*/
    ADC_SetComparisonWindow(pAdc,((dwHighPotThreshold<<16)|dwLowPotThreshold));

    /* enable adc interrupt*/
    NVIC_EnableIRQ(ADC_IRQn);

    /* Enable Compare Interrupt*/
    ADC_EnableIt(pAdc, ADC_IDR_COMPE);

    /* Start TC0 and hardware trigger*/
    TC_Start(TC0,0);

    return SAMGUI_E_OK ;
}

static uint32_t _WFE_Potentiometer_IOCtl( uint32_t dwCommand, uint32_t* pdwValue, uint32_t* pdwValueLength )
{
    /* Return the current potentiometer value */
    if (pdwValue) {
        *pdwValue = gdwPotentiometerValue;
        if (pdwValueLength)
            *pdwValueLength = sizeof(gdwPotentiometerValue);
    }
    return SAMGUI_E_OK ;
}

const SWGTFrontend sWGT_Frontend_Potentiometer=
{
    .sData={ .dwId=WGT_FRONTEND_POTENTIOMETER },

    .Initialize=_WFE_Potentiometer_Initialize,

    .IOCtl=_WFE_Potentiometer_IOCtl
} ;
