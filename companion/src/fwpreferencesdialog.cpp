#include "fwpreferencesdialog.h"
#include "ui_fwpreferencesdialog.h"
#include "mainwindow.h"
#include "eeprominterface.h"
#include "helpers.h"
#include <QDesktopServices>
#include <QtGui>

fwPreferencesDialog::fwPreferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::fwPreferencesDialog),
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


fwPreferencesDialog::~fwPreferencesDialog()
{
  delete ui;
}

void fwPreferencesDialog::showVoice(bool show)
{
  if (show)
    showVoice();
  else
    hideVoice();
}

void fwPreferencesDialog::showVoice()
{
  ui->voiceLine->show(); 
  ui->voiceLabel->show();
  ui->voiceCombo->show();
  ui->voice_dnld->show();
}

void fwPreferencesDialog::hideVoice()
{
  ui->voiceLine->hide(); 
  ui->voiceLabel->hide();
  ui->voiceCombo->hide();
  ui->voice_dnld->hide();  
  QTimer::singleShot(0, this, SLOT(shrink()));
}

void fwPreferencesDialog::shrink()
{
    resize(0,0);
}

void fwPreferencesDialog::baseFirmwareChanged()
{
  QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());
  voice=NULL;
  foreach(FirmwareInfo * firmware, firmwares) {
    if (firmware->id == selected_firmware) {
      showVoice(firmware->voice);
      populateFirmwareOptions(firmware);
      break;
    }
  }
  firmwareChanged();
}

FirmwareVariant fwPreferencesDialog::getFirmwareVariant()
{
  QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());
  bool voice=false;
  foreach(FirmwareInfo * firmware, firmwares) {
    if (firmware->id == selected_firmware) {
      QString id = firmware->id;
      foreach(QCheckBox *cb, optionsCheckBoxes) {
        if (cb->isChecked()) {
          if (cb->text()=="voice" && cb->isChecked())
            voice=true;
          id += QString("-") + cb->text();
        }
      }
      if (! firmware->eepromInterface->getCapability(MultiLangVoice)) {
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

void fwPreferencesDialog::firmwareOptionChanged(bool state)
{
  QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
  FirmwareInfo * firmware=NULL;
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

void fwPreferencesDialog::firmwareLangChanged()
{
  firmwareChanged();
}

void fwPreferencesDialog::firmwareChanged()
{
  if (updateLock)
    return;
  
  FirmwareVariant variant = getFirmwareVariant();
  QString stamp;
  stamp.append(variant.firmware->stamp);
  ui->fw_dnld->setEnabled(!variant.firmware->getUrl(variant.id).isNull());
  QString url=variant.firmware->getUrl(variant.id);
  // B-Plan 
  if (false) {
    ui->CPU_ID_LE->show();
    ui->CPU_ID_LABEL->show();
  } else {
    ui->CPU_ID_LE->hide();
    ui->CPU_ID_LABEL->hide();
  }
  QSettings settings;
  settings.beginGroup("FwRevisions");
  int fwrev = settings.value(variant.id, -1).toInt();
  settings.endGroup();
  if (fwrev != -1) {
    ui->FwInfo->setText(tr("Last downloaded release: %1").arg(fwrev));
    if (!stamp.isEmpty()) {
      ui->checkFWUpdates->show();
    } else {
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

void fwPreferencesDialog::writeValues()
{
  QSettings settings;

  settings.setValue("cpu_id", ui->CPU_ID_LE->text());
  current_firmware_variant = getFirmwareVariant();
  settings.setValue("firmware", current_firmware_variant.id);
}

void fwPreferencesDialog::populateFirmwareOptions(const FirmwareInfo * firmware)
{
  const FirmwareInfo * parent = firmware->parent ? firmware->parent : firmware;

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

void fwPreferencesDialog::initSettings()
{
  QSettings settings;

  ui->CPU_ID_LE->setText(settings.value("cpu_id", "").toString());
  FirmwareInfo * current_firmware = GetCurrentFirmware();

  foreach(FirmwareInfo * firmware, firmwares) {
    ui->downloadVerCB->addItem(firmware->name, firmware->id);
    if (current_firmware == firmware) {
      ui->downloadVerCB->setCurrentIndex(ui->downloadVerCB->count() - 1);
    }
  }
  
  baseFirmwareChanged();
  firmwareChanged();
}

void fwPreferencesDialog::on_checkFWUpdates_clicked()
{
    QSettings settings;

    FirmwareVariant variant = getFirmwareVariant();
    if (settings.value("burnFirmware", true).toBool()) {
      current_firmware_variant = variant;
      settings.setValue("firmware", variant.id);
    }
    MainWindow * mw = (MainWindow *)this->parent();
    mw->checkForUpdates(true, variant.id);
    firmwareChanged();
}

void fwPreferencesDialog::on_fw_dnld_clicked()
{
  QSettings settings;
  MainWindow * mw = (MainWindow *)this->parent();
  FirmwareVariant variant = getFirmwareVariant();
  writeValues();
  if (!variant.firmware->getUrl(variant.id).isNull()) {
    if (settings.value("burnFirmware", true).toBool()) {
      current_firmware_variant = getFirmwareVariant();
      settings.setValue("firmware", current_firmware_variant.id);
    }
    mw->downloadLatestFW(current_firmware_variant.firmware, current_firmware_variant.id);
  }
  firmwareChanged();
}

void fwPreferencesDialog::on_voice_dnld_clicked()
{
  QString url="http://fw.opentx.it/voices/";
  FirmwareVariant variant = getFirmwareVariant();
  url.append(QString("%1/%2/").arg(variant.firmware->id).arg(ui->voiceCombo->currentText()));
  QDesktopServices::openUrl(url);
}


