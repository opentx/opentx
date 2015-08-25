#ifndef _MODELPRINTER_H_
#define _MODELPRINTER_H_

#include <QString>
#include <QStringList>
#include <QPainter>
#include "eeprominterface.h"

QString changeColor(const QString & input, const QString & to, const QString & from = "grey");

QString addFont(const QString & input, const QString & color = "", const QString & size = "", const QString & face = "");

void debugHtml(const QString & html);

class CurveImage
{
  public:
    CurveImage();
    void drawCurve(const CurveData & curve, QColor color);
    void save(const QString & filename);

  protected:
    int size;
    QImage image;
    QPainter painter;
};

class ModelPrinter: public QObject
{
  public:
    ModelPrinter(Firmware * firmware, const GeneralSettings & generalSettings, const ModelData & model);
    virtual ~ModelPrinter();
  
    QString printEEpromSize();
    QString printTrimIncrementMode();
    QString printThrottleTrimMode();
    static QString printModuleProtocol(unsigned int protocol);
    QString printFlightModeSwitch(int index);
    QString printFlightModeName(int index);
    QString printFlightModes(unsigned int flightModes);
    QString printModule(int idx);
    QString printTrainerMode();
    QString printCenterBeep();
    QString printTrim(int flightModeIndex, int stickIndex);
    QString printGlobalVar(int flightModeIndex, int gvarIndex);
    QString printRotaryEncoder(int flightModeIndex, int reIndex);
    QString printTimer(int idx);
    QString printTimer(const TimerData & timer);
    QString printInputName(int idx);
    QString printInputLine(int idx);
    QString printInputLine(const ExpoData & ed);
    QString printMixerName(int curDest);
    QString printMixerLine(int idx, int highlightedSource = 0);
    QString printMixerLine(const MixData & md, int highlightedSource = 0);
    QString printLogicalSwitchLine(int idx);
    QString printCustomFunctionLine(int idx);
    static QString printChannelName(int idx);
    QString printOutputName(int idx);
    QString printCurve(int idx);
    QString createCurvesImage();
    QString createCurveImage(int idx);

  private:
    Firmware * firmware;
    const GeneralSettings & generalSettings;
    const ModelData & model;
    QStringList curvefiles;

};

#endif // _MODELPRINTER_H_
