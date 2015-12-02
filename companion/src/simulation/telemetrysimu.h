#ifndef telemetrysimu_h
#define telemetrysimu_h

#include <QCloseEvent>
#include <QDialog>
#include <QTimer>
#include <QDateTime>
#include <QtCore/qmath.h>
#include <QFileDialog>
// #include <QDebug>
#include "simulatorinterface.h"

static float const SPEEDS[] = { 0.2, 0.4, 0.6, 0.8, 1, 2, 3, 4, 5 };

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
    class LogPlaybackController
    {
    public:
      LogPlaybackController(Ui::TelemetrySimulator * ui);
      void loadLogFile();
      void play();
      void stop();
      void rewind();
      void stepForward();
      void stepBack();
      void updatePositionLabel(int32_t percentage);
      void setUiDataValues();
      float logFrequency; // in seconds
    private:
      enum CONVERT_TYPE {
        RXBT_V,
        RSSI,
        SWR,
        A1,
        A2,
        A3,
        A4,
        T1_DEGC,
        T2_DEGC,
        RPM,
        FUEL,
        VSPD_MS,
        ALT_FEET,
        FASV,
        FASC,
        CELS_GRE,
        ASPD,
        GALT_FEET,
        GSPD_KNTS,
        GHDG_DEG,
        GDATE,
        G_LATLON,
        ACCX,
        ACCY,
        ACCZ,
      };
      QMap<QString, CONVERT_TYPE> colToFuncMap; // contains all 'known' column headings and how they are to be processed
      Ui::TelemetrySimulator * ui;
      struct DATA_TO_FUNC_XREF {
        CONVERT_TYPE functionIndex;
        uint32_t dataIndex;
      };
      QStringList csvRecords; // contents of the log file (one string per line);
      QStringList columnNames;
      QList<DATA_TO_FUNC_XREF> supportedCols;
      uint32_t recordIndex;
      float convertFeetToMeters(QString input);
      QString convertLogDate(QString input);
      QString convertGPS(QString input);
      void addColumnHash(QString key, CONVERT_TYPE functionIndex);
      float convertDegMin(QString input);
      bool stepping;
      void calcLogFrequency();
      int32_t replayRate;
    };

private:
    Ui::TelemetrySimulator * ui;
    QTimer * timer;
    QTimer * logTimer;
    SimulatorInterface *simulator;
    void generateTelemetryFrame();
    TelemetrySimulator::LogPlaybackController *logPlayback;
  
  private slots:
    void onSimulateToggled(bool isChecked);
    void onTimerEvent();
    void onLogTimerEvent();
    void onLoadLogFile();
    void onPlay();
    void onRewind();
    void onStepForward();
    void onStepBack();
    void onStop();
    void onPositionIndicatorReleased();
    void onPositionIndicatorChanged(int value);
    void onReplayRateChanged(int value);


  private: // private classes follow
    class FlvssEmulator
    {
    public:
        uint32_t setAllCells_GetNextPair(float cellValues[6]);
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
      void setGPSCourse(float course);
      void setGPSSpeedKMH(float speed);
      void setGPSAltitude(float altitude);
    private:
      QDateTime dt;
      bool sendLat;
      bool sendDate;
      float lat;
      float lon;
      float course;
      float speedKNTS;
      float altitude; // in meters
      uint32_t nextDataIndex;
      uint32_t encodeLatLon(float latLon, bool isLat);
      uint32_t encodeDateTime(uint8_t yearOrHour, uint8_t monthOrMinute, uint8_t dayOrSecond, bool isDate);
    };

};

#endif

