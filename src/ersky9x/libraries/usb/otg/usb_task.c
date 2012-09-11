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
#include <msd/conf_usb.h>
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

#if ((USB_HOST_FEATURE == ENABLE))
   #include "usb/otg/usb_host_task.h"
#endif

#if ((USB_DEVICE_FEATURE == ENABLE))
#include <memories/Media.h>
#include <usb/common/core/USBConfigurationDescriptor.h>
#include <usb/device/core/USBD.h>
#include <usb/device/massstorage/MSDDriver.h>
#include <usb/device/massstorage/MSDLun.h>
#include <usb/device/core/USBDCallbacks.h>
#include <usb/device/massstorage/MSD.h>
#endif

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


#if (USB_DEVICE_FEATURE == ENABLE)
///
/// Public : (bit) usb_connected
/// usb_connected is set to TRUE when VBUS has been detected
/// usb_connected is set to FALSE otherwise
/// Used with USB_DEVICE_FEATURE == ENABLE only
////
bit   usb_connected;

///
/// Public : (U8) usb_configuration_nb
/// Store the number of the USB configuration used by the USB device
/// when its value is different from zero, it means the device mode is enumerated
/// Used with USB_DEVICE_FEATURE == ENABLE only
////
U8    usb_configuration_nb;

///
/// Public : (U8) remote_wakeup_feature
/// Store a host request for remote wake up (set feature received)
////
U8 remote_wakeup_feature;
#endif


#if (USB_HOST_FEATURE == ENABLE)
///
/// Private : (U8) private_sof_counter
/// Incremented  by host SOF interrupt subroutime
/// This counter is used to detect timeout in host requests.
/// It must not be modified by the user application tasks.
////
volatile U8 private_sof_counter=0;

extern volatile S_pipe_int   it_pipe_str[MAX_EP_NB];

#endif

#if ((USB_DEVICE_FEATURE == ENABLE)&& (USB_HOST_FEATURE == ENABLE))
///
/// Public : (U8) g_usb_mode
/// Used in dual role application (both device/host) to store
/// the current mode the usb controller is operating
////
   U8 g_usb_mode=USB_MODE_UNDEFINED;
   U8 g_old_usb_mode;
#endif


#if (USB_OTG_FEATURE == ENABLE)
  ///
  /// Public : (U8) otg_features_supported;
  ///   -> A-Device side : indicates if the B-Device supports HNP and SRP (this is the bmAttributes field of OTG Decriptor)
  ///   -> B-Device side : indicates if the A-Device has enabled the "a_hnp_support" and "b_hnp_enable" features
  volatile U8 otg_features_supported;
  
  /// Public : (U8) otg_user_request;
  /// Store the last request of the user (see usb_device_task.h)
  U8 otg_user_request;
     
  /// Public : (U16) otg_msg_event_delay;
  /// Contains the current display duration of the OTG event message
  U16 otg_msg_event_delay;
  
  /// Public : (U16) otg_msg_failure_delay;
  /// Contains the current display duration of the OTG failure message
  U16 otg_msg_failure_delay;
 
  /// Public : (U16) g_otg_event;
  /// Contains several bits corresponding to differents OTG events (similar to g_usb_event)
  volatile U16 g_otg_event;
   
  /// Public : (U8) otg_device_nb_hnp_retry;
  /// Counts the number of times a HNP fails, before aborting the operations
  U8 otg_device_nb_hnp_retry;

  U8 id_changed_to_host_event;
  
  volatile U8 otg_last_sof_received;     // last SOF received in SOF interrupt
  volatile U8 otg_last_sof_stored;       // last SOF value stored in OTG Timer interrupt
  volatile U8 reset_received;            // indicates if a reset has been received from B-Host after a HNP (used in A-Periph mode)
  
  /// @brief VBUS Overload management
  ///
  /// Nothing to do ! If the condition is not defined in the board driver file
  /// (i.e. if the board does not support Vbus overcurrent detection),
  /// the macro is defined to false for firmware compatibility
#define   Is_vbus_overcurrent()       (FALSE)
   
  /// Private function prototypes
  /// Tasks for OTG Messaging features
  void Otg_message_task_init(void);
  void Otg_message_task(void);
#endif


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

// ---- DUAL ROLE DEVICE USB MODE ---------------------------------------------
#if (USB_OTG_FEATURE == ENABLE)
    Otg_message_task_init();       // OTG program needs to display event messages to the user
    b_uut_device_state = B_IDLE;   // init state machines variables
#if (USB_HOST_FEATURE == ENABLE)
    device_state = DEVICE_UNATTACHED;
#endif
    otg_device_nb_hnp_retry = BDEV_HNP_NB_RETRY;
    Clear_all_user_request();
#endif

#if (((USB_DEVICE_FEATURE == ENABLE) && (USB_HOST_FEATURE == ENABLE)) || (USB_OTG_FEATURE == ENABLE))
#if (USB_OTG_FEATURE == ENABLE)
    Usb_enable_uid_pin();
#endif

    // delay=PORTA;
    g_usb_mode=USB_MODE_UNDEFINED;

    if(Is_usb_id_device())
    {
        g_usb_mode=USB_MODE_DEVICE;
        //usb_device_task_init();
        MSDDriver_Initialize(luns, numMedias);

#if (USB_OTG_FEATURE == ENABLE)
        id_changed_to_host_event = DISABLE;
#endif
    }
   else
   {
        Usb_send_event(EVT_USB_HOST_FUNCTION);
        g_usb_mode=USB_MODE_HOST;
        Usb_ack_id_transition(); // REQUIRED !!! Startup with ID=0, Ack ID pin transistion (default hwd start up is device mode)
        Usb_enable_id_interrupt();
        usb_host_task_init();
#if (USB_OTG_FEATURE == ENABLE)
        id_changed_to_host_event = ENABLE;
#endif
    }
#if ((USB_DEVICE_FEATURE == ENABLE)&& (USB_HOST_FEATURE == ENABLE))
    g_old_usb_mode = g_usb_mode;   // Store current usb mode, for mode change detection
#endif
    // -------------------------------------------------------------------------

    // ---- DEVICE ONLY USB MODE -----------------------------------------------
#elif ((USB_DEVICE_FEATURE == ENABLE)&& (USB_HOST_FEATURE == DISABLE))
    //jcbUsb_force_device_mode();
    //usb_device_task_init();
    MSDDriver_Initialize(luns, numMedias);
    // -------------------------------------------------------------------------

    // ---- REDUCED HOST ONLY USB MODE -----------------------------------------
#elif ((USB_DEVICE_FEATURE == DISABLE)&& (USB_HOST_FEATURE == ENABLE))
    //jcbUsb_force_host_mode();
    usb_host_task_init();
#elif ((USB_DEVICE_FEATURE == DISABLE)&& (USB_HOST_FEATURE == DISABLE))
#error  at least one of USB_DEVICE_FEATURE or USB_HOST_FEATURE should be enabled
#endif
    // -------------------------------------------------------------------------

}


//------------------------------------------------------------------------------
/// Entry point of the USB mamnagement
/// Depending on the USB mode supported (HOST/DEVICE/DUAL_ROLE) the function
/// calls the coresponding USB management function
//------------------------------------------------------------------------------
void usb_task(void)
{
    // ---- DUAL ROLE DEVICE USB MODE ---------------------------------------------
#if ((USB_DEVICE_FEATURE == ENABLE) && (USB_HOST_FEATURE == ENABLE))
    if(Is_usb_id_device()) {
        g_usb_mode = USB_MODE_DEVICE;
    }
    else {
        g_usb_mode = USB_MODE_HOST;
    }
    g_old_usb_mode = g_usb_mode;   // Store current usb mode, for mode change detection
    // Depending on current usb mode, launch the correct usb task (device or host)

#if (USB_OTG_FEATURE == ENABLE)  
    // Configure OTG timers
    Set_otg_custom_timer(VBUSRISE_70MS);
    Set_otg_custom_timer(VBUSPULSE_40MS);
    Set_otg_custom_timer(VFALLTMOUT_131MS);
    Set_otg_custom_timer(SRPMINDET_100US);
#endif
   
    switch(g_usb_mode) {
        case USB_MODE_DEVICE:
            //usb_device_task();
            MSDDriver_StateMachine();
            break;

        case USB_MODE_HOST:
          if( pOTGDescriptor.bOTGADevSRPReaction == VBUS_PULSE) {
            Usb_select_vbus_srp_method();
          }
          else {
            Usb_select_data_srp_method();
          }
            usb_host_task();
            // Handle Vbus overcurrent error (auto-disabled if not supported or not defined in board driver file)
#if (USB_OTG_FEATURE == ENABLE)
            if (Is_vbus_overcurrent()) {
                Otg_print_new_event_message(OTGMSG_VBUS_SURCHARGE,OTG_TEMPO_3SEC);
            }
#endif
            break;

        case USB_MODE_UNDEFINED:  // No break !
            default:
            break;
    }
    // -------------------------------------------------------------------------
    // ---- DEVICE ONLY USB MODE -----------------------------------------------
#elif ((USB_DEVICE_FEATURE == ENABLE)&& (USB_HOST_FEATURE == DISABLE))
    //usb_device_task();
    MSDDriver_StateMachine();
    // -------------------------------------------------------------------------

    // ---- REDUCED HOST ONLY USB MODE -----------------------------------------
#elif ((USB_DEVICE_FEATURE == DISABLE)&& (USB_HOST_FEATURE == ENABLE))
    usb_host_task();
    // -------------------------------------------------------------------------

    /// ---- ERROR, NO MODE ENABLE ---------------------------------------------
#elif ((USB_DEVICE_FEATURE == DISABLE)&& (USB_HOST_FEATURE == DISABLE))
#error  at least one of USB_DEVICE_FEATURE or USB_HOST_FEATURE should be enabled
#error  otherwise the usb task has nothing to do ...
#endif
    // -------------------------------------------------------------------------

#if (USB_OTG_FEATURE == ENABLE)
    Otg_message_task();
#endif
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
    U8 i;
    U8 save_pipe_nb;

    // ---------- DEVICE events management -----------------------------------
    // -----------------------------------------------------------------------
#if ((USB_DEVICE_FEATURE == ENABLE) || (USB_OTG_FEATURE == ENABLE))

    //- VBUS state detection
    // -----------------------------------------------------------------------
    /// Arbitrer
    // -----------------------------------------------------------------------
    if (Is_usb_vbus_transition() && Is_usb_vbus_interrupt_enabled() 
        && Is_usb_id_device()) {
        Usb_ack_vbus_transition();

        if (Is_usb_vbus_high()) {
            usb_connected = TRUE;
            Usb_vbus_on_action();
            Usb_send_event(EVT_USB_POWERED);
            //jcb    Usb_enable_reset_interrupt();
            //usb_start_device();
            USBD_Connect();//Usb_attach();
        }
        else {
            TRACE_DEBUG("VBUS low\n\r");
            USBD_Disconnect();
#if  (USB_OTG_FEATURE == ENABLE)
            Usb_device_stop_hnp();
            TRACE_DEBUG("Usb_select_device4\n\r");
            Usb_select_device();
            Clear_all_user_request();
#endif
            Usb_vbus_off_action();
            usb_connected = FALSE;
            usb_configuration_nb = 0;
            Usb_send_event(EVT_USB_UNPOWERED);
        }

    }

    // -----------------------------------------------------------------------
    /// Device
    // -----------------------------------------------------------------------
    // - Device start of frame received
    if (Is_usb_sof() && Is_sof_interrupt_enabled()) {
        //  TRACE_DEBUG_WP("F");   // device
        Usb_ack_sof();
        Usb_sof_action();
#if (USB_OTG_FEATURE == ENABLE)
        //sof_seen_in_session = TRUE;
        otg_last_sof_received = UDFNUML;  // store last frame number received
#endif
    }

    // -----------------------------------------------------------------------
    /// Device
    // -----------------------------------------------------------------------
    // - Device Suspend event (no more USB activity detected)
    if (Is_usb_suspend()) { //&& Is_suspend_interrupt_enabled()) {
        //TRACE_DEBUG_WP("D\n\r");
#if (USB_OTG_FEATURE == ENABLE)
        // 1st : B-PERIPH mode ?
        if (Is_usb_id_device()) {
            // HNP Handler
            TRACE_DEBUG("HNP Handler\n\r");
            //TRACE_DEBUG("device_state = %d\n\r", device_state);
            //TRACE_DEBUG("b_uut_device_state = %d\n\r", b_uut_device_state);
            if (Is_host_requested_hnp() // "b_hnp_enable" feature received
            && (Is_session_started_with_srp() || Is_user_requested_hnp() {
                if (otg_device_nb_hnp_retry == 0) {
                    otg_features_supported &= ~USBFeatureRequest_OTG_B_HNP_ENABLE;
                }
                else {
                    Ack_user_request_hnp();
                    Usb_ack_hnp_error_interrupt();
                    Usb_ack_role_exchange_interrupt();
                    Usb_enable_role_exchange_interrupt();
                    Usb_enable_hnp_error_interrupt();
                    Usb_device_initiate_hnp();
                    otg_device_nb_hnp_retry--;
                }
            }
            else
            {
                // Remote wake-up handler
                //TRACE_DEBUG("Remote wake-up handler\n\r");
                //TRACE_DEBUG("device_state = %d\n\r", device_state);
                //TRACE_DEBUG("b_uut_device_state = %d\n\r", b_uut_device_state);
                if ((remote_wakeup_feature == ENABLE) && (usb_configuration_nb != 0))
                {
                    //TRACE_DEBUG("enabled\n\r");
                    // After that user can execute "Usb_initiate_remote_wake_up()" to initiate a remote wake-up
                    // Note that the suspend interrupt flag SUSPI must still be set to enable upstream resume
                    // So the SUSPE enable bit must be cleared to avoid redundant interrupt
                    // ****************
                    // Please note also that is Vbus is lost during an upstream resume (Host disconnection),
                    // the RMWKUP bit (used to initiate remote wake up and that is normally cleared by hardware when sent)
                    // remains set after the event, so that a good way to handle this feature is :
                    //            Usb_initiate_remote_wake_up();
                    //            while (Is_usb_pending_remote_wake_up())
                    //            {
                    //              if (Is_usb_vbus_low())
                    //              {
                    //                // Emergency action (reset macro, etc.) if Vbus lost during resuming
                    //                break;
                    //              }
                    //            }
                    //            Usb_ack_remote_wake_up_start();
                    // ****************
                }
                else {
                    //TRACE_DEBUG("disabled: %d %d\n\r", usb_configuration_nb, remote_wakeup_feature);
                    // No remote wake-up supported
                    Usb_send_event(EVT_USB_SUSPEND);
                }
            }
        }
        else
        {
            //TRACE_DEBUG("ici\n\r");
            // A-PERIPH mode (will cause a session end, handled in usb_host_task.c)
            Usb_send_event(EVT_USB_SUSPEND);
            Usb_suspend_action();
            //jcb   Usb_ack_suspend();
        }
#else
        // Remote wake-up handler
        if ((remote_wakeup_feature == ENABLE) && (usb_configuration_nb != 0)) {
            TRACE_DEBUG("Remote wake-up handler\n\r");
            Usb_disable_suspend_interrupt();
            Usb_ack_wake_up();
            Usb_enable_wake_up_interrupt();
            Usb_suspend_action();
            Usb_freeze_clock();
            // After that user can execute "Usb_initiate_remote_wake_up()" to initiate a remote wake-up
            // Note that the suspend interrupt flag SUSPI must still be set to enable upstream resume
            // So the SUSPE enable bit must be cleared to avoid redundant interrupt
            // ****************
            // Please note also that is Vbus is lost during an upstream resume (Host disconnection),
            // the RMWKUP bit (used to initiate remote wake up and that is normally cleared by hardware when sent)
            // remains set after the event, so that a good way to handle this feature is :
            //            Usb_unfreeze_clock();
            //            Usb_initiate_remote_wake_up();
            //            while (Is_usb_pending_remote_wake_up())
            //            {
            //              if (Is_usb_vbus_low())
            //              {
            //                // Emergency action (reset macro, etc.) if Vbus lost during resuming
            //                break;
            //              }
            //            }
            //            Usb_ack_remote_wake_up_start();
            // ****************
        }
        else {
            // No remote wake-up supported
            Usb_send_event(EVT_USB_SUSPEND);
            Usb_suspend_action();
            Usb_ack_suspend();
            Usb_ack_wake_up();                  // clear wake up to detect next event
            Usb_enable_wake_up_interrupt();
            Usb_freeze_clock();
        }
#endif
    }
   
    // -----------------------------------------------------------------------
    /// Device
    // -----------------------------------------------------------------------
    // - Wake up event (USB activity detected): Used to resume
    if (Is_usb_wake_up() && Is_swake_up_interrupt_enabled()) {
        TRACE_DEBUG("W\n\r");
        Usb_unfreeze_clock();
        Usb_send_event(EVT_USB_WAKE_UP);
    }

    // -----------------------------------------------------------------------
    /// Device
    // -----------------------------------------------------------------------
    // - Resume state bus detection
    if (Is_usb_resume() && Is_resume_interrupt_enabled()) {
        TRACE_DEBUG("Resume state bus detect\n\r");
        Usb_send_event(EVT_USB_RESUME);
    }

    // -----------------------------------------------------------------------
    /// Device
    // -----------------------------------------------------------------------
    // - USB bus reset detection
    if (Is_usb_reset()&& Is_reset_interrupt_enabled()) {
        TRACE_DEBUG_WP("B\n\r");
#if (USB_OTG_FEATURE == ENABLE)
        if (Is_usb_id_host()) {
            //TRACE_DEBUG_WP("id_host\n\r");
            dev_configure_endpoint(EP_CONTROL,
            TYPE_CONTROL,
            DIRECTION_OUT,
            SIZE_64,
            ONE_BANK,
            NYET_DISABLED);
        }
        // First initialization is important to be synchronized
        // A reset must first have been received
        if (device_state == A_PERIPHERAL) {
            //TRACE_DEBUG_WP("r A_PERIPHERAL\n\r");
            otg_last_sof_received = UDFNUML;
            otg_last_sof_stored   = UDFNUML;
            Usb_ack_sof();
            Usb_enable_sof_interrupt();
            reset_received = TRUE;
            Timer16_set_counter(0);
        }
#endif
        Usb_reset_action();
        Usb_send_event(EVT_USB_RESET);
    }

    // ---------- OTG events management ------------------------------------
    // ---------------------------------------------------------------------
#if (USB_OTG_FEATURE == ENABLE) && (USB_HOST_FEATURE == ENABLE)

    // -----------------------------------------------------------------------
    /// Arbitrer
    // -----------------------------------------------------------------------
    // - OTG HNP Success detection
    if (Is_usb_role_exchange_interrupt() && Is_role_exchange_interrupt_enabled()) {
        //     TRACE_DEBUG("OTG HNP detect\n\r");
        Usb_ack_role_exchange_interrupt();
        Host_ack_device_connection();
        Host_ack_device_disconnection();
        Otg_send_event(EVT_OTG_HNP_SUCCESS);
        End_session_with_srp();
        Clear_otg_features_from_host();
        if (Is_usb_id_host()) {
            // HOST (A- or B-) mode
            if ((device_state != A_PERIPHERAL) && (device_state != A_END_HNP_WAIT_VFALL)) {
                static volatile unsigned int jcb;
                // Current mode is A-HOST, device will take the A-PERIPHERAL role
                b_uut_device_state = B_PERIPHERAL;
                device_state = A_PERIPHERAL;
                usb_connected = FALSE;
                usb_configuration_nb = 0;
                Usb_select_device();
                USBD_Connect();//Usb_attach();
                Usb_unfreeze_clock();
                Usb_disable_role_exchange_interrupt();
                Usb_disable_hnp_error_interrupt();
                Usb_device_stop_hnp();
                Usb_ack_reset();
                Timer16_set_counter(0);
                Usb_freeze_clock();                // USB clock can be freezed to slow down events and condition detection
                //jcb       while (Timer16_get_counter_low() != 20);
                jcb=0;
                while( jcb <100000){ jcb++; }

                Usb_unfreeze_clock();
                reset_received = FALSE;
                Usb_disable_sof_interrupt();       // will be set in the next OTG Timer IT (mandatory)

                Usb_enable_suspend_interrupt();
                Usb_enable_reset_interrupt();
                dev_configure_endpoint(EP_CONTROL,
                    TYPE_CONTROL,
                    DIRECTION_OUT,
                    SIZE_64,
                    ONE_BANK,
                    NYET_DISABLED);
            }
        }
        else {
            // In B_HOST mode, the HNPREQ bit must not be cleared because it releases the bus in suspend mode (and sof can't start)
            if ((b_uut_device_state != B_HOST) && (b_uut_device_state != B_END_HNP_SUSPEND)) {
                static volatile unsigned int jcb2;
                // Current mode is B-PERIPHERAL, device will go into B-HOST role
                End_session_with_srp();
                Clear_otg_features_from_host();
                b_uut_device_state = B_HOST;
                device_state = DEVICE_ATTACHED;
                usb_connected = FALSE;
                usb_configuration_nb = 0;
                TRACE_DEBUG("Select host 3\n\r");
                Usb_select_host();

                TRACE_DEBUG("Send reset\n\r");
                Host_send_reset();     // send the first RESET
                while (Host_is_reset());
                TRACE_DEBUG("Reset passed\n\r");

                jcb2=0;
                while( jcb2 <1000000){ jcb2++; }
                Host_enable_sof();     // start Host (sof)
                Usb_disable_role_exchange_interrupt();
                Usb_disable_hnp_error_interrupt();
                Clear_all_user_request();

                TRACE_DEBUG("Select host 3\n\r");
            }
        }
    }

    // -----------------------------------------------------------------------
    /// Arbitrer
    // -----------------------------------------------------------------------
    // - OTG HNP Failure detection
    if (Is_usb_hnp() && Is_usb_hnp_error_interrupt()&& Is_hnp_error_interrupt_enabled()) {
        TRACE_DEBUG("OTG HNP failure\n\r");
        Usb_device_stop_hnp();
        Usb_disable_role_exchange_interrupt();
        Usb_disable_hnp_error_interrupt();
        Usb_ack_hnp_error_interrupt();
        if (Is_usb_id_device()) {
            Otg_send_event(EVT_OTG_HNP_ERROR);
            Clear_all_user_request();
        }
    }   
#endif
#endif// End DEVICE FEATURE MODE && USB_HOST_FEATURE

    // ---------- HOST events management -----------------------------------
    // ---------------------------------------------------------------------
#if (((USB_HOST_FEATURE == ENABLE) && (USB_DEVICE_FEATURE == ENABLE)) || (USB_OTG_FEATURE == ENABLE))
    // -----------------------------------------------------------------------
    /// Arbitrer
    // -----------------------------------------------------------------------
    // - ID pin change detection
    if(Is_usb_id_transition()&&Is_usb_id_interrupt_enabled()) {
        TRACE_DEBUG("ID pin change\n\r");
        TRACE_DEBUG("device_state = %d\n\r", device_state);
        TRACE_DEBUG("b_uut_device_state = %d\n\r", b_uut_device_state);
        Usb_device_stop_hnp();
#if (USB_OTG_FEATURE == ENABLE)
        Clear_all_user_request();
#endif
        if(Is_usb_id_device()) { 
            g_usb_mode = USB_MODE_DEVICE;
        }
        else {
            g_usb_mode = USB_MODE_HOST;
        }
        Usb_ack_id_transition();
        if( g_usb_mode != g_old_usb_mode) { // Basic Debounce
            if(Is_usb_id_device()) { // Going into device mode
                Usb_send_event(EVT_USB_DEVICE_FUNCTION);
#if (USB_OTG_FEATURE == ENABLE)
                b_uut_device_state = B_IDLE;
#endif
                device_state = DEVICE_UNATTACHED;
#if (USB_OTG_FEATURE == ENABLE)
                id_changed_to_host_event = DISABLE;
#endif
            }
            else {                  // Going into host mode
#if (USB_OTG_FEATURE == ENABLE)
                b_uut_device_state = B_IDLE;
#endif
                device_state = DEVICE_UNATTACHED;
                Usb_send_event(EVT_USB_HOST_FUNCTION);
#if (USB_OTG_FEATURE == ENABLE)
                id_changed_to_host_event = ENABLE;
#endif
            }
            Usb_id_transition_action();
            TRACE_INFO("Pin Id changed\n\r");
#if ( ID_PIN_CHANGE_GENERATE_RESET == ENABLE)
            // Hot ID transition generates wdt reset
#endif
        }
    }
   
#endif
#if ((USB_HOST_FEATURE == ENABLE) || (USB_OTG_FEATURE == ENABLE))
    // -----------------------------------------------------------------------
    /// Host
    // -----------------------------------------------------------------------
    // - The device has been disconnected
    // JCB to be fix
    if(Is_device_disconnection() && Is_host_device_disconnection_interrupt_enabled()) {
        TRACE_DEBUG("device disconnect\n\r");
        host_disable_all_pipe();
        Host_ack_device_disconnection();
        device_state=DEVICE_DISCONNECTED;
        Usb_send_event(EVT_HOST_DISCONNECTION);
        TRACE_INFO("Device disconnected\n\r");
        Host_device_disconnection_action();
#if (USB_OTG_FEATURE == ENABLE)
        Clear_all_user_request();
#endif
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
#if (USB_OTG_FEATURE == ENABLE)
        if (Is_usb_hnp()) {
            Usb_host_reject_hnp();
            Usb_disable_hnp_error_interrupt();
            Usb_disable_role_exchange_interrupt();
        }
#endif
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
#if (USB_OTG_FEATURE == ENABLE)
        if (Is_usb_hnp()) {
            Usb_host_reject_hnp();
            Usb_disable_hnp_error_interrupt();
            Usb_disable_role_exchange_interrupt();
        }
#endif
        Host_send_resume();
    }
#endif // End HOST FEATURE MODE
}


#if (USB_OTG_FEATURE == ENABLE)

//------------------------------------------------------------------------------
/// OTG TIMER interrupt subroutine
/// This function is called each time a OTG Timer interrupt occurs
/// Function decrements the variables required by OTG program
void otg_timer_interrupt(void)
{
    /// OTG Messaging timer
    if( (pOTGDescriptor->bOTGMessagingOutput == OTGMSG_ALL) 
     || (pOTGDescriptor->bOTGMessagingOutput == OTGMSG_FAIL)) {
        if ((Get_failure_msg_delay() != 0x0000) && (Get_failure_msg_delay() != 0xFFFF)) {
            Decrement_failure_msg_delay(); 
        }
        if( (pOTGDescriptor->bOTGMessagingOutput == OTGMSG_ALL) {
            if ((Get_event_msg_delay() != 0x0000) && (Get_event_msg_delay() != 0xFFFF)) { 
                Decrement_event_msg_delay(); 
            }
        }
    }
    /// Increments Tb_Srp counter if needed
    if (Is_srp_sent_and_waiting_answer()) {
        otg_tb_srp_cpt++; 
    }
    /// Increments T_vbus_wait_connect if needed
    if (Is_srp_received_and_waiting_connect()) {
        otg_ta_srp_wait_connect++;
    }
    /// Decrements Ta_aidl_bdis timer if needed (A_suspend state)
    if ((device_state == A_SUSPEND) && (otg_ta_aidl_bdis_tmr > 1)) {
        otg_ta_aidl_bdis_tmr--;
    }
    /// Decrements Timeout_bdev_respond timer if needed
    if ((device_state == DEVICE_DEFAULT) && (!Is_timeout_bdev_response_overflow())) {
        otg_timeout_bdev_respond--;
    }
    /// Decrements Ta_vbus_rise timer if needed
    if (!Is_ta_vbus_rise_counter_overflow()) {
        otg_ta_vbus_rise--;
    }
    /// Decrements Ta_vbus_fall timer if needed  
    if (!Is_ta_vbus_fall_counter_overflow()) {
        otg_end_hnp_vbus_delay--;
    }

    /// Needed for compliance only
    if (device_state == A_PERIPHERAL) {
        if (Is_sof_interrupt_enabled() && (reset_received == TRUE)) {
            if (otg_last_sof_stored != otg_last_sof_received) {
                // No SOF is missing
                otg_last_sof_received = otg_last_sof_stored;
            }
            else {
                // SOF seems to be missing..
                Usb_freeze_clock();
                Usb_disable_sof_interrupt();
                reset_received = FALSE;
                //jcb   while (Timer16_get_counter_low() != 20);  // overflow set to 62 in usb_task.h
                Usb_unfreeze_clock();
            }
            otg_last_sof_received = UDFNUML;
            otg_last_sof_stored = UDFNUML;
        }
    }
}


//------------------------------------------------------------------------------
/// OTG Messaging task initialization
/// Initializes variables and screen to prepare next messages to be handled
//------------------------------------------------------------------------------
void Otg_message_task_init(void)
{
    TRACE_DEBUG("Otg_message_task_init\n\r");
    Otg_messaging_init();
    otg_msg_event_delay = 0;
    otg_msg_failure_delay = 0;
}


//------------------------------------------------------------------------------
/// OTG Messaging main task
/// OTG specifies that user must be kept informed of several events
/// This task allows user to display two kinds of messages : EVENT or FAILURE
/// For each new message, it can specify if the message remains displayed all
/// the time or only during a specified delay
//------------------------------------------------------------------------------
void Otg_message_task(void)
{
    // Check if an OTG message must be erased (if it was set up for a specified delay)
    if( (pOTGDescriptor->bOTGMessagingOutput == OTGMSG_ALL) 
     || (pOTGDescriptor->bOTGMessagingOutput == OTGMSG_FAIL)) {
        if (Get_failure_msg_delay() == 0) {
            Otg_clear_failure_message();
        }
        if( (pOTGDescriptor->bOTGMessagingOutput == OTGMSG_ALL) {
            if (Get_event_msg_delay() == 0) {
                Otg_clear_event_message();
            }
        }
    }
}
#endif // #if (USB_OTG_FEATURE == ENABLE)


//------------------------------------------------------------------------------
/// Message "OTG not supported"
//------------------------------------------------------------------------------
void otg_not_supported_device(void)
{
    TRACE_DEBUG("otg_not_supported_device\n\r");
    Otg_send_event(EVT_OTG_DEV_UNSUPPORTED);
}


