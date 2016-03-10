#include "joystickdialog.h"
#include "ui_joystickdialog.h"
#include "appdata.h"

joystickDialog::joystickDialog(QWidget *parent, int stick) :
QDialog(parent),
ui(new Ui::joystickDialog) {
  ui->setupUi(this);
  int jscaltmp[8][3] = {
    {32767, 0, -32767},
    {32767, 0, -32767},
    {32767, 0, -32767},
    {32767, 0, -32767},
    {32767, 0, -32767},
    {32767, 0, -32767},
    {32767, 0, -32767},
    {32767, 0, -32767}
  };
  memcpy(jscal, jscaltmp, sizeof (jscal));
  ui->okButton->setDisabled(true);
  ui->howtoLabel->setText(tr("Move sticks and pots in every direction making full movement\nPress next when finished"));
  step = 0;

  foreach(QComboBox *cb, findChildren<QComboBox *>(QRegExp("jsmapCB_[0-9]+"))) {
    cb->setDisabled(true);
  }
  foreach(QCheckBox *ib, findChildren<QCheckBox *>(QRegExp("ChInv_[0-9]+"))) {
    ib->setDisabled(true);
  }
  joystickOpen(stick);
  connect(joystick, SIGNAL(axisValueChanged(int, int)), this, SLOT(onjoystickAxisValueChanged(int, int)));
}

joystickDialog::~joystickDialog() {
  delete ui;
}

void joystickDialog::joystickOpen(int stick) {
  if (stick == -1) {
    this->close();
    return;
  }
  joystick = new Joystick(this, 1, false, 0);
  int numAxes=std::min(joystick->numAxes,8);
  if (joystick) {
    joystick->open(stick);
    
    for (int j=0; j<numAxes; j++) {
        joystick->sensitivities[j] = 0;
        joystick->deadzones[j]=20;
    }
  }
  else {
    QMessageBox::critical(this, tr("Error"), tr("Cannot open joystick."));
    this->close();
  }
}

void joystickDialog::onjoystickAxisValueChanged(int axis, int value) {
  if (value > jscal[axis][2]) {
    jscal[axis][2] = value;
  }
  if (value < jscal[axis][0]) {
    jscal[axis][0] = value;
  }
  QSlider * sl[]= {ui->Ch_1, ui->Ch_2, ui->Ch_3, ui->Ch_4, ui->Ch_5, ui->Ch_6, ui->Ch_7, ui->Ch_8};
  QCheckBox * ib[]={ui->ChInv_1, ui->ChInv_2, ui->ChInv_3, ui->ChInv_4, ui->ChInv_5, ui->ChInv_6, ui->ChInv_7, ui->ChInv_8};
  if (ib[axis]->isChecked()) {
    sl[axis]->setInvertedAppearance(true);
  } else {
    sl[axis]->setInvertedAppearance(false);
  }
  sl[axis]->setMinimum(jscal[axis][0]);
  sl[axis]->setMaximum(jscal[axis][2]);    
  sl[axis]->setValue(value);
}

void joystickDialog::on_nextButton_clicked() {
  int numAxes=std::min(joystick->numAxes,8);
  switch (step) {
    case 0:
      ui->howtoLabel->setText(tr("Place sticks and pots in middle position.\nPress next when done"));
      step++;
      break;
    case 1:
      for (int i=0; i<numAxes; i++) {
        jscal[i][1]=0;
        for (int j=0; j<100;j++) {
          jscal[i][1]+=joystick->getAxisValue(i);
        }
        jscal[i][1]/=100;
      }
      ui->howtoLabel->setText(tr("Map joystick channels to controls using comboboxes.\nPress next when done"));
      step++;
      foreach(QComboBox *cb, findChildren<QComboBox *>(QRegExp("jsmapCB_[0-9]+"))) {
        cb->setEnabled(true);
      }
      break;
    case 2:
      ui->howtoLabel->setText(tr("Check inversion checkbox to get maximum at top-right position.\nPress next when done"));
      step++;
      foreach(QCheckBox *ib, findChildren<QCheckBox *>(QRegExp("ChInv_[0-9]+"))) {
        ib->setEnabled(true);
      }
      break;
    case 3:
      ui->howtoLabel->setText(tr("Press ok to save configuration\nPress cancel to abort joystick calibration"));
      ui->okButton->setEnabled(true);
  }
}

void joystickDialog::on_cancelButton_clicked() {
  joystick->close();
  this->close();
}

void joystickDialog::on_okButton_clicked()
{
  int stickAssignments[MAX_JOYSTICKS] = {0};
  foreach(QComboBox *cb, findChildren<QComboBox *>(QRegExp("jsmapCB_[0-9]+"))) {
    int mappedValue = cb->currentIndex() - 1;
    // qDebug() << "cb->currentIndex()" << mappedValue;
    if (mappedValue >= 0 && mappedValue < MAX_JOYSTICKS) {
      stickAssignments[mappedValue]++;
    }
  }

  for (int i=0; i<MAX_JOYSTICKS; i++) {
    if (stickAssignments[i] > 1) {
      QMessageBox::critical(this, tr("Error"), tr("Duplicated stick assignment"));
      return;
    }
  }

  joystick->close();
  for (int i=0; i<MAX_JOYSTICKS; i++) {
    g.joystick[i].remove();
  }
  QCheckBox * ib[]={ui->ChInv_1, ui->ChInv_2, ui->ChInv_3, ui->ChInv_4, ui->ChInv_5, ui->ChInv_6, ui->ChInv_7, ui->ChInv_8};
  foreach(QComboBox *cb, findChildren<QComboBox *>(QRegExp("jsmapCB_[0-9]+"))) {
    int axe=cb->objectName().mid(cb->objectName().lastIndexOf("_")+1).toInt()-1;
    int stick=cb->currentIndex() - 1;
    qDebug() << "joystick mapping " << cb->objectName() <<"axe:" << axe << "stick:" << stick;
    if (stick >= 0 && stick < MAX_JOYSTICKS) {
      g.joystick[stick].stick_axe( axe );
      g.joystick[stick].stick_max( jscal[axe][2] );
      g.joystick[stick].stick_med( jscal[axe][1] );
      g.joystick[stick].stick_min( jscal[axe][0] );
      g.joystick[stick].stick_inv( ib[axe]->isChecked() ? 1 : 0 );
    }
  }
  this->close();
}
