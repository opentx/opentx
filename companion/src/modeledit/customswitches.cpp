#include "customswitches.h"
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include "helpers.h"

LogicalSwitchesPanel::LogicalSwitchesPanel(QWidget * parent, ModelData & model):
  ModelPanel(parent, model),
  selectedSwitch(0)
{
  QGridLayout * gridLayout = new QGridLayout(this);

  int col = 1;
  addLabel(gridLayout, tr("Condition"), col++);
  addLabel(gridLayout, tr("V1"), col++);
  addLabel(gridLayout, tr("V2"), col++);
  addLabel(gridLayout, tr("AND"), col++);
  if (GetEepromInterface()->getCapability(LogicalSwitchesExt)) {
    addLabel(gridLayout, tr("Duration"), col++);
    addLabel(gridLayout, tr("Delay"), col++);
  }

  lock = true;
  for (int i=0; i<GetEepromInterface()->getCapability(LogicalSwitches); i++) {
    // The label
    QLabel * label = new QLabel(this);
    label->setProperty("index", i);
    if (i < 9)
      label->setText(tr("LS%1").arg(i+1));
    else
      label->setText(tr("LS%1").arg(QChar('A'+i-9)));
    label->setContextMenuPolicy(Qt::CustomContextMenu);
    label->setMouseTracking(true);
    connect(label, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(csw_customContextMenuRequested(QPoint)));
    gridLayout->addWidget(label, i+1, 0);

    // The function
    csw[i] = new QComboBox(this);
    csw[i]->setProperty("index", i);
    connect(csw[i], SIGNAL(currentIndexChanged(int)), this, SLOT(edited()));
    gridLayout->addWidget(csw[i], i+1, 1);

    // V1
    cswitchSource1[i] = new QComboBox(this);
    cswitchSource1[i]->setProperty("index",i);
    connect(cswitchSource1[i], SIGNAL(currentIndexChanged(int)), this, SLOT(v1Edited(int)));
    gridLayout->addWidget(cswitchSource1[i], i+1, 2);
    cswitchSource1[i]->setVisible(false);
    cswitchValue[i] = new QDoubleSpinBox(this);
    cswitchValue[i]->setMaximum(125);
    cswitchValue[i]->setMinimum(-125);
    cswitchValue[i]->setAccelerated(true);
    cswitchValue[i]->setDecimals(0);
    cswitchValue[i]->setProperty("index", i);
    connect(cswitchValue[i], SIGNAL(editingFinished()), this, SLOT(edited()));
    gridLayout->addWidget(cswitchValue[i], i+1, 2);
    cswitchValue[i]->setVisible(false);

    // V2
    cswitchSource2[i] = new QComboBox(this);
    cswitchSource2[i]->setProperty("index", i);
    connect(cswitchSource2[i], SIGNAL(currentIndexChanged(int)), this, SLOT(v2Edited(int)));
    gridLayout->addWidget(cswitchSource2[i], i+1, 3);
    cswitchSource2[i]->setVisible(false);
    cswitchOffset[i] = new QDoubleSpinBox(this);
    cswitchOffset[i]->setProperty("index",i);
    cswitchOffset[i]->setMaximum(125);
    cswitchOffset[i]->setMinimum(-125);
    cswitchOffset[i]->setAccelerated(true);
    cswitchOffset[i]->setDecimals(0);
    connect(cswitchOffset[i], SIGNAL(editingFinished()), this, SLOT(edited()));
    gridLayout->addWidget(cswitchOffset[i], i+1, 3);
    cswitchOffset[i]->setVisible(false);

    // AND
    cswitchAnd[i] = new QComboBox(this);
    cswitchAnd[i]->setProperty("index", i);
    connect(cswitchAnd[i], SIGNAL(currentIndexChanged(int)), this, SLOT(andEdited(int)));
    gridLayout->addWidget(cswitchAnd[i], i+1, 4);

    if (GetEepromInterface()->getCapability(LogicalSwitchesExt)) {
      // Duration
      cswitchDuration[i] = new QDoubleSpinBox(this);
      cswitchDuration[i]->setProperty("index", i);
      cswitchDuration[i]->setSingleStep(0.1);
      cswitchDuration[i]->setMaximum(25);
      cswitchDuration[i]->setMinimum(0);
      cswitchDuration[i]->setAccelerated(true);
      cswitchDuration[i]->setDecimals(1);
      connect(cswitchDuration[i], SIGNAL(valueChanged(double)), this, SLOT(durationEdited(double)));
      gridLayout->addWidget(cswitchDuration[i], i+1, 5);

      // Delay
      cswitchDelay[i] = new QDoubleSpinBox(this);
      cswitchDelay[i]->setProperty("index", i);
      cswitchDelay[i]->setSingleStep(0.1);
      cswitchDelay[i]->setMaximum(25);
      cswitchDelay[i]->setMinimum(0);
      cswitchDelay[i]->setAccelerated(true);
      cswitchDelay[i]->setDecimals(1);
      connect(cswitchDelay[i], SIGNAL(valueChanged(double)), this, SLOT(delayEdited(double)));
      gridLayout->addWidget(cswitchDelay[i], i+1, 6);
    }
  }

  lock = false;
}

LogicalSwitchesPanel::~LogicalSwitchesPanel()
{
}

void LogicalSwitchesPanel::v1Edited(int value)
{
  if (!lock) {
    int i = sender()->property("index").toInt();
    model.customSw[i].val1 = cswitchSource1[i]->itemData(value).toInt();
    if (model.customSw[i].getFunctionFamily() == LS_FAMILY_VOFS) {
      RawSource source = RawSource(model.customSw[i].val1, &model);
      if (source.type == SOURCE_TYPE_TELEMETRY) {
        if (model.customSw[i].func == LS_FN_DPOS || model.customSw[i].func == LS_FN_DAPOS) {
          model.customSw[i].val2 = 0;
        }
        else {
          model.customSw[i].val2 = -128;
        }
      }
      else {
        RawSourceRange range = source.getRange();
        if (model.customSw[i].func == LS_FN_DPOS || model.customSw[i].func == LS_FN_DAPOS) {
          model.customSw[i].val2 = (cswitchOffset[i]->value() / range.step);
        }
        else {
          model.customSw[i].val2 = (cswitchOffset[i]->value() - range.offset) / range.step/* TODO - source.getRawOffset(model)*/;
        }
      }
      setSwitchWidgetVisibility(i);
    }
    emit modified();
  }
}

void LogicalSwitchesPanel::v2Edited(int value)
{
  if (!lock) {
    int i = sender()->property("index").toInt();
    model.customSw[i].val2 = cswitchSource2[i]->itemData(value).toInt();
    emit modified();
  }
}

void LogicalSwitchesPanel::andEdited(int value)
{
  if (!lock) {
    int index = sender()->property("index").toInt();
    model.customSw[index].andsw = cswitchAnd[index]->itemData(value).toInt();
    emit modified();
  }
}

void LogicalSwitchesPanel::durationEdited(double duration)
{
  int index = sender()->property("index").toInt();
  model.customSw[index].duration = (uint8_t)round(duration*10);
  emit modified();
}

void LogicalSwitchesPanel::delayEdited(double delay)
{
  int index = sender()->property("index").toInt();
  model.customSw[index].delay = (uint8_t)round(delay*10);
  emit modified();
}

void LogicalSwitchesPanel::edited()
{
  if (!lock) {
    lock = true;
    int i = sender()->property("index").toInt();
    int newFunc = csw[i]->itemData(csw[i]->currentIndex()).toInt();
    bool chAr = (model.customSw[i].getFunctionFamily() != LogicalSwitchData(newFunc).getFunctionFamily());
    model.customSw[i].func = newFunc;
    if (chAr) {
      if (model.customSw[i].getFunctionFamily() == LS_FAMILY_TIMER) {
        model.customSw[i].val1 = -119;
        model.customSw[i].val2 = -119;
      }
      else if (model.customSw[i].getFunctionFamily() == LS_FAMILY_STAY) {
        model.customSw[i].val1 = 0;
        model.customSw[i].val2 = -129;
        model.customSw[i].val3 = 0;
      }
      else {
        model.customSw[i].val1 = 0;
        model.customSw[i].val2 = 0;
      }
      model.customSw[i].andsw = 0;
      setSwitchWidgetVisibility(i);
    }

    RawSource source;
    switch (model.customSw[i].getFunctionFamily())
    {
      case LS_FAMILY_VOFS:
        {
          source = RawSource(model.customSw[i].val1, &model);
          RawSourceRange range = source.getRange();
          if (model.customSw[i].func == LS_FN_DPOS || model.customSw[i].func == LS_FN_DAPOS) {
            model.customSw[i].val2 = (cswitchOffset[i]->value() / range.step);
            cswitchOffset[i]->setValue(model.customSw[i].val2*range.step);
          }
          else {
            model.customSw[i].val2 = ((cswitchOffset[i]->value()-range.offset)/range.step)/* TODO - source.getRawOffset(model)*/;
            cswitchOffset[i]->setValue((model.customSw[i].val2 /* + TODO source.getRawOffset(model)*/)*range.step+range.offset);
          }
        }
        break;
      case LS_FAMILY_TIMER:
        model.customSw[i].val1 = TimToVal(cswitchValue[i]->value());
        model.customSw[i].val2 = TimToVal(cswitchOffset[i]->value());
        updateTimerParam(cswitchValue[i], model.customSw[i].val1);
        updateTimerParam(cswitchOffset[i], model.customSw[i].val2);
        break;
      case LS_FAMILY_STAY:
        model.customSw[i].val2 = TimToVal(cswitchOffset[i]->value());
        updateTimerParam(cswitchOffset[i], model.customSw[i].val2, true);
        break;
      default:
        break;
    }
    emit modified();
    lock = false;
  }
}

void LogicalSwitchesPanel::updateTimerParam(QDoubleSpinBox *sb, int timer, bool allowZero)
{
  sb->setVisible(true);
  sb->setDecimals(1);
  sb->setMinimum(allowZero ? 0.0 : 0.1);
  sb->setMaximum(175);
  float value = ValToTim(timer);
  if (value>60)
    sb->setSingleStep(1);
  else if (value>2)
    sb->setSingleStep(0.5);
  else
    sb->setSingleStep(0.1);
  sb->setValue(value);
}

void LogicalSwitchesPanel::setSwitchWidgetVisibility(int i)
{
  lock = true;

    RawSource source = RawSource(model.customSw[i].val1, &model);
    RawSourceRange range = source.getRange();

    switch (model.customSw[i].getFunctionFamily())
    {
      case LS_FAMILY_VOFS:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(false);
        cswitchValue[i]->setVisible(false);
        cswitchOffset[i]->setVisible(true);
        populateSourceCB(cswitchSource1[i], source, model, POPULATE_SOURCES | POPULATE_VIRTUAL_INPUTS | POPULATE_TRIMS | POPULATE_SWITCHES | POPULATE_TELEMETRY | (GetEepromInterface()->getCapability(GvarsInCS) ? POPULATE_GVARS : 0));
        cswitchOffset[i]->setDecimals(range.decimals);
        cswitchOffset[i]->setSingleStep(range.step);
        if (model.customSw[i].func == LS_FN_DPOS || model.customSw[i].func == LS_FN_DAPOS) {
          cswitchOffset[i]->setMinimum(range.step*-127);
          cswitchOffset[i]->setMaximum(range.step*127);
          cswitchOffset[i]->setValue(range.step*model.customSw[i].val2);
        }
        else {
          cswitchOffset[i]->setMinimum(range.min);
          cswitchOffset[i]->setMaximum(range.max);
          cswitchOffset[i]->setValue(range.step*(model.customSw[i].val2/* TODO+source.getRawOffset(model)*/)+range.offset);
        }
        break;
      case LS_FAMILY_VBOOL:
      case LS_FAMILY_STICKY:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(true);
        cswitchValue[i]->setVisible(false);
        cswitchOffset[i]->setVisible(false);
        populateSwitchCB(cswitchSource1[i], RawSwitch(model.customSw[i].val1));
        populateSwitchCB(cswitchSource2[i], RawSwitch(model.customSw[i].val2));
        break;
      case LS_FAMILY_STAY:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(false);
        cswitchValue[i]->setVisible(false);
        populateSwitchCB(cswitchSource1[i], RawSwitch(model.customSw[i].val1));
        updateTimerParam(cswitchOffset[i], model.customSw[i].val2, true);
        break;
      case LS_FAMILY_VCOMP:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(true);
        cswitchValue[i]->setVisible(false);
        cswitchOffset[i]->setVisible(false);
        populateSourceCB(cswitchSource1[i], RawSource(model.customSw[i].val1), model, POPULATE_SOURCES | POPULATE_VIRTUAL_INPUTS | POPULATE_TRIMS | POPULATE_SWITCHES | POPULATE_TELEMETRY | (GetEepromInterface()->getCapability(GvarsInCS) ? POPULATE_GVARS : 0));
        populateSourceCB(cswitchSource2[i], RawSource(model.customSw[i].val2), model, POPULATE_SOURCES | POPULATE_TRIMS | POPULATE_VIRTUAL_INPUTS | POPULATE_SWITCHES | POPULATE_TELEMETRY | (GetEepromInterface()->getCapability(GvarsInCS) ? POPULATE_GVARS : 0));
        break;
      case LS_FAMILY_TIMER:
        cswitchSource1[i]->setVisible(false);
        cswitchSource2[i]->setVisible(false);
        updateTimerParam(cswitchValue[i], model.customSw[i].val1);
        updateTimerParam(cswitchOffset[i], model.customSw[i].val2);
        break;
    }

    lock = false;
}

void LogicalSwitchesPanel::update()
{
  for (int i=0; i<GetEepromInterface()->getCapability(LogicalSwitches); i++) {
    lock = true;
    populateCSWCB(csw[i], model.customSw[i].func);
    lock = false;
    setSwitchWidgetVisibility(i);
    lock = true;
    populateAndSwitchCB(cswitchAnd[i], RawSwitch(model.customSw[i].andsw));
    if (GetEepromInterface()->getCapability(LogicalSwitchesExt)) {
      cswitchDuration[i]->setValue(model.customSw[i].duration/10.0);
      cswitchDelay[i]->setValue(model.customSw[i].delay/10.0);
    }
    lock = false;
  }
}

void LogicalSwitchesPanel::cswPaste()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if (mimeData->hasFormat("application/x-companion-csw")) {
      QByteArray cswData = mimeData->data("application/x-companion-csw");

      LogicalSwitchData *csw = &model.customSw[selectedSwitch];
      memcpy(csw, cswData.mid(0, sizeof(LogicalSwitchData)).constData(), sizeof(LogicalSwitchData));
      emit modified();
      updateSelectedSwitch();
    }
}

void LogicalSwitchesPanel::cswDelete()
{
    model.customSw[selectedSwitch].clear();
    emit modified();
    updateSelectedSwitch();
}

void LogicalSwitchesPanel::cswCopy()
{
    QByteArray cswData;
    cswData.append((char*)&model.customSw[selectedSwitch],sizeof(LogicalSwitchData));
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion-csw", cswData);
    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void LogicalSwitchesPanel::updateSelectedSwitch()
{
    lock = true;
    populateCSWCB(csw[selectedSwitch], model.customSw[selectedSwitch].func);
    setSwitchWidgetVisibility(selectedSwitch);
    lock = false;
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

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion-csw");

    QMenu contextMenu;
    contextMenu.addAction(CompanionIcon("clear.png"), tr("&Delete"),this,SLOT(cswDelete()),tr("Delete"));
    contextMenu.addAction(CompanionIcon("copy.png"), tr("&Copy"),this,SLOT(cswCopy()),tr("Ctrl+C"));
    contextMenu.addAction(CompanionIcon("cut.png"), tr("&Cut"),this,SLOT(cswCut()),tr("Ctrl+X"));
    contextMenu.addAction(CompanionIcon("paste.png"), tr("&Paste"),this,SLOT(cswPaste()),tr("Ctrl+V"))->setEnabled(hasData);

    contextMenu.exec(globalPos);
}
