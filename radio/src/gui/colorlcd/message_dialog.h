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

#ifndef _MESSAGE_DIALOG_H_
#define _MESSAGE_DIALOG_H_

#include "dialog.h"

class MessageDialog: public Dialog {
  public:
    MessageDialog(const char * title, const char * message);

#if defined(HARDWARE_KEYS)
    void onKeyEvent(event_t event) override;
#endif
};

inline void POPUP_INFORMATION(const char * str)
{
  new MessageDialog("Message", str);
}

#endif // _MESSAGE_DIALOG_H_
