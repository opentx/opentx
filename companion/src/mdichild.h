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

#ifndef _MDICHILD_H_
#define _MDICHILD_H_

#include "eeprominterface.h"
#include "modelslist.h"
#include <QtGui>

class MainWindow;

namespace Ui {
class MdiChild;
}

class MdiChild : public QWidget
{
    // friend class ModelsListWidget;

    Q_OBJECT

  public:
    MdiChild(MainWindow * parent);
    ~MdiChild();

    void newFile();
    bool loadFile(const QString & fileName, bool resetCurrentFile=true);
    bool save();
    bool saveAs(bool isNew=false);
    bool saveFile(const QString & fileName, bool setCurrent=true);
    bool hasModelSelected();
    bool hasPasteData() const;
    QString userFriendlyCurrentFile() const;
    QString currentFile() const { return curFile; }
    int getCurrentModel() const;
    int getCurrentCategory() const;
    void refresh(bool expand=false);
    void keyPressEvent(QKeyEvent * event);

  signals:
    void copyAvailable(bool val);

  protected:
    void convertStorage(Board::Type from, Board::Type to);
    void forceNewFilename(const QString & suffix, const QString & ext);
    void closeEvent(QCloseEvent * event);

  protected slots:
    void documentWasModified();
    void on_simulateButton_clicked();
    void on_radioSettings_clicked();
    void setDefault();
    void onFirmwareChanged();

  public slots:
    void showModelsListContextMenu(const QPoint & pos);
    void checkAndInitModel(int row);
    void generalEdit();
    void categoryAdd();
    void categoryRename();
    void categoryDelete();
    void modelAdd();
    void modelEdit();
    void modelDuplicate();
    void wizardEdit();
    void openModelEditWindow();
    bool loadBackup();
    void confirmDelete();
    void deleteSelectedModels();
    void onDataChanged(const QModelIndex & index);

    void cut();
    void copy();
    void paste();
    void writeEeprom();
    void modelSimulate();
    void radioSimulate();
    void print(int model=-1, const QString & filename="");
    void setModified();
    void updateTitle();
    void onItemSelected(QModelIndex);
    bool isModel(QModelIndex);
    bool isCategory(QModelIndex);

  private:
    bool maybeSave();
    void setCurrentFile(const QString & fileName);
    void doCopy(QByteArray * gmData);
    void doPaste(QByteArray * gmData, QModelIndex row);
    void initModelsList();

    MainWindow * parent;
    Ui::MdiChild * ui;
    TreeModel * modelsListModel;

    QString curFile;

    Firmware * firmware;
    RadioData radioData;

    bool isUntitled;
    bool fileChanged;
};

#endif // _MDICHILD_H_
