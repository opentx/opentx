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

#include "wizarddata.h"

QString inputName(Input input)
{
  switch (input){
  case THROTTLE: return "THR";
  case RUDDER:   return "RUD";
  case ELEVATOR: return "ELE";
  case AILERON:  return "AIL";
  case FLAP:     return "FLP";
  case AIRBREAK: return "AIR";
  default:       return "---";
  }
}

QString vehicleName(Vehicle vehicle)
{
  switch (vehicle){
  case PLANE:       return "Plane";
  case MULTICOPTER: return "Multicopter";
  case HELICOPTER:  return "Helicopter";
  default:          return "---";
  }
}

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

bool Channel::isEmpty()
{
  return sourceDlg < 0;
}

QString Channel::toString()
{
  QString str;
  str =  QString("[%1, %2]").arg(inputName(input1)).arg(weight1);
  if ( input2 != NOINPUT )
    str += QString("[%1, %2]").arg(inputName(input2)).arg(weight2);
  return str;
}

QString Mix::toString()
{
  QString str;
  str = QString(tr("Model Name: ")) + name + "\n";
  str += QString(tr("Model Type: ")) + vehicleName(vehicleType) + "\n";
  for (int i=0; i<MAX_CHANNELS; i++){
    if (!channel[i].isEmpty()){
      str += QString(tr("Channel %1: ").arg(i+1));
      str += channel[i].toString();
      str += QString("\n");
    }
  }
  return str;
}