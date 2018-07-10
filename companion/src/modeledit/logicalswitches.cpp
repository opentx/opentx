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

#include "logicalswitches.h"
#include "switchitemmodel.h"
#include "helpers.h"

#include <TimerEdit>

LogicalSwitchesPanel::LogicalSwitchesPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  rawSourceItemModel(NULL),
  selectedSwitch(0)
{
  Stopwatch s1("LogicalSwitchesPanel");

  int channelsMax = model.getChannelsMax(true);
  rawSwitchItemModel = new RawSwitchFilterItemModel(&generalSettings, &model, LogicalSwitchesContext);

  setDataModels();

  QStringList headerLabels;
  headerLabels << "#" << tr("Function") << tr("V1") << tr("V2") << tr("AND Switch");
  if (firmware->getCapability(LogicalSwitchesExt)) {
    headerLabels << tr("Duration") << tr("Delay");
  }
  TableLayout * tableLayout = new TableLayout(this, firmware->getCapability(LogicalSwitches), headerLabels);

  s1.report("header");

  lock = true;
  for (int i=0; i<firmware->getCapability(LogicalSwitches); i++) {
    // The label
    QLabel * label = new QLabel(this);
    label->setProperty("index", i);
    label->setText(RawSwitch(SWITCH_TYPE_VIRTUAL, i+1).toString());
    label->setContextMenuPolicy(Qt::CustomContextMenu);
    label->setMouseTracking(true);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    connect(label, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(csw_customContextMenuRequested(QPoint)));
    tableLayout->addWidget(i, 0, label);

    // The function
    csw[i] = new QComboBox(this);
    csw[i]->setProperty("index", i);
    populateCSWCB(csw[i]);
    connect(csw[i], SIGNAL(currentIndexChanged(int)), this, SLOT(functionChanged()));
    tableLayout->addWidget(i, 1, csw[i]);

    // V1
    QHBoxLayout *v1Layout = new QHBoxLayout();
    cswitchSource1[i] = new QComboBox(this);
    cswitchSource1[i]->setProperty("index",i);
    connect(cswitchSource1[i], SIGNAL(currentIndexChanged(int)), this, SLOT(v1Edited(int)));
    v1Layout->addWidget(cswitchSource1[i]);
    cswitchSource1[i]->setVisible(false);
    cswitchValue[i] = new QDoubleSpinBox(this);
    cswitchValue[i]->setMaximum(channelsMax);
    cswitchValue[i]->setMinimum(-channelsMax);
    cswitchValue[i]->setAccelerated(true);
    cswitchValue[i]->setDecimals(0);
    cswitchValue[i]->setProperty("index", i);
    connect(cswitchValue[i], SIGNAL(editingFinished()), this, SLOT(offsetEdited()));
    v1Layout->addWidget(cswitchValue[i]);
    cswitchValue[i]->setVisible(false);
    tableLayout->addLayout(i, 2, v1Layout);

    // V2
    QHBoxLayout *v2Layout = new QHBoxLayout();
    cswitchSource2[i] = new QComboBox(this);
    cswitchSource2[i]->setProperty("index", i);
    connect(cswitchSource2[i], SIGNAL(currentIndexChanged(int)), this, SLOT(v2Edited(int)));
    v2Layout->addWidget(cswitchSource2[i]);
    cswitchSource2[i]->setVisible(false);
    cswitchOffset[i] = new QDoubleSpinBox(this);
    cswitchOffset[i]->setProperty("index",i);
    cswitchOffset[i]->setMaximum(channelsMax);
    cswitchOffset[i]->setMinimum(-channelsMax);
    cswitchOffset[i]->setAccelerated(true);
    cswitchOffset[i]->setDecimals(0);
    connect(cswitchOffset[i], SIGNAL(editingFinished()), this, SLOT(offsetEdited()));
    cswitchOffset[i]->setVisible(false);
    v2Layout->addWidget(cswitchOffset[i]);
    cswitchOffset2[i] = new QDoubleSpinBox(this);
    cswitchOffset2[i]->setProperty("index",i);
    cswitchOffset2[i]->setMaximum(channelsMax);
    cswitchOffset2[i]->setMinimum(-channelsMax);
    cswitchOffset2[i]->setAccelerated(true);
    cswitchOffset2[i]->setDecimals(0);
    cswitchOffset2[i]->setSpecialValueText(" " + tr("(instant)"));
    connect(cswitchOffset2[i], SIGNAL(editingFinished()), this, SLOT(offsetEdited()));
    cswitchOffset2[i]->setVisible(false);
    v2Layout->addWidget(cswitchOffset2[i]);
    cswitchTOffset[i] = new TimerEdit(this);
    cswitchTOffset[i]->setProperty("index",i);
    connect(cswitchTOffset[i],SIGNAL(editingFinished()),this,SLOT(offsetEdited()));
    v2Layout->addWidget(cswitchTOffset[i]);
    cswitchTOffset[i]->setVisible(false);
    tableLayout->addLayout(i, 3, v2Layout);

    // AND
    cswitchAnd[i] = new QComboBox(this);
    cswitchAnd[i]->setProperty("index", i);
    populateAndSwitchCB(cswitchAnd[i]);
    connect(cswitchAnd[i], SIGNAL(currentIndexChanged(int)), this, SLOT(andEdited(int)));
    tableLayout->addWidget(i, 4, cswitchAnd[i]);

    if (firmware->getCapability(LogicalSwitchesExt)) {
      // Duration
      cswitchDuration[i] = new QDoubleSpinBox(this);
      cswitchDuration[i]->setProperty("index", i);
      cswitchDuration[i]->setSingleStep(0.1);
      cswitchDuration[i]->setMaximum(25);
      cswitchDuration[i]->setMinimum(0);
      cswitchDuration[i]->setAccelerated(true);
      cswitchDuration[i]->setDecimals(1);
      connect(cswitchDuration[i], SIGNAL(valueChanged(double)), this, SLOT(durationEdited(double)));
      tableLayout->addWidget(i, 5, cswitchDuration[i]);

      // Delay
      cswitchDelay[i] = new QDoubleSpinBox(this);
      cswitchDelay[i]->setProperty("index", i);
      cswitchDelay[i]->setSingleStep(0.1);
      cswitchDelay[i]->setMaximum(25);
      cswitchDelay[i]->setMinimum(0);
      cswitchDelay[i]->setAccelerated(true);
      cswitchDelay[i]->setDecimals(1);
      connect(cswitchDelay[i], SIGNAL(valueChanged(double)), this, SLOT(delayEdited(double)));
      tableLayout->addWidget(i, 6, cswitchDelay[i]);
    }
  }

  s1.report("added elements");

  disableMouseScrolling();
  lock = false;
  update();
  tableLayout->resizeColumnsToContents();
  tableLayout->pushRowsUp(firmware->getCapability(LogicalSwitches)+1);
  s1.report("end");
}

LogicalSwitchesPanel::~LogicalSwitchesPanel()
{
}

void LogicalSwitchesPanel::setDataModels()
{
  if (rawSourceItemModel)
    rawSourceItemModel->deleteLater();

  int srcFlags =  POPULATE_NONE | POPULATE_SOURCES | POPULATE_SCRIPT_OUTPUTS | POPULATE_VIRTUAL_INPUTS | POPULATE_TRIMS | POPULATE_SWITCHES | POPULATE_TELEMETRY | (firmware->getCapability(GvarsInCS) ? POPULATE_GVARS : 0);
  rawSourceItemModel = Helpers::getRawSourceItemModel(&generalSettings, model, srcFlags);
  rawSourceItemModel->setParent(this);
}

void LogicalSwitchesPanel::functionChanged()
{
  int i = sender()->property("index").toInt();
  unsigned newFunc = csw[i]->currentData().toUInt();

  if (model->logicalSw[i].func == newFunc)
    return;

  CSFunctionFamily oldFuncFamily = model->logicalSw[i].getFunctionFamily();
  model->logicalSw[i].func = newFunc;
  CSFunctionFamily newFuncFamily = model->logicalSw[i].getFunctionFamily();

  if (oldFuncFamily != newFuncFamily) {
    model->logicalSw[i].clear();
    model->logicalSw[i].func = newFunc;
    if (newFuncFamily == LS_FAMILY_TIMER) {
      model->logicalSw[i].val1 = -119;
      model->logicalSw[i].val2 = -119;
    }
    else if (newFuncFamily == LS_FAMILY_EDGE) {
      model->logicalSw[i].val2 = -129;
    }
  }
  update();
  emit modified();
}

void LogicalSwitchesPanel::v1Edited(int value)
{
  if (!lock) {
    int i = sender()->property("index").toInt();
    model->logicalSw[i].val1 = cswitchSource1[i]->itemData(value).toInt();
    if (model->logicalSw[i].getFunctionFamily() == LS_FAMILY_VOFS) {
      if (!offsetEditedAt(i))
        updateLine(i);
    }
    else {
      emit modified();
    }
  }
}

void LogicalSwitchesPanel::v2Edited(int value)
{
  if (!lock) {
    int i = sender()->property("index").toInt();
    model->logicalSw[i].val2 = cswitchSource2[i]->itemData(value).toInt();
    emit modified();
  }
}

void LogicalSwitchesPanel::andEdited(int value)
{
  if (!lock) {
    int index = sender()->property("index").toInt();
    model->logicalSw[index].andsw = cswitchAnd[index]->itemData(value).toInt();
    emit modified();
  }
}

void LogicalSwitchesPanel::durationEdited(double duration)
{
  if (!lock) {
    int index = sender()->property("index").toInt();
    model->logicalSw[index].duration = (uint8_t)round(duration*10);
    emit modified();
  }
}

void LogicalSwitchesPanel::delayEdited(double delay)
{
  if (!lock) {
    int index = sender()->property("index").toInt();
    model->logicalSw[index].delay = (uint8_t)round(delay*10);
    emit modified();
  }
}

void LogicalSwitchesPanel::offsetEdited()
{
  offsetEditedAt(sender()->property("index").toInt());
}

bool LogicalSwitchesPanel::offsetEditedAt(int index)
{
  if (lock)
    return false;

  lock = true;
  bool mod = false;
  int value;

  switch (model->logicalSw[index].getFunctionFamily())
  {
    case LS_FAMILY_VOFS:
    {
      RawSource source = RawSource(model->logicalSw[index].val1);
      RawSourceRange range = source.getRange(model, generalSettings, model->logicalSw[index].getRangeFlags());
      double currVal = source.isTimeBased() ? cswitchTOffset[index]->timeInSeconds() : cswitchOffset[index]->value();
      value = round((currVal - range.offset) / range.step);
      mod = (mod || value != model->logicalSw[index].val2);
      model->logicalSw[index].val2 = value;
      break;
    }

    case LS_FAMILY_TIMER:
      value = TimToVal(cswitchValue[index]->value());
      mod = (mod || value != model->logicalSw[index].val1);
      model->logicalSw[index].val1 = value;
      value = TimToVal(cswitchOffset[index]->value());
      mod = (mod || value != model->logicalSw[index].val2);
      model->logicalSw[index].val2 = value;
      break;

    case LS_FAMILY_EDGE:
      if (sender() == cswitchOffset[index]) {
        value = TimToVal(cswitchOffset[index]->value());
        mod = (mod || value != model->logicalSw[index].val2);
        model->logicalSw[index].val2 = value;
      }
      else {
        value = TimToVal(cswitchOffset2[index]->value()) - model->logicalSw[index].val2;
        mod = (mod || value != model->logicalSw[index].val3);
        model->logicalSw[index].val3 = value;
      }
      break;

    default:
      break;
  }
  lock = false;

  if (mod) {
    updateLine(index);
    emit modified();
  }
  return mod;
}

void LogicalSwitchesPanel::updateTimerParam(QDoubleSpinBox *sb, int timer, double minimum)
{
  sb->setVisible(true);
  sb->setDecimals(1);
  sb->setMinimum(minimum);
  sb->setMaximum(175);
  float value = ValToTim(timer);
  if (value>=60)
    sb->setSingleStep(1);
  else if (value>=2)
    sb->setSingleStep(0.5);
  else
    sb->setSingleStep(0.1);
  sb->setValue(value);
}

#define SOURCE1_VISIBLE  0x1
#define SOURCE2_VISIBLE  0x2
#define VALUE1_VISIBLE   0x4
#define VALUE2_VISIBLE   0x8
#define VALUE3_VISIBLE   0x10
#define VALUE_TO_VISIBLE 0x20
#define DELAY_ENABLED    0x40
#define DURATION_ENABLED 0x80
#define LINE_ENABLED     0x100

void LogicalSwitchesPanel::updateLine(int i)
{
  lock = true;
  unsigned int mask;

  csw[i]->setCurrentIndex(csw[i]->findData(model->logicalSw[i].func));
  cswitchAnd[i]->setCurrentIndex(cswitchAnd[i]->findData(RawSwitch(model->logicalSw[i].andsw).toValue()));

  if (!model->logicalSw[i].func) {
    mask = 0;
  }
  else {
    mask = LINE_ENABLED | DELAY_ENABLED | DURATION_ENABLED;

    switch (model->logicalSw[i].getFunctionFamily())
    {
      case LS_FAMILY_VOFS:
      {
        mask |= SOURCE1_VISIBLE;
        RawSource source = RawSource(model->logicalSw[i].val1);
        RawSourceRange range = source.getRange(model, generalSettings, model->logicalSw[i].getRangeFlags());
        double value = range.step * model->logicalSw[i].val2 + range.offset;  /* TODO+source.getRawOffset(model)*/
        cswitchSource1[i]->setModel(rawSourceItemModel);
        cswitchSource1[i]->setCurrentIndex(cswitchSource1[i]->findData(source.toValue()));
        if (source.isTimeBased()) {
          mask |= VALUE_TO_VISIBLE;
          cswitchTOffset[i]->setTimeRange(range.min, range.max);
          cswitchTOffset[i]->setSingleStep(range.step);
          cswitchTOffset[i]->setPageStep(range.step * 60);
          cswitchTOffset[i]->setShowSeconds(range.step != 60);
          cswitchTOffset[i]->setTime((int)value);
        }
        else {
          mask |= VALUE2_VISIBLE;
          if (range.unit.isEmpty())
            cswitchOffset[i]->setSuffix("");
          else
            cswitchOffset[i]->setSuffix(" " + range.unit);
          cswitchOffset[i]->setDecimals(range.decimals);
          cswitchOffset[i]->setMinimum(range.min);
          cswitchOffset[i]->setMaximum(range.max);
          cswitchOffset[i]->setSingleStep(range.step);
          cswitchOffset[i]->setValue(value);
        }

        break;
      }

      case LS_FAMILY_STICKY:  // no break
      case LS_FAMILY_VBOOL:
        mask |= SOURCE1_VISIBLE | SOURCE2_VISIBLE;
        cswitchSource1[i]->setModel(rawSwitchItemModel);
        cswitchSource1[i]->setCurrentIndex(cswitchSource1[i]->findData(model->logicalSw[i].val1));
        cswitchSource2[i]->setModel(rawSwitchItemModel);
        cswitchSource2[i]->setCurrentIndex(cswitchSource2[i]->findData(model->logicalSw[i].val2));
        break;

      case LS_FAMILY_EDGE:
        mask |= SOURCE1_VISIBLE | VALUE2_VISIBLE | VALUE3_VISIBLE;
        mask &= ~DELAY_ENABLED;
        cswitchSource1[i]->setModel(rawSwitchItemModel);
        cswitchSource1[i]->setCurrentIndex(cswitchSource1[i]->findData(model->logicalSw[i].val1));
        updateTimerParam(cswitchOffset[i], model->logicalSw[i].val2, 0.0);
        updateTimerParam(cswitchOffset2[i], model->logicalSw[i].val2+model->logicalSw[i].val3, ValToTim(TimToVal(cswitchOffset[i]->value())-1));
        cswitchOffset2[i]->setSuffix((model->logicalSw[i].val3) ? "" : tr(" (infinite)"));
        break;

      case LS_FAMILY_VCOMP:
        mask |= SOURCE1_VISIBLE | SOURCE2_VISIBLE;
        cswitchSource1[i]->setModel(rawSourceItemModel);
        cswitchSource1[i]->setCurrentIndex(cswitchSource1[i]->findData(model->logicalSw[i].val1));
        cswitchSource2[i]->setModel(rawSourceItemModel);
        cswitchSource2[i]->setCurrentIndex(cswitchSource2[i]->findData(model->logicalSw[i].val2));
        break;

      case LS_FAMILY_TIMER:
        mask |= VALUE1_VISIBLE | VALUE2_VISIBLE;
        updateTimerParam(cswitchValue[i], model->logicalSw[i].val1, 0.1);
        updateTimerParam(cswitchOffset[i], model->logicalSw[i].val2, 0.1);
        break;
    }
  }

  cswitchSource1[i]->setVisible(mask & SOURCE1_VISIBLE);
  cswitchSource2[i]->setVisible(mask & SOURCE2_VISIBLE);
  cswitchValue[i]->setVisible(mask & VALUE1_VISIBLE);
  cswitchOffset[i]->setVisible(mask & VALUE2_VISIBLE);
  cswitchOffset2[i]->setVisible(mask & VALUE3_VISIBLE);
  cswitchTOffset[i]->setVisible(mask & VALUE_TO_VISIBLE);
  cswitchAnd[i]->setVisible(mask & LINE_ENABLED);
  if (firmware->getCapability(LogicalSwitchesExt)) {
    cswitchDuration[i]->setVisible(mask & DURATION_ENABLED);
    cswitchDelay[i]->setVisible(mask & DELAY_ENABLED);
    if (mask & DURATION_ENABLED)
      cswitchDuration[i]->setValue(model->logicalSw[i].duration/10.0);
    if (mask & DELAY_ENABLED)
      cswitchDelay[i]->setValue(model->logicalSw[i].delay/10.0);
  }

  rawSwitchItemModel->update();
  lock = false;
}

void LogicalSwitchesPanel::populateCSWCB(QComboBox *b)
{
  int order[] = {
    LS_FN_OFF,
    LS_FN_VEQUAL, // added at the end to avoid everything renumbered
    LS_FN_VALMOSTEQUAL, // added at the end to avoid everything renumbered
    LS_FN_VPOS,
    LS_FN_VNEG,
    // LS_FN_RANGE,
    LS_FN_APOS,
    LS_FN_ANEG,
    LS_FN_AND,
    LS_FN_OR,
    LS_FN_XOR,
    LS_FN_EDGE,
    LS_FN_EQUAL,
    LS_FN_NEQUAL,
    LS_FN_GREATER,
    LS_FN_LESS,
    LS_FN_EGREATER,
    LS_FN_ELESS,
    LS_FN_DPOS,
    LS_FN_DAPOS,
    LS_FN_TIMER,
    LS_FN_STICKY
  };

  b->clear();
  for (int i=0; i<LS_FN_MAX; i++) {
    int func = order[i];
    if (func == LS_FN_NEQUAL || func == LS_FN_EGREATER || func == LS_FN_ELESS)
      continue;
    if (!IS_ARM(firmware->getBoard())) {
      if (func == LS_FN_VEQUAL || func == LS_FN_EDGE)
        continue;
    }
    b->addItem(LogicalSwitchData(func).funcToString(), func);
  }
  b->setMaxVisibleItems(10);
}

void LogicalSwitchesPanel::populateAndSwitchCB(QComboBox *b)
{
  if (IS_ARM(firmware->getBoard())) {
    b->setModel(rawSwitchItemModel);
  }
  else {
    RawSwitch item;

    b->clear();

    item = RawSwitch(SWITCH_TYPE_NONE);
    b->addItem(item.toString(), item.toValue());

    for (int i=1; i <= Boards::getCapability(firmware->getBoard(), Board::SwitchPositions); i++) {
      item = RawSwitch(SWITCH_TYPE_SWITCH, i);
      b->addItem(item.toString(), item.toValue());
    }

    for (int i=1; i<=6; i++) {
      item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
      b->addItem(item.toString(), item.toValue());
    }
  }
  b->setVisible(true);
}

void LogicalSwitchesPanel::update()
{
  setDataModels();
  for (int i=0; i<firmware->getCapability(LogicalSwitches); i++) {
    updateLine(i);
  }
}

void LogicalSwitchesPanel::cswPaste()
{
  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();
  if (mimeData->hasFormat("application/x-companion-csw")) {
    QByteArray cswData = mimeData->data("application/x-companion-csw");
    LogicalSwitchData *csw = &model->logicalSw[selectedSwitch];
    memcpy(csw, cswData.constData(), sizeof(LogicalSwitchData));
    emit modified();
    updateLine(selectedSwitch);
  }
}

void LogicalSwitchesPanel::cswDelete()
{
  model->logicalSw[selectedSwitch].clear();
  emit modified();
  updateLine(selectedSwitch);
}

void LogicalSwitchesPanel::cswCopy()
{
  QByteArray cswData;
  cswData.append((char*)&model->logicalSw[selectedSwitch],sizeof(LogicalSwitchData));
  QMimeData *mimeData = new QMimeData;
  mimeData->setData("application/x-companion-csw", cswData);
  QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void LogicalSwitchesPanel::cswCut()
{
  cswCopy();
  cswDelete();
}

// TODO make something generic here!
void LogicalSwitchesPanel::csw_customContextMenuRequested(QPoint pos)
{
    QLabel *label = (QLabel *)sender();
    selectedSwitch = label->property("index").toInt();

    QPoint globalPos = label->mapToGlobal(pos);

    const QClipboard * clipboard = QApplication::clipboard();
    const QMimeData * mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion-csw");

    QMenu contextMenu;
    contextMenu.addAction(CompanionIcon("copy.png"), tr("&Copy"),this,SLOT(cswCopy()));
    contextMenu.addAction(CompanionIcon("cut.png"), tr("&Cut"),this,SLOT(cswCut()));
    contextMenu.addAction(CompanionIcon("paste.png"), tr("&Paste"),this,SLOT(cswPaste()))->setEnabled(hasData);
    contextMenu.addAction(CompanionIcon("clear.png"), tr("&Delete"),this,SLOT(cswDelete()));

    contextMenu.exec(globalPos);
}
