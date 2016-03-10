/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MODELSLIST_H
#define MODELSLIST_H

#include <QtWidgets>
#include "eeprominterface.h"

struct CurrentSelection
{
  QListWidgetItem *current_item;
  bool selected[C9X_MAX_MODELS+1];
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

#endif
