/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "opentx.h"
#include <ctype.h>

gpsdata_t gpsData;

/* This is a light implementation of a GPS frame decoding
   This should work with most of modern GPS devices configured to output 5 frames.
   It assumes there are some NMEA GGA frames to decode on the serial bus
   Now verifies checksum correctly before applying data

   Here we use only the following data :
     - latitude
     - longitude
     - GPS fix is/is not ok
     - GPS num sat (4 is enough to be +/- reliable)
     // added by Mis
     - GPS altitude (for OSD displaying)
     - GPS speed (for OSD displaying)
*/

#define NO_FRAME   0
#define FRAME_GGA  1
#define FRAME_RMC  2

#define DIGIT_TO_VAL(_x)    (_x - '0')

uint32_t GPS_coord_to_degrees(const char * coordinateString)
{
  const char * fieldSeparator, * remainingString;
  uint8_t degrees = 0, minutes = 0;
  uint16_t fractionalMinutes = 0;
  uint8_t digitIndex;
  
  // scan for decimal point or end of field
  for (fieldSeparator = coordinateString; isdigit((unsigned char) *fieldSeparator); fieldSeparator++) {
    if (fieldSeparator >= coordinateString + 15)
      return 0; // stop potential fail
  }
  remainingString = coordinateString;
  
  // convert degrees
  while ((fieldSeparator - remainingString) > 2) {
    if (degrees)
      degrees *= 10;
    degrees += DIGIT_TO_VAL(*remainingString++);
  }
  // convert minutes
  while (fieldSeparator > remainingString) {
    if (minutes)
      minutes *= 10;
    minutes += DIGIT_TO_VAL(*remainingString++);
  }
  // convert fractional minutes
  // expect up to four digits, result is in
  // ten-thousandths of a minute
  if (*fieldSeparator == '.') {
    remainingString = fieldSeparator + 1;
    for (digitIndex = 0; digitIndex < 4; digitIndex++) {
      fractionalMinutes *= 10;
      if (isdigit((unsigned char) *remainingString))
        fractionalMinutes += *remainingString++ - '0';
    }
  }
  // TODO return degrees * 10000000UL + (minutes * 1000000UL + fractionalMinutes * 100UL) / 6;
  return degrees * 1000000UL + (minutes * 100000UL + fractionalMinutes * 10UL) / 6;
}

// helper functions
uint32_t grab_fields(char * src, uint8_t mult)
{
  uint32_t i;
  uint32_t tmp = 0;
  for (i = 0; src[i] != 0; i++) {
    if (src[i] == '.') {
      i++;
      if (mult == 0)
        break;
      else
        src[i + mult] = 0;
    }
    tmp *= 10;
    if (src[i] >= '0' && src[i] <= '9')
      tmp += src[i] - '0';
    if (i >= 15)
      return 0; // out of bounds
  }
  return tmp;
}

typedef struct gpsDataNmea_s
{
  uint8_t fix;
  int32_t latitude;
  int32_t longitude;
  uint8_t numSat;
  uint16_t altitude;
  uint16_t speed;
  uint16_t groundCourse;
} gpsDataNmea_t;

bool gpsNewFrameNMEA(char c)
{
  static gpsDataNmea_t gps_Msg;
  
  uint8_t frameOK = 0;
  static uint8_t param = 0, offset = 0, parity = 0;
  static char string[15];
  static uint8_t checksum_param, gps_frame = NO_FRAME;
  
  switch (c) {
    case '$':
      param = 0;
      offset = 0;
      parity = 0;
      break;
    case ',':
    case '*':
      string[offset] = 0;
      if (param == 0) {
        // Frame identification
        gps_frame = NO_FRAME;
        if (string[0] == 'G' && string[1] == 'P' && string[2] == 'G' && string[3] == 'G' && string[4] == 'A')
          gps_frame = FRAME_GGA;
        if (string[0] == 'G' && string[1] == 'P' && string[2] == 'R' && string[3] == 'M' && string[4] == 'C')
          gps_frame = FRAME_RMC;
      }
      
      switch (gps_frame) {
        case FRAME_GGA:        //************* GPGGA FRAME parsing
          switch (param) {
            // case 1: // Time information
            //   break;
            case 2:
              gps_Msg.latitude = GPS_coord_to_degrees(string);
              break;
            case 3:
              if (string[0] == 'S')
                gps_Msg.latitude *= -1;
              break;
            case 4:
              gps_Msg.longitude = GPS_coord_to_degrees(string);
              break;
            case 5:
              if (string[0] == 'W')
                gps_Msg.longitude *= -1;
              break;
            case 6:
              if (string[0] > '0') {
                gps_Msg.fix = 1;
              }
              else {
                gps_Msg.fix = 0;
              }
              break;
            case 7:
              gps_Msg.numSat = grab_fields(string, 0);
              break;
            case 9:
              gps_Msg.altitude = grab_fields(string, 0);     // altitude in meters added by Mis
              break;
          }
          break;
        case FRAME_RMC:        //************* GPRMC FRAME parsing
          switch (param) {
            case 7:
              gps_Msg.speed = ((grab_fields(string, 1) * 5144L) / 1000L);    // speed in cm/s added by Mis
              break;
            case 8:
              gps_Msg.groundCourse = (grab_fields(string, 1));      // ground course deg * 10
              break;
          }
          break;

      }
      
      param++;
      offset = 0;
      if (c == '*')
        checksum_param = 1;
      else
        parity ^= c;
      break;
    case '\r':
    case '\n':
      if (checksum_param) {   //parity checksum
        uint8_t checksum = 16 * ((string[0] >= 'A') ? string[0] - 'A' + 10 : string[0] - '0') +
                           ((string[1] >= 'A') ? string[1] - 'A' + 10 : string[1] - '0');
        if (checksum == parity) {
          gpsData.packetCount++;
          switch (gps_frame) {
            case FRAME_GGA:
              frameOK = 1;
              if (gps_Msg.fix) {
                gpsData.fix = 1;
                gpsData.latitude = gps_Msg.latitude;
                gpsData.longitude = gps_Msg.longitude;
                gpsData.numSat = gps_Msg.numSat;
                gpsData.altitude = gps_Msg.altitude;
              }
              break;
            case FRAME_RMC:
              gpsData.speed = gps_Msg.speed;
              gpsData.groundCourse = gps_Msg.groundCourse;
              break;
          } // end switch
        }
        else {
          gpsData.errorCount++;
        }
      }
      checksum_param = 0;
      break;
    default:
      if (offset < 15)
        string[offset++] = c;
      if (!checksum_param)
        parity ^= c;
  }
  return frameOK;
}

bool gpsNewFrame(uint8_t c)
{
  return gpsNewFrameNMEA(c);
}

void gpsNewData(uint8_t c)
{
  if (!gpsNewFrame(c)) {
    return;
  }
}

void gpsWakeup()
{
  uint8_t byte;
  while (gpsGetByte(&byte)) {
    gpsNewData(byte);
  }
}
