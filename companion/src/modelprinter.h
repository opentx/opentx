#ifndef MODELPRINTER_H
#define MODELPRINTER_H

#include <QString>
#include "eeprominterface.h"

QString changeColor(const QString & input, const QString & to, const QString & from = "grey");

QString addFont(const QString & input, const QString & color = "", const QString & size = "", const QString & face = "");

void debugHtml(const QString & html);


class ModelPrinter
{
public:
  ModelPrinter(Firmware * firmware,  GeneralSettings *gg, ModelData *gm);
  ~ModelPrinter() {};
  
  QString printInputName(int idx);
  QString printInputLine(const ExpoData * ed);
  QString printMixerName(int curDest);
  QString printMixerLine(const MixData * md, int highlightedSource = 0);
  QString printLogicalSwitchLine(int idx);
  QString printCustomFunctionLine(int idx);

private:
  Firmware * firmware;
  GeneralSettings * g_eeGeneral;
  ModelData * g_model;

  QString printPhases(unsigned int phases);
};

#endif //#ifndef MODELPRINTER_H