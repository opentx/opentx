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
    ui->pot4Label->hide();
    ui->pot5Label->hide();
  }

  if (IS_TARANIS(firmware->getBoard())) {
    ui->rudName->setField(generalSettings.anaNames[0], 3, this);
    ui->eleName->setField(generalSettings.anaNames[1], 3, this);
    ui->thrName->setField(generalSettings.anaNames[2], 3, this);
    ui->ailName->setField(generalSettings.anaNames[3], 3, this);
    ui->pot1Name->setField(generalSettings.anaNames[4], 3, this);
    ui->pot2Name->setField(generalSettings.anaNames[5], 3, this);
    ui->pot3Name->setField(generalSettings.anaNames[6], 3, this);
    ui->pot4Name->setField(generalSettings.anaNames[7], 3, this);
    ui->pot5Name->setField(generalSettings.anaNames[8], 3, this);
    ui->saName->setField(generalSettings.switchNames[0], 3, this);
    ui->saType->setField(generalSettings.switchConfig[0], this);
    ui->sbName->setField(generalSettings.switchNames[1], 3, this);
    ui->sbType->setField(generalSettings.switchConfig[1], this);
    ui->scName->setField(generalSettings.switchNames[2], 3, this);
    ui->scType->setField(generalSettings.switchConfig[2], this);
    ui->sdName->setField(generalSettings.switchNames[3], 3, this);
    ui->sdType->setField(generalSettings.switchConfig[3], this);
    ui->seName->setField(generalSettings.switchNames[4], 3, this);
    ui->seType->setField(generalSettings.switchConfig[4], this);
    ui->sfName->setField(generalSettings.switchNames[5], 3, this);
    ui->sfType->setField(generalSettings.switchConfig[5], this);
    ui->sgName->setField(generalSettings.switchNames[6], 3, this);
    ui->sgType->setField(generalSettings.switchConfig[6], this);
    ui->shName->setField(generalSettings.switchNames[7], 3, this);
    ui->shType->setField(generalSettings.switchConfig[7], this);
    ui->siName->setField(generalSettings.switchNames[8], 3, this);
    ui->sjName->setField(generalSettings.switchNames[9], 3, this);
    ui->skName->setField(generalSettings.switchNames[10], 3, this);
    ui->slName->setField(generalSettings.switchNames[11], 3, this);
    ui->smName->setField(generalSettings.switchNames[12], 3, this);
    ui->snName->setField(generalSettings.switchNames[13], 3, this);
    connect(ui->saType, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(ui->sbType, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(ui->scType, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(ui->sdType, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(ui->seType, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(ui->sgType, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
  }
  else {
    ui->rudLabel->hide();
    ui->rudName->hide();
    ui->eleLabel->hide();
    ui->eleName->hide();
    ui->thrLabel->hide();
    ui->thrName->hide();
    ui->ailLabel->hide();
    ui->ailName->hide();
    ui->pot1Name->hide();
    ui->pot2Name->hide();
    ui->pot3Name->hide();
    ui->pot4Name->hide();
    ui->pot5Name->hide();
    ui->saLabel->hide();
    ui->saName->hide();
    ui->saType->hide();
    ui->sbLabel->hide();
    ui->sbName->hide();
    ui->sbType->hide();
    ui->scLabel->hide();
    ui->scName->hide();
    ui->scType->hide();
    ui->sdLabel->hide();
    ui->sdName->hide();
    ui->sdType->hide();
    ui->seLabel->hide();
    ui->seName->hide();
    ui->seType->hide();
    ui->sfLabel->hide();
    ui->sfName->hide();
    ui->sfType->hide();
    ui->sgLabel->hide();
    ui->sgName->hide();
    ui->sgType->hide();
    ui->shLabel->hide();
    ui->shName->hide();
    ui->shType->hide();
    ui->siName->hide();
    ui->sjName->hide();
    ui->skName->hide();
    ui->slName->hide();
    ui->smName->hide();
    ui->snName->hide();
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

void CalibrationPanel::update()
{
  ui->siLabel->setVisible(generalSettings.switchConfig[0] == GeneralSettings::SWITCH_2x2POS);
  ui->siName->setVisible(generalSettings.switchConfig[0] == GeneralSettings::SWITCH_2x2POS);
  ui->sjLabel->setVisible(generalSettings.switchConfig[1] == GeneralSettings::SWITCH_2x2POS);
  ui->sjName->setVisible(generalSettings.switchConfig[1] == GeneralSettings::SWITCH_2x2POS);
  ui->skLabel->setVisible(generalSettings.switchConfig[2] == GeneralSettings::SWITCH_2x2POS);
  ui->skName->setVisible(generalSettings.switchConfig[2] == GeneralSettings::SWITCH_2x2POS);
  ui->slLabel->setVisible(generalSettings.switchConfig[3] == GeneralSettings::SWITCH_2x2POS);
  ui->slName->setVisible(generalSettings.switchConfig[3] == GeneralSettings::SWITCH_2x2POS);
  ui->smLabel->setVisible(generalSettings.switchConfig[4] == GeneralSettings::SWITCH_2x2POS);
  ui->smName->setVisible(generalSettings.switchConfig[4] == GeneralSettings::SWITCH_2x2POS);
  ui->snLabel->setVisible(generalSettings.switchConfig[6] == GeneralSettings::SWITCH_2x2POS);
  ui->snName->setVisible(generalSettings.switchConfig[6] == GeneralSettings::SWITCH_2x2POS);
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
