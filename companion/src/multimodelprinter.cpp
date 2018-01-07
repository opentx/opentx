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
#include "appdata.h"
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

void MultiModelPrinter::MultiColumns::appendLineBreak()
{
  append("<br/>");
}

void MultiModelPrinter::MultiColumns::appendSectionTableStart()
{
  append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
}

void MultiModelPrinter::MultiColumns::appendTableEnd()
{
  append("</table>");
}

void MultiModelPrinter::MultiColumns::appendRowStart(const QString & title, const unsigned int titlewidth)
{
  append("<tr>");
  if (!title.isEmpty())
    appendLabelCell(QString(title), titlewidth);
}

void MultiModelPrinter::MultiColumns::appendRowEnd()
{
  append("</tr>");
}

void MultiModelPrinter::MultiColumns::appendCellStart(const unsigned int width)
{
  QString str = "<td";
  if (width)
    str.append(QString(" width=%1%").arg(QString::number(width)));
  str.append(">");
  append(str);
}

void MultiModelPrinter::MultiColumns::appendCellEnd()
{
  append("</td>");
}

void MultiModelPrinter::MultiColumns::appendLabelCell(const QString & str, const unsigned int width, const QString & align, const QString & color)
{
  append(doTableCell(str, width, align, color, true));
}

void MultiModelPrinter::MultiColumns::appendValueCell(const QString & str, const unsigned int width, const QString & align, const QString & color)
{
  append(doTableCell(str, width, align, color, false));
}

void MultiModelPrinter::MultiColumns::appendRow(const QStringList & strl, const unsigned int width, const QString & align, const QString & color)
{
  append(doTableRow(strl, width, align, color, false));
}

void MultiModelPrinter::MultiColumns::appendHeaderRow(const QStringList & strl, const unsigned int width, const QString & align, const QString & color)
{
  append(doTableRow(strl, width, align, color, true));
}

#define COMPARE(what) \
  columns.beginCompare(); \
  for (int cc=0; cc < modelPrinterMap.size(); cc++) { \
    ModelPrinter * modelPrinter = modelPrinterMap.value(cc).second; \
    const ModelData * model = modelPrinterMap.value(cc).first; \
    (void)(model); (void)(modelPrinter); \
    columns.append(cc, (what)); \
  } \
  columns.endCompare();

#define COMPARECELL(what) \
  columns.appendCellStart(); \
  COMPARE(what); \
  columns.appendCellEnd();

#define COMPARELABELVALUE(lbl, what) \
  columns.appendTitle(lbl); \
  COMPARE(what);

QString MultiModelPrinter::printTitle(const QString & label)
{
  return QString("<tr><td colspan='%1'><h2>").arg(modelPrinterMap.count()) + label + "</h2></td></tr>";
}

MultiModelPrinter::MultiModelPrinter(Firmware * firmware):
  firmware(firmware)
{
}

MultiModelPrinter::~MultiModelPrinter()
{
  clearModels();
}

void MultiModelPrinter::setModel(int idx, const ModelData * model, const GeneralSettings * generalSettings)
{
  if (modelPrinterMap.contains(idx) && modelPrinterMap.value(idx).second) {
    // free existing model printer
    delete modelPrinterMap.value(idx).second;
    modelPrinterMap[idx].second = NULL;
  }

  QPair<const ModelData *, ModelPrinter *> pair(model, new ModelPrinter(firmware, *generalSettings, *model));
  modelPrinterMap.insert(idx, pair);  // QMap.insert will replace any existing key
}

void MultiModelPrinter::setModel(int idx, const ModelData * model)
{
  setModel(idx, model, &defaultSettings);
}

void MultiModelPrinter::clearModels()
{
  for(int i=0; i < modelPrinterMap.size(); i++) {
    if (modelPrinterMap.value(i).second)
      delete modelPrinterMap.value(i).second;
  }
  modelPrinterMap.clear();
}

QString MultiModelPrinter::print(QTextDocument * document)
{
  if (document) document->clear();

  QString str = "<table border='1' cellspacing='0' cellpadding='3' width='100%' style='font-family: monospace;'>";
  str.append(printSetup());
  if (firmware->getCapability(Timers)) {
    str.append(printTimers());
  }
  str.append(printModules());
  if (firmware->getCapability(Heli))
    str.append(printHeliSetup());
  if (firmware->getCapability(FlightModes))
    str.append(printFlightModes());
  str.append(printInputs());
  str.append(printMixers());
  str.append(printOutputs());
  str += printCurves(document);
  if (firmware->getCapability(Gvars) && !firmware->getCapability(GvarsFlightModes))
    str.append(printGvars());
  str.append(printLogicalSwitches());
  str.append(printCustomFunctions());
  str.append(printTelemetry());
  str.append(printSensors());
  str.append("</table>");
  return str;
}

QString MultiModelPrinter::printSetup()
{
  QString str = printTitle(tr("General"));

  MultiColumns columns(modelPrinterMap.size());
  columns.appendTitle(tr("Name:"));
  COMPARE(model->name);
  columns.append(tr("  EEprom Size: "));
  COMPARE(modelPrinter->printEEpromSize());
  if (firmware->getCapability(ModelImage)) {
    columns.append(tr("  Model Image: "));
    COMPARE(model->bitmap);
  }
  columns.append("<br/>");
  columns.appendTitle(tr("Throttle:"));
  COMPARE(modelPrinter->printThrottle());
  columns.append("<br/>");
  columns.appendTitle(tr("Trims:"));
  COMPARE(modelPrinter->printSettingsTrim());
  columns.append("<br/>");
  columns.appendTitle(tr("Center Beep:"));
  COMPARE(modelPrinter->printCenterBeep());
  columns.append("<br/>");
  columns.appendTitle(tr("Switch Warnings:"));
  COMPARE(modelPrinter->printSwitchWarnings());
  columns.append("<br/>");
  if (IS_HORUS_OR_TARANIS(firmware->getBoard())) {
      columns.appendTitle(tr("Pot Warnings:"));
      COMPARE(modelPrinter->printPotWarnings());
      columns.append("<br/>");
  }
  columns.appendTitle(tr("Other:"));
  COMPARE(modelPrinter->printSettingsOther());
  str.append(columns.print());
  return str;
}

QString MultiModelPrinter::printTimers()
{
  QString str;
  MultiColumns columns(modelPrinterMap.size());
  columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  columns.append("<tr>");
  columns.append("<td><b>" + tr("Timers") + "</b></td>");
  columns.append("<td><b>" + tr("Time") + "</b></td>");
  columns.append("<td><b>" + tr("Switch") + "</b></td>");
  columns.append("<td><b>" + tr("Countdown") + "</b></td>");
  columns.append("<td><b>" + tr("Minute call") + "</b></td>");
  columns.append("<td><b>" + tr("Persistence") + "</b></td>");
  columns.append("</tr>");

  for (int i=0; i<firmware->getCapability(Timers); i++) {
    columns.append("<tr><td><b>");
    COMPARE(modelPrinter->printTimerName(i));
    columns.append("</b></td><td>");
    COMPARE(modelPrinter->printTimerTimeValue(model->timers[i].val));
    columns.append("</td><td>");
    COMPARE(model->timers[i].mode.toString());
    columns.append("</td><td>");
    COMPARE(modelPrinter->printTimerCountdownBeep(model->timers[i].countdownBeep));
    columns.append("</td><td>");
    COMPARE(modelPrinter->printTimerMinuteBeep(model->timers[i].minuteBeep));
    columns.append("</td><td>");
    COMPARE(modelPrinter->printTimerPersistent(model->timers[i].persistent));
    columns.append("</td></tr>");
  }
  columns.append("</table>");
  str.append(columns.print());
  return str;
}

QString MultiModelPrinter::printModules()
{
  QString str = printTitle(tr("Modules"));
  MultiColumns columns(modelPrinterMap.size());
  columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  for (int i=0; i<firmware->getCapability(NumModules); i++) {
    columns.append("<tr><td>");
    columns.appendTitle(tr("%1:").arg(i+1));
    COMPARE(modelPrinter->printModule(i));
    columns.append("</td></tr>");
  }
  if (firmware->getCapability(ModelTrainerEnable))
    columns.append("<tr><td>");
    columns.appendTitle(tr("Trainer port:"));
    COMPARE(modelPrinter->printModule(-1));
    columns.append("</td></tr>");
  columns.append("</table>");
  str.append(columns.print());
  return str;
}

QString MultiModelPrinter::printHeliSetup()
{
  bool heliEnabled = false;
  for (int k=0; k < modelPrinterMap.size(); k++) {
    heliEnabled =  heliEnabled || modelPrinterMap.value(k).first->swashRingData.type != HELI_SWASH_TYPE_NONE;
  }

  if (!heliEnabled)
    return "";

  QString str = printTitle(tr("Helicopter"));
  MultiColumns columns(modelPrinterMap.size());
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
    MultiColumns columns(modelPrinterMap.size());
    columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
    columns.append("<tr>");
    columns.append("<td><b>" + tr("Flight mode") + "</b></td>");
    columns.append("<td><b>" + tr("Switch") + "</b></td>");
    columns.append("<td><b>" + tr("Fade IN") + "</b></td>");
    columns.append("<td><b>" + tr("Fade OUT") + "</b></td>");
    for (int i=0; i < getBoardCapability(getCurrentBoard(), Board::NumTrims); i++) {
      columns.append("<td><b>" + RawSource(SOURCE_TYPE_TRIM, i).toString() + "</b></td>");
    }
    columns.append("</tr>");

    for (int i=0; i<firmware->getCapability(FlightModes); i++) {
      columns.append("<tr><td><b>" + tr("FM%1").arg(i) + "</b>&nbsp;");
      COMPARE(model->flightModeData[i].name);
      columns.append("</td><td>");
      COMPARE(modelPrinter->printFlightModeSwitch(model->flightModeData[i].swtch));
      columns.append("</td><td>");
      COMPARE(model->flightModeData[i].fadeIn);
      columns.append("</td><td>");
      COMPARE(model->flightModeData[i].fadeOut);
      columns.append("</td>");
      for (int k=0; k < getBoardCapability(getCurrentBoard(), Board::NumTrims); k++) {
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
    MultiColumns columns(modelPrinterMap.size());
    columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
    columns.append("<tr><td><b>" + tr("Global variables") + "</b></td>");
    if (firmware->getCapability(GvarsFlightModes)) {
      for (int i=0; i<gvars; i++) {
        columns.append("<td><b>" + tr("GV%1").arg(i+1) + "</b></td>");
      }
    }
    for (int i=0; i<firmware->getCapability(RotaryEncoders); i++) {
      columns.append("<td><b>" + tr("RE%1").arg(i+1) + "</b></td>");
    }
    columns.append("</tr>");

    if (firmware->getCapability(GvarsFlightModes)) {
      columns.append("<tr><td><b>Name</b></td>");
      for (int i=0; i<gvars; i++) {
        columns.append("<td>");
        COMPARE(model->gvarData[i].name);
        columns.append("</td>");
      }
      columns.append("</tr>");
      columns.append("<tr><td><b>Unit</b></td>");
      for (int i=0; i<gvars; i++) {
        columns.append("<td>");
        COMPARE(modelPrinter->printGlobalVarUnit(i));
        columns.append("</td>");
      }
      columns.append("</tr>");
      columns.append("<tr><td><b>Prec</b></td>");
      for (int i=0; i<gvars; i++) {
        columns.append("<td>");
        COMPARE(modelPrinter->printGlobalVarPrec(i));
        columns.append("</td>");
      }
      columns.append("</tr>");
      columns.append("<tr><td><b>Min</b></td>");
      for (int i=0; i<gvars; i++) {
        columns.append("<td>");
        COMPARE(modelPrinter->printGlobalVarMin(i));
        columns.append("</td>");
      }
      columns.append("</tr>");
      columns.append("<tr><td><b>Max</b></td>");
      for (int i=0; i<gvars; i++) {
        columns.append("<td>");
        COMPARE(modelPrinter->printGlobalVarMax(i));
        columns.append("</td>");
      }
      columns.append("</tr>");
      columns.append("<tr><td><b>Popup</b></td>");
      for (int i=0; i<gvars; i++) {
        columns.append("<td>");
        COMPARE(modelPrinter->printGlobalVarPopup(i));
        columns.append("</td>");
      }
      columns.append("</tr>");
    }

    columns.append("<tr><td><b>" + tr("Flight mode") + "</b></td></tr>");

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

QString MultiModelPrinter::printOutputs()
{
  QString str = printTitle(tr("Outputs"));
  MultiColumns columns(modelPrinterMap.size());
  columns.append("<table border='0' cellspacing='0' cellpadding='1' width='100%'>" \
                 "<tr>" \
                 "<td><b>" + tr("Channel") + "</b></td>" \
                 "<td><b>" + tr("Subtrim") + "</b></td>" \
                 "<td><b>" + tr("Min") + "</b></td>" \
                 "<td><b>" + tr("Max") + "</b></td>" \
                 "<td><b>" + tr("Direct") + "</b></td>");
  if (IS_HORUS_OR_TARANIS(firmware->getBoard()))
    columns.append(" <td><b>" + tr("Curve") + "</b></td>");
  if (firmware->getCapability(PPMCenter))
    columns.append(" <td><b>" + tr("PPM") + "</b></td>");
  if (firmware->getCapability(SYMLimits))
    columns.append(" <td><b>" + tr("Linear") + "</b></td>");
  columns.append("</tr>");
  for (int i=0; i<firmware->getCapability(Outputs); i++) {
    int count = 0;
    for (int k=0; k < modelPrinterMap.size(); k++)
      count = std::max(count, modelPrinterMap.value(k).first->mixes(i).size());
    if (!count)
      continue;
    columns.append("<tr><td><b>");
    COMPARE(modelPrinter->printChannelName(i));
    columns.append("</td><td>");
    COMPARE(modelPrinter->printOutputOffset(i));
    columns.append("</td><td>");
    COMPARE(modelPrinter->printOutputMin(i));
    columns.append("</td><td>");
    COMPARE(modelPrinter->printOutputMax(i));
    columns.append("</td><td>");
    COMPARE(modelPrinter->printOutputRevert(i));
    columns.append("</td>");
    if (IS_HORUS_OR_TARANIS(firmware->getBoard())) {
      columns.append("<td>");
      COMPARE(modelPrinter->printOutputCurve(i));
      columns.append("</td>");
    }
    if (firmware->getCapability(PPMCenter)) {
      columns.append("<td>");
      COMPARE(modelPrinter->printOutputPpmCenter(i));
      columns.append("</td>");
    }
    if (firmware->getCapability(SYMLimits)) {
      columns.append("<td>");
      COMPARE(modelPrinter->printOutputSymetrical(i));
      columns.append("</td>");
    }
    columns.append("</tr>");
  }
  columns.append("</table>");
  str.append(columns.print());
  return str;
}

QString MultiModelPrinter::printGvars()
{
  QString str = printTitle(tr("Global Variables"));
  int gvars = firmware->getCapability(Gvars);
  MultiColumns columns(modelPrinterMap.size());
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
  MultiColumns columns(modelPrinterMap.size());
  columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  for (int i=0; i<std::max(4, firmware->getCapability(VirtualInputs)); i++) {
    int count = 0;
    for (int k=0; k < modelPrinterMap.size(); k++) {
      count = std::max(count, modelPrinterMap.value(k).first->expos(i).size());
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
  MultiColumns columns(modelPrinterMap.size());
  columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  for (int i=0; i<firmware->getCapability(Outputs); i++) {
    int count = 0;
    for (int k=0; k < modelPrinterMap.size(); k++) {
      count = std::max(count, modelPrinterMap.value(k).first->mixes(i).size());
    }
    if (count > 0) {
      columns.append("<tr><td width='20%'><b>");
      COMPARE(modelPrinter->printChannelName(i));
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
  MultiColumns columns(modelPrinterMap.size());
  int count = 0;
  columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  for (int i=0; i<firmware->getCapability(NumCurves); i++) {
    bool curveEmpty = true;
    for (int k=0; k < modelPrinterMap.size(); k++) {
      if (!modelPrinterMap.value(k).first->curves[i].isEmpty()) {
        curveEmpty = false;
        break;
      }
    }
    if (!curveEmpty) {
      count++;
      columns.append("<tr><td width='20%'><b>");
      COMPARE(modelPrinter->printCurveName(i));
      columns.append("</b></td><td>");
      COMPARE(modelPrinter->printCurve(i));
      for (int k=0; k < modelPrinterMap.size(); k++)
        columns.append(k, QString("<br/><img src='%1' border='0' />").arg(modelPrinterMap.value(k).second->createCurveImage(i, document)));
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
  MultiColumns columns(modelPrinterMap.size());
  int count = 0;
  columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  for (int i=0; i<firmware->getCapability(LogicalSwitches); i++) {
    bool lsEmpty = true;
    for (int k=0; k < modelPrinterMap.size(); k++) {
      if (!modelPrinterMap.value(k).first->logicalSw[i].isEmpty()) {
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
  MultiColumns columns(modelPrinterMap.size());
  int count = 0;
  columns.append("<table cellspacing='0' cellpadding='1' width='100%' border='0' style='border-collapse:collapse'>");
  for (int i=0; i < firmware->getCapability(CustomFunctions); i++) {
    bool sfEmpty = true;
    for (int k=0; k < modelPrinterMap.size(); k++) {
      if (!modelPrinterMap.value(k).first->customFn[i].isEmpty()) {
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
  QString str = printTitle(tr("Telemetry"));
  MultiColumns columns(modelPrinterMap.size());
  columns.appendSectionTableStart();

  // Analogs on non ARM boards
  if (!IS_ARM(firmware->getBoard())) {
    columns.appendHeaderRow(QStringList() << tr("Analogs") << tr("Unit") << tr("Scale") << tr("Offset"));
    for (int i=0; i<2; i++) {
      columns.appendRowStart();
      columns.appendValueCell(QString("A%1").arg(i+1),20);
      COMPARECELL(getFrSkyUnits(model->frsky.channels[i].type));
      COMPARECELL(QString::number((model->frsky.channels[i].ratio / (model->frsky.channels[i].type==0 ? 10.0 : 1)), 10, (model->frsky.channels[i].type==0 ? 1 : 0)));
      COMPARECELL(QString::number((model->frsky.channels[i].offset*(model->frsky.channels[i].ratio / (model->frsky.channels[i].type==0 ?10.0 : 1)))/255, 10, (model->frsky.channels[i].type==0 ? 1 : 0)));
      columns.appendRowEnd();
    }
  }
  else {
    // Protocol
    columns.appendRowStart(tr("Protocol"),20);
    COMPARECELL(modelPrinter->printTelemetryProtocol(model->telemetryProtocol));
    columns.appendRowEnd();
  }

  // RSSI alarms
  {
    columns.appendRowStart(tr("RSSI Alarms"),20);
    columns.appendCellStart(80);
    COMPARE((IS_ARM(getCurrentBoard()) ? tr("Low") : getFrSkyAlarmType(model->rssiAlarms.level[0])));
    columns.append(": ");
    COMPARE(QString("&lt; %1").arg(QString::number(model->rssiAlarms.warning, 10)));
    columns.append("; ");
    COMPARE((IS_ARM(getCurrentBoard()) ? tr("Critical") : getFrSkyAlarmType(model->rssiAlarms.level[1])));
    columns.append(": ");
    COMPARE(QString("&lt; %1").arg(QString::number(model->rssiAlarms.critical, 10)));
    columns.append("; ");
    COMPARELABELVALUE("Telemetry audio:", modelPrinter->printRssiAlarmsDisabled(model->rssiAlarms.disabled));
    columns.appendCellEnd();
    columns.appendRowEnd();
  }

  // Altimetry
  if (firmware->getCapability(HasVario)) {
    columns.appendRowStart(tr("Altimetry"),20);
    columns.appendRowEnd();
    columns.appendRowStart(tr("Vario Source"));
    if (IS_HORUS_OR_TARANIS(firmware->getBoard())) {
      COMPARECELL(modelPrinter->printTelemetrySource(model->frsky.varioSource));
    }
    else {
      COMPARECELL(modelPrinter->printVarioSource(model->frsky.varioSource));
    }
    columns.appendRowEnd();
    columns.appendRowStart(tr("Vario limits"));
    columns.appendCellStart(80);
    if (firmware->getCapability(HasVarioSink)) {
      COMPARELABELVALUE("Sink max:", QString::number(model->frsky.varioMin - 10));
      columns.append("; ");
      COMPARELABELVALUE("Sink min:", QString::number((float(model->frsky.varioCenterMin) / 10.0) - 0.5));
      columns.append("; ");
    }
    COMPARELABELVALUE("Climb min:", QString::number((float(model->frsky.varioCenterMax) / 10.0) + 0.5));
    columns.append("; ");
    COMPARELABELVALUE("Climb max:", QString::number(model->frsky.varioMax + 10));
    columns.append("; ");
    COMPARELABELVALUE("Center silent:", modelPrinter->printVarioCenterSilent(model->frsky.varioCenterSilent));
    columns.appendCellEnd();
    columns.appendRowEnd();
  }

  // Top Bar
  if (IS_HORUS_OR_TARANIS(firmware->getBoard())) {
    columns.appendRowStart(tr("Top Bar"),20);
    columns.appendRowEnd();
    columns.appendRowStart(tr("Volts source"),20);
    columns.appendCellStart(80);
    COMPARE(modelPrinter->printTelemetrySource(model->frsky.voltsSource));
    columns.append("; ");
    COMPARELABELVALUE("Altitude source:", modelPrinter->printTelemetrySource(model->frsky.altitudeSource));
    columns.appendCellEnd();
    columns.appendRowEnd();
  }

  // Various
  if (!IS_ARM(firmware->getBoard())) {
    columns.appendRowStart(tr("Various"), 20);
    columns.appendRowEnd();
    if (!firmware->getCapability(NoTelemetryProtocol)) {
      columns.appendRowStart();
      columns.appendValueCell("", 20);
      columns.appendCellStart(80);
      COMPARELABELVALUE(tr("Serial protocol"), modelPrinter->printTelemetrySource(model->frsky.voltsSource));
      columns.appendCellEnd();
      columns.appendRowEnd();
    }
    columns.appendRowStart();
    columns.appendValueCell("", 20);
    columns.appendCellStart(80);
    QString firmware_id = g.profile[g.id()].fwType();
    if (firmware->getCapability(HasFasOffset) && firmware_id.contains("fasoffset")) {
      COMPARELABELVALUE(tr("FAS offset:"), QString("%1 A").arg(model->frsky.fasOffset/10.0));
      columns.append("; ");
    }
    if (firmware->getCapability(HasMahPersistent)) {
      COMPARELABELVALUE(tr("mAh count:"), QString("%1 mAh").arg(model->frsky.storedMah));
      columns.append("; ");
      COMPARELABELVALUE(tr("Persistent mAh:"), modelPrinter->printMahPersistent(model->frsky.mAhPersistent));
    }
    columns.appendRowEnd();
    columns.appendRowStart();
    columns.appendValueCell("", 20);
    columns.appendCellStart(80);
    COMPARELABELVALUE(tr("Volts source:"), modelPrinter->printVoltsSource(model->frsky.voltsSource));
    columns.append("; ");
    COMPARELABELVALUE(tr("Current source:"), modelPrinter->printCurrentSource(model->frsky.currentSource));
    columns.appendCellEnd();
    columns.appendRowEnd();
    columns.appendRowStart();
    columns.appendValueCell("", 20);
    columns.appendCellStart(80);
    COMPARELABELVALUE(tr("Blades:"), model->frsky.blades);
    columns.appendRowEnd();
  }
  columns.appendTableEnd();
  str.append(columns.print());
  return str;
}
