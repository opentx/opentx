#include "telemetry.h"
#include "ui_telemetry.h"
#include "ui_telemetry_analog.h"
#include "helpers.h"

TelemetryAnalog::TelemetryAnalog(QWidget *parent, FrSkyChannelData & analog):
  QWidget(parent),
  ui(new Ui::TelemetryAnalog),
  analog(analog),
  lock(false)
{
  ui->setupUi(this);

  float ratio;

  if (analog.type==0 || analog.type==1 || analog.type==2) {
    ratio = (analog.ratio<<analog.multiplier)/10.0;
    ui->RatioSB->setDecimals(1);
    ui->RatioSB->setMaximum(25.5*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
  }
  else {
    ratio = analog.ratio<<analog.multiplier;
    ui->RatioSB->setDecimals(0);
    ui->RatioSB->setMaximum(255*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
  }
  ui->RatioSB->setValue(ratio);

  update();

  ui->UnitCB->setCurrentIndex(analog.type);
  ui->alarm1LevelCB->setCurrentIndex(analog.alarms[0].level);
  ui->alarm1GreaterCB->setCurrentIndex(analog.alarms[0].greater);
  ui->alarm2LevelCB->setCurrentIndex(analog.alarms[1].level);
  ui->alarm2GreaterCB->setCurrentIndex(analog.alarms[1].greater);

  if (!(GetEepromInterface()->getCapability(Telemetry) & TM_HASOFFSET)) {
    ui->CalibSB->hide();
    ui->CalibLabel->hide();
  }
  else {
    ui->label_Max->setText(tr("Range"));
  }
}

void TelemetryAnalog::update()
{
    float ratio, alarm1value, alarm2value;
    if (analog.ratio==0) {
      ui->alarm1ValueSB->setMinimum(0);
      ui->alarm1ValueSB->setMaximum(0);
      ui->alarm2ValueSB->setMinimum(0);
      ui->alarm2ValueSB->setMaximum(0);
      ui->CalibSB->setMinimum(0);
      ui->CalibSB->setMaximum(0);
      ui->CalibSB->setValue(0);
      ui->alarm1ValueSB->setValue(0);
      ui->alarm2ValueSB->setValue(0);
      return;
    }

    if (analog.type==0 || analog.type==1 || analog.type==2) {
      ratio=(analog.ratio<<analog.multiplier)/10.0;
      ui->alarm1ValueSB->setDecimals(2);
      ui->alarm1ValueSB->setSingleStep(ratio/255.0);
      ui->alarm2ValueSB->setDecimals(2);
      ui->alarm2ValueSB->setSingleStep(ratio/255.0);
      ui->alarm1ValueSB->setMinimum((analog.ratio*1.0*analog.offset)/2550.0);
      ui->alarm1ValueSB->setMaximum(ratio+(analog.ratio*1.0*analog.offset)/2550.0);
      ui->alarm2ValueSB->setMinimum((analog.offset*1.0*analog.ratio)/2550.0);
      ui->alarm2ValueSB->setMaximum(ratio+(analog.offset*1.0*analog.ratio)/2550.0);
    }
    else {
      ratio=analog.ratio<<analog.multiplier;
      ui->alarm1ValueSB->setDecimals(2);
      ui->alarm1ValueSB->setSingleStep(ratio/255.0);
      ui->alarm2ValueSB->setDecimals(2);
      ui->alarm2ValueSB->setSingleStep(ratio/255.0);
      ui->alarm1ValueSB->setMinimum((analog.offset*1.0*analog.ratio)/255.0);
      ui->alarm1ValueSB->setMaximum(ratio+(analog.offset*1.0*analog.ratio)/255.0);
      ui->alarm2ValueSB->setMinimum((analog.offset*1.0*analog.ratio)/255.0);
      ui->alarm2ValueSB->setMaximum(ratio+(analog.offset*1.0*analog.ratio)/255.0);
    }
    ui->CalibSB->setDecimals(2);
    ui->CalibSB->setMaximum((ratio*127)/255.0);
    ui->CalibSB->setMinimum((-ratio*128)/255.0);
    ui->CalibSB->setSingleStep(ratio/255.0);
    ui->CalibSB->setValue((analog.offset*ratio)/255);
    alarm1value=ratio*(analog.alarms[0].value/255.0+analog.offset/255.0);
    alarm2value=ratio*(analog.alarms[1].value/255.0+analog.offset/255.0);;
    ui->alarm1ValueSB->setValue(alarm1value);
    ui->alarm2ValueSB->setValue(alarm2value);
}

void TelemetryAnalog::on_UnitCB_currentIndexChanged(int index)
{
    float ratio;
    analog.type=index;
    switch (index) {
      case 0:
      case 1:
      case 2:
        ui->RatioSB->setDecimals(1);
        ui->RatioSB->setMaximum(25.5*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
        ratio=(analog.ratio<<analog.multiplier)/10.0;
        break;
      default:
        ui->RatioSB->setDecimals(0);
        ui->RatioSB->setMaximum(255*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
        ratio=(analog.ratio<<analog.multiplier);
        break;
    }
    ui->RatioSB->setValue(ratio);
    update();
    emit modified();
}

void TelemetryAnalog::on_RatioSB_valueChanged()
{
  if (!lock) {
    if (analog.type==0 || analog.type==1 || analog.type==2) {
      analog.multiplier = findmult(ui->RatioSB->value(), 25.5);
      float singlestep =(1<<analog.multiplier)/10.0;
      lock=true;
      ui->RatioSB->setSingleStep(singlestep);
      ui->RatioSB->setValue(round(ui->RatioSB->value()/singlestep)*singlestep);
      lock=false;
    }
    else {
      analog.multiplier = findmult(ui->RatioSB->value(), 255);
      float singlestep =(1<<analog.multiplier);
      lock = true;
      ui->RatioSB->setSingleStep(singlestep);
      ui->RatioSB->setValue(round(ui->RatioSB->value()/singlestep)*singlestep);
      lock = false;
    }
    emit modified();
  }
}

void TelemetryAnalog::on_RatioSB_editingFinished()
{
    float ratio, calib, alarm1value,alarm2value;
    if (lock) return;
    if (analog.type==0 || analog.type==1 || analog.type==2) {
      analog.multiplier = findmult(ui->RatioSB->value(), 25.5);
      ui->CalibSB->setSingleStep((1<<analog.multiplier)/10.0);
      ui->alarm1ValueSB->setSingleStep((1<<analog.multiplier)/10.0);
      ui->alarm2ValueSB->setSingleStep((1<<analog.multiplier)/10.0);
      analog.ratio = ((int)(round(ui->RatioSB->value()*10))/(1 <<analog.multiplier));
    }  else {
      analog.multiplier = findmult(ui->RatioSB->value(), 255);
      ui->CalibSB->setSingleStep(1<<analog.multiplier);
      ui->alarm1ValueSB->setSingleStep(1<<analog.multiplier);
      ui->alarm2ValueSB->setSingleStep(1<<analog.multiplier);
      analog.ratio = (ui->RatioSB->value()/(1 << analog.multiplier));
    }
    ui->CalibSB->setMaximum((ui->RatioSB->value()*127)/255);
    ui->CalibSB->setMinimum((ui->RatioSB->value()*-128)/255);
    ui->alarm1ValueSB->setMaximum(ui->RatioSB->value());
    ui->alarm2ValueSB->setMaximum(ui->RatioSB->value());
    repaint();
    ratio=analog.ratio * (1 << analog.multiplier);
    calib=ui->CalibSB->value();
    alarm1value=ui->alarm1ValueSB->value();
    alarm2value=ui->alarm2ValueSB->value();
    if (analog.type==0) {
      calib*=10;
      alarm1value*=10;
      alarm2value*=10;
    }
    if (calib>0) {
      if (calib>((ratio*127)/255)) {
        analog.offset=127;
      } else {
        analog.offset=round(calib*255/ratio);
      }
    }
    if (calib<0) {
      if (calib<((ratio*-128)/255)) {
        analog.offset=-128;
      } else {
        analog.offset=round(calib*255/ratio);
      }
    }
    analog.alarms[0].value=round((alarm1value*255-analog.offset*(analog.ratio<<analog.multiplier))/(analog.ratio<<analog.multiplier));
    analog.alarms[1].value=round((alarm2value*255-analog.offset*(analog.ratio<<analog.multiplier))/(analog.ratio<<analog.multiplier));
    update();
    // TODO ? telBarUpdate();
    emit modified();
}

void TelemetryAnalog::on_CalibSB_editingFinished()
{
    float ratio,calib,alarm1value,alarm2value;
    if (analog.type==0 || analog.type==1 || analog.type==2) {
      ratio=(analog.ratio<<analog.multiplier)/10.0;
    } else {
      ratio=analog.ratio<<analog.multiplier;
    }
    if (ratio!=0) {
      analog.offset = round((255*ui->CalibSB->value()/ratio));
      calib=ratio*analog.offset/255.0;
      alarm1value=ui->alarm1ValueSB->value();
      alarm2value=ui->alarm2ValueSB->value();
      if (alarm1value<calib) {
        alarm1value=calib;
      } else if (alarm1value>(ratio+calib)) {
        alarm1value=ratio+calib;
      }
      if (alarm2value<calib) {
        alarm2value=calib;
      } else if (alarm2value>(ratio+calib)) {
        alarm2value=ratio+calib;
      }
      analog.alarms[0].value=round(((alarm1value-calib)*255)/ratio);
      analog.alarms[1].value=round(((alarm2value-calib)*255)/ratio);
    } else {
      analog.offset=0;
      analog.alarms[0].value=0;
      analog.alarms[1].value=0;
    }
    update();
    // TODO ? telBarUpdate();
    emit modified();
}

void TelemetryAnalog::on_alarm1LevelCB_currentIndexChanged(int index)
{
    analog.alarms[0].level = index;
    emit modified();
}


void TelemetryAnalog::on_alarm1GreaterCB_currentIndexChanged(int index)
{
    analog.alarms[0].greater = index;
    emit modified();
}

void TelemetryAnalog::on_alarm1ValueSB_editingFinished()
{
    float ratio, calib, alarm1value;
    ratio=analog.ratio<<analog.multiplier;
    calib=analog.offset;
    alarm1value=ui->alarm1ValueSB->value();
    if (analog.type==0) {
      ratio/=10;
    }
    if (alarm1value<((calib*ratio)/255)) {
      analog.alarms[0].value=0;
    } else if (alarm1value>(ratio+(calib*ratio)/255)) {
      analog.alarms[0].value=255;
    } else {
      analog.alarms[0].value = round((alarm1value-((calib*ratio)/255))/ratio*255);
    }
    update();
    emit modified();
}

void TelemetryAnalog::on_alarm2LevelCB_currentIndexChanged(int index)
{
    analog.alarms[1].level = index;
    emit modified();
}

void TelemetryAnalog::on_alarm2GreaterCB_currentIndexChanged(int index)
{
    analog.alarms[1].greater = index;
    emit modified();
}

void TelemetryAnalog::on_alarm2ValueSB_editingFinished()
{
    float ratio, calib, alarm2value;
    ratio = analog.ratio<<analog.multiplier;
    calib = analog.offset;
    alarm2value = ui->alarm2ValueSB->value();
    if (analog.type==0) {
      ratio/=10;
    }
    if (alarm2value<((calib*ratio)/255)) {
      analog.alarms[1].value=0;
    } else if (alarm2value>(ratio+(calib*ratio)/255)) {
      analog.alarms[1].value=255;
    } else {
      analog.alarms[1].value = round((alarm2value-((calib*ratio)/255))/ratio*255);
    }
    update();
    emit modified();
}

TelemetryAnalog::~TelemetryAnalog()
{
  delete ui;
}

/******************************************************/

TelemetryPanel::TelemetryPanel(QWidget *parent, ModelData & model):
  ModelPanel(parent, model),
  ui(new Ui::Telemetry)
{
  ui->setupUi(this);

  analogs[0] = new TelemetryAnalog(this, model.frsky.channels[0]);
  ui->A1Layout->addWidget(analogs[0]);
  connect(analogs[0], SIGNAL(modified()), this, SLOT(onAnalogModified()));
  analogs[1] = new TelemetryAnalog(this, model.frsky.channels[1]);
  ui->A2Layout->addWidget(analogs[1]);
  connect(analogs[1], SIGNAL(modified()), this, SLOT(onAnalogModified()));

  QGroupBox* barsgb[3] = { ui->CS1Bars, ui->CS2Bars, ui->CS3Bars };
  QGroupBox* numsgb[3] = { ui->CS1Nums, ui->CS2Nums, ui->CS3Nums };

  QComboBox* barscb[12] = { ui->telBarCS1B1_CB, ui->telBarCS1B2_CB,  ui->telBarCS1B3_CB,  ui->telBarCS1B4_CB,
                            ui->telBarCS2B1_CB, ui->telBarCS2B2_CB,  ui->telBarCS2B3_CB,  ui->telBarCS2B4_CB,
                            ui->telBarCS3B1_CB, ui->telBarCS3B2_CB,  ui->telBarCS3B3_CB,  ui->telBarCS3B4_CB};
  QDoubleSpinBox* minSB[12] = { ui->telMinCS1SB1,  ui->telMinCS1SB2,  ui->telMinCS1SB3,  ui->telMinCS1SB4,
                                ui->telMinCS2SB1,  ui->telMinCS2SB2,  ui->telMinCS2SB3,  ui->telMinCS2SB4,
                                ui->telMinCS3SB1,  ui->telMinCS3SB2,  ui->telMinCS3SB3,  ui->telMinCS3SB4};
  QDoubleSpinBox* maxSB[12] = { ui->telMaxCS1SB1,  ui->telMaxCS1SB2,  ui->telMaxCS1SB3,  ui->telMaxCS1SB4,
                                ui->telMaxCS2SB1,  ui->telMaxCS2SB2,  ui->telMaxCS2SB3,  ui->telMaxCS2SB4,
                                ui->telMaxCS3SB1,  ui->telMaxCS3SB2,  ui->telMaxCS3SB3,  ui->telMaxCS3SB4};
  QComboBox* tmp[36] = { ui->telemetryCS1F1_CB, ui->telemetryCS1F2_CB, ui->telemetryCS1F3_CB, ui->telemetryCS1F4_CB, ui->telemetryCS1F5_CB, ui->telemetryCS1F6_CB, ui->telemetryCS1F7_CB, ui->telemetryCS1F8_CB, ui->telemetryCS1F9_CB, ui->telemetryCS1F10_CB, ui->telemetryCS1F11_CB, ui->telemetryCS1F12_CB,
                         ui->telemetryCS2F1_CB, ui->telemetryCS2F2_CB, ui->telemetryCS2F3_CB, ui->telemetryCS2F4_CB, ui->telemetryCS2F5_CB, ui->telemetryCS2F6_CB, ui->telemetryCS2F7_CB, ui->telemetryCS2F8_CB, ui->telemetryCS2F9_CB, ui->telemetryCS2F10_CB, ui->telemetryCS2F11_CB, ui->telemetryCS2F12_CB,
                         ui->telemetryCS3F1_CB, ui->telemetryCS3F2_CB, ui->telemetryCS3F3_CB, ui->telemetryCS3F4_CB, ui->telemetryCS3F5_CB, ui->telemetryCS3F6_CB, ui->telemetryCS3F7_CB, ui->telemetryCS3F8_CB, ui->telemetryCS3F9_CB, ui->telemetryCS3F10_CB, ui->telemetryCS3F11_CB, ui->telemetryCS3F12_CB};

  memcpy(barsGB, barsgb, sizeof(barsGB));
  memcpy(numsGB, numsgb, sizeof(numsGB));
  memcpy(barsCB, barscb, sizeof(barsCB));
  memcpy(this->maxSB, maxSB, sizeof(this->maxSB));
  memcpy(this->minSB, minSB, sizeof(this->minSB));
  memcpy(csf, tmp, sizeof(csf));

  setup();
}

TelemetryPanel::~TelemetryPanel()
{
  delete ui;
}

void TelemetryPanel::setup()
{
    QSettings settings;
    QString firmware_id = settings.value("firmware", default_firmware_variant.id).toString();

    lock=true;
    ui->telemetryCSType1->setCurrentIndex(model.frsky.screens[0].type);
    ui->telemetryCSType2->setCurrentIndex(model.frsky.screens[1].type);
    ui->telemetryCSType3->setCurrentIndex(model.frsky.screens[2].type);

    for (int i=0; i<3; i++) {
      bool isNum = (model.frsky.screens[i].type==0);
      barsGB[i]->setVisible(!isNum);
      numsGB[i]->setVisible(isNum);
    }

    connect(ui->telemetryCSType1,SIGNAL(currentIndexChanged(int)),this,SLOT(ScreenTypeCBcurrentIndexChanged(int)));
    connect(ui->telemetryCSType2,SIGNAL(currentIndexChanged(int)),this,SLOT(ScreenTypeCBcurrentIndexChanged(int)));
    connect(ui->telemetryCSType3,SIGNAL(currentIndexChanged(int)),this,SLOT(ScreenTypeCBcurrentIndexChanged(int)));

    //frsky Settings
    if (!GetEepromInterface()->getCapability(TelemetryRSSIModel) ) {
      ui->RSSIGB->hide();
    }
    ui->rssiAlarm1SB->setValue(model.frsky.rssiAlarms[0].value);
    ui->rssiAlarm2SB->setValue(model.frsky.rssiAlarms[1].value);
    ui->rssiAlarm1CB->setCurrentIndex(model.frsky.rssiAlarms[0].level);
    ui->rssiAlarm2CB->setCurrentIndex(model.frsky.rssiAlarms[1].level);

    if (!GetEepromInterface()->getCapability(HasAltitudeSel)) {
      ui->AltitudeGPS_ChkB->hide();
    }
    else {
      ui->AltitudeGPS_ChkB->setChecked(model.frsky.FrSkyGpsAlt);
    }
    
    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      ui->AltitudeToolbar_ChkB->setChecked(model.frsky.altitudeDisplayed);
    }
    else {
      ui->AltitudeToolbar_ChkB->hide();
    }

    int varioCap = GetEepromInterface()->getCapability(HasVario);
    if (!varioCap) {
      ui->varioLimitMax_DSB->hide();
      ui->varioLimitMinOff_ChkB->hide();
      ui->varioLimitMin_DSB->hide();
      ui->varioLimitCenterMin_DSB->hide();
      ui->varioLimitCenterMax_DSB->hide();
      ui->varioLimit_label->hide();
      ui->VarioLabel_1->hide();
      ui->VarioLabel_2->hide();
      ui->VarioLabel_3->hide();
      ui->VarioLabel_4->hide();
      ui->varioSourceCB->hide();
      ui->varioSource_label->hide();
    } else {
      if (!GetEepromInterface()->getCapability(HasVarioSink)) {
        ui->varioLimitMinOff_ChkB->hide();
        ui->varioLimitMin_DSB->hide();
        ui->varioLimitCenterMin_DSB->hide();
        ui->VarioLabel_1->hide();
        ui->VarioLabel_2->hide();
      }
      ui->varioLimitMin_DSB->setValue(model.frsky.varioMin-10);
      ui->varioLimitMax_DSB->setValue(model.frsky.varioMax+10);
      ui->varioLimitCenterMax_DSB->setValue((model.frsky.varioCenterMax/10.0)+0.5);
      ui->varioSourceCB->setCurrentIndex(model.frsky.varioSource);
      if (model.frsky.varioCenterMin==-16) {
        ui->varioLimitMinOff_ChkB->setChecked(true);
        ui->varioLimitCenterMin_DSB->setValue(-2.0);
        ui->varioLimitCenterMin_DSB->setDisabled(true);
      } else {
        ui->varioLimitMinOff_ChkB->setChecked(false);
        ui->varioLimitCenterMin_DSB->setValue((model.frsky.varioCenterMin/10.0)-0.5);
      }
      int mask=1;
      for (int i=0; i< ui->varioSourceCB->count(); i++) {
        if (!(varioCap&mask)) {
          QModelIndex index = ui->varioSourceCB->model()->index(i, 0);
          QVariant v(0);
          ui->varioSourceCB->model()->setData(index, v, Qt::UserRole - 1);
        }
        mask <<=1;
      }
    }

    if (!(GetEepromInterface()->getCapability(HasAltitudeSel)||GetEepromInterface()->getCapability(HasVario))) {
      ui->altimetryGB->hide();
    }
    if (GetEepromInterface()->getCapability(NoTelemetryProtocol)) {
      model.frsky.usrProto=1;
      ui->frskyProtoCB->setDisabled(true);
    } else {
      ui->frskyProtoCB->setEnabled(true);
    }

    if (GetEepromInterface()->getCapability(TelemetryCSFields)==0) {
      ui->groupBox_5->hide();
    } else {
      if (GetEepromInterface()->getCapability(TelemetryCSFields)==8) {
        ui->tabCsView->removeTab(1);
        ui->tabCsView->removeTab(2);
      }
      if (GetEepromInterface()->getCapability(TelemetryCSFields)==16) {
        ui->tabCsView->removeTab(2);
      }
      int cols=GetEepromInterface()->getCapability(TelemetryColsCSFields);
      if (cols==0) cols=2;
      for (int screen=0; screen<(GetEepromInterface()->getCapability(TelemetryCSFields)/(4*cols)); screen++) {
        for (int c=0; c<cols; c++) {
          for (int r=0; r<4; r++) {
            int index=screen*12+c*4+r;
            if (model.frsky.screens[screen].type==0) {
              populateCustomScreenFieldCB(csf[index], model.frsky.screens[screen].body.lines[r].source[c], (r<4), model.frsky.usrProto);
            } else {
              populateCustomScreenFieldCB(csf[index], 0, (r<4), model.frsky.usrProto);
            }
            connect(csf[index], SIGNAL(currentIndexChanged(int)), this, SLOT(customFieldEdited()));
          }
        }
        for (int c=cols; c<3; c++) {
          for (int r=0; r<4; r++) {
            int index=screen*12+c*4+r;
            csf[index]->hide();
          }
        }
      }
    }

    if (!GetEepromInterface()->getCapability(TelemetryUnits)) {
      ui->frskyUnitsCB->setDisabled(true);
      int index=0;
      if (firmware_id.contains("imperial")) {
        index=1;
      }
      ui->frskyUnitsCB->setCurrentIndex(index);
    }
    if ((GetEepromInterface()->getCapability(Telemetry)&TM_HASWSHH)) {
      ui->frskyProtoCB->addItem(tr("Winged Shadow How High"));
    } else {
      ui->frskyProtoCB->addItem(tr("Winged Shadow How High (not supported)"));
    }
    
    ui->variousGB->hide();
    if (!(GetEepromInterface()->getCapability(HasFasOffset)) && !(firmware_id.contains("fasoffset"))) {
      ui->fasOffset_label->hide();
      ui->fasOffset_DSB->hide();
    }
    else {
      ui->fasOffset_DSB->setValue(model.frsky.fasOffset/10.0);
      ui->variousGB->show();
    }

    if (!(GetEepromInterface()->getCapability(HasMahPersistent))) {
      ui->mahCount_label->hide();
      ui->mahCount_SB->hide();
      ui->mahCount_ChkB->hide();
    }
    else {
      if (model.frsky.mAhPersistent) {
        ui->mahCount_ChkB->setChecked(true);
        ui->mahCount_SB->setValue(model.frsky.storedMah);
      }
      else {
        ui->mahCount_SB->setDisabled(true);
      }
      ui->variousGB->show();
    }

    ui->frskyProtoCB->setCurrentIndex(model.frsky.usrProto);
    ui->frskyUnitsCB->setCurrentIndex(model.frsky.imperial);
    ui->frskyBladesCB->setCurrentIndex(model.frsky.blades);
    ui->frskyCurrentCB->setCurrentIndex(model.frsky.currentSource);
    ui->frskyVoltCB->setCurrentIndex(model.frsky.voltsSource);

    for (int screen=0; screen<2;screen++) {
      for (int rows=0; rows<4; rows++) {
        for (int cols=0; cols<3; cols++) {
          int index=screen*12+cols*4+rows;
          populateCustomScreenFieldCB(csf[index], model.frsky.screens[screen].body.lines[rows].source[cols], (rows<4), model.frsky.usrProto);
        }
      }
    }

    for (int j=0; j<12; j++) {
      int screen = j/4;
      int field = j%4;
      populateCustomScreenFieldCB(barsCB[j], model.frsky.screens[screen].body.bars[field].source, false, model.frsky.usrProto);
      RawSource source = RawSource(SOURCE_TYPE_TELEMETRY, model.frsky.screens[screen].body.bars[field].source, &model);
      RawSourceRange range = source.getRange();
      minSB[j]->setDecimals(range.decimals);
      minSB[j]->setMinimum(range.min);
      minSB[j]->setMaximum(range.max);
      minSB[j]->setSingleStep(range.step);
      minSB[j]->setValue(range.getValue(model.frsky.screens[screen].body.bars[field].barMin));
      maxSB[j]->setDecimals(range.decimals);
      maxSB[j]->setMinimum(range.min);
      maxSB[j]->setMaximum(range.max);
      maxSB[j]->setSingleStep(range.step);
      maxSB[j]->setValue(range.getValue(255 - model.frsky.screens[screen].body.bars[field].barMax));
      if (model.frsky.screens[screen].body.bars[field].source==0 || model.frsky.screens[screen].type==0) {
        minSB[j]->setDisabled(true);
        maxSB[j]->setDisabled(true);
      }
      connect(barsCB[j], SIGNAL(currentIndexChanged(int)), this, SLOT(telBarCBcurrentIndexChanged(int)));
      connect(maxSB[j], SIGNAL(editingFinished()), this, SLOT(telMaxSBeditingFinished()));
      connect(minSB[j], SIGNAL(editingFinished()), this, SLOT(telMinSBeditingFinished()));
    }

    lock=false;
}

void TelemetryPanel::onAnalogModified()
{
  emit modified();
}

void TelemetryPanel::on_frskyUnitsCB_currentIndexChanged(int index)
{
    model.frsky.imperial=index;
    emit modified();
}

void TelemetryPanel::on_frskyBladesCB_currentIndexChanged(int index)
{
    model.frsky.blades=index;
    emit modified();
}

void TelemetryPanel::on_frskyCurrentCB_currentIndexChanged(int index)
{
    model.frsky.currentSource=index;
    emit modified();
}

void TelemetryPanel::on_frskyVoltCB_currentIndexChanged(int index)
{
    model.frsky.voltsSource=index;
    emit modified();
}

void TelemetryPanel::on_frskyProtoCB_currentIndexChanged(int index)
{
    if (lock) return;
    int bindex[12];
    lock=true;
    for (int i=0; i<12; i++) {
      bindex[i]=barsCB[i]->currentIndex();
      model.frsky.usrProto=index;
      populateCustomScreenFieldCB(barsCB[i], bindex[i], false, model.frsky.usrProto);
    }
    if (!GetEepromInterface()->getCapability(TelemetryCSFields)) {
      ui->groupBox_5->hide();
    } else {
      for (int screen=0; screen<2;screen++) {
        for (int rows=0; rows<4; rows++) {
          for (int cols=0; cols<3; cols++) {
            int index=screen*12+cols*4+rows;
            populateCustomScreenFieldCB(csf[index], model.frsky.screens[screen].body.lines[rows].source[cols], (rows<4), model.frsky.usrProto);
          }
        }
      }
    }
    lock=false;
    if (index==0) {
      for (int i=0; i<12; i++) {
        if (bindex[i]>2) {
          barsCB[i]->setCurrentIndex(0);
        } else {
          barsCB[i]->setCurrentIndex(bindex[i]);
        }
      }
    } else if (index==2) {
      for (int i=0; i<12; i++) {
        if (bindex[i]>3) {
          barsCB[i]->setCurrentIndex(0);
        } else {
          barsCB[i]->setCurrentIndex(bindex[i]);
        }
      }
    } else {
      for (int i=0; i<12; i++) {
        barsCB[i]->setCurrentIndex(bindex[i]);
      }
    }
    emit modified();
}

void TelemetryPanel::on_rssiAlarm1CB_currentIndexChanged(int index) {
    if (lock) return;
    model.frsky.rssiAlarms[0].level=index;
    emit modified();
}

void TelemetryPanel::on_rssiAlarm2CB_currentIndexChanged(int index) {
    if (lock) return;
    model.frsky.rssiAlarms[1].level=index;
    emit modified();
}

void TelemetryPanel::on_rssiAlarm1SB_editingFinished()
{
    if (lock) return;
    model.frsky.rssiAlarms[0].value=(ui->rssiAlarm1SB->value());
    emit modified();
}

void TelemetryPanel::on_rssiAlarm2SB_editingFinished()
{
    if (lock) return;
    model.frsky.rssiAlarms[1].value=(ui->rssiAlarm2SB->value());
    emit modified();
}

void TelemetryPanel::on_AltitudeGPS_ChkB_toggled(bool checked)
{
    if (lock) return;
    model.frsky.FrSkyGpsAlt = checked;
    emit modified();
}

void TelemetryPanel::on_AltitudeToolbar_ChkB_toggled(bool checked)
{
    model.frsky.altitudeDisplayed = checked;
    emit modified();
}

void TelemetryPanel::on_varioSourceCB_currentIndexChanged(int index)
{
    if (lock) return;
    model.frsky.varioSource = index;
    emit modified();
}

void TelemetryPanel::on_varioLimitMin_DSB_editingFinished()
{
    if (lock) return;
    model.frsky.varioMin= round(ui->varioLimitMin_DSB->value()+10);
    emit modified();
}

void TelemetryPanel::on_varioLimitMax_DSB_editingFinished()
{
    if (lock) return;
    model.frsky.varioMax= round(ui->varioLimitMax_DSB->value()-10);
    emit modified();
}

void TelemetryPanel::on_varioLimitCenterMin_DSB_editingFinished()
{
    if (lock) return;
    if (ui->varioLimitCenterMin_DSB->value()>ui->varioLimitCenterMax_DSB->value()) {
      ui->varioLimitCenterMax_DSB->setValue(ui->varioLimitCenterMin_DSB->value());
    }
    model.frsky.varioCenterMin= round((ui->varioLimitCenterMin_DSB->value()+0.5)*10);
    emit modified();
}

void TelemetryPanel::on_varioLimitMinOff_ChkB_toggled(bool checked)
{
    if (lock) return;
    model.frsky.varioCenterMin = -16;
    if (!checked) {
      lock=true;
      ui->varioLimitCenterMin_DSB->setValue(-2.0);
      ui->varioLimitCenterMin_DSB->setEnabled(true);
      lock=false;
    } else {
      ui->varioLimitCenterMin_DSB->setValue(-2.0);
      ui->varioLimitCenterMin_DSB->setDisabled(true);
    }
    emit modified();
}

void TelemetryPanel::on_varioLimitCenterMax_DSB_editingFinished()
{
    if (lock) return;
    if (ui->varioLimitCenterMin_DSB->value()>ui->varioLimitCenterMax_DSB->value()) {
      ui->varioLimitCenterMax_DSB->setValue(ui->varioLimitCenterMin_DSB->value());
    }
    model.frsky.varioCenterMax= round((ui->varioLimitCenterMax_DSB->value()-0.5)*10);
    emit modified();
}

void TelemetryPanel::on_fasOffset_DSB_editingFinished()
{
    model.frsky.fasOffset = ui->fasOffset_DSB->value()*10;
    emit modified();
}

void TelemetryPanel::on_mahCount_SB_editingFinished()
{
    model.frsky.storedMah = ui->mahCount_SB->value();
    emit modified();
}

void TelemetryPanel::on_mahCount_ChkB_toggled(bool checked)
{
    model.frsky.mAhPersistent = checked;
    ui->mahCount_SB->setDisabled(!checked);
    emit modified();
}

void TelemetryPanel::telBarUpdate()
{
    int index;
    lock=true;
    for (int i=0; i<12; i++) {
      int screen=i/4;
      index=barsCB[i]->currentIndex();
      if (index==TELEMETRY_SOURCE_A1 || index==TELEMETRY_SOURCE_A1 || index==TELEMETRY_SOURCE_A1_MIN || index==TELEMETRY_SOURCE_A2_MIN) {
        RawSource source = RawSource(SOURCE_TYPE_TELEMETRY, index, &model);
        RawSourceRange range = source.getRange();
        minSB[i]->setMinimum(range.min);
        minSB[i]->setMaximum(range.max);
        minSB[i]->setSingleStep(range.step);
        maxSB[i]->setMinimum(range.min);
        maxSB[i]->setMaximum(range.max);
        maxSB[i]->setSingleStep(range.step);
        minSB[i]->setValue(range.getValue(model.frsky.screens[screen].body.bars[i%4].barMin));
        maxSB[i]->setValue(range.getValue(255 - model.frsky.screens[screen].body.bars[i%4].barMax));
      }
    }
    lock=false;
}

void TelemetryPanel::ScreenTypeCBcurrentIndexChanged(int index)
{
  if (!lock) {
    lock = true;
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int screen = comboBox->objectName().right(1).toInt() -1;
    model.frsky.screens[screen].type = index;
    for (int i=0; i<3; i++) {
      bool isNum = (model.frsky.screens[i].type==0);
      barsGB[i]->setVisible(!isNum);
      numsGB[i]->setVisible(isNum);
    }
    lock = false;
    emit modified();
  }
}

void TelemetryPanel::telBarCBcurrentIndexChanged(int index)
{
  if (!lock) {
    lock = true;
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int screenId = comboBox->objectName().mid(8,1).toInt() - 1;
    int barId = comboBox->objectName().mid(10,1).toInt() - 1;
    int bar=barId+screenId*4;
    model.frsky.screens[screenId].body.bars[barId].source=index;
    if (index==0) {
      model.frsky.screens[screenId].body.bars[barId].barMin=0;
      model.frsky.screens[screenId].body.bars[barId].barMax=0;
      minSB[bar]->setDisabled(true);
      maxSB[bar]->setDisabled(true);
    }
    else {
      minSB[bar]->setEnabled(true);
      maxSB[bar]->setEnabled(true);
    }
    RawSource source = RawSource(SOURCE_TYPE_TELEMETRY, index-1, &model);
    RawSourceRange range = source.getRange();
    minSB[bar]->setDecimals(range.decimals);
    minSB[bar]->setMinimum(range.min);
    minSB[bar]->setMaximum(range.max);
    minSB[bar]->setSingleStep(range.step);
    maxSB[bar]->setDecimals(range.decimals);
    maxSB[bar]->setMinimum(range.min);
    maxSB[bar]->setMaximum(range.max);
    maxSB[bar]->setSingleStep(range.step);
    minSB[bar]->setValue(range.getValue(model.frsky.screens[screenId].body.bars[barId].barMin));
    maxSB[bar]->setValue(range.getValue(255 - model.frsky.screens[screenId].body.bars[barId].barMax));
    lock = false;
    emit modified();
  }
}

void TelemetryPanel::telMinSBeditingFinished()
{
  if (!lock) {
    lock = true;
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int screenId = spinBox->objectName().mid(8,1).toInt() - 1;
    int barId = spinBox->objectName().right(1).toInt() - 1;
    int minId = barId+screenId*4;
    RawSource source = RawSource(SOURCE_TYPE_TELEMETRY, model.frsky.screens[screenId].body.bars[barId].source, &model);
    RawSourceRange range = source.getRange();
    if (model.frsky.screens[screenId].body.bars[barId].source==TELEMETRY_SOURCE_A1 || model.frsky.screens[screenId].body.bars[barId].source==TELEMETRY_SOURCE_A1_MIN) {
      model.frsky.screens[screenId].body.bars[barId].barMin = round((minSB[minId]->value()-analogs[0]->ui->CalibSB->value())/range.step);
    }
    else if (model.frsky.screens[screenId].body.bars[minId].source==TELEMETRY_SOURCE_A2 || model.frsky.screens[screenId].body.bars[minId].source==TELEMETRY_SOURCE_A2_MIN) {
      model.frsky.screens[screenId].body.bars[barId].barMin = round((minSB[minId]->value()-analogs[1]->ui->CalibSB->value())/range.step);
    }
    else {
     model.frsky.screens[screenId].body.bars[barId].barMin = round((minSB[minId]->value()-range.getValue(0))/range.step);
    }
    spinBox->setValue(range.getValue(model.frsky.screens[screenId].body.bars[barId].barMin));
    if (maxSB[minId]->value()<minSB[minId]->value()) {
      model.frsky.screens[screenId].body.bars[minId].barMax = (255-model.frsky.screens[screenId].body.bars[barId].barMin+1);
      maxSB[minId]->setValue(range.getValue(255 - model.frsky.screens[screenId].body.bars[barId].barMax));
    }
    maxSB[minId]->setMinimum(range.getValue((model.frsky.screens[screenId].body.bars[barId].barMin+1)));

    lock = false;
    emit modified();
  }
}

void TelemetryPanel::telMaxSBeditingFinished()
{
  if (!lock) {
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int screenId = spinBox->objectName().mid(8,1).toInt() - 1;
    int barId = spinBox->objectName().right(1).toInt() - 1;
    lock = true;
    RawSource source = RawSource(SOURCE_TYPE_TELEMETRY, model.frsky.screens[screenId].body.bars[barId].source, &model);
    RawSourceRange range = source.getRange();
    if (model.frsky.screens[screenId].body.bars[barId].source==5) {
      model.frsky.screens[screenId].body.bars[barId].barMax = (255-round((spinBox->value()-analogs[0]->ui->CalibSB->value())/range.step));
    }
    else if (model.frsky.screens[screenId].body.bars[barId].source==6) {
      model.frsky.screens[screenId].body.bars[barId].barMax = (255-round((spinBox->value()-analogs[1]->ui->CalibSB->value())/range.step));
    }
    else {
      model.frsky.screens[screenId].body.bars[barId].barMax = (255-round((spinBox->value()-range.getValue(0))/range.step));
    }
    spinBox->setValue(range.getValue(255-model.frsky.screens[screenId].body.bars[barId].barMax));
    lock = false;
    emit modified();
  }
}

void TelemetryPanel::customFieldEdited()
{
  if (!lock) {
    lock = true;

    int cols = GetEepromInterface()->getCapability(TelemetryColsCSFields);
    if (cols==0) cols=2;

    for (int i=0; i<GetEepromInterface()->getCapability(TelemetryCSFields); i++) {
      int screen=i/(4*cols);
      int r=((i%(4*cols))%4);
      int c=((i%(4*cols))/4);
      if (model.frsky.screens[screen].type==0) {
        model.frsky.screens[screen].body.lines[r].source[c]=csf[i]->currentIndex();
      }
    }

    lock = false;
    emit modified();
  }
}

