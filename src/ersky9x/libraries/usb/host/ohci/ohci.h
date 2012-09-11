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

#ifndef _OHCI_H
#define _OHCI_H

#ifdef __ICCARM__          // IAR
#pragma pack(8)
#define __attribute__(...) // IAR
#endif                     // IAR
// Endpoint Descriptor Field Definitions
typedef struct {
    // FunctionAddress | EndpointNumber | Direction | Speed | sKip | Format
    // MaximumPacketSize
    volatile unsigned int Control;
    // TailP: TDQueueTailPointer
    // If TailP and HeadP are the same, then the list contains no TD that the HC
    // can process. If TailP and HeadP are different, then the list contains a TD to be
    // processed.
    volatile unsigned int TailP;
    // HeadP: TDQueueHeadPointer Points to the next TD to be processed for this
    // endpoint.
    volatile unsigned int HeadP;
    // NextED: If nonzero, then this entry points to the next ED on the list
    volatile unsigned int NextEd;
} __attribute__((aligned(16))) OHCIEndpointDescriptor;

/// General Transfer Descriptor Format
typedef struct {
    volatile unsigned int Control;  // bufferRounding | Direction/PID | DelayInterrupt
                                    // DataToggle | ErrorCount | ConditionCode
    volatile unsigned int CBP;      // Current Buffer Pointer
    volatile unsigned int NextTD;   // Next TD
    volatile unsigned int BE;       // Buffer End
} __attribute__((aligned(16))) OHCITransferDescriptor;

typedef struct {
    volatile unsigned int Control; // StartingFrame | DelayInterrupt | FrameCount
                                   // ConditionCode
    volatile unsigned int BP0;     // Buffer Page 0
    volatile unsigned int NextTD;  // NextTD
    volatile unsigned int BE;      // Buffer End
    volatile unsigned int PSW0;    // Offset0/PSW0
    volatile unsigned int PSW1;    // Offset1/PSW1
    volatile unsigned int PSW2;    // Offset2/PSW2
    volatile unsigned int PSW3;    // Offset3/PSW3
    volatile unsigned int PSW4;    // Offset4/PSW4
    volatile unsigned int PSW5;    // Offset5/PSW5
    volatile unsigned int PSW6;    // Offset6/PSW6
    volatile unsigned int PSW7;    // Offset7/PSW7
} __attribute__((aligned(8))) OHCIPeriodicTransferDescriptor;

//Host Controller Communications Area Description
typedef struct {
    // pointers to an Interrupt List each of which is a list of EDs
    volatile unsigned int    UHP_HccaInterruptTable[32];
    // This 16-bit value is updated by the Host Controller on each frame.
    volatile unsigned short  UHP_HccaFrameNumber; // current frame number
    // When the HC updates HccaFrameNumber, it sets this word to 0.
    volatile unsigned short  UHP_HccaPad1;
    // When a TD is complete (with or without an error) it is unlinked from the 
    // queue that it is on and linked to the Done Queue
    volatile unsigned int    UHP_HccaDoneHead;
    // Reserved for use by HC
    volatile unsigned char   reserved[116];
} __attribute__((aligned(8))) OHCI_HCCA;

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR


extern void OHCI_CreateEd( unsigned int EDAddr,
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
                    unsigned int NextED );
extern void OHCI_CreateGenTd( unsigned int GenTdAddr,
                       unsigned int DataToggle,
                       unsigned int DelayInterrupt,
                       unsigned int Direction,
                       unsigned int BufRnding,
                       unsigned int CurBufPtr,
                       unsigned int NextTD,
                       unsigned int BuffLen);
extern void OHCI_CreateGenITd( unsigned int GenTdAddr,
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
                        unsigned int PswOffset7);
#endif //_OHCI_H


