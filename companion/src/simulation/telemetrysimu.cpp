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
  timer->start(100);
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
  memset(buffer, 0, sizeof(buffer));

  switch(item++) {
    case 0:
      if (ui->Rssi->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, RSSI_ID, LIMIT<uint32_t>(0, ui->Rssi->text().toInt(&ok, 0), 0xFF));
      break;

    case 1:
#if defined(XJT_VERSION_ID)
      generateSportPacket(buffer, 1, DATA_FRAME, XJT_VERSION_ID, 11);
#endif
      break;

    case 2:
      if (ui->Swr->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, SWR_ID, LIMIT<uint32_t>(0, ui->Swr->text().toInt(&ok, 0), 0xFFFF));
      break;

    case 3:
      if (ui->A1->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, ADC1_ID, LIMIT<uint32_t>(0, ui->A1->text().toInt(&ok, 0), 0xFF));
      break;

    case 4:
      if (ui->A2->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, ADC2_ID, LIMIT<uint32_t>(0, ui->A2->text().toInt(&ok, 0), 0xFF));
      break;

    case 5:
      if (ui->A3->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, A3_FIRST_ID, LIMIT<uint32_t>(0, ui->A3->text().toInt(&ok, 0), 0xFFFFFFFF));
      break;

    case 6:
      if (ui->A4->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, A4_FIRST_ID, LIMIT<uint32_t>(0, ui->A4->text().toInt(&ok, 0), 0xFFFFFFFF));
      break;

    case 7:
      if (ui->T1->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, T1_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->T1->text().toInt(&ok, 0), 0x7FFFFFFF));
      break;

    case 8:
      if (ui->T2->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, T2_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->T2->text().toInt(&ok, 0), 0x7FFFFFFF));
      break;

    case 9:
      if (ui->rpm->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, RPM_FIRST_ID, LIMIT<uint32_t>(0, ui->rpm->text().toInt(&ok, 0), 0xFFFF));
      break;

    case 10:
      if (ui->fuel->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, FUEL_FIRST_ID, LIMIT<uint32_t>(0, ui->fuel->text().toInt(&ok, 0), 0xFFFF));
      break;

    case 11:
      if (ui->aspeed->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, AIR_SPEED_FIRST_ID, LIMIT<uint32_t>(0, ui->aspeed->text().toInt(&ok, 0), 0xFFFFFFFF));
      break;

    case 12:
      if (ui->vspeed->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, VARIO_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->vspeed->text().toInt(&ok, 0), 0x7FFFFFFF));
      break;

    case 13:
      if (ui->valt->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, ALT_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->valt->text().toInt(&ok, 0), 0x7FFFFFFF));
      break;

    case 14:
      if (ui->accx->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, ACCX_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->accx->text().toInt(&ok, 0), 0x7FFFFFFF));
      break;

    case 15:
      if (ui->accy->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, ACCY_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->accy->text().toInt(&ok, 0), 0x7FFFFFFF));
      break;

    case 16:
      if (ui->accz->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, ACCZ_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->accz->text().toInt(&ok, 0), 0x7FFFFFFF));
      break;

    case 17:
      if (ui->vfas->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, VFAS_FIRST_ID, LIMIT<uint32_t>(0, ui->vfas->text().toInt(&ok, 0), 0xFFFFFFFF));
      break;

    case 18:
      if (ui->curr->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, CURR_FIRST_ID, LIMIT<uint32_t>(0, ui->curr->text().toInt(&ok, 0), 0xFFFFFFFF));
      break;

    case 19:
      if (ui->cells->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, CELLS_FIRST_ID, LIMIT<uint32_t>(0, ui->cells->text().toInt(&ok, 0), 0xFFFFFFFF));
      break;

    case 20:
      if (ui->gps_alt->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, GPS_ALT_FIRST_ID, LIMIT<int32_t>(-0x7FFFFFFF, ui->gps_alt->text().toInt(&ok, 0), 0x7FFFFFFF));
      break;

    case 21:
      if (ui->gps_speed->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, GPS_SPEED_FIRST_ID, LIMIT<uint32_t>(0, ui->gps_speed->text().toInt(&ok, 0), 0xFFFFFFFF));
      break;

    case 22:
      if (ui->gps_course->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, GPS_COURS_FIRST_ID, LIMIT<uint32_t>(0, ui->gps_course->text().toInt(&ok, 0), 0xFFFFFFFF));
      break;

    case 23:
      if (ui->gps_time->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, GPS_TIME_DATE_FIRST_ID, LIMIT<uint32_t>(0, ui->gps_time->text().toInt(&ok, 0), 0xFFFFFFFF));
      break;

    case 24:
      if (ui->gps_latlon->text().length())
        generateSportPacket(buffer, 1, DATA_FRAME, GPS_LONG_LATI_FIRST_ID, LIMIT<uint32_t>(0, ui->gps_latlon->text().toInt(&ok, 0), 0xFFFFFFFF));
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
