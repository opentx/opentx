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

#include <QtGui>
#if defined _MSC_VER
  #include <io.h>
  #include <stdio.h>
#elif defined __GNUC__
  #include <unistd.h>
#endif

#include "appdata.h"
#include "macros.h"
#include "helpers.h"
#include "simulatorinterface.h"
#include "simulatormainwindow.h"
#include "storage/sdcard.h"

#include <QLabel>
#include <QMessageBox>

using namespace Helpers;

Stopwatch gStopwatch("global");

const QColor colors[CPN_MAX_CURVES] = {
  QColor(0,0,127),
  QColor(0,127,0),
  QColor(127,0,0),
  QColor(0,127,127),
  QColor(127,0,127),
  QColor(127,127,0),
  QColor(127,127,127),
  QColor(0,0,255),
  QColor(0,127,255),
  QColor(127,0,255),
  QColor(0,255,0),
  QColor(0,255,127),
  QColor(127,255,0),
  QColor(255,0,0),
  QColor(255,0,127),
  QColor(255,127,0),
  QColor(0,0,127),
  QColor(0,127,0),
  QColor(127,0,0),
  QColor(0,127,127),
  QColor(127,0,127),
  QColor(127,127,0),
  QColor(127,127,127),
  QColor(0,0,255),
  QColor(0,127,255),
  QColor(127,0,255),
  QColor(0,255,0),
  QColor(0,255,127),
  QColor(127,255,0),
  QColor(255,0,0),
  QColor(255,0,127),
  QColor(255,127,0),
};


/*
 * CompanionIcon
*/

static QString iconThemeFolder(int theme_set)
{
  switch(theme_set) {
    case 0:
      return QStringLiteral("classic");
    case 2:
      return QStringLiteral("monowhite");
    case 3:
      return QStringLiteral("monochrome");
    case 4:
      return QStringLiteral("monoblue");
    default:
      return QStringLiteral("yerico");
  }
}

CompanionIcon::CompanionIcon(const QString &baseimage)
{
  addImage(baseimage);
}

void CompanionIcon::addImage(const QString & baseimage, Mode mode, State state)
{
  const QString theme = iconThemeFolder(g.theme());
  addFile(":/themes/"+theme+"/16/"+baseimage, QSize(16,16), mode, state);
  addFile(":/themes/"+theme+"/24/"+baseimage, QSize(24,24), mode, state);
  addFile(":/themes/"+theme+"/32/"+baseimage, QSize(32,32), mode, state);
  addFile(":/themes/"+theme+"/48/"+baseimage, QSize(48,48), mode, state);
}


/*
 * GVarGroup
*/

GVarGroup::GVarGroup(QCheckBox * weightGV, QAbstractSpinBox * weightSB, QComboBox * weightCB, int & weight, const ModelData & model, const int deflt, const int mini, const int maxi, const double step, bool allowGvars):
  QObject(),
  weightGV(weightGV),
  weightSB(weightSB),
  sb(dynamic_cast<QSpinBox *>(weightSB)),
  dsb(dynamic_cast<QDoubleSpinBox *>(weightSB)),
  weightCB(weightCB),
  weight(weight),
  deflt(deflt),
  mini(mini),
  maxi(maxi),
  step(step),
  lock(true)
{
  if (allowGvars && getCurrentFirmware()->getCapability(Gvars)) {
    Helpers::populateGVCB(*weightCB, weight, model);
    connect(weightGV, SIGNAL(stateChanged(int)), this, SLOT(gvarCBChanged(int)));
    connect(weightCB, SIGNAL(currentIndexChanged(int)), this, SLOT(valuesChanged()));
  }
  else {
    weightGV->hide();
    if (weight > maxi || weight < mini) {
      weight = deflt;
    }
  }

  setWeight(weight);

  connect(weightSB, SIGNAL(editingFinished()), this, SLOT(valuesChanged()));

  lock = false;
}

void GVarGroup::gvarCBChanged(int state)
{
  weightCB->setVisible(state);
  if (weightSB)
    weightSB->setVisible(!state);
  else
    weightSB->setVisible(!state);
  valuesChanged();
}

void GVarGroup::valuesChanged()
{
  if (!lock) {
    if (weightGV->isChecked())
      weight = weightCB->itemData(weightCB->currentIndex()).toInt();
    else if (sb)
      weight = sb->value();
    else
      weight = round(dsb->value()/step);

    emit valueChanged();
  }
}

void GVarGroup::setWeight(int val)
{
  lock = true;

  int tval;

  if (val>maxi || val<mini) {
    tval = deflt;
    weightGV->setChecked(true);
    weightSB->hide();
    weightCB->setCurrentIndex(weightCB->findData(val));
    if (weightCB->currentIndex() == -1)
      weightCB->setCurrentIndex(getCurrentFirmware()->getCapability(Gvars));
    weightCB->show();
  }
  else {
    tval = val;
    weightGV->setChecked(false);
    weightSB->show();
    weightCB->hide();
  }

  if (sb) {
    sb->setMinimum(mini);
    sb->setMaximum(maxi);
    sb->setValue(tval);
  }
  else {
    dsb->setMinimum(mini*step);
    dsb->setMaximum(maxi*step);
    dsb->setValue(tval*step);
  }

  lock = false;
}


/*
 * CurveGroup
*/

CurveGroup::CurveGroup(QComboBox * curveTypeCB, QCheckBox * curveGVarCB, QComboBox * curveValueCB, QSpinBox * curveValueSB, CurveReference & curve, const ModelData & model, unsigned int flags):
  QObject(),
  curveTypeCB(curveTypeCB),
  curveGVarCB(curveGVarCB),
  curveValueCB(curveValueCB),
  curveValueSB(curveValueSB),
  curve(curve),
  model(model),
  flags(flags),
  lock(false),
  lastType(-1)
{
  if (!(flags & HIDE_DIFF)) curveTypeCB->addItem(tr("Diff"), 0);
  if (!(flags & HIDE_EXPO)) curveTypeCB->addItem(tr("Expo"), 1);
  curveTypeCB->addItem(tr("Func"), 2);
  curveTypeCB->addItem(tr("Curve"), 3);

  curveValueCB->setMaxVisibleItems(10);

  connect(curveTypeCB, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));
  connect(curveGVarCB, SIGNAL(stateChanged(int)), this, SLOT(gvarCBChanged(int)));
  connect(curveValueCB, SIGNAL(currentIndexChanged(int)), this, SLOT(valuesChanged()));
  connect(curveValueSB, SIGNAL(editingFinished()), this, SLOT(valuesChanged()));

  update();
}

void CurveGroup::update()
{
  lock = true;

  int found = curveTypeCB->findData(curve.type);
  if (found < 0) found = 0;
  curveTypeCB->setCurrentIndex(found);

  if (curve.type == CurveReference::CURVE_REF_DIFF || curve.type == CurveReference::CURVE_REF_EXPO) {
    curveGVarCB->setVisible(getCurrentFirmware()->getCapability(Gvars));
    if (curve.value > 100 || curve.value < -100) {
      curveGVarCB->setChecked(true);
      if (lastType != CurveReference::CURVE_REF_DIFF && lastType != CurveReference::CURVE_REF_EXPO) {
        lastType = curve.type;
        Helpers::populateGVCB(*curveValueCB, curve.value, model);
      }
      curveValueCB->show();
      curveValueSB->hide();
    }
    else {
      curveGVarCB->setChecked(false);
      curveValueSB->setMinimum(-100);
      curveValueSB->setMaximum(100);
      curveValueSB->setValue(curve.value);
      curveValueSB->show();
      curveValueCB->hide();
    }
  }
  else {
    curveGVarCB->hide();
    curveValueSB->hide();
    curveValueCB->show();
    switch (curve.type) {
      case CurveReference::CURVE_REF_FUNC:
        if (lastType != curve.type) {
          lastType = curve.type;
          curveValueCB->clear();
          for (int i=0; i<=6/*TODO constant*/; i++) {
            curveValueCB->addItem(CurveReference(CurveReference::CURVE_REF_FUNC, i).toString(&model, false));
          }
        }
        curveValueCB->setCurrentIndex(curve.value);
        break;
      case CurveReference::CURVE_REF_CUSTOM:
      {
        int numcurves = getCurrentFirmware()->getCapability(NumCurves);
        if (lastType != curve.type) {
          lastType = curve.type;
          curveValueCB->clear();
          for (int i= ((flags & HIDE_NEGATIVE_CURVES) ? 0 : -numcurves); i<=numcurves; i++) {
            curveValueCB->addItem(CurveReference(CurveReference::CURVE_REF_CUSTOM, i).toString(&model, false), i);
            if (i == curve.value) {
              curveValueCB->setCurrentIndex(curveValueCB->count() - 1);
            }
          }
        }
        break;
      }
      default:
        break;
    }
  }

  lock = false;
}

void CurveGroup::gvarCBChanged(int state)
{
  if (!lock) {
    if (state) {
      curve.value = 10000+1; // TODO constant in EEpromInterface ...
      lastType = -1; // quickfix for issue #3518: force refresh of curveValueCB at next update() to set current index to GV1
    }
    else {
      curve.value = 0; // TODO could be better
    }

    update();
  }
}

void CurveGroup::typeChanged(int value)
{
  if (!lock) {
    int type = curveTypeCB->itemData(curveTypeCB->currentIndex()).toInt();
    switch (type) {
      case 0:
        curve = CurveReference(CurveReference::CURVE_REF_DIFF, 0);
        break;
      case 1:
        curve = CurveReference(CurveReference::CURVE_REF_EXPO, 0);
        break;
      case 2:
        curve = CurveReference(CurveReference::CURVE_REF_FUNC, 0);
        break;
      case 3:
        curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, 0);
        break;
    }

    update();
  }
}

void CurveGroup::valuesChanged()
{
  if (!lock) {
    switch (curveTypeCB->itemData(curveTypeCB->currentIndex()).toInt()) {
      case 0:
      case 1:
      {
        int value;
        if (curveGVarCB->isChecked())
          value = curveValueCB->itemData(curveValueCB->currentIndex()).toInt();
        else
          value = curveValueSB->value();
        curve = CurveReference(curveTypeCB->itemData(curveTypeCB->currentIndex()).toInt() == 0 ? CurveReference::CURVE_REF_DIFF : CurveReference::CURVE_REF_EXPO, value);
        break;
      }
      case 2:
        curve = CurveReference(CurveReference::CURVE_REF_FUNC, curveValueCB->currentIndex());
        break;
      case 3:
        curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, curveValueCB->itemData(curveValueCB->currentIndex()).toInt());
        break;
    }

    update();
  }
}


/*
 * Helpers namespace functions
*/

void Helpers::populateGVCB(QComboBox & b, int value, const ModelData & model)
{
  int count = getCurrentFirmware()->getCapability(Gvars);

  b.clear();

  for (int i=-count; i<=-1; i++) {
    int16_t gval = (int16_t)(-10000+i);
    b.addItem("-" + RawSource(SOURCE_TYPE_GVAR, abs(i)-1).toString(&model), gval);
  }

  for (int i=1; i<=count; i++) {
    int16_t gval = (int16_t)(10000+i);
    b.addItem(RawSource(SOURCE_TYPE_GVAR, i-1).toString(&model), gval);
  }

  b.setCurrentIndex(b.findData(value));
  if (b.currentIndex() == -1)
    b.setCurrentIndex(count);
}

// Returns Diff/Expo/Weight/Offset adjustment value as either a percentage or a global variable name.
QString Helpers::getAdjustmentString(int16_t val, const ModelData * model, bool sign)
{
  QString ret;
  if (val >= -10000 && val <= 10000) {
    ret = "%1%";
    if (sign && val > 0)
      ret.prepend("+");
    ret = ret.arg(val);
  }
  else {
    ret = RawSource(SOURCE_TYPE_GVAR, abs(val) - 10001).toString(model);
    if (val < 0)
      ret.prepend("-");
    else if (sign)
      ret.prepend("+");
  }
  return ret;
}

// TODO: Move lookup to GVarData class (w/out combobox)
void Helpers::populateGvarUseCB(QComboBox * b, unsigned int phase)
{
  b->addItem(QCoreApplication::translate("GVarData", "Own value"));
  for (int i=0; i<getCurrentFirmware()->getCapability(FlightModes); i++) {
    if (i != (int)phase) {
      b->addItem(QCoreApplication::translate("GVarData", "Flight mode %1 value").arg(i));
    }
  }
}

static bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
  return s1.toLower() < s2.toLower();
}

void Helpers::populateFileComboBox(QComboBox * b, const QSet<QString> & set, const QString & current)
{
  b->clear();
  b->addItem("----");

  bool added = false;
  // Convert set into list and sort it alphabetically case insensitive
  QStringList list = QStringList::fromSet(set);
  qSort(list.begin(), list.end(), caseInsensitiveLessThan);
  foreach (QString entry, list) {
    b->addItem(entry);
    if (entry == current) {
      b->setCurrentIndex(b->count()-1);
      added = true;
    }
  }

  if (!added && !current.isEmpty()) {
    b->addItem(current);
    b->setCurrentIndex(b->count()-1);
  }
}

void Helpers::getFileComboBoxValue(QComboBox * b, char * dest, int length)
{
  memset(dest, 0, length+1);
  if (b->currentText() != "----") {
    strncpy(dest, b->currentText().toLatin1(), length);
  }
}

void Helpers::addRawSourceItems(QStandardItemModel * itemModel, const RawSourceType & type, int count, const GeneralSettings * const generalSettings,
                                const ModelData * const model, const int start)
{
  for (int i = start; i < start + count; i++) {
    RawSource src = RawSource(type, i);
    if (!src.isAvailable(model, generalSettings, getCurrentBoard()))
      continue;

    QStandardItem * modelItem = new QStandardItem(src.toString(model, generalSettings));
    modelItem->setData(src.toValue(), Qt::UserRole);
    itemModel->appendRow(modelItem);
  }
}

QStandardItemModel * Helpers::getRawSourceItemModel(const GeneralSettings * const generalSettings, const ModelData * const model, unsigned int flags)
{
  QStandardItemModel * itemModel = new QStandardItemModel();
  Boards board = Boards(getCurrentBoard());
  Firmware * fw = getCurrentFirmware();

  if (flags & POPULATE_NONE) {
    addRawSourceItems(itemModel, SOURCE_TYPE_NONE, 1, generalSettings, model);
  }

  if (flags & POPULATE_SCRIPT_OUTPUTS) {
    for (int i=0; i < getCurrentFirmware()->getCapability(LuaScripts); i++) {
      addRawSourceItems(itemModel, SOURCE_TYPE_LUA_OUTPUT, fw->getCapability(LuaOutputsPerScript), generalSettings, model, i * 16);
    }
  }

  if (model && (flags & POPULATE_VIRTUAL_INPUTS)) {
    addRawSourceItems(itemModel, SOURCE_TYPE_VIRTUAL_INPUT, fw->getCapability(VirtualInputs), generalSettings, model);
  }

  if (flags & POPULATE_SOURCES) {
    int totalSources = CPN_MAX_STICKS + board.getCapability(Board::Pots) + board.getCapability(Board::Sliders) +  board.getCapability(Board::MouseAnalogs);
    addRawSourceItems(itemModel, SOURCE_TYPE_STICK, totalSources, generalSettings, model);
    addRawSourceItems(itemModel, SOURCE_TYPE_ROTARY_ENCODER, fw->getCapability(RotaryEncoders), generalSettings, model);
  }

  if (flags & POPULATE_TRIMS) {
    addRawSourceItems(itemModel, SOURCE_TYPE_TRIM, board.getCapability(Board::NumTrims), generalSettings, model);
  }

  if (flags & POPULATE_SOURCES) {
    addRawSourceItems(itemModel, SOURCE_TYPE_MAX, 1, generalSettings, model);
  }

  if (flags & POPULATE_SWITCHES) {
    addRawSourceItems(itemModel, SOURCE_TYPE_SWITCH, board.getCapability(Board::Switches), generalSettings, model);
    addRawSourceItems(itemModel, SOURCE_TYPE_CUSTOM_SWITCH, fw->getCapability(LogicalSwitches), generalSettings, model);
  }

  if (flags & POPULATE_SOURCES) {
    addRawSourceItems(itemModel, SOURCE_TYPE_CYC, CPN_MAX_CYC, generalSettings, model);
    addRawSourceItems(itemModel, SOURCE_TYPE_PPM, fw->getCapability(TrainerInputs), generalSettings, model);
    addRawSourceItems(itemModel, SOURCE_TYPE_CH, fw->getCapability(Outputs), generalSettings, model);
  }

  if (flags & POPULATE_TELEMETRY) {
    int count = 0;
    if (IS_ARM(board.getBoardType())) {
      addRawSourceItems(itemModel, SOURCE_TYPE_SPECIAL, 5, generalSettings, model);
      count = CPN_MAX_SENSORS * 3;
    }
    else {
      count = ((flags & POPULATE_TELEMETRYEXT) ? TELEMETRY_SOURCES_STATUS_COUNT : TELEMETRY_SOURCES_COUNT);
    }
    if (model && count)
      addRawSourceItems(itemModel, SOURCE_TYPE_TELEMETRY, count, generalSettings, model);
  }

  if (flags & POPULATE_GVARS) {
    addRawSourceItems(itemModel, SOURCE_TYPE_GVAR, fw->getCapability(Gvars), generalSettings, model);
  }

  return itemModel;
}

void startSimulation(QWidget * parent, RadioData & radioData, int modelIdx)
{
  QString fwId = SimulatorLoader::findSimulatorByFirmwareName(getCurrentFirmware()->getId());
  if (fwId.isEmpty()) {
    QMessageBox::warning(NULL,
                         CPN_STR_TTL_WARNING,
                         QCoreApplication::translate("Companion", "Simulator for this firmware is not yet available"));
    return;
  }

  RadioData * simuData = new RadioData(radioData);
  unsigned int flags = 0;

  if (modelIdx >= 0) {
    flags |= SIMULATOR_FLAGS_NOTX;
    simuData->setCurrentModel(modelIdx);
  }

  SimulatorMainWindow * dialog = new SimulatorMainWindow(parent, fwId, flags);
  dialog->setWindowModality(Qt::ApplicationModal);
  dialog->setAttribute(Qt::WA_DeleteOnClose);

  QObject::connect(dialog, &SimulatorMainWindow::destroyed, [simuData] (void) {
    // TODO simuData and Horus tmp directory is deleted on simulator close OR we could use it to get back data from the simulation
    delete simuData;
  });

  QString resultMsg;
  if (dialog->getExitStatus(&resultMsg)) {
    if (resultMsg.isEmpty())
      resultMsg = QCoreApplication::translate("Companion", "Uknown error during Simulator startup.");
    QMessageBox::critical(NULL, QCoreApplication::translate("Companion", "Simulator Error"), resultMsg);
    dialog->deleteLater();
  }
   else if (dialog->setRadioData(simuData)) {
    dialog->show();
  }
  else {
    QMessageBox::critical(NULL, QCoreApplication::translate("Companion", "Data Load Error"), QCoreApplication::translate("Companion", "Error occurred while starting simulator."));
    dialog->deleteLater();
  }
}

QPixmap makePixMap(const QImage & image)
{
  Firmware * firmware = getCurrentFirmware();
  QImage result = image.scaled(firmware->getCapability(LcdWidth), firmware->getCapability(LcdHeight));
  if (firmware->getCapability(LcdDepth) == 4) {
    result = result.convertToFormat(QImage::Format_RGB32);
    for (int i = 0; i < result.width(); ++i) {
      for (int j = 0; j < result.height(); ++j) {
        QRgb col = result.pixel(i, j);
        int gray = qGray(col);
        result.setPixel(i, j, qRgb(gray, gray, gray));
      }
    }
  }
  else {
    result = result.convertToFormat(QImage::Format_Mono);
  }

  return QPixmap::fromImage(result);
}


int version2index(const QString & version)
{
  int result = 999;
  QStringList parts;
  QString mainVersion = version;
  if (version.contains("RC")) {
    parts = version.split("RC");
    result = parts[1].toInt() + 900; // RC0 = 900; RC1=901,..
    mainVersion = parts[0];
  }
  else if (version.contains("N")) {
    parts = version.split("N");
    result = parts[1].toInt(); // nightly build up to 899
    mainVersion = parts[0];
  }
  parts = mainVersion.split('.');
  if (parts.size() > 2)
    result += 1000 * parts[2].toInt();
  if (parts.size() > 1)
    result += 100000 * parts[1].toInt();
  if (parts.size() > 0)
    result += 10000000 * parts[0].toInt();
  return result;
}

const QString index2version(int index)
{
  QString result;
  QString templt("%1.%2.%3");
  if (index >= 19900000) {
    int nightly = index % 1000;
    index /= 1000;
    int revision = index % 100;
    index /= 100;
    int minor = index % 100;
    int major = index / 100;
    result = templt.arg(major).arg(minor).arg(revision);
    if (nightly > 0 && nightly < 900) {
      result += "N" + QString::number(nightly);
    }
    else if (nightly >= 900 && nightly < 999) {
      result += "RC" + QString::number(nightly-900);
    }
  }
  else if (index >= 19900) {
    int revision = index % 100;
    index /= 100;
    int minor = index % 100;
    int major = index / 100;
    result = templt.arg(major).arg(minor).arg(revision);
  }
  return result;
}

bool qunlink(const QString & fileName)
{
  return QFile::remove(fileName);
}

QString generateProcessUniqueTempFileName(const QString & fileName)
{
  QString sanitizedFileName = fileName;
  sanitizedFileName.remove('/');
  return QDir::tempPath() + QString("/%1-").arg(QCoreApplication::applicationPid()) + sanitizedFileName;
}

bool isTempFileName(const QString & fileName)
{
  return fileName.startsWith(QDir::tempPath());
}

QString getSoundsPath(const GeneralSettings &generalSettings)
{
  QString path = g.profile[g.id()].sdPath() + "/SOUNDS/";
  QString lang = generalSettings.ttsLanguage;
  if (lang.isEmpty())
    lang = "en";
  path.append(lang);
  return path;
}

QSet<QString> getFilesSet(const QString &path, const QStringList &filter, int maxLen)
{
  QSet<QString> result;
  QDir dir(path);
  if (dir.exists()) {
    foreach (QString filename, dir.entryList(filter, QDir::Files)) {
      QFileInfo file(filename);
      QString name = file.completeBaseName();
      if (name.length() <= maxLen) {
        result.insert(name);
      }
    }
  }
  return result;
}

bool GpsGlitchFilter::isGlitch(GpsCoord coord)
{
  if ((fabs(coord.latitude) < 0.1) && (fabs(coord.longitude) < 0.1)) {
    return true;
  }

  if (lastValid) {
    if (fabs(coord.latitude - lastLat) > 0.01) {
      // qDebug() << "GpsGlitchFilter(): latitude glitch " << coord.latitude << lastLat;
      if ( ++glitchCount < 10) {
        return true;
      }
    }
    if (fabs(coord.longitude - lastLon) > 0.01) {
      // qDebug() << "GpsGlitchFilter(): longitude glitch " << coord.longitude << lastLon;
      if ( ++glitchCount < 10) {
        return true;
      }
    }
  }
  lastLat = coord.latitude;
  lastLon = coord.longitude;
  lastValid = true;
  glitchCount = 0;
  return false;
}

bool GpsLatLonFilter::isValid(GpsCoord coord)
{
  if (lastLat == coord.latitude) {
    return false;
  }
  if (lastLon == coord.longitude) {
    return false;
  }
  lastLat = coord.latitude;
  lastLon = coord.longitude;
  return true;
}

double toDecimalCoordinate(const QString & value)
{
  if (value.isEmpty()) return 0.0;
  double temp = int(value.left(value.length()-1).toDouble() / 100);
  double result = temp + (value.left(value.length() - 1).toDouble() - temp * 100) / 60.0;
  QChar direction = value.at(value.size()-1);
  if ((direction == 'S') || (direction == 'W')) {
    result = -result;
  }
  return result;
}

GpsCoord extractGpsCoordinates(const QString & position)
{
  GpsCoord result;
  QStringList parts = position.split(' ');
  if (parts.size() == 2) {
    QString value = parts.at(0).trimmed();
    QChar direction = value.at(value.size()-1);
    if (direction == 'E' || direction == 'W') {
      // OpenTX 2.1 format: "NNN.MMM[E|W] NNN.MMM[N|S]" <longitude> <latitude>
      result.latitude = toDecimalCoordinate(parts.at(1).trimmed());
      result.longitude = toDecimalCoordinate(parts.at(0).trimmed());
    }
    else {
      // OpenTX 2.2 format: "DD.DDDDDD DD.DDDDDD" <latitude> <longitude> both in Signed degrees format (DDD.dddd)
      // Precede South latitudes and West longitudes with a minus sign.
      // Latitudes range from -90 to 90.
      // Longitudes range from -180 to 180.
      result.latitude = parts.at(0).trimmed().toDouble();
      result.longitude = parts.at(1).trimmed().toDouble();
    }
  }
  return result;
}

TableLayout::TableLayout(QWidget * parent, int rowCount, const QStringList & headerLabels)
{
#if defined(TABLE_LAYOUT)
  tableWidget = new QTableWidget(parent);
  QVBoxLayout * layout = new QVBoxLayout();
  layout->addWidget(tableWidget);
  layout->setContentsMargins(0, 0, 0, 0);
  parent->setLayout(layout);

  tableWidget->setRowCount(rowCount);
  tableWidget->setColumnCount(headerLabels.size());
  tableWidget->setShowGrid(false);
  tableWidget->verticalHeader()->setVisible(false);
  tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
  tableWidget->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
  tableWidget->setStyleSheet("QTableWidget {background-color: transparent;}");
  tableWidget->setHorizontalHeaderLabels(headerLabels);
#else
  gridWidget = new QGridLayout(parent);

  int col = 0;
  foreach(QString text, headerLabels) {
    QLabel *label = new QLabel();
    label->setFrameShape(QFrame::Panel);
    label->setFrameShadow(QFrame::Raised);
    label->setMidLineWidth(0);
    label->setAlignment(Qt::AlignCenter);
    label->setMargin(5);
    label->setText(text);
    // if (!minimize)
    //   label->setMinimumWidth(100);
    gridWidget->addWidget(label, 0, col++);
  }
#endif
}

void TableLayout::addWidget(int row, int column, QWidget * widget)
{
#if defined(TABLE_LAYOUT)
  QHBoxLayout * layout = new QHBoxLayout(tableWidget);
  layout->addWidget(widget);
  addLayout(row, column, layout);
#else
  gridWidget->addWidget(widget, row + 1, column);
#endif
}

void TableLayout::addLayout(int row, int column, QLayout * layout)
{
#if defined(TABLE_LAYOUT)
  layout->setContentsMargins(1, 3, 1, 3);
  QWidget * containerWidget = new QWidget(tableWidget);
  containerWidget->setLayout(layout);
  tableWidget->setCellWidget(row, column, containerWidget);
#else
  gridWidget->addLayout(layout, row + 1, column);
#endif
}

void TableLayout::resizeColumnsToContents()
{
#if defined(TABLE_LAYOUT)
  tableWidget->resizeColumnsToContents();
#else
#endif
}

void TableLayout::setColumnWidth(int col, int width)
{
#if defined(TABLE_LAYOUT)
  tableWidget->setColumnWidth(col, width);
#else
#endif
}

void TableLayout::pushRowsUp(int row)
{
#if defined(TABLE_LAYOUT)
#else
  // Push the rows up
  QSpacerItem * spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
  gridWidget->addItem(spacer, row, 0);
#endif
  // Push rows upward
  // addDoubleSpring(gridLayout, 5, num_fsw+1);

}
