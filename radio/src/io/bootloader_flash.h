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

#ifndef _BOOTLOADER_FLASH_H_
#define _BOOTLOADER_FLASH_H_

bool isBootloader(const char * filename);

class BootloaderDeviceFirmwareUpdate
{
  public:
    explicit BootloaderDeviceFirmwareUpdate(ModuleIndex module):
      module(module) {
    }

    void flashFirmware(const char * filename, ProgressHandler progressHandler);

  protected:
    ModuleIndex module;
};

#endif // _BOOTLOADER_FLASH_H_
