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

  polygon.setPoints(23, 72,125, 85,126, 92,129, 98,133, 100,136, 100,141, 99,145, 96,148, 92,149, 87,147, 82,146, 76,145, 69,146, 63,147, 58,148, 54,148, 52,147, 49,144, 48,139, 47,135, 53,130, 63,127, 72,125);
  ui->rightbuttons->addArea(polygon, Qt::Key_Up, "Horus/right_btnU.png");
  polygon.setPoints(25, 125,163, 129,164, 132,167, 135,171, 136,177, 137,184, 138,189, 138,195, 137,202, 135,207, 134,211, 131,214, 130,216, 125,217, 121,216, 118,214, 115,211, 115,204, 118,195, 118,187, 118,181, 116,176, 116,171, 118,165, 125,163);
  ui->rightbuttons->addArea(polygon, Qt::Key_Right, "Horus/right_btnR.png");
  polygon.setPoints(20, 64,234, 72,235, 84,234, 91,232, 97,234, 99,237, 101,242, 100,248, 96,250, 92,252, 83,254, 72,255, 64,254, 57,253, 50,249, 48,245, 47,240, 50,235, 55,233, 64,234);
  ui->rightbuttons->addArea(polygon, Qt::Key_Down, "Horus/right_btnD.png");
  polygon.setPoints(22, 24,163, 29,166, 30,168, 31,171, 31,175, 30,179, 29,187, 29,192, 30,198, 32,204, 32,209, 30,214, 27,216, 22,217, 17,216, 13,208, 10,197, 9,188, 9,178, 12,169, 15,165, 24,162);
  ui->rightbuttons->addArea(polygon, Qt::Key_Left, "Horus/right_btnL.png");
  polygon.setPoints(21, 72,148, 83,149, 97,155, 106,163, 112,174, 114,187, 114,198, 110,210, 101,221, 89,228, 78,230, 66,230, 53,226, 43,218, 36,208, 32,196, 32,181, 37,168, 47,157, 57,151, 73,148);
  ui->rightbuttons->addArea(polygon, Qt::Key_Enter, "Horus/right_ent.png");
  polygon.setPoints(26, 25,164, 31,165, 33,167, 35,169, 35,172, 35,174, 34,179, 34,183, 34,190, 34,195, 36,200, 37,204, 38,208, 37,212, 34,216, 29,218, 25,217, 21,215, 18,210, 16,203, 14,194, 14,184, 15,175, 17,169, 21,165, 25,164);
  ui->leftbuttons->addArea(polygon, Qt::Key_PageUp, "Horus/left_btn1.png");
  polygon.setPoints(27, 60,227, 66,228, 71,229, 75,230, 81,229, 86,228, 89,227, 94,227, 97,229, 100,232, 100, 234, 100,238, 100,241, 99,243, 94,246, 88,248, 81,249, 73,249, 63,248, 56,246, 50,243, 48,240, 47,235, 48,232, 51,228, 56,227, 60,227);
  ui->leftbuttons->addArea(polygon, Qt::Key_Escape, "Horus/left_btn2.png");
  ui->leftbuttons->addArea(9, 259, 34, 282, Qt::Key_Print, "Horus/left_scrnsht.png");

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
  connect(ui->teleSim, SIGNAL(released()), this, SLOT(openTelemetrySimulator()));
  connect(ui->trainerSim, SIGNAL(released()), this, SLOT(openTrainerSimulator()));
  connect(ui->debugConsole, SIGNAL(released()), this, SLOT(openDebugOutput()));
  connect(ui->luaReload, SIGNAL(released()), this, SLOT(luaReload()));
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
      buttonPressed == Qt::Key_Enter || middleButtonPressed,
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
