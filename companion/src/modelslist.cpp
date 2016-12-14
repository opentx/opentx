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

#include "modelslist.h"
#include "mdichild.h"
#include "helpers.h"

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

ModelsListWidget::ModelsListWidget(QWidget * parent):
  QTreeWidget(parent)
{
  BoardEnum board = GetCurrentFirmware()->getBoard();
  QStringList labels;
  labels << tr("Index") << tr("Name");
  if (!(IS_HORUS(board) || IS_SKY9X(board))) {
    labels << tr("Size");
  }
  setColumnCount(labels.size());
  setHeaderLabels(labels);
  setColumnWidth(0, 50);
  setColumnWidth(2, 100);
  
  connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OpenEditWindow()));
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
  connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(onCurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

  setContextMenuPolicy(Qt::CustomContextMenu);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  setDragEnabled(true);
  setAcceptDrops(true);
  setDragDropOverwriteMode(true);
  setDropIndicatorShown(true);
  if (!IS_HORUS(board)) {
    setIndentation(0);
  }
  
  active_highlight_color = palette().color(QPalette::Active, QPalette::Highlight);

  radioData = &((MdiChild *)parent)->radioData;
  refreshList();

  for (int i=0; i<labels.size(); i++) {
    resizeColumnToContents(i);
  }
}

void ModelsListWidget::ShowContextMenu(const QPoint& pos)
{
  QPoint globalPos = this->mapToGlobal(pos);
  QMenu contextMenu;
  if (((MdiChild *)parent())->getCurrentRow() > 0) {
    // context menu for model
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion");

    contextMenu.addAction(CompanionIcon("edit.png"), tr("&Edit"), this, SLOT(EditModel()));
    contextMenu.addAction(CompanionIcon("open.png"), tr("&Restore from backup"), this, SLOT(LoadBackup()));
    contextMenu.addAction(CompanionIcon("wizard.png"), tr("&Model Wizard"), this, SLOT(OpenWizard()));
    contextMenu.addSeparator();
    contextMenu.addAction(CompanionIcon("clear.png"), tr("&Delete"), this, SLOT(confirmDelete()), tr("Delete"));
    contextMenu.addAction(CompanionIcon("copy.png"), tr("&Copy"), this, SLOT(copy()), tr("Ctrl+C"));
    contextMenu.addAction(CompanionIcon("cut.png"), tr("&Cut"), this, SLOT(cut()), tr("Ctrl+X"));
    contextMenu.addAction(CompanionIcon("paste.png"), tr("&Paste"), this, SLOT(paste()), tr("Ctrl+V"))->setEnabled(hasData);
    contextMenu.addAction(CompanionIcon("duplicate.png"), tr("D&uplicate"), this, SLOT(duplicate()), tr("Ctrl+U"));
    contextMenu.addSeparator();
    contextMenu.addAction(CompanionIcon("currentmodel.png"), tr("&Use as default"), this, SLOT(setdefault()));
    contextMenu.addSeparator();
    contextMenu.addAction(CompanionIcon("print.png"), tr("P&rint model"), this, SLOT(print()),QKeySequence(tr("Ctrl+P")));
    contextMenu.addSeparator();
    contextMenu.addAction(CompanionIcon("simulate.png"), tr("&Simulate model"), this, SLOT(simulate()), tr("Alt+S"));
  }
  else {
    // context menu for radio settings
    contextMenu.addAction(CompanionIcon("edit.png"), tr("&Edit"), this, SLOT(EditModel()));
  }
  contextMenu.exec(globalPos);
}

int ModelsListWidget::currentRow() const
{
  return indexOfTopLevelItem(currentItem());
}

void ModelsListWidget::EditModel()
{
  ((MdiChild *)parent())->modelEdit();
}

void ModelsListWidget::OpenEditWindow()
{
  ((MdiChild *)parent())->openEditWindow();
}

void ModelsListWidget::OpenWizard()
{
  ((MdiChild *)parent())->wizardEdit();
}

void ModelsListWidget::LoadBackup()
{
  ((MdiChild *)parent())->loadBackup();
}

void ModelsListWidget::simulate()
{
  ((MdiChild *)parent())->simulate();
}

void ModelsListWidget::print()
{
  ((MdiChild *)parent())->print();
}

void ModelsListWidget::setdefault()
{
  if (currentRow() > 0) {
    unsigned int currModel = currentRow() - 1;
    if (!radioData->models[currModel].isEmpty() && radioData->generalSettings.currModelIndex != currModel) {
      radioData->setCurrentModel(currModel);
      refreshList();
      ((MdiChild *) parent())->setModified();
    }
  }
}

void ModelsListWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    dragStartPosition = event->pos();

  QTreeWidget::mousePressEvent(event);
}

void ModelsListWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (!(event->buttons() & Qt::LeftButton))
    return;
  
  if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
    return;

  QDrag * drag = new QDrag(this);

  QByteArray gmData;
  doCopy(&gmData);

  QMimeData * mimeData = new QMimeData;
  mimeData->setData("application/x-companion", gmData);

  drag->setMimeData(mimeData);

  //Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);

  //if(dropAction==Qt::MoveAction)

 // QTreeWidget::mouseMoveEvent(event);
}

void ModelsListWidget::saveSelection()
{
  currentSelection.current_item = currentItem();
  for (int i=0; i<GetEepromInterface()->getMaxModels()+1; ++i) {
    currentSelection.selected[i] = selectionModel()->isSelected(model()->index(i, 0));
  }
}

void ModelsListWidget::restoreSelection()
{
  setCurrentItem(currentSelection.current_item);
  for (int i=0; i<GetEepromInterface()->getMaxModels()+1; ++i) {
    selectionModel()->select(model()->index(i, 0), currentSelection.selected[i] ? QItemSelectionModel::Select : QItemSelectionModel::Deselect);
  }
}

void ModelsListWidget::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasFormat("application/x-companion")) {
    event->acceptProposedAction();
    saveSelection();
  }
}

void ModelsListWidget::dragLeaveEvent(QDragLeaveEvent *)
{
  restoreSelection();
}

void ModelsListWidget::dragMoveEvent(QDragMoveEvent *event)
{
  int row = indexAt(event->pos()).row();
  const QMimeData * mimeData = event->mimeData();
  if (mimeData->hasFormat("application/x-companion")) {
    QByteArray gmData = mimeData->data("application/x-companion");
    event->acceptProposedAction();
    clearSelection();
    DragDropHeader * header = (DragDropHeader *)gmData.data();
    if (row >= 0) {
      if (header->general_settings)
        selectionModel()->select(model()->index(0, 0), QItemSelectionModel::Select);
      for (int i=row, end=std::min(GetEepromInterface()->getMaxModels()+1, row+header->models_count); i<end; i++)
        selectionModel()->select(model()->index(i, 0), QItemSelectionModel::Select);
    }
  }
}

void ModelsListWidget::dropEvent(QDropEvent *event)
{
  int row = this->indexAt(event->pos()).row();
  if (row < 0)
    return;

  // QMessageBox::warning(this, tr("Companion"), tr("Index :%1").arg(row));
  const QMimeData * mimeData = event->mimeData();

  if (mimeData->hasFormat("application/x-companion")) {
    QByteArray gmData = mimeData->data("application/x-companion");
    if (event->source() && event->dropAction() == Qt::MoveAction)
      ((ModelsListWidget*)event->source())->doCut(&gmData);
    doPaste(&gmData, row);
    clearSelection();
    setCurrentItem(topLevelItem(row));
    DragDropHeader * header = (DragDropHeader *)gmData.data();
    if (header->general_settings)
      selectionModel()->select(model()->index(0, 0), QItemSelectionModel::Select);
    for (int i=row, end=std::min(GetEepromInterface()->getMaxModels()+1, row+header->models_count); i<end; i++)
      selectionModel()->select(model()->index(i, 0), QItemSelectionModel::Select);
  }
  event->acceptProposedAction();
}

#ifndef WIN32
void ModelsListWidget::focusInEvent ( QFocusEvent * event )
{
  QTreeWidget::focusInEvent(event);
  QPalette palette = this->palette();
  palette.setColor(QPalette::Active, QPalette::Highlight, active_highlight_color);
  palette.setColor(QPalette::Inactive, QPalette::Highlight, active_highlight_color);
  setPalette(palette);
}

void ModelsListWidget::focusOutEvent ( QFocusEvent * event )
{
  QTreeWidget::focusOutEvent(event);
  QPalette palette = this->palette();
  palette.setColor(QPalette::Active, QPalette::Highlight, palette.color(QPalette::Active, QPalette::Midlight));
  palette.setColor(QPalette::Inactive, QPalette::Highlight, palette.color(QPalette::Active, QPalette::Midlight));
  setPalette(palette);
}
#endif

void ModelsListWidget::refreshList()
{
  int current = std::max(0, indexOfTopLevelItem(currentItem()));
  
  clear();
  
  QTreeWidgetItem * item = new QTreeWidgetItem();
  item->setText(1, tr("General Settings"));
  addTopLevelItem(item);

  EEPROMInterface * eepromInterface = GetEepromInterface();
  BoardEnum board = eepromInterface->getBoard();

  // TODO here we calculate the size used by the RLE format, this is clearly not the right place to do that...
  int availableEEpromSize = eepromInterface->getEEpromSize() - 64; // let's consider fat
  availableEEpromSize -= 16 * ((eepromInterface->getSize(radioData->generalSettings) + 14) / 15);
  for (uint8_t i=0; i<GetEepromInterface()->getMaxModels(); i++) {
    QTreeWidgetItem * item = new QTreeWidgetItem();
    item->setTextAlignment(0, Qt::AlignLeft);
    item->setText(0, QString().sprintf("%02d", i+1));
    if (!radioData->models[i].isEmpty()) {
      QString modelName;
      if (strlen(radioData->models[i].name) > 0)
        modelName = radioData->models[i].name;
      else
        modelName = QString().sprintf("Model%02d", i+1);
      item->setText(1, modelName);
      if (!IS_SKY9X(board) && !IS_HORUS(board)) {
        int size = eepromInterface->getSize(radioData->models[i]);
        item->setText(2, QString().sprintf("%5d", size));
        size = 16 * ((size + 14) / 15);
        availableEEpromSize -= size;
        if (i == radioData->generalSettings.currModelIndex) {
          // Because we need this space for a TEMP model each time we have to write it again
          availableEEpromSize -= size;
        }
      }
      if (i == radioData->generalSettings.currModelIndex) {
        QFont font = item->font(0);
        font.setBold(true);
        for (int j=0; j<columnCount(); j++) {
          item->setFont(j, font);
        }
      }
    }
    addTopLevelItem(item);
  }
  
  selectionModel()->select(model()->index(current, 0), QItemSelectionModel::Current | QItemSelectionModel::Select | QItemSelectionModel::Rows);
  setCurrentItem(topLevelItem(current));
  
  if (!IS_SKY9X(board) && !IS_HORUS(board)) {
    ((MdiChild*)parent())->setEEpromAvail((availableEEpromSize/16)*15);
  }
}

void ModelsListWidget::cut()
{
  copy();
  deleteSelected(false);
}

void ModelsListWidget::confirmDelete()
{
  deleteSelected(true);
}


void ModelsListWidget::deleteSelected(bool ask=true)
{
  bool isModel=false;
  unsigned int selModel;
  QMessageBox::StandardButton ret = QMessageBox::Yes;
  if (ask) {
    foreach (QModelIndex index, this->selectionModel()->selectedIndexes()) {
      if (index.row()>0 && !radioData->models[index.row()-1].isEmpty()) {
        isModel = true;
        selModel=index.row()-1;
      }
    }
    if (isModel) {
      if (radioData->generalSettings.currModelIndex != selModel) {
        ret = QMessageBox::warning(this, "Companion", tr("Delete Selected Models?"), QMessageBox::Yes | QMessageBox::No);
      }
      else {
        ret = QMessageBox::warning(this, "Companion", tr("Cannot delete default model."), QMessageBox::Ok);
      }
    }
  }
  if (ret == QMessageBox::Yes) {
    foreach (QModelIndex index, this->selectionModel()->selectedIndexes()) {
      if (index.row() > 0 && radioData->generalSettings.currModelIndex != (unsigned int)(index.row()-1)) {
        radioData->models[index.row()-1].clear();
        ((MdiChild *)parent())->setModified();
      }
      else if (index.row()>0) {
        if (ask) {
          QMessageBox::warning(this, "Companion", tr("Cannot delete default model."), QMessageBox::Ok);
        }
        else {
          QMessageBox::warning(this, "Companion", tr("Cannot cut default model."), QMessageBox::Ok);
        }
      }
    }
  }
}

void ModelsListWidget::doCut(QByteArray * gmData)
{
  bool modified = false;
  DragDropHeader * header = (DragDropHeader *)gmData->data();
  for (int i=0; i<header->models_count; i++) {
    if (radioData->generalSettings.currModelIndex != (unsigned int)header->models[i]-1) {
      radioData->models[header->models[i]-1].clear();
      modified=true;
    }
  }
  if (modified) {
    ((MdiChild *)parent())->setModified();
  }
}

void ModelsListWidget::doCopy(QByteArray * gmData)
{
  DragDropHeader header;
    
  qDebug() << selectionModel()->selectedIndexes();
  foreach(QModelIndex index, selectionModel()->selectedIndexes()) {
    char column = index.column();
    if (column == 0) {
      char row = index.row();
      if (!row) {
        header.general_settings = true;
        gmData->append('G');
        gmData->append((char *) &radioData->generalSettings, sizeof(GeneralSettings));
      }
      else {
        header.models[header.models_count++] = row;
        gmData->append('M');
        gmData->append((char *) &radioData->models[row - 1], sizeof(ModelData));
      }
    }
  }

  gmData->prepend((char *)&header, sizeof(header));
}

void ModelsListWidget::copy()
{
  QByteArray gmData;
  doCopy(&gmData);

  QMimeData * mimeData = new QMimeData;
  mimeData->setData("application/x-companion", gmData);

  QClipboard * clipboard = QApplication::clipboard();
  clipboard->setMimeData(mimeData, QClipboard::Clipboard);
}

void ModelsListWidget::doPaste(QByteArray * gmData, int index)
{
  // QByteArray gmData = mimeD->data("application/x-companion");
  char * gData = gmData->data() + sizeof(DragDropHeader); // new char[gmData.size() + 1];
  int i = sizeof(DragDropHeader);
  int id = index;
  int ret, modified=0;
  if(!id) id++;

  while (i<gmData->size() && id<=GetEepromInterface()->getMaxModels()) {
    qDebug() << i << gmData->size();
    char c = *gData;
    i++;
    gData++;
    if (c == 'G') {
      // General settings
      ret = QMessageBox::question(this, "Companion", tr("Do you want to overwrite radio general settings?"),
              QMessageBox::Yes | QMessageBox::No);
      if (ret == QMessageBox::Yes) {
        radioData->generalSettings = *((GeneralSettings *)gData);
        modified = 1;
      }
      gData += sizeof(GeneralSettings);
      i += sizeof(GeneralSettings);
    }
    else {
      // Model data
      if (!radioData->models[id-1].isEmpty()) {
        ret = QMessageBox::question(this, "Companion", tr("You are pasting on an not empty model, are you sure?"),
                QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes) {
          radioData->models[id-1] = *((ModelData *)gData);
          strcpy(radioData->models[id-1].filename, radioData->getNextModelFilename().toStdString().c_str());
          gData += sizeof(ModelData);
          i += sizeof(ModelData);
          id++;
          modified = 1;
        }
        else {
          gData += sizeof(ModelData);
          i += sizeof(ModelData);
          id++;
        }
      } 
      else {
        radioData->models[id-1] = *((ModelData *)gData);
        strcpy(radioData->models[id-1].filename, radioData->getNextModelFilename().toStdString().c_str());
        gData += sizeof(ModelData);
        i += sizeof(ModelData);
        id++;
        modified=1;
      }
    }
  }
  if (modified==1) {
    ((MdiChild *)parent())->setModified();
  }
}

bool ModelsListWidget::hasPasteData()
{
  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();

  return mimeData->hasFormat("application/x-companion");
}

void ModelsListWidget::paste()
{
  if (hasPasteData()) {
    const QClipboard * clipboard = QApplication::clipboard();
    const QMimeData * mimeData = clipboard->mimeData();

    QByteArray gmData = mimeData->data("application/x-companion");
    doPaste(&gmData, currentRow());
  }
}

void ModelsListWidget::duplicate()
{
  int i = this->currentRow();
  if (i && i<GetEepromInterface()->getMaxModels()) {
    ModelData * model = &radioData->models[i-1];
    while (i<GetEepromInterface()->getMaxModels()) {
      if (radioData->models[i].isEmpty()) {
        radioData->models[i] = *model;
        strcpy(radioData->models[i].filename, radioData->getNextModelFilename().toStdString().c_str());
        ((MdiChild *)parent())->setModified();
        break;
      }
      i++;
    }
    if (i==GetEepromInterface()->getMaxModels()) {
      QMessageBox::warning(this, "Companion", tr("No free slot available, cannot duplicate"), QMessageBox::Ok);
    }
  }
}

bool ModelsListWidget::hasSelection()
{
  return (this->selectionModel()->hasSelection());
}

void ModelsListWidget::keyPressEvent(QKeyEvent *event)
{
  if (event->matches(QKeySequence::Delete)) {
    deleteSelected();
    return;
  }

  if (event->matches(QKeySequence::Cut)) {
    cut();
    return;
  }

  if (event->matches(QKeySequence::Copy)) {
    copy();
    return;
  }

  if (event->matches(QKeySequence::Paste)) {
    paste();
    return;
  }

  if (event->matches(QKeySequence::Underline)) {
    duplicate();
    return;
  }

  QTreeWidget::keyPressEvent(event);//run the standard event in case we didn't catch an action
}

void ModelsListWidget::onCurrentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem *)
{
  int index = indexOfTopLevelItem(current);
  if (!isVisible())
    ((MdiChild*)parent())->viableModelSelected(false);
  else if (index<1)
    ((MdiChild*)parent())->viableModelSelected(false);
  else
    ((MdiChild*)parent())->viableModelSelected(!radioData->models[currentRow()-1].isEmpty());
}
