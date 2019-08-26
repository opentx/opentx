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
 \unit

 !!!Purpose

 Definitions used when declaring an HID report descriptor.

 !!!Usage

 Use the definitions provided here when declaring a report descriptor,
 which shall be an unsigned char array.
*/

#ifndef HIDREPORT_H
#define HIDREPORT_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Main Item Tags"
/// This page lists the Main Item Tags defined for HID %device.
/// ( HID Spec. 6.2.2 )
///
/// !Tags
/// - HIDReport_INPUT
/// - HIDReport_OUPUT
/// - HIDReport_FEATURE
/// - HIDReport_COLLECTION
/// - HIDReport_ENDCOLLECTION

/// Input item.
#define HIDReport_INPUT                         0x80
/// Output item.
#define HIDReport_OUTPUT                        0x90
/// Feature item.
#define HIDReport_FEATURE                       0xB0
/// Collection item.
#define HIDReport_COLLECTION                    0xA0
/// End of collection item.
#define HIDReport_ENDCOLLECTION                 0xC0
//------------------------------------------------------------------------------
                                                
//------------------------------------------------------------------------------
/// \page "HID Items for Data Fields"
/// This page lists defintions for HID Input, Output and Feature items that
/// are used to create the data fields within a report.
/// ( HID Spec. 6.2.2.5 )
///
/// !Items
/// - HIDReport_CONSTANT
/// - HIDReport_VARIABLE
/// - HIDReport_RELATIVE
/// - HIDReport_WRAP
/// - HIDReport_NONLINEAR
/// - HIDReport_NOPREFERRED
/// - HIDReport_NULLSTATE
/// - HIDReport_VOLATILE
/// - HIDReport_BUFFEREDBYTES

/// The report value is constant (vs. variable).
#define HIDReport_CONSTANT                      (1 << 0)
/// Data reported is a variable (vs. array).
#define HIDReport_VARIABLE                      (1 << 1)
/// Data is relative (vs. absolute).
#define HIDReport_RELATIVE                      (1 << 2)
/// Value rolls over when it reach a maximum/minimum.
#define HIDReport_WRAP                          (1 << 3)
/// Indicates that the data reported has been processed and is no longuer
/// linear with the original measurements.
#define HIDReport_NONLINEAR                     (1 << 4)
/// Device has no preferred state to which it automatically returns.
#define HIDReport_NOPREFERRED                   (1 << 5)
/// Device has a null state, in which it does not report meaningful
/// information.
#define HIDReport_NULLSTATE                     (1 << 6)
/// Indicates data can change without the host intervention.
#define HIDReport_VOLATILE                      (1 << 7)
/// Indicates the device produces a fixed-length stream of bytes.
#define HIDReport_BUFFEREDBYTES                 (1 << 8)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Collection Items"
/// This page lists definitions for HID Collection Items.
/// ( HID Spec. 6.2.2.6 )
///
/// !Items
/// - HIDReport_COLLECTION_PHYSICAL
/// - HIDReport_COLLECTION_APPLICATION
/// - HIDReport_COLLECTION_LOGICAL
/// - HIDReport_COLLECTION_REPORT
/// - HIDReport_COLLECTION_NAMEDARRAY
/// - HIDReport_COLLECTION_USAGESWITCH
/// - HIDReport_COLLECTION_USAGEMODIFIER

/// Physical collection.
#define HIDReport_COLLECTION_PHYSICAL           0x00
/// Application collection.
#define HIDReport_COLLECTION_APPLICATION        0x01
/// Logical collection.
#define HIDReport_COLLECTION_LOGICAL            0x02
/// Report collection.
#define HIDReport_COLLECTION_REPORT             0x03
/// Named array collection.
#define HIDReport_COLLECTION_NAMEDARRAY         0x04
/// Usage switch collection.
#define HIDReport_COLLECTION_USAGESWITCH        0x05
/// Usage modifier collection
#define HIDReport_COLLECTION_USAGEMODIFIER      0x06
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Global Items"
/// This page lists HID Global Items.
/// ( HID Spec. 6.2.2.7 )
///
/// !Items
/// - HIDReport_GLOBAL_USAGEPAGE
/// - HIDReport_GLOBAL_LOGICALMINIMUM
/// - HIDReport_GLOBAL_LOGICALMAXIMUM
/// - HIDReport_GLOBAL_PHYSICALMINIMUM
/// - HIDReport_GLOBAL_PHYSICALMAXIMUM
/// - HIDReport_GLOBAL_UNITEXPONENT
/// - HIDReport_GLOBAL_UNIT
/// - HIDReport_GLOBAL_REPORTSIZE
/// - HIDReport_GLOBAL_REPORTID
/// - HIDReport_GLOBAL_REPORTCOUNT
/// - HIDReport_GLOBAL_PUSH
/// - HIDReport_GLOBAL_POP

/// Current usage page.
#define HIDReport_GLOBAL_USAGEPAGE              0x04
/// Minimum value that a variable or array item will report.
#define HIDReport_GLOBAL_LOGICALMINIMUM         0x14
/// Maximum value that a variable or array item will report.
#define HIDReport_GLOBAL_LOGICALMAXIMUM         0x24
/// Minimum value for the physical extent of a variable item.
#define HIDReport_GLOBAL_PHYSICALMINIMUM        0x34
/// Maximum value for the physical extent of a variable item.
#define HIDReport_GLOBAL_PHYSICALMAXIMUM        0x44
/// Value of the unit exponent in base 10.
#define HIDReport_GLOBAL_UNITEXPONENT           0x54
/// Unit values.
#define HIDReport_GLOBAL_UNIT                   0x64
/// Size of the report fields in bits.
#define HIDReport_GLOBAL_REPORTSIZE             0x74
/// Specifies the report ID.
#define HIDReport_GLOBAL_REPORTID               0x84
/// Number of data fields for an item.
#define HIDReport_GLOBAL_REPORTCOUNT            0x94
/// Places a copy of the global item state table on the stack.
#define HIDReport_GLOBAL_PUSH                   0xA4
/// Replaces the item state table with the top structure from the stack.
#define HIDReport_GLOBAL_POP                    0xB4
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Local Items"
/// This page lists definitions for HID Local Items.
///
/// !Items
/// - HIDReport_LOCAL_USAGE
/// - HIDReport_LOCAL_USAGEMINIMUM
/// - HIDReport_LOCAL_USAGEMAXIMUM
/// - HIDReport_LOCAL_DESIGNATORINDEX
/// - HIDReport_LOCAL_DESIGNATORMINIMUM
/// - HIDReport_LOCAL_DESIGNATORMAXIMUM
/// - HIDReport_LOCAL_STRINGINDEX
/// - HIDReport_LOCAL_STRINGMINIMUM
/// - HIDReport_LOCAL_STRINGMAXIMUM
/// - HIDReport_LOCAL_DELIMITER

/// Suggested usage for an item or collection.
#define HIDReport_LOCAL_USAGE                   0x08
/// Defines the starting usage associated with an array or bitmap.
#define HIDReport_LOCAL_USAGEMINIMUM            0x18
/// Defines the ending usage associated with an array or bitmap.
#define HIDReport_LOCAL_USAGEMAXIMUM            0x28
/// Determines the body part used for a control.
#define HIDReport_LOCAL_DESIGNATORINDEX         0x38
/// Defines the index of the starting designator associated with an array or
/// bitmap.
#define HIDReport_LOCAL_DESIGNATORMINIMUM       0x48
/// Defines the index of the ending designator associated with an array or
/// bitmap.
#define HIDReport_LOCAL_DESIGNATORMAXIMUM       0x58
/// String index for a string descriptor.
#define HIDReport_LOCAL_STRINGINDEX             0x78
/// Specifies the first string index when assigning a group of sequential
/// strings to controls in an array or bitmap.
#define HIDReport_LOCAL_STRINGMINIMUM           0x88
/// Specifies the last string index when assigning a group of sequential
/// strings to controls in an array or bitmap.
#define HIDReport_LOCAL_STRINGMAXIMUM           0x98
/// Defines the beginning or end of a set of local items.
#define HIDReport_LOCAL_DELIMITER               0xA8
//------------------------------------------------------------------------------

#endif //#ifndef HIDREPORT_H

