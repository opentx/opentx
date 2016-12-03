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

#include "mdichild.h"
#include "ui_mdichild.h"
#include "xmlinterface.h"
#include "hexinterface.h"
#include "mainwindow.h"
#include "modeledit/modeledit.h"
#include "generaledit/generaledit.h"
#include "burnconfigdialog.h"
#include "printdialog.h"
#include "flasheepromdialog.h"
#include "helpers.h"
#include "appdata.h"
#include "wizarddialog.h"
#include "flashfirmwaredialog.h"
#include "storage_eeprom.h"
#include "storage_sdcard.h"
#include <QFileInfo>

#if defined WIN32 || !defined __GNUC__
#include <windows.h>
#define sleep(x) Sleep(x*1000)
#else
#include <unistd.h>
#endif

MdiChild::MdiChild():
  QWidget(),
  ui(new Ui::mdiChild),
  firmware(GetCurrentFirmware()),
  isUntitled(true),
  fileChanged(false)
{
  ui->setupUi(this);
  setWindowIcon(CompanionIcon("open.png"));
  ui->SimulateTxButton->setIcon(CompanionIcon("simulate.png"));
  setAttribute(Qt::WA_DeleteOnClose);

  eepromInterfaceChanged();

  if (!(this->isMaximized() || this->isMinimized())) {
    adjustSize();
  }
}

MdiChild::~MdiChild()
{
  delete ui;
}

void MdiChild::qSleep(int ms)
{
  if (ms<0)
    return;

#if defined WIN32 || !defined __GNUC__
    Sleep(uint(ms));
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}

void MdiChild::eepromInterfaceChanged()
{
  ui->modelsList->refreshList();
  if (GetCurrentFirmware()->getBoard() == BOARD_HORUS && !HORUS_READY_FOR_RELEASE()) {
      ui->SimulateTxButton->setEnabled(false);
    }
  else {
      ui->SimulateTxButton->setEnabled(GetCurrentFirmware()->getCapability(Simulation));
  }
  updateTitle();
}

void MdiChild::cut()
{
  ui->modelsList->cut();
}

void MdiChild::copy()
{
  ui->modelsList->copy();
}

void MdiChild::paste()
{
  ui->modelsList->paste();
}

bool MdiChild::hasPasteData()
{
  return ui->modelsList->hasPasteData();
}

bool MdiChild::hasSelection()
{
    return ui->modelsList->hasSelection();
}

void MdiChild::updateTitle()
{
  QString title = userFriendlyCurrentFile() + "[*]" + " (" + GetCurrentFirmware()->getName() + QString(")");
  if (!IS_SKY9X(GetCurrentFirmware()->getBoard()))
    title += QString(" - %1 ").arg(EEPromAvail) + tr("free bytes");
  setWindowTitle(title);
}

void MdiChild::setModified()
{
  ui->modelsList->refreshList();
  fileChanged = true;
  updateTitle();
  documentWasModified();
}

void MdiChild::on_SimulateTxButton_clicked()
{
  startSimulation(this, radioData, -1);
}

void MdiChild::checkAndInitModel(int row)
{
  ModelData &model = radioData.models[row - 1];
  if (model.isEmpty()) {
    model.setDefaultValues(row - 1, radioData.generalSettings);
    setModified();
  }
}

void MdiChild::generalEdit()
{
  GeneralEdit *t = new GeneralEdit(this, radioData, GetCurrentFirmware()/*firmware*/);
  connect(t, SIGNAL(modified()), this, SLOT(setModified()));
  t->show();
}

void MdiChild::modelEdit()
{
  int row = getCurrentRow();

  if (row == 0){
    generalEdit();
  }
  else {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    checkAndInitModel( row );
    ModelData & model = radioData.models[row - 1];
    gStopwatch.restart();
    gStopwatch.report("ModelEdit creation");
    ModelEdit *t = new ModelEdit(this, radioData, (row - 1), GetCurrentFirmware()/*firmware*/);
    gStopwatch.report("ModelEdit created");
    t->setWindowTitle(tr("Editing model %1: ").arg(row) + model.name);
    connect(t, SIGNAL(modified()), this, SLOT(setModified()));
    gStopwatch.report("STARTING MODEL EDIT");
    t->show();
    QApplication::restoreOverrideCursor();
    gStopwatch.report("ModelEdit shown");
  }
}

void MdiChild::wizardEdit()
{
  int row = getCurrentRow();
  if (row > 0) {
    checkAndInitModel(row);
    WizardDialog * wizard = new WizardDialog(radioData.generalSettings, row, this);
    wizard->exec();
    if (wizard->mix.complete /*TODO rather test the exec() result?*/) {
      radioData.models[row - 1] = wizard->mix;
      setModified();
    }
  }
}

void MdiChild::openEditWindow()
{
  int row = getCurrentRow();
  if (row == 0){
    generalEdit();
  }
  else{
    ModelData & model = radioData.models[row - 1];
    if (model.isEmpty() && g.useWizard()) {
      wizardEdit();
    }
    else {
      modelEdit();
    }
  }
}

void MdiChild::newFile()
{
  static int sequenceNumber = 1;

  isUntitled = true;
  curFile = QString("document%1.eepe").arg(sequenceNumber++);
  updateTitle();
}

bool MdiChild::loadFile(const QString & fileName, bool resetCurrentFile)
{
  QFile file(fileName);

  if (!file.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("Unable to find file %1!").arg(fileName));
    return false;
  }

  int fileType = getFileType(fileName);

#if 0
  if (fileType==FILE_TYPE_XML) {
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
      QMessageBox::critical(this, tr("Error"),
          tr("Error opening file %1:\n%2.")
          .arg(fileName)
          .arg(file.errorString()));
      return false;
    }
    QTextStream inputStream(&file);
    XmlInterface(inputStream).load(radioData);
  }
  else
#endif
  if (fileType==FILE_TYPE_HEX || fileType==FILE_TYPE_EEPE) { //read HEX file
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
        QMessageBox::critical(this, tr("Error"),
                             tr("Error opening file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QDomDocument doc(ER9X_EEPROM_FILE_TYPE);
    bool xmlOK = doc.setContent(&file);
    if (xmlOK) {
      std::bitset<NUM_ERRORS> errors((unsigned long long)LoadEepromXml(radioData, doc));
      if (errors.test(ALL_OK)) {
        ui->modelsList->refreshList();
        if(resetCurrentFile) setCurrentFile(fileName);
        return true;
      }
    }
    file.reset();

    QTextStream inputStream(&file);

    if (fileType==FILE_TYPE_EEPE) {  // read EEPE file header
      QString hline = inputStream.readLine();
      if (hline!=EEPE_EEPROM_FILE_HEADER) {
        file.close();
        return false;
      }
    }

    QByteArray eeprom(EESIZE_MAX, 0);
    int eeprom_size = HexInterface(inputStream).load((uint8_t *)eeprom.data(), EESIZE_MAX);
    if (!eeprom_size) {
      QMessageBox::critical(this, tr("Error"),
          tr("Invalid EEPROM File %1")
          .arg(fileName));
      file.close();
      return false;
    }

    file.close();

    std::bitset<NUM_ERRORS> errors((unsigned long long)LoadEeprom(radioData, (uint8_t *)eeprom.data(), eeprom_size));
    if (!errors.test(ALL_OK)) {
      ShowEepromErrors(this, tr("Error"), tr("Invalid EEPROM File %1").arg(fileName), errors.to_ulong());
      return false;
    }
    if (errors.test(HAS_WARNINGS)) {
      ShowEepromWarnings(this, tr("Warning"), errors.to_ulong());
    }

    ui->modelsList->refreshList();
    if (resetCurrentFile) setCurrentFile(fileName);

    return true;
  }
  else if (fileType==FILE_TYPE_BIN) { //read binary
    int eeprom_size = file.size();

    if (!file.open(QFile::ReadOnly)) {  //reading binary file   - TODO HEX support
        QMessageBox::critical(this, tr("Error"),
                             tr("Error opening file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    uint8_t * eeprom = (uint8_t *)malloc(eeprom_size);
    memset(eeprom, 0, eeprom_size);
    long result = file.read((char*)eeprom, eeprom_size);
    file.close();

    if (result != eeprom_size) {
        QMessageBox::critical(this, tr("Error"),
                             tr("Error reading file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        free(eeprom);
        return false;
    }

    std::bitset<NUM_ERRORS> errorsEeprom((unsigned long long)LoadEeprom(radioData, eeprom, eeprom_size));
    if (!errorsEeprom.test(ALL_OK)) {
      std::bitset<NUM_ERRORS> errorsBackup((unsigned long long)LoadBackup(radioData, eeprom, eeprom_size, 0));
      if (!errorsBackup.test(ALL_OK)) {
        ShowEepromErrors(this, tr("Error"), tr("Invalid binary EEPROM File %1").arg(fileName), (errorsEeprom | errorsBackup).to_ulong());
        free(eeprom);
        return false;
      }
      if (errorsBackup.test(HAS_WARNINGS)) {
        ShowEepromWarnings(this, tr("Warning"), errorsBackup.to_ulong());
      }
    }
    else if (errorsEeprom.test(HAS_WARNINGS)) {
      ShowEepromWarnings(this, tr("Warning"), errorsEeprom.to_ulong());
    }

    ui->modelsList->refreshList();
    if (resetCurrentFile)
      setCurrentFile(fileName);

    free(eeprom);
    return true;
  }
  else if (fileType == FILE_TYPE_OTX) { //read zip archive
    if (!GetEepromInterface()->loadFile(radioData, fileName)) {
      ui->modelsList->refreshList();
      if (resetCurrentFile)
        setCurrentFile(fileName);
      return true;
    }
  }

  return false;
}

bool MdiChild::save()
{
  if (isUntitled) {
    return saveAs(true);
  }
  else {
    return saveFile(curFile);
  }
}

bool MdiChild::saveAs(bool isNew)
{
    QString fileName;
    if (IS_SKY9X(GetEepromInterface()->getBoard())) {
      curFile.replace(".eepe", ".bin");
      QFileInfo fi(curFile);
#ifdef __APPLE__
      fileName = QFileDialog::getSaveFileName(this, tr("Save As"), g.eepromDir() + "/" +fi.fileName());
#else
      fileName = QFileDialog::getSaveFileName(this, tr("Save As"), g.eepromDir() + "/" +fi.fileName(), tr(BIN_FILES_FILTER));
#endif
    }
    else {
      QFileInfo fi(curFile);
#ifdef __APPLE__
      fileName = QFileDialog::getSaveFileName(this, tr("Save As"), g.eepromDir() + "/" +fi.fileName());
#else
      fileName = QFileDialog::getSaveFileName(this, tr("Save As"), g.eepromDir() + "/" +fi.fileName(), tr(EEPROM_FILES_FILTER));
#endif
    }
    if (fileName.isEmpty())
      return false;
    g.eepromDir( QFileInfo(fileName).dir().absolutePath() );
    if (isNew)
      return saveFile(fileName);
    else
      return saveFile(fileName,true);
}

bool MdiChild::saveFile(const QString &fileName, bool setCurrent)
{
  QString myFile;
  myFile = fileName;
  if (IS_SKY9X(GetEepromInterface()->getBoard())) {
    myFile.replace(".eepe", ".bin");
  }
  QFile file(myFile);

  int fileType = getFileType(myFile);

  uint8_t * eeprom = (uint8_t*)malloc(GetEepromInterface()->getEEpromSize());
  int eeprom_size = 0;

  if (fileType != FILE_TYPE_XML) {
    eeprom_size = GetEepromInterface()->save(eeprom, radioData, 0, GetCurrentFirmware()->getVariantNumber());
    if (!eeprom_size) {
      QMessageBox::warning(this, tr("Error"),tr("Cannot write file %1:\n%2.").arg(myFile).arg(file.errorString()));
      return false;
    }
  }

  if (!file.open(fileType == FILE_TYPE_BIN ? QIODevice::WriteOnly : (QIODevice::WriteOnly | QIODevice::Text))) {
    QMessageBox::warning(this, tr("Error"),tr("Cannot write file %1:\n%2.").arg(myFile).arg(file.errorString()));
    return false;
  }

  QTextStream outputStream(&file);

#if 0
    if (fileType==FILE_TYPE_XML) {
      if (!XmlInterface(outputStream).save(radioData)) {
        QMessageBox::warning(this, tr("Error"),tr("Cannot write file %1:\n%2.").arg(myFile).arg(file.errorString()));
        file.close();
        return false;
      }
    }
    else
#endif
  if (fileType==FILE_TYPE_HEX || fileType==FILE_TYPE_EEPE) { // write hex
    if (fileType==FILE_TYPE_EEPE)
      outputStream << EEPE_EEPROM_FILE_HEADER << "\n";

    if (!HexInterface(outputStream).save(eeprom, eeprom_size)) {
        QMessageBox::warning(this, tr("Error"),tr("Cannot write file %1:\n%2.").arg(myFile).arg(file.errorString()));
        file.close();
        return false;
    }
  }
  else if (fileType==FILE_TYPE_BIN) // write binary
  {
    long result = file.write((char*)eeprom, eeprom_size);
    if(result!=eeprom_size) {
      QMessageBox::warning(this, tr("Error"),tr("Error writing file %1:\n%2.").arg(myFile).arg(file.errorString()));
      return false;
    }
  }
  else {
    QMessageBox::warning(this, tr("Error"),tr("Error writing file %1:\n%2.").arg(myFile).arg("Unknown format"));
    return false;
  }

  free(eeprom); // TODO free in all cases ...
  file.close();
  if(setCurrent) setCurrentFile(myFile);

  return true;
}

QString MdiChild::userFriendlyCurrentFile()
{
  return strippedName(curFile);
}

void MdiChild::closeEvent(QCloseEvent *event)
{
  if (maybeSave()) {
    event->accept();
  }
  else {
    event->ignore();
  }
}

void MdiChild::documentWasModified()
{
  setWindowModified(fileChanged);
}

bool MdiChild::maybeSave()
{
  if (fileChanged) {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Companion"),
        tr("%1 has been modified.\n"
           "Do you want to save your changes?").arg(userFriendlyCurrentFile()),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
      return save();
    else if (ret == QMessageBox::Cancel)
      return false;
  }
  return true;
}

void MdiChild::setCurrentFile(const QString &fileName)
{
  curFile = QFileInfo(fileName).canonicalFilePath();
  isUntitled = false;
  fileChanged = false;
  setWindowModified(false);
  updateTitle();
  int MaxRecentFiles = g.historySize();
  QStringList files = g.recentFiles();
  files.removeAll(fileName);
  files.prepend(fileName);
  while (files.size() > MaxRecentFiles)
      files.removeLast();

  g.recentFiles( files );
}

QString MdiChild::strippedName(const QString &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}

void MdiChild::writeEeprom()  // write to Tx
{
  QString tempFile = generateProcessUniqueTempFileName("temp.bin");
  saveFile(tempFile, false);
  if(!QFileInfo(tempFile).exists()) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot write temporary file!"));
    return;
  }

  FlashEEpromDialog *cd = new FlashEEpromDialog(this, tempFile);
  cd->exec();
}

void MdiChild::simulate()
{
  if (getCurrentRow() > 0) {
    startSimulation(this, radioData, getCurrentRow()-1);
  }
}

void MdiChild::print(int model, QString filename)
{
  PrintDialog * pd = NULL;

  if (model>=0 && !filename.isEmpty()) {
    pd = new PrintDialog(this, GetCurrentFirmware()/*firmware*/, radioData.generalSettings, radioData.models[model], filename);
  }
  else if (getCurrentRow() > 0) {
    pd = new PrintDialog(this, GetCurrentFirmware()/*firmware*/, radioData.generalSettings, radioData.models[getCurrentRow()-1]);
  }

  if (pd) {
    pd->setAttribute(Qt::WA_DeleteOnClose, true);
    pd->show();
  }
}

void MdiChild::viableModelSelected(bool viable)
{
  emit copyAvailable(viable);
}

void MdiChild::setEEpromAvail(int eavail)
{
  EEPromAvail=eavail;
}

int MdiChild::getCurrentRow() const
{
  return ui->modelsList->currentRow();
}

bool MdiChild::loadBackup()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open backup Models and Settings file"), g.eepromDir(),tr(EEPROM_FILES_FILTER));
  if (fileName.isEmpty())
    return false;
  QFile file(fileName);

  if (!file.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("Unable to find file %1!").arg(fileName));
    return false;
  }
  if(getCurrentRow() < 1) return false;
  int index = getCurrentRow() - 1;

  int eeprom_size = file.size();
  if (!file.open(QFile::ReadOnly)) {  //reading binary file   - TODO HEX support
    QMessageBox::critical(this, tr("Error"),
                          tr("Error opening file %1:\n%2.")
                          .arg(fileName)
                          .arg(file.errorString()));
    return false;
  }
  QByteArray eeprom(eeprom_size, 0);
  long result = file.read((char*)eeprom.data(), eeprom_size);
  file.close();

  if (result != eeprom_size) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Error reading file %1:\n%2.")
                          .arg(fileName)
                          .arg(file.errorString()));

    return false;
  }

    std::bitset<NUM_ERRORS> errorsEeprom((unsigned long long)LoadBackup(radioData, (uint8_t *)eeprom.data(), eeprom_size, index));
    if (!errorsEeprom.test(ALL_OK)) {
      ShowEepromErrors(this, tr("Error"), tr("Invalid binary backup File %1").arg(fileName), (errorsEeprom).to_ulong());
      return false;
    }
    if (errorsEeprom.test(HAS_WARNINGS)) {
      ShowEepromWarnings(this, tr("Warning"), errorsEeprom.to_ulong());
    }

  ui->modelsList->refreshList();

  return true;
}
