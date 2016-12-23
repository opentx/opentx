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
#include "appdata.h"
#include "lcdwidget.h"
#include <iostream>
#include "helpers.h"
#include "simulatorinterface.h"
#include "virtualjoystickwidget.h"

int SimulatorDialog::screenshotIdx = 0;
SimulatorDialog * traceCallbackInstance = 0;

void traceCb(const char * text)
{
  // divert C callback into simulator instance
  if (traceCallbackInstance) {
    traceCallbackInstance->traceCallback(text);
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

void SimulatorDialog::wheelEvent (QWheelEvent *event)
{
  if ( event->delta() != 0) {
    simulator->wheelEvent(event->delta() > 0 ? 1 : -1);
  }
}

SimulatorDialog::SimulatorDialog(QWidget * parent, SimulatorInterface *simulator, unsigned int flags):
  QDialog(parent),
  flags(flags),
  timer(NULL),
  lightOn(false),
  simulator(simulator),
  lastPhase(-1),
  beepVal(0),
  TelemetrySimu(0),
  TrainerSimu(0),
  DebugOut(0),
  buttonPressed(0),
  trimPressed(TRIM_NONE),
  middleButtonPressed(false)
{
  setWindowFlags(Qt::Window);
  //shorcut for telemetry simulator
  // new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_T), this, SLOT(openTelemetrySimulator()));
  new QShortcut(QKeySequence(Qt::Key_F4), this, SLOT(openTelemetrySimulator()));
  new QShortcut(QKeySequence(Qt::Key_F5), this, SLOT(openTrainerSimulator()));
  new QShortcut(QKeySequence(Qt::Key_F6), this, SLOT(openDebugOutput()));
  new QShortcut(QKeySequence(Qt::Key_F7), this, SLOT(luaReload()));
  traceCallbackInstance = this;
}

SimulatorDialog::~SimulatorDialog()
{
  traceCallbackInstance = 0;
  delete timer;
  delete simulator;
}

void SimulatorDialog::closeEvent (QCloseEvent *)
{
  simulator->stop();
  timer->stop();
  //g.simuWinGeo(GetCurrentFirmware()->getId(), saveGeometry());
}

void SimulatorDialog::showEvent(QShowEvent * event)
{
  static bool firstShow = true;
  if (firstShow) {
    if (flags & SIMULATOR_FLAGS_STICK_MODE_LEFT) {
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
  if (event->button() == Qt::MidButton) {
    middleButtonPressed = true;
  }
}

void SimulatorDialog::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::MidButton) {
    middleButtonPressed = false;
  }
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

void SimulatorDialog::openTelemetrySimulator()
{
  // allow only one instance
  if (TelemetrySimu == 0) {
    TelemetrySimu = new TelemetrySimulator(this, simulator);
    TelemetrySimu->show();
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
  }
  else if (!TrainerSimu->isVisible()) {
    TrainerSimu->show();
  }
}

void SimulatorDialog::openDebugOutput()
{
  // allow only one instance, but install signal handler to catch dialog destruction just in case
  if (DebugOut == 0) {
    DebugOut = new DebugOutput(this);
    QObject::connect(DebugOut, SIGNAL(destroyed()), this, SLOT(onDebugOutputClose()));
    DebugOut->traceCallback(traceBuffer);
    DebugOut->show();
  }
  else if (!DebugOut->isVisible()) {
    DebugOut->show();
  }
}

void SimulatorDialog::luaReload()
{
  // force a reload of the lua environment (as if a standalone script were run)
  simulator->setLuaStateReloadPermanentScripts();
}

void SimulatorDialog::onDebugOutputClose()
{
  DebugOut = 0;
}


void SimulatorDialog::keyPressEvent (QKeyEvent *event)
{
  switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
      buttonPressed = Qt::Key_Enter;
      break;
    case Qt::Key_Escape:
    case Qt::Key_Backspace:
      buttonPressed = Qt::Key_Escape;
      break;
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Right:
    case Qt::Key_Left:
    case Qt::Key_Minus:
    case Qt::Key_Plus:
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:    
      buttonPressed = event->key();
      break;
    case Qt::Key_X:
      simulator->wheelEvent(-1);
      break;
    case Qt::Key_C:
      simulator->wheelEvent(1);
      break;

  }
}

void SimulatorDialog::keyReleaseEvent(QKeyEvent * event)
{
  switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Escape:
    case Qt::Key_Backspace:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Right:
    case Qt::Key_Left:
    case Qt::Key_Plus:
    case Qt::Key_Minus:
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:
      buttonPressed = 0;
      break;
  }
}

void SimulatorDialog::setupTimer()
{
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
  timer->start(10);
}

template <class T>
void SimulatorDialog::initUi(T * ui)
{
  ui->setupUi(this);

  windowName = tr("Simulating Radio (%1)").arg(GetCurrentFirmware()->getName());
  setWindowTitle(windowName);

  simulator->setSdPath(g.profile[g.id()].sdPath());
  simulator->setVolumeGain(g.profile[g.id()].volumeGain());

  lcd = ui->lcd;
  lcd->setData(simulator->getLcd(), lcdWidth, lcdHeight, lcdDepth);

  tabWidget = ui->tabWidget;
  pots = findWidgets<QDial *>(this, "pot%1");
  potLabels = findWidgets<QLabel *>(this, "potLabel%1");
  potValues = findWidgets<QLabel *>(this, "potValue%1");
  sliders = findWidgets<QSlider *>(this, "slider%1");

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

  setupOutputsDisplay();
  setupGVarsDisplay();
  setTrims();

  //restoreGeometry(g.simuWinGeo(GetCurrentFirmware()->getId()));

  if (flags & SIMULATOR_FLAGS_NOTX)
    tabWidget->setCurrentIndex(1);

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

  tabWidget->insertTab(1, outputsWidget, QString(tr("Outputs")));

  // populate outputs
  int outputs = std::min(32, GetCurrentFirmware()->getCapability(Outputs));
  int column = 0;
  for (int i=0; i<outputs; i++) {
    QLabel * label = new QLabel(tabWidget);
    label->setText(" " + RawSource(SOURCE_TYPE_CH, i).toString() + " ");
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    channelsLayout->addWidget(label, 0, column, 1, 1);

    QSlider * slider = new QSlider(tabWidget);
    slider->setEnabled(false);
    slider->setMinimum(-1024);
    slider->setMaximum(1024);
    slider->setPageStep(128);
    slider->setTracking(false);
    slider->setOrientation(Qt::Vertical);
    slider->setInvertedAppearance(false);
    slider->setTickPosition(QSlider::TicksRight);
    slider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QLabel * value = new QLabel(tabWidget);
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
  int switches = GetCurrentFirmware()->getCapability(LogicalSwitches);
  int rows = switches / (switches > 16 ? 4 : 2);
  for (int i=0; i<switches; i++) {
    QFrame * swtch = createLogicalSwitch(tabWidget, i, logicalSwitchLabels);
    logicalSwitchesLayout->addWidget(swtch, i / rows, i % rows, 1, 1);
  }
}

void SimulatorDialog::setupGVarsDisplay()
{
  int fmodes = GetCurrentFirmware()->getCapability(FlightModes);
  int gvars = GetCurrentFirmware()->getCapability(Gvars);
  if (gvars>0) {
    // setup GVars tab
    QWidget * gvarsWidget = new QWidget();
    QGridLayout * gvarsLayout = new QGridLayout(gvarsWidget);
    tabWidget->addTab(gvarsWidget, QString(tr("GVars")));

    for (int fm=0; fm<fmodes; fm++) {
      QLabel * label = new QLabel(tabWidget);
      label->setText(QString("FM%1").arg(fm));
      label->setAlignment(Qt::AlignCenter);
      gvarsLayout->addWidget(label, 0, fm+1);
    }
    for (int i=0; i<gvars; i++) {
      QLabel * label = new QLabel(tabWidget);
      label->setText(QString("GV%1").arg(i+1));
      label->setAutoFillBackground(true);
      if ((i % 2) ==0 ) {
        label->setStyleSheet("QLabel { background-color: rgb(220, 220, 220) }");
      }
      gvarsLayout->addWidget(label, i+1, 0);
      for (int fm=0; fm<fmodes; fm++) {
        QLabel * value = new QLabel(tabWidget);
        value->setAutoFillBackground(true);
        value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if ((i % 2) ==0 ) {
          value->setStyleSheet("QLabel { background-color: rgb(220, 220, 220) }");
        }
        gvarValues << value;
        gvarsLayout->addWidget(value, i+1, fm+1);
      }
    }
  }
}

void SimulatorDialog::onButtonPressed(int value)
{
  if (value == Qt::Key_Print) {
    QString fileName = "";
    if (!g.snapToClpbrd()) {
      fileName = QString("screenshot-%1.png").arg(++screenshotIdx);
    }
    lcd->makeScreenshot(fileName);
  }
  else {
    buttonPressed = value;
  }
}

void SimulatorDialog::onTimerEvent()
{
  static unsigned int lcd_counter = 0;
  if (!simulator->timer10ms()) {
    QMessageBox::critical(this, "Companion", tr("Firmware %1 error: %2").arg(GetCurrentFirmware()->getName()).arg(simulator->getError()));
    timer->stop();
    return;
  }

  getValues();

  if (tabWidget->currentIndex()==0) {
    bool lightEnable;
    if (simulator->lcdChanged(lightEnable)) {
      lcd->onLcdChanged(lightEnable);
      if (lightOn != lightEnable) {
        setLightOn(lightEnable);
        lightOn = lightEnable;
      }
    }
  }

  // display current flight mode in window title
  unsigned int currentPhase = simulator->getPhase();
  if (currentPhase != lastPhase) {
    lastPhase = currentPhase;
    const char * phase_name = simulator->getPhaseName(currentPhase);
    if (phase_name && phase_name[0]) {
      setWindowTitle(windowName + QString(" - Flight Mode %1").arg(QString(phase_name)));
    }
    else {
      setWindowTitle(windowName + QString(" - Flight Mode %1").arg(simulator->getPhase()));
    }
  }

  if (!(lcd_counter++ % 5)) {

    setValues();

    setTrims();

    centerSticks();

    updateBeepButton();

    if (beepVal) {
      beepVal = 0;
      QApplication::beep();
    }

  }

  updateDebugOutput();
}

void SimulatorDialog::centerSticks()
{
  if (vJoyLeft)
    vJoyLeft->centerStick();

  if (vJoyRight)
    vJoyRight->centerStick();
}

void SimulatorDialog::start(QByteArray & eeprom)
{
  lastPhase = -1;
  numGvars = GetCurrentFirmware()->getCapability(Gvars);
  numFlightModes = GetCurrentFirmware()->getCapability(FlightModes);
  simulator->start(eeprom, (flags & SIMULATOR_FLAGS_NOTX) ? false : true);
  getValues();
  setupTimer();
}

void SimulatorDialog::start(const char * filename)
{
  lastPhase = -1;
  numGvars = GetCurrentFirmware()->getCapability(Gvars);
  numFlightModes = GetCurrentFirmware()->getCapability(FlightModes);
  simulator->start(filename);
  getValues();
  setupTimer();
}

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

inline int chVal(int val)
{
  return qMin(1024, qMax(-1024, val));
}

void SimulatorDialog::setValues()
{
  TxOutputs outputs;
  simulator->getValues(outputs);
  Trims trims;
  simulator->getTrims(trims);

  for (int i=0; i<GetCurrentFirmware()->getCapability(Outputs); i++) {
    if (i < channelSliders.size()) {
      channelSliders[i]->setValue(chVal(outputs.chans[i]));
      channelValues[i]->setText(QString("%1").arg((qreal)outputs.chans[i]*100/1024, 0, 'f', 1));
    }
  }

  QString CSWITCH_ON = "QLabel { background-color: #4CC417 }";
  QString CSWITCH_OFF = "QLabel { }";

  for (int i=0; i<GetCurrentFirmware()->getCapability(LogicalSwitches); i++) {
    logicalSwitchLabels[i]->setStyleSheet(outputs.vsw[i] ? CSWITCH_ON : CSWITCH_OFF);
  }

  for (unsigned int gv=0; gv<numGvars; gv++) {
    for (unsigned int fm=0; fm<numFlightModes; fm++) {
      gvarValues[gv*numFlightModes+fm]->setText(QString((fm==lastPhase)?"<b>%1</b>":"%1").arg(outputs.gvars[fm][gv]));
    }
  }

  if (outputs.beep) {
    beepVal = outputs.beep;
  }
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
    else if (stick >= 5 && stick < 5+pots.count()) {
      pots[stick-5]->setValue(stickval);
    }
  }
}
#endif

#include "simulatordialog9x.cpp"
#include "simulatordialogtaranis.cpp"
#include "simulatordialogflamenco.cpp"
#include "simulatordialoghorus.cpp"
