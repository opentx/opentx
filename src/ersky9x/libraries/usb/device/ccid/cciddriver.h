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
/// \unit
///
/// !Purpose
///
/// Definition of methods for using a CCID device driver.
/// 
/// !Usage
/// 
/// -# CCIDDriver_Initialize
/// -# CCID_Read
/// -# CCID_Write
/// -# CCID_SmartCardRequest
/// -# CCID_Insertion
/// -# CCID_Removal
/// -# RDRtoPCHardwareError
//------------------------------------------------------------------------------

#ifndef CCID_DRIVER_H
#define CCID_DRIVER_H

/// For reference, the absolute maximum block size 
/// for a TPDU T=0 block is 260 bytes (5 bytes command; 255 bytes data), or 
/// for a TPDU T=1 block is 259 bytes, or 
/// for a short APDU T=1 block is 261 bytes, or 
/// for an extended APDU T=1 block is 65544 bytes.
#define ABDATA_SIZE 260

/// define protocol T=0
#define PROTOCOL_TO 0
/// define protocol T=1
#define PROTOCOL_T1 1

/// define for dwFeatures see Table 5.1-1 Smart Card Device Class Descriptors
/// No special characteristics
#define CCID_FEATURES_NADA       0x00000000
/// Automatic parameter configuration based on ATR data
#define CCID_FEATURES_AUTO_PCONF 0x00000002
/// Automatic activation of ICC on inserting
#define CCID_FEATURES_AUTO_ACTIV 0x00000004 
/// Automatic ICC voltage selection
#define CCID_FEATURES_AUTO_VOLT  0x00000008 
/// Automatic ICC clock frequency change according to active parameters provided 
/// by the Host or self determined
#define CCID_FEATURES_AUTO_CLOCK 0x00000010 
/// Automatic baud rate change according to active parameters provided by the 
/// Host or self determined
#define CCID_FEATURES_AUTO_BAUD  0x00000020 
/// Automatic parameters negotiation made by the CCID (use of warm or cold 
/// resets or PPS according to a manufacturer proprietary algorithm to select 
/// the communication parameters with the ICC)
#define CCID_FEATURES_AUTO_PNEGO 0x00000040 
/// Automatic PPS made by the CCID according to the active parameters
#define CCID_FEATURES_AUTO_PPS   0x00000080 
/// CCID can set ICC in clock stop mode
#define CCID_FEATURES_ICCSTOP    0x00000100 
/// NAD value other than 00 accepted (T=1 protocol in use)
#define CCID_FEATURES_NAD        0x00000200 
/// Automatic IFSD exchange as first exchange (T=1 protocol in use)
#define CCID_FEATURES_AUTO_IFSD  0x00000400 
/// TPDU level exchanges with CCID
#define CCID_FEATURES_EXC_TPDU   0x00010000 
/// Short APDU level exchange with CCID
#define CCID_FEATURES_EXC_SAPDU  0x00020000 
/// Short and Extended APDU level exchange with CCID
#define CCID_FEATURES_EXC_APDU   0x00040000 
/// USB Wake up signaling supported on card insertion and removal
#define CCID_FEATURES_WAKEUP     0x00100000 

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

/// Bulk CCID Message header structure
typedef struct
{
   unsigned char bMessageType;
   /// Message-specific data length
   unsigned long wLength;
   /// Identifies the slot number for this command
   unsigned char bSlot;
   /// Sequence number for command.
   unsigned char bSeq;
   /// Slot status register
   unsigned char bStatus;
   /// Slot error
   unsigned char bError;
   /// specific register
   unsigned char bSpecific;
   /// Data block sent to the CCID.
   unsigned char abData[ABDATA_SIZE];
   unsigned char bSizeToSend;
} __attribute__ ((packed)) S_ccid_bulk_in_header;

/// 6.1 Bulk Transfers
typedef struct
{
   unsigned char bMessageType;
   /// Message-specific data length
   unsigned long wLength;
   /// Identifies the slot number for this command
   unsigned char bSlot;
   /// Sequence number for command.
   unsigned char bSeq;
   /// specific register
   unsigned char bSpecific_0;
   unsigned char bSpecific_1;
   unsigned char bSpecific_2;
   /// Application Protocol Data Unit
   unsigned char APDU[ABDATA_SIZE];
} __attribute__ ((packed)) S_ccid_bulk_out_header;


/// 6.1.11.2 PIN Verification Data Structure
typedef struct
{
    /// Number of seconds.
    unsigned char bTimerOut;
    /// Several parameters for the PIN format options
    unsigned char bmFormatString;
    /// Define the length of the PIN to present in the APDU command
    unsigned char bmPINBlockString;
    /// Allows the length PIN insertion in the APDU command
    unsigned char bmPINLengthFormat;
    /// Minimum PIN size in digit and Maximum PIN size in digit
    unsigned char wPINMaxExtraDigit;
    /// The value is a bit wise OR operation.
    unsigned char bEntryValidationCondition;
    /// Number of messages to display for the PIN modify command
    unsigned char bNumberMessage;
    /// Language used to display the messages.
    unsigned char wLangId;
    /// Message index in the Reader message table
    unsigned char bMsgIndex;
    /// T=1 I-block prologue field to use
    unsigned char bTeoPrologue[3];
    /// APDU to send to the ICC
    unsigned char abPINApdu[255];
}__attribute__ ((packed)) S_ccid_PIN_Verification;


/// 6.1.11.7 PIN Modification Data Structure
typedef struct
{
    /// Number of seconds. If 00h then CCID default value is used.
    unsigned char bTimeOut;
    /// Several parameters for the PIN format options (defined in § 6.1.11.4)
    unsigned char bmFormatString4;
    /// Define the length of the PIN to present in the APDU command
    unsigned char bmPINBlockString;
    /// Allows the length PIN insertion in the APDU command (defined in § 6.1.11.6)
    unsigned char bmPinLengthFormat;
    /// Insertion position offset in byte for the current PIN
    unsigned char bInsertionOffsetOld;
    /// Insertion position offset in byte for the new PIN
    unsigned char bInsertionOffsetNew;
    /// XXYYh
    /// XX: Minimum PIN size in digit
    /// YY: Maximum PIN size in digit
    unsigned char wPINMaxExtraDigit;
    /// 00h,01h,02h,03h
    /// Indicates if a confirmation is requested before acceptance of a new PIN (meaning that the user has to enter this new PIN twice before it is accepted)
    /// Indicates if the current PIN must be entered and set in the same APDU field of not.
    unsigned char bConfirmPIN;
    /// The value is a bit wise OR operation.
    /// 01h Max size reached
    /// 02h Validation key pressed
    /// 04h Timeout occurred
    unsigned char bEntryValidationCondition;
    /// 00h,01h,02h,03h,or FFh
    /// Number of messages to display for the PIN modify command.
    unsigned char bNumberMessage;
    /// Language used to display the messages. The 16 bit
    unsigned char wLangId;
    /// Message index in the Reader message table (should be 00h or 01h).
    unsigned char bMsgIndex1;
    /// Message index in the Reader message table (should be 01h or 02h).
    unsigned char bMsgIndex2;
    /// Message index in the Reader message table (should be 02h).
    unsigned char bMsgIndex3;
    /// T=1 I-block prologue field to use. Significant only if protocol in use is T=1.
    unsigned char bTeoPrologue[3];
    /// Byte array APDU to send to the ICC
    unsigned char abPINApdu[255];
}__attribute__ ((packed)) S_ccid_PIN_Modification;

/// Protocol Data Structure for Protocol T=0 (bProtocolNum=0, dwLength=00000005h)
typedef struct
{
    /// B7-4 – FI – Index into the table 7 in ISO/IEC 7816-3:1997 selecting a 
    /// clock rate conversion factor
    /// B3-0 – DI - Index into the table 8 in ISO/IEC 7816-3:1997 selecting a 
    /// baud rate conversion factor
    unsigned char bmFindexDindex;
    /// For T=0 ,B0 – 0b, B7-2 – 000000b
    /// B1 – Convention used (b1=0 for direct, b1=1 for inverse)
    unsigned char bmTCCKST0;         // 0 to 2
    /// Extra Guardtime between two characters. Add 0 to 254 etu to the normal 
    /// guardtime of 12etu. FFh is the same as 00h.
    unsigned char bGuardTimeT0;      // 0 to FF
    /// WI for T=0 used to define WWT
    unsigned char bWaitingIntegerT0; // 0 to FF
    /// ICC Clock Stop Support
    /// 00 = Stopping the Clock is not allowed
    /// 01 = Stop with Clock signal Low
    /// 02 = Stop with Clock signal High
    /// 03 = Stop with Clock either High or Low
    unsigned char bClockStop;        // 0 to 3
} __attribute__ ((packed)) S_ccid_protocol_t0;


/// Protocol Data Structure for Protocol T=1 (bProtocolNum=1, dwLength=00000007h)
typedef struct
{
    /// B7-4 – FI – Index into the table 7 in ISO/IEC 7816-3:1997 selecting a 
    /// clock rate conversion factor
    /// B3-0 – DI - Index into the table 8 in ISO/IEC 7816-3:1997 selecting a 
    /// baud rate conversion factor
    unsigned char bmFindexDindex;
    /// For T=1, B7-2 – 000100b
    /// B0 – Checksum type (b0=0 for LRC, b0=1 for CRC
    /// B1 – Convention used (b1=0 for direct, b1=1 for inverse)
    unsigned char bmTCCKST1;           // 10h, 11h, 12h, 13h
    /// Extra Guardtime (0 to 254 etu between two characters). 
    /// If value is FFh, then guardtime is reduced by 1.
    unsigned char bGuardTimeT1;        // 0 to FF
    /// B7-4 = BWI
    /// B3-0 = CWI
    unsigned char bmWaitingIntegersT1; // 0 to 9
    /// ICC Clock Stop Support
    /// 00 = Stopping the Clock is not allowed
    /// 01 = Stop with Clock signal Low
    /// 02 = Stop with Clock signal High
    /// 03 = Stop with Clock either High or Low
    unsigned char bClockStop;          // 0 to 3
    /// Size of negotiated IFSC
    unsigned char bIFSC;               // 0 to FE
    /// Nad value used by CCID
    unsigned char bNadValue;           // 0 to FF
} __attribute__ ((packed)) S_ccid_protocol_t1;


/// Identifies the length of type of subordinate descriptors of a CCID device
/// Table 5.1-1 Smart Card Device Class descriptors
typedef struct
{
   /// Size of this descriptor, in bytes.
   unsigned char  bLength;
   /// Functional Descriptor type
   unsigned char  bDescriptorType;
   /// Integrated Circuit(s) Cards Interface Devices (CCID) Specification 
   /// Release Number
   unsigned short bcdCCID;
   /// Index of the highest available slot. An USB-ICC is regarded as a single
   /// slot CCID.
   unsigned char  bMaxSlotIndex;
   /// This value indicates what voltages the CCID can supply to its slots.
   /// It is a bitwise OR operation performed on the following values:
   /// - 01h 5.0V
   /// - 02h 3.0V
   /// - 04h 1.8V
   /// Other bits are RFU.
   unsigned char  bVoltageSupport;
   /// RRRR –Upper Word- is RFU = 0000h
   /// PPPP –Lower Word- Encodes the supported protocol types. A ‘1’ in a given
   /// bit position indicates support for the associated ISO protocol.
   /// 0001h = Protocol T=0
   /// 0002h = Protocol T=1
   /// All other bits are reserved and must be set to zero. The field is 
   /// intended to correspond to the PCSC specification definitions. 
   unsigned long  dwProtocols;
   /// Default ICC clock frequency in KHz. This is an integer value.
   unsigned long  dwDefaultClock;
   /// Maximum supported ICC clock frequency in KHz. This is an integer value.
   unsigned long  dwMaximumClock;
   /// The number of clock frequencies that are supported by the CCID. If the 
   /// value is 00h, the supported clock frequencies are assumed to be the 
   /// default clock frequency defined by dwDefaultClock and the maximum clock 
   /// frequency defined by dwMaximumClock.
   unsigned char  bNumClockSupported;
   /// Default ICC I/O data rate in bps. This is an integer value
   unsigned long  dwDataRate;
   /// Maximum supported ICC I/O data rate in bps
   unsigned long  dwMaxDataRate;
   /// The number of data rates that are supported by the CCID.
   unsigned char  bNumDataRatesSupported;
   /// Indicates the maximum IFSD supported by CCID for protocol T=1.
   unsigned long  dwMaxIFSD;
   /// - RRRR-Upper Word- is RFU = 0000h
   /// - PPPP-Lower Word- encodes the supported protocol types. A ‘1’ in a given
   ///   bit position indicates support for the associated protocol.
   ///   0001h indicates support for the 2-wire protocol 1
   ///   0002h indicates support for the 3-wire protocol 1
   ///   0004h indicates support for the I2C protocol 1
   /// All other values are outside of this specification, and must be handled 
   /// by vendor-supplied drivers.
   unsigned long  dwSynchProtocols;
   /// The value is a bitwise OR operation performed on the following values:
   /// - 00000000h No special characteristics
   /// - 00000001h Card accept mechanism 2
   /// - 00000002h Card ejection mechanism 2
   /// - 00000004h Card capture mechanism 2
   /// - 00000008h Card lock/unlock mechanism
   unsigned long  dwMechanical;
   /// This value indicates what intelligent features the CCID has.
   unsigned long  dwFeatures;
   /// For extended APDU level the value shall be between 261 + 10 (header) and 
   /// 65544 +10, otherwise the minimum value is the wMaxPacketSize of the 
   /// Bulk-OUT endpoint.
   unsigned long  dwMaxCCIDMessageLength;
   /// Significant only for CCID that offers an APDU level for exchanges.
   unsigned char  bClassGetResponse;
   /// Significant only for CCID that offers an extended APDU level for exchanges.
   unsigned char  bClassEnvelope;
   /// Number of lines and characters for the LCD display used to send messages for PIN entry.
   unsigned short wLcdLayout;
   /// This value indicates what PIN support features the CCID has.
   unsigned char  bPINSupport;
   /// Maximum number of slots which can be simultaneously busy.
   unsigned char  bMaxCCIDBusySlots;

} __attribute__ ((packed)) CCIDDescriptor;

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern unsigned char RDRtoPCHardwareError( unsigned char bSlot, 
                                           unsigned char bSeq, 
                                           unsigned char bHardwareErrorCode );

#if !defined(NOAUTOCALLBACK)
extern void USBDCallbacks_RequestReceived(const USBGenericRequest *pRequest);
#endif
extern void CCID_SmartCardRequest( void );
extern void CCIDDriver_Initialize( void );
extern unsigned char CCID_Read(void *pBuffer,
                               unsigned int dLength,
                               TransferCallback fCallback,
                               void *pArgument);
extern unsigned char CCID_Write(void *pBuffer,
                                unsigned int dLength,
                                TransferCallback fCallback,
                                void *pArgument);
extern unsigned char CCID_Insertion( void );
extern unsigned char CCID_Removal( void );

#endif //#ifndef CCID_DRIVER_H

