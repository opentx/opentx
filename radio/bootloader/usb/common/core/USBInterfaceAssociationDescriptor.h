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

/**
 *  \page USBInterfaceAssociateDescriptor
 *  !!!Purpose
 * 
 *      Class for manipulating USB IAD descriptors.
 * 
 *  !!!Usage
 * 
 *      -# Test
 */

#ifndef USBDIADDESCRIPTOR_H
#define USBDIADDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

/*
    Type: USBDeviceDescriptor
        USB standard device descriptor structure.

    Variables:
        bLength - Size of this descriptor in bytes.
        bDescriptorType - Descriptor type (<USBGenericDescriptor_IAD>).
        bFirstInterface - Interface number of the first interface that is 
                          associated with this function. 
        bInterfaceCount - Number of contiguous interfaces that are 
                          associated with this function. 
        bFunctionClass - Class code (assigned by USB-IF).
        bFunctionSubClass - Subclass code (assigned by USB-IF).
        bFunctionProtocol - Protocol code (assigned by USB-IF)
        iFunction - Index of string descriptor describing this function.
*/
typedef struct {

   unsigned char bLength;
   unsigned char bDescriptorType;
   unsigned char bFirstInterface;
   unsigned char bInterfaceCount;
   unsigned char bFunctionClass;
   unsigned char bFunctionSubClass;
   unsigned char bFunctionProtocol;
   unsigned char iFunction;
} __attribute__ ((packed)) USBInterfaceAssociationDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

#endif //#ifndef USBDIADDESCRIPTOR_H

