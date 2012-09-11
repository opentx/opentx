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

/// This file manages the host enumeration process

//------------------------------------------------------------------------------
//      Headers
//------------------------------------------------------------------------------
#include <conf_usb.h>
#include "usb/otg/usb_drv.h"
#include "usb_host_enum.h"
#include "usb/otg/usb_task.h"
#include "usb_host_task.h"
#include <utility/trace.h>
#include <usb/common/core/USBGenericDescriptor.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------
#if (MAX_INTERFACE_SUPPORTED<1)
#error MAX_INTERFACE_SUPPORTED<1 : The host controller should support at least one interface...
#endif

#ifndef VID_PID_TABLE
   #error VID_PID_TABLE should be defined somewhere (conf_usb.h)
  //   VID_PID_TABLE format definition:
  //   #define VID_PID_TABLE      {VID1, number_of_pid_for_this_VID1, PID11_value,..., PID1X_Value 
  //                              ...
  //                              ,VIDz, number_of_pid_for_this_VIDz, PIDz1_value,..., PIDzX_Value}
#endif

#ifndef CLASS_SUBCLASS_PROTOCOL
   #error CLASS_SUBCLASS_PROTOCOL shoud be defined somewhere (conf_usb.h)
  //   CLASS_SUBCLASS_PROTOCOL format definition:
  //   #define CLASS_SUBCLASS_PROTOCOL  {CLASS1, SUB_CLASS1,PROTOCOL1, 
  //                                     ...
  //                                     CLASSz, SUB_CLASSz,PROTOCOLz}
#endif

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

//! Const table of known devices (see conf_usb.h for table content)
U16 registered_VID_PID[]   = VID_PID_TABLE;

//! Const table of known class (see conf_usb.h for table content)
U8  registered_class[]     = CLASS_SUBCLASS_PROTOCOL;

//! Physical EP to address device endpoints look-up table
//  This table is dynamically built with the "host_configure_endpoint_class" function
U8 ep_table[MAX_EP_NB]={0,0,0,0,0,0,0};

//! The number of interface the host is able to support in the device connected
U8 nb_interface_supported=0;

S_interface interface_supported[MAX_INTERFACE_SUPPORTED];

//! PID of device connected
U16 device_PID;
//! VID of device connected
U16 device_VID;
//! bmAttributes byte of the connected device
U8 bmattributes;
//! maxpower byte of the connected device (Caution, unit is 2mA)
U8 maxpower;


//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// checks if the VID and the PID are supported
/// (if the VID/PID belongs to the VID_PID table)
//------------------------------------------------------------------------------
U8 host_check_VID_PID(void)
{
    U8 nb_of_vid;
    U8 nb_of_pid;

    TRACE_DEBUG("host_check_VID_PID\n\r");
    // Rebuild VID PID from data stage
    LSB(device_VID) = data_stage[OFFSET_FIELD_LSB_VID];
    MSB(device_VID) = data_stage[OFFSET_FIELD_MSB_VID];
    LSB(device_PID) = data_stage[OFFSET_FIELD_LSB_PID];
    MSB(device_PID) = data_stage[OFFSET_FIELD_MSB_PID];

    // Compare detected VID PID with supported table
    nb_of_vid=0;
    while (nb_of_vid< sizeof(registered_VID_PID)/2)   // /2 because registered_VID_PID table is U16...
    {
        TRACE_DEBUG("registered_VID_PID[%d]: 0x%X\n\r", nb_of_vid, registered_VID_PID[nb_of_vid]);
        TRACE_DEBUG("device_VID: 0x%X\n\r", device_VID);
        if (registered_VID_PID[nb_of_vid] == device_VID)   // VID is correct
        {
            TRACE_DEBUG("Good VID\n\r");
            nb_of_pid = (U8)registered_VID_PID[nb_of_vid+1];    // store nb of PID for this VID
            while (nb_of_pid != 0)
            {
                TRACE_DEBUG("registered_VID_PID[%d]: 0x%X\n\r", nb_of_vid+nb_of_pid+1, registered_VID_PID[nb_of_vid+nb_of_pid+1]);
                TRACE_DEBUG("device_PID: 0x%X\n\r", device_PID);
                if (registered_VID_PID[nb_of_vid+nb_of_pid+1] == device_PID)
                {
                    TRACE_DEBUG("Good PID\n\r");
                    return HOST_TRUE;
                }
                nb_of_pid--;
            }
        }
        nb_of_vid+=registered_VID_PID[nb_of_vid+1]+2;
    }
    return HOST_FALSE;
}


//------------------------------------------------------------------------------
/// checks if the device class is supported.
/// The function looks in all interface declared in the received dewcriptors, if
/// one of them match with the CLASS/SUB_CLASS/PROTOCOL table
//------------------------------------------------------------------------------
U8 host_check_class(void)
{
    U8  class_table;
    T_DESC_OFFSET  descriptor_offset;
    T_DESC_OFFSET  conf_offset_end;
    U16  config_size;
    U8  device_class;
    U8  device_subclass;
    U8  device_protocol;

    TRACE_DEBUG("host_check_class\n\r");
    nb_interface_supported=0;   //First asumes ,no interface is supported!
    if (data_stage[OFFSET_FIELD_DESCRIPTOR_TYPE] != USBGenericDescriptor_CONFIGURATION)           // check if configuration descriptor
    { 
        TRACE_DEBUG("host_check_class problem\n\r");
        return HOST_FALSE;
    }
    LSB(config_size) = data_stage[OFFSET_FIELD_TOTAL_LENGHT];
    MSB(config_size) = data_stage[OFFSET_FIELD_TOTAL_LENGHT+1];
    bmattributes = data_stage[OFFSET_FIELD_BMATTRIBUTES];
    maxpower = data_stage[OFFSET_FIELD_MAXPOWER];
    descriptor_offset = 0;
    conf_offset_end = descriptor_offset + config_size;

    // Look in all interfaces declared in the configuration
    while(descriptor_offset < conf_offset_end)
    {
        // Find next interface descriptor
        while (data_stage[descriptor_offset+OFFSET_FIELD_DESCRIPTOR_TYPE] != USBGenericDescriptor_INTERFACE)
        {
            descriptor_offset += data_stage[descriptor_offset];
            if(descriptor_offset >= conf_offset_end)
            {
                if(nb_interface_supported)
                {
                    TRACE_DEBUG("host_check_class ok\n\r");
                    return HOST_TRUE;
                }
                else
                { 
                    TRACE_DEBUG("host_check_class problem2\n\r");
                    return HOST_FALSE;
                }
            }
        }
        // Found an interface descriptor
        // Get charateristics of this interface
        device_class    = data_stage[descriptor_offset + OFFSET_FIELD_CLASS];
        device_subclass = data_stage[descriptor_offset + OFFSET_FIELD_SUB_CLASS];
        device_protocol = data_stage[descriptor_offset + OFFSET_FIELD_PROTOCOL];
        // Look in registered class table for match
        class_table=0;
        while (class_table< sizeof(registered_class))
        {
            if (registered_class[class_table] == device_class)                 // class is correct!
            {
                if (registered_class[class_table+1] == device_subclass)         // sub class is correct!
                {
                    if (registered_class[class_table+2] == device_protocol)      // protocol is correct!
                    {
                        // Prepare for another item CLASS/SUB_CLASS/PROTOCOL in table
                        class_table+=3;
                        // Store this interface as supported interface
                        // Memorize its interface nb
                        interface_supported[nb_interface_supported].interface_nb=data_stage[descriptor_offset+OFFSET_FIELD_INTERFACE_NB];
                        //          its alternate setting
                        interface_supported[nb_interface_supported].altset_nb=data_stage[descriptor_offset+OFFSET_FIELD_ALT];
                        //          its USB class
                        interface_supported[nb_interface_supported].class=device_class;
                        //          its USB subclass
                        interface_supported[nb_interface_supported].subclass=device_subclass;
                        //          its USB protocol
                        interface_supported[nb_interface_supported].protocol=device_protocol;
                        //          the number of endpoints associated to this interface
                        //          Note: The associated endpoints addresses are stored during pipe attribution...
                        interface_supported[nb_interface_supported].nb_ep=data_stage[descriptor_offset+OFFSET_FIELS_NB_OF_EP];
                        // Update the number of interface supported
                        nb_interface_supported++;
                        // Check the maximum number of interfaces we can support
                        if(nb_interface_supported>=MAX_INTERFACE_SUPPORTED)
                        {
                            TRACE_DEBUG("host_check_class ok1\n\r");
                            return HOST_TRUE;
                        }
                    }
                }
            }
            class_table+=3; // Check other item CLASS/SUB_CLASS/PROTOCOL in table
        }
        descriptor_offset += data_stage[descriptor_offset]; // Next descriptor
        if(descriptor_offset > SIZEOF_DATA_STAGE)           // Check overflow
        {
            if(nb_interface_supported)
            {
                TRACE_DEBUG("host_check_class ok3\n\r");
                return HOST_TRUE;
            }
            else
            { 
                TRACE_DEBUG("host_check_class problem3\n\r");
                return HOST_FALSE;
            }
        }
    }
    if(nb_interface_supported)
    {
        TRACE_DEBUG("host_check_class ok4\n\r");
        return HOST_TRUE;
    }
    else
    { 
        TRACE_DEBUG("host_check_class problem4\n\r");
        return HOST_FALSE;
    }
}

//------------------------------------------------------------------------------
/// configures the pipe according to the device class of the
//------------------------------------------------------------------------------
U8 host_auto_configure_endpoint(void)
{
    U8  nb_endpoint_to_configure;
    T_DESC_OFFSET  descriptor_offset;
    U8  physical_pipe=1;   // =1 cause lookup table assumes that physiacl pipe 0 is reserved for control
    U8 i;
    U8 ep_index;

    TRACE_DEBUG("host_auto_configure_endpoint\n\r");
    // For all interfaces to configure...
    for(i=0;i<nb_interface_supported;i++)
    {
        ep_index=0;
        // First look for the target interface descriptor offset
        descriptor_offset = get_interface_descriptor_offset(interface_supported[i].interface_nb,interface_supported[i].altset_nb);
        // Get the number of endpoint to configure for this interface
        nb_endpoint_to_configure = data_stage[descriptor_offset+OFFSET_FIELS_NB_OF_EP];
        // Get the first Endpoint descriptor offset to configure
        descriptor_offset += data_stage[descriptor_offset+OFFSET_DESCRIPTOR_LENGHT];  // pointing on endpoint descriptor

        // While there is at least one pipe to configure
        while (nb_endpoint_to_configure)
        {
            // Check and look for an Endpoint descriptor
            while (data_stage[descriptor_offset+OFFSET_FIELD_DESCRIPTOR_TYPE] != USBGenericDescriptor_ENDPOINT)
            {
                descriptor_offset += data_stage[descriptor_offset];
                if(descriptor_offset > SIZEOF_DATA_STAGE)   // No more endpoint descriptor found -> Errror !
                { 
                    TRACE_DEBUG("PB\n\r");
                    return HOST_FALSE;
                }
            }

            // Select the new physical pipe to configure and get ride of any previous configuration for this physical pipe
            Host_select_pipe(physical_pipe);
            Host_disable_pipe();
            Host_enable_pipe();

            // Build the pipe configuration according to the endpoint descriptors fields received
            //
            // host_configure_pipe(
            //    physical_pipe,                                                                    // pipe nb in USB interface
            //    data_stage[descriptor_offset+OFFSET_FIELD_EP_TYPE],                               // pipe type (interrupt/BULK/ISO)
            //    Get_pipe_token(data_stage[descriptor_offset+OFFSET_FIELD_EP_ADDR]),               // pipe addr
            //    (data_stage[descriptor_offset+2] & MSK_EP_DIR),                                   // pipe dir (IN/OUT)
            //    host_determine_pipe_size((U16)data_stage[descriptor_offset+OFFSET_FIELD_EP_SIZE]),// pipe size
            //    ONE_BANK,                                                                         // bumber of bank to allocate for pipe
            //    data_stage[descriptor_offset+OFFSET_FIELD_EP_INTERVAL]                            // interrupt period (for interrupt pipe)
            //  );

            host_configure_pipe(
                    physical_pipe,
                    data_stage[descriptor_offset+OFFSET_FIELD_EP_TYPE],
                    Get_pipe_token(data_stage[descriptor_offset+OFFSET_FIELD_EP_ADDR]),
                    (data_stage[descriptor_offset+OFFSET_FIELD_EP_ADDR] & MSK_EP_DIR),
                    host_determine_pipe_size(  
                        (U16)data_stage[descriptor_offset+OFFSET_FIELD_EP_SIZE_LOW]
                        + (((U16)data_stage[descriptor_offset+OFFSET_FIELD_EP_SIZE_HIGH])<<8)),
                    ONE_BANK,
                    data_stage[descriptor_offset+OFFSET_FIELD_EP_INTERVAL] );

            host_configure_address(physical_pipe, DEVICE_ADDRESS);

            // Update Physical Pipe lookup table with device enpoint address
            ep_table[physical_pipe]=data_stage[descriptor_offset+OFFSET_FIELD_EP_ADDR];
            physical_pipe++;
            // Update endpoint addr table in supported interface structure
            interface_supported[i].ep_addr[ep_index++]=data_stage[descriptor_offset+OFFSET_FIELD_EP_ADDR];
            descriptor_offset += data_stage[descriptor_offset];             // pointing on next descriptor

            // All target endpoints configured ?
            nb_endpoint_to_configure--;
        } //for(i=0;i<nb_interface_supported;i++)
    }
    Host_set_configured();
    return HOST_TRUE;
}

//------------------------------------------------------------------------------
/// returns the offset in data_stage where to find the interface descriptor
/// whose number and alternate setting values are passed as parameters
//------------------------------------------------------------------------------
T_DESC_OFFSET get_interface_descriptor_offset(U8 interface, U8 alt)
{
    T_DESC_OFFSET descriptor_offset;

    TRACE_DEBUG("get_interface_descriptor_offset\n\r");
    //nb_interface = data_stage[OFFSET_FIELD_NB_INTERFACE];      // Detects the number of interfaces in this configuration
    descriptor_offset = data_stage[OFFSET_DESCRIPTOR_LENGHT];  // now pointing on next descriptor

    while(descriptor_offset < SIZEOF_DATA_STAGE)            // Look in all interfaces declared in the configuration
    {
        while (data_stage[descriptor_offset+OFFSET_FIELD_DESCRIPTOR_TYPE] != USBGenericDescriptor_INTERFACE)
        {
            descriptor_offset += data_stage[descriptor_offset];
            if(descriptor_offset > SIZEOF_DATA_STAGE)
            {  
                return HOST_FALSE;
            }
        }
        if (data_stage[descriptor_offset+OFFSET_FIELD_INTERFACE_NB]==interface
        && data_stage[descriptor_offset+OFFSET_FIELD_ALT]==alt)
        {
            return  descriptor_offset;
        }
        descriptor_offset += data_stage[descriptor_offset];
    }
    return descriptor_offset;
}

//------------------------------------------------------------------------------
/// returns the physical pipe number linked to a logical endpoint address.
//------------------------------------------------------------------------------
U8 host_get_hwd_pipe_nb(U8 ep_addr)
{
    U8 i;

    for(i=0; i<MAX_EP_NB; i++)
    {
        if(ep_table[i] == ep_addr)
        { 
            return i;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/// generic Pipe 0 management function
/// used to send and receive control request over pipe 0
//------------------------------------------------------------------------------
U8 host_send_control(U8* data_pointer)
{
    U16  data_length;
    U8   sav_int_sof_enable;
    U16  status;
    int  *pFifoInt;

    Usb_ack_event(EVT_HOST_SOF);
    sav_int_sof_enable=Is_host_sof_interrupt_enabled();
    Host_enable_sof_interrupt();                   // SOF software detection is in interrupt sub-routine
    while(Is_not_usb_event(EVT_HOST_SOF))          // Wait 1 sof
    {
        if (Is_host_emergency_exit())
        {
            status=CONTROL_TIMEOUT;
            TRACE_DEBUG("Is_host_emergency_exit\n\r");
            Host_freeze_pipe();
            Host_reset_pipe(0);
            goto host_send_control_end;
        }
    }
    if (sav_int_sof_enable==FALSE)
    {
        Host_disable_sof_interrupt();  // enter in suspend mode
    }

    Host_select_pipe(0);
    Host_set_token_setup();
    Host_ack_setup();
    Host_unfreeze_pipe();

    // Send the setup request fields
    pFifoInt = (int*)&usb_request;
    Host_write_32(*pFifoInt);
    pFifoInt++;
    Host_write_32(*pFifoInt);

    Host_send_setup();

    while(Is_host_setup_sent() == FALSE)  // wait for SETUP ack
    {
        if (Is_host_emergency_exit())
        {
            status = CONTROL_TIMEOUT;
            Host_freeze_pipe();
            Host_reset_pipe(0);
            goto host_send_control_end;
        }
        if(Is_host_pipe_error())           // Any error ?
        {
            status = Host_error_status();
            Host_ack_all_errors();
            goto host_send_control_end;     // Send error status
        }
    }

    // Setup token sent now send In or OUT token
    // Before just wait one SOF
    Usb_ack_event(EVT_HOST_SOF);
    sav_int_sof_enable=Is_host_sof_interrupt_enabled();
    Host_enable_sof_interrupt();
    Host_freeze_pipe();
    data_length = usb_request.wLength;
    while(Is_not_usb_event(EVT_HOST_SOF))         // Wait 1 sof
    {
        if (Is_host_emergency_exit())
        {
            status=CONTROL_TIMEOUT;
            Host_freeze_pipe();
            Host_reset_pipe(0);
            goto host_send_control_end;
        }
   }
    if (sav_int_sof_enable==FALSE)
    {
        Host_disable_sof_interrupt();
    }   // Restore SOF interrupt enable

    // IN request management ---------------------------------------------
    if(usb_request.bmRequestType & 0x80)           // bmRequestType : Data stage IN (bmRequestType==1)
    {
        Host_standard_in_mode();
        //jcb // We send 1 IN 
        //    Host_in_request_number(0, 0);
        Host_set_token_in();
        while(data_length != 0)
        {
            Host_unfreeze_pipe();
            while(!Is_host_control_in_received())
            {
                if (Is_host_emergency_exit())
                {
                    status=CONTROL_TIMEOUT;
                    Host_freeze_pipe();
                    Host_reset_pipe(0);
                    goto host_send_control_end;
                }
                if(Is_host_pipe_error())
                {
                    status = Host_error_status();
                    Host_ack_all_errors();
                    goto host_send_control_end;
                }
                if(Is_host_stall())
                {
                    status=CONTROL_STALL;
                    Host_ack_stall();
                    goto host_send_control_end;
                }
            }
            status = Host_data_length_U8();
            if (status == host_get_pipe_length())
            {
                data_length -= status;
                if (usb_request.uncomplete_read == TRUE)           // uncomplete_read
                {
                    data_length = 0;
                }
            }
            else
            {
                data_length = 0;
            }
            Address_fifochar_endpoint(0);

            while (status!=0)
            {
                *data_pointer = Host_read_byte();
                data_pointer++;
                status--;
            }
            Host_freeze_pipe();
            Host_ack_control_in();
            Host_send_control_in();
            //jcb
            //         if( 0 != data_length )
            //         {
            //            Host_unfreeze_pipe();
            //         }
        }                                // end of IN data stage
        Host_set_token_out();
        Host_unfreeze_pipe();
        Host_ack_control_out();
        Host_send_control_out();
      
        while(!Is_host_control_out_sent())
        {
            if (Is_host_emergency_exit())
            {
                status=CONTROL_TIMEOUT;
                Host_freeze_pipe();
                Host_reset_pipe(0);
                goto host_send_control_end;
            }
            if(Is_host_pipe_error())
            {
                status = Host_error_status();
                Host_ack_all_errors();
                goto host_send_control_end;
            }
            if(Is_host_stall())
            {
                status=CONTROL_STALL;
                Host_ack_stall();
                goto host_send_control_end;
            }
        }

        // Test Low Speed
        if( AT91C_OTGHS_SPEED_SR_LS == (*AT91C_OTGHS_SR & AT91C_OTGHS_SPEED_SR_LS)) {
            TRACE_DEBUG("LOW SPEED\n\r");
            // Reset UTMI
            *AT91C_OTGHS_TSTA2 |= AT91C_OTGHS_UTMIRESET;
            *AT91C_OTGHS_TSTA2 &= ~(unsigned int)AT91C_OTGHS_UTMIRESET;
        }
        Host_ack_control_out();
        // Wait end of transfer
        while( (AT91C_BASE_OTGHS->OTGHS_HSTPIPISR[0] & AT91C_OTGHS_TXSTPI) != 0);
        // Freeze the pipe
        Host_freeze_pipe();

        status=(CONTROL_GOOD);
        goto host_send_control_end;
    }

    // OUT request management ---------------------------------------------
    else                                 // Data stage OUT (bmRequestType==0)
    {
        Host_set_token_out();
        Host_ack_control_out();
        while(data_length != 0)
        {
            Host_unfreeze_pipe();
            status = host_get_pipe_length();
            if ( (U16)status > data_length)
            {
                status = (U8)data_length;
                data_length = 0;
            }
            else
            {
                data_length -= status;
            }
            Address_fifochar_endpoint(global_pipe_nb);
            while (status!=0)
            {
                Host_write_byte(*data_pointer);
                data_pointer++;
                status--;
            }
            Host_send_control_out();
            while (!Is_host_control_out_sent())
            {
                if (Is_host_emergency_exit())
                {
                    status=CONTROL_TIMEOUT;
                    Host_freeze_pipe();
                    Host_reset_pipe(0);
                    goto host_send_control_end;
                }
                if(Is_host_pipe_error())
                {
                    status = Host_error_status();
                    Host_ack_all_errors();
                    goto host_send_control_end;
                }
                if(Is_host_stall())
                {
                    status=CONTROL_STALL;
                    Host_ack_stall();
                    goto host_send_control_end;
                }
            }
            Host_ack_control_out();
        }                                // end of OUT data stage
        Host_freeze_pipe();
        Host_set_token_in();
        Host_unfreeze_pipe();
        while(!Is_host_control_in_received())
        {
            if (Is_host_emergency_exit())
            {
                status=CONTROL_TIMEOUT;
                Host_freeze_pipe();
                Host_reset_pipe(0);
                goto host_send_control_end;
            }
            if(Is_host_pipe_error())
            {
                status = Host_error_status();
                Host_ack_all_errors();
                goto host_send_control_end;
            }
            if(Is_host_stall())
            {
                status=CONTROL_STALL;
                Host_ack_stall();
                goto host_send_control_end;
            }
        }
        Host_ack_control_in();
        Host_freeze_pipe();
        Host_send_control_in();
        status=(CONTROL_GOOD);
        goto host_send_control_end;
    }

host_send_control_end:
    return ((U8)status);
}


