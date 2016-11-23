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
#include "sliderwidget.h"

#define GBALL_SIZE  20
#define RESX        1024

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
  simulator->wheelEvent(event->delta() > 0 ? 1 : -1);
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
  trimPressed (TRIM_NONE),
  middleButtonPressed(false)
{
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

void SimulatorDialog::onTrimPressed()
{
  if (sender()->objectName() == QString("trimHL_L"))
    trimPressed = TRIM_LH_L;
  else if (sender()->objectName() == QString("trimHL_R"))
    trimPressed = TRIM_LH_R;
  else if (sender()->objectName() == QString("trimVL_D"))      
    trimPressed = TRIM_LV_DN;
  else if (sender()->objectName() == QString("trimVL_U"))     
    trimPressed = TRIM_LV_UP;
  else if (sender()->objectName() == QString("trimVR_D"))
    trimPressed = TRIM_RV_DN;
  else if (sender()->objectName() == QString("trimVR_U"))
    trimPressed = TRIM_RV_UP;
  else if (sender()->objectName() == QString("trimHR_L")) 
    trimPressed = TRIM_RH_L;
  else if (sender()->objectName() == QString("trimHR_R"))
    trimPressed = TRIM_RH_R;
}

void SimulatorDialog::onTrimReleased()
{
  trimPressed = TRIM_NONE;
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

  lcd = ui->lcd;
  leftStick = ui->leftStick;
  rightStick = ui->rightStick;
  pots = findWidgets<QDial *>(this, "pot%1");
  potLabels = findWidgets<QLabel *>(this, "potLabel%1");
  potValues = findWidgets<QLabel *>(this, "potValue%1");
  sliders = findWidgets<QSlider *>(this, "slider%1");

  trimHLeft = ui->trimHLeft;
  trimVLeft = ui->trimVLeft;
  trimHRight = ui->trimHRight;
  trimVRight = ui->trimVRight;
  tabWidget = ui->tabWidget;
  leftXPerc = ui->leftXPerc;
  leftYPerc = ui->leftYPerc;
  rightXPerc = ui->rightXPerc;
  rightYPerc = ui->rightYPerc;

  setupSticks();

  resize(0, 0); // to force min height, min width
  setFixedSize(width(), height());

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
            nodeRight->setCenteringY(false);   //mode 1,3 -> THR on right
            ui->holdRightY->setChecked(true);
            nodeRight->setCenteringX(false);   //mode 1,3 -> THR on right
            ui->holdRightX->setChecked(true);
            nodeLeft->setCenteringY(false);   //mode 1,3 -> THR on right
            ui->holdLeftY->setChecked(true);
            nodeLeft->setCenteringX(false);   //mode 1,3 -> THR on right
            ui->holdLeftX->setChecked(true);
            connect(joystick, SIGNAL(axisValueChanged(int, int)), this, SLOT(onjoystickAxisValueChanged(int, int)));
          }
          else {
            QMessageBox::critical(this, tr("Warning"), tr("Cannot open joystick, joystick disabled"));
          }
        }
      }
    }
#endif

  windowName = tr("Simulating Radio (%1)").arg(GetCurrentFirmware()->getName());
  setWindowTitle(windowName);

  simulator->setSdPath(g.profile[g.id()].sdPath());
  simulator->setVolumeGain(g.profile[g.id()].volumeGain());
  lcd->setData(simulator->getLcd(), lcdWidth, lcdHeight, lcdDepth);

  if (flags & SIMULATOR_FLAGS_STICK_MODE_LEFT) {
    nodeLeft->setCenteringY(false);
    ui->holdLeftY->setChecked(true);
  }
  else {
    nodeRight->setCenteringY(false);
    ui->holdRightY->setChecked(true);
  }

  setTrims();

  int outputs = std::min(32, GetCurrentFirmware()->getCapability(Outputs));
  if (outputs <= 16) {
    // hide second Outputs tab
    tabWidget->removeTab(tabWidget->indexOf(ui->outputs2));
  }
  else {
    tabWidget->setTabText(tabWidget->indexOf(ui->outputs), tr("Outputs") + QString(" 1-%1").arg(16));
    tabWidget->setTabText(tabWidget->indexOf(ui->outputs2), tr("Outputs") + QString(" 17-%1").arg(outputs));
  }
  for (int i=0; i<outputs; i++) {
    QGridLayout * outputTab = ui->channelsLayout;
    int column = i / (std::min(16,outputs)/2);
    int line =   i % (std::min(16,outputs)/2);
    if (i >= 16 ) {
      outputTab = ui->channelsLayout2;
      column = (i-16) / (std::min(16,outputs-16)/2);
      line =   (i-16) % (std::min(16,outputs-16)/2);
    }
    QLabel * label = new QLabel(tabWidget);
    label->setText(RawSource(SOURCE_TYPE_CH, i).toString());
    outputTab->addWidget(label, line, column == 0 ? 0 : 5, 1, 1);

    QSlider * slider = new QSlider(tabWidget);
    slider->setEnabled(false);
    /*slider->setMaximumSize(QSize(16777215, 18));
    slider->setStyleSheet(QString::fromUtf8("QSlider::sub-page:horizontal:disabled {\n"
    "border-color: #999;\n"
    "}\n"
    "\n"
    "QSlider::add-page:horizontal:disabled {\n"
    "border-color: #999;\n"
    "}\n"
    "\n"
    "QSlider::handle:horizontal:disabled {\n"
    "background: #0000CC;\n"
    "border: 1px solid #aaa;\n"
    "border-radius: 4px;\n"
    "}")); */
    slider->setMinimum(-1024);
    slider->setMaximum(1024);
    slider->setPageStep(128);
    slider->setTracking(false);
    slider->setOrientation(Qt::Horizontal);
    slider->setInvertedAppearance(false);
    slider->setTickPosition(QSlider::TicksBelow);
    channelSliders << slider;
    outputTab->addWidget(slider, line, column == 0 ? 1 : 4, 1, 1);

    QLabel * value = new QLabel(tabWidget);
    value->setMinimumSize(QSize(50, 0));
    value->setAlignment(Qt::AlignCenter);
    channelValues << value;
    outputTab->addWidget(value, line, column == 0 ? 2 : 3, 1, 1);
  }

  int switches = GetCurrentFirmware()->getCapability(LogicalSwitches);
  for (int i=0; i<switches; i++) {
    QFrame * swtch = createLogicalSwitch(tabWidget, i, logicalSwitchLabels);
    ui->logicalSwitchesLayout->addWidget(swtch, i / (switches/2), i % (switches/2), 1, 1);
    if (outputs > 16) {
      // repeat logical switches on second outputs tab
      swtch = createLogicalSwitch(tabWidget, i, logicalSwitchLabels2);
      ui->logicalSwitchesLayout2->addWidget(swtch, i / (switches/2), i % (switches/2), 1, 1);
    }
  }

  int fmodes = GetCurrentFirmware()->getCapability(FlightModes);
  int gvars = GetCurrentFirmware()->getCapability(Gvars);
  if (gvars>0) {
    for (int fm=0; fm<fmodes; fm++) {
      QLabel * label = new QLabel(tabWidget);
      label->setText(QString("FM%1").arg(fm));
      label->setAlignment(Qt::AlignCenter);
      ui->gvarsLayout->addWidget(label, 0, fm+1);
    }
    for (int i=0; i<gvars; i++) {
      QLabel * label = new QLabel(tabWidget);
      label->setText(QString("GV%1").arg(i+1));
      label->setAutoFillBackground(true);
      if ((i % 2) ==0 ) {
        label->setStyleSheet("QLabel { background-color: rgb(220, 220, 220) }");
      }
      ui->gvarsLayout->addWidget(label, i+1, 0);
      for (int fm=0; fm<fmodes; fm++) {
        QLabel * value = new QLabel(tabWidget);
        value->setAutoFillBackground(true);
        value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if ((i % 2) ==0 ) {
          value->setStyleSheet("QLabel { background-color: rgb(220, 220, 220) }");
        }
        gvarValues << value;
        ui->gvarsLayout->addWidget(value, i+1, fm+1);
      }
    }
  }

  if (flags & SIMULATOR_FLAGS_NOTX) {
    ui->tabWidget->setCurrentWidget(ui->outputs);
  }
  else {
    ui->tabWidget->setCurrentWidget(ui->simu);
  }
}

QFrame * SimulatorDialog::createLogicalSwitch(QWidget * parent, int switchNo, QVector<QLabel *> & labels)
{
    QFrame * swtch = new QFrame(parent);
    swtch->setAutoFillBackground(true);
    swtch->setFrameShape(QFrame::Panel);
    swtch->setFrameShadow(QFrame::Raised);
    swtch->setLineWidth(2);
    QVBoxLayout * layout = new QVBoxLayout(swtch);
    layout->setContentsMargins(2, 2, 2, 2);
    QLabel * label = new QLabel(swtch);
    label->setText(RawSwitch(SWITCH_TYPE_VIRTUAL, switchNo+1).toString());
    label->setAlignment(Qt::AlignCenter);
    labels << label;
    layout->addWidget(label);
    return swtch;
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
  if (leftStick->scene())
    nodeLeft->stepToCenter();

  if (rightStick->scene())
    nodeRight->stepToCenter();
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
  Trims trims;
  simulator->getTrims(trims);

  int trimMin = -125, trimMax = +125;
  if (trims.extended) {
    trimMin = -500;
    trimMax = +500;
  }
  trimHLeft->setRange(trimMin, trimMax);  trimHLeft->setValue(trims.values[0]);
  trimVLeft->setRange(trimMin, trimMax);  trimVLeft->setValue(trims.values[1]);
  trimVRight->setRange(trimMin, trimMax); trimVRight->setValue(trims.values[2]);
  trimHRight->setRange(trimMin, trimMax); trimHRight->setValue(trims.values[3]);
}

inline int chVal(int val)
{
  return qMin(1024, qMax(-1024, val));
}

void SimulatorDialog::on_trimHLeft_valueChanged(int value)
{
  simulator->setTrim(0, value);
}

void SimulatorDialog::on_trimVLeft_valueChanged(int value)
{
  simulator->setTrim(1, value);
}

void SimulatorDialog::on_trimVRight_valueChanged(int value)
{
  simulator->setTrim(2, value);
}

void SimulatorDialog::on_trimHRight_valueChanged(int value)
{
  simulator->setTrim(3, value);
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

  leftXPerc->setText(QString("X %1%").arg((qreal)nodeLeft->getX()*100+trims.values[0]/5, 2, 'f', 0));
  leftYPerc->setText(QString("Y %1%").arg((qreal)nodeLeft->getY()*-100+trims.values[1]/5, 2, 'f', 0));

  rightXPerc->setText(QString("X %1%").arg((qreal)nodeRight->getX()*100+trims.values[3]/5, 2, 'f', 0));
  rightYPerc->setText(QString("Y %1%").arg((qreal)nodeRight->getY()*-100+trims.values[2]/5, 2, 'f', 0));

  QString CSWITCH_ON = "QLabel { background-color: #4CC417 }";
  QString CSWITCH_OFF = "QLabel { }";

  for (int i=0; i<GetCurrentFirmware()->getCapability(LogicalSwitches); i++) {
    logicalSwitchLabels[i]->setStyleSheet(outputs.vsw[i] ? CSWITCH_ON : CSWITCH_OFF);
    if (!logicalSwitchLabels2.isEmpty()) {
      logicalSwitchLabels2[i]->setStyleSheet(outputs.vsw[i] ? CSWITCH_ON : CSWITCH_OFF);
    }
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

void SimulatorDialog::setupSticks()
{
  QGraphicsScene *leftScene = new QGraphicsScene(leftStick);
  leftScene->setItemIndexMethod(QGraphicsScene::NoIndex);
  leftStick->setScene(leftScene);

  // leftStick->scene()->addLine(0,10,20,30);

  QGraphicsScene *rightScene = new QGraphicsScene(rightStick);
  rightScene->setItemIndexMethod(QGraphicsScene::NoIndex);
  rightStick->setScene(rightScene);

  // rightStick->scene()->addLine(0,10,20,30);

  nodeLeft = new Node();
  nodeLeft->setPos(-GBALL_SIZE/2,-GBALL_SIZE/2);
  nodeLeft->setBallSize(GBALL_SIZE);
  leftScene->addItem(nodeLeft);

  nodeRight = new Node();
  nodeRight->setPos(-GBALL_SIZE/2,-GBALL_SIZE/2);
  nodeRight->setBallSize(GBALL_SIZE);
  rightScene->addItem(nodeRight);
}

void SimulatorDialog::resizeEvent(QResizeEvent *event)
{
  if (leftStick->scene()) {
    QRect qr = leftStick->contentsRect();
    qreal w  = (qreal)qr.width()  - GBALL_SIZE;
    qreal h  = (qreal)qr.height() - GBALL_SIZE;
    qreal cx = (qreal)qr.width()/2;
    qreal cy = (qreal)qr.height()/2;
    leftStick->scene()->setSceneRect(-cx,-cy,w,h);

    QPointF p = nodeLeft->pos();
    p.setX(qMin(cx, qMax(p.x(), -cx)));
    p.setY(qMin(cy, qMax(p.y(), -cy)));
    nodeLeft->setPos(p);
  }

  if (rightStick->scene()) {
    QRect qr = rightStick->contentsRect();
    qreal w  = (qreal)qr.width()  - GBALL_SIZE;
    qreal h  = (qreal)qr.height() - GBALL_SIZE;
    qreal cx = (qreal)qr.width()/2;
    qreal cy = (qreal)qr.height()/2;
    rightStick->scene()->setSceneRect(-cx,-cy,w,h);

    QPointF p = nodeRight->pos();
    p.setX(qMin(cx, qMax(p.x(), -cx)));
    p.setY(qMin(cy, qMax(p.y(), -cy)));
    nodeRight->setPos(p);
  }
  QDialog::resizeEvent(event);
}

void SimulatorDialog::on_holdLeftX_clicked(bool checked)
{
  nodeLeft->setCenteringX(!checked);
}

void SimulatorDialog::on_holdLeftY_clicked(bool checked)
{
  nodeLeft->setCenteringY(!checked);
}

void SimulatorDialog::on_holdRightX_clicked(bool checked)
{
  nodeRight->setCenteringX(!checked);
}

void SimulatorDialog::on_holdRightY_clicked(bool checked)
{
  nodeRight->setCenteringY(!checked);
}

void SimulatorDialog::on_FixLeftX_clicked(bool checked)
{
  nodeLeft->setFixedX(checked);
}

void SimulatorDialog::on_FixLeftY_clicked(bool checked)
{
  nodeLeft->setFixedY(checked);
}

void SimulatorDialog::on_FixRightX_clicked(bool checked)
{
  nodeRight->setFixedX(checked);
}

void SimulatorDialog::on_FixRightY_clicked(bool checked)
{
  nodeRight->setFixedY(checked);
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
       nodeRight->setY(-stickval/1024.0);
    } 
    else if (stick==2) {
      nodeRight->setX(stickval/1024.0);
    } 
    else if (stick==3) {
      nodeLeft->setY(-stickval/1024.0);
    } 
    else if (stick==4) {
      nodeLeft->setX(stickval/1024.0);
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
