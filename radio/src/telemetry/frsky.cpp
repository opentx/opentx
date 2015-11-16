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
 * opentx is based on code named
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

#include "../opentx.h"

uint8_t frskyStreaming = 0;

#if defined(WS_HOW_HIGH)
uint8_t frskyUsrStreaming = 0;
#endif

uint8_t link_counter = 0;

#define FRSKY_RX_PACKET_SIZE   19
uint8_t frskyRxBuffer[FRSKY_RX_PACKET_SIZE];   // Receive buffer. 9 bytes (full packet), worst case 18 bytes with byte-stuffing (+1)

#if !defined(CPUARM) && !defined(PCBFLAMENCO)
uint8_t frskyTxBuffer[FRSKY_TX_PACKET_SIZE];
#endif

#if !defined(CPUARM)
uint8_t frskyTxBufferCount = 0;
#endif

#if defined(CPUARM)
uint8_t telemetryState = TELEMETRY_INIT;
#endif

#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
Fifo<512> telemetryFifo; // TODO should be in the driver
#endif

uint8_t frskyRxBufferCount = 0;

FrskyData frskyData;

#if defined(CPUARM)
uint8_t telemetryProtocol = 255;
#define IS_FRSKY_D_PROTOCOL()      (telemetryProtocol == PROTOCOL_FRSKY_D)
#define IS_FRSKY_SPORT_PROTOCOL()  (telemetryProtocol == PROTOCOL_FRSKY_SPORT)
#else
#define IS_FRSKY_D_PROTOCOL()     (true)
#define IS_FRSKY_SPORT_PROTOCOL() (false)
#endif

#if defined(CPUARM)
void FrskyValueWithMin::reset()
{
  memclear(this, sizeof(*this));
}
#endif

void FrskyValueWithMin::set(uint8_t value)
{
#if defined(CPUARM)
  if (this->value == 0) {
    memset(values, value, TELEMETRY_AVERAGE_COUNT);
    this->value = value;
  }
  else {
    //calculate the average from values[] and value
    //also shift readings in values [] array
    unsigned int sum = values[0];
    for (int i=0; i<TELEMETRY_AVERAGE_COUNT-1; i++) {
      uint8_t tmp = values[i+1];
      values[i] = tmp;
      sum += tmp;
    }
    values[TELEMETRY_AVERAGE_COUNT-1] = value;
    sum += value;
    this->value = sum/(TELEMETRY_AVERAGE_COUNT+1);
  }
#else
  if (this->value == 0) {
    this->value = value;
  }
  else {
    sum += value;
    if (link_counter == 0) {
      this->value = sum / (IS_FRSKY_D_PROTOCOL() ? FRSKY_D_AVERAGING : FRSKY_SPORT_AVERAGING);
      sum = 0;
    }
  }
#endif

  if (!min || value < min) {
    min = value;
  }
}

void FrskyValueWithMinMax::set(uint8_t value, uint8_t unit)
{
  FrskyValueWithMin::set(value);
  if (unit != UNIT_VOLTS) {
    this->value = value;
  }
  if (!max || value > max) {
    max = value;
  }
}

#if !defined(CPUARM)
uint16_t getChannelRatio(source_t channel)
{
  return (uint16_t)g_model.frsky.channels[channel].ratio << g_model.frsky.channels[channel].multiplier;
}

lcdint_t applyChannelRatio(source_t channel, lcdint_t val)
{
  return ((int32_t)val+g_model.frsky.channels[channel].offset) * getChannelRatio(channel) * 2 / 51;
}
#endif

#if defined(TELEMETREZ)
#define PRIVATE         0x1B
uint8_t privateDataLen;
uint8_t privateDataPos;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION) && defined(TELEMETREZ)
extern uint8_t TrotCount;
extern uint8_t TezRotary;
#endif

NOINLINE void processSerialData(uint8_t data)
{
  static uint8_t dataState = STATE_DATA_IDLE;

#if defined(BLUETOOTH)
  // TODO if (g_model.bt_telemetry)
  btPushByte(data);
#endif

#if defined(PCBTARANIS)
    if (g_eeGeneral.serial2Mode == UART_MODE_TELEMETRY_MIRROR) {
      serial2Putc(data);
    }
#endif

#if defined(PCBTARANIS) && defined(REV9E) && !defined(SIMU)
    #define BLUETOOTH_BUFFER_LENGTH     20
    static uint8_t bluetoothBuffer[BLUETOOTH_BUFFER_LENGTH];
    static uint8_t bluetoothIndex = 0;
    bluetoothBuffer[bluetoothIndex++] = data;
    if (bluetoothIndex == BLUETOOTH_BUFFER_LENGTH) {
      if (bluetoothReady()) {
        bluetoothWrite(bluetoothBuffer, BLUETOOTH_BUFFER_LENGTH);
      }
      bluetoothIndex = 0;
    }
#endif

  switch (dataState)
  {
    case STATE_DATA_START:
      if (data == START_STOP) {
        if (IS_FRSKY_SPORT_PROTOCOL()) {
          dataState = STATE_DATA_IN_FRAME ;
          frskyRxBufferCount = 0;
        }
      }
      else {
        if (frskyRxBufferCount < FRSKY_RX_PACKET_SIZE) {
          frskyRxBuffer[frskyRxBufferCount++] = data;
        }
        dataState = STATE_DATA_IN_FRAME;
      }
      break;

    case STATE_DATA_IN_FRAME:
      if (data == BYTESTUFF) {
        dataState = STATE_DATA_XOR; // XOR next byte
      }
      else if (data == START_STOP) {
        if (IS_FRSKY_SPORT_PROTOCOL()) {
          dataState = STATE_DATA_IN_FRAME ;
          frskyRxBufferCount = 0;
        }
        else {
          // end of frame detected
          frskyDProcessPacket(frskyRxBuffer);
          dataState = STATE_DATA_IDLE;
        }
        break;
      }
      else if (frskyRxBufferCount < FRSKY_RX_PACKET_SIZE) {
        frskyRxBuffer[frskyRxBufferCount++] = data;
      }
      break;

    case STATE_DATA_XOR:
      if (frskyRxBufferCount < FRSKY_RX_PACKET_SIZE) {
        frskyRxBuffer[frskyRxBufferCount++] = data ^ STUFF_MASK;
      }
      dataState = STATE_DATA_IN_FRAME;
      break;

    case STATE_DATA_IDLE:
      if (data == START_STOP) {
        frskyRxBufferCount = 0;
        dataState = STATE_DATA_START;
      }
#if defined(TELEMETREZ)
      if (data == PRIVATE) {
        dataState = STATE_DATA_PRIVATE_LEN;
      }
#endif
      break;

#if defined(TELEMETREZ)
    case STATE_DATA_PRIVATE_LEN:
      dataState = STATE_DATA_PRIVATE_VALUE;
      privateDataLen = data; // Count of bytes to receive
      privateDataPos = 0;
      break;

    case STATE_DATA_PRIVATE_VALUE :
      if (privateDataPos == 0) {
        // Process first private data byte
        // PC6, PC7
        if ((data & 0x3F) == 0) {// Check byte is valid
          DDRC |= 0xC0;          // Set as outputs
          PORTC = ( PORTC & 0x3F ) | ( data & 0xC0 ); // update outputs
        }
      }
#if defined(ROTARY_ENCODER_NAVIGATION)
      if (privateDataPos == 1) {
        TrotCount = data;
      }
      if (privateDataPos == 2) { // rotary encoder switch
        RotEncoder = data;
      }
#endif
      if (++privateDataPos == privateDataLen) {
        dataState = STATE_DATA_IDLE;
      }
      break;
#endif
  } // switch

#if defined(FRSKY_SPORT)
  if (IS_FRSKY_SPORT_PROTOCOL() && frskyRxBufferCount >= FRSKY_SPORT_PACKET_SIZE) {
    processSportPacket(frskyRxBuffer);
    dataState = STATE_DATA_IDLE;
  }
#endif
}

void telemetryWakeup()
{
#if defined(CPUARM)
  uint8_t requiredTelemetryProtocol = MODEL_TELEMETRY_PROTOCOL();
  if (telemetryProtocol != requiredTelemetryProtocol) {
    telemetryProtocol = requiredTelemetryProtocol;
    telemetryInit();
  }
#endif

#if defined(PCBTARANIS)
  uint8_t data;
#if defined(SPORT_FILE_LOG) && !defined(SIMU)
  static tmr10ms_t lastTime = 0;
  tmr10ms_t newTime = get_tmr10ms();
  struct gtm utm;
  gettime(&utm);
#endif
  while (telemetryFifo.pop(data)) {
    processSerialData(data);
#if defined(SPORT_FILE_LOG) && !defined(SIMU)
    extern FIL g_telemetryFile;
    if (lastTime != newTime) {
      f_printf(&g_telemetryFile, "\r\n%4d-%02d-%02d,%02d:%02d:%02d.%02d0: %02X", utm.tm_year+1900, utm.tm_mon+1, utm.tm_mday, utm.tm_hour, utm.tm_min, utm.tm_sec, g_ms100, data);
      lastTime = newTime;
    }
    else {
      f_printf(&g_telemetryFile, " %02X", data);
    }
#endif
  }
#elif defined(PCBSKY9X)
  if (telemetryProtocol == PROTOCOL_FRSKY_D_SECONDARY) {
    uint8_t data;
    while (telemetrySecondPortReceive(data)) {
      processSerialData(data);
    }
  }
  else {
    // Receive serial data here
    rxPdcUsart(processSerialData);
  }
#endif

#if !defined(CPUARM) && !defined(PCBFLAMENCO)
  if (IS_FRSKY_D_PROTOCOL()) {
    // Attempt to transmit any waiting Fr-Sky alarm set packets every 50ms (subject to packet buffer availability)
    static uint8_t frskyTxDelay = 5;
    if (frskyAlarmsSendState && (--frskyTxDelay == 0)) {
      frskyTxDelay = 5; // 50ms
#if !defined(SIMU)
      frskyDSendNextAlarm();
#endif
    }
  }
#endif

#if defined(CPUARM)
  for (int i=0; i<MAX_SENSORS; i++) {
    const TelemetrySensor & sensor = g_model.telemetrySensors[i];
    if (sensor.type == TELEM_TYPE_CALCULATED) {
      telemetryItems[i].eval(sensor);
    }
  }
#endif

#if defined(VARIO)
  if (TELEMETRY_STREAMING() && !IS_FAI_ENABLED()) {
    varioWakeup();
  }
#endif

#if defined(PCBTARANIS) && defined(REVPLUS)
  #define FRSKY_BAD_ANTENNA() (IS_VALID_XJT_VERSION() && frskyData.swr.value > 0x33)
#else
  #define FRSKY_BAD_ANTENNA() (frskyData.swr.value > 0x33)
#endif

#if defined(CPUARM)
  static tmr10ms_t alarmsCheckTime = 0;
  #define SCHEDULE_NEXT_ALARMS_CHECK(seconds) alarmsCheckTime = get_tmr10ms() + (100*(seconds))
  if (int32_t(get_tmr10ms() - alarmsCheckTime) > 0) {

    SCHEDULE_NEXT_ALARMS_CHECK(1/*second*/);

    uint8_t now = TelemetryItem::now();
    for (int i=0; i<MAX_SENSORS; i++) {
      if (isTelemetryFieldAvailable(i)) {
        uint8_t lastReceived = telemetryItems[i].lastReceived;
        if (lastReceived < TELEMETRY_VALUE_TIMER_CYCLE && uint8_t(now - lastReceived) > TELEMETRY_VALUE_OLD_THRESHOLD) {
          telemetryItems[i].lastReceived = TELEMETRY_VALUE_OLD;
          TelemetrySensor * sensor = & g_model.telemetrySensors[i];
          if (sensor->unit == UNIT_DATETIME) {
            telemetryItems[i].datetime.datestate = 0;
            telemetryItems[i].datetime.timestate = 0;
          }
        }
      }
    }

#if defined(PCBTARANIS)
    if ((g_model.moduleData[INTERNAL_MODULE].rfProtocol != RF_PROTO_OFF || g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_XJT) && FRSKY_BAD_ANTENNA()) {
      AUDIO_SWR_RED();
      POPUP_WARNING(STR_ANTENNAPROBLEM);
      SCHEDULE_NEXT_ALARMS_CHECK(10/*seconds*/);
    }
#endif

    if (TELEMETRY_STREAMING()) {
      if (getRssiAlarmValue(1) && TELEMETRY_RSSI() < getRssiAlarmValue(1)) {
        AUDIO_RSSI_RED();
        SCHEDULE_NEXT_ALARMS_CHECK(10/*seconds*/);
      }
      else if (getRssiAlarmValue(0) && TELEMETRY_RSSI() < getRssiAlarmValue(0)) {
        AUDIO_RSSI_ORANGE();
        SCHEDULE_NEXT_ALARMS_CHECK(10/*seconds*/);
      }
    }
  }

  if (TELEMETRY_STREAMING()) {
    if (telemetryState == TELEMETRY_KO) {
      AUDIO_TELEMETRY_BACK();
    }
    telemetryState = TELEMETRY_OK;
  }
  else if (telemetryState == TELEMETRY_OK) {
    telemetryState = TELEMETRY_KO;
    AUDIO_TELEMETRY_LOST();
  }
#endif
}

void telemetryInterrupt10ms()
{
#if defined(CPUARM)
#elif defined(FRSKY_HUB)
  uint16_t voltage = 0; /* unit: 1/10 volts */
  for (uint8_t i=0; i<frskyData.hub.cellsCount; i++)
    voltage += frskyData.hub.cellVolts[i];
  voltage /= (10 / TELEMETRY_CELL_VOLTAGE_MUTLIPLIER);
  frskyData.hub.cellsSum = voltage;
  if (frskyData.hub.cellsSum < frskyData.hub.minCells) {
    frskyData.hub.minCells = frskyData.hub.cellsSum;
  }
#endif

  if (TELEMETRY_STREAMING()) {
    if (!TELEMETRY_OPENXSENSOR()) {
#if defined(CPUARM)
      for (int i=0; i<MAX_SENSORS; i++) {
        const TelemetrySensor & sensor = g_model.telemetrySensors[i];
        if (sensor.type == TELEM_TYPE_CALCULATED) {
          telemetryItems[i].per10ms(sensor);
        }
      }
#else
      // power calculation
      uint8_t channel = g_model.frsky.voltsSource;
      if (channel <= FRSKY_VOLTS_SOURCE_A2) {
        voltage = applyChannelRatio(channel, frskyData.analog[channel].value) / 10;
      }

#if defined(FRSKY_HUB)
      else if (channel == FRSKY_VOLTS_SOURCE_FAS) {
        voltage = frskyData.hub.vfas;
      }
#endif

#if defined(FRSKY_HUB)
      uint16_t current = frskyData.hub.current; /* unit: 1/10 amps */
#else
      uint16_t current = 0;
#endif

      channel = g_model.frsky.currentSource - FRSKY_CURRENT_SOURCE_A1;
      if (channel < MAX_FRSKY_A_CHANNELS) {
        current = applyChannelRatio(channel, frskyData.analog[channel].value) / 10;
      }

      frskyData.hub.power = ((current>>1) * (voltage>>1)) / 25;

      frskyData.hub.currentPrescale += current;
      if (frskyData.hub.currentPrescale >= 3600) {
        frskyData.hub.currentConsumption += 1;
        frskyData.hub.currentPrescale -= 3600;
      }
#endif
    }

#if !defined(CPUARM)
    if (frskyData.hub.power > frskyData.hub.maxPower) {
      frskyData.hub.maxPower = frskyData.hub.power;
    }
#endif
  }

#if defined(WS_HOW_HIGH)
  if (frskyUsrStreaming > 0) {
    frskyUsrStreaming--;
  }
#endif

  if (frskyStreaming > 0) {
    frskyStreaming--;
  }
  else {
#if !defined(SIMU)
#if defined(CPUARM)
    frskyData.rssi.reset();
#else
    frskyData.rssi[0].set(0);
    frskyData.rssi[1].set(0);
#endif
#endif
  }
}

void telemetryReset()
{
  memclear(&frskyData, sizeof(frskyData));

#if defined(CPUARM)
  for (int index=0; index<MAX_SENSORS; index++) {
    telemetryItems[index].clear();
  }
#endif

  frskyStreaming = 0; // reset counter only if valid frsky packets are being detected
  link_counter = 0;

#if defined(CPUARM)
  telemetryState = TELEMETRY_INIT;
#endif

#if defined(FRSKY_HUB) && !defined(CPUARM)
  frskyData.hub.gpsLatitude_bp = 2;
  frskyData.hub.gpsLongitude_bp = 2;
  frskyData.hub.gpsFix = -1;
#endif

#if defined(SIMU)

#if defined(CPUARM)
  frskyData.swr.value = 30;
  frskyData.rssi.value = 75;
#else
  frskyData.rssi[0].value = 75;
  frskyData.rssi[1].value = 75;
  frskyData.analog[TELEM_ANA_A1].set(120, UNIT_VOLTS);
  frskyData.analog[TELEM_ANA_A2].set(240, UNIT_VOLTS);
#endif

#if !defined(CPUARM)
  frskyData.hub.fuelLevel = 75;
  frskyData.hub.rpm = 12000;
  frskyData.hub.vfas = 100;
  frskyData.hub.minVfas = 90;

#if defined(GPS)
  frskyData.hub.gpsFix = 1;
  frskyData.hub.gpsLatitude_bp = 4401;
  frskyData.hub.gpsLatitude_ap = 7710;
  frskyData.hub.gpsLongitude_bp = 1006;
  frskyData.hub.gpsLongitude_ap = 8872;
  frskyData.hub.gpsSpeed_bp = 200;  //in knots
  frskyData.hub.gpsSpeed_ap = 0;
  getGpsPilotPosition();

  frskyData.hub.gpsLatitude_bp = 4401;
  frskyData.hub.gpsLatitude_ap = 7455;
  frskyData.hub.gpsLongitude_bp = 1006;
  frskyData.hub.gpsLongitude_ap = 9533;
  getGpsDistance();
#endif

  frskyData.hub.airSpeed = 1000; // 185.1 km/h

  frskyData.hub.cellsCount = 6;
  frskyData.hub.cellVolts[0] = 410/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.cellVolts[1] = 420/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.cellVolts[2] = 430/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.cellVolts[3] = 440/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.cellVolts[4] = 450/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.cellVolts[5] = 460/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.minCellVolts = 250/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.minCell = 300;    //unit 10mV
  frskyData.hub.minCells = 220;  //unit 100mV
  //frskyData.hub.cellsSum = 261;    //calculated from cellVolts[]

  frskyData.hub.gpsAltitude_bp = 50;
  frskyData.hub.baroAltitude_bp = 50;
  frskyData.hub.minAltitude = 10;
  frskyData.hub.maxAltitude = 500;

  frskyData.hub.accelY = 100;
  frskyData.hub.temperature1 = -30;
  frskyData.hub.maxTemperature1 = 100;

  frskyData.hub.current = 55;
  frskyData.hub.maxCurrent = 65;
#endif
#endif

/*Add some default sensor values to the simulator*/
#if defined(CPUARM) && defined(SIMU)
  for (int i=0; i<MAX_SENSORS; i++) {
    const TelemetrySensor & sensor = g_model.telemetrySensors[i];
    switch (sensor.id)
    {
      case RSSI_ID:
        setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, RSSI_ID, sensor.instance , 75, UNIT_RAW, 0);
        break;
      case ADC1_ID:
        setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, ADC1_ID, sensor.instance, 100, UNIT_RAW, 0);
        break;
      case ADC2_ID:
        setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, ADC2_ID, sensor.instance, 245, UNIT_RAW, 0);
        break;
      case SWR_ID:
        setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, SWR_ID, sensor.instance, 30, UNIT_RAW, 0);
        break;
      case BATT_ID:
        setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, BATT_ID, sensor.instance, 100, UNIT_RAW, 0);
        break;
    }
  }
#endif
}

void telemetryInit(void)
{
#if defined(CPUARM)
  if (telemetryProtocol == PROTOCOL_FRSKY_D) {
    telemetryPortInit(FRSKY_D_BAUDRATE);
  }
  else if (telemetryProtocol==PROTOCOL_FRSKY_D_SECONDARY) {
    telemetryPortInit(0);
    serial2TelemetryInit(PROTOCOL_FRSKY_D_SECONDARY);
  }
  else {
    telemetryPortInit(FRSKY_SPORT_BAUDRATE);
  }
#elif !defined(SIMU)
  telemetryPortInit();
#endif

  // we don't reset the telemetry here as we would also reset the consumption after model load
}

#if defined(CPUARM)
#elif defined(FRSKY_HUB)
void frskyUpdateCells(void)
{
  // Voltage => Cell number + Cell voltage
  uint8_t battnumber = ((frskyData.hub.volts & 0x00F0) >> 4);
  if (battnumber < 12) {
    if (frskyData.hub.cellsCount < battnumber+1) {
      frskyData.hub.cellsCount = battnumber+1;
    }
    uint8_t cellVolts = (uint8_t)(((((frskyData.hub.volts & 0xFF00) >> 8) + ((frskyData.hub.volts & 0x000F) << 8))) / 10);
    frskyData.hub.cellVolts[battnumber] = cellVolts;
    if (!frskyData.hub.minCellVolts || cellVolts<frskyData.hub.minCellVolts || battnumber==frskyData.hub.minCellIdx) {
      frskyData.hub.minCellIdx = battnumber;
      frskyData.hub.minCellVolts = cellVolts;
      if (!frskyData.hub.minCell || frskyData.hub.minCellVolts<frskyData.hub.minCell)
        frskyData.hub.minCell = frskyData.hub.minCellVolts;
    }
  }
}
#endif
