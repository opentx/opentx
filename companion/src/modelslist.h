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

#ifndef _MODELSLIST_H_
#define _MODELSLIST_H_

#include "eeprominterface.h"
#include <QtWidgets>

struct CurrentSelection
{
  QTreeWidgetItem * current_item;
  bool selected[CPN_MAX_MODELS+1];
};

class TreeItem
{
  public:
    explicit TreeItem(const QVector<QVariant> & itemData);
    explicit TreeItem(TreeItem * parent, int categoryIndex, int modelIndex);
    ~TreeItem();
    
    TreeItem * child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    TreeItem * appendChild(int categoryIndex, int modelIndex);
    TreeItem * parent();
    bool removeChildren(int position, int count);
    
    int childNumber() const;
    bool setData(int column, const QVariant &value);
    
    int getModelIndex() const { return modelIndex; }
    int getCategoryIndex() const { return categoryIndex; }
  
  private:
    QList<TreeItem*> childItems;
    QVector<QVariant> itemData;
    TreeItem * parentItem;
    int categoryIndex;
    int modelIndex;
};


class TreeModel : public QAbstractItemModel
{
    // Q_OBJECT
  
  public:
    TreeModel(RadioData * radioData, QObject *parent = 0);
    ~TreeModel();
    
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) Q_DECL_OVERRIDE;
    
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    
    void refresh();
  
    int getAvailableEEpromSize() { return availableEEpromSize; }
    
    int getModelIndex(const QModelIndex & index) const {
      return getItem(index)->getModelIndex();
    }
    
    int getCategoryIndex(const QModelIndex & index) const {
      return getItem(index)->getCategoryIndex();
    }
    
  private:
    TreeItem * getItem(const QModelIndex & index) const;
    TreeItem * rootItem;
    RadioData * radioData;
    int availableEEpromSize;
};

#endif // _MODELSLIST_H_
