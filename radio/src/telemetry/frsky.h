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

#ifndef _FRSKY_H_
#define _FRSKY_H_

#include "../definitions.h"
#include "telemetry_holders.h"
#include "../io/pxx2.h"

// Receive buffer state machine state enum
enum FrSkyDataState {
  STATE_DATA_IDLE,
  STATE_DATA_START,
  STATE_DATA_IN_FRAME,
  STATE_DATA_XOR,
};

#define FRSKY_SPORT_BAUDRATE      57600

#define FRSKY_D_BAUDRATE          9600

#define FRSKY_SPORT_PACKET_SIZE   9

#define WSHH_TIMEOUT10ms          60  // 600ms

#define FRSKY_SPORT_AVERAGING     4
#define FRSKY_D_AVERAGING         8

// Enumerate FrSky packet codes
#define LINKPKT                   0xfe
#define USRPKT                    0xfd
#define A11PKT                    0xfc
#define A12PKT                    0xfb
#define A21PKT                    0xfa
#define A22PKT                    0xf9
#define ALRM_REQUEST              0xf8
#define RSSI1PKT                  0xf7
#define RSSI2PKT                  0xf6
#define RSSI_REQUEST              0xf1


// FrSky PRIM IDs (1 byte)
#define DATA_FRAME                0x10

// FrSky old DATA IDs (1 byte)
#define GPS_ALT_BP_ID             0x01
#define TEMP1_ID                  0x02
#define RPM_ID                    0x03
#define FUEL_ID                   0x04
#define TEMP2_ID                  0x05
#define VOLTS_ID                  0x06
#define GPS_ALT_AP_ID             0x09
#define BARO_ALT_BP_ID            0x10
#define GPS_SPEED_BP_ID           0x11
#define GPS_LONG_BP_ID            0x12
#define GPS_LAT_BP_ID             0x13
#define GPS_COURS_BP_ID           0x14
#define GPS_DAY_MONTH_ID          0x15
#define GPS_YEAR_ID               0x16
#define GPS_HOUR_MIN_ID           0x17
#define GPS_SEC_ID                0x18
#define GPS_SPEED_AP_ID           0x19
#define GPS_LONG_AP_ID            0x1A
#define GPS_LAT_AP_ID             0x1B
#define GPS_COURS_AP_ID           0x1C
#define BARO_ALT_AP_ID            0x21
#define GPS_LONG_EW_ID            0x22
#define GPS_LAT_NS_ID             0x23
#define ACCEL_X_ID                0x24
#define ACCEL_Y_ID                0x25
#define ACCEL_Z_ID                0x26
#define CURRENT_ID                0x28
#define VARIO_ID                  0x30
#define VFAS_ID                   0x39
#define VOLTS_BP_ID               0x3A
#define VOLTS_AP_ID               0x3B
#define FRSKY_LAST_ID             0x3F
#define D_RSSI_ID                 0xF0
#define D_A1_ID                   0xF1
#define D_A2_ID                   0xF2

#define VFAS_D_HIPREC_OFFSET      2000


// FrSky new DATA IDs (2 bytes)
#define ALT_FIRST_ID              0x0100
#define ALT_LAST_ID               0x010f
#define VARIO_FIRST_ID            0x0110
#define VARIO_LAST_ID             0x011f
#define CURR_FIRST_ID             0x0200
#define CURR_LAST_ID              0x020f
#define VFAS_FIRST_ID             0x0210
#define VFAS_LAST_ID              0x021f
#define CELLS_FIRST_ID            0x0300
#define CELLS_LAST_ID             0x030f
#define T1_FIRST_ID               0x0400
#define T1_LAST_ID                0x040f
#define T2_FIRST_ID               0x0410
#define T2_LAST_ID                0x041f
#define RPM_FIRST_ID              0x0500
#define RPM_LAST_ID               0x050f
#define FUEL_FIRST_ID             0x0600
#define FUEL_LAST_ID              0x060f
#define ACCX_FIRST_ID             0x0700
#define ACCX_LAST_ID              0x070f
#define ACCY_FIRST_ID             0x0710
#define ACCY_LAST_ID              0x071f
#define ACCZ_FIRST_ID             0x0720
#define ACCZ_LAST_ID              0x072f
#define GPS_LONG_LATI_FIRST_ID    0x0800
#define GPS_LONG_LATI_LAST_ID     0x080f
#define GPS_ALT_FIRST_ID          0x0820
#define GPS_ALT_LAST_ID           0x082f
#define GPS_SPEED_FIRST_ID        0x0830
#define GPS_SPEED_LAST_ID         0x083f
#define GPS_COURS_FIRST_ID        0x0840
#define GPS_COURS_LAST_ID         0x084f
#define GPS_TIME_DATE_FIRST_ID    0x0850
#define GPS_TIME_DATE_LAST_ID     0x085f
#define A3_FIRST_ID               0x0900
#define A3_LAST_ID                0x090f
#define A4_FIRST_ID               0x0910
#define A4_LAST_ID                0x091f
#define AIR_SPEED_FIRST_ID        0x0a00
#define AIR_SPEED_LAST_ID         0x0a0f
#define RBOX_BATT1_FIRST_ID       0x0b00
#define RBOX_BATT1_LAST_ID        0x0b0f
#define RBOX_BATT2_FIRST_ID       0x0b10
#define RBOX_BATT2_LAST_ID        0x0b1f
#define RBOX_STATE_FIRST_ID       0x0b20
#define RBOX_STATE_LAST_ID        0x0b2f
#define RBOX_CNSP_FIRST_ID        0x0b30
#define RBOX_CNSP_LAST_ID         0x0b3f
#define SD1_FIRST_ID              0x0b40
#define SD1_LAST_ID               0x0b4f
#define ESC_POWER_FIRST_ID        0x0b50
#define ESC_POWER_LAST_ID         0x0b5f
#define ESC_RPM_CONS_FIRST_ID     0x0b60
#define ESC_RPM_CONS_LAST_ID      0x0b6f
#define ESC_TEMPERATURE_FIRST_ID  0x0b70
#define ESC_TEMPERATURE_LAST_ID   0x0b7f
#define X8R_FIRST_ID              0x0c20
#define X8R_LAST_ID               0x0c2f
#define S6R_FIRST_ID              0x0c30
#define S6R_LAST_ID               0x0c3f
#define GASSUIT_TEMP1_FIRST_ID    0x0d00
#define GASSUIT_TEMP1_LAST_ID     0x0d0f
#define GASSUIT_TEMP2_FIRST_ID    0x0d10
#define GASSUIT_TEMP2_LAST_ID     0x0d1f
#define GASSUIT_SPEED_FIRST_ID    0x0d20
#define GASSUIT_SPEED_LAST_ID     0x0d2f
#define GASSUIT_RES_VOL_FIRST_ID  0x0d30
#define GASSUIT_RES_VOL_LAST_ID   0x0d3f
#define GASSUIT_RES_PERC_FIRST_ID 0x0d40
#define GASSUIT_RES_PERC_LAST_ID  0x0d4f
#define GASSUIT_FLOW_FIRST_ID     0x0d50
#define GASSUIT_FLOW_LAST_ID      0x0d5f
#define GASSUIT_MAX_FLOW_FIRST_ID 0x0d60
#define GASSUIT_MAX_FLOW_LAST_ID  0x0d6f
#define GASSUIT_AVG_FLOW_FIRST_ID 0x0d70
#define GASSUIT_AVG_FLOW_LAST_ID  0x0d7f
#define DIY_FIRST_ID              0x5000
#define DIY_LAST_ID               0x52ff
#define DIY_STREAM_FIRST_ID       0x5000
#define DIY_STREAM_LAST_ID        0x50ff
#define FACT_TEST_ID              0xf000
#define RSSI_ID                   0xf101
#define ADC1_ID                   0xf102
#define ADC2_ID                   0xf103
#define SP2UART_A_ID              0xfd00
#define SP2UART_B_ID              0xfd01
#define BATT_ID                   0xf104
#define RAS_ID                    0xf105
#define XJT_VERSION_ID            0xf106
#define FUEL_QTY_FIRST_ID         0x0a10
#define FUEL_QTY_LAST_ID          0x0a1f
#define R9_PWR_ID                 0xf107 // TODO: *CRITICAL* agree on ID with FrSky, this value is just a placeholder


// Default sensor data IDs (Physical IDs + CRC)
#define DATA_ID_VARIO             0x00 // 0
#define DATA_ID_FLVSS             0xA1 // 1
#define DATA_ID_FAS               0x22 // 2
#define DATA_ID_GPS               0x83 // 3
#define DATA_ID_RPM               0xE4 // 4
#define DATA_ID_SP2UH             0x45 // 5
#define DATA_ID_SP2UR             0xC6 // 6

#if defined(NO_RAS)
  #define IS_RAS_VALUE_VALID()            (false)
#elif defined(PCBX10)
  #define IS_RAS_VALUE_VALID()            (false)
#elif defined(PCBX9DP) || defined(PCBX9E)
  #define IS_RAS_VALUE_VALID()            (telemetryData.xjtVersion != 0 && telemetryData.xjtVersion != 0xff)
#else
  #define IS_RAS_VALUE_VALID()            (true)
#endif

#define IS_HIDDEN_TELEMETRY_VALUE(id)     ((id == SP2UART_A_ID) || (id == SP2UART_B_ID) || (id == XJT_VERSION_ID) || (id == RAS_ID) || (id == FACT_TEST_ID))

#define ALARM_GREATER(channel, alarm)     ((g_model.frsky.channels[channel].alarms_greater >> alarm) & 1)
#define ALARM_LEVEL(channel, alarm)       ((g_model.frsky.channels[channel].alarms_level >> (2*alarm)) & 3)

  #define TELEMETRY_STREAMING()           (telemetryData.rssi.value > 0)
  #define TELEMETRY_RSSI()                (telemetryData.rssi.value)
  #define TELEMETRY_RSSI_MIN()            (telemetryData.rssi.min)

  #define TELEMETRY_CELL_VOLTAGE_MUTLIPLIER  1

  #define TELEMETRY_GPS_SPEED_BP          telemetryData.hub.gpsSpeed_bp
  #define TELEMETRY_GPS_SPEED_AP          telemetryData.hub.gpsSpeed_ap

  #define TELEMETRY_ABSOLUTE_GPS_ALT      (telemetryData.hub.gpsAltitude)
  #define TELEMETRY_RELATIVE_GPS_ALT      (telemetryData.hub.gpsAltitude + telemetryData.hub.gpsAltitudeOffset)
  #define TELEMETRY_RELATIVE_GPS_ALT_BP   (TELEMETRY_RELATIVE_GPS_ALT / 100)

  #define TELEMETRY_RELATIVE_BARO_ALT_BP  (telemetryData.hub.baroAltitude / 100)
  #define TELEMETRY_RELATIVE_BARO_ALT_AP  (telemetryData.hub.baroAltitude % 100)

  #define TELEMETRY_BARO_ALT_PREPARE()    div_t baroAltitudeDivision = div(getConvertedTelemetryValue(telemetryData.hub.baroAltitude, UNIT_DIST), 100)
  #define TELEMETRY_BARO_ALT_FORMAT       "%c%d.%02d,"
  #define TELEMETRY_BARO_ALT_ARGS         telemetryData.hub.baroAltitude < 0 ? '-' : ' ', abs(baroAltitudeDivision.quot), abs(baroAltitudeDivision.rem),
  #define TELEMETRY_GPS_ALT_FORMAT        "%c%d.%02d,"
  #define TELEMETRY_GPS_ALT_ARGS          telemetryData.hub.gpsAltitude < 0 ? '-' : ' ', abs(telemetryData.hub.gpsAltitude / 100), abs(telemetryData.hub.gpsAltitude % 100),
  #define TELEMETRY_SPEED_UNIT            (IS_IMPERIAL_ENABLE() ? SPEED_UNIT_IMP : SPEED_UNIT_METR)
  #define TELEMETRY_GPS_SPEED_FORMAT      "%d,"
  #define TELEMETRY_GPS_SPEED_ARGS        telemetryData.hub.gpsSpeed_bp,

  #define TELEMETRY_CELLS_ARGS          telemetryData.hub.cellsSum / 10, telemetryData.hub.cellsSum % 10, TELEMETRY_CELL_VOLTAGE(0)/100, TELEMETRY_CELL_VOLTAGE(0)%100, TELEMETRY_CELL_VOLTAGE(1)/100, TELEMETRY_CELL_VOLTAGE(1)%100, TELEMETRY_CELL_VOLTAGE(2)/100, TELEMETRY_CELL_VOLTAGE(2)%100, TELEMETRY_CELL_VOLTAGE(3)/100, TELEMETRY_CELL_VOLTAGE(3)%100, TELEMETRY_CELL_VOLTAGE(4)/100, TELEMETRY_CELL_VOLTAGE(4)%100, TELEMETRY_CELL_VOLTAGE(5)/100, TELEMETRY_CELL_VOLTAGE(5)%100, TELEMETRY_CELL_VOLTAGE(6)/100, TELEMETRY_CELL_VOLTAGE(6)%100, TELEMETRY_CELL_VOLTAGE(7)/100, TELEMETRY_CELL_VOLTAGE(7)%100, TELEMETRY_CELL_VOLTAGE(8)/100, TELEMETRY_CELL_VOLTAGE(8)%100, TELEMETRY_CELL_VOLTAGE(9)/100, TELEMETRY_CELL_VOLTAGE(9)%100, TELEMETRY_CELL_VOLTAGE(10)/100, TELEMETRY_CELL_VOLTAGE(10)%100, TELEMETRY_CELL_VOLTAGE(11)/100, TELEMETRY_CELL_VOLTAGE(11)%100,
  #define TELEMETRY_CELLS_FORMAT        "%d.%d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,"
  #define TELEMETRY_CELLS_LABEL         "Cell volts,Cell 1,Cell 2,Cell 3,Cell 4,Cell 5,Cell 6,Cell 7,Cell 8,Cell 9,Cell 10,Cell 11,Cell 12,"

  #define TELEMETRY_CURRENT_FORMAT        "%d.%d,"
  #define TELEMETRY_CURRENT_ARGS          telemetryData.hub.current / 10, telemetryData.hub.current % 10,
  #define TELEMETRY_VFAS_FORMAT           "%d.%d,"
  #define TELEMETRY_VFAS_ARGS             telemetryData.hub.vfas / 10, telemetryData.hub.vfas % 10,
  #define TELEMETRY_VSPEED_FORMAT         "%c%d.%02d,"
  #define TELEMETRY_VSPEED_ARGS           telemetryData.hub.varioSpeed < 0 ? '-' : ' ', abs(telemetryData.hub.varioSpeed / 100), abs(telemetryData.hub.varioSpeed % 100),
  #define TELEMETRY_ASPEED_FORMAT         "%d.%d,"
  #define TELEMETRY_ASPEED_ARGS           telemetryData.hub.airSpeed / 10, telemetryData.hub.airSpeed % 10,
  #define TELEMETRY_OPENXSENSOR()         (0)

#define TELEMETRY_CELL_VOLTAGE(k)         (telemetryData.hub.cellVolts[k] * TELEMETRY_CELL_VOLTAGE_MUTLIPLIER)
#define TELEMETRY_MIN_CELL_VOLTAGE        (telemetryData.hub.minCellVolts * TELEMETRY_CELL_VOLTAGE_MUTLIPLIER)

#define START_STOP                        0x7e
#define BYTESTUFF                         0x7d
#define STUFF_MASK                        0x20

#define frskySendAlarms()

typedef enum {
  TS_IDLE = 0,  // waiting for 0x5e frame marker
  TS_DATA_ID,   // waiting for dataID
  TS_DATA_LOW,  // waiting for data low byte
  TS_DATA_HIGH, // waiting for data high byte
  TS_XOR = 0x80 // decode stuffed byte
} TS_STATE;

// FrSky D Telemetry Protocol
void processHubPacket(uint8_t id, int16_t value);
void frskyDProcessPacket(const uint8_t *packet);

// FrSky S.PORT Telemetry Protocol
void sportProcessTelemetryPacket(const uint8_t * packet);
void sportProcessTelemetryPacketWithoutCrc(const uint8_t * packet);

void telemetryWakeup();
void telemetryReset();

extern uint8_t telemetryProtocol;
void telemetryInit(uint8_t protocol);

void telemetryInterrupt10ms();

enum TelemetryProtocol
{
  TELEM_PROTO_FRSKY_D,
  TELEM_PROTO_FRSKY_SPORT,
  TELEM_PROTO_CROSSFIRE,
  TELEM_PROTO_SPEKTRUM,
  TELEM_PROTO_LUA,
  TELEM_PROTO_FLYSKY_IBUS,
};

struct TelemetryData {
  TelemetryValueWithMin swr;          // TODO Min not needed
  TelemetryValueWithMin rssi;         // TODO Min not needed
  uint16_t xjtVersion;
  bool varioHighPrecision;
};

extern TelemetryData telemetryData;

void processFrskyTelemetryData(uint8_t data);
void processFrskyPXX2Data(uint8_t data);

#endif // _FRSKY_H_
