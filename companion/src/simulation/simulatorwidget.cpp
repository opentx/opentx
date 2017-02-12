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

#include "simulatorwidget.h"
#include "ui_simulatorwidget.h"

#include "appdata.h"
#include "radiofaderwidget.h"
#include "radioknobwidget.h"
#include "radioswitchwidget.h"
#include "radiouiaction.h"
#include "sdcard.h"
#include "simulateduiwidget.h"
#include "simulatorinterface.h"
#include "storage.h"
#include "virtualjoystickwidget.h"
#ifdef JOYSTICKS
#include "joystick.h"
#include "joystickdialog.h"
#endif

#include <QDebug>
#include <QFile>
#include <iostream>

SimulatorWidget::SimulatorWidget(QWidget * parent, SimulatorInterface *simulator, quint8 flags):
  QWidget(parent),
  ui(new Ui::SimulatorWidget),
  simulator(simulator),
  firmware(getCurrentFirmware()),
  radioSettings(GeneralSettings()),
  timer(new QTimer(this)),
  radioUiWidget(NULL),
  vJoyLeft(NULL),
  vJoyRight(NULL),
  m_board(getCurrentBoard()),
  flags(flags),
  lastPhase(-1),
  buttonPressed(0),
  trimPressed(TRIM_NONE),
  startupFromFile(false),
  deleteTempRadioData(false),
  saveTempRadioData(false),
  middleButtonPressed(false),
  firstShow(true)
{

#ifdef JOYSTICKS
  joystick = NULL;
#endif

  // defaults
  setRadioProfileId(g.sessionId());
  setSdPath(g.profile[radioProfileId].sdPath());

  ui->setupUi(this);

  windowName = tr("Radio Simulator (%1)").arg(firmware->getName());
  setWindowTitle(windowName);

  switch(m_board) {
    case Board::BOARD_TARANIS_X7 :
      radioUiWidget = new SimulatedUIWidgetX7(simulator, this);
      break;
    case Board::BOARD_TARANIS_X9D  :
    case Board::BOARD_TARANIS_X9DP :
      radioUiWidget = new SimulatedUIWidgetX9(simulator, this);
      break;
    case Board::BOARD_TARANIS_X9E :
      radioUiWidget = new SimulatedUIWidgetX9E(simulator, this);
      break;
    case Board::BOARD_X12S :
    case Board::BOARD_X10  :
      radioUiWidget = new SimulatedUIWidgetX12(simulator, this);
      break;
    default:
      radioUiWidget = new SimulatedUIWidget9X(simulator, this);
      break;
  }

  foreach (keymapHelp_t item, *radioUiWidget->getKeymapHelp())
    keymapHelp.append(item);

  ui->radioUiWidget->layout()->removeItem(ui->radioUiTempSpacer);
  ui->radioUiWidget->layout()->addWidget(radioUiWidget);
  radioUiWidget->setFocusPolicy(Qt::WheelFocus);
  radioUiWidget->setFocus();

  connect(radioUiWidget, &SimulatedUIWidget::customStyleRequest, this, &SimulatorWidget::setUiAreaStyle);

  vJoyLeft = new VirtualJoystickWidget(this, 'L');
  ui->leftStickLayout->addWidget(vJoyLeft);

  vJoyRight = new VirtualJoystickWidget(this, 'R');
  ui->rightStickLayout->addWidget(vJoyRight);

  connect(vJoyLeft, SIGNAL(trimButtonPressed(int)), this, SLOT(onTrimPressed(int)));
  connect(vJoyLeft, SIGNAL(trimButtonReleased()), this, SLOT(onTrimReleased()));
  connect(vJoyLeft, SIGNAL(trimSliderMoved(int,int)), this, SLOT(onTrimSliderMoved(int,int)));

  connect(vJoyRight, SIGNAL(trimButtonPressed(int)), this, SLOT(onTrimPressed(int)));
  connect(vJoyRight, SIGNAL(trimButtonReleased()), this, SLOT(onTrimReleased()));
  connect(vJoyRight, SIGNAL(trimSliderMoved(int,int)), this, SLOT(onTrimSliderMoved(int,int)));

  timer->setInterval(10);
  connect(timer, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
  connect(timer, SIGNAL(timeout()), radioUiWidget, SLOT(updateUi()));
}

SimulatorWidget::~SimulatorWidget()
{
  shutdown();

  if (timer)
    timer->deleteLater();
  if (radioUiWidget)
    delete radioUiWidget;
  if (vJoyLeft)
    delete vJoyLeft;
  if (vJoyRight)
    delete vJoyRight;
#ifdef JOYSTICKS
  if (joystick)
    delete joystick;
#endif

  firmware = NULL;  // Not sure we should delete this but at least release our pointer.
  // NOTE : <simulator> should be deleted (or not) in the parent process which gave it to us in the first place.

  delete ui;
}


/*
 * Public slots/setters
 */

void SimulatorWidget::setSdPath(const QString & sdPath)
{
  setPaths(sdPath, radioDataPath);
}

void SimulatorWidget::setDataPath(const QString & dataPath)
{
  setPaths(sdCardPath, dataPath);
}

void SimulatorWidget::setPaths(const QString & sdPath, const QString & dataPath)
{
  sdCardPath = sdPath;
  radioDataPath = dataPath;
  if (simulator)
    simulator->setSdPath(sdPath, dataPath);
}

void SimulatorWidget::setRadioSettings(const GeneralSettings settings)
{
  radioSettings = settings;
}

/*
 * This function can accept no parameters, a file name (QString is a QBA), or a data array. It will attempt to load radio settings data from one of
 *   several sources into a RadioData object, parse the data, and then pass it on as appropriate to the SimulatorInterface in start().
 * If given no/blank <dataSource>, and setDataPath() was already called, then it will check that directory for "Horus-style" data files.
 * If given a file name, set the <fromFile> parameter to 'true'. This will attempt to load radio settings from said file
 *   and later start the simulator interface in start() using the same data.
 * If <dataSource> is a byte array of data, attempts to load radio settings from there and will also start the simulator interface
 *   with the same data when start() is called.
 * If you already have a valid RadioData structure, call setRadioData() instead.
 */
bool SimulatorWidget::setStartupData(const QByteArray & dataSource, bool fromFile)
{
  RadioData simuData;
  quint16 ret = 1;
  QString error;
  QString fileName(dataSource);

  // If <dataSource> is blank but we have a data path, use that for individual radio/model files.
  if (dataSource.isEmpty() && !radioDataPath.isEmpty()) {
    // If directory structure already exists, try to load data from there.
    // FIXME : need Storage class to return formal error code, not just a boolean, because it would be better
    //   to avoid hard-coding paths like "RADIO" here. E.g. did it fail due to no data at all, or corrupt data, or...?
    if (QDir(QString(radioDataPath).append("/RADIO")).exists()) {
      SdcardFormat sdcard(radioDataPath);
      if (!(ret = sdcard.load(simuData))) {
        error = sdcard.error();
      }
    }
  }
  // Supposedly we're being given a file name to use, try that out.
  else if (fromFile && !fileName.isEmpty()) {
    Storage store = Storage(fileName);
    ret = store.load(simuData);
    if (!ret && QFile(fileName).exists()) {
      error = store.error();
    }
    else {
      if (fileName.endsWith(".otx", Qt::CaseInsensitive)) {
        // no radios can work with .otx files directly, so we load contents into either
        //   a temporary folder (Horus) or local data array (other radios) which we'll save back to .otx upon exit
        if ((ret = setRadioData(&simuData))) {
          startupFromFile = false;
          return true;
        }
      }
      else {
        // the binary file will be read/written directly by the fw interface, save the file name for simulator->start()
        startupData = dataSource;
      }
    }
  }
  // Assume a byte array of radio data was passed, load it.
  else if (!dataSource.isEmpty()) {
    ret = firmware->getEEpromInterface()->load(simuData, (uint8_t *)dataSource.constData(), getEEpromSize(m_board));
    startupData = dataSource;  // save the data for start()
  }
  // we're :-(
  else {
    ret = 0;
    error = tr("Could not determine startup data source.");
  }

  if (!ret) {
    if (error.isEmpty())
      error = tr("Could not load data, possibly wrong format.");
    QMessageBox::critical(this, tr("Data Load Error"), error);
    return false;
  }

  radioSettings = simuData.generalSettings;
  startupFromFile = fromFile;

  return true;
}

bool SimulatorWidget::setRadioData(RadioData * radioData)
{
  bool ret = true;

  saveTempRadioData = (flags & SIMULATOR_FLAGS_STANDALONE);

  if (IS_HORUS(m_board))
    ret = useTempDataPath(true);

  if (ret) {
    if (radioDataPath.isEmpty()) {
      startupData.fill(0, getEEpromSize(m_board));
      if (firmware->getEEpromInterface()->save((uint8_t *)startupData.data(), *radioData, 0, firmware->getCapability(SimulatorVariant)) <= 0) {
        ret = false;
      }
    }
    else {
      ret = saveRadioData(radioData, radioDataPath);
    }
  }

  if (ret)
    radioSettings = radioData->generalSettings;

  return ret;
}

bool SimulatorWidget::setOptions(SimulatorOptions & options, bool withSave)
{
  bool ret = false;

  setSdPath(options.sdPath);

  if (options.startupDataType == SimulatorOptions::START_WITH_FOLDER && !options.dataFolder.isEmpty()) {
    setDataPath(options.dataFolder);
    ret = setStartupData();
  }
  else if (options.startupDataType == SimulatorOptions::START_WITH_SDPATH && !options.sdPath.isEmpty()) {
    setDataPath(options.sdPath);
    ret = setStartupData();
  }
  else if (options.startupDataType == SimulatorOptions::START_WITH_FILE && !options.dataFile.isEmpty()) {
    ret = setStartupData(options.dataFile.toLocal8Bit(), true);
  }
  else {
    QString error = tr("Invalid startup data provided. Plese specify a proper file/path.");
    QMessageBox::critical(this, tr("Simulator Startup Error"), error);
  }

  if (ret && withSave)
    g.profile[radioProfileId].simulatorOptions(options);

  return ret;
}

bool SimulatorWidget::saveRadioData(RadioData * radioData, const QString & path, QString * error)
{
  QString dir = path;
  if (dir.isEmpty())
    dir = radioDataPath;

  if (radioData && !dir.isEmpty()) {
    SdcardFormat sdcard(dir);
    bool ret = sdcard.write(*radioData);
    if (!ret && error)
      *error = sdcard.error();
    return ret;
  }

  return false;
}

bool SimulatorWidget::useTempDataPath(bool deleteOnClose)
{
  if (deleteTempRadioData)
    deleteTempData();

  QTemporaryDir tmpDir(QDir::tempPath() + "/otx-XXXXXX");
  if (tmpDir.isValid()) {
    setDataPath(tmpDir.path());
    tmpDir.setAutoRemove(false);
    deleteTempRadioData = deleteOnClose;
    qDebug() << __FILE__ << __LINE__ << "Created temporary settings directory" << radioDataPath << "with delteOnClose:" << deleteOnClose;
    return true;
  }
  else {
    qDebug() << __FILE__ << __LINE__ << "ERROR : Failed to create temporary settings directory" << radioDataPath;
    return false;
  }
}

// This will save radio data from temporary folder structure back into an .otx file, eg. for Horus.
bool SimulatorWidget::saveTempData()
{
  bool ret = false;
  QString error;
  QString file = g.profile[radioProfileId].simulatorOptions().dataFile;

  if (!file.isEmpty()) {
    RadioData radioData;

    if (radioDataPath.isEmpty()) {
      if (!startupData.isEmpty()) {
        if (!QFile(file).exists()) {
          QFile fh(file);
          if (!fh.open(QIODevice::WriteOnly))
            error = tr("Error saving data: could open file for writing: '%1'").arg(file);
          else
            fh.close();
        }

        if (!firmware->getEEpromInterface()->load(radioData, (uint8_t *)startupData.constData(), getEEpromSize(m_board))) {
          error = tr("Error saving data: could not get data from simulator interface.");
        }
        else {
          radioData.fixModelFilenames();
          ret = true;
        }
      }
    }
    else {
      SdcardFormat sdcard(radioDataPath);
      if (!(ret = sdcard.load(radioData))) {
        error = sdcard.error();
      }
    }
    if (ret) {
      Storage store(file);
      if (!(ret = store.write(radioData)))
        error = store.error();
      else
        qDebug() << __FILE__ << __LINE__ << "Saved radio data to file" << file;
    }
  }

  if (!ret) {
    if (error.isEmpty())
      error = tr("An unexpected error occurred while attempting to save radio data to file '%1'.").arg(file);
    QMessageBox::critical(this, tr("Data Save Error"), error);
  }

  return ret;
}

void SimulatorWidget::deleteTempData()
{
  if (!radioDataPath.isEmpty()) {
    QDir tpath(radioDataPath);
    qDebug() << __FILE__ << __LINE__ << "Deleting temporary settings directory" << tpath.absolutePath();
    tpath.removeRecursively();
    tpath.rmdir(radioDataPath);  // for some reason this is necessary to remove the base folder
  }
}

void SimulatorWidget::saveState()
{
  SimulatorOptions opts = g.profile[radioProfileId].simulatorOptions();
  //opts.windowGeometry = saveGeometry();
  opts.controlsState = saveRadioWidgetsState();
  g.profile[radioProfileId].simulatorOptions(opts);
}

void SimulatorWidget::setUiAreaStyle(const QString & style)
{
  setStyleSheet(style);
}

void SimulatorWidget::captureScreenshot(bool)
{
  if (radioUiWidget)
    radioUiWidget->captureScreenshot();
}

/*
 * Startup
 */

void SimulatorWidget::start()
{
  setupRadioWidgets();
  setupJoysticks();
  restoreRadioWidgetsState();

  if (startupData.isEmpty())
    simulator->start((const char *)0);
  else if (startupFromFile)
    simulator->start(startupData.constData());
  else
    simulator->start(startupData, (flags & SIMULATOR_FLAGS_NOTX) ? false : true);

  setTrims();
  getValues();
  setupTimer();
}

void SimulatorWidget::stop()
{
  if (timer)
    timer->stop();
  if (simulator) {
    simulator->stop();
    if (saveTempRadioData) {
      startupData.fill(0, getEEpromSize(m_board));
      simulator->readEepromData(startupData);
    }
  }
}

void SimulatorWidget::restart()
{
  stop();
  saveState();
  setStartupData(startupData, startupFromFile);
  start();
}

void SimulatorWidget::shutdown()
{
  stop();
  saveState();
  if (saveTempRadioData)
    saveTempData();
  if (deleteTempRadioData)
    deleteTempData();
}

/*
 * Setup
 */

void SimulatorWidget::setRadioProfileId(int value)
{
  radioProfileId = value;
  if (simulator)
    simulator->setVolumeGain(g.profile[radioProfileId].volumeGain());
}

void SimulatorWidget::setupRadioWidgets()
{
  int i, midpos, aIdx;
  QString wname;
  Board::Type board = firmware->getBoard();

  // First clear out any existing widgets.
  if (switches.size()) {
    foreach (RadioWidget * w, switches) {
      ui->radioWidgetsHTLayout->removeWidget(w);
      w->deleteLater();
    }
    switches.clear();
  }
  if (analogs.size()) {
    foreach (RadioWidget * w, analogs) {
      if (w->getType() == RadioWidget::RADIO_WIDGET_KNOB)
        ui->radioWidgetsHTLayout->removeWidget(w);
      else
        ui->VCGridLayout->removeWidget(w);
      w->deleteLater();
    }
    analogs.clear();
  }

  // Now set up new widgets.

  // switches
  Board::SwitchInfo switchInfo;
  Board::SwitchType swcfg;
  for (i = 0; i < getBoardCapability(board, Board::Switches) && i < CPN_MAX_SWITCHES; ++i) {
    if (radioSettings.switchConfig[i] == Board::SWITCH_NOT_AVAILABLE)
      continue;

    swcfg = Board::SwitchType(radioSettings.switchConfig[i]);

    if ((wname = QString(radioSettings.switchName[i])).isEmpty()) {
      switchInfo = getSwitchInfo(board, i);
      wname = QString(switchInfo.name);
    }
    RadioSwitchWidget * sw = new RadioSwitchWidget(swcfg, wname, -1, ui->radioWidgetsHT);
    sw->setIndex(i);
    ui->radioWidgetsHTLayout->addWidget(sw);
    switches.append(sw);
  }

  midpos = (int)floorf(switches.size() / 2.0f);
  aIdx = 0;

  // pots in middle of switches
  for (i = 0; i < getBoardCapability(board, Board::Pots) && i < CPN_MAX_POTS; ++i) {
    if (!radioSettings.isPotAvailable(i))
      continue;

    if ((wname = QString(radioSettings.potName[i])).isEmpty())
      wname = firmware->getAnalogInputName(4 + aIdx + i);

    RadioKnobWidget * pot = new RadioKnobWidget(Board::PotType(radioSettings.potConfig[i]), wname, 0, ui->radioWidgetsHT);
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
  for (i = 0; i < getBoardCapability(board, Board::Sliders) && i + aIdx < CPN_MAX_POTS; ++i) {
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

void SimulatorWidget::setupJoysticks()
{
#ifdef JOYSTICKS
  static bool joysticksEnabled = false;
  if (g.jsSupport() && g.jsCtrl() > -1) {
    int count=0, axe;
    for (int j=0; j < MAX_JOYSTICKS; j++){
      axe = g.joystick[j].stick_axe();
      if (axe >= 0 && axe < MAX_JOYSTICKS) {
        jsmap[axe] = j + 1;
        jscal[axe][0] = g.joystick[j].stick_min();
        jscal[axe][1] = g.joystick[j].stick_med();
        jscal[axe][2] = g.joystick[j].stick_max();
        jscal[axe][3] = g.joystick[j].stick_inv();
        count++;
      }
    }
    if (count<3) {
      QMessageBox::critical(this, tr("Warning"), tr("Joystick enabled but not configured correctly"));
      return;
    }

    if (!joystick)
      joystick = new Joystick(this);
    else
      joystick->close();

    if (joystick && joystick->open(g.jsCtrl())) {
      int numAxes = std::min(joystick->numAxes, MAX_JOYSTICKS);
      for (int j=0; j<numAxes; j++) {
        joystick->sensitivities[j] = 0;
        joystick->deadzones[j] = 0;
      }
      //mode 1,3 -> THR on right
      vJoyRight->setStickConstraint(VirtualJoystickWidget::HOLD_Y, true);
      vJoyRight->setStickConstraint(VirtualJoystickWidget::HOLD_X, true);
      vJoyLeft->setStickConstraint(VirtualJoystickWidget::HOLD_Y, true);
      vJoyLeft->setStickConstraint(VirtualJoystickWidget::HOLD_X, true);
      connect(joystick, SIGNAL(axisValueChanged(int, int)), this, SLOT(onjoystickAxisValueChanged(int, int)));
      joysticksEnabled = true;
    }
    else {
      QMessageBox::critical(this, tr("Warning"), tr("Cannot open joystick, joystick disabled"));
    }
  }
  else if (joysticksEnabled && joystick) {
    disconnect(joystick, 0, this, 0);
    vJoyRight->setStickConstraint(VirtualJoystickWidget::HOLD_Y, false);
    vJoyRight->setStickConstraint(VirtualJoystickWidget::HOLD_X, false);
    vJoyLeft->setStickConstraint(VirtualJoystickWidget::HOLD_Y, false);
    vJoyLeft->setStickConstraint(VirtualJoystickWidget::HOLD_X, false);
  }
#endif
}

void SimulatorWidget::setupTimer()
{
  if (!timer)
    return;

  if (timer->isActive())
    timer->stop();

  timer->start();
}

void SimulatorWidget::restoreRadioWidgetsState()
{
  RadioWidget::RadioWidgetState state;
  QMap<int, QByteArray> switchesMap;
  QMap<int, QByteArray> analogsMap;
  QList<QByteArray> states = g.profile[radioProfileId].simulatorOptions().controlsState;

  foreach (QByteArray ba, states) {
    QDataStream stream(ba);
    stream >> state;
    if (state.type == RadioWidget::RADIO_WIDGET_SWITCH)
      switchesMap.insert(state.index, ba);
    else
      analogsMap.insert(state.index, ba);
  }

  for (int i = 0; i < analogs.size(); ++i) {
    if (analogsMap.contains(analogs[i]->getIndex()))
      analogs[i]->setStateData(analogsMap.value(analogs[i]->getIndex()));
  }

  for (int i = 0; i < switches.size(); ++i) {
    if (switchesMap.contains(switches[i]->getIndex()))
      switches[i]->setStateData(switchesMap.value(switches[i]->getIndex()));
  }

  // Set throttle stick down and locked, side depends on mode
  if (radioSettings.stickMode & 1) {
    vJoyLeft->setStickConstraint(VirtualJoystickWidget::HOLD_Y, true);
    vJoyLeft->setStickY(1);
  }
  else {
    vJoyRight->setStickConstraint(VirtualJoystickWidget::HOLD_Y, true);
    vJoyRight->setStickY(1);
  }
}

QList<QByteArray> SimulatorWidget::saveRadioWidgetsState()
{
  QList<QByteArray> states;

  for (int i = 0; i < analogs.size(); ++i)
    states.append(analogs[i]->getStateData());

  for (int i = 0; i < switches.size(); ++i)
    states.append(switches[i]->getStateData());

  return states;
}

/*
 * Input/Output handlers for SimulatorInterface
 */

// Read various values from firmware simulator and populate values in this UI
void SimulatorWidget::setValues()
{
  int currentPhase = simulator->getPhase();

  // display current flight mode in window title
  if (currentPhase != lastPhase) {
    lastPhase = currentPhase;
    QString phase_name = QString(simulator->getPhaseName(currentPhase));
    if (phase_name.isEmpty())
      phase_name = QString::number(currentPhase);
    setWindowTitle(windowName + QString(" - Flight Mode %1").arg(phase_name));
  }
}

// "get" values from this UI and send them to the firmware simulator.
void SimulatorWidget::getValues()
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
void SimulatorWidget::setTrims()
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

//void SimulatorDialog::showEvent(QShowEvent *)
//{
//  if (firstShow && isVisible()) {
//    firstShow = false;
//  }
//}

//void SimulatorDialog::closeEvent(QCloseEvent *)
//{
//}

void SimulatorWidget::mousePressEvent(QMouseEvent *event)
{
  if (radioUiWidget)
    radioUiWidget->mousePressEvent(event);
}

void SimulatorWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if (radioUiWidget)
    radioUiWidget->mouseReleaseEvent(event);
}

void SimulatorWidget::wheelEvent(QWheelEvent *event)
{
  if (radioUiWidget)
    radioUiWidget->wheelEvent(event);
}

void SimulatorWidget::onTimerEvent()
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
}

void SimulatorWidget::onTrimPressed(int which)
{
  trimPressed = which;
}

void SimulatorWidget::onTrimReleased()
{
  trimPressed = TRIM_NONE;
}

void SimulatorWidget::onTrimSliderMoved(int which, int value)
{
  simulator->setTrim(which, value);
}

void SimulatorWidget::centerSticks()
{
  if (vJoyLeft)
    vJoyLeft->centerStick();

  if (vJoyRight)
    vJoyRight->centerStick();
}

void SimulatorWidget::onjoystickAxisValueChanged(int axis, int value)
{
#ifdef JOYSTICKS
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
#endif
}
