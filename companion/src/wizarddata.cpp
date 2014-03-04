/*
 * Author - Kjell Kernen
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
#include <string.h>
#include "wizarddata.h"

Channel::Channel()
{
  clear();
}

void Channel::clear()
{
  sourceDlg = -1;
  input1 = NOINPUT;
  input2 = NOINPUT;
  weight1 = 0;  
  weight2 = 0;
}

WizMix::WizMix()
{
  complete = false;
  strcpy(name, "            ");
  vehicle = NOVEHICLE;
}


