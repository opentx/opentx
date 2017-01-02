#ifndef HELPERS_H
#define HELPERS_H

#include <QtGui>
#include <QTableWidget>
#include <QGridLayout>
#include <QDebug>
#include "eeprominterface.h"
#include "modeledit/modeledit.h"

extern const QColor colors[C9X_MAX_CURVES];

#define TMR_NUM_OPTION  (TMRMODE_COUNT+2*9+2*GetCurrentFirmware()->getCapability(LogicalSwitches)-1)

//convert from mode 1 to mode generalSettings.stickMode
//NOTICE!  =>  1..4 -> 1..4
#define CONVERT_MODE(x)  (((x)<=4) ? modn12x3[generalSettings.stickMode][((x)-1)] : (x))

#define CURVE_BASE   7
#define CH(x) (SRC_CH1+(x)-1-(SRC_SWC-SRC_3POS))
#define CV(x) (CURVE_BASE+(x)-1)

#define CURVE5(x) ((x)-1)
#define CURVE9(x) (MAX_CURVE5+(x)-1)

#define TRIM_ON  0
#define TRIM_OFF 1
#define TRIM_OFFSET 2

#define TRIM_MODE_NONE  0x1F  // 0b11111

void populateGvSourceCB(QComboBox *b, int value);
void populateFileComboBox(QComboBox * b, const QSet<QString> & set, const QString & current);
void getFileComboBoxValue(QComboBox * b, char * dest, int length);
void populateRotEncCB(QComboBox *b, int value, int renumber);

QString getTheme();

class CompanionIcon: public QIcon {
  public:
    CompanionIcon(const QString &baseimage);
};

class GVarGroup: public QObject {

  Q_OBJECT

  public:
    GVarGroup(QCheckBox * weightGV, QAbstractSpinBox * weightSB, QComboBox * weightCB, int & weight, const ModelData & model, const int deflt, const int mini, const int maxi, const double step=1.0, bool allowGVars=true, ModelPanel * panel=NULL);

  protected slots:
    void gvarCBChanged(int);
    void valuesChanged();

  protected:
    QCheckBox *weightGV;
    QAbstractSpinBox *weightSB;
    QSpinBox *sb;
    QDoubleSpinBox *dsb;
    QComboBox *weightCB;
    int & weight;
    double step;
    bool lock;
    ModelPanel * panel;
};

#define HIDE_DIFF             0x01
#define HIDE_EXPO             0x02
#define HIDE_NEGATIVE_CURVES  0x04

class CurveGroup : public QObject {

  Q_OBJECT

  public:
    CurveGroup(QComboBox *curveTypeCB, QCheckBox *curveGVarCB, QComboBox *curveValueCB, QSpinBox *curveValueSB, CurveReference & curve, const ModelData & model, unsigned int flags=0);
    void update();

  protected slots:
    void gvarCBChanged(int);
    void typeChanged(int);
    void valuesChanged();

  protected:
    QComboBox *curveTypeCB;
    QCheckBox *curveGVarCB;
    QComboBox *curveValueCB;
    QSpinBox *curveValueSB;
    CurveReference & curve;
    const ModelData & model;
    unsigned int flags;
    bool lock;
    int lastType;
};

enum SwitchContext
{
  LogicalSwitchesContext,
  SpecialFunctionsContext,
  GlobalFunctionsContext,
  TimersContext,
  MixesContext
};

void populateSwitchCB(QComboBox *b, const RawSwitch & value, const GeneralSettings & generalSettings, SwitchContext context);

void populatePhasesCB(QComboBox *b, int value);
void populateGvarUseCB(QComboBox *b, unsigned int phase);

#define POPULATE_NONE           (1<<0)
#define POPULATE_SOURCES        (1<<1)
#define POPULATE_TRIMS          (1<<2)
#define POPULATE_SWITCHES       (1<<3)
#define POPULATE_GVARS          (1<<4)
#define POPULATE_TELEMETRY      (1<<5)
#define POPULATE_TELEMETRYEXT   (1<<6)
#define POPULATE_VIRTUAL_INPUTS (1<<7)
#define POPULATE_SCRIPT_OUTPUTS (1<<8)

#define GVARS_VARIANT 0x0001
#define FRSKY_VARIANT 0x0002

void populateGVCB(QComboBox & b, int value, const ModelData & model);
void populateSourceCB(QComboBox *b, const RawSource &source, const GeneralSettings generalSettings, const ModelData * model, unsigned int flags);
QString image2qstring(QImage image);
int findmult(float value, float base);

/* FrSky helpers */
QString getFrSkyAlarmType(int alarm);
QString getFrSkyUnits(int units);
QString getFrSkyProtocol(int protocol);
QString getFrSkyMeasure(int units);
QString getFrSkySrc(int index);

void startSimulation(QWidget * parent, RadioData & radioData, int modelIdx);

template <class T>
QVector<T> findWidgets(QObject * object, const QString & name)
{
  QVector<T> result;
  QRegExp rx(name.arg("([0-9]+)"));
  QList<T> children = object->findChildren<T>();
  foreach(T child, children) {
    int pos = rx.indexIn(child->objectName());
    if (pos >= 0) {
      QStringList list = rx.capturedTexts();
      int index = list[1].toInt();
      if (result.size() <= index) {
        result.resize(index+1);
      }
      result[index] = child;
    }
  }
  return result;
}

// Format a pixmap to fit on the radio using a specific firmware
QPixmap makePixMap( QImage image, QString firmwareType );

int version2index(const QString & version);
QString index2version(int index);

class QTimeS : public QTime
{
  public:
    QTimeS(int s) { int h = s/3600; s %= 3600; int m = s/60; s %=60; setHMS(h, m, s); };
    QTimeS(const QTime & q) : QTime(q) {};
    int seconds() const { return hour()*3600 + minute()*60 + second(); };
};

int qunlink(const QString & fileName);

QString generateProcessUniqueTempFileName(const QString & fileName);
bool isTempFileName(const QString & fileName);

QString getSoundsPath(const GeneralSettings &generalSettings);
QSet<QString> getFilesSet(const QString &path, const QStringList &filter, int maxLen);

bool caseInsensitiveLessThan(const QString &s1, const QString &s2);


class GpsGlitchFilter
{
public:
  GpsGlitchFilter() : lastValid(false), glitchCount(0) {};
  bool isGlitch(double latitude, double longitude);

private:
  bool lastValid;
  int glitchCount;
  double lastLat;
  double lastLon;
};

class GpsLatLonFilter
{
public:
  GpsLatLonFilter() {};
  bool isValid(const QString & latitude, const QString & longitude);
  
private:
  QString lastLat;
  QString lastLon;
};

double toDecimalCoordinate(const QString & value);
QStringList extractLatLon(const QString & position);

class TableLayout
{
public:
  TableLayout(QWidget * parent, int rowCount, const QStringList & headerLabels);
  // ~TableLayout() ;

  void addWidget(int row, int column, QWidget * widget);
  void addLayout(int row, int column, QLayout * layout);

  void resizeColumnsToContents();
  void setColumnWidth(int col, int width);
  void pushRowsUp(int row); 

private:
#if defined(TABLE_LAYOUT)
  QTableWidget * tableWidget; 
#else
  QGridLayout * gridWidget; 
#endif
};


class Stopwatch
{
public:
  Stopwatch(const QString & name) :
    name(name), total(0) {
    timer.start();
  };
  ~Stopwatch() {};

  void restart() {
    total = 0;
    timer.restart();
  };

  void report() {
    qint64 elapsed = timer.restart();
    total += elapsed;
    qDebug() << name << QString("%1 ms [%2 ms]").arg(elapsed).arg(total);
  };

  void report(const QString & text) {
    qint64 elapsed = timer.restart();
    total += elapsed;
    qDebug() << name << text << QString("%1 ms [%2 ms]").arg(elapsed).arg(total);
  };

private:
  QString name;
  QElapsedTimer timer;
  qint64 total;
};

extern Stopwatch gStopwatch;

#endif // HELPERS_H
