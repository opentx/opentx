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

#ifndef _APPPREFERENCESDIALOG_H_
#define _APPPREFERENCESDIALOG_H_

#include <QDialog>
#include <QCheckBox>
#include "eeprominterface.h"

namespace Ui {
  class AppPreferencesDialog;
}

class Joystick;

class AppPreferencesDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit AppPreferencesDialog(QWidget * parent = 0);
    ~AppPreferencesDialog();

    Joystick * joystick;

  public slots:
    void accept() Q_DECL_OVERRIDE;
    void setMainWinHasDirtyChild(bool value);

  signals:
    void firmwareProfileChanged(int profId);
    void firmwareProfileAboutToChange(bool saveFiles = true);

  private:
    QList<QCheckBox *> optionsCheckBoxes;
    bool updateLock;
    bool mainWinHasDirtyChild;
    void showVoice(bool);
    void showVoice();
    void hideVoice();
    void populateLocale();
    void populateFirmwareOptions(const Firmware *);
    Firmware * getFirmwareVariant();
    QCheckBox * voice;

    Ui::AppPreferencesDialog *ui;
    void initSettings();
    bool displayImage(const QString & fileName);

  protected slots:
    void shrink();
    void baseFirmwareChanged();
    void firmwareOptionChanged(bool state);
    void toggleAppLogSettings();

    void on_libraryPathButton_clicked();
    void on_snapshotPathButton_clicked();
    void on_snapshotClipboardCKB_clicked();
    void on_backupPathButton_clicked();
    void on_ProfilebackupPathButton_clicked();
    void on_ge_pathButton_clicked();

    void on_sdPathButton_clicked();
    void on_SplashSelect_clicked();
    void on_clearImageButton_clicked();
    void on_btn_appLogsDir_clicked();

#if defined(JOYSTICKS)
    void on_joystickChkB_clicked();
    void on_joystickcalButton_clicked();
#endif
};

#endif // _APPPREFERENCESDIALOG_H_
