/* 
 crt0.S for LPC2xxx
 - based on examples from R O Software
 - based on examples from newlib-lpc
 - based on an example from Anglia Designs

 collected and modified by Martin Thomas
*/

        .global _etext                  // -> .data initial values in ROM
        .global _data                   // -> .data area in RAM
        .global _edata                  // end of .data area
        .global __bss_start             // -> .bss area in RAM
        .global __bss_end__             // end of .bss area
        .global _stack                  // top of stack

// Stack Sizes
        .set  UND_STACK_SIZE, 0x00000004
        .set  ABT_STACK_SIZE, 0x00000004
        .set  FIQ_STACK_SIZE, 0x00000004
        .set  IRQ_STACK_SIZE, 0X00000080
        .set  SVC_STACK_SIZE, 0x00000004

// Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs
        .set  MODE_USR, 0x10            // User Mode
        .set  MODE_FIQ, 0x11            // FIQ Mode
        .set  MODE_IRQ, 0x12            // IRQ Mode
        .set  MODE_SVC, 0x13            // Supervisor Mode
        .set  MODE_ABT, 0x17            // Abort Mode
        .set  MODE_UND, 0x1B            // Undefined Mode
        .set  MODE_SYS, 0x1F            // System Mode

        .equ  I_BIT, 0x80               // when I bit is set, IRQ is disabled
        .equ  F_BIT, 0x40               // when F bit is set, FIQ is disabled

        .text
	.arm
	.section .init, "ax"

        .code 32
        .align 2

        .global _boot
        .func   _boot
_boot:

// Runtime Interrupt Vectors
// -------------------------
Vectors:
        b     _start                    // reset - _start
        ldr   pc,_undf                  // undefined - _undf
        ldr   pc,_swi                   // SWI - _swi
        ldr   pc,_pabt                  // program abort - _pabt
        ldr   pc,_dabt                  // data abort - _dabt
        nop                             // reserved
        ldr   pc,[pc,#-0xFF0]           // IRQ - read the VIC
        ldr   pc,_fiq                   // FIQ - _fiq

#if 0
// Use this group for production
_undf:  .word _reset                    // undefined - _reset
_swi:   .word _reset                    // SWI - _reset
_pabt:  .word _reset                    // program abort - _reset
_dabt:  .word _reset                    // data abort - _reset
_irq:   .word _reset                    // IRQ - _reset
_fiq:   .word _reset                    // FIQ - _reset

#else
// Use this group for development
_undf:  .word __undf                    // undefined
_swi:   .word __swi                     // SWI
_pabt:  .word __pabt                    // program abort
_dabt:  .word __dabt                    // data abort
_irq:   .word __irq                     // IRQ
_fiq:   .word __fiq                     // FIQ

__undf: b     .                         // undefined
__swi:  b     .                         // SWI
__pabt: b     .                         // program abort
__dabt: b     .                         // data abort
__irq:  b     .                         // IRQ
__fiq:  b     .                         // FIQ
#endif
        .size _boot, . - _boot
        .endfunc


// Setup the operating mode & stack.
// ---------------------------------
        .global _start, start, _mainCRTStartup
        .func   _start

_start:
start:
_mainCRTStartup:

// Initialize Interrupt System
// - Set stack location for each mode
// - Leave in System Mode with Interrupts Disabled
// -----------------------------------------------
        ldr   r0,=_stack
        msr   CPSR_c,#MODE_UND|I_BIT|F_BIT // Undefined Instruction Mode
        mov   sp,r0
        sub   r0,r0,#UND_STACK_SIZE
        msr   CPSR_c,#MODE_ABT|I_BIT|F_BIT // Abort Mode
        mov   sp,r0
        sub   r0,r0,#ABT_STACK_SIZE
        msr   CPSR_c,#MODE_FIQ|I_BIT|F_BIT // FIQ Mode
        mov   sp,r0
        sub   r0,r0,#FIQ_STACK_SIZE
        msr   CPSR_c,#MODE_IRQ|I_BIT|F_BIT // IRQ Mode
        mov   sp,r0
        sub   r0,r0,#IRQ_STACK_SIZE
        msr   CPSR_c,#MODE_SVC|I_BIT|F_BIT // Supervisor Mode
        mov   sp,r0
        sub   r0,r0,#SVC_STACK_SIZE
        msr   CPSR_c,#MODE_SYS|I_BIT|F_BIT // System Mode
        mov   sp,r0

// Copy initialized data to its execution address in RAM
// -----------------------------------------------------
#ifdef ROM_RUN
        ldr   r1,=_etext                // -> ROM data start
        ldr   r2,=_data                 // -> data start
        ldr   r3,=_edata                // -> end of data
1:      cmp   r2,r3                     // check if data to move
        ldrlo r0,[r1],#4                // copy it
        strlo r0,[r2],#4
        blo   1b                        // loop until done
#endif
// Clear .bss
// ----------
        mov   r0,#0                     // get a zero
        ldr   r1,=__bss_start           // -> bss start
        ldr   r2,=__bss_end__           // -> bss end
2:      cmp   r1,r2                     // check if data to clear
        strlo r0,[r1],#4                // clear 4 bytes
        blo   2b                        // loop until done
		
/*
   Call C++ constructors (for objects in "global scope")
   ctor loop added by Martin Thomas 4/2005 
   based on a Anglia Design example-application for ST ARM
*/

		LDR 	r0, =__ctors_start__
		LDR 	r1, =__ctors_end__
ctor_loop:
		CMP 	r0, r1
		BEQ 	ctor_end
		LDR 	r2, [r0], #4
		STMFD 	sp!, {r0-r1}
		MOV 	lr, pc
		MOV 	pc, r2
		LDMFD 	sp!, {r0-r1}
		B 		ctor_loop
ctor_end:

// Call main program: main(0)
// --------------------------
        mov   r0,#0                     // no arguments (argc = 0)
        mov   r1,r0
        mov   r2,r0
        mov   fp,r0                     // null frame pointer
        mov   r7,r0                     // null frame pointer for thumb
        ldr   r10,=main
        mov   lr,pc

/* Enter the C code, use BX instruction so as to never return */
/* use BLX (?) main if you want to use c++ destructors below */

        bx    r10                       // enter main()

/* "global object"-dtors are never called and it should not be 
   needed since there is no OS to exit to. */
/* Call destructors */
#		LDR		r0, =__dtors_start__
#		LDR		r1, =__dtors_end__
dtor_loop:
#		CMP		r0, r1
#		BEQ		dtor_end
#		LDR		r2, [r0], #4
#		STMFD	sp!, {r0-r1}
#		MOV		lr, pc
#		MOV		pc, r2
#		LDMFD	sp!, {r0-r1}
#		B		dtor_loop
dtor_end:
   
        .size   _start, . - _start
        .endfunc

        .global _reset, reset, exit, abort
        .func   _reset
_reset:
reset:
exit:
abort:
#if 0
// Disable interrupts, then force a hardware reset by driving P23 low
// -------------------------------------------------------------------
        mrs   r0,cpsr                   // get PSR
        orr   r0,r0,#I_BIT|F_BIT        // disable IRQ and FIQ
        msr   cpsr,r0                   // set up status register

        ldr   r1,=(PS_BASE)             // PS Base Address
        ldr   r0,=(PS_PIO)              // PIO Module
        str   r0,[r1,#PS_PCER_OFF]      // enable its clock
        ldr   r1,=(PIO_BASE)            // PIO Base Address
        ldr   r0,=(1<<23)               // P23
        str   r0,[r1,#PIO_PER_OFF]      // make sure pin is contolled by PIO
        str   r0,[r1,#PIO_CODR_OFF]     // set the pin low
        str   r0,[r1,#PIO_OER_OFF]      // make it an output
#endif
        b     .                         // loop until reset

        .size _reset, . - _reset
        .endfunc

        .end
