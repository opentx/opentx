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

#ifndef APPLET_H
#define APPLET_H

/*----------------------------------------------------------------------------
 *        Global definitions
 *----------------------------------------------------------------------------*/

/** Refers to the Version of SAM-BA */
#define SAM_BA_APPLETS_VERSION  "2.11"

/** Applet initialization command code.*/
#define APPLET_CMD_INIT              0x00
/** Applet full erase command code.*/
#define APPLET_CMD_FULL_ERASE        0x01
/** Applet write command code.*/
#define APPLET_CMD_WRITE             0x02
/** Applet read command code.*/
#define APPLET_CMD_READ              0x03
/** Applet read command code.*/
#define APPLET_CMD_LOCK              0x04
/** Applet read command code.*/
#define APPLET_CMD_UNLOCK            0x05
/** Applet set/clear GPNVM command code.*/
#define APPLET_CMD_GPNVM             0x06
/** Applet set security bit command code.*/
#define APPLET_CMD_SECURITY          0x07
/** Applet buffer erase command code.*/
#define APPLET_CMD_BUFFER_ERASE      0x08
/** Applet binary page command code for Dataflash.*/
#define APPLET_CMD_BINARY_PAGE       0x09
/** List Bad Blocks of a Nandflash*/
#define APPLET_CMD_LIST_BAD_BLOCKS   0x10
/** Tag a Nandflash Block*/
#define APPLET_CMD_TAG_BLOCK         0x11
/** Read the Unique ID bits (on SAM3)*/
#define APPLET_CMD_READ_UNIQUE_ID    0x12
/** Applet blocks erase command code. */
#define APPLET_CMD_ERASE_BLOCKS      0x13
/** Applet batch full erase command code. */
#define APPLET_CMD_BATCH_ERASE       0x14

/** Operation was successful.*/
#define APPLET_SUCCESS          0x00
/** Device unknown.*/
#define APPLET_DEV_UNKNOWN      0x01
/** Write operation failed.*/
#define APPLET_WRITE_FAIL       0x02
/** Read operation failed.*/
#define APPLET_READ_FAIL        0x03
/** Protect operation failed.*/
#define APPLET_PROTECT_FAIL     0x04
/** Unprotect operation failed.*/
#define APPLET_UNPROTECT_FAIL   0x05
/** Erase operation failed.*/
#define APPLET_ERASE_FAIL       0x06
/** No device defined in board.h*/
#define APPLET_NO_DEV           0x07
/** Read / write address is not aligned*/
#define APPLET_ALIGN_ERROR      0x08
/** Read / write found bad block*/
#define APPLET_BAD_BLOCK        0x09
/** Applet failure.*/
#define APPLET_FAIL             0x0f


/** Communication link identification*/
#define USB_COM_TYPE            0x00
#define DBGU_COM_TYPE           0x01
#define JTAG_COM_TYPE           0x02

#endif //#ifndef APPLET_H

