/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"

#if defined(PCBSKY9X)
  #include "AT91SAM3S4.h"
#else
  #include "targets/taranis/board.h"
#endif

#if defined(STM32)
void bwdt_reset()
{
  IWDG->KR = 0xAAAA; // reload
}

// TODO needed?
__attribute__ ((section(".bootrodata"), used))
void _bootStart(void);

__attribute__ ((section(".isr_boot_vector"), used))
const uint32_t BootVectors[] = {
  (uint32_t) &_estack,
  (uint32_t) (void (*)(void)) ((unsigned long) &_bootStart) };

__attribute__ ((section(".bootrodata.*"), used))
#elif defined(PCBSKY9X)
__attribute__ ((section(".bootrodata"), used))
#endif

const uint8_t BootCode[] = {
#include "bootloader.lbm"
};

#if defined(STM32)
__attribute__ ((section(".bootrodata"), used))
void _bootStart()
{
#if defined(PCBX9E)
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIODEN;
#else
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIODEN;
#endif

  // these two NOPs are needed (see STM32F errata sheet) before the peripheral 
  // register can be written after the peripheral clock was enabled
  __ASM volatile ("nop");
  __ASM volatile ("nop");

  // Turn soft power ON now, but only if we got started because of the watchdog
  // or software reset. If the radio was started by user pressing the power button
  // then that button is providing power and we don't need to enable it here.
  //
  // If we were to turn it on here indiscriminately, then the radio can go into the 
  // power on/off loop after being powered off by the user. (issue #2790)
  if (WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()) {
    GPIOD->BSRRL = 1;                                  // set PWR_ON_GPIO_PIN pin to 1
    GPIOD->MODER = (GPIOD->MODER & 0xFFFFFFFC) | 1;    // General purpose output mode
  }

  // TRIMS_GPIO_PIN_LHR is on PG0 on X9E and on PE3 on Taranis
  // TRIMS_GPIO_PIN_RHL is on PC1 on all versions
  // turn on pull-ups on trim keys 
  GPIOC->PUPDR = 0x00000004;
#if defined(PCBX9E)
  GPIOG->PUPDR = 0x00000001;
#else
  GPIOE->PUPDR = 0x00000040;
#endif

  // wait for inputs to stabilize
  for (uint32_t i = 0; i < 50000; i += 1) {
    bwdt_reset();
  }

  // now the second part of power on sequence
  // If we got here and the radio was not started by the watchdog/software reset,
  // then we must have a power button pressed. If not then we are in power on/off loop
  // and to terminate it, just wait here without turning on PWR pin. The power supply will
  // eventually exhaust and the radio will turn off.
  if (!WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()) {
    // wait here until the power key is pressed
    while (GPIOD->IDR & PWR_SWITCH_GPIO_PIN) {
      bwdt_reset();
    }
  }

  if (!(TRIMS_GPIO_REG_LHR & TRIMS_GPIO_PIN_LHR) && !(TRIMS_GPIO_REG_RHL & TRIMS_GPIO_PIN_RHL)) {
    // Bootloader needed
    const uint8_t *src;
    uint8_t *dest;
    uint32_t size;

    bwdt_reset();
    size = sizeof(BootCode);
    src = BootCode;
    dest = (uint8_t *) 0x20000000;

    for (; size; size -= 1) {
      *dest++ = *src++;
    }
    // Could check for a valid copy to RAM here
    // Go execute bootloader
    bwdt_reset();

    uint32_t address = *(uint32_t *) 0x20000004;

    ((void (*)(void)) (address))();		// Go execute the loaded application
  }

// run_application() ;
  asm(" mov.w	r1, #134217728");
  // 0x8000000
  asm(" add.w	r1, #32768");
  // 0x8000

  asm(" movw	r0, #60680");
  // 0xED08
  asm(" movt	r0, #57344");
  // 0xE000
  asm(" str	r1, [r0, #0]");
  // Set the VTOR

  asm("ldr	r0, [r1, #0]");
  // Stack pointer value
  asm("msr msp, r0");
  // Set it
  asm("ldr	r0, [r1, #4]");
  // Reset address
  asm("mov.w	r1, #1");
  asm("orr		r0, r1");
  // Set lsbit
  asm("bx r0");
  // Execute application
}
#endif

