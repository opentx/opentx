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
#if defined(WIN32) && defined(WIN_USE_CONSOLE_STDIO)
  #include "windows.h"
#endif

#include "appdata.h"
#include "helpers.h"
#include "modeledit/modeledit.h"
#include "simulatordialog.h"
#include "storage/sdcard.h"

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

void populateGvSourceCB(QComboBox *b, int value)
{
  QString strings[] = { QObject::tr("---"), QObject::tr("Rud Trim"), QObject::tr("Ele Trim"), QObject::tr("Thr Trim"), QObject::tr("Ail Trim"), QObject::tr("Rot Enc"), QObject::tr("Rud"), QObject::tr("Ele"), QObject::tr("Thr"), QObject::tr("Ail"), QObject::tr("P1"), QObject::tr("P2"), QObject::tr("P3")};
  b->clear();
  for (int i=0; i<= 12; i++) {
    b->addItem(strings[i]);
  }
  b->setCurrentIndex(value);
}

void populateFileComboBox(QComboBox * b, const QSet<QString> & set, const QString & current)
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

void getFileComboBoxValue(QComboBox * b, char * dest, int length)
{
  memset(dest, 0, length+1);
  if (b->currentText() != "----") {
    strncpy(dest, b->currentText().toLatin1(), length);
  }
}

void populatePhasesCB(QComboBox *b, int value)
{
  for (int i=-GetCurrentFirmware()->getCapability(FlightModes); i<=GetCurrentFirmware()->getCapability(FlightModes); i++) {
    if (i < 0)
      b->addItem(QObject::tr("!Flight mode %1").arg(-i-1), i);
    else if (i > 0)
      b->addItem(QObject::tr("Flight mode %1").arg(i-1), i);
    else
      b->addItem(QObject::tr("----"), 0);
  }
  b->setCurrentIndex(value + GetCurrentFirmware()->getCapability(FlightModes));
}

GVarGroup::GVarGroup(QCheckBox * weightGV, QAbstractSpinBox * weightSB, QComboBox * weightCB, int & weight, const ModelData & model, const int deflt, const int mini, const int maxi, const double step, bool allowGvars, ModelPanel * panel):
  QObject(),
  weightGV(weightGV),
  weightSB(weightSB),
  sb(dynamic_cast<QSpinBox *>(weightSB)),
  dsb(dynamic_cast<QDoubleSpinBox *>(weightSB)),
  weightCB(weightCB),
  weight(weight),
  step(step),
  lock(true),
  panel(panel)
{
  if (allowGvars && GetCurrentFirmware()->getCapability(Gvars)) {
    populateGVCB(*weightCB, weight, model);
    connect(weightGV, SIGNAL(stateChanged(int)), this, SLOT(gvarCBChanged(int)));
    connect(weightCB, SIGNAL(currentIndexChanged(int)), this, SLOT(valuesChanged()));
  }
  else {
    weightGV->hide();
    if (weight > maxi || weight < mini) {
      weight = deflt;
    }
  }

  int val;

  if (weight>maxi || weight<mini) {
    val = deflt;
    weightGV->setChecked(true);
    weightSB->hide();
    weightCB->show();
  }
  else {
    val = weight;
    weightGV->setChecked(false);
    weightSB->show();
    weightCB->hide();
  }

  if (sb) {
    sb->setMinimum(mini);
    sb->setMaximum(maxi);
    sb->setValue(val);
  }
  else {
    dsb->setMinimum(mini*step);
    dsb->setMaximum(maxi*step);
    dsb->setValue(val*step);
  }

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
    if (panel)
      emit panel->modified();
    
  }
}

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
    curveGVarCB->setVisible(GetCurrentFirmware()->getCapability(Gvars));
    if (curve.value > 100 || curve.value < -100) {
      curveGVarCB->setChecked(true);
      if (lastType != CurveReference::CURVE_REF_DIFF && lastType != CurveReference::CURVE_REF_EXPO) {
        lastType = curve.type;
        populateGVCB(*curveValueCB, curve.value, model);
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
            curveValueCB->addItem(CurveReference(CurveReference::CURVE_REF_FUNC, i).toString());
          }
        }
        curveValueCB->setCurrentIndex(curve.value);
        break;
      case CurveReference::CURVE_REF_CUSTOM:
      {
        int numcurves = GetCurrentFirmware()->getCapability(NumCurves);
        if (lastType != curve.type) {
          lastType = curve.type;
          curveValueCB->clear();
          for (int i= ((flags & HIDE_NEGATIVE_CURVES) ? 0 : -numcurves); i<=numcurves; i++) {
            curveValueCB->addItem(CurveReference(CurveReference::CURVE_REF_CUSTOM, i).toString(), i);
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

void populateGvarUseCB(QComboBox *b, unsigned int phase)
{
  b->addItem(QObject::tr("Own value"));
  for (int i=0; i<GetCurrentFirmware()->getCapability(FlightModes); i++) {
    if (i != (int)phase) {
      b->addItem(QObject::tr("Flight mode %1 value").arg(i));
    }
  }
}

void populateSwitchCB(QComboBox *b, const RawSwitch & value, const GeneralSettings & generalSettings, SwitchContext context)
{
  BoardEnum board = GetCurrentFirmware()->getBoard();
  RawSwitch item;

  b->clear();

  if (context != MixesContext && context != GlobalFunctionsContext) {
    // !FMx
    if (IS_ARM(board)) {
      for (int i=-GetCurrentFirmware()->getCapability(FlightModes); i<0; i++) {
        item = RawSwitch(SWITCH_TYPE_FLIGHT_MODE, i);
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  if (context != GlobalFunctionsContext) {
    for (int i=-GetCurrentFirmware()->getCapability(LogicalSwitches); i<0; i++) {
      item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  for (int i=-GetCurrentFirmware()->getCapability(RotaryEncoders); i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_ROTARY_ENCODER, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=-8; i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_TRIM, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=GetCurrentFirmware()->getCapability(MultiposPots)-1; i>=0; i--) {
    if (generalSettings.potConfig[i] == GeneralSettings::POT_MULTIPOS_SWITCH) {
      for (int j=-GetCurrentFirmware()->getCapability(MultiposPotsPositions); j<0; j++) {
        item = RawSwitch(SWITCH_TYPE_MULTIPOS_POT, -i*GetCurrentFirmware()->getCapability(MultiposPotsPositions)+j);
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  for (int i=-GetCurrentFirmware()->getCapability(SwitchesPositions); i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_SWITCH, i);
    if (IS_HORUS_OR_TARANIS(board) && !generalSettings.switchPositionAllowedTaranis(i)) {
      continue;
    }
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  if (context == TimersContext) {
    for (int i=0; i<5; i++) {
      item = RawSwitch(SWITCH_TYPE_TIMER_MODE, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }
  else {
    item = RawSwitch(SWITCH_TYPE_NONE);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=1; i<=GetCurrentFirmware()->getCapability(SwitchesPositions); i++) {
    item = RawSwitch(SWITCH_TYPE_SWITCH, i);
    if (IS_HORUS_OR_TARANIS(board) && !generalSettings.switchPositionAllowedTaranis(i)) {
      continue;
    }
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=0; i<GetCurrentFirmware()->getCapability(MultiposPots); i++) {
    if (generalSettings.potConfig[i] == GeneralSettings::POT_MULTIPOS_SWITCH) {
      for (int j=1; j<=GetCurrentFirmware()->getCapability(MultiposPotsPositions); j++) {
        item = RawSwitch(SWITCH_TYPE_MULTIPOS_POT, i*GetCurrentFirmware()->getCapability(MultiposPotsPositions)+j);
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  for (int i=1; i<=8; i++) {
    item = RawSwitch(SWITCH_TYPE_TRIM, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=1; i<=GetCurrentFirmware()->getCapability(RotaryEncoders); i++) {
    item = RawSwitch(SWITCH_TYPE_ROTARY_ENCODER, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  if (context != GlobalFunctionsContext) {
    for (int i=1; i<=GetCurrentFirmware()->getCapability(LogicalSwitches); i++) {
      item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  if (context == SpecialFunctionsContext || context == GlobalFunctionsContext) {
    // ON
    item = RawSwitch(SWITCH_TYPE_ON);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
    // One
    item = RawSwitch(SWITCH_TYPE_ONE, 1);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  // FMx
  if (context != MixesContext && context != GlobalFunctionsContext) {
    if (IS_ARM(board)) {
      for (int i=1; i<=GetCurrentFirmware()->getCapability(FlightModes); i++) {
        item = RawSwitch(SWITCH_TYPE_FLIGHT_MODE, i);
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  b->setMaxVisibleItems(10);
}

void populateGVCB(QComboBox & b, int value, const ModelData & model)
{
  bool selected = false;

  b.clear();

  int count = GetCurrentFirmware()->getCapability(Gvars);
  for (int i=-count; i<=-1; i++) {
    int16_t gval = (int16_t)(-10000+i);
    if (strlen(model.gvars_names[-i-1]) > 0)
      b.addItem(QObject::tr("-GV%1 (%2)").arg(-i).arg(model.gvars_names[-i-1]), gval);
    else
      b.addItem(QObject::tr("-GV%1").arg(-i), gval);
    if (value == gval) {
      b.setCurrentIndex(b.count()-1);
      selected = true;
    }
  }

  for (int i=1; i<=count; i++) {
    int16_t gval = (int16_t)(10000+i);
    if (strlen(model.gvars_names[i-1]) > 0)
      b.addItem(QObject::tr("GV%1 (%2)").arg(i).arg(model.gvars_names[i-1]), gval);
    else
      b.addItem(QObject::tr("GV%1").arg(i), gval);
    if (value == gval) {
      b.setCurrentIndex(b.count()-1);
      selected = true;
    }
  }

  if (!selected) {
    b.setCurrentIndex(count);
  }
}

void populateSourceCB(QComboBox *b, const RawSource & source, const GeneralSettings generalSettings, const ModelData * model, unsigned int flags)
{
  BoardEnum board = GetCurrentFirmware()->getBoard();
  RawSource item;

  b->clear();

  if (flags & POPULATE_NONE) {
    item = RawSource(SOURCE_TYPE_NONE);
    b->addItem(item.toString(model), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }

  if (flags & POPULATE_SCRIPT_OUTPUTS) {
    for (int i=0; i<GetCurrentFirmware()->getCapability(LuaScripts); i++) {
      for (int j=0; j<GetCurrentFirmware()->getCapability(LuaOutputsPerScript); j++) {
        item = RawSource(SOURCE_TYPE_LUA_OUTPUT, i*16+j);
        b->addItem(item.toString(model), item.toValue());
        if (item == source) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  if (model && (flags & POPULATE_VIRTUAL_INPUTS)) {
    int virtualInputs = GetCurrentFirmware()->getCapability(VirtualInputs);
    for (int i=0; i<virtualInputs; i++) {
      if (model->isInputValid(i)) {
        item = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, i);
        b->addItem(item.toString(model), item.toValue());
        if (item == source) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  if (flags & POPULATE_SOURCES) {
    for (int i=0; i<CPN_MAX_STICKS+GetCurrentFirmware()->getCapability(Pots)+GetCurrentFirmware()->getCapability(Sliders); i++) {
      item = RawSource(SOURCE_TYPE_STICK, i);
      // skip unavailable pots and sliders
      if (item.isPot() && !generalSettings.isPotAvailable(i-CPN_MAX_STICKS)) continue;
      if (item.isSlider() && !generalSettings.isSliderAvailable(i-CPN_MAX_STICKS-GetCurrentFirmware()->getCapability(Pots))) continue;
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
    for (int i=0; i<GetCurrentFirmware()->getCapability(RotaryEncoders); i++) {
      item = RawSource(SOURCE_TYPE_ROTARY_ENCODER, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_TRIMS) {
    for (int i=0; i<4; i++) {
      item = RawSource(SOURCE_TYPE_TRIM, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_SOURCES) {
    item = RawSource(SOURCE_TYPE_MAX);
    b->addItem(item.toString(model), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }

  if (flags & POPULATE_SWITCHES) {
    for (int i=0; i<GetCurrentFirmware()->getCapability(Switches); i++) {
      item = RawSource(SOURCE_TYPE_SWITCH, i);
      b->addItem(item.toString(model), item.toValue());
      if (IS_HORUS_OR_TARANIS(board) && !generalSettings.switchSourceAllowedTaranis(i)) {
        QModelIndex index = b->model()->index(b->count()-1, 0);
        QVariant v(0);
        b->model()->setData(index, v, Qt::UserRole - 1);
      }
      if (item == source) b->setCurrentIndex(b->count()-1);
    }

    for (int i=0; i<GetCurrentFirmware()->getCapability(LogicalSwitches); i++) {
      item = RawSource(SOURCE_TYPE_CUSTOM_SWITCH, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_SOURCES) {
    for (int i=0; i<CPN_MAX_CYC; i++) {
      item = RawSource(SOURCE_TYPE_CYC, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }

    for (int i=0; i<GetCurrentFirmware()->getCapability(TrainerInputs); i++) {
      item = RawSource(SOURCE_TYPE_PPM, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }

    for (int i=0; i<GetCurrentFirmware()->getCapability(Outputs); i++) {
      item = RawSource(SOURCE_TYPE_CH, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_TELEMETRY) {
    if (IS_ARM(GetCurrentFirmware()->getBoard())) {
      for (int i=0; i<5; ++i) {
        item = RawSource(SOURCE_TYPE_SPECIAL, i);
        b->addItem(item.toString(model), item.toValue());
        if (item == source) b->setCurrentIndex(b->count()-1);
      }
      for (int i=0; i<CPN_MAX_SENSORS; ++i) {
        if (model && model->sensorData[i].isAvailable()) {    //this conditon must be false if we populate Global Functions where model = 0
          for (int j=0; j<3; ++j) {
            item = RawSource(SOURCE_TYPE_TELEMETRY, 3*i+j);
            b->addItem(item.toString(model), item.toValue());
            // qDebug() << item.toString(model) << source.toString(model);
            if (item == source) b->setCurrentIndex(b->count()-1);
          }
        }
      }
    }
    else {
      for (int i=0; i<(flags & POPULATE_TELEMETRYEXT ? TELEMETRY_SOURCES_STATUS_COUNT : TELEMETRY_SOURCES_COUNT); i++) {
        if (i==TELEMETRY_SOURCE_TX_TIME && !GetCurrentFirmware()->getCapability(RtcTime))
          continue;
        if (i==TELEMETRY_SOURCE_SWR && !GetCurrentFirmware()->getCapability(SportTelemetry))
          continue;
        if (i==TELEMETRY_SOURCE_TIMER3 && !IS_ARM(board))
          continue;
        item = RawSource(SOURCE_TYPE_TELEMETRY, i);
        b->addItem(item.toString(model), item.toValue());
        if (item == source) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  if (flags & POPULATE_GVARS) {
    for (int i=0; i<GetCurrentFirmware()->getCapability(Gvars); i++) {
      item = RawSource(SOURCE_TYPE_GVAR, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  b->setMaxVisibleItems(10);
}

QString image2qstring(QImage image)
{
    if (image.isNull())
      return "";
    QBuffer buffer;
    image.save(&buffer, "PNG");
    QString ImageStr;
    int b=0;
    int size=buffer.data().size();
    for (int j = 0; j < size; j++) {
      b=buffer.data().at(j);
      ImageStr += QString("%1").arg(b&0xff, 2, 16, QChar('0'));
    }
    return ImageStr;
}

int findmult(float value, float base)
{
  int vvalue = value*10;
  int vbase = base*10;
  vvalue--;

  int mult = 0;
  for (int i=8; i>=0; i--) {
    if (vvalue/vbase >= (1<<i)) {
      mult = i+1;
      break;
    }
  }

  return mult;
}

QString getFrSkyAlarmType(int alarm)
{
  switch (alarm) {
    case 1:
      return QObject::tr("Yellow");
    case 2:
      return QObject::tr("Orange");
    case 3:
      return QObject::tr("Red");
    default:
      return "----";
  }
}

QString getFrSkyUnits(int units)
{
  switch(units) {
    case 1:
      return QObject::tr("---");
    default:
      return "V";
  }
}

QString getFrSkyProtocol(int protocol)
{
  switch(protocol) {
    case 2:
      if ((GetCurrentFirmware()->getCapability(Telemetry) & TM_HASWSHH))
        return QObject::tr("Winged Shadow How High");
      else
        return QObject::tr("Winged Shadow How High (not supported)");
    case 1:
      return QObject::tr("FrSky Sensor Hub");
    default:
      return QObject::tr("None");
  }
}

QString getFrSkyMeasure(int units)
{
  switch(units) {
    case 1:
      return QObject::tr("Imperial");
    default:
      return QObject::tr("Metric");
  }
}

QString getFrSkySrc(int index)
{
  return RawSource(SOURCE_TYPE_TELEMETRY, index-1).toString();
}

QString getTheme()
{
  int theme_set = g.theme();
  QString Theme;
  switch(theme_set) {
    case 0:
      Theme="classic";
      break;
    case 2:
      Theme="monowhite";
      break;
    case 3:
      Theme="monochrome";
      break;
    case 4:
      Theme="monoblue";
      break;
    default:
      Theme="yerico";
      break;
  }
  return Theme;
}

CompanionIcon::CompanionIcon(const QString &baseimage)
{
  static QString theme = getTheme();
  addFile(":/themes/"+theme+"/16/"+baseimage, QSize(16,16));
  addFile(":/themes/"+theme+"/24/"+baseimage, QSize(24,24));
  addFile(":/themes/"+theme+"/32/"+baseimage, QSize(32,32));
  addFile(":/themes/"+theme+"/48/"+baseimage, QSize(48,48));
}

void startSimulation(QWidget * parent, RadioData & radioData, int modelIdx)
{
  Firmware * firmware = GetCurrentFirmware();
  SimulatorInterface * simulator = GetCurrentFirmwareSimulator();
  if (simulator) {
#if defined(WIN32) && defined(WIN_USE_CONSOLE_STDIO)
    AllocConsole();
    SetConsoleTitle("Companion Console");
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
#endif
    RadioData * simuData = new RadioData(radioData);
    unsigned int flags = 0;
    if (modelIdx >= 0) {
      flags |= SIMULATOR_FLAGS_NOTX;
      simuData->setCurrentModel(modelIdx);
    }
    if (radioData.generalSettings.stickMode & 1) {
      flags |= SIMULATOR_FLAGS_STICK_MODE_LEFT;
    }
    BoardEnum board = GetCurrentFirmware()->getBoard();
    SimulatorDialog * dialog;

    if (board == BOARD_HORUS && HORUS_READY_FOR_RELEASE()) {
      dialog = new SimulatorDialogHorus(parent, simulator, flags);
      SdcardFormat sdcard(g.profile[g.id()].sdPath());
      sdcard.write(*simuData);
      dialog->start(NULL);
    }
    else if (board == BOARD_FLAMENCO) {
      dialog = new SimulatorDialogFlamenco(parent, simulator, flags);
      QByteArray eeprom(getEEpromSize(board), 0);
      firmware->saveEEPROM((uint8_t *)eeprom.data(), *simuData);
      dialog->start(eeprom);
    }
    else if (board == BOARD_TARANIS_X9D || board == BOARD_TARANIS_X9DP || board == BOARD_TARANIS_X9E) {
      for (int i=0; i<GetCurrentFirmware()->getCapability(Pots); i++) {
        if (radioData.generalSettings.isPotAvailable(i)) {
          flags |= (SIMULATOR_FLAGS_S1 << i);
          if (radioData.generalSettings.potConfig[1] == GeneralSettings::POT_MULTIPOS_SWITCH ) {
            flags |= (SIMULATOR_FLAGS_S1_MULTI << i);
          }
        }
      }
      dialog = new SimulatorDialogTaranis(parent, simulator, flags);
      QByteArray eeprom(getEEpromSize(board), 0);
      firmware->saveEEPROM((uint8_t *)eeprom.data(), *simuData);
      dialog->start(eeprom);
    }
    else {
      dialog = new SimulatorDialog9X(parent, simulator, flags);
      QByteArray eeprom(getEEpromSize(board), 0);
      firmware->saveEEPROM((uint8_t *)eeprom.data(), *simuData, 0, firmware->getCapability(SimulatorVariant));
      dialog->start(eeprom);
    }

    dialog->exec();
    dialog->deleteLater();
    delete simuData;
#if defined(WIN32) && defined(WIN_USE_CONSOLE_STDIO)
    FreeConsole();
#endif
  }
  else {
    QMessageBox::warning(NULL,
      QObject::tr("Warning"),
      QObject::tr("Simulator for this firmware is not yet available"));
  }
}

QPixmap makePixMap(const QImage & image)
{
  Firmware * firmware = GetCurrentFirmware();
  QImage result = image.scaled(firmware->getCapability(LcdWidth), firmware->getCapability(LcdHeight));
  if (firmware->getCapability(LcdDepth) == 4) {
    result = result.convertToFormat(QImage::Format_RGB32);
    for (int i = 0; i < image.width(); ++i) {
      for (int j = 0; j < image.height(); ++j) {
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
  QStringList parts = version.split("N");
  if (parts.size() > 1)
    result = parts[1].toInt(); // nightly build
  parts = parts[0].split('.');
  if (parts.size() > 2)
    result += 1000 * parts[2].toInt();
  if (parts.size() > 1)
    result += 100000 * parts[1].toInt();
  if (parts.size() > 0)
    result += 10000000 * parts[0].toInt();
  return result;
}

QString index2version(int index)
{
  if (index >= 19900000) {
    int nightly = index % 1000;
    index /= 1000;
    int revision = index % 100;
    index /= 100;
    int minor = index % 100;
    int major = index / 100;
    QString result = QString("%1.%2.%3").arg(major).arg(minor).arg(revision);
    if (nightly > 0 && nightly < 999) {
      result += QString("N%1").arg(nightly);
    }
    return result;
  }
  else if (index >= 19900) {
    int revision = index % 100;
    index /= 100;
    int minor = index % 100;
    int major = index / 100;
    return QString("%1.%2.%3").arg(major).arg(minor).arg(revision);
  }
  else {
    return QString();
  }
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

bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
  return s1.toLower() < s2.toLower();
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
