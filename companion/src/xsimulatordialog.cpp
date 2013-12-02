#include "xsimulatordialog.h"
#include "ui_xsimulatordialog.h"
#include <iostream>
#include "helpers.h"
#include "simulatorinterface.h"
#ifdef JOYSTICKS
#include "joystick.h"
#endif

#define GBALL_SIZE  20
#define RESX        1024
#define W           212
#define H           64

int xsimulatorDialog::screenshotIdx = 0;
uint32_t xsimulatorDialog::xswitchstatus = 0;

xsimulatorDialog::xsimulatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xsimulatorDialog),
    timer(NULL),
    lightOn(false),
    txInterface(NULL),
    simulator(NULL),
    g_modelIdx(-1),
    beepVal(0),
    beepShow(0),
    buttonPressed(0)
{
    ui->setupUi(this);
    ui->lcd->setFocus();

    QSettings settings("companion9x", "companion9x");
    ui->lcd->setBackgroundColor(47, 123, 227);
    setupSticks();
    resize(0, 0); // to force min height, min width
    this->setFixedSize(this->width(), this->height());
    bool simuSW=settings.value("simuSW",false).toBool();
#ifdef JOYSTICKS
    bool js_enable=settings.value("js_support",false).toBool();
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
    if (simuSW) {
      ui->switchH->setValue(xswitchstatus & 0x3);
      xswitchstatus>>=2;
      ui->switchG->setValue(xswitchstatus & 0x3);
      xswitchstatus>>=2;
      ui->switchF->setValue(xswitchstatus & 0x3);
      xswitchstatus>>=2;
      ui->switchE->setValue(xswitchstatus & 0x3);
      xswitchstatus>>=2;
      ui->switchD->setValue(xswitchstatus & 0x3);
      xswitchstatus>>=2;
      ui->switchC->setValue(xswitchstatus & 0x3);
      xswitchstatus>>=2;
      ui->switchB->setValue(xswitchstatus & 0x3);
      xswitchstatus>>=2;
      ui->switchA->setValue(xswitchstatus & 0x3);
    }
    connect(ui->cursor, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
    connect(ui->menu, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
}

xsimulatorDialog::~xsimulatorDialog()
{
  delete timer;
  delete ui;
}

void xsimulatorDialog::closeEvent (QCloseEvent *)
{
  simulator->stop();
  timer->stop();
}

void xsimulatorDialog::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::MidButton)
    middleButtonPressed = true;
}

void xsimulatorDialog::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::MidButton)
    middleButtonPressed = false;
}

void xsimulatorDialog::wheelEvent (QWheelEvent *event)
{
  simulator->wheelEvent(event->delta() > 0 ? 1 : -1);
}

void xsimulatorDialog::keyPressEvent (QKeyEvent *event)
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
    case Qt::Key_Minus:
    case Qt::Key_Plus:
    case Qt::Key_PageDown:
    case Qt::Key_Menu:
      buttonPressed = event->key();
      break;
  }
}

void xsimulatorDialog::keyReleaseEvent(QKeyEvent * event)
{
  switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Escape:
    case Qt::Key_Backspace:
    case Qt::Key_Plus:
    case Qt::Key_Minus:
    case Qt::Key_PageDown:
    case Qt::Key_Menu:
      buttonPressed = 0;
      break;
  }
}

void xsimulatorDialog::setupTimer()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
    timer->start(10);
}

void xsimulatorDialog::onButtonPressed(int value)
{
  if (value==Qt::Key_Print) {
      QSettings settings("companion9x", "companion9x");
      bool toclipboard=settings.value("snapshot_to_clipboard", false).toBool();
      QString fileName ="";
      if (!toclipboard) {
        fileName = QString("screenshot-%1.png").arg(++screenshotIdx);
      }    
      ui->lcd->makeScreenshot(fileName);      
  } else {
      buttonPressed = value;
  }
}

void xsimulatorDialog::onTimerEvent()
{
  static unsigned int lcd_counter = 0;
  if (!simulator->timer10ms()) {
    QMessageBox::critical(this, "companion9x", QString(tr("Firmware %1 error: %2")).arg(txInterface->getName()).arg(simulator->getError()));
    timer->stop();
    return;
  }

  getValues();

  if (g_modelIdx >= 0) {
    ModelData *model = & g_radioData.models[g_modelIdx];
    setWindowTitle(windowName + (txInterface->getCapability(FlightPhases) ? tr(" - Flight mode: %1(%2)").arg(model->phaseData[simulator->getPhase()].name).arg(simulator->getPhase()) : ""));
  }
  else if (ui->tabWidget->currentIndex() == 0) {
    bool lightEnable;
    if (simulator->lcdChanged(lightEnable)) {
      ui->lcd->onLcdChanged(lightEnable);
      if (lightOn!=lightEnable) {
        lightOn=lightEnable;
      }
    }
  }

  if (!(lcd_counter++ % 5)) {

    setValues();

    setTrims();

    centerSticks();

    if (beepVal) {
      beepVal = 0;
      QApplication::beep();
    }

#define CBEEP_ON  "QLabel { background-color: #FF364E }"
#define CBEEP_OFF "QLabel { }"

    if (beepShow) {
      beepShow--;
    }
  }
}

void xsimulatorDialog::centerSticks()
{
    if(ui->leftStick->scene()) nodeLeft->stepToCenter();
    if(ui->rightStick->scene()) nodeRight->stepToCenter();
}

void xsimulatorDialog::loadParams(RadioData &radioData, const int model_idx)
{
    g_modelIdx = model_idx;

    txInterface = GetEepromInterface();
    simulator = txInterface->getSimulator();

    g_radioData = radioData;
   
    if (model_idx < 0) {
      windowName = QString(tr("Simulating Tx (%1)").arg(txInterface->getName()));
      ui->lcd->setData(simulator->getLcd(), W, H, 4);
    }
    else {
      ui->tabWidget->removeTab(0);
      g_radioData.generalSettings.currModel = model_idx;
      windowName = tr("Simulating ") + g_radioData.models[model_idx].name;
    }

    setWindowTitle(windowName);

    if (g_radioData.generalSettings.stickMode & 1)
    {
        nodeLeft->setCenteringY(false);   //mode 1,3 -> THR on left
        ui->holdLeftY->setChecked(true);
    }
    else
    {
        nodeRight->setCenteringY(false);   //mode 1,3 -> THR on right
        ui->holdRightY->setChecked(true);
    }

    setTrims();

    beepVal = 0;
    beepShow = 0;

    simulator->start(g_radioData, model_idx<0);

    setupTimer();
}

void xsimulatorDialog::setTrims()
{
  Trims trims;
  simulator->getTrims(trims);

  int trimMin = -125, trimMax = +125;
  if (trims.extended) {
    trimMin = -500;
    trimMax = +500;
  }
  ui->trimHLeft->setRange(trimMin, trimMax);  ui->trimHLeft->setValue(trims.values[0]);
  ui->trimVLeft->setRange(trimMin, trimMax);  ui->trimVLeft->setValue(trims.values[1]);
  ui->trimVRight->setRange(trimMin, trimMax); ui->trimVRight->setValue(trims.values[2]);
  ui->trimHRight->setRange(trimMin, trimMax); ui->trimHRight->setValue(trims.values[3]);
}

void xsimulatorDialog::getValues()
{
  TxInputs inputs = {{ int(1024*nodeLeft->getX()),  // LEFT HORZ
                       int(-1024*nodeLeft->getY()),  // LEFT VERT
                       int(-1024*nodeRight->getY()), // RGHT VERT
                       int(1024*nodeRight->getX()) },  // RGHT HORZ
                     { -ui->dialP_1->value(),
                       ui->dialP_2->value(),
                       -ui->dialP_3->value(),
                       ui->dialP_4->value() },
                     { ui->switchA->value() - 1,
                       ui->switchB->value() - 1,
                       ui->switchC->value() - 1,
                       ui->switchD->value() - 1,
                       ui->switchE->value() - 1,
                       ui->switchF->value(),
                       ui->switchG->value() - 1,
                       ui->switchH->value(), 0, 0 },
                     {
                       buttonPressed == Qt::Key_Menu,
                       buttonPressed == Qt::Key_Escape,
                       buttonPressed == Qt::Key_Enter,
                       buttonPressed == Qt::Key_PageDown,
                       buttonPressed == Qt::Key_Plus,
                       buttonPressed == Qt::Key_Minus },
                     middleButtonPressed
                    };
  
   simulator->setValues(inputs);
   xswitchstatus=ui->switchA->value();
   xswitchstatus<<=2;
   xswitchstatus+=ui->switchB->value();
   xswitchstatus<<=2;
   xswitchstatus+=ui->switchC->value();
   xswitchstatus<<=2;
   xswitchstatus+=ui->switchD->value();
   xswitchstatus<<=2;
   xswitchstatus+=ui->switchE->value();
   xswitchstatus<<=2;
   xswitchstatus+=ui->switchF->value();
   xswitchstatus<<=2;
   xswitchstatus+=ui->switchG->value();
   xswitchstatus<<=2;
   xswitchstatus+=ui->switchH->value();
}

inline int chVal(int val)
{
  return qMin(1024, qMax(-1024, val));
}

void xsimulatorDialog::on_switchH_sliderReleased()
{
  QTimer::singleShot(400, this, SLOT(resetTrn()));
}

void xsimulatorDialog::on_trimHLeft_valueChanged(int value)
{
  simulator->setTrim(0, value);
}

void xsimulatorDialog::on_trimVLeft_valueChanged(int value)
{
  simulator->setTrim(1, value);
}

void xsimulatorDialog::on_trimHRight_valueChanged(int value)
{
  simulator->setTrim(3, value);
}

void xsimulatorDialog::on_trimVRight_valueChanged(int value)
{
  simulator->setTrim(2, value);
}

void xsimulatorDialog::setValues()
{
  TxOutputs outputs;
  simulator->getValues(outputs);

  ui->chnout_1->setValue(chVal(outputs.chans[0]));
  ui->chnout_2->setValue(chVal(outputs.chans[1]));
  ui->chnout_3->setValue(chVal(outputs.chans[2]));
  ui->chnout_4->setValue(chVal(outputs.chans[3]));
  ui->chnout_5->setValue(chVal(outputs.chans[4]));
  ui->chnout_6->setValue(chVal(outputs.chans[5]));
  ui->chnout_7->setValue(chVal(outputs.chans[6]));
  ui->chnout_8->setValue(chVal(outputs.chans[7]));
  if (txInterface->getCapability(Outputs) > 8) {
    ui->chnout_9->setValue(chVal(outputs.chans[8]));
    ui->chnout_10->setValue(chVal(outputs.chans[9]));
    ui->chnout_11->setValue(chVal(outputs.chans[10]));
    ui->chnout_12->setValue(chVal(outputs.chans[11]));
    ui->chnout_13->setValue(chVal(outputs.chans[12]));
    ui->chnout_14->setValue(chVal(outputs.chans[13]));
    ui->chnout_15->setValue(chVal(outputs.chans[14]));
    ui->chnout_16->setValue(chVal(outputs.chans[15]));
  }

  ui->chnoutV_1->setText(QString("%1").arg((qreal)outputs.chans[0]*100/1024, 0, 'f', 1));
  ui->chnoutV_2->setText(QString("%1").arg((qreal)outputs.chans[1]*100/1024, 0, 'f', 1));
  ui->chnoutV_3->setText(QString("%1").arg((qreal)outputs.chans[2]*100/1024, 0, 'f', 1));
  ui->chnoutV_4->setText(QString("%1").arg((qreal)outputs.chans[3]*100/1024, 0, 'f', 1));
  ui->chnoutV_5->setText(QString("%1").arg((qreal)outputs.chans[4]*100/1024, 0, 'f', 1));
  ui->chnoutV_6->setText(QString("%1").arg((qreal)outputs.chans[5]*100/1024, 0, 'f', 1));
  ui->chnoutV_7->setText(QString("%1").arg((qreal)outputs.chans[6]*100/1024, 0, 'f', 1));
  ui->chnoutV_8->setText(QString("%1").arg((qreal)outputs.chans[7]*100/1024, 0, 'f', 1));
  if (txInterface->getCapability(Outputs) > 8) {
    ui->chnoutV_9->setText(QString("%1").arg((qreal)outputs.chans[8]*100/1024, 0, 'f', 1));
    ui->chnoutV_10->setText(QString("%1").arg((qreal)outputs.chans[9]*100/1024, 0, 'f', 1));
    ui->chnoutV_11->setText(QString("%1").arg((qreal)outputs.chans[10]*100/1024, 0, 'f', 1));
    ui->chnoutV_12->setText(QString("%1").arg((qreal)outputs.chans[11]*100/1024, 0, 'f', 1));
    ui->chnoutV_13->setText(QString("%1").arg((qreal)outputs.chans[12]*100/1024, 0, 'f', 1));
    ui->chnoutV_14->setText(QString("%1").arg((qreal)outputs.chans[13]*100/1024, 0, 'f', 1));
    ui->chnoutV_15->setText(QString("%1").arg((qreal)outputs.chans[14]*100/1024, 0, 'f', 1));
    ui->chnoutV_16->setText(QString("%1").arg((qreal)outputs.chans[15]*100/1024, 0, 'f', 1));
  }

  ui->leftXPerc->setText(QString("X %1%").arg((qreal)nodeLeft->getX()*100, 2, 'f', 0));
  ui->leftYPerc->setText(QString("Y %1%").arg((qreal)nodeLeft->getY()*-100, 2, 'f', 0));

  ui->rightXPerc->setText(QString("X %1%").arg((qreal)nodeRight->getX()*100, 2, 'f', 0));
  ui->rightYPerc->setText(QString("Y %1%").arg((qreal)nodeRight->getY()*-100, 2, 'f', 0));

#define CSWITCH_ON  "QLabel { background-color: #4CC417 }"
#define CSWITCH_OFF "QLabel { }"

  ui->labelCSW_1->setStyleSheet(outputs.vsw[0] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_2->setStyleSheet(outputs.vsw[1] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_3->setStyleSheet(outputs.vsw[2] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_4->setStyleSheet(outputs.vsw[3] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_5->setStyleSheet(outputs.vsw[4] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_6->setStyleSheet(outputs.vsw[5] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_7->setStyleSheet(outputs.vsw[6] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_8->setStyleSheet(outputs.vsw[7] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_9->setStyleSheet(outputs.vsw[8] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_10->setStyleSheet(outputs.vsw[9] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_11->setStyleSheet(outputs.vsw[10] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_12->setStyleSheet(outputs.vsw[11] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_13->setStyleSheet(outputs.vsw[12] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_14->setStyleSheet(outputs.vsw[13] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_15->setStyleSheet(outputs.vsw[14] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_16->setStyleSheet(outputs.vsw[15] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_17->setStyleSheet(outputs.vsw[16] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_18->setStyleSheet(outputs.vsw[17] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_19->setStyleSheet(outputs.vsw[18] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_20->setStyleSheet(outputs.vsw[19] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_21->setStyleSheet(outputs.vsw[20] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_22->setStyleSheet(outputs.vsw[21] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_23->setStyleSheet(outputs.vsw[22] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_24->setStyleSheet(outputs.vsw[23] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_25->setStyleSheet(outputs.vsw[24] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_26->setStyleSheet(outputs.vsw[25] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_27->setStyleSheet(outputs.vsw[26] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_28->setStyleSheet(outputs.vsw[27] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_29->setStyleSheet(outputs.vsw[28] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_30->setStyleSheet(outputs.vsw[29] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_31->setStyleSheet(outputs.vsw[30] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_32->setStyleSheet(outputs.vsw[31] ? CSWITCH_ON : CSWITCH_OFF);

  if (outputs.beep) {
    beepVal = outputs.beep;
    beepShow=20;
  }
}

void xsimulatorDialog::setupSticks()
{
    QGraphicsScene *leftScene = new QGraphicsScene(ui->leftStick);
    leftScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    ui->leftStick->setScene(leftScene);

    // ui->leftStick->scene()->addLine(0,10,20,30);

    QGraphicsScene *rightScene = new QGraphicsScene(ui->rightStick);
    rightScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    ui->rightStick->setScene(rightScene);

    // ui->rightStick->scene()->addLine(0,10,20,30);

    nodeLeft = new Node();
    nodeLeft->setPos(-GBALL_SIZE/2,-GBALL_SIZE/2);
    nodeLeft->setBallSize(GBALL_SIZE);
    leftScene->addItem(nodeLeft);

    nodeRight = new Node();
    nodeRight->setPos(-GBALL_SIZE/2,-GBALL_SIZE/2);
    nodeRight->setBallSize(GBALL_SIZE);
    rightScene->addItem(nodeRight);
}

void xsimulatorDialog::resizeEvent(QResizeEvent *event)
{

    if(ui->leftStick->scene())
    {
        QRect qr = ui->leftStick->contentsRect();
        qreal w  = (qreal)qr.width()  - GBALL_SIZE;
        qreal h  = (qreal)qr.height() - GBALL_SIZE;
        qreal cx = (qreal)qr.width()/2;
        qreal cy = (qreal)qr.height()/2;
        ui->leftStick->scene()->setSceneRect(-cx,-cy,w,h);

        QPointF p = nodeLeft->pos();
        p.setX(qMin(cx, qMax(p.x(), -cx)));
        p.setY(qMin(cy, qMax(p.y(), -cy)));
        nodeLeft->setPos(p);
    }

    if(ui->rightStick->scene())
    {
        QRect qr = ui->rightStick->contentsRect();
        qreal w  = (qreal)qr.width()  - GBALL_SIZE;
        qreal h  = (qreal)qr.height() - GBALL_SIZE;
        qreal cx = (qreal)qr.width()/2;
        qreal cy = (qreal)qr.height()/2;
        ui->rightStick->scene()->setSceneRect(-cx,-cy,w,h);

        QPointF p = nodeRight->pos();
        p.setX(qMin(cx, qMax(p.x(), -cx)));
        p.setY(qMin(cy, qMax(p.y(), -cy)));
        nodeRight->setPos(p);
    }
    QDialog::resizeEvent(event);
}

void xsimulatorDialog::on_holdLeftX_clicked(bool checked)
{
    nodeLeft->setCenteringX(!checked);
}

void xsimulatorDialog::on_holdLeftY_clicked(bool checked)
{
    nodeLeft->setCenteringY(!checked);
}

void xsimulatorDialog::on_holdRightX_clicked(bool checked)
{
    nodeRight->setCenteringX(!checked);
}

void xsimulatorDialog::on_holdRightY_clicked(bool checked)
{
    nodeRight->setCenteringY(!checked);
}


void xsimulatorDialog::on_FixLeftX_clicked(bool checked)
{
    nodeLeft->setFixedX(checked);
}

void xsimulatorDialog::on_FixLeftY_clicked(bool checked)
{
    nodeLeft->setFixedY(checked);
}

void xsimulatorDialog::on_FixRightX_clicked(bool checked)
{
    nodeRight->setFixedX(checked);
}

void xsimulatorDialog::on_FixRightY_clicked(bool checked)
{
    nodeRight->setFixedY(checked);
}

void xsimulatorDialog::resetTrn()
{
  ui->switchH->setValue(0);
}


#ifdef JOYSTICKS
void xsimulatorDialog::onjoystickAxisValueChanged(int axis, int value) {
  int stick;
  if (axis>=0 && axis<8) {
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
      ui->dialP_1->setValue(stickval);
    }
    if (stick==6) {
      ui->dialP_2->setValue(stickval);
    }
    if (stick==7) {
      ui->dialP_3->setValue(stickval);
    }
    if (stick==8) {
      ui->dialP_4->setValue(stickval);
    }
  }
}
#endif
