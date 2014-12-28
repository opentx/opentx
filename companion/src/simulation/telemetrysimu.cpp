#include <stdint.h>
#include "telemetrysimu.h"
#include "ui_telemetrysimu.h"
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
  delete ui;
}

void TelemetrySimulator::onTimerEvent()
{
  if (ui->Simulate->isChecked()) {
    generateTelemetryFrame();
  }
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

template<class t> t abs(t a) { return a>0?a:-a; }
/// liefert das Minimum der Argumente
template<class t> t min(t a, t b) { return a<b?a:b; }
/// liefert das Maximum der Argumente
template<class t> t max(t a, t b) { return a>b?a:b; }
template<class t> t sgn(t a) { return a>0 ? 1 : (a < 0 ? -1 : 0); }
template<class t> t limit(t mi, t x, t ma) { return min(max(mi,x),ma); }

void TelemetrySimulator::generateTelemetryFrame()
{
  static int item = 0;
  int value;
  uint8_t buffer[FRSKY_SPORT_PACKET_SIZE];


  switch(item++) {
    case 0:
      generateSportPacket(buffer, 1, DATA_FRAME, RSSI_ID, limit(0, ui->Rssi->text().toInt(), 0xFF));
      break;

    case 1:
      generateSportPacket(buffer, 1, DATA_FRAME, XJT_VERSION_ID, 11);
      break;

    case 2:
      generateSportPacket(buffer, 1, DATA_FRAME, SWR_ID, limit(0, ui->Swr->text().toInt(), 0xFF));
      break;

    case 3:
      generateSportPacket(buffer, 1, DATA_FRAME, ADC1_ID, limit(0, ui->A1->text().toInt(), 0xFF));
      break;

    case 4:
      generateSportPacket(buffer, 1, DATA_FRAME, ADC2_ID, limit(0, ui->A2->text().toInt(), 0xFF));
      break;

    default:
      item = 0;
      return;
  }
  simulator->sendTelemetry(buffer, 9);
}