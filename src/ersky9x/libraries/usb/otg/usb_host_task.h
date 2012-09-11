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
#ifndef _USB_HOST_TASK_H_
#define _USB_HOST_TASK_H_

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------
#include <conf_usb.h>
#include <usb/otg/usb_host_enum.h>

#include "main.h"

//! @brief Selects the Reset Length (x11ms)
//! This value is the number of consecutives Reset sent by the Host
#define OTG_RESET_LENGTH                        1

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------
typedef struct
{
   bit enable;
   U16 nb_byte_to_process;
   U16 nb_byte_processed;
   U16 nb_byte_on_going;
   U8 *ptr_buf;
   void(*handle)(U8 status, U16 nb_byte);
   U8 status;
   U8 timeout;
   U16 nak_timeout;
} S_pipe_int;


//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------
#define PIPE_GOOD             0
//#define PIPE_DATA_TOGGLE   0x01
//#define PIPE_DATA_PID      0x02
//#define PIPE_PID           0x04
//#define PIPE_TIMEOUT       0x08
//#define PIPE_CRC16         0x10
#define PIPE_STALL         0x20
#define PIPE_NAK_TIMEOUT   0x40
#define PIPE_DELAY_TIMEOUT 0x80

// usb_host_task USB host task module
// Returns true when device connected and correctly enumerated.
// The host high level application should tests this before performing any applicative requests
// to the device connected
#define Is_host_ready()        ((device_state==DEVICE_READY)   ? TRUE : FALSE)

/// Check if host controller is in suspend mode
#define Is_host_suspended()    (((device_state==DEVICE_WAIT_RESUME) ||(device_state==DEVICE_SUSPENDED))  ? TRUE : FALSE)

/// Check if host controller is not suspend mode
#define Is_host_not_suspended()    (((device_state==DEVICE_WAIT_RESUME) ||(device_state==DEVICE_SUSPENDED))  ? FALSE : TRUE)

/// This function should be called to make the host controller enter USB suspend mode
#define Host_request_suspend()     (device_state=DEVICE_SUSPENDED)

/// This function should be called to request the host controller to resume the USB bus
#define Host_request_resume()      (request_resume=TRUE)

/// Private ack for software event
#define Host_ack_request_resume()  (request_resume=FALSE)

/// Private check for resume sequence
#define Is_host_request_resume()   ((request_resume==TRUE)   ? TRUE : FALSE)

/// Returns true when a new device is enumerated
#define Is_new_device_connection_event()   (new_device_connected ? TRUE : FALSE)

/// Returns true when the device disconnects from the host
#define Is_device_disconnection_event()   ((device_state==DEVICE_DISCONNECTED_ACK || device_state==DEVICE_DISCONNECTED) ? TRUE : FALSE)

/// Stop all interrupt attached to a pipe
#define Host_stop_pipe_interrupt(i) {Host_disable_transmit_interrupt();\
                                     Host_disable_receive_interrupt();\
                                     Host_disable_stall_interrupt();\
                                     Host_disable_error_interrupt();\
                                     Host_disable_nak_interrupt();\
                                     Host_reset_pipe(i); }

/// device_state_value Host controller states
/// Defines for device state coding
#define DEVICE_UNATTACHED        0
#define DEVICE_ATTACHED          1
#define DEVICE_POWERED           2
#define DEVICE_DEFAULT           3
#define DEVICE_ADDRESSED         4
#define DEVICE_CONFIGURED        5
#define DEVICE_READY             6
#define DEVICE_ERROR             7
#define DEVICE_SUSPENDED         8
#define DEVICE_WAIT_RESUME       9
#define DEVICE_DISCONNECTED      10
#define DEVICE_DISCONNECTED_ACK  11
#define A_PERIPHERAL             12    // used for A-Device in OTG mode
#define A_INIT_HNP               13
#define A_SUSPEND                14
#define A_END_HNP_WAIT_VFALL     15
#define A_TEMPO_VBUS_DISCHARGE   16


#define Host_set_device_supported()   (device_status |=  0x01)
#define Host_clear_device_supported() (device_status &= ~0x01)

#define Host_set_device_ready()       (device_status |=  0x02)
#define Host_clear_device_ready()     (device_status &= ~0x02)

#define Host_set_configured()         (device_status |=  0x04)
#define Host_clear_configured()       (device_status &= ~0x04)
#define Is_host_configured()          (device_status &   0x04)




/// Has a SRP been received, and waiting for a connect of B-Device (Vbus delivered)
#define Srp_received_and_waiting_connect()     (otg_a_device_srp |= 0x01)
#define Ack_srp_received_and_connect()         (otg_a_device_srp &= ~0x01)
#define Is_srp_received_and_waiting_connect()  (((otg_a_device_srp&0x01) != 0) ? TRUE : FALSE)
/// Is the current session has been started with SRP
#define Host_session_started_srp()             (otg_a_device_srp |= 0x02)
#define Host_end_session_started_srp()         (otg_a_device_srp &= ~0x02)
#define Is_host_session_started_srp()          (((otg_a_device_srp&0x02) != 0) ? TRUE : FALSE)

/// Is the current peripheral is an OTG device ?
#define Peripheral_is_not_otg_device()         (otg_device_connected = 0x00)
#define Peripheral_is_otg_device()             (otg_device_connected = 0x11)
#define Is_peripheral_otg_device()             ((otg_device_connected != 0) ? TRUE : FALSE)

/// Check if counter of Vbus delivery time after SRP is elapsed
// 0xDAC = 3500, 3500x2 = 7000 => 7s
#define T_VBUS_DELIV_AFTER_SRP        0xDAC    // minimum time (x2ms) with Vbus ON once a SRP has been received (5s<t<30s)
#define Init_ta_srp_counter()                  (otg_ta_srp_wait_connect = 0)
#define Is_ta_srp_counter_overflow()           ((otg_ta_srp_wait_connect > T_VBUS_DELIV_AFTER_SRP) ? TRUE : FALSE)

/// Check if counter of A-Suspend delay is elapsed
/// Max. delay for a B-DEVICE to disconnect once the A-HOST has set suspend mode
// Min 200 ms
// 0x96 = 150 ,150x2 = 300 => 300 ms
#define TA_AIDL_BDIS                  0x96    // minimum time (x2ms) in A_suspend state before stop Vbus (must be > 200ms)
                                               // if value = 0, this is an infinite delay
#define Init_ta_aidl_bdis_counter()            (otg_ta_aidl_bdis_tmr = TA_AIDL_BDIS)
#define Is_ta_aidl_bdis_counter_overflow()     ((otg_ta_aidl_bdis_tmr == 0x0001) ? TRUE : FALSE)

/// Check if counter otg_timeout_bdev_respond is elapsed
// 0xD00 = 3328, 3328x2 = 6656 => 6s
#define TM_OUT_MAX_BDEV_RESPONSE      0xD00    // maximum delay for the A-Device waiting for a response from B-Device once attached
#define Init_timeout_bdev_response()           (otg_timeout_bdev_respond = TM_OUT_MAX_BDEV_RESPONSE)
#define Is_timeout_bdev_response_overflow()    ((otg_timeout_bdev_respond == 0) ? TRUE : FALSE)

/// Check if counter otg_ta_vbus_rise is elapsed
// 0x28 = 40, 40x2 = 80 => 80ms
#define TA_VBUS_RISE                  0x28     // maximum delay for Vbus to reach the Va_vbus_valid threshold after being requested (must be < 100ms)
#define Init_ta_vbus_rise_counter()            (otg_ta_vbus_rise = TA_VBUS_RISE)
#define Is_ta_vbus_rise_counter_overflow()     ((otg_ta_vbus_rise == 0) ? TRUE : FALSE)

/// Check if counter otg_ta_vbus_fall is elapsed
// 50x2 = 100 => 100ms
#define TA_VBUS_FALL                  50
#define Init_ta_vbus_fall_counter()            (otg_end_hnp_vbus_delay = TA_VBUS_FALL)
#define Is_ta_vbus_fall_counter_overflow()     ((otg_end_hnp_vbus_delay == 0) ? TRUE : FALSE)


//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------
extern U8 g_sav_int_sof_enable;
extern U8 device_state;
extern U8 request_resume;
extern U8 new_device_connected;

// OTG Defines, Variables and Macros
// See "usb_host_task.c" file for description of variables role and use
extern U16 otg_ta_srp_wait_connect;
extern U16 otg_ta_aidl_bdis_tmr;
extern U8 otg_ta_vbus_rise;
extern U16 otg_timeout_bdev_respond;
extern U16 otg_end_hnp_vbus_delay;

extern U8 otg_a_device_srp;
extern U8 otg_device_connected;
extern S_usb_setup_data usb_request;
extern U8 device_status;
extern U8 data_stage[SIZEOF_DATA_STAGE];

//-----------------------------------------------------------------------------
//      Exported functions
//-----------------------------------------------------------------------------
extern void usb_host_task_init(void);
extern void usb_host_task(void);
extern U8 host_send_data(U8 pipe, U16 nb_data, U8 *buf);
extern U8 host_get_data(U8 pipe, U16 *nb_data, U8 *buf);
extern void reset_it_pipe_str(void);
extern U8 is_any_interrupt_pipe_active(void);
extern U8 host_get_data_interrupt(U8 pipe, U16 nb_data, U8 *buf, void  (*handle)(U8 status, U16 nb_byte));
extern U8 host_send_data_interrupt(U8 pipe, U16 nb_data, U8 *buf, void  (*handle)(U8 status, U16 nb_byte));
extern void usb_pipe_interrupt(void);

#endif // _USB_HOST_TASK_H_

