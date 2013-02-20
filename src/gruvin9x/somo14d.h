/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#define PROMPT_SYSTEM_BASE      230
#define PROMPT_CUSTOM_BASE      256

extern void pushPrompt(uint16_t prompt, uint8_t id=0);
extern bool isPlaying(uint8_t id);

#define PLAY_FUNCTION(x, ...) void x(__VA_ARGS__, uint8_t id)
#define PUSH_PROMPT(p) pushPrompt((p), id)
#define PUSH_CUSTOM_PROMPT(p, id) pushPrompt(PROMPT_CUSTOM_BASE+(p), (id))
#define PLAY_NUMBER(n, u, a) playNumber((n), (u), (a), id)
#define PLAY_DURATION(d) playDuration((d), id)
#define IS_PLAYING(id) isPlaying((id))
#define PLAY_VALUE(v, id) playValue((v), (id))

#define VOLUME_LEVEL_MAX  7
#define VOLUME_LEVEL_DEF  7
#define setVolume(v)
