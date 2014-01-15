#include "customswitches.h"
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include "helpers.h"

CustomSwitchesPanel::CustomSwitchesPanel(QWidget * parent, ModelData & model):
  ModelPanel(parent, model),
  selectedSwitch(0)
{
  QGridLayout * gridLayout = new QGridLayout(this);

  int col = 1;
  addLabel(gridLayout, tr("Function"), col++);
  addLabel(gridLayout, tr("V1"), col++);
  addLabel(gridLayout, tr("V2"), col++);
  addLabel(gridLayout, tr("AND"), col++);
  if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
    addLabel(gridLayout, tr("Duration"), col++);
    addLabel(gridLayout, tr("Delay"), col++);
  }

  lock = true;
  for (int i=0; i<GetEepromInterface()->getCapability(CustomSwitches); i++) {
    // The label
    QLabel * label = new QLabel(this);
    label->setProperty("index", i);
    if (i < 9)
      label->setText(tr("CS%1").arg(i+1));
    else
      label->setText(tr("CS%1").arg(QChar('A'+i-9)));
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
    connect(cswitchSource1[i], SIGNAL(currentIndexChanged(int)), this, SLOT(edited()));
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
    connect(cswitchSource2[i], SIGNAL(currentIndexChanged(int)), this, SLOT(edited()));
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
    connect(cswitchAnd[i], SIGNAL(currentIndexChanged(int)), this, SLOT(edited()));
    gridLayout->addWidget(cswitchAnd[i], i+1, 4);
    cswitchAnd[i]->setVisible(false);

    if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
      // Duration
      cswitchDuration[i] = new QDoubleSpinBox(this);
      cswitchDuration[i]->setProperty("index", i);
      cswitchDuration[i]->setSingleStep(0.5);
      cswitchDuration[i]->setMaximum(50);
      cswitchDuration[i]->setMinimum(0);
      cswitchDuration[i]->setAccelerated(true);
      cswitchDuration[i]->setDecimals(1);
      connect(cswitchDuration[i],SIGNAL(editingFinished()), this, SLOT(edited()));
      gridLayout->addWidget(cswitchDuration[i], i+1, 5);
      cswitchDuration[i]->setVisible(false);

      // Delay
      cswitchDelay[i] = new QDoubleSpinBox(this);
      cswitchDelay[i]->setProperty("index", i);
      cswitchDelay[i]->setSingleStep(0.5);
      cswitchDelay[i]->setMaximum(50);
      cswitchDelay[i]->setMinimum(0);
      cswitchDelay[i]->setAccelerated(true);
      cswitchDelay[i]->setDecimals(1);
      connect(cswitchDelay[i], SIGNAL(editingFinished()), this, SLOT(edited()));
      gridLayout->addWidget(cswitchDelay[i], i+1, 6);
      cswitchDelay[i]->setVisible(false);
    }
  }

  lock = false;
}

CustomSwitchesPanel::~CustomSwitchesPanel()
{
}

void CustomSwitchesPanel::edited()
{
  if (!lock) {
    lock = true;
    int i = sender()->property("index").toInt();
    bool chAr;
    float value, step;
    int newval;
    chAr = (getCSFunctionFamily(model.customSw[i].func) != getCSFunctionFamily(csw[i]->itemData(csw[i]->currentIndex()).toInt()));
    model.customSw[i].func = csw[i]->itemData(csw[i]->currentIndex()).toInt();
    if(chAr) {
      if (getCSFunctionFamily(model.customSw[i].func)==CS_FAMILY_TIMERS) {
        model.customSw[i].val1 = -119;
        model.customSw[i].val2 = -119;
      } else {
        model.customSw[i].val1 = 0;
        model.customSw[i].val2 = 0;
      }
      model.customSw[i].andsw = 0;
      setSwitchWidgetVisibility(i);
    }
    if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
      model.customSw[i].duration= (uint8_t)round(cswitchDuration[i]->value()*2);
      model.customSw[i].delay= (uint8_t)round(cswitchDelay[i]->value()*2);
    }
    RawSource source;
    switch (getCSFunctionFamily(model.customSw[i].func))
    {
      case (CS_FAMILY_VOFS):
        if (model.customSw[i].val1 != cswitchSource1[i]->itemData(cswitchSource1[i]->currentIndex()).toInt()) {
          source=RawSource(model.customSw[i].val1);
          model.customSw[i].val1 = cswitchSource1[i]->itemData(cswitchSource1[i]->currentIndex()).toInt();
          RawSource newSource = RawSource(model.customSw[i].val1);
          if (newSource.type == SOURCE_TYPE_TELEMETRY) {
            if (model.customSw[i].func>CS_FN_ELESS && model.customSw[i].func<CS_FN_VEQUAL) {
              model.customSw[i].val2 = 0;
            } else {
              model.customSw[i].val2 = -128;
            }
          } else {
            if (model.customSw[i].func>CS_FN_ELESS && model.customSw[i].func<CS_FN_VEQUAL) {
              model.customSw[i].val2 = (cswitchOffset[i]->value()/source.getStep(model));
            } else {
              model.customSw[i].val2 = ((cswitchOffset[i]->value()-source.getOffset(model))/source.getStep(model))-source.getRawOffset(model);
            }
          }
          setSwitchWidgetVisibility(i);
       } else {
          source=RawSource(model.customSw[i].val1);
          if (model.customSw[i].func>CS_FN_ELESS && model.customSw[i].func<CS_FN_VEQUAL) {
            model.customSw[i].val2 = (cswitchOffset[i]->value()/source.getStep(model));
            cswitchOffset[i]->setValue(model.customSw[i].val2*source.getStep(model));
          } else {
            model.customSw[i].val2 = ((cswitchOffset[i]->value()-source.getOffset(model))/source.getStep(model))-source.getRawOffset(model);
            cswitchOffset[i]->setValue((model.customSw[i].val2 +source.getRawOffset(model))*source.getStep(model)+source.getOffset(model));
          }
        }
        break;
      case (CS_FAMILY_TIMERS): {
        value=cswitchOffset[i]->value();
        newval=TimToVal(value);
        if (newval>model.customSw[i].val2) {
          if (value >=60) {
            value=round(value);
            step=1;
          } else if (value>=2) {
            value=(round(value*2.0)/2);
            step=0.5;
          } else {
            step=0.1;
          }
        } else {
          if (value <=2) {
            step=0.1;
          } else if (value<=60) {
            value=(round(value*2.0)/2);
            step=0.5;
          } else {
            value=round(value);
            step=1;
          }
        }
        model.customSw[i].val2=TimToVal(value);
        value=ValToTim(model.customSw[i].val2);
        cswitchOffset[i]->setValue(value);
        cswitchOffset[i]->setSingleStep(step);

        value=cswitchValue[i]->value();
        newval=TimToVal(value);
        if (newval>model.customSw[i].val1) {
          if (value >=60) {
            value=round(value);
            step=1;
          } else if (value>=2) {
            value=(round(value*2.0)/2);
            step=0.5;
          } else {
            step=0.1;
          }
        } else {
          if (value <=2) {
            step=0.1;
          } else if (value<=60) {
            value=(round(value*2.0)/2);
            step=0.5;
          } else {
            value=round(value);
            step=1;
          }
        }
        model.customSw[i].val1=TimToVal(value);
        value=ValToTim(model.customSw[i].val1);
        cswitchValue[i]->setValue(value);
        cswitchValue[i]->setSingleStep(step);
        break;
        }
      case (CS_FAMILY_VBOOL):
      case (CS_FAMILY_VCOMP):
        model.customSw[i].val1 = cswitchSource1[i]->itemData(cswitchSource1[i]->currentIndex()).toInt();
        model.customSw[i].val2 = cswitchSource2[i]->itemData(cswitchSource2[i]->currentIndex()).toInt();
        break;
    }
    model.customSw[i].andsw = cswitchAnd[i]->itemData(cswitchAnd[i]->currentIndex()).toInt();
    emit modified();
    lock = false;
  }
}

void CustomSwitchesPanel::setSwitchWidgetVisibility(int i)
{
    RawSource source = RawSource(model.customSw[i].val1);
    switch (getCSFunctionFamily(model.customSw[i].func))
    {
      case CS_FAMILY_VOFS:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(false);
        cswitchValue[i]->setVisible(false);
        cswitchOffset[i]->setVisible(true);
        populateSourceCB(cswitchSource1[i], source, POPULATE_SOURCES | (GetEepromInterface()->getCapability(ExtraTrims) ? POPULATE_TRIMS : 0) | POPULATE_SWITCHES | POPULATE_TELEMETRY | (GetEepromInterface()->getCapability(GvarsInCS) ? POPULATE_GVARS : 0));
        cswitchOffset[i]->setDecimals(source.getDecimals(model));
        cswitchOffset[i]->setSingleStep(source.getStep(model));
        if (model.customSw[i].func>CS_FN_ELESS && model.customSw[i].func<CS_FN_VEQUAL) {
          cswitchOffset[i]->setMinimum(source.getStep(model)*-127);
          cswitchOffset[i]->setMaximum(source.getStep(model)*127);
          cswitchOffset[i]->setValue(source.getStep(model)*model.customSw[i].val2);
        } else {
          cswitchOffset[i]->setMinimum(source.getMin(model));
          cswitchOffset[i]->setMaximum(source.getMax(model));
          cswitchOffset[i]->setValue(source.getStep(model)*(model.customSw[i].val2+source.getRawOffset(model))+source.getOffset(model));
        }
        break;
      case CS_FAMILY_VBOOL:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(true);
        cswitchValue[i]->setVisible(false);
        cswitchOffset[i]->setVisible(false);
        populateSwitchCB(cswitchSource1[i], RawSwitch(model.customSw[i].val1));
        populateSwitchCB(cswitchSource2[i], RawSwitch(model.customSw[i].val2));
        break;
      case CS_FAMILY_VCOMP:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(true);
        cswitchValue[i]->setVisible(false);
        cswitchOffset[i]->setVisible(false);
        populateSourceCB(cswitchSource1[i], RawSource(model.customSw[i].val1), POPULATE_SOURCES | (GetEepromInterface()->getCapability(ExtraTrims) ? POPULATE_TRIMS : 0) | POPULATE_SWITCHES | POPULATE_TELEMETRY | (GetEepromInterface()->getCapability(GvarsInCS) ? POPULATE_GVARS : 0));
        populateSourceCB(cswitchSource2[i], RawSource(model.customSw[i].val2), POPULATE_SOURCES | (GetEepromInterface()->getCapability(ExtraTrims) ? POPULATE_TRIMS : 0) | POPULATE_SWITCHES | POPULATE_TELEMETRY | (GetEepromInterface()->getCapability(GvarsInCS) ? POPULATE_GVARS : 0));
        break;
      case CS_FAMILY_TIMERS:
        cswitchSource1[i]->setVisible(false);
        cswitchSource2[i]->setVisible(false);
        cswitchValue[i]->setVisible(true);
        cswitchOffset[i]->setVisible(true);
        cswitchOffset[i]->setDecimals(1);
        cswitchOffset[i]->setMinimum(0.1);
        cswitchOffset[i]->setMaximum(175);
        float value=ValToTim(model.customSw[i].val2);
        cswitchOffset[i]->setSingleStep(0.1);
        if (value>60) {
           cswitchOffset[i]->setSingleStep(1);
        } else if (value>2) {
          cswitchOffset[i]->setSingleStep(0.5);
        }
        cswitchOffset[i]->setValue(value);

        cswitchValue[i]->setDecimals(1);
        cswitchValue[i]->setMinimum(0.1);
        cswitchValue[i]->setMaximum(175);
        value=ValToTim(model.customSw[i].val1);
        cswitchValue[i]->setSingleStep(0.1);
        if (value>60) {
           cswitchValue[i]->setSingleStep(1);
        } else if (value>2) {
          cswitchValue[i]->setSingleStep(0.5);
        }
        cswitchValue[i]->setValue(value);
        break;
    }
    cswitchAnd[i]->setVisible(true);
    populateSwitchCB(cswitchAnd[i], RawSwitch(model.customSw[i].andsw), POPULATE_AND_SWITCHES);
    if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
      cswitchDuration[i]->setVisible(true);
      cswitchDuration[i]->setValue(model.customSw[i].duration/2.0);
      cswitchDelay[i]->setVisible(true);
      cswitchDelay[i]->setValue(model.customSw[i].delay/2.0);
    }
}

void CustomSwitchesPanel::update()
{
  lock = true;
  for (int i=0; i<GetEepromInterface()->getCapability(CustomSwitches); i++) {
    populateCSWCB(csw[i], model.customSw[i].func);
    setSwitchWidgetVisibility(i);
  }
  lock = false;
}

void CustomSwitchesPanel::cswPaste()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if (mimeData->hasFormat("application/x-companion9x-csw")) {
      QByteArray cswData = mimeData->data("application/x-companion9x-csw");

      CustomSwData *csw = &model.customSw[selectedSwitch];
      memcpy(csw, cswData.mid(0, sizeof(CustomSwData)).constData(), sizeof(CustomSwData));
      emit modified();
      updateSelectedSwitch();
    }
}

void CustomSwitchesPanel::cswDelete()
{
    model.customSw[selectedSwitch].clear();
    emit modified();
    updateSelectedSwitch();
}

void CustomSwitchesPanel::cswCopy()
{
    QByteArray cswData;
    cswData.append((char*)&model.customSw[selectedSwitch],sizeof(CustomSwData));
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion9x-csw", cswData);
    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void CustomSwitchesPanel::updateSelectedSwitch()
{
    lock = true;
    populateCSWCB(csw[selectedSwitch], model.customSw[selectedSwitch].func);
    setSwitchWidgetVisibility(selectedSwitch);
    lock = false;
}

void CustomSwitchesPanel::cswCut()
{
  cswCopy();
  cswDelete();
}

// TODO make something generic here!
void CustomSwitchesPanel::csw_customContextMenuRequested(QPoint pos)
{
    QLabel *label = (QLabel *)sender();
    selectedSwitch = label->property("index").toInt();

    QPoint globalPos = label->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion9x-csw");

    QMenu contextMenu;
    contextMenu.addAction(QIcon(":/images/clear.png"), tr("&Delete"),this,SLOT(cswDelete()),tr("Delete"));
    contextMenu.addAction(QIcon(":/images/copy.png"), tr("&Copy"),this,SLOT(cswCopy()),tr("Ctrl+C"));
    contextMenu.addAction(QIcon(":/images/cut.png"), tr("&Cut"),this,SLOT(cswCut()),tr("Ctrl+X"));
    contextMenu.addAction(QIcon(":/images/paste.png"), tr("&Paste"),this,SLOT(cswPaste()),tr("Ctrl+V"))->setEnabled(hasData);

    contextMenu.exec(globalPos);
}
