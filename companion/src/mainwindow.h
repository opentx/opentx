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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>
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

  protected:
    QString getCompanionUpdateBaseUrl();
    void dowloadLastFirmwareUpdate();
    void startFirmwareDownload();
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void setLanguage(const QString & langString);

  private slots:
    void openDocURL();

    void setSysLanguage() { setLanguage("");      };
    void setCZLanguage()  { setLanguage("cs_CZ"); };
    void setDELanguage()  { setLanguage("de_DE"); };
    void setENLanguage()  { setLanguage("en");    };
    void setFILanguage()  { setLanguage("fi_FI"); };
    void setFRLanguage()  { setLanguage("fr_FR"); };
    void setITLanguage()  { setLanguage("it_IT"); };
    void setHELanguage()  { setLanguage("he_IL"); };
    void setPLLanguage()  { setLanguage("pl_PL"); };
    void setESLanguage()  { setLanguage("es_ES"); };
    void setPTLanguage()  { setLanguage("pt_PT"); };
    void setRULanguage()  { setLanguage("ru_RU"); };
    void setSELanguage()  { setLanguage("sv_SE"); };
    void setNLLanguage()  { setLanguage("nl_NL"); };

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
    MdiChild *createMdiChild();
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

    int getFileType(const QString &fullFileName);
    QString Theme;
    QString ISize;
    QString strippedName(const QString &fullFileName);

    MdiChild *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);
    int getEpromVersion(QString fileName);

    bool readEepromFromRadio(const QString &filename);
    bool writeEepromToRadio(const QString &filename);
    bool readFirmwareFromRadio(const QString &filename);
    bool writeFirmwareToRadio(const QString &filename);

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
    QAction *recentAct;
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
    QAction *englishLangAct;
    QAction *czechLangAct;
    QAction *germanLangAct;
    QAction *frenchLangAct;
    QAction *finnishLangAct;
    QAction *italianLangAct;
    QAction *hebrewLangAct;
    QAction *polishLangAct;
    QAction *spanishLangAct;
    QAction *portugueseLangAct;
    QAction *swedishLangAct;
    QAction *russianLangAct;
    QAction *dutchLangAct;
    QAction *openDocURLAct;
};

#endif
