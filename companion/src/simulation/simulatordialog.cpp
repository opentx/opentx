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

#include "simulatordialog.h"
#include "ui_simulatordialog.h"

#include "appdata.h"
#include "debugoutput.h"
#include "radiofaderwidget.h"
#include "radioknobwidget.h"
#include "radioswitchwidget.h"
#include "radiouiaction.h"
#include "sdcard.h"
#include "simulateduiwidget.h"
#include "simulatorinterface.h"
//#include "storage.h"
#include "telemetrysimu.h"
#include "trainersimu.h"
#include "virtualjoystickwidget.h"
#ifdef JOYSTICKS
#include "joystick.h"
#endif

#include <QDebug>
#include <QFile>
#include <iostream>

SimulatorDialog * traceCallbackInstance = 0;

void traceCb(const char * text)
{
  // divert C callback into simulator instance
  if (traceCallbackInstance) {
    traceCallbackInstance->traceCallback(text);
  }
}

SimulatorDialog::SimulatorDialog(QWidget * parent, SimulatorInterface *simulator, SimulatorUiFlavor uiflavor, quint8 flags):
  QDialog(parent),
  ui(new Ui::SimulatorDialog),
  simulator(simulator),
  firmware(getCurrentFirmware()),
  uiFlavor(uiflavor),
  radioSettings(GeneralSettings()),
  timer(NULL),
  radioUiWidget(NULL),
  vJoyLeft(NULL),
  vJoyRight(NULL),
  TelemetrySimu(NULL),
  TrainerSimu(NULL),
  DebugOut(NULL),
  m_board(getCurrentBoard()),
  flags(flags),
  radioProfileId(g.id()),
  lastPhase(-1),
  buttonPressed(0),
  trimPressed(TRIM_NONE),
  eepromDataFromFile(false),
  middleButtonPressed(false)
{
  setWindowFlags(Qt::Window);

  // install simulator TRACE hook
  traceCallbackInstance = this;
  simulator->installTraceHook(traceCb);

  setupUi();
}

SimulatorDialog::~SimulatorDialog()
{
  traceCallbackInstance = 0;
  delete timer;
  delete simulator;
}


/*
 * Public slots/setters
 */

void SimulatorDialog::setRadioProfileId(int value)
{
  radioProfileId = value;
  simulator->setVolumeGain(g.profile[radioProfileId].volumeGain());
  setPaths(g.profile[radioProfileId].sdPath(), radioDataPath);
}

void SimulatorDialog::setDataPath(const QString & dataPath)
{
  setPaths(sdCardPath, dataPath);
}

void SimulatorDialog::setPaths(const QString & sdPath, const QString & dataPath)
{
  sdCardPath = sdPath;
  radioDataPath = dataPath;
  simulator->setSdPath(sdPath, dataPath);
}

void SimulatorDialog::setRadioSettings(const GeneralSettings settings)
{
  radioSettings = settings;
}

void SimulatorDialog::setEepromData(const QByteArray & eeprom, bool fromFile)
{
  RadioData simuData;
  quint16 ret = 1;
  QString error;

  if (eeprom.isEmpty() && !radioDataPath.isEmpty()) {
    SdcardFormat sdcard(radioDataPath);
    ret = sdcard.load(simuData);
    if (!ret)
      error = sdcard.error();
  }
  else if (fromFile) {
    // Storage store = Storage(QString(eeprom));
    // if (!(ret = store.load(simuData))) {
    //   error = store.error();
    // }
    QFile file;
    QByteArray ba;
    file.setFileName(QString(eeprom));
    if (file.exists() && file.open(QIODevice::ReadOnly) && !(ba = file.readAll()).isEmpty()) {
      ret = firmware->getEEpromInterface()->load(simuData, (uint8_t *)ba.constData(), getEEpromSize(m_board));
    }
    else {
      ret = 0;
      error = tr("File not found or not readable: %1").arg(QString(eeprom));
    }
  }
  else if (!eeprom.isEmpty()) {
    ret = firmware->getEEpromInterface()->load(simuData, (uint8_t *)eeprom.constData(), getEEpromSize(m_board));
  }

  if (!ret) {
    if (error.isEmpty())
      error = tr("Could not load data, possibly wrong format.");
    QMessageBox::critical(this, tr("Data Load Error"), error);
    return;
  }

  radioSettings = simuData.generalSettings;
  eepromData = eeprom;
  eepromDataFromFile = fromFile;
}

void SimulatorDialog::setRadioData(RadioData * radioData)
{
  if (radioDataPath.isEmpty()) {
    QByteArray eeprom(getEEpromSize(m_board), 0);
    if (firmware->getEEpromInterface()->save((uint8_t *)eeprom.data(), *radioData, 0, firmware->getCapability(SimulatorVariant)) > 0)
      setEepromData(eeprom, false);
  }
  else {
    SdcardFormat sdcard(radioDataPath);
    sdcard.write(*radioData);
    radioSettings = radioData->generalSettings;
  }
}

void SimulatorDialog::traceCallback(const char * text)
{
  // this function is called from other threads
  traceMutex.lock();
  // limit the size of list
  if (traceList.size() < 1000) {
    traceList.append(QString(text));
  }
  traceMutex.unlock();
}

/*
 * Setup
 */

void SimulatorDialog::setupUi()
{
  ui->setupUi(this);

  windowName = tr("Simulating Radio (%1)").arg(firmware->getName());
  setWindowTitle(windowName);

  switch(uiFlavor) {
    case SIMULATOR_UI_X7 :
      radioUiWidget = new SimulatedUIWidget9X(simulator, this);  // TODO
      break;
    case SIMULATOR_UI_X9 :
      radioUiWidget = new SimulatedUIWidgetX9(simulator, this);
      break;
    case SIMULATOR_UI_X12 :
      radioUiWidget = new SimulatedUIWidgetX12(simulator, this);
      break;
    case SIMULATOR_UI_9X :
    default:
      radioUiWidget = new SimulatedUIWidget9X(simulator, this);
      break;
  }
  keymapHelp.append(*radioUiWidget->getKeymapHelp());

  ui->radioUiTab->layout()->addWidget(radioUiWidget);
  ui->tabWidget->setTabText(0, windowName);
  ui->tabWidget->setFixedHeight(radioUiWidget->height() + ui->tabWidget->tabBar()->height() - 8);

  vJoyLeft = new VirtualJoystickWidget(this, 'L');
  ui->leftStickLayout->addWidget(vJoyLeft);

  vJoyRight = new VirtualJoystickWidget(this, 'R');
  ui->rightStickLayout->addWidget(vJoyRight);

#ifdef JOYSTICKS
  if (g.jsSupport()) {
    int count=0;
    for (int j=0; j<8; j++){
      int axe = g.joystick[j].stick_axe();
      if (axe>=0 && axe<8) {
        jsmap[axe]=j+1;
        jscal[axe][0] = g.joystick[j].stick_min();
        jscal[axe][1] = g.joystick[j].stick_med();
        jscal[axe][2] = g.joystick[j].stick_max();
        jscal[axe][3] = g.joystick[j].stick_inv();
        count++;
      }
    }
    if (count<3) {
      QMessageBox::critical(this, tr("Warning"), tr("Joystick enabled but not configured correctly"));
    }
    if (g.jsCtrl()!=-1) {
      joystick = new Joystick(this);
      if (joystick) {
        if (joystick->open(g.jsCtrl())) {
          int numAxes=std::min(joystick->numAxes,8);
          for (int j=0; j<numAxes; j++) {
            joystick->sensitivities[j] = 0;
            joystick->deadzones[j]=0;
          }
          //mode 1,3 -> THR on right
          vJoyRight->setStickConstraint(VirtualJoystickWidget::HOLD_Y, true);
          vJoyRight->setStickConstraint(VirtualJoystickWidget::HOLD_X, true);
          vJoyLeft->setStickConstraint(VirtualJoystickWidget::HOLD_Y, true);
          vJoyLeft->setStickConstraint(VirtualJoystickWidget::HOLD_X, true);
          connect(joystick, SIGNAL(axisValueChanged(int, int)), this, SLOT(onjoystickAxisValueChanged(int, int)));
        }
        else {
          QMessageBox::critical(this, tr("Warning"), tr("Cannot open joystick, joystick disabled"));
        }
      }
    }
  }
#endif

  ui->tabWidget->setCurrentIndex(flags & SIMULATOR_FLAGS_NOTX);

  connect(vJoyLeft, SIGNAL(trimButtonPressed(int)), this, SLOT(onTrimPressed(int)));
  connect(vJoyLeft, SIGNAL(trimButtonReleased()), this, SLOT(onTrimReleased()));
  connect(vJoyLeft, SIGNAL(trimSliderMoved(int,int)), this, SLOT(onTrimSliderMoved(int,int)));

  connect(vJoyRight, SIGNAL(trimButtonPressed(int)), this, SLOT(onTrimPressed(int)));
  connect(vJoyRight, SIGNAL(trimButtonReleased()), this, SLOT(onTrimReleased()));
  connect(vJoyRight, SIGNAL(trimSliderMoved(int,int)), this, SLOT(onTrimSliderMoved(int,int)));

  connect(ui->btn_help, SIGNAL(released()), this, SLOT(showHelp()));
  connect(ui->btn_telemSim, SIGNAL(released()), this, SLOT(openTelemetrySimulator()));
  connect(ui->btn_trainerSim, SIGNAL(released()), this, SLOT(openTrainerSimulator()));
  connect(ui->btn_debugConsole, SIGNAL(released()), this, SLOT(openDebugOutput()));
  connect(ui->btn_luaReload, SIGNAL(released()), this, SLOT(luaReload()));
  connect(ui->btn_screenshot, SIGNAL(released()), radioUiWidget, SLOT(captureScreenshot()));

  keymapHelp.append(keymapHelp_t(ui->btn_help->shortcut().toString(QKeySequence::NativeText), ui->btn_help->statusTip()));
  if (!firmware->getCapability(Capability(LuaInputsPerScript)))  // hackish! but using "LuaScripts" checks for id "lua" in fw.
    keymapHelp.append(keymapHelp_t(ui->btn_telemSim->shortcut().toString(QKeySequence::NativeText), ui->btn_telemSim->statusTip()));
  else
    ui->btn_luaReload->hide();
  keymapHelp.append(keymapHelp_t(ui->btn_trainerSim->shortcut().toString(QKeySequence::NativeText), ui->btn_trainerSim->statusTip()));
  keymapHelp.append(keymapHelp_t(ui->btn_debugConsole->shortcut().toString(QKeySequence::NativeText), ui->btn_debugConsole->statusTip()));
  if (firmware->getCapability(Capability(SportTelemetry)))
    keymapHelp.append(keymapHelp_t(ui->btn_luaReload->shortcut().toString(QKeySequence::NativeText), ui->btn_luaReload->statusTip()));
  else
    ui->btn_telemSim->hide();
  keymapHelp.append(keymapHelp_t(ui->btn_screenshot->shortcut().toString(QKeySequence::NativeText), ui->btn_screenshot->statusTip()));

}

void SimulatorDialog::setupRadioWidgets()
{
  int i, midpos, aIdx;
  QString wname;
  BoardEnum board = firmware->getBoard();

  // First clear out any existing widgets.
  if (switches.size()) {
    foreach (RadioWidget * w, switches) {
      ui->radioWidgetsHTLayout->removeWidget(w);
      w->deleteLater();
    }
  }
  if (analogs.size()) {
    foreach (RadioWidget * w, analogs) {
      if (w->getType() == RadioWidget::RADIO_WIDGET_KNOB)
        ui->radioWidgetsHTLayout->removeWidget(w);
      else
        ui->VCGridLayout->removeWidget(w);
      w->deleteLater();
    }
  }

  // Now set up new widgets.

  // switches
  Firmware::Switch swinfo;
  // FIXME :  CPN_MAX_SWITCHES == 32 but GeneralSettings::switchConfig[18] !!
  for (i = 0; i < firmware->getCapability(Capability(Switches)) && i < 18 /*CPN_MAX_SWITCHES*/; ++i) {
    if (radioSettings.switchConfig[i] == GeneralSettings::SWITCH_NONE)
      continue;

    if ((wname = QString(radioSettings.switchName[i])).isEmpty()) {
      swinfo = firmware->getSwitch(i);
      wname = QString(swinfo.name);
    }
    RadioSwitchWidget * sw = new RadioSwitchWidget(GeneralSettings::SwitchConfig(radioSettings.switchConfig[i]), wname, 0, ui->radioWidgetsHT);
    sw->setIndex(i);
    ui->radioWidgetsHTLayout->addWidget(sw);
    switches.append(sw);
  }

  midpos = (int)floorf(switches.size() / 2.0f);
  aIdx = 0;

  // pots in middle of switches
  for (i = 0; i < firmware->getCapability(Pots) && i < CPN_MAX_POTS; ++i) {
    if (!radioSettings.isPotAvailable(i))
      continue;

    if ((wname = QString(radioSettings.potName[i])).isEmpty())
      wname = firmware->getAnalogInputName(4 + aIdx + i);

    RadioKnobWidget * pot = new RadioKnobWidget(GeneralSettings::PotConfig(radioSettings.potConfig[i]), wname, 0, ui->radioWidgetsHT);
    pot->setIndex(aIdx + i);
    // FIXME : total hack here -- this needs to follow the exception in radio/src/mixer.cpp:evalInputs()
    if (i == 0 && IS_TARANIS(board) && !IS_TARANIS_X7(board))
      pot->setInvertValue(true);
    ui->radioWidgetsHTLayout->insertWidget(midpos++, pot);
    analogs.append(pot);
  }

  aIdx += i;

  // faders between sticks
  int r = 0, c = 0;
  for (i = 0; i < firmware->getCapability(Sliders) && i + aIdx < CPN_MAX_POTS; ++i) {
    if (!radioSettings.isSliderAvailable(i))
      continue;

    if ((wname = QString(radioSettings.sliderName[i])).isEmpty())
      wname = firmware->getAnalogInputName(4 + aIdx + i);

    RadioFaderWidget * sl = new RadioFaderWidget(wname, 0, ui->radioWidgetsVC);
    sl->setIndex(aIdx + i);
    // FIXME : total hack here -- this needs to follow the exception in radio/src/mixer.cpp:evalInputs()
    if (i == 0 && IS_TARANIS(board) && !IS_TARANIS_X7(board))
      sl->setInvertValue(true);
    /* 2-row option
    if (!(i % 2)) {
      ++r;
      c = 0;
    } */
    ui->VCGridLayout->addWidget(sl, r, c++, 1, 1);
    analogs.append(sl);
  }
}

void SimulatorDialog::setupOutputsDisplay()
{
  // setup Outputs tab
  QWidget * outputsWidget = new QWidget();
  QGridLayout * gridLayout = new QGridLayout(outputsWidget);
  gridLayout->setHorizontalSpacing(0);
  gridLayout->setVerticalSpacing(3);
  gridLayout->setContentsMargins(5, 3, 5, 3);
  // logical switches area
  QWidget * logicalSwitches = new QWidget(outputsWidget);
  logicalSwitches->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  QGridLayout * logicalSwitchesLayout = new QGridLayout(logicalSwitches);
  logicalSwitchesLayout->setHorizontalSpacing(3);
  logicalSwitchesLayout->setVerticalSpacing(2);
  logicalSwitchesLayout->setContentsMargins(0, 0, 0, 0);
  gridLayout->addWidget(logicalSwitches, 0, 0, 1, 1);
  // channels area
  QScrollArea * scrollArea = new QScrollArea(outputsWidget);
  QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);
  sp.setHorizontalStretch(0);
  sp.setVerticalStretch(0);
  scrollArea->setSizePolicy(sp);
  scrollArea->setWidgetResizable(true);
  QWidget * channelsWidget = new QWidget();
  QGridLayout * channelsLayout = new QGridLayout(channelsWidget);
  channelsLayout->setHorizontalSpacing(4);
  channelsLayout->setVerticalSpacing(3);
  channelsLayout->setContentsMargins(0, 0, 0, 3);
  scrollArea->setWidget(channelsWidget);
  gridLayout->addWidget(scrollArea, 1, 0, 1, 1);

  ui->tabWidget->insertTab(1, outputsWidget, QString(tr("Outputs")));

  // populate outputs
  int outputs = std::min(32, firmware->getCapability(Outputs));
  int column = 0;
  for (int i=0; i<outputs; i++) {
    QLabel * label = new QLabel(outputsWidget);
    label->setText(" " + RawSource(SOURCE_TYPE_CH, i).toString() + " ");
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    channelsLayout->addWidget(label, 0, column, 1, 1);

    QSlider * slider = new QSlider(outputsWidget);
    slider->setEnabled(false);
    slider->setMinimum(-1024);
    slider->setMaximum(1024);
    slider->setPageStep(128);
    slider->setTracking(false);
    slider->setOrientation(Qt::Vertical);
    slider->setInvertedAppearance(false);
    slider->setTickPosition(QSlider::TicksRight);
    slider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QLabel * value = new QLabel(outputsWidget);
    value->setMinimumSize(QSize(value->fontMetrics().size(Qt::TextSingleLine, "-100.0").width(), 0));
    value->setAlignment(Qt::AlignCenter);
    value->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    channelValues << value;
    channelsLayout->addWidget(value, 1, column, 1, 1);

    channelSliders << slider;
    channelsLayout->addWidget(slider, 2, column++, 1, 1);
    channelsLayout->setAlignment(slider, Qt::AlignHCenter);
  }

  // populate logical switches
  int switches = firmware->getCapability(LogicalSwitches);
  int rows = switches / (switches > 16 ? 4 : 2);
  for (int i=0; i<switches; i++) {
    QFrame * swtch = createLogicalSwitch(outputsWidget, i, logicalSwitchLabels);
    logicalSwitchesLayout->addWidget(swtch, i / rows, i % rows, 1, 1);
  }
}

void SimulatorDialog::setupGVarsDisplay()
{
  int fmodes = firmware->getCapability(FlightModes);
  int gvars = firmware->getCapability(Gvars);
  if (gvars>0) {
    // setup GVars tab
    QWidget * gvarsWidget = new QWidget();
    QGridLayout * gvarsLayout = new QGridLayout(gvarsWidget);
    ui->tabWidget->addTab(gvarsWidget, QString(tr("GVars")));

    for (int fm=0; fm<fmodes; fm++) {
      QLabel * label = new QLabel(gvarsWidget);
      label->setText(QString("FM%1").arg(fm));
      label->setAlignment(Qt::AlignCenter);
      gvarsLayout->addWidget(label, 0, fm+1);
    }
    for (int i=0; i<gvars; i++) {
      QLabel * label = new QLabel(gvarsWidget);
      label->setText(QString("GV%1").arg(i+1));
      label->setAutoFillBackground(true);
      if ((i % 2) ==0 ) {
        label->setStyleSheet("QLabel { background-color: rgb(220, 220, 220) }");
      }
      gvarsLayout->addWidget(label, i+1, 0);
      for (int fm=0; fm<fmodes; fm++) {
        QLabel * value = new QLabel(gvarsWidget);
        value->setAutoFillBackground(true);
        value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if ((i % 2) ==0 ) {
          value->setStyleSheet("QLabel { background-color: rgb(220, 220, 220) }");
        }
        value->setText("0");
        gvarValues << value;
        gvarsLayout->addWidget(value, i+1, fm+1);
      }
    }
  }
}

QFrame * SimulatorDialog::createLogicalSwitch(QWidget * parent, int switchNo, QVector<QLabel *> & labels)
{
    QFrame * swtch = new QFrame(parent);
    swtch->setAutoFillBackground(true);
    swtch->setFrameShape(QFrame::Panel);
    swtch->setFrameShadow(QFrame::Raised);
    swtch->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    swtch->setMaximumHeight(18);
    QVBoxLayout * layout = new QVBoxLayout(swtch);
    layout->setContentsMargins(2, 0, 2, 0);
    QFont font;
    font.setPointSize(8);
    QLabel * label = new QLabel(swtch);
    label->setFont(font);
    label->setText(RawSwitch(SWITCH_TYPE_VIRTUAL, switchNo+1).toString());
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    labels << label;
    layout->addWidget(label);
    return swtch;
}

void SimulatorDialog::setupTimer()
{
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
  timer->start(10);
}

/*
 * Startup
 */

void SimulatorDialog::start()
{
  setupRadioWidgets();
  setupOutputsDisplay();
  setupGVarsDisplay();
  setTrims();

  if (eepromData.isEmpty())
    simulator->start((const char *)0);
  else if (eepromDataFromFile)
    simulator->start(eepromData.constData());
  else
    simulator->start(eepromData, (flags & SIMULATOR_FLAGS_NOTX) ? false : true);

  setupTimer();
  eepromData.clear();  // this is safe because simulator->start() makes copy of data/discards the file name
}

/*
 * Input/Output handlers for SimulatorInterface
 */

// Read various values from firmware simulator and populate values in this UI
void SimulatorDialog::setValues()
{
  static const int numGvars  = firmware->getCapability(Gvars);
  static const int numFlightModes = firmware->getCapability(FlightModes);
  static QString phase_name;
  static TxOutputs prevOutputs;
  static int prevPhase = -1;
  int currentPhase;

  TxOutputs outputs;
  simulator->getValues(outputs);

  for (int i = 0; i < channelSliders.size() && i < CPN_MAX_CHNOUT; i++) {
    channelSliders[i]->setValue(qMin(1024, qMax(-1024, outputs.chans[i])));
    channelValues[i]->setText(QString("%1").arg((qreal)outputs.chans[i]*100/1024, 0, 'f', 1));
  }

  for (int i = 0; i < logicalSwitchLabels.size() && i < CPN_MAX_CSW; i++) {
    if (prevOutputs.vsw[i] != outputs.vsw[i]) {
      // setStyleSheet() is very CPU time consuming, doing it only on the actual
      // switch state change brings down CPU usage time (for Taranis simulation) from 40% to 7% (Linux, one core usage)
      logicalSwitchLabels[i]->setStyleSheet(outputs.vsw[i] ? CSWITCH_ON : CSWITCH_OFF);
    }
  }

  for (int gv = 0; gv < numGvars; gv++) {
    for (int fm = 0; fm < numFlightModes; fm++) {
      if (prevPhase != lastPhase || prevOutputs.gvars[fm][gv] != outputs.gvars[fm][gv]) {
        // same trick for GVARS, but this has far less effect on CPU usage as setStyleSheet()
        gvarValues[gv*numFlightModes+fm]->setText(QString((fm == lastPhase) ? "<b>%1</b>" : "%1").arg(outputs.gvars[fm][gv]));
      }
    }
  }

  // display current flight mode in window title
  currentPhase = simulator->getPhase();
  if (currentPhase != lastPhase) {
    lastPhase = currentPhase;
    phase_name = QString(simulator->getPhaseName(currentPhase));
    if (phase_name.isEmpty())
      phase_name = QString::number(currentPhase);
    setWindowTitle(windowName + QString(" - Flight Mode %1").arg(phase_name));
  }

  prevOutputs = outputs;
  prevPhase = lastPhase;
}

// "get" values from this UI and send them to the firmware simulator.
void SimulatorDialog::getValues()
{
  static const int numTrims  = firmware->getCapability(NumTrimSwitches);
  int i;

  TxInputs inp;
  memset(&inp, 0, sizeof(TxInputs));

  inp.sticks[0] = int(1024 * vJoyLeft->getStickX());    // LEFT HORZ
  inp.sticks[1] = int(-1024 * vJoyLeft->getStickY());   // LEFT VERT
  inp.sticks[2] = int(-1024 * vJoyRight->getStickY());  // RGHT VERT
  inp.sticks[3] = int(1024 * vJoyRight->getStickX());   // RGHT HORZ

  for (i = 0; i < analogs.size() && i < CPN_MAX_POTS; ++i)
    inp.pots[analogs[i]->getIndex()] = analogs[i]->getValue();

  for (i = 0; i < switches.size() && i < CPN_MAX_SWITCHES; ++i)
    inp.switches[switches[i]->getIndex()] = switches[i]->getValue();

  for (i = 0; i < numTrims; ++i)
    inp.trims[i] = (trimPressed == i);

  foreach (RadioUiAction * act, radioUiWidget->getActions()) {
    if (act->getIndex() > -1 && act->getIndex() < CPN_MAX_KEYS)
      inp.keys[act->getIndex()] = act->isActive();
  }

  if (radioUiWidget->getRotEncAction())
    inp.rotenc = radioUiWidget->getRotEncAction()->isActive();

  simulator->setValues(inp);
}

// Read stick trim values from firmware simulator and set joystick widgets as needed.
void SimulatorDialog::setTrims()
{
  typedef VirtualJoystickWidget VJW;
  static Trims lastTrims;
  Trims trims;
  simulator->getTrims(trims);

  if (trims.values[VJW::TRIM_AXIS_L_X] != lastTrims.values[VJW::TRIM_AXIS_L_X])
    vJoyLeft->setTrimValue(VJW::TRIM_AXIS_L_X, trims.values[VJW::TRIM_AXIS_L_X]);
  if (trims.values[VJW::TRIM_AXIS_L_Y] != lastTrims.values[VJW::TRIM_AXIS_L_Y])
    vJoyLeft->setTrimValue(VJW::TRIM_AXIS_L_Y, trims.values[VJW::TRIM_AXIS_L_Y]);
  if (trims.values[VJW::TRIM_AXIS_R_Y] != lastTrims.values[VJW::TRIM_AXIS_R_Y])
    vJoyRight->setTrimValue(VJW::TRIM_AXIS_R_Y, trims.values[VJW::TRIM_AXIS_R_Y]);
  if (trims.values[VJW::TRIM_AXIS_R_X] != lastTrims.values[VJW::TRIM_AXIS_R_X])
    vJoyRight->setTrimValue(VJW::TRIM_AXIS_R_X, trims.values[VJW::TRIM_AXIS_R_X]);

  if (trims.extended != lastTrims.extended) {
    int trimMin = -125, trimMax = +125;
    if (trims.extended) {
      trimMin = -500;
      trimMax = +500;
    }
    vJoyLeft->setTrimRange(VJW::TRIM_AXIS_L_X, trimMin, trimMax);
    vJoyLeft->setTrimRange(VJW::TRIM_AXIS_L_Y, trimMin, trimMax);
    vJoyRight->setTrimRange(VJW::TRIM_AXIS_R_Y, trimMin, trimMax);
    vJoyRight->setTrimRange(VJW::TRIM_AXIS_R_X, trimMin, trimMax);
  }
  lastTrims = trims;
}

/*
 * Event handlers/private slots
 */

void SimulatorDialog::closeEvent(QCloseEvent *)
{
  simulator->stop();
  timer->stop();
  g.profile[radioProfileId].simuWinGeo(saveGeometry());
}

void SimulatorDialog::showEvent(QShowEvent *)
{
  static bool firstShow = true;
  if (firstShow) {
    restoreGeometry(g.profile[radioProfileId].simuWinGeo());

    // The stick position needs to be set after the final show event, otherwise resizes during dialog creation will screw it up.
    if (radioSettings.stickMode & 1) {
      vJoyLeft->setStickConstraint(VirtualJoystickWidget::HOLD_Y, true);
      vJoyLeft->setStickY(1);
    }
    else {
      vJoyRight->setStickConstraint(VirtualJoystickWidget::HOLD_Y, true);
      vJoyRight->setStickY(1);
    }

    firstShow = false;
  }
}

void SimulatorDialog::mousePressEvent(QMouseEvent *event)
{
  if (radioUiWidget)
    radioUiWidget->mousePressEvent(event);
}

void SimulatorDialog::mouseReleaseEvent(QMouseEvent *event)
{
  if (radioUiWidget)
    radioUiWidget->mouseReleaseEvent(event);
}

void SimulatorDialog::wheelEvent(QWheelEvent *event)
{
  if (radioUiWidget)
    radioUiWidget->wheelEvent(event);
}

void SimulatorDialog::onTimerEvent()
{
  static unsigned int lcd_counter = 0;

  if (!simulator->timer10ms()) {
    QMessageBox::critical(this, "Companion", tr("Firmware %1 error: %2").arg(firmware->getName()).arg(simulator->getError()));
    timer->stop();
    return;
  }

  getValues();

  if (!(lcd_counter++ % 5)) {
    setValues();
    setTrims();
    centerSticks();
  }
  radioUiWidget->timedUpdate(lcd_counter);
  updateDebugOutput();
}

void SimulatorDialog::onTrimPressed(int which)
{
  trimPressed = which;
}

void SimulatorDialog::onTrimReleased()
{
  trimPressed = TRIM_NONE;
}

void SimulatorDialog::onTrimSliderMoved(int which, int value)
{
  simulator->setTrim(which, value);
}

void SimulatorDialog::centerSticks()
{
  if (vJoyLeft)
    vJoyLeft->centerStick();

  if (vJoyRight)
    vJoyRight->centerStick();
}

void SimulatorDialog::openTelemetrySimulator()
{
  // allow only one instance
  if (TelemetrySimu == 0) {
    TelemetrySimu = new TelemetrySimulator(this, simulator);
    TelemetrySimu->show();
    connect(TelemetrySimu, &TelemetrySimulator::destroyed, [this](QObject *) {
      this->TelemetrySimu = NULL;
    });
  }
  else if (!TelemetrySimu->isVisible()) {
    TelemetrySimu->show();
  }
}

void SimulatorDialog::openTrainerSimulator()
{
  // allow only one instance
  if (TrainerSimu == 0) {
    TrainerSimu = new TrainerSimulator(this, simulator);
    TrainerSimu->show();
    connect(TrainerSimu, &TrainerSimulator::destroyed, [this](QObject *) {
      this->TrainerSimu = NULL;
    });
  }
  else if (!TrainerSimu->isVisible()) {
    TrainerSimu->show();
  }
}

void SimulatorDialog::openDebugOutput()
{
  // allow only one instance, but install signal handler to catch dialog destruction just in case
  if (!DebugOut) {
    DebugOut = new DebugOutput(this);
    DebugOut->traceCallback(traceBuffer);
    DebugOut->show();
    connect(DebugOut, &DebugOutput::destroyed, [this](QObject *) {
      this->DebugOut = NULL;
    });
  }
  else if (!DebugOut->isVisible()) {
    DebugOut->show();
  }
}

void SimulatorDialog::updateDebugOutput()
{
  traceMutex.lock();
  while (!traceList.isEmpty()) {
    QString text = traceList.takeFirst();
    traceBuffer.append(text);
    // limit the size of traceBuffer
    if (traceBuffer.size() > 10*1024) {
      traceBuffer.remove(0, 1*1024);
    }
    if (DebugOut) {
      DebugOut->traceCallback(QString(text));
    }
  }
  traceMutex.unlock();
}

void SimulatorDialog::luaReload()
{
  // force a reload of the lua environment
  simulator->setLuaStateReloadPermanentScripts();
}

void SimulatorDialog::showHelp()
{
  QString helpText = tr("Simulator Controls:");
  helpText += "<table cellspacing=4 cellpadding=0>";
  helpText += tr("<tr><th>Key/Mouse</td><th>Action</td></tr>");
  QString keyTemplate = "<tr><td align='center'><pre>%1</pre></td><td align='center'>%2</td></tr>";
  foreach (keymapHelp_t pair, keymapHelp)
    helpText += keyTemplate.arg(pair.first, pair.second);
  helpText += "</table>";

  QMessageBox * msgBox = new QMessageBox(this);
  msgBox->setAttribute(Qt::WA_DeleteOnClose);
  msgBox->setWindowFlags(msgBox->windowFlags() | Qt::WindowStaysOnTopHint);
  msgBox->setStandardButtons( QMessageBox::Ok );
  msgBox->setWindowTitle(tr("Simulator Help"));
  msgBox->setText(helpText);
  msgBox->setModal(false);
  msgBox->show();
}

#ifdef JOYSTICKS
void SimulatorDialog::onjoystickAxisValueChanged(int axis, int value)
{
  int stick;
  if (axis>=0 && axis<=8) {
    stick=jsmap[axis];
    int stickval;
    if (value>jscal[axis][1]) {
      if ((jscal[axis][2]-jscal[axis][1])==0)
        return;
      stickval=(1024*(value-jscal[axis][1]))/(jscal[axis][2]-jscal[axis][1]);
    }
    else {
      if ((jscal[axis][1]-jscal[axis][0])==0)
        return;
      stickval=(1024*(value-jscal[axis][1]))/(jscal[axis][1]-jscal[axis][0]);
    }
    if (jscal[axis][3]==1) {
      stickval*=-1;
    }
    if (stick==1 ) {
      vJoyRight->setStickY(-stickval/1024.0);
    }
    else if (stick==2) {
      vJoyRight->setStickX(stickval/1024.0);
    }
    else if (stick==3) {
      vJoyLeft->setStickY(-stickval/1024.0);
    }
    else if (stick==4) {
      vJoyLeft->setStickX(stickval/1024.0);
    }
    else if (stick >= 5 && stick < 5 + analogs.count()) {
      analogs[stick-5]->setValue(stickval);
    }
  }
}
#endif
