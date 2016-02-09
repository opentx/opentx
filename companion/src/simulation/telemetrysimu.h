#ifndef telemetrysimu_h
#define telemetrysimu_h

#include <QCloseEvent>
#include <QDialog>
#include <QTimer>
#include <QDateTime>
#include <QtCore/qmath.h>
#include <QFileDialog>

#include "simulatorinterface.h"

static double const SPEEDS[] = { 0.2, 0.4, 0.6, 0.8, 1, 2, 3, 4, 5 };

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
      bool isReady();
      void loadLogFile();
      void play();
      void stop();
      void rewind();
      void stepForward(bool focusOnStop);
      void stepBack();
      void updatePositionLabel(int32_t percentage);
      void setUiDataValues();
      double logFrequency; // in seconds
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
        T1_DEGF,
        T2_DEGC,
        T2_DEGF,
        RPM,
        FUEL,
        VSPD_MS,
        VSPD_FS,
        ALT_FEET,
        ALT_METERS,
        FASV,
        FASC,
        CELS_GRE,
        ASPD_KTS,
        ASPD_KMH,
        ASPD_MPH,
        GALT_FEET,
        GALT_METERS,
        GSPD_KNTS,
        GSPD_KMH,
        GSPD_MPH,
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
        int32_t dataIndex;
      };
      QStringList csvRecords; // contents of the log file (one string per line);
      QStringList columnNames;
      QList<DATA_TO_FUNC_XREF> supportedCols;
      int32_t recordIndex;
      double convertFeetToMeters(QString input);
      double convertFahrenheitToCelsius(QString input);
      QString convertGPSDate(QString input);
      QString convertGPS(QString input);
      void addColumnHash(QString key, CONVERT_TYPE functionIndex);
      double convertDegMin(QString input);
      bool stepping;
      QDateTime parseTransmittterTimestamp(QString row);
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
    void onPositionIndicatorChanged(int value);
    void onReplayRateChanged(int value);


  private: // private classes follow
    class FlvssEmulator
    {
    public:
        uint32_t setAllCells_GetNextPair(double cellValues[6]);
        static const uint32_t MAXCELLS = 6;
    private:
      void encodeAllCells();
      void splitIntoCells(double totalVolts);
      static uint32_t encodeCellPair(uint8_t cellNum, uint8_t firstCellNo, double cell1, double cell2);
      double cellFloats[6];
      uint32_t nextCellNum;
      uint32_t numCells;
      uint32_t cellData1;
      uint32_t cellData2;
      uint32_t cellData3;
      uint32_t cellDataTime;
      double cell[MAXCELLS];
    };

    class GPSEmulator
    {
    public:
      GPSEmulator();
      uint32_t getNextPacketData(uint32_t packetType);
      void setGPSDateTime(QString dateTime);
      void setGPSLatLon(QString latLon);
      void setGPSCourse(double course);
      void setGPSSpeedKMH(double speed);
      void setGPSAltitude(double altitude);
    private:
      QDateTime dt;
      bool sendLat;
      bool sendDate;
      double lat;
      double lon;
      double course;
      double speedKNTS;
      double altitude; // in meters
      uint32_t nextDataIndex;
      uint32_t encodeLatLon(double latLon, bool isLat);
      uint32_t encodeDateTime(uint8_t yearOrHour, uint8_t monthOrMinute, uint8_t dayOrSecond, bool isDate);
    };

};

#endif

