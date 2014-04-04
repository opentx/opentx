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
  txInterface(NULL),
  simulator(NULL),
  beepVal(0),
  buttonPressed(0),
  middleButtonPressed(false)
{
}

SimulatorDialog9X::SimulatorDialog9X(QWidget * parent, unsigned int flags):
  SimulatorDialog(parent, flags),
  ui(new Ui::SimulatorDialog9X),
  beepShow(0)
{
  lcdWidth = 128;
  lcdDepth = 1;

  initUi<Ui::SimulatorDialog9X>(ui);

  QSettings settings;
  backLight = settings.value("backLight", 0).toInt();
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

  connect(ui->dialP_1, SIGNAL(valueChanged(int)), this, SLOT(dialChanged()));
  connect(ui->dialP_2, SIGNAL(valueChanged(int)), this, SLOT(dialChanged()));
  connect(ui->dialP_3, SIGNAL(valueChanged(int)), this, SLOT(dialChanged()));
  connect(ui->cursor, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  connect(ui->menu, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
}

SimulatorDialog9X::~SimulatorDialog9X()
{
  delete ui;
}

SimulatorDialogTaranis::SimulatorDialogTaranis(QWidget * parent, unsigned int flags):
  SimulatorDialog(parent, flags),
  ui(new Ui::SimulatorDialogTaranis)
{
  lcdWidth = 212;
  lcdDepth = 4;

  initUi<Ui::SimulatorDialogTaranis>(ui);
  dialP_4 = ui->dialP_4;

  ui->lcd->setBackgroundColor(47, 123, 227);

  connect(ui->cursor, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  connect(ui->menu, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
}

SimulatorDialogTaranis::~SimulatorDialogTaranis()
{
  delete ui;
}

SimulatorDialog::~SimulatorDialog()
{
  delete timer;
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
  logicalSwitchesLayout = ui->logicalSwitchesLayout;
  channelsLayout = ui->channelsLayout;
  leftXPerc = ui->leftXPerc;
  leftYPerc = ui->leftYPerc;
  rightXPerc = ui->rightXPerc;
  rightYPerc = ui->rightYPerc;

  setupSticks();

  resize(0, 0); // to force min height, min width
  setFixedSize(width(), height());

#ifdef JOYSTICKS
    QSettings settings;
    bool js_enable = settings.value("js_support",false).toBool();
    int js_ctrl=settings.value("js_ctrl",-1).toInt();
    if (js_enable) {
      settings.beginGroup("JsCalibration");
      int count=0;
      for (int j=0; j<8;j++){
        int axe=settings.value(QString("stick%1_axe").arg(j),-1).toInt();
        if (axe>=0 && axe<8) {
          jsmap[axe]=j;
          jscal[axe][0]=settings.value(QString("stick%1_min").arg(j),-32767).toInt();
          jscal[axe][1]=settings.value(QString("stick%1_med").arg(j),0).toInt();
          jscal[axe][2]=settings.value(QString("stick%1_max").arg(j),0).toInt();
          jscal[axe][3]=settings.value(QString("stick%1_inv").arg(j),0).toInt();
          count++;
        }
      }
      settings.endGroup();
      if (count<3) {
        QMessageBox::critical(this, tr("Warning"), tr("Joystick enabled but not configured correctly"));
      }
      if (js_ctrl!=-1) {
        joystick = new Joystick(this);
        if (joystick) {
          if (joystick->open(js_ctrl)) {
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

  txInterface = GetEepromInterface();

  windowName = tr("Simulating Radio (%1)").arg(txInterface->getName());
  setWindowTitle(windowName);

  simulator = txInterface->getSimulator();
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

  int outputs = std::min(16, txInterface->getCapability(Outputs));
  for (int i=0; i<outputs; i++) {
    int column = i / (outputs/2);
    int line = i % (outputs/2);
    QLabel * label = new QLabel(tabWidget);
    label->setText(RawSource(SOURCE_TYPE_CH, i).toString());
    channelsLayout->addWidget(label, line, column == 0 ? 0 : 5, 1, 1);

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
    channelsLayout->addWidget(slider, line, column == 0 ? 1 : 4, 1, 1);

    QLabel * value = new QLabel(tabWidget);
    value->setMinimumSize(QSize(50, 0));
    value->setAlignment(Qt::AlignCenter);
    channelValues << value;
    channelsLayout->addWidget(value, line, column == 0 ? 2 : 3, 1, 1);
  }

  int switches = txInterface->getCapability(LogicalSwitches);
  for (int i=0; i<switches; i++) {
    QFrame * swtch = new QFrame(tabWidget);
    swtch->setAutoFillBackground(true);
    swtch->setFrameShape(QFrame::Panel);
    swtch->setFrameShadow(QFrame::Raised);
    swtch->setLineWidth(2);
    QVBoxLayout * layout = new QVBoxLayout(swtch);
    layout->setContentsMargins(2, 2, 2, 2);
    QLabel * label = new QLabel(swtch);
    label->setText(RawSwitch(SWITCH_TYPE_VIRTUAL, i+1).toString());
    label->setAlignment(Qt::AlignCenter);
    logicalSwitchLabels << label;
    layout->addWidget(label);
    logicalSwitchesLayout->addWidget(swtch, i / (switches/2), i % (switches/2), 1, 1);
  }

  if (flags & SIMULATOR_FLAGS_NOTX) {
    ui->tabWidget->setCurrentIndex(1);
  }
  else {
    ui->lcd->setFocus();
  }

  setupTimer();
}

void SimulatorDialog::onButtonPressed(int value)
{
  if (value == Qt::Key_Print) {
    QSettings settings;
    bool toclipboard = settings.value("snapshot_to_clipboard", false).toBool();
    QString fileName = "";
    if (!toclipboard) {
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
    QMessageBox::critical(this, "Companion", tr("Firmware %1 error: %2").arg(txInterface->getName()).arg(simulator->getError()));
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
  simulator->start(eeprom, (flags & SIMULATOR_FLAGS_NOTX) ? false : true);
}

void SimulatorDialog::start(const char * filename)
{
  simulator->start(filename);
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

    middleButtonPressed
  };

  simulator->setValues(inputs);
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

    middleButtonPressed
  };

  simulator->setValues(inputs);
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

  for (int i=0; i<std::min(16, GetEepromInterface()->getCapability(Outputs)); i++) {
    channelSliders[i]->setValue(chVal(outputs.chans[i]));
    channelValues[i]->setText(QString("%1").arg((qreal)outputs.chans[i]*100/1024, 0, 'f', 1));
  }

  leftXPerc->setText(QString("X %1%").arg((qreal)nodeLeft->getX()*100+trims.values[0]/5, 2, 'f', 0));
  leftYPerc->setText(QString("Y %1%").arg((qreal)nodeLeft->getY()*-100+trims.values[1]/5, 2, 'f', 0));

  rightXPerc->setText(QString("X %1%").arg((qreal)nodeRight->getX()*100+trims.values[3]/5, 2, 'f', 0));
  rightYPerc->setText(QString("Y %1%").arg((qreal)nodeRight->getY()*-100+trims.values[2]/5, 2, 'f', 0));

  QString CSWITCH_ON = "QLabel { background-color: #4CC417 }";
  QString CSWITCH_OFF = "QLabel { }";

  for (int i=0; i<GetEepromInterface()->getCapability(LogicalSwitches); i++) {
    logicalSwitchLabels[i]->setStyleSheet(outputs.vsw[i] ? CSWITCH_ON : CSWITCH_OFF);
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
    if (stick==1 || stick==2) {
      float currX=nodeRight->getX();
      float currY=nodeRight->getY();
      if (stick==1 ) {
         nodeRight->setPos(currX*100-10,-stickval*100/1024-10);
      } 
      if (stick==2) {
        nodeRight->setPos(stickval*100/1024-10,currY*100-10);
      } 
    }
    else if (stick==3 || stick==4) {
      float currX=nodeLeft->getX();
      float currY=nodeLeft->getY();
      if (stick==3) {
        nodeLeft->setPos(currX*100-10,-stickval*100/1024-10);
      } 
      if (stick==4) {
        nodeLeft->setPos(stickval*100/1024-10,currY*100-10);
      } 
    }

    if (stick==5) {
      dialP_1->setValue(stickval);
    }
    if (stick==6) {
      dialP_2->setValue(stickval);
    }
    if (stick==7) {
      dialP_3->setValue(stickval);
    }
    if (stick==8 && dialP_4) {
      dialP_4->setValue(stickval);
    }
  }
}
#endif
