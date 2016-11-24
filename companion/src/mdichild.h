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

#include <QtGui>
#include "eeprominterface.h"

namespace Ui {
class mdiChild;
}
#define ER9X_EEPROM_FILE_TYPE        "ER9X_EEPROM_FILE"

#define EEPE_EEPROM_FILE_HEADER  "EEPE EEPROM FILE"
#define EEPE_MODEL_FILE_HEADER   "EEPE MODEL FILE"

class MdiChild : public QWidget
{
    friend class ModelsListWidget;

    Q_OBJECT

  public:
    MdiChild();
    ~MdiChild();

    void newFile();
    bool loadFile(const QString & fileName, bool resetCurrentFile=true);
    bool loadBackup();
    bool save();
    bool saveAs(bool isNew=false);
    bool saveFile(const QString & fileName, bool setCurrent=true);
    bool hasSelection();
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
    bool hasPasteData();
    void viableModelSelected(bool viable);
    void eepromInterfaceChanged();
    void setEEpromAvail(int eavail);
    int getCurrentRow() const;

  signals:
    void copyAvailable(bool val);

  protected:
    void closeEvent(QCloseEvent * event);

  private slots:
    void documentWasModified();
    void on_SimulateTxButton_clicked();
    void qSleep(int ms);

  public slots:
    void checkAndInitModel(int row);
    void generalEdit();
    void modelEdit();
    void wizardEdit();
    void openEditWindow();
    
    void cut();
    void copy();
    void paste();
    void writeEeprom();
    void simulate();
    void print(int model=-1, QString filename="");
    void setModified();
    void updateTitle();

  private:
    bool maybeSave();
    void setCurrentFile(const QString & fileName);
    QString strippedName(const QString & fullFileName);
    bool loadOtxFile(const QString & fileName);

    Ui::mdiChild * ui;

    QString curFile;

    Firmware * firmware;
    RadioData radioData;

    bool isUntitled;
    bool fileChanged;
    int EEPromAvail;
};

#endif // _MDICHILD_H_
