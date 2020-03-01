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
#include "flysky.h"

#define FRAME_TYPE_REQUEST_ACK          0x01
#define FRAME_TYPE_REQUEST_NACK         0x02
#define FRAME_TYPE_ANSWER               0x10

enum FlySkyModuleCommandID {
  CMD_NONE,
  CMD_RF_INIT,
  CMD_BIND,
  CMD_SET_RECEIVER_ID,
  CMD_RF_GET_CONFIG,
  CMD_SEND_CHANNEL_DATA,
  CMD_RX_SENSOR_DATA,
  CMD_SET_RX_PWM_PPM,
  CMD_SET_RX_SERVO_FREQ,
  CMD_GET_VERSION_INFO,
  CMD_SET_RX_IBUS_SBUS,
  CMD_SET_RX_IBUS_SERVO_EXT,
  CMD_UPDATE_RF_FIRMWARE = 0x0C,
  CMD_SET_TX_POWER = 0x0D,
  CMD_SET_RF_PROTOCOL,
  CMD_TEST_RANGE,
  CMD_TEST_RF_RESERVED,
  CMD_UPDATE_RX_FIRMWARE = 0x20,
  CMD_LAST
};
#define IS_VALID_COMMAND_ID(id)         ((id) < CMD_LAST)

enum DEBUG_RF_FRAME_PRINT_E {
  FRAME_PRINT_OFF,
  RF_FRAME_ONLY,
  TX_FRAME_ONLY,
  BOTH_FRAME_PRINT
};
#define DEBUG_RF_FRAME_PRINT            FRAME_PRINT_OFF
#define FLYSKY_MODULE_TIMEOUT           155 /* ms */
#define NUM_OF_NV14_CHANNELS            (14)
#define VALID_CH_DATA(v)                ((v) > 900 && (v) < 2100)
#define FAILSAVE_SEND_COUNTER_MAX       (400)

#define modelData                        g_model.moduleData[INTERNAL_MODULE].flysky
#define SET_DIRTY()                     storageDirty(EE_MODEL)

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
#define GET_FLYSKY_PWM_PPM    (modelData.mode < 2 ? FLYSKY_PWM: FLYSKY_PPM)
#define GET_FLYSKY_IBUS_SBUS  (modelData.mode & 1 ? FLYSKY_SBUS: FLYSKY_IBUS)

typedef struct RX_FLYSKY_IBUS_S {
  uint8_t id[2];
  uint8_t channel[2];
} rx_ibus_t;

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
  uint8_t fw_state; // 0: normal, COMMAND_ID0C_UPDATE_RF_FIRMWARE or CMD_UPDATE_FIRMWARE_END
  fw_info_t fw_info;
} rf_info_t;

typedef struct RX_INFO_S {
  int16_t servo_value[NUM_OF_NV14_CHANNELS];
  rx_ibus_t ibus;
  fw_info_t fw_info;
} rx_info_t;

uint8_t tx_working_power = 90;
static STRUCT_HALL rfProtocolRx = {0};
static uint32_t rfRxCount = 0;
static uint8_t lastState = STATE_IDLE;
static rf_info_t rf_info = {
  .id               = {8, 8, 8, 8},
  .bind_power       = BIND_LOW_POWER,
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

void traceBuffer(const char* message, const uint8_t* data, uint32_t len) {
  char buffer[256];
  char *pos = buffer;
  for (uint32_t i = 0; i < len; i++) {
    pos += std::snprintf(pos, buffer + sizeof(buffer) - pos, "%02X ", data[i]);
  }
  (*pos) = 0;
  TRACE("%s size = %d data %s", message, len, buffer);
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
    traceBuffer("HALL CHANNEL DATA", (uint8_t*)rx_info.servo_value, NUM_OF_NV14_CHANNELS*2);
  }
}

bool isFlySkyUsbDownload(void)
{
  return rf_info.fw_state != 0;
}

void usbSetFrameTransmit(uint8_t packetID, uint8_t *dataBuf, uint32_t nBytes)
{
    // send to host via usb
    uint8_t *pt = (uint8_t*)&rfProtocolRx;
   // rfProtocolRx.head = HALL_PROTOLO_HEAD;
    rfProtocolRx.hallID.hall_Id.packetID = packetID;//0x08;
    rfProtocolRx.hallID.hall_Id.senderID = 0x03;
    rfProtocolRx.hallID.hall_Id.receiverID = 0x02;

    if ( packetID == 0x08 ) {
      uint8_t fwVerision[40];
      for(uint32_t idx = 40; idx > 0; idx--)
      {
          if ( idx <= nBytes ) {
              fwVerision[idx-1] = dataBuf[idx-1];
          }
          else fwVerision[idx-1] = 0;
      }
      dataBuf = fwVerision;
      nBytes = 40;
    }

    rfProtocolRx.length = nBytes;

    TRACE("To USB: 55 %02X %02X ", rfProtocolRx.hallID.ID, nBytes);

    for ( uint32_t idx = 0; idx < nBytes; idx++ )
    {
        rfProtocolRx.data[idx] = dataBuf[idx];
    }

    traceBuffer("", dataBuf, nBytes);
#if !defined(SIMU)
    uint16_t checkSum = calc_crc16(pt, rfProtocolRx.length+3);
    TRACE("CRC: %04X;", checkSum);

    pt[rfProtocolRx.length + 3] = checkSum & 0xFF;
    pt[rfProtocolRx.length + 4] = checkSum >> 8;

    usbDownloadTransmit(pt, rfProtocolRx.length + 5);
#endif
}


void setFlyskyState(uint8_t port, uint8_t state) {
  if(intmodulePulsesData.afhds2.state != state) {
    TRACE("AFHDS 2 set state %d", state);
    intmodulePulsesData.afhds2.state = state;
  }
}

void onFlySkyUsbDownloadStart(uint8_t fw_state)
{
  rf_info.fw_state = fw_state;
}

void getVersionInfoAFHDS2(uint8_t port, uint8_t isRfTransfer)
{
  lastState = intmodulePulsesData.afhds2.state;
  if ( isRfTransfer != 0 )
    setFlyskyState(port, STATE_GET_RF_VERSION_INFO);
  else setFlyskyState(port, STATE_GET_RX_VERSION_INFO);
}

void initFlySkyArray(uint8_t port)
{
  intmodulePulsesData.afhds2.ptr = intmodulePulsesData.afhds2.pulses;
  intmodulePulsesData.afhds2.crc = 0;
}

inline void putFlySkyByte(uint8_t port, uint8_t byte)
{
  if (END == byte) {
    *intmodulePulsesData.afhds2.ptr++ = ESC;
    *intmodulePulsesData.afhds2.ptr++ = ESC_END;
  }
  else if (ESC == byte) {
    *intmodulePulsesData.afhds2.ptr++ = ESC;
    *intmodulePulsesData.afhds2.ptr++ = ESC_ESC;
  }
  else {
    *intmodulePulsesData.afhds2.ptr++ = byte;
  }
}

void putFlySkyFrameByte(uint8_t port, uint8_t byte)
{
  intmodulePulsesData.afhds2.crc += byte;
  putFlySkyByte(port, byte);
}

void putFlySkyFrameBytes(uint8_t port, uint8_t* data, int length)
{
  for(int i = 0; i < length; i++) {
    intmodulePulsesData.afhds2.crc += data[i];
    putFlySkyByte(port, data[i]);
  }
}


void putFlySkyFrameHead(uint8_t port)
{
  *intmodulePulsesData.afhds2.ptr++ = END;
}

void putFlySkyFrameIndex(uint8_t port)
{
  putFlySkyFrameByte(port, intmodulePulsesData.afhds2.frame_index);
}

void putFlySkyFrameCrc(uint8_t port)
{
  putFlySkyByte(port, intmodulePulsesData.afhds2.crc ^ 0xff);
}

void putFlySkyFrameTail(uint8_t port)
{
  *intmodulePulsesData.afhds2.ptr++ = END;
}

void putFlySkyGetFirmwareVersion(uint8_t port, uint8_t fw_word)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  putFlySkyFrameByte(port, CMD_GET_VERSION_INFO);
  putFlySkyFrameByte(port, fw_word); // 0x00:RX firmware, 0x01:RF firmware
}

void putFlySkySendChannelData(uint8_t port)
{
  uint16_t pulseValue = 0;
  uint8_t channels_start = g_model.moduleData[port].channelsStart;
  uint8_t channels_last = channels_start + 8 + g_model.moduleData[port].channelsCount;
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_NACK);
  putFlySkyFrameByte(port, CMD_SEND_CHANNEL_DATA);
  if ( moduleState[port].counter-- == 0 ) {
    moduleState[port].counter = FAILSAVE_SEND_COUNTER_MAX;
    putFlySkyFrameByte(port, 0x01);
    putFlySkyFrameByte(port, channels_last - channels_start);
    for (uint8_t channel = channels_start; channel < channels_last; channel++) {
      if ( g_model.moduleData[port].failsafeMode == FAILSAFE_CUSTOM) {
        int16_t failsafeValue = g_model.failsafeChannels[channel];
        pulseValue = limit<uint16_t>(0, 988 + ((failsafeValue + 1024) / 2), 0xfff);
      }
      else if (g_model.moduleData[port].failsafeMode == FAILSAFE_HOLD) {
        //protocol uses hold by default
        pulseValue = 0xfff;
      }
      else  {
        int16_t failsafeValue = -1024 + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
        pulseValue = limit<uint16_t>(0, 988 + ((failsafeValue + 1024) / 2), 0xfff);
      }
      putFlySkyFrameByte(port, pulseValue & 0xff);
      putFlySkyFrameByte(port, pulseValue >> 8);
    }
    if (DEBUG_RF_FRAME_PRINT & RF_FRAME_ONLY) {
        TRACE("------FAILSAFE------");
    }
  }
  else {
    putFlySkyFrameByte(port, 0x00);
    putFlySkyFrameByte(port, channels_last - channels_start);
    for (uint8_t channel = channels_start; channel < channels_last; channel++) {
      int channelValue = channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
      pulseValue = limit<uint16_t>(0, 988 + ((channelValue + 1024) / 2), 0xfff);
      putFlySkyFrameByte(port, pulseValue & 0xff);
      putFlySkyFrameByte(port, pulseValue >> 8);
    }
  }
}

void putFlySkyUpdateFirmwareStart(uint8_t port, uint8_t fw_word)
{
  putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
  if (fw_word == FLYSKY_RX_FIRMWARE) {
    fw_word = CMD_UPDATE_RX_FIRMWARE;
  }
  else {
    fw_word = CMD_UPDATE_RF_FIRMWARE;
  }
  putFlySkyFrameByte(port, fw_word);
}

void incrFlySkyFrame(uint8_t port)
{
  if (++intmodulePulsesData.afhds2.frame_index == 0)
    intmodulePulsesData.afhds2.frame_index = 1;
}

bool checkFlySkyFrameCrc(const uint8_t * ptr, uint8_t size)
{
  uint8_t crc = 0;

  for (uint8_t i = 0; i < size; i++) {
    crc += ptr[i];
  }

  if (DEBUG_RF_FRAME_PRINT & RF_FRAME_ONLY) {
    if (ptr[2] != 0x06 || (set_loop_cnt++ % 50 == 0)) {
      //TRACE("RF(%0d): C0", AFHDS2_BAUDRATE);
      traceBuffer("RF: C0", ptr, size);
      //TRACE(" C0;");
    }
  }
  if ((crc ^ 0xff) != ptr[size]) {
    TRACE("ErrorCRC %02X especting %02X", crc ^ 0xFF, ptr[size]);
  }
  return (crc ^ 0xff) == ptr[size];
}


void parseResponse(uint8_t port)
{
  const uint8_t * ptr = intmodulePulsesData.afhds2.telemetry;
  uint8_t dataLen = intmodulePulsesData.afhds2.telemetry_index;

  if (*ptr++ != END || dataLen < 2 )
    return;
  uint8_t frame_number = *ptr++;
  uint8_t frame_type = *ptr++;
  uint8_t command_id = *ptr++;
  uint8_t first_para = *ptr++;
  // uint8_t * p_data = NULL;

  dataLen -= 2;
  if (!checkFlySkyFrameCrc(intmodulePulsesData.afhds2.telemetry + 1, dataLen)) {
    return;
  }

  if ((moduleState[port].mode != MODULE_MODE_BIND) && (frame_type == FRAME_TYPE_ANSWER)
       && (intmodulePulsesData.afhds2.frame_index -1) != frame_number ) {
      return;
  }
  else if ( frame_type == FRAME_TYPE_REQUEST_ACK) {
     intmodulePulsesData.afhds2.frame_index = frame_number;
  }
  switch (command_id) {
    default:
      if (moduleState[port].mode == MODULE_MODE_NORMAL && intmodulePulsesData.afhds2.state >= STATE_IDLE) {
        setFlyskyState(port, STATE_DEFAULT_AFHDS2);
        if (DEBUG_RF_FRAME_PRINT & RF_FRAME_ONLY) TRACE("State back to channel data");
      }
      break;

    case CMD_RF_INIT: {
      if (first_para == 0x01) { // action only RF ready
          if (moduleState[port].mode == MODULE_MODE_BIND) setFlyskyState(port, STATE_BIND);
          else setFlyskyState(port, STATE_SET_RECEIVER_ID);
      }
      else {
        //Try one more time;
        uint8_t mode = MODULE_MODE_NORMAL;
        if (moduleState[port].mode == MODULE_MODE_BIND) mode = MODULE_MODE_BIND;
        if (moduleState[port].mode == MODULE_MODE_RANGECHECK) mode = MODULE_MODE_RANGECHECK;
        resetPulsesAFHDS2(port, mode);
        setFlyskyState(port, STATE_INIT);
      }
      break; }

    case CMD_BIND: {
      if (frame_type != FRAME_TYPE_ANSWER) {
        setFlyskyState(port, STATE_IDLE);
        return;
      }
      if (moduleState[port].mode == MODULE_MODE_BIND) moduleState[port].setMode(MODULE_MODE_NORMAL);
      g_model.header.modelId[port] = ptr[2];
      *((uint32_t*)modelData.rx_id) = *((uint32_t*)(ptr - 1));
      TRACE("New Rx ID: %02X %02X %02X %02X", modelData.rx_id[0], modelData.rx_id[1], modelData.rx_id[2], modelData.rx_id[3]);
      SET_DIRTY();
      resetPulsesAFHDS2(port, MODULE_MODE_NORMAL);
      setFlyskyState(port, STATE_INIT);
      break;
    }
    case CMD_RF_GET_CONFIG: {
      setFlyskyState(port, STATE_GET_RECEIVER_CONFIG);
      intmodulePulsesData.afhds2.timeout = FLYSKY_MODULE_TIMEOUT;
      break;
    }

    case CMD_RX_SENSOR_DATA: {
      flySkyNv14ProcessTelemetryPacket(ptr, first_para);
      if (moduleState[port].mode == MODULE_MODE_NORMAL && intmodulePulsesData.afhds2.state == STATE_IDLE) {
        setFlyskyState(port, STATE_DEFAULT_AFHDS2);
      }
      break;
    }
    case CMD_SET_RECEIVER_ID: {
      //range check seems to be not working
      //it disconnects receiver
      //if (moduleFlag[port] == MODULE_RANGECHECK) {
      //  setFlyskyState(port, STATE_SET_RANGE_TEST);
      //}
      //else
      {
        setFlyskyState(port, STATE_DEFAULT_AFHDS2);
      }
      return;
    }
    case CMD_TEST_RANGE: {
      if(moduleState[port].mode != MODULE_MODE_RANGECHECK) resetPulsesAFHDS2(port, MODULE_MODE_NORMAL);
      else setFlyskyState(port, STATE_RANGE_TEST_RUNNING);
      break;
    }
    case CMD_SET_TX_POWER: {
      setFlyskyState(port, STATE_INIT);
      break;
    }

    case CMD_SET_RX_PWM_PPM: {
      setFlyskyState(port, STATE_SET_RX_IBUS_SBUS);
      break;
    }

    case CMD_SET_RX_IBUS_SBUS: {
      setFlyskyState(port, STATE_SET_RX_FREQUENCY);
      break;
    }

    case CMD_SET_RX_SERVO_FREQ: {
      setFlyskyState(port, STATE_DEFAULT_AFHDS2);
      break;
    }

    case CMD_UPDATE_RF_FIRMWARE: {
      rf_info.fw_state = STATE_UPDATE_RF_FIRMWARE;
      setFlyskyState(port, STATE_IDLE);
      break;
    }

    case CMD_GET_VERSION_INFO: {
      if ( dataLen > 4 ) {
        usbSetFrameTransmit(0x08, (uint8_t*)ptr, dataLen - 4 );
      }
      if ( lastState == STATE_GET_RF_VERSION_INFO || lastState == STATE_GET_RX_VERSION_INFO ) {
        lastState = STATE_INIT;
      }
      setFlyskyState(port, lastState);
      break;
    }
  }
}

bool isRfProtocolRxMsgOK(void)
{
  bool isMsgOK = (0 != rfRxCount);
  rfRxCount = 0;
  return isMsgOK && isFlySkyUsbDownload();
}

#if !defined(SIMU)
void checkResponse(uint8_t port)
{
  uint8_t byte;
  while (intmoduleFifo.pop(byte)) {
    //if ( intmodulePulsesData.afhds2.state == STATE_IDLE
    //  && rf_info.fw_state == STATE_UPDATE_RF_FIRMWARE )
    {
        parseCharacter(&rfProtocolRx, byte );
        if ( rfProtocolRx.msg_OK )
        {
            rfRxCount++;
            rfProtocolRx.msg_OK = 0;
            uint8_t *pt = (uint8_t*)&rfProtocolRx;
            //rfProtocolRx.head = HALL_PROTOLO_HEAD;
            pt[rfProtocolRx.length + 3] = rfProtocolRx.checkSum & 0xFF;
            pt[rfProtocolRx.length + 4] = rfProtocolRx.checkSum >> 8;

            if((DEBUG_RF_FRAME_PRINT & RF_FRAME_ONLY)) {
                TRACE("RF: %02X %02X %02X ...%04X; CRC:%04X", pt[0], pt[1], pt[2], rfProtocolRx.checkSum, calc_crc16(pt, rfProtocolRx.length+3));
            }

            if ( 0x01 == rfProtocolRx.length &&
               ( 0x05 == rfProtocolRx.data[0] || 0x06 == rfProtocolRx.data[0]) )
            {
                setFlyskyState(port, STATE_INIT);
                rf_info.fw_state = 0;
            }
#if !defined(SIMU)
            usbDownloadTransmit(pt, rfProtocolRx.length + 5);
#endif
        }
    }

    if (byte == END && intmodulePulsesData.afhds2.telemetry_index > 0) {
      parseResponse(port);
      intmodulePulsesData.afhds2.telemetry_index = 0;
    }
    else {
      if (byte == ESC) {
        intmodulePulsesData.afhds2.esc_state = 1;
      }
      else {
        if (intmodulePulsesData.afhds2.esc_state) {
          intmodulePulsesData.afhds2.esc_state = 0;
          if (byte == ESC_END)
            byte = END;
          else if (byte == ESC_ESC)
            byte = ESC;
        }
        intmodulePulsesData.afhds2.telemetry[intmodulePulsesData.afhds2.telemetry_index++] = byte;
        if (intmodulePulsesData.afhds2.telemetry_index >= sizeof(intmodulePulsesData.afhds2.telemetry)) {
          // TODO buffer is full, log an error?
          intmodulePulsesData.afhds2.telemetry_index = 0;
        }
      }
    }
  }
}
#endif

void resetPulsesAFHDS2(uint8_t port, uint8_t targetMode)
{
  intmodulePulsesData.afhds2.frame_index = 1;
  setFlyskyState(port, STATE_SET_TX_POWER);
  intmodulePulsesData.afhds2.timeout = 0;
  intmodulePulsesData.afhds2.esc_state = 0;
  moduleState[port].setMode(targetMode);
  uint16_t rx_freq = g_model.moduleData[port].flysky.rxFreq();
  if (50 > rx_freq || 400 < rx_freq) {
    g_model.moduleData[port].flysky.rx_freq[0] = 50;
    g_model.moduleData[port].flysky.rx_freq[1] = 0;
  }
}

void setupPulsesAFHDS2(uint8_t port)
{
#if !defined(SIMU)
  checkResponse(port);
#endif

  initFlySkyArray(port);
  putFlySkyFrameHead(port);
  putFlySkyFrameIndex(port);

  if (intmodulePulsesData.afhds2.state < STATE_DEFAULT_AFHDS2) {

    if (++intmodulePulsesData.afhds2.timeout >= FLYSKY_MODULE_TIMEOUT / 9) {

      intmodulePulsesData.afhds2.timeout = 0;
      switch (intmodulePulsesData.afhds2.state) {
        case STATE_INIT:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_RF_INIT);
        }
        break;
        case STATE_BIND:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_BIND);
          putFlySkyFrameByte(port, rf_info.bind_power);
          putFlySkyFrameBytes(port, rf_info.id, 4);
        }
        break;

        case STATE_SET_RECEIVER_ID:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_SET_RECEIVER_ID);
          traceBuffer("RX ID: ", modelData.rx_id, 4);
          putFlySkyFrameBytes(port, modelData.rx_id, 4);
        }
        break;
        case STATE_GET_RECEIVER_CONFIG:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_ANSWER);
          putFlySkyFrameByte(port, CMD_RF_GET_CONFIG);
          putFlySkyFrameByte(port, GET_FLYSKY_PWM_PPM);  // 00:PWM, 01:PPM
          putFlySkyFrameByte(port, GET_FLYSKY_IBUS_SBUS);// 00:I-BUS, 01:S-BUS
          putFlySkyFrameByte(port, modelData.rx_freq[0]); // receiver servo freq bit[7:0]
          putFlySkyFrameByte(port, modelData.rx_freq[1]); // receiver servo freq bit[15:8]
          setFlyskyState(port, STATE_INIT);
        }
        break;
        case STATE_SET_TX_POWER:
        {
          uint8_t power = moduleState[port].mode == MODULE_MODE_RANGECHECK ? 0 : tx_working_power;
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_SET_TX_POWER);
          putFlySkyFrameByte(port, power);
        }
        break;
        case STATE_SET_RANGE_TEST:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_TEST_RANGE);
          putFlySkyFrameByte(port, moduleState[port].mode == MODULE_MODE_RANGECHECK);
        }
        break;
        case STATE_RANGE_TEST_RUNNING:
        {
          if(moduleState[port].mode != MODULE_MODE_RANGECHECK) {
            //this will send stop command
            setFlyskyState(port, STATE_SET_RANGE_TEST);
          }
        }
        break;
        case STATE_SET_RX_PWM_PPM:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_SET_RX_PWM_PPM);
          putFlySkyFrameByte(port, GET_FLYSKY_PWM_PPM); // 00:PWM, 01:PPM
        }
        break;
        case STATE_SET_RX_IBUS_SBUS:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_SET_RX_IBUS_SBUS);
          putFlySkyFrameByte(port, GET_FLYSKY_IBUS_SBUS); // 0x00:I-BUS, 0x01:S-BUS
        }
        break;
        case STATE_SET_RX_FREQUENCY:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_SET_RX_SERVO_FREQ);
          putFlySkyFrameByte(port, modelData.rx_freq[0]); // receiver servo freq bit[7:0]
          putFlySkyFrameByte(port, modelData.rx_freq[1]); // receiver servo freq bit[15:8]
        }
        break;
        case STATE_UPDATE_RF_PROTOCOL:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_SET_RF_PROTOCOL);
          putFlySkyFrameByte(port, rf_info.protocol); // 0x00:AFHDS1 0x01:AFHDS2 0x02:AFHDS2A
        }
        break;
        case STATE_UPDATE_RX_FIRMWARE:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_UPDATE_RX_FIRMWARE);
        }
        break;
        case STATE_UPDATE_RF_FIRMWARE:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_UPDATE_RF_FIRMWARE);
        }
        break;
        case STATE_GET_RX_VERSION_INFO:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_GET_VERSION_INFO);
          putFlySkyFrameByte(port, FLYSKY_RX_FIRMWARE);
        }
        break;
        case STATE_GET_RF_VERSION_INFO:
        {
          putFlySkyFrameByte(port, FRAME_TYPE_REQUEST_ACK);
          putFlySkyFrameByte(port, CMD_GET_VERSION_INFO);
          putFlySkyFrameByte(port, FLYSKY_RF_FIRMWARE);
        }
        break;
        case STATE_IDLE:
          initFlySkyArray(port);
          break;

        default:
          setFlyskyState(port, STATE_INIT);
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
    if (moduleState[port].mode == MODULE_MODE_BIND) moduleState[port].setMode(MODULE_MODE_NORMAL);
    putFlySkySendChannelData(port);
  }

  incrFlySkyFrame(port);

  putFlySkyFrameCrc(port);
  putFlySkyFrameTail(port);

  if ((DEBUG_RF_FRAME_PRINT & TX_FRAME_ONLY)) {
    /* print each command, except channel data by interval */
    uint8_t * data = intmodulePulsesData.afhds2.pulses;
    if (data[3] != CMD_SEND_CHANNEL_DATA || (set_loop_cnt++ % 100 == 0)) {
      uint8_t size = intmodulePulsesData.afhds2.ptr - data;
      TRACE("TX (State %0d):", intmodulePulsesData.afhds2.state);
      traceBuffer("", data, size);
    }
  }
}

#if !defined(SIMU)
void usbDownloadTransmit(uint8_t *buffer, uint32_t size)
{
    if (USB_SERIAL_MODE != getSelectedUsbMode()) return;
    buffer[0] = HALL_PROTOLO_HEAD;
    for (uint32_t idx = 0; idx < size; idx++)
    {
        usbSerialPutc(buffer[idx]);
    }
}
#endif


static int parse_ps_state = 0;

void parseCharacter(STRUCT_HALL *hallBuffer, unsigned char ch)
{
    if (parse_ps_state != 0) return;
    parse_ps_state = 1;

    switch( hallBuffer->status )
    {
        case GET_START:
        {
            if ( HALL_PROTOLO_HEAD == ch )
            {
                hallBuffer->head  = HALL_PROTOLO_HEAD;
                hallBuffer->status = GET_ID;
                hallBuffer->msg_OK = 0;
            }
            break;
        }
        case GET_ID:
        {
            hallBuffer->hallID.ID = ch;
            hallBuffer->status = GET_LENGTH;
            break;
        }
        case GET_LENGTH:
        {
            hallBuffer->length = ch;
            hallBuffer->dataIndex = 0;
            hallBuffer->status = GET_DATA;
            if( 0 == hallBuffer->length )
            {
                hallBuffer->status = GET_CHECKSUM;
                hallBuffer->checkSum=0;
            }
            break;
        }
        case GET_DATA:
        {
            hallBuffer->data[hallBuffer->dataIndex++] = ch;
            if( hallBuffer->dataIndex >= hallBuffer->length)
            {
                hallBuffer->checkSum = 0;
                hallBuffer->dataIndex = 0;
                hallBuffer->status = GET_STATE;
            }
            break;
        }
        case GET_STATE:
        {
            hallBuffer->checkSum = 0;
            hallBuffer->dataIndex = 0;
            hallBuffer->status = GET_CHECKSUM;
        }
        case GET_CHECKSUM:
        {
            hallBuffer->checkSum |= ch << ((hallBuffer->dataIndex++) * 8);
            if( hallBuffer->dataIndex >= 2 )
            {
                hallBuffer->dataIndex = 0;
                hallBuffer->status = CHECKSUM;
            }
            else
            {
                break;
            }
        }
        case CHECKSUM:
        {
            if(hallBuffer->checkSum == calc_crc16(&hallBuffer->head, hallBuffer->length + 3))
            {
                hallBuffer->msg_OK = 1;
                goto Label_restart;
            }
            else
            {
                goto Label_error;
            }
        }
    }

    goto exit;

    Label_error:
    Label_restart:
        hallBuffer->status = GET_START;
exit: parse_ps_state = 0;
    return ;
}

/* crc16 implementation according to CCITT standards */
const unsigned short CRC16Table[256]= {
 0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
 0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
 0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
 0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
 0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
 0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
 0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
 0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
 0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
 0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
 0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
 0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
 0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
 0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
 0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
 0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
 0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
 0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
 0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
 0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
 0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
 0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
 0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
 0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
 0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
 0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
 0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
 0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
 0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
 0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
 0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
 0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

unsigned short calc_crc16(void *pBuffer,unsigned char BufferSize)
{
    unsigned short crc16;
    crc16 = 0xffff;
    while (BufferSize)
    {
        crc16 = (crc16 << 8) ^ CRC16Table[((crc16>>8) ^ (*(unsigned char *)pBuffer)) & 0x00ff];
        pBuffer = (void *)((unsigned char *)pBuffer + 1);
        BufferSize--;
    }
    return crc16;
}
