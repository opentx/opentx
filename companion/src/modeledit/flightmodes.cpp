#include "flightmodes.h"
#include "ui_flightmode.h"
// #include "firmwares/opentx/opentxeeprom.h" // TODO shouldn't be there
#include "helpers.h"
#include <QComboBox>
#include <QGridLayout>

FlightMode::FlightMode(QWidget * parent, ModelData & model, int phaseIdx, GeneralSettings & generalSettings):
  ModelPanel(parent, model),
  ui(new Ui::FlightMode),
  generalSettings(generalSettings),
  phaseIdx(phaseIdx),
  phase(model.phaseData[phaseIdx]),
  reCount(GetEepromInterface()->getCapability(RotaryEncoders)),
  gvCount(((!GetEepromInterface()->getCapability(HasVariants)) || (GetCurrentFirmwareVariant() & GVARS_VARIANT)) ?
      GetEepromInterface()->getCapability(Gvars) : 0)
{
  ui->setupUi(this);

  int modesCount = GetEepromInterface()->getCapability(FlightPhases);

  // Phase name
  QRegExp rx(CHAR_FOR_NAMES_REGEX);
  if (modesCount) {
    ui->name->setValidator(new QRegExpValidator(rx, this));
    ui->name->setMaxLength(GetEepromInterface()->getCapability(FlightModesName));
    connect(ui->name, SIGNAL(editingFinished()), this, SLOT(phaseName_editingFinished()));
  }
  else {
    ui->name->setDisabled(true);
  }

  // Phase switch
  if (phaseIdx > 0) {
    populateSwitchCB(ui->swtch, phase.swtch);
    connect(ui->swtch, SIGNAL(currentIndexChanged(int)), this, SLOT(phaseSwitch_currentIndexChanged(int)));
  }
  else {
    ui->swtch->hide();
  }

  // FadeIn / FadeOut
  if (GetEepromInterface()->getCapability(FlightPhasesHaveFades)) {
    int scale = GetEepromInterface()->getCapability(SlowScale);
    int range = GetEepromInterface()->getCapability(SlowRange);
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
  QString labels[] = { tr("Rud"), tr("Ele"), tr("Thr"), tr("Ail") }; // TODO is elsewhere for sure

  trimsLabel << ui->trim1Label << ui->trim2Label << ui->trim3Label << ui->trim4Label;
  trimsUse << ui->trim1Use << ui->trim2Use << ui->trim3Use << ui->trim4Use;
  trimsValue << ui->trim1Value << ui->trim2Value << ui->trim3Value << ui->trim4Value;
  trimsSlider << ui->trim1Slider << ui->trim2Slider << ui->trim3Slider << ui->trim4Slider;

  for (int i=0; i<4; i++) {
    trimsLabel[i]->setText(labels[CONVERT_MODE(i+1)-1]);

    QComboBox * cb = trimsUse[i];
    cb->setProperty("index", i);
    cb->addItem(QObject::tr("Trim disabled"), -1);
    for (int m=0; m<modesCount; m++) {
      if (m == phaseIdx) {
        cb->addItem(QObject::tr("Own Trim"), m*2);
      }
      else if (phaseIdx > 0) {
        cb->addItem(QObject::tr("Use Trim from Flight mode %1").arg(m), m*2);
        cb->addItem(QObject::tr("Use Trim from Flight mode %1 + Own Trim as an offset").arg(m), m*2+1);
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
        QComboBox *link = new QComboBox(ui->gvGB);
        link->setProperty("index", i);
        populateGvarUseCB(link, phaseIdx);
        connect(link, SIGNAL(currentIndexChanged(int)), this, SLOT(phaseREUse_currentIndexChanged(int)));
        if (phase.rotaryEncoders[i] > 1024) {
          link->setCurrentIndex(phase.rotaryEncoders[i] - 1024);
        }
        reLayout->addWidget(link, i, 1, 1, 1);
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
  if (gvCount > 0) {
    QGridLayout *gvLayout = new QGridLayout(ui->gvGB);
    for (int i=0; i<gvCount; i++) {
      int col = 0;
      // GVar label
      QLabel *label = new QLabel(ui->gvGB);
      label->setText(tr("GVAR%1").arg(i+1));
      gvLayout->addWidget(label, i, col++, 1, 1);
      // GVar name
      int nameLen = GetEepromInterface()->getCapability(GvarsName);
      if (nameLen > 0) {
        gvNames[i] = new QLineEdit(ui->gvGB);
        gvNames[i]->setProperty("index", i);
        gvNames[i]->setMaxLength(nameLen);
        connect(gvNames[i], SIGNAL(editingFinished()), this, SLOT(GVName_editingFinished()));
        gvLayout->addWidget(gvNames[i], i, col++, 1, 1);
      }
      if (phaseIdx > 0) {
        // GVar link to another GVar
        QComboBox *link = new QComboBox(ui->gvGB);
        link->setProperty("index", i);
        populateGvarUseCB(link, phaseIdx);
        if (phase.gvars[i] > 1024) {
          link->setCurrentIndex(phase.gvars[i] - 1024);
        }
        connect(link, SIGNAL(currentIndexChanged(int)), this, SLOT(phaseGVUse_currentIndexChanged(int)));
        gvLayout->addWidget(link, i, col++, 1, 1);
      }
      // GVar value
      gvValues[i] = new QSpinBox(ui->gvGB);
      gvValues[i]->setProperty("index", i);
      connect(gvValues[i], SIGNAL(editingFinished()), this, SLOT(phaseGVValue_editingFinished()));
      gvValues[i]->setMinimum(-1024);
      gvValues[i]->setMaximum(1024);
      gvLayout->addWidget(gvValues[i], i, col++, 1, 1);
    }
  }
  else {
    ui->gvGB->hide();
  }

  update();
}

FlightMode::~FlightMode()
{
  delete ui;
}

void FlightMode::update()
{
  ui->name->setText(phase.name);

  int scale = GetEepromInterface()->getCapability(SlowScale);
  ui->fadeIn->setValue(float(phase.fadeIn)/scale);
  ui->fadeOut->setValue(float(phase.fadeOut)/scale);

  for (int i=0; i<4; i++) {
    int trimsMax = GetEepromInterface()->getCapability(ExtendedTrims);
    if (trimsMax == 0 || !model.extendedTrims) {
      trimsMax = 125;
    }
    trimsSlider[i]->setRange(-trimsMax, +trimsMax);
    trimsValue[i]->setRange(-trimsMax, +trimsMax);
    int chn = CONVERT_MODE(i+1)-1;
    if (chn == 2/*TODO constant*/ && model.throttleReversed)
      trimsSlider[i]->setInvertedAppearance(true);
    trimUpdate(i);
  }

  for (int i=0; i<gvCount; i++) {
    gvNames[i]->setText(model.gvars_names[i]);
    gvValues[i]->setDisabled(false);
    int idx = phase.gvars[i];
    PhaseData *phasegvar = &phase;
    while (idx >= 1024) {
      idx -= 1025;
      phasegvar = &model.phaseData[idx];
    	idx = phasegvar->gvars[i];
    	gvValues[i]->setDisabled(true);
    }
    gvValues[i]->setValue(phasegvar->gvars[i]);
  }

  for (int i=0; i<reCount; i++) {    
    reValues[i]->setDisabled(false);
    int idx = phase.rotaryEncoders[i];
    PhaseData *phasere = &phase;
    while (idx >= 1024) {
      idx -= 1025;
      phasere = &model.phaseData[idx];
      idx = phasere->rotaryEncoders[i];
      reValues[i]->setDisabled(true);
    }
    reValues[i]->setValue(phasere->rotaryEncoders[i]);
  }
}

void FlightMode::phaseName_editingFinished()
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
    strcpy(phase.name, lineEdit->text().toAscii());
    emit modified();
    emit nameModified();
}

void FlightMode::phaseSwitch_currentIndexChanged(int index)
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    phase.swtch = RawSwitch(comboBox->itemData(index).toInt());
    emit modified();
}

void FlightMode::phaseFadeIn_editingFinished()
{
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int scale = GetEepromInterface()->getCapability(SlowScale);
    phase.fadeIn = round(spinBox->value()*scale);
    emit modified();
}

void FlightMode::phaseFadeOut_editingFinished()
{
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int scale = GetEepromInterface()->getCapability(SlowScale);
    phase.fadeOut = round(spinBox->value()*scale);
    emit modified();
}

void FlightMode::trimUpdate(unsigned int trim)
{
  lock = true;
  int chn = CONVERT_MODE(trim+1)-1;
  int value = model.getTrimValue(phaseIdx, chn);
  trimsSlider[trim]->setValue(value);
  trimsValue[trim]->setValue(value);
  if (phase.trimMode[chn] < 0) {
    trimsUse[trim]->setCurrentIndex(0);
    trimsValue[trim]->setEnabled(false);
    trimsSlider[trim]->setEnabled(false);
  }
  else {
    trimsUse[trim]->setCurrentIndex(1 + 2*phase.trimRef[chn] + phase.trimMode[chn] - (phase.trimRef[chn] > phaseIdx ? 1 : 0));
    trimsValue[trim]->setEnabled(true);
    trimsSlider[trim]->setEnabled(true);
  }
  lock = false;
}

void FlightMode::phaseGVValue_editingFinished()
{
  if (!lock) {
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    int gvar = spinBox->property("index").toInt();
    phase.gvars[gvar] = spinBox->value();
    emit modified();
  }
}

void FlightMode::GVName_editingFinished()
{
  if (!lock) {
    QLineEdit *lineedit = qobject_cast<QLineEdit*>(sender());
    int gvar = lineedit->property("index").toInt();
    memset(&model.gvars_names[gvar], 0, sizeof(model.gvars_names[gvar]));
    strcpy(model.gvars_names[gvar], lineedit->text().toAscii());
    emit modified();
  }
}

void FlightMode::GVSource_currentIndexChanged(int index)
{
  QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
  int gvar = comboBox->property("index").toInt();
  model.gvsource[gvar] = index;
  emit modified();
}

void FlightMode::phaseGVUse_currentIndexChanged(int index)
{
  if (!lock) {
    lock = true;
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int gvar = comboBox->property("index").toInt();
    if (index == 0) {
      phase.gvars[gvar]=0;
    }
    else {
      phase.gvars[gvar] = 1024+index;
    }
    update();
    emit modified();
    lock = false;
  }
}

void FlightMode::phaseREValue_editingFinished()
{
  if (!lock) {
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    int gvar = spinBox->property("index").toInt();
    phase.rotaryEncoders[gvar] = spinBox->value();
    emit modified();
  }
}

void FlightMode::phaseREUse_currentIndexChanged(int index)
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

void FlightMode::phaseTrimUse_currentIndexChanged(int index)
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
      phase.trimMode[chn] = data % 2;
      phase.trimRef[chn] = data / 2;
      phase.trim[chn] = 0;
    }
    trimUpdate(trim);
    emit modified();
  }
}

void FlightMode::phaseTrim_valueChanged()
{
  if (!lock) {
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    int trim = spinBox->property("index").toInt();
    int chn = CONVERT_MODE(trim+1)-1;
    int value = spinBox->value();
    model.setTrimValue(phaseIdx, chn, value);
    lock = true;
    trimsSlider[trim]->setValue(value);
    lock = false;
    emit modified();
  }
}

void FlightMode::phaseTrimSlider_valueChanged()
{
  if (!lock) {
    QSlider *slider = qobject_cast<QSlider*>(sender());
    int trim = slider->property("index").toInt();
    int chn = CONVERT_MODE(trim+1)-1;
    int value = slider->value();
    model.setTrimValue(phaseIdx, chn, value);
    lock = true;
    trimsValue[trim]->setValue(value);
    lock = false;
    emit modified();
  }
}

/**********************************************************/

FlightModes::FlightModes(QWidget * parent, ModelData & model, GeneralSettings & generalSettings):
  ModelPanel(parent, model),
  modesCount(GetEepromInterface()->getCapability(FlightPhases))
{
  QGridLayout * gridLayout = new QGridLayout(this);
  tabWidget = new QTabWidget(this);
  for (int i=0; i<modesCount; i++) {
    FlightMode *tab = new FlightMode(tabWidget, model, i, generalSettings);
    tab->setProperty("index", i);
    panels << tab;
    connect(tab, SIGNAL(modified()), this, SLOT(onPhaseModified()));
    connect(tab, SIGNAL(nameModified()), this, SLOT(onPhaseNameChanged()));
    tabWidget->addTab(tab, getTabName(i));
  }
  gridLayout->addWidget(tabWidget, 0, 0, 1, 1);
  connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_tabWidget_currentChanged(int)));
}

FlightModes::~FlightModes()
{
}

void FlightModes::onPhaseModified()
{
  emit modified();
}

QString FlightModes::getTabName(int index)
{
  QString result = tr("Flight Mode %1").arg(index);
  const char *name = model.phaseData[index].name;
  if (GetEepromInterface()->getCapability(FlightModesName) && strlen(name) > 0) {
    result += tr(" (%1)").arg(name);
  }
  else if (index == 0) {
    result += tr(" (default)");
  }

  return result;
}

void FlightModes::onPhaseNameChanged()
{
  int index = sender()->property("index").toInt();
  tabWidget->setTabText(index, getTabName(index));
}

void FlightModes::update()
{
  on_tabWidget_currentChanged(tabWidget->currentIndex());
}

void FlightModes::on_tabWidget_currentChanged(int index)
{
  panels[index]->update();
}
