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
//         Headers
//------------------------------------------------------------------------------

#include <conf_usb.h>
#include "usb_drv.h"
#include <utility/trace.h>

U8 global_pipe_nb=0;
U8 global_endpoint_nb=0;
unsigned char dBytes=0;
char* pFifo;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//!  This function configures an endpoint with the selected type.
U8 dev_configure_endpoint(U8 ept, U8 type, U8 dir, U8 size, U8 bank, U8 nyet)
{
    unsigned char status=0;

    // Enable endpoint
    AT91C_BASE_OTGHS->OTGHS_DEVEPT |= (1<<ept);

    AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[ept] = (bank<<2)|(size<<4)|(dir<<8)
                                           |AT91C_OTGHS_AUTOSW |(type<<11);

    AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[ept] |= AT91C_OTGHS_ALLOC;
    if(!(AT91C_BASE_OTGHS->OTGHS_DEVEPTISR[ept]&AT91C_OTGHS_CFGOK)) {
      TRACE_ERROR("Bad endpoint configuration\n\r");
      status = 1;
    }
    if(nyet == NYET_ENABLED) {
        AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[ept] = AT91C_OTGHS_NYETDIS;
    }
    else {
        AT91C_BASE_OTGHS->OTGHS_DEVEPTIER[ept] = AT91C_OTGHS_NYETDIS;
    }
    return status;
}




//------------------------------------------------------------------------------
// HOST
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//!  This function configures a pipe with the selected type.
//! @param config0
//! @param config1
//! @return Is_endpoint_configured.
//------------------------------------------------------------------------------
U8 host_config_pipe(U8 config0, U8 config1)
{
    TRACE_DEBUG("host_config_pipe\n\r");
    Host_enable_pipe();
//    UPCFG0X = config0;
//    UPCFG1X = config1;
    Host_allocate_memory();
    return (Is_pipe_configured());
}

//------------------------------------------------------------------------------
//!  This function returns the size configuration register value according
//!  to the endpint size detected inthe device enumeration process.
//! @return pipe size register value.
//------------------------------------------------------------------------------
U8 host_determine_pipe_size(U16 size)
{
    //TRACE_DEBUG("host_determine_pipe_size\n\r");
         if(size <= 8  ) {return (SIZE_8   );}
    else if(size <= 16 ) {return (SIZE_16  );}
    else if(size <= 32 ) {return (SIZE_32  );}
    else if(size <= 64 ) {return (SIZE_64  );}
    else if(size <= 128) {return (SIZE_128 );}
    else if(size <= 256) {return (SIZE_256 );}
    else if(size <= 512) {return (SIZE_512 );}
    else                 {return (SIZE_1024);}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
U16 host_get_pipe_length(void)
{
    unsigned int size;

    size = (AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[global_pipe_nb]&AT91C_OTGHS_PSIZE)>>4;
    //TRACE_DEBUG("host_get_pipe_length = %d\n\r", (int)size);

         if(size == SIZE_8  ) {return (8  );}
    else if(size == SIZE_16 ) {return (16 );}
    else if(size == SIZE_32 ) {return (32 );}
    else if(size == SIZE_64 ) {return (64 );}
    else if(size == SIZE_128) {return (128);}
    else if(size == SIZE_256) {return (256);}
    else if(size == SIZE_512) {return (512);}
    else if(size == SIZE_1024){return (1024);}
    else {TRACE_ERROR("Error size\n\r");return 0;}
}


//------------------------------------------------------------------------------
//! host_disable_all_pipe.
//!
//!  This function disable all pipes for the host controller
//!  Usefull to execute upon device disconnection.
//!
//! @return none.
//------------------------------------------------------------------------------
void host_disable_all_pipe(void)
{
    U8 i;

    TRACE_DEBUG("host_disable_all_pipe\n\r");
    for (i=0; i<7; i++) {
        Host_reset_pipe(i);
        Host_select_pipe(i);
        Host_unallocate_memory();
        Host_disable_pipe();
    }
}

//------------------------------------------------------------------------------
//! @brief Returns the pipe number that generates a USB communication interrupt
//!
//! This function sould be called only when an interrupt has been detected. Otherwize
//! the return value is incorect
//!
//! @param none
//!
//! @return pipe_number
//------------------------------------------------------------------------------
U8 host_get_nb_pipe_interrupt(void)
{
   U8 interrupt_flags;
   U8 i;

   TRACE_DEBUG("host_get_nb_pipe_interrupt\n\r");
   interrupt_flags = Host_get_pipe_interrupt();
   for(i=0; i<MAX_EP_NB; i++) {
      if (interrupt_flags & (1<<i)) {
         return (i);
      }
   }
   // This return should never occurs ....
   TRACE_ERROR("Error\n\r");
   return MAX_EP_NB+1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void host_configure_address( unsigned char pipe, unsigned char addr)
{
    if( pipe == 0 ) {
        AT91C_BASE_OTGHS->OTGHS_HSTADDR1 &= ~(unsigned int)AT91C_OTGHS_UHADDR_P0;
        AT91C_BASE_OTGHS->OTGHS_HSTADDR1 |= ((addr<<0) & AT91C_OTGHS_UHADDR_P0);
    }
    else if( pipe == 1 ) {
        AT91C_BASE_OTGHS->OTGHS_HSTADDR1 &= ~(unsigned int)AT91C_OTGHS_UHADDR_P1;
        AT91C_BASE_OTGHS->OTGHS_HSTADDR1 |= ((addr<<8) & AT91C_OTGHS_UHADDR_P1);
    }
    else if( pipe == 2 ) {
        AT91C_BASE_OTGHS->OTGHS_HSTADDR1 &= ~(unsigned int)AT91C_OTGHS_UHADDR_P2;
        AT91C_BASE_OTGHS->OTGHS_HSTADDR1 |= ((addr<<16) & AT91C_OTGHS_UHADDR_P2);
    }
    else if( pipe == 3 ) {
        AT91C_BASE_OTGHS->OTGHS_HSTADDR1 &= ~(unsigned int)AT91C_OTGHS_UHADDR_P3;
        AT91C_BASE_OTGHS->OTGHS_HSTADDR1 |= ((addr<<24) & AT91C_OTGHS_UHADDR_P3);
    }
    else if( pipe == 4 ) {
        AT91C_BASE_OTGHS->OTGHS_HSTADDR2 &= ~(unsigned int)AT91C_OTGHS_UHADDR_P4;
        AT91C_BASE_OTGHS->OTGHS_HSTADDR2 |= ((addr<<0) & AT91C_OTGHS_UHADDR_P4);
    }
    else if( pipe == 5 ) {
        AT91C_BASE_OTGHS->OTGHS_HSTADDR2 &= ~(unsigned int)AT91C_OTGHS_UHADDR_P5;
        AT91C_BASE_OTGHS->OTGHS_HSTADDR2 |= ((addr<<8) & AT91C_OTGHS_UHADDR_P5);
    }
    else if( pipe == 6 ) {
        AT91C_BASE_OTGHS->OTGHS_HSTADDR2 &= ~(unsigned int)AT91C_OTGHS_UHADDR_P6;
        AT91C_BASE_OTGHS->OTGHS_HSTADDR2 |= ((addr<<16) & AT91C_OTGHS_UHADDR_P6);
    }
    else if( pipe == 7 ) {
        AT91C_BASE_OTGHS->OTGHS_HSTADDR2 &= ~(unsigned int)AT91C_OTGHS_UHADDR_P7;
        AT91C_BASE_OTGHS->OTGHS_HSTADDR2 |= ((addr<<24) & AT91C_OTGHS_UHADDR_P7);
    }
    else if( pipe == 8 ) {
        AT91C_BASE_OTGHS->OTGHS_HSTADDR3 &= ~(unsigned int)AT91C_OTGHS_UHADDR_P8;
        AT91C_BASE_OTGHS->OTGHS_HSTADDR3 |= ((addr<<0) & AT91C_OTGHS_UHADDR_P8);
    }
    else if( pipe == 9 ) {
        AT91C_BASE_OTGHS->OTGHS_HSTADDR3 &= ~(unsigned int)AT91C_OTGHS_UHADDR_P9;
        AT91C_BASE_OTGHS->OTGHS_HSTADDR3 |= ((addr<<8) & AT91C_OTGHS_UHADDR_P9);
    }
    else {
        TRACE_ERROR("pipe not defined\n\r");
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void host_configure_pipe(U8 pip, U8 type, U8 token, U8 ep_num, U8 size, U8 bank, U8 freq)
{
    AT91C_BASE_OTGHS->OTGHS_HSTPIP |= (1<<pip);

    if((type & AT91C_OTGHS_PTYPE_INT_PIP)==0) {
        freq =0;
    }

    AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[pip] = (bank<< 2) | (size << 4) | (token<< 8)
                                           | (type<<12) |(ep_num<<16)|  (freq <<24);
    AT91C_BASE_OTGHS->OTGHS_HSTPIPCFG[pip] |= AT91C_OTGHS_ALLOC;
}


