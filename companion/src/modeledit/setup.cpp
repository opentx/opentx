#include "setup.h"
#include "ui_setup.h"
#include "ui_setup_timer.h"
#include "ui_setup_module.h"
#include "helpers.h"
#include "appdata.h"
#include "modelprinter.h"

TimerPanel::TimerPanel(QWidget *parent, ModelData & model, TimerData & timer, GeneralSettings & generalSettings, Firmware * firmware, QWidget * prevFocus):
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
  ui->countdownBeep->addItem(tr("Silent"), TimerData::COUNTDOWN_SILENT);
  ui->countdownBeep->addItem(tr("Beeps"), TimerData::COUNTDOWN_BEEPS);
  if (IS_ARM(board) || IS_2560(board)) {
    ui->countdownBeep->addItem(tr("Voice"), TimerData::COUNTDOWN_VOICE);
    ui->countdownBeep->addItem(tr("Haptic"), TimerData::COUNTDOWN_HAPTIC);
  }
  
  ui->value->setMaximumTime(firmware->getMaxTimerStart());

  ui->persistent->setField(timer.persistent, this);
  ui->persistent->addItem(tr("Not persistent"), 0);
  ui->persistent->addItem(tr("Persistent (flight)"), 1);
  ui->persistent->addItem(tr("Persistent (manual reset)"), 2);

  disableMouseScrolling();
  QWidget::setTabOrder(prevFocus, ui->name);
  QWidget::setTabOrder(ui->name, ui->value);
  QWidget::setTabOrder(ui->value, ui->mode);
  QWidget::setTabOrder(ui->mode, ui->countdownBeep);
  QWidget::setTabOrder(ui->countdownBeep, ui->minuteBeep);
  QWidget::setTabOrder(ui->minuteBeep, ui->persistent);

  lock = false;
}

TimerPanel::~TimerPanel()
{
  delete ui;
}

void TimerPanel::update()
{
  int hour = timer.val / 3600;
  int min = (timer.val - (hour * 3600)) / 60;
  int sec = (timer.val - (hour * 3600)) % 60;

  ui->value->setTime(QTime(hour, min, sec));

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

QWidget * TimerPanel::getLastFocus()
{
  return ui->persistent;
}

void TimerPanel::on_value_editingFinished()
{
  timer.val = ui->value->time().hour()*3600 + ui->value->time().minute()*60 + ui->value->time().second();
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
  strncpy(timer.name, ui->name->text().toLatin1(), length);
  emit modified();
}

/******************************************************************************/

#define FAILSAFE_CHANNEL_HOLD    2000
#define FAILSAFE_CHANNEL_NOPULSE 2001

ModulePanel::ModulePanel(QWidget *parent, ModelData & model, ModuleData & module, GeneralSettings & generalSettings, Firmware * firmware, int moduleIdx):
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
    if (generalSettings.hw_uartMode != UART_MODE_SBUS_TRAINER) {
      ui->trainerMode->setItemData(TRAINER_MODE_MASTER_BATTERY_COMPARTMENT, 0, Qt::UserRole - 1);
    }
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
  for (int i=0; i<PULSES_PROTOCOL_LAST; i++) {
    if (firmware->isAvailable((PulsesProtocol)i, moduleIdx)) {
      ui->protocol->addItem(ModelPrinter::printModuleProtocol(i), (QVariant)i);
      if (i == module.protocol) ui->protocol->setCurrentIndex(ui->protocol->count()-1);
    }
  }

  for (int i=0; i<=MM_RF_PROTO_LAST; i++)
  {
    ui->multiProtocol->addItem(ModelPrinter::printMultiRfProtocol(i, false), (QVariant) i);
  }

  if (firmware->getCapability(HasFailsafe)) {
    for (int i=0; i<maxChannels; i++) {
      QLabel * label = new QLabel(this);
      label->setText(QString::number(i+1));
      QComboBox * combo = new QComboBox(this);
      combo->setProperty("index", i);
      combo->addItem(tr("Value"), 0);
      combo->addItem(tr("Hold"), FAILSAFE_CHANNEL_HOLD);
      combo->addItem(tr("No Pulse"), FAILSAFE_CHANNEL_NOPULSE);
      QDoubleSpinBox * spinbox = new QDoubleSpinBox(this);
      spinbox->setMinimumSize(QSize(20, 0));
      spinbox->setRange(-150, 150);
      spinbox->setSingleStep(0.1);
      spinbox->setDecimals(1);
      label->setProperty("index", i);
      spinbox->setProperty("index", i);
      ui->failsafesLayout->addWidget(label, 3*(i/8), i%8, Qt::AlignHCenter);
      ui->failsafesLayout->addWidget(combo, 1+3*(i/8), i%8, Qt::AlignHCenter);
      ui->failsafesLayout->addWidget(spinbox, 2+3*(i/8), i%8, Qt::AlignHCenter);
      failsafeGroups[i].combo = combo;
      failsafeGroups[i].spinbox = spinbox;
      failsafeGroups[i].label = label;
      updateFailsafe(i);
      connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onFailsafeComboIndexChanged(int)));
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
#define MASK_OPEN_DRAIN     64
#define MASK_MULTIMODULE    128

void ModulePanel::update()
{
  unsigned int mask = 0;
  PulsesProtocol protocol = (PulsesProtocol)module.protocol;
  unsigned int max_rx_num = 63;

  if (moduleIdx >= 0) {
    mask |= MASK_PROTOCOL;
    switch (protocol) {
      case PULSES_PXX_XJT_X16:
      case PULSES_PXX_XJT_D8:
      case PULSES_PXX_XJT_LR12:
      case PULSES_PXX_DJT:
        mask |= MASK_CHANNELS_RANGE | MASK_CHANNELS_COUNT;
        if (protocol==PULSES_PXX_XJT_X16) mask |= MASK_FAILSAFES | MASK_RX_NUMBER;
        if (protocol==PULSES_PXX_XJT_LR12) mask |= MASK_RX_NUMBER;
        break;
      case PULSES_LP45:
      case PULSES_DSM2:
      case PULSES_DSMX:
        mask |= MASK_CHANNELS_RANGE | MASK_RX_NUMBER;
        module.channelsCount = 6;
        max_rx_num = 20;
        break;
      case PULSES_CROSSFIRE:
        mask |= MASK_CHANNELS_RANGE;
        module.channelsCount = 16;
        break;
      case PULSES_PPM:
        mask |= MASK_PPM_FIELDS | MASK_CHANNELS_RANGE| MASK_CHANNELS_COUNT;
        if (IS_9XRPRO(firmware->getBoard())) {
          mask |= MASK_OPEN_DRAIN;
        }
        break;
      case PULSES_MULTIMODULE:
        mask |= MASK_CHANNELS_RANGE | MASK_RX_NUMBER | MASK_MULTIMODULE;
        break;
      case PULSES_OFF:
      default:
        break;
    }
  }
  else if (IS_TARANIS(firmware->getBoard())) {
    if (model->trainerMode == TRAINER_SLAVE_JACK) {
      mask |= MASK_PPM_FIELDS | MASK_CHANNELS_RANGE | MASK_CHANNELS_COUNT;
    }
  }
  else if (model->trainerMode != TRAINER_MASTER_JACK) {
    mask |= MASK_PPM_FIELDS | MASK_CHANNELS_RANGE | MASK_CHANNELS_COUNT;
  }

  ui->label_protocol->setVisible(mask & MASK_PROTOCOL);
  ui->protocol->setVisible(mask & MASK_PROTOCOL);
  ui->label_rxNumber->setVisible(mask & MASK_RX_NUMBER);
  ui->rxNumber->setVisible(mask & MASK_RX_NUMBER);
  ui->rxNumber->setMaximum(max_rx_num);
  ui->rxNumber->setValue(module.modelId);
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
  ui->ppmPolarity->setCurrentIndex(module.ppm.pulsePol);
  ui->label_ppmOutputType->setVisible(mask & MASK_OPEN_DRAIN);
  ui->ppmOutputType->setVisible(mask & MASK_OPEN_DRAIN);
  ui->ppmOutputType->setCurrentIndex(module.ppm.outputType);
  ui->label_ppmDelay->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmDelay->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmDelay->setValue(module.ppm.delay);
  ui->label_ppmFrameLength->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmFrameLength->setVisible(mask & MASK_PPM_FIELDS);
  ui->ppmFrameLength->setMinimum(module.channelsCount*(model->extendedLimits ? 2.250 : 2)+3.5);
  ui->ppmFrameLength->setMaximum(firmware->getCapability(PPMFrameLength));
  ui->ppmFrameLength->setValue(22.5+((double)module.ppm.frameLength)*0.5);

  // Multi settings fields
  ui->label_multiProtocol->setVisible(mask & MASK_MULTIMODULE);
  ui->multiProtocol->setVisible(mask & MASK_MULTIMODULE);
  ui->multiProtocol->setCurrentIndex(module.multi.rfProtocol);
  ui->label_multiSubType->setVisible(mask & MASK_MULTIMODULE);
  ui->multiSubType->setVisible(mask & MASK_MULTIMODULE);

  if (mask & MASK_MULTIMODULE) {
    int numEntries = getNumSubtypes(static_cast<MultiModuleRFProtocols>(module.multi.rfProtocol));
    if (module.multi.customProto)
      numEntries=8;
    // Removes extra items
    ui->multiSubType->setMaxCount(numEntries);
    for (int i=0; i < numEntries; i++) {
      if (i < ui->multiSubType->count())
        ui->multiSubType->setItemText(i, ModelPrinter::printMultiSubType(module.multi.rfProtocol, module.multi.customProto, i));
      else
        ui->multiSubType->addItem(ModelPrinter::printMultiSubType(module.multi.rfProtocol, module.multi.customProto, i), (QVariant) i);
    }
  }
  ui->multiSubType->setCurrentIndex(module.subType);

  ui->cb_autoBind->setVisible(mask & MASK_MULTIMODULE);
  ui->cb_autoBind->setChecked(module.multi.autoBindMode ? Qt::Checked : Qt::Unchecked);
  ui->cb_lowPower->setVisible(mask & MASK_MULTIMODULE);
  ui->cb_lowPower->setChecked(module.multi.lowPowerMode ? Qt::Checked : Qt::Unchecked);


  if (firmware->getCapability(HasFailsafe)) {
    ui->label_failsafeMode->setVisible(mask & MASK_FAILSAFES);
    ui->failsafeMode->setVisible(mask & MASK_FAILSAFES);
    ui->failsafeMode->setCurrentIndex(module.failsafeMode);
    ui->failsafesFrame->setEnabled(module.failsafeMode == FAILSAFE_CUSTOM);
    if (firmware->getCapability(ChannelsName) > 0) {
      for(int i=0; i<maxChannels;i++) {
        QString name = QString(model->limitData[i+module.channelsStart].name).trimmed();
        if (!name.isEmpty()) {
          failsafeGroups[i].label->setText(name);
        }
        else {
          failsafeGroups[i].label->setText(QString::number(i+1));
        }
      }
    }
  }
  else {
    mask = 0;
  }

  ui->failsafesLayoutLabel->setVisible(mask & MASK_FAILSAFES);
  ui->failsafesFrame->setVisible(mask & MASK_FAILSAFES);

  if (mask & MASK_CHANNELS_RANGE) {
    ui->channelsStart->setMaximum(33 - ui->channelsCount->value());
    ui->channelsCount->setMaximum(qMin(16, 33-ui->channelsStart->value()));
  }
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
  module.ppm.pulsePol = index;
  emit modified();
}

void ModulePanel::on_ppmOutputType_currentIndexChanged(int index)
{
  module.ppm.outputType = index;
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
    module.ppm.delay = ui->ppmDelay->value();
    emit modified();
  }
}

void ModulePanel::on_rxNumber_editingFinished()
{
  module.modelId = ui->rxNumber->value();
  emit modified();
}

void ModulePanel::on_ppmFrameLength_editingFinished()
{
  module.ppm.frameLength = (ui->ppmFrameLength->value()-22.5) / 0.5;
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
    int channel = sender()->property("index").toInt();
    module.failsafeChannels[channel] = (value*1024)/100;
    emit modified();
  }
}

void ModulePanel::onFailsafeComboIndexChanged(int index)
{
  if (!lock) {
    lock = true;
    int channel = sender()->property("index").toInt();
    module.failsafeChannels[channel] = ((QComboBox *)sender())->itemData(index).toInt();
    updateFailsafe(channel);
    emit modified();
    lock = false;
  }
}

void ModulePanel::on_multiProtocol_currentIndexChanged(int index)
{
  if (!lock) {
    lock=true;
    module.multi.rfProtocol = index;
    unsigned int maxSubTypes = getNumSubtypes(static_cast<MultiModuleRFProtocols>(index));
    if (module.multi.customProto)
      maxSubTypes=8;
    module.subType = std::min(module.subType, maxSubTypes -1);
    update();
    emit modified();
    lock = false;
  }
}

void ModulePanel::on_multiSubType_currentIndexChanged(int index)
{
  if (!lock) {
    lock=true;
    module.subType = index;
    update();
    emit modified();
    lock =  false;
  }
}

void ModulePanel::on_autoBind_stateChanged(int state)
{
  module.multi.autoBindMode = (state == Qt::Checked);
}
void ModulePanel::on_lowPower_stateChanged(int state)
{
  module.multi.lowPowerMode = (state == Qt::Checked);
}

void ModulePanel::updateFailsafe(int channel)
{
  int failsafeValue = module.failsafeChannels[channel];
  QComboBox * combo = failsafeGroups[channel].combo;
  QDoubleSpinBox * spinbox = failsafeGroups[channel].spinbox;
  if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
    combo->setCurrentIndex(1);
    spinbox->setEnabled(false);
    spinbox->setValue(0);
  }
  else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
    combo->setCurrentIndex(2);
    spinbox->setEnabled(false);
    spinbox->setValue(0);
  }
  else {
    combo->setCurrentIndex(0);
    spinbox->setEnabled(true);
    spinbox->setValue(((double)failsafeValue*100)/1024);
  }
}

/******************************************************************************/

SetupPanel::SetupPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  ui(new Ui::Setup)
{
  BoardEnum board = firmware->getBoard();

  lock = true;

  memset(modules, 0, sizeof(modules));

  ui->setupUi(this);

  QRegExp rx(CHAR_FOR_NAMES_REGEX);
  ui->name->setValidator(new QRegExpValidator(rx, this));
  ui->name->setMaxLength(IS_TARANIS(board) ? 12 : 10);

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

  QWidget *prevFocus = ui->image;
  for (int i=0; i<C9X_MAX_TIMERS; i++) {
    if (i<firmware->getCapability(Timers)) {
      timers[i] = new TimerPanel(this, model, model.timers[i], generalSettings, firmware, prevFocus);
      ui->gridLayout->addWidget(timers[i], 1+i, 1);
      connect(timers[i], SIGNAL(modified()), this, SLOT(onChildModified()));
      prevFocus = timers[i]->getLastFocus();
    }
    else {
      foreach(QLabel *label, findChildren<QLabel *>(QRegExp(QString("label_timer%1").arg(i+1)))) {
        label->hide();
      }
    }
  }

  if (firmware->getCapability(HasTopLcd)) {
    ui->toplcdTimer->setField(model.toplcdTimer, this);
    for (int i=0; i<C9X_MAX_TIMERS; i++) {
      if (i<firmware->getCapability(Timers)) {
        ui->toplcdTimer->addItem(tr("Timer %1").arg(i+1), i);
      }
    }
  }
  else {
    ui->toplcdTimerLabel->hide();
    ui->toplcdTimer->hide();
  }

  if (!firmware->getCapability(HasDisplayText)) {
    ui->displayText->hide();
  }
  
  if (!firmware->getCapability(GlobalFunctions)) {
    ui->gfEnabled->hide();
  }

  // Beep Center checkboxes
  prevFocus = ui->trimsDisplay;
  int analogs = NUM_STICKS + firmware->getCapability(Pots) + firmware->getCapability(Sliders);
  for (int i=0; i<analogs+firmware->getCapability(RotaryEncoders); i++) {
    QCheckBox * checkbox = new QCheckBox(this);
    checkbox->setProperty("index", i);
    checkbox->setText(i<analogs ? AnalogString(i) : RotaryEncoderString(i-analogs));
    ui->centerBeepLayout->addWidget(checkbox, 0, i+1);
    connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(onBeepCenterToggled(bool)));
    centerBeepCheckboxes << checkbox;
    if (IS_TARANIS(board)) {
      RawSource src(SOURCE_TYPE_STICK, i);
      if (src.isPot() && !generalSettings.isPotAvailable(i-NUM_STICKS)) {
        checkbox->hide();
      }
      else if (src.isSlider() && !generalSettings.isSliderAvailable(i-NUM_STICKS-firmware->getCapability(Pots))) {
        checkbox->hide();
      }
    }
    QWidget::setTabOrder(prevFocus, checkbox);
    prevFocus = checkbox;
  }

  // Startup switches warnings
  for (int i=0; i<firmware->getCapability(Switches); i++) {
    if (IS_TARANIS(firmware->getBoard())) {
      if (generalSettings.switchConfig[i] == GeneralSettings::SWITCH_NONE || generalSettings.switchConfig[i] == GeneralSettings::SWITCH_TOGGLE) {
        continue;
      }
    }
    else {
      if (i==firmware->getCapability(Switches)-1) {
        continue;
      }
    }
    QLabel * label = new QLabel(this);
    QSlider * slider = new QSlider(this);
    QCheckBox * cb = new QCheckBox(this);
    slider->setProperty("index", i);
    slider->setOrientation(Qt::Vertical);
    slider->setMinimum(0);
    slider->setInvertedAppearance(true);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setMinimumSize(QSize(30, 50));
    slider->setMaximumSize(QSize(50, 50));
    slider->setSingleStep(1);
    slider->setPageStep(1);
    slider->setTickInterval(1);
    if (IS_TARANIS(board)) {
      label->setText(switchesX9D[i]);
      slider->setMaximum(generalSettings.switchConfig[i] == GeneralSettings::SWITCH_3POS ? 2 : 1);
    }
    else {
      label->setText(switches9X[i]);
      slider->setMaximum(i==0 ? 2 : 1);
    }
    cb->setProperty("index", i);
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
    QWidget::setTabOrder(prevFocus, slider);
    QWidget::setTabOrder(slider, cb);
    prevFocus = cb;
  }
  ui->switchesStartupLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, firmware->getCapability(Switches));

  // Pot warnings
  prevFocus = ui->potWarningMode;
  if (IS_TARANIS(board)) {
    for (int i=0; i<firmware->getCapability(Pots)+firmware->getCapability(Sliders); i++) {
      QCheckBox * cb = new QCheckBox(this);
      cb->setProperty("index", i);
      cb->setText(AnalogString(i+4));
      ui->potWarningLayout->addWidget(cb, 0, i+1);
      connect(cb, SIGNAL(toggled(bool)), this, SLOT(potWarningToggled(bool)));
      potWarningCheckboxes << cb;
      if (RawSource(SOURCE_TYPE_STICK, NUM_STICKS+i).isPot()) {
        if (!generalSettings.isPotAvailable(i)) {
          cb->hide();
        }
      }
      else {
        if (!generalSettings.isSliderAvailable(i-firmware->getCapability(Pots))) {
          cb->hide();
        }
      }
      QWidget::setTabOrder(prevFocus, cb);
      prevFocus = cb;
    }
  }
  else {
    ui->label_potWarning->hide();
    ui->potWarningMode->hide();
  }

  if (IS_ARM(board)) {
    ui->trimsDisplay->setField(model.trimsDisplay, this);
  }
  else {
    ui->labelTrimsDisplay->hide();
    ui->trimsDisplay->hide();
  }

  for (int i=0; i<firmware->getCapability(NumModules); i++) {
    modules[i] = new ModulePanel(this, model, model.moduleData[i], generalSettings, firmware, i);
    ui->modulesLayout->addWidget(modules[i]);
    connect(modules[i], SIGNAL(modified()), this, SLOT(onChildModified()));
  }

  if (firmware->getCapability(ModelTrainerEnable)) {
    modules[C9X_NUM_MODULES] = new ModulePanel(this, model, model.moduleData[C9X_NUM_MODULES], generalSettings, firmware, -1);
    ui->modulesLayout->addWidget(modules[C9X_NUM_MODULES]);
    connect(modules[C9X_NUM_MODULES], SIGNAL(modified()), this, SLOT(onChildModified()));
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
    model->thrTraceSrc = index;
    emit modified();
  }
}

void SetupPanel::on_name_editingFinished()
{
  int length = ui->name->maxLength();
  strncpy(model->name, ui->name->text().toLatin1(), length);
  emit modified();
}

void SetupPanel::on_image_currentIndexChanged(int index)
{
  if (!lock) {
    strncpy(model->bitmap, ui->image->currentText().toLatin1(), 10);
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
  const QString pots9x[] = { QObject::tr("P1"), QObject::tr("P2"), QObject::tr("P3")};
  const QString potsTaranis[] = { QObject::tr("S1"), QObject::tr("S2"), QObject::tr("S3"), QObject::tr("LS"), QObject::tr("RS")};
  const QString potsTaranisX9E[] = { QObject::tr("F1"), QObject::tr("F2"), QObject::tr("F3"), QObject::tr("F4"), QObject::tr("S1"), QObject::tr("S2"), QObject::tr("LS"), QObject::tr("RS")};

  unsigned int i;

  lock = true;

  ui->throttleSource->clear();
  ui->throttleSource->addItem(QObject::tr("THR"));

  if (IS_TARANIS_X9E(GetEepromInterface()->getBoard())) {
    for (i=0; i<8; i++) {
      ui->throttleSource->addItem(potsTaranisX9E[i], i);
    }
  }
  else if (IS_TARANIS(GetEepromInterface()->getBoard())) {
    for (i=0; i<5; i++) {
      ui->throttleSource->addItem(potsTaranis[i], i);
    }
  }
  else {
    for (i=0; i<3; i++) {
      ui->throttleSource->addItem(pots9x[i], i);
    }
  }

  int channels = (IS_ARM(GetEepromInterface()->getBoard()) ? 32 : 16);
  for (int i=0; i<channels; i++) {
    ui->throttleSource->addItem(ModelPrinter::printChannelName(i));
  }

  ui->throttleSource->setCurrentIndex(model->thrTraceSrc);

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
  ui->gfEnabled->setChecked(!model->noGlobalFunctions);

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
  unsigned int value;

  for (int i=0; i<startupSwitchesSliders.size(); i++) {
    QSlider *slider = startupSwitchesSliders[i];
    QCheckBox * cb = startupSwitchesCheckboxes[i];
    int index = slider->property("index").toInt();
    bool enabled = !(model->switchWarningEnable & (1 << index));
    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      value = (switchStates >> 2*index) & 0x03;
      if (generalSettings.switchConfig[index] != GeneralSettings::SWITCH_3POS && value == 2)
        value = 1;
    }
    else {
      value = (i==0 ? switchStates & 0x3 : switchStates & 0x1);
      switchStates >>= (i==0 ? 2 : 1);
    }
    slider->setValue(value);
    slider->setEnabled(enabled);
    cb->setChecked(enabled);
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
      shift = index * 2;
      mask = 0x03 << shift;
    }
    else {
      if (index == 0) {
        mask = 0x03;
      }
      else {
        shift = index+1;
        mask = 0x01 << shift;
      }
    }

    model->switchWarningStates &= ~mask;
    
    if (IS_TARANIS(GetEepromInterface()->getBoard()) && generalSettings.switchConfig[index] != GeneralSettings::SWITCH_3POS) {
      if (value == 1) value = 2;
    }

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
    int index = sender()->property("index").toInt();

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
  ui->potWarningMode->setCurrentIndex(model->potsWarningMode);
  for (int i=0; i<potWarningCheckboxes.size(); i++) {
    QCheckBox *checkbox = potWarningCheckboxes[i];
    int index = checkbox->property("index").toInt();
    checkbox->setChecked(!model->potsWarningEnabled[index]);
    checkbox->setDisabled(model->potsWarningMode == 0);
  }
  lock = false;
}

void SetupPanel::potWarningToggled(bool checked)
{
  if (!lock) {
    int index = sender()->property("index").toInt();
    model->potsWarningEnabled[index] = !checked;
    updatePotWarnings();
    emit modified();
  }
}

void SetupPanel::on_potWarningMode_currentIndexChanged(int index)
{
  if (!lock) {
    model->potsWarningMode = index;
    updatePotWarnings();
    emit modified();
  }
}

void SetupPanel::on_displayText_toggled(bool checked)
{
  model->displayChecklist = checked;
  emit modified();
}

void SetupPanel::on_gfEnabled_toggled(bool checked)
{
  model->noGlobalFunctions = !checked;
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
