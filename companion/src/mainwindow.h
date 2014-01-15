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

#define MAX_RECENT  15
#define MAX_PROFILES  10
#define SPLASH_TIME 5

class MdiChild;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    friend class preferencesDialog;
    friend class MdiChild; // TODO GetAvrdudeArgs could be external to this class

    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);


public slots:
    void downloadLatestFW(FirmwareInfo *firmware, const QString & firmwareId);
    void unloadProfile();
    
private slots:
    void checkForUpdates(bool ignoreSettings, QString & fwId);
    void checkForUpdateFinished(QNetworkReply * reply);
    void displayWarnings();
    void doAutoUpdates();
    void doUpdates();
    void updateDownloaded();
    void reply1Finished(QNetworkReply * reply);
    void reply1Accepted();
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
    void burnTo();
    void burnFrom();
    void burnToFlash(QString fileToFlash="");
    void burnFromFlash();
    void burnExtenalToEEPROM();
    void burnExtenalFromEEPROM();
    void burnConfig();
    void burnList();
    void burnFuses();
    void simulate();
    void contributors();
    void changelog();
    void fwchangelog();
    void customizeSplash();
    void about();
    void compare();
    void print();
    void loadBackup();
    void preferences();
    void updateMenus();
    void updateWindowMenu();
    MdiChild *createMdiChild();
    void switchLayoutDirection();
    void setActiveSubWindow(QWidget *window);
    QMenu * createRecentFileMenu();
    QMenu * createProfilesMenu();
    void autoClose();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    void updateRecentFileActions();
    void updateProfilesActions();
    int getFileType(const QString &fullFileName);
    QString FindTaranisPath();
    QString strippedName(const QString &fullFileName);

    MdiChild *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);
    QString GetAvrdudeLocation();
    QStringList GetAvrdudeArguments(const QString &cmd, const QString &filename);
    QStringList GetSambaArguments(const QString &tcl);
    QStringList GetDFUUtilArguments(const QString &cmd, const QString &filename);
    QStringList GetReceiveEEpromCommand(const QString &filename);
    QStringList GetSendEEpromCommand(const QString &filename);
    QStringList GetReceiveFlashCommand(const QString &filename);
    QStringList GetSendFlashCommand(const QString &filename);
    int getEpromVersion(QString fileName);


    bool convertEEPROM(QString backupFile, QString restoreFile, QString flashFile);
    bool isValidEEPROM(QString eepromfile);

    QMdiArea *mdiArea;
    QSignalMapper *windowMapper;

    QString installer_fileName;
    QString downloadedFW;
    QString downloadedFWFilename;
    QString ActiveProfileName;
    downloadDialog * downloadDialog_forWait;

    bool checkCompanion9x;
    bool checkFW;
    bool needRename;
    bool showcheckForUpdatesResult;
    int MaxRecentFiles;
    int ActiveProfile;
    int currentFWrev;
    int currentFWrev_temp;
    int NewFwRev;
    bool check1done;
    bool check2done;
    
    QNetworkAccessManager *manager1;
    QNetworkAccessManager *manager2;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *burnMenu;
    QMenu *windowMenu;
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
    QAction *preferencesAct;
    QAction *checkForUpdatesAct;
    QAction *contributorsAct;
    QAction *changelogAct;
    QAction *fwchangelogAct;
    QAction *compareAct;
    QAction *customizeSplashAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *burnToAct;
    QAction *burnFromAct;
    QAction *burnConfigAct;
    QAction *burnListAct;
    QAction *burnFusesAct;
    QAction *burnToFlashAct;
    QAction *burnFromFlashAct;
    QAction *burnExtenalToEEPROMAct;
    QAction *burnExtenalFromEEPROMAct;
    QAction *simulateAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;
    QAction *aboutAct;
    QAction *printAct;
    QAction *loadbackupAct;
    QAction *logsAct;
    QAction *switchLayoutDirectionAct;
    QAction *recentFileActs[MAX_RECENT];
    QAction *profileActs[MAX_PROFILES];
    QString fwToUpdate;
    QToolButton * profileButton;
};

#endif
