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

/// This file contains the USB low level driver definition


#ifndef _USB_DRV_H
#define _USB_DRV_H

//------------------------------------------------------------------------------
//      Headers
//------------------------------------------------------------------------------
#include "board.h"

extern unsigned char dBytes;
extern char* pFifo_ctrl;
extern char* pFifo;

//#define Address_fifo_endpoint(ep)      (char*)&(AT91C_BASE_OTGHS_EPTFIFO->OTGHS_READEPT0[EPT_VIRTUAL_SIZE*ep])
#define Address_fifochar_endpoint(ep)     pFifo = (char*)((unsigned int *)AT91C_BASE_OTGHS_EPTFIFO + (EPT_VIRTUAL_SIZE * ep));dBytes=0
//#define Address_fifochar_endpoint(ep)     (char*)((unsigned int *)AT91C_BASE_OTGHS_EPTFIFO + (EPT_VIRTUAL_SIZE * ep))
#define Address_fifo_endpoint(ep)      (int*) ((int)(AT91C_BASE_OTGHS_EPTFIFO->OTGHS_READEPT0) + (EPT_VIRTUAL_SIZE*(ep)))

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

/// USB_low_level_drivers USB low level drivers module

#define MAX_EP_NB             7

#define EPT_VIRTUAL_SIZE      8192

#define EP_CONTROL            0
#define PIPE_CONTROL          0

// USB EndPoint
#define MSK_EP_DIR            0x7F

// Parameters for endpoint configuration
// These define are the values used to enable and configure an endpoint.
#define TYPE_CONTROL             0
#define TYPE_ISOCHRONOUS         1
#define TYPE_BULK                2
#define TYPE_INTERRUPT           3

#define DIRECTION_OUT            0
#define DIRECTION_IN             1

#define SIZE_8                   0
#define SIZE_16                  1
#define SIZE_32                  2
#define SIZE_64                  3
#define SIZE_128                 4
#define SIZE_256                 5
#define SIZE_512                 6
#define SIZE_1024                7

#define ONE_BANK                 0
#define TWO_BANKS                1
#define TREE_BANKS               2

#define NYET_ENABLED             0
#define NYET_DISABLED            1

#define TOKEN_SETUP              0
#define TOKEN_IN                 1
#define TOKEN_OUT                2

#define Is_ep_addr_in(x)         (x & 0x80)

//Set_bits(AVR32_USB_uatst1,AVR32_USB_UATST1_LOADCNTA_MASK);
#define Usb_load_tsta1_countMask() AT91C_BASE_OTGHS->OTGHS_TSTA1 &= ~(unsigned int)AT91C_OTGHS_COUNTERA

//#define AVR32_USB_UATST1_LOADCNTA_MASK_NONZERO             0x00008001
#define Usb_load_tsta1_counta()    AT91C_BASE_OTGHS->OTGHS_TSTA1 = AT91C_OTGHS_LOADCNTA | (AT91C_OTGHS_COUNTERA&0x01)

//#define AVR32_USB_UATST2_FORCE_51MS_RESETHS_MASK           0x00000080
#define USb_ForceHSRst_50ms()      AT91C_BASE_OTGHS->OTGHS_TSTA2 |= AT91C_OTGHS_FORCHSRST

//#define AVR32_USB_UATST2_FORCE_RESET_UTMI                  0x00000100
#define Usb_SetForceResetUTMI()    AT91C_BASE_OTGHS->OTGHS_TSTA2 |= AT91C_OTGHS_UTMIRESET
#define Usb_ClrForceResetUTMI()    AT91C_BASE_OTGHS->OTGHS_TSTA2 &= ~(unsigned int)AT91C_OTGHS_UTMIRESET

//#define AVR32_USB_UATST2_HOSTHSDISCONNECTDISABLE_MASK
#define Usb_SetHSTDiconnectDisableMask() AT91C_BASE_OTGHS->OTGHS_TSTA2 |= AT91C_OTGHS_HSTHSDISCDIS
#define Usb_ClrHSTDiconnectDisableMask() AT91C_BASE_OTGHS->OTGHS_TSTA2 &= ~(unsigned int)AT91C_OTGHS_HSTHSDISCDIS

#define Usb_enable_vbus_error_interrupt()         AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_VBERR

#define Is_Usb_InHighSpeed()    (AT91C_BASE_OTGHS->OTGHS_SR&AT91C_OTGHS_SPEED_SR) == AT91C_OTGHS_SPEED_SR_HS

/// OTG Timers customizing
// HOST : Minimum delay after Vbus requested to get it > Va_vbus_valid (otherwise => error)
//#define VBUSRISE_20MS            0x00   
//#define VBUSRISE_50MS            0x01
#define VBUSRISE_70MS            0x02
//#define VBUSRISE_100MS           0x03
// DEVICE : Duration of Vbus pulse during SRP protocol
//#define VBUSPULSE_15MS           0x20
//#define VBUSPULSE_23MS           0x21
//#define VBUSPULSE_31MS           0x22
#define VBUSPULSE_40MS           0x23
// DEVICE : Minimum delay after Vbus < Vb_sess_end to enable SRP
//#define VFALLTMOUT_93MS          0x40   
//#define VFALLTMOUT_105MS         0x41
//#define VFALLTMOUT_118MS         0x42
#define VFALLTMOUT_131MS         0x43
// HOST : Minimum pulse duration accepted as SRP pulse
//#define SRPMINDET_10US           0x60   
#define SRPMINDET_100US          0x61
//#define SRPMINDET_1MS            0x62
//#define SRPMINDET_11MS           0x63

//------------------------------------------------------------------------------
//   A R B I T R E R
//------------------------------------------------------------------------------

/// gen_usb USB common management drivers
/// These macros manage the USB controller

/// Enable external UID pin
//#define Usb_enable_uid_pin()            (UHWCON |= (1<<UIDE))
#define Usb_enable_uid_pin()     AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_UIDE
/// Disable external UID pin
//#define Usb_disable_uid_pin()    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~AT91C_OTGHS_UIDE
/// Disable external UID pin and force device mode
//#define Usb_force_device_mode() //{AT91C_BASE_OTGHS->OTGHS_CTRL &= ~AT91C_OTGHS_UIDE;
                                // AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_UIMOD;}
/// Disable external UID pin and force host mode
//#define Usb_force_host_mode()   //{AT91C_BASE_OTGHS->OTGHS_CTRL &= ~AT91C_OTGHS_UIDE;
                                // AT91C_BASE_OTGHS->OTGHS_CTRL &= ~AT91C_OTGHS_UIMOD;}
/// Enable external UVCON pin
//#define Usb_enable_uvcon_pin()        //  (UHWCON |= (1<<UVCONE))
#define Usb_enable_uvcon_pin()
//#define Usb_enable_uvcon_pin()   { const Pin pinDRVVUS = PIN_USB_DRV_VBUS;PIO_Set(&pinDRVVUS); }
//jcb
/// Use device full speed mode (default mode)
//#define Usb_full_speed_mode()           (UDCON &= ~(1<<LSM))
/// For device mode, force low speed mode
//#define Usb_low_speed_mode()            (UDCON |= (1<<LSM))
#define Usb_low_speed_mode()     AT91C_BASE_OTGHS->OTGHS_DEVCTRL |= AT91C_OTGHS_LS

#define Usb_enable()            AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_USBECTRL
#define Usb_disable()           AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_USBECTRL

/// Enable VBUS pad
//#define Usb_enable_vbus_pad()  (USBCON |= (1<<OTGPADE))
#define Usb_enable_vbus_pad()   {AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_USBECTRL;\
                                 AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_OTGPADE;\
                                 AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_FRZCLKCTRL;}
/// Disable VBUS pad
//#define Usb_disable_vbus_pad()  (USBCON &= ~(1<<OTGPADE))
#define Usb_disable_vbus_pad()    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_OTGPADE

#define Usb_select_device()      Usb_enable_uid_pin()
#define Usb_select_host()         Usb_enable_uid_pin()
#define Is_usb_device_enabled()       (AT91C_BASE_OTGHS->OTGHS_SR & AT91C_OTGHS_ID)
#define Is_usb_host_enabled()         !Is_usb_device_enabled()
/// Stop internal USB clock in interface (freeze the interface register)
//#define Usb_freeze_clock()          (USBCON  |=  (1<<FRZCLK))
#define Usb_freeze_clock()          //  AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_FRZCLKCTRL
//jcb
//#define Usb_unfreeze_clock()        (USBCON  &= ~(1<<FRZCLK))
#define Usb_unfreeze_clock()        //  AT91C_BASE_OTGHS->OTGHS_CTRL &= ~AT91C_OTGHS_FRZCLKCTRL
//#define Is_usb_clock_freezed()      ((USBCON  &   (1<<FRZCLK)) ? TRUE : FALSE)
#define Is_usb_clock_freezed()        (AT91C_BASE_OTGHS->OTGHS_CTRL & AT91C_OTGHS_FRZCLKCTRL)

//#define Usb_enable_id_interrupt()   (USBCON  |=  (1<<IDTE))
#define Usb_enable_id_interrupt()     AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_IDT
//#define Usb_disable_id_interrupt()  (USBCON  &= ~(1<<IDTE))
#define Usb_disable_id_interrupt()    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_IDT
//#define Is_usb_id_interrupt_enabled() ((USBCON &  (1<<IDTE))     ? TRUE : FALSE)
#define Is_usb_id_interrupt_enabled() (AT91C_BASE_OTGHS->OTGHS_CTRL & AT91C_OTGHS_IDT)
//#define Is_usb_id_device()          ((USBSTA &   (1<<ID))      ? TRUE : FALSE)
#define Is_usb_id_device()            (AT91C_BASE_OTGHS->OTGHS_SR & AT91C_OTGHS_ID)
//#define Is_usb_id_host()            ((USBSTA & (1<<ID))        ? FALSE : TRUE)
#define Is_usb_id_host()              !Is_usb_id_device()
//#define Usb_ack_id_transition()     (USBINT  = ~(1<<IDTI))
#define Usb_ack_id_transition()       AT91C_BASE_OTGHS->OTGHS_SCR = AT91C_OTGHS_IDT
//#define Is_usb_id_transition()      ((USBINT &   (1<<IDTI))    ? TRUE : FALSE)
#define Is_usb_id_transition()        (AT91C_BASE_OTGHS->OTGHS_SR & AT91C_OTGHS_IDT)

//#define Usb_enable_vbus_interrupt()     (USBCON  |=  (1<<VBUSTE))
//#define Usb_enable_vbus_interrupt()   (Set_bits(AVR32_USB_usbcon, AVR32_USB_USBCON_VBUSTE_MASK))
#define Usb_enable_vbus_interrupt()       AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_VBUSTI
//#define Usb_disable_vbus_interrupt()    (USBCON  &= ~(1<<VBUSTE))
#define Usb_disable_vbus_interrupt()      AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_VBUSTI
//#define Is_usb_vbus_interrupt_enabled() ((USBCON &  (1<<VBUSTE))     ? TRUE : FALSE)
#define Is_usb_vbus_interrupt_enabled()   (AT91C_BASE_OTGHS->OTGHS_CTRL &  AT91C_OTGHS_VBUSTI)
//#define Is_usb_vbus_high()              ((USBSTA &   (1<<VBUS))    ? TRUE : FALSE)

#define Is_usb_vbus_high()            ((AT91C_BASE_OTGHS->OTGHS_SR&AT91C_OTGHS_VBUSSR)==AT91C_OTGHS_VBUSSR)
//#define Is_usb_vbus_low()             ((USBSTA &   (1<<VBUS))    ? FALSE : TRUE)
#define Is_usb_vbus_low()             ((AT91C_BASE_OTGHS->OTGHS_SR&AT91C_OTGHS_VBUSSR)!=AT91C_OTGHS_VBUSSR)
//#define Usb_ack_vbus_transition()     (USBINT  = ~(1<<VBUSTI))
#define Usb_ack_vbus_transition()     AT91C_BASE_OTGHS->OTGHS_SCR = AT91C_OTGHS_VBUSTI
//#define Is_usb_vbus_transition()      ((USBINT &   (1<<VBUSTI))  ? TRUE : FALSE)
#define Is_usb_vbus_transition()      (AT91C_BASE_OTGHS->OTGHS_CTRL & AT91C_OTGHS_VBUSTI)

/// requests for VBus activation
//#define Usb_enable_vbus()           (OTGCON  |=  (1<<VBUSREQ))
#define Usb_enable_vbus()             AT91C_BASE_OTGHS->OTGHS_SFR = AT91C_OTGHS_VBUSRQ //JCB
/// requests for VBus desactivation
#define Usb_disable_vbus()            AT91C_BASE_OTGHS->OTGHS_SCR = AT91C_OTGHS_VBUSRQ //JCB
/// Manually request VBUS without UVCON signal from USB interface
//#define Usb_disable_uvcon_pin()     (UHWCON &= ~(1<<UVCONE))
//#define Usb_enable_manual_vbus()    (PORTE|=0x80,DDRE|=0x80,Usb_disable_uvcon_pin())
#define Usb_enable_manual_vbus()      AT91C_BASE_OTGHS->OTGHS_SFR = AT91C_OTGHS_VBUSRQ //JCB
/// Manually request VBUS OFF without UVCON signal from USB interface
//#define Usb_disable_manual_vbus()     (PORTE&=~0x80,DDRE|=0x80,Usb_enable_uvcon_pin())
#define Usb_disable_manual_vbus()     AT91C_BASE_OTGHS->OTGHS_SCR = AT91C_OTGHS_VBUSRQ //JCB

/// initiates a Host Negociation Protocol
//#define Usb_device_initiate_hnp()     (OTGCON  |=  (1<<HNPREQ))
#define Usb_device_initiate_hnp()       AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_HNPREQ
/// stops a Host Negociation Protocol
//#define Usb_device_stop_hnp()         (OTGCON  &=  ~(1<<HNPREQ))
#define Usb_device_stop_hnp()           AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_HNPREQ
/// accepts a Host Negociation Protocol
//#define Usb_host_accept_hnp()         (OTGCON  |=  (1<<HNPREQ))
#define Usb_host_accept_hnp()           AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_HNPREQ
/// rejects a Host Negociation Protocol
//#define Usb_host_reject_hnp()         (OTGCON  &= ~(1<<HNPREQ))
#define Usb_host_reject_hnp()           AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_HNPREQ
/// initiates a Session Request Protocol
//#define Usb_device_initiate_srp()     (OTGCON  |=  (1<<SRPREQ))
#define Usb_device_initiate_srp()       AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_SRPREQ
/// selects VBus as SRP method
//#define Usb_select_vbus_srp_method()  (OTGCON  |=  (1<<SRPSEL))
#define Usb_select_vbus_srp_method()    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_SRPSEL
/// selects data line as SRP method
//#define Usb_select_data_srp_method()  (OTGCON  &= ~(1<<SRPSEL))
#define Usb_select_data_srp_method()    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_SRPSEL


/// enables hardware control on VBus
#define Usb_enable_vbus_hw_control()   // AT91C_BASE_OTGHS->OTGHS_CTRL &= ~AT91C_OTGHS_VBUSHWC
/// disables hardware control on VBus
#define Usb_disable_vbus_hw_control()   AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_VBUSHWC
/// tests if VBus has been requested
//#define Is_usb_vbus_enabled()         ((OTGCON &   (1<<VBUSREQ)) ? TRUE : FALSE)
/// tests if a HNP occurs
//#define Is_usb_hnp()                  ((OTGCON &   (1<<HNPREQ))  ? TRUE : FALSE)
#define Is_usb_hnp()                    ((AT91C_BASE_OTGHS->OTGHS_SR&AT91C_OTGHS_ROLEEX) == AT91C_OTGHS_ROLEEX)
/// tests if a SRP from device occurs
//#define Is_usb_device_srp()           ((OTGCON &   (1<<SRPREQ))  ? TRUE : FALSE)
/// tests if device is initiating SRP
//#define Is_usb_device_initiating_srp()  ((OTGCON &   (1<<SRPREQ))  ? TRUE : FALSE)
#define Is_usb_device_initiating_srp()    (AT91C_BASE_OTGHS->OTGHS_CTRL&AT91C_OTGHS_SRPREQ)

/// acks suspend time out interrupt
//#define Usb_ack_suspend_time_out_interrupt()     (OTGINT  &= ~(1<<STOI))
#define Usb_ack_suspend_time_out_interrupt()       AT91C_BASE_OTGHS->OTGHS_SCR = AT91C_OTGHS_STO
/// enables suspend time out interrupt
//#define Usb_enable_suspend_time_out_interrupt()  (OTGIEN  |=  (1<<STOE))
#define Usb_enable_suspend_time_out_interrupt()    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_STO
/// disables suspend time out interrupt
//#define Usb_disable_suspend_time_out_interrupt() (OTGIEN  &= ~(1<<STOE))
#define Usb_disable_suspend_time_out_interrupt()   AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_STO
//#define Is_suspend_time_out_interrupt_enabled()  ((OTGIEN &  (1<<STOE))   ? TRUE : FALSE)
#define Is_suspend_time_out_interrupt_enabled()    ((AT91C_BASE_OTGHS->OTGHS_CTRL&AT91C_OTGHS_STO)==AT91C_OTGHS_STO)
/// tests if a suspend time out occurs
//#define Is_usb_suspend_time_out_interrupt()      ((OTGINT &   (1<<STOI))    ? TRUE : FALSE)
#define Is_usb_suspend_time_out_interrupt()        ((AT91C_BASE_OTGHS->OTGHS_SR&AT91C_OTGHS_STO)==AT91C_OTGHS_STO)

/// enables HNP error interrupt
//#define Usb_enable_hnp_error_interrupt()   (OTGIEN  |=  (1<<HNPERRE))
#define Usb_enable_hnp_error_interrupt()     AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_HNPERR
/// disables HNP error interrupt
//#define Usb_disable_hnp_error_interrupt()  (OTGIEN  &= ~(1<<HNPERRE))
#define Usb_disable_hnp_error_interrupt()    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_HNPERR
//#define Is_hnp_error_interrupt_enabled()   ((OTGIEN &  (1<<HNPERRE))   ? TRUE : FALSE)
#define Is_hnp_error_interrupt_enabled()     ((AT91C_BASE_OTGHS->OTGHS_CTRL&AT91C_OTGHS_HNPERR)==AT91C_OTGHS_HNPERR)
/// acks HNP error interrupt
//#define Usb_ack_hnp_error_interrupt()      (OTGINT  &= ~(1<<HNPERRI))
#define Usb_ack_hnp_error_interrupt()        AT91C_BASE_OTGHS->OTGHS_SCR |= AT91C_OTGHS_HNPERR
/// tests if a HNP error occurs
//#define Is_usb_hnp_error_interrupt()       ((OTGINT &   (1<<HNPERRI)) ? TRUE : FALSE)
#define Is_usb_hnp_error_interrupt()         ((AT91C_BASE_OTGHS->OTGHS_SR&AT91C_OTGHS_HNPERR) == AT91C_OTGHS_HNPERR)

/// enables role exchange interrupt
//#define Usb_enable_role_exchange_interrupt()   (OTGIEN  |=  (1<<ROLEEXE))
#define Usb_enable_role_exchange_interrupt()     AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_ROLEEX
/// disables role exchange interrupt
//#define Usb_disable_role_exchange_interrupt()  (OTGIEN  &= ~(1<<ROLEEXE))
#define Usb_disable_role_exchange_interrupt()    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_ROLEEX
//#define Is_role_exchange_interrupt_enabled()   ((OTGIEN &  (1<<ROLEEXE))   ? TRUE : FALSE)
#define Is_role_exchange_interrupt_enabled()     ((AT91C_BASE_OTGHS->OTGHS_CTRL&AT91C_OTGHS_ROLEEX)==AT91C_OTGHS_ROLEEX)
/// acks role exchange interrupt
//#define Usb_ack_role_exchange_interrupt()      (OTGINT  &= ~(1<<ROLEEXI))
#define Usb_ack_role_exchange_interrupt()        AT91C_BASE_OTGHS->OTGHS_SCR |= AT91C_OTGHS_ROLEEX
/// tests if a role exchange occurs
//#define Is_usb_role_exchange_interrupt()       ((OTGINT &   (1<<ROLEEXI)) ? TRUE : FALSE)
#define Is_usb_role_exchange_interrupt()         ((AT91C_BASE_OTGHS->OTGHS_SR&AT91C_OTGHS_ROLEEX)==AT91C_OTGHS_ROLEEX)

/// enables B device connection error interrupt
//#define Usb_enable_bconnection_error_interrupt()  (OTGIEN  |=  (1<<BCERRE))
#define Usb_enable_bconnection_error_interrupt()    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_BCERR
/// disables B device connection error interrupt
//#define Usb_disable_bconnection_error_interrupt() (OTGIEN  &= ~(1<<BCERRE))
#define Usb_disable_bconnection_error_interrupt()   AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_BCERR
//#define Is_bconnection_error_interrupt_enabled()  ((OTGIEN &  (1<<BCERRE))   ? TRUE : FALSE)
/// acks B device connection error interrupt
#define Usb_ack_bconnection_error_interrupt()       AT91C_BASE_OTGHS->OTGHS_SCR = AT91C_OTGHS_BCERR
/// tests if a B device connection error occurs
#define Is_usb_bconnection_error_interrupt()     FALSE//jcb   (AT91C_BASE_OTGHS->OTGHS_SR & AT91C_OTGHS_BCERR)

/// enables VBus error interrupt
//#define Usb_enable_vbus_error_interrupt()    (OTGIEN  |=  (1<<VBERRE))
/// disables VBus error interrupt
//#define Usb_disable_vbus_error_interrupt()   (OTGIEN  &= ~(1<<VBERRE))
//#define Is_vbus_error_interrupt_enabled()    ((OTGIEN &  (1<<VBERRE))   ? TRUE : FALSE)
/// acks VBus error interrupt
#define Usb_ack_vbus_error_interrupt()    AT91C_BASE_OTGHS->OTGHS_SCR = AT91C_OTGHS_VBERR
/// tests if a VBus error occurs
#define Is_usb_vbus_error_interrupt()     ((AT91C_BASE_OTGHS->OTGHS_SR&AT91C_OTGHS_VBERR)==AT91C_OTGHS_VBERR)

/// enables SRP interrupt
//#define Usb_enable_srp_interrupt()      (OTGIEN  |=  (1<<SRPE))
#define Usb_enable_srp_interrupt()        AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_SRP
/// disables SRP interrupt
//#define Usb_disable_srp_interrupt()     (OTGIEN  &= ~(1<<SRPE))
#define Usb_disable_srp_interrupt()       AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_SRP
//#define Is_srp_interrupt_enabled()      ((OTGIEN &  (1<<SRPE))   ? TRUE : FALSE)
#define Is_usb_srp_interrupt_enabled()    ((AT91C_BASE_OTGHS->OTGHS_CTRL&AT91C_OTGHS_SRP)==AT91C_OTGHS_SRP)
/// acks SRP interrupt
#define Usb_ack_srp_interrupt()           AT91C_BASE_OTGHS->OTGHS_SCR = AT91C_OTGHS_SRP
/// tests if a SRP occurs
#define Is_usb_srp_interrupt()            ((AT91C_BASE_OTGHS->OTGHS_SR&AT91C_OTGHS_SRP)==AT91C_OTGHS_SRP)


//------------------------------------------------------------------------------
//   D E V I C E
//------------------------------------------------------------------------------

/// USB_device_driver USB device controller drivers
/// These macros manage the USB Device controller.

/// test if remote wake-up still running
//#define Is_usb_pending_remote_wake_up()           ((UDCON & (1<<RMWKUP)) ? TRUE : FALSE)
#define Is_usb_pending_remote_wake_up()             (AT91C_BASE_OTGHS->OTGHS_DEVCTRL | AT91C_OTGHS_RMWKUP)
/// acks remote wake-up
//#define Usb_ack_remote_wake_up_start()            (UDINT   = ~(1<<UPRSMI))
#define Usb_ack_remote_wake_up_start()              AT91C_BASE_OTGHS->OTGHS_DEVICR = AT91C_OTGHS_UPRSM

/// enables resume interrupt
//#define Usb_enable_resume_interrupt()   (UDIEN   |=  (1<<EORSME))
/// disables resume interrupt
#define Usb_disable_resume_interrupt()    AT91C_BASE_OTGHS->OTGHS_DEVIDR = AT91C_OTGHS_EORSM
#define Is_resume_interrupt_enabled()     (AT91C_BASE_OTGHS->OTGHS_DEVISR & AT91C_OTGHS_EORSM)
/// acks resume
#define Usb_ack_resume()                  AT91C_BASE_OTGHS->OTGHS_DEVICR = AT91C_OTGHS_EORSM
/// tests if resume occurs
#define Is_usb_resume()                   (AT91C_BASE_OTGHS->OTGHS_DEVISR & AT91C_OTGHS_EORSM)

/// enables wake-up interrupt
#define Usb_enable_wake_up_interrupt()    AT91C_BASE_OTGHS->OTGHS_DEVIER = AT91C_OTGHS_WAKEUP
/// disables wake-up interrupt
#define Usb_disable_wake_up_interrupt()   AT91C_BASE_OTGHS->OTGHS_DEVIDR = AT91C_OTGHS_WAKEUP
#define Is_swake_up_interrupt_enabled()   (AT91C_BASE_OTGHS->OTGHS_DEVIMR & AT91C_OTGHS_WAKEUP)
/// acks wake-up
#define Usb_ack_wake_up()                 AT91C_BASE_OTGHS->OTGHS_DEVICR = AT91C_OTGHS_WAKEUP
/// tests if wake-up occurs
#define Is_usb_wake_up()                  (AT91C_BASE_OTGHS->OTGHS_DEVISR & AT91C_OTGHS_WAKEUP)

/// enables USB reset interrupt
#define Usb_enable_reset_interrupt()      AT91C_BASE_OTGHS->OTGHS_DEVIER = AT91C_OTGHS_EORST
/// disables USB reset interrupt
//#define Usb_disable_reset_interrupt()   (UDIEN   &= ~(1<<EORSTE))
#define Is_reset_interrupt_enabled()      (AT91C_BASE_OTGHS->OTGHS_DEVIMR & AT91C_OTGHS_EORST)
/// acks USB reset
#define Usb_ack_reset()                   AT91C_BASE_OTGHS->OTGHS_DEVICR = AT91C_OTGHS_EORST
/// tests if USB reset occurs
#define Is_usb_reset()                    (AT91C_BASE_OTGHS->OTGHS_DEVISR & AT91C_OTGHS_EORST)

/// enables Start Of Frame Interrupt
#define Usb_enable_sof_interrupt()        AT91C_BASE_OTGHS->OTGHS_DEVIER = AT91C_OTGHS_SOF
/// disables Start Of Frame Interrupt
//#define Usb_disable_sof_interrupt()     (UDIEN   &= ~(1<<SOFE))
#define Usb_disable_sof_interrupt()       AT91C_BASE_OTGHS->OTGHS_DEVIDR = AT91C_OTGHS_SOF
#define Is_sof_interrupt_enabled()        (AT91C_BASE_OTGHS->OTGHS_DEVIMR & AT91C_OTGHS_SOF)
/// acks Start Of Frame
#define Usb_ack_sof()                     AT91C_BASE_OTGHS->OTGHS_DEVICR = AT91C_OTGHS_SOF
/// tests if Start Of Frame occurs
#define Is_usb_sof()                      (AT91C_BASE_OTGHS->OTGHS_DEVISR & AT91C_OTGHS_SOF)

/// enables suspend state interrupt
#define Usb_enable_suspend_interrupt()    AT91C_BASE_OTGHS->OTGHS_DEVIER = AT91C_OTGHS_SUSP
/// disables suspend state interrupt
//#define Usb_disable_suspend_interrupt() (UDIEN   &= ~(1<<SUSPE))
#define Usb_disable_suspend_interrupt()    AT91C_BASE_OTGHS->OTGHS_DEVIDR = AT91C_OTGHS_SUSP
#define Is_suspend_interrupt_enabled()    (AT91C_BASE_OTGHS->OTGHS_DEVIMR & AT91C_OTGHS_SUSP)
/// acks Suspend
#define Usb_ack_suspend()                 AT91C_BASE_OTGHS->OTGHS_DEVICR = AT91C_OTGHS_SUSP
/// tests if Suspend state detected
#define Is_usb_suspend()                  (AT91C_BASE_OTGHS->OTGHS_DEVISR & AT91C_OTGHS_SUSP)

/// returns the last frame number
//#define Usb_frame_number()                 ((unsigned short)((((unsigned short)UDFNUMH) << 8) | ((unsigned short)UDFNUML)))
/// tests if a crc error occurs in frame number
//#define Is_usb_frame_number_crc_error()         ((UDMFN & (1<<FNCERR)) ? TRUE : FALSE)
/// @}
#define UDFNUML ((AT91C_BASE_OTGHS->OTGHS_DEVFNUM&AT91C_OTGHS_FRAME_NUMBER)>>3)


// ****************************************************************************
// ****************************************************************************
//   H O S T
// ****************************************************************************
// ****************************************************************************

/// @defgroup host_management USB host controller drivers
/// These macros manage the USB Host controller.
/// @{
/// allocates the current configuration in DPRAM memory
//#define Host_allocate_memory()                 (UPCFG1X |=  (1<<ALLOC))
#define Host_allocate_memory()                
/// un-allocates the current configuration in DPRAM memory
//#define Host_unallocate_memory()               (UPCFG1X &= ~(1<<ALLOC))
#define Host_unallocate_memory()

extern void host_configure_pipe(U8 pip, U8 type, U8 token, U8 ep_num, U8 size, U8 bank, U8 freq);

#define Is_usb_pipe_configured(pip) \
      ((0 == (AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[pip] & AT91C_OTGHS_CFGOK))?FALSE:TRUE)

/// enables SOF generation
//#define Host_enable_sof()           (UHCON |= (1<<SOFEN))
#define Host_enable_sof()             AT91C_BASE_OTGHS->OTGHS_HSTCTRL |= AT91C_OTGHS_SOFHST

/// disables SOF generation
#define Host_disable_sof()            AT91C_BASE_OTGHS->OTGHS_HSTCTRL &= ~(unsigned int)AT91C_OTGHS_SOFHST

/// sends a USB Reset to the device
#define Host_send_reset()             AT91C_BASE_OTGHS->OTGHS_HSTCTRL |= AT91C_OTGHS_RESET
//                                    *AT91C_OTGHS_TSTA2 |= AT91C_OTGHS_FORCHSRST;
//                                    *AT91C_OTGHS_TSTA2 |= AT91C_OTGHS_UTMIRESET;
//                                    *AT91C_OTGHS_TSTA2 &= ~AT91C_OTGHS_UTMIRESET

/// tests if USB Reset running
#define Is_host_sending_reset()       (AT91C_BASE_OTGHS->OTGHS_HSTCTRL&AT91C_OTGHS_RESET)
//#define Host_is_reset()             ((UHCON & (1<<RESET)) ? TRUE : FALSE)
#define Host_is_reset()               (AT91C_BASE_OTGHS->OTGHS_HSTCTRL&AT91C_OTGHS_RESET)
#define Is_host_reset()               (AT91C_BASE_OTGHS->OTGHS_HSTCTRL&AT91C_OTGHS_RESET)

/// sends a USB Resume to the device
#define Host_send_resume()            AT91C_BASE_OTGHS->OTGHS_HSTCTRL |= AT91C_OTGHS_RESUME
/// tests if USB Resume running
//#define Host_is_resume()            ((UHCON & (1<<RESUME)) ? TRUE : FALSE)
#define Host_is_resume()              (AT91C_BASE_OTGHS->OTGHS_HSTCTRL&AT91C_OTGHS_RESUME)

/// enables host start of frame interrupt
//#define Host_enable_sof_interrupt()           (UHIEN |= (1<<HSOFE))
#define Host_enable_sof_interrupt()             AT91C_BASE_OTGHS->OTGHS_HSTIER = AT91C_OTGHS_HSOFI
/// enables host start of frame interrupt
#define Host_disable_sof_interrupt()            AT91C_BASE_OTGHS->OTGHS_HSTIDR = AT91C_OTGHS_HSOFI
#define Is_host_sof_interrupt_enabled()         (AT91C_BASE_OTGHS->OTGHS_HSTIMR&AT91C_OTGHS_HSOFI)
/// tests if SOF detected
//#define Host_is_sof()                         ((UHINT & (1<<HSOFI)) ? TRUE : FALSE)
#define Is_host_sof()                           (AT91C_BASE_OTGHS->OTGHS_HSTISR&AT91C_OTGHS_HSOFI)
#define Host_ack_sof()                          AT91C_BASE_OTGHS->OTGHS_HSTICR = AT91C_OTGHS_HSOFI

/// enables host wake up interrupt detection
#define Host_enable_hwup_interrupt()            AT91C_BASE_OTGHS->OTGHS_HSTIER = AT91C_OTGHS_HWUPI
/// disables host wake up interrupt detection
#define Host_disable_hwup_interrupt()           AT91C_BASE_OTGHS->OTGHS_HSTIDR = AT91C_OTGHS_HWUPI
#define Is_host_hwup_interrupt_enabled()        (AT91C_BASE_OTGHS->OTGHS_HSTIMR&AT91C_OTGHS_HWUPI)
/// tests if host wake up detected
//#define Host_is_hwup()                        ((UHINT & (1<<HWUPI)) ? TRUE : FALSE)
/// Ack host wake up detection
#define Is_host_hwup()                          (AT91C_BASE_OTGHS->OTGHS_HSTISR&AT91C_OTGHS_HWUPI)
#define Host_ack_hwup()                         AT91C_BASE_OTGHS->OTGHS_HSTICR = AT91C_OTGHS_HWUPI


/// enables host down stream rsm sent interrupt detection
#define Host_enable_down_stream_resume_interrupt()            AT91C_BASE_OTGHS->OTGHS_HSTIER = AT91C_OTGHS_RSMEDI
/// disables host down stream rsm sent interrupt detection
#define Host_disable_down_stream_resume_interrupt()           AT91C_BASE_OTGHS->OTGHS_HSTIDR = AT91C_OTGHS_RSMEDI
//#define Is_host_down_stream_resume_interrupt_enabled()      ((UHIEN &  (1<<RSMEDE))   ? TRUE : FALSE)
/// Ack host down stream resume sent
//#define Is_host_down_stream_resume()       ((UHINT & (1<<RSMEDI)) ? TRUE : FALSE)
#define Is_host_down_stream_resume()         (AT91C_BASE_OTGHS->OTGHS_HSTISR&AT91C_OTGHS_RSMEDI)
//#define Host_ack_down_stream_resume()      (UHINT &= ~(1<<RSMEDI))
#define Host_ack_down_stream_resume()        AT91C_BASE_OTGHS->OTGHS_HSTICR = AT91C_OTGHS_RSMEDI

/// enables host remote wake up interrupt detection
//#define Host_enable_remote_wakeup_interrupt()         (UHIEN |= (1<<RXRSME))
#define Host_enable_remote_wakeup_interrupt()           AT91C_BASE_OTGHS->OTGHS_HSTIER = AT91C_OTGHS_RXRSMI
/// disables host remote wake up interrupt detection
//#define Host_disable_remote_wakeup_interrupt()        (UHIEN &= ~(1<<RXRSME))
#define Host_disable_remote_wakeup_interrupt()          AT91C_BASE_OTGHS->OTGHS_HSTIDR = AT91C_OTGHS_RXRSMI
//#define Is_host_remote_wakeup_interrupt_enabled()     ((UHIEN &  (1<<RXRSME))   ? TRUE : FALSE)
#define Is_host_remote_wakeup_interrupt_enabled()       (AT91C_BASE_OTGHS->OTGHS_HSTISR&AT91C_OTGHS_RXRSMI)
/// tests if host wake up detected
//#define Host_is_remote_wakeup()                       ((UHINT & (1<<RXRSMI)) ? TRUE : FALSE)
/// Ack host wake up detection
//#define Is_host_remote_wakeup()                       ((UHINT & (1<<RXRSMI)) ? TRUE : FALSE)
#define Is_host_remote_wakeup()                         ((AT91C_BASE_OTGHS->OTGHS_HSTISR&AT91C_OTGHS_RXRSMI) == AT91C_OTGHS_RXRSMI)
#define Host_ack_remote_wakeup()                        AT91C_BASE_OTGHS->OTGHS_HSTICR = AT91C_OTGHS_RXRSMI

#define Host_disable_device_connection_interrupt()       AT91C_BASE_OTGHS->OTGHS_HSTIDR = AT91C_OTGHS_DCONN
/// enables host device connection interrupt
//#define Host_enable_device_connection_interrupt()      (UHIEN |= (1<<DCONNE))
#define Host_enable_device_connection_interrupt()        AT91C_BASE_OTGHS->OTGHS_HSTIER = AT91C_OTGHS_DCONN
/// disables USB device connection interrupt
//#define Host_disable_device_connection_interrupt()     (UHIEN &= ~(1<<DCONNE))
#define Is_host_device_connection_interrupt_enabled()    (AT91C_BASE_OTGHS->OTGHS_HSTIMR & AT91C_OTGHS_DCONN)
/// tests if a USB device has been detected
//#define Is_device_connection()                 (UHINT & (1<<DCONNI))
#define Is_device_connection()                   (AT91C_BASE_OTGHS->OTGHS_HSTISR & AT91C_OTGHS_DCONN)
#define Is_host_device_connection()              (AT91C_BASE_OTGHS->OTGHS_HSTISR & AT91C_OTGHS_DCONN)
/// acks device connection
//#define Host_ack_device_connection()           (UHINT = ~(1<<DCONNI))
#define Host_ack_device_connection()             AT91C_BASE_OTGHS->OTGHS_HSTICR = AT91C_OTGHS_DCONN

/// tests if a USB device has been removed
//#define Is_device_disconnection()              (UHINT & (1<<DDISCI)   ? TRUE : FALSE)
#define Is_device_disconnection()                (AT91C_BASE_OTGHS->OTGHS_HSTISR & AT91C_OTGHS_DDIS)
/// enables host device disconnection interrupt
//#define Host_enable_device_disconnection_interrupt()      (UHIEN |= (1<<DDISCE))
#define Host_enable_device_disconnection_interrupt()        AT91C_BASE_OTGHS->OTGHS_HSTIER = AT91C_OTGHS_DDIS
/// disables USB device connection interrupt
//#define Host_disable_device_disconnection_interrupt()     (UHIEN &= ~(1<<DDISCE))
#define Host_disable_device_disconnection_interrupt()       AT91C_BASE_OTGHS->OTGHS_HSTIDR = AT91C_OTGHS_DDIS
//#define Is_host_device_disconnection_interrupt_enabled()  ((UHIEN &  (1<<DDISCE))   ? TRUE : FALSE)
#define Is_host_device_disconnection_interrupt_enabled()    (AT91C_BASE_OTGHS->OTGHS_HSTIMR & AT91C_OTGHS_DDIS)
/// tests if a USB device has been removed
//#define Is_device_disconnection()             (UHINT & (1<<DDISCI)   ? TRUE : FALSE)
//#define Is_host_device_disconnection()          (AT91C_BASE_OTGHS->OTGHS_HSTISR & AT91C_OTGHS_DDIS)
/// acks device disconnection
//#define Host_ack_device_disconnection()       (UHINT = ~(1<<DDISCI))
#define Host_ack_device_disconnection()         AT91C_BASE_OTGHS->OTGHS_HSTICR = AT91C_OTGHS_DDIS

#define host_get_device_descriptor_incomplete() (usb_request.uncomplete_read == TRUE)
#define Host_enable_reset_sent_interrupt()      AT91C_BASE_OTGHS->OTGHS_HSTIER = AT91C_OTGHS_RSTI 


#define Host_disable_reset_sent_interrupt()     AT91C_BASE_OTGHS->OTGHS_HSTIDR = AT91C_OTGHS_RSTI 
/// enables host USB reset interrupt
//#define Host_enable_reset_interrupt()         (UHIEN   |=  (1<<RSTE))
/// disables host USB reset interrupt
//#define Host_disable_reset_interrupt()        (UHIEN   &= ~(1<<RSTE))
#define Host_disable_reset_interrupt()          (AT91C_BASE_OTGHS->OTGHS_HSTIDR = AT91C_OTGHS_RSTI)
//#define Is_host_reset_interrupt_enabled()     ((UHIEN &  (1<<RSTE))   ? TRUE : FALSE)
/// acks host USB reset sent
//#define Host_ack_reset()                      (UHINT   = ~(1<<RSTI))
#define Host_ack_reset()                        AT91C_BASE_OTGHS->OTGHS_HSTICR = AT91C_OTGHS_RSTI
/// tests if USB reset has been sent


/// switches on VBus
//#define Host_vbus_request()                  (OTGCON |= (1<<VBUSREQ))
/// switches off VBus
//#define Host_clear_vbus_request()            (OTGCON |= (1<<VBUSRQC))
#define Host_clear_vbus_request()              AT91C_BASE_OTGHS->OTGHS_SCR = AT91C_OTGHS_VBUSRQ //JCB
/// configures the address to use for the device
//#define Host_configure_address(addr)         (UHADDR = addr & MSK_HADDR)
//#define Host_configure_address(addr)           AT91C_BASE_OTGHS->OTGHS_HSTCTRL |= addr & 0x3F
//#define Host_configure_address(addr)  AT91C_BASE_OTGHS->OTGHS_HSTADDR1 &= ~AT91C_OTGHS_UHADDR_P0;
//                                      AT91C_BASE_OTGHS->OTGHS_HSTADDR1 |= (addr & AT91C_OTGHS_UHADDR_P0)

/// Get connected device speed, returns TRUE when in full speed mode
#define Is_host_full_speed()                   (AT91C_BASE_OTGHS->OTGHS_SR&AT91C_OTGHS_SPEED)
/// @}

#define Usb_disable_otg_pad()                  AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_OTGPADE



/// @defgroup general_pipe USB pipe drivers
/// These macros manage the common features of the pipes.
/// @{
/// selects pipe for CPU interface
//#define Host_select_pipe(p)                 (UPNUM = (unsigned char)p)
#define Host_select_pipe(p)                   global_pipe_nb = p

/// get the currently selected pipe number
//#define Host_get_selected_pipe()            (UPNUM )
#define Host_get_selected_pipe()              (global_pipe_nb)

/// enables pipe
//#define Host_enable_pipe()                  (UPCONX |= (1<<PEN))
#define Host_enable_pipe()                    AT91C_BASE_OTGHS->OTGHS_HSTPIP |= (1<<global_pipe_nb)
/// disables pipe
//#define Host_disable_pipe()                 (UPCONX &= ~(1<<PEN))
#define Host_disable_pipe()                   AT91C_BASE_OTGHS->OTGHS_HSTPIP &= ~(unsigned int)(1<<global_pipe_nb)

/// sets SETUP token
//#define Host_set_token_setup()  (UPCFG0X =  UPCFG0X & ~MSK_TOKEN_SETUP) AT91C_OTGHS_PTOKEN
#define Host_set_token_setup()    AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[global_pipe_nb] &= ~(unsigned int)AT91C_OTGHS_PTOKEN;\
                                  AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[global_pipe_nb] |= AT91C_OTGHS_PTOKEN_SETUP
/// sets IN token
//#define Host_set_token_in()     (UPCFG0X = (UPCFG0X & ~MSK_TOKEN_SETUP) | MSK_TOKEN_IN)
#define Host_set_token_in()       AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[global_pipe_nb] &= ~(unsigned int)AT91C_OTGHS_PTOKEN;\
                                  AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[global_pipe_nb] |= AT91C_OTGHS_PTOKEN_IN
/// sets OUT token
//#define Host_set_token_out()    (UPCFG0X = (UPCFG0X & ~MSK_TOKEN_SETUP) | MSK_TOKEN_OUT)
#define Host_set_token_out()      AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[global_pipe_nb] &= ~(unsigned int)AT91C_OTGHS_PTOKEN;\
                                  AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[global_pipe_nb] |= AT91C_OTGHS_PTOKEN_OUT

/// returns the number of the endpoint associated to the current pipe
//#define Host_get_endpoint_number()             (UPCFG0X & (1<<MSK_PEPNUM))
//#define Host_get_endpoint_number()             (UPCFG0X & (MSK_PEPNUM))
#define Host_get_endpoint_number() (unsigned char)((AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[global_pipe_nb]&AT91C_OTGHS_PEPNUM)>>16)

/// returns pipe interrupt register
#define Host_get_pipe_interrupt()           ((AT91C_BASE_OTGHS->OTGHS_HSTISR>>8)&0x7F)

/// sets the interrupt frequency
//#define Host_set_interrupt_frequency(pip, frq) (AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[pip]|=AT91C_OTGHS_INTFRQ&(frq<<24))

/// tests if current pipe is configured
#define Is_pipe_configured()                (AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[global_pipe_nb]&AT91C_OTGHS_CFGOK)
/// tests if at least one bank is busy
//#define Is_host_one_bank_busy()           ((UPSTAX &  (1<<MSK_NBUSYBK)) != 0)
#define Is_host_one_bank_busy(pip)          ((AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[pip]&AT91C_OTGHS_NBUSYBK)!=0)
/// returns the number of busy banks
//#define Host_number_of_busy_bank()        (UPSTAX &  (1<<MSK_NBUSYBK))

/// resets the pipe
//#define Host_reset_pipe(p)                (UPRST = 1<<p , UPRST = 0)
#define Host_reset_pipe(pip)                AT91C_BASE_OTGHS->OTGHS_HSTPIP|=(1<<16<<pip);\
                                            AT91C_BASE_OTGHS->OTGHS_HSTPIP&=~(unsigned int)(1<<16<<pip)

/// writes a byte into the pipe FIFO
//#define Host_write_byte(dat)              (UPDATX = dat)
//#define Host_write_byte(data)             AT91C_BASE_OTGHS_EPTFIFO->OTGHS_READEPT0[global_pipe_nb << 14]=data
//#define Host_write_byte(data)      (((char*)((unsigned int *)AT91C_BASE_OTGHS_EPTFIFO->OTGHS_READEPT0))[dBytes++])=data
#define Host_write_byte(data)        pFifo[dBytes++]=data

#define Host_write_32(data)      AT91C_BASE_OTGHS_EPTFIFO->OTGHS_READEPT0[0]=data
/// reads a byte from the pipe FIFO
//#define Host_read_byte()                       (UPDATX)
//#define Host_read_byte()           (U8)(((char*)((unsigned int *)AT91C_BASE_OTGHS_EPTFIFO->OTGHS_READEPT0))[dBytes++])
#define Host_read_byte()             pFifo[dBytes++]
//#define Host_read_byte_index(pip, index)     pfifo[index] = pEndpoint->pData[index++];
//#define Host_read_32(pip)          AT91C_BASE_OTGHS_EPTFIFO->OTGHS_READEPT0[pip << 14]

/// freezes the pipe
//#define Host_freeze_pipe()                     (UPCONX |=  (1<<PFREEZE))
//#define Host_freeze_pipe()                    TRACE_DEBUG("freeze\n\r");AT91C_BASE_OTGHS->OTGHS_HSTPIPIER[global_pipe_nb] = AT91C_OTGHS_FREEZE
#define Host_freeze_pipe()      AT91C_BASE_OTGHS->OTGHS_HSTPIPIER[global_pipe_nb] = AT91C_OTGHS_FREEZE
/// un-freezees the pipe
//#define Host_unfreeze_pipe()                   (UPCONX &= ~(1<<PFREEZE))
//#define Host_unfreeze_pipe()  TRACE_DEBUG("unfreeze\n\r");while(!(AT91C_BASE_OTGHS->OTGHS_HSTPIPIMR[global_pipe_nb] & AT91C_OTGHS_FREEZE));
//                              AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[global_pipe_nb] = AT91C_OTGHS_FREEZE
#define Host_unfreeze_pipe()    while(!(AT91C_BASE_OTGHS->OTGHS_HSTPIPIMR[global_pipe_nb] & AT91C_OTGHS_FREEZE));\
                                AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[global_pipe_nb] = AT91C_OTGHS_FREEZE
/// tests if the current pipe is frozen
//#define Is_host_pipe_freeze()                  (UPCONX &   (1<<PFREEZE))
#define Is_host_pipe_freeze()                  (AT91C_BASE_OTGHS->OTGHS_HSTPIPIMR[global_pipe_nb] & AT91C_OTGHS_FREEZE)

/// resets data toggle
#define Host_reset_pipe_data_toggle(pip)          AT91C_BASE_OTGHS->OTGHS_HSTPIPIER[pip] = AT91C_OTGHS_RSTDT

/// tests if SETUP has been sent
#define Is_host_setup_sent()                   (AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[0] & AT91C_OTGHS_TXSTPI)
/// tests if control IN has been received
#define Is_host_control_in_received()          (AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[0] & AT91C_OTGHS_RXINI)
/// tests if control OUT has been sent
//#define Is_host_control_out_sent()             ((UPINTX & (1<<TXOUTI))    ? TRUE : FALSE)
#define Is_host_control_out_sent()             (AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[0] & AT91C_OTGHS_TXOUT)
/// tests if a STALL has been received
//#define Is_host_stall()                        ((UPINTX & (1<<RXSTALLI))    ? TRUE : FALSE)
#define Is_host_stall()                        (AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[global_pipe_nb] & AT91C_OTGHS_RXSTALL)
/// tests if an error occurs on current pipe
//#define Is_host_pipe_error()                   ((UPINTX & (1<<PERRI))    ? TRUE : FALSE)
#define Is_host_pipe_error()                   (AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[global_pipe_nb] & AT91C_OTGHS_PERR)

/// sends a setup
//#define Host_send_setup()                      (UPINTX  &= ~(1<<FIFOCON))
#define Host_send_setup()                      AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[0] = AT91C_OTGHS_FIFOCON
/// sends a control IN
//#define Host_send_control_in()                 (UPINTX  &= ~(1<<FIFOCON))
#define Host_send_control_in()                 AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[0] = AT91C_OTGHS_FIFOCON
#define Host_send_control()                    AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[0] = AT91C_OTGHS_FIFOCON
/// sends a control OUT
//#define Host_send_control_out()                (UPINTX  &= ~(1<<FIFOCON))
#define Host_send_control_out()                AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[0] = AT91C_OTGHS_FIFOCON

/// acks control OUT
//#define Host_ack_control_out()                 (UPINTX  &= ~(1<<TXOUTI))
#define Host_ack_control_out()              AT91C_BASE_OTGHS->OTGHS_HSTPIPICR[0] = AT91C_OTGHS_TXOUT
/// acks control IN
//#define Host_ack_control_in()                  (UPINTX  &= ~(1<<RXINI))
#define Host_ack_control_in()               AT91C_BASE_OTGHS->OTGHS_HSTPIPICR[0] = AT91C_OTGHS_RXINI
/// acks setup
//#define Host_ack_setup()                       (UPINTX  &= ~(1<<TXSTPI))
#define Host_ack_setup()                    AT91C_BASE_OTGHS->OTGHS_HSTPIPICR[0] = AT91C_OTGHS_TXSTPI
/// acks STALL reception
//#define Host_ack_stall()                       (UPINTX  &= ~(1<<RXSTALLI))
#define Host_ack_stall()                    AT91C_BASE_OTGHS->OTGHS_HSTPIPICR[global_pipe_nb] = AT91C_OTGHS_RXSTALL

/// sends a OUT
//#define Host_send_out()                        (UPINTX &= ~(1<<FIFOCON))
#define Host_send_out()                        AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[global_pipe_nb] = AT91C_OTGHS_FIFOCON
/// tests if OUT has been sent
//#define Is_host_out_sent()                     ((UPINTX & (1<<TXOUTI))    ? TRUE : FALSE)
#define Is_host_out_sent()                     (AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[global_pipe_nb] & AT91C_OTGHS_TXOUT)
/// acks OUT sent
//#define Host_ack_out_sent()                    (UPINTX &= ~(1<<TXOUTI))
#define Host_ack_out_sent()                    AT91C_BASE_OTGHS->OTGHS_HSTPIPICR[global_pipe_nb] = AT91C_OTGHS_TXOUT

/// tests if IN received
//#define Is_host_in_received()                  ((UPINTX & (1<<RXINI))    ? TRUE : FALSE)
#define Is_host_in_received()                  (AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[global_pipe_nb] & AT91C_OTGHS_RXINI)
/// acks IN reception
//#define Host_ack_in_received()                 (UPINTX &= ~(1<<RXINI))
#define Host_ack_in_received()                AT91C_BASE_OTGHS->OTGHS_HSTPIPICR[global_pipe_nb] = AT91C_OTGHS_RXINI
/// sends a IN
//#define Host_send_in()                         (UPINTX &= ~(1<<FIFOCON))
#define Host_send_in()                         AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[global_pipe_nb] = AT91C_OTGHS_FIFOCON
/// tests if nak handshake has been received
//#define Is_host_nak_received()               ((UPINTX & (1<<NAKEDI))    ? TRUE : FALSE)
#define Is_host_nak_received()              (AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[global_pipe_nb] & AT91C_OTGHS_NAKEDI)
/// acks NAk received sent
//#define Host_ack_nak_received()              (UPINTX &= ~(1<<NAKEDI))
#define Host_ack_nak_received()             AT91C_BASE_OTGHS->OTGHS_HSTPIPICR[global_pipe_nb] = AT91C_OTGHS_NAKEDI

/// tests if endpoint read allowed
//#define Is_host_read_enabled()               (UPINTX&(1<<RWAL))
#define Is_host_read_enabled()              (AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[global_pipe_nb] & AT91C_OTGHS_RWALL)
/// tests if endpoint read allowed
//#define Is_host_write_enabled()              (UPINTX&(1<<RWAL))

/// sets IN in standard mode
//#define Host_standard_in_mode()              (UPCONX &= ~(1<<INMODE))
#define Host_standard_in_mode()             AT91C_BASE_OTGHS->OTGHS_HSTPIPINRQ[global_pipe_nb] &= ~(unsigned int)AT91C_OTGHS_INMOD
/// sets IN in continuous mode
//#define Host_continuous_in_mode()            (UPCONX |=  (1<<INMODE))
#define Host_continuous_in_mode()           AT91C_BASE_OTGHS->OTGHS_HSTPIPINRQ[global_pipe_nb] |= AT91C_OTGHS_INMOD

/// sets number of IN requests to perform before freeze
//#define Host_in_request_number(in_num)         (UPINRQX = (unsigned char)in_num)
#define Host_in_request_number(pip, in_num)     AT91C_BASE_OTGHS->OTGHS_HSTPIPINRQ[pip] &= ~(unsigned int)AT91C_OTGHS_INMOD;\
                                                AT91C_BASE_OTGHS->OTGHS_HSTPIPINRQ[pip] &= ~(unsigned int)AT91C_OTGHS_INRQ;\
                                                AT91C_BASE_OTGHS->OTGHS_HSTPIPINRQ[pip] |= in_num
/// returns number of remaining IN requests
//#define Host_get_in_request_number()         (UPINRQX)

/// returns number of bytes (8 bits)
//#define Host_data_length_U8()                (UPBCLX)
#define Host_data_length_U8()               (unsigned short)((AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[global_pipe_nb]&AT91C_OTGHS_PBYCT)>>20)
#define UPBCLX                              (unsigned short)((AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[global_pipe_nb]&AT91C_OTGHS_PBYCT)>>20)

/// returns number of bytes (16 bits)
//#define Host_data_length_U16()               ((((unsigned short)UPBCHX)<<8) | UPBCLX)
#define Host_data_length_U16()              (unsigned short)((AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[global_pipe_nb]&AT91C_OTGHS_PBYCT)>>20)
/// for device compatibility
//#define Host_byte_counter()                  Host_data_length_U16()
#define Host_byte_counter()                 Host_data_length_U16()
/// for device compatibility
//#define Host_byte_counter_8()                Host_data_length_U8()

/// returns the size of the current pipe
//#define Host_get_pipe_length()               ((unsigned short)0x08 << ((UPCFG1X & MSK_PSIZE)>>4))
//#define Host_get_pipe_length()              (unsigned short)(((AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[global_pipe_nb]&AT91C_OTGHS_PSIZE)>>4)?
//                                           (8<<((AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[global_pipe_nb]&AT91C_OTGHS_PSIZE)>>4)):8)

/// returns the type of the current pipe
//#define Host_get_pipe_type()                 (UPCFG0X>>6)
#define Host_get_pipe_type()                ((AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[global_pipe_nb]&AT91C_OTGHS_PTYPE)>>12)

/// tests if error occurs on pipe
//#define Host_error_status()                  (UPERRX & MSK_ERROR)
#define Host_error_status()                 (unsigned long)(AT91C_BASE_OTGHS->OTGHS_HSTPIPERR[global_pipe_nb])
/// acks all pipe error
//#define Host_ack_all_errors()                (UPERRX = 0x00)
#define Host_ack_all_errors()               AT91C_BASE_OTGHS->OTGHS_HSTPIPERR[global_pipe_nb] = 0

/// Enable pipe end transmission interrupt
//#define Host_enable_transmit_interrupt()     (UPIENX |= (1<<TXOUTE))
#define Host_enable_transmit_interrupt()     AT91C_BASE_OTGHS->OTGHS_HSTPIPIER[global_pipe_nb] = AT91C_OTGHS_TXOUT
/// Disable pipe end transmission interrupt
//#define Host_disable_transmit_interrupt()    (UPIENX &= ~(1<<TXOUTE))
#define Host_disable_transmit_interrupt()    AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[global_pipe_nb] = AT91C_OTGHS_TXOUT

/// Enable pipe reception interrupt
//#define Host_enable_receive_interrupt()      (UPIENX |= (1<<RXINE))
#define Host_enable_receive_interrupt()     AT91C_BASE_OTGHS->OTGHS_HSTPIPIER[global_pipe_nb] = AT91C_OTGHS_RXINI
/// Disable pipe recption interrupt
//#define Host_disable_receive_interrupt()     (UPIENX &= ~(1<<RXINE))
#define Host_disable_receive_interrupt()    AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[global_pipe_nb] = AT91C_OTGHS_RXINI

/// Enable pipe stall interrupt
//#define Host_enable_stall_interrupt()        (UPIENX |= (1<<RXSTALLE))
#define Host_enable_stall_interrupt()       AT91C_BASE_OTGHS->OTGHS_HSTPIPIER[global_pipe_nb] = AT91C_OTGHS_RXSTALL
/// Disable pipe stall interrupt
//#define Host_disable_stall_interrupt()       (UPIENX &= ~(1<<RXSTALLE))
#define Host_disable_stall_interrupt()      AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[global_pipe_nb] = AT91C_OTGHS_RXSTALL

/// Enable pipe error interrupt
//#define Host_enable_error_interrupt()        (UPIENX |= (1<<PERRE))
#define Host_enable_error_interrupt()       AT91C_BASE_OTGHS->OTGHS_HSTPIPIER[global_pipe_nb] = AT91C_OTGHS_PERR
/// Disable pipe error interrupt
//#define Host_disable_error_interrupt()       (UPIENX &= ~(1<<PERRE))
#define Host_disable_error_interrupt()      AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[global_pipe_nb] = AT91C_OTGHS_PERR

/// Enable pipe NAK interrupt
//#define Host_enable_nak_interrupt()        (UPIENX |= (1<<NAKEDE))
#define Host_enable_nak_interrupt()       AT91C_BASE_OTGHS->OTGHS_HSTPIPIER[global_pipe_nb] = AT91C_OTGHS_NAKEDI
/// Disable pipe NAK interrupt
//#define Host_disable_nak_interrupt()       (UPIENX &= ~(1<<NAKEDE))
#define Host_disable_nak_interrupt()      AT91C_BASE_OTGHS->OTGHS_HSTPIPIDR[global_pipe_nb] = AT91C_OTGHS_NAKEDI

//#define Get_pipe_token(x)                 ((x & (0x80)) ? TOKEN_IN : TOKEN_OUT)
#define Get_pipe_token(x)                 ((x & (0x80)) ? TOKEN_IN : TOKEN_OUT)


//_____ D E C L A R A T I O N ______________________________________________

extern unsigned char dev_configure_endpoint(unsigned char ept,
                                            unsigned char type, 
                                            unsigned char dir,
                                            unsigned char size,
                                            unsigned char bank,
                                            unsigned char nyet);


extern unsigned char usb_select_enpoint_interrupt (void);
extern unsigned char usb_send_packet(unsigned char, unsigned char*, unsigned char);
extern unsigned char usb_read_packet(unsigned char, unsigned char*, unsigned char);

extern unsigned char host_config_pipe(unsigned char, unsigned char);
extern unsigned char host_determine_pipe_size(unsigned short);
extern void host_disable_all_pipe(void);
extern unsigned char host_get_nb_pipe_interrupt(void);
extern U16 host_get_pipe_length(void);
extern void host_configure_address( unsigned char pipe, unsigned char addr);

#endif  // _USB_DRV_H

