#include "fwpreferencesdialog.h"
#include "ui_fwpreferencesdialog.h"
#include "mainwindow.h"
#include "eeprominterface.h"
#include "helpers.h"
#include "appdata.h"
#include <QDesktopServices>
#include <QtGui>

FirmwarePreferencesDialog::FirmwarePreferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::FirmwarePreferencesDialog),
  updateLock(false)
{
  ui->setupUi(this);
  setWindowIcon(CompanionIcon("fwpreferences.png"));

  QCheckBox * OptionCheckBox[]= {
      ui->optionCheckBox_1, ui->optionCheckBox_2, ui->optionCheckBox_3, ui->optionCheckBox_4,  ui->optionCheckBox_5, ui->optionCheckBox_6,  ui->optionCheckBox_7,
      ui->optionCheckBox_8, ui->optionCheckBox_9, ui->optionCheckBox_10,  ui->optionCheckBox_11, ui->optionCheckBox_12, ui->optionCheckBox_13, ui->optionCheckBox_14,
      ui->optionCheckBox_15,ui->optionCheckBox_16, ui->optionCheckBox_17, ui->optionCheckBox_18, ui->optionCheckBox_19, ui->optionCheckBox_20, ui->optionCheckBox_21,
      ui->optionCheckBox_22, ui->optionCheckBox_23, ui->optionCheckBox_24, ui->optionCheckBox_25, ui->optionCheckBox_26, ui->optionCheckBox_27, ui->optionCheckBox_28,
      ui->optionCheckBox_29, ui->optionCheckBox_30, ui->optionCheckBox_31, ui->optionCheckBox_32, ui->optionCheckBox_33, ui->optionCheckBox_34, ui->optionCheckBox_35,
      ui->optionCheckBox_36, ui->optionCheckBox_37, ui->optionCheckBox_38, ui->optionCheckBox_39, ui->optionCheckBox_40, ui->optionCheckBox_41, ui->optionCheckBox_42,
      NULL };

  voice=NULL;
  
  connect(ui->langCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(firmwareLangChanged()));
  connect(ui->voiceCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(firmwareLangChanged()));

  for (int i=0; OptionCheckBox[i]; i++) {
    optionsCheckBoxes.push_back(OptionCheckBox[i]);
    connect(OptionCheckBox[i], SIGNAL(toggled(bool)), this, SLOT(firmwareOptionChanged(bool)));
  }

  initSettings();

  connect(ui->downloadVerCB, SIGNAL(currentIndexChanged(int)), this, SLOT(baseFirmwareChanged()));
  connect(this, SIGNAL(accepted()), this, SLOT(writeValues()));

  shrink();
}


FirmwarePreferencesDialog::~FirmwarePreferencesDialog()
{
  delete ui;
}

void FirmwarePreferencesDialog::showVoice(bool show)
{
  if (show)
    showVoice();
  else
    hideVoice();
}

void FirmwarePreferencesDialog::showVoice()
{
  ui->voiceLine->show(); 
  ui->voiceLabel->show();
  ui->voiceCombo->show();
  ui->voice_dnld->show();
}

void FirmwarePreferencesDialog::hideVoice()
{
  ui->voiceLine->hide(); 
  ui->voiceLabel->hide();
  ui->voiceCombo->hide();
  ui->voice_dnld->hide();  
  QTimer::singleShot(0, this, SLOT(shrink()));
}

void FirmwarePreferencesDialog::shrink()
{
    resize(0,0);
}

void FirmwarePreferencesDialog::baseFirmwareChanged()
{
  QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());
  voice=NULL;
  foreach(FirmwareInterface * firmware, firmwares) {
    if (firmware->id == selected_firmware) {
      showVoice(firmware->voice);
      populateFirmwareOptions(firmware);
      break;
    }
  }
  firmwareChanged();
}

FirmwareVariant FirmwarePreferencesDialog::getFirmwareVariant()
{
  QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());
  bool voice=false;
  foreach(FirmwareInterface * firmware, firmwares) {
    if (firmware->id == selected_firmware) {
      QString id = firmware->id;
      foreach(QCheckBox *cb, optionsCheckBoxes) {
        if (cb->isChecked()) {
          if (cb->text()=="voice" && cb->isChecked())
            voice=true;
          id += QString("-") + cb->text();
        }
      }
      if (!firmware->getCapability(MultiLangVoice)) {
        if (ui->voiceCombo->count() && (voice || firmware->voice))
          id += QString("-tts") + ui->voiceCombo->currentText();
      }
      if (ui->langCombo->count())
        id += QString("-") + ui->langCombo->currentText();

      return GetFirmwareVariant(id);
    }
  }

  // Should never occur...
  return default_firmware_variant;
}

void FirmwarePreferencesDialog::firmwareOptionChanged(bool state)
{
  QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
  FirmwareInterface * firmware=NULL;
  if (cb && state) {
    QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());
    foreach(firmware, firmwares) {
      if (firmware->id == selected_firmware) {
        foreach(QList<Option> opts, firmware->opts) {
          foreach(Option opt, opts) {
            if (cb->text() == opt.name) {
              foreach(Option other, opts) {
                if (other.name != opt.name) {
                  foreach(QCheckBox *ocb, optionsCheckBoxes) {
                    if (ocb->text() == other.name)
                      ocb->setChecked(false);
                  }
                }
              }
              if (voice) {
                showVoice(voice->isChecked());
              }
              
              return firmwareChanged();
            }
          }
        }
      }
    }
  } else if (cb && !state) {
    if (cb->text()=="voice") {
      hideVoice();
    }
  }
  if (voice) {
    showVoice(voice->isChecked());
  }  else if (firmware) {
    if (firmware->voice) {
      showVoice();    
    }
  }
  return firmwareChanged();
}

void FirmwarePreferencesDialog::firmwareLangChanged()
{
  firmwareChanged();
}

void FirmwarePreferencesDialog::firmwareChanged()
{
  if (updateLock)
    return;
  
  FirmwareVariant variant = getFirmwareVariant();
  QString stamp = variant.firmware->getStampUrl();
  ui->fw_dnld->setEnabled(!variant.getFirmwareUrl().isNull());
  QString url = variant.getFirmwareUrl();
  // B-Plan 
  if (false) {
    ui->CPU_ID_LE->show();
    ui->CPU_ID_LABEL->show();
  }
  else {
    ui->CPU_ID_LE->hide();
    ui->CPU_ID_LABEL->hide();
  }
  int fwrev = g.fwRev.get(variant.id);
  if (fwrev != 0) {
    ui->FwInfo->setText(tr("Last downloaded release: %1").arg(fwrev));
    if (!stamp.isEmpty()) {
      ui->checkFWUpdates->show();
    }
    else {
      ui->checkFWUpdates->hide();
    }
  }
  else {
    if (ui->fw_dnld->isEnabled()) {
      ui->FwInfo->setText(tr("The selected firmware has never been downloaded by Companion."));
        ui->checkFWUpdates->hide();   
    }
    else {
      ui->FwInfo->setText(tr("The selected firmware cannot be downloaded by Companion."));
      ui->checkFWUpdates->hide();
    }
  }
}

void FirmwarePreferencesDialog::writeValues()
{
  g.cpuId( ui->CPU_ID_LE->text() );
  current_firmware_variant = getFirmwareVariant();
  g.profile[g.id()].fwType( current_firmware_variant.id );
}

void FirmwarePreferencesDialog::populateFirmwareOptions(const FirmwareInterface * firmware)
{
  const FirmwareInterface * parent = /*firmware->parent ? firmware->parent : */firmware;

  updateLock = true;

  ui->langCombo->clear();
  foreach(const char *lang, parent->languages) {
    ui->langCombo->addItem(lang);
    if (current_firmware_variant.id.endsWith(lang))
      ui->langCombo->setCurrentIndex(ui->langCombo->count() - 1);
  }
  ui->voiceCombo->clear();
  foreach(const char *lang, parent->ttslanguages) {
    ui->voiceCombo->addItem(lang);
    if (current_firmware_variant.id.contains(QString("-tts%1").arg(lang)))
      ui->voiceCombo->setCurrentIndex(ui->voiceCombo->count() - 1);
  }

  showVoice(ui->langCombo->count()!=0);

  int index = 0;
  foreach(QList<Option> opts, parent->opts) {
    foreach(Option opt, opts) {
      if (index >= optionsCheckBoxes.size()) {
        qDebug() << "This firmware needs more options checkboxes!";
      }
      else {
        QCheckBox *cb = optionsCheckBoxes.at(index++);
        if (cb) {
          cb->show();
          cb->setText(opt.name);
          cb->setToolTip(opt.tooltip);
          cb->setCheckState(current_firmware_variant.id.contains(opt.name) ? Qt::Checked : Qt::Unchecked);
            
          if (opt.name==QString("voice")) {
            voice=cb;
            showVoice(current_firmware_variant.id.contains(opt.name) ||firmware->voice);
          }
        }
      }
    }
  }
  for (; index<optionsCheckBoxes.size(); index++) {
    QCheckBox *cb = optionsCheckBoxes.at(index);
    cb->hide();
    cb->setCheckState(Qt::Unchecked);
  }

  updateLock = false;
  QTimer::singleShot(0, this, SLOT(shrink()));
}

void FirmwarePreferencesDialog::initSettings()
{
  ui->CPU_ID_LE->setText(g.cpuId());
  FirmwareInterface * current_firmware = GetCurrentFirmware();

  foreach(FirmwareInterface * firmware, firmwares) {
    ui->downloadVerCB->addItem(firmware->name, firmware->id);
    if (current_firmware == firmware) {
      ui->downloadVerCB->setCurrentIndex(ui->downloadVerCB->count() - 1);
    }
  }
  
  baseFirmwareChanged();
  firmwareChanged();
}

void FirmwarePreferencesDialog::on_checkFWUpdates_clicked()
{
    FirmwareVariant variant = getFirmwareVariant();
    if (g.profile[g.id()].burnFirmware()) {
      current_firmware_variant = variant;
      g.profile[g.id()].fwType( variant.id );
    }
    MainWindow * mw = (MainWindow *)this->parent();
    mw->checkForUpdates(true, variant.id);
    firmwareChanged();
}

void FirmwarePreferencesDialog::on_fw_dnld_clicked()
{
  MainWindow * mw = (MainWindow *)this->parent();
  FirmwareVariant variant = getFirmwareVariant();
  writeValues();
  if (!variant.getFirmwareUrl().isNull()) {
    if (g.profile[g.id()].burnFirmware()) {
      current_firmware_variant = getFirmwareVariant();
      g.profile[g.id()].fwType( current_firmware_variant.id );
    }
    mw->downloadLatestFW(current_firmware_variant);
  }
  firmwareChanged();
}

void FirmwarePreferencesDialog::on_voice_dnld_clicked()
{
  QString url="http://fw.opentx.it/voices/";
  FirmwareVariant variant = getFirmwareVariant();
  url.append(QString("%1/%2/").arg(variant.firmware->id).arg(ui->voiceCombo->currentText()));
  QDesktopServices::openUrl(url);
}


