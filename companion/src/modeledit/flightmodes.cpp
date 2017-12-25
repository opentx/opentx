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

#include "flightmodes.h"
#include "ui_flightmode.h"
#include "switchitemmodel.h"
#include "helpers.h"
#include "customdebug.h"

FlightModePanel::FlightModePanel(QWidget * parent, ModelData & model, int phaseIdx, GeneralSettings & generalSettings, Firmware * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  ui(new Ui::FlightMode),
  phaseIdx(phaseIdx),
  phase(model.flightModeData[phaseIdx]),
  reCount(firmware->getCapability(RotaryEncoders)),
  gvCount(firmware->getCapability(Gvars)),
  rawSwitchItemModel(NULL)
{
  ui->setupUi(this);

  ui->labelName->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->labelName, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(name_customContextMenuRequested(const QPoint &)));

  int modesCount = firmware->getCapability(FlightModes);

  // Phase name
  QRegExp rx(CHAR_FOR_NAMES_REGEX);
  if (modesCount) {
    ui->name->setValidator(new QRegExpValidator(rx, this));
    ui->name->setMaxLength(firmware->getCapability(FlightModesName));
    connect(ui->name, SIGNAL(editingFinished()), this, SLOT(phaseName_editingFinished()));
  }
  else {
    ui->name->setDisabled(true);
  }

  // Phase switch
  if (phaseIdx > 0) {
    rawSwitchItemModel = new RawSwitchFilterItemModel(&generalSettings, &model, MixesContext);
    ui->swtch->setModel(rawSwitchItemModel);
    ui->swtch->setCurrentIndex(ui->swtch->findData(phase.swtch.toValue()));
    connect(ui->swtch, SIGNAL(currentIndexChanged(int)), this, SLOT(phaseSwitch_currentIndexChanged(int)));
  }
  else {
    ui->swtch->hide();
  }

  // FadeIn / FadeOut
  if (firmware->getCapability(FlightModesHaveFades)) {
    int scale = firmware->getCapability(SlowScale);
    int range = firmware->getCapability(SlowRange);
    ui->fadeIn->setMaximum(float(range)/scale);
    ui->fadeIn->setSingleStep(1.0/scale);
    ui->fadeIn->setDecimals((scale==1 ? 0 :1) );
    connect(ui->fadeIn, SIGNAL(editingFinished()), this, SLOT(phaseFadeIn_editingFinished()));
    ui->fadeOut->setMaximum(float(range)/scale);
    ui->fadeOut->setSingleStep(1.0/scale);
    ui->fadeOut->setDecimals((scale==1 ? 0 :1));
    connect(ui->fadeOut, SIGNAL(editingFinished()), this, SLOT(phaseFadeOut_editingFinished()));
  }
  else {
    ui->fadeIn->setDisabled(true);
    ui->fadeOut->setDisabled(true);
  }

  // The trims
  QString labels[CPN_MAX_STICKS];
  for(int i=0; i < CPN_MAX_STICKS; i++) {
    labels[i] = firmware->getAnalogInputName(i);
  }


  trimsLabel << ui->trim1Label << ui->trim2Label << ui->trim3Label << ui->trim4Label;
  trimsUse << ui->trim1Use << ui->trim2Use << ui->trim3Use << ui->trim4Use;
  trimsValue << ui->trim1Value << ui->trim2Value << ui->trim3Value << ui->trim4Value;
  trimsSlider << ui->trim1Slider << ui->trim2Slider << ui->trim3Slider << ui->trim4Slider;

  Board::Type board = firmware->getBoard();

  for (int i=0; i<4; i++) {
    trimsLabel[i]->setText(labels[CONVERT_MODE(i+1)-1]);

    QComboBox * cb = trimsUse[i];
    cb->setProperty("index", i);
    if (IS_HORUS_OR_TARANIS(board)) {
      cb->addItem(tr("Trim disabled"), -1);
    }
    for (int m=0; m<modesCount; m++) {
      if (m == phaseIdx) {
        cb->addItem(tr("Own Trim"), m*2);
      }
      else if (phaseIdx > 0) {
        cb->addItem(tr("Use Trim from Flight mode %1").arg(m), m*2);
        if (IS_HORUS_OR_TARANIS(board)) {
          cb->addItem(tr("Use Trim from Flight mode %1 + Own Trim as an offset").arg(m), m*2+1);
        }
      }
    }
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(phaseTrimUse_currentIndexChanged(int)));

    trimsValue[i]->setProperty("index", i);
    connect(trimsValue[i], SIGNAL(valueChanged(int)), this, SLOT(phaseTrim_valueChanged()));

    trimsSlider[i]->setProperty("index", i);
    connect(trimsSlider[i], SIGNAL(valueChanged(int)), this, SLOT(phaseTrimSlider_valueChanged()));
  }

  // Rotary encoders
  if (reCount > 0) {
    QGridLayout *reLayout = new QGridLayout(ui->reGB);
    for (int i=0; i<reCount; i++) {
      // RE label
      QLabel *label = new QLabel(ui->reGB);
      label->setText(tr("Rotary Encoder %1").arg(i+1));
      reLayout->addWidget(label, i, 0, 1, 1);
      if (phaseIdx > 0) {
        // RE link to another RE
        reUse[i] = new QComboBox(ui->reGB);
        reUse[i]->setProperty("index", i);
        Helpers::populateGvarUseCB(reUse[i], phaseIdx);
        if (phase.rotaryEncoders[i] > 1024) {
          reUse[i]->setCurrentIndex(phase.rotaryEncoders[i] - 1024);
        }
        connect(reUse[i], SIGNAL(currentIndexChanged(int)), this, SLOT(phaseREUse_currentIndexChanged(int)));
        reLayout->addWidget(reUse[i], i, 1, 1, 1);
      }
      // RE value
      reValues[i] = new QSpinBox(ui->reGB);
      reValues[i]->setProperty("index", i);
      reValues[i]->setMinimum(-1024);
      reValues[i]->setMaximum(1024);
      connect(reValues[i], SIGNAL(editingFinished()), this, SLOT(phaseREValue_editingFinished()));
      reLayout->addWidget(reValues[i], i, 2, 1, 1);
    }
  }
  else {
    ui->reGB->hide();
  }

  // GVars
  if (gvCount > 0 && (firmware->getCapability(GvarsFlightModes) || phaseIdx == 0) ) {
    QGridLayout *gvLayout = new QGridLayout(ui->gvGB);

    // Column headers
    int headerCol = 1;

    if (firmware->getCapability(GvarsName)) {
      QLabel *nameLabel = new QLabel(ui->gvGB);
      nameLabel->setText(tr("Name"));
      gvLayout->addWidget(nameLabel, 0, headerCol++, 1, 1);
    }

    if (phaseIdx > 0) {
      QLabel *sourceLabel = new QLabel(ui->gvGB);
      sourceLabel->setText(tr("Value source"));
      gvLayout->addWidget(sourceLabel, 0, headerCol++, 1, 1);
    }

    QLabel *valueLabel = new QLabel(ui->gvGB);
    valueLabel->setText(tr("Value"));
    gvLayout->addWidget(valueLabel, 0, headerCol++, 1, 1);

    if (IS_HORUS_OR_TARANIS(board) && phaseIdx == 0) {
      QLabel *unitLabel = new QLabel(ui->gvGB);
      unitLabel->setText(tr("Unit"));
      gvLayout->addWidget(unitLabel, 0, headerCol++, 1, 1);

      QLabel *precLabel = new QLabel(ui->gvGB);
      precLabel->setText(tr("Prec"));
      gvLayout->addWidget(precLabel, 0, headerCol++, 1, 1);

      QLabel *minLabel = new QLabel(ui->gvGB);
      minLabel->setText(tr("Min"));
      gvLayout->addWidget(minLabel, 0, headerCol++, 1, 1);

      QLabel *maxLabel = new QLabel(ui->gvGB);
      maxLabel->setText(tr("Max"));
      gvLayout->addWidget(maxLabel, 0, headerCol++, 1, 1);

      QLabel *popupLabel = new QLabel(ui->gvGB);
      popupLabel->setText(tr("Popup enabled"));
      gvLayout->addWidget(popupLabel, 0, headerCol++, 1, 1);
    }

    for (int i=0; i<gvCount; i++) {
      int col = 0;
      // GVar label
      QLabel *label = new QLabel(ui->gvGB);
      label->setText(tr("GVAR%1").arg(i+1));
      label->setProperty("index", i);
      label->setContextMenuPolicy(Qt::CustomContextMenu);
      connect(label, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(gvLabel_customContextMenuRequested(const QPoint &)));
      gvLayout->addWidget(label, i+1, col++, 1, 1);
      // GVar name
      int nameLen = firmware->getCapability(GvarsName);
      if (nameLen > 0) {
        gvNames[i] = new QLineEdit(ui->gvGB);
        gvNames[i]->setProperty("index", i);
        gvNames[i]->setMaxLength(nameLen);
        connect(gvNames[i], SIGNAL(editingFinished()), this, SLOT(GVName_editingFinished()));
        gvLayout->addWidget(gvNames[i], i+1, col++, 1, 1);
      }
      if (phaseIdx > 0) {
        // GVar link to another GVar
        gvUse[i] = new QComboBox(ui->gvGB);
        gvUse[i]->setProperty("index", i);
        Helpers::populateGvarUseCB(gvUse[i], phaseIdx);
        if (phase.gvars[i] > GVAR_MAX_VALUE) {
          gvUse[i]->setCurrentIndex(phase.gvars[i] - GVAR_MAX_VALUE);
        }
        connect(gvUse[i], SIGNAL(currentIndexChanged(int)), this, SLOT(phaseGVUse_currentIndexChanged(int)));
        gvLayout->addWidget(gvUse[i], i+1, col++, 1, 1);
      }
      // GVar value
      gvValues[i] = new QDoubleSpinBox(ui->gvGB);
      gvValues[i]->setProperty("index", i);
      connect(gvValues[i], SIGNAL(editingFinished()), this, SLOT(phaseGVValue_editingFinished()));
      gvLayout->addWidget(gvValues[i], i+1, col++, 1, 1);

      if (IS_HORUS_OR_TARANIS(board) && phaseIdx == 0) {
        // GVar unit
        gvUnit[i] = new QComboBox(ui->gvGB);
        gvUnit[i]->setProperty("index", i);
        populateGvarUnitCB(gvUnit[i]);
        connect(gvUnit[i], SIGNAL(currentIndexChanged(int)), this, SLOT(phaseGVUnit_currentIndexChanged(int)));
        gvLayout->addWidget(gvUnit[i], i+1, col++, 1, 1);

        // GVar precision
        gvPrec[i] = new QComboBox(ui->gvGB);
        gvPrec[i]->setProperty("index", i);
        populateGvarPrecCB(gvPrec[i]);
        connect(gvPrec[i], SIGNAL(currentIndexChanged(int)), this, SLOT(phaseGVPrec_currentIndexChanged(int)));
        gvLayout->addWidget(gvPrec[i], i+1, col++, 1, 1);

        // GVar min
        gvMin[i] = new QDoubleSpinBox(ui->gvGB);
        gvMin[i]->setProperty("index", i);
        connect(gvMin[i], SIGNAL(editingFinished()), this, SLOT(phaseGVMin_editingFinished()));
        gvLayout->addWidget(gvMin[i], i+1, col++, 1, 1);

        // GVar max
        gvMax[i] = new QDoubleSpinBox(ui->gvGB);
        gvMax[i]->setProperty("index", i);
        connect(gvMax[i], SIGNAL(editingFinished()), this, SLOT(phaseGVMax_editingFinished()));
        gvLayout->addWidget(gvMax[i], i+1, col++, 1, 1);

        // Popups
        gvPopups[i] = new QCheckBox(ui->gvGB);
        gvPopups[i]->setProperty("index", i);
        //gvPopups[i]->setText(tr("Popup enabled"));
        connect(gvPopups[i], SIGNAL(toggled(bool)), this, SLOT(phaseGVPopupToggled(bool)));
        gvLayout->addWidget(gvPopups[i], i+1, col++, 1, 1);
      }
    }
  }
  else {
    ui->gvGB->hide();
    gvCount = 0;
  }

  disableMouseScrolling();

  update();
}

FlightModePanel::~FlightModePanel()
{
  delete ui;
}

void FlightModePanel::update()
{
  if (rawSwitchItemModel) {
    rawSwitchItemModel->update();
  }

  ui->name->setText(phase.name);

  int scale = firmware->getCapability(SlowScale);
  ui->fadeIn->setValue(float(phase.fadeIn)/scale);
  ui->fadeOut->setValue(float(phase.fadeOut)/scale);

  for (int i=0; i<4; i++) {
    int trimsMax = firmware->getCapability(ExtendedTrimsRange);
    if (trimsMax == 0 || !model->extendedTrims) {
      trimsMax = firmware->getCapability(TrimsRange);
    }
    trimsSlider[i]->setRange(-trimsMax, +trimsMax);
    trimsValue[i]->setRange(-trimsMax, +trimsMax);
    int chn = CONVERT_MODE(i+1)-1;
    if (chn == 2/*TODO constant*/ && model->throttleReversed)
      trimsSlider[i]->setInvertedAppearance(true);
    trimUpdate(i);
  }

  for (int i=0; i<gvCount; i++) {
    updateGVar(i);
  }

  for (int i=0; i<reCount; i++) {
    reValues[i]->setDisabled(false);
    int idx = phase.rotaryEncoders[i];
    FlightModeData *phasere = &phase;
    while (idx > 1024) {
      int nextPhase = idx - 1025;
      if (nextPhase >= phaseIdx) nextPhase += 1;
      phasere = &model->flightModeData[nextPhase];
      idx = phasere->rotaryEncoders[i];
      reValues[i]->setDisabled(true);
    }
    reValues[i]->setValue(phasere->rotaryEncoders[i]);
  }
}
void FlightModePanel::updateGVar(int index)
{
  lock = true;
  if (firmware->getCapability(GvarsName) > 0) {
    gvNames[index]->setText(model->gvarData[index].name);
  }
  gvValues[index]->setDisabled(model->isGVarLinked(phaseIdx, index));
  setGVSB(gvValues[index], model->gvarData[index].getMin(), model->gvarData[index].getMax(), model->getGVarFieldValue(phaseIdx, index));
  if (IS_HORUS_OR_TARANIS(getCurrentBoard()) && phaseIdx == 0) {
    gvUnit[index]->setCurrentIndex(model->gvarData[index].unit);
    gvPrec[index]->setCurrentIndex(model->gvarData[index].prec);
    setGVSB(gvMin[index], GVAR_MIN_VALUE, model->gvarData[index].getMax(), model->gvarData[index].getMin());
    setGVSB(gvMax[index], model->gvarData[index].getMin(), GVAR_MAX_VALUE, model->gvarData[index].getMax());
    gvPopups[index]->setChecked(model->gvarData[index].popup);
  }
  lock = false;
}

void FlightModePanel::setGVSB(QDoubleSpinBox * sb, int min, int max, int val)
{
  int idx = sb->property("index").toInt();
  float mul = model->gvarData[idx].multiplierGet();
  sb->setDecimals(model->gvarData[idx].prec);
  sb->setSingleStep(mul);
  sb->setSuffix(model->gvarData[idx].unitToString());
  sb->setMinimum(min * mul);
  sb->setMaximum(max * mul);
  sb->setValue(val * mul);
}

// TODO: pull units from GVarData
void FlightModePanel::populateGvarUnitCB(QComboBox * cb)
{
  cb->clear();
  cb->addItem("");
  cb->addItem("%");
}

// TODO: pull precision string from GVarData
void FlightModePanel::populateGvarPrecCB(QComboBox * cb)
{
  cb->clear();
  cb->addItem(tr("0._"));
  cb->addItem(tr("0.0"));
}

void FlightModePanel::phaseName_editingFinished()
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
    strcpy(phase.name, lineEdit->text().toLatin1());
    emit modified();
    emit nameModified();
}

void FlightModePanel::phaseSwitch_currentIndexChanged(int index)
{
  if (!lock) {
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    phase.swtch = RawSwitch(comboBox->itemData(index).toInt());
    emit modified();
  }
}

void FlightModePanel::phaseFadeIn_editingFinished()
{
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int scale = firmware->getCapability(SlowScale);
    phase.fadeIn = round(spinBox->value()*scale);
    emit modified();
}

void FlightModePanel::phaseFadeOut_editingFinished()
{
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int scale = firmware->getCapability(SlowScale);
    phase.fadeOut = round(spinBox->value()*scale);
    emit modified();
}

void FlightModePanel::trimUpdate(unsigned int trim)
{
  lock = true;
  int chn = CONVERT_MODE(trim+1)-1;
  int value = model->getTrimValue(phaseIdx, chn);
  trimsSlider[trim]->setValue(value);
  trimsValue[trim]->setValue(value);
  if (phase.trimMode[chn] < 0) {
    trimsUse[trim]->setCurrentIndex(0);
    trimsValue[trim]->setEnabled(false);
    trimsSlider[trim]->setEnabled(false);
  }
  else {
    Board::Type board = firmware->getBoard();
    if (IS_HORUS_OR_TARANIS(board))
      trimsUse[trim]->setCurrentIndex(1 + 2*phase.trimRef[chn] + phase.trimMode[chn] - (phase.trimRef[chn] > phaseIdx ? 1 : 0));
    else
      trimsUse[trim]->setCurrentIndex(phase.trimRef[chn]);
    if (phaseIdx == 0 || phase.trimRef[chn] == phaseIdx || (IS_HORUS_OR_TARANIS(board) && phase.trimMode[chn] != 0)) {
      trimsValue[trim]->setEnabled(true);
      trimsSlider[trim]->setEnabled(true);
    }
    else {
      trimsValue[trim]->setEnabled(false);
      trimsSlider[trim]->setEnabled(false);
    }
  }
  lock = false;
}

void FlightModePanel::phaseGVValue_editingFinished()
{
  if (!lock) {
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int gvar = spinBox->property("index").toInt();
    phase.gvars[gvar] = spinBox->value() * model->gvarData[gvar].multiplierSet();
    emit modified();
  }
}

void FlightModePanel::GVName_editingFinished()
{
  if (!lock) {
    QLineEdit *lineedit = qobject_cast<QLineEdit*>(sender());
    int gvar = lineedit->property("index").toInt();
    memset(&model->gvarData[gvar].name, 0, sizeof(model->gvarData[gvar].name));
    strcpy(model->gvarData[gvar].name, lineedit->text().toLatin1());
    emit modified();
  }
}

void FlightModePanel::phaseGVUse_currentIndexChanged(int index)
{
  if (!lock) {
    lock = true;
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int gvar = comboBox->property("index").toInt();
    if (index == 0) {
      phase.gvars[gvar] = 0;
    }
    else {
      phase.gvars[gvar] = GVAR_MAX_VALUE + index;
    }
    updateGVar(gvar);
    emit modified();
    lock = false;
  }
}

void FlightModePanel::phaseGVUnit_currentIndexChanged(int index)
{
  if (!lock) {
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int gvar = comboBox->property("index").toInt();
    model->gvarData[gvar].unit = index;
    emit modified();
    updateGVar(gvar);
  }
}

void FlightModePanel::phaseGVPrec_currentIndexChanged(int index)
{
  if (!lock) {
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int gvar = comboBox->property("index").toInt();
    model->gvarData[gvar].prec = index;
    emit modified();
    updateGVar(gvar);
  }
}

void FlightModePanel::phaseGVMin_editingFinished()
{
  if (!lock) {
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int gvar = spinBox->property("index").toInt();
    model->gvarData[gvar].setMin(spinBox->value());
    if (!model->isGVarLinked(phaseIdx, gvar)) {
      if (model->getGVarFieldValuePrec(phaseIdx, gvar) < spinBox->value()) {
        phase.gvars[gvar] = model->gvarData[gvar].getMin();
      }
    }
    for (int x=1; x<firmware->getCapability(FlightModes); x++) {
      if (!model->isGVarLinked(x, gvar)) {
        if (model->flightModeData[x].gvars[gvar] < model->gvarData[gvar].getMin()) {
          model->flightModeData[x].gvars[gvar] = model->gvarData[gvar].getMin();
        }
      }
    }
    emit modified();
    updateGVar(gvar);
  }
}

void FlightModePanel::phaseGVMax_editingFinished()
{
  if (!lock) {
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int gvar = spinBox->property("index").toInt();
    model->gvarData[gvar].setMax(spinBox->value());
    if (!model->isGVarLinked(phaseIdx, gvar)) {
      if (model->getGVarFieldValuePrec(phaseIdx, gvar) > spinBox->value()) {
        phase.gvars[gvar] = model->gvarData[gvar].getMax();
      }
    }
    for (int x=1; x<firmware->getCapability(FlightModes); x++) {
      if (!model->isGVarLinked(x, gvar)) {
        if (model->flightModeData[x].gvars[gvar] > model->gvarData[gvar].getMax()) {
          model->flightModeData[x].gvars[gvar] = model->gvarData[gvar].getMax();
        }
      }
    }
    emit modified();
    updateGVar(gvar);
  }
}

void FlightModePanel::phaseGVPopupToggled(bool checked)
{
  if (!lock) {
    QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
    int gvar = cb->property("index").toInt();
    model->gvarData[gvar].popup = checked;
    emit modified();
  }
}

void FlightModePanel::phaseREValue_editingFinished()
{
  if (!lock) {
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    int re = spinBox->property("index").toInt();
    phase.rotaryEncoders[re] = spinBox->value();
    emit modified();
  }
}

void FlightModePanel::phaseREUse_currentIndexChanged(int index)
{
  if (!lock) {
    lock = true;
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int re = comboBox->property("index").toInt();
    if (index == 0) {
      phase.rotaryEncoders[re] = 0;
    }
    else {
      phase.rotaryEncoders[re] = 1024 + index;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    ;
    }
    update();
    lock = false;
    emit modified();
  }
}

void FlightModePanel::phaseTrimUse_currentIndexChanged(int index)
{
  if (!lock) {
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int trim = comboBox->property("index").toInt();
    int chn = CONVERT_MODE(trim+1)-1;
    int data = comboBox->itemData(index).toInt();
    if (data < 0) {
      phase.trimMode[chn] = -1;
      phase.trimRef[chn] = 0;
      phase.trim[chn] = 0;
    }
    else {
      Board::Type board = firmware->getBoard();
      if (IS_HORUS_OR_TARANIS(board)) {
        phase.trimMode[chn] = data % 2;
        phase.trimRef[chn] = data / 2;
      }
      else {
        phase.trimMode[chn] = 0;
        phase.trimRef[chn] = data / 2;
      }
      phase.trim[chn] = 0;
    }
    trimUpdate(trim);
    emit modified();
  }
}

void FlightModePanel::phaseTrim_valueChanged()
{
  if (!lock) {
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    int trim = spinBox->property("index").toInt();
    int chn = CONVERT_MODE(trim+1)-1;
    int value = spinBox->value();
    model->setTrimValue(phaseIdx, chn, value);
    lock = true;
    trimsSlider[trim]->setValue(value);
    lock = false;
    emit modified();
  }
}

void FlightModePanel::phaseTrimSlider_valueChanged()
{
  if (!lock) {
    QSlider *slider = qobject_cast<QSlider*>(sender());
    int trim = slider->property("index").toInt();
    int chn = CONVERT_MODE(trim+1)-1;
    int value = slider->value();
    model->setTrimValue(phaseIdx, chn, value);
    lock = true;
    trimsValue[trim]->setValue(value);
    lock = false;
    emit modified();
  }
}

void FlightModePanel::name_customContextMenuRequested(const QPoint & pos)
{
  QLabel *label = (QLabel *)sender();
  QPoint globalPos = label->mapToGlobal(pos);
  QMenu contextMenu;
  contextMenu.addAction(CompanionIcon("clear.png"), tr("&Clear"),this,SLOT(fmClear()),tr("Clear"));
  contextMenu.exec(globalPos);
}

void FlightModePanel::fmClear()
{
  int res = QMessageBox::question(this, CPN_STR_APP_NAME, tr("Clear all current Flight Mode properties?"), QMessageBox::Yes | QMessageBox::No);
  if (res == QMessageBox::Yes) {
    phase.clear(phaseIdx);
    if (phaseIdx == 0) {
      if (IS_HORUS_OR_TARANIS(getCurrentBoard())) {
        for (int i=0; i < gvCount; ++i) {
          model->gvarData[i].clear();
        }
      }
    }
    else {
      lock = true;
      QComboBox * pswtch = ui->swtch;
      RawSwitch item = RawSwitch(SWITCH_TYPE_NONE);
      pswtch->setCurrentIndex(pswtch->findText(item.toString()));
      if (gvCount > 0 && (firmware->getCapability(GvarsFlightModes))) {
        for (int i=0; i<gvCount; i++) {
          gvUse[i]->setCurrentIndex((phase.gvars[i] > GVAR_MAX_VALUE ? (phase.gvars[i] - GVAR_MAX_VALUE) : 0));
        }
      }
      for (int i=0; i<reCount; i++) {
        reUse[i]->setCurrentIndex((phase.rotaryEncoders[i] > GVAR_MAX_VALUE ? (phase.rotaryEncoders[i] - GVAR_MAX_VALUE) : 0));
      }
      lock = false;
    }
    update();
    emit modified();
    emit nameModified();
  }
}

void FlightModePanel::gvLabel_customContextMenuRequested(const QPoint & pos)
{
  QLabel *label = (QLabel *)sender();
  gvIdx = label->property("index").toInt();
  QPoint globalPos = label->mapToGlobal(pos);
  QMenu contextMenu;
  contextMenu.addAction(CompanionIcon("clear.png"), tr("&Clear"),this,SLOT(gvClear()),tr("Clear"));
  contextMenu.exec(globalPos);
}

void FlightModePanel::gvClear()
{
  if (phaseIdx == 0) {
    if (IS_HORUS_OR_TARANIS(getCurrentBoard())) {
      model->gvarData[gvIdx].clear();
      phase.gvars[gvIdx] = 0;
    }
  }
  else {
    lock = true;
    if (gvCount > 0 && (firmware->getCapability(GvarsFlightModes))) {
      phase.gvars[gvIdx] = GVAR_MAX_VALUE + 1;
      gvUse[gvIdx]->setCurrentIndex(1);
    }
    lock = false;
  }
  update();
  emit modified();
}

/**********************************************************/

FlightModesPanel::FlightModesPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  modesCount(firmware->getCapability(FlightModes))
{
  QGridLayout * gridLayout = new QGridLayout(this);
  tabWidget = new QTabWidget(this);
  for (int i=0; i<modesCount; i++) {
    FlightModePanel * tab = new FlightModePanel(tabWidget, model, i, generalSettings, firmware);
    tab->setProperty("index", i);
    panels << tab;
    connect(tab, SIGNAL(modified()), this, SLOT(onPhaseModified()));
    connect(tab, SIGNAL(nameModified()), this, SLOT(onPhaseNameChanged()));
    tabWidget->addTab(tab, getTabName(i));
  }
  gridLayout->addWidget(tabWidget, 0, 0, 1, 1);
  connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_tabWidget_currentChanged(int)));
}

FlightModesPanel::~FlightModesPanel()
{
}

void FlightModesPanel::onPhaseModified()
{
  emit modified();
}

QString FlightModesPanel::getTabName(int index)
{
  QString result = tr("Flight Mode %1").arg(index);
  const char *name = model->flightModeData[index].name;
  if (firmware->getCapability(FlightModesName) && strlen(name) > 0) {
    result += tr(" (%1)").arg(name);
  }
  else if (index == 0) {
    result += tr(" (default)");
  }

  return result;
}

void FlightModesPanel::onPhaseNameChanged()
{
  int index = sender()->property("index").toInt();
  tabWidget->setTabText(index, getTabName(index));
}

void FlightModesPanel::update()
{
  on_tabWidget_currentChanged(tabWidget->currentIndex());
}

void FlightModesPanel::on_tabWidget_currentChanged(int index)
{
  panels[index]->update();
}
