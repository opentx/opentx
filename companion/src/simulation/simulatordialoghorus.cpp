#include "simulatordialog.h"
#include "ui_simulatordialog-horus.h"
#include "helpers.h"

uint32_t SimulatorDialogHorus::switchstatus = 0;

SimulatorDialogHorus::SimulatorDialogHorus(QWidget * parent, SimulatorInterface *simulator, unsigned int flags):
  SimulatorDialog(parent, simulator, flags),
  ui(new Ui::SimulatorDialogHorus)
{
  QPolygon polygon;

  lcdWidth = 480;
  lcdHeight = 272;
  lcdDepth = 16;

  initUi<Ui::SimulatorDialogHorus>(ui);

  polygon.setPoints(6, 68, 83, 28, 45, 51, 32, 83, 32, 105, 45, 68, 83);
  ui->rightbuttons->addArea(polygon, Qt::Key_Up, NULL);
  polygon.setPoints(6, 74, 90, 114, 51, 127, 80, 127, 106, 114, 130, 74, 90);
  ui->rightbuttons->addArea(polygon, Qt::Key_Right, NULL);
  polygon.setPoints(6, 68, 98, 28, 137, 51, 151, 83, 151, 105, 137, 68, 98);
  ui->rightbuttons->addArea(polygon, Qt::Key_Down, NULL);
  polygon.setPoints(6, 80, 90, 20, 51, 7, 80, 7, 106, 20, 130, 80, 90);
  ui->rightbuttons->addArea(polygon, Qt::Key_Left, NULL);
  ui->rightbuttons->addArea(5, 148, 39, 182, Qt::Key_Print, NULL);

  ui->leftbuttons->addArea(25, 60, 71, 81, Qt::Key_PageUp, NULL);
  ui->leftbuttons->addArea(25, 117, 71, 139, Qt::Key_Escape, NULL);

  // install simulator TRACE hook
  simulator->installTraceHook(traceCb);

  // TODO dialP_4 = ui->dialP_4;

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

  connect(ui->leftbuttons, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  connect(ui->rightbuttons, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
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

SimulatorDialogHorus::~SimulatorDialogHorus()
{
  saveSwitches();
  delete ui;
}

void SimulatorDialogHorus::resetSH()
{
  ui->switchH->setValue(0);
}

void SimulatorDialogHorus::on_switchH_sliderReleased()
{
  QTimer::singleShot(400, this, SLOT(resetSH()));
}

void SimulatorDialogHorus::getValues()
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
      buttonPressed == Qt::Key_PageUp,
      buttonPressed == Qt::Key_Escape,
      buttonPressed == Qt::Key_Enter,
      buttonPressed == Qt::Key_Up,
      buttonPressed == Qt::Key_Down,
      buttonPressed == Qt::Key_Right,
      buttonPressed == Qt::Key_Left
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

void SimulatorDialogHorus::saveSwitches(void)
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

void SimulatorDialogHorus::restoreSwitches(void)
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
