/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "apppreferencesdialog.h"
#include "ui_apppreferencesdialog.h"
#include "mainwindow.h"
#include "appdata.h"
#include "helpers.h"
#include "storage.h"
#if defined(JOYSTICKS)
#include "joystick.h"
#include "joystickdialog.h"
#endif

AppPreferencesDialog::AppPreferencesDialog(QWidget * parent) :
  QDialog(parent),
  updateLock(false),
  mainWinHasDirtyChild(false),
  ui(new Ui::AppPreferencesDialog)
{
  ui->setupUi(this);
  setWindowIcon(CompanionIcon("apppreferences.png"));

  initSettings();
  connect(ui->downloadVerCB, SIGNAL(currentIndexChanged(int)), this, SLOT(baseFirmwareChanged()));
  connect(ui->opt_appDebugLog, &QCheckBox::toggled, this, &AppPreferencesDialog::toggleAppLogSettings);
  connect(ui->opt_fwTraceLog, &QCheckBox::toggled, this, &AppPreferencesDialog::toggleAppLogSettings);

#if !defined(JOYSTICKS)
  ui->joystickCB->hide();
  ui->joystickCB->setDisabled(true);
  ui->joystickcalButton->hide();
  ui->joystickChkB->hide();
  ui->label_11->hide();
#endif

  shrink();
}

AppPreferencesDialog::~AppPreferencesDialog()
{
  delete ui;
}

void AppPreferencesDialog::setMainWinHasDirtyChild(bool value)
{
  mainWinHasDirtyChild = value;
}

void AppPreferencesDialog::accept()
{
  g.autoCheckApp(ui->autoCheckCompanion->isChecked());
  g.OpenTxBranch(DownloadBranchType(ui->OpenTxBranch->currentIndex()));
  g.autoCheckFw(ui->autoCheckFirmware->isChecked());
  g.showSplash(ui->showSplash->isChecked());
  g.simuSW(ui->simuSW->isChecked());
  g.removeModelSlots(ui->opt_removeBlankSlots->isChecked());
  g.newModelAction(ui->opt_newMdl_useWizard->isChecked() ? 1 : ui->opt_newMdl_useEditor->isChecked() ? 2 : 0);
  g.historySize(ui->historySize->value());
  g.backLight(ui->backLightColor->currentIndex());
  g.profile[g.id()].volumeGain(round(ui->volumeGain->value() * 10.0));
  g.libDir(ui->libraryPath->text());
  g.gePath(ui->ge_lineedit->text());
  g.embedSplashes(ui->splashincludeCB->currentIndex());
  g.enableBackup(ui->backupEnable->isChecked());

  g.appDebugLog(ui->opt_appDebugLog->isChecked());
  g.fwTraceLog(ui->opt_fwTraceLog->isChecked());
  g.appLogsDir(ui->appLogsDir->text());

  if (ui->joystickChkB ->isChecked() && ui->joystickCB->isEnabled()) {
    g.jsSupport(ui->joystickChkB ->isChecked());
    g.jsCtrl(ui->joystickCB ->currentIndex());
  }
  else {
    g.jsSupport(false);
    g.jsCtrl(0);
  }
  g.profile[g.id()].channelOrder(ui->channelorderCB->currentIndex());
  g.profile[g.id()].defaultMode(ui->stickmodeCB->currentIndex());
  g.profile[g.id()].renameFwFiles(ui->renameFirmware->isChecked());
  g.profile[g.id()].burnFirmware(ui->burnFirmware->isChecked());
  g.profile[g.id()].sdPath(ui->sdPath->text());
  g.profile[g.id()].pBackupDir(ui->profilebackupPath->text());
  g.profile[g.id()].penableBackup(ui->pbackupEnable->isChecked());
  g.profile[g.id()].splashFile(ui->SplashFileName->text());

  // The profile name may NEVER be empty
  if (ui->profileNameLE->text().isEmpty())
    g.profile[g.id()].name(tr("My Radio"));
  else
    g.profile[g.id()].name(ui->profileNameLE->text());

  bool fwchange = false;
  Firmware * newFw = getFirmwareVariant();
  // If a new fw type has been choosen, several things need to reset
  if (Firmware::getCurrentVariant()->getId() != newFw->getId()) {
    // check if we're going to be converting to a new radio type and there are unsaved files in the main window
    if (mainWinHasDirtyChild && !Boards::isBoardCompatible(Firmware::getCurrentVariant()->getBoard(), newFw->getBoard())) {
      QString q = tr("<p><b>You cannot switch Radio Type or change Build Options while there are unsaved file changes. What do you wish to do?</b></p> <ul>" \
                     "<li><i>Save All</i> - Save any open file(s) before saving Settings.<li>" \
                     "<li><i>Reset</i> - Revert to the previous Radio Type and Build Options before saving Settings.</li>" \
                     "<li><i>Cancel</i> - Return to the Settings editor dialog.</li></ul>");
      int resp = QMessageBox::question(this, windowTitle(), q, (QMessageBox::SaveAll | QMessageBox::Reset | QMessageBox::Cancel), QMessageBox::Cancel);
      if (resp == QMessageBox::SaveAll) {
        // signal main window to save files, need to do this before the fw actually changes
        emit firmwareProfileAboutToChange();
      }
      else if (resp == QMessageBox::Reset) {
        // bail out early before saving the radio type & firmware options
        QDialog::accept();
        return;
      }
      else {
        // we do not accept the dialog close
        return;
      }
    }
    Firmware::setCurrentVariant(newFw);
    g.profile[g.id()].fwName("");
    g.profile[g.id()].initFwVariables();
    g.profile[g.id()].fwType(newFw->getId());
    fwchange = true;
  }

  QDialog::accept();

  if (fwchange)
    emit firmwareProfileChanged(g.id());  // important to do this after the accepted() signal
}

void AppPreferencesDialog::on_snapshotPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this, tr("Select your snapshot folder"), g.snapshotDir());
  if (!fileName.isEmpty()) {
    g.snapshotDir(fileName);
    g.snapToClpbrd(false);
    ui->snapshotPath->setText(fileName);
  }
}

void AppPreferencesDialog::initSettings()
{
  ui->snapshotClipboardCKB->setChecked(g.snapToClpbrd());
  ui->burnFirmware->setChecked(g.profile[g.id()].burnFirmware());
  ui->snapshotPath->setText(g.snapshotDir());
  ui->snapshotPath->setReadOnly(true);
  if (ui->snapshotClipboardCKB->isChecked()) {
    ui->snapshotPath->setDisabled(true);
    ui->snapshotPathButton->setDisabled(true);
  }
#if !defined(ALLOW_NIGHTLY_BUILDS)
  // TODO should we gray out nightly builds here?
#endif
  ui->OpenTxBranch->setCurrentIndex(g.OpenTxBranch());
  ui->autoCheckCompanion->setChecked(g.autoCheckApp());
  ui->autoCheckFirmware->setChecked(g.autoCheckFw());
  ui->showSplash->setChecked(g.showSplash());
  ui->historySize->setValue(g.historySize());
  ui->backLightColor->setCurrentIndex(g.backLight());
  ui->volumeGain->setValue(g.profile[g.id()].volumeGain() / 10.0);

  if (IS_TARANIS(getCurrentBoard())) {
    ui->backLightColor->setEnabled(false);
  }

  ui->simuSW->setChecked(g.simuSW());
  ui->opt_removeBlankSlots->setChecked(g.removeModelSlots());
  ui->opt_newMdl_useNone->setChecked(g.newModelAction() == 0);
  ui->opt_newMdl_useWizard->setChecked(g.newModelAction() == 1);
  ui->opt_newMdl_useEditor->setChecked(g.newModelAction() == 2);
  ui->libraryPath->setText(g.libDir());
  ui->ge_lineedit->setText(g.gePath());

  if (!g.backupDir().isEmpty()) {
    if (QDir(g.backupDir()).exists()) {
      ui->backupPath->setText(g.backupDir());
      ui->backupEnable->setEnabled(true);
      ui->backupEnable->setChecked(g.enableBackup());
    }
    else {
      ui->backupEnable->setDisabled(true);
    }
  }
  else {
    ui->backupEnable->setDisabled(true);
  }
  ui->splashincludeCB->setCurrentIndex(g.embedSplashes());

  ui->opt_appDebugLog->setChecked(g.appDebugLog());
  ui->opt_fwTraceLog->setChecked(g.fwTraceLog());
  ui->appLogsDir->setText(g.appLogsDir());
  toggleAppLogSettings();

#if defined(JOYSTICKS)
  ui->joystickChkB->setChecked(g.jsSupport());
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
    ui->joystickCB->setCurrentIndex(g.jsCtrl());
  }
  else {
    ui->joystickCB->clear();
    ui->joystickCB->setDisabled(true);
    ui->joystickcalButton->setDisabled(true);
  }
#endif
  //  Profile Tab Inits
  ui->channelorderCB->setCurrentIndex(g.profile[g.id()].channelOrder());
  ui->stickmodeCB->setCurrentIndex(g.profile[g.id()].defaultMode());
  ui->renameFirmware->setChecked(g.profile[g.id()].renameFwFiles());
  ui->sdPath->setText(g.profile[g.id()].sdPath());
  if (!g.profile[g.id()].pBackupDir().isEmpty()) {
    if (QDir(g.profile[g.id()].pBackupDir()).exists()) {
      ui->profilebackupPath->setText(g.profile[g.id()].pBackupDir());
      ui->pbackupEnable->setEnabled(true);
      ui->pbackupEnable->setChecked(g.profile[g.id()].penableBackup());
    } else {
      ui->pbackupEnable->setDisabled(true);
    }
  }
  else {
      ui->pbackupEnable->setDisabled(true);
  }

  ui->profileNameLE->setText(g.profile[g.id()].name());

  QString hwSettings;
  if (g.profile[g.id()].stickPotCalib() == "" ) {
    hwSettings = tr("EMPTY: No radio settings stored in profile");
  }
  else  {
    QString str = g.profile[g.id()].timeStamp();
    if (str.isEmpty())
      hwSettings = tr("AVAILABLE: Radio settings of unknown age");
    else
      hwSettings = tr("AVAILABLE: Radio settings stored %1").arg(str);
  }
  ui->lblGeneralSettings->setText(hwSettings);

  QString currType = QStringList(g.profile[g.id()].fwType().split('-').mid(0, 2)).join('-');
  foreach(Firmware * firmware, Firmware::getRegisteredFirmwares()) {
    ui->downloadVerCB->addItem(firmware->getName(), firmware->getId());
    if (currType == firmware->getId()) {
      ui->downloadVerCB->setCurrentIndex(ui->downloadVerCB->count() - 1);
    }
  }

  baseFirmwareChanged();
}

void AppPreferencesDialog::on_libraryPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your library folder"), g.libDir());
  if (!fileName.isEmpty()) {
    g.libDir(fileName);
    ui->libraryPath->setText(fileName);
  }
}

void AppPreferencesDialog::on_snapshotClipboardCKB_clicked()
{
  if (ui->snapshotClipboardCKB->isChecked()) {
    ui->snapshotPath->setDisabled(true);
    ui->snapshotPathButton->setDisabled(true);
    g.snapToClpbrd(true);
  }
  else {
    ui->snapshotPath->setEnabled(true);
    ui->snapshotPath->setReadOnly(true);
    ui->snapshotPathButton->setEnabled(true);
    g.snapToClpbrd(false);
  }
}

void AppPreferencesDialog::on_backupPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your Models and Settings backup folder"), g.backupDir());
  if (!fileName.isEmpty()) {
    g.backupDir(fileName);
    ui->backupPath->setText(fileName);
    ui->backupEnable->setEnabled(true);
  }
}

void AppPreferencesDialog::on_ProfilebackupPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your Models and Settings backup folder"), g.backupDir());
  if (!fileName.isEmpty()) {
    ui->profilebackupPath->setText(fileName);
    ui->pbackupEnable->setEnabled(true);
  }
}


void AppPreferencesDialog::on_btn_appLogsDir_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this, tr("Select a folder for application logs"), ui->appLogsDir->text());
  if (!fileName.isEmpty()) {
    ui->appLogsDir->setText(fileName);
  }
}

void AppPreferencesDialog::on_ge_pathButton_clicked()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Select Google Earth executable"),ui->ge_lineedit->text());
  if (!fileName.isEmpty()) {
    ui->ge_lineedit->setText(fileName);
  }
}

#if defined(JOYSTICKS)
void AppPreferencesDialog::on_joystickChkB_clicked() {
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

void AppPreferencesDialog::on_joystickcalButton_clicked() {
   joystickDialog * jd=new joystickDialog(this, ui->joystickCB->currentIndex());
   jd->exec();
}
#endif

// ******** Profile tab functions

void AppPreferencesDialog::on_sdPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select the folder replicating your SD structure"), g.profile[g.id()].sdPath());
  if (!fileName.isEmpty()) {
    ui->sdPath->setText(fileName);
  }
}

bool AppPreferencesDialog::displayImage(const QString & fileName)
{
  // Start by clearing the label
  ui->imageLabel->clear();

  if (fileName.isEmpty())
    return false;

  QImage image(fileName);
  if (image.isNull())
    return false;

  ui->imageLabel->setPixmap(makePixMap(image));
  ui->imageLabel->setFixedSize(getCurrentFirmware()->getCapability(LcdWidth), getCurrentFirmware()->getCapability(LcdHeight));
  return true;
}

void AppPreferencesDialog::on_SplashSelect_clicked()
{
  QString supportedImageFormats;
  for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
    supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
  }

  QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image to load"), g.imagesDir(), tr("Images (%1)").arg(supportedImageFormats));

  if (!fileName.isEmpty()){
    g.imagesDir(QFileInfo(fileName).dir().absolutePath());

    displayImage(fileName);
    ui->SplashFileName->setText(fileName);
  }
}

void AppPreferencesDialog::on_clearImageButton_clicked()
{
  ui->imageLabel->clear();
  ui->SplashFileName->clear();
}


void AppPreferencesDialog::showVoice(bool show)
{
  ui->voiceLabel->setVisible(show);
  ui->voiceCombo->setVisible(show);
}

void AppPreferencesDialog::baseFirmwareChanged()
{
  QString selected_firmware = ui->downloadVerCB->currentData().toString();

  foreach(Firmware * firmware, Firmware::getRegisteredFirmwares()) {
    if (firmware->getId() == selected_firmware) {
      populateFirmwareOptions(firmware);
      break;
    }
  }
}

Firmware * AppPreferencesDialog::getFirmwareVariant()
{
  QString selected_firmware = ui->downloadVerCB->currentData().toString();

  foreach(Firmware * firmware, Firmware::getRegisteredFirmwares()) {
    QString id = firmware->getId();
    if (id == selected_firmware) {
      foreach(QCheckBox *cb, optionsCheckBoxes) {
        if (cb->isChecked()) {
          id += "-" + cb->text();
        }
      }

      if (voice && voice->isChecked()) {
        id += "-tts" + ui->voiceCombo->currentText();
      }

      if (ui->langCombo->count()) {
        id += "-" + ui->langCombo->currentText();
      }

      return Firmware::getFirmwareForId(id);
    }
  }

  // Should never occur...
  return Firmware::getDefaultVariant();
}

void AppPreferencesDialog::firmwareOptionChanged(bool state)
{
  QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
  if (cb == voice) {
    showVoice(voice->isChecked());
  }
  Firmware * firmware=NULL;
  if (cb && state) {
    QVariant selected_firmware = ui->downloadVerCB->currentData();
    foreach(firmware, Firmware::getRegisteredFirmwares()) {
      if (firmware->getId() == selected_firmware) {
        foreach(QList<Option> opts, firmware->opts) {
          foreach(Option opt, opts) {
            if (cb->text() == opt.name) {
              foreach(Option other, opts) {
                if (other.name != opt.name) {
                  foreach(QCheckBox *ocb, optionsCheckBoxes) {
                    if (ocb->text() == other.name) {
                      ocb->setChecked(false);
                    }
                  }
                }
              }
              return;
            }
          }
        }
      }
    }
  }
}

void AppPreferencesDialog::toggleAppLogSettings()
{
  bool vis = (ui->opt_appDebugLog->isChecked() || ui->opt_fwTraceLog->isChecked());
  ui->appLogsDir->setVisible(vis);
  ui->lbl_appLogsDir->setVisible(vis);
  ui->btn_appLogsDir->setVisible(vis);
}

void AppPreferencesDialog::populateFirmwareOptions(const Firmware * firmware)
{
  const Firmware * parent = firmware->getFirmwareBase();

  updateLock = true;

  QString id = Firmware::getCurrentVariant()->getId();
  ui->langCombo->clear();
  foreach(const char *lang, parent->languages) {
    ui->langCombo->addItem(lang);
    if (id.endsWith(lang)) {
      ui->langCombo->setCurrentIndex(ui->langCombo->count() - 1);
    }
  }

  voice = NULL; // we will search for a voice checkbox

  int index = 0;
  QWidget * prevFocus = ui->voiceCombo;
  foreach(QList<Option> opts, parent->opts) {
    foreach(Option opt, opts) {
      if (index >= optionsCheckBoxes.size()) {
        QCheckBox * checkbox = new QCheckBox(ui->profileTab);
        ui->optionsLayout->addWidget(checkbox, optionsCheckBoxes.count()/4, optionsCheckBoxes.count()%4, 1, 1);
        optionsCheckBoxes.push_back(checkbox);
        connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(firmwareOptionChanged(bool)));
        if (prevFocus) {
          QWidget::setTabOrder(prevFocus, checkbox);
        }
      }

      QCheckBox *cb = optionsCheckBoxes.at(index++);
      if (cb) {
        cb->show();
        cb->setText(opt.name);
        cb->setToolTip(opt.tooltip);
        cb->setCheckState(id.contains(opt.name) ? Qt::Checked : Qt::Unchecked);
        if (opt.name == QString("voice")) {
          voice = cb;
        }
        prevFocus = cb;
      }
    }
  }

  for (; index<optionsCheckBoxes.size(); index++) {
    QCheckBox *cb = optionsCheckBoxes.at(index);
    cb->hide();
    cb->setCheckState(Qt::Unchecked);
  }

  ui->voiceCombo->clear();
  foreach(const char *lang, parent->ttslanguages) {
    ui->voiceCombo->addItem(lang);
    if (id.contains(QString("-tts%1").arg(lang))) {
      ui->voiceCombo->setCurrentIndex(ui->voiceCombo->count() - 1);
    }
  }

  showVoice(voice && voice->isChecked());

  // TODO: Remove once splash replacement supported on Horus
  // NOTE: 480x272 image causes issues on screens <800px high, needs a solution like scrolling once reinstated
  if (IS_HORUS(parent->getBoard())) {
    ui->widget_splashImage->hide();
    ui->SplashFileName->setText("");
  }
  else {
    ui->widget_splashImage->show();
    ui->SplashFileName->setText(g.profile[g.id()].splashFile());
    displayImage(g.profile[g.id()].splashFile());
  }

  updateLock = false;
  QTimer::singleShot(50, this, SLOT(shrink()));
}

void AppPreferencesDialog::shrink()
{
  adjustSize();
}
