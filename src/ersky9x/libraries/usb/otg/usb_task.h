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


//! @brief This file manages the USB task either device/host or both.
//!
//! The USB task selects the correct USB task (usb_device task or usb_host task
//! to be executed depending on the current mode available.
//!
//! This module also contains the general USB interrupt subroutine. This subroutine is used
//! to detect asynchronous USB events.
//!
//! Note:
//!   - The usb_task belongs to the scheduler, the usb_device_task and usb_host do not, they are called
//!     from the general usb_task
//!   - See conf_usb.h file for more details about the configuration of this module

#ifndef _USB_TASK_H_
#define _USB_TASK_H_

#include <otg_user_task.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

/// Definitions of B-Device states
#define B_IDLE              0
//#define B_SRP_INIT        1
#define B_PERIPHERAL        2
//#define B_WAIT_ACON       3
#define B_HOST              4
#define B_END_HNP_SUSPEND   5

// OTG B-Device SRP protocole specific states or events

//!  Is the current session a result of a SRP ?
//#define Start_session_with_srp()          (otg_device_sessions |= 0x01)
#define End_session_with_srp()            (otg_device_sessions &= ~0x01)
#define Is_session_started_with_srp()     (((otg_device_sessions&0x01) != 0) ? TRUE : FALSE)

//! Has a SRP been sent, and waiting for an answer
//#define Srp_sent_and_waiting_answer()     (otg_device_sessions |= 0x02)
//#define Ack_srp_sent_and_answer()         (otg_device_sessions &= ~0x02)
#define Is_srp_sent_and_waiting_answer()  (((otg_device_sessions&0x02) != 0) ? TRUE : FALSE)

//! Is the Tb_Srp counter enabled ? Cleared by timer if Tb_Srp_Fail elapsed
//! Tb_Srp_Fail must be between 5 and 6 sec. With an interrupt routine executed each 2ms, its value becomes 2500 (used:5.2sec)
// 0x0A28 = 2600, 2600x2 = 5200 => 5s
//#define TB_SRP_FAIL_MIN       0x0A28
//extern  U16  otg_tb_srp_cpt;
//#define Init_tb_srp_counter()             (otg_tb_srp_cpt = 0)
//#define Is_tb_srp_counter_overflow()      ((otg_tb_srp_cpt > TB_SRP_FAIL_MIN) ? TRUE : FALSE)

// usb_task USB task entry point module

//! usb_software_evts USB software Events Management
//! Macros to manage USB events detected under interrupt
#define Usb_send_event(x)               g_usb_event |= (1<<x)
#define Usb_ack_event(x)                g_usb_event &= ~(1<<x)
#define Usb_clear_all_event()           g_usb_event = 0
#define Is_usb_event(x)                 ((g_usb_event & (1<<x)) ? TRUE : FALSE)
#define Is_not_usb_event(x)             ((g_usb_event & (1<<x)) ? FALSE: TRUE)

#define Is_host_emergency_exit()        (Is_usb_event(EVT_HOST_DISCONNECTION) || Is_usb_event(EVT_USB_DEVICE_FUNCTION))
//#define Is_usb_device()                 ((g_usb_mode==USB_MODE_DEVICE) ? TRUE : FALSE)
//#define Is_usb_host()                   ((g_usb_mode==USB_MODE_HOST)   ? TRUE : FALSE)

#define Otg_send_event(x)               g_otg_event |= (1<<x)
//#define Otg_ack_event(x)                g_otg_event &= ~(1<<x)
//#define Otg_clear_all_event()           g_otg_event = 0
//#define Is_otg_event(x)                 ((g_otg_event & (1<<x)) ? TRUE : FALSE)
//#define Is_not_otg_event(x)             ((g_otg_event & (1<<x)) ? FALSE: TRUE)

#define EVT_USB_POWERED               1         // USB plugged
#define EVT_USB_UNPOWERED             2         // USB un-plugged
#define EVT_USB_DEVICE_FUNCTION       3         // USB in device
#define EVT_USB_HOST_FUNCTION         4         // USB in host
#define EVT_USB_SUSPEND               5         // USB suspend
#define EVT_USB_WAKE_UP               6         // USB wake up
#define EVT_USB_RESUME                7         // USB resume
#define EVT_USB_RESET                 8         // USB reset
#define EVT_HOST_SOF                  9         // Host start of frame sent
#define EVT_HOST_HWUP                 10        // Host wakeup detected
#define EVT_HOST_DISCONNECTION        11        // The target device is disconnected
#define EVT_HOST_REMOTE_WAKEUP        12        // Remote WakeUp has been received (Host)


#define EVT_OTG_HNP_ERROR             1        // The HNP has failed (A has not responded)
#define EVT_OTG_HNP_SUCCESS           2        // The HNP has succedeed
//#define EVT_OTG_SRP_RECEIVED          3        // A-Device received a SRP
#define EVT_OTG_DEV_UNSUPPORTED       4        // An error occured while the device was enumerated
//#define EVT_OTG_DEVICE_CONNECTED      5        // B-Device has been configured



#define   BDEV_HNP_NB_RETRY     3       // number of times that a B-Device retries a HNP with a failure as a result
                                        // after "n" failures, the "b_hnp_enable" feature is cancelled

//!
//! @brief Definitions of OTG user requests (user software requests)
//!
#define USER_RQST_SRP           0x01
#define USER_RQST_SUSPEND       0x02
#define USER_RQST_VBUS          0x04    // Vbus TOGGLE
#define USER_RQST_HNP           0x08
//#define USER_RQST_RESUME        0x10
//#define USER_RQST_RESET         0x20
#define USER_RQST_DISCONNECT    0x40

// Ask for the B-PERIPH to generate a SRP
//#define Is_user_requested_srp()       (((otg_user_request&USER_RQST_SRP) != 0) ? TRUE : FALSE)
#define Set_user_request_srp()        otg_user_request |= USER_RQST_SRP
//#define Ack_user_request_srp()        otg_user_request &= ~USER_RQST_SRP

// Ask for the A-HOST to enter suspend
#define Is_user_requested_suspend()   (((otg_user_request&USER_RQST_SUSPEND) != 0) ? TRUE : FALSE)
#define Set_user_request_suspend()    otg_user_request |= USER_RQST_SUSPEND
#define Ack_user_request_suspend()    otg_user_request &= ~USER_RQST_SUSPEND

// Ask for the A-HOST to toggle Vbus
#define Is_user_requested_vbus()      (((otg_user_request&USER_RQST_VBUS) != 0) ? TRUE : FALSE)
#define Set_user_request_vbus()       otg_user_request |= USER_RQST_VBUS
#define Ack_user_request_vbus()       otg_user_request &= ~USER_RQST_VBUS

// Ask for an HNP initiation on both devices
#define Is_user_requested_hnp()       (((otg_user_request&USER_RQST_HNP) != 0) ? TRUE : FALSE)
#define Set_user_request_hnp()        otg_user_request |= USER_RQST_HNP
#define Ack_user_request_hnp()        otg_user_request &= ~USER_RQST_HNP

// Ask for the B-PERIPH or the A-PERIPH to disconnect from the bus
#define Is_user_requested_disc()      (((otg_user_request&USER_RQST_DISCONNECT) != 0) ? TRUE : FALSE)
#define Set_user_request_disc()       otg_user_request |= USER_RQST_DISCONNECT
//#define Ack_user_request_disc()       otg_user_request &= ~USER_RQST_DISCONNECT


#define Clear_all_user_request()      otg_user_request = 0



//! @brief  OTG Messaging definitions
//!         "No Silent Failure" rule makes any OTG compliant device handle messaging functions
//!         Differents means are supported : LCD display, LEDs, etc.

//#define   MSG_DISPLAY_NODELAY     0xFFFF
//#define   OTG_TEMPO_1SEC          0x01F4  // 500 (x2ms)
#define   OTG_TEMPO_2SEC          0x03E8  // 1000
#define   OTG_TEMPO_3SEC          0x05DC  // 1500
#define   OTG_TEMPO_4SEC          0x07D0  // 2000
//#define   OTR_TEMPO_5SEC          0x09C4  // 2500

// These messages ID numbers must be used with the OTG messaging functions
//#define   OTGMSG_SRP_STARTED      1      // Event
#define   OTGMSG_SRP_A_NO_RESP    2      // FAILURE msg
//#define   OTGMSG_A_RESPONDED      3      // Event
//#define   OTGMSG_CONNECTED_TO_A   4      // Event
#define   OTGMSG_UNSUPPORTED      5      // FAILURE msg
#define   OTGMSG_UNSUPPORTED_HUB  6      // FAILURE msg
#define   OTGMSG_SRP_RECEIVED     7      // Event
#define   OTGMSG_DEVICE_NO_RESP   8      // FAILURE msg
#define   OTGMSG_VBUS_SURCHARGE   9      // Event

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

extern U8 usb_configuration_nb;
extern U8 otg_device_sessions;
extern U8 b_uut_device_state;
extern volatile U8 otg_features_supported;
extern U8 otg_user_request;
extern U8 g_usb_mode;
extern volatile U8 private_sof_counter;
extern volatile U16 g_usb_event;
extern U16 otg_msg_event_delay;
extern U16 otg_msg_failure_delay;

extern void Otg_messaging_init(void);
extern void Otg_output_failure_msg(U8);
extern void Otg_output_failure_clear(void);
//! Otg_print_new_event_message(str,tm) displays the "str" message on the EVENT line during the "tm" delay (x2ms)  
#define   Otg_print_new_event_message(str,tm)     (Otg_output_event_msg(str), otg_msg_event_delay = tm)
#define   Otg_clear_event_message()               Otg_output_event_clear()
#define   Get_event_msg_delay()                   (otg_msg_event_delay)
#define   Decrement_event_msg_delay()             (otg_msg_event_delay--)
//! Otg_print_new_failure_message(str,tm) displays the "str" message on the FAILURE line during the "tm" delay (x2ms)
#define   Otg_print_new_failure_message(str,tm)   (Otg_output_failure_msg(str), otg_msg_failure_delay = tm)
#define   Otg_clear_failure_message()             Otg_output_failure_clear()
#define   Get_failure_msg_delay()                 (otg_msg_failure_delay)
#define   Decrement_failure_msg_delay()           (otg_msg_failure_delay--)



//-----------------------------------------------------------------------------
//      Exported functions
//-----------------------------------------------------------------------------
extern void usb_task_init(void);
extern void usb_task(void);
extern void usb_general_interrupt(void);
extern void otg_timer_interrupt(void);
extern void Otg_message_task_init(void);
extern void Otg_message_task(void);
extern void otg_not_supported_device(void);

#endif /* _USB_TASK_H_ */

