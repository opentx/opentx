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

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtWidgets>
#include <QDateTime>
#include "downloaddialog.h"
#include "eeprominterface.h"

#define SPLASH_TIME 5
#define MAX_RECENT 15
#define MAX_PROFILES 15

class MdiChild;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
QT_END_NAMESPACE

#define CHECK_COMPANION    1
#define CHECK_FIRMWARE     2
#define SHOW_DIALOG_WAIT   4
#define AUTOMATIC_DOWNLOAD 8

class MainWindow : public QMainWindow
{
    friend class FirmwarePreferencesDialog;

    Q_OBJECT

  public:
    MainWindow();

  signals:
    void FirmwareChanged();

  protected:
    QString getCompanionUpdateBaseUrl();
    QString seekCodeString(const QByteArray & qba, const QString & label);

  protected slots:
    void dowloadLastFirmwareUpdate();
    void startFirmwareDownload();
    virtual void closeEvent(QCloseEvent *event);
    virtual void changeEvent(QEvent *e);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

  private slots:
    void openDocURL();
    void retranslateUi();

    void setLanguage(const QString & langString);
    void onLanguageChanged(QAction * act);
    void setTheme(int index);
    void onThemeChanged(QAction * act);
    void setIconThemeSize(int index);
    void onIconSizeChanged(QAction * act);

    void checkForUpdates();
    void checkForFirmwareUpdate();

    void checkForCompanionUpdateFinished(QNetworkReply * reply);
    void checkForFirmwareUpdateFinished(QNetworkReply * reply);

    void displayWarnings();
    void doAutoUpdates();
    void doUpdates();
    void updateDownloaded();
    void firmwareDownloadAccepted();
    void newFile();
    void openFile();
    void save();
    void saveAs();
    void cut();
    void openRecentFile();
    void loadProfileId(const unsigned pid);
    void loadProfile();
    void logFile();
    void copy();
    void paste();
    void writeEeprom();
    void readEeprom();
    void writeFlash(QString fileToFlash="");
    void readFlash();
    void writeBackup();
    void readBackup();
    void burnConfig();
    void burnList();
    void burnFuses();
    void simulate();
    void contributors();
    void sdsync();
    void changelog();
    void fwchangelog();
    void customizeSplash();
    void about();
    void compare();
    void print();
    void loadBackup();
    void appPrefs();
    void fwPrefs();
    void updateMenus();
    void createProfile();
    void copyProfile();
    void deleteProfile(const int pid);
    void deleteCurrentProfile();
    void setActiveSubWindow(QWidget *window);
    void autoClose();

    void closeUpdatesWaitDialog();
    void onUpdatesError();
    void openFile(const QString & fileName, bool updateLastUsedDir = false);

  private:
    QAction * addAct(const QString &, const QString &, const QString &, enum QKeySequence::StandardKey, const char *, QObject *slotObj=NULL);
    QAction * addAct(const QString &, const QString &, const QString &, const QKeySequence &, const char *, QObject *slotObj=NULL);
    QAction * addAct(const QString &, const QString &, const QString &, const char *);
    QAction * addActToGroup(QActionGroup * aGroup, const QString & sName, const QString & lName,
                            const char * propName = 0, const QVariant & propValue = QVariant(), const QVariant & dfltValue = QVariant());

    QMenu * createLanguageMenu(QWidget * parent = Q_NULLPTR);
    QMenu * createRecentFileMenu();
    QMenu * createProfilesMenu();

    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void updateRecentFileActions();
    void updateProfilesActions();

    int newProfile(bool loadProfile = true);
    QString strippedName(const QString & fullFileName);

    MdiChild * createMdiChild();
    MdiChild * activeMdiChild();
    QMdiSubWindow * findMdiChild(const QString & fileName);

    bool readEepromFromRadio(const QString & filename);
    bool readFirmwareFromRadio(const QString & filename);

    QMdiArea *mdiArea;
    QSignalMapper *windowMapper;

    QString installer_fileName;
    downloadDialog * downloadDialog_forWait;
    unsigned int checkForUpdatesState;
    QString firmwareVersionString;

    QNetworkAccessManager *networkManager;

    QVector<QAction *> actionsList;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *settingsMenu;
    QMenu *burnMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *burnToolBar;
    QToolBar *helpToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *appPrefsAct;
    QAction *fwPrefsAct;
    QAction *checkForUpdatesAct;
    QAction *contributorsAct;
    QAction *sdsyncAct;
    QAction *changelogAct;
    QAction *fwchangelogAct;
    QAction *compareAct;
    QAction *editSplashAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *writeEepromAct;
    QAction *readEepromAct;
    QAction *burnConfigAct;
    QAction *burnListAct;
    QAction *burnFusesAct;
    QAction *writeFlashAct;
    QAction *readFlashAct;
    QAction *writeBackupToRadioAct;
    QAction *readBackupToFileAct;
    QAction *simulateAct;
    QAction *separatorAct;
    QAction *aboutAct;
    QAction *printAct;
    QAction *loadbackupAct;
    QAction *logsAct;
    QAction *recentFileActs[MAX_RECENT];
    QAction *profileActs[MAX_PROFILES];
    QAction *createProfileAct;
    QAction *copyProfileAct;
    QAction *deleteProfileAct;
    QAction *openDocURLAct;
};

#endif // _MAINWINDOW_H_
