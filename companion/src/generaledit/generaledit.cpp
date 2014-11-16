#include <QDateTime>
#include <QtGui>
#include <QScrollArea>
#include "generaledit.h"
#include "ui_generaledit.h"
#include "helpers.h"
#include "appdata.h"
#include "generalsetup.h"
#include "trainer.h"
#include "calibration.h"
#include "../modeledit/customfunctions.h"
#include "verticalscrollarea.h"

GeneralEdit::GeneralEdit(QWidget * parent, RadioData & radioData, FirmwareInterface * firmware) :
  QDialog(parent),
  ui(new Ui::GeneralEdit),
  generalSettings(radioData.generalSettings),
  firmware(firmware)
{
  ui->setupUi(this);
  this->setWindowIcon(CompanionIcon("open.png"));

  QString firmware_id = g.profile[g.id()].fwType();
  ui->tabWidget->setCurrentIndex( g.generalEditTab() );
  QString name=g.profile[g.id()].name();
  if (name.isEmpty()) {
    ui->calstore_PB->setDisabled(true);
  }

  ui->profile_CB->clear();
  for (int i=0; i<MAX_PROFILES; ++i) {
    QString name=g.profile[i].name();
    if (!name.isEmpty()) {
      ui->profile_CB->addItem(name, i);
      if (i==g.id()) {
        ui->profile_CB->setCurrentIndex(ui->profile_CB->count()-1);
      }
    }
  }

  addTab(new GeneralSetupPanel(this, generalSettings, firmware), tr("Setup"));
  if (IS_ARM(firmware->getBoard())) {
    addTab(new CustomFunctionsPanel(this, NULL, generalSettings, firmware), tr("Global Functions"));
  }
  addTab(new TrainerPanel(this, generalSettings, firmware), tr("Trainer"));
  addTab(new CalibrationPanel(this, generalSettings, firmware), tr("Calibration"));
}

GeneralEdit::~GeneralEdit()
{
  delete ui;
}

void GeneralEdit::addTab(GenericPanel *panel, QString text)
{
  panels << panel;
  QWidget * widget = new QWidget(ui->tabWidget);
  QVBoxLayout *baseLayout = new QVBoxLayout(widget);
  VerticalScrollArea * area = new VerticalScrollArea(widget, panel);
  baseLayout->addWidget(area);
  ui->tabWidget->addTab(widget, text);
  connect(panel, SIGNAL(modified()), this, SLOT(onTabModified()));
}

void GeneralEdit::onTabModified()
{
  emit modified();
}

void GeneralEdit::on_tabWidget_currentChanged(int index)
{
  panels[index]->update();
  g.generalEditTab(index);
}

void GeneralEdit::on_calretrieve_PB_clicked()
{
  int profile_id=ui->profile_CB->itemData(ui->profile_CB->currentIndex()).toInt();
  QString calib=g.profile[profile_id].stickPotCalib();
  int potsnum=GetCurrentFirmware()->getCapability(Pots);
  if (calib.isEmpty()) {
    return;
  }
  else {
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
          generalSettings.calibMid[i]=byte16;
        Byte=calib.mid(4+i*12,4);
        byte16=(int16_t)Byte.toInt(&ok,16);
        if (ok)
          generalSettings.calibSpanNeg[i]=byte16;
        Byte=calib.mid(8+i*12,4);
        byte16=(int16_t)Byte.toInt(&ok,16);
        if (ok)
          generalSettings.calibSpanPos[i]=byte16;
      }
      for (int i=0; i<4; i++) {
        Byte=trainercalib.mid(i*4,4);
        byte16=(int16_t)Byte.toInt(&ok,16);
        if (ok)
          generalSettings.trainer.calib[i]=byte16;
      }
      generalSettings.currentCalib=currentCalib;
      generalSettings.vBatCalib=vBatCalib;
      generalSettings.vBatWarn=vBatWarn;
      generalSettings.PPM_Multiplier=PPM_Multiplier;
    } else {
      QMessageBox::critical(this, tr("Warning"), tr("Wrong data in profile, radio calibration was not retrieved"));
    }
    if ((DisplaySet.length()==6) && (BeeperSet.length()==4) && (HapticSet.length()==6) && (SpeakerSet.length()==6)) {
      generalSettings.stickMode=GSStickMode;
      uint8_t byte8u;
      int8_t byte8;
      QString chars;
      bool ok;
      byte8=(int8_t)DisplaySet.mid(0,2).toInt(&ok,16);
      if (ok)
        generalSettings.optrexDisplay=(byte8==1 ? true : false);
      byte8u=(uint8_t)DisplaySet.mid(2,2).toUInt(&ok,16);
      if (ok)
        generalSettings.contrast=byte8u;
      byte8u=(uint8_t)DisplaySet.mid(4,2).toUInt(&ok,16);
      if (ok)
        generalSettings.backlightBright=byte8u;
      byte8=(int8_t)BeeperSet.mid(0,2).toUInt(&ok,16);
      if (ok)
        generalSettings.beeperMode = (GeneralSettings::BeeperMode)byte8;
      byte8=(int8_t)BeeperSet.mid(2,2).toInt(&ok,16);
      if (ok)
        generalSettings.beeperLength=byte8;
      byte8=(int8_t)HapticSet.mid(0,2).toUInt(&ok,16);
      if (ok)
        generalSettings.hapticMode=(GeneralSettings::BeeperMode)byte8;
      byte8=(int8_t)HapticSet.mid(2,2).toInt(&ok,16);
      if (ok)
        generalSettings.hapticStrength=byte8;
      byte8=(int8_t)HapticSet.mid(4,2).toInt(&ok,16);
      if (ok)
        generalSettings.hapticLength=byte8;
      byte8u=(uint8_t)SpeakerSet.mid(0,2).toUInt(&ok,16);
      if (ok)
        generalSettings.speakerMode=byte8u;
      byte8u=(uint8_t)SpeakerSet.mid(2,2).toUInt(&ok,16);
      if (ok)
        generalSettings.speakerPitch=byte8u;
      byte8u=(uint8_t)SpeakerSet.mid(4,2).toUInt(&ok,16);
      if (ok)
        generalSettings.speakerVolume=byte8u;
      if (CountrySet.length()==6) {
        byte8u=(uint8_t)CountrySet.mid(0,2).toUInt(&ok,16);
        if (ok)
          generalSettings.countryCode=byte8u;
        byte8u=(uint8_t)CountrySet.mid(2,2).toUInt(&ok,16);
        if (ok)
          generalSettings.imperial=byte8u;
        chars=CountrySet.mid(4,2);
        generalSettings.ttsLanguage[0]=chars[0].toAscii();
        generalSettings.ttsLanguage[1]=chars[1].toAscii();
      }
    }
    else {
      QMessageBox::critical(this, tr("Warning"), tr("Wrong data in profile, hw related parameters were not retrieved"));
    }
  }

  emit modified();
}

void GeneralEdit::on_calstore_PB_clicked()
{
  int profile_id=ui->profile_CB->itemData(ui->profile_CB->currentIndex()).toInt();

  QString name=g.profile[profile_id].name();
  int potsnum=GetCurrentFirmware()->getCapability(Pots);
  if (name.isEmpty()) {
    ui->calstore_PB->setDisabled(true);
    return;
  }
  else {
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
      calib.append(QString("%1").arg((uint16_t)generalSettings.calibMid[i], 4, 16, QChar('0')));
      calib.append(QString("%1").arg((uint16_t)generalSettings.calibSpanNeg[i], 4, 16, QChar('0')));
      calib.append(QString("%1").arg((uint16_t)generalSettings.calibSpanPos[i], 4, 16, QChar('0')));
    }
    g.profile[profile_id].stickPotCalib( calib );
    calib.clear();
    for (int i=0; i< 4; i++) {
      calib.append(QString("%1").arg((uint16_t)generalSettings.trainer.calib[i], 4, 16, QChar('0')));
    }
    g.profile[profile_id].trainerCalib( calib );
    g.profile[profile_id].vBatCalib( generalSettings.vBatCalib );
    g.profile[profile_id].currentCalib( generalSettings.currentCalib );
    g.profile[profile_id].vBatWarn( generalSettings.vBatWarn );
    g.profile[profile_id].ppmMultiplier( generalSettings.PPM_Multiplier );
    g.profile[profile_id].gsStickMode( generalSettings.stickMode );
    g.profile[profile_id].display( QString("%1%2%3").arg((generalSettings.optrexDisplay ? 1:0), 2, 16, QChar('0')).arg((uint8_t)generalSettings.contrast, 2, 16, QChar('0')).arg((uint8_t)generalSettings.backlightBright, 2, 16, QChar('0')) );
    g.profile[profile_id].beeper( QString("%1%2").arg(((uint8_t)generalSettings.beeperMode), 2, 16, QChar('0')).arg((uint8_t)generalSettings.beeperLength, 2, 16, QChar('0')));
    g.profile[profile_id].haptic( QString("%1%2%3").arg(((uint8_t)generalSettings.hapticMode), 2, 16, QChar('0')).arg((int8_t)generalSettings.hapticStrength, 2, 16, QChar('0')).arg((uint8_t)generalSettings.hapticLength, 2, 16, QChar('0')));
    g.profile[profile_id].speaker( QString("%1%2%3").arg((uint8_t)generalSettings.speakerMode, 2, 16, QChar('0')).arg((uint8_t)generalSettings.speakerPitch, 2, 16, QChar('0')).arg((uint8_t)generalSettings.speakerVolume, 2, 16, QChar('0')));
    g.profile[profile_id].countryCode( QString("%1%2%3").arg((uint8_t)generalSettings.countryCode, 2, 16, QChar('0')).arg((uint8_t)generalSettings.imperial, 2, 16, QChar('0')).arg(generalSettings.ttsLanguage));

    QDateTime dateTime = QDateTime::currentDateTime();
    g.profile[profile_id].timeStamp(dateTime.toString("yyyy-MM-dd hh:mm"));
    QMessageBox::information(this, "Companion", tr("Calibration and HW parameters saved."));
  }
}
