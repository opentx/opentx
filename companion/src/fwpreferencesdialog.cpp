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
  setWindowIcon(CompanionIcon("preferences.png"));

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

  resize(0,0);
}


fwPreferencesDialog::~fwPreferencesDialog()
{
  delete ui;
}

void fwPreferencesDialog::baseFirmwareChanged()
{
  QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());
  voice=NULL;
  foreach(FirmwareInfo * firmware, firmwares) {
    if (firmware->id == selected_firmware) {
      if (firmware->voice) {
        ui->voiceLabel->show();
        ui->voiceCombo->show();
        ui->voice_dnld->show();
        ui->sdPathButton->show();
        ui->sdPath->show();
        ui->sdPathLabel->show();
        ui->voiceLabel->setEnabled(true);
        ui->voiceCombo->setEnabled(true);
        ui->voice_dnld->setEnabled(true);
        ui->sdPathButton->setEnabled(true);
        ui->sdPath->setEnabled(true);
      } else {
        ui->voiceLabel->hide();
        ui->voiceCombo->hide();
        ui->voice_dnld->hide();        
        ui->sdPathButton->hide();
        ui->sdPath->hide();
        ui->sdPathLabel->hide();
      }
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

void fwPreferencesDialog::firmwareLangChanged()
{
  firmwareChanged();
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
                if (voice->isChecked()) {
                  ui->voiceLabel->setEnabled(true);
                  ui->voiceCombo->setEnabled(true);
                  ui->voice_dnld->setEnabled(true);
                  ui->sdPathButton->setEnabled(true);
                  ui->sdPath->setEnabled(true);
                } else {
                  ui->voiceLabel->setDisabled(true);
                  ui->voiceCombo->setDisabled(true);
                  ui->voice_dnld->setDisabled(true);
                  ui->sdPathButton->setDisabled(true);
                  ui->sdPath->setDisabled(true);
                }
              }
              
              return firmwareChanged();
            }
          }
        }
      }
    }
  } else if (cb && !state) {
    if (cb->text()=="voice") {
      ui->voiceLabel->setDisabled(true);
      ui->voiceCombo->setDisabled(true);
      ui->voice_dnld->setDisabled(true);
      ui->sdPathButton->setEnabled(true);
      ui->sdPath->setEnabled(true);
    }
  }
  if (voice ) {
    if (voice->isChecked()) {
      ui->voiceLabel->setEnabled(true);
      ui->voiceCombo->setEnabled(true);
      ui->voice_dnld->setEnabled(true);
    } else {
      ui->voiceLabel->setDisabled(true);
      ui->voiceCombo->setDisabled(true);
      ui->voice_dnld->setDisabled(true);
      ui->sdPathButton->setDisabled(true);
      ui->sdPath->setDisabled(true);
    }
  }  else if (firmware) {
    if (firmware->voice) {
      ui->voiceLabel->setEnabled(true);
      ui->voiceCombo->setEnabled(true);
      ui->voice_dnld->setEnabled(true);    
      ui->sdPathButton->setEnabled(true);
      ui->sdPath->setEnabled(true);
    }
  }
  return firmwareChanged();
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
  settings.setValue("default_channel_order", ui->channelorderCB->currentIndex());
  settings.setValue("default_mode", ui->stickmodeCB->currentIndex());
  settings.setValue("cpu_id", ui->CPU_ID_LE->text());
  settings.setValue("rename_firmware_files", ui->renameFirmware->isChecked());
  settings.setValue("burnFirmware", ui->burnFirmware->isChecked());
  current_firmware_variant = getFirmwareVariant();
  settings.setValue("firmware", current_firmware_variant.id);
  settings.setValue("profileId", ui->ProfSlot_SB->value());
  settings.setValue("sdPath", ui->sdPath->text());
  settings.setValue("SplashFileName", ui->SplashFileName->text());
  if (!ui->SplashFileName->text().isEmpty())
    settings.setValue("SplashImage", "");

  MainWindow * mw = (MainWindow *)this->parent();
  mw->unloadProfile();
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

  if (ui->langCombo->count()) {
    ui->langCombo->show();
    ui->langLabel->show();
  }
  else {
    ui->langCombo->hide();
    ui->langLabel->hide();
  }

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
            ui->voiceLabel->show();
            ui->voiceCombo->show();
            ui->voice_dnld->show();
            ui->sdPathButton->show();
            ui->sdPath->show();
            ui->sdPathLabel->show();
            if (current_firmware_variant.id.contains(opt.name) ||firmware->voice) {
              ui->voiceLabel->setEnabled(true);
              ui->voiceCombo->setEnabled(true);
              ui->voice_dnld->setEnabled(true);
              ui->sdPathButton->setEnabled(true);
              ui->sdPath->setEnabled(true);
            } else {
              ui->voiceLabel->setDisabled(true);
              ui->voiceCombo->setDisabled(true);
              ui->voice_dnld->setDisabled(true);
              ui->sdPathButton->setDisabled(true);
              ui->sdPath->setDisabled(true);
            }
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
  ui->channelorderCB->setCurrentIndex(settings.value("default_channel_order", 0).toInt());
  ui->stickmodeCB->setCurrentIndex(settings.value("default_mode", 1).toInt());
  ui->renameFirmware->setChecked(settings.value("rename_firmware_files", false).toBool());
  ui->burnFirmware->setChecked(settings.value("burnFirmware", true).toBool());
  ui->CPU_ID_LE->setText(settings.value("cpu_id", "").toString());
  QString Path=settings.value("sdPath", "").toString();
  if (QDir(Path).exists()) {
    ui->sdPath->setText(Path);
  }

  FirmwareInfo * current_firmware = GetCurrentFirmware();

  foreach(FirmwareInfo * firmware, firmwares) {
    ui->downloadVerCB->addItem(firmware->name, firmware->id);
    if (current_firmware == firmware) {
      ui->downloadVerCB->setCurrentIndex(ui->downloadVerCB->count() - 1);
    }
  }
  
  baseFirmwareChanged();

  ui->ProfSlot_SB->setValue(settings.value("profileId", 1).toInt());
  on_ProfSlot_SB_valueChanged();
  QString fileName=settings.value("SplashFileName","").toString();
  if (!fileName.isEmpty()) {
    QFile file(fileName);
    if (file.exists()){ 
      ui->SplashFileName->setText(fileName);
    }
  }
  firmwareChanged();
}

void fwPreferencesDialog::on_fw_dnld_clicked()
{
  MainWindow * mw = (MainWindow *)this->parent();
  FirmwareVariant variant = getFirmwareVariant();
  writeValues();
  if (!variant.firmware->getUrl(variant.id).isNull()) {
    if (ui->burnFirmware->isChecked()) {
      QSettings settings;
      current_firmware_variant = getFirmwareVariant();
      settings.setValue("firmware", current_firmware_variant.id);
    }
    mw->downloadLatestFW(current_firmware_variant.firmware, current_firmware_variant.id);
  }
  firmwareChanged();
}

void fwPreferencesDialog::on_voice_dnld_clicked()
{
  ui->ProfSave_PB->setEnabled(true);
  QString url="http://fw.opentx.it/voices/";
  FirmwareVariant variant = getFirmwareVariant();
  url.append(QString("%1/%2/").arg(variant.firmware->id).arg(ui->voiceCombo->currentText()));
  QDesktopServices::openUrl(url);
}

void fwPreferencesDialog::on_sdPathButton_clicked()
{
  QSettings settings;
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select the folder replicating your SD structure"), settings.value("sdPath").toString());
  if (!fileName.isEmpty()) {
    ui->sdPath->setText(fileName);
  }
  ui->ProfSave_PB->setEnabled(true);
}

void fwPreferencesDialog::on_ProfSlot_SB_valueChanged()
{
  QSettings settings;
  settings.beginGroup("Profiles");
  QString profile=QString("profile%1").arg(ui->ProfSlot_SB->value());
  settings.beginGroup(profile);
  QString name=settings.value("Name","").toString();
  ui->ProfName_LE->setText(name);
/*  if (!(name.isEmpty())) {
    QString firmwarename=settings.value("firmware", default_firmware_id).toString();
    FirmwareInfo * fw = getFirmware(firmwarename);
    int i=0;
    foreach(FirmwareInfo * firmware, firmwares) {
      if (fw == firmware) {
        qDebug() << fw->id;
        qDebug() << firmware->id;
        qDebug() << i;
        ui->downloadVerCB->setCurrentIndex(i);
        break;
      }
      i++;
    }
    baseFirmwareChanged();
    populateFirmwareOptions(fw);
  }*/
  settings.endGroup();
  settings.endGroup();
}

void fwPreferencesDialog::on_ProfSave_PB_clicked()
{
  QSettings settings;
  settings.beginGroup("Profiles");
  QString profile=QString("profile%1").arg(ui->ProfSlot_SB->value());
  QString name=ui->ProfName_LE->text();
  if (name.isEmpty()) {
    int ret = QMessageBox::question(this, "Companion", 
                tr("Profile name is empty, profile slot %1 will be deleted.<br>Are you sure ?").arg(ui->ProfSlot_SB->value()) ,
                QMessageBox::Yes | QMessageBox::No);
    if (ret==QMessageBox::Yes) {
      settings.remove(profile);
    } else {
      settings.beginGroup(profile);
      ui->ProfName_LE->setText(settings.value("Name","").toString());
    }
  } else {
    settings.beginGroup(profile);
    settings.setValue("Name",name);
    settings.setValue("default_channel_order", ui->channelorderCB->currentIndex());
    settings.setValue("default_mode", ui->stickmodeCB->currentIndex());
    settings.setValue("burnFirmware", ui->burnFirmware->isChecked());
    settings.setValue("rename_firmware_files", ui->renameFirmware->isChecked());
    settings.setValue("sdPath", ui->sdPath->text());
    settings.setValue("SplashFileName", ui->SplashFileName->text());
    current_firmware_variant = getFirmwareVariant();
    settings.setValue("firmware", current_firmware_variant.id);
    settings.endGroup();
    settings.endGroup();
  }
}

void fwPreferencesDialog::on_SplashSelect_clicked()
{
  QString supportedImageFormats;
  for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
    supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
  }

  QSettings settings;
  QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image to load"), settings.value("lastImagesDir").toString(), tr("Images (%1)").arg(supportedImageFormats));

  if (!fileName.isEmpty()) {
    settings.setValue("lastImagesDir", QFileInfo(fileName).dir().absolutePath());
    QImage image(fileName);
    if (image.isNull()) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot load %1.").arg(fileName));
      return;
    }
    ui->SplashFileName->setText(fileName);
  }
}


void fwPreferencesDialog::on_clearImageButton_clicked() {
  ui->SplashFileName->clear();
}

void fwPreferencesDialog::on_checkFWUpdates_clicked()
{
    FirmwareVariant variant = getFirmwareVariant();
    if (ui->burnFirmware->isChecked()) {
      QSettings settings;
      current_firmware_variant = variant;
      settings.setValue("firmware", variant.id);
    }
    MainWindow * mw = (MainWindow *)this->parent();
    mw->checkForUpdates(true, variant.id);
    firmwareChanged();
}

void fwPreferencesDialog::shrink()
{
    resize(0,0);
}

