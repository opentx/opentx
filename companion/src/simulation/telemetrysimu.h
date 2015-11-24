#ifndef telemetrysimu_h
#define telemetrysimu_h


#include <QCloseEvent>
#include <QDialog>
#include <QTimer>
#include <QDateTime>
#include <QtCore/qmath.h>
#include "simulatorinterface.h"

#define INSTANCE 2

namespace Ui {
  class TelemetrySimulator;
}


class TelemetrySimulator : public QDialog
{
  Q_OBJECT

  public:
    explicit TelemetrySimulator(QWidget * parent, SimulatorInterface * simulator);
    virtual ~TelemetrySimulator();


  protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void showEvent(QShowEvent *event);

  private:
    Ui::TelemetrySimulator * ui;
    QTimer * timer;
    SimulatorInterface *simulator;
    void generateTelemetryFrame();
  
  private slots:
    void onTimerEvent();


  private: // private classes follow
    class FlvssEmulator
    {
    public:
        uint32_t setAllCells_GetNextPair(QString cellValues);
        static const uint32_t MAXCELLS = 6;
    private:
      void encodeAllCells();
      void splitIntoCells(float totalVolts);
      static uint32_t encodeCellPair(uint8_t cellNum, uint8_t firstCellNo, float cell1, float cell2);
      float cellFloats[6];
      uint32_t nextCellNum;
      uint32_t numCells;
      uint32_t cellData1;
      uint32_t cellData2;
      uint32_t cellData3;
      uint32_t cellDataTime;
      float cell[MAXCELLS];
    };

    class GPSEmulator
    {
    public:
      GPSEmulator();
      uint32_t getNextPacketData(uint32_t packetType);
      void setGPSDateTime(QString dateTime);
      void setGPSLatLon(QString latLon);
      void setGPSCourse(QString course);
      void setGPSSpeed(QString speed);
      void setGPSAltitude(QString altitude);
    private:
      QDateTime dt;
      bool sendLat;
      bool sendDate;
      float lat;
      float lon;
      int32_t course;
      int32_t speed;
      int32_t altitude;
      uint32_t nextDataIndex;
      uint32_t encodeLatLon(float latLon, bool isLat);
      uint32_t encodeDateTime(uint8_t yearOrHour, uint8_t monthOrMinute, uint8_t dayOrSecond, bool isDate);
    };

};

#endif

