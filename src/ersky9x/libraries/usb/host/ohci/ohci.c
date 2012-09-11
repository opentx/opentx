/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
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

/// OHCI ED, TD HCCA definitions + inlined functions


#include "ohci.h"


//------------------------------------------------------------------------------
/// Init a pre-allocated endpoint descriptor
/// TD must be aligned on a 16 bytes boundary
//------------------------------------------------------------------------------
void OHCI_CreateEd( unsigned int EDAddr,
                    unsigned int MaxPacket,
                    unsigned int TDFormat,
                    unsigned int Skip,
                    unsigned int Speed,
                    unsigned int Direction,
                    unsigned int EndPt,
                    unsigned int FuncAddress,
                    unsigned int TDQTailPntr,
                    unsigned int TDQHeadPntr,
                    unsigned int ToggleCarry,
                    unsigned int NextED )
{
    OHCIEndpointDescriptor *pED = (OHCIEndpointDescriptor*) EDAddr;

    pED->Control = (MaxPacket << 16) | (TDFormat << 15) |
                   (Skip << 14) | (Speed << 13) | (Direction << 11) |
                   (EndPt << 7) | FuncAddress;
    pED->TailP   = (TDQTailPntr & 0xFFFFFFF0);
    pED->HeadP   = (TDQHeadPntr & 0xFFFFFFF0) | (ToggleCarry << 1);
    pED->NextEd  = (NextED & 0xFFFFFFF0);
}

//------------------------------------------------------------------------------
/// Init a pre-allocated transfer descriptor
/// TD must be aligned on a 16 bytes boundary
//------------------------------------------------------------------------------
void OHCI_CreateGenTd( unsigned int GenTdAddr,
                       unsigned int DataToggle,
                       unsigned int DelayInterrupt,
                       unsigned int Direction,
                       unsigned int BufRnding,
                       unsigned int CurBufPtr,
                       unsigned int NextTD,
                       unsigned int BuffLen)
{
    OHCITransferDescriptor *pTD = (OHCITransferDescriptor*) GenTdAddr;

    pTD->Control = (DataToggle << 24) | (DelayInterrupt << 21)
                 | (Direction << 19) | (BufRnding << 18);
    pTD->CBP     = CurBufPtr;
    pTD->NextTD  = (NextTD & 0xFFFFFFF0);
    pTD->BE      = (BuffLen) ? (CurBufPtr + BuffLen - 1) : CurBufPtr;
}

//------------------------------------------------------------------------------
/// Init a pre-allocated periodic transfer descriptor
/// TD must be aligned on a 16 bytes boundary
//------------------------------------------------------------------------------
void OHCI_CreateGenITd( unsigned int GenTdAddr,
                        unsigned int CondCode,
                        unsigned int FrameCount,
                        unsigned int DelayInterrupt,
                        unsigned int StartFrame,
                        unsigned int BuffPage0,
                        unsigned int NextTD,
                        unsigned int BufEnd,
                        unsigned int PswOffset0,
                        unsigned int PswOffset1,
                        unsigned int PswOffset2,
                        unsigned int PswOffset3,
                        unsigned int PswOffset4,
                        unsigned int PswOffset5,
                        unsigned int PswOffset6,
                        unsigned int PswOffset7)
{
    OHCIPeriodicTransferDescriptor *pITD = (OHCIPeriodicTransferDescriptor*) GenTdAddr;

    pITD->Control = (CondCode << 28) | (FrameCount << 24)
                  | (DelayInterrupt << 21) | StartFrame;
    pITD->BP0 = (BuffPage0 << 12);
    pITD->NextTD = (NextTD << 4);
    pITD->BE = BufEnd;
    pITD->PSW0 = PswOffset0;
    pITD->PSW1 = PswOffset1;
    pITD->PSW2 = PswOffset2;
    pITD->PSW3 = PswOffset3;
    pITD->PSW4 = PswOffset4;
    pITD->PSW5 = PswOffset5;
    pITD->PSW6 = PswOffset6;
    pITD->PSW7 = PswOffset7;

}


