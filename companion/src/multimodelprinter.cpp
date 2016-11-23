/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "helpers.h"
#include "helpers_html.h"
#include "multimodelprinter.h"
#include <algorithm>

MultiModelPrinter::MultiColumns::MultiColumns(int count):
  count(count),
  compareColumns(NULL)
{
  columns = new QString[count];
}

MultiModelPrinter::MultiColumns::~MultiColumns()
{
  delete[] columns;
}

void MultiModelPrinter::MultiColumns::append(const QString & str)
{
  for (int i=0; i<count; i++) {
    append(i, str);
  }
}

void MultiModelPrinter::MultiColumns::appendTitle(const QString & name)
{
  append("<b>" + name + "</b>&nbsp;");
}

void MultiModelPrinter::MultiColumns::append(int idx, const QString & str)
{
  if (compareColumns)
    compareColumns[idx].append(str);
  else
    columns[idx].append(str);
}

void MultiModelPrinter::MultiColumns::beginCompare()
{
  compareColumns = new QString[count];
}

void MultiModelPrinter::MultiColumns::endCompare(const QString & color)
{
  for (int i=0; i<count; i++) {
    QString cellColor = color;
    if (i==0 && count>1 && compareColumns[0]!=compareColumns[1])
      cellColor = "green";
    else if (i>0 && compareColumns[i]!=compareColumns[0])
      cellColor = "red";
    columns[i].append(QString("<font color='%1'>%2</font>").arg(cellColor).arg(compareColumns[i]));
  }
  delete[] compareColumns;
  compareColumns = NULL;
}

template <class T>
void MultiModelPrinter::MultiColumns::append(int idx, T val)
{
  append(idx, QString("%1").arg(val));
}

QString MultiModelPrinter::MultiColumns::print()
{
  QString result = "<tr>";
  for (int i=0; i<count; i++) {
    result.append(QString("<td width='%1%'>%2</td>").arg(100.0/count).arg(columns[i]));
  }
  result.append("</tr>");
  return result;
}

bool MultiModelPrinter::MultiColumns::isEmpty()
{
  for (int i=0; i<count; i++) {
    if (!columns[i].isEmpty())
      return false;
  }
  return true;
}

#define COMPARE(what) \
  columns.beginCompare(); \
  for (int cc=0; cc<models.size(); cc++) { \
    ModelPrinter * modelPrinter = modelPrinters[cc]; \
    ModelData * model = models[cc]; \
    (void)(model); (void)(modelPrinter); \
    columns.append(cc, (what)); \
  } \
  columns.endCompare();

QString MultiModelPrinter::printTitle(const QString & label)
{
  return QString("<tr><td colspan='%1'><h2>").arg(modelPrinters.count()) + label + "</h2></td></tr>";
}

MultiModelPrinter::MultiModelPrinter(Firmware * firmware):
  firmware(firmware)
{
}

MultiModelPrinter::~MultiModelPrinter()
{
  for(int i=0; i<modelPrinters.size(); i++) {
    delete modelPrinters[i];
  }
}

void MultiModelPrinter::setModel(int idx, const ModelData & model)
{
  int count = std::max(models.size(), idx+1);
  models.resize(count);
  modelPrinters.resize(count);

  if (modelPrinters[idx]) {
    // free existing model printer
    delete modelPrinters[idx];
    modelPrinters[idx] = 0;
  }

  models[idx] = (ModelData *)&model; // TODO remove cast
  modelPrinters[idx] = new ModelPrinter(firmware, defaultSettings, model);
}

QString MultiModelPrinter::print(QTextDocument * document)
{
  if (document) document->clear();

  QString str = "<table border='1' cellspacing='0' cellpadding='3' width='100%' style='font-family: monospace;'>";
  str += printSetup();
  if (firmware->getCapability(Heli))
    str += printHeliSetup();
  if (firmware->getCapability(FlightModes))
    str += printFlightModes();
  str += printInputs();
  str += printMixers();
  str += printLimits();
  str += printCurves(document);
  if (firmware->getCapability(Gvars) && !firmware->getCapability(GvarsFlightModes))
    str += printGvars();
  str += printLogicalSwitches();
  str += printCustomFunctions();
  str += printTelemetry();
  str += "</table>";
  return str;
}

QString MultiModelPrinter::printSetup()
{
  QString str = printTitle(tr("General Model Settings"));

  MultiColumns columns(models.size());
  columns.appendTitle(tr("Name:"));
  COMPARE(model->name);
  columns.append("<br/>");
  columns.appendTitle(tr("EEprom Size:"));
  COMPARE(modelPrinter->printEEpromSize());
  columns.append("<br/>");
  for (int i=0; i<firmware->getCapability(Timers); i++) {
    columns.appendTitle(tr("Timer%1:").arg(i+1));
    COMPARE(modelPrinter->printTimer(i));
    columns.append("<br/>");
  }
  for (int i=0; i<firmware->getCapability(NumModules); i++) {
    columns.appendTitle(firmware->getCapability(NumModules) > 1 ? tr("Module%1:").arg(i+1) : tr("Module:"));
    COMPARE(modelPrinter->printModule(i));
    columns.append("<br/>");
  }
  if (IS_TARANIS(firmware->getBoard())) {
    columns.appendTitle(tr("Trainer port:"));
    COMPARE(modelPrinter->printTrainerMode());
    columns.append("<br/>");
  }
  columns.appendTitle(tr("Throttle Trim:"));
  COMPARE(modelPrinter->printThrottleTrimMode());
  columns.append("<br/>");
  columns.appendTitle(tr("Trim Increment:"));
  COMPARE(modelPrinter->printTrimIncrementMode());
  columns.append("<br/>");
  columns.appendTitle(tr("Center Beep:"));
  COMPARE(modelPrinter->printCenterBeep());
  str.append(columns.print());
  return str;
}


QString MultiModelPrinter::printHeliSetup()
{
  bool heliEnabled = false;
  for (int k=0; k<models.size(); k++) {
    heliEnabled =  heliEnabled || models[k]->swashRingData.type != HELI_SWASH_TYPE_NONE;
  }

  if (!heliEnabled)
    return "";

  QString str = printTitle(tr("Helicopter Setup"));
  MultiColumns columns(models.size());
  columns.appendTitle (tr("Swash Type:"));
  COMPARE(modelPrinter->printHeliSwashType());
  columns.append ("<br/>");

  columns.appendTitle (tr("Swash Ring:"));
  COMPARE(model->swashRingData.value);

  columns.append ("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  columns.append("<tr>");
  columns.append("<td></td><td><b>" + tr("Input") + "</b></td><td><b>" + tr("Weight") + "</b></td>");
  columns.append("</tr>");

  columns.append("<tr><td><b>" + tr("Long. cyc") + "</b></td><td>");
  COMPARE(model->swashRingData.elevatorSource.toString(model));
  columns.append("</td><td>");
  COMPARE(model->swashRingData.elevatorWeight)
  columns.append("</td></tr>");

  columns.append("<tr><td><b>" + tr("Lateral cyc") + "</b></td><td>");
  COMPARE(model->swashRingData.aileronSource.toString(model));
  columns.append("</td><td>");
  COMPARE(model->swashRingData.aileronWeight)
  columns.append("</td></tr>");


  columns.append("<tr><td><b>" + tr("Collective") + "</b></td><td>");
  COMPARE(model->swashRingData.collectiveSource.toString(model));
  columns.append("</td><td>");
  COMPARE(model->swashRingData.collectiveWeight)
  columns.append("</td></tr>");
  columns.append("</table>");



  str.append(columns.print());
  return str;
}

QString MultiModelPrinter::printFlightModes()
{
  QString str = printTitle(tr("Flight modes"));

  // Trims
  {
    MultiColumns columns(models.size());
    columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
    columns.append("<tr>");
    columns.append("<td><b>" + tr("Flight mode") + "</b></td>");
    columns.append("<td><b>" + tr("Switch") + "</b></td>");
    columns.append("<td><b>" + tr("Fade IN") + "</b></td>");
    columns.append("<td><b>" + tr("Fade OUT") + "</b></td>");
    for (int i=0; i<CPN_MAX_STICKS; i++) {
      columns.append("<td><b>" + AnalogString(i) + " trim</b></td>");
    }
    columns.append("</tr>");

    for (int i=0; i<firmware->getCapability(FlightModes); i++) {
      columns.append("<tr><td><b>" + tr("FM%1").arg(i) + "</b>&nbsp;");
      COMPARE(model->flightModeData[i].name);
      columns.append("</td><td>");
      COMPARE(model->flightModeData[i].swtch.toString());
      columns.append("</td><td>");
      COMPARE(model->flightModeData[i].fadeIn);
      columns.append("</td><td>");
      COMPARE(model->flightModeData[i].fadeOut);
      columns.append("</td>");
      for (int k=0; k<CPN_MAX_STICKS; k++) {
        columns.append("<td>");
        COMPARE(modelPrinter->printTrim(i, k));
        columns.append("</td>");
      }
      columns.append("</tr>");
    }

    columns.append("</table>");
    str.append(columns.print());
  }

  // GVars and Rotary Encoders
  int gvars = firmware->getCapability(Gvars);
  if ((gvars && firmware->getCapability(GvarsFlightModes)) || firmware->getCapability(RotaryEncoders)) {
    MultiColumns columns(models.size());
    columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
    columns.append("<tr><td><b>" + tr("Flight mode") + "</b></td>");
    if (firmware->getCapability(GvarsFlightModes)) {
      for (int i=0; i<gvars; i++) {
        columns.append("<td><b>" + tr("GV%1").arg(i+1) + "</b><br/>");
        COMPARE(model->gvars_names[i]);
        columns.append("</td>");
      }
    }
    for (int i=0; i<firmware->getCapability(RotaryEncoders); i++) {
      columns.append("<td><b>" + tr("RE%1").arg(i+1) + "</b></td>");
    }
    columns.append("</tr>");
    for (int i=0; i<firmware->getCapability(FlightModes); i++) {
      columns.append("<tr><td><b>" + tr("FM%1").arg(i) + "</b>&nbsp;");
      COMPARE(model->flightModeData[i].name);
      columns.append("</td>");
      if (firmware->getCapability(GvarsFlightModes)) {
        for (int k=0; k<gvars; k++) {
          columns.append("<td>");
          COMPARE(modelPrinter->printGlobalVar(i, k));
          columns.append("</td>");
        }
      }
      for (int k=0; k<firmware->getCapability(RotaryEncoders); k++) {
        columns.append("<td>");
        COMPARE(modelPrinter->printRotaryEncoder(i, k));
        columns.append("</td>");
      }
      columns.append("</tr>");
    }
    columns.append("</table>");
    str.append(columns.print());
  }

  return str;
}

QString MultiModelPrinter::printLimits()
{
  QString str = printTitle(tr("Limits"));
  MultiColumns columns(models.size());
  columns.append("<table border='0' cellspacing='0' cellpadding='1' width='100%'>" \
                 "<tr>" \
                 " <td><b>" + tr("Channel") + "</b></td>" \
                 " <td><b>" + (firmware->getCapability(ChannelsName) > 0 ? tr("Name") : "") + "</b></td>" \
                 " <td><b>" + tr("Offset") + "</b></td>" \
                 " <td><b>" + tr("Min") + "</b></td>" \
                 " <td><b>" + tr("Max") + "</b></td>" \
                 " <td><b>" + tr("Invert") + "</b></td>" \
                 "</tr>");
  for (int i=0; i<firmware->getCapability(Outputs); i++) {
    columns.append("<tr><td><b>");
    COMPARE(modelPrinter->printChannelName(i));
    columns.append("</b></td><td>");
    COMPARE(modelPrinter->printOutputName(i));
    columns.append("</td><td>");
    COMPARE(model->limitData[i].offsetToString());
    columns.append("</td><td>");
    COMPARE(model->limitData[i].minToString());
    columns.append("</td><td>");
    COMPARE(model->limitData[i].maxToString());
    columns.append("</td><td>");
    COMPARE(model->limitData[i].revertToString());
    columns.append("</td></tr>");
  }
  columns.append("</table>");
  str.append(columns.print());
  return str;
}

QString MultiModelPrinter::printGvars()
{
  QString str = printTitle(tr("Global Variables"));
  int gvars = firmware->getCapability(Gvars);
  MultiColumns columns(models.size());
  columns.append("<table border='0' cellspacing='0' cellpadding='1' width='100%'><tr>");
  for (int i=0; i<gvars; i++) {
    columns.append(QString("<td><b>") + tr("GV%1").arg(i+1) + "</b></td>");
  }
  columns.append("</tr><tr>");
  for (int i=0; i<gvars; i++) {
    columns.append("<td>");
    COMPARE(model->flightModeData[0].gvars[i]);
    columns.append("</td>");
  }
  columns.append("</tr>");
  str.append(columns.print());
  return str;
}

QString MultiModelPrinter::printInputs()
{
  QString str = printTitle(tr("Inputs"));
  MultiColumns columns(models.size());
  columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  for (int i=0; i<std::max(4, firmware->getCapability(VirtualInputs)); i++) {
    int count = 0;
    for (int k=0; k<models.size(); k++) {
      count = std::max(count, models[k]->expos(i).size());
    }
    if (count > 0) {
      columns.append("<tr><td width='20%'><b>");
      COMPARE(modelPrinter->printInputName(i));
      columns.append("</b></td><td>");
      for (int j=0; j<count; j++) {
        if (j > 0)
          columns.append("<br/>");
        COMPARE(j<model->expos(i).size() ? modelPrinter->printInputLine(*model->expos(i)[j]) : "");
      }
      columns.append("</td></tr>");
    }
  }
  str.append(columns.print());
  return str;
}

QString MultiModelPrinter::printMixers()
{
  QString str = printTitle(tr("Mixers"));
  MultiColumns columns(models.size());
  columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  for (int i=0; i<firmware->getCapability(Outputs); i++) {
    int count = 0;
    for (int k=0; k<models.size(); k++) {
      count = std::max(count, models[k]->mixes(i).size());
    }
    if (count > 0) {
      columns.append("<tr><td width='20%'><b>");
      COMPARE(modelPrinter->printMixerName(i+1));
      columns.append("</b></td><td>");
      for (int j=0; j<count; j++) {
        if (j > 0)
          columns.append("<br/>");
        COMPARE((j < model->mixes(i).size()) ? modelPrinter->printMixerLine(*model->mixes(i)[j], (j>0)) : "&nbsp;");
      }
      columns.append("</td></tr>");
    }
  }
  str.append(columns.print());
  return str;
}

QString MultiModelPrinter::printCurves(QTextDocument * document)
{
  QString str;
  MultiColumns columns(models.size());
  int count = 0;
  columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  for (int i=0; i<firmware->getCapability(NumCurves); i++) {
    bool curveEmpty = true;
    for (int k=0; k<models.size(); k++) {
      if (!models[k]->curves[i].isEmpty()) {
        curveEmpty = false;
        break;
      }
    }
    if (!curveEmpty) {
      count++;
      columns.append("<tr><td width='20%'><b>" + tr("CV%1").arg(i+1) + "</b></td><td>");
      COMPARE(modelPrinter->printCurve(i));
      for (int k=0; k<models.size(); k++)
        columns.append(k, QString("<br/><img src='%1' border='0' />").arg(modelPrinters[k]->createCurveImage(i, document)));
      columns.append("</td></tr>");
    }
  }
  columns.append("</table><br/>");
  if (count > 0) {
    str.append(printTitle(tr("Curves")));
    str.append(columns.print());
  }
  return str;
}

QString MultiModelPrinter::printLogicalSwitches()
{
  QString str;
  MultiColumns columns(models.size());
  int count = 0;
  columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  for (int i=0; i<firmware->getCapability(LogicalSwitches); i++) {
    bool lsEmpty = true;
    for (int k=0; k<models.size(); k++) {
      if (!modelPrinters[k]->printLogicalSwitchLine(i).isEmpty()) {
        lsEmpty = false;
        break;
      }
    }
    if (!lsEmpty) {
      count++;
      columns.append("<tr><td width='20%'><b>" + tr("L%1").arg(i+1) + "</b></td><td>");
      COMPARE(modelPrinter->printLogicalSwitchLine(i));
      columns.append("</td></tr>");
    }
  }
  columns.append("</table>");
  if (count > 0) {
    str.append(printTitle(tr("Logical Switches")));
    str.append(columns.print());
  }
  return str;
}

QString MultiModelPrinter::printCustomFunctions()
{
  QString str;
  MultiColumns columns(models.size());
  int count = 0;
  columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  for (int i=0; i<firmware->getCapability(CustomFunctions); i++) {
    bool sfEmpty = true;
    for (int k=0; k<models.size(); k++) {
      if (!modelPrinters[k]->printCustomFunctionLine(i).isEmpty()) {
        sfEmpty = false;
        break;
      }
    }
    if (!sfEmpty) {
      count++;
      columns.append("<tr><td width='20%'><b>" + tr("SF%1").arg(i+1) + "</b></td><td>");
      COMPARE(modelPrinter->printCustomFunctionLine(i));
      columns.append("</td></tr>");
    }
  }
  columns.append("</table>");
  if (count > 0) {
    str.append(printTitle(tr("Special Functions")));
    str.append(columns.print());
  }
  return str;
}

QString MultiModelPrinter::printTelemetry()
{
  QString str = printTitle(tr("Telemetry Settings"));

  // Analogs on non ARM boards
  if (!IS_ARM(firmware->getBoard())) {
    MultiColumns columns(models.size());
    columns.append("<table border='0' cellspacing='0' cellpadding='1' width='100%'>" \
                   "<tr><td width='22%'><b>" + tr("Analogs") + "</b></td><td width='26%'><b>" + tr("Unit") + "</b></td><td width='26%'><b>" + tr("Scale") + "</b></td><td width='26%'><b>" + tr("Offset") + "</b></td></tr>");
    for (int i=0; i<2; i++) {
      columns.append("<tr><td><b>"+tr("A%1").arg(i+1)+"</b></td><td>");
      COMPARE(getFrSkyUnits(model->frsky.channels[i].type));
      columns.append("</td><td>");
      COMPARE(QString::number((model->frsky.channels[i].ratio / (model->frsky.channels[i].type==0 ? 10.0 : 1)), 10, (model->frsky.channels[i].type==0 ? 1 : 0)));
      columns.append("</td><td>");
      COMPARE(QString::number((model->frsky.channels[i].offset*(model->frsky.channels[i].ratio / (model->frsky.channels[i].type==0 ?10.0 : 1)))/255, 10, (model->frsky.channels[i].type==0 ? 1 : 0)));
      columns.append("</td></tr>");
    }
    columns.append("</table><br/>");
    str.append(columns.print());
    // TODO I remove the analogs alarms for now
  }

  // RSSI alarms
  {
    MultiColumns columns(models.size());
    columns.append("<table border='0' cellspacing='0' cellpadding='1' width='100%'>");
    for (int i=0; i<2; i++) {
      columns.append("<tr><td><b>" + QString(i==0 ? tr("RSSI Alarms") : "") + "</b></td><td>");
      COMPARE(getFrSkyAlarmType(model->frsky.rssiAlarms[i].level));
      columns.append("</td><td>&lt;</td><td>");
      COMPARE(QString::number(model->frsky.rssiAlarms[i].value, 10));
      columns.append("</td></tr>");
    }
    columns.append("</table><br/>");
    str.append(columns.print());
  }
  
  return str;
}
