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

#include <inttypes.h>

/// Format the 96-bit MCU unique ID into the given buffer
/// as a null terminated hexadecimal decimal string.
/// Each 32 bit segment is separated by a space
/// (e.g. 00510046 3136510C 35393532).
///
/// @param s Buffer to write the string to. Must be at least
/// LEN_CPU_UID + 1 bytes long.
void getCPUUniqueID(char * s);
/// Generate a serial number from the MCU unique ID following
/// ST's algorithm used by the DFU bootloader. The resulting
/// string uses only uppercase hexadecimal characters and is
/// null terminated.
///
/// @param s Buffer to write the string to. Must be at least
/// LEN_CPU_DFU_SERIAL + 1 bytes long.
void getCPUDFUSerial(char * s);

/// Length of the CPU UUID string without the null terminator
#define LEN_CPU_UID                    (3*8+2)
/// Length of the CPU DFU serial string without the null terminator
#define LEN_CPU_DFU_SERIAL             (12)

#if defined(SIMU)
extern const uint32_t cpu_uid[3];
#else
extern const uint32_t * cpu_uid;
#endif
