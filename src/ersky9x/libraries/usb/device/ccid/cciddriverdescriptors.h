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

//------------------------------------------------------------------------------
//  Title: cciddriverdescriptors.h
//
//  About: Purpose
//      Definitions of the descriptors required by the ccid device driver.
//      DWG_Smart-Card_CCID_Rev110.pdf
//------------------------------------------------------------------------------

#ifndef CCID_DRIVER_DESCRIPTORS_H
#define CCID_DRIVER_DESCRIPTORS_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Constants: Endpoints
//   CCID_EPT_DATA_OUT endpoint data out bulk     1
//   CCID_EPT_DATA_IN endpoint data in bulk       2
//   CCID_EPT_NOTIFICATION endpoint data interupt 3
//------------------------------------------------------------------------------
#define CCID_EPT_DATA_OUT       1  
#define CCID_EPT_DATA_IN        2
#define CCID_EPT_NOTIFICATION   3

//------------------------------------------------------------------------------
// USB-ICC protocol
//------------------------------------------------------------------------------
// CCID specification version 1.10
#define CCID1_10                               0x0110

#define SMART_CARD_DEVICE_CLASS                0x0B
// Smart Card Device Class Descriptor Type
#define CCID_DECRIPTOR_TYPE                    0x21

// Table 5.3-1 Summary of CCID Class Specific Request
#define CCIDGenericRequest_ABORT                 0x01
#define CCIDGenericRequest_GET_CLOCK_FREQUENCIES 0x02
#define CCIDGenericRequest_GET_DATA_RATES        0x03

// 6.1 Command Pipe, Bulk-OUT Messages
#define PC_TO_RDR_ICCPOWERON                   0x62
#define PC_TO_RDR_ICCPOWEROFF                  0x63
#define PC_TO_RDR_GETSLOTSTATUS                0x65
#define PC_TO_RDR_XFRBLOCK                     0x6F
#define PC_TO_RDR_GETPARAMETERS                0x6C
#define PC_TO_RDR_RESETPARAMETERS              0x6D
#define PC_TO_RDR_SETPARAMETERS                0x61
#define PC_TO_RDR_ESCAPE                       0x6B
#define PC_TO_RDR_ICCCLOCK                     0x6E
#define PC_TO_RDR_T0APDU                       0x6A
#define PC_TO_RDR_SECURE                       0x69
#define PC_TO_RDR_MECHANICAL                   0x71
#define PC_TO_RDR_ABORT                        0x72
#define PC_TO_RDR_SETDATARATEANDCLOCKFREQUENCY 0x73

// 6.2 Response Pipe, Bulk-IN Messages
#define RDR_TO_PC_DATABLOCK                    0x80
#define RDR_TO_PC_SLOTSTATUS                   0x81
#define RDR_TO_PC_PARAMETERS                   0x82
#define RDR_TO_PC_ESCAPE                       0x83
#define RDR_TO_PC_DATARATEANDCLOCKFREQUENCY    0x84

// 6.3 Interrupt-IN Messages
#define RDR_TO_PC_NOTIFYSLOTCHANGE             0x50
#define RDR_TO_PC_HARDWAREERROR                0x51

// Table 6.2-2 Slot error register when bmCommandStatus = 1
#define CMD_ABORTED                                   0xFF
#define ICC_MUTE                                      0xFE
#define XFR_PARITY_ERROR                              0xFD
#define XFR_OVERRUN                                   0xFC
#define HW_ERROR                                      0xFB
#define BAD_ATR_TS                                    0xF8
#define BAD_ATR_TCK                                   0xF7
#define ICC_PROTOCOL_NOT_SUPPORTED                    0xF6
#define ICC_CLASS_NOT_SUPPORTED                       0xF5
#define PROCEDURE_BYTE_CONFLICT                       0xF4
#define DEACTIVATED_PROTOCOL                          0xF3
#define BUSY_WITH_AUTO_SEQUENCE                       0xF2
#define PIN_TIMEOUT                                   0xF0
#define PIN_CANCELLED                                 0xEF
#define CMD_SLOT_BUSY                                 0xE0
//      User defined                              0xC0 to 0x81
//      Reserved for futur use                        0x80
//      not supported incorrect message parameter 0x7F to 0x01
//      Command not supported                         0x00

// CCID rev 1.1, p.27
#define VOLTS_AUTO                            0x00
#define VOLTS_5_0                             0x01
#define VOLTS_3_0                             0x02
#define VOLTS_1_8                             0x03

// 6.3.1 RDR_to_PC_NotifySlotChange
#define ICC_NOT_PRESENT                       0x00
#define ICC_PRESENT                           0x01
#define ICC_CHANGE                            0x02
#define ICC_INSERTED_EVENT                    ICC_PRESENT+ICC_CHANGE

// ICCD: Table 6.1-8 Bitmap for bStatus field
#define ICC_BS_PRESENT_ACTIVATED     0x00 // USB-ICC is present and activated
#define ICC_BS_PRESENT_NOTACTIVATED  0x01 // USB-ICC is present but not activated
#define ICC_BS_NOTPRESENT            0x02 // USB-ICC is virtually not present
#define ICC_BS_RFU                   0x03 // RFU
#define ICC_CS_NO_ERROR           (0x00<<6) // Processed without error
#define ICC_CS_FAILED             (0x01<<6) // Failed, error condition given by bError
#define ICC_CS_TIME_EXT           (0x02<<6) // Time extension is requested
#define ICC_CS_RFU                (0x03<<6) // RFU

/*
#define NO_ERROR                               0x00
#define NO_EXTRA_BYTES                         0x00
#define CCID_FLAG_INITIAL_VALUE                0x05
#define CCID_EVENT_SIZE                        0x02
#define STATUS_MASK                            0x41
*/
//------------------------------------------------------------------------------
//      Structures
//------------------------------------------------------------------------------

#endif //#ifndef CCID_DRIVER_DESCRIPTORS_H

