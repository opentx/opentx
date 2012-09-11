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
//      Headers
//------------------------------------------------------------------------------

#include <memories/Media.h>
#include "main.h"
#include <conf_usb.h>
#include <utility/trace.h>
#include <usb/common/core/USBConfigurationDescriptor.h>
#include <usb/device/core/USBD.h>
#include <usb/device/massstorage/MSDDriver.h>
#include <usb/device/massstorage/MSDLun.h>
#include <usb/device/core/USBDCallbacks.h>
#include <usb/common/core/USBFeatureRequest.h>
#include "otg_user_task.h"
#include "usb_task.h"
#include "usb/otg/usb_drv.h"
#include "usb/otg/usb_host_task.h"

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------



extern volatile unsigned int timer_set_counter;
#define Timer16_select(...)
#define Timer16_set_counter(value)      timer_set_counter = value
#define Timer16_get_counter_low()       (unsigned char)timer_set_counter


/// Returns the OTG features sent by the host by a SetFeature
/// Allow also to clear the features (only on a bus Reset or Session End)
#define Is_host_requested_hnp()           (((otg_features_supported&USBFeatureRequest_OTG_B_HNP_ENABLE) != 0) ? TRUE : FALSE)
#define Clear_otg_features_from_host()    (otg_features_supported = 0)

#define Set_otg_custom_timer(ep)   // STUB: todo

#define USB_MODE_UNDEFINED            0x00
#define USB_MODE_HOST                 0x01
#define USB_MODE_DEVICE               0x02

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

U8  otg_device_sessions;
U8  b_uut_device_state;
U16  otg_tb_srp_cpt;

/// usb_connected is used to store USB events detected upon
/// USB general interrupt subroutine
/// Its value is managed by the following macros (See usb_task.h file)
/// Usb_send_event(x)
/// Usb_ack_event(x)
/// Usb_clear_all_event()
/// Is_usb_event(x)
/// Is_not_usb_event(x)
volatile U16 g_usb_event=0;


///
/// Private : (U8) private_sof_counter
/// Incremented  by host SOF interrupt subroutime
/// This counter is used to detect timeout in host requests.
/// It must not be modified by the user application tasks.
////
volatile U8 private_sof_counter=0;
extern volatile S_pipe_int   it_pipe_str[MAX_EP_NB];

//------------------------------------------------------------------------------
//        Local Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      Internal Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes the USB process
/// Depending on the mode supported (HOST/DEVICE/DUAL_ROLE) the function calls
/// the coresponding USB mode initialization function
//------------------------------------------------------------------------------
void usb_task_init(void)
{
    // ---- REDUCED HOST ONLY USB MODE -----------------------------------------
    //jcbUsb_force_host_mode();
    usb_host_task_init();
}


//------------------------------------------------------------------------------
/// Entry point of the USB mamnagement
/// Depending on the USB mode supported (HOST/DEVICE/DUAL_ROLE) the function
/// calls the coresponding USB management function
//------------------------------------------------------------------------------
void usb_task(void)
{
    // ---- REDUCED HOST ONLY USB MODE -----------------------------------------
    usb_host_task();
}



//------------------------------------------------------------------------------
/// USB interrupt subroutine
///
/// This function is called each time a USB interrupt occurs.
/// The following USB DEVICE events are taken in charge:
/// - VBus On / Off
/// - Start Of Frame
/// - Suspend
/// - Wake-Up
/// - Resume
/// - Reset
/// - Start of frame
///
/// The following USB HOST events are taken in charge:
/// - Device connection
/// - Device Disconnection
/// - Start Of Frame
/// - ID pin change
/// - SOF (or Keep alive in low speed) sent
/// - Wake up on USB line detected
///
/// The following USB HOST events are taken in charge:
/// - HNP success (Role Exchange)
/// - HNP failure (HNP Error)
///
/// For each event, the user can launch an action by completing
/// the associate define (See conf_usb.h file to add action upon events)
///
/// Note: Only interrupts events that are enabled are processed
//------------------------------------------------------------------------------
void usb_general_interrupt(void)
{
    //TRACE_DEBUG("usb_general_interrupt\n\r");
    U8 i;
    U8 save_pipe_nb;

    // ---------- HOST events management -----------------------------------
    // ---------------------------------------------------------------------
    // -----------------------------------------------------------------------
    /// Host
    // -----------------------------------------------------------------------
    // - The device has been disconnected
    if(Is_device_disconnection() && Is_host_device_disconnection_interrupt_enabled()) {
        TRACE_DEBUG("device disconnect\n\r");
        host_disable_all_pipe();
        Host_ack_device_disconnection();
        device_state=DEVICE_DISCONNECTED;
        Usb_send_event(EVT_HOST_DISCONNECTION);
        TRACE_INFO("Device disconnected\n\r");
        Host_device_disconnection_action();
    }

    // -----------------------------------------------------------------------
    /// Host
    // -----------------------------------------------------------------------
    // - Device connection
    if(Is_device_connection() && Is_host_device_connection_interrupt_enabled()) {
        TRACE_DEBUG("device connect\n\r");
        Host_ack_device_connection();
        host_disable_all_pipe();
        Host_device_connection_action();
    }

    // -----------------------------------------------------------------------
    /// Host
    // -----------------------------------------------------------------------
    // - Host Start of frame has been sent
    if (Is_host_sof() && Is_host_sof_interrupt_enabled()) {
        //TRACE_DEBUG_WP("_");  // host
        Host_ack_sof();
        Usb_send_event(EVT_HOST_SOF);
        private_sof_counter++;

        // delay timeout management for interrupt tranfer mode in host mode
#if ((USB_HOST_PIPE_INTERRUPT_TRANSFER==ENABLE) && (TIMEOUT_DELAY_ENABLE==ENABLE))
        if (private_sof_counter>=250) {  // Count 1/4 sec
            private_sof_counter=0;
            for(i=0;i<MAX_EP_NB;i++) {
                if(it_pipe_str[i].enable==ENABLE) {
                    save_pipe_nb=Host_get_selected_pipe();
                    Host_select_pipe(i);
                    if((++it_pipe_str[i].timeout>TIMEOUT_DELAY) && (Host_get_pipe_type()!=TYPE_INTERRUPT)) {
                        it_pipe_str[i].enable=DISABLE;
                        it_pipe_str[i].status=PIPE_DELAY_TIMEOUT;
                        Host_stop_pipe_interrupt(i);
                        if (is_any_interrupt_pipe_active()==FALSE) {   // If no more transfer is armed
                            if (g_sav_int_sof_enable==FALSE) {
                                Host_disable_sof_interrupt();
                            }
                        }
                        it_pipe_str[i].handle(PIPE_DELAY_TIMEOUT,it_pipe_str[i].nb_byte_processed);
                    }
                    Host_select_pipe(save_pipe_nb);
                }
            }
        }
#endif  // (USB_HOST_PIPE_INTERRUPT_TRANSFER==ENABLE) && (TIMEOUT_DELAY_ENABLE==ENABLE))
        Host_sof_action();
    }

    // -----------------------------------------------------------------------
    /// Host
    // -----------------------------------------------------------------------
    // - Host Wake-up has been received
    if (Is_host_hwup() && Is_host_hwup_interrupt_enabled()) {
        TRACE_DEBUG("Host wake up\n\r");
        Host_disable_hwup_interrupt();  // Wake up interrupt should be disable host is now wake up !
        Host_disable_remote_wakeup_interrupt();
        // CAUTION HWUP can be cleared only when USB clock is active (not frozen)!
        Usb_unfreeze_clock();           // Enable clock on USB interface
        Host_enable_sof();              // start sending SOF
        Host_ack_hwup();                // Clear HWUP interrupt flag
        Host_ack_remote_wakeup();
        Usb_send_event(EVT_HOST_HWUP);  // Send software event
        Usb_send_event(EVT_HOST_REMOTE_WAKEUP);
        Host_hwup_action();             // Map custom action
        Host_send_resume();
    }

    // -----------------------------------------------------------------------
    /// Host
    // -----------------------------------------------------------------------
    // Remote Wake Up has been received
    if (Is_host_remote_wakeup_interrupt_enabled() && Is_host_remote_wakeup()) {
        TRACE_DEBUG("Remote wake up\n\r");
        Host_disable_remote_wakeup_interrupt();
        Host_disable_hwup_interrupt();
        Host_ack_remote_wakeup();
        Host_ack_hwup();                // Clear HWUP interrupt flag
        Usb_unfreeze_clock();           // Enable clock on USB interface
        Host_enable_sof();     // start sending SOF
        Usb_send_event(EVT_HOST_REMOTE_WAKEUP);
        Usb_send_event(EVT_HOST_HWUP);  // Send software event
        Host_send_resume();
    }
}

//------------------------------------------------------------------------------
/// Message "OTG not supported"
//------------------------------------------------------------------------------
void otg_not_supported_device(void)
{
    TRACE_DEBUG("otg_not_supported_device\n\r");
    Otg_send_event(EVT_OTG_DEV_UNSUPPORTED);
}


