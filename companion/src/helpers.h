#ifndef HELPERS_H
#define HELPERS_H

#include <QtGui>
#include "eeprominterface.h"

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
void populateVoiceLangCB(QComboBox *b, QString language);
void populateRotEncCB(QComboBox *b, int value, int renumber);
void populateBacklightCB(QComboBox *b, const uint8_t value);

QString getTheme();

class CompanionIcon: public QIcon {
  public:
    CompanionIcon(QString baseimage);
};

class GVarGroup: public QObject {

  Q_OBJECT

  public:
    GVarGroup(QCheckBox *weightGV, QAbstractSpinBox *weightSB, QComboBox *weightCB, int & weight, const int deflt, const int mini, const int maxi, const double step=1.0, bool allowGVars=true);

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
};

#define HIDE_DIFF             0x01
#define HIDE_EXPO             0x02
#define HIDE_NEGATIVE_CURVES  0x04

class CurveGroup : public QObject {

  Q_OBJECT

  public:
    CurveGroup(QComboBox *curveTypeCB, QCheckBox *curveGVarCB, QComboBox *curveValueCB, QSpinBox *curveValueSB, CurveReference & curve, unsigned int flags=0);
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
    unsigned int flags;
    bool lock;
    int lastType;
};

enum SwitchContext
{
  LogicalSwitchesContext,
  CustomFunctionsContext,
  TimersContext,
  MixesContext
};

void populateSwitchCB(QComboBox *b, const RawSwitch & value, const GeneralSettings & generalSettings, SwitchContext context);

void populatePhasesCB(QComboBox *b, int value);
void populateGvarUseCB(QComboBox *b, unsigned int phase);
QString getProtocolStr(const int proto);
QString getPhasesStr(unsigned int phases, ModelData & model);

#define POPULATE_SOURCES        (1<<0)
#define POPULATE_TRIMS          (1<<1)
#define POPULATE_SWITCHES       (1<<2)
#define POPULATE_GVARS          (1<<3)
#define POPULATE_TELEMETRY      (1<<4)
#define POPULATE_TELEMETRYEXT   (1<<5)
#define POPULATE_VIRTUAL_INPUTS (1<<6)
#define POPULATE_SCRIPT_OUTPUTS (1<<7)

#define GVARS_VARIANT 0x0001
#define FRSKY_VARIANT 0x0002

// void populateGVarCB(QComboBox *b, int value, int min, int max,int pgvars=5); //TODO: Clean Up
void populateGVCB(QComboBox *b, int value);
void populateSourceCB(QComboBox *b, const RawSource &source, const ModelData & model, unsigned int flags);
QString getPhaseName(int val, const char * phasename=NULL);
QString getInputStr(ModelData & model, int index);
QString image2qstring(QImage image);
QImage qstring2image(QString imagestr);
int findmult(float value, float base);

QString getTrimInc(ModelData * g_model);
QString getTimerStr(TimerData & timer);
QString getProtocol(ModelData * g_model);
QString getCenterBeepStr(ModelData * g_model);

/* FrSky helpers */
QString getFrSkyAlarmType(int alarm);
QString getFrSkyUnits(int units);
QString getFrSkyProtocol(int protocol);
QString getFrSkyMeasure(int units);
QString getFrSkySrc(int index);

void startSimulation(QWidget * parent, RadioData & radioData, int modelIdx);

// Format a pixmap to fit on the radio using a specific firmware
QPixmap makePixMap( QImage image, QString firmwareType );

int version2index(QString version);
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

QString getSoundsPath(const GeneralSettings &generalSettings);
QSet<QString> getFilesSet(const QString &path, const QStringList &filter, int maxLen);

#endif // HELPERS_H
