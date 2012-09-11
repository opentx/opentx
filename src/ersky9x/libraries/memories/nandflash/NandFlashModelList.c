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

/**
 * \file
 *
 * Static array of the various NandFlashModels which are supported.
 *
 */
 
/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "NandFlashModelList.h"
#include "NandSpareScheme.h"

/*----------------------------------------------------------------------------
 *        Exported variables
 *----------------------------------------------------------------------------*/

/// List of NandFlash models which can be recognized by the software.
const struct NandFlashModel nandFlashModelList[NandFlashModelList_SIZE] = {

// |  ID    | Options                 | Page    | Mo  | Block   |BlkPg   |DevBlk
    {0x6e,   NandFlashModel_DATABUS8,    256,      1,    4,    &nandSpareScheme256},
	{0x64,   NandFlashModel_DATABUS8,    256,      2,    4,    &nandSpareScheme256},
	{0x6b,   NandFlashModel_DATABUS8,    512,      4,    8,    &nandSpareScheme512},
	{0xe8,   NandFlashModel_DATABUS8,    256,      1,    4,    &nandSpareScheme256},
	{0xec,   NandFlashModel_DATABUS8,    256,      1,    4,    &nandSpareScheme256},
	{0xea,   NandFlashModel_DATABUS8,    256,      2,    4,    &nandSpareScheme256},
	{0xd5,   NandFlashModel_DATABUS8,    512,      4,    8,    &nandSpareScheme512},
	{0xe3,   NandFlashModel_DATABUS8,    512,      4,    8,    &nandSpareScheme512},
	{0xe5,   NandFlashModel_DATABUS8,    512,      4,    8,    &nandSpareScheme512},
	{0xd6,   NandFlashModel_DATABUS8,    512,      8,    8,    &nandSpareScheme512},
                                                              
	{0x39,   NandFlashModel_DATABUS8,    512,      8,    8,    &nandSpareScheme512},
	{0xe6,   NandFlashModel_DATABUS8,    512,      8,    8,    &nandSpareScheme512},
	{0x49,   NandFlashModel_DATABUS16,   512,      8,    8,    &nandSpareScheme512},
	{0x59,   NandFlashModel_DATABUS16,   512,      8,    8,    &nandSpareScheme512},
                                                                 
	{0x33,   NandFlashModel_DATABUS8,    512,     16,   16,    &nandSpareScheme512},
	{0x73,   NandFlashModel_DATABUS8,    512,     16,   16,    &nandSpareScheme512},
	{0x43,   NandFlashModel_DATABUS16,   512,     16,   16,    &nandSpareScheme512},
	{0x53,   NandFlashModel_DATABUS16,   512,     16,   16,    &nandSpareScheme512},
                                                             
	{0x35,   NandFlashModel_DATABUS8,    512,     32,   16,    &nandSpareScheme512},
	{0x75,   NandFlashModel_DATABUS8,    512,     32,   16,    &nandSpareScheme512},
	{0x45,   NandFlashModel_DATABUS16,   512,     32,   16,    &nandSpareScheme512},
	{0x55,   NandFlashModel_DATABUS16,   512,     32,   16,    &nandSpareScheme512},
                                                             
	{0x36,   NandFlashModel_DATABUS8,    512,     64,   16,    &nandSpareScheme512},
	{0x76,   NandFlashModel_DATABUS8,    512,     64,   16,    &nandSpareScheme512},
	{0x46,   NandFlashModel_DATABUS16,   512,     64,   16,    &nandSpareScheme512},
	{0x56,   NandFlashModel_DATABUS16,   512,     64,   16,    &nandSpareScheme512},
                                                             
	{0x78,   NandFlashModel_DATABUS8,    512,    128,   16,    &nandSpareScheme512},
	{0x39,   NandFlashModel_DATABUS8,    512,    128,   16,    &nandSpareScheme512},
	{0x79,   NandFlashModel_DATABUS8,    512,    128,   16,    &nandSpareScheme512},
	{0x72,   NandFlashModel_DATABUS16,   512,    128,   16,    &nandSpareScheme512},
	{0x49,   NandFlashModel_DATABUS16,   512,    128,   16,    &nandSpareScheme512},
	{0x74,   NandFlashModel_DATABUS16,   512,    128,   16,    &nandSpareScheme512},
	{0x59,   NandFlashModel_DATABUS16,   512,    128,   16,    &nandSpareScheme512},
                                                             
	{0x71,   NandFlashModel_DATABUS8,    512,    256,   16,    &nandSpareScheme512},
	
// Large blocks devices. Parameters must be fetched from the extended I
#define OPTIONS     NandFlashModel_COPYBACK                   
                                                                                          
	{0xA2,   NandFlashModel_DATABUS8  | OPTIONS,   0,     64, 0,  &nandSpareScheme2048},
	{0xF2,   NandFlashModel_DATABUS8  | OPTIONS,   0,     64, 0,  &nandSpareScheme2048},
	{0xB2,   NandFlashModel_DATABUS16 | OPTIONS,   0,     64, 0,  &nandSpareScheme2048},
	{0xC2,   NandFlashModel_DATABUS16 | OPTIONS,   0,     64, 0,  &nandSpareScheme2048},
                                                                  
	{0xA1,   NandFlashModel_DATABUS8  | OPTIONS,   0,    128, 0,  &nandSpareScheme2048}, 
	{0xF1,   NandFlashModel_DATABUS8  | OPTIONS,   0,    128, 0,  &nandSpareScheme2048}, 
	{0xB1,   NandFlashModel_DATABUS16 | OPTIONS,   0,    128, 0,  &nandSpareScheme2048},
	{0xC1,   NandFlashModel_DATABUS16 | OPTIONS,   0,    128, 0,  &nandSpareScheme2048},
                                                                  
	{0xAA,   NandFlashModel_DATABUS8  | OPTIONS,   0,    256, 0,  &nandSpareScheme2048},
	{0xDA,   NandFlashModel_DATABUS8  | OPTIONS,   0,    256, 0,   &nandSpareScheme2048},                                              
	{0xBA,   NandFlashModel_DATABUS16 | OPTIONS,   0,    256, 0,  &nandSpareScheme2048},
	{0xCA,   NandFlashModel_DATABUS16 | OPTIONS,   0,    256, 0,  &nandSpareScheme2048},
	                                                                
	{0xAC,   NandFlashModel_DATABUS8  | OPTIONS,   0,    512, 0,  &nandSpareScheme2048}, 
	{0xDC,   NandFlashModel_DATABUS8  | OPTIONS,   0,    512, 0,  &nandSpareScheme2048}, 
	{0xBC,   NandFlashModel_DATABUS16 | OPTIONS,   0,    512, 0,  &nandSpareScheme2048},
	{0xCC,   NandFlashModel_DATABUS16 | OPTIONS,   0,    512, 0,  &nandSpareScheme2048},
                                                                  
	{0xA3,   NandFlashModel_DATABUS8  | OPTIONS,   0,   1024, 0,  &nandSpareScheme2048}, 
	{0xD3,   NandFlashModel_DATABUS8  | OPTIONS,   0,   1024, 0,  &nandSpareScheme2048}, 
	{0xB3,   NandFlashModel_DATABUS16 | OPTIONS,   0,   1024, 0,  &nandSpareScheme2048},
	{0xC3,   NandFlashModel_DATABUS16 | OPTIONS,   0,   1024, 0,  &nandSpareScheme2048},
                                                                  
	{0xA5,   NandFlashModel_DATABUS8  | OPTIONS,   0,   2048, 0,  &nandSpareScheme2048}, 
	{0xD5,   NandFlashModel_DATABUS8  | OPTIONS,   0,   2048, 0,  &nandSpareScheme2048}, 
	{0xB5,   NandFlashModel_DATABUS16 | OPTIONS,   0,   2048, 0,  &nandSpareScheme2048},
	{0xC5,   NandFlashModel_DATABUS16 | OPTIONS,   0,   2048, 0,  &nandSpareScheme2048},
};
