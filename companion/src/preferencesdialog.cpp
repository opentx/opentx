#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "mainwindow.h"
#include "eeprominterface.h"
#include "splashlibrary.h"
#include "helpers.h"
#ifdef JOYSTICKS
#include "joystick.h"
#include "joystickdialog.h"
#endif
#include <QDesktopServices>
#include <QtGui>

preferencesDialog::preferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::preferencesDialog),
  updateLock(false)
{
  ui->setupUi(this);


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

  populateLocale();
  initSettings();
  connect(ui->downloadVerCB, SIGNAL(currentIndexChanged(int)), this, SLOT(baseFirmwareChanged()));
  connect(this, SIGNAL(accepted()), this, SLOT(writeValues()));
#ifndef JOYSTICKS
  ui->joystickCB->hide();
  ui->joystickCB->setDisabled(true);
  ui->joystickcalButton->hide();
  ui->joystickChkB->hide();
  ui->label_11->hide();
#endif
  resize(0,0);
}


preferencesDialog::~preferencesDialog()
{
  delete ui;
}

void preferencesDialog::baseFirmwareChanged()
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
      int width=firmware->eepromInterface->getCapability(LCDWidth);
      ui->imageLabel->setFixedWidth(width);
      break;
    }
  }
  firmwareChanged();
}

FirmwareVariant preferencesDialog::getFirmwareVariant()
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

void preferencesDialog::firmwareLangChanged()
{
  firmwareChanged();
}

void preferencesDialog::firmwareOptionChanged(bool state)
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

void preferencesDialog::firmwareChanged()
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
  QSettings settings("companion9x", "companion9x");
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
      ui->FwInfo->setText(tr("The selected firmware has never been downloaded by companion9x."));
        ui->checkFWUpdates->hide();   
    }
    else {
      ui->FwInfo->setText(tr("The selected firmware cannot be downloaded by companion9x."));
      ui->checkFWUpdates->hide();
    }
  }
}

void preferencesDialog::writeValues()
{
  QSettings settings("companion9x", "companion9x");
  if (ui->locale_QB->currentIndex() > 0)
    settings.setValue("locale", ui->locale_QB->itemData(ui->locale_QB->currentIndex()));
  else
    settings.remove("locale");
  
  settings.setValue("default_channel_order", ui->channelorderCB->currentIndex());
  settings.setValue("default_mode", ui->stickmodeCB->currentIndex());
  settings.setValue("cpu_id", ui->CPU_ID_LE->text());
  settings.setValue("startup_check_companion9x", ui->startupCheck_companion9x->isChecked());
  settings.setValue("startup_check_fw", ui->startupCheck_fw->isChecked());
  settings.setValue("rename_firmware_files", ui->renameFirmware->isChecked());
  settings.setValue("wizardEnable", ui->wizardEnable_ChkB->isChecked());
  settings.setValue("show_splash", ui->showSplash->isChecked());
  settings.setValue("simuSW", ui->simuSW->isChecked());
  settings.setValue("history_size", ui->historySize->value());
  settings.setValue("burnFirmware", ui->burnFirmware->isChecked());
  current_firmware_variant = getFirmwareVariant();
  settings.setValue("firmware", current_firmware_variant.id);
  settings.setValue("profileId", ui->ProfSlot_SB->value());
  settings.setValue("backLight", ui->backLightColor->currentIndex());
  settings.setValue("libraryPath", ui->libraryPath->text());
  settings.setValue("sdPath", ui->sdPath->text());
  settings.setValue("embedded_splashes", ui->splashincludeCB->currentIndex());
  if (!ui->SplashFileName->text().isEmpty()) {
    QImage Image = ui->imageLabel->pixmap()->toImage();
    settings.setValue("SplashImage", image2qstring(Image));
    settings.setValue("SplashFileName", ui->SplashFileName->text());
  }
  else {
    settings.setValue("SplashFileName", "");
    settings.setValue("SplashImage", "");
  }
  
  if (ui->joystickChkB ->isChecked() && ui->joystickCB->isEnabled()) {
    settings.setValue("js_support", ui->joystickChkB ->isChecked());  
    settings.setValue("js_ctrl", ui->joystickCB ->currentIndex());
  }
  else {
    settings.remove("js_support");
    settings.remove("js_ctrl");
  }
  settings.setValue("backupEnable", ui->backupEnable->isChecked());
  MainWindow * mw = (MainWindow *)this->parent();
  mw->unloadProfile();
}

void preferencesDialog::populateFirmwareOptions(const FirmwareInfo * firmware)
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

void preferencesDialog::initSettings()
{
  QSettings settings("companion9x", "companion9x");
  int i = ui->locale_QB->findData(settings.value("locale"));
  if (i < 0) i = 0;
  ui->locale_QB->setCurrentIndex(i);
  ui->snapshotClipboardCKB->setChecked(settings.value("snapshot_to_clipboard", false).toBool());
  if (ui->snapshotClipboardCKB->isChecked()) {
    ui->snapshotPath->setDisabled(true);
    ui->snapshotPathButton->setDisabled(true);
  } else {
    QString Path=settings.value("snapshotPath", "").toString();
    if (QDir(Path).exists()) {
      ui->snapshotPath->setText(Path);
      ui->snapshotPath->setReadOnly(true);
      ui->snapshotPathButton->setEnabled(true);
    }
  }
  ui->channelorderCB->setCurrentIndex(settings.value("default_channel_order", 0).toInt());
  ui->stickmodeCB->setCurrentIndex(settings.value("default_mode", 1).toInt());
  ui->startupCheck_companion9x->setChecked(settings.value("startup_check_companion9x", true).toBool());
  ui->renameFirmware->setChecked(settings.value("rename_firmware_files", false).toBool());
  ui->wizardEnable_ChkB->setChecked(settings.value("wizardEnable", true).toBool());
  ui->showSplash->setChecked(settings.value("show_splash", true).toBool());
  ui->historySize->setValue(settings.value("history_size", 10).toInt());
  ui->backLightColor->setCurrentIndex(settings.value("backLight", 0).toInt());
  ui->startupCheck_fw->setChecked(settings.value("startup_check_fw", true).toBool());
  ui->burnFirmware->setChecked(settings.value("burnFirmware", true).toBool());
  ui->simuSW->setChecked(settings.value("simuSW", false).toBool());
  ui->CPU_ID_LE->setText(settings.value("cpu_id", "").toString());
  QString Path=settings.value("libraryPath", "").toString();
  if (QDir(Path).exists()) {
    ui->libraryPath->setText(Path);
  }
  Path=settings.value("sdPath", "").toString();
  if (QDir(Path).exists()) {
    ui->sdPath->setText(Path);
  }
  Path=settings.value("backupPath", "").toString();
  if (!Path.isEmpty()) {
    if (QDir(Path).exists()) {
      ui->backupPath->setText(Path);
      ui->backupEnable->setEnabled(true);
      ui->backupEnable->setChecked(settings.value("backupEnable", true).toBool());
    } else {
      ui->backupEnable->setDisabled(true);
    }
  } else {
      ui->backupEnable->setDisabled(true);
  }
  ui->splashincludeCB->setCurrentIndex(settings.value("embedded_splashes", 0).toInt());
  FirmwareInfo * current_firmware = GetCurrentFirmware();

  // qDebug() << current_firmware->id;

  foreach(FirmwareInfo * firmware, firmwares) {
    ui->downloadVerCB->addItem(firmware->name, firmware->id);
    if (current_firmware == firmware) {
      ui->downloadVerCB->setCurrentIndex(ui->downloadVerCB->count() - 1);
    }
  }
  
  baseFirmwareChanged();
  ui->ProfSlot_SB->setValue(settings.value("profileId", 1).toInt());
  on_ProfSlot_SB_valueChanged();
  QString ImageStr = settings.value("SplashImage", "").toString();
  if (!ImageStr.isEmpty()) {
    QImage Image = qstring2image(ImageStr);
    ui->imageLabel->setPixmap(QPixmap::fromImage(Image.convertToFormat(QImage::Format_Mono)));
    ui->InvertPixels->setEnabled(true);
    QString fileName=settings.value("SplashFileName","").toString();
    if (!fileName.isEmpty()) {
      QFile file(fileName);
      if (!file.exists()) {
        ui->SplashFileName->setText(tr("Image stored in settings"));
      }
      else {
        ui->SplashFileName->setText(fileName);
      }
    }
  }
#ifdef JOYSTICKS
  ui->joystickChkB->setChecked(settings.value("js_support", false).toBool());
  if (ui->joystickChkB->isChecked()) {
    QStringList joystickNames;
    joystickNames << tr("No joysticks found");
    joystick = new Joystick(0,false,0,0);
    ui->joystickcalButton->setDisabled(true);
    ui->joystickCB->setDisabled(true);

    if ( joystick ) {
      if ( joystick->joystickNames.count() > 0 ) {
        joystickNames = joystick->joystickNames;
        ui->joystickCB->setEnabled(true);
        ui->joystickcalButton->setEnabled(true);
      }
      joystick->close();
    }
    ui->joystickCB->clear();
    ui->joystickCB->insertItems(0, joystickNames);
    ui->joystickCB->setCurrentIndex(settings.value("js_ctrl", 0).toInt());
  }
  else {
    ui->joystickCB->clear();
    ui->joystickCB->setDisabled(true);
    ui->joystickcalButton->setDisabled(true);
  }
#endif  
  firmwareChanged();
}

void preferencesDialog::populateLocale()
{
  ui->ProfSave_PB->setEnabled(true);
  ui->locale_QB->clear();
  ui->locale_QB->addItem("System default language", "");
  ui->locale_QB->addItem("English", "en");

  QStringList strl = QApplication::arguments();
  if (!strl.count()) return;

  QDir directory = QDir(":/");
  QStringList files = directory.entryList(QStringList("companion9x_*.qm"), QDir::Files | QDir::NoSymLinks);

  foreach(QString file, files) {
    QLocale loc(file.mid(12, 2));
    ui->locale_QB->addItem(QLocale::languageToString(loc.language()), loc.name());
  }
}

void preferencesDialog::on_fw_dnld_clicked()
{
  MainWindow * mw = (MainWindow *)this->parent();
  FirmwareVariant variant = getFirmwareVariant();
  writeValues();
  if (!variant.firmware->getUrl(variant.id).isNull()) {
    if (ui->burnFirmware->isChecked()) {
      QSettings settings("companion9x", "companion9x");
      current_firmware_variant = getFirmwareVariant();
      settings.setValue("firmware", current_firmware_variant.id);
    }
    mw->downloadLatestFW(current_firmware_variant.firmware, current_firmware_variant.id);
  }
  firmwareChanged();
}

void preferencesDialog::on_voice_dnld_clicked()
{
  ui->ProfSave_PB->setEnabled(true);
  QString url="http://93.51.182.154/voices/";
  FirmwareVariant variant = getFirmwareVariant();
  url.append(QString("%1/%2/").arg(variant.firmware->id).arg(ui->voiceCombo->currentText()));
  QDesktopServices::openUrl(url);
}

void preferencesDialog::on_libraryPathButton_clicked()
{
  ui->ProfSave_PB->setEnabled(true);
  QSettings settings("companion9x", "companion9x");
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your library folder"), settings.value("libraryPath").toString());
  if (!fileName.isEmpty()) {
    settings.setValue("libraryPath", fileName);
    ui->libraryPath->setText(fileName);
  }
}

void preferencesDialog::on_snapshotPathButton_clicked()
{
  QSettings settings("companion9x", "companion9x");
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your snapshot folder"), settings.value("snapshotPath").toString());
  if (!fileName.isEmpty()) {
    settings.setValue("snapshotpath", fileName);
    settings.setValue("snapshot_to_clipboard", false);
    ui->snapshotPath->setText(fileName);
  }
}

void preferencesDialog::on_snapshotClipboardCKB_clicked()
{
  QSettings settings("companion9x", "companion9x");
  if (ui->snapshotClipboardCKB->isChecked()) {
    ui->snapshotPath->setDisabled(true);
    ui->snapshotPathButton->setDisabled(true);
    settings.setValue("snapshot_to_clipboard", true);
  } else {
    ui->snapshotPath->setEnabled(true);
    ui->snapshotPath->setReadOnly(true);
    ui->snapshotPathButton->setEnabled(true);
    settings.setValue("snapshot_to_clipboard", false);
  }
}


void preferencesDialog::on_backupPathButton_clicked()
{
  QSettings settings("companion9x", "companion9x");
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your eeprom backup folder"), settings.value("backupPath").toString());
  if (!fileName.isEmpty()) {
    settings.setValue("backupPath", fileName);
    ui->backupPath->setText(fileName);
  }
  ui->backupEnable->setEnabled(true);
}

void preferencesDialog::on_splashLibraryButton_clicked()
{
  QString fileName;
  splashLibrary *ld = new splashLibrary(this,&fileName);
  ld->exec();
  if (!fileName.isEmpty()) {
    QImage image(fileName);
    if (image.isNull()) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot load %1.").arg(fileName));
      return;
    }
    ui->SplashFileName->setText(fileName);
    ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(128, 64).convertToFormat(QImage::Format_Mono)));
    ui->InvertPixels->setEnabled(true);
  }  
}

void preferencesDialog::on_sdPathButton_clicked()
{
  QSettings settings("companion9x", "companion9x");
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select the folder replicating your SD structure"), settings.value("sdPath").toString());
  if (!fileName.isEmpty()) {
    ui->sdPath->setText(fileName);
  }
  ui->ProfSave_PB->setEnabled(true);
}

void preferencesDialog::on_ProfSlot_SB_valueChanged()
{
  QSettings settings("companion9x", "companion9x");
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

void preferencesDialog::on_ProfSave_PB_clicked()
{
  QSettings settings("companion9x", "companion9x");
  settings.beginGroup("Profiles");
  QString profile=QString("profile%1").arg(ui->ProfSlot_SB->value());
  QString name=ui->ProfName_LE->text();
  if (name.isEmpty()) {
    int ret = QMessageBox::question(this, "companion9x", 
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
    if (ui->imageLabel->pixmap()) {
      QImage Image = ui->imageLabel->pixmap()->toImage();
      settings.setValue("SplashImage", image2qstring(Image));
    } else {
      settings.remove("SplashImage");
    }
    current_firmware_variant = getFirmwareVariant();
    settings.setValue("firmware", current_firmware_variant.id);
    settings.endGroup();
    settings.endGroup();
  }
}

void preferencesDialog::on_export_PB_clicked()
{
    QSettings settings("companion9x", "companion9x");
    QString profile=QString("profile%1").arg(ui->ProfSlot_SB->value());
    QString name=ui->ProfName_LE->text();
    if (!name.isEmpty()) {
      QString fileName = QFileDialog::getSaveFileName(this, tr("Export profile As"), settings.value("lastDir").toString() + "/" +name+".ini" , "*.ini");
      if (fileName.isEmpty())
        return;
      QSettings exportfile(fileName, QSettings::IniFormat);
      settings.beginGroup("Profiles");
      settings.beginGroup(profile);
      QStringList keys = settings.childKeys();
      foreach (QString key, keys) {
        exportfile.setValue(key,settings.value(key));
      }
      settings.endGroup();      
    }
    settings.endGroup();
}

void preferencesDialog::on_import_PB_clicked()
{
    QSettings settings("companion9x", "companion9x");
    QString profile=QString("profile%1").arg(ui->ProfSlot_SB->value());
    QString name=ui->ProfName_LE->text();
    if (!name.isEmpty()) {
      int ret = QMessageBox::question(this, "companion9x", 
                  tr("Profile slot is not empty, profile slot %1 will we overwritten.<br>Are you sure ?").arg(ui->ProfSlot_SB->value()) ,
                  QMessageBox::Yes | QMessageBox::No);
      if (ret==QMessageBox::No) {
        return;
      }
    }
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Profile to import"), settings.value("lastDir").toString(), "*.ini");
    if (fileName.isEmpty())
      return;
    QSettings importfile(fileName, QSettings::IniFormat);
    settings.beginGroup("Profiles");
    settings.beginGroup(profile);
    QStringList keys = importfile.childKeys();
    foreach (QString key, keys) {
      settings.setValue(key,importfile.value(key));
    }
    settings.endGroup();
    settings.endGroup();
    on_ProfSlot_SB_valueChanged();
}

void preferencesDialog::on_SplashSelect_clicked()
{
  QString supportedImageFormats;
  for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
    supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
  }

  QSettings settings("companion9x", "companion9x");
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
    int width=ui->imageLabel->width();
    ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(width, 64)));
    if (width==212) {
      image=image.convertToFormat(QImage::Format_RGB32);
      QRgb col;
      int gray;
      int height = image.height();
      for (int i = 0; i < width; ++i)
      {
          for (int j = 0; j < height; ++j)
          {
              col = image.pixel(i, j);
              gray = qGray(col);
              image.setPixel(i, j, qRgb(gray, gray, gray));
          }
      }      
      ui->imageLabel->setPixmap(QPixmap::fromImage(image));
    } else {
      ui->imageLabel->setPixmap(QPixmap::fromImage(image.convertToFormat(QImage::Format_Mono)));
    }
    ui->InvertPixels->setEnabled(true);
  }
}

void preferencesDialog::on_InvertPixels_clicked() {
  QImage image = ui->imageLabel->pixmap()->toImage();
  image.invertPixels();
  ui->imageLabel->setPixmap(QPixmap::fromImage(image));
}

void preferencesDialog::on_clearImageButton_clicked() {
  ui->imageLabel->clear();
  ui->SplashFileName->clear();
}


#ifdef JOYSTICKS

void preferencesDialog::on_joystickChkB_clicked() {
  if (ui->joystickChkB->isChecked()) {
    QStringList joystickNames;
    joystickNames << tr("No joysticks found");
    joystick = new Joystick(0,false,0,0);
    ui->joystickcalButton->setDisabled(true);
    ui->joystickCB->setDisabled(true);

    if ( joystick ) {
      if ( joystick->joystickNames.count() > 0 ) {
        joystickNames = joystick->joystickNames;
        ui->joystickCB->setEnabled(true);
        ui->joystickcalButton->setEnabled(true);
      }
      joystick->close();
    }
    ui->joystickCB->clear();
    ui->joystickCB->insertItems(0, joystickNames);
  }
  else {
    ui->joystickCB->clear();
    ui->joystickCB->setDisabled(true);
    ui->joystickcalButton->setDisabled(true);
  }
}

void preferencesDialog::on_joystickcalButton_clicked() {
   //QSettings settings("companion9x", "companion9x");
   //settings.setValue("joystick-name",ui->joystickCB->currentText());
   joystickDialog * jd=new joystickDialog(this, ui->joystickCB->currentIndex());
   jd->exec();
}

#endif

void preferencesDialog::on_checkFWUpdates_clicked()
{
    FirmwareVariant variant = getFirmwareVariant();
    if (ui->burnFirmware->isChecked()) {
      QSettings settings("companion9x", "companion9x");
      current_firmware_variant = variant;
      settings.setValue("firmware", variant.id);
    }
    MainWindow * mw = (MainWindow *)this->parent();
    mw->checkForUpdates(true, variant.id);
    firmwareChanged();
}

void preferencesDialog::shrink()
{
    resize(0,0);
}

