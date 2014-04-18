#include "generaledit.h"
#include "ui_generaledit.h"
#include "helpers.h"
#include "appdata.h"
#include <QDateTime>
#include <QtGui>

#define BIT_WARN_THR     ( 0x01 )
#define BIT_WARN_SW      ( 0x02 )
#define BIT_WARN_MEM     ( 0x04 )
#define BIT_WARN_BEEP    ( 0x80 )
#define BIT_BEEP_VAL     ( 0x38 ) // >>3
#define BEEP_VAL_SHIFT   3

GeneralEdit::GeneralEdit(RadioData &radioData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneralEdit),
    radioData(radioData),
    g_eeGeneral(radioData.generalSettings)
{
    ui->setupUi(this);
    this->setWindowIcon(CompanionIcon("open.png"));

    QString firmware_id = g.profile[g.id()].fwType();
    ui->tabWidget->setCurrentIndex( g.generalEditTab() );
    QString name=g.profile[g.id()].name();
    if (name.isEmpty()) {
      ui->calstore_PB->setDisabled(true);
    }
    EEPROMInterface *eepromInterface = GetEepromInterface();
    QLabel * pmsl[] = {ui->ro_label,ui->ro1_label,ui->ro2_label,ui->ro3_label,ui->ro4_label,ui->ro5_label,ui->ro6_label,ui->ro7_label,ui->ro8_label, NULL};
    QSlider * tpmsld[] = {ui->chkSA, ui->chkSB, ui->chkSC, ui->chkSD, ui->chkSE, ui->chkSF, ui->chkSG, ui->chkSH, NULL};

    if (IS_TARANIS(eepromInterface->getBoard())) {
      ui->contrastSB->setMinimum(0);
      if (firmware_id.contains("readonly")) {
        uint16_t switchstate=(g_eeGeneral.switchUnlockStates);
        ui->chkSA->setValue(switchstate & 0x3);
        switchstate >>= 2;
        ui->chkSB->setValue(switchstate & 0x3);
        switchstate >>= 2;
        ui->chkSC->setValue(switchstate & 0x3);
        switchstate >>= 2;
        ui->chkSD->setValue(switchstate & 0x3);
        switchstate >>= 2;
        ui->chkSE->setValue(switchstate & 0x3);
        switchstate >>= 2;
        ui->chkSF->setValue((switchstate & 0x3)/2);
        switchstate >>= 2;
        ui->chkSG->setValue(switchstate & 0x3);
        switchstate >>= 2;
        ui->chkSH->setValue(switchstate & 0x3);      
      } else {
        for (int i=0; pmsl[i]; i++) {
          pmsl[i]->hide();
        }
        for (int i=0; tpmsld[i]; i++) {
          tpmsld[i]->hide();
        }
        this->layout()->removeItem(ui->TaranisReadOnlyUnlock);
      }
    }
    else {
      for (int i=0; pmsl[i]; i++) {
        pmsl[i]->hide();
      }
      for (int i=0; tpmsld[i]; i++) {
        tpmsld[i]->hide();
      }
      this->layout()->removeItem(ui->TaranisReadOnlyUnlock);      
    }
    ui->profile_CB->clear();
    for ( int i = 0; i < MAX_PROFILES; ++i) {
      QString name=g.profile[i].name();
      if (!name.isEmpty()) {
        ui->profile_CB->addItem(name, i);
        if (i==g.id()) {
          ui->profile_CB->setCurrentIndex(ui->profile_CB->count()-1);
        }
      }
    }

    switchDefPosEditLock=true;
    populateBacklightCB(ui->backlightswCB, g_eeGeneral.backlightMode);
    bool voice = current_firmware_variant.id.contains("voice");
    if (!GetCurrentFirmware()->getCapability(MultiLangVoice)) {
      ui->VoiceLang_label->hide();
      ui->voiceLang_CB->hide();
    } else {
      voiceLangEditLock=true;
      populateVoiceLangCB(ui->voiceLang_CB, g_eeGeneral.ttsLanguage);
      voiceLangEditLock=false;
    }
    bool mavlink = current_firmware_variant.id.contains("mavlink");
    if (!mavlink) {
      ui->mavbaud_CB->hide();
      ui->mavbaud_label->hide();
    } else {
      mavbaudEditLock=true;
      ui->mavbaud_CB->setCurrentIndex(g_eeGeneral.mavbaud);
      populateVoiceLangCB(ui->voiceLang_CB, g_eeGeneral.ttsLanguage);
      mavbaudEditLock=false;
    }
    
    if (!GetCurrentFirmware()->getCapability(HasContrast)) {
      ui->contrastSB->hide();
      ui->label_contrast->hide();
    }
    if (!GetCurrentFirmware()->getCapability(HasSoundMixer)) {
      ui->beepVolume_SL->hide();
      ui->beepVolume_label->hide();
      ui->varioVolume_SL->hide();
      ui->varioVolume_label->hide();
      ui->bgVolume_SL->hide();
      ui->bgVolume_label->hide();
      ui->wavVolume_SL->hide();
      ui->wavVolume_label->hide();
      ui->varioP0_label->hide();
      ui->varioP0_SB->hide();
      ui->varioPMax_label->hide();
      ui->varioPMax_SB->hide();
      ui->varioR0_label->hide();
      ui->varioR0_SB->hide();
    } else {
      ui->beepVolume_SL->setValue(g_eeGeneral.beepVolume);
      ui->varioVolume_SL->setValue(g_eeGeneral.varioVolume);
      ui->bgVolume_SL->setValue(g_eeGeneral.backgroundVolume);
      ui->wavVolume_SL->setValue(g_eeGeneral.wavVolume);
      ui->varioP0_SB->setValue(700+(g_eeGeneral.varioPitch*10));
      updateVarioPitchRange();
      ui->varioPMax_SB->setValue(700+(g_eeGeneral.varioPitch*10)+1000+(g_eeGeneral.varioRange*10));
      ui->varioR0_SB->setValue(500+(g_eeGeneral.varioRepeat*10));
    }
    if (!GetCurrentFirmware()->getCapability(HasFAIMode)) {
      ui->faimode_CB->hide();
      ui->label_faimode->hide();
    }
    else {
      ui->faimode_CB->setChecked(g_eeGeneral.fai);
    }
    if (!GetCurrentFirmware()->getCapability( HasPxxCountry)) {
      ui->countrycode_label->hide();
      ui->countrycode_CB->hide();
      layout()->removeItem(ui->pxxCountry);
    } else {
      ui->countrycode_CB->setCurrentIndex(g_eeGeneral.countryCode);
    }
    if (!GetCurrentFirmware()->getCapability( HasGeneralUnits)) {
      ui->units_label->hide();
      ui->units_CB->hide();
    } else {
      ui->units_CB->setCurrentIndex(g_eeGeneral.imperial);
    }
    
    if (!GetCurrentFirmware()->getCapability(TelemetryTimeshift)) {
      ui->label_timezone->hide();
      ui->timezoneSB->hide();
      ui->timezoneSB->setDisabled(true);
      ui->gpsFormatCB->hide();
      ui->gpsFormatLabel->hide();
    }
    ui->gpsFormatCB->setCurrentIndex(g_eeGeneral.gpsFormat);
    ui->timezoneSB->setValue(g_eeGeneral.timezone);
    
    if (!GetCurrentFirmware()->getCapability(OptrexDisplay)) {
      ui->label_displayType->hide();
      ui->displayTypeCB->setDisabled(true);
      ui->displayTypeCB->hide();
    }
    if (!GetCurrentFirmware()->getCapability(HasVolume) && !voice) {
      ui->volume_SB->hide();
      ui->volume_SB->setDisabled(true);
      ui->label_volume->hide();
    } else {
      ui->volume_SB->setMaximum(GetCurrentFirmware()->getCapability(MaxVolume));
    }    
    if (!GetCurrentFirmware()->getCapability(HasBrightness)) {
      ui->BLBright_SB->hide();
      ui->BLBright_SB->setDisabled(true);
      ui->label_BLBright->hide();
    }
    if (!GetCurrentFirmware()->getCapability(HasCurrentCalibration)) {
      ui->CurrentCalib_SB->hide();
      ui->CurrentCalib_SB->setDisabled(true);
      ui->label_CurrentCalib->hide();
    }
    
    ui->tabWidget->setCurrentIndex(0);

    if (!GetCurrentFirmware()->getCapability(SoundMod)) {
      ui->soundModeCB->setDisabled(true);
      ui->label_soundMode->hide();
      ui->soundModeCB->hide();
    }

    if (!GetCurrentFirmware()->getCapability(SoundPitch)) {
      ui->speakerPitchSB->setDisabled(true);
      ui->label_speakerPitch->hide();
      ui->speakerPitchSB->hide();
    }
    
    if (!GetCurrentFirmware()->getCapability(Haptic)) {
      ui->hapticStrengthSB->setDisabled(true);
      ui->hapticStrengthSB->hide();
      ui->label_hapticStrengthSB->hide();
    } 
    
    if (!GetCurrentFirmware()->getCapability(HapticMode)) {
      ui->hapticmodeCB->setDisabled(true);
      ui->hapticmodeCB->hide();
      ui->label_hapticmode->hide();
    } 

    int renumber=GetCurrentFirmware()->getCapability(RotaryEncoders);
    if (renumber==0) {
      ui->re_label->hide();
      ui->re_CB->hide();
    } else {
      populateRotEncCB(ui->re_CB, g_eeGeneral.reNavigation, renumber);
    }
    ui->contrastSB->setValue(g_eeGeneral.contrast);
    ui->battwarningDSB->setValue((double)g_eeGeneral.vBatWarn/10);
    ui->backlightautoSB->setValue(g_eeGeneral.backlightDelay*5);
    ui->inactimerSB->setValue(g_eeGeneral.inactivityTimer);
    
    ui->memwarnChkB->setChecked(!g_eeGeneral.disableMemoryWarning);   //Default is zero=checked
    ui->alarmwarnChkB->setChecked(!g_eeGeneral.disableAlarmWarning);//Default is zero=checked

    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      ui->splashScreenChkB->hide();
      ui->splashScreenDuration->setCurrentIndex(3-g_eeGeneral.splashDuration);
    }
    else {
      ui->splashScreenDuration->hide();
      ui->splashScreenChkB->setChecked(!g_eeGeneral.splashMode);
    }

    ui->trnMode_1->setCurrentIndex(g_eeGeneral.trainer.mix[0].mode);
    ui->trnChn_1->setCurrentIndex(g_eeGeneral.trainer.mix[0].src);
    ui->trnWeight_1->setValue(g_eeGeneral.trainer.mix[0].weight);
    ui->trnMode_2->setCurrentIndex(g_eeGeneral.trainer.mix[1].mode);
    ui->trnChn_2->setCurrentIndex(g_eeGeneral.trainer.mix[1].src);
    ui->trnWeight_2->setValue(g_eeGeneral.trainer.mix[1].weight);
    ui->trnMode_3->setCurrentIndex(g_eeGeneral.trainer.mix[2].mode);
    ui->trnChn_3->setCurrentIndex(g_eeGeneral.trainer.mix[2].src);
    ui->trnWeight_3->setValue(g_eeGeneral.trainer.mix[2].weight);
    ui->trnMode_4->setCurrentIndex(g_eeGeneral.trainer.mix[3].mode);
    ui->trnChn_4->setCurrentIndex(g_eeGeneral.trainer.mix[3].src);
    ui->trnWeight_4->setValue(g_eeGeneral.trainer.mix[3].weight);
    int potsnum=GetCurrentFirmware()->getCapability(Pots);
    if (potsnum==3) {
      ui->label_pot4->hide();
      ui->ana8Neg->hide();
      ui->ana8Mid->hide();
      ui->ana8Pos->hide();
    }
    setValues();
    switchDefPosEditLock=false;
    QTimer::singleShot(0, this, SLOT(shrink()));
    for (int i=0; tpmsld[i]; i++) {
      connect(tpmsld[i], SIGNAL(valueChanged(int)),this,SLOT(unlockSwitchEdited()));
    }

    if (GetCurrentFirmware()->getCapability(MultiposPots)) {
      ui->pot1Type->setCurrentIndex(g_eeGeneral.potsType[0]);
      ui->pot2Type->setCurrentIndex(g_eeGeneral.potsType[1]);
      ui->pot3Type->setCurrentIndex(g_eeGeneral.potsType[2]);
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
    
    if (IS_TARANIS(eepromInterface->getBoard())) {
      ui->serialPortMode->setCurrentIndex(g_eeGeneral.hw_uartMode);
    }
    else {
      ui->serialPortMode->hide();
      ui->serialPortLabel->hide();
    }
}

GeneralEdit::~GeneralEdit()
{
    delete ui;
}

void GeneralEdit::on_pot1Type_currentIndexChanged(int index)
{
  g_eeGeneral.potsType[0] = index;
  updateSettings();
}

void GeneralEdit::on_pot2Type_currentIndexChanged(int index)
{
  g_eeGeneral.potsType[1] = index;
  updateSettings();
}

void GeneralEdit::on_pot3Type_currentIndexChanged(int index)
{
  g_eeGeneral.potsType[2] = index;
  updateSettings();
}

void GeneralEdit::on_serialPortMode_currentIndexChanged(int index)
{
  g_eeGeneral.hw_uartMode = index;
  updateSettings();
}

void GeneralEdit::unlockSwitchEdited()
{
  int i=0;
  i|=(((uint16_t)ui->chkSA->value()));
  i|=(((uint16_t)ui->chkSB->value())<<2);
  i|=(((uint16_t)ui->chkSC->value())<<4);
  i|=(((uint16_t)ui->chkSD->value())<<6);
  i|=(((uint16_t)ui->chkSE->value())<<8);
  i|=(((uint16_t)ui->chkSF->value())<<10);
  i|=(((uint16_t)ui->chkSG->value())<<12);
  i|=(((uint16_t)ui->chkSH->value())<<14);
  g_eeGeneral.switchUnlockStates=i;
  updateSettings();
}

void GeneralEdit::setValues()
{
    ui->beeperCB->setCurrentIndex(g_eeGeneral.beeperMode+2);
    ui->channelorderCB->setCurrentIndex(g_eeGeneral.templateSetup);
    ui->stickmodeCB->setCurrentIndex(g_eeGeneral.stickMode);
    if (!GetCurrentFirmware()->getCapability(HapticLength)) {
      ui->label_HL->hide();
      ui->hapticLengthCB->hide();
    } else {
      ui->hapticLengthCB->setCurrentIndex(g_eeGeneral.hapticLength+2);
    }
    ui->BLBright_SB->setValue(100-g_eeGeneral.backlightBright);
    ui->soundModeCB->setCurrentIndex(g_eeGeneral.speakerMode);
    ui->volume_SB->setValue(g_eeGeneral.speakerVolume);
    ui->beeperlenCB->setCurrentIndex(g_eeGeneral.beeperLength+2);
    ui->speakerPitchSB->setValue(g_eeGeneral.speakerPitch);
    ui->hapticStrengthSB->setValue(g_eeGeneral.hapticStrength);
    ui->hapticmodeCB->setCurrentIndex(g_eeGeneral.hapticMode+2);
    ui->battCalibDSB->setValue((double)g_eeGeneral.vBatCalib/10);
    ui->CurrentCalib_SB->setValue((double)g_eeGeneral.currentCalib);

    ui->ana1Neg->setValue(g_eeGeneral.calibSpanNeg[0]);
    ui->ana2Neg->setValue(g_eeGeneral.calibSpanNeg[1]);
    ui->ana3Neg->setValue(g_eeGeneral.calibSpanNeg[2]);
    ui->ana4Neg->setValue(g_eeGeneral.calibSpanNeg[3]);
    ui->ana5Neg->setValue(g_eeGeneral.calibSpanNeg[4]);
    ui->ana6Neg->setValue(g_eeGeneral.calibSpanNeg[5]);
    ui->ana7Neg->setValue(g_eeGeneral.calibSpanNeg[6]);
    ui->ana8Neg->setValue(g_eeGeneral.calibSpanNeg[7]);

    ui->ana1Mid->setValue(g_eeGeneral.calibMid[0]);
    ui->ana2Mid->setValue(g_eeGeneral.calibMid[1]);
    ui->ana3Mid->setValue(g_eeGeneral.calibMid[2]);
    ui->ana4Mid->setValue(g_eeGeneral.calibMid[3]);
    ui->ana5Mid->setValue(g_eeGeneral.calibMid[4]);
    ui->ana6Mid->setValue(g_eeGeneral.calibMid[5]);
    ui->ana7Mid->setValue(g_eeGeneral.calibMid[6]);
    ui->ana8Mid->setValue(g_eeGeneral.calibMid[7]);

    ui->ana1Pos->setValue(g_eeGeneral.calibSpanPos[0]);
    ui->ana2Pos->setValue(g_eeGeneral.calibSpanPos[1]);
    ui->ana3Pos->setValue(g_eeGeneral.calibSpanPos[2]);
    ui->ana4Pos->setValue(g_eeGeneral.calibSpanPos[3]);
    ui->ana5Pos->setValue(g_eeGeneral.calibSpanPos[4]);
    ui->ana6Pos->setValue(g_eeGeneral.calibSpanPos[5]);
    ui->ana7Pos->setValue(g_eeGeneral.calibSpanPos[6]);
    ui->ana8Pos->setValue(g_eeGeneral.calibSpanPos[7]);

    ui->PPM1->setValue(g_eeGeneral.trainer.calib[0]);
    ui->PPM2->setValue(g_eeGeneral.trainer.calib[1]);
    ui->PPM3->setValue(g_eeGeneral.trainer.calib[2]);
    ui->PPM4->setValue(g_eeGeneral.trainer.calib[3]);  
    ui->PPM_MultiplierDSB->setValue((qreal)(g_eeGeneral.PPM_Multiplier+10)/10);
}

void GeneralEdit::updateSettings()
{
    radioData.generalSettings = g_eeGeneral;
    emit modelValuesChanged();
}

void GeneralEdit::on_contrastSB_editingFinished()
{
    g_eeGeneral.contrast = ui->contrastSB->value();
    updateSettings();
}

void GeneralEdit::on_battwarningDSB_editingFinished()
{
    g_eeGeneral.vBatWarn = (int)(ui->battwarningDSB->value()*10);
    updateSettings();
}

void GeneralEdit::on_battCalibDSB_editingFinished()
{
    g_eeGeneral.vBatCalib = ui->battCalibDSB->value()*10;
    updateSettings();
}

void GeneralEdit::on_re_CB_currentIndexChanged(int index)
{
  g_eeGeneral.reNavigation = ui->re_CB->currentIndex();
  updateSettings();
}

void GeneralEdit::on_countrycode_CB_currentIndexChanged(int index)
{
  g_eeGeneral.countryCode = ui->countrycode_CB->currentIndex();
  updateSettings();
}

void GeneralEdit::on_units_CB_currentIndexChanged(int index)
{
  g_eeGeneral.imperial = ui->units_CB->currentIndex();
  updateSettings();
}

void GeneralEdit::on_backlightswCB_currentIndexChanged(int index)
{
  if (switchDefPosEditLock)
    return;
  g_eeGeneral.backlightMode = ui->backlightswCB->currentIndex();
  updateSettings();
}

void GeneralEdit::on_mavbaud_CB_currentIndexChanged(int index)
{
  if (mavbaudEditLock)
    return;
  g_eeGeneral.mavbaud = ui->mavbaud_CB->currentIndex();
  updateSettings();
}


void GeneralEdit::on_voiceLang_CB_currentIndexChanged(int index)
{
  if (voiceLangEditLock)
    return;
  QString code=ui->voiceLang_CB->itemData(index).toString();
  for (int i=0; i<2; i++) {
    g_eeGeneral.ttsLanguage[i]=code.at(i).toAscii();
  }
  g_eeGeneral.ttsLanguage[2]=0;
  updateSettings();
}

void GeneralEdit::on_beeperlenCB_currentIndexChanged(int index)
{
  g_eeGeneral.beeperLength = index-2;
  updateSettings();
}

void GeneralEdit::on_hapticLengthCB_currentIndexChanged(int index)
{
  g_eeGeneral.hapticLength = index-2;
  updateSettings();
}

void GeneralEdit::on_gpsFormatCB_currentIndexChanged(int index)
{
  g_eeGeneral.gpsFormat = index;
  updateSettings();
}


void GeneralEdit::on_backlightautoSB_editingFinished()
{
  int i = ui->backlightautoSB->value()/5;
  if((i*5)!=ui->backlightautoSB->value())
    ui->backlightautoSB->setValue(i*5);
  else
  {
    g_eeGeneral.backlightDelay = i;
    updateSettings();
  }
}

void GeneralEdit::on_timezoneSB_editingFinished()
{
  g_eeGeneral.timezone = ui->timezoneSB->value();
  updateSettings();
}

void GeneralEdit::on_inactimerSB_editingFinished()
{
    g_eeGeneral.inactivityTimer = ui->inactimerSB->value();
    updateSettings();
}

void GeneralEdit::on_memwarnChkB_stateChanged(int )
{
    g_eeGeneral.disableMemoryWarning = ui->memwarnChkB->isChecked() ? 0 : 1;
    updateSettings();
}

void GeneralEdit::on_alarmwarnChkB_stateChanged(int )
{
    g_eeGeneral.disableAlarmWarning = ui->alarmwarnChkB->isChecked() ? 0 : 1;
    updateSettings();
}

void GeneralEdit::on_beeperCB_currentIndexChanged(int index)
{
    g_eeGeneral.beeperMode = (BeeperMode)(index-2);
    updateSettings();
}

void GeneralEdit::on_displayTypeCB_currentIndexChanged(int index)
{
    g_eeGeneral.optrexDisplay = index;
    updateSettings();
}

void GeneralEdit::on_hapticmodeCB_currentIndexChanged(int index)
{
    g_eeGeneral.hapticMode = (BeeperMode)(index-2);
    updateSettings();
}


void GeneralEdit::on_channelorderCB_currentIndexChanged(int index)
{
    g_eeGeneral.templateSetup = index;
    updateSettings();
}

void GeneralEdit::on_stickmodeCB_currentIndexChanged(int index)
{
    g_eeGeneral.stickMode = index;
    updateSettings();
}

void GeneralEdit::on_trnMode_1_currentIndexChanged(int index)
{
    g_eeGeneral.trainer.mix[0].mode = index;
    updateSettings();
}

void GeneralEdit::on_trnChn_1_currentIndexChanged(int index)
{
    g_eeGeneral.trainer.mix[0].src = index;
    updateSettings();
}

void GeneralEdit::on_trnWeight_1_editingFinished()
{
    g_eeGeneral.trainer.mix[0].weight = ui->trnWeight_1->value();
    updateSettings();
}

void GeneralEdit::on_trnMode_2_currentIndexChanged(int index)
{
    g_eeGeneral.trainer.mix[1].mode = index;
    updateSettings();
}

void GeneralEdit::on_trnChn_2_currentIndexChanged(int index)
{
    g_eeGeneral.trainer.mix[1].src = index;
    updateSettings();
}

void GeneralEdit::on_trnWeight_2_editingFinished()
{
    g_eeGeneral.trainer.mix[1].weight = ui->trnWeight_2->value();
    updateSettings();
}

void GeneralEdit::on_trnMode_3_currentIndexChanged(int index)
{
    g_eeGeneral.trainer.mix[2].mode = index;
    updateSettings();
}

void GeneralEdit::on_trnChn_3_currentIndexChanged(int index)
{
    g_eeGeneral.trainer.mix[2].src = index;
    updateSettings();
}

void GeneralEdit::on_trnWeight_3_editingFinished()
{
    g_eeGeneral.trainer.mix[2].weight = ui->trnWeight_3->value();
    updateSettings();
}

void GeneralEdit::on_trnMode_4_currentIndexChanged(int index)
{
    g_eeGeneral.trainer.mix[3].mode = index;
    updateSettings();
}

void GeneralEdit::on_trnChn_4_currentIndexChanged(int index)
{
    g_eeGeneral.trainer.mix[3].src = index;
    updateSettings();
}

void GeneralEdit::on_trnWeight_4_editingFinished()
{
    g_eeGeneral.trainer.mix[3].weight = ui->trnWeight_4->value();
    updateSettings();
}

void GeneralEdit::on_ana1Neg_editingFinished()
{
    g_eeGeneral.calibSpanNeg[0] = ui->ana1Neg->value();
    updateSettings();
}

void GeneralEdit::on_ana2Neg_editingFinished()
{
    g_eeGeneral.calibSpanNeg[1] = ui->ana2Neg->value();
    updateSettings();
}

void GeneralEdit::on_ana3Neg_editingFinished()
{
    g_eeGeneral.calibSpanNeg[2] = ui->ana3Neg->value();
    updateSettings();
}

void GeneralEdit::on_ana4Neg_editingFinished()
{
    g_eeGeneral.calibSpanNeg[3] = ui->ana4Neg->value();
    updateSettings();
}

void GeneralEdit::on_ana5Neg_editingFinished()
{
    g_eeGeneral.calibSpanNeg[4] = ui->ana5Neg->value();
    updateSettings();
}

void GeneralEdit::on_ana6Neg_editingFinished()
{
    g_eeGeneral.calibSpanNeg[5] = ui->ana6Neg->value();
    updateSettings();
}

void GeneralEdit::on_ana7Neg_editingFinished()
{
    g_eeGeneral.calibSpanNeg[6] = ui->ana7Neg->value();
    updateSettings();
}

void GeneralEdit::on_ana8Neg_editingFinished()
{
    g_eeGeneral.calibSpanNeg[7] = ui->ana8Neg->value();
    updateSettings();
}

void GeneralEdit::on_ana1Mid_editingFinished()
{
    g_eeGeneral.calibMid[0] = ui->ana1Mid->value();
    updateSettings();
}

void GeneralEdit::on_ana2Mid_editingFinished()
{
    g_eeGeneral.calibMid[1] = ui->ana2Mid->value();
    updateSettings();
}

void GeneralEdit::on_ana3Mid_editingFinished()
{
    g_eeGeneral.calibMid[2] = ui->ana3Mid->value();
    updateSettings();
}

void GeneralEdit::on_ana4Mid_editingFinished()
{
    g_eeGeneral.calibMid[3] = ui->ana4Mid->value();
    updateSettings();
}

void GeneralEdit::on_ana5Mid_editingFinished()
{
    g_eeGeneral.calibMid[4] = ui->ana5Mid->value();
    updateSettings();
}

void GeneralEdit::on_ana6Mid_editingFinished()
{
    g_eeGeneral.calibMid[5] = ui->ana6Mid->value();
    updateSettings();
}

void GeneralEdit::on_ana7Mid_editingFinished()
{
    g_eeGeneral.calibMid[6] = ui->ana7Mid->value();
    updateSettings();
}

void GeneralEdit::on_ana8Mid_editingFinished()
{
    g_eeGeneral.calibMid[7] = ui->ana8Mid->value();
    updateSettings();
}


void GeneralEdit::on_ana1Pos_editingFinished()
{
    g_eeGeneral.calibSpanPos[0] = ui->ana1Pos->value();
    updateSettings();
}

void GeneralEdit::on_ana2Pos_editingFinished()
{
    g_eeGeneral.calibSpanPos[1] = ui->ana2Pos->value();
    updateSettings();
}

void GeneralEdit::on_ana3Pos_editingFinished()
{
    g_eeGeneral.calibSpanPos[2] = ui->ana3Pos->value();
    updateSettings();
}

void GeneralEdit::on_ana4Pos_editingFinished()
{
    g_eeGeneral.calibSpanPos[3] = ui->ana4Pos->value();
    updateSettings();
}

void GeneralEdit::on_ana5Pos_editingFinished()
{
    g_eeGeneral.calibSpanPos[4] = ui->ana5Pos->value();
    updateSettings();
}

void GeneralEdit::on_ana6Pos_editingFinished()
{
    g_eeGeneral.calibSpanPos[5] = ui->ana6Pos->value();
    updateSettings();
}

void GeneralEdit::on_ana7Pos_editingFinished()
{
    g_eeGeneral.calibSpanPos[6] = ui->ana7Pos->value();
    updateSettings();
}

void GeneralEdit::on_ana8Pos_editingFinished()
{
    g_eeGeneral.calibSpanPos[7] = ui->ana8Pos->value();
    updateSettings();
}

void GeneralEdit::on_volume_SB_editingFinished()
{
    g_eeGeneral.speakerVolume = ui->volume_SB->value();
    updateSettings();
}

void GeneralEdit::on_BLBright_SB_editingFinished()
{
    g_eeGeneral.backlightBright = 100 - ui->BLBright_SB->value();
    updateSettings();
}

void GeneralEdit::on_CurrentCalib_SB_editingFinished()
{
    g_eeGeneral.currentCalib = ui->CurrentCalib_SB->value();
    updateSettings();
}

void GeneralEdit::on_beepVolume_SL_valueChanged()
{
    g_eeGeneral.beepVolume=ui->beepVolume_SL->value();
    updateSettings();
}

void GeneralEdit::on_wavVolume_SL_valueChanged()
{
    g_eeGeneral.wavVolume=ui->wavVolume_SL->value();
    updateSettings();
}

void GeneralEdit::on_varioVolume_SL_valueChanged()
{
    g_eeGeneral.varioVolume=ui->varioVolume_SL->value();
    updateSettings();
}

void GeneralEdit::on_bgVolume_SL_valueChanged()
{
    g_eeGeneral.backgroundVolume=ui->bgVolume_SL->value();
    updateSettings();
}

void GeneralEdit::on_varioP0_SB_editingFinished()
{
  g_eeGeneral.varioPitch = (ui->varioP0_SB->value()-700)/10;
  updateVarioPitchRange();
  updateSettings();
}

void GeneralEdit::updateVarioPitchRange()
{
  ui->varioPMax_SB->setMaximum(700+(g_eeGeneral.varioPitch*10)+1000+800);
  ui->varioPMax_SB->setMinimum(700+(g_eeGeneral.varioPitch*10)+1000-800);
}

void GeneralEdit::on_varioPMax_SB_editingFinished()
{
  g_eeGeneral.varioRange = (ui->varioPMax_SB->value()-(700+(g_eeGeneral.varioPitch*10))-1000)/10;
  updateSettings();
}

void GeneralEdit::on_varioR0_SB_editingFinished()
{
  g_eeGeneral.varioRepeat = (ui->varioR0_SB->value()-500)/10;
  updateSettings();
}

void GeneralEdit::on_PPM1_editingFinished()
{
    g_eeGeneral.trainer.calib[0] = ui->PPM1->value();
    updateSettings();
}

void GeneralEdit::on_PPM2_editingFinished()
{
    g_eeGeneral.trainer.calib[1] = ui->PPM2->value();
    updateSettings();
}

void GeneralEdit::on_PPM3_editingFinished()
{
    g_eeGeneral.trainer.calib[2] = ui->PPM3->value();
    updateSettings();
}

void GeneralEdit::on_PPM4_editingFinished()
{
    g_eeGeneral.trainer.calib[3] = ui->PPM4->value();
    updateSettings();
}

void GeneralEdit::on_tabWidget_currentChanged(int index)
{
    g.generalEditTab(index);
}

void GeneralEdit::on_splashScreenChkB_stateChanged(int )
{
    g_eeGeneral.splashMode = ui->splashScreenChkB->isChecked() ? 0 : 1;
    updateSettings();
}

void GeneralEdit::on_splashScreenDuration_currentIndexChanged(int index)
{
    g_eeGeneral.splashDuration = 3-index;
    updateSettings();
}

void GeneralEdit::on_PPM_MultiplierDSB_editingFinished()
{
    g_eeGeneral.PPM_Multiplier = (int)(ui->PPM_MultiplierDSB->value()*10)-10;
    updateSettings();
}

void GeneralEdit::on_speakerPitchSB_editingFinished()
{
    g_eeGeneral.speakerPitch = ui->speakerPitchSB->value();
    updateSettings();
}

void GeneralEdit::on_hapticStrengthSB_editingFinished()
{
    g_eeGeneral.hapticStrength = ui->hapticStrengthSB->value();
    updateSettings();
}

void GeneralEdit::on_soundModeCB_currentIndexChanged(int index)
{
    g_eeGeneral.speakerMode = index;
    updateSettings();
}

void GeneralEdit::on_faimode_CB_stateChanged(int )
{
    if (ui->faimode_CB->isChecked()) {
      int ret = QMessageBox::question(this, "Companion", 
                     tr("If you enable FAI, you loose the vario, the play functions, the telemetry screen.\nThis function cannot be disabled by the radio.\nAre you sure ?") ,
                     QMessageBox::Yes | QMessageBox::No);
      if (ret==QMessageBox::Yes) {
        g_eeGeneral.fai = true;
      } else {
        ui->faimode_CB->setChecked(false);
      }
    } else {
      g_eeGeneral.fai = false;
    }
    updateSettings();
}

void GeneralEdit::on_calretrieve_PB_clicked()
{
  int profile_id=ui->profile_CB->itemData(ui->profile_CB->currentIndex()).toInt();
  QString calib=g.profile[profile_id].stickPotCalib();
  int potsnum=GetCurrentFirmware()->getCapability(Pots);
  if (calib.isEmpty()) {
    return;
  } else {
    QString trainercalib = g.profile[profile_id].trainerCalib();
    int8_t vBatCalib = (int8_t)g.profile[profile_id].vBatCalib();
    int8_t currentCalib = (int8_t)g.profile[profile_id].currentCalib();
    int8_t PPM_Multiplier = (int8_t)g.profile[profile_id].ppmMultiplier();
    uint8_t GSStickMode = (uint8_t)g.profile[profile_id].gsStickMode();
    uint8_t vBatWarn = (uint8_t)g.profile[profile_id].vBatWarn();
    QString DisplaySet = g.profile[profile_id].display();
    QString BeeperSet = g.profile[profile_id].beeper();
    QString HapticSet = g.profile[profile_id].haptic();
    QString SpeakerSet = g.profile[profile_id].speaker();
    QString CountrySet = g.profile[profile_id].countryCode();
    
    if ((calib.length()==(NUM_STICKS+potsnum)*12) && (trainercalib.length()==16)) {
      QString Byte;
      int16_t byte16;
      bool ok;
      for (int i=0; i<(NUM_STICKS+potsnum); i++) {
        Byte=calib.mid(i*12,4);
        byte16=(int16_t)Byte.toInt(&ok,16);
        if (ok)
          g_eeGeneral.calibMid[i]=byte16;
        Byte=calib.mid(4+i*12,4);
        byte16=(int16_t)Byte.toInt(&ok,16);
        if (ok)
          g_eeGeneral.calibSpanNeg[i]=byte16;
        Byte=calib.mid(8+i*12,4);
        byte16=(int16_t)Byte.toInt(&ok,16);
        if (ok)
          g_eeGeneral.calibSpanPos[i]=byte16;
      }
      for (int i=0; i<4; i++) {
        Byte=trainercalib.mid(i*4,4);
        byte16=(int16_t)Byte.toInt(&ok,16);
        if (ok)
          g_eeGeneral.trainer.calib[i]=byte16;
      }
      g_eeGeneral.currentCalib=currentCalib;
      g_eeGeneral.vBatCalib=vBatCalib;
      g_eeGeneral.vBatWarn=vBatWarn;
      g_eeGeneral.PPM_Multiplier=PPM_Multiplier;
    } else {
      QMessageBox::critical(this, tr("Warning"), tr("Wrong data in profile, radio calibration was not retrieved"));
    }
    if ((DisplaySet.length()==6) && (BeeperSet.length()==4) && (HapticSet.length()==6) && (SpeakerSet.length()==6)) {
      g_eeGeneral.stickMode=GSStickMode;
      uint8_t byte8u;
      int8_t byte8;
      QString chars;
      bool ok;
      byte8=(int8_t)DisplaySet.mid(0,2).toInt(&ok,16);
      if (ok)
        g_eeGeneral.optrexDisplay=(byte8==1 ? true : false);
      byte8u=(uint8_t)DisplaySet.mid(2,2).toUInt(&ok,16);
      if (ok)
        g_eeGeneral.contrast=byte8u;
      byte8u=(uint8_t)DisplaySet.mid(4,2).toUInt(&ok,16);
      if (ok)
        g_eeGeneral.backlightBright=byte8u;
      byte8=(int8_t)BeeperSet.mid(0,2).toUInt(&ok,16);
      if (ok)
        g_eeGeneral.beeperMode=(BeeperMode)byte8;
      byte8=(int8_t)BeeperSet.mid(2,2).toInt(&ok,16);
      if (ok)
        g_eeGeneral.beeperLength=byte8;
      byte8=(int8_t)HapticSet.mid(0,2).toUInt(&ok,16);
      if (ok)
        g_eeGeneral.hapticMode=(BeeperMode)byte8;
      byte8u=(uint8_t)HapticSet.mid(2,2).toUInt(&ok,16);
      if (ok)
        g_eeGeneral.hapticStrength=byte8u;
      byte8=(int8_t)HapticSet.mid(4,2).toInt(&ok,16);
      if (ok)
        g_eeGeneral.hapticLength=byte8;
      byte8u=(uint8_t)SpeakerSet.mid(0,2).toUInt(&ok,16);
      if (ok)
        g_eeGeneral.speakerMode=byte8u;
      byte8u=(uint8_t)SpeakerSet.mid(2,2).toUInt(&ok,16);
      if (ok)
        g_eeGeneral.speakerPitch=byte8u;
      byte8u=(uint8_t)SpeakerSet.mid(4,2).toUInt(&ok,16);
      if (ok)
        g_eeGeneral.speakerVolume=byte8u;
      if (CountrySet.length()==6) {
        byte8u=(uint8_t)CountrySet.mid(0,2).toUInt(&ok,16);
        if (ok)
          g_eeGeneral.countryCode=byte8u;
        byte8u=(uint8_t)CountrySet.mid(2,2).toUInt(&ok,16);
        if (ok)
          g_eeGeneral.imperial=byte8u;
        chars=CountrySet.mid(4,2);
        g_eeGeneral.ttsLanguage[0]=chars[0].toAscii();
        g_eeGeneral.ttsLanguage[1]=chars[1].toAscii();
      }
    } else {
      QMessageBox::critical(this, tr("Warning"), tr("Wrong data in profile, hw related parameters were not retrieved"));
    }
  }
  setValues();
  updateSettings();
}

void GeneralEdit::on_calstore_PB_clicked()
{
  int profile_id=ui->profile_CB->itemData(ui->profile_CB->currentIndex()).toInt();

  QString name=g.profile[profile_id].name();
  int potsnum=GetCurrentFirmware()->getCapability(Pots);
  if (name.isEmpty()) {
    ui->calstore_PB->setDisabled(true);
    return;
  } else {
    QString calib=g.profile[profile_id].stickPotCalib();
    if (!(calib.isEmpty())) {
      int ret = QMessageBox::question(this, "Companion", 
                      tr("Do you want to store calibration in %1 profile<br>overwriting existing calibration?").arg(name) ,
                      QMessageBox::Yes | QMessageBox::No);
      if (ret == QMessageBox::No) {
        return;
      }
    }
    calib.clear();
    for (int i=0; i< (NUM_STICKS+potsnum); i++) {
      calib.append(QString("%1").arg((uint16_t)g_eeGeneral.calibMid[i], 4, 16, QChar('0')));
      calib.append(QString("%1").arg((uint16_t)g_eeGeneral.calibSpanNeg[i], 4, 16, QChar('0')));
      calib.append(QString("%1").arg((uint16_t)g_eeGeneral.calibSpanPos[i], 4, 16, QChar('0')));
    }
    g.profile[profile_id].stickPotCalib( calib );
    calib.clear();
    for (int i=0; i< 4; i++) {
      calib.append(QString("%1").arg((uint16_t)g_eeGeneral.trainer.calib[i], 4, 16, QChar('0')));
    }
    g.profile[profile_id].trainerCalib( calib );
    g.profile[profile_id].vBatCalib( g_eeGeneral.vBatCalib );
    g.profile[profile_id].currentCalib( g_eeGeneral.currentCalib );
    g.profile[profile_id].vBatWarn( g_eeGeneral.vBatWarn );
    g.profile[profile_id].ppmMultiplier( g_eeGeneral.PPM_Multiplier );
    g.profile[profile_id].gsStickMode( g_eeGeneral.stickMode );
    g.profile[profile_id].display( QString("%1%2%3").arg((g_eeGeneral.optrexDisplay ? 1:0), 2, 16, QChar('0')).arg((uint8_t)g_eeGeneral.contrast, 2, 16, QChar('0')).arg((uint8_t)g_eeGeneral.backlightBright, 2, 16, QChar('0')) );
    g.profile[profile_id].beeper( QString("%1%2").arg(((uint8_t)g_eeGeneral.beeperMode), 2, 16, QChar('0')).arg((uint8_t)g_eeGeneral.beeperLength, 2, 16, QChar('0')));
    g.profile[profile_id].haptic( QString("%1%2%3").arg(((uint8_t)g_eeGeneral.hapticMode), 2, 16, QChar('0')).arg((uint8_t)g_eeGeneral.hapticStrength, 2, 16, QChar('0')).arg((uint8_t)g_eeGeneral.hapticLength, 2, 16, QChar('0')));
    g.profile[profile_id].speaker( QString("%1%2%3").arg((uint8_t)g_eeGeneral.speakerMode, 2, 16, QChar('0')).arg((uint8_t)g_eeGeneral.speakerPitch, 2, 16, QChar('0')).arg((uint8_t)g_eeGeneral.speakerVolume, 2, 16, QChar('0')));
    g.profile[profile_id].countryCode( QString("%1%2%3").arg((uint8_t)g_eeGeneral.countryCode, 2, 16, QChar('0')).arg((uint8_t)g_eeGeneral.imperial, 2, 16, QChar('0')).arg(g_eeGeneral.ttsLanguage));

    QDateTime dateTime = QDateTime::currentDateTime();
    g.profile[profile_id].timeStamp(dateTime.toString("yyyy-MM-dd hh:mm"));
    QMessageBox::information(this, "Companion", tr("Calibration and HW parameters saved."));
  }
}

void GeneralEdit::shrink() {
    resize(100,100);
    resize(0,0);
}
