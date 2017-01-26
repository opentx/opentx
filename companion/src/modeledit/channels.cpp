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

#include "channels.h"
#include "helpers.h"

LimitsGroup::LimitsGroup(Firmware * firmware, TableLayout * tableLayout, int row, int col, int & value, const ModelData & model, int min, int max, int deflt, ModelPanel * panel):
  firmware(firmware),
  spinbox(new QDoubleSpinBox()),
  value(value),
  displayStep(0.1)
{
  Board::Type board = firmware->getBoard();
  bool allowGVars = IS_HORUS_OR_TARANIS(board);
  int internalStep = 1;

  spinbox->setProperty("index", row);
  spinbox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  spinbox->setAccelerated(true);

  if (firmware->getCapability(PPMUnitMicroseconds)) {
    displayStep = 0.512;
    spinbox->setDecimals(1);
    spinbox->setSuffix("us");
  }
  else {
    spinbox->setDecimals(0);
    spinbox->setSuffix("%");
  }

  if (IS_ARM(board) || deflt == 0 /*it's the offset*/) {
    spinbox->setDecimals(1);
  }
  else {
    internalStep *= 10;
  }

  spinbox->setSingleStep(displayStep*internalStep);
  spinbox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

  QHBoxLayout * horizontalLayout = new QHBoxLayout();
  QCheckBox * gv = new QCheckBox(QObject::tr("GV"));
  gv->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  horizontalLayout->addWidget(gv);
  QComboBox * cb = new QComboBox();
  cb->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  horizontalLayout->addWidget(cb);
  horizontalLayout->addWidget(spinbox);
  tableLayout->addLayout(row, col, horizontalLayout);
  gvarGroup = new GVarGroup(gv, spinbox, cb, value, model, deflt, min, max, displayStep, allowGVars, panel);
}

LimitsGroup::~LimitsGroup()
{
  delete gvarGroup;
}

void LimitsGroup::updateMinMax(int max)
{
  if (spinbox->maximum() == 0) {
    spinbox->setMinimum(-max*displayStep);
    if (value < -max) {
      value = -max;
    }
  }
  if (spinbox->minimum() == 0) {
    spinbox->setMaximum(max*displayStep);
    if (value > max) {
      value = max;
    }
  }
}

Channels::Channels(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware):
  ModelPanel(parent, model, generalSettings, firmware)
{
  Stopwatch s1("Channels");

  int channelNameMaxLen = firmware->getCapability(ChannelsName);

  QStringList headerLabels;
  headerLabels << "#";
  if (channelNameMaxLen > 0) {
    headerLabels << tr("Name");
  }
  headerLabels << tr("Subtrim") << tr("Min") << tr("Max") << tr("Direction");
  if (IS_HORUS_OR_TARANIS(firmware->getBoard()))
    headerLabels << tr("Curve");
  if (firmware->getCapability(PPMCenter))
    headerLabels << tr("PPM Center");
  if (firmware->getCapability(SYMLimits))
    headerLabels << tr("Linear Subtrim");
  TableLayout * tableLayout = new TableLayout(this, firmware->getCapability(LogicalSwitches), headerLabels);

  s1.report("header");

  for (int i=0; i<firmware->getCapability(Outputs); i++) {
    int col = 0;

    // Channel label
    QLabel *label = new QLabel(this);
    label->setText(tr("CH%1").arg(i+1));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    tableLayout->addWidget(i, col++, label);

    // Channel name
    if (channelNameMaxLen > 0) {
      QLineEdit * name = new QLineEdit(this);
      name->setProperty("index", i);
      name->setMaxLength(channelNameMaxLen);
      QRegExp rx(CHAR_FOR_NAMES_REGEX);
      name->setValidator(new QRegExpValidator(rx, this));
      name->setText(model.limitData[i].name);
      connect(name, SIGNAL(editingFinished()), this, SLOT(nameEdited()));
      tableLayout->addWidget(i, col++, name);
    }

    // Channel offset
    limitsGroups << new LimitsGroup(firmware, tableLayout, i, col++, model.limitData[i].offset, model, -1000, 1000, 0, this);

    // Channel min
    limitsGroups << new LimitsGroup(firmware, tableLayout, i, col++, model.limitData[i].min, model, -model.getChannelsMax()*10, 0, -1000, this);

    // Channel max
    limitsGroups << new LimitsGroup(firmware, tableLayout, i, col++, model.limitData[i].max, model, 0, model.getChannelsMax()*10, 1000, this);

    // Channel inversion
    QComboBox * invCB = new QComboBox(this);
    invCB->insertItems(0, QStringList() << tr("---") << tr("INV"));
    invCB->setProperty("index", i);
    invCB->setCurrentIndex((model.limitData[i].revert) ? 1 : 0);
    connect(invCB, SIGNAL(currentIndexChanged(int)), this, SLOT(invEdited()));
    tableLayout->addWidget(i, col++, invCB);

    // Curve
    if (IS_HORUS_OR_TARANIS(firmware->getBoard())) {
      QComboBox * curveCB = new QComboBox(this);
      curveCB->setProperty("index", i);
      int numcurves = firmware->getCapability(NumCurves);
      for (int j=-numcurves; j<=numcurves; j++) {
        curveCB->addItem(CurveReference(CurveReference::CURVE_REF_CUSTOM, j).toString(), j);
      }
      curveCB->setCurrentIndex(model.limitData[i].curve.value+numcurves);
      connect(curveCB, SIGNAL(currentIndexChanged(int)), this, SLOT(curveEdited()));
      tableLayout->addWidget(i, col++, curveCB);
    }

    // PPM center
    int ppmCenterMax = firmware->getCapability(PPMCenter);
    if (ppmCenterMax) {
      QSpinBox * center = new QSpinBox(this);
      center->setProperty("index", i);
      center->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
      center->setSuffix("us");
      center->setMinimum(1500-ppmCenterMax);
      center->setMaximum(1500+ppmCenterMax);
      center->setValue(1500);
      center->setValue(model.limitData[i].ppmCenter + 1500);
      connect(center, SIGNAL(editingFinished()), this, SLOT(ppmcenterEdited()));
      tableLayout->addWidget(i, col++, center);
    }

    // Symetrical limits
    if (firmware->getCapability(SYMLimits)) {
      QCheckBox * symlimits = new QCheckBox(this);
      symlimits->setProperty("index", i);
      symlimits->setChecked(model.limitData[i].symetrical);
      connect(symlimits, SIGNAL(toggled(bool)), this, SLOT(symlimitsEdited()));
      tableLayout->addWidget(i, col++, symlimits);
    }
  }
  s1.report("add elements");

  disableMouseScrolling();
  tableLayout->resizeColumnsToContents();
  tableLayout->pushRowsUp(firmware->getCapability(Outputs)+1);
  s1.report("end");
}

Channels::~Channels()
{
  foreach(LimitsGroup *group, limitsGroups) {
    delete group;
  }
}

void Channels::symlimitsEdited()
{
  QCheckBox * ckb = qobject_cast<QCheckBox*>(sender());
  int index = ckb->property("index").toInt();
  model->limitData[index].symetrical = (ckb->checkState() ? 1 : 0);
  emit modified();
}

void Channels::nameEdited()
{
  if (!lock) {
    QLineEdit *le = qobject_cast<QLineEdit*>(sender());
    int index = le->property("index").toInt();
    strcpy(model->limitData[index].name, le->text().toLatin1());
    emit modified();
  }
}

void Channels::refreshExtendedLimits()
{
  int channelMax = model->getChannelsMax();

  foreach(LimitsGroup *group, limitsGroups) {
    group->updateMinMax(10*channelMax);
  }

  emit modified(); 
}

void Channels::invEdited()
{
  QComboBox *cb = qobject_cast<QComboBox*>(sender());
  int index = cb->property("index").toInt();
  model->limitData[index].revert = cb->currentIndex();
  emit modified();
}

void Channels::curveEdited()
{
  QComboBox *cb = qobject_cast<QComboBox*>(sender());
  int index = cb->property("index").toInt();
  model->limitData[index].curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, cb->itemData(cb->currentIndex()).toInt());
  emit modified();
}

void Channels::ppmcenterEdited()
{
  QSpinBox *sb = qobject_cast<QSpinBox*>(sender());
  int index = sb->property("index").toInt();
  model->limitData[index].ppmCenter = sb->value() - 1500;
  emit modified();
}

