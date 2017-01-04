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

#if defined _MSC_VER || !defined __GNUC__
#include <windows.h>
#define sleep(x) Sleep(x*1000)
#else
#include <unistd.h>
#endif

class DragDropHeader {
  public:
    DragDropHeader():
      general_settings(false),
      models_count(0)
    {
    }
    bool general_settings;
    uint8_t models_count;
    uint8_t models[CPN_MAX_MODELS];
};

MdiChild::MdiChild():
  QWidget(),
  ui(new Ui::MdiChild),
  firmware(GetCurrentFirmware()),
  isUntitled(true),
  fileChanged(false)
{
  BoardEnum board = GetCurrentFirmware()->getBoard();
  
  ui->setupUi(this);
  setWindowIcon(CompanionIcon("open.png"));
  
  modelsListModel = new TreeModel(&radioData, this);
  ui->modelsList->setModel(modelsListModel);
  ui->simulateButton->setIcon(CompanionIcon("simulate.png"));
  setAttribute(Qt::WA_DeleteOnClose);
  
  eepromInterfaceChanged();
  
  connect(ui->modelsList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openModelEditWindow()));
  connect(ui->modelsList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showModelsListContextMenu(const QPoint &)));
  // connect(ui->modelsList, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(onCurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
  
  ui->modelsList->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->modelsList->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->modelsList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->modelsList->setDragEnabled(true);
  ui->modelsList->setAcceptDrops(true);
  ui->modelsList->setDragDropOverwriteMode(true);
  ui->modelsList->setDropIndicatorShown(true);
  
  if (IS_HORUS(board)) {
    ui->modelsList->header()->hide();
  }
  else {
    ui->modelsList->setIndentation(0);
  }

  
  if (!(isMaximized() || isMinimized())) {
    adjustSize();
  }
}

MdiChild::~MdiChild()
{
  delete ui;
}

void MdiChild::refresh(bool expand)
{
  modelsListModel->refresh();
  if (1 || expand) {
    ui->modelsList->expandAll();
  }
  if (GetCurrentFirmware()->getBoard() == BOARD_HORUS && !HORUS_READY_FOR_RELEASE()) {
    ui->simulateButton->setEnabled(false);
  }
  else {
    ui->simulateButton->setEnabled(GetCurrentFirmware()->getCapability(Simulation));
  }
  updateTitle();
}

void MdiChild::confirmDelete()
{
  if (QMessageBox::warning(this, "Companion", tr("Delete selected models?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
    deleteSelectedModels();
  }
}

void MdiChild::deleteSelectedModels()
{
  foreach (QModelIndex index, ui->modelsList->selectionModel()->selectedIndexes()) {
    unsigned int modelIndex = modelsListModel->getItem(index)->getModelIndex();
    if (radioData.generalSettings.currModelIndex != modelIndex) {
      qDebug() << "delete" << modelIndex;
      radioData.models[modelIndex].clear();
      setModified();
    }
    else {
      QMessageBox::warning(this, "Companion", tr("Cannot delete default model."), QMessageBox::Ok);
    }
  }
}

void MdiChild::showModelsListContextMenu(const QPoint & pos)
{
  int modelIndex = getCurrentRow();
  QPoint globalPos = ui->modelsList->mapToGlobal(pos);
  QMenu contextMenu;
  
  const QClipboard * clipboard = QApplication::clipboard();
  const QMimeData * mimeData = clipboard->mimeData();
  bool hasData = mimeData->hasFormat("application/x-companion");
  
  if (modelIndex >= 0) {
    contextMenu.addAction(CompanionIcon("edit.png"), tr("&Edit"), this, SLOT(modelEdit()));
    contextMenu.addAction(CompanionIcon("open.png"), tr("&Restore from backup"), this, SLOT(loadBackup()));
    contextMenu.addAction(CompanionIcon("wizard.png"), tr("&Model Wizard"), this, SLOT(wizardEdit()));
    contextMenu.addSeparator();
    contextMenu.addAction(CompanionIcon("clear.png"), tr("&Delete"), this, SLOT(confirmDelete()), tr("Delete"));
    contextMenu.addAction(CompanionIcon("copy.png"), tr("&Copy"), this, SLOT(copy()), tr("Ctrl+C"));
    contextMenu.addAction(CompanionIcon("cut.png"), tr("&Cut"), this, SLOT(cut()), tr("Ctrl+X"));
    contextMenu.addAction(CompanionIcon("paste.png"), tr("&Paste"), this, SLOT(paste()), tr("Ctrl+V"))->setEnabled(hasData);
    contextMenu.addAction(CompanionIcon("duplicate.png"), tr("D&uplicate"), this, SLOT(duplicate()), tr("Ctrl+U"));
    contextMenu.addSeparator();
    contextMenu.addAction(CompanionIcon("currentmodel.png"), tr("&Use as default"), this, SLOT(setdefault()));
    contextMenu.addSeparator();
    contextMenu.addAction(CompanionIcon("print.png"), tr("P&rint model"), this, SLOT(print()), QKeySequence(tr("Ctrl+P")));
    contextMenu.addSeparator();
    contextMenu.addAction(CompanionIcon("simulate.png"), tr("&Simulate model"), this, SLOT(modelSimulate()), tr("Alt+S"));
  }
  
  // TODO context menu for radio settings
  // contextMenu.addAction(CompanionIcon("edit.png"), tr("&Edit"), this, SLOT(EditModel()));
  
  contextMenu.exec(globalPos);
}

void MdiChild::eepromInterfaceChanged()
{
  refresh();
}

void MdiChild::cut()
{
  copy();
  deleteSelectedModels();
}

void MdiChild::copy()
{
  QByteArray gmData;
  doCopy(&gmData);
  
  QMimeData * mimeData = new QMimeData;
  mimeData->setData("application/x-companion", gmData);
  
  QClipboard * clipboard = QApplication::clipboard();
  clipboard->setMimeData(mimeData, QClipboard::Clipboard);
}

void MdiChild::doCopy(QByteArray * gmData)
{
  DragDropHeader header;
  
  qDebug() << ui->modelsList->selectionModel()->selectedIndexes();
  foreach(QModelIndex index, ui->modelsList->selectionModel()->selectedIndexes()) {
    char column = index.column();
    if (column == 0) {
      char row = index.row();
      if (!row) {
        header.general_settings = true;
        gmData->append('G');
        gmData->append((char *) &radioData.generalSettings, sizeof(GeneralSettings));
      }
      else {
        header.models[header.models_count++] = row;
        gmData->append('M');
        gmData->append((char *) &radioData.models[row - 1], sizeof(ModelData));
      }
    }
  }
  
  gmData->prepend((char *)&header, sizeof(header));
}


void MdiChild::paste()
{
//  ui->modelsList->paste();
}

bool MdiChild::hasPasteData() const
{
  return false; // ui->modelsList->hasPasteData();
}

bool MdiChild::hasSelection() const
{
    return false; // ui->modelsList->hasSelection();
}

void MdiChild::updateTitle()
{
  QString title = userFriendlyCurrentFile() + "[*]" + " (" + GetCurrentFirmware()->getName() + QString(")");
  int availableEEpromSize = modelsListModel->getAvailableEEpromSize();
  if (availableEEpromSize >= 0) {
    title += QString(" - %1 ").arg(availableEEpromSize) + tr("free bytes");
  }
  setWindowTitle(title);
}

void MdiChild::setModified()
{
  refresh();
  fileChanged = true;
  documentWasModified();
}

void MdiChild::on_simulateButton_clicked()
{
  radioSimulate();
}

void MdiChild::checkAndInitModel(int row)
{
  ModelData &model = radioData.models[row];
  if (model.isEmpty()) {
    model.setDefaultValues(row, radioData.generalSettings);
    setModified();
  }
}

void MdiChild::generalEdit()
{
  GeneralEdit * t = new GeneralEdit(this, radioData, GetCurrentFirmware()/*firmware*/);
  connect(t, SIGNAL(modified()), this, SLOT(setModified()));
  t->show();
}

void MdiChild::modelEdit()
{
  int row = getCurrentRow();
  QApplication::setOverrideCursor(Qt::WaitCursor);
  checkAndInitModel(row);
  ModelData & model = radioData.models[row];
  gStopwatch.restart();
  gStopwatch.report("ModelEdit creation");
  ModelEdit * t = new ModelEdit(this, radioData, (row), GetCurrentFirmware()/*firmware*/);
  gStopwatch.report("ModelEdit created");
  t->setWindowTitle(tr("Editing model %1: ").arg(row+1) + model.name);
  connect(t, SIGNAL(modified()), this, SLOT(setModified()));
  gStopwatch.report("STARTING MODEL EDIT");
  t->show();
  QApplication::restoreOverrideCursor();
  gStopwatch.report("ModelEdit shown");
}

void MdiChild::wizardEdit()
{
  int row = getCurrentRow();
  checkAndInitModel(row);
  WizardDialog * wizard = new WizardDialog(radioData.generalSettings, row+1, this);
  wizard->exec();
  if (wizard->mix.complete /*TODO rather test the exec() result?*/) {
    radioData.models[row] = wizard->mix;
    setModified();
  }
}

void MdiChild::openModelEditWindow()
{
  int row = getCurrentRow();
  if (row >= 0) {
    ModelData & model = radioData.models[row];
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
        refresh(true);
        if (resetCurrentFile) setCurrentFile(fileName);
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

    refresh(true);
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

    refresh(true);
    if (resetCurrentFile)
      setCurrentFile(fileName);

    free(eeprom);
    return true;
  }
  else if (fileType == FILE_TYPE_OTX) { //read zip archive
    if (!GetEepromInterface()->loadFile(radioData, fileName)) {
      refresh(true);
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

QString MdiChild::userFriendlyCurrentFile() const
{
  return QFileInfo(curFile).fileName();
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
  g.recentFiles(files);
}

void MdiChild::writeEeprom()  // write to Tx
{
  QString tempFile = generateProcessUniqueTempFileName("temp.bin");
  saveFile(tempFile, false);
  if (!QFileInfo(tempFile).exists()) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot write temporary file!"));
    return;
  }
  FlashEEpromDialog * cd = new FlashEEpromDialog(this, tempFile);
  cd->exec();
}

void MdiChild::on_radioSettings_clicked()
{
  generalEdit();
}

void MdiChild::radioSimulate()
{
  startSimulation(this, radioData, -1);
}

void MdiChild::modelSimulate()
{
  startSimulation(this, radioData, getCurrentRow());
}

void MdiChild::print(int model, const QString & filename)
{
  // TODO
  PrintDialog * pd = NULL;

  if (model>=0 && !filename.isEmpty()) {
    pd = new PrintDialog(this, GetCurrentFirmware()/*firmware*/, radioData.generalSettings, radioData.models[model], filename);
  }
  else if (getCurrentRow()) {
    pd = new PrintDialog(this, GetCurrentFirmware()/*firmware*/, radioData.generalSettings, radioData.models[getCurrentRow()]);
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

int MdiChild::getCurrentRow() const
{
  TreeItem * item = modelsListModel->getItem(ui->modelsList->currentIndex());
  return item->getModelIndex();
}

bool MdiChild::loadBackup()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open backup Models and Settings file"), g.eepromDir(), tr(EEPROM_FILES_FILTER));
  if (fileName.isEmpty())
    return false;
  QFile file(fileName);

  if (!file.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("Unable to find file %1!").arg(fileName));
    return false;
  }
  
  int index = getCurrentRow();

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

  refresh(true);

  return true;
}
