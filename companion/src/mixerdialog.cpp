#include "mixerdialog.h"
#include "ui_mixerdialog.h"
#include "eeprominterface.h"
#include "helpers.h"

MixerDialog::MixerDialog(QWidget *parent, MixData *mixdata, int stickMode) :
    QDialog(parent),
    ui(new Ui::MixerDialog),
    md(mixdata)
{
    ui->setupUi(this);
    QRegExp rx(CHAR_FOR_NAMES_REGEX);
    QLabel * lb_fp[] = {ui->lb_FP0,ui->lb_FP1,ui->lb_FP2,ui->lb_FP3,ui->lb_FP4,ui->lb_FP5,ui->lb_FP6,ui->lb_FP7,ui->lb_FP8 };
    QCheckBox * cb_fp[] = {ui->cb_FP0,ui->cb_FP1,ui->cb_FP2,ui->cb_FP3,ui->cb_FP4,ui->cb_FP5,ui->cb_FP6,ui->cb_FP7,ui->cb_FP8 };

    if (md->destCh > (unsigned int)GetEepromInterface()->getCapability(Outputs))
      this->setWindowTitle(tr("DEST -> X%1").arg(md->destCh-GetEepromInterface()->getCapability(Outputs)));
    else
      this->setWindowTitle(tr("DEST -> CH%1%2").arg(md->destCh/10).arg(md->destCh%10));
    populateSourceCB(ui->sourceCB, md->srcRaw, POPULATE_SOURCES | POPULATE_SWITCHES | (GetEepromInterface()->getCapability(ExtraTrims) ? POPULATE_TRIMS : 0) | (GetEepromInterface()->getCapability(GvarsAsSources) ? POPULATE_GVARS : 0));
    ui->sourceCB->removeItem(0);
    int limit=GetEepromInterface()->getCapability(OffsetWeight);
    if (GetEepromInterface()->getCapability(GvarsAsWeight)) {
      int gvars=0;
      if (GetEepromInterface()->getCapability(HasVariants)) {
        if ((GetCurrentFirmwareVariant() & GVARS_VARIANT)) {
          gvars=1;
        }
      } else {
        gvars=1;
      }
      if (gvars==0) {
        ui->offsetGV->setDisabled(true);
        ui->weightGV->setDisabled(true);
        ui->differentialGV->setDisabled(true);
        if (md->weight>limit || md->weight<-limit) {
          md->weight=100;
        }
        if (md->sOffset>limit || md->sOffset<-limit) {
          md->sOffset=0;
        }
        if (md->differential>100 || md->weight<-100) {
          md->differential=0;
        }
      }
      populateGVCB(ui->offsetCB,md->sOffset);
      populateGVCB(ui->weightCB,md->weight);
      populateGVCB(ui->differentialCB,md->differential);
      ui->weightSB->setMinimum(-limit);
      ui->weightSB->setMaximum(limit);
      if (md->weight>limit || md->weight<-limit) {
        ui->weightGV->setChecked(true);
        ui->weightSB->hide();
        ui->weightCB->show();
      } else {
        ui->weightGV->setChecked(false);
        ui->weightSB->setValue(md->weight);
        ui->weightSB->show();
        ui->weightCB->hide();
      }

      ui->offsetSB->setMinimum(-limit);
      ui->offsetSB->setMaximum(limit);
      if (md->sOffset>limit || md->sOffset<-limit) {
        ui->offsetGV->setChecked(true);
        ui->offsetSB->hide();
        ui->offsetCB->show();
      } else {
        ui->offsetGV->setChecked(false);
        ui->offsetSB->setValue(md->sOffset);
        ui->offsetSB->show();
        ui->offsetCB->hide();
      }

      ui->differentialSB->setMinimum(-100);
      ui->differentialSB->setMaximum(100);
      if (md->differential>100 || md->differential<-100) {
        ui->differentialGV->setChecked(true);
        ui->differentialSB->hide();
        ui->differentialCB->show();
      } else {
        ui->differentialGV->setChecked(false);
        ui->differentialSB->setValue(md->differential);
        ui->differentialSB->show();
        ui->differentialCB->hide();
      }
    } else {
      ui->offsetGV->hide();
      ui->weightGV->hide();
      ui->differentialGV->hide();
      ui->offsetSB->setMinimum(-limit);
      ui->offsetSB->setMaximum(limit);
      ui->offsetSB->setValue(md->sOffset);
      ui->differentialSB->setMinimum(-100);
      ui->differentialSB->setMaximum(100);
      ui->differentialSB->setValue(md->differential);
      ui->weightSB->setMinimum(-limit);
      ui->weightSB->setMaximum(limit);
      ui->weightSB->setValue(md->weight);
    }

    ui->FixOffsetChkB->setChecked(md->lateOffset);
    ui->MixDR_CB->setChecked(md->noExpo==0);
    if (md->enableFmTrim==1) {
      ui->label_4->setText(tr("FM Trim Value"));
    }
    else {
      ui->label_4->setText(tr("Offset"));
    }
    if (!GetEepromInterface()->getCapability(MixesWithoutExpo)) {
      ui->MixDR_CB->hide();
      ui->label_MixDR->hide();
    }
    if (!GetEepromInterface()->getCapability(MixFmTrim)) {
      ui->FMtrimChkB->hide();
      ui->label_FMtrim->hide();
      ui->label_4->setText(tr("Offset"));
    }
    if (!GetEepromInterface()->getCapability(HasFixOffset)) {
      ui->FixOffsetChkB->hide();
      ui->label_FixOffset->hide();
    }
    if (GetEepromInterface()->getCapability(ExtraTrims)) {
      ui->trimCB->addItem(tr("Rud"),1);
      ui->trimCB->addItem(tr("Ele"),2);
      ui->trimCB->addItem(tr("Thr"),3);
      ui->trimCB->addItem(tr("Ail"),4);      
    }
    ui->trimCB->setCurrentIndex((-md->carryTrim)+1);
    if (!GetEepromInterface()->getCapability(DiffMixers)) {
      ui->differentialGV->hide();
      ui->differentialSB->hide();
      ui->differentialCB->hide();
      ui->label_curve->setText(tr("Curve"));
    }
    int namelength=GetEepromInterface()->getCapability(HasMixerNames);
    if (!namelength) {
      ui->label_name->hide();
      ui->mixerName->hide();
    } else {
      ui->mixerName->setMaxLength(namelength);
    }
    if (!GetEepromInterface()->getCapability(FlightPhases)) {
      ui->label_phases->hide();
      for (int i=0; i<9; i++) {
        lb_fp[i]->hide();
        cb_fp[i]->hide();
      }
    } else {
      int mask=1;
      for (int i=0; i<9 ; i++) {
        if ((md->phases & mask)==0) {
          cb_fp[i]->setChecked(true);
        }
        mask <<= 1;
      }
      for (int i=GetEepromInterface()->getCapability(FlightPhases); i<9;i++) {
        lb_fp[i]->hide();
        cb_fp[i]->hide();
      }
    }
    ui->mixerName->setValidator(new QRegExpValidator(rx, this));
    ui->mixerName->setText(md->name);
    populateCurvesCB(ui->curvesCB,md->curve);
    populateSwitchCB(ui->switchesCB,md->swtch);
    ui->warningCB->setCurrentIndex(md->mixWarn);
    ui->mltpxCB->setCurrentIndex(md->mltpx);
    int scale=GetEepromInterface()->getCapability(SlowScale);  
    float range=GetEepromInterface()->getCapability(SlowRange);  
    ui->slowDownSB->setMaximum(range/scale);
    ui->slowDownSB->setSingleStep(1.0/scale);
    ui->slowDownSB->setDecimals((scale==1 ? 0 :1));
    ui->slowDownSB->setValue((float)md->speedDown/scale);
    ui->slowUpSB->setMaximum(range/scale);
    ui->slowUpSB->setSingleStep(1.0/scale);
    ui->slowUpSB->setDecimals((scale==1 ? 0 :1));
    ui->slowUpSB->setValue((float)md->speedUp/scale);
    ui->delayDownSB->setMaximum(range/scale);
    ui->delayDownSB->setSingleStep(1.0/scale);
    ui->delayDownSB->setDecimals((scale==1 ? 0 :1));
    ui->delayDownSB->setValue((float)md->delayDown/scale);
    ui->delayUpSB->setMaximum(range/scale);
    ui->delayUpSB->setSingleStep(1.0/scale);
    ui->delayUpSB->setDecimals((scale==1 ? 0 :1));
    ui->delayUpSB->setValue((float)md->delayUp/scale);
    QTimer::singleShot(0, this, SLOT(shrink()));

    valuesChanged();
    connect(ui->mixerName,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->sourceCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->weightCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->offsetCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->differentialCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->weightSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->offsetSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->differentialSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->weightGV,SIGNAL(stateChanged(int)),this,SLOT(widgetChanged()));
    connect(ui->offsetGV,SIGNAL(stateChanged(int)),this,SLOT(widgetChanged()));
    connect(ui->differentialGV,SIGNAL(stateChanged(int)),this,SLOT(widgetChanged()));
    connect(ui->trimCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->MixDR_CB,SIGNAL(toggled(bool)),this,SLOT(valuesChanged()));
    connect(ui->FMtrimChkB,SIGNAL(toggled(bool)),this,SLOT(valuesChanged()));
    connect(ui->FixOffsetChkB,SIGNAL(toggled(bool)),this,SLOT(valuesChanged()));
    connect(ui->curvesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->switchesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->warningCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->mltpxCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->delayDownSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->delayUpSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->slowDownSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->slowUpSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    for (int i=0; i<9; i++) {
      connect(cb_fp[i],SIGNAL(toggled(bool)),this,SLOT(valuesChanged()));
    }
}

MixerDialog::~MixerDialog()
{
    delete ui;
}

void MixerDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MixerDialog::widgetChanged()
{
    if (GetEepromInterface()->getCapability(GvarsAsWeight)) {
      int gvars=0;
      if (GetEepromInterface()->getCapability(HasVariants)) {
        if ((GetCurrentFirmwareVariant() & GVARS_VARIANT)) {
          gvars=1;
        }
      } else {
        gvars=1;
      }
      if (gvars==1) {
        if (!GetEepromInterface()->getCapability(DiffMixers)) {
          if (ui->differentialGV->isChecked()) {
            ui->differentialCB->show();
            ui->differentialSB->hide();
          } else {
            ui->differentialCB->hide();
            ui->differentialSB->show();
          }
        } 
        if (ui->weightGV->isChecked()) {
          ui->weightCB->show();
          ui->weightSB->hide();
        } else {
          ui->weightCB->hide();
          ui->weightSB->show();
        }
        if (ui->offsetGV->isChecked()) {
          ui->offsetCB->show();
          ui->offsetSB->hide();
        } else {
          ui->offsetCB->hide();
          ui->offsetSB->show();
        }
      }
      valuesChanged();
      QTimer::singleShot(0, this, SLOT(shrink()));      
    }
}

void MixerDialog::valuesChanged()
{
    QCheckBox * cb_fp[] = {ui->cb_FP0,ui->cb_FP1,ui->cb_FP2,ui->cb_FP3,ui->cb_FP4,ui->cb_FP5,ui->cb_FP6,ui->cb_FP7,ui->cb_FP8 };
    md->srcRaw  = RawSource(ui->sourceCB->itemData(ui->sourceCB->currentIndex()).toInt());
    if ((ui->sourceCB->itemData(ui->sourceCB->currentIndex()).toInt()-65536)<4) {
      if (!GetEepromInterface()->getCapability(MixesWithoutExpo)) {
        ui->MixDR_CB->hide();
        ui->label_MixDR->hide();
      } else {
        ui->MixDR_CB->setVisible(true);
        ui->label_MixDR->setVisible(true);
      }
    } else {
      ui->MixDR_CB->setHidden(true);
      ui->label_MixDR->setHidden(true);
    }
    if (ui->weightGV->isChecked()) {
      md->weight = ui->weightCB->itemData(ui->weightCB->currentIndex()).toInt();
    } else {
      md->weight = ui->weightSB->value();
    }
    if (ui->offsetGV->isChecked()) {
      md->sOffset = ui->offsetCB->itemData(ui->offsetCB->currentIndex()).toInt();
    } else {
      md->sOffset = ui->offsetSB->value();
    }
    md->carryTrim = -(ui->trimCB->currentIndex()-1);
    md->noExpo = ui->MixDR_CB->checkState() ? 0 : 1;
    md->lateOffset = ui->FixOffsetChkB->checkState() ? 1 : 0;
    md->enableFmTrim = ui->FMtrimChkB->checkState() ? 1 : 0;
    int numcurves=GetEepromInterface()->getCapability(NumCurves);
    if (numcurves==0) {
      numcurves=16;
    }
    if (GetEepromInterface()->getCapability(DiffMixers) && (ui->curvesCB->currentIndex()-(numcurves)*GetEepromInterface()->getCapability(HasNegCurves))==0){
      ui->differentialGV->show();
       if (ui->differentialGV->isChecked()) {
         ui->differentialSB->hide();
         ui->differentialCB->show();
       } else {
         ui->differentialSB->show();
         ui->differentialCB->hide();            
       }
    } else {
      ui->differentialGV->hide();
      ui->differentialGV->setChecked(false);
      ui->differentialSB->hide();
      ui->differentialSB->setValue(0);
      ui->differentialCB->hide(); 
    }
    md->curve     = ui->curvesCB->currentIndex()-(numcurves)*GetEepromInterface()->getCapability(HasNegCurves);
    md->swtch     = RawSwitch(ui->switchesCB->itemData(ui->switchesCB->currentIndex()).toInt());
    md->mixWarn   = ui->warningCB->currentIndex();
    md->mltpx     = (MltpxValue)ui->mltpxCB->currentIndex();
    int scale=GetEepromInterface()->getCapability(SlowScale);
    md->delayDown = round(ui->delayDownSB->value()*scale);
    md->delayUp   = round(ui->delayUpSB->value()*scale);
    md->speedDown = round(ui->slowDownSB->value()*scale);
    md->speedUp   = round(ui->slowUpSB->value()*scale);
    if (ui->differentialGV->isChecked()) {
      md->differential = ui->differentialCB->itemData(ui->differentialCB->currentIndex()).toInt();
    } else {
      md->differential = ui->differentialSB->value();
    }
    if (GetEepromInterface()->getCapability(MixFmTrim) && md->enableFmTrim==1) {
        ui->label_4->setText(tr("FM Trim Value"));
    } else {
        ui->label_4->setText(tr("Offset"));
    }
    int i=0;
    for (i=0; i<ui->mixerName->text().toAscii().length(); i++) {
      md->name[i]=ui->mixerName->text().toAscii().at(i);
    }
    md->name[i]=0;
    md->phases=0;
    for (int i=8; i>=0 ; i--) {
      if (!cb_fp[i]->checkState()) {
        md->phases+=1;
      }
      md->phases<<=1;
    }
    md->phases>>=1;
}

void MixerDialog::shrink() {
    resize(0,0);
}
