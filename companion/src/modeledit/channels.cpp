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

  spinbox->setSingleStep(displayStep * internalStep);
  spinbox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

  QHBoxLayout *horizontalLayout = new QHBoxLayout();
  QCheckBox *gv = new QCheckBox(tr("GV"));
  gv->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  horizontalLayout->addWidget(gv);
  QComboBox *cb = new QComboBox();
  cb->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  horizontalLayout->addWidget(cb);
  horizontalLayout->addWidget(spinbox);
  tableLayout->addLayout(row, col, horizontalLayout);
  gvarGroup = new GVarGroup(gv, spinbox, cb, value, model, deflt, min, max, displayStep, allowGVars);
  QObject::connect(gvarGroup, &GVarGroup::valueChanged, panel, &ModelPanel::modified);
}

LimitsGroup::~LimitsGroup()
{
  delete gvarGroup;
}

void LimitsGroup::setValue(int val)
{
  gvarGroup->setWeight(val);
}

void LimitsGroup::updateMinMax(int max)
{
  if (spinbox->maximum() == 0) {
    spinbox->setMinimum(-max * displayStep);
    if (value < -max) {
      value = -max;
    }
  }
  if (spinbox->minimum() == 0) {
    spinbox->setMaximum(max * displayStep);
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
  TableLayout *tableLayout = new TableLayout(this, firmware->getCapability(LogicalSwitches), headerLabels);

  s1.report("header");

  for (int i=0; i<firmware->getCapability(Outputs); i++) {
    int col = 0;

    // Channel label
    QLabel *label = new QLabel(this);
    label->setText(tr("CH%1").arg(i+1));
    label->setProperty("index", i);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    label->setContextMenuPolicy(Qt::CustomContextMenu);
    label->setToolTip(tr("Popup menu available"));
    label->setMouseTracking(true);
    connect(label, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(chn_customContextMenuRequested(QPoint)));
    tableLayout->addWidget(i, col++, label);

    // Channel name
    if (channelNameMaxLen > 0) {
      name[i] = new QLineEdit(this);
      name[i]->setProperty("index", i);
      name[i]->setMaxLength(channelNameMaxLen);
      QRegExp rx(CHAR_FOR_NAMES_REGEX);
      name[i]->setValidator(new QRegExpValidator(rx, this));
      connect(name[i], SIGNAL(editingFinished()), this, SLOT(nameEdited()));
      tableLayout->addWidget(i, col++, name[i]);
    }

    // Channel offset
    chnOffset[i] = new LimitsGroup(firmware, tableLayout, i, col++, model.limitData[i].offset, model, -1000, 1000, 0, this);

    // Channel min
    chnMin[i] = new LimitsGroup(firmware, tableLayout, i, col++, model.limitData[i].min, model, -model.getChannelsMax() * 10, 0, -1000, this);

    // Channel max
    chnMax[i] = new LimitsGroup(firmware, tableLayout, i, col++, model.limitData[i].max, model, 0, model.getChannelsMax() * 10, 1000, this);

    // Channel inversion
    invCB[i] = new QComboBox(this);
    invCB[i]->insertItems(0, QStringList() << tr("---") << tr("INV"));
    invCB[i]->setProperty("index", i);
    connect(invCB[i], SIGNAL(currentIndexChanged(int)), this, SLOT(invEdited()));
    tableLayout->addWidget(i, col++, invCB[i]);

    // Curve
    if (IS_HORUS_OR_TARANIS(firmware->getBoard())) {
      curveCB[i] = new QComboBox(this);
      curveCB[i]->setProperty("index", i);
      int numcurves = firmware->getCapability(NumCurves);
      for (int j=-numcurves; j<=numcurves; j++) {
        curveCB[i]->addItem(CurveReference(CurveReference::CURVE_REF_CUSTOM, j).toString(&model, false), j);
      }
      curveCB[i]->setCurrentIndex(model.limitData[i].curve.value+numcurves);
      connect(curveCB[i], SIGNAL(currentIndexChanged(int)), this, SLOT(curveEdited()));
      tableLayout->addWidget(i, col++, curveCB[i]);
    }

    // PPM center
    int ppmCenterMax = firmware->getCapability(PPMCenter);
    if (ppmCenterMax) {
      centerSB[i] = new QSpinBox(this);
      centerSB[i]->setProperty("index", i);
      centerSB[i]->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
      centerSB[i]->setSuffix("us");
      centerSB[i]->setMinimum(1500 - ppmCenterMax);
      centerSB[i]->setMaximum(1500 + ppmCenterMax);
      centerSB[i]->setValue(1500);
      connect(centerSB[i], SIGNAL(editingFinished()), this, SLOT(ppmcenterEdited()));
      tableLayout->addWidget(i, col++, centerSB[i]);
    }

    // Symetrical limits
    if (firmware->getCapability(SYMLimits)) {
      symlimitsChk[i] = new QCheckBox(this);
      symlimitsChk[i]->setProperty("index", i);
      symlimitsChk[i]->setChecked(model.limitData[i].symetrical);
      connect(symlimitsChk[i], SIGNAL(toggled(bool)), this, SLOT(symlimitsEdited()));
      tableLayout->addWidget(i, col++, symlimitsChk[i]);
    }
  }
  update();
  s1.report("add elements");

  disableMouseScrolling();
  tableLayout->resizeColumnsToContents();
  tableLayout->pushRowsUp(firmware->getCapability(Outputs)+1);
  s1.report("end");
}

Channels::~Channels()
{
  // compiler warning if delete[]
  for (int i=0; i<CPN_MAX_CHNOUT;i++) {
    delete name[i];
    delete chnOffset[i];
    delete chnMin[i];
    delete chnMax[i];
    delete invCB[i];
    delete curveCB[i];
    delete centerSB[i];
    delete symlimitsChk[i];
  }
}

void Channels::symlimitsEdited()
{
  if (!lock) {
    QCheckBox *ckb = qobject_cast<QCheckBox*>(sender());
    int index = ckb->property("index").toInt();
    model->limitData[index].symetrical = (ckb->checkState() ? 1 : 0);
    emit modified();
  }
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

  for (int i=0 ; i<CPN_MAX_CHNOUT; i++) {
    chnOffset[i]->updateMinMax(10 * channelMax);
    chnMin[i]->updateMinMax(10 * channelMax);
    chnMax[i]->updateMinMax(10 * channelMax);
  }
  emit modified();
}

void Channels::invEdited()
{
  if (!lock) {
    QComboBox *cb = qobject_cast<QComboBox*>(sender());
    int index = cb->property("index").toInt();
    model->limitData[index].revert = cb->currentIndex();
    emit modified();
  }
}

void Channels::curveEdited()
{
  if (!lock) {
    QComboBox *cb = qobject_cast<QComboBox*>(sender());
    int index = cb->property("index").toInt();
    model->limitData[index].curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, cb->itemData(cb->currentIndex()).toInt());
    emit modified();
  }
}

void Channels::ppmcenterEdited()
{
  if (!lock) {
    QSpinBox *sb = qobject_cast<QSpinBox*>(sender());
    int index = sb->property("index").toInt();
    model->limitData[index].ppmCenter = sb->value() - 1500;
    emit modified();
  }
}

void Channels::update()
{
  for (int i=0; i<firmware->getCapability(Outputs); i++) {
    updateLine(i);
  }
}

void Channels::updateLine(int i)
{
  lock = true;
  if (firmware->getCapability(ChannelsName) > 0) {
    name[i]->setText(model->limitData[i].name);
  }
  chnOffset[i]->setValue(model->limitData[i].offset);
  chnMin[i]->setValue(model->limitData[i].min);
  chnMax[i]->setValue(model->limitData[i].max);
  invCB[i]->setCurrentIndex((model->limitData[i].revert) ? 1 : 0);
  if (IS_HORUS_OR_TARANIS(firmware->getBoard())) {
    curveCB[i]->setCurrentIndex(model->limitData[i].curve.value + firmware->getCapability(NumCurves));
  }
  if (firmware->getCapability(PPMCenter)) {
    centerSB[i]->setValue(model->limitData[i].ppmCenter + 1500);
  }
  if (firmware->getCapability(SYMLimits)) {
    symlimitsChk[i]->setChecked(model->limitData[i].symetrical);
  }
  lock = false;
}

void Channels::chnPaste()
{
  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();
  if (mimeData->hasFormat("application/x-companion-chn")) {
    QByteArray chnData = mimeData->data("application/x-companion-chn");
    LimitData *chn = &model->limitData[selectedChannel];
    memcpy(chn, chnData.constData(), sizeof(LimitData));
    updateLine(selectedChannel);
    emit modified();
  }
}

void Channels::chnDelete()
{
  model->limitData[selectedChannel].clear();
  updateLine(selectedChannel);
  emit modified();
}

void Channels::chnCopy()
{
  QByteArray chnData;
  chnData.append((char*)&model->limitData[selectedChannel],sizeof(LimitData));
  QMimeData *mimeData = new QMimeData;
  mimeData->setData("application/x-companion-chn", chnData);
  QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void Channels::chnCut()
{
  chnCopy();
  chnDelete();
}

void Channels::chn_customContextMenuRequested(QPoint pos)
{
    QLabel *label = (QLabel *)sender();
    selectedChannel = label->property("index").toInt();

    QPoint globalPos = label->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion-chn");

    QMenu contextMenu;
    contextMenu.addAction(CompanionIcon("copy.png"), tr("&Copy"),this,SLOT(chnCopy()));
    contextMenu.addAction(CompanionIcon("cut.png"), tr("&Cut"),this,SLOT(chnCut()));
    contextMenu.addAction(CompanionIcon("paste.png"), tr("&Paste"),this,SLOT(chnPaste()))->setEnabled(hasData);
    contextMenu.addAction(CompanionIcon("clear.png"), tr("&Delete"),this,SLOT(chnDelete()));

    contextMenu.exec(globalPos);
}
