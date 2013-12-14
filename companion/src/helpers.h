#ifndef HELPERS_H
#define HELPERS_H

#include <QtGui>
#include "eeprominterface.h"

#define TMR_NUM_OPTION  (TMR_VAROFS+2*9+2*GetEepromInterface()->getCapability(CustomSwitches)-1)

//convert from mode 1 to mode g_eeGeneral.stickMode
//NOTICE!  =>  1..4 -> 1..4
#define CONVERT_MODE(x) (((x)<=4) ? modn12x3[g_eeGeneral.stickMode][((x)-1)] : (x))
#define CHANNEL_ORDER(x) (chout_ar[g_eeGeneral.templateSetup*4 + (x)-1])

#define CURVE_BASE   7
#define CH(x) (SRC_CH1+(x)-1-(SRC_SWC-SRC_3POS))
#define CV(x) (CURVE_BASE+(x)-1)
#define CC(x) (CHANNEL_ORDER(x)) //need to invert this to work with dest

#define CURVE5(x) ((x)-1)
#define CURVE9(x) (MAX_CURVE5+(x)-1)

#define TRIM_ON  0
#define TRIM_OFF 1
#define TRIM_OFFSET 2

void populateGvSourceCB(QComboBox *b, int value);
void populateVoiceLangCB(QComboBox *b, QString language);
void populateTTraceCB(QComboBox *b, int value);
void populateRotEncCB(QComboBox *b, int value, int renumber);
void populateBacklightCB(QComboBox *b, const uint8_t value);

#define POPULATE_ONOFF        0x01
#define POPULATE_MSWITCHES    0x02
#define POPULATE_AND_SWITCHES 0x04
void populateSwitchCB(QComboBox *b, const RawSwitch & value, unsigned long attr=0, UseContext context=DefaultContext);
void populateFuncCB(QComboBox *b, unsigned int value);
void populateRepeatCB(QComboBox *b, unsigned int value);
void populateGVmodeCB(QComboBox *b, unsigned int value);
QString FuncParam(uint function, int value, QString paramT="",unsigned int adjustmode=0);
void populateFuncParamCB(QComboBox *b, uint function, unsigned int value, unsigned int adjustmode=0);
void populateFuncParamArmTCB(QComboBox *b, ModelData * g_model, char * value, QStringList & paramsList);
void populatePhasesCB(QComboBox *b, int value);
void populateTrimUseCB(QComboBox *b, unsigned int phase);
void populateGvarUseCB(QComboBox *b, unsigned int phase);
void populateCurvesCB(QComboBox *b, int value);
void populateCustomScreenFieldCB(QComboBox *b, unsigned int value, bool last, int hubproto);
void populateExpoCurvesCB(QComboBox *b, int value);
void populateTimerSwitchCB(QComboBox *b, int value, int extrafields=0);
void populateTimerSwitchBCB(QComboBox *b, int value, int extrafields=0);
QString getCustomSwitchStr(CustomSwData * customSw, const ModelData & model);
QString getProtocolStr(const int proto);

#define POPULATE_SOURCES       1
#define POPULATE_TRIMS         2
#define POPULATE_SWITCHES      4
#define POPULATE_GVARS         8
#define POPULATE_TELEMETRY    16
#define POPULATE_TELEMETRYEXT 32

#define GVARS_VARIANT 0x0001
#define FRSKY_VARIANT 0x0002

// void populateGVarCB(QComboBox *b, int value, int min, int max,int pgvars=5); //TODO: Clean Up
void populateGVCB(QComboBox *b, int value);
void populateSourceCB(QComboBox *b, const RawSource &source, unsigned int flags);
void populateCSWCB(QComboBox *b, int value);
QString getTimerMode(int tm);
QString getTimerModeB(int tm);
QString getPhaseName(int val, char * phasename=NULL);
QString getStickStr(int index);
QString getCSWFunc(int val);
QString getFuncName(unsigned int val);
QString getRepeatString(unsigned int val);
QString getSignedStr(int value);
QString getCurveStr(int curve);
QString getGVarString(int16_t val, bool sign=false);
QString image2qstring(QImage image);
QImage qstring2image(QString imagestr);
int findmult(float value, float base);
bool checkbit(int value, int bit);

QString getTrimInc(ModelData * g_model);
QString getTimerStr(TimerData & timer);
QString getProtocol(ModelData * g_model);
QString getCenterBeep(ModelData * g_model);

/* FrSky helpers */
QString getFrSkyAlarmType(int alarm);
QString getFrSkyBlades(int blades);
QString getFrSkyUnits(int units);
QString getFrSkyProtocol(int protocol);
QString getFrSkyMeasure(int units);
QString getFrSkySrc(int index);
float getBarValue(int barId, int value, FrSkyData *fd);
float c9xexpou(float point, float coeff);
float ValToTim(int value);
int TimToVal(float value);
#endif // HELPERS_H
