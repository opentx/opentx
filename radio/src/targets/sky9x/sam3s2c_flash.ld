/****************************************************************************
*  Copyright (c) 2011 by Michael Fischer. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*  
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may 
*     be used to endorse or promote products derived from this software 
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
*
*****************************************************************************
*  History:
*
*  22.05.2011  mifi  First Version
****************************************************************************/


/*
 * In this linker script there is no heap available.
 * The stack start at the end of the ram segment.
 */


/*
 * Take a look in the "The GNU linker" manual, here you get
 * the following information about the "MEMORY":
 *
 * "The MEMORY command describes the location and size of 
 * blocks of memory in the target."
 */
MEMORY
{
   FLASH (rx)  : ORIGIN = 0x00400000, LENGTH = 128K
   RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
}


/*
 * And the "SECTION" is used for:
 *
 * "The SECTIONS command tells the linker how to map input
 * sections into output sections, and how to place the output
 * sections in memory.
 */
SECTIONS
{

   /*
    * The ".text" section is used for the code, and
    * read only (.rodata) data. Even the vectors (.vectors)
    * MUST be saved at the start of this section.
    */
   .text :
   {
      _stext = .;          /* Provide the name for the start of this section */
      
      CREATE_OBJECT_SYMBOLS
      KEEP(*(.vectors))
      
      *(.text)
      *(.text.*)
 
      . = ALIGN(4);        /* Align the start of the exidx part */
      *(.ARM.exidx)
      *(.ARM.exidx.*)

      . = ALIGN(4);        /* Align the start of the rodata part */
      *(.rodata)
      *(.rodata.*)

      KEEP (*(.init))
      KEEP (*(.fini))
      
      . = ALIGN(4);        /* Align the end of the section */
   } > FLASH = 0
   _etext = .;             /* Provide the name for the end of this section */
   
   
   /*
    * The ".data" section is used for initialized data
    * and for functions (.fastrun) which should be copied 
    * from flash to ram. This functions will later be
    * executed from ram instead of flash.
    */
   .data : AT (_etext)
   {
      . = ALIGN(4);        /* Align the start of the section */
      _sdata = .;          /* Provide the name for the start of this section */
      
      *(.data)
      *(.data.*)
      
      . = ALIGN(4);        /* Align the start of the fastrun part */
      *(.fastrun)
      *(.fastrun.*)
      
      . = ALIGN(4);        /* Align the end of the section */
   } > RAM
   _edata = .;             /* Provide the name for the end of this section */
   

   /*
    * The ".bss" section is used for uninitialized data.
    * This section will be cleared by the startup code.
    */
   .bss :
   {
      . = ALIGN(4);        /* Align the start of the section */
      _sbss = .;           /* Provide the name for the start of this section */
      
      *(.bss)
      *(.bss.*)
      
      . = ALIGN(4);        /* Align the end of the section */
   } > RAM
   _ebss = .;              /* Provide the name for the end of this section */  
   
   . = ALIGN(4);           /* Align the end of the section */
   __end = .;
   _end = .;
   
   /* 
    * The ".stack" section is our stack.
    * Here this section starts at the end of the ram segment.
    */
   _estack = ORIGIN(RAM) + LENGTH(RAM);

}

/*** EOF **/

