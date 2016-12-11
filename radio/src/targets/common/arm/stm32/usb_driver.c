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

#include "board.h"
#include "usb_dcd_int.h"
#include "usb_bsp.h"
#include "debug.h"

int usbPlugged(void)
{
  // debounce
  static uint8_t debounced_state = 0;
  static uint8_t last_state = 0;

  if (GPIO_ReadInputDataBit(USB_GPIO, USB_GPIO_PIN_VBUS)) {
    if (last_state) {
      debounced_state = 1;
    }
    last_state = 1;
  }
  else {
    if (!last_state) {
      debounced_state = 0;
    }
    last_state = 0;
  }
  return debounced_state;
}

USB_OTG_CORE_HANDLE USB_OTG_dev;

void OTG_FS_IRQHandler(void)
{
  DEBUG_INTERRUPT(INT_OTG_FS);
  USBD_OTG_ISR_Handler(&USB_OTG_dev);
}

void usbInit(void)
{
  // Initialize hardware
  USB_OTG_BSP_Init(&USB_OTG_dev);
}

void usbStart(void)
{
#if defined(USB_JOYSTICK)
  // initialize USB as HID device
  USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_HID_cb, &USR_cb);
#elif defined(USB_SERIAL)
  // initialize USB as CDC device (virtual serial port)
  USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);
#elif defined(USB_MASS_STORAGE)
  // initialize USB as MSC device
  USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_MSC_cb, &USR_cb);
#endif
}

void usbStop(void)
{
  USBD_DeInit(&USB_OTG_dev);
}
