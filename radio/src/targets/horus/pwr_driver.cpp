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

#include "pwr.h"
#include "board.h"

uint32_t shutdownRequest;          // Stores intentional shutdown to avoid reboot loop
uint32_t shutdownReason;           // Used for detecting unexpected reboots regardless of reason
uint32_t powerupReason __NOINIT;   // Stores power up reason beyond initialization for emergency mode activation

void pwrInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  // Init Module PWR
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_ResetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN);
  GPIO_InitStructure.GPIO_Pin = INTMODULE_PWR_GPIO_PIN;
  GPIO_Init(INTMODULE_PWR_GPIO, &GPIO_InitStructure);

  GPIO_ResetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN);
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PWR_GPIO_PIN;
  GPIO_Init(EXTMODULE_PWR_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

  // Init PWR SWITCH PIN
  GPIO_InitStructure.GPIO_Pin = PWR_SWITCH_GPIO_PIN;
  GPIO_Init(PWR_GPIO, &GPIO_InitStructure);

  // Init PCBREV PIN
  // TODO to be removed on X10?
  GPIO_ResetBits(PCBREV_GPIO, PCBREV_GPIO_PIN);
  GPIO_InitStructure.GPIO_Pin = PCBREV_GPIO_PIN;
  GPIO_Init(PCBREV_GPIO, &GPIO_InitStructure);

  // Init SD-DETECT PIN
  GPIO_ResetBits(SD_PRESENT_GPIO, SD_PRESENT_GPIO_PIN);
  GPIO_InitStructure.GPIO_Pin = SD_PRESENT_GPIO_PIN;
  GPIO_Init(SD_PRESENT_GPIO, &GPIO_InitStructure);

  // Init TRAINER DETECT PIN
  GPIO_InitStructure.GPIO_Pin = TRAINER_DETECT_GPIO_PIN;
  GPIO_Init(TRAINER_DETECT_GPIO, &GPIO_InitStructure);

  pwrOn();
}

void pwrOn()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = PWR_ON_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(PWR_GPIO, &GPIO_InitStructure);

  GPIO_SetBits(PWR_GPIO, PWR_ON_GPIO_PIN);

  shutdownRequest = NO_SHUTDOWN_REQUEST;
  shutdownReason = DIRTY_SHUTDOWN;
}

void pwrOff()
{
#if defined(PCBX12S)
  // Shutdown the Audio amp
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = AUDIO_SHUTDOWN_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(AUDIO_SHUTDOWN_GPIO, &GPIO_InitStructure);
  GPIO_ResetBits(AUDIO_SHUTDOWN_GPIO, AUDIO_SHUTDOWN_GPIO_PIN);
#endif

  // Shutdown the Haptic
  hapticDone();

  shutdownRequest = SHUTDOWN_REQUEST;
  shutdownReason = NORMAL_POWER_OFF;
  GPIO_ResetBits(PWR_GPIO, PWR_ON_GPIO_PIN);
}

uint32_t pwrPressed()
{
  return GPIO_ReadInputDataBit(PWR_GPIO, PWR_SWITCH_GPIO_PIN) == Bit_RESET;
}

void pwrResetHandler()
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;

  // these two NOPs are needed (see STM32F errata sheet) before the peripheral
  // register can be written after the peripheral clock was enabled
  __ASM volatile ("nop");
  __ASM volatile ("nop");

  // We get here whether we are powering up normally, we had an unexpected reboot or we have just powered down normally.
  // We want:
  // - In the 2nd case, to power ON as soon as possible if an unexpected reboot happened
  //   (we get there running on remaining capacitor charge, soft power having been cut by the RESET).
  // - In the 3rd case, NOT power on as that would prevent from turning the system off.
  // - The 1st case does not need to be handled here, but will be as a result of the handling for the 3rd case, see below.
  //
  // shutdownRequest is used to handle the 3rd case. If we really powered down on purpose this will still be set to SHUTDOWN_REQUEST
  // as we left it in pwrOff(). If however we had an unexpected reboot, it would be set to NO_SHUTDOWN_REQUEST as we set it in pwrOn().
  // Any other value (e.g. resulting from data corruption) would also keep power on for safety, so this variable can NOT be used
  // to detect an unexpected reboot (on a normal power on the contents of the variable are random).
  //
  // shutdownReason is used to differentiate between an unexpected reboot and a normal power on. We set it to DIRTY_SHUTDOWN in pwrOn()
  // in anticipation of a potential reboot. Should there be one the value should be preserved and signal below that we rebooted unexpectedly.
  // If it is NOT set to DIRTY_SHUTDOWN we likely had a normal boot and its contents are random. Due to the need to initialize it to detect a
  // potential failure ASAP we cannot use it to determine in the firmware why we got there, it has to be buffered.
  //
  // powerupReason is there to cater for that, and is what is used in the firmware to decide whether we have to enter emergency mode.
  // This variable needs to be in a RAM section that is not initialized or zeroed, since once we exit this pwrResetHandler() function the
  // C runtime would otherwise overwrite it during program init.
  // Only for X12, X10 power circuit causes inability to shut down on some samples.

#if defined(PCBX12S)
  if (shutdownRequest != SHUTDOWN_REQUEST || WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()) {
    if (shutdownReason == DIRTY_SHUTDOWN) {
      powerupReason = DIRTY_SHUTDOWN;
    }
#else
   if (WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()) {
#endif
    pwrOn();
  }
}
