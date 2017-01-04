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

TreeItem::TreeItem(const QVector<QVariant> & itemData):
  itemData(itemData),
  parentItem(NULL),
  modelIndex(-1)
{
}

TreeItem::TreeItem(TreeItem * parent, int modelIndex):
  itemData(parent->columnCount()),
  parentItem(parent),
  modelIndex(modelIndex)
{
}

TreeItem::~TreeItem()
{
  qDeleteAll(childItems);
}

TreeItem * TreeItem::child(int number)
{
  return childItems.value(number);
}

int TreeItem::childCount() const
{
  return childItems.count();
}

int TreeItem::childNumber() const
{
  if (parentItem)
    return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));
  
  return 0;
}

int TreeItem::columnCount() const
{
  return itemData.count();
}

QVariant TreeItem::data(int column) const
{
  return itemData.value(column);
}

TreeItem * TreeItem::appendChild(int modelIndex)
{
  TreeItem * item = new TreeItem(this, modelIndex);
  childItems.insert(childItems.size(), item);
  return item;
}

TreeItem * TreeItem::parent()
{
  return parentItem;
}

bool TreeItem::removeChildren(int position, int count)
{
  if (position < 0 || position + count > childItems.size())
    return false;
  
  for (int row = 0; row < count; ++row)
    delete childItems.takeAt(position);
  
  return true;
}

bool TreeItem::setData(int column, const QVariant & value)
{
  if (column < 0 || column >= itemData.size())
    return false;
  
  itemData[column] = value;
  return true;
}

TreeModel::TreeModel(RadioData * radioData, QObject * parent):
  QAbstractItemModel(parent),
  radioData(radioData),
  availableEEpromSize(-1)
{
  BoardEnum board = GetCurrentFirmware()->getBoard();
  QVector<QVariant> labels;
  if (!IS_HORUS(board))
    labels << tr("Index");
  labels << tr("Name");
  if (!(IS_HORUS(board) || IS_SKY9X(board))) {
    labels << tr("Size");
  }
  rootItem = new TreeItem(labels);
  refresh();
}

TreeModel::~TreeModel()
{
  delete rootItem;
}

int TreeModel::columnCount(const QModelIndex & /* parent */) const
{
  return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();
  
  if (role != Qt::DisplayRole && role != Qt::EditRole)
    return QVariant();
  
  TreeItem *item = getItem(index);
  
  return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return 0;
  
  return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

TreeItem * TreeModel::getItem(const QModelIndex &index) const
{
  if (index.isValid()) {
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item)
      return item;
  }
  return rootItem;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return rootItem->data(section);
  
  return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid() && parent.column() != 0)
    return QModelIndex();
  
  TreeItem *parentItem = getItem(parent);
  
  TreeItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();
  
  TreeItem *childItem = getItem(index);
  TreeItem *parentItem = childItem->parent();
  
  if (parentItem == rootItem)
    return QModelIndex();
  
  return createIndex(parentItem->childNumber(), 0, parentItem);
}


bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
  TreeItem *parentItem = getItem(parent);
  bool success = true;
  
  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();
  
  return success;
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
  TreeItem *parentItem = getItem(parent);
  
  return parentItem->childCount();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (role != Qt::EditRole)
    return false;
  
  TreeItem *item = getItem(index);
  bool result = item->setData(index.column(), value);
  
  if (result)
    emit dataChanged(index, index);
  
  return result;
}

void TreeModel::refresh()
{
  EEPROMInterface * eepromInterface = GetEepromInterface();
  BoardEnum board = eepromInterface->getBoard();
  
  if (!IS_SKY9X(board) && !IS_HORUS(board)) {
    availableEEpromSize = eepromInterface->getEEpromSize() - 64; // let's consider fat
    availableEEpromSize -= 16 * ((eepromInterface->getSize(radioData->generalSettings) + 14) / 15);
  }
  
  removeRows(0, rowCount());
  
  for (unsigned int i=0; i<radioData->categories.size(); i++) {
    TreeItem * current = rootItem->appendChild(-1);
    current->setData(0, QString(radioData->categories[i].name));
  }
    
  for (unsigned int i=0; i<(unsigned)GetCurrentFirmware()->getCapability(Models) && i<radioData->models.size(); i++) {
    ModelData & model = radioData->models[i];
    int currentColumn = 0;
    TreeItem * current;
    if (IS_HORUS(board)) {
      if (!model.isEmpty()) {
        current = rootItem->child(model.category)->appendChild(i);
      }
    }
    else {
      current = rootItem->appendChild(i);
      current->setData(currentColumn++, QString().sprintf("%02d", i + 1));
    }
    
    if (!model.isEmpty()) {
      QString modelName;
      if (strlen(model.name) > 0)
        modelName = model.name;
      else
        modelName = QString().sprintf("Model%02d", i+1);
      current->setData(currentColumn++, modelName);
      if (!IS_SKY9X(board) && !IS_HORUS(board)) {
        int size = eepromInterface->getSize(model);
        current->setData(currentColumn++, QString().sprintf("%5d", size));
        size = 16 * ((size + 14) / 15);
        availableEEpromSize -= size;
        if (i == radioData->generalSettings.currModelIndex) {
          // Because we need this space for a TEMP model each time we have to write it again
          availableEEpromSize -= size;
        }
      }
      
      /* TODO if (i == radioData->generalSettings.currModelIndex) {
        QFont font = item->font(0);
        font.setBold(true);
        for (int j=0; j<columnCount(); j++) {
          item->setFont(j, font);
        }
      } */
    }
    // addTopLevelItem(item);
  }
  
  if (!IS_SKY9X(board) && !IS_HORUS(board)) {
    availableEEpromSize = (availableEEpromSize / 16) * 15;
  }
}

#if 0
ModelsListWidget::ModelsListWidget(QWidget * parent):
  QTreeView(parent),
  radioData(NULL)
{
  setColumnWidth(0, 50);
  setColumnWidth(2, 100);
  
  active_highlight_color = palette().color(QPalette::Active, QPalette::Highlight);
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

  QTreeView::mousePressEvent(event);
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
  /*currentSelection.current_item = currentItem();
  for (int i=0; i<GetCurrentFirmware()->getCapability(Models)+1; ++i) {
    currentSelection.selected[i] = selectionModel()->isSelected(model()->index(i, 0));
  }*/
}

void ModelsListWidget::restoreSelection()
{
  /*setCurrentItem(currentSelection.current_item);
  for (int i=0; i<GetCurrentFirmware()->getCapability(Models)+1; ++i) {
    selectionModel()->select(model()->index(i, 0), currentSelection.selected[i] ? QItemSelectionModel::Select : QItemSelectionModel::Deselect);
  }*/
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
      for (int i=row, end=std::min(GetCurrentFirmware()->getCapability(Models)+1, row+header->models_count); i<end; i++)
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
    // setCurrentItem(topLevelItem(row));
    DragDropHeader * header = (DragDropHeader *)gmData.data();
    if (header->general_settings)
      selectionModel()->select(model()->index(0, 0), QItemSelectionModel::Select);
    for (int i=row, end=std::min(GetCurrentFirmware()->getCapability(Models)+1, row+header->models_count); i<end; i++)
      selectionModel()->select(model()->index(i, 0), QItemSelectionModel::Select);
  }
  event->acceptProposedAction();
}

#ifndef WIN32
void ModelsListWidget::focusInEvent ( QFocusEvent * event )
{
  QTreeView::focusInEvent(event);
  QPalette palette = this->palette();
  palette.setColor(QPalette::Active, QPalette::Highlight, active_highlight_color);
  palette.setColor(QPalette::Inactive, QPalette::Highlight, active_highlight_color);
  setPalette(palette);
}

void ModelsListWidget::focusOutEvent ( QFocusEvent * event )
{
  QTreeView::focusOutEvent(event);
  QPalette palette = this->palette();
  palette.setColor(QPalette::Active, QPalette::Highlight, palette.color(QPalette::Active, QPalette::Midlight));
  palette.setColor(QPalette::Inactive, QPalette::Highlight, palette.color(QPalette::Active, QPalette::Midlight));
  setPalette(palette);
}
#endif

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

void ModelsListWidget::duplicate()
{
  int i = this->currentRow();
  if (i && i<GetCurrentFirmware()->getCapability(Models)) {
    ModelData * model = &radioData->models[i-1];
    while (i<GetCurrentFirmware()->getCapability(Models)) {
      if (radioData->models[i].isEmpty()) {
        radioData->models[i] = *model;
        strcpy(radioData->models[i].filename, radioData->getNextModelFilename().toStdString().c_str());
        ((MdiChild *)parent())->setModified();
        break;
      }
      i++;
    }
    if (i==GetCurrentFirmware()->getCapability(Models)) {
      QMessageBox::warning(this, "Companion", tr("No free slot available, cannot duplicate"), QMessageBox::Ok);
    }
  }
}

void ModelsListWidget::onCurrentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem *)
{
  /*int index = indexOfTopLevelItem(current);
  if (!isVisible())
    ((MdiChild*)parent())->viableModelSelected(false);
  else if (index<1)
    ((MdiChild*)parent())->viableModelSelected(false);
  else
    ((MdiChild*)parent())->viableModelSelected(!radioData->models[currentRow()-1].isEmpty()); */
}
#endif