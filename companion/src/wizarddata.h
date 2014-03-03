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

#ifndef WIZARDDATA_H
#define WIZARDDATA_H
#include <QObject> 

#define MAX_CHANNELS 8
enum Input {NOINPUT, THROTTLE, RUDDER, ELEVATOR, AILERON, FLAP, AIRBREAK};
enum Vehicle {NOVEHICLE, PLANE, MULTICOPTER, HELICOPTER };

QString inputName(Input);
QString vehicleName(Input);

class Channel
{
public:
  int sourceDlg;     // Originating dialog, only of interest for producer
  Input input1;   
  Input input2;  
  int weight1;    
  int weight2;

  Channel();
  bool isEmpty();
  void clear(); 

  QString toString();
};

class Mix:QObject
{
  Q_OBJECT
public:
  QString name;
  Vehicle vehicleType;
  Channel channel[MAX_CHANNELS];

  QString toString();
};

#endif // WIZARDDATA_H