#include <stdint.h>
#include "telemetrysimu.h"
#include "ui_telemetrysimu.h"
#include "simulatorinterface.h"
#include "radio/src/telemetry/frsky.h"

TelemetrySimulator::TelemetrySimulator(QWidget * parent, SimulatorInterface * simulator):
  QDialog(parent),
  ui(new Ui::TelemetrySimulator),
  simulator(simulator)
{
  ui->setupUi(this);

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
  timer->start(10);
}

TelemetrySimulator::~TelemetrySimulator()
{
  timer->stop();
  delete ui;
}

void TelemetrySimulator::onTimerEvent()
{
  if (ui->Simulate->isChecked()) {
    generateTelemetryFrame();
  }
}

void TelemetrySimulator::closeEvent(QCloseEvent *event)
{
  ui->Simulate->setChecked(false);
  event->accept();
}

void TelemetrySimulator::showEvent(QShowEvent *event)
{
  ui->Simulate->setChecked(true);

  ui->rssi_inst->setText(QString::number(simulator->getSensorInstance(RSSI_ID)));
  ui->swr_inst->setText(QString::number(simulator->getSensorInstance(SWR_ID)));
  ui->a1_inst->setText(QString::number(simulator->getSensorInstance(ADC1_ID)));
  ui->a2_inst->setText(QString::number(simulator->getSensorInstance(ADC2_ID)));
  ui->a3_inst->setText(QString::number(simulator->getSensorInstance(A3_FIRST_ID)));
  ui->a4_inst->setText(QString::number(simulator->getSensorInstance(A4_FIRST_ID)));
  ui->t1_inst->setText(QString::number(simulator->getSensorInstance(T1_FIRST_ID)));
  ui->t2_inst->setText(QString::number(simulator->getSensorInstance(T2_FIRST_ID)));
  ui->rpm_inst->setText(QString::number(simulator->getSensorInstance(RPM_FIRST_ID)));
  ui->fuel_inst->setText(QString::number(simulator->getSensorInstance(FUEL_FIRST_ID)));
  ui->aspd_inst->setText(QString::number(simulator->getSensorInstance(AIR_SPEED_FIRST_ID)));
  ui->vvspd_inst->setText(QString::number(simulator->getSensorInstance(VARIO_FIRST_ID)));
  ui->valt_inst->setText(QString::number(simulator->getSensorInstance(ALT_FIRST_ID)));
  ui->accx_inst->setText(QString::number(simulator->getSensorInstance(ACCX_FIRST_ID)));
  ui->accy_inst->setText(QString::number(simulator->getSensorInstance(ACCY_FIRST_ID)));
  ui->accz_inst->setText(QString::number(simulator->getSensorInstance(ACCZ_FIRST_ID)));
  ui->fasv_inst->setText(QString::number(simulator->getSensorInstance(VFAS_FIRST_ID)));
  ui->fasc_inst->setText(QString::number(simulator->getSensorInstance(CURR_FIRST_ID)));
  ui->cells_inst->setText(QString::number(simulator->getSensorInstance(CELLS_FIRST_ID)));
  ui->gpsa_inst->setText(QString::number(simulator->getSensorInstance(GPS_ALT_FIRST_ID)));
  ui->gpss_inst->setText(QString::number(simulator->getSensorInstance(GPS_SPEED_FIRST_ID)));
  ui->gpsc_inst->setText(QString::number(simulator->getSensorInstance(GPS_COURS_FIRST_ID)));
  ui->gpst_inst->setText(QString::number(simulator->getSensorInstance(GPS_TIME_DATE_FIRST_ID)));
  ui->gpsll_inst->setText(QString::number(simulator->getSensorInstance(GPS_LONG_LATI_FIRST_ID)));
}

void setSportPacketCrc(uint8_t * packet)
{
  short crc = 0;
  for (int i=1; i<FRSKY_SPORT_PACKET_SIZE-1; i++) {
    crc += packet[i]; //0-1FF
    crc += crc >> 8; //0-100
    crc &= 0x00ff;
    crc += crc >> 8; //0-0FF
    crc &= 0x00ff;
  }
  packet[FRSKY_SPORT_PACKET_SIZE-1] = 0xFF - (crc & 0x00ff);
  //TRACE("crc set: %x", packet[FRSKY_SPORT_PACKET_SIZE-1]);
}

void generateSportPacket(uint8_t * packet, uint8_t dataId, uint8_t prim, uint16_t appId, uint32_t data)
{
  packet[0] = dataId;
  packet[1] = prim;
  *((uint16_t *)(packet+2)) = appId;
  *((int32_t *)(packet+4)) = data;
  setSportPacketCrc(packet);
}

void TelemetrySimulator::generateTelemetryFrame()
{
  static int item = 0;
  bool ok = true;
  uint8_t buffer[FRSKY_SPORT_PACKET_SIZE];
  static FlvssEmulator *flvss = new FlvssEmulator();
  static GPSEmulator *gps = new GPSEmulator();

  memset(buffer, 0, sizeof(buffer));

  switch (item++) {
  case 0:
    if (ui->Rssi->text().length())
      generateSportPacket(buffer, ui->rssi_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, RSSI_ID, LIMIT<uint32_t>(0, ui->Rssi->text().toInt(&ok, 0), 0xFF));
    break;

  case 1:
#if defined(XJT_VERSION_ID)
    generateSportPacket(buffer, 1, DATA_FRAME, XJT_VERSION_ID, 11);
#endif
    break;

  case 2:
    if (ui->Swr->text().length())
      generateSportPacket(buffer, ui->swr_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, SWR_ID, LIMIT<uint32_t>(0, ui->Swr->text().toInt(&ok, 0), 0xFFFF));
    break;

  case 3:
    if (ui->A1->text().length())
      generateSportPacket(buffer, ui->a1_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, ADC1_ID, LIMIT<uint32_t>(0, ui->A1->text().toInt(&ok, 0), 0xFF));
    break;

  case 4:
    if (ui->A2->text().length())
      generateSportPacket(buffer, ui->a2_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, ADC2_ID, LIMIT<uint32_t>(0, ui->A2->text().toInt(&ok, 0), 0xFF));
    break;

  case 5:
    if (ui->A3->text().length())
      generateSportPacket(buffer, ui->a3_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, A3_FIRST_ID, LIMIT<uint32_t>(0, ui->A3->text().toInt(&ok, 0), 0xFFFFFFFF));
    break;

  case 6:
    if (ui->A4->text().length())
      generateSportPacket(buffer, ui->a4_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, A4_FIRST_ID, LIMIT<uint32_t>(0, ui->A4->text().toInt(&ok, 0), 0xFFFFFFFF));
    break;

  case 7:
    if (ui->T1->text().length())
      generateSportPacket(buffer, ui->t1_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, T1_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->T1->text().toInt(&ok, 0), 0x7FFFFFFF));
    break;

  case 8:
    if (ui->T2->text().length())
      generateSportPacket(buffer, ui->t2_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, T2_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->T2->text().toInt(&ok, 0), 0x7FFFFFFF));
    break;

  case 9:
    if (ui->rpm->text().length())
      generateSportPacket(buffer, ui->rpm_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, RPM_FIRST_ID, LIMIT<uint32_t>(0, ui->rpm->text().toInt(&ok, 0), 0xFFFF));
    break;

  case 10:
    if (ui->fuel->text().length())
      generateSportPacket(buffer, ui->fuel_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, FUEL_FIRST_ID, LIMIT<uint32_t>(0, ui->fuel->text().toInt(&ok, 0), 0xFFFF));
    break;

  case 11:
    if (ui->aspeed->text().length())
      generateSportPacket(buffer, ui->aspd_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, AIR_SPEED_FIRST_ID, LIMIT<uint32_t>(0, ui->aspeed->text().toInt(&ok, 0), 0xFFFFFFFF));
    break;

  case 12:
    if (ui->vspeed->text().length())
      generateSportPacket(buffer, ui->vvspd_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, VARIO_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->vspeed->text().toInt(&ok, 0), 0x7FFFFFFF));
    break;

  case 13:
    if (ui->valt->text().length())
      generateSportPacket(buffer, ui->valt_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, ALT_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->valt->text().toInt(&ok, 0), 0x7FFFFFFF));
    break;

  case 14:
    if (ui->accx->text().length())
      generateSportPacket(buffer, ui->accx_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, ACCX_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->accx->text().toInt(&ok, 0), 0x7FFFFFFF));
    break;

  case 15:
    if (ui->accy->text().length())
      generateSportPacket(buffer, ui->accy_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, ACCY_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->accy->text().toInt(&ok, 0), 0x7FFFFFFF));
    break;

  case 16:
    if (ui->accz->text().length())
      generateSportPacket(buffer, ui->accz_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, ACCZ_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->accz->text().toInt(&ok, 0), 0x7FFFFFFF));
    break;

  case 17:
    if (ui->vfas->text().length())
      generateSportPacket(buffer, ui->fasv_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, VFAS_FIRST_ID, LIMIT<uint32_t>(0, ui->vfas->text().toInt(&ok, 0), 0xFFFFFFFF));
    break;

  case 18:
    if (ui->curr->text().length())
      generateSportPacket(buffer, ui->fasc_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, CURR_FIRST_ID, LIMIT<uint32_t>(0, ui->curr->text().toInt(&ok, 0), 0xFFFFFFFF));
    break;

  case 19:
    if (ui->cells->text().length()) {
      generateSportPacket(buffer, ui->cells_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, CELLS_FIRST_ID, flvss->setAllCells_GetNextPair(ui->cells->text()));
    }
    break;

  case 20:
    if (ui->gps_alt->text().length()) {
      gps->setGPSAltitude(ui->gps_alt->text());
      generateSportPacket(buffer, ui->gpsa_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, GPS_ALT_FIRST_ID, gps->getNextPacketData(GPS_ALT_FIRST_ID));
    }
    break;

  case 21:
    if (ui->gps_speed->text().length()) {
      gps->setGPSSpeed(ui->gps_speed->text());
      generateSportPacket(buffer, ui->gpss_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, GPS_SPEED_FIRST_ID, gps->getNextPacketData(GPS_SPEED_FIRST_ID));
    }
    break;

  case 22:
    if (ui->gps_course->text().length()) {
      gps->setGPSCourse(ui->gps_course->text());
      generateSportPacket(buffer, ui->gpsc_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, GPS_COURS_FIRST_ID, gps->getNextPacketData(GPS_COURS_FIRST_ID));
    }
    break;

    case 23:
      if (ui->gps_time->text().length()) {
        gps->setGPSDateTime(ui->gps_time->text());
        generateSportPacket(buffer, ui->gpst_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, GPS_TIME_DATE_FIRST_ID, gps->getNextPacketData(GPS_TIME_DATE_FIRST_ID));
      }
      break;

    case 24:
      if (ui->gps_latlon->text().length()) {
        gps->setGPSLatLon(ui->gps_latlon->text());
        generateSportPacket(buffer, ui->gpsll_inst->text().toInt(&ok, 0) - 1, DATA_FRAME, GPS_LONG_LATI_FIRST_ID, gps->getNextPacketData(GPS_LONG_LATI_FIRST_ID));
      }
      break;

    default:
      item = 0;
      return;
  }

  if (ok && buffer[0])
    simulator->sendTelemetry(buffer, FRSKY_SPORT_PACKET_SIZE);
  else
    onTimerEvent();
}

uint32_t TelemetrySimulator::FlvssEmulator::encodeCellPair(uint8_t cellNum, uint8_t firstCellNo, float cell1, float cell2)
{
  uint16_t cell1Data = cell1 * 500.0;
  uint16_t cell2Data = cell2 * 500.0;
  uint32_t cellData = 0;

  cellData = cell2Data & 0x0FFF;
  cellData <<= 12;
  cellData |= cell1Data & 0x0FFF;
  cellData <<= 4;
  cellData |= cellNum & 0x0F;
  cellData <<= 4;
  cellData |= firstCellNo & 0x0F;

  return cellData;
}

void TelemetrySimulator::FlvssEmulator::encodeAllCells()
{
  cellData1 = encodeCellPair(numCells, 0, cellFloats[0], cellFloats[1]);
  if (numCells > 2) cellData2 = encodeCellPair(numCells, 2, cellFloats[2], cellFloats[3]); else cellData2 = 0;
  if (numCells > 4) cellData3 = encodeCellPair(numCells, 4, cellFloats[4], cellFloats[5]); else cellData3 = 0;
}

void TelemetrySimulator::FlvssEmulator::splitIntoCells(float totalVolts)
{
  numCells = qFloor((totalVolts / 3.7) + .5);
  float avgVolts = totalVolts / numCells;
  float remainder = (totalVolts - (avgVolts * numCells));
    for (int i = 0; (i < numCells) && ( i < MAXCELLS); i++) {
    cellFloats[i] = avgVolts;
  }
  for (int i = numCells; i < MAXCELLS; i++) {
    cellFloats[i] = 0;
  }
  cellFloats[0] += remainder;
  numCells = numCells > MAXCELLS ? MAXCELLS : numCells; // force into valid cell count in case of input out of range
}

uint32_t TelemetrySimulator::FlvssEmulator::setAllCells_GetNextPair(QString cellValues)
{
  // parse the cell values into floats
  numCells = 0;
  QStringList valueList = cellValues.split(",");
  if (valueList.count() == 1) {
    splitIntoCells(valueList[0].toFloat());
  }
  else {
    for (int i = 0; i < MAXCELLS; i++) {
      if (i < valueList.count()) {
        cellFloats[i] = valueList[i].toFloat();
        numCells++;
      }
      else cellFloats[i] = 0;
    }
  }

  // encode the float values into telemetry format
  encodeAllCells();

  // return the value for the current pair 
  uint32_t cellData = 0;
  if (nextCellNum >= numCells) {
    nextCellNum = 0;
  }
  switch (nextCellNum) {
  case 0:
    cellData = cellData1;
    break;
  case 2:
    cellData = cellData2;
    break;
  case 4:
    cellData = cellData3;
    break;
  }
  nextCellNum += 2;
  return cellData;
}

TelemetrySimulator::GPSEmulator::GPSEmulator()
{
  lat = 0;
  lon = 0;
  dt = QDateTime::currentDateTime();
  sendLat = true;
  sendDate = true;
}


uint32_t TelemetrySimulator::GPSEmulator::encodeLatLon(float latLon, bool isLat)
{
  uint32_t data = (uint32_t)((latLon < 0 ? -latLon : latLon) * 60 * 10000) & 0x3FFFFFFF;
  if (isLat == false) {
    data |= 0x80000000;
  }
  if (latLon < 0) {
    data |= 0x40000000;
  }
  return data;
}

uint32_t TelemetrySimulator::GPSEmulator::encodeDateTime(uint8_t yearOrHour, uint8_t monthOrMinute, uint8_t dayOrSecond, bool isDate)
{
  uint32_t data = yearOrHour;
  data <<= 8;
  data |= monthOrMinute;
  data <<= 8;
  data |= dayOrSecond;
  data <<= 8;
  if (isDate == true) {
    data |= 0xFF;
  }
  return data;
}

uint32_t TelemetrySimulator::GPSEmulator::getNextPacketData(uint32_t packetType)
{
  switch (packetType) {
  case GPS_LONG_LATI_FIRST_ID:
    sendLat = !sendLat;
    return sendLat ? encodeLatLon(lat, TRUE) : encodeLatLon(lon, FALSE);
    break;
  case GPS_TIME_DATE_FIRST_ID:
    sendDate = !sendDate;
    return sendDate ? encodeDateTime(dt.date().year() - 2000, dt.date().month(), dt.date().day(), true) : encodeDateTime(dt.time().hour(), dt.time().minute(), dt.time().second(), false);
    break;
  case GPS_ALT_FIRST_ID:
    return altitude;
    break;
  case GPS_SPEED_FIRST_ID:
    return speed;
    break;
  case GPS_COURS_FIRST_ID:
    return course;
    break;
  }
  return 0;
}

void TelemetrySimulator::GPSEmulator::setGPSDateTime(QString dateTime)
{
  dt = QDateTime::currentDateTime().toTimeSpec(Qt::UTC); // default to current systemtime
  if (!dateTime.startsWith('*')) {
    QString format("dd-MM-yyyy hh:mm:ss");
    dt = QDateTime::fromString(dateTime, format);
  }
}

void TelemetrySimulator::GPSEmulator::setGPSLatLon(QString latLon)
{
  QStringList coords = latLon.split(",");
  lat = 0.0;
  lon = 0.0;
  if (coords.length() > 1)
  {
    lat = coords[0].toFloat();
    lon = coords[1].toFloat();
  }
}

void TelemetrySimulator::GPSEmulator::setGPSCourse(QString course)
{
  this->course = course.toInt();
}

void TelemetrySimulator::GPSEmulator::setGPSSpeed(QString speed)
{
  this->speed = speed.toInt();
}

void TelemetrySimulator::GPSEmulator::setGPSAltitude(QString altitude)
{
  this->altitude = altitude.toInt();
}