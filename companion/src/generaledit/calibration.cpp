#include "calibration.h"
#include "ui_calibration.h"

CalibrationPanel::CalibrationPanel(QWidget * parent, GeneralSettings & generalSettings, FirmwareInterface * firmware):
  GeneralPanel(parent, generalSettings, firmware),
  ui(new Ui::Calibration)
{
  ui->setupUi(this);

  if (firmware->getCapability(MultiposPots)) {
    ui->pot1Type->setCurrentIndex(generalSettings.potsType[0]);
    ui->pot2Type->setCurrentIndex(generalSettings.potsType[1]);
    ui->pot3Type->setCurrentIndex(generalSettings.potsType[2]);
  }
  else {
    ui->potsTypeSeparator->hide();
    ui->pot1Type->hide();
    ui->pot1TypeLabel->hide();
    ui->pot2Type->hide();
    ui->pot2TypeLabel->hide();
    ui->pot3Type->hide();
    ui->pot3TypeLabel->hide();
  }

  int potsCount = GetCurrentFirmware()->getCapability(Pots);
  if (potsCount == 3) {
    ui->label_pot4->hide();
    ui->ana8Neg->hide();
    ui->ana8Mid->hide();
    ui->ana8Pos->hide();
  }

  if (IS_TARANIS(firmware->getBoard())) {
    ui->serialPortMode->setCurrentIndex(generalSettings.hw_uartMode);
  }
  else {
    ui->serialPortMode->hide();
    ui->serialPortLabel->hide();
  }

  disableMouseScrolling();
}

CalibrationPanel::~CalibrationPanel()
{
  delete ui;
}

void CalibrationPanel::on_PPM_MultiplierDSB_editingFinished()
{
  generalSettings.PPM_Multiplier = (int)(ui->PPM_MultiplierDSB->value()*10)-10;
  emit modified();
}

void CalibrationPanel::on_PPM1_editingFinished()
{
  generalSettings.trainer.calib[0] = ui->PPM1->value();
  emit modified();
}

void CalibrationPanel::on_PPM2_editingFinished()
{
  generalSettings.trainer.calib[1] = ui->PPM2->value();
  emit modified();
}

void CalibrationPanel::on_PPM3_editingFinished()
{
  generalSettings.trainer.calib[2] = ui->PPM3->value();
  emit modified();
}

void CalibrationPanel::on_PPM4_editingFinished()
{
  generalSettings.trainer.calib[3] = ui->PPM4->value();
  emit modified();
}


void CalibrationPanel::on_CurrentCalib_SB_editingFinished()
{
  generalSettings.currentCalib = ui->CurrentCalib_SB->value();
  emit modified();
}

void CalibrationPanel::setValues()
{
  ui->battCalibDSB->setValue((double)generalSettings.vBatCalib/10);
  ui->CurrentCalib_SB->setValue((double)generalSettings.currentCalib);

  ui->ana1Neg->setValue(generalSettings.calibSpanNeg[0]);
  ui->ana2Neg->setValue(generalSettings.calibSpanNeg[1]);
  ui->ana3Neg->setValue(generalSettings.calibSpanNeg[2]);
  ui->ana4Neg->setValue(generalSettings.calibSpanNeg[3]);
  ui->ana5Neg->setValue(generalSettings.calibSpanNeg[4]);
  ui->ana6Neg->setValue(generalSettings.calibSpanNeg[5]);
  ui->ana7Neg->setValue(generalSettings.calibSpanNeg[6]);
  ui->ana8Neg->setValue(generalSettings.calibSpanNeg[7]);

  ui->ana1Mid->setValue(generalSettings.calibMid[0]);
  ui->ana2Mid->setValue(generalSettings.calibMid[1]);
  ui->ana3Mid->setValue(generalSettings.calibMid[2]);
  ui->ana4Mid->setValue(generalSettings.calibMid[3]);
  ui->ana5Mid->setValue(generalSettings.calibMid[4]);
  ui->ana6Mid->setValue(generalSettings.calibMid[5]);
  ui->ana7Mid->setValue(generalSettings.calibMid[6]);
  ui->ana8Mid->setValue(generalSettings.calibMid[7]);

  ui->ana1Pos->setValue(generalSettings.calibSpanPos[0]);
  ui->ana2Pos->setValue(generalSettings.calibSpanPos[1]);
  ui->ana3Pos->setValue(generalSettings.calibSpanPos[2]);
  ui->ana4Pos->setValue(generalSettings.calibSpanPos[3]);
  ui->ana5Pos->setValue(generalSettings.calibSpanPos[4]);
  ui->ana6Pos->setValue(generalSettings.calibSpanPos[5]);
  ui->ana7Pos->setValue(generalSettings.calibSpanPos[6]);
  ui->ana8Pos->setValue(generalSettings.calibSpanPos[7]);

  ui->PPM1->setValue(generalSettings.trainer.calib[0]);
  ui->PPM2->setValue(generalSettings.trainer.calib[1]);
  ui->PPM3->setValue(generalSettings.trainer.calib[2]);
  ui->PPM4->setValue(generalSettings.trainer.calib[3]);
  ui->PPM_MultiplierDSB->setValue((qreal)(generalSettings.PPM_Multiplier+10)/10);
}

void CalibrationPanel::on_battCalibDSB_editingFinished()
{
  generalSettings.vBatCalib = ui->battCalibDSB->value()*10;
  emit modified();
}


void CalibrationPanel::on_ana1Neg_editingFinished()
{
  generalSettings.calibSpanNeg[0] = ui->ana1Neg->value();
  emit modified();
}

void CalibrationPanel::on_ana2Neg_editingFinished()
{
  generalSettings.calibSpanNeg[1] = ui->ana2Neg->value();
  emit modified();
}

void CalibrationPanel::on_ana3Neg_editingFinished()
{
  generalSettings.calibSpanNeg[2] = ui->ana3Neg->value();
  emit modified();
}

void CalibrationPanel::on_ana4Neg_editingFinished()
{
  generalSettings.calibSpanNeg[3] = ui->ana4Neg->value();
  emit modified();
}

void CalibrationPanel::on_ana5Neg_editingFinished()
{
  generalSettings.calibSpanNeg[4] = ui->ana5Neg->value();
  emit modified();
}

void CalibrationPanel::on_ana6Neg_editingFinished()
{
  generalSettings.calibSpanNeg[5] = ui->ana6Neg->value();
  emit modified();
}

void CalibrationPanel::on_ana7Neg_editingFinished()
{
  generalSettings.calibSpanNeg[6] = ui->ana7Neg->value();
  emit modified();
}

void CalibrationPanel::on_ana8Neg_editingFinished()
{
  generalSettings.calibSpanNeg[7] = ui->ana8Neg->value();
  emit modified();
}

void CalibrationPanel::on_ana1Mid_editingFinished()
{
  generalSettings.calibMid[0] = ui->ana1Mid->value();
  emit modified();
}

void CalibrationPanel::on_ana2Mid_editingFinished()
{
  generalSettings.calibMid[1] = ui->ana2Mid->value();
  emit modified();
}

void CalibrationPanel::on_ana3Mid_editingFinished()
{
  generalSettings.calibMid[2] = ui->ana3Mid->value();
  emit modified();
}

void CalibrationPanel::on_ana4Mid_editingFinished()
{
  generalSettings.calibMid[3] = ui->ana4Mid->value();
  emit modified();
}

void CalibrationPanel::on_ana5Mid_editingFinished()
{
  generalSettings.calibMid[4] = ui->ana5Mid->value();
  emit modified();
}

void CalibrationPanel::on_ana6Mid_editingFinished()
{
  generalSettings.calibMid[5] = ui->ana6Mid->value();
  emit modified();
}

void CalibrationPanel::on_ana7Mid_editingFinished()
{
  generalSettings.calibMid[6] = ui->ana7Mid->value();
  emit modified();
}

void CalibrationPanel::on_ana8Mid_editingFinished()
{
  generalSettings.calibMid[7] = ui->ana8Mid->value();
  emit modified();
}


void CalibrationPanel::on_ana1Pos_editingFinished()
{
  generalSettings.calibSpanPos[0] = ui->ana1Pos->value();
  emit modified();
}

void CalibrationPanel::on_ana2Pos_editingFinished()
{
  generalSettings.calibSpanPos[1] = ui->ana2Pos->value();
  emit modified();
}

void CalibrationPanel::on_ana3Pos_editingFinished()
{
  generalSettings.calibSpanPos[2] = ui->ana3Pos->value();
  emit modified();
}

void CalibrationPanel::on_ana4Pos_editingFinished()
{
  generalSettings.calibSpanPos[3] = ui->ana4Pos->value();
  emit modified();
}

void CalibrationPanel::on_ana5Pos_editingFinished()
{
  generalSettings.calibSpanPos[4] = ui->ana5Pos->value();
  emit modified();
}

void CalibrationPanel::on_ana6Pos_editingFinished()
{
  generalSettings.calibSpanPos[5] = ui->ana6Pos->value();
  emit modified();
}

void CalibrationPanel::on_ana7Pos_editingFinished()
{
  generalSettings.calibSpanPos[6] = ui->ana7Pos->value();
  emit modified();
}

void CalibrationPanel::on_ana8Pos_editingFinished()
{
  generalSettings.calibSpanPos[7] = ui->ana8Pos->value();
  emit modified();
}

void CalibrationPanel::on_pot1Type_currentIndexChanged(int index)
{
  generalSettings.potsType[0] = index;
  emit modified();
}

void CalibrationPanel::on_pot2Type_currentIndexChanged(int index)
{
  generalSettings.potsType[1] = index;
  emit modified();
}

void CalibrationPanel::on_pot3Type_currentIndexChanged(int index)
{
  generalSettings.potsType[2] = index;
  emit modified();
}

void CalibrationPanel::on_serialPortMode_currentIndexChanged(int index)
{
  generalSettings.hw_uartMode = index;
  emit modified();
}
