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
    void dowloadLastFirmwareUpdate();
    void startFirmwareDownload();
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void setLanguage(const QString & langString);
    QString seekCodeString(const QByteArray & qba, const QString & label);

  private slots:
    void openDocURL();

    void setSysLanguage() { setLanguage("");      };
    void setCZLanguage()  { setLanguage("cs_CZ"); };
    void setDELanguage()  { setLanguage("de_DE"); };
    void setENLanguage()  { setLanguage("en");    };
    void setFILanguage()  { setLanguage("fi_FI"); };
    void setFRLanguage()  { setLanguage("fr_FR"); };
    void setITLanguage()  { setLanguage("it_IT"); };
    void setPLLanguage()  { setLanguage("pl_PL"); };
    void setESLanguage()  { setLanguage("es_ES"); };
    void setSELanguage()  { setLanguage("sv_SE"); };
    void setCNLanguage()  { setLanguage("zh_CN"); };

    void setTheme(int index);
    void setClassicTheme()   {setTheme(0);};
    void setYericoTheme()    {setTheme(1);};
    void setMonoWhiteTheme() {setTheme(2);};
    void setMonochromeTheme(){setTheme(3);};
    void setMonoBlueTheme()  {setTheme(4);};

    void setIconThemeSize(int index);
    void setSmallIconThemeSize()  {setIconThemeSize(0);};
    void setNormalIconThemeSize() {setIconThemeSize(1);};
    void setBigIconThemeSize()    {setIconThemeSize(2);};
    void setHugeIconThemeSize()   {setIconThemeSize(3);};

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
    MdiChild * createMdiChild();
    void setActiveSubWindow(QWidget *window);
    QMenu * createRecentFileMenu();
    QMenu * createProfilesMenu();
    void autoClose();

  private:
    void closeUpdatesWaitDialog();
    void onUpdatesError();

    void createActions();
    QAction * addAct(const QString &, const QString &, const QString &, enum QKeySequence::StandardKey, const char *, QObject *slotObj=NULL);
    QAction * addAct(const QString &, const QString &, const QString &, const QKeySequence &, const char *, QObject *slotObj=NULL);
    QAction * addAct(QActionGroup *, const QString &, const QString &, const char *);
    QAction * addAct(const QString &, const QString &, const QString &, const char *);

    void createMenus();
    void createToolBars();
    void createStatusBar();
    void updateRecentFileActions();
    void updateProfilesActions();
    void updateIconSizeActions();
    void updateLanguageActions();
    void updateIconThemeActions();
    void openFile(const QString & fileName, bool updateLastUsedDir = false);

    QString strippedName(const QString & fullFileName);

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
    QAction *classicThemeAct;
    QAction *yericoThemeAct;
    QAction *monoThemeAct;
    QAction *monoBlueAct;
    QAction *monoWhiteAct;
    QAction *smallIconAct;
    QAction *normalIconAct;
    QAction *bigIconAct;
    QAction *hugeIconAct;

    QAction *sysLangAct;
    QAction *czechLangAct;
    QAction *germanLangAct;
    QAction *englishLangAct;
    QAction *finnishLangAct;
    QAction *frenchLangAct;
    QAction *italianLangAct;
    QAction *polishLangAct;
    QAction *spanishLangAct;
    QAction *swedishLangAct;
    QAction *chineseLangAct;

    QAction *openDocURLAct;
};

#endif // _MAINWINDOW_H_
