#include "setup.h"
#include "ui_setup.h"
#include "ui_setup_timer.h"
#include "ui_setup_module.h"
#include "helpers.h"
#include "appdata.h"

TimerPanel::TimerPanel(QWidget *parent, ModelData & model, TimerData & timer, GeneralSettings & generalSettings, FirmwareInterface * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  timer(timer),
  ui(new Ui::Timer)
{
  BoardEnum board = firmware->getBoard();

  ui->setupUi(this);

  lock = true;

  // Name
  int length = firmware->getCapability(TimersName);
  if (length == 0) {
    ui->name->hide();
  }
  else {
    ui->name->setMaxLength(length);
    ui->name->setText(timer.name);
  }

  // Mode
  populateSwitchCB(ui->mode, timer.mode, generalSettings, TimersContext);

  if (!firmware->getCapability(PermTimers)) {
    ui->persistent->hide();
    ui->persistentValue->hide();
  }

  ui->countdownBeep->setField(timer.countdownBeep, this);
  ui->countdownBeep->addItem(tr("Silent"), 0);
  ui->countdownBeep->addItem(tr("Beeps"), 1);
  if (IS_ARM(board) || IS_2560(board)) {
    ui->countdownBeep->addItem(tr("Voice"), 2);
  }

  ui->persistent->setField(timer.persistent, this);
  ui->persistent->addItem(tr("Not persistent"), 0);
  ui->persistent->addItem(tr("Persistent (flight)"), 1);
  ui->persistent->addItem(tr("Persistent (manual reset)"), 2);

  disableMouseScrolling();

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

  if (firmware->getCapability(PermTimers)) {
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
    ui->persistentValue->setText(QString(" %1(%2:%3:%4)").arg(sign<0 ? "-" :" ").arg(hours, 2, 10, QLatin1Char('0')).arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0')));
  }

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

void TimerPanel::on_minuteBeep_toggled(bool checked)
{
  timer.minuteBeep = checked;
  emit modified();
}

void TimerPanel::on_name_editingFinished()
{
  int length = ui->name->maxLength();
  strncpy(timer.name, ui->name->text().toAscii(), length);
  emit modified();
}

/******************************************************************************/

ModulePanel::ModulePanel(QWidget *parent, ModelData & model, ModuleData & module, GeneralSettings & generalSettings, FirmwareInterface * firmware, int moduleIdx):
  ModelPanel(parent, model, generalSettings, firmware),
  module(module),
  moduleIdx(moduleIdx),
  ui(new Ui::Module)
{
  lock = true;

  ui->setupUi(this);

  QString label;
  if (moduleIdx < 0) {
    label = tr("Trainer Port");
    ui->trainerMode->setCurrentIndex(model.trainerMode);
    if (!IS_TARANIS(firmware->getBoard())) {
      ui->label_trainerMode->hide();
      ui->trainerMode->hide();
    }
  }
  else {
    ui->label_trainerMode->hide();
    ui->trainerMode->hide();
    if (firmware->getCapability(NumModules) > 1) {
      if (IS_TARANIS(firmware->getBoard())) {
        if (moduleIdx == 0)
          label = tr("Internal Radio System");
        else
          label = tr("External Radio Module");
      }
      else {
        if (moduleIdx == 0)
          label = tr("Radio System");
        else
          label = tr("Extra Radio System");
      }
    }
    else {
      label = tr("Radio System");
    }
  }
  ui->label_module->setText(label);

  // The protocols available on this board
  for (int i=0; i<PROTO_LAST; i++) {
    if (GetEepromInterface()->isAvailable((Protocol)i, moduleIdx)) {
      ui->protocol->addItem(getProtocolStr(i), (QVariant)i);
      if (i == module.protocol) ui->protocol->setCurrentIndex(ui->protocol->count()-1);
    }
  }

  if (firmware->getCapability(HasFailsafe)) {
    for (int i=0; i<16; i++) {
      QLabel * label = new QLabel(this);
      label->setText(QString::number(i+1));
      QDoubleSpinBox * spinbox = new QDoubleSpinBox(this);
      spinbox->setMinimumSize(QSize(20, 0));
      spinbox->setRange(-150, 150);
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

  disableMouseScrolling();

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
  unsigned int max_rx_num = 63;

  if (moduleIdx >= 0) {
    mask |= MASK_PROTOCOL;
    switch (protocol) {
      case PXX_XJT_X16:
      case PXX_XJT_D8:
      case PXX_XJT_LR12:
      case PXX_DJT:
        mask |= MASK_CHANNELS_RANGE | MASK_CHANNELS_COUNT;
        if ((protocol==PXX_XJT_X16) || (protocol==PXX_XJT_LR12)) mask |= MASK_FAILSAFES | MASK_RX_NUMBER;
        break;
      case LP45:
      case DSM2:
      case DSMX:
        mask |= MASK_CHANNELS_RANGE | MASK_RX_NUMBER;
        module.channelsCount = 6;
        max_rx_num = 20;
        break;
      case PPM:
        mask |= MASK_PPM_FIELDS | MASK_CHANNELS_RANGE| MASK_CHANNELS_COUNT;
        break;
      case OFF:
      default:
        break;
    }
  }
  else if (!IS_TARANIS(firmware->getBoard()) || model->trainerMode != 0) {
    mask |= MASK_PPM_FIELDS | MASK_CHANNELS_RANGE | MASK_CHANNELS_COUNT;
  }

  ui->label_protocol->setVisible(mask & MASK_PROTOCOL);
  ui->protocol->setVisible(mask & MASK_PROTOCOL);
  ui->label_rxNumber->setVisible(mask & MASK_RX_NUMBER);
  ui->rxNumber->setVisible(mask & MASK_RX_NUMBER);
  ui->rxNumber->setMaximum(max_rx_num);
  ui->rxNumber->setValue(model->modelId);
  ui->label_channelsStart->setVisible(mask & MASK_CHANNELS_RANGE);
  ui->channelsStart->setVisible(mask & MASK_CHANNELS_RANGE);
  ui->channelsStart->setValue(module.channelsStart+1);
  ui->label_channelsCount->setVisible(mask & MASK_CHANNELS_RANGE);
  ui->channelsCount->setVisible(mask & MASK_CHANNELS_RANGE);
  ui->channelsCount->setEnabled(mask & MASK_CHANNELS_COUNT);
  ui->channelsCount->setValue(module.channelsCount);
  ui->channelsCount->setSingleStep(firmware->getCapability(HasPPMStart) ? 1 : 2);

  // PPM settings fields
  ui->label_ppmPolarity->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmPolarity->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmPolarity->setCurrentIndex(module.ppmPulsePol);
  ui->label_ppmDelay->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmDelay->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmDelay->setValue(module.ppmDelay);
  ui->label_ppmFrameLength->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmFrameLength->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmFrameLength->setMinimum(module.channelsCount*(model->extendedLimits ? 2.250 : 2)+3.5);
  ui->ppmFrameLength->setMaximum(firmware->getCapability(PPMFrameLength));
  ui->ppmFrameLength->setValue(22.5+((double)module.ppmFrameLength)*0.5);

  if (firmware->getCapability(HasFailsafe)) {
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
    model->trainerMode = index;
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
  model->modelId = ui->rxNumber->value();
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

SetupPanel::SetupPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  ui(new Ui::Setup)
{
  BoardEnum board = firmware->getBoard();

  lock = true;

  memset(modules, 0, sizeof(modules));

  ui->setupUi(this);

  ui->name->setMaxLength(IS_TARANIS(board) ? 12 : 10);

  for (int i=0; i<C9X_MAX_TIMERS; i++) {
    if (i<firmware->getCapability(Timers)) {
      timers[i] = new TimerPanel(this, model, model.timers[i], generalSettings, firmware);
      ui->gridLayout->addWidget(timers[i], 1+i, 1);
      connect(timers[i], SIGNAL(modified()), this, SLOT(onChildModified()));
    }
    else {
      foreach(QLabel *label, findChildren<QLabel *>(QRegExp(QString("label_timer%1").arg(i+1)))) {
        label->hide();
      }
    }
  }

  for (int i=0; i<firmware->getCapability(NumModules); i++) {
    modules[i] = new ModulePanel(this, model, model.moduleData[i], generalSettings, firmware, i);
    ui->modulesLayout->addWidget(modules[i]);
    connect(modules[i], SIGNAL(modified()), this, SLOT(onChildModified()));
  }

  if (firmware->getCapability(ModelTrainerEnable)) {
    modules[C9X_NUM_MODULES] = new ModulePanel(this, model, model.moduleData[C9X_NUM_MODULES], generalSettings, firmware, -1);
    ui->modulesLayout->addWidget(modules[C9X_NUM_MODULES]);
  }

  if (firmware->getCapability(ModelImage)) {
    QStringList items;
    items.append("");
    QString path = g.profile[g.id()].sdPath();
    path.append("/BMP/");
    QDir qd(path);
    if (qd.exists()) {
      QStringList filters;
      filters << "*.bmp" << "*.bmp";
      foreach ( QString file, qd.entryList(filters, QDir::Files) ) {
        QFileInfo fi(file);
        QString temp = fi.completeBaseName();
        if (!items.contains(temp) && temp.length() <= 10+4) {
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
  
  if (!firmware->getCapability(HasDisplayText)) {
    ui->displayText->hide();
  }

  // Beep Center checkboxes
  int analogs = 4 + firmware->getCapability(Pots);
  for (int i=0; i<analogs+firmware->getCapability(RotaryEncoders); i++) {
    QCheckBox * checkbox = new QCheckBox(this);
    checkbox->setProperty("index", i);
    checkbox->setText(i<analogs ? AnalogString(i) : RotaryEncoderString(i-analogs));
    ui->centerBeepLayout->addWidget(checkbox, 0, i+1);
    connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(onBeepCenterToggled(bool)));
    centerBeepCheckboxes << checkbox;
    if (!IS_TARANIS_PLUS(board) && i==6) {
      checkbox->hide();
    }
  }

  // Startup switches warnings
  for (int i=0; i<firmware->getCapability(Switches); i++) {
    if (!IS_TARANIS(firmware->getBoard()) && i==firmware->getCapability(Switches)-1)
      continue;
    QLabel * label = new QLabel(this);
    QSlider * slider = new QSlider(this);
    QCheckBox * cb = new QCheckBox(this);
    if (IS_TARANIS(firmware->getBoard()) && !generalSettings.isSwitchWarningAllowedTaranis(i)) {
      label->hide();
      slider->hide();
      cb->hide();
    }
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
    if (IS_TARANIS(board)) {
      label->setText(switchesX9D[i]);
      slider->setMaximum((i==5 || i>=7) ? 1 : 2);
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
  ui->switchesStartupLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, firmware->getCapability(Switches));

  // Pot warnings
  if (IS_TARANIS(board)) {
    for (int i=0; i<firmware->getCapability(Pots); i++) {
      QCheckBox * cb = new QCheckBox(this);
      cb->setProperty("index", i+1);
      cb->setText(AnalogString(i+4));
      ui->potWarningLayout->addWidget(cb, 0, i+1);
      connect(cb, SIGNAL(toggled(bool)), this, SLOT(potWarningToggled(bool)));
      potWarningCheckboxes << cb;
      if (!IS_TARANIS_PLUS(board) && i==2) {
        cb->hide();
      }
    }
  }
  else {
    ui->label_potWarning->hide();
    ui->potWarningMode->hide();
  }

  disableMouseScrolling();

  lock = false;
}

SetupPanel::~SetupPanel()
{
  delete ui;
}

void SetupPanel::on_extendedLimits_toggled(bool checked)
{
  model->extendedLimits = checked;
  emit extendedLimitsToggled();
  emit modified();
}

void SetupPanel::on_throttleWarning_toggled(bool checked)
{
  model->disableThrottleWarning = !checked;
  emit modified();
}

void SetupPanel::on_throttleReverse_toggled(bool checked)
{
  model->throttleReversed = checked;
  emit modified();
}

void SetupPanel::on_extendedTrims_toggled(bool checked)
{
  model->extendedTrims = checked;
  emit modified();
}

void SetupPanel::on_trimIncrement_currentIndexChanged(int index)
{
  model->trimInc = index-2;
  emit modified();
}

void SetupPanel::on_throttleSource_currentIndexChanged(int index)
{
  if (!lock) {
    model->thrTraceSrc = ui->throttleSource->itemData(index).toInt();
    emit modified();
  }
}

void SetupPanel::on_name_editingFinished()
{
  int length = ui->name->maxLength();
  strncpy(model->name, ui->name->text().toAscii(), length);
  emit modified();
}

void SetupPanel::on_image_currentIndexChanged(int index)
{
  if (!lock) {
    strncpy(model->bitmap, ui->image->currentText().toAscii(), 10);
    QString path = g.profile[g.id()].sdPath();
    path.append("/BMP/");
    QDir qd(path);
    if (qd.exists()) {
      QString fileName=path;
      fileName.append(model->bitmap);
      fileName.append(".bmp");
      QImage image(fileName);
      if (image.isNull()) {
        fileName=path;
        fileName.append(model->bitmap);
        fileName.append(".BMP");
        image.load(fileName);
      }
      if (!image.isNull()) {
        ui->imagePreview->setPixmap(QPixmap::fromImage(image.scaled(64, 32)));;
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

void SetupPanel::populateThrottleSourceCB()
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

  if (model->thrTraceSrc < i)
    ui->throttleSource->setCurrentIndex(model->thrTraceSrc);

  int channels = (IS_ARM(GetEepromInterface()->getBoard()) ? 32 : 16);
  for (int i=0; i<channels; i++) {
    ui->throttleSource->addItem(QObject::tr("CH%1").arg(i+1, 2, 10, QChar('0')), THROTTLE_SOURCE_FIRST_CHANNEL+i);
    if (model->thrTraceSrc == unsigned(THROTTLE_SOURCE_FIRST_CHANNEL+i))
      ui->throttleSource->setCurrentIndex(ui->throttleSource->count()-1);
  }

  lock = false;
}

void SetupPanel::update()
{
  ui->name->setText(model->name);

  ui->throttleReverse->setChecked(model->throttleReversed);
  populateThrottleSourceCB();
  ui->throttleWarning->setChecked(!model->disableThrottleWarning);

  //trim inc, thro trim, thro expo, instatrim
  ui->trimIncrement->setCurrentIndex(model->trimInc+2);
  ui->throttleTrim->setChecked(model->thrTrim);
  ui->extendedLimits->setChecked(model->extendedLimits);
  ui->extendedTrims->setChecked(model->extendedTrims);
  ui->displayText->setChecked(model->displayChecklist);

  updateBeepCenter();
  updateStartupSwitches();
  
  if(IS_TARANIS(GetEepromInterface()->getBoard())) {
    updatePotWarnings();
  }

  for (int i=0; i<firmware->getCapability(Timers); i++)
    timers[i]->update();

  for (int i=0; i<C9X_NUM_MODULES+1; i++)
    if (modules[i])
      modules[i]->update();
}

void SetupPanel::updateBeepCenter()
{
  for (int i=0; i<centerBeepCheckboxes.size(); i++) {
    centerBeepCheckboxes[i]->setChecked(model->beepANACenter & (0x01 << i));
  }
}

void SetupPanel::updateStartupSwitches()
{
  lock = true;

  unsigned int switchStates = model->switchWarningStates;

  for (int i=0; i<firmware->getCapability(Switches); i++) {
    QSlider * slider = startupSwitchesSliders[i];
    QCheckBox * cb = startupSwitchesCheckboxes[i];
    bool enabled = !(model->switchWarningEnable & (1 << i));
    slider->setEnabled(enabled);
    cb->setChecked(enabled);
    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      slider->setValue((i==5 || i>=7) ? (switchStates & 0x3)/2 : switchStates & 0x3);
      switchStates >>= 2;
    }
    else {
      if (i == firmware->getCapability(Switches)-1) {
        // Trainer switch, no switch warning
        continue;
      }
      slider->setValue(i==0 ? switchStates & 0x3 : switchStates & 0x1);
      switchStates >>= (i==0 ? 2 : 1);
    }
  }

  lock = false;
}

void SetupPanel::startupSwitchEdited(int value)
{
  if (!lock) {
    int shift = 0;
    unsigned int mask;
    int index = sender()->property("index").toInt();

    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      if (index == 6 || index >= 8) {
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

    model->switchWarningStates &= ~mask;

    if (value) {
      model->switchWarningStates |= (value << shift);
    }

    updateStartupSwitches();
    emit modified();
  }
}

void SetupPanel::startupSwitchToggled(bool checked)
{
  if (!lock) {
    int index = sender()->property("index").toInt()-1;
  
    if (checked)
      model->switchWarningEnable &= ~(1 << index);
    else
      model->switchWarningEnable |= (1 << index);

    updateStartupSwitches();
    emit modified();
  }
}

void SetupPanel::updatePotWarnings()
{
  lock = true;
  int mode = model->nPotsToWarn >> 6;
  ui->potWarningMode->setCurrentIndex(mode);

  if (mode == 0)
    model->nPotsToWarn = 0x3F;

  for (int i=0; i<potWarningCheckboxes.size(); i++) {
    bool enabled = !(model->nPotsToWarn & (1 << i));

    potWarningCheckboxes[i]->setChecked(enabled);
    potWarningCheckboxes[i]->setDisabled(mode == 0);
  }
  lock = false;
}

void SetupPanel::potWarningToggled(bool checked)
{
  if (!lock) {
    int index = sender()->property("index").toInt()-1;

    if(checked)
      model->nPotsToWarn &= ~(1 << index);
    else
      model->nPotsToWarn |= (1 << index);

    updatePotWarnings();
    emit modified();
  }
}

void SetupPanel::on_potWarningMode_currentIndexChanged(int index)
{
  if (!lock) {
    int mask = 0xC0;
    model->nPotsToWarn = model->nPotsToWarn & ~mask;
    model->nPotsToWarn = model->nPotsToWarn | ((index << 6) & mask);

    updatePotWarnings();
    emit modified();
  }
}

void SetupPanel::on_displayText_toggled(bool checked)
{
  model->displayChecklist = checked;
  emit modified();
}

void SetupPanel::on_throttleTrim_toggled(bool checked)
{
  model->thrTrim = checked;
  emit modified();
}

void SetupPanel::onBeepCenterToggled(bool checked)
{
  if (!lock) {
    int index = sender()->property("index").toInt();
    unsigned int mask = (0x01 << index);
    if (checked)
      model->beepANACenter |= mask;
    else
      model->beepANACenter &= ~mask;
    emit modified();
  }
}

void SetupPanel::onChildModified()
{
  emit modified();
}
