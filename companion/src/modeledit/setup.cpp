#include "setup.h"
#include "ui_setup.h"
#include "helpers.h"

Setup::Setup(QWidget *parent, ModelData & model):
  ModelPanel(parent, model),
  ui(new Ui::Setup)
{
  ui->setupUi(this);
  tabModelEditSetup();
}

Setup::~Setup()
{
  delete ui;
}

void Setup::tabModelEditSetup()
{
    lock = true;
    //name
    QLabel * pmsl[] = {ui->swwarn_label, ui->swwarn0_label, ui->swwarn4_label, NULL};
    QCheckBox * pmchkb[] = {ui->swwarn1_ChkB,ui->swwarn2_ChkB,ui->swwarn3_ChkB,ui->swwarn5_ChkB,ui->swwarn6_ChkB, NULL};
    QSlider * tpmsld[] = {ui->chkSA, ui->chkSB, ui->chkSC, ui->chkSD, ui->chkSE, ui->chkSF, ui->chkSG, NULL};
    QSlider * fssld1[] = { ui->fsm1SL_1, ui->fsm1SL_2,ui->fsm1SL_3,ui->fsm1SL_4,ui->fsm1SL_5,ui->fsm1SL_6,ui->fsm1SL_7,ui->fsm1SL_8,
                                 ui->fsm1SL_9, ui->fsm1SL_10,ui->fsm1SL_11,ui->fsm1SL_12,ui->fsm1SL_13,ui->fsm1SL_14,ui->fsm1SL_15,ui->fsm1SL_16, NULL };
    QSlider * fssld2[] = { ui->fsm2SL_1, ui->fsm2SL_2,ui->fsm2SL_3,ui->fsm2SL_4,ui->fsm2SL_5,ui->fsm2SL_6,ui->fsm2SL_7,ui->fsm2SL_8,
                                 ui->fsm2SL_9, ui->fsm2SL_10,ui->fsm2SL_11,ui->fsm2SL_12,ui->fsm2SL_13,ui->fsm2SL_14,ui->fsm2SL_15,ui->fsm2SL_16, NULL };
    QSpinBox * fssb1[] = { ui->fsm1SB_1, ui->fsm1SB_2,ui->fsm1SB_3,ui->fsm1SB_4,ui->fsm1SB_5,ui->fsm1SB_6,ui->fsm1SB_7,ui->fsm1SB_8,
                                 ui->fsm1SB_9, ui->fsm1SB_10,ui->fsm1SB_11,ui->fsm1SB_12,ui->fsm1SB_13,ui->fsm1SB_14,ui->fsm1SB_15,ui->fsm1SB_16, NULL };
    QSpinBox * fssb2[] = { ui->fsm2SB_1, ui->fsm2SB_2,ui->fsm2SB_3,ui->fsm2SB_4,ui->fsm2SB_5,ui->fsm2SB_6,ui->fsm2SB_7,ui->fsm2SB_8,
                                 ui->fsm2SB_9, ui->fsm2SB_10,ui->fsm2SB_11,ui->fsm2SB_12,ui->fsm2SB_13,ui->fsm2SB_14,ui->fsm2SB_15,ui->fsm2SB_16, NULL };
    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      ui->modelNameLE->setMaxLength(12);
    } else {
      ui->modelNameLE->setMaxLength(10);
    }
    ui->modelNameLE->setText(model.name);

    if (GetEepromInterface()->getCapability(NumModules)<2) {
      ui->rf2_GB->hide();
    }

    if (!GetEepromInterface()->getCapability(HasFailsafe)) {
      ui->FSGB_1->hide();
      ui->FSGB_2->hide();
      ui->ModelSetupTab->setTabEnabled(1,0);
    } else {
      if (GetEepromInterface()->getCapability(HasFailsafe)<32) {
        ui->FSGB_2->hide();
      }

      for (int i=0; fssld1[i]; i++) {
        fssld1[i]->setValue(model.moduleData[0].failsafeChannels[i]);
        fssld2[i]->setValue(model.moduleData[1].failsafeChannels[i]);
        fssb1[i]->setValue(model.moduleData[0].failsafeChannels[i]);
        fssb2[i]->setValue(model.moduleData[1].failsafeChannels[i]);
        connect(fssld1[i],SIGNAL(valueChanged(int)),this,SLOT(fssldValueChanged()));
        connect(fssld2[i],SIGNAL(valueChanged(int)),this,SLOT(fssldValueChanged()));
        connect(fssb1[i],SIGNAL(valueChanged(int)),this,SLOT(fssbValueChanged()));
        connect(fssb2[i],SIGNAL(valueChanged(int)),this,SLOT(fssbValueChanged()));
        connect(fssld1[i],SIGNAL(sliderReleased()),this,SLOT(fssldEdited()));
        connect(fssld2[i],SIGNAL(sliderReleased()),this,SLOT(fssldEdited()));
        connect(fssb1[i],SIGNAL(editingFinished()),this,SLOT(fssbEdited()));
        connect(fssb2[i],SIGNAL(editingFinished()),this,SLOT(fssbEdited()));
      }
    }

    //timer1 mode direction value
    populateTimerSwitchCB(ui->timer1ModeCB,model.timers[0].mode,GetEepromInterface()->getCapability(TimerTriggerB));
    int min = model.timers[0].val/60;
    int sec = model.timers[0].val%60;
    ui->timer1ValTE->setTime(QTime(0,min,sec));
    ui->timer1DirCB->setCurrentIndex(model.timers[0].dir);
    if (!GetEepromInterface()->getCapability(ModelVoice)) {
      ui->modelVoice_SB->hide();
      ui->modelVoice_label->hide();
    } else {
      ui->modelVoice_SB->setValue(model.modelVoice+260);
    }
    if (!GetEepromInterface()->getCapability(PerModelThrottleInvert)) {
      ui->label_thrrev->hide();
      ui->thrrevChkB->hide();
    }
    else {
      ui->thrrevChkB->setChecked(model.throttleReversed);
    }
    if (!GetEepromInterface()->getCapability(ModelImage)) {
      ui->modelImage_CB->hide();
      ui->modelImage_label->hide();
      ui->modelImage_image->hide();
    } else {

      QStringList items;
      items.append("");
      QSettings settings("companion9x", "companion9x");
      QString path=settings.value("sdPath", ".").toString();
      path.append("/BMP/");
      QDir qd(path);
      int vml= GetEepromInterface()->getCapability(VoicesMaxLength)+4;
      if (qd.exists()) {
        QStringList filters;
        filters << "*.bmp" << "*.bmp";
        foreach ( QString file, qd.entryList(filters, QDir::Files) ) {
          QFileInfo fi(file);
          QString temp=fi.completeBaseName();
          if (!items.contains(temp) && temp.length()<=vml) {
            items.append(temp);
          }
        }
      }
      if (!items.contains(model.bitmap)) {
        items.append(model.bitmap);
      }
      items.sort();
      ui->modelImage_CB->clear();
      foreach ( QString file, items ) {
        ui->modelImage_CB->addItem(file);
        if (file==model.bitmap) {
          ui->modelImage_CB->setCurrentIndex(ui->modelImage_CB->count()-1);
          QString fileName=path;
          fileName.append(model.bitmap);
          fileName.append(".bmp");
          QImage image(fileName);
          if (image.isNull()) {
            fileName=path;
            fileName.append(model.bitmap);
            fileName.append(".BMP");
            image.load(fileName);
          }
          if (!image.isNull()) {
            ui->modelImage_image->setPixmap(QPixmap::fromImage(image.scaled( 64,32)));;
          }
        }
      }

    }

    if (!GetEepromInterface()->getCapability(pmSwitchMask)) {
      for (int i=0; pmsl[i]; i++) {
        pmsl[i]->hide();
      }
      for (int i=0; pmchkb[i]; i++) {
        pmchkb[i]->hide();
      }
      ui->tswwarn0_CB->hide();
      for (int i=0; tpmsld[i]; i++) {
        tpmsld[i]->hide();
      }
      ui->swwarn0_line->hide();
      ui->swwarn0_line->hide();
      ui->swwarn0_CB->hide();
      ui->swwarn4_CB->hide();
      ui->swwarn_line0->hide();
      ui->swwarn_line1->hide();
      ui->swwarn_line2->hide();
      ui->swwarn_line3->hide();
      ui->swwarn_line4->hide();
      ui->swwarn_line5->hide();
      ui->tswwarn0_label->hide();
      ui->tswwarn1_label->hide();
      ui->tswwarn2_label->hide();
      ui->tswwarn3_label->hide();
      ui->tswwarn4_label->hide();
      ui->tswwarn5_label->hide();
      ui->tswwarn6_label->hide();
      ui->tswwarn7_label->hide();
    } else {
      if (GetEepromInterface()->getCapability(Pots)==3) {
        ui->swwarn0_CB->setCurrentIndex(model.switchWarningStates & 0x01);
        ui->swwarn1_ChkB->setChecked(checkbit(model.switchWarningStates, 1));
        ui->swwarn2_ChkB->setChecked(checkbit(model.switchWarningStates, 2));
        ui->swwarn3_ChkB->setChecked(checkbit(model.switchWarningStates, 3));
        ui->swwarn4_CB->setCurrentIndex((model.switchWarningStates & 0x30)>>4);
        ui->swwarn5_ChkB->setChecked(checkbit(model.switchWarningStates, 6));
        ui->swwarn6_ChkB->setChecked(checkbit(model.switchWarningStates, 7));
        for (int i=0; pmchkb[i]; i++) {
          connect(pmchkb[i], SIGNAL(stateChanged(int)),this,SLOT(startupSwitchEdited()));
        }
        connect(ui->swwarn0_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(startupSwitchEdited()));
        connect(ui->swwarn4_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(startupSwitchEdited()));
      } else {
        ui->tswwarn0_CB->setCurrentIndex(model.switchWarningStates & 0x01);
        uint16_t switchstate=(model.switchWarningStates>>1);
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
        connect(ui->tswwarn0_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(startupSwitchEdited()));
        for (int i=0; tpmsld[i]; i++) {
          connect(tpmsld[i], SIGNAL(valueChanged(int)),this,SLOT(startupSwitchEdited()));
        }
      }
    }
    int ppmmax=GetEepromInterface()->getCapability(PPMFrameLength);
    if (ppmmax>0) {
      ui->ppmFrameLengthDSB->setMaximum(ppmmax);
    }
    if (!GetEepromInterface()->getCapability(InstantTrimSW)) {
      ui->instantTrim_label->hide();
      ui->instantTrim_CB->setDisabled(true);
      ui->instantTrim_CB->hide();
    }
    else {
      int found=false;
      for (int i=0; i< C9X_MAX_CUSTOM_FUNCTIONS; i++) {
        if (model.funcSw[i].func==FuncInstantTrim) {
          populateSwitchCB(ui->instantTrim_CB,model.funcSw[i].swtch,POPULATE_MSWITCHES & POPULATE_ONOFF);
          found=true;
          break;
        }
      }
      if (found==false) {
        populateSwitchCB(ui->instantTrim_CB,RawSwitch(),POPULATE_MSWITCHES & POPULATE_ONOFF);
      }
    }
    if (GetEepromInterface()->getCapability(NoTimerDirs)) {
      ui->timer1DirCB->hide();
      ui->timer2DirCB->hide();
    }
    if (GetEepromInterface()->getCapability(NoThrExpo)) {
      ui->label_thrExpo->hide();
      ui->thrExpoChkB->hide();
    }
    if (!(GetEepromInterface()->getCapability(ExtendedTrims)>0)) {
      ui->extendedTrimsChkB->hide();
      ui->extendedTrims_label->hide();
    } else {
      ui->extendedTrimsChkB->setChecked(model.extendedTrims);
    }
    if (!GetEepromInterface()->getCapability(HasTTrace)) {
      ui->label_ttrace->hide();
      ui->ttraceCB->hide();
    } else {
      populateTTraceCB(ui->ttraceCB,model.thrTraceSrc);
    }
    if (GetEepromInterface()->getCapability(RotaryEncoders)==0) {
      ui->bcREaChkB->hide();
      ui->bcREbChkB->hide();
    }
    if (GetEepromInterface()->getCapability(Pots)==3) {
      ui->tswwarn0_CB->hide();
      ui->tswwarn0_label->hide();
      ui->tswwarn1_label->hide();
      ui->tswwarn2_label->hide();
      ui->tswwarn3_label->hide();
      ui->tswwarn4_label->hide();
      ui->tswwarn5_label->hide();
      ui->tswwarn6_label->hide();
      ui->tswwarn7_label->hide();
      ui->chkSA->hide();
      ui->chkSB->hide();
      ui->chkSC->hide();
      ui->chkSD->hide();
      ui->chkSE->hide();
      ui->chkSF->hide();
      ui->chkSG->hide();
      this->layout()->removeItem(ui->TaranisSwitchStartup);
      ui->bcP4ChkB->hide();
    } else {
      ui->swwarn0_CB->hide();
      ui->swwarn0_label->hide();
      ui->swwarn0_line->hide();
      ui->swwarn1_ChkB->hide();
      ui->swwarn1_line->hide();
      ui->swwarn2_ChkB->hide();
      ui->swwarn3_ChkB->hide();
      ui->swwarn4_CB->hide();
      ui->swwarn4_label->hide();
      ui->swwarn5_ChkB->hide();
      ui->swwarn6_ChkB->hide();
      ui->swwarn_line0->hide();
      ui->swwarn_line1->hide();
      ui->swwarn_line2->hide();
      ui->swwarn_line3->hide();
      ui->swwarn_line4->hide();
      ui->swwarn_line5->hide();
      this->layout()->removeItem(ui->StockSwitchStartup);
      ui->bcP1ChkB->setText(tr("S1"));
      ui->bcP2ChkB->setText(tr("S2"));
      ui->bcP3ChkB->setText(tr("LS"));
    }

    if (!GetEepromInterface()->getCapability(PerModelThrottleWarning)) {
      ui->thrwarnChkB->setDisabled(true);
      ui->thrwarnChkB->hide();
      ui->thrwarnLabel->hide();
    }
    else {
      ui->thrwarnChkB->setChecked(model.disableThrottleWarning);
    }
    if (!GetEepromInterface()->getCapability(TimerTriggerB)) {
      ui->timer1ModeBCB->hide();
      ui->timer1ModeB_label->hide();
      ui->timer2ModeBCB->hide();
      ui->timer2ModeB_label->hide();
    } else {
      populateTimerSwitchBCB(ui->timer1ModeBCB,model.timers[0].modeB,GetEepromInterface()->getCapability(TimerTriggerB));
      populateTimerSwitchBCB(ui->timer2ModeBCB,model.timers[1].modeB,GetEepromInterface()->getCapability(TimerTriggerB));
    }

    int index=0;
    int selindex;
    int selindex2;
    ui->protocolCB->clear();
    for (int i=0; i<PROTO_LAST; i++) {
      if (GetEepromInterface()->isAvailable((Protocol)i)) {
        ui->protocolCB->addItem(getProtocolStr(i), (QVariant)i);
        if (model.moduleData[0].protocol == i) {
          selindex = index;
        }
        index++;
      }
    }
    if (GetEepromInterface()->getCapability(NumModules)>1) {
      index=0;
      ui->protocolCB_2->clear();
      for (int i=0; i<PROTO_LAST; ++i) {
        if (GetEepromInterface()->isAvailable((Protocol)i, 1)) {
          ui->protocolCB_2->addItem(getProtocolStr(i), (QVariant)i);
          if (model.moduleData[1].protocol == i) {
            selindex2 = index;
          }
          index++;
        }
      }
    }
    if (GetEepromInterface()->getCapability(ModelTrainerEnable)) {
      if (!(model.trainerMode||model.traineron)) {
        ui->protocolCB_3->setCurrentIndex(0);
        ui->label_PPM_3->hide();
        ui->ppmDelaySB_3->hide();
        ui->label_PPMCH_3->hide();
        ui->label_pulsePol_3->hide();
        ui->pulsePolCB_3->hide();
        ui->numChannelsSB_3->hide();
        ui->label_ppmFrameLength_3->hide();
        ui->ppmFrameLengthDSB_3->hide();
        ui->label_numChannelsStart_3->hide();
        ui->numChannelsStart_3->hide();
      } else {
        ui->protocolCB_3->setCurrentIndex(1);
        ui->label_PPM_3->show();
        ui->ppmDelaySB_3->show();
        ui->label_PPMCH_3->show();
        ui->label_pulsePol_3->show();
        ui->pulsePolCB_3->show();
        ui->numChannelsSB_3->show();
        ui->label_ppmFrameLength_3->show();
        ui->ppmFrameLengthDSB_3->show();
        ui->label_numChannelsStart_3->show();
        ui->numChannelsStart_3->show();
      }
      on_protocolCB_3_currentIndexChanged(model.traineron||model.trainerMode);
    } else {
      ui->rf3_GB->hide();
    }

    ui->label_PPM->hide();
    ui->ppmDelaySB->hide();
    ui->label_PPMCH->hide();
    ui->label_pulsePol->hide();
    ui->pulsePolCB->hide();
    ui->numChannelsSB->hide();
    ui->label_ppmFrameLength->hide();
    ui->ppmFrameLengthDSB->hide();
    ui->label_DSM->hide();
    ui->DSM_Type->hide();
    ui->label_PXX->hide();
    ui->pxxRxNum->hide();
    ui->label_numChannelsStart->hide();
    ui->numChannelsStart->hide();
    ui->pxxRxNum->setEnabled(false);
    ui->protocolCB->setCurrentIndex(selindex);
    on_protocolCB_currentIndexChanged(selindex);
    if (GetEepromInterface()->getCapability(NumModules)>1) {
      ui->label_PPM_2->hide();
      ui->ppmDelaySB_2->hide();
      ui->label_PPMCH_2->hide();
      ui->label_pulsePol_2->hide();
      ui->pulsePolCB_2->hide();
      ui->numChannelsSB_2->hide();
      ui->label_ppmFrameLength_2->hide();
      ui->ppmFrameLengthDSB_2->hide();
      ui->label_DSM_2->hide();
      ui->DSM_Type_2->hide();
      ui->label_PXX_2->hide();
      ui->pxxRxNum_2->hide();
      ui->label_numChannelsStart_2->hide();
      ui->numChannelsStart_2->hide();
      ui->pxxRxNum_2->setEnabled(false);
      ui->protocolCB_2->setCurrentIndex(selindex2);
      on_protocolCB_2_currentIndexChanged(selindex2);
    }

    //timer2 mode direction value
    if (GetEepromInterface()->getCapability(Timers)<2) {
      ui->timer2DirCB->hide();
      ui->timer2ValTE->hide();
      ui->timer2DirCB->hide();
      ui->timer2ModeCB->hide();
      ui->timer2ModeBCB->hide();
      ui->timer2ModeB_label->hide();
      ui->label_timer2->hide();
    } else {
      populateTimerSwitchCB(ui->timer2ModeCB,model.timers[1].mode,GetEepromInterface()->getCapability(TimerTriggerB));
      min = model.timers[1].val/60;
      sec = model.timers[1].val%60;
      ui->timer2ValTE->setTime(QTime(0,min,sec));
      ui->timer2DirCB->setCurrentIndex(model.timers[1].dir);
    }
    if (!GetEepromInterface()->getCapability(PermTimers)) {
      ui->timer1Perm->hide();
      ui->timer2Perm->hide();
      ui->timer1PermValue->hide();
      ui->timer2PermValue->hide();
    } else {
      int sign=1;
      int pvalue=model.timers[0].pvalue;
      if (pvalue<0) {
        pvalue=-pvalue;
        sign=-1;
      }
      int hours=pvalue/3600;
      pvalue-=hours*3600;
      int minutes = pvalue/60;
      int seconds = pvalue%60;
      ui->timer1PermValue->setText(QString(" %1(%2:%3:%4)").arg(sign<0 ? "-" :" ").arg(hours,2,10,QLatin1Char('0')).arg(minutes,2,10,QLatin1Char('0')).arg(seconds,2,10,QLatin1Char('0')));
      // QString QString::arg ( int a, int fieldWidth = 0, int base = 10, const QChar & fillChar = QLatin1Char( ' ' ) ) const
      sign=1;
      pvalue=model.timers[1].pvalue;
      if (pvalue<0) {
        pvalue=-pvalue;
        sign=-1;
      }
      hours=pvalue/3600;
      pvalue-=hours*3600;
      minutes = pvalue/60;
      seconds = pvalue%60;
      ui->timer2PermValue->setText(QString(" %1(%2:%3:%4)").arg(sign<0 ? "-" :" ").arg(hours,2,10,QLatin1Char('0')).arg(minutes,2,10,QLatin1Char('0')).arg(seconds,2,10,QLatin1Char('0')));
      ui->timer1Perm->setChecked(model.timers[0].persistent);
      ui->timer2Perm->setChecked(model.timers[1].persistent);
    }
    if (!GetEepromInterface()->getCapability(minuteBeep)) {
      ui->timer1Minute->hide();
      ui->timer2Minute->hide();
    } else {
      ui->timer1Minute->setChecked(model.timers[0].minuteBeep);
      ui->timer2Minute->setChecked(model.timers[1].minuteBeep);
    }

    if (!GetEepromInterface()->getCapability(countdownBeep)) {
      ui->timer1CountDownBeep->hide();
      ui->timer2CountDownBeep->hide();
    } else {
      ui->timer1CountDownBeep->setChecked(model.timers[0].countdownBeep);
      ui->timer2CountDownBeep->setChecked(model.timers[1].countdownBeep);
    }

    //trim inc, thro trim, thro expo, instatrim
    ui->trimIncCB->setCurrentIndex(model.trimInc);
    ui->thrExpoChkB->setChecked(model.thrExpo);
    ui->thrTrimChkB->setChecked(model.thrTrim);

    //center beep
    ui->bcRUDChkB->setChecked(model.beepANACenter & BC_BIT_RUD);
    ui->bcELEChkB->setChecked(model.beepANACenter & BC_BIT_ELE);
    ui->bcTHRChkB->setChecked(model.beepANACenter & BC_BIT_THR);
    ui->bcAILChkB->setChecked(model.beepANACenter & BC_BIT_AIL);
    ui->bcP1ChkB->setChecked(model.beepANACenter & BC_BIT_P1);
    ui->bcP2ChkB->setChecked(model.beepANACenter & BC_BIT_P2);
    ui->bcP3ChkB->setChecked(model.beepANACenter & BC_BIT_P3);
    ui->bcP4ChkB->setChecked(model.beepANACenter & BC_BIT_P4);
    ui->bcREaChkB->setChecked(model.beepANACenter & BC_BIT_REA);
    ui->bcREbChkB->setChecked(model.beepANACenter & BC_BIT_REB);

    // PPM settings fields
    ui->ppmDelaySB->setEnabled(model.moduleData[0].protocol == PPM);
    ui->pulsePolCB->setCurrentIndex(model.moduleData[0].ppmPulsePol);
    ui->ppmDelaySB->setEnabled(model.moduleData[0].protocol == PPM);
    ui->ppmDelaySB->setValue(model.moduleData[0].ppmDelay);
    // TODO? ui->numChannelsSB->setEnabled(model.moduleData[0].protocol == PPM);

    ui->extendedLimitsChkB->setChecked(model.extendedLimits);
    ui->T2ThrTrgChkB->setChecked(model.t2throttle);
    if (!GetEepromInterface()->getCapability(Timer2ThrTrig)) {
      ui->T2ThrTrg->hide();
      ui->T2ThrTrgChkB->hide();
    }
    ui->ppmFrameLengthDSB->setValue(22.5+((double)model.moduleData[0].ppmFrameLength)*0.5);
    if (!GetEepromInterface()->getCapability(PPMExtCtrl)) {
      ui->ppmFrameLengthDSB->hide();
      ui->label_ppmFrameLength->hide();
    }
    switch (model.moduleData[0].protocol) {
      case PXX_DJT:
      case PXX_XJT_X16:
      case PXX_XJT_D8:
      case PXX_XJT_LR12:
        ui->pxxRxNum->setMinimum(0);
        ui->pxxRxNum->setValue((model.modelId));
        break;
      case DSM2:
        if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
          ui->pxxRxNum->setValue(1);
        }
        else {
          ui->pxxRxNum->setMinimum(0);
          ui->pxxRxNum->setValue((model.modelId));
        }
        ui->numChannelsSB->setValue(8);
        break;
      default:
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        ui->DSM_Type->setEnabled(false);
        ui->label_PXX->hide();
        ui->pxxRxNum->hide();
        ui->pxxRxNum->setEnabled(false);
        ui->numChannelsSB->setValue(model.moduleData[0].channelsCount);
        break;
    }
    lock = false;
}

void Setup::on_protocolCB_currentIndexChanged(int index)
{
    Protocol protocol = (Protocol)ui->protocolCB->itemData(index).toInt();

    if (!lock) {
      model.moduleData[0].protocol = protocol;
      emit modified();
    }

    if (protocol==PXX_XJT_D8 || protocol==OFF) {
      ui->FSGB_1->hide();
    } else {
      ui->fsm1CB->setCurrentIndex(model.moduleData[0].failsafeMode);
      on_fsm1CB_currentIndexChanged(model.moduleData[0].failsafeMode);
      ui->FSGB_1->show();
    }

    ui->ppmDelaySB->setEnabled(!protocol);
    ui->numChannelsSB->setEnabled(!protocol);

    if (GetEepromInterface()->getCapability(HasPPMStart)) {
      ui->numChannelsSB->setSingleStep(1);
    } else {
      ui->numChannelsSB->setSingleStep(2);
    }

    switch (protocol) {
      case OFF:
        ui->label_PPM->hide();
        ui->ppmDelaySB->hide();
        ui->label_PPMCH->hide();
        ui->label_pulsePol->hide();
        ui->pulsePolCB->hide();
        ui->numChannelsSB->hide();
        ui->label_ppmFrameLength->hide();
        ui->ppmFrameLengthDSB->hide();
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        ui->label_PXX->hide();
        ui->pxxRxNum->hide();
        ui->label_numChannelsStart->hide();
        ui->numChannelsStart->hide();
        break;
      case PXX_XJT_X16:
      case PXX_XJT_D8:
      case PXX_XJT_LR12:
      case PXX_DJT:
        ui->label_PPM->hide();
        ui->ppmDelaySB->hide();
        ui->ppmDelaySB->setEnabled(false);
        ui->label_PPMCH->show();
        ui->label_pulsePol->hide();
        ui->pulsePolCB->hide();
        ui->numChannelsSB->show();
        ui->numChannelsSB->setEnabled(true);
        ui->label_ppmFrameLength->hide();
        ui->ppmFrameLengthDSB->hide();
        ui->ppmFrameLengthDSB->setEnabled(false);
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        ui->DSM_Type->setEnabled(false);
        ui->label_PXX->show();
        ui->pxxRxNum->setMinimum(0);
        ui->pxxRxNum->show();
        ui->pxxRxNum->setEnabled(true);
        ui->pxxRxNum->setValue((model.moduleData[0].channelsCount-8)/2+1);
        ui->label_numChannelsStart->show();
        ui->numChannelsStart->show();
        ui->numChannelsStart->setValue(model.moduleData[0].channelsStart+1);
        ui->numChannelsSB->setMinimum(model.moduleData[0].channelsStart+4);
        ui->numChannelsSB->setValue(model.moduleData[0].channelsStart+model.moduleData[0].channelsCount);
        break;
      case LP45:
      case DSM2:
      case DSMX:
        ui->label_pulsePol->hide();
        ui->pulsePolCB->hide();
        ui->label_PPM->hide();
        ui->ppmDelaySB->hide();
        ui->ppmDelaySB->setEnabled(false);
        ui->label_PPMCH->hide();
        ui->label_numChannelsStart->hide();
        ui->numChannelsStart->hide();
        ui->numChannelsSB->hide();
        ui->numChannelsSB->setEnabled(false);
        ui->label_ppmFrameLength->hide();
        ui->ppmFrameLengthDSB->hide();
        ui->ppmFrameLengthDSB->setEnabled(false);
        if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
          ui->label_PXX->hide();
          ui->pxxRxNum->hide();
          ui->pxxRxNum->setEnabled(false);
        }
        else {
          ui->pxxRxNum->setMinimum(0);
          ui->pxxRxNum->setValue(model.modelId);
          ui->label_PXX->show();
          ui->pxxRxNum->show();
          ui->pxxRxNum->setEnabled(true);
        }
        ui->DSM_Type->setEnabled(false);
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        break;
      default:
        if (GetEepromInterface()->getCapability(HasPPMStart)) {
          ui->label_numChannelsStart->show();
          ui->numChannelsStart->show();
        }
        else {
          ui->label_numChannelsStart->hide();
          ui->numChannelsStart->hide();
        }
        ui->numChannelsStart->setValue(model.moduleData[0].channelsStart+1);
        ui->numChannelsSB->setMinimum(model.moduleData[0].channelsStart+4);
        ui->numChannelsSB->setValue(model.moduleData[0].channelsStart + model.moduleData[0].channelsCount);
        ui->label_pulsePol->show();
        ui->pulsePolCB->show();
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        ui->DSM_Type->setEnabled(false);
        ui->label_PXX->hide();
        ui->pxxRxNum->hide();
        ui->pxxRxNum->setEnabled(false);
        ui->label_PPM->show();
        ui->ppmDelaySB->show();
        ui->ppmDelaySB->setEnabled(true);
        ui->label_PPMCH->show();
        ui->numChannelsSB->show();
        ui->numChannelsSB->setEnabled(true);
        ui->ppmFrameLengthDSB->setEnabled(true);
        ui->ppmFrameLengthDSB->setMinimum(model.moduleData[0].channelsCount*(model.extendedLimits ? 2.250 :2)+3.5);
        if (GetEepromInterface()->getCapability(PPMExtCtrl)) {
          ui->ppmFrameLengthDSB->show();
          ui->label_ppmFrameLength->show();
        }

        break;
    }
}

void Setup::on_protocolCB_2_currentIndexChanged(int index)
{
    Protocol protocol = (Protocol)ui->protocolCB_2->itemData(index).toInt();

    if (!lock) {
      model.moduleData[1].protocol = protocol;
      emit modified();
    }

    if (protocol==PXX_XJT_X16 || protocol==PXX_XJT_LR12) {
      ui->FSGB_2->show();
      ui->fsm2CB->setCurrentIndex(model.moduleData[1].failsafeMode);
      on_fsm2CB_currentIndexChanged(model.moduleData[1].failsafeMode);
    }
    else {
      ui->FSGB_2->hide();
    }

    ui->ppmDelaySB_2->setEnabled(!protocol);
    ui->numChannelsSB_2->setEnabled(!protocol);

    if (GetEepromInterface()->getCapability(HasPPMStart)) {
      ui->numChannelsSB_2->setSingleStep(1);
    } else {
      ui->numChannelsSB_2->setSingleStep(2);
    }

    switch (protocol) {
      case OFF:
        ui->label_PPM_2->hide();
        ui->ppmDelaySB_2->hide();
        ui->label_PPMCH_2->hide();
        ui->label_pulsePol_2->hide();
        ui->pulsePolCB_2->hide();
        ui->numChannelsSB_2->hide();
        ui->label_ppmFrameLength_2->hide();
        ui->ppmFrameLengthDSB_2->hide();
        ui->label_DSM_2->hide();
        ui->DSM_Type_2->hide();
        ui->label_PXX_2->hide();
        ui->pxxRxNum_2->hide();
        ui->label_numChannelsStart_2->hide();
        ui->numChannelsStart_2->hide();
        break;
      case PXX_XJT_X16:
      case PXX_XJT_D8:
      case PXX_XJT_LR12:
      case PXX_DJT:
        ui->label_PPM_2->hide();
        ui->ppmDelaySB_2->hide();
        ui->ppmDelaySB_2->setEnabled(false);
        ui->label_PPMCH_2->show();
        ui->label_pulsePol_2->hide();
        ui->pulsePolCB_2->hide();
        ui->numChannelsSB_2->show();
        ui->numChannelsSB_2->setEnabled(true);
        ui->label_ppmFrameLength_2->hide();
        ui->ppmFrameLengthDSB_2->hide();
        ui->ppmFrameLengthDSB_2->setEnabled(false);
        ui->label_DSM_2->hide();
        ui->DSM_Type_2->hide();
        ui->DSM_Type_2->setEnabled(false);
        ui->label_PXX_2->show();
        ui->pxxRxNum_2->setMinimum(0);
        ui->pxxRxNum_2->show();
        ui->pxxRxNum_2->setEnabled(true);
        ui->pxxRxNum_2->setValue(model.modelId);
        ui->label_numChannelsStart_2->show();
        ui->numChannelsStart_2->show();
        ui->numChannelsStart_2->setValue(model.moduleData[1].channelsStart+1);
        ui->numChannelsSB_2->setMinimum(model.moduleData[1].channelsStart+4);
        ui->numChannelsSB_2->setValue(model.moduleData[1].channelsStart+model.moduleData[1].channelsCount);
        break;
      case LP45:
      case DSM2:
      case DSMX:
        ui->label_pulsePol_2->hide();
        ui->pulsePolCB_2->hide();
        ui->label_PPM_2->hide();
        ui->ppmDelaySB_2->hide();
        ui->ppmDelaySB_2->setEnabled(false);
        ui->label_PPMCH_2->hide();
        ui->numChannelsSB_2->hide();
        ui->numChannelsSB_2->setEnabled(false);
        ui->label_ppmFrameLength_2->hide();
        ui->ppmFrameLengthDSB_2->hide();
        ui->ppmFrameLengthDSB_2->setEnabled(false);
        if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
          ui->label_PXX_2->hide();
          ui->pxxRxNum_2->hide();
          ui->pxxRxNum_2->setEnabled(false);
        } else {
          ui->pxxRxNum_2->setMinimum(0);
          ui->pxxRxNum_2->setValue(model.modelId);
          ui->label_PXX_2->show();
          ui->pxxRxNum_2->show();
          ui->pxxRxNum_2->setEnabled(true);
        }
        ui->DSM_Type_2->setEnabled(false);
        ui->label_DSM_2->hide();
        ui->DSM_Type_2->hide();
        break;
      default:
        if (GetEepromInterface()->getCapability(HasPPMStart)) {
          ui->label_numChannelsStart_2->show();
          ui->numChannelsStart_2->show();
        } else {
          ui->label_numChannelsStart_2->hide();
          ui->numChannelsStart_2->hide();
        }
        ui->numChannelsStart_2->setValue(model.moduleData[1].channelsStart+1);
        ui->numChannelsSB_2->setMinimum(model.moduleData[1].channelsStart+4);
        ui->numChannelsSB_2->setValue(model.moduleData[1].channelsStart+model.moduleData[1].channelsCount);
        ui->label_pulsePol_2->show();
        ui->pulsePolCB_2->show();
        ui->pulsePolCB_2->setCurrentIndex(model.moduleData[1].ppmPulsePol);
        ui->label_DSM_2->hide();
        ui->DSM_Type_2->hide();
        ui->DSM_Type_2->setEnabled(false);
        ui->label_PXX_2->hide();
        ui->pxxRxNum_2->hide();
        ui->pxxRxNum_2->setEnabled(false);
        ui->label_PPM_2->show();
        ui->ppmDelaySB_2->show();
        ui->ppmDelaySB_2->setEnabled(true);
        ui->ppmDelaySB_2->setValue(model.moduleData[1].ppmDelay);
        ui->label_PPMCH_2->show();
        ui->numChannelsSB_2->show();
        ui->numChannelsSB_2->setEnabled(true);
        ui->ppmFrameLengthDSB_2->setEnabled(true);
        ui->ppmFrameLengthDSB_2->setMinimum(model.moduleData[1].channelsCount*(model.extendedLimits ? 2.250 :2)+3.5);
        if (GetEepromInterface()->getCapability(PPMExtCtrl)) {
          ui->ppmFrameLengthDSB_2->show();
          ui->label_ppmFrameLength_2->show();
          ui->ppmFrameLengthDSB_2->setValue(model.moduleData[1].ppmFrameLength/2.0+22.5);
        }
        break;
    }
}

void Setup::on_protocolCB_3_currentIndexChanged(int index)
{
    Protocol protocol = (Protocol)ui->protocolCB_3->currentIndex();

    if (!lock) {
      model.moduleData[2].protocol = protocol;
      model.trainerMode = ui->protocolCB_3->currentIndex();
      emit modified();
    }
    
    switch (index) {
      case 0:
        ui->label_PPM_3->hide();
        ui->ppmDelaySB_3->hide();
        ui->label_PPMCH_3->hide();
        ui->label_pulsePol_3->hide();
        ui->pulsePolCB_3->hide();
        ui->numChannelsSB_3->hide();
        ui->label_ppmFrameLength_3->hide();
        ui->ppmFrameLengthDSB_3->hide();
        ui->label_numChannelsStart_3->hide();
        ui->numChannelsStart_3->hide();
        break;
      default:
        ui->label_PPM_3->show();
        ui->ppmDelaySB_3->show();
        ui->label_PPMCH_3->show();
        ui->label_pulsePol_3->show();
        ui->pulsePolCB_3->show();
        ui->numChannelsSB_3->show();
        ui->label_ppmFrameLength_3->show();
        ui->ppmFrameLengthDSB_3->show();
        ui->label_numChannelsStart_3->show();
        ui->numChannelsStart_3->show();
        ui->numChannelsStart_3->setValue(model.moduleData[2].channelsStart+1);
        ui->numChannelsSB_3->setMinimum(model.moduleData[2].channelsStart+4);
        ui->numChannelsSB_3->setValue(model.moduleData[2].channelsStart+model.moduleData[2].channelsCount);
        ui->pulsePolCB_3->setCurrentIndex(model.moduleData[2].ppmPulsePol);
        ui->ppmDelaySB_3->setValue(model.moduleData[2].ppmDelay);
        ui->ppmFrameLengthDSB_3->setMinimum(model.moduleData[2].channelsCount*(model.extendedLimits ? 2.250 :2)+3.5);
        if (GetEepromInterface()->getCapability(PPMExtCtrl)) {
          ui->ppmFrameLengthDSB_3->show();
          ui->label_ppmFrameLength_3->show();
          ui->ppmFrameLengthDSB_3->setValue(model.moduleData[2].ppmFrameLength/2.0+22.5);
        }
        break;
    }
}

void Setup::on_T2ThrTrgChkB_toggled(bool checked)
{
  model.t2throttle = checked;
  emit modified();
}

void Setup::on_extendedLimitsChkB_toggled(bool checked)
{
  model.extendedLimits = checked;
  emit modified();
}

void Setup::on_thrwarnChkB_toggled(bool checked)
{
  model.disableThrottleWarning = checked;
  emit modified();
}

void Setup::on_thrrevChkB_toggled(bool checked)
{
  model.throttleReversed = checked;
  emit modified();
}

void Setup::on_extendedTrimsChkB_toggled(bool checked)
{
  model.extendedTrims = checked;
  emit modified();
}

void Setup::on_fsm1CB_currentIndexChanged(int index)
{
  QSpinBox * fssb[] = { ui->fsm1SB_1, ui->fsm1SB_2,ui->fsm1SB_3,ui->fsm1SB_4,ui->fsm1SB_5,ui->fsm1SB_6,ui->fsm1SB_7,ui->fsm1SB_8,
                               ui->fsm1SB_9, ui->fsm1SB_10,ui->fsm1SB_11,ui->fsm1SB_12,ui->fsm1SB_13,ui->fsm1SB_14,ui->fsm1SB_15,ui->fsm1SB_16, NULL };
  QSlider * fssld[] = { ui->fsm1SL_1, ui->fsm1SL_2,ui->fsm1SL_3,ui->fsm1SL_4,ui->fsm1SL_5,ui->fsm1SL_6,ui->fsm1SL_7,ui->fsm1SL_8,
                               ui->fsm1SL_9, ui->fsm1SL_10,ui->fsm1SL_11,ui->fsm1SL_12,ui->fsm1SL_13,ui->fsm1SL_14,ui->fsm1SL_15,ui->fsm1SL_16, NULL };
  model.moduleData[0].failsafeMode=index;
  for (int i=0; fssb[i]; i++) {
   if (index==1) {
      fssb[i]->setEnabled(true);
      fssld[i]->setEnabled(true);
    } else {
      fssb[i]->setDisabled(true);
      fssld[i]->setDisabled(true);
    }
  }
  emit modified();
}

void Setup::on_fsm2CB_currentIndexChanged(int index)
{
  QSpinBox * fssb[] = { ui->fsm2SB_1, ui->fsm2SB_2,ui->fsm2SB_3,ui->fsm2SB_4,ui->fsm2SB_5,ui->fsm2SB_6,ui->fsm2SB_7,ui->fsm2SB_8,
                               ui->fsm2SB_9, ui->fsm2SB_10,ui->fsm2SB_11,ui->fsm2SB_12,ui->fsm2SB_13,ui->fsm2SB_14,ui->fsm2SB_15,ui->fsm2SB_16, NULL };
  QSlider * fssld[] = { ui->fsm2SL_1, ui->fsm2SL_2,ui->fsm2SL_3,ui->fsm2SL_4,ui->fsm2SL_5,ui->fsm2SL_6,ui->fsm2SL_7,ui->fsm2SL_8,
                               ui->fsm2SL_9, ui->fsm2SL_10,ui->fsm2SL_11,ui->fsm2SL_12,ui->fsm2SL_13,ui->fsm2SL_14,ui->fsm2SL_15,ui->fsm2SL_16, NULL };
  model.moduleData[1].failsafeMode=index;
  for (int i=0; fssb[i]; i++) {
   if (index==1) {
      fssb[i]->setEnabled(true);
      fssld[i]->setEnabled(true);
    } else {
      fssb[i]->setDisabled(true);
      fssld[i]->setDisabled(true);
    }
  }
  emit modified();
}

void Setup::fssldValueChanged()
{
  if (!lock) {
    lock = true;
    QSpinBox * fssb1[] = { ui->fsm1SB_1, ui->fsm1SB_2,ui->fsm1SB_3,ui->fsm1SB_4,ui->fsm1SB_5,ui->fsm1SB_6,ui->fsm1SB_7,ui->fsm1SB_8,
                                 ui->fsm1SB_9, ui->fsm1SB_10,ui->fsm1SB_11,ui->fsm1SB_12,ui->fsm1SB_13,ui->fsm1SB_14,ui->fsm1SB_15,ui->fsm1SB_16, NULL };
    QSpinBox * fssb2[] = { ui->fsm2SB_1, ui->fsm2SB_2,ui->fsm2SB_3,ui->fsm2SB_4,ui->fsm2SB_5,ui->fsm2SB_6,ui->fsm2SB_7,ui->fsm2SB_8,
                                 ui->fsm2SB_9, ui->fsm2SB_10,ui->fsm2SB_11,ui->fsm2SB_12,ui->fsm2SB_13,ui->fsm2SB_14,ui->fsm2SB_15,ui->fsm2SB_16, NULL };
    QSlider *sl = qobject_cast<QSlider*>(sender());
    int fsId=sl->objectName().mid(sl->objectName().lastIndexOf("_")+1).toInt()-1;
    int moduleid=sl->objectName().mid(3,1).toInt();
    if (moduleid==1) {
      fssb1[fsId]->setValue(sl->value());
    } else {
      fssb2[fsId]->setValue(sl->value());
    }
    lock = false;
  }
}

void Setup::fssbValueChanged()
{
  if (!lock) {
    lock = true;
    QSlider * fssld1[] = { ui->fsm1SL_1, ui->fsm1SL_2,ui->fsm1SL_3,ui->fsm1SL_4,ui->fsm1SL_5,ui->fsm1SL_6,ui->fsm1SL_7,ui->fsm1SL_8,
                                 ui->fsm1SL_9, ui->fsm1SL_10,ui->fsm1SL_11,ui->fsm1SL_12,ui->fsm1SL_13,ui->fsm1SL_14,ui->fsm1SL_15,ui->fsm1SL_16, NULL };
    QSlider * fssld2[] = { ui->fsm2SL_1, ui->fsm2SL_2,ui->fsm2SL_3,ui->fsm2SL_4,ui->fsm2SL_5,ui->fsm2SL_6,ui->fsm2SL_7,ui->fsm2SL_8,
                                 ui->fsm2SL_9, ui->fsm2SL_10,ui->fsm2SL_11,ui->fsm2SL_12,ui->fsm2SL_13,ui->fsm2SL_14,ui->fsm2SL_15,ui->fsm2SL_16, NULL };
    QSpinBox *sb = qobject_cast<QSpinBox*>(sender());
    int fsId=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
    int moduleid=sb->objectName().mid(3,1).toInt();
    if (moduleid==1) {
      fssld1[fsId]->setValue(sb->value());
    } else {
      fssld2[fsId]->setValue(sb->value());
    }
    lock = false;
  }
}

void Setup::fssldEdited()
{
  QSlider *sl = qobject_cast<QSlider*>(sender());
  int fsId=sl->objectName().mid(sl->objectName().lastIndexOf("_")+1).toInt()-1;
  int moduleid=sl->objectName().mid(3,1).toInt();
  if (moduleid==1) {
    model.moduleData[0].failsafeChannels[fsId]=sl->value();
  } else {
    model.moduleData[1].failsafeChannels[fsId]=sl->value();
  }
  emit modified();
}

void Setup::fssbEdited()
{
  QSpinBox *sb = qobject_cast<QSpinBox*>(sender());
  int fsId=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
  int moduleid=sb->objectName().mid(3,1).toInt();
  if (moduleid==1) {
    model.moduleData[0].failsafeChannels[fsId]=sb->value();
  } else {
    model.moduleData[1].failsafeChannels[fsId]=sb->value();
  }
  emit modified();
}



void Setup::on_modelVoice_SB_editingFinished()
{
  model.modelVoice=ui->modelVoice_SB->value()-260;
  emit modified();
}

void Setup::on_timer1Perm_toggled(bool checked)
{
  model.timers[0].persistent=checked;
  emit modified();
}

void Setup::on_timer2Perm_toggled(bool checked)
{
  model.timers[1].persistent=checked;
  emit modified();
}

void Setup::on_timer1Minute_toggled(bool checked)
{
  model.timers[0].minuteBeep=checked;
  emit modified();
}

void Setup::on_timer2Minute_toggled(bool checked)
{
  model.timers[1].minuteBeep=checked;
  emit modified();
}

void Setup::on_timer1CountDownBeep_toggled(bool checked)
{
  model.timers[0].countdownBeep=checked;
  emit modified();
}

void Setup::on_timer2CountDownBeep_toggled(bool checked)
{
  model.timers[1].countdownBeep=checked;
  emit modified();
}

void Setup::on_timer1ModeCB_currentIndexChanged(int index)
{
    model.timers[0].mode = TimerMode(ui->timer1ModeCB->itemData(index).toInt());
    emit modified();
}

void Setup::on_timer1DirCB_currentIndexChanged(int index)
{
    model.timers[0].dir = index;
    emit modified();
}

void Setup::on_timer1ValTE_editingFinished()
{
    model.timers[0].val = ui->timer1ValTE->time().minute()*60 + ui->timer1ValTE->time().second();
    emit modified();
}

void Setup::on_timer1ModeBCB_currentIndexChanged(int index)
{
    model.timers[0].modeB = ui->timer1ModeBCB->itemData(index).toInt();
    emit modified();
}

void Setup::on_timer2ModeCB_currentIndexChanged(int index)
{
    model.timers[1].mode = TimerMode(ui->timer2ModeCB->itemData(index).toInt());
    emit modified();
}

void Setup::on_timer2DirCB_currentIndexChanged(int index)
{
    model.timers[1].dir = index;
    emit modified();
}

void Setup::on_timer2ValTE_editingFinished()
{
    model.timers[1].val = ui->timer2ValTE->time().minute()*60 + ui->timer2ValTE->time().second();
    emit modified();
}

void Setup::on_timer2ModeBCB_currentIndexChanged(int index)
{
    model.timers[1].modeB = ui->timer2ModeBCB->itemData(index).toInt();
    emit modified();
}

void Setup::on_trimIncCB_currentIndexChanged(int index)
{
    model.trimInc = index;
    emit modified();
}

void Setup::on_ttraceCB_currentIndexChanged(int index)
{
    model.thrTraceSrc = index;
    emit modified();
}

void Setup::on_pulsePolCB_currentIndexChanged(int index)
{
    model.moduleData[0].ppmPulsePol = index;
    emit modified();
}

void Setup::on_pulsePolCB_2_currentIndexChanged(int index)
{
    model.moduleData[1].ppmPulsePol = index;
    emit modified();
}

void Setup::on_pulsePolCB_3_currentIndexChanged(int index)
{
    model.moduleData[2].ppmPulsePol = index;
    emit modified();
}

void Setup::on_numChannelsSB_editingFinished()
{
    // TODO only accept valid values
    model.moduleData[0].channelsCount = 1+ui->numChannelsSB->value()-ui->numChannelsStart->value();
    ui->ppmFrameLengthDSB->setMinimum(model.moduleData[0].channelsCount*(model.extendedLimits ? 2.250 :2)+3.5);
    emit modified();
}

void Setup::on_numChannelsSB_2_editingFinished()
{
    // TODO only accept valid values
    model.moduleData[1].channelsCount = 1+ui->numChannelsSB_2->value()-ui->numChannelsStart_2->value();
    ui->ppmFrameLengthDSB_2->setMinimum(model.moduleData[1].channelsCount*(model.extendedLimits ? 2.250 :2.0)+3.5);
    emit modified();
}

void Setup::on_numChannelsSB_3_editingFinished()
{
    // TODO only accept valid values
    model.moduleData[2].channelsCount = 1+ui->numChannelsSB_3->value()-ui->numChannelsStart_3->value();
    ui->ppmFrameLengthDSB_3->setMinimum(model.moduleData[2].channelsCount*(model.extendedLimits ? 2.250 :2)+3.5);
    emit modified();
}

void Setup::on_numChannelsStart_editingFinished()
{
    // TODO only accept valid values
    model.moduleData[0].channelsStart = ui->numChannelsStart->value()-1;
    ui->numChannelsSB->setMinimum(model.moduleData[0].channelsStart+4);
    ui->numChannelsSB->setValue(model.moduleData[0].channelsStart+model.moduleData[0].channelsCount);
    emit modified();
}

void Setup::on_numChannelsStart_2_editingFinished()
{
    // TODO only accept valid values
    model.moduleData[1].channelsStart = ui->numChannelsStart_2->value()-1;
    ui->numChannelsSB_2->setMinimum(model.moduleData[1].channelsStart+4);
    ui->numChannelsSB_2->setValue(model.moduleData[1].channelsStart+model.moduleData[1].channelsCount);
    emit modified();
}

void Setup::on_numChannelsStart_3_editingFinished()
{
    // TODO only accept valid values
    model.moduleData[2].channelsStart = ui->numChannelsStart_3->value()-1;
    ui->numChannelsSB_3->setMinimum(model.moduleData[2].channelsStart+4);
    ui->numChannelsSB_3->setValue(model.moduleData[2].channelsStart+model.moduleData[2].channelsCount);
    emit modified();
}

void Setup::on_ppmDelaySB_editingFinished()
{
  if (!lock) {
    // TODO only accept valid values
    model.moduleData[0].ppmDelay = ui->ppmDelaySB->value();
    emit modified();
  }
}

void Setup::on_ppmDelaySB_2_editingFinished()
{
  if (!lock) {
    // TODO only accept valid values
    model.moduleData[1].ppmDelay = ui->ppmDelaySB_2->value();
    emit modified();
  }
}

void Setup::on_ppmDelaySB_3_editingFinished()
{
  if (!lock) {
    // TODO only accept valid values
    model.moduleData[2].ppmDelay = ui->ppmDelaySB_3->value();
    emit modified();
  }
}

void Setup::on_DSM_Type_currentIndexChanged(int index)
{
  if (!lock) {
    // model.moduleData[0].channelsCount = (index*2)+8;
    emit modified();
  }
}

void Setup::on_DSM_Type_2_currentIndexChanged(int index)
{
  if (!lock) {
    // model.moduleData[1].channelsCount = (index*2)+8;
    emit modified();
  }
}

void Setup::on_pxxRxNum_editingFinished()
{
  if (!lock) {
    if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
      // model.moduleData[0].channelsCount = (ui->pxxRxNum->value()-1)*2+8;
    }
    else {
      model.modelId = ui->pxxRxNum->value();
    }
    emit modified();
  }
}

void Setup::on_pxxRxNum_2_editingFinished()
{
  if (!lock) {
    /* if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
      model.moduleData[1].channelsCount = (ui->pxxRxNum_2->value()-1)*2+8;
    } */
    emit modified();
  }
}

void Setup::on_ppmFrameLengthDSB_editingFinished()
{
  model.moduleData[0].ppmFrameLength = (ui->ppmFrameLengthDSB->value()-22.5)/0.5;
  emit modified();
}

void Setup::on_ppmFrameLengthDSB_2_editingFinished()
{
  model.moduleData[1].ppmFrameLength = (ui->ppmFrameLengthDSB_2->value()-22.5)/0.5;
  emit modified();
}

void Setup::on_ppmFrameLengthDSB_3_editingFinished()
{
  model.moduleData[2].ppmFrameLength = (ui->ppmFrameLengthDSB_3->value()-22.5)/0.5;
  emit modified();
}

void Setup::on_instantTrim_CB_currentIndexChanged(int index)
{
  if (!lock) {
    bool found=false;
    for (int i=0; i< C9X_MAX_CUSTOM_FUNCTIONS; i++) {
      if (model.funcSw[i].func==FuncInstantTrim) {
        model.funcSw[i].swtch = RawSwitch(ui->instantTrim_CB->itemData(ui->instantTrim_CB->currentIndex()).toInt());
        found=true;
      }
    }
    if (found==false) {
      for (int i=0; i< C9X_MAX_CUSTOM_FUNCTIONS; i++) {
        if (model.funcSw[i].swtch==RawSwitch()) {
          model.funcSw[i].swtch = RawSwitch(ui->instantTrim_CB->itemData(ui->instantTrim_CB->currentIndex()).toInt());
          model.funcSw[i].func = FuncInstantTrim;
          break;
        }
      }
    }
    emit modified();
  }
}

void Setup::on_modelNameLE_editingFinished()
{
  int length=ui->modelNameLE->maxLength();
  strncpy(model.name, ui->modelNameLE->text().toAscii(), length);
  emit modified();
}

void Setup::on_modelImage_CB_currentIndexChanged(int index)
{
  if (!lock) {
    strncpy(model.bitmap, ui->modelImage_CB->currentText().toAscii(), GetEepromInterface()->getCapability(VoicesMaxLength));
    QSettings settings("companion9x", "companion9x");
    QString path=settings.value("sdPath", ".").toString();
    path.append("/BMP/");
    QDir qd(path);
    if (qd.exists()) {
      QString fileName=path;
      fileName.append(model.bitmap);
      fileName.append(".bmp");
      QImage image(fileName);
      if (image.isNull()) {
        fileName=path;
        fileName.append(model.bitmap);
        fileName.append(".BMP");
        image.load(fileName);
      }
      if (!image.isNull()) {
        ui->modelImage_image->setPixmap(QPixmap::fromImage(image.scaled( 64,32)));;
      }
      else {
        ui->modelImage_image->clear();
      }
    }
    else {
      ui->modelImage_image->clear();
    }
    emit modified();
  }
}

void Setup::startupSwitchEdited()
{
  if (GetEepromInterface()->getCapability(Pots)==3) {
    uint8_t i= 0;
    i|=(uint8_t)ui->swwarn0_CB->currentIndex();
    if (i==1) {
      ui->swwarn1_ChkB->setDisabled(true) ;
      ui->swwarn2_ChkB->setDisabled(true) ;
      ui->swwarn3_ChkB->setDisabled(true) ;
      ui->swwarn4_CB->setDisabled(true) ;
      ui->swwarn5_ChkB->setDisabled(true) ;
      ui->swwarn6_ChkB->setDisabled(true) ;
    }
    else {
      ui->swwarn1_ChkB->setEnabled(true) ;
      ui->swwarn2_ChkB->setEnabled(true) ;
      ui->swwarn3_ChkB->setEnabled(true) ;
      ui->swwarn4_CB->setEnabled(true) ;
      ui->swwarn5_ChkB->setEnabled(true) ;
      ui->swwarn6_ChkB->setEnabled(true) ;
      i|=(ui->swwarn1_ChkB->isChecked() ? 1 : 0)<<1;
      i|=(ui->swwarn2_ChkB->isChecked() ? 1 : 0)<<2;
      i|=(ui->swwarn3_ChkB->isChecked() ? 1 : 0)<<3;
      i|=((uint8_t)ui->swwarn4_CB->currentIndex() & 0x03)<<4;
      i|=(ui->swwarn5_ChkB->isChecked() ? 1 : 0)<<6;
      i|=(ui->swwarn6_ChkB->isChecked() ? 1 : 0)<<7;
    }
    model.switchWarningStates=i;
  }
  else {
    uint16_t i= 0;
    i|=(uint16_t)ui->tswwarn0_CB->currentIndex();
    if (i==1) {
      ui->chkSA->setDisabled(true);
      ui->chkSB->setDisabled(true);
      ui->chkSC->setDisabled(true);
      ui->chkSD->setDisabled(true);
      ui->chkSE->setDisabled(true);
      ui->chkSF->setDisabled(true);
      ui->chkSG->setDisabled(true);
    }
    else {
      ui->chkSA->setEnabled(true);
      ui->chkSB->setEnabled(true);
      ui->chkSC->setEnabled(true);
      ui->chkSD->setEnabled(true);
      ui->chkSE->setEnabled(true);
      ui->chkSF->setEnabled(true);
      ui->chkSG->setEnabled(true);
      i|=(((uint16_t)ui->chkSA->value())<<1);
      i|=(((uint16_t)ui->chkSB->value())<<3);
      i|=(((uint16_t)ui->chkSC->value())<<5);
      i|=(((uint16_t)ui->chkSD->value())<<7);
      i|=(((uint16_t)ui->chkSE->value())<<9);
      i|=(((uint16_t)ui->chkSF->value())<<12);
      i|=(((uint16_t)ui->chkSG->value())<<13);
    }
    model.switchWarningStates=i;
  }
  emit modified();
}

void Setup::on_thrTrimChkB_toggled(bool checked)
{
    model.thrTrim = checked;
    emit modified();
}

void Setup::on_thrExpoChkB_toggled(bool checked)
{
    model.thrExpo = checked;
    emit modified();
}

void Setup::on_bcRUDChkB_toggled(bool checked)
{
    if(checked) {
      model.beepANACenter |= BC_BIT_RUD;
    } else {
      model.beepANACenter &= ~BC_BIT_RUD;
    }
    emit modified();
}

void Setup::on_bcELEChkB_toggled(bool checked)
{
    if(checked) {
      model.beepANACenter |= BC_BIT_ELE;
    } else {
      model.beepANACenter &= ~BC_BIT_ELE;
    }
    emit modified();
}

void Setup::on_bcTHRChkB_toggled(bool checked)
{
    if(checked) {
      model.beepANACenter |= BC_BIT_THR;
    } else {
      model.beepANACenter &= ~BC_BIT_THR;
    }
    emit modified();
}

void Setup::on_bcAILChkB_toggled(bool checked)
{
    if(checked) {
      model.beepANACenter |= BC_BIT_AIL;
    } else {
      model.beepANACenter &= ~BC_BIT_AIL;
    }
    emit modified();
}

void Setup::on_bcP1ChkB_toggled(bool checked)
{
    if(checked) {
      model.beepANACenter |= BC_BIT_P1;
    } else {
      model.beepANACenter &= ~BC_BIT_P1;
    }
    emit modified();
}

void Setup::on_bcP2ChkB_toggled(bool checked)
{
    if(checked) {
      model.beepANACenter |= BC_BIT_P2;
    } else {
      model.beepANACenter &= ~BC_BIT_P2;
    }
    emit modified();
}

void Setup::on_bcP3ChkB_toggled(bool checked)
{
    if(checked) {
      model.beepANACenter |= BC_BIT_P3;
    } else {
      model.beepANACenter &= ~BC_BIT_P3;
    }
    emit modified();
}

void Setup::on_bcP4ChkB_toggled(bool checked)
{
    if(checked) {
      model.beepANACenter |= BC_BIT_P4;
    } else {
      model.beepANACenter &= ~BC_BIT_P4;
    }
    emit modified();
}

void Setup::on_bcREaChkB_toggled(bool checked)
{
    if(checked) {
      model.beepANACenter |= BC_BIT_REA;
    } else {
      model.beepANACenter &= ~BC_BIT_REA;
    }
    emit modified();
}

void Setup::on_bcREbChkB_toggled(bool checked)
{
    if(checked) {
      model.beepANACenter |= BC_BIT_REB;
    } else {
      model.beepANACenter &= ~BC_BIT_REB;
    }
    emit modified();
}

