#include "setup.h"
#include "ui_setup.h"
#include "ui_setup_timer.h"
#include "ui_setup_module.h"
#include "helpers.h"
#include "appdata.h"

TimerPanel::TimerPanel(QWidget *parent, ModelData & model, TimerData & timer):
  ModelPanel(parent, model),
  timer(timer),
  ui(new Ui::Timer)
{
  ui->setupUi(this);

  lock = true;

  // Mode
  populateSwitchCB(ui->mode, timer.mode, POPULATE_TIMER_MODES);

  if (!GetEepromInterface()->getCapability(PermTimers)) {
    ui->persistent->hide();
    ui->persistentValue->hide();
  }

  ui->countdownBeep->addItem(tr("None"));
  ui->countdownBeep->addItem(tr("Beeps"));
  if (IS_ARM(GetEepromInterface()->getBoard()) || IS_2560(GetEepromInterface()->getBoard()))
    ui->countdownBeep->addItem(tr("Countdown"));

  lock = false;
}

TimerPanel::~TimerPanel()
{
  delete ui;
}

void TimerPanel::update()
{
  int min = timer.val / 60;
  int sec = timer.val % 60;
  ui->value->setTime(QTime(0, min, sec));

  if (GetEepromInterface()->getCapability(PermTimers)) {
    int sign = 1;
    int pvalue = timer.pvalue;
    if (pvalue < 0) {
      pvalue = -pvalue;
      sign = -1;
    }
    int hours = pvalue / 3600;
    pvalue -= hours * 3600;
    int minutes = pvalue / 60;
    int seconds = pvalue % 60;
    ui->persistent->setChecked(timer.persistent);
    ui->persistentValue->setText(QString(" %1(%2:%3:%4)").arg(sign<0 ? "-" :" ").arg(hours, 2, 10, QLatin1Char('0')).arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0')));
  }

  ui->countdownBeep->setCurrentIndex(timer.countdownBeep);
  ui->minuteBeep->setChecked(timer.minuteBeep);
}

void TimerPanel::on_value_editingFinished()
{
  timer.val = ui->value->time().minute()*60 + ui->value->time().second();
  emit modified();
}

void TimerPanel::on_mode_currentIndexChanged(int index)
{
  if (!lock) {
    timer.mode = RawSwitch(ui->mode->itemData(index).toInt());
    emit modified();
  }
}

void TimerPanel::on_persistent_toggled(bool checked)
{
  timer.persistent = checked;
  emit modified();
}

void TimerPanel::on_countdownBeep_currentIndexChanged(int index)
{
  timer.countdownBeep = index;
  emit modified();
}

void TimerPanel::on_minuteBeep_toggled(bool checked)
{
  timer.minuteBeep = checked;
  emit modified();
}

/******************************************************************************/

ModulePanel::ModulePanel(QWidget *parent, ModelData & model, ModuleData & module, int moduleIdx):
  ModelPanel(parent, model),
  module(module),
  moduleIdx(moduleIdx),
  ui(new Ui::Module)
{
  lock = true;

  ui->setupUi(this);

  QString label;
  if (moduleIdx < 0) {
    label = tr("Trainer Output");
    ui->trainerMode->setCurrentIndex(model.trainerMode);
  }
  else {
    ui->label_trainerMode->hide();
    ui->trainerMode->hide();
    if (moduleIdx == 0)
      label = tr("Internal Radio System");
    else
      label = tr("External Radio Module");

  }
  ui->label_module->setText(label);

  // The protocols available on this board
  for (int i=0; i<PROTO_LAST; i++) {
    if (GetEepromInterface()->isAvailable((Protocol)i, moduleIdx)) {
      ui->protocol->addItem(getProtocolStr(i), (QVariant)i);
      if (i == module.protocol) ui->protocol->setCurrentIndex(ui->protocol->count()-1);
    }
  }

  if (GetEepromInterface()->getCapability(HasFailsafe)) {
    for (int i=0; i<16; i++) {
      QLabel * label = new QLabel(this);
      label->setText(QString::number(i+1));
      QDoubleSpinBox * spinbox = new QDoubleSpinBox(this);
      spinbox->setMinimumSize(QSize(20, 0));
      spinbox->setRange(-100, 100);
      spinbox->setSingleStep(0.1);
      spinbox->setDecimals(1);
      spinbox->setValue(((double)module.failsafeChannels[i]*100)/1024);
      label->setProperty("index", i);
      spinbox->setProperty("index", i);
      failsafeSpins << spinbox;
      ui->failsafesLayout->addWidget(label, 2*(i/8), i%8, Qt::AlignHCenter);
      ui->failsafesLayout->addWidget(spinbox, 1+2*(i/8), i%8, Qt::AlignHCenter);
      connect(spinbox, SIGNAL(valueChanged(double)), this, SLOT(onFailsafeSpinChanged(double)));
    }
  }

  lock = false;
}

ModulePanel::~ModulePanel()
{
  delete ui;
}

#define MASK_PROTOCOL       1
#define MASK_CHANNELS_COUNT 2
#define MASK_RX_NUMBER      4
#define MASK_CHANNELS_RANGE 8
#define MASK_PPM_FIELDS     16
#define MASK_FAILSAFES      32

void ModulePanel::update()
{
  unsigned int mask = 0;
  Protocol protocol = (Protocol)module.protocol;

  if (moduleIdx >= 0 || model.trainerMode != 0) {
    mask |= MASK_PROTOCOL;
    switch (protocol) {
      case OFF:
        break;
      case PXX_XJT_X16:
      case PXX_XJT_D8:
      case PXX_XJT_LR12:
      case PXX_DJT:
        mask |= MASK_CHANNELS_RANGE | MASK_CHANNELS_COUNT | MASK_RX_NUMBER;
        if (protocol==PXX_XJT_X16) mask |= MASK_FAILSAFES;
        break;
      case LP45:
      case DSM2:
      case DSMX:
        mask |= MASK_CHANNELS_RANGE | MASK_RX_NUMBER;
        module.channelsCount = 8;
        break;
      default:
        mask |= MASK_PPM_FIELDS | MASK_CHANNELS_RANGE| MASK_CHANNELS_COUNT;
        break;
    }
  }

  ui->label_protocol->setVisible(mask & MASK_PROTOCOL);
  ui->protocol->setVisible(mask & MASK_PROTOCOL);
  ui->label_rxNumber->setVisible(mask & MASK_PROTOCOL);
  ui->rxNumber->setVisible(mask & MASK_PROTOCOL);
  ui->rxNumber->setEnabled(mask & MASK_RX_NUMBER);
  ui->rxNumber->setValue(model.modelId);
  ui->label_channelsStart->setVisible(mask & MASK_CHANNELS_RANGE);
  ui->channelsStart->setVisible(mask & MASK_CHANNELS_RANGE);
  ui->channelsStart->setValue(module.channelsStart+1);
  ui->label_channelsCount->setVisible(mask & MASK_CHANNELS_RANGE);
  ui->channelsCount->setVisible(mask & MASK_CHANNELS_RANGE);
  ui->channelsCount->setEnabled(mask & MASK_CHANNELS_COUNT);
  ui->channelsCount->setValue(module.channelsCount);
  ui->channelsCount->setSingleStep(GetEepromInterface()->getCapability(HasPPMStart) ? 1 : 2);

  // PPM settings fields
  ui->label_ppmPolarity->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmPolarity->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmPolarity->setCurrentIndex(module.ppmPulsePol);
  ui->label_ppmDelay->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmDelay->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmDelay->setValue(module.ppmDelay);
  ui->label_ppmFrameLength->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmFrameLength->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmFrameLength->setMinimum(module.channelsCount*(model.extendedLimits ? 2.250 :2)+3.5);
  ui->ppmFrameLength->setMaximum(GetEepromInterface()->getCapability(PPMFrameLength));
  ui->ppmFrameLength->setValue(22.5+((double)module.ppmFrameLength)*0.5);

  if (GetEepromInterface()->getCapability(HasFailsafe)) {
    ui->label_failsafeMode->setVisible(mask & MASK_FAILSAFES);
    ui->failsafeMode->setVisible(mask & MASK_FAILSAFES);
    ui->failsafeMode->setCurrentIndex(module.failsafeMode);
    ui->failsafesFrame->setEnabled(module.failsafeMode == 1);
  }
  else {
    mask = 0;
  }
  
  ui->failsafesLayoutLabel->setVisible(mask & MASK_FAILSAFES);
  ui->failsafesFrame->setVisible(mask & MASK_FAILSAFES);
}

void ModulePanel::on_trainerMode_currentIndexChanged(int index)
{
  if (!lock) {
    model.trainerMode = index;
    update();
    emit modified();
  }
}

void ModulePanel::on_protocol_currentIndexChanged(int index)
{
  if (!lock) {
    module.protocol = ui->protocol->itemData(index).toInt();
    update();
    emit modified();
  }
}

void ModulePanel::on_ppmPolarity_currentIndexChanged(int index)
{
  module.ppmPulsePol = index;
  emit modified();
}

void ModulePanel::on_channelsCount_editingFinished()
{
  if (!lock) {
    module.channelsCount = ui->channelsCount->value();
    update();
    emit modified();
  }
}

void ModulePanel::on_channelsStart_editingFinished()
{
  if (!lock) {
    module.channelsStart = ui->channelsStart->value() - 1;
    update();
    emit modified();
  }
}

void ModulePanel::on_ppmDelay_editingFinished()
{
  if (!lock) {
    // TODO only accept valid values
    module.ppmDelay = ui->ppmDelay->value();
    emit modified();
  }
}

void ModulePanel::on_rxNumber_editingFinished()
{
  model.modelId = ui->rxNumber->value();
  emit modified();
}

void ModulePanel::on_ppmFrameLength_editingFinished()
{
  module.ppmFrameLength = (ui->ppmFrameLength->value()-22.5) / 0.5;
  emit modified();
}

void ModulePanel::on_failsafeMode_currentIndexChanged(int value)
{
  if (!lock) {
    module.failsafeMode = value;
    update();
    emit modified();
  }
}

void ModulePanel::onFailsafeSpinChanged(double value)
{
  if (!lock) {
    int index = sender()->property("index").toInt();
    module.failsafeChannels[index] = (value*1024)/100;
    emit modified();
  }
}

/******************************************************************************/

Setup::Setup(QWidget *parent, ModelData & model):
  ModelPanel(parent, model),
  ui(new Ui::Setup)
{
  lock = true;

  memset(modules, 0, sizeof(modules));

  ui->setupUi(this);

  ui->name->setMaxLength(IS_TARANIS(GetEepromInterface()->getBoard()) ? 12 : 10);

  for (int i=0; i<C9X_MAX_TIMERS; i++) {
    timers[i] = new TimerPanel(this, model, model.timers[i]);
    ui->gridLayout->addWidget(timers[i], 1+i, 1);
    connect(timers[i], SIGNAL(modified()), this, SLOT(onChildModified()));
  }

  for (int i=0; i<GetEepromInterface()->getCapability(NumModules); i++) {
    modules[i] = new ModulePanel(this, model, model.moduleData[i], i);
    ui->modulesLayout->addWidget(modules[i]);
    connect(modules[i], SIGNAL(modified()), this, SLOT(onChildModified()));
  }

  if (GetEepromInterface()->getCapability(ModelTrainerEnable)) {
    modules[C9X_NUM_MODULES] = new ModulePanel(this, model, model.moduleData[C9X_NUM_MODULES], -1);
    ui->modulesLayout->addWidget(modules[C9X_NUM_MODULES]);
  }

  if (GetEepromInterface()->getCapability(ModelImage)) {
    QStringList items;
    items.append("");
    QString path = g.profile[g.id()].sdPath();
    path.append("/BMP/");
    QDir qd(path);
    int vml = GetEepromInterface()->getCapability(VoicesMaxLength)+4;
    if (qd.exists()) {
      QStringList filters;
      filters << "*.bmp" << "*.bmp";
      foreach ( QString file, qd.entryList(filters, QDir::Files) ) {
        QFileInfo fi(file);
        QString temp = fi.completeBaseName();
        if (!items.contains(temp) && temp.length() <= vml) {
          items.append(temp);
        }
      }
    }
    if (!items.contains(model.bitmap)) {
      items.append(model.bitmap);
    }
    items.sort();
    foreach ( QString file, items ) {
      ui->image->addItem(file);
      if (file == model.bitmap) {
        ui->image->setCurrentIndex(ui->image->count()-1);
        QString fileName = path;
        fileName.append(model.bitmap);
        fileName.append(".bmp");
        QImage image(fileName);
        if (image.isNull()) {
          fileName = path;
          fileName.append(model.bitmap);
          fileName.append(".BMP");
          image.load(fileName);
        }
        if (!image.isNull()) {
          ui->imagePreview->setPixmap(QPixmap::fromImage(image.scaled( 64,32)));;
        }
      }
    }
  }
  else {
    ui->image->hide();
    ui->modelImage_label->hide();
    ui->imagePreview->hide();
  }
  
  if (!GetEepromInterface()->getCapability(HasDisplayText)) {
    ui->displayText->hide();
  }

  // Beep Center checkboxes
  int analogs = 4 + GetEepromInterface()->getCapability(Pots);
  for (int i=0; i<analogs+GetEepromInterface()->getCapability(RotaryEncoders); i++) {
    QCheckBox * checkbox = new QCheckBox(this);
    checkbox->setProperty("index", i);
    checkbox->setText(i<analogs ? AnalogString(i) : RotaryEncoderString(i-analogs));
    ui->centerBeepLayout->addWidget(checkbox, 0, i+1);
    connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(onBeepCenterToggled(bool)));
    centerBeepCheckboxes << checkbox;
  }

  // Startup switches warnings
  for (int i=0; i<GetEepromInterface()->getCapability(Switches)-1; i++) {
    QLabel * label = new QLabel(this);
    QSlider * slider = new QSlider(this);
    QCheckBox * cb = new QCheckBox(this);
    slider->setProperty("index", i+1);
    slider->setOrientation(Qt::Vertical);
    slider->setMinimum(0);
    slider->setSingleStep(1);
    slider->setPageStep(1);
    slider->setInvertedAppearance(true);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(1);
    slider->setMinimumSize(QSize(30, 50));
    slider->setMaximumSize(QSize(50, 50));
    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      label->setText(switchesX9D[i]);
      slider->setMaximum(i==5 ? 1 : 2);
    }
    else {
      label->setText(switches9X[i]);
      slider->setMaximum(i==0 ? 2 : 1);
    }
    cb->setProperty("index", i+1);
    ui->switchesStartupLayout->addWidget(label, 0, i+1);
    ui->switchesStartupLayout->setAlignment(label, Qt::AlignCenter);
    ui->switchesStartupLayout->addWidget(slider, 1, i+1);
    ui->switchesStartupLayout->setAlignment(slider, Qt::AlignCenter);
    ui->switchesStartupLayout->addWidget(cb, 2, i+1);
    ui->switchesStartupLayout->setAlignment(cb, Qt::AlignCenter);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(startupSwitchEdited(int)));
    connect(cb, SIGNAL(toggled(bool)), this, SLOT(startupSwitchToggled(bool)));
    startupSwitchesSliders << slider;
    startupSwitchesCheckboxes << cb;
  }
  ui->switchesStartupLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, GetEepromInterface()->getCapability(Switches));

  // Pot warnings
  if(IS_TARANIS(GetEepromInterface()->getBoard())) {
    for (int i=0; i<GetEepromInterface()->getCapability(Pots); i++) {
      QCheckBox * cb = new QCheckBox(this);
      cb->setProperty("index", i+1);
      cb->setText(AnalogString(i+4));
      ui->potWarningLayout->addWidget(cb, 0, i+1);
      connect(cb, SIGNAL(toggled(bool)), this, SLOT(potWarningToggled(bool)));
      potWarningCheckboxes << cb;
    }
  }
  else {
    ui->label_potWarning->hide();
    ui->potWarningMode->hide();
  }
  lock = false;
}

Setup::~Setup()
{
  delete ui;
}

void Setup::on_extendedLimits_toggled(bool checked)
{
  model.extendedLimits = checked;
  emit extendedLimitsToggled();
  emit modified();
}

void Setup::on_throttleWarning_toggled(bool checked)
{
  model.disableThrottleWarning = !checked;
  emit modified();
}

void Setup::on_throttleReverse_toggled(bool checked)
{
  model.throttleReversed = checked;
  emit modified();
}

void Setup::on_extendedTrims_toggled(bool checked)
{
  model.extendedTrims = checked;
  emit modified();
}

void Setup::on_trimIncrement_currentIndexChanged(int index)
{
  model.trimInc = index-2;
  emit modified();
}

void Setup::on_throttleSource_currentIndexChanged(int index)
{
  if (!lock) {
    model.thrTraceSrc = ui->throttleSource->itemData(index).toInt();
    emit modified();
  }
}

void Setup::on_name_editingFinished()
{
  int length = ui->name->maxLength();
  strncpy(model.name, ui->name->text().toAscii(), length);
  emit modified();
}

void Setup::on_image_currentIndexChanged(int index)
{
  if (!lock) {
    strncpy(model.bitmap, ui->image->currentText().toAscii(), GetEepromInterface()->getCapability(VoicesMaxLength));
    QString path=g.profile[g.id()].sdPath();
    path.append("/BMP/");
    QDir qd(path);
    if (qd.exists()) {
      QString fileName=path;
      fileName.append(model.bitmap);
      fileName.append(".bmp");
      QImage image(fileName);
      if (image.isNull()) {
        fileName=path;
        fileName.append(model.bitmap);
        fileName.append(".BMP");
        image.load(fileName);
      }
      if (!image.isNull()) {
        ui->imagePreview->setPixmap(QPixmap::fromImage(image.scaled( 64,32)));;
      }
      else {
        ui->imagePreview->clear();
      }
    }
    else {
      ui->imagePreview->clear();
    }
    emit modified();
  }
}

void Setup::populateThrottleSourceCB()
{
  const QString sources9x[] = { QObject::tr("THR"), QObject::tr("P1"), QObject::tr("P2"), QObject::tr("P3")};
  const QString sourcesTaranis[] = { QObject::tr("THR"), QObject::tr("S1"), QObject::tr("S2"), QObject::tr("S3"), QObject::tr("LS"), QObject::tr("RS")};

  unsigned int i;

  lock = true;

  if (IS_TARANIS(GetEepromInterface()->getBoard())) {
    for (i=0; i<6; i++) {
      ui->throttleSource->addItem(sourcesTaranis[i], i);
    }
  }
  else {
    for (i=0; i<4; i++) {
      ui->throttleSource->addItem(sources9x[i], i);
    }
  }

  if (model.thrTraceSrc < i)
    ui->throttleSource->setCurrentIndex(model.thrTraceSrc);

  int channels = (IS_ARM(GetEepromInterface()->getBoard()) ? 32 : 16);
  for (int i=0; i<channels; i++) {
    ui->throttleSource->addItem(QObject::tr("CH%1").arg(i+1, 2, 10, QChar('0')), THROTTLE_SOURCE_FIRST_CHANNEL+i);
    if (model.thrTraceSrc == unsigned(THROTTLE_SOURCE_FIRST_CHANNEL+i))
      ui->throttleSource->setCurrentIndex(ui->throttleSource->count()-1);
  }

  lock = false;
}

void Setup::update()
{
  ui->name->setText(model.name);

  ui->throttleReverse->setChecked(model.throttleReversed);
  populateThrottleSourceCB();
  ui->throttleWarning->setChecked(!model.disableThrottleWarning);

  //trim inc, thro trim, thro expo, instatrim
  ui->trimIncrement->setCurrentIndex(model.trimInc+2);
  ui->throttleTrim->setChecked(model.thrTrim);
  ui->extendedLimits->setChecked(model.extendedLimits);
  ui->extendedTrims->setChecked(model.extendedTrims);
  ui->displayText->setChecked(model.displayText);

  updateBeepCenter();
  updateStartupSwitches();
  
  if(IS_TARANIS(GetEepromInterface()->getBoard())) {
    updatePotWarnings();
  }

  for (int i=0; i<C9X_MAX_TIMERS; i++)
    timers[i]->update();

  for (int i=0; i<C9X_NUM_MODULES+1; i++)
    if (modules[i])
      modules[i]->update();
}

void Setup::updateBeepCenter()
{
  for (int i=0; i<centerBeepCheckboxes.size(); i++) {
    centerBeepCheckboxes[i]->setChecked(model.beepANACenter & (0x01 << i));
  }
}

void Setup::updateStartupSwitches()
{
  lock = true;

  unsigned int switchStates = model.switchWarningStates;

  for (int i=0; i<GetEepromInterface()->getCapability(Switches)-1; i++) {
    QSlider * slider = startupSwitchesSliders[i];
    QCheckBox * cb = startupSwitchesCheckboxes[i];
    bool enabled = !(model.nSwToWarn & (1 << i));
    slider->setEnabled(enabled);
    cb->setChecked(enabled);
    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      slider->setValue(i==5 ? (switchStates & 0x3)/2 : switchStates & 0x3);
      switchStates >>= 2;
    }
    else {
      slider->setValue(i==0 ? switchStates & 0x3 : switchStates & 0x1);
      switchStates >>= (i==0 ? 2 : 1);
    }
  }

  lock = false;
}

void Setup::startupSwitchEdited(int value)
{
  if (!lock) {
    int shift = 0;
    unsigned int mask;
    int index = sender()->property("index").toInt();

    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      if (index == 6) {
        shift = (index - 1) * 2;
        mask = 0x02 << shift;
        shift++;
      }
      else {
        shift = (index - 1) * 2;
        mask = 0x03 << shift;
      }
    }
    else {
      if (index == 1) {
        mask = 0x03;
      }
      else {
        shift = index;
        mask = 0x01 << shift;
      }
    }

    model.switchWarningStates &= ~mask;

    if (value) {
      model.switchWarningStates |= (value << shift);
    }

    updateStartupSwitches();
    emit modified();
  }
}

void Setup::startupSwitchToggled(bool checked)
{
  if (!lock) {
    int index = sender()->property("index").toInt()-1;
  
    if (checked)
      model.nSwToWarn &= ~(1 << index);
    else
      model.nSwToWarn |= (1 << index);

    updateStartupSwitches();
    emit modified();
  }
}

void Setup::updatePotWarnings()
{
  lock = true;
  int mode = model.nPotsToWarn >> 6;
  ui->potWarningMode->setCurrentIndex(mode);

  if (mode == 0)
    model.nPotsToWarn = 0x3F;

  for (int i=0; i<potWarningCheckboxes.size(); i++) {
    bool enabled = !(model.nPotsToWarn & (1 << i));

    potWarningCheckboxes[i]->setChecked(enabled);
    potWarningCheckboxes[i]->setDisabled(mode == 0);
  }
  lock = false;
}

void Setup::potWarningToggled(bool checked)
{
  if (!lock) {
    int index = sender()->property("index").toInt()-1;

    if(checked)
      model.nPotsToWarn &= ~(1 << index);
    else
      model.nPotsToWarn |= (1 << index);

    updatePotWarnings();
    emit modified();
  }
}

void Setup::on_potWarningMode_currentIndexChanged(int index)
{
  if (!lock) {
    int mask = 0xC0;
    model.nPotsToWarn = model.nPotsToWarn & ~mask;
    model.nPotsToWarn = model.nPotsToWarn | ((index << 6) & mask);

    updatePotWarnings();
    emit modified();
  }
}

void Setup::on_displayText_toggled(bool checked)
{
  model.displayText = checked;
  emit modified();
}

void Setup::on_throttleTrim_toggled(bool checked)
{
  model.thrTrim = checked;
  emit modified();
}

void Setup::onBeepCenterToggled(bool checked)
{
  if (!lock) {
    int index = sender()->property("index").toInt();
    unsigned int mask = (0x01 << index);
    if (checked)
      model.beepANACenter |= mask;
    else
      model.beepANACenter &= ~mask;
    emit modified();
  }
}

void Setup::onChildModified()
{
  emit modified();
}
