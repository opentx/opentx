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

#include <QtWidgets>
#include "eeprominterface.h"

struct CurrentSelection
{
  QTreeWidgetItem * current_item;
  bool selected[CPN_MAX_MODELS+1];
};


class TreeItem
{
  public:
    explicit TreeItem(const QVector<QVariant> & itemData);
    explicit TreeItem(TreeItem * parent, int modelIndex = -1);
    ~TreeItem();
    
    TreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    TreeItem * appendChild(int modelIndex);
    TreeItem * parent();
    bool removeChildren(int position, int count);
    
    int childNumber() const;
    bool setData(int column, const QVariant &value);
    
    int getModelIndex() const { return modelIndex; }
  
  private:
    QList<TreeItem*> childItems;
    QVector<QVariant> itemData;
    TreeItem * parentItem;
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
    
  public:
  // private:
    TreeItem * getItem(const QModelIndex &index) const;
    TreeItem * rootItem;
    RadioData * radioData;
    int availableEEpromSize;
};

#if 0
class ModelsListWidget : public QTreeView
{
    Q_OBJECT

public:
    ModelsListWidget(QWidget * parent = 0);

    void setRadioData(RadioData * radioData);
    bool hasSelection();
    void keyPressEvent(QKeyEvent * event);
    bool hasPasteData();
    int currentRow() const;

protected:
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
#ifndef WIN32
    void focusInEvent ( QFocusEvent * event );
    void focusOutEvent ( QFocusEvent * event );
#endif
    
public slots:
    void refreshList();
    
    void cut();
    void copy();
    void paste();
    void print();
    void EditModel();
    void LoadBackup();
    void OpenWizard();
    void duplicate();
    void setdefault();
    void deleteSelected(bool ask);
    void confirmDelete();
    void onCurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);

private:
    void doCut(QByteArray *gmData);
    void doPaste(QByteArray *gmData, int index);
    void doCopy(QByteArray *gmData);
    void saveSelection();
    void restoreSelection();

    RadioData * radioData;
    QPoint dragStartPosition;

    CurrentSelection currentSelection;
    QColor active_highlight_color;
    
};
#endif

#endif // _MODELSLIST_H_
