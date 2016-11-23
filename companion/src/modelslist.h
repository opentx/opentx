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
  QListWidgetItem *current_item;
  bool selected[CPN_MAX_MODELS+1];
};

class ModelsListWidget : public QListWidget
{
    Q_OBJECT

public:
    ModelsListWidget(QWidget *parent = 0);

    bool hasSelection();
    void keyPressEvent(QKeyEvent *event);
    bool hasPasteData();

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
    void ShowContextMenu(const QPoint& pos);
    void cut();
    void copy();
    void paste();
    void print();
    void EditModel();
    void OpenEditWindow();
    void LoadBackup();
    void OpenWizard();
    void simulate();
    void duplicate();
    void setdefault();
    void deleteSelected(bool ask);
    void confirmDelete();
    void viableModelSelected(int idx);

private:
    void doCut(QByteArray *gmData);
    void doPaste(QByteArray *gmData, int index);
    void doCopy(QByteArray *gmData);
    void saveSelection();
    void restoreSelection();

    RadioData *radioData;
    QPoint dragStartPosition;

    CurrentSelection currentSelection;
    QColor active_highlight_color;
    
};

#endif // _MODELSLIST_H_
