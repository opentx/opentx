#include "flightmodes.h"
#include "ui_flightmode.h"
// #include "firmwares/opentx/opentxeeprom.h" // TODO shouldn't be there
#include "helpers.h"
#include <QComboBox>
#include <QGridLayout>

FlightModePanel::FlightModePanel(QWidget * parent, ModelData & model, int phaseIdx, GeneralSettings & generalSettings, Firmware * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  ui(new Ui::FlightMode),
  phaseIdx(phaseIdx),
  phase(model.flightModeData[phaseIdx]),
  reCount(firmware->getCapability(RotaryEncoders)),
  gvCount(firmware->getCapability(Gvars))
{
  ui->setupUi(this);

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
    populateSwitchCB(ui->swtch, phase.swtch, generalSettings, MixesContext);
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
  QString labels[NUM_STICKS]; 
  for(int i=0; i < NUM_STICKS; i++) {
    labels[i] = AnalogString(i);
  }


  trimsLabel << ui->trim1Label << ui->trim2Label << ui->trim3Label << ui->trim4Label;
  trimsUse << ui->trim1Use << ui->trim2Use << ui->trim3Use << ui->trim4Use;
  trimsValue << ui->trim1Value << ui->trim2Value << ui->trim3Value << ui->trim4Value;
  trimsSlider << ui->trim1Slider << ui->trim2Slider << ui->trim3Slider << ui->trim4Slider;

  BoardEnum board = firmware->getBoard();

  for (int i=0; i<4; i++) {
    trimsLabel[i]->setText(labels[CONVERT_MODE(i+1)-1]);

    QComboBox * cb = trimsUse[i];
    cb->setProperty("index", i);
    if (IS_TARANIS(board)) {
      cb->addItem(QObject::tr("Trim disabled"), -1);
    }
    for (int m=0; m<modesCount; m++) {
      if (m == phaseIdx) {
        cb->addItem(QObject::tr("Own Trim"), m*2);
      }
      else if (phaseIdx > 0) {
        cb->addItem(QObject::tr("Use Trim from Flight mode %1").arg(m), m*2);
        if (IS_TARANIS(board)) {
          cb->addItem(QObject::tr("Use Trim from Flight mode %1 + Own Trim as an offset").arg(m), m*2+1);
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
  if (gvCount > 0 && (firmware->getCapability(GvarsFlightModes) || phaseIdx == 0) ) {
    QGridLayout *gvLayout = new QGridLayout(ui->gvGB);
    for (int i=0; i<gvCount; i++) {
      int col = 0;
      // GVar label
      QLabel *label = new QLabel(ui->gvGB);
      label->setText(tr("GVAR%1").arg(i+1));
      gvLayout->addWidget(label, i, col++, 1, 1);
      // GVar name
      int nameLen = firmware->getCapability(GvarsName);
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
      
      // Popups
      if (IS_TARANIS(board) && phaseIdx == 0) {
        gvPopups[i] = new QCheckBox(ui->gvGB);
        gvPopups[i]->setProperty("index", i);
        gvPopups[i]->setText(tr("Popup enabled"));
        connect(gvPopups[i], SIGNAL(toggled(bool)), this, SLOT(phaseGVPopupToggled(bool)));
        gvLayout->addWidget(gvPopups[i], i, col++, 1, 1);
      }
    }
  }
  else {
    ui->gvGB->hide();
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
  ui->name->setText(phase.name);

  int scale = firmware->getCapability(SlowScale);
  ui->fadeIn->setValue(float(phase.fadeIn)/scale);
  ui->fadeOut->setValue(float(phase.fadeOut)/scale);

  for (int i=0; i<4; i++) {
    int trimsMax = firmware->getCapability(ExtendedTrims);
    if (trimsMax == 0 || !model->extendedTrims) {
      trimsMax = 125;
    }
    trimsSlider[i]->setRange(-trimsMax, +trimsMax);
    trimsValue[i]->setRange(-trimsMax, +trimsMax);
    int chn = CONVERT_MODE(i+1)-1;
    if (chn == 2/*TODO constant*/ && model->throttleReversed)
      trimsSlider[i]->setInvertedAppearance(true);
    trimUpdate(i);
  }

  if (ui->gvGB->isVisible()) {
    for (int i=0; i<gvCount; i++) {
      if (firmware->getCapability(GvarsName) > 0) {
        gvNames[i]->setText(model->gvars_names[i]);
      }
      gvValues[i]->setDisabled(model->isGVarLinked(phaseIdx, i));
      gvValues[i]->setValue(model->getGVarValue(phaseIdx, i));
      if (IS_TARANIS(GetEepromInterface()->getBoard()) && phaseIdx == 0) { 
        gvPopups[i]->setChecked(model->gvars_popups[i]);
      }
    }
  }

  for (int i=0; i<reCount; i++) {    
    reValues[i]->setDisabled(false);
    int idx = phase.rotaryEncoders[i];
    FlightModeData *phasere = &phase;
    while (idx > 1024) {
      idx -= 1025;
      phasere = &model->flightModeData[idx];
      idx = phasere->rotaryEncoders[i];
      reValues[i]->setDisabled(true);
    }
    reValues[i]->setValue(phasere->rotaryEncoders[i]);
  }
}

void FlightModePanel::phaseName_editingFinished()
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
    strcpy(phase.name, lineEdit->text().toAscii());
    emit modified();
    emit nameModified();
}

void FlightModePanel::phaseSwitch_currentIndexChanged(int index)
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    phase.swtch = RawSwitch(comboBox->itemData(index).toInt());
    emit modified();
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
    BoardEnum board = firmware->getBoard();
    if (IS_TARANIS(board))
      trimsUse[trim]->setCurrentIndex(1 + 2*phase.trimRef[chn] + phase.trimMode[chn] - (phase.trimRef[chn] > phaseIdx ? 1 : 0));
    else
      trimsUse[trim]->setCurrentIndex(phase.trimRef[chn]);
    trimsValue[trim]->setEnabled(true);
    trimsSlider[trim]->setEnabled(true);
  }
  lock = false;
}

void FlightModePanel::phaseGVValue_editingFinished()
{
  if (!lock) {
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    int gvar = spinBox->property("index").toInt();
    phase.gvars[gvar] = spinBox->value();
    emit modified();
  }
}

void FlightModePanel::GVName_editingFinished()
{
  if (!lock) {
    QLineEdit *lineedit = qobject_cast<QLineEdit*>(sender());
    int gvar = lineedit->property("index").toInt();
    memset(&model->gvars_names[gvar], 0, sizeof(model->gvars_names[gvar]));
    strcpy(model->gvars_names[gvar], lineedit->text().toAscii());
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
      phase.gvars[gvar] = 1024 + index;
    }
    update();
    emit modified();
    lock = false;
  }
}

void FlightModePanel::phaseGVPopupToggled(bool checked)
{
  QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
  int gvar = cb->property("index").toInt();
  model->gvars_popups[gvar] = checked;
}

void FlightModePanel::phaseREValue_editingFinished()
{
  if (!lock) {
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    int gvar = spinBox->property("index").toInt();
    phase.rotaryEncoders[gvar] = spinBox->value();
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
      BoardEnum board = firmware->getBoard();
      if (IS_TARANIS(board)) {
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

/**********************************************************/

FlightModesPanel::FlightModesPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  modesCount(firmware->getCapability(FlightModes))
{
  QGridLayout * gridLayout = new QGridLayout(this);
  tabWidget = new QTabWidget(this);
  for (int i=0; i<modesCount; i++) {
    FlightModePanel *tab = new FlightModePanel(tabWidget, model, i, generalSettings, firmware);
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
