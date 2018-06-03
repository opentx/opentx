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

#ifndef _INPUTS_H_
#define _INPUTS_H_

#include "modeledit.h"
#include "mixerslistwidget.h"
#include "modelprinter.h"

class InputsPanel : public ModelPanel
{
    Q_OBJECT

  public:
    InputsPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~InputsPanel();

    virtual void update();

  private slots:
    void clearExpos();
    void moveExpoList(bool down);
    void moveExpoUp();
    void moveExpoDown();
    void mimeExpoDropped(int index, const QMimeData *data, Qt::DropAction action);
    void expolistWidget_customContextMenuRequested(QPoint pos);
    void expolistWidget_doubleClicked(QModelIndex index);
    void expolistWidget_KeyPress(QKeyEvent *event);
    void exposDelete(bool ask=true);
    void exposCut();
    void exposCopy();
    void exposPaste();
    void exposDuplicate();
    void expoOpen(QListWidgetItem *item = NULL);
    void expoAdd();
    void maybeCopyInputName(int srcChan, int destChan);

  private:
    bool expoInserted;
    MixersListWidget *ExposlistWidget;
    int inputsCount;
    ModelPrinter modelPrinter;

    int getExpoIndex(unsigned int dch);
    bool gm_insertExpo(int idx);
    void gm_deleteExpo(int index, bool clearName = true);
    void gm_openExpo(int index);
    int gm_moveExpo(int idx, bool dir);
    void exposDeleteList(QList<int> list, bool clearName = true);
    QList<int> createExpoListFromSelected();
    void setSelectedByExpoList(QList<int> list);
    void pasteExpoMimeData(const QMimeData * mimeData, int destIdx);
    void AddInputLine(int dest);
    QString getInputText(int dest, bool newChan);

};

#endif // _INPUTS_H_
