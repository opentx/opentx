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
#include "conf_usb.h"
#include <usb/common/core/USBConfigurationOTG.h>
#include <usb/device/core/USBD.h>
#include <usb/device/massstorage/MSDDriver.h>
#include "usb/otg/usb_task.h"
#include "usb_host_task.h"
#include "usb/otg/usb_drv.h"
#include "usb/otg/usb_host_enum.h"
#include <utility/trace.h>
#include "main.h"

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------
//jcb #define WAIT_100MS  100
// Wait 100 x 125 us = 12,5 ms before USB reset
//#define WAIT_100MS  800
#define WAIT_100MS  100

#ifndef DEVICE_ADDRESS
#error DEVICE_ADDRESS should be defined somewhere in config files (conf_usb.h)
#endif

#ifndef SIZEOF_DATA_STAGE
#error SIZEOF_DATA_STAGE should be defined in conf_usb.h
#endif

#ifndef HOST_CONTINUOUS_SOF_INTERRUPT
#error HOST_CONTINUOUS_SOF_INTERRUPT should be defined as ENABLE or DISABLE in conf_usb.h
#endif

#ifndef Usb_id_transition_action
#define Usb_id_transition_action()
#endif
#ifndef  Host_device_disconnection_action
#define Host_device_disconnection_action()
#endif
#ifndef  Host_device_connection_action
#define Host_device_connection_action()
#endif
#ifndef  Host_sof_action
#define Host_sof_action()
#endif
#ifndef  Host_suspend_action
#define Host_suspend_action()
#endif
#ifndef  Host_hwup_action
#define Host_hwup_action()
#endif
#ifndef  Host_device_not_supported_action
#define Host_device_not_supported_action()
#endif
#ifndef  Host_device_class_not_supported_action
#define Host_device_class_not_supported_action()
#endif
#ifndef  Host_device_supported_action
#define Host_device_supported_action()
#endif
#ifndef  Host_device_error_action
#define Host_device_error_action()
#endif

extern U8 id_changed_to_host_event;

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

volatile S_pipe_int it_pipe_str[MAX_EP_NB];
volatile U8 pipe_nb_save;
U8 g_sav_int_sof_enable;

//! Its value represent the current state of the
//! device connected to the usb host controller
//! Value can be:
//! - DEVICE_ATTACHED
//! - DEVICE_POWERED
//! - DEVICE_SUSPENDED
//! - DEVICE_DEFAULT
//! - DEVICE_ADDRESSED
//! - DEVICE_CONFIGURED
//! - DEVICE_ERROR
//! - DEVICE_UNATTACHED
//! - DEVICE_READY
//! - DEVICE_WAIT_RESUME
//! - DEVICE_DISCONNECTED
//! - DEVICE_DISCONNECTED_ACK
//! - and these have been added for OTG :
//! - A_PERIPHERAL
//! - A_INIT_HNP
//! - A_SUSPEND
//! - A_END_HNP_WAIT_VFALL
U8 device_state;

//! For control requests management over pipe 0
S_usb_setup_data    usb_request;

//! Internal RAM buffer for USB data stage content
//! This buffer is required to setup host enumeration process
//! Its contains the device descriptors received.
//! Depending on the device descriptors lenght, its size can be optimized
//! with the SIZEOF_DATA_STAGE define of conf_usb.h file
U8 data_stage[SIZEOF_DATA_STAGE];

U8 device_status;
U8 request_resume;


U8 new_device_connected=0;

//------------------------------------------------------------------------------
//        Local Variables
//------------------------------------------------------------------------------
static U16  hostSOFCounter;     // As internal host start of frame counter


//------------------------------------------------------------------------------
//      Internal Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// This function initializes the USB controller in host mode and the associated
/// variables.
/// This function enables the USB controller for host mode operation.
//------------------------------------------------------------------------------
void usb_host_task_init(void)
{
    unsigned int i;

    TRACE_DEBUG("usb_host_task_init\n\r");

    for( i=0; i<SIZEOF_DATA_STAGE; i++ ) {
        data_stage[i] = 0;
    }
    Usb_disable();
    Usb_enable();
    Usb_unfreeze_clock();
    USBD_Connect();
    Usb_enable_uvcon_pin();
    Usb_select_host();
    Usb_disable_vbus_hw_control();   // Force Vbus generation without timeout
    Host_enable_device_disconnection_interrupt();
    device_state=DEVICE_UNATTACHED;
}

//------------------------------------------------------------------------------
/// Entry point of the USB host management
/// The aim is to manage the device target connection and enumeration
/// depending on the device_state, the function performs the required operations
/// to get the device enumerated and configured
/// Once the device is operationnal, the device_state value is DEVICE_READY
/// This state should be tested by the host task application before performing
/// any applicative requests to the device.
//------------------------------------------------------------------------------
void usb_host_task(void)
{
    U32 dec;

    //TRACE_DEBUG("host_task ");
    switch (device_state) {
        case DEVICE_UNATTACHED:
            //   DEVICE_UNATTACHED state
            //   - Default init state
            //   - Try to give device power supply
            //TRACE_DEBUG("DEVICE_UNATTACHED\n\r");
            for (hostSOFCounter=0; hostSOFCounter<MAX_EP_NB; hostSOFCounter++) {
                ep_table[hostSOFCounter]=0;
            }// Reset PIPE lookup table with device EP addr
            nb_interface_supported=0;
            Host_clear_device_supported();        // Reset Device status
            Host_clear_configured();
            Host_clear_device_ready();
            Usb_clear_all_event();                // Clear all software events
            new_device_connected=0;
            if (Is_usb_id_host()) {
              if(pOTGDescriptor->bOTGSoftwareVbusCtrl == ENABLE) {
                if( Is_usb_bconnection_error_interrupt()||Is_usb_vbus_error_interrupt())
                {
                    Usb_ack_bconnection_error_interrupt();
                    Usb_ack_vbus_error_interrupt();
                    Host_clear_vbus_request();
                }
                if (Is_usb_id_host()) {
                    Host_ack_device_connection();
                    // Usb_enable_manual_vbus();
                    Usb_ack_bconnection_error_interrupt();
                    Usb_enable_vbus_pad();
                    Usb_enable_vbus();
                    Host_vbus_action();
                }

                Usb_disable_vbus_pad();
                Usb_enable_manual_vbus();

                if(Is_usb_srp_interrupt()) {
                    Usb_ack_srp_interrupt();
                    // if Vbus was not already delivered, it is really an SRP (OTG B-Device)
                    device_state=DEVICE_ATTACHED;
                    Usb_ack_bconnection_error_interrupt();
                    Usb_enable_vbus_pad();
                    Usb_enable_vbus();
                    Host_vbus_action();
                }
              }
              else {
                Usb_enable_vbus();                    // Give at least device power supply!!!
                Host_vbus_action();
                if(Is_usb_vbus_high()) { 
                    device_state=DEVICE_ATTACHED; 
                }     // If VBUS ok goto to device connection expectation
              }
            }
            break;

        case DEVICE_ATTACHED :
            //   DEVICE_ATTACHED state
            //   - Vbus is on
            //   - Try to detect device connection
            if (Is_device_connection()) {    // Device pull-up detected
                //TRACE_DEBUG("Is_device_connection\n\r");
                Host_ack_device_connection();
                // Now device is connected, enable disconnection interrupt
                Host_enable_device_disconnection_interrupt();
                // Reset device status
                Host_clear_device_supported();
                Host_clear_configured();
                Host_clear_device_ready();
                Host_ack_sof();

                TRACE_DEBUG("begin timer\n\r");
                gSystemTick = 0;
                DelayMS(200);
                TRACE_DEBUG("end timer\n\r");
                // Clear bad VBUS error
                Usb_ack_vbus_error_interrupt();
                Host_disable_device_disconnection_interrupt();

                Host_send_reset();  // First USB reset
                Host_enable_sof();  // Start Start Of Frame generation
                Host_enable_sof_interrupt();  // SOF will be detected under interrupt

                Usb_ack_event(EVT_HOST_SOF);
                while (Is_host_reset()) {
                } // Active wait of end of reset send
                Host_ack_reset();
                //Workaround for some bugly devices with powerless pull up
                //usually low speed where data line rise slowly and can be interpretaded as disconnection
                for(hostSOFCounter=0;hostSOFCounter!=0xFFFF;hostSOFCounter++) {   // Basic Timeout counter
                    if(Is_usb_event(EVT_HOST_SOF)) {  //If we detect SOF, device is still alive and connected, just clear false disconnect flag
                        if(Is_device_disconnection()) {
                            Host_ack_device_connection();
                            Host_ack_device_disconnection();
                            break;
                        }
                    }
                }
                Host_enable_device_disconnection_interrupt();
                hostSOFCounter = 0;
                while (hostSOFCounter<WAIT_100MS) {              // wait 100ms after USB reset
                    if (Is_usb_event(EVT_HOST_SOF)) {
                        Usb_ack_event(EVT_HOST_SOF);
                        hostSOFCounter++;
                    }// Count Start Of frame
                    if (Is_host_emergency_exit() || Is_usb_bconnection_error_interrupt()) {
                        TRACE_DEBUG("goto error 2\n\r");
                        goto device_attached_error;
                    }
                }
                device_state = DEVICE_POWERED;
                hostSOFCounter=0;

            }


device_attached_error:
            // Device connection error, or vbus pb -> Retry the connection process from the begining
            if( Is_usb_bconnection_error_interrupt()||Is_usb_vbus_error_interrupt()||Is_usb_vbus_low()) {
                TRACE_DEBUG("HOST device_attached_error\n\r");
                TRACE_DEBUG("Is_usb_bconnection_error_interrupt: 0x%X\n\r", Is_usb_bconnection_error_interrupt());
                TRACE_DEBUG("Is_usb_vbus_error_interrupt: 0x%X\n\r", Is_usb_vbus_error_interrupt());
                TRACE_DEBUG("Is_usb_vbus_low: 0x%X\n\r", Is_usb_vbus_low());
                //while(1);  //JCB for test, to be removed.
                if (Is_usb_id_host()) {
                    Usb_ack_bconnection_error_interrupt();
                    Usb_enable_vbus_hw_control();
                    device_state=DEVICE_UNATTACHED;
                    Usb_disable_vbus();
                    Usb_disable_vbus_pad();
                    Usb_enable_vbus_pad();
                    Usb_ack_vbus_error_interrupt();
                    Usb_enable_vbus();
                    Usb_disable_vbus_hw_control();
                    Host_disable_sof();
                    Host_vbus_action();
                }
                else {
                    device_state = DEVICE_UNATTACHED;
                }
            }
            break;

        case DEVICE_POWERED :
            //   DEVICE_POWERED state
            //
            //   - Device connection (attach) as been detected,
            //   - Wait 100ms and configure default control pipe
            //
            //TRACE_INFO("Device Connection\n\r");
            Host_device_connection_action();
            if (Is_usb_event(EVT_HOST_SOF)) {
                Usb_ack_event(EVT_HOST_SOF);
                if (hostSOFCounter++ >= WAIT_100MS) {
                    device_state = DEVICE_DEFAULT;
                    Host_select_pipe(PIPE_CONTROL);
                    Host_enable_pipe();
                    host_configure_pipe(PIPE_CONTROL,
                                        TYPE_CONTROL,
                                        TOKEN_SETUP,
                                        EP_CONTROL,
                                        SIZE_64,
                                        ONE_BANK,
                                        0 );
                    device_state = DEVICE_DEFAULT;
                }
            }
            break;

        case DEVICE_DEFAULT :
            //   DEVICE_DEFAULT state
            //   - Get device descriptor
            //   - Reconfigure Pipe 0 according to Device EP0
            //   - Attribute device address
            TRACE_DEBUG("DEVICE_DEFAULT\n\r");
            // Get first device descriptor

            if( CONTROL_GOOD == host_get_device_descriptor_uncomplete()) {
                hostSOFCounter = 0;
                while(hostSOFCounter<20) {          // wait 20ms before USB reset (special buggly devices...)
                    if (Is_usb_event(EVT_HOST_SOF)) {
                        Usb_ack_event(EVT_HOST_SOF);
                        hostSOFCounter++;
                    }
                    if (Is_host_emergency_exit() || Is_usb_bconnection_error_interrupt()) {
                        break;
                    }
                }
                Host_disable_device_disconnection_interrupt();
                Host_send_reset();          // First USB reset
                Usb_ack_event(EVT_HOST_SOF);
                while (Is_host_reset());    // Active wait of end of reset send
                Host_ack_reset();


                //Workaround for some bugly devices with powerless pull up
                //usually low speed where data line rise slowly and can be interpretaded as disconnection
                for(hostSOFCounter=0;hostSOFCounter!=0xFFFF;hostSOFCounter++) {   // Basic Timeout counter
                    if(Is_usb_event(EVT_HOST_SOF)) {  //If we detect SOF, device is still alive and connected, just clear false disconnect flag
                        if(Is_device_disconnection()) {
                            Host_ack_device_connection();
                            Host_ack_device_disconnection();
                            break;
                        }
                    }
                }
                Host_enable_device_disconnection_interrupt();
                hostSOFCounter = 0;
                while(hostSOFCounter<200) {          // wait 200ms after USB reset
                    if (Is_usb_event(EVT_HOST_SOF)) {
                        Usb_ack_event(EVT_HOST_SOF);
                        hostSOFCounter++;
                    }
                    if (Is_host_emergency_exit() || Is_usb_bconnection_error_interrupt()) {
                        break;
                    }
                }

                Host_select_pipe(PIPE_CONTROL);
                Host_disable_pipe();
                Host_unallocate_memory();
                Host_enable_pipe();
                // Re-Configure the Ctrl Pipe according to the device ctrl EP

                TRACE_DEBUG("Size: 0x%X\n\r", (U16)data_stage[OFFSET_FIELD_MAXPACKETSIZE]);
                TRACE_DEBUG("Size Pipe: 0x%X\n\r", host_determine_pipe_size((U16)data_stage[OFFSET_FIELD_MAXPACKETSIZE]));

                host_configure_pipe(PIPE_CONTROL,
                                    TYPE_CONTROL,
                                    TOKEN_SETUP,
                                    EP_CONTROL,
                                    host_determine_pipe_size((U16)data_stage[OFFSET_FIELD_MAXPACKETSIZE]),
                                    ONE_BANK,
                                    0 );
                // Give an absolute device address
                host_set_address(DEVICE_ADDRESS);
                host_configure_address(PIPE_CONTROL, DEVICE_ADDRESS);
                device_state = DEVICE_ADDRESSED;
            }
            else {
                device_state = DEVICE_ERROR;
            }
            break;

        case DEVICE_ADDRESSED :
            //   DEVICE_ADDRESSED state
            //   - Check if VID PID is in supported list
            TRACE_DEBUG("DEVICE_ADDRESSED\n\r");
            if (CONTROL_GOOD == host_get_device_descriptor()) {
                // Detect if the device connected belongs to the supported devices table
                if (HOST_TRUE == host_check_VID_PID()) {
                    Host_set_device_supported();
                    Host_device_supported_action();
                    device_state = DEVICE_CONFIGURED;
                }
                else
                {
                }

            }
            else  {// Can not get device descriptor
                device_state = DEVICE_ERROR;
            }
            break;

        case DEVICE_CONFIGURED :
            //   DEVICE_CONFIGURED state
            //   - Configure pipes for the supported interface
            //   - Send Set_configuration() request
            //   - Goto full operating mode (device ready)
            TRACE_DEBUG("DEVICE_CONFIGURED\n\r");
            if (CONTROL_GOOD == host_get_configuration_descriptor()) {
                if (HOST_FALSE != host_check_class()) { // Class support OK?
                    host_auto_configure_endpoint();

                    if (Is_host_configured()) {
                        if (CONTROL_GOOD== host_set_configuration(1)) { // Send Set_configuration
                            // host_set_interface(interface_bound,interface_bound_alt_set);
                            // device and host are now fully configured
                            // goto DEVICE READY normal operation
                            device_state = DEVICE_READY;
                            // monitor device disconnection under interrupt
                            Host_enable_device_disconnection_interrupt();
                            // If user host application requires SOF interrupt event
                            // Keep SOF interrupt enable otherwize, disable this interrupt
#if (HOST_CONTINUOUS_SOF_INTERRUPT==DISABLE)
                            Host_disable_sof_interrupt();
#endif
                            new_device_connected=TRUE;
                            TRACE_INFO("Device Enumerated\n\r");
                        }
                        else { // Problem during Set_configuration request...
                            device_state = DEVICE_ERROR;
                        }
                    }
                }
                else { // device class not supported...
                    device_state = DEVICE_ERROR;
                    TRACE_INFO("Unsupported Device\n\r");
                    Host_device_class_not_supported_action();
                }
            }
            else { // Can not get configuration descriptors...
                device_state = DEVICE_ERROR;
            }
            break;

        case DEVICE_READY :
            //   DEVICE_READY state
            //   - Full standard operating mode
            //   - Nothing to do...
            // Host full std operating mode!
            new_device_connected=FALSE;

            // Handles user requests : "stop Vbus" and "suspend"
            break;

        case DEVICE_ERROR :
            //------------------------------------------------------
            //   DEVICE_ERROR state
            //   - Error state
            //   - Do custom action call (probably go to default mode...)
            TRACE_DEBUG("DEVICE_ERROR\n\r");
            Host_device_error_action();
            break;

        case DEVICE_SUSPENDED :
            //   DEVICE_SUSPENDED state
            //   - Host application request to suspend the device activity
            //   - State machine comes here thanks to Host_request_suspend()
            TRACE_DEBUG("DEVICE_SUSPENDED\n\r");
                device_state=DEVICE_WAIT_RESUME;    // wait for device resume event
                if(Is_device_supports_remote_wakeup()) { // If the connected device supports remote wake up
                    if (CONTROL_GOOD != host_set_feature_remote_wakeup()) {
                        device_state = DEVICE_DISCONNECTED;   // stop connexion because device has not accepted the feature
                    }
                }
                TRACE_INFO("Usb suspend\n\r");
                hostSOFCounter = Is_host_sof_interrupt_enabled(); //Save current sof interrupt enable state
                Host_disable_sof_interrupt();
                Host_ack_sof();
                Host_disable_sof();           // Stop start of frame generation, this generates the suspend state
                Host_ack_remote_wakeup();
                Host_enable_remote_wakeup_interrupt();
                Host_ack_hwup();
                Host_enable_hwup_interrupt(); // Enable host wake-up interrupt
                // (this is the unique USB interrupt able to wake up the CPU core from power-down mode)
                Usb_freeze_clock();
                Host_suspend_action();              // Custom action here! (for example go to power-save mode...)
            }
            break;

        case DEVICE_WAIT_RESUME :
            //   DEVICE_WAIT_RESUME state
            //   - Wait in this state till the host receives an upstream resume from the device
            //   - or the host software request the device to resume
            TRACE_DEBUG("DEVICE_WAIT_RESUME\n\r");
            if(Is_usb_event(EVT_HOST_REMOTE_WAKEUP)|| Is_host_request_resume()) { // Remote wake up has been detected
                // or Local resume request has been received
                if(Is_host_request_resume()) {      // Not a remote wakeup, but an host application request
                    Host_disable_hwup_interrupt();  // Wake up interrupt should be disable host is now wake up !
                    // CAUTION HWUP can be cleared only when USB clock is active
                    //               Pll_start_auto();               // First Restart the PLL for USB operation
                    //               Wait_pll_ready();               // Get sure pll is lock
                    Usb_unfreeze_clock();           // Enable clock on USB interface
                }
                Host_ack_hwup();                // Clear HWUP interrupt flag
                Host_enable_sof();

                if (Is_usb_event(EVT_HOST_REMOTE_WAKEUP)) {
                    Usb_ack_event(EVT_HOST_REMOTE_WAKEUP);    // Ack software event
                    Host_disable_sof_interrupt();
                    Host_ack_device_disconnection();
                    Host_disable_device_disconnection_interrupt();

                    Host_send_resume();     // this other downstream resume is done to ensure min. 20ms of HOST DRIVING RESUME (not Device)
                    while (!Is_device_disconnection() && Host_is_resume());
                    hostSOFCounter = 0;
                    Host_ack_sof();
                    while (!Is_device_disconnection() && (hostSOFCounter != 12)) {  // wait for min. 10ms of device recovery time
                        if (Is_host_sof()) {
                            Host_ack_sof();
                            hostSOFCounter++;
                        }
                    }
                    if (Is_device_disconnection()) {
                        usb_host_task_init();
                        device_state = DEVICE_DISCONNECTED;
                        Host_ack_remote_wakeup();        // Ack remote wake-up reception
                        Host_ack_request_resume();       // Ack software request
                        Host_ack_down_stream_resume();   // Ack down stream resume sent
                    }
                    else {
                        device_state = DEVICE_READY;
                        Host_ack_remote_wakeup();        // Ack remote wake-up reception
                        Host_ack_request_resume();       // Ack software request
                        Host_ack_down_stream_resume();   // Ack down stream resume sent
                    }
                    Host_enable_device_disconnection_interrupt();
                    Host_ack_sof();
                }
                else {
                    Host_send_resume();                            // Send down stream resume
                    //-----------------------
                    // Work-around for case of Device disconnection during Suspend
                    // The disconnection is never detected and the Resume bit remains high (and RSMEDI flag never set)
                    // If the timeout elapses, it implies that the device has disconnected => macro is reset (to reset the Resume bit)
                    dec = 0;
                    while (dec < 0x4FFFF) {  // several hundreds of ms
                        if (Is_host_down_stream_resume()) {  // Wait Down stream resume sent
                            Host_ack_remote_wakeup();        // Ack remote wake-up reception
                            Host_ack_request_resume();       // Ack software request
                            Host_ack_down_stream_resume();   // Ack down stream resume sent
                            if(hostSOFCounter) {
                                Host_enable_sof_interrupt();
                            } // Restore SOF interrupt enable state before suspend
                            device_state=DEVICE_READY;       // Come back to full operating mode
                            TRACE_INFO("Usb resumed\n\r");
                            dec = 0x3FFFFE; // will cause a loop end
                        }
                        dec++;
                    }
                    if (dec != 0x3FFFFF) {   // if resume failed
                        usb_host_task_init();
                        device_state = DEVICE_DISCONNECTED;
                    }
                    else
                    {
                        hostSOFCounter = 0;
                        Host_ack_sof();
                        while (!Is_device_disconnection() && (hostSOFCounter != 12)) {  // wait for min. 10ms of device recovery time
                            if (Is_host_sof()) {
                                Host_ack_sof();
                                hostSOFCounter++;
                            }
                        }
                    }
                    //-----------------------End of Work Around
                }
            }
            break;

        case DEVICE_DISCONNECTED :
            //   DEVICE_DISCONNECTED state
            //   - Device disconnection has been detected
            //   - Run scheduler in this state at least two times to get sure event is detected by all host application tasks
            //   - Go to DEVICE_DISCONNECTED_ACK state before DEVICE_UNATTACHED, to get sure scheduler calls all app tasks...
            TRACE_DEBUG("DEVICE_DISCONNECTED\n\r");
            device_state = DEVICE_DISCONNECTED_ACK;
            break;

        case DEVICE_DISCONNECTED_ACK :
            //   DEVICE_DISCONNECTED_ACK state
            //   - Device disconnection has been detected and managed bu applicatives tasks
            //   - Go to DEVICE_UNATTACHED state
            TRACE_DEBUG("DEVICE_DISCONNECTED_ACK\n\r");
            host_disable_all_pipe();
            device_state = DEVICE_UNATTACHED;
            break;


        default :
            //   default state
            //   - Default case: ERROR
            //   - Goto no device state
            TRACE_DEBUG("default\n\r");
            device_state = DEVICE_UNATTACHED;
            break;
    }
}

//------------------------------------------------------------------------------
/// This function send nb_data pointed with *buf with the pipe number specified
/// This function will activate the host sof interrupt to detect timeout. The
/// interrupt enable sof will be restore.
//------------------------------------------------------------------------------
U8 host_send_data(U8 pipe, U16 nb_data, U8 *buf)
{
    U16 length;
    U8 status=PIPE_GOOD;
    U8 sav_int_sof_enable;
    U8 nak_timeout;
#if (NAK_TIMEOUT_ENABLE==ENABLE)
    U16 cpt_nak;
#endif
    U16 nb_data_loaded;

    TRACE_DEBUG("host_send_data[]%d: %d\n\r", pipe, nb_data);

    sav_int_sof_enable=Is_host_sof_interrupt_enabled();  // Save state of enable sof interrupt
    Host_enable_sof_interrupt();
    Host_select_pipe(pipe);

    Host_set_token_in();

    Host_set_token_out();
    Host_ack_out_sent();
    /*
    TRACE_DEBUG("CTRL: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_CTRL);
    TRACE_DEBUG("SR: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_SR);
    TRACE_DEBUG("DEVCTRL: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_DEVCTRL);
    TRACE_DEBUG("DEVISR: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_DEVISR);
    TRACE_DEBUG("DEVIMR: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_DEVIMR);
    TRACE_DEBUG("DEVEPTISR: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_DEVEPTISR[0]);
    TRACE_DEBUG("HSTCTRL: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTCTRL);
    TRACE_DEBUG("HSTISR: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTISR);
    TRACE_DEBUG("HSTIMR: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTIMR);
    TRACE_DEBUG("HSTPIP: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIP);
    TRACE_DEBUG("HSTPIPCFG[0]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[0]);
    TRACE_DEBUG("HSTPIPCFG[1]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[1]);
    TRACE_DEBUG("HSTPIPCFG[2]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[2]);
    TRACE_DEBUG("HSTPIPINRQ[0]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPINRQ[0]);
    TRACE_DEBUG("HSTPIPINRQ[1]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPINRQ[1]);
    TRACE_DEBUG("HSTPIPINRQ[2]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPINRQ[2]);
    TRACE_DEBUG("HSTPIPERR[0]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPERR[0]);
    TRACE_DEBUG("HSTPIPERR[1]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPERR[1]);
    TRACE_DEBUG("HSTPIPERR[2]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPERR[2]);
    TRACE_DEBUG("HSTPIPISR[0]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[0]);
    TRACE_DEBUG("HSTPIPISR[1]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[1]);
    TRACE_DEBUG("HSTPIPISR[2]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[2]);
    TRACE_DEBUG("HSTPIPIMR[0]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPIMR[0]);
    TRACE_DEBUG("HSTPIPIMR[1]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPIMR[1]);
    TRACE_DEBUG("HSTPIPIMR[2]: 0x%X\n\r",AT91C_BASE_OTGHS->OTGHS_HSTPIPIMR[2]);
    */
    while (nb_data != 0) {        // While there is something to send...
        Host_unfreeze_pipe();
        // Prepare data to be sent
        length = host_get_pipe_length();
        TRACE_DEBUG("size=%d\n\r", length);

        if ( length > nb_data) {
            nb_data_loaded = nb_data;
            length = nb_data;
        }
        else {
            nb_data_loaded = length;
        }
        Address_fifochar_endpoint(global_pipe_nb);
        while (length!=0) {             // Load Pipe buffer
            Host_write_byte(*buf++);
            //(((char*)((unsigned int *)AT91C_BASE_OTGHS_EPTFIFO->OTGHS_READEPT0))[dBytes++])=*(buf++);
            //pFifo[dBytes++] = *buf;
            //buf++;
            length--;
        }
        private_sof_counter=0;    // Reset the counter in SOF detection sub-routine
#if (NAK_TIMEOUT_ENABLE==ENABLE)
        cpt_nak=0;
#endif
        nak_timeout=0;
        Host_ack_out_sent();
        Host_send_out();
        while (!Is_host_out_sent()) {
            if (Is_host_emergency_exit()) { // Async disconnection or role change detected under interrupt
                TRACE_DEBUG("Emergency exit\n\r");
                status=PIPE_DELAY_TIMEOUT;
                Host_reset_pipe(pipe);
                goto host_send_data_end;
            }
            if (private_sof_counter>=250) {           // Count 250ms (250sof)
                TRACE_DEBUG("TimeOut Send Data\n\r");
                private_sof_counter=0;
                if (nak_timeout++>=TIMEOUT_DELAY) { // Inc timeout and check for overflow
                    status=PIPE_DELAY_TIMEOUT;
                    Host_reset_pipe(pipe);
                    goto host_send_data_end;
                }
            }
            if (Is_host_pipe_error()) { // Any error ?
                TRACE_DEBUG("pipe error\n\r");
                status = Host_error_status();
                Host_ack_all_errors();
                goto host_send_data_end;
            }
            if (Is_host_stall()) {      // Stall management
                TRACE_DEBUG("stall\n\r");
                status =PIPE_STALL;
                Host_ack_stall();
                goto host_send_data_end;
            }
#if (NAK_TIMEOUT_ENABLE==ENABLE)
            if(Is_host_nak_received()) { //NAK received
                Host_ack_nak_received();
                if (cpt_nak++>NAK_SEND_TIMEOUT) {
                   TRACE_DEBUG("NAK timeout\n\r");
                   status = PIPE_NAK_TIMEOUT;
                   Host_reset_pipe(pipe);
                   goto host_send_data_end;
                }
            }
#endif
        }
        // Here OUT sent
        nb_data -= nb_data_loaded;
        status=PIPE_GOOD;         // Frame correctly sent
        Host_ack_out_sent();
    }
    Host_freeze_pipe();
    
host_send_data_end:
    // Restore sof interrupt enable state
    if (sav_int_sof_enable==FALSE) {
        Host_disable_sof_interrupt();
    }
    // And return...
    return ((U8)status);
}



//------------------------------------------------------------------------------
/// This function receives nb_data pointed with *buf with the pipe number specified
/// The nb_data parameter is passed as a U16 pointer, thus the data pointed by this pointer
/// is updated with the final number of data byte received.
//------------------------------------------------------------------------------
U8 host_get_data(U8 pipe, U16 *nb_data, U8 *buf)
{
    U8  status=PIPE_GOOD;
    U8  sav_int_sof_enable;
    U8  nak_timeout;
    U16 n;
    U16 i;
#if (NAK_TIMEOUT_ENABLE==ENABLE)
    U16 cpt_nak;
#endif

    //   TRACE_DEBUG("host_get_data[%d]\n\r", pipe);
    n=*nb_data;
    *nb_data=0;

    sav_int_sof_enable=Is_host_sof_interrupt_enabled();
    Host_enable_sof_interrupt();

    Host_select_pipe(pipe);
    Host_set_token_in();
    Host_continuous_in_mode();
    //   Host_ack_in_received();

    while (n) {             // While missing data...
        // start IN request generation
        Host_unfreeze_pipe();
        Host_send_in();
        private_sof_counter=0; // Reset the counter in SOF detection sub-routine
        nak_timeout=0;
#if (NAK_TIMEOUT_ENABLE==ENABLE)
        cpt_nak=0;
#endif
        while (!Is_host_in_received())
        {
            if (Is_host_emergency_exit())   // Async disconnection or role change detected under interrupt
            {
                status=PIPE_DELAY_TIMEOUT;
                Host_reset_pipe(pipe);
                goto host_get_data_end;
            }
            if (private_sof_counter>=250)   // Timeout management
            {
                private_sof_counter=0;       // Done in host SOF interrupt
                if (nak_timeout++>=TIMEOUT_DELAY)// Check for local timeout
                {
                    status=PIPE_DELAY_TIMEOUT;
                    Host_reset_pipe(pipe);
                    goto host_get_data_end;
                }
            }
            if(Is_host_pipe_error())        // Error management
            {
                status = Host_error_status();
                Host_ack_all_errors();
                goto host_get_data_end;
            }
            if(Is_host_stall())             // STALL management
            {
                status =PIPE_STALL;
                Host_reset_pipe(pipe);
                Host_ack_stall();
                goto host_get_data_end;
            }
#if (NAK_TIMEOUT_ENABLE==ENABLE)
            if(Is_host_nak_received())  //NAK received
            {
                Host_ack_nak_received();
                if (cpt_nak++>NAK_RECEIVE_TIMEOUT)
                {
                    status = PIPE_NAK_TIMEOUT;
                    Host_reset_pipe(pipe);
                    goto host_get_data_end;
                }
            }
#endif
        }
        status=PIPE_GOOD;
        Host_freeze_pipe();
        if (Host_byte_counter()<=n)
        {
            if ((Host_byte_counter() < n)&&(Host_byte_counter()<host_get_pipe_length()))
            { 
                n = 0;
            }
            else
            { 
                n -= Host_byte_counter();
            }
            (*nb_data) += Host_byte_counter();  // Update nb of byte received

            Address_fifochar_endpoint(global_pipe_nb);
            for (i=Host_byte_counter(); i; i--)
            { 
                *buf = Host_read_byte(); 
                buf++;
            }
        }
        else  // more bytes received than expected
        {     // TODO error code management
            *nb_data += n;
            Address_fifochar_endpoint(global_pipe_nb);
            for (i=n; i; i--)                  // Byte number limited to the initial request (limit tab over pb)
            {
                *buf = Host_read_byte();
                buf++;
            }
            n=0;
        }
        Host_ack_in_received();
    }

    Host_freeze_pipe();

host_get_data_end:
    if (sav_int_sof_enable==FALSE)
    {
        Host_disable_sof_interrupt();
    }
    return ((U8)status);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void reset_it_pipe_str(void)
{
    U8 i;
    TRACE_DEBUG("reset_it_pipe_str\n\r");
    for(i=0;i<MAX_EP_NB;i++)
    {
        it_pipe_str[i].enable=DISABLE;
        it_pipe_str[i].timeout=0;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
U8 is_any_interrupt_pipe_active(void)
{
    U8 i;
    TRACE_DEBUG("is_any_interrupt_pipe_active\n\r");
    for(i=0;i<MAX_EP_NB;i++)
    {
        if(it_pipe_str[i].enable==ENABLE) return TRUE;
    }
    return FALSE;
}

//------------------------------------------------------------------------------
/// This function receives nb_data pointed with *buf with the pipe number specified
/// The nb_data parameter is passed as a U16 pointer, thus the data pointed by this pointer
/// is updated with the final number of data byte received.
//------------------------------------------------------------------------------
U8 host_get_data_interrupt(U8 pipe, U16 nb_data, U8 *buf,void(*handle)(U8 status, U16 nb_byte))
{
    TRACE_DEBUG("host_get_data_interrupt\n\r");
    Host_select_pipe(pipe);
    if(it_pipe_str[pipe].enable==ENABLE)
    {
        return HOST_FALSE;
    }
    else
    {
        if(is_any_interrupt_pipe_active()==FALSE)
        {
            g_sav_int_sof_enable=Is_host_sof_interrupt_enabled();
            Host_enable_sof_interrupt();
        }
        it_pipe_str[pipe].enable=ENABLE;
        it_pipe_str[pipe].nb_byte_to_process=nb_data;
        it_pipe_str[pipe].nb_byte_processed=0;
        it_pipe_str[pipe].ptr_buf=buf;
        it_pipe_str[pipe].handle=handle;
        it_pipe_str[pipe].timeout=0;
        it_pipe_str[pipe].nak_timeout=NAK_RECEIVE_TIMEOUT;

        private_sof_counter=0;           // Reset the counter in SOF detection sub-routine
        Host_reset_pipe(pipe);
        Host_enable_stall_interrupt();
#if (NAK_TIMEOUT_ENABLE==ENABLE)
        Host_enable_nak_interrupt();
#endif
        Host_enable_error_interrupt();
        Host_enable_receive_interrupt();
        Host_ack_stall();
        Host_ack_nak_received();

        Host_continuous_in_mode();
        Host_set_token_in();
        Host_unfreeze_pipe();
        return HOST_TRUE;
    }
}

//------------------------------------------------------------------------------
/// This function send nb_data pointed with *buf with the pipe number specified
//------------------------------------------------------------------------------
U8 host_send_data_interrupt(U8 pipe,
                            U16 nb_data,
                            U8 *buf,
                            void(*handle)(U8 status, U16 nb_byte))
{
    U16 i;
    U8 *ptr_buf=buf;

    TRACE_DEBUG("host_send_data_interrupt\n\r");
    Host_select_pipe(pipe);
    if(it_pipe_str[pipe].enable==ENABLE)
    {
        return HOST_FALSE;
    }
    else
    {
        if(is_any_interrupt_pipe_active()==FALSE)
        {
            g_sav_int_sof_enable=Is_host_sof_interrupt_enabled();
            Host_enable_sof_interrupt();
        }
        it_pipe_str[pipe].enable=ENABLE;
        it_pipe_str[pipe].nb_byte_to_process=nb_data;
        it_pipe_str[pipe].nb_byte_processed=0;
        it_pipe_str[pipe].ptr_buf=buf;
        it_pipe_str[pipe].handle=handle;

        it_pipe_str[pipe].timeout=0;
        it_pipe_str[pipe].nak_timeout=NAK_SEND_TIMEOUT;
        it_pipe_str[pipe].nb_byte_on_going=0;

        Host_reset_pipe(pipe);
        Host_unfreeze_pipe();
        // Prepare data to be sent
        i = host_get_pipe_length();
        if ( i > nb_data)                // Pipe size> remaining data
        {
            i = nb_data;
            nb_data = 0;
        }
        else                             // Pipe size < remaining data
        {
            nb_data -= i;
        }
        it_pipe_str[pipe].nb_byte_on_going+=i;   // Update nb data processed
        Address_fifochar_endpoint(global_pipe_nb);
        while (i!=0)                    // Load Pipe buffer
        {  
            Host_write_byte(*ptr_buf++);
            i--;
        }
        private_sof_counter=0;          // Reset the counter in SOF detection sub-routine
        it_pipe_str[pipe].timeout=0;    // Refresh timeout counter
        Host_ack_out_sent();
        Host_ack_stall();
        Host_ack_nak_received();

        Host_enable_stall_interrupt();
        Host_enable_error_interrupt();
#if (NAK_TIMEOUT_ENABLE==ENABLE)
        Host_enable_nak_interrupt();
#endif
        Host_enable_transmit_interrupt();
        Host_send_out();                // Send the USB frame
        return HOST_TRUE;
    }
}

//------------------------------------------------------------------------------
//! @brief USB pipe interrupt subroutine
//!
//! @param none
//!
//! @return none
//------------------------------------------------------------------------------
void usb_pipe_interrupt(void)
{
    U8 pipe_nb;
    U8 *ptr_buf;
    void  (*fct_handle)(U8 status,U16 nb_byte);
    U16 n;
    U8 i;
    U8 do_call_back=FALSE;

    TRACE_DEBUG("usb_pipe_interrupt\n\r");

    if(Host_get_pipe_interrupt()) {

        pipe_nb_save = Host_get_selected_pipe();       // Important! Save here working pipe number
        pipe_nb=host_get_nb_pipe_interrupt();  // work with the correct pipe number that generates the interrupt
        Host_select_pipe(pipe_nb);                        // Select this pipe
        fct_handle=*(it_pipe_str[pipe_nb].handle);

        // Now try to detect what event generate an interrupt...
        if (Is_host_pipe_error())             // Any error ?
        {
            TRACE_DEBUG("host_pipe_error\n\r");
            it_pipe_str[pipe_nb].status = Host_error_status();
            it_pipe_str[pipe_nb].enable=DISABLE;
            Host_stop_pipe_interrupt(pipe_nb);
            Host_ack_all_errors();
            do_call_back=TRUE;
            goto usb_pipe_interrupt_end;
        }

        if (Is_host_stall())                  // Stall handshake received ?
        {
            TRACE_DEBUG("host_stall\n\r");
            it_pipe_str[pipe_nb].status=PIPE_STALL;
            it_pipe_str[pipe_nb].enable=DISABLE;
            Host_stop_pipe_interrupt(pipe_nb);
            do_call_back=TRUE;
            goto usb_pipe_interrupt_end;
        }

#if (NAK_TIMEOUT_ENABLE==ENABLE)
        if (Is_host_nak_received())           // NAK ?
        {
            Host_ack_nak_received();
            // check if number of NAK timeout error occurs (not for interrupt type pipe)
            if((--it_pipe_str[pipe_nb].nak_timeout==0) && (Host_get_pipe_type()!=TYPE_INTERRUPT))
            {
                it_pipe_str[pipe_nb].status=PIPE_NAK_TIMEOUT;
                it_pipe_str[pipe_nb].enable=DISABLE;
                Host_stop_pipe_interrupt(pipe_nb);
                do_call_back=TRUE;
                goto usb_pipe_interrupt_end;
            }
        }
#endif

        if (Is_host_in_received())            // Pipe IN reception ?
        {
            TRACE_DEBUG("host_in received\n\r");
            ptr_buf =  it_pipe_str[pipe_nb].ptr_buf;
            ptr_buf += it_pipe_str[pipe_nb].nb_byte_processed;       // Build pointer to data buffer
            n  = it_pipe_str[pipe_nb].nb_byte_to_process;
            n -= it_pipe_str[pipe_nb].nb_byte_processed;  // Remaining data bytes
            Host_freeze_pipe();
            if (Host_byte_counter()<=n)
            {
                if ((Host_byte_counter() < n)&&(Host_byte_counter()<host_get_pipe_length())) //Received less than remaining, but less than pipe capacity
                {
                    n=0;
                }
                else
                {
                    n-=Host_byte_counter();
                }
                it_pipe_str[pipe_nb].nb_byte_processed += Host_byte_counter();  // Update nb of byte received

                Address_fifochar_endpoint(global_pipe_nb);
                for (i=Host_byte_counter();i;i--)
                { 
                    *ptr_buf = Host_read_byte();
                    ptr_buf++;
                }
            }
            else  // more bytes received than expected
            {     // TODO error code management
                it_pipe_str[pipe_nb].nb_byte_processed+=n;
                Address_fifochar_endpoint(global_pipe_nb);
                for ( i=n; i; i-- )                  // Byte number limited to the initial request (limit tab over pb)
                { 
                    *ptr_buf = Host_read_byte();
                    ptr_buf++;
                }
                n=0;
            }
            Host_ack_in_received();
            if(n>0) //still something to process
            {
                Host_unfreeze_pipe();            // Request another IN transfer
                Host_send_in();
                private_sof_counter=0;           // Reset the counter in SOF detection sub-routine
                it_pipe_str[pipe_nb].timeout=0;  // Reset timeout
                it_pipe_str[pipe_nb].nak_timeout=NAK_RECEIVE_TIMEOUT;
            }
            else //end of transfer
            {
                it_pipe_str[pipe_nb].enable=DISABLE;
                it_pipe_str[pipe_nb].status=PIPE_GOOD;
                Host_stop_pipe_interrupt(pipe_nb);
                do_call_back=TRUE;
            }
        }

        if(Is_host_out_sent())                  // Pipe OUT sent ?
        {
            TRACE_DEBUG("host_out send\n\r");
            Host_ack_out_sent();
            it_pipe_str[pipe_nb].nb_byte_processed+=it_pipe_str[pipe_nb].nb_byte_on_going;
            it_pipe_str[pipe_nb].nb_byte_on_going=0;
            ptr_buf  = it_pipe_str[pipe_nb].ptr_buf;
            ptr_buf += it_pipe_str[pipe_nb].nb_byte_processed;       // Build pointer to data buffer
            n  = it_pipe_str[pipe_nb].nb_byte_to_process;
            n -= it_pipe_str[pipe_nb].nb_byte_processed;  // Remaining data bytes
            if(n>0)   // Still data to process...
            {
                Host_unfreeze_pipe();
                // Prepare data to be sent
                i = host_get_pipe_length();
                if ( i > n)     // Pipe size> remaining data
                {
                    i = n;
                    n = 0;
                }
                else                // Pipe size < remaining data
                { 
                    n -= i; 
                }
                it_pipe_str[pipe_nb].nb_byte_on_going+=i;   // Update nb data processed
                Address_fifochar_endpoint(global_pipe_nb);
                while (i!=0)                     // Load Pipe buffer
                {
                    Host_write_byte(*ptr_buf++); i--;
                }
                private_sof_counter=0;           // Reset the counter in SOF detection sub-routine
                it_pipe_str[pipe_nb].timeout=0;  // Refresh timeout counter
                it_pipe_str[pipe_nb].nak_timeout=NAK_SEND_TIMEOUT;
                Host_send_out();                 // Send the USB frame
            }
            else                                //n==0 Transfer is finished
            {
                it_pipe_str[pipe_nb].enable=DISABLE;    // Tranfer end
                it_pipe_str[pipe_nb].status=PIPE_GOOD;  // Status OK
                Host_stop_pipe_interrupt(pipe_nb);
                do_call_back=TRUE;
            }
        }

usb_pipe_interrupt_end:
        Host_select_pipe(pipe_nb_save);   // Restore pipe number !!!!
        if (is_any_interrupt_pipe_active()==FALSE)    // If no more transfer is armed
        {
            if (g_sav_int_sof_enable==FALSE)
            {
                Host_disable_sof_interrupt();
            }
        }
        if(do_call_back)      // Any callback functions to perform ?
        {
            fct_handle(it_pipe_str[pipe_nb].status,it_pipe_str[pipe_nb].nb_byte_processed);
        }

    }

}



