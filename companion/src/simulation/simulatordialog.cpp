#include "simulatordialog.h"
#include "ui_simulatordialog-9x.h"
#include "ui_simulatordialog-taranis.h"
#include <iostream>
#include "helpers.h"
#include "simulatorinterface.h"

#define GBALL_SIZE  20
#define RESX        1024

int SimulatorDialog::screenshotIdx = 0;

SimulatorDialog::SimulatorDialog(QWidget * parent, unsigned int flags):
  QDialog(parent),
  flags(flags),
  dialP_4(NULL),
  timer(NULL),
  lightOn(false),
  simulator(NULL),
  lastPhase(-1),
  beepVal(0),
  buttonPressed(0),
  trimPressed (TRIM_NONE),
  middleButtonPressed(false)
{
  //shorcut for telemetry simulator
  // new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_T), this, SLOT(openTelemetrySimulator()));
  new QShortcut(QKeySequence(Qt::Key_F4), this, SLOT(openTelemetrySimulator()));
}

uint32_t SimulatorDialog9X::switchstatus = 0;

SimulatorDialog9X::SimulatorDialog9X(QWidget * parent, unsigned int flags):
  SimulatorDialog(parent, flags),
  ui(new Ui::SimulatorDialog9X),
  beepShow(0)
{
  lcdWidth = 128;
  lcdDepth = 1;

  initUi<Ui::SimulatorDialog9X>(ui);

  backLight = g.backLight();
  if (backLight > 4) backLight = 0;
  switch (backLight) {
    case 1:
      ui->lcd->setBackgroundColor(166,247,159);
      break;
    case 2:
      ui->lcd->setBackgroundColor(247,159,166);
      break;
    case 3:
      ui->lcd->setBackgroundColor(255,195,151);
      break;
    case 4:
      ui->lcd->setBackgroundColor(247,242,159);
      break;
    default:
      ui->lcd->setBackgroundColor(159,165,247);
      break;
  }

  //restore switches
  if (g.simuSW())
    restoreSwitches();

  ui->trimHR_L->setText(QString::fromUtf8(leftArrow));
  ui->trimHR_R->setText(QString::fromUtf8(rightArrow));
  ui->trimVR_U->setText(QString::fromUtf8(upArrow));
  ui->trimVR_D->setText(QString::fromUtf8(downArrow));
  ui->trimHL_L->setText(QString::fromUtf8(leftArrow));
  ui->trimHL_R->setText(QString::fromUtf8(rightArrow));
  ui->trimVL_U->setText(QString::fromUtf8(upArrow));
  ui->trimVL_D->setText(QString::fromUtf8(downArrow));

  connect(ui->dialP_1, SIGNAL(valueChanged(int)), this, SLOT(dialChanged()));
  connect(ui->dialP_2, SIGNAL(valueChanged(int)), this, SLOT(dialChanged()));
  connect(ui->dialP_3, SIGNAL(valueChanged(int)), this, SLOT(dialChanged()));
  connect(ui->cursor, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  connect(ui->menu, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  connect(ui->trimHR_L, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimHR_R, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimVR_U, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimVR_D, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimHL_R, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimHL_L, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimVL_U, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimVL_D, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimHR_L, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimHR_R, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimVR_U, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimVR_D, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimHL_R, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimHL_L, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimVL_U, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimVL_D, SIGNAL(released()), this, SLOT(onTrimReleased()));
}

SimulatorDialog9X::~SimulatorDialog9X()
{
  saveSwitches();
  delete ui;
}

uint32_t SimulatorDialogTaranis::switchstatus = 0;

SimulatorDialogTaranis::SimulatorDialogTaranis(QWidget * parent, unsigned int flags):
  SimulatorDialog(parent, flags),
  ui(new Ui::SimulatorDialogTaranis)
{
  lcdWidth = 212;
  lcdDepth = 4;

  initUi<Ui::SimulatorDialogTaranis>(ui);
  dialP_4 = ui->dialP_4;

  ui->lcd->setBackgroundColor(47, 123, 227);

  //restore switches
  if (g.simuSW())
    restoreSwitches();

  ui->trimHR_L->setText(QString::fromUtf8(leftArrow));
  ui->trimHR_R->setText(QString::fromUtf8(rightArrow));
  ui->trimVR_U->setText(QString::fromUtf8(upArrow));
  ui->trimVR_D->setText(QString::fromUtf8(downArrow));
  ui->trimHL_L->setText(QString::fromUtf8(leftArrow));
  ui->trimHL_R->setText(QString::fromUtf8(rightArrow));
  ui->trimVL_U->setText(QString::fromUtf8(upArrow));
  ui->trimVL_D->setText(QString::fromUtf8(downArrow));

  connect(ui->cursor, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  connect(ui->menu, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  connect(ui->trimHR_L, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimHR_R, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimVR_U, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimVR_D, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimHL_R, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimHL_L, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimVL_U, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimVL_D, SIGNAL(pressed()), this, SLOT(onTrimPressed()));
  connect(ui->trimHR_L, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimHR_R, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimVR_U, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimVR_D, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimHL_R, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimHL_L, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimVL_U, SIGNAL(released()), this, SLOT(onTrimReleased()));
  connect(ui->trimVL_D, SIGNAL(released()), this, SLOT(onTrimReleased()));
}

SimulatorDialogTaranis::~SimulatorDialogTaranis()
{
  saveSwitches();
  delete ui;
}

SimulatorDialog::~SimulatorDialog()
{
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

void SimulatorDialog9X::dialChanged()
{
  ui->dialP_1value->setText(QString("%1 %").arg((ui->dialP_1->value()*100)/1024));
  ui->dialP_2value->setText(QString("%1 %").arg((ui->dialP_2->value()*100)/1024));
  ui->dialP_3value->setText(QString("%1 %").arg((ui->dialP_3->value()*100)/1024));
}

void SimulatorDialog::wheelEvent (QWheelEvent *event)
{
  simulator->wheelEvent(event->delta() > 0 ? 1 : -1);
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
  TelemetrySimu = new TelemetrySimulator(this, simulator);
  TelemetrySimu->show();
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
    case Qt::Key_Menu:    
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
    case Qt::Key_Menu:
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
  dialP_1 = ui->dialP_1;
  dialP_2 = ui->dialP_2;
  dialP_3 = ui->dialP_3;
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

  simulator = GetCurrentFirmware()->getSimulator();
  lcd->setData(simulator->getLcd(), lcdWidth, 64, lcdDepth);

  if (flags & SIMULATOR_FLAGS_STICK_MODE_LEFT) {
    nodeLeft->setCenteringY(false);
    ui->holdLeftY->setChecked(true);
  }
  else {
    nodeRight->setCenteringY(false);
    ui->holdRightY->setChecked(true);
  }

  setTrims();

  int outputs = std::min(32,GetCurrentFirmware()->getCapability(Outputs));
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
    ModelData model;
    label->setText(RawSource(SOURCE_TYPE_CH, i).toString(model));
    outputTab->addWidget(label, line, column == 0 ? 0 : 5, 1, 1);

    QSlider * slider = new QSlider(tabWidget);
    slider->setEnabled(false);
    slider->setMaximumSize(QSize(16777215, 18));
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
    "}"));
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
      ui->gvarsLayout->addWidget(label, 0, fm+1);
    }
    for (int i=0; i<gvars; i++) {
      QLabel * label = new QLabel(tabWidget);
      label->setText(QString("GV%1").arg(i+1));
      ui->gvarsLayout->addWidget(label, i+1, 0);
      for (int fm=0; fm<fmodes; fm++) {
        QLabel * value = new QLabel(tabWidget);
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

void SimulatorDialog9X::setLightOn(bool enable)
{
  QString bg = "";
  if (enable) {
    QStringList list;
    list << "bl" << "gr" << "rd" << "or" << "yl";
    bg = QString("-") + list[backLight];
  }
  ui->top->setStyleSheet(QString("background:url(:/images/9xdt%1.png);").arg(bg));
  ui->bottom->setStyleSheet(QString("background:url(:/images/9xdb%1.png);").arg(bg));
  ui->left->setStyleSheet(QString("background:url(:/images/9xdl%1.png);").arg(bg));
  ui->right->setStyleSheet(QString("background:url(:/images/9xdr%1.png);").arg(bg));
}

void SimulatorDialog9X::updateBeepButton()
{
  #define CBEEP_ON  "QLabel { background-color: #FF364E }"
  #define CBEEP_OFF "QLabel { }"

  if (beepVal) {
    beepShow = 20;
  }

  ui->label_beep->setStyleSheet(beepShow ? CBEEP_ON : CBEEP_OFF);

  if (beepShow) {
    beepShow--;
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

void SimulatorDialog9X::getValues()
{
  TxInputs inputs = {
    {
      int(1024*nodeLeft->getX()),  // LEFT HORZ
      int(-1024*nodeLeft->getY()),  // LEFT VERT
      int(-1024*nodeRight->getY()), // RGHT VERT
      int(1024*nodeRight->getX())   // RGHT HORZ
    },

    {
      ui->dialP_1->value(),
      ui->dialP_2->value(),
      ui->dialP_3->value(), 0
    },

    {
      ui->switchTHR->isChecked(),
      ui->switchRUD->isChecked(),
      ui->switchELE->isChecked(),
      ui->switchID2->isChecked() ? 1 : (ui->switchID1->isChecked() ? 0 : -1),
      ui->switchAIL->isChecked(),
      ui->switchGEA->isChecked(),
      ui->switchTRN->isDown(),
      0, 0, 0
    },

    {
      buttonPressed == Qt::Key_Enter,
      buttonPressed == Qt::Key_Escape,
      buttonPressed == Qt::Key_Down,
      buttonPressed == Qt::Key_Up,
      buttonPressed == Qt::Key_Right,
      buttonPressed == Qt::Key_Left,
    },

    middleButtonPressed,
        
    {
      trimPressed == TRIM_LH_L,
      trimPressed == TRIM_LH_R,
      trimPressed == TRIM_LV_DN,
      trimPressed == TRIM_LV_UP,
      trimPressed == TRIM_RV_DN,
      trimPressed == TRIM_RV_UP,
      trimPressed == TRIM_RH_L,
      trimPressed == TRIM_RH_R
    }
  };

  simulator->setValues(inputs);
}

void SimulatorDialog9X::saveSwitches(void)
{
  // qDebug() << "SimulatorDialog9X::saveSwitches()";
  switchstatus=ui->switchTHR->isChecked();
  switchstatus<<=1;
  switchstatus+=(ui->switchRUD->isChecked()&0x1);
  switchstatus<<=1;
  switchstatus+=(ui->switchID2->isChecked()&0x1);
  switchstatus<<=1;
  switchstatus+=(ui->switchID1->isChecked()&0x1);
  switchstatus<<=1;
  switchstatus+=(ui->switchID0->isChecked()&0x1);
  switchstatus<<=1;
  switchstatus+=(ui->switchGEA->isChecked()&0x1);
  switchstatus<<=1;
  switchstatus+=(ui->switchELE->isChecked()&0x1);
  switchstatus<<=1;
  switchstatus+=(ui->switchAIL->isChecked()&0x1);
}

void SimulatorDialog9X::restoreSwitches(void)
{
  // qDebug() << "SimulatorDialog9X::restoreSwitches()";
  ui->switchAIL->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchELE->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchGEA->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchID0->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchID1->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchID2->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchRUD->setChecked(switchstatus & 0x1);
  switchstatus >>=1;
  ui->switchTHR->setChecked(switchstatus & 0x1);
}

void SimulatorDialogTaranis::resetSH()
{
  ui->switchH->setValue(0);
}

void SimulatorDialogTaranis::on_switchH_sliderReleased()
{
  QTimer::singleShot(400, this, SLOT(resetSH()));
}

void SimulatorDialogTaranis::getValues()
{
  TxInputs inputs = {
    {
      int(1024*nodeLeft->getX()),  // LEFT HORZ
      int(-1024*nodeLeft->getY()),  // LEFT VERT
      int(-1024*nodeRight->getY()), // RGHT VERT
      int(1024*nodeRight->getX())  // RGHT HORZ
    },

    {
      -ui->dialP_1->value(),
      ui->dialP_2->value(),
      0,
      -ui->dialP_3->value(),
      ui->dialP_4->value()
    },

    {
      ui->switchA->value() - 1,
      ui->switchB->value() - 1,
      ui->switchC->value() - 1,
      ui->switchD->value() - 1,
      ui->switchE->value() - 1,
      ui->switchF->value(),
      ui->switchG->value() - 1,
      ui->switchH->value(), 0, 0
    },

    {
      buttonPressed == Qt::Key_Menu,
      buttonPressed == Qt::Key_Escape,
      buttonPressed == Qt::Key_Enter,
      buttonPressed == Qt::Key_PageDown,
      buttonPressed == Qt::Key_Plus,
      buttonPressed == Qt::Key_Minus
    },

    middleButtonPressed,
    
    {
      trimPressed == TRIM_LH_L,
      trimPressed == TRIM_LH_R,
      trimPressed == TRIM_LV_DN,
      trimPressed == TRIM_LV_UP,
      trimPressed == TRIM_RV_DN,
      trimPressed == TRIM_RV_UP,
      trimPressed == TRIM_RH_L,
      trimPressed == TRIM_RH_R
    }
  };

  simulator->setValues(inputs);
}

void SimulatorDialogTaranis::saveSwitches(void)
{
  // qDebug() << "SimulatorDialogTaranis::saveSwitches()";
  switchstatus=ui->switchA->value();
  switchstatus<<=2;
  switchstatus+=ui->switchB->value();
  switchstatus<<=2;
  switchstatus+=ui->switchC->value();
  switchstatus<<=2;
  switchstatus+=ui->switchD->value();
  switchstatus<<=2;
  switchstatus+=ui->switchE->value();
  switchstatus<<=2;
  switchstatus+=ui->switchF->value();
  switchstatus<<=2;
  switchstatus+=ui->switchG->value();
  switchstatus<<=2;
  switchstatus+=ui->switchH->value();
}

void SimulatorDialogTaranis::restoreSwitches(void)
{
  // qDebug() << "SimulatorDialogTaranis::restoreSwitches()";
  ui->switchH->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchG->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchF->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchE->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchD->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchC->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchB->setValue(switchstatus & 0x3);
  switchstatus>>=2;
  ui->switchA->setValue(switchstatus & 0x3);
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
    else if (stick==5) {
      dialP_1->setValue(stickval);
    }
    else if (stick==6) {
      dialP_2->setValue(stickval);
    }
    else if (stick==7) {
      dialP_3->setValue(stickval);
    }
    else if (stick==8 && dialP_4) {
      dialP_4->setValue(stickval);
    }
  }
}
#endif
