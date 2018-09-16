/*
 * Copyright (C) OpenTX
 *
 * Dedicate for FlySky NV14 board.
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

#include "opentx.h"

#define END                             0xC0
#define ESC                             0xDB
#define ESC_END                         0xDC
#define ESC_ESC                         0xDD

#define FRAME_TYPE_REQUEST_ACK          0x01
#define FRAME_TYPE_REQUEST_NACK         0x02
#define FRAME_TYPE_ANSWER               0x10

enum FlySkyModuleCommandID {
  COMMAND_ID_NONE,
  COMMAND_ID_RF_INIT,
  COMMAND_ID_BIND,
  COMMAND_ID_SET_RECEIVER_ID,
  COMMAND_ID_RF_GET_CONFIG,
  COMMAND_ID_SEND_CHANNEL_DATA,
  COMMAND_ID_RX_SENSOR_DATA,
  COMMAND_ID_SET_RX_PWM_PPM,
  COMMAND_ID_SET_RX_SERVO_FREQ,
  COMMAND_ID_GET_FIRMWARE_REV,
  COMMAND_ID_SET_RX_IBUS_SBUS,
  COMMAND_ID_SET_RX_IBUS_SERVO_EXT,
  COMMAND_ID0C_UPDATE_RF_FIRMWARE = 0x0C,
  COMMAND_ID0D_SET_TX_POWER = 0x0D,
  COMMAND_ID_SET_RF_PROTOCOL,
  COMMAND_ID_TEST_RANGE,
  COMMAND_ID_TEST_RF_RESERVED,
  COMMAND_ID20_UPDATE_RX_FIRMWARE = 0x20,

  COMMAND_ID_LAST
};
#define IS_VALID_COMMAND_ID(id)         ((id) < COMMAND_ID_LAST)

#ifndef custom_log
#define custom_log
#endif
enum DEBUG_RF_FRAME_PRINT_E {
  FRAME_PRINT_OFF,// 0:OFF, 1:RF frame only, 2:TX frame only, 3:Both RF and TX frame
  RF_FRAME_ONLY,
  TX_FRAME_ONLY,
  BOTH_FRAME_PRINT
};
#define DEBUG_RF_FRAME_PRINT            FRAME_PRINT_OFF
#define FLYSKY_MODULE_TIMEOUT           155 /* ms */
#define NUM_OF_NV14_CHANNELS            (14)
#define VALID_CH_DATA(v)                ((v) > 900 && (v) < 2100)

#define gRomData                        g_model.moduleData[INTERNAL_MODULE].romData
#define SET_DIRTY()                     storageDirty(EE_MODEL)

enum FlySkySensorType_E {
  FLYSKY_SENSOR_RX_VOLTAGE,
  FLYSKY_SENSOR_RX_SIGNAL,
  FLYSKY_SENSOR_RX_RSSI,
  FLYSKY_SENSOR_RX_NOISE,
  FLYSKY_SENSOR_RX_SNR,
  FLYSKY_SENSOR_TEMP,
  FLYSKY_SENSOR_EXT_VOLTAGE,
  FLYSKY_SENSOR_MOTO_RPM,
  FLYSKY_SENSOR_PRESURRE,
  FLYSKY_SENSOR_GPS
};

enum FlySkyModuleState_E {
  FLYSKY_MODULE_STATE_SET_TX_POWER,
  FLYSKY_MODULE_STATE_INIT,
  FLYSKY_MODULE_STATE_BIND,
  FLYSKY_MODULE_STATE_SET_RECEIVER_ID,
  FLYSKY_MODULE_STATE_SET_RX_PWM_PPM,
  FLYSKY_MODULE_STATE_SET_RX_IBUS_SBUS,
  FLYSKY_MODULE_STATE_SET_RX_FREQUENCY,
  FLYSKY_MODULE_STATE_UPDATE_RF_FIRMWARE,
  FLYSKY_MODULE_STATE_UPDATE_RX_FIRMWARE,
  FLYSKY_MODULE_STATE_UPDATE_HALL_FIRMWARE,
  FLYSKY_MODULE_STATE_UPDATE_RF_PROTOCOL,
  FLYSKY_MODULE_STATE_GET_RECEIVER_CONFIG,
  FLYSKY_MODULE_STATE_GET_RX_FIRMWARE_INFO,
  FLYSKY_MODULE_STATE_GET_RX_FW_VERSION,
  FLYSKY_MODULE_STATE_GET_RF_FIRMWARE_INFO,
  FLYSKY_MODULE_STATE_GET_RF_FW_VERSION,
  FLYSKY_MODULE_STATE_IDLE,
  FLYSKY_MODULE_STATE_DEFAULT,
};

enum FlySkyBindState_E {
  BIND_LOW_POWER,
  BIND_NORMAL_POWER,
  BIND_EXIT,
};

enum FlySkyRxPulse_E {
  FLYSKY_PWM,
  FLYSKY_PPM
};

enum FlySkyRxPort_E {
  FLYSKY_IBUS,
  FLYSKY_SBUS
};

enum FlySkyFirmwareType_E {
  FLYSKY_RX_FIRMWARE,
  FLYSKY_RF_FIRMWARE
};

enum FlySkyChannelDataType_E {
  FLYSKY_CHANNEL_DATA_NORMAL,
  FLYSKY_CHANNEL_DATA_FAILSAFE
};

enum FlySkyPulseModeValue_E {
  PWM_IBUS, PWM_SBUS,
  PPM_IBUS, PPM_SBUS
};
#define GET_FLYSKY_PWM_PPM    (gRomData.mode < 2 ? FLYSKY_PWM: FLYSKY_PPM)
#define GET_FLYSKY_IBUS_SBUS  (gRomData.mode & 1 ? FLYSKY_SBUS: FLYSKY_IBUS)

typedef struct RX_FLYSKY_IBUS_S {
  uint8_t id[2];
  uint8_t channel[2];
} rx_ibus_t;

typedef struct FLYSKY_GPS_INFO_S {
  uint8_t position_fix;
  uint8_t satell_cnt;
  uint8_t latitude[4];
  uint8_t longtitude[4];
  uint8_t altitude[4];
  uint8_t g_speed[2];
  uint8_t direction[2];
} gps_info_t;

typedef struct FLYSKY_SENSOR_DATA_S {
  uint8_t sensor_type;
  uint8_t sensor_id;
  uint8_t voltage[2];
  uint8_t signal;
  uint8_t rssi[2];
  uint8_t noise[2];
  uint8_t snr[2];
  uint8_t temp[2];
  uint8_t ext_voltage[2];
  uint8_t moto_rpm[2];
  uint8_t pressure_value[2];
  gps_info_t gps_info;
} rx_sensor_t;

typedef struct FLYSKY_FIRMWARE_INFO_S {
  uint8_t fw_id[4];
  uint8_t fw_len[4];
  uint8_t hw_rev[4];
  uint8_t fw_rev[4];
  uint8_t fw_pkg_addr[4];
  uint8_t fw_pkg_len[4];
  uint8_t * pkg_data;
} fw_info_t;

typedef struct RF_INFO_S {
  uint8_t id[4];
  uint8_t bind_power;
  uint8_t num_of_channel;
  uint8_t channel_data_type;
  uint8_t protocol;
  uint8_t fw_state; // 0: normal, COMMAND_ID0C_UPDATE_RF_FIRMWARE or COMMAND_ID_UPDATE_FIRMWARE_END
  fw_info_t fw_info;
} rf_info_t;

typedef struct RX_INFO_S {
  int16_t servo_value[NUM_OF_NV14_CHANNELS];
  rx_ibus_t ibus;
  fw_info_t fw_info;
} rx_info_t;

static rx_sensor_t rx_sensor_info;
static uint8_t tx_working_power = 90;

static rf_info_t rf_info = {
  .id               = {8, 8, 8, 8},
  .bind_power       = BIND_NORMAL_POWER,
  .num_of_channel   = NUM_OF_NV14_CHANNELS, // TODO + g_model.moduleData[port].channelsCount;
  .channel_data_type= FLYSKY_CHANNEL_DATA_NORMAL,
  .protocol         = 0,
  .fw_state         = 0,
  .fw_info          = {0}
};

static rx_info_t rx_info = {
  .servo_value      = {1500, 1500, 1500, 1500},
  .ibus             = {{0, 0},
                       {0, 0}},
  .fw_info          = {0}
};

/// APIs: ->SetXXXX ->OnXXXX ->GetXXXX
/// For Rx Binding: void onFlySkyBindReceiver(uint8_t port);

uint16_t getFlySkyReceiverSensorVoltage(uint8_t port, uint16_t * voltage)
{
  uint8_t * flysky_sensor = (uint8_t *) voltage;
  uint8_t * ptr = rx_sensor_info.voltage;

  if (flysky_sensor != NULL) {
    flysky_sensor[0] = ptr[0];
    flysky_sensor[1] = ptr[1];
  }

  return ptr[1] * 256 + ptr[0];
}

uint8_t getFlySkyReceiverSensorSignal(uint8_t port, uint8_t * signal)
{
  uint8_t * flysky_sensor = (uint8_t *) signal;

  if (flysky_sensor != NULL) {
    flysky_sensor[0] = rx_sensor_info.signal;
  }

  return rx_sensor_info.signal;
}

uint16_t getFlySkyReceiverSensorRSSI(uint8_t port, uint16_t * rssi)
{
  uint8_t * flysky_sensor = (uint8_t *) rssi;
  uint8_t * ptr = rx_sensor_info.rssi;

  if (flysky_sensor != NULL) {
    flysky_sensor[0] = ptr[0];
    flysky_sensor[1] = ptr[1];
  }

  return ptr[1] * 256 + ptr[0];
}

uint16_t getFlySkyReceiverSensorNoise(uint8_t port, uint16_t * noise)
{
  uint8_t * flysky_sensor = (uint8_t *) noise;
  uint8_t * ptr = rx_sensor_info.noise;

  if (flysky_sensor != NULL) {
    flysky_sensor[0] = ptr[0];
    flysky_sensor[1] = ptr[1];
  }

  return ptr[1] * 256 + ptr[0];
}

uint16_t getFlySkyReceiverSensorSNR(uint8_t port, uint16_t * snr)
{
  uint8_t * flysky_sensor = (uint8_t *) snr;
  uint8_t * ptr = rx_sensor_info.snr;

  if (flysky_sensor != NULL) {
    flysky_sensor[0] = ptr[0];
    flysky_sensor[1] = ptr[1];
  }

  return ptr[1] * 256 + ptr[0];
}

uint16_t getFlySkyReceiverSensorTemperature(uint8_t port, uint16_t * temp)
{
  uint8_t * flysky_sensor = (uint8_t *) temp;
  uint8_t * ptr = rx_sensor_info.temp;

  if (flysky_sensor != NULL) {
    flysky_sensor[0] = ptr[0];
    flysky_sensor[1] = ptr[1];
  }

  return ptr[1] * 256 + ptr[0];
}

uint16_t getFlySkyReceiverSensorExtVoltage(uint8_t port, uint16_t * voltage)
{
  uint8_t * flysky_sensor = (uint8_t *) voltage;
  uint8_t * ptr = rx_sensor_info.ext_voltage;

  if (flysky_sensor != NULL) {
    flysky_sensor[0] = ptr[0];
    flysky_sensor[1] = ptr[1];
  }

  return ptr[1] * 256 + ptr[0];
}


void getFlySkyReceiverFirmwareRevision(uint8_t port, uint32_t * revision)
{
  uint8_t * fw_info = (uint8_t *) revision;
  fw_info[0] = rx_info.fw_info.fw_rev[0];
  fw_info[1] = rx_info.fw_info.fw_rev[1];
  fw_info[2] = rx_info.fw_info.fw_rev[2];
  fw_info[3] = rx_info.fw_info.fw_rev[3];
}

void getFlySkyTransmitterFirmwareRevision(uint8_t port, uint32_t * revision)
{
  uint8_t * fw_info = (uint8_t *) revision;
  fw_info[0] = rf_info.fw_info.fw_rev[0];
  fw_info[1] = rf_info.fw_info.fw_rev[1];
  fw_info[2] = rf_info.fw_info.fw_rev[2];
  fw_info[3] = rf_info.fw_info.fw_rev[3];
}

void getFlySkyFirmwareId(uint8_t port, bool is_receiver, uint32_t * firmware_id)
{
  uint8_t * fw_info = (uint8_t *) firmware_id;
  if (is_receiver) {
    fw_info[0] = rx_info.fw_info.fw_id[0];
    fw_info[1] = rx_info.fw_info.fw_id[1];
    fw_info[2] = rx_info.fw_info.fw_id[2];
    fw_info[3] = rx_info.fw_info.fw_id[3];
  }
  else {
    fw_info[0] = rf_info.fw_info.fw_id[0];
    fw_info[1] = rf_info.fw_info.fw_id[1];
    fw_info[2] = rf_info.fw_info.fw_id[2];
    fw_info[3] = rf_info.fw_info.fw_id[3];
  }
}

void getFlySkyHardwareRevision(uint8_t port, bool is_receiver, uint32_t * revision)
{
  uint8_t * fw_info = (uint8_t *) revision;
  if (is_receiver) {
    fw_info[0] = rx_info.fw_info.hw_rev[0];
    fw_info[1] = rx_info.fw_info.hw_rev[1];
    fw_info[2] = rx_info.fw_info.hw_rev[2];
    fw_info[3] = rx_info.fw_info.hw_rev[3];
  }
  else {
    fw_info[0] = rf_info.fw_info.hw_rev[0];
    fw_info[1] = rf_info.fw_info.hw_rev[1];
    fw_info[2] = rf_info.fw_info.hw_rev[2];
    fw_info[3] = rf_info.fw_info.hw_rev[3];
  }
}

void getFlySkyFirmwareRevision(uint8_t port, bool is_receiver, uint32_t * revision)
{
  if (is_receiver) getFlySkyReceiverFirmwareRevision(port, revision);
  else getFlySkyTransmitterFirmwareRevision(port, revision);
}


void setFlySkyGetFirmwarePackageAddr(uint8_t port, bool is_receiver, uint32_t * package_address)
{
  uint8_t * fw_package = (uint8_t *) package_address;
  if (is_receiver) {
    fw_package[0] = rx_info.fw_info.fw_pkg_addr[0];
    fw_package[1] = rx_info.fw_info.fw_pkg_addr[1];
    fw_package[2] = rx_info.fw_info.fw_pkg_addr[2];
    fw_package[3] = rx_info.fw_info.fw_pkg_addr[3];
  }
  else {
    fw_package[0] = rf_info.fw_info.fw_pkg_addr[0];
    fw_package[1] = rf_info.fw_info.fw_pkg_addr[1];
    fw_package[2] = rf_info.fw_info.fw_pkg_addr[2];
    fw_package[3] = rf_info.fw_info.fw_pkg_addr[3];
  }
}

void setFlySkyGetFirmwarePackageLen(uint8_t port, bool is_receiver, uint32_t * package_len)
{
  uint8_t * fw_package = (uint8_t *) package_len;
  if (is_receiver) {
    fw_package[0] = rx_info.fw_info.fw_pkg_len[0];
    fw_package[1] = rx_info.fw_info.fw_pkg_len[1];
    fw_package[2] = rx_info.fw_info.fw_pkg_len[2];
    fw_package[3] = rx_info.fw_info.fw_pkg_len[3];
  }
  else {
    fw_package[0] = rf_info.fw_info.fw_pkg_len[0];
    fw_package[1] = rf_info.fw_info.fw_pkg_len[1];
    fw_package[2] = rf_info.fw_info.fw_pkg_len[2];
    fw_package[3] = rf_info.fw_info.fw_pkg_len[3];
  }
}

void setFlySkyGetFirmwarePackageBuffer(uint8_t port, bool is_receiver, uint8_t * buffer)
{
  if (is_receiver) {
    rx_info.fw_info.pkg_data = buffer;
  }
  else {
    rf_info.fw_info.pkg_data = buffer;
  }
}


void setFlySkyTransmitterId(uint8_t port, uint32_t rf_id)
{
  rf_info.id[0] = rf_id & 0xff;
  rf_info.id[1] = rf_id & 0xff00 >> 8;
  rf_info.id[2] = rf_id & 0xff0000 >> 16;
  rf_info.id[3] = rf_id & 0xff000000 >> 24;
}

void setFlySkyTransmitterProtocol(uint8_t port, uint8_t protocol)
{
  rf_info.protocol = protocol;
}

void setFlySkyReceiverBindPowerLow(uint8_t port)
{
  rf_info.bind_power = BIND_LOW_POWER;
}

void setFlySkyReceiverBindPowerNormal(uint8_t port)
{
  rf_info.bind_power = BIND_NORMAL_POWER;
}

void setFlySkyReceiverChannelCounts(uint8_t port, uint8_t channle_counts)
{
  rf_info.num_of_channel = channle_counts;
}

void setFlySkyChannelDataFailSafe(uint8_t port)
{
  rf_info.channel_data_type = FLYSKY_CHANNEL_DATA_FAILSAFE;
}

void setFlySkyChannelDataNormalMode(uint8_t port)
{
  rf_info.channel_data_type = FLYSKY_CHANNEL_DATA_NORMAL;
}

void setFlySkyChannelOutputs(int channel, int16_t outValue)
{
  channelOutputs[channel] = outValue;
}

static uint32_t set_loop_cnt = 0;

void setFlySkyChannelData(int channel, int16_t servoValue)
{
  if (channel < NUM_OF_NV14_CHANNELS && VALID_CH_DATA(servoValue)) {
    rx_info.servo_value[channel] = (1000 * (servoValue + 1024) / 2048) + 1000;
  }

  if ((DEBUG_RF_FRAME_PRINT & TX_FRAME_ONLY) && (set_loop_cnt++ % 1000 == 0)) {
    TRACE_NOCRLF("HALL(%0d): ", FLYSKY_HALL_BAUDRATE);
    for (int idx = 0; idx < NUM_OF_NV14_CHANNELS; idx++) {
      TRACE_NOCRLF("CH%0d:%0d ", idx + 1, rx_info.servo_value[idx]);
    }
    TRACE(" ");
  }
}

void sendPulsesFrameByState(uint8_t port, uint8_t frameState)
{
  modulePulsesData[port].flysky.state = frameState;
  modulePulsesData[port].flysky.state_index = FLYSKY_MODULE_TIMEOUT;
  //setupPulsesFlySky(port); // effect immediately
}


bool isRxBindingState(uint8_t port)
{
  return moduleFlag[port] == MODULE_BIND;
}

bool isFlySkyUsbDownload(void)
{
  return rf_info.fw_state != 0;
}

void onIntmoduleSetPower(bool isPowerOn)
{
  if (DEBUG_RF_FRAME_PRINT & RF_FRAME_ONLY) TRACE("RF Power %s", isPowerOn != 0 ? "ON" : "OFF");

  if (isPowerOn) {
    INTERNAL_MODULE_ON();
  }
  else {
    INTERNAL_MODULE_OFF();
  }
}

void onIntmoduleBindReceiver(uint8_t port)
{
  resetPulsesFlySky(port);
  moduleFlag[port] = MODULE_BIND;
  modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_INIT;
}

void onFlySkyReceiverPulseMode(uint8_t port)
{
  modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_SET_RX_PWM_PPM;
}

void onFlySkyReceiverPulsePort(uint8_t port)
{
  modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_SET_RX_IBUS_SBUS;
}

void onIntmoduleReceiverSetFrequency(uint8_t port)
{
  sendPulsesFrameByState(port, FLYSKY_MODULE_STATE_SET_RX_FREQUENCY);
}

void onIntmoduleReceiverSetPulse(uint8_t port, uint8_t mode_and_port) // mode_and_port = 0,1,2,3
{
  if ((DEBUG_RF_FRAME_PRINT & TX_FRAME_ONLY)) TRACE("PulseMode+Port: %0d", mode_and_port);
  sendPulsesFrameByState(port, FLYSKY_MODULE_STATE_SET_RX_PWM_PPM);
}

void onFlySkyTransmitterPower(uint8_t port, uint8_t dBmValue)
{
  tx_working_power = dBmValue;
  modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_SET_TX_POWER;
}


void onFlySkyUpdateReceiverFirmwareStart(uint8_t port)
{
  modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_UPDATE_RX_FIRMWARE;
}

void onIntmoduleUsbDownloadStart(uint8_t port)
{
  modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_UPDATE_RF_FIRMWARE;
}

void onFlySkyUsbDownloadStart(uint8_t fw_state)
{
  rf_info.fw_state = fw_state;
}

void onFlySkyUpdateTransmitterProtocol(uint8_t port)
{
  modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_UPDATE_RF_PROTOCOL;
}


void onFlySkyGetReceiverFirmwareInfo(uint8_t port)
{
  modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_GET_RX_FIRMWARE_INFO;
}

void onFlySkyGetTransmitterFirmwareInfo(uint8_t port)
{
  modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_GET_RF_FIRMWARE_INFO;
}

void onFlySkyGetReceiverFirmwareVersion(uint8_t port)
{
  modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_GET_RX_FW_VERSION;
}

void onFlySkyGetTransmitterFirmwareVersion(uint8_t port)
{
  modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_GET_RF_FW_VERSION;
}

void initFlySkyArray(uint8_t port)
{
  modulePulsesData[port].flysky.ptr = modulePulsesData[port].flysky.pulses;
  modulePulsesData[port].flysky.crc = 0;
}

inline void putFlySkyByte(uint8_t port, uint8_t byte)
{
  if (END == byte) {
    *modulePulsesData[port].flysky.ptr++ = ESC;
    *modulePulsesData[port].flysky.ptr++ = ESC_END;
  }
  else if (ESC == byte) {
    *modulePulsesData[port].flysky.ptr++ = ESC;
    *modulePulsesData[port].flysky.ptr++ = ESC_ESC;
  }
  else {
    *modulePulsesData[port].flysky.ptr++ = byte;
  }
}

void putFlySkyFrameByte(uint8_t port, uint8_t byte)
{
  modulePulsesData[port].flysky.crc += byte;
  putFlySkyByte(port, byte);
}

void putFlySkyFrameHead(uint8_t port)
{
  *modulePulsesData[port].flysky.ptr++ = END;
}

void putFlySkyFrameIndex(uint8_t port)
{
  putFlySkyFrameByte(port, modulePulsesData[port].flysky.frame_index);
}

void putFlySkyFrameCrc(uint8_t port)
{
  putFlySkyByte(port, modulePulsesData[port].flysky.crc ^ 0xff);
}

void putFlySkyFrameTail(uint8_t port)
{
  *modulePulsesData[port].flysky.ptr++ = END;
}


void putFlySkyRfInit(uint8_t port)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  putFlySkyFrameByte(port, COMMAND_ID_RF_INIT);
}

void putFlySkyBindReceiver(uint8_t port)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  putFlySkyFrameByte(port, COMMAND_ID_BIND);
  putFlySkyFrameByte(port, rf_info.bind_power);
  putFlySkyFrameByte(port, rf_info.id[0]);
  putFlySkyFrameByte(port, rf_info.id[1]);
  putFlySkyFrameByte(port, rf_info.id[2]);
  putFlySkyFrameByte(port, rf_info.id[3]);
}

void putFlySkySetReceiverID(uint8_t port)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  putFlySkyFrameByte(port, COMMAND_ID_SET_RECEIVER_ID);
  putFlySkyFrameByte(port, gRomData.rx_id[0]); // receiver ID (byte 0)
  putFlySkyFrameByte(port, gRomData.rx_id[1]); // receiver ID (byte 1)
  putFlySkyFrameByte(port, gRomData.rx_id[2]); // receiver ID (byte 2)
  putFlySkyFrameByte(port, gRomData.rx_id[3]); // receiver ID (byte 3)
}

void putFlySkyGetReceiverConfig(uint8_t port)
{
  putFlySkyFrameByte(port, FRAME_TYPE_ANSWER);
  putFlySkyFrameByte(port, COMMAND_ID_RF_GET_CONFIG);
  putFlySkyFrameByte(port, GET_FLYSKY_PWM_PPM);  // 00:PWM, 01:PPM
  putFlySkyFrameByte(port, GET_FLYSKY_IBUS_SBUS);// 00:I-BUS, 01:S-BUS
  putFlySkyFrameByte(port, gRomData.rx_freq[0] < 50 ? 50 : gRomData.rx_freq[0]); // receiver servo freq bit[7:0]
  putFlySkyFrameByte(port, gRomData.rx_freq[1]); // receiver servo freq bit[15:8]
}

void putFlySkySetReceiverPulseMode(uint8_t port)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  putFlySkyFrameByte(port, COMMAND_ID_SET_RX_PWM_PPM);
  putFlySkyFrameByte(port, GET_FLYSKY_PWM_PPM); // 00:PWM, 01:PPM
}

void putFlySkySetReceiverPort(uint8_t port)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  putFlySkyFrameByte(port, COMMAND_ID_SET_RX_IBUS_SBUS);
  putFlySkyFrameByte(port, GET_FLYSKY_IBUS_SBUS); // 0x00:I-BUS, 0x01:S-BUS
}

void putFlySkySetReceiverServoFreq(uint8_t port)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  putFlySkyFrameByte(port, COMMAND_ID_SET_RX_SERVO_FREQ);
  putFlySkyFrameByte(port, gRomData.rx_freq[0]); // receiver servo freq bit[7:0]
  putFlySkyFrameByte(port, gRomData.rx_freq[1]); // receiver servo freq bit[15:8]
}

void putFlySkyGetFirmwareInfo(uint8_t port, uint8_t fw_word)
{
#ifdef RF_PROTOCOL_V13
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  putFlySkyFrameByte(port, COMMAND_ID_GET_FIRMWARE_INFO);
  putFlySkyFrameByte(port, fw_word); // 0x00:RX firmware, 0x01:RF firmware
#endif
}

void putFlySkySetPowerdBm(uint8_t port, uint8_t dBm)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  putFlySkyFrameByte(port, COMMAND_ID0D_SET_TX_POWER);
  putFlySkyFrameByte(port, dBm); // 0x00:RX firmware, 0x01:RF firmware
}

void putFlySkyGetFirmwareVersion(uint8_t port, uint8_t fw_word)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  putFlySkyFrameByte(port, COMMAND_ID_GET_FIRMWARE_REV);
  putFlySkyFrameByte(port, fw_word); // 0x00:RX firmware, 0x01:RF firmware
}

void putFlySkySendChannelData(uint8_t port)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_NACK);
  putFlySkyFrameByte(port, COMMAND_ID_SEND_CHANNEL_DATA);

  if (0 /* TODO send failsafe*/) {
    putFlySkyFrameByte(port, 0x01);
    uint8_t channels_start = g_model.moduleData[port].channelsStart;
    uint8_t channels_count = min<unsigned int>(NUM_OF_NV14_CHANNELS, channels_start + 8 + g_model.moduleData[port].channelsCount);
    putFlySkyFrameByte(port, channels_count);
    for (uint8_t channel = channels_start; channel < channels_count; channel++) {
      int16_t failsafeValue = g_model.moduleData[port].failsafeChannels[channel];
      uint16_t pulseValue = limit<uint16_t>(900, 900 + ((2100 - 900) * (failsafeValue + 1024) / 2048), 2100);
      putFlySkyFrameByte(port, pulseValue & 0xff);
      putFlySkyFrameByte(port, pulseValue >> 8);
    }
  }
  else {
    putFlySkyFrameByte(port, 0x00);
    uint8_t channels_start = g_model.moduleData[port].channelsStart;
    uint8_t channels_count = min<unsigned int>(NUM_OF_NV14_CHANNELS, channels_start + 8 + g_model.moduleData[port].channelsCount);
    putFlySkyFrameByte(port, channels_count);
    for (uint8_t channel = channels_start; channel < channels_count; channel++) {
      int channelValue = channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
      uint16_t pulseValue = limit<uint16_t>(900, 900 + ((2100 - 900) * (channelValue + 1024) / 2048), 2100);
      putFlySkyFrameByte(port, pulseValue & 0xff);
      putFlySkyFrameByte(port, pulseValue >> 8);
    }
  }
}

void putFlySkyUpdateFirmwareStart(uint8_t port, uint8_t fw_word)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  if (fw_word == FLYSKY_RX_FIRMWARE) {
    fw_word = COMMAND_ID20_UPDATE_RX_FIRMWARE;
  }
  else {
    fw_word = COMMAND_ID0C_UPDATE_RF_FIRMWARE;
  }
  putFlySkyFrameByte(port, fw_word); // 0x00:RX firmware, 0x01:RF firmware
}

void putFlySkyUpdateRfProtocol(uint8_t port)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  putFlySkyFrameByte(port, COMMAND_ID_SET_RF_PROTOCOL);
  putFlySkyFrameByte(port, rf_info.protocol); // 0x00:AFHDS1 0x01:AFHDS2 0x02:AFHDS2A
}


void incrFlySkyFrame(uint8_t port)
{
  if (++modulePulsesData[port].flysky.frame_index == 0)
    modulePulsesData[port].flysky.frame_index = 1;
}

bool checkFlySkyFrameCrc(const uint8_t * ptr, uint8_t size)
{
  uint8_t crc = 0;

  for (uint8_t i = 0; i < size; i++) {
    crc += ptr[i];
  }

  if (DEBUG_RF_FRAME_PRINT & RF_FRAME_ONLY) {
    if (ptr[2] != 0x06 || (set_loop_cnt++ % 100 == 0)) {
      TRACE_NOCRLF("RF(%0d): C0", INTERNAL_MODULE_BAUDRATE);
      for (int idx = 0; idx <= size; idx++) {
        TRACE_NOCRLF(" %02X", ptr[idx]);
      }
      TRACE(" C0;");

      if ((crc ^ 0xff) != ptr[size]) {
        TRACE("ErrorCRC %02X especting %02X", crc ^ 0xFF, ptr[size]);
      }
    }
  }

  return (crc ^ 0xff) == ptr[size];
}

void parseFlySkyFeedbackFrame(uint8_t port)
{
  const uint8_t * ptr = modulePulsesData[port].flysky.telemetry;
  if (*ptr++ != END)
    return;

  uint8_t frame_number = *ptr++;
  uint8_t frame_type = *ptr++;
  uint8_t command_id = *ptr++;
  uint8_t first_para = *ptr++;
  uint8_t * p_data = NULL;

  if (!checkFlySkyFrameCrc(modulePulsesData[port].flysky.telemetry + 1, modulePulsesData[port].flysky.telemetry_index - 2)) {
    return;
  }

  switch (command_id) {
    default:
      if (moduleFlag[port] == MODULE_NORMAL_MODE
          && modulePulsesData[port].flysky.state >= FLYSKY_MODULE_STATE_IDLE) {
        modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_DEFAULT;
        if (DEBUG_RF_FRAME_PRINT & RF_FRAME_ONLY) TRACE("State back to channel data");
      }
      break;

    case COMMAND_ID_RF_INIT: {
      if (first_para == 0x01) { // action only RF ready

        if (moduleFlag[port] == MODULE_BIND) {
          modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_BIND;
          incrFlySkyFrame(port);
        }

        /*else if (moduleFlag[port] == MODULE_RANGECHECK) {

        }*/

        if (modulePulsesData[port].flysky.state == FLYSKY_MODULE_STATE_INIT) {
          modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_SET_RECEIVER_ID;
          incrFlySkyFrame(port);
        }
      }
      else modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_IDLE;
      break;
    }

    case COMMAND_ID_BIND: {
      if (frame_type != FRAME_TYPE_ANSWER) {
        modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_IDLE;
        return;
      }
      g_model.header.modelId[port] = ptr[2];
      gRomData.rx_id[0] = first_para;
      gRomData.rx_id[1] = *ptr++;
      gRomData.rx_id[2] = *ptr++;
      gRomData.rx_id[3] = *ptr++;
      if (DEBUG_RF_FRAME_PRINT & RF_FRAME_ONLY)
        TRACE("New Rx ID: %02X %02X %02X %02X", gRomData.rx_id[0], gRomData.rx_id[1], gRomData.rx_id[2], gRomData.rx_id[3]);
      SET_DIRTY();
      resetPulsesFlySky(port);
      modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_INIT;
      break;

      case COMMAND_ID_RF_GET_CONFIG:
        modulePulsesData[port].flysky.frame_index = frame_number;
      sendPulsesFrameByState(port, FLYSKY_MODULE_STATE_GET_RECEIVER_CONFIG);
      break;
    }

    case COMMAND_ID_RX_SENSOR_DATA: {
      // rx_sensor_info.sensor_id = *ptr++; // TBC: in protocol doc, but no such byte in sample data
      if (first_para == FLYSKY_SENSOR_RX_VOLTAGE) {
        rx_sensor_info.voltage[0] = *ptr++;
        rx_sensor_info.voltage[1] = *ptr++;
      }

      else if (first_para == FLYSKY_SENSOR_GPS) {
        p_data = &rx_sensor_info.gps_info.position_fix;
        for (int idx = 17; idx > 0; idx--) {
          *p_data++ = *ptr++;
        }
      }

      else if (first_para == FLYSKY_SENSOR_RX_SIGNAL) {
        rx_sensor_info.signal = *ptr++;
      }

      else if (first_para > FLYSKY_SENSOR_RX_SIGNAL && first_para < FLYSKY_SENSOR_GPS) {
        p_data = rx_sensor_info.rssi + (first_para - FLYSKY_SENSOR_RX_RSSI) * 2;
        p_data[0] = *ptr++;
        p_data[1] = *ptr++;
      }

      if (moduleFlag[port] == MODULE_RANGECHECK) {
        moduleFlag[port] = MODULE_NORMAL_MODE;
        onFlySkyTransmitterPower(port, 0); // set power 0
        break;
      }

      if (moduleFlag[port] == MODULE_NORMAL_MODE && modulePulsesData[port].flysky.state >= FLYSKY_MODULE_STATE_IDLE) {
        modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_DEFAULT;
      }
      break;
    }

    case COMMAND_ID_SET_RECEIVER_ID: {
      modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_DEFAULT;
      return;
    }

    case COMMAND_ID0D_SET_TX_POWER: {
      modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_INIT;
      break;
    }

    case COMMAND_ID_SET_RX_PWM_PPM: {
      modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_SET_RX_IBUS_SBUS;
      break;
    }

    case COMMAND_ID_SET_RX_IBUS_SBUS: {
      modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_SET_RX_FREQUENCY;
      break;
    }

    case COMMAND_ID_SET_RX_SERVO_FREQ: {
      modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_DEFAULT;
      break;
    }

    case COMMAND_ID0C_UPDATE_RF_FIRMWARE: {
      rf_info.fw_state = FLYSKY_MODULE_STATE_UPDATE_RF_FIRMWARE;
      modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_IDLE;
      break;
    }
  }
}

extern uint8_t intmoduleGetByte(uint8_t * byte);

static STRUCT_HALL rfProtocolRx = {0};
static uint32_t rfRxCount = 0;

bool isRfProtocolRxMsgOK(void)
{
  bool isMsgOK = (0 != rfRxCount);
  rfRxCount = 0;
  return isMsgOK && isFlySkyUsbDownload();
}

#if !defined(SIMU)
void checkFlySkyFeedback(uint8_t port)
{
  uint8_t byte;

  while (intmoduleGetByte(&byte)) {
    //if ( modulePulsesData[port].flysky.state == FLYSKY_MODULE_STATE_IDLE
    //  && rf_info.fw_state == FLYSKY_MODULE_STATE_UPDATE_RF_FIRMWARE )
    {
        Parse_Character(&rfProtocolRx, byte );
        if ( rfProtocolRx.msg_OK )
        {
            rfRxCount++;
            rfProtocolRx.msg_OK = 0;
            uint8_t *pt = (uint8_t*)&rfProtocolRx;
            pt[rfProtocolRx.length + 3] = rfProtocolRx.checkSum & 0xFF;
            pt[rfProtocolRx.length + 4] = rfProtocolRx.checkSum >> 8;

            if((DEBUG_RF_FRAME_PRINT & RF_FRAME_ONLY)) {
                TRACE("RF: %02X %02X %02X ...%04X; CRC:%04X", pt[0], pt[1], pt[2],
                      rfProtocolRx.checkSum, calc_crc16(pt, rfProtocolRx.length+3));
            }

            if ( 0x01 == rfProtocolRx.length &&
               ( 0x05 == rfProtocolRx.data[0] || 0x06 == rfProtocolRx.data[0]) )
            {
                modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_INIT;
                rf_info.fw_state = 0;
            }

            usbDownloadTransmit(pt, rfProtocolRx.length + 5);
        }
        //continue;
    }

    if (byte == END && modulePulsesData[port].flysky.telemetry_index > 0) {
      parseFlySkyFeedbackFrame(port);
      modulePulsesData[port].flysky.telemetry_index = 0;
    }
    else {
      if (byte == ESC) {
        modulePulsesData[port].flysky.esc_state = 1;
      }
      else {
        if (modulePulsesData[port].flysky.esc_state) {
          modulePulsesData[port].flysky.esc_state = 0;
          if (byte == ESC_END)
            byte = END;
          else if (byte == ESC_ESC)
            byte = ESC;
        }
        modulePulsesData[port].flysky.telemetry[modulePulsesData[port].flysky.telemetry_index++] = byte;
        if (modulePulsesData[port].flysky.telemetry_index >= sizeof(modulePulsesData[port].flysky.telemetry)) {
          // TODO buffer is full, log an error?
          modulePulsesData[port].flysky.telemetry_index = 0;
        }
      }
    }
  }
}
#endif

void resetPulsesFlySky(uint8_t port)
{
  modulePulsesData[port].flysky.frame_index = 1;
  modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_SET_TX_POWER;
  modulePulsesData[port].flysky.state_index = 0;
  modulePulsesData[port].flysky.esc_state = 0;
  moduleFlag[port] = MODULE_NORMAL_MODE;
  uint16_t rx_freq = g_model.moduleData[port].romData.rx_freq[0];
  rx_freq += (g_model.moduleData[port].romData.rx_freq[1] * 256);
  if (50 > rx_freq || 400 < rx_freq) {
    g_model.moduleData[port].romData.rx_freq[0] = 50;
  }
}

void setupPulsesFlySky(uint8_t port)
{
#if !defined(SIMU)
  checkFlySkyFeedback(port);
#endif

  initFlySkyArray(port);
  putFlySkyFrameHead(port);
  putFlySkyFrameIndex(port);

  if (modulePulsesData[port].flysky.state < FLYSKY_MODULE_STATE_DEFAULT) {

    if (++modulePulsesData[port].flysky.state_index >= FLYSKY_MODULE_TIMEOUT / PXX_PERIOD_DURATION) {

      modulePulsesData[port].flysky.state_index = 0;

      switch (modulePulsesData[port].flysky.state) {

        case FLYSKY_MODULE_STATE_INIT:
          putFlySkyRfInit(port);
          break;

        case FLYSKY_MODULE_STATE_BIND:
          putFlySkyBindReceiver(port);
          break;

        case FLYSKY_MODULE_STATE_SET_RECEIVER_ID:
          putFlySkySetReceiverID(port);
          break;

        case FLYSKY_MODULE_STATE_GET_RECEIVER_CONFIG:
          putFlySkyGetReceiverConfig(port);
          modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_INIT;
          break;

        case FLYSKY_MODULE_STATE_SET_TX_POWER:
          putFlySkySetPowerdBm(port, tx_working_power);
          break;

        case FLYSKY_MODULE_STATE_SET_RX_PWM_PPM:
          putFlySkySetReceiverPulseMode(port);
          break;

        case FLYSKY_MODULE_STATE_SET_RX_IBUS_SBUS:
          putFlySkySetReceiverPort(port);
          break;

        case FLYSKY_MODULE_STATE_SET_RX_FREQUENCY:
          putFlySkySetReceiverServoFreq(port);
          break;

        case FLYSKY_MODULE_STATE_UPDATE_RF_PROTOCOL:
          putFlySkyUpdateRfProtocol(port);
          break;

        case FLYSKY_MODULE_STATE_UPDATE_RX_FIRMWARE:
          putFlySkyUpdateFirmwareStart(port, FLYSKY_RX_FIRMWARE);
          break;

        case FLYSKY_MODULE_STATE_UPDATE_RF_FIRMWARE:
          putFlySkyUpdateFirmwareStart(port, FLYSKY_RF_FIRMWARE);
          break;

        case FLYSKY_MODULE_STATE_GET_RX_FIRMWARE_INFO:
          putFlySkyGetFirmwareInfo(port, FLYSKY_RX_FIRMWARE);
          break;

        case FLYSKY_MODULE_STATE_GET_RF_FIRMWARE_INFO:
          putFlySkyGetFirmwareInfo(port, FLYSKY_RF_FIRMWARE);
          break;

        case FLYSKY_MODULE_STATE_GET_RX_FW_VERSION:
          putFlySkyGetFirmwareVersion(port, FLYSKY_RX_FIRMWARE);
          break;

        case FLYSKY_MODULE_STATE_GET_RF_FW_VERSION:
          putFlySkyGetFirmwareVersion(port, FLYSKY_RF_FIRMWARE);
          break;

        case FLYSKY_MODULE_STATE_IDLE:
          modulePulsesData[port].pxx_uart.ptr = modulePulsesData[port].pxx_uart.pulses;
          return;

        default:
          modulePulsesData[port].flysky.state = FLYSKY_MODULE_STATE_INIT;
          initFlySkyArray(port);
          if ((DEBUG_RF_FRAME_PRINT & TX_FRAME_ONLY)) {
            TRACE("State back to INIT\r\n");
          }
          return;
      }
    }
    else {
      initFlySkyArray(port);
      return;
    }
  }
  else {
    putFlySkySendChannelData(port);
    incrFlySkyFrame(port);
  }

  putFlySkyFrameCrc(port);
  putFlySkyFrameTail(port);

  if ((DEBUG_RF_FRAME_PRINT & TX_FRAME_ONLY)) {
    /* print each command, except channel data by interval */
    uint8_t * data = modulePulsesData[port].pxx_uart.pulses;
    if (data[3] != COMMAND_ID_SEND_CHANNEL_DATA || (set_loop_cnt++ % 100 == 0)) {
      uint8_t size = modulePulsesData[port].pxx_uart.ptr - data;
      TRACE_NOCRLF("TX(State%0d)%0dB:", modulePulsesData[port].flysky.state, size);
      for (int idx = 0; idx < size; idx++) {
        TRACE_NOCRLF(" %02X", data[idx]);
      }
      TRACE(";");
    }
  }
}

#if !defined(SIMU)
void usbDownloadTransmit(uint8_t *buffer, uint32_t size)
{
    for (int idx = 0; idx < size; idx++)
    {
        usbSerialPutc(buffer[idx]);
    }
}
#endif
