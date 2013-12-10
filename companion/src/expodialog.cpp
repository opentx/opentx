#include "expodialog.h"
#include "ui_expodialog.h"
#include "eeprominterface.h"
#include "helpers.h"

ExpoDialog::ExpoDialog(QWidget *parent, ExpoData *expoData, int stickMode) :
    QDialog(parent),
    ui(new Ui::ExpoDialog),
    ed(expoData)
{
    ui->setupUi(this);
    QLabel * lb_fp[] = {ui->lb_FP0,ui->lb_FP1,ui->lb_FP2,ui->lb_FP3,ui->lb_FP4,ui->lb_FP5,ui->lb_FP6,ui->lb_FP7,ui->lb_FP8 };
    QCheckBox * cb_fp[] = {ui->cb_FP0,ui->cb_FP1,ui->cb_FP2,ui->cb_FP3,ui->cb_FP4,ui->cb_FP5,ui->cb_FP6,ui->cb_FP7,ui->cb_FP8 };

    setWindowTitle(tr("DEST -> %1").arg(getStickStr(ed->chn)));
    QRegExp rx(CHAR_FOR_NAMES_REGEX);
    
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
        ui->expoGV->setDisabled(true);
        ui->weightGV->setDisabled(true);
        ui->expoCurveGV->setDisabled(true);
        if (ed->expo>100) {
          ed->expo=0;
        }
        if (ed->weight>100 || ed->weight<-100) {
          ed->weight=100;
        }
        if (ed->curveParam>100 || ed->curveParam<-100) {
          ed->curveParam=0;
        }
      }
      populateGVCB(ui->expoCB,ed->expo);
      populateGVCB(ui->weightCB,ed->weight);
      populateGVCB(ui->expoCurveCB,ed->curveParam);

      ui->weightSB->setMinimum(0);
      ui->weightSB->setMaximum(100);
      if (ed->weight>100 || ed->weight<0) {
        ui->weightGV->setChecked(true);
        ui->weightSB->hide();
        ui->weightCB->show();
      } else {
        ui->weightGV->setChecked(false);
        ui->weightSB->setValue(ed->weight);
        ui->weightSB->show();
        ui->weightCB->hide();
      }

      ui->expoSB->setMinimum(-100);
      ui->expoSB->setMaximum(100);
      if (ed->expo>100 || ed->expo<-100) {
        ui->expoGV->setChecked(true);
        ui->expoSB->hide();
        ui->expoCB->show();
      } else {
        ui->expoGV->setChecked(false);
        ui->expoSB->setValue(ed->expo);
        ui->expoSB->show();
        ui->expoCB->hide();
      }

      ui->expoCurveSB->setMinimum(-100);
      ui->expoCurveSB->setMaximum(100);
      if (ed->curveParam>100 || ed->curveParam<-100) {
        ui->expoCurveGV->setChecked(true);
        ui->expoCurveSB->hide();
        ui->expoCurveCB->show();
      } else {
        ui->expoCurveGV->setChecked(false);
        ui->expoCurveSB->setValue(ed->curveParam);
        ui->expoCurveSB->show();
        ui->expoCurveCB->hide();
      }
    } else {
      ui->expoGV->hide();
      ui->weightGV->hide();
      ui->expoCurveGV->hide();
      ui->expoSB->setMinimum(-100);
      ui->expoSB->setMaximum(100);
      ui->expoSB->setValue(ed->expo);
      ui->expoCurveSB->setMinimum(-100);
      ui->expoCurveSB->setMaximum(100);
      ui->expoCurveSB->setValue(ed->curveParam);
      ui->weightSB->setMinimum(0);
      ui->weightSB->setMaximum(100);
      ui->weightSB->setValue(ed->weight);
    }
    populateSwitchCB(ui->switchesCB,ed->swtch);
    if (ed->curveMode==0) {
      populateExpoCurvesCB(ui->curvesCB,0); // TODO capacity for er9x
    } else {
      populateExpoCurvesCB(ui->curvesCB,ed->curveParam); // TODO capacity for er9x
    }
    ui->modeCB->setCurrentIndex(ed->mode-1);
    if (!GetEepromInterface()->getCapability(HasExpoCurves)) {
      ui->label_curves->hide();
      ui->curvesCB->hide();
      ed->curveMode=1;
    }
    if (!GetEepromInterface()->getCapability(ExpoIsCurve)) {
      ui->expoCurveCB->hide();
      ui->expoCurveSB->hide();
      ui->expoCurveGV->hide();
      ui->label_curves->setText(tr("Curve"));
    } else {
      ui->label_expo->hide();
      ui->expoCB->hide();
      ui->expoGV->hide();
      ui->expoSB->hide();
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
        if ((ed->phases & mask)==0) {
          cb_fp[i]->setChecked(true);
        }
        mask <<= 1;
      }
      for (int i=GetEepromInterface()->getCapability(FlightPhases); i<9;i++) {
        lb_fp[i]->hide();
        cb_fp[i]->hide();
      }      
    }
    int expolength=GetEepromInterface()->getCapability(HasExpoNames);
    if (!expolength) {
      ui->label_name->hide();
      ui->expoName->hide();
    } else {
      ui->expoName->setMaxLength(expolength);
    }
    ui->expoName->setValidator(new QRegExpValidator(rx, this));
    ui->expoName->setText(ed->name);
    valuesChanged();
    connect(ui->expoName,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->expoCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->expoSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->expoCurveCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->expoCurveSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->weightCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->weightSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->switchesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->curvesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->modeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->expoGV,SIGNAL(stateChanged(int)),this,SLOT(widgetChanged()));
    connect(ui->expoCurveGV,SIGNAL(stateChanged(int)),this,SLOT(widgetChanged()));
    connect(ui->weightGV,SIGNAL(stateChanged(int)),this,SLOT(widgetChanged()));
    for (int i=0; i<9; i++) {
      connect(cb_fp[i],SIGNAL(toggled(bool)),this,SLOT(valuesChanged()));
    }
    QTimer::singleShot(0, this, SLOT(shrink()));
}

ExpoDialog::~ExpoDialog()
{
    delete ui;
}

void ExpoDialog::changeEvent(QEvent *e)
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

void ExpoDialog::widgetChanged()
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
        if (!GetEepromInterface()->getCapability(ExpoIsCurve)) {
          if (ui->expoGV->isChecked()) {
            ui->expoCB->show();
            ui->expoSB->hide();
          } else {
            ui->expoCB->hide();
            ui->expoSB->show();
          }
        } else {
          if (ui->expoCurveGV->isChecked()) {
            ui->expoCurveCB->show();
            ui->expoCurveSB->hide();
          } else {
            ui->expoCurveCB->hide();
            ui->expoCurveSB->show();
          }
        }
        if (ui->weightGV->isChecked()) {
          ui->weightCB->show();
          ui->weightSB->hide();
        } else {
          ui->weightCB->hide();
          ui->weightSB->show();
        }
      }
      valuesChanged();
      QTimer::singleShot(0, this, SLOT(shrink()));      
    }
}


void ExpoDialog::valuesChanged()
{
    QCheckBox * cb_fp[] = {ui->cb_FP0,ui->cb_FP1,ui->cb_FP2,ui->cb_FP3,ui->cb_FP4,ui->cb_FP5,ui->cb_FP6,ui->cb_FP7,ui->cb_FP8 };
    if (ui->curvesCB->currentIndex()==0)  {
      if (GetEepromInterface()->getCapability(ExpoIsCurve)) {
        ed->curveMode = 0;
        ui->expoCurveGV->show();
        if (ui->expoCurveGV->isChecked()) {
          ui->expoCurveCB->show();
          ui->expoCurveSB->hide();
          ed->curveParam = ui->expoCurveCB->itemData(ui->expoCurveCB->currentIndex()).toInt();
        } else {
          ui->expoCurveCB->hide();
          ui->expoCurveSB->show();
          ed->curveParam = ui->expoCurveSB->value();
        }
        ed->expo = ed->curveParam;
      } else {
        ui->expoCurveCB->hide();
        ui->expoCurveSB->hide();
        ui->expoCurveGV->hide();
        ed->curveMode = 0;
        if (ui->expoGV->isChecked()) {
          ed->expo = ui->expoCB->itemData(ui->expoCB->currentIndex()).toInt();
        } else {
          ed->expo = ui->expoSB->value();
        }
        ed->curveParam = ed->expo;
      }
    } else {
      if (!GetEepromInterface()->getCapability(ExpoIsCurve)) {
        ed->curveMode = 1;
        ed->curveParam = ui->curvesCB->currentIndex();
        if (ui->expoGV->isChecked()) {
          ed->expo = ui->expoCB->itemData(ui->expoCB->currentIndex()).toInt();
        } else {
          ed->expo = ui->expoSB->value();
        }
      } else {
        ed->curveMode = 1;
        ed->curveParam = ui->curvesCB->currentIndex();
        ed->expo = 0;
      }
      ui->expoCurveCB->hide();
      ui->expoCurveSB->hide();
      ui->expoCurveGV->hide();
    }
    if (ui->weightGV->isChecked()) {
      ed->weight = ui->weightCB->itemData(ui->weightCB->currentIndex()).toInt();
    } else {
      ed->weight = ui->weightSB->value();
    }
    ed->swtch  = RawSwitch(ui->switchesCB->itemData(ui->switchesCB->currentIndex()).toInt());
    ed->mode   = ui->modeCB->currentIndex() + 1;
    int i=0;
    for (i=0; i<ui->expoName->text().toAscii().length(); i++) {
      ed->name[i]=ui->expoName->text().toAscii().at(i);
    }
    ed->name[i]=0;
    ed->phases=0;
    for (int i=8; i>=0 ; i--) {
      if (!cb_fp[i]->checkState()) {
        ed->phases+=1;
      }
      ed->phases<<=1;
    }
    ed->phases>>=1;
    if (GetEepromInterface()->getCapability(FlightPhases)) {
      int zeros=0;
      int ones=0;
      int phtemp=ed->phases;
      for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
        if (phtemp & 1) {
          ones++;
        }
        else {
          zeros++;
        }
        phtemp >>=1;
      }
      if (zeros==1) {
        phtemp=ed->phases;
        for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
          if ((phtemp & 1)==0) {
            break;
          }
          phtemp >>=1;
        }
      }
      else if (ones==1) {
        phtemp=ed->phases;
        for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
          if (phtemp & 1) {
            break;
          }
          phtemp >>=1;
        }
      }
    } else {
      ed->phases=0;
    }  
}

void ExpoDialog::shrink()
{
    resize(0,0);
}
