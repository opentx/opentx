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

#ifndef _USB_HOST_ENUM_H_
#define _USB_HOST_ENUM_H_

//------------------------------------------------------------------------------
//      Headers
//------------------------------------------------------------------------------
#include <usb/common/core/USBFeatureRequest.h>
#include <usb/common/core/USBEndpointDescriptor.h>
#include <usb/common/core/USBGenericRequest.h>
#include <usb/common/core/USBGenericDescriptor.h>
#include <usb/otg/usb_task.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------
#ifndef SIZEOF_DATA_STAGE
   #error SIZEOF_DATA_STAGE should be defined in conf_usb.h
#endif

#if (SIZEOF_DATA_STAGE<0xFF)     //! Optimize descriptor offset index according to data_stage[] size
   #define T_DESC_OFFSET   U8    //! U8 is enought and faster
#else
   #define T_DESC_OFFSET   U16   //! U16 required !
#endif

#ifndef MAX_EP_PER_INTERFACE
   #define MAX_EP_PER_INTERFACE 4
#endif




//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------
typedef struct
{
    U8      bmRequestType;        //!< Characteristics of the request
    U8      bRequest;             //!< Specific request
    U16     wValue;               //!< field that varies according to request
    U16     wIndex;               //!< field that varies according to request
    U16     wLength;              //!< Number of bytes to transfer if Data
    U8      uncomplete_read;      //!< 1 = only one read
}  S_usb_setup_data;


typedef struct
{
    U8  interface_nb;
    U8  altset_nb;
    U16 class;
    U16 subclass;
    U16 protocol;
    U8  nb_ep;
    U8  ep_addr[MAX_EP_PER_INTERFACE];
} S_interface;

#define CONTROL_GOOD             0
#define CONTROL_TIMEOUT       0x08
#define CONTROL_STALL         0x20


//!< Set of defines for offset in data stage
#define OFFSET_FIELD_MAXPACKETSIZE     7
#define OFFSET_FIELD_MSB_VID           9
#define OFFSET_FIELD_LSB_VID           8
#define OFFSET_FIELD_MSB_PID           11
#define OFFSET_FIELD_LSB_PID           10

#define OFFSET_DESCRIPTOR_LENGHT       0
#define OFFSET_FIELD_DESCRIPTOR_TYPE   1
#define OFFSET_FIELD_TOTAL_LENGHT      2
#define OFFSET_FIELD_BMATTRIBUTES      7
#define OFFSET_FIELD_MAXPOWER          8

#define OFFSET_FIELD_OTG_FEATURES      2
#define OTG_DESCRIPTOR_bLength         0x03

//! OFFSET for INTERFACE DESCRIPTORS
#define OFFSET_FIELD_INTERFACE_NB      2
#define OFFSET_FIELD_ALT               3
#define OFFSET_FIELS_NB_OF_EP          4
#define OFFSET_FIELD_NB_INTERFACE      4  // Nb of endpoint used by this interface
#define OFFSET_FIELD_CLASS             5
#define OFFSET_FIELD_SUB_CLASS         6
#define OFFSET_FIELD_PROTOCOL          7


#define OFFSET_FIELD_EP_ADDR           2
#define OFFSET_FIELD_EP_TYPE           3
#define OFFSET_FIELD_EP_SIZE_LOW       4
#define OFFSET_FIELD_EP_SIZE_HIGH      5
#define OFFSET_FIELD_EP_INTERVAL       6

//! defines for Hub detection
#define OFFSET_DEV_DESC_CLASS          4    // offset for the CLASS field in the Device Descriptor
#define HUB_CLASS_CODE                 9    // value of Hub CLASS


#define HOST_FALSE                     0
#define HOST_TRUE                      1


//-----------------------------------------------------------------------------
//         Exported macro
//-----------------------------------------------------------------------------

/// send a clear endpoint request
#define host_clear_endpoint_feature(ep)   (usb_request.bmRequestType = 0x02,\
                                           usb_request.bRequest      = USBGenericRequest_CLEARFEATURE,\
                                           usb_request.wValue        = USBFeatureRequest_ENDPOINTHALT << 8,\
                                           usb_request.wIndex        = ep,\
                                           usb_request.wLength       = 0,\
                                           usb_request.uncomplete_read = FALSE,\
                                           host_send_control(data_stage))
/// send a get configuration request
#define host_get_configuration()          (usb_request.bmRequestType = 0x80,\
                                           usb_request.bRequest      = USBGenericRequest_GETCONFIGURATION,\
                                           usb_request.wValue        = 0,\
                                           usb_request.wIndex        = 0,\
                                           usb_request.wLength       = 1,\
                                           usb_request.uncomplete_read = FALSE,\
                                           host_send_control(data_stage))
/// send a set configuration request
#define host_set_configuration(cfg_nb)    (usb_request.bmRequestType = 0x00,\
                                           usb_request.bRequest      = USBGenericRequest_SETCONFIGURATION,\
                                           usb_request.wValue        = cfg_nb,\
                                           usb_request.wIndex        = 0,\
                                           usb_request.wLength       = 0,\
                                           usb_request.uncomplete_read = FALSE,\
                                           host_send_control(data_stage))
/// send a set interface request to specify a specific alt setting for an 
/// interface
#define host_set_interface(interface_nb,alt_setting) \
                                          (usb_request.bmRequestType = 0x00,\
                                           usb_request.bRequest      = USBGenericRequest_SETINTERFACE,\
                                           usb_request.wValue        = alt_setting,\
                                           usb_request.wIndex        = interface_nb,\
                                           usb_request.wLength       = 0,\
                                           usb_request.uncomplete_read = FALSE,\
                                           host_send_control(data_stage))

/// send a get device desriptor request.
/// The descriptor table received is stored in data_stage array.
/// The received descriptors is limited to the control pipe lenght
#define host_get_device_descriptor_uncomplete() \
                                          (usb_request.bmRequestType = 0x80,\
                                           usb_request.bRequest      = USBGenericRequest_GETDESCRIPTOR,\
                                           usb_request.wValue        = USBGenericDescriptor_DEVICE << 8,\
                                           usb_request.wIndex        = 0,\
                                           usb_request.wLength       = 64,\
                                           usb_request.uncomplete_read = TRUE,\
                                           host_send_control(data_stage))

/// send a get device desriptor request.
/// The descriptor table received is stored in data_stage array.
#define host_get_device_descriptor()      (usb_request.bmRequestType = 0x80,\
                                           usb_request.bRequest      = USBGenericRequest_GETDESCRIPTOR,\
                                           usb_request.wValue        = USBGenericDescriptor_DEVICE << 8,\
                                           usb_request.wIndex        = 0,\
                                           usb_request.wLength       = 18,\
                                           usb_request.uncomplete_read = FALSE,\
                                           host_send_control(data_stage))
/// send a get device configuration request.
/// The configuration descriptor table received is stored in data_stage array.
#define host_get_configuration_descriptor()(usb_request.bmRequestType = 0x80,\
                                           usb_request.bRequest      = USBGenericRequest_GETDESCRIPTOR,\
                                           usb_request.wValue        = USBGenericDescriptor_CONFIGURATION << 8,\
                                           usb_request.wIndex        = 0,\
                                           usb_request.wLength       = 255,\
                                           usb_request.uncomplete_read = FALSE,\
                                           host_send_control(data_stage))

#define host_get_descriptor_uncomplete()  (usb_request.bmRequestType = 0x80,\
                                           usb_request.bRequest      = USBGenericRequest_GETDESCRIPTOR,\
                                           usb_request.wValue        = 0,\
                                           usb_request.wIndex        = 0,\
                                           usb_request.wLength       = 64,\
                                           usb_request.uncomplete_read = FALSE,\
                                           host_send_control(data_stage))
/// send a set address request.
#define host_set_address(addr)            (usb_request.bmRequestType = 0x00,\
                                           usb_request.bRequest      = USBGenericRequest_SETADDRESS,\
                                           usb_request.wValue        = (U16)addr,\
                                           usb_request.wIndex        = 0,\
                                           usb_request.wLength       = 0,\
                                           usb_request.uncomplete_read = FALSE,\
                                           host_send_control(data_stage))

/// send a set feature device remote wakeup
#define host_set_feature_remote_wakeup()   (usb_request.bmRequestType = 0x00,\
                                           usb_request.bRequest      = USBGenericRequest_SETFEATURE,\
                                           usb_request.wValue        = 1,\
                                           usb_request.wIndex        = 1,\
                                           usb_request.wLength       = 0,\
                                           usb_request.uncomplete_read = FALSE,\
                                           host_send_control(data_stage))

/// send a set feature "a_hnp_support" to tell to B-Device that A-Device support HNP
#define host_set_feature_a_hnp_support()   (usb_request.bmRequestType = 0x00,\
                                           usb_request.bRequest      = USBGenericRequest_SETFEATURE,\
                                           usb_request.wValue        = 4,\
                                           usb_request.wIndex        = 0,\
                                           usb_request.wLength       = 0,\
                                           usb_request.uncomplete_read = FALSE,\
                                           host_send_control(data_stage))

/// send a set feature "b_hnp_enable" to make B-Device initiating a HNP
#define host_set_feature_b_hnp_enable()    (usb_request.bmRequestType = 0x00,\
                                           usb_request.bRequest      = USBGenericRequest_SETFEATURE,\
                                           usb_request.wValue        = 3,\
                                           usb_request.wIndex        = 0,\
                                           usb_request.wLength       = 0,\
                                           usb_request.uncomplete_read = FALSE,\
                                           host_send_control(data_stage))

/// send the mass storage specific request "get max lun"
#define host_ms_get_max_lun()             (usb_request.bmRequestType = 0xA1,\
                                           usb_request.bRequest      = MS_GET_MAX_LUN,\
                                           usb_request.wValue        = 0,\
                                           usb_request.wIndex        = 0,\
                                           usb_request.wLength       = 1,\
                                           usb_request.uncomplete_read = FALSE,\
                                           host_send_control(data_stage))
/// returns the VID of the device connected
#define Get_VID()      (device_VID)

/// returns the PID of the device connected
#define Get_PID()      (device_PID)

/// returns the maximum power consumption ot hte connected device (unit is 2mA)
#define Get_maxpower() (maxpower)

/// returns the USB class associated to the specified interface
#define Get_class(s_interface)         (interface_supported[s_interface].class)

/// returns the USB subclass associated to the specified interface
#define Get_subclass(s_interface)      (interface_supported[s_interface].subclass)

/// returns the USB protocol associated to the specified interface
#define Get_protocol(s_interface)      (interface_supported[s_interface].protocol)

/// returns endpoint address associated to the specified interface and
/// endpoint number in this interface.
#define Get_ep_addr(s_interface,n_ep)  (interface_supported[s_interface].ep_addr[n_ep])

/// returns number of endpoints associated to
#define Get_nb_ep(s_interface)      (interface_supported[s_interface].nb_ep)

/// returns number of the alternate setting field associated to
#define Get_alts_s(s_interface)     (interface_supported[s_interface].altset_nb)

/// returns number of the interface number associated to
#define Get_interface_number(s_interface) (interface_supported[s_interface].interface_nb)

/// returns the number of interface supported in the device connected
#define Get_nb_supported_interface()  (nb_interface_supported)

#define BIT_SELF_POWERED   6  // offset
/// returns true if the device connected is self powered
#define Is_device_self_powered()     ((bmattributes & (1<<BIT_SELF_POWERED)) ? TRUE : FALSE)

#define BIT_REMOTE_WAKEUP  5  // offset
/// returns true if the device supports remote wake_up
#define Is_device_supports_remote_wakeup()  ((bmattributes & (1<<BIT_REMOTE_WAKEUP)) ? TRUE : FALSE)

#define BIT_SRP_SUPPORT    0  // offset
/// returns true if the device supports SRP
#define Is_device_supports_srp()    ((otg_features_supported & (1<<BIT_SRP_SUPPORT)) ? TRUE : FALSE)

#define BIT_HNP_SUPPORT    1  // offset
/// returns true if the device supports HNP
#define Is_device_supports_hnp()    ((otg_features_supported & (1<<BIT_HNP_SUPPORT)) ? TRUE : FALSE)


//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------
extern U8 ep_table[];
extern U8 nb_interface_supported;
extern S_interface interface_supported[MAX_INTERFACE_SUPPORTED];
extern U16 device_PID;
extern U16 device_VID;
extern U8 bmattributes;
extern U8 maxpower;

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------
extern U8 host_check_VID_PID(void);
extern U8 host_check_OTG_features(void);
extern U8 host_check_class  (void);
extern U8 host_auto_configure_endpoint(void);
extern T_DESC_OFFSET get_interface_descriptor_offset(U8 interface, U8 alt);
extern U8 host_get_hwd_pipe_nb(U8 ep_addr);
extern U8 host_send_control(U8*);


#endif  // _USB_HOST_ENUM_H_

