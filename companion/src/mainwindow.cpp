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

#include <QtGui>
#include <QNetworkProxyFactory>
#include <QFileInfo>
#include <QDesktopServices>
#include "mainwindow.h"
#include "mdichild.h"
#include "burnconfigdialog.h"
#include "avroutputdialog.h"
#include "comparedialog.h"
#include "logsdialog.h"
#include "apppreferencesdialog.h"
#include "fwpreferencesdialog.h"
#include "flashinterface.h"
#include "fusesdialog.h"
#include "downloaddialog.h"
#include "printdialog.h"
#include "version.h"
#include "contributorsdialog.h"
#include "customizesplashdialog.h"
#include "burndialog.h"
#include "hexinterface.h"
#include "warnings.h"
#include "helpers.h"
#include "appdata.h"
#include "radionotfound.h"

#define OPENTX_COMPANION_DOWNLOADS   "http://downloads-20.open-tx.org/companion"
#define DONATE_STR      "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=QUZ48K4SEXDP2"

#ifdef __APPLE__
  #define C9X_STAMP     OPENTX_COMPANION_DOWNLOADS "/companion-macosx.stamp"
  #define C9X_INSTALLER "/CompanionMacUpdate.%1.pkg.zip"
#else
  #define C9X_STAMP     OPENTX_COMPANION_DOWNLOADS "/companion-windows.stamp"
  #define C9X_INSTALLER "/companionInstall_%1.exe"
#endif

#if defined WIN32 || !defined __GNUC__
  #include <windows.h>
  #define sleep(x) Sleep(x*1000)
#else
  #include <unistd.h>
  #include "mountlist.h"
#endif

MainWindow::MainWindow():
  downloadDialog_forWait(NULL),
  checkForUpdatesState(0)
{
    mdiArea = new QMdiArea(this);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    restoreState(g.mainWinState());
    restoreGeometry(g.mainWinGeo());

    // setUnifiedTitleAndToolBarOnMac(true);
    this->setWindowIcon(QIcon(":/icon.png"));
    this->setIconSize(QSize(32, 32));
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    setAcceptDrops(true);
    
    // give time to the splash to disappear and main window to open before starting updates
    int updateDelay = 1000;
    bool showSplash = g.showSplash();
    if (showSplash) {
      updateDelay += (SPLASH_TIME*1000); 
    }
    QTimer::singleShot(updateDelay, this, SLOT(doAutoUpdates()));
    QTimer::singleShot(updateDelay, this, SLOT(displayWarnings()));

    QStringList strl = QApplication::arguments();
    QString str;
    QString printfilename;
    int printing=false;
    int model=-1;
    if (strl.contains("--print"))
      printing=true;
    int count=0;
    foreach(QString arg, strl) {
      count++;
      if (arg.contains("--model")) {
        model=strl[count].toInt()-1;
      }
      if (arg.contains("--filename")) {
        printfilename=strl[count];
      }
    }
    if (strl.count()>1) str = strl[1];
    if (!str.isEmpty()) {
      int fileType = getFileType(str);

      if (fileType==FILE_TYPE_HEX) {
        writeFlash(str);
      }

      if (fileType==FILE_TYPE_EEPE || fileType==FILE_TYPE_EEPM || fileType==FILE_TYPE_BIN) {
        MdiChild *child = createMdiChild();
        if (child->loadFile(str)) {
          if (!(printing && (model >=0 && model<GetEepromInterface()->getMaxModels()) && !printfilename.isEmpty()  )) {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
          }
          else {
            child->show();            
            child->print(model,printfilename);
            child->close();
          }
        }
      }
    } 
    if (printing) {
      QTimer::singleShot(0, this, SLOT(autoClose()));
    }
}

void MainWindow::displayWarnings()
{
  int warnId = g.warningId();
  if (warnId<WARNING_ID && warnId!=0) {
    int res=0;
    if (WARNING_LEVEL>0)
      QMessageBox::warning(this, "Companion", WARNING);
    else
      QMessageBox::about(this, "Companion", WARNING);
    res = QMessageBox::question(this, "Companion", tr("Display previous warning again at startup ?"), QMessageBox::Yes | QMessageBox::No);
    if (res == QMessageBox::No) {
      g.warningId(WARNING_ID);
    }
  }
  else if (warnId==0) {
    g.warningId(WARNING_ID);    
  }
}

void MainWindow::doAutoUpdates()
{
  if (g.autoCheckApp())
    checkForUpdatesState |= CHECK_COMPANION;
  if (g.autoCheckFw())
    checkForUpdatesState |= CHECK_FIRMWARE;
  checkForUpdates();
}

void MainWindow::doUpdates()
{
  checkForUpdatesState = CHECK_COMPANION | CHECK_FIRMWARE | SHOW_DIALOG_WAIT;
  checkForUpdates();
}

void MainWindow::checkForFirmwareUpdate()
{
  checkForUpdatesState = CHECK_FIRMWARE | SHOW_DIALOG_WAIT;
  checkForUpdates();
}

void MainWindow::dowloadLastFirmwareUpdate()
{
  checkForUpdatesState = CHECK_FIRMWARE | AUTOMATIC_DOWNLOAD | SHOW_DIALOG_WAIT;
  checkForUpdates();
}

void MainWindow::checkForUpdates()
{
  if (checkForUpdatesState & SHOW_DIALOG_WAIT) {
    checkForUpdatesState -= SHOW_DIALOG_WAIT;
    downloadDialog_forWait = new downloadDialog(NULL, tr("Checking for updates"));
    downloadDialog_forWait->show();
  }

  if (checkForUpdatesState & CHECK_COMPANION) {
    checkForUpdatesState -= CHECK_COMPANION;
    // TODO why create each time a network manager?
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),this, SLOT(checkForCompanionUpdateFinished(QNetworkReply*)));
    QNetworkRequest request(QUrl(C9X_STAMP));
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    networkManager->get(request);
  }
  else if (checkForUpdatesState & CHECK_FIRMWARE) {
    checkForUpdatesState -= CHECK_FIRMWARE;
    QString stamp = GetCurrentFirmware()->getStampUrl();
    if (!stamp.isEmpty()) {
      networkManager = new QNetworkAccessManager(this);
      connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(checkForFirmwareUpdateFinished(QNetworkReply*)));
      QUrl url(stamp);
      QNetworkRequest request(url);
      request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
      networkManager->get(request);
    }
  }
  else if (checkForUpdatesState==0) {
    closeUpdatesWaitDialog();
  }
}

void MainWindow::onUpdatesError()
{
  checkForUpdatesState = 0;
  closeUpdatesWaitDialog();
  QMessageBox::warning(this, "Companion", tr("Unable to check for updates."));
}

void MainWindow::closeUpdatesWaitDialog()
{
  if (downloadDialog_forWait) {
    downloadDialog_forWait->close();
    delete downloadDialog_forWait;
    downloadDialog_forWait = NULL;
  }
}

void MainWindow::checkForCompanionUpdateFinished(QNetworkReply * reply)
{
    QByteArray qba = reply->readAll();
    int i = qba.indexOf("C9X_VERSION");
    if (i>0) {
      QString version = qba.mid(i+14, qba.indexOf("\"", i+14)-i-14);

      if (version.isNull()) {
        return onUpdatesError();
      }

      int vnum = version2index(version);

      QString c9xversion = QString(C9X_VERSION);
      int c9xver = version2index(c9xversion);

      if (c9xver < vnum) {
#if defined WIN32 || !defined __GNUC__ // || defined __APPLE__  // OSX should only notify of updates since no update packages are available. 
        int ret = QMessageBox::question(this, "Companion", tr("A new version of Companion is available (version %1)<br>"
                                                            "Would you like to download it?").arg(version) ,
                                        QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::Yes) {
#if defined __APPLE__          
          QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), g.updatesDir() + QString(C9X_INSTALLER).arg(version));
#else            
          QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), g.updatesDir() + QString(C9X_INSTALLER).arg(version), tr("Executable (*.exe)"));
#endif
          if (!fileName.isEmpty()) {
            g.updatesDir(QFileInfo(fileName).dir().absolutePath());
            downloadDialog * dd = new downloadDialog(this, QString(OPENTX_COMPANION_DOWNLOADS C9X_INSTALLER).arg(version), fileName);
            installer_fileName = fileName;
            connect(dd, SIGNAL(accepted()), this, SLOT(updateDownloaded()));
            dd->exec();
          }
        }
#else
        QMessageBox::warning(this, tr("New release available"), tr("A new release of Companion is available, please check the OpenTX website!"));
#endif            
      }
      else {
        if (downloadDialog_forWait && checkForUpdatesState==0) {
          QMessageBox::information(this, "Companion", tr("No updates available at this time."));
        }
      }
    }
    else {
      return onUpdatesError();
    }

    checkForUpdates();
}

void MainWindow::updateDownloaded()
{
    int ret = QMessageBox::question(this, "Companion", tr("Would you like to launch the installer?") ,
                                     QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
      if (QDesktopServices::openUrl(QUrl::fromLocalFile(installer_fileName)))
        QApplication::exit();
    }
}

void MainWindow::firmwareDownloadAccepted()
{
    QString errormsg;
    QFile file(g.profile[g.id()].fwName());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
      QMessageBox::critical(this, tr("Error"),
          tr("Error opening file %1:\n%2.")
          .arg(g.profile[g.id()].fwName())
          .arg(file.errorString()));
      return;
    }
    file.reset();
    QTextStream inputStream(&file);
    QString hline = inputStream.readLine();
    if (hline.startsWith("ERROR:")) {
      int errnum=hline.mid(6).toInt();
      switch(errnum) {
        case 1:
          errormsg = tr("Not enough memory for all the selected firmware options");
          break;
        case 2:
          errormsg = tr("Compilation server termporary failure, try later");
          break;
        case 3:
          errormsg = tr("Compilation server too busy, try later");
          break;
        case 4:
          errormsg = tr("Compilation server requires registration, please check OpenTX web site");
          break;
        default:
          errormsg = tr("Unknown server failure, try later");
          break;
      }
      file.close();
      QMessageBox::critical(this, tr("Error"), errormsg);
      return;
    }
    file.close();
    g.fwRev.set(current_firmware_variant->getId(), version2index(firmwareVersionString));
    if (g.profile[g.id()].burnFirmware()) {
      int ret = QMessageBox::question(this, "Companion", tr("Do you want to write the firmware to the radio now ?"), QMessageBox::Yes | QMessageBox::No);
      if (ret == QMessageBox::Yes) {
        writeFlash(g.profile[g.id()].fwName());
      }
    }
}

void MainWindow::checkForFirmwareUpdateFinished(QNetworkReply * reply)
{
    bool download = false;
    bool ignore = false;

    QByteArray qba = reply->readAll();

    long version = 0;

    int versionIndex = qba.indexOf("VERS_STR");
    int dateIndex = qba.indexOf("DATE_STR");

    QString versionString;
    if (versionIndex > 0 && dateIndex > 0) {
      firmwareVersionString = qba.mid(versionIndex+10, qba.indexOf("\"", versionIndex+10)-versionIndex-10);
      QString dateString = qba.mid(dateIndex+10, 10);
      version = version2index(firmwareVersionString);
      versionString = QString("%1 (%2)").arg(firmwareVersionString).arg(dateString);
    }

    if (version > 0) {
      if (checkForUpdatesState & AUTOMATIC_DOWNLOAD) {
        checkForUpdatesState -= AUTOMATIC_DOWNLOAD;
        download = true;
      }
      else {
        int currentVersion = g.fwRev.get(current_firmware_variant->getId());
        QString currentVersionString = index2version(currentVersion);

        QMessageBox msgBox;
        QSpacerItem * horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout * layout = (QGridLayout*)msgBox.layout();
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

        if (currentVersion == 0) {
          QString rn = GetCurrentFirmware()->getReleaseNotesUrl();
          QAbstractButton *rnButton = NULL;
          msgBox.setWindowTitle("Companion");
          msgBox.setInformativeText(tr("Firmware %1 does not seem to have ever been downloaded.\nRelease %2 is available.\nDo you want to download it now?").arg(current_firmware_variant->getId()).arg(versionString));
          QAbstractButton *YesButton = msgBox.addButton(trUtf8("Yes"), QMessageBox::YesRole);
          msgBox.addButton(trUtf8("No"), QMessageBox::NoRole);
          if (!rn.isEmpty()) {
            rnButton = msgBox.addButton(trUtf8("Release Notes"), QMessageBox::ActionRole);
          }
          msgBox.setIcon(QMessageBox::Question);
          msgBox.resize(0, 0);
          msgBox.exec();
          if (msgBox.clickedButton() == rnButton) {
            contributorsDialog *cd = new contributorsDialog(this,2,rn);
            cd->exec();
            int ret2 = QMessageBox::question(this, "Companion", tr("Do you want to download release %1 now ?").arg(versionString), QMessageBox::Yes | QMessageBox::No);
            if (ret2 == QMessageBox::Yes)
              download = true;
            else
              ignore = true;
          }
          else if (msgBox.clickedButton() == YesButton ) {
            download = true;
          }
          else {
            ignore = true;
          }
        }
        else if (version > currentVersion) {
          QString rn = GetCurrentFirmware()->getReleaseNotesUrl();
          QAbstractButton *rnButton = NULL;
          msgBox.setText("Companion");
          msgBox.setInformativeText(tr("A new version of %1 firmware is available:\n  - current is %2\n  - newer is %3\n\nDo you want to download it now ?").arg(current_firmware_variant->getId()).arg(currentVersionString).arg(versionString));
          QAbstractButton *YesButton = msgBox.addButton(trUtf8("Yes"), QMessageBox::YesRole);
          msgBox.addButton(trUtf8("No"), QMessageBox::NoRole);
          if (!rn.isEmpty()) {
            rnButton = msgBox.addButton(trUtf8("Release Notes"), QMessageBox::ActionRole);
          }
          msgBox.setIcon(QMessageBox::Question);
          msgBox.resize(0,0);
          msgBox.exec();
          if( msgBox.clickedButton() == rnButton ) {
            contributorsDialog *cd = new contributorsDialog(this, 2, rn);
            cd->exec();
            int ret2 = QMessageBox::question(this, "Companion", tr("Do you want to download release %1 now ?").arg(versionString),
                  QMessageBox::Yes | QMessageBox::No);
            if (ret2 == QMessageBox::Yes) {
              download = true;
            }
            else {
              ignore = true;
            }
          }
          else if (msgBox.clickedButton() == YesButton ) {
            download = true;
          }
          else {
            ignore = true;
          }
        }
        else {
          if (downloadDialog_forWait && checkForUpdatesState==0) {
            QMessageBox::information(this, "Companion", tr("No updates available at this time."));
          }
        }
      }

      if (ignore) {
        int res = QMessageBox::question(this, "Companion", tr("Ignore this release %1?").arg(versionString), QMessageBox::Yes | QMessageBox::No);
        if (res==QMessageBox::Yes)   {
          g.fwRev.set(current_firmware_variant->getId(), version);
        }
      }
      else if (download == true) {
        startFirmwareDownload();
      }
    }
    else {
      // TODO remove serverFails config?
      return onUpdatesError();
    }

    checkForUpdates();
}

void MainWindow::startFirmwareDownload()
{
  QString url = current_firmware_variant->getFirmwareUrl();
  qDebug() << url;
  QString ext = url.mid(url.lastIndexOf("."));
  QString defaultFilename = g.flashDir() + "/" + current_firmware_variant->getId();
  if (g.profile[g.id()].renameFwFiles()) {
    defaultFilename += "-" + firmwareVersionString;
  }
  defaultFilename += ext;

  QString filename = QFileDialog::getSaveFileName(this, tr("Save As"), defaultFilename);
  if (!filename.isEmpty()) {
    g.profile[g.id()].fwName(filename);
    g.flashDir(QFileInfo(filename).dir().absolutePath());
    downloadDialog * dd = new downloadDialog(this, url, filename);
    connect(dd, SIGNAL(accepted()), this, SLOT(firmwareDownloadAccepted()));
    dd->exec();
  }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    g.mainWinGeo(saveGeometry());
    g.mainWinState(saveState());
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
      event->ignore();
    }
    else {
      event->accept();
    }
}

void MainWindow::setLanguage(QString langString)
{    
    g.locale( langString );
    
    QMessageBox msgBox;
    msgBox.setText(tr("The selected language will be used the next time you start Companion."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    msgBox.exec();
}

void  MainWindow::setTheme(int index)
{    
    g.theme( index );
    
    QMessageBox msgBox;
    msgBox.setText(tr("The new theme will be loaded the next time you start Companion."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    msgBox.exec();
}

void  MainWindow::setIconThemeSize(int index)
{    
    g.iconSize( index );

    QMessageBox msgBox;
    msgBox.setText(tr("The icon size will be used the next time you start Companion."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    msgBox.exec();
}

void MainWindow::newFile()
{
    MdiChild * child = createMdiChild();
    child->newFile();
    child->show();
}

void MainWindow::openDocURL()
{
  QString link = "http://www.open-tx.org/documents.html";
  QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Models and Settings file"), g.eepromDir(), tr(EEPROM_FILES_FILTER));
    if (!fileName.isEmpty()) {
      g.eepromDir(QFileInfo(fileName).dir().absolutePath());

      QMdiSubWindow *existing = findMdiChild(fileName);
      if (existing) {
        mdiArea->setActiveSubWindow(existing);
        return;
      }

      MdiChild *child = createMdiChild();
      if (child->loadFile(fileName)) {
        statusBar()->showMessage(tr("File loaded"), 2000);
        child->show();
      }
    }
}

void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->save())
      statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    if (activeMdiChild() && activeMdiChild()->saveAs())
      statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::openRecentFile()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) {
    QString fileName = action->data().toString();

    QMdiSubWindow *existing = findMdiChild(fileName);
    if (existing) {
      mdiArea->setActiveSubWindow(existing);
    }
    else {
      MdiChild *child = createMdiChild();
      if (child->loadFile(fileName)) {
        statusBar()->showMessage(tr("File loaded"), 2000);
        child->show();
      }
    }
  }
}

void MainWindow::loadProfile()  //TODO Load all variables - Also HW!
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
      // Set the new profile number
      int profnum=action->data().toInt();
      g.id( profnum );

      current_firmware_variant = GetFirmware(g.profile[g.id()].fwType());

      foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        mdiChild->eepromInterfaceChanged();
      }

      updateMenus();
    }
}

void MainWindow::appPrefs()
{
    AppPreferencesDialog *pd = new AppPreferencesDialog(this);
    pd->exec();
    updateMenus();
}

void MainWindow::fwPrefs()
{
    FirmwarePreferencesDialog *pd = new FirmwarePreferencesDialog(this);
    pd->exec();
    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
      MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
      mdiChild->eepromInterfaceChanged();
    }
    updateMenus();
}

void MainWindow::contributors()
{
    contributorsDialog *cd = new contributorsDialog(this,0);
    cd->exec();
}

void MainWindow::changelog()
{
    contributorsDialog *cd = new contributorsDialog(this,1);
    cd->exec();
}

void MainWindow::fwchangelog()
{
    FirmwareInterface *currfirm = GetCurrentFirmware();
    QString rn=currfirm->getReleaseNotesUrl();
    if (rn.isEmpty()) {
      QMessageBox::information(this, tr("Firmware updates"), tr("Current firmware does not provide release notes informations."));
    }
    else {
      contributorsDialog *cd = new contributorsDialog(this,2, rn);
      cd->exec();
    }
}

void MainWindow::customizeSplash()
{    
    customizeSplashDialog *cd = new customizeSplashDialog(this);
    cd->exec();
}

void MainWindow::cut()
{
    if (activeMdiChild()) {
      activeMdiChild()->cut();
      updateMenus();
    }
}

void MainWindow::copy()
{
    if (activeMdiChild()) {
      activeMdiChild()->copy();
      updateMenus();
    }
}

void MainWindow::paste()
{
    if (activeMdiChild())
      activeMdiChild()->paste();
}

void MainWindow::writeEeprom()
{
    if (activeMdiChild())
      activeMdiChild()->writeEeprom();
}

void MainWindow::simulate()
{
    if (activeMdiChild())
      activeMdiChild()->simulate();
}


void MainWindow::print()
{
    if (activeMdiChild())
      activeMdiChild()->print();
}

void MainWindow::loadBackup()
{
    if (activeMdiChild())
      activeMdiChild()->loadBackup();
}

QString MainWindow::GetAvrdudeLocation()
{
    burnConfigDialog bcd;
    EEPROMInterface *eepromInterface = GetEepromInterface();
    if (IS_TARANIS(eepromInterface->getBoard())) {
      return bcd.getDFU();
    } else if (IS_SKY9X(GetEepromInterface()->getBoard())) {
      return bcd.getSAMBA();
    } else {
      return bcd.getAVRDUDE();
    }
}

QStringList MainWindow::GetAvrdudeArguments(const QString &cmd, const QString &filename)
{
    QStringList arguments;

    burnConfigDialog bcd;
    QString programmer = bcd.getProgrammer();
    QStringList args   = bcd.getAVRArgs();
    QString mcu   = bcd.getMCU();

    if(!bcd.getPort().isEmpty()) args << "-P" << bcd.getPort();

    arguments << "-c" << programmer << "-p";
    if (GetEepromInterface()->getBoard() == BOARD_GRUVIN9X)
      arguments << "m2560";
    else if (GetEepromInterface()->getBoard() == BOARD_M128)
      arguments << "m128";
    else
      arguments << mcu;

    arguments << args;

    QString fullcmd = cmd + filename;
    if(QFileInfo(filename).suffix().toUpper()=="HEX") fullcmd += ":i";
    else if(QFileInfo(filename).suffix().toUpper()=="BIN") fullcmd += ":r";
    else fullcmd += ":a";

    arguments << "-U" << fullcmd;

    return arguments;
}

QStringList MainWindow::GetDFUUtilArguments(const QString &cmd, const QString &filename)
{
    QStringList arguments;
    burnConfigDialog bcd;
    QStringList args   = bcd.getDFUArgs();
    QString memory="0x08000000";
    if (cmd=="-U") {
      memory.append(QString(":%1").arg(MAX_FSIZE));
    }
    arguments << args << "--dfuse-address" << memory << "-d" << "0483:df11";
    QString fullcmd = cmd + filename;

    arguments << "" << fullcmd;

    return arguments;
}

QStringList MainWindow::GetSambaArguments(const QString &tcl)
{
    QStringList result;

    QString tclFilename = QDir::tempPath() + "/temp.tcl";
    if (QFile::exists(tclFilename)) {
      unlink(tclFilename.toAscii());
    }
    QFile tclFile(tclFilename);
    if (!tclFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QMessageBox::warning(this, tr("Error"),
          tr("Cannot write file %1:\n%2.")
          .arg(tclFilename)
          .arg(tclFile.errorString()));
      return result;
    }

    QTextStream outputStream(&tclFile);
    outputStream << tcl;

    burnConfigDialog bcd;
    result << bcd.getSambaPort() << bcd.getArmMCU() << tclFilename ;
    return result;

}

QStringList MainWindow::GetReceiveEEpromCommand(const QString &filename)
{
    QStringList ret;
    EEPROMInterface *eepromInterface = GetEepromInterface();
    if (IS_TARANIS(eepromInterface->getBoard())) {
      // impossible
    }
    else if (IS_SKY9X(eepromInterface->getBoard())) {
      ret=GetSambaArguments(QString("SERIALFLASH::Init 0\n") + "receive_file {SerialFlash AT25} \"" + filename + "\" 0x0 0x80000 0\n");
    }
    else {
      ret=GetAvrdudeArguments("eeprom:r:", filename);
    }
    return ret;
}

QStringList MainWindow::GetSendEEpromCommand(const QString &filename)
{
    QStringList ret;
    EEPROMInterface *eepromInterface = GetEepromInterface();
    if (IS_TARANIS(eepromInterface->getBoard())) {
      // impossible
    }
    else if (IS_SKY9X(eepromInterface->getBoard())) {
      ret=GetSambaArguments(QString("SERIALFLASH::Init 0\n") + "send_file {SerialFlash AT25} \"" + filename + "\" 0x0 0\n");
    }
    else {
      ret=GetAvrdudeArguments("eeprom:w:", filename);
    }
    return ret;
}

QStringList MainWindow::GetSendFlashCommand(const QString &filename)
{
    QStringList ret;
    EEPROMInterface *eepromInterface = GetEepromInterface();
    if (IS_TARANIS(eepromInterface->getBoard())) {
      ret=GetDFUUtilArguments("-D", filename);    
    }
    else if (eepromInterface->getBoard() == BOARD_SKY9X) {
      ret=GetSambaArguments(QString("send_file {Flash} \"") + filename + "\" 0x400000 0\n" + "FLASH::ScriptGPNMV 2\n");
    }
    else if (eepromInterface->getBoard() == BOARD_9XRPRO) {
      ret=GetSambaArguments(QString("send_file {Flash} \"") + filename + "\" 0x400000 0\n" + "FLASH::ScriptGPNMV 2\n");
    }
    else {
      ret=GetAvrdudeArguments("flash:w:", filename);
    }
    return ret;
}

QStringList MainWindow::GetReceiveFlashCommand(const QString &filename)
{
    EEPROMInterface *eepromInterface = GetEepromInterface();
    if (IS_TARANIS(eepromInterface->getBoard())) {
      return GetDFUUtilArguments("-U", filename);    
    }
    else if (eepromInterface->getBoard() == BOARD_SKY9X) {
      return GetSambaArguments(QString("receive_file {Flash} \"") + filename + "\" 0x400000 0x40000 0\n");
    }
    else if (eepromInterface->getBoard() == BOARD_9XRPRO) {
      return GetSambaArguments(QString("receive_file {Flash} \"") + filename + "\" 0x400000 0x80000 0\n");
    }
    else {
      return GetAvrdudeArguments("flash:r:", filename);
    }
}

QString MainWindow::FindMassstoragePath(QString filename)
{
    QString temppath;
    QStringList drives;
    QString eepromfile;
    QString fsname;
#if defined WIN32 || !defined __GNUC__
    foreach( QFileInfo drive, QDir::drives() ) {
      WCHAR szVolumeName[256] ;
      WCHAR szFileSystemName[256];
      DWORD dwSerialNumber = 0;
      DWORD dwMaxFileNameLength=256;
      DWORD dwFileSystemFlags=0;
      bool ret = GetVolumeInformationW( (WCHAR *) drive.absolutePath().utf16(),szVolumeName,256,&dwSerialNumber,&dwMaxFileNameLength,&dwFileSystemFlags,szFileSystemName,256);
      if (ret) {
        QString vName = QString::fromUtf16 ( (const ushort *) szVolumeName) ;
        temppath = drive.absolutePath();
        eepromfile = temppath;
        eepromfile.append("/" + filename);
        if (QFile::exists(eepromfile)) {
          return eepromfile;
        }
      }
    }
#else
    struct mount_entry *entry;
    entry = read_file_system_list(true);
    while (entry != NULL) {
      if (!drives.contains(entry->me_devname)) {
        drives.append(entry->me_devname);
        temppath = entry->me_mountdir;
        eepromfile = temppath;
        eepromfile.append("/" + filename);
#if !defined __APPLE__
        QString fstype = entry->me_type;
        if (QFile::exists(eepromfile) && fstype.contains("fat") ) {
#else
        if (QFile::exists(eepromfile)) {
#endif
          return eepromfile;
        }
      }
      entry = entry->me_next;
    }
#endif

    return QString();
}


void MainWindow::readEeprom()
{
    QString tempDir = QDir::tempPath();
    QString tempFile;

    EEPROMInterface *eepromInterface = GetEepromInterface();

    if (IS_ARM(eepromInterface->getBoard())) 
      tempFile = tempDir + "/temp.bin";
    else
      tempFile = tempDir + "/temp.hex";

    if (readEepromFromRadio(tempFile, tr("Read Models and Settings From Radio"))) {
      MdiChild *child = createMdiChild();
      child->newFile();
      child->loadFile(tempFile, false);
      child->show();
    }
}

bool MainWindow::readFirmwareFromRadio(const QString filename)
{
  bool result = false;

  QString message = tr("Read Firmware From Radio");

  QFile file(filename);
  if (file.exists()) {
    file.remove();
  }

  g.flashDir(QFileInfo(filename).dir().absolutePath());

  if (IS_ARM(GetCurrentFirmware()->getBoard())) {
    QString path = FindMassstoragePath("FIRMWARE.BIN");
    if (!path.isEmpty()) {
      QStringList str;
      str << path << filename;
      avrOutputDialog *ad = new avrOutputDialog(this, "", str, message);
      ad->setWindowIcon(CompanionIcon("read_flash.png"));
      ad->exec();
      delete ad;
      result = true;
    }
  }

  if (result == false) {
    QStringList str = GetReceiveFlashCommand(filename);
    avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, message);
    ad->setWindowIcon(CompanionIcon("read_flash.png"));
    ad->exec();
    delete ad;
    result = true;
  }

  if (!QFileInfo(filename).exists())
    result = false;

  return result;
}

bool MainWindow::writeFirmwareToRadio(const QString filename)
{
  bool result = false;

  QString message = tr("Write Firmware To Radio");

  if (IS_ARM(GetCurrentFirmware()->getBoard())) {
    QString path = FindMassstoragePath("FIRMWARE.BIN");
    if (!path.isEmpty()) {
      QStringList str;
      str << filename << path;
      avrOutputDialog *ad = new avrOutputDialog(this, "", str, message);
      ad->setWindowIcon(CompanionIcon("write_flash.png"));
      ad->exec();
      delete ad;
      result = true;
    }
  }

  if (result == false) {
    QStringList str = GetSendFlashCommand(filename);
    avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, message, AVR_DIALOG_CLOSE_IF_SUCCESSFUL);
    CompanionIcon iconw("write_flash.png");
    ad->setWindowIcon(iconw);
    result = ad->exec();
    delete ad;
  }

  return result;
}

bool MainWindow::readEepromFromRadio(const QString filename, const QString message)
{
  bool result = false;

  QFile file(filename);
  if (file.exists()) {
    file.remove();
  }

  if (IS_ARM(GetCurrentFirmware()->getBoard())) {
    QString path = FindMassstoragePath("EEPROM.BIN");
    if (path.isEmpty()) {
      // On previous OpenTX we called the EEPROM file "TARANIS.BIN" :(
      path = FindMassstoragePath("TARANIS.BIN");
    }
    if (path.isEmpty()) {
      // Mike's bootloader calls the EEPROM file "ERSKY9X.BIN" :(
      path = FindMassstoragePath("ERSKY9X.BIN");
    }
    if (!path.isEmpty()) {
      QStringList str;
      str << path << filename;
      avrOutputDialog *ad = new avrOutputDialog(this, "", str, message);
      ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
      ad->exec();
      delete ad;
      result = true;
    }
  }

  if (result == false && !IS_TARANIS(GetCurrentFirmware()->getBoard())) {
    QStringList str = GetReceiveEEpromCommand(filename);
    avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, message);
    ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
    ad->exec();
    delete ad;
    result = true;
  }

  if (result == false && IS_ARM(GetCurrentFirmware()->getBoard())) {
    RadioNotFoundDialog *dialog = new RadioNotFoundDialog(this);
    dialog->exec();
    delete dialog;
  }

  if (!QFileInfo(filename).exists()) {
    result = false;
  }

  return result;
}

bool MainWindow::writeEepromToRadio(const QString filename, const QString message)
{
  bool result = false;

  if (IS_ARM(GetCurrentFirmware()->getBoard())) {
    QString path = FindMassstoragePath("EEPROM.BIN");
    if (path.isEmpty()) {
      // On previous OpenTX we called the EEPROM file "TARANIS.BIN" :(
      path = FindMassstoragePath("TARANIS.BIN");
    }
    if (path.isEmpty()) {
      // Mike's bootloader calls the EEPROM file "ERSKY9X.BIN" :(
      path = FindMassstoragePath("ERSKY9X.BIN");
    }
    if (!path.isEmpty()) {
      QStringList str;
      str << filename << path;
      avrOutputDialog *ad = new avrOutputDialog(this, "", str, message, AVR_DIALOG_SHOW_DONE);
      ad->setWindowIcon(CompanionIcon("write_eeprom.png"));
      ad->exec();
      delete ad;
      result = true;
    }
  }

  if (result == false && !IS_TARANIS(GetCurrentFirmware()->getBoard())) {
    QStringList str = GetSendEEpromCommand(filename);
    avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, "Write EEPROM To Radio", AVR_DIALOG_SHOW_DONE);
    ad->setWindowIcon(CompanionIcon("write_eeprom.png"));
    ad->exec();
    delete ad;
    result = true;
  }

  if (result == false && IS_ARM(GetCurrentFirmware()->getBoard())) {
    RadioNotFoundDialog *dialog = new RadioNotFoundDialog(this);
    dialog->exec();
    delete dialog;
  }

  return result;
}

void MainWindow::writeBackup()
{
    QString fileName;
    bool backup = false;
    burnDialog *cd = new burnDialog(this, 1, &fileName, &backup);
    cd->exec();
    if (!fileName.isEmpty()) {
      g.eepromDir(QFileInfo(fileName).dir().absolutePath());
      int ret = QMessageBox::question(this, "Companion", tr("Write Radio Backup from %1 to the radio?").arg(QFileInfo(fileName).fileName()), QMessageBox::Yes | QMessageBox::No);
      if (ret != QMessageBox::Yes) return;
      if (!isValidEEPROM(fileName))
        ret = QMessageBox::question(this, "Companion", tr("The file %1\nhas not been recognized as a valid Models and Settings file\nWrite anyway ?").arg(QFileInfo(fileName).fileName()), QMessageBox::Yes | QMessageBox::No);
      if (ret != QMessageBox::Yes) return;
      bool backupEnable = g.enableBackup();
      QString backupPath = g.backupDir();
      if (!backupPath.isEmpty()) {
        if (!QDir(backupPath).exists()) {
          if (backupEnable) {
            QMessageBox::warning(this, tr("Backup is impossible"), tr("The backup dir set in preferences does not exist"));
          }
          backupEnable = false;
        }
      }
      else {
        backupEnable = false;
      }

      if (backup) {
        if (backupEnable) {
          QString backupFile = backupPath + "/backup-" + QDateTime().currentDateTime().toString("yyyy-MM-dd-HHmmss") + ".bin";
          if (!readEepromFromRadio(backupFile, tr("Backup Models and Settings From Radio")))
            return;
        }
        int oldrev = getEpromVersion(fileName);
        QString tempDir = QDir::tempPath();
        QString tempFlash = tempDir + "/flash.bin";

        if (!readFirmwareFromRadio(tempFlash))
          return;

        QString restoreFile = tempDir + "/compat.bin";
        if (!convertEEPROM(fileName, restoreFile, tempFlash)) {
         int ret = QMessageBox::question(this, "Error", tr("Cannot check Models and Settings compatibility! Continue anyway?") ,
                                              QMessageBox::Yes | QMessageBox::No);
         if (ret==QMessageBox::No)
           return;
        }
        else {
          int rev = getEpromVersion(restoreFile);
          if ((rev / 100) != (oldrev / 100)) {
            QMessageBox::warning(this, tr("Warning"), tr("The radio firmware belongs to another product family, check file and preferences!"));
          }
          else if (rev < oldrev) {
            QMessageBox::warning(this, tr("Warning"), tr("The radio firmware is outdated, please upgrade!"));
          }
          fileName = restoreFile;
        }
        QByteArray ba = tempFlash.toLatin1();
        char *name = ba.data();
        unlink(name);
      }
      else {
        if (backupEnable) {
          QString backupFile = backupPath + "/backup-" + QDateTime().currentDateTime().toString("yyyy-MM-dd-hhmmss") + ".bin";
          if (!readEepromFromRadio(backupFile, tr("Backup Models and Settings From Radio")))
            return;
        }
      }
      if (!writeEepromToRadio(fileName, tr("Write Backup To Radio")))
        return;
    }
}

int MainWindow::getFileType(const QString &fullFileName)
{
    if(QFileInfo(fullFileName).suffix().toUpper()=="HEX")  return FILE_TYPE_HEX;
    if(QFileInfo(fullFileName).suffix().toUpper()=="BIN")  return FILE_TYPE_BIN;
    if(QFileInfo(fullFileName).suffix().toUpper()=="EEPM") return FILE_TYPE_EEPM;
    if(QFileInfo(fullFileName).suffix().toUpper()=="EEPE") return FILE_TYPE_EEPE;
    if(QFileInfo(fullFileName).suffix().toUpper()=="XML") return FILE_TYPE_XML;
    return 0;
}

bool MainWindow::isValidEEPROM(QString eepromfile)
{  
    int eeprom_size;
    QFile file(eepromfile);
    int fileType = getFileType(eepromfile);
    if (fileType==FILE_TYPE_HEX || fileType==FILE_TYPE_EEPE) {
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
      eeprom_size = file.size();
      QByteArray eeprom(eeprom_size, 0);
      QTextStream inputStream(&file);
      eeprom_size = HexInterface(inputStream).load((uint8_t *)eeprom.data(), eeprom_size);
      if (!eeprom_size) {
        return false;
      }
      file.close();
      RadioData * radioData = new RadioData();
      bool result = LoadEeprom(*radioData, (uint8_t *)eeprom.data(), eeprom_size);
      delete radioData;
      return result;
    }
    else if (fileType==FILE_TYPE_BIN) { //read binary
      if (!file.open(QFile::ReadOnly))
        return false;
      eeprom_size = file.size();
      QByteArray eeprom(eeprom_size, 0);
      long read = file.read(eeprom.data(), eeprom_size);
      file.close();
      if (read != eeprom_size) {
        return false;
      }
      RadioData * radioData = new RadioData();
      bool result = LoadEeprom(*radioData, (uint8_t *)eeprom.data(), eeprom_size);
      delete radioData;
      return result;
    }
    return false;
}

bool MainWindow::convertEEPROM(QString backupFile, QString restoreFile, QString flashFile)
{
    FirmwareInterface *firmware = GetCurrentFirmware();
    FlashInterface flash(flashFile);
    if (!flash.isValid())
      return false;

    unsigned int version = flash.getEEpromVersion();
    unsigned int variant = flash.getEEpromVariant();

    QFile file(backupFile);
    int eeprom_size = file.size();
    if (!eeprom_size)
      return false;

    if (!file.open(QIODevice::ReadOnly))
      return false;

    QByteArray eeprom(eeprom_size, 0);
    long result = file.read(eeprom.data(), eeprom_size);
    file.close();

    QSharedPointer<RadioData> radioData = QSharedPointer<RadioData>(new RadioData());
    if (!LoadEeprom(*radioData, (uint8_t *)eeprom.data(), eeprom_size) || !firmware->saveEEPROM((uint8_t *)eeprom.data(), *radioData, variant, version))
      return false;

    QFile file2(restoreFile);
    if (!file2.open(QIODevice::WriteOnly))
      return false;

    result = file2.write(eeprom.constData(), eeprom_size);
    file2.close();
    if (result != eeprom_size)
      return false;

    return true;
}

void MainWindow::writeFlash(QString fileToFlash)
{
    QString fileName;
    bool backup = g.backupOnFlash();
    if(!fileToFlash.isEmpty())
      fileName = fileToFlash;
    burnDialog *cd = new burnDialog(this, 2, &fileName, &backup);
    cd->exec();
    if (IS_TARANIS(GetEepromInterface()->getBoard()))
      backup=false;
    if (!fileName.isEmpty()) {
      g.backupOnFlash(backup);
      if (backup) {
        QString tempDir    = QDir::tempPath();
        QString backupFile = tempDir + "/backup.bin";
        bool backupEnable=g.enableBackup();
        QString backupPath=g.backupDir();
        if (!backupPath.isEmpty() && !IS_TARANIS(GetEepromInterface()->getBoard())) {
          if (!QDir(backupPath).exists()) {
            if (backupEnable) {
              QMessageBox::warning(this, tr("Backup is impossible"), tr("The backup dir set in preferences does not exist"));
            }
            backupEnable=false;
          }
        }
        else {
          backupEnable=false;
        }

        if (backupEnable) {
          QDateTime datetime;
          backupFile.clear();
          backupFile = backupPath+"/backup-"+QDateTime().currentDateTime().toString("yyyy-MM-dd-hhmmss")+".bin";
        }

        if (readEepromFromRadio(backupFile, tr("Backup Models and Settings From Radio"))) {
          sleep(2);
          int res = writeFirmwareToRadio(fileName);
          if (res) {
            QString restoreFile = tempDir + "/restore.bin";
            if (!convertEEPROM(backupFile, restoreFile, fileName)) {
              QMessageBox::warning(this, tr("Conversion failed"), tr("Cannot convert Models and Settings for use with this firmware, original data will be used"));
              restoreFile = backupFile;
            }
            sleep(2);
            if (!writeEepromToRadio(restoreFile, tr("Restore Models and Settings To Radio"))) {
              QMessageBox::warning(this, tr("Restore failed"), tr("Could not restore Models and Settings to Radio. The models and settings data file can be found at: %1").arg(backupFile));
            }
          }
          else {
            QMessageBox::warning(this, tr("Firmware write failed"), tr("Could not write firmware to radio. The models and settings data file can be found at: %1").arg(backupFile));
          }
        }
        else {
          QMessageBox::warning(this, tr("Backup failed"), tr("Cannot backup existing Models and Settings from Radio. Firmware write process aborted"));
        }
      }
      else {
        bool backupEnable=g.enableBackup();
        QString backupPath=g.backupDir();
        if (!QDir(backupPath).exists()) {
          if (backupEnable) {
            QMessageBox::warning(this, tr("Backup is impossible"), tr("The backup dir set in preferences does not exist"));
          }
          backupEnable=false;
        }
        if (backupEnable && !IS_TARANIS(GetEepromInterface()->getBoard())) {
          QDateTime datetime;
          QString backupFile = backupPath+"/backup-"+QDateTime().currentDateTime().toString("yyyy-MM-dd-hhmmss")+".bin";
          readEepromFromRadio(backupFile, tr("Backup Models and Settings From Radio"));
          sleep(2);
        }

        writeFirmwareToRadio(fileName);
      }
    }
}

void MainWindow::readBackup()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Radio Backup to File"), g.eepromDir(), tr(EXTERNAL_EEPROM_FILES_FILTER));
  if (!fileName.isEmpty()) {
    if (!readEepromFromRadio(fileName, tr("Read Radio Backup")))
      return;
  }
}

void MainWindow::readFlash()
{
  QString fileName = QFileDialog::getSaveFileName(this,tr("Read Radio Firmware to File"), g.flashDir(),tr(FLASH_FILES_FILTER));
  if (!fileName.isEmpty()) {
    readFirmwareFromRadio(fileName);
  }
}

void MainWindow::burnConfig()
{
  burnConfigDialog *bcd = new burnConfigDialog(this);
  bcd->exec();
  delete bcd;
}

void MainWindow::burnList()
{
  burnConfigDialog *bcd = new burnConfigDialog(this);
  bcd->listProgrammers();
}

void MainWindow::burnFuses()
{
  fusesDialog *fd = new fusesDialog(this);
  fd->exec();
  delete fd;
}

void MainWindow::compare()
{
  CompareDialog *fd = new CompareDialog(this);
  fd->setAttribute(Qt::WA_DeleteOnClose, true);
  fd->show();
}

void MainWindow::logFile()
{
  logsDialog *fd = new logsDialog(this);
  fd->setWindowFlags(Qt::Window);   //to show minimize an maximize buttons
  fd->setAttribute(Qt::WA_DeleteOnClose, true);
  fd->show();
}

void MainWindow::about()
{
    QString aboutStr = "<center><img src=\":/images/companion-title.png\"></center><br/>";
    aboutStr.append(tr("OpenTX Home Page: <a href='%1'>%1</a>").arg("http://www.open-tx.org"));
    aboutStr.append("<br/><br/>");
    aboutStr.append(tr("The OpenTX Companion project was originally forked from <a href='%1'>eePe</a>").arg("http://code.google.com/p/eepe"));
    aboutStr.append("<br/><br/>");
    aboutStr.append(tr("If you've found this program useful, please support by <a href='%1'>donating</a>").arg(DONATE_STR));
    aboutStr.append("<br/><br/>");
    aboutStr.append(QString("Version %1, %2").arg(C9X_VERSION).arg(__DATE__));
    aboutStr.append("<br/><br/>");
    aboutStr.append(tr("Copyright OpenTX Team") + "<br/>&copy; 2011-2014<br/>");
    QMessageBox msgBox(this);
    msgBox.setWindowIcon(CompanionIcon("information.png"));
    msgBox.setWindowTitle(tr("About Companion"));
    msgBox.setText(aboutStr);
    msgBox.exec();
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
    pasteAct->setEnabled(hasMdiChild ? activeMdiChild()->hasPasteData() : false);
    writeEepromAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);
    
    bool hasSelection = (activeMdiChild() && activeMdiChild()->hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
    simulateAct->setEnabled(hasSelection);
    printAct->setEnabled(hasSelection);
    loadbackupAct->setEnabled(hasMdiChild);
    compareAct->setEnabled(activeMdiChild());
    updateRecentFileActions();
    updateProfilesActions();
    updateLanguageActions();
    updateIconSizeActions();
    updateIconThemeActions();

    setWindowTitle(tr("OpenTX Companion - FW: %1 - Profile: %2").arg(GetCurrentFirmware()->getName()).arg( g.profile[g.id()].name() ));
}

MdiChild *MainWindow::createMdiChild()
{
  MdiChild * child = new MdiChild();
  mdiArea->addSubWindow(child);
  if (!child->parentWidget()->isMaximized() && !child->parentWidget()->isMinimized()) {
    child->parentWidget()->resize(400, 400);
  }

  connect(child, SIGNAL(copyAvailable(bool)),cutAct, SLOT(setEnabled(bool)));
  connect(child, SIGNAL(copyAvailable(bool)),copyAct, SLOT(setEnabled(bool)));
  connect(child, SIGNAL(copyAvailable(bool)),simulateAct, SLOT(setEnabled(bool)));
  connect(child, SIGNAL(copyAvailable(bool)),printAct, SLOT(setEnabled(bool)));

  return child;
}

QAction * MainWindow::addAct(const QString & icon, const QString & sName, const QString & lName, enum QKeySequence::StandardKey shortcut, const char *slot, QObject *slotObj)
{
  return addAct(icon, sName, lName, QKeySequence(shortcut), slot, slotObj);
}

QAction * MainWindow::addAct(const QString & icon, const QString & sName, const QString & lName, const QKeySequence & shortcut, const char *slot, QObject *slotObj)
{
  QAction * newAction = new QAction( this );
  if (!icon.isEmpty())
    newAction->setIcon(CompanionIcon(icon));
  if (!sName.isEmpty()) 
    newAction->setText(sName);
  if (!lName.isEmpty())
    newAction->setStatusTip(lName);
  newAction->setShortcut(shortcut);
  if (slotObj == NULL)
    slotObj = this;
  connect(newAction, SIGNAL(triggered()), slotObj, slot);
  return newAction;
}


QAction * MainWindow::addAct(const QString & icon, const QString & sName, const QString & lName, const char *slot)
{
  return addAct(icon, sName, lName, QKeySequence::UnknownKey, slot);
}

QAction * MainWindow::addAct(QActionGroup *aGroup, const QString & sName, const QString & lName, const char *slot)
{
  QAction *action = addAct("", sName, lName, QKeySequence::UnknownKey, slot);
  action->setCheckable(true);
  aGroup->addAction(action);   
  return action;
}

void MainWindow::createActions()
{
    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    for (int i = 0; i < MAX_RECENT; ++i)  {
      recentFileActs[i] = new QAction(this);
      recentFileActs[i]->setVisible(false);
      connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
    updateRecentFileActions();

    QActionGroup *profilesAlignmentGroup = new QActionGroup(this);
    for (int i=0; i<MAX_PROFILES; i++) {
      profileActs[i] = new QAction(this);
      profileActs[i]->setVisible(false);
      profileActs[i]->setCheckable(true);
      profilesAlignmentGroup->addAction(profileActs[i]);
      connect(profileActs[i], SIGNAL(triggered()), this, SLOT(loadProfile()));
    }
    updateProfilesActions();

    newAct =             addAct("new.png",    tr("New"),                    tr("Create a new Models and Settings file"), QKeySequence::New,    SLOT(newFile()));
    openAct =            addAct("open.png",   tr("Open..."),                tr("Open Models and Settings file"),         QKeySequence::Open,   SLOT(openFile()));
    saveAct =            addAct("save.png",   tr("Save"),                   tr("Save Models and Settings file"),         QKeySequence::Save,   SLOT(save()));
    saveAsAct =          addAct("saveas.png", tr("Save As..."),             tr("Save Models and Settings file"),         QKeySequence::SaveAs, SLOT(saveAs()));
    exitAct =            addAct("exit.png",   tr("Exit"),                   tr("Exit the application"),                  QKeySequence::Quit,   SLOT(closeAllWindows()), qApp);
    cutAct =             addAct("cut.png",    tr("Cut Model"),              tr("Cut current model to the clipboard"),    QKeySequence::Cut,    SLOT(cut()));
    copyAct =            addAct("copy.png",   tr("Copy Model"),             tr("Copy current model to the clipboard"),   QKeySequence::Copy,   SLOT(copy()));
    pasteAct =           addAct("paste.png",  tr("Paste Model"),            tr("Paste model from clipboard"),            QKeySequence::Paste,  SLOT(paste()));
 
    QActionGroup *themeAlignGroup = new QActionGroup(this);
    classicThemeAct =    addAct( themeAlignGroup,    tr("Classical"),       tr("The classic companion9x icon theme"),   SLOT(setClassicTheme()));
    yericoThemeAct =     addAct( themeAlignGroup,    tr("Yerico"),          tr("Yellow round honey sweet icon theme"),  SLOT(setYericoTheme()));
    monoThemeAct =       addAct( themeAlignGroup,    tr("Monochrome"),      tr("A monochrome black icon theme"),        SLOT(setMonochromeTheme()));
    monoWhiteAct =       addAct( themeAlignGroup,    tr("MonoWhite"),       tr("A monochrome white icon theme"),        SLOT(setMonoWhiteTheme()));
    monoBlueAct =        addAct( themeAlignGroup,    tr("MonoBlue"),        tr("A monochrome blue icon theme"),         SLOT(setMonoBlueTheme()));

    QActionGroup *iconAlignGroup = new QActionGroup(this);
    smallIconAct =       addAct( iconAlignGroup,     tr("Small"),           tr("Use small toolbar icons"),              SLOT(setSmallIconThemeSize()));
    normalIconAct =      addAct( iconAlignGroup,     tr("Normal"),          tr("Use normal size toolbar icons"),        SLOT(setNormalIconThemeSize()));
    bigIconAct =         addAct( iconAlignGroup,     tr("Big"),             tr("Use big toolbar icons"),                SLOT(setBigIconThemeSize()));
    hugeIconAct =        addAct( iconAlignGroup,     tr("Huge"),            tr("Use huge toolbar icons"),               SLOT(setHugeIconThemeSize()));

    QActionGroup *langAlignGroup = new QActionGroup(this);
    sysLangAct =         addAct( langAlignGroup,     tr("System language"), tr("Use system language in menus"),         SLOT(setSysLanguage()));
    czechLangAct =       addAct( langAlignGroup,     tr("Czech"),           tr("Use Czech in menus"),                   SLOT(setCZLanguage()));
    germanLangAct =      addAct( langAlignGroup,     tr("German"),          tr("Use German in menus"),                  SLOT(setDELanguage()));
    englishLangAct =     addAct( langAlignGroup,     tr("English"),         tr("Use English in menus"),                 SLOT(setENLanguage()));
    finnishLangAct =     addAct( langAlignGroup,     tr("Finnish"),         tr("Use Finnish in menus"),                 SLOT(setFILanguage()));
    frenchLangAct =      addAct( langAlignGroup,     tr("French"),          tr("Use French in menus"),                  SLOT(setFRLanguage()));
    italianLangAct =     addAct( langAlignGroup,     tr("Italian"),         tr("Use Italian in menus"),                 SLOT(setITLanguage()));
//    hebrewLangAct =      addAct( langAlignGroup,     tr("Hebrew"),          tr("Use Hebrew in menus"),                  SLOT(setHELanguage()));
    polishLangAct =      addAct( langAlignGroup,     tr("Polish"),          tr("Use Polish in menus"),                  SLOT(setPLLanguage()));
//    portugueseLangAct =  addAct( langAlignGroup,     tr("Portuguese"),      tr("Use Portuguese in menus"),              SLOT(setPTLanguage()));
    swedishLangAct =     addAct( langAlignGroup,     tr("Swedish"),         tr("Use Swedish in menus"),                 SLOT(setSELanguage()));
//    russianLangAct =     addAct( langAlignGroup,     tr("Russian"),         tr("Use Russian in menus"),                 SLOT(setRULanguage()));
//    dutchLangAct =       addAct( langAlignGroup,     tr("Dutch"),           tr("Use Dutch in menus"),                   SLOT(setNLLanguage()));

    aboutAct =           addAct("information.png",   tr("About..."),                tr("Show the application's About box"),   SLOT(about()));
    printAct =           addAct("print.png",         tr("Print..."),                tr("Print current model"),                QKeySequence::Print,       SLOT(print()));
    simulateAct =        addAct("simulate.png",      tr("Simulate..."),             tr("Simulate current model"),             QKeySequence(tr("Alt+S")), SLOT(simulate()));
    loadbackupAct =      addAct("open.png",          tr("Load Backup..."),          tr("Load backup from file"),              SLOT(loadBackup()));
    logsAct =            addAct("logs.png",          tr("View Log File..."),        tr("Open and view log file"),             SLOT(logFile()));
    appPrefsAct =        addAct("apppreferences.png",tr("Settings..."),             tr("Edit Settings"),                      SLOT(appPrefs()));
    fwPrefsAct =         addAct("fwpreferences.png", tr("Download..."),             tr("Download firmware and voice files"),  SLOT(fwPrefs()));
    checkForUpdatesAct = addAct("update.png",        tr("Check for Updates..."),    tr("Check OpenTX and Companion updates"), SLOT(doUpdates()));
    changelogAct =       addAct("changelog.png",     tr("Companion Changes..."),    tr("Show Companion change log"),          SLOT(changelog()));
    fwchangelogAct =     addAct("changelog.png",     tr("Firmware Changes..."),     tr("Show firmware change log"),           SLOT(fwchangelog()));
    compareAct =         addAct("compare.png",       tr("Compare Models..."),       tr("Compare models"),                     SLOT(compare()));
    editSplashAct =      addAct("paintbrush.png",    tr("Edit Radio Splash Image..."), tr("Edit the splash image of your Radio"),   SLOT(customizeSplash()));
    burnListAct =        addAct("list.png",          tr("List programmers..."),     tr("List available programmers"),         SLOT(burnList()));
    burnFusesAct =       addAct("fuses.png",         tr("Fuses..."),                tr("Show fuses dialog"),                  SLOT(burnFuses()));
    readFlashAct =       addAct("read_flash.png",    tr("Read Firmware from Radio"),tr("Read firmware from Radio"),           SLOT(readFlash()));
    writeFlashAct =      addAct("write_flash.png",   tr("Write Firmware to Radio"), tr("Write firmware to Radio"),            SLOT(writeFlash()));
    createProfileAct =   addAct("",                  tr("Add Radio Profile"),       tr("Create a new Radio Setting Profile"), SLOT(createProfile()));
    openDocURLAct =      addAct("",                  tr("Manuals and other Documents"),      tr("Open the OpenTX document page in a web browser"), SLOT(openDocURL()));
    writeEepromAct =     addAct("write_eeprom.png",  tr("Write Models and Settings To Radio"),  tr("Write Models and Settings to Radio"),       SLOT(writeEeprom()));
    readEepromAct =      addAct("read_eeprom.png",   tr("Read Models and Settings From Radio"), tr("Read Models and Settings from Radio"),      SLOT(readEeprom()));
    burnConfigAct =      addAct("configure.png",     tr("Configure Communications..."), tr("Configure software for communicating with the Radio"), SLOT(burnConfig()));
    writeBackupToRadioAct = addAct("write_eeprom_file.png", tr("Write Backup to Radio"), tr("Write Backup from file to to Radio"), SLOT(writeBackup()));
    readBackupToFileAct = addAct("read_eeprom_file.png", tr("Backup Radio to File"), tr("Save a complete backup file of all settings and model data in the Radio"), SLOT(readBackup()));
    contributorsAct =    addAct("contributors.png",  tr("Contributors..."), tr("A tribute to those who have contributed to OpenTX and Companion"), SLOT(contributors()));
    
    compareAct->setEnabled(false);
    simulateAct->setEnabled(false);
    printAct->setEnabled(false);
}

void MainWindow::createMenus()
{
    QMenu *recentFileMenu = new QMenu(tr("Recent Files"), this);
    QMenu *languageMenu = new QMenu(tr("Set Menu Language"), this);
    QMenu *themeMenu = new QMenu(tr("Set Icon Theme"), this);
    QMenu *iconThemeSizeMenu = new QMenu(tr("Set Icon Size"), this);

    fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addMenu(recentFileMenu);
    recentFileMenu->setIcon(CompanionIcon("recentdocument.png"));
    for (int i=0; i<MAX_RECENT; ++i)
      recentFileMenu->addAction(recentFileActs[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(logsAct);
    fileMenu->addAction(fwPrefsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(simulateAct);
    fileMenu->addAction(printAct);
    fileMenu->addAction(compareAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    settingsMenu = menuBar()->addMenu(tr("Settings"));
    settingsMenu->addMenu(languageMenu);
      languageMenu->addAction(sysLangAct);
      languageMenu->addAction(englishLangAct);
      languageMenu->addAction(czechLangAct);
      languageMenu->addAction(germanLangAct);
      languageMenu->addAction(finnishLangAct);
      languageMenu->addAction(frenchLangAct);
//      languageMenu->addAction(hebrewLangAct);
      languageMenu->addAction(italianLangAct);
      languageMenu->addAction(polishLangAct);
//      languageMenu->addAction(portugueseLangAct);
      languageMenu->addAction(swedishLangAct);
//      languageMenu->addAction(russianLangAct);
//      languageMenu->addAction(dutchLangAct);

    settingsMenu->addMenu(themeMenu);
      themeMenu->addAction(classicThemeAct);
      themeMenu->addAction(yericoThemeAct);
      themeMenu->addAction(monoThemeAct);
      themeMenu->addAction(monoBlueAct);
      themeMenu->addAction(monoWhiteAct);
    settingsMenu->addMenu(iconThemeSizeMenu);
      iconThemeSizeMenu->addAction(smallIconAct);
      iconThemeSizeMenu->addAction(normalIconAct);
      iconThemeSizeMenu->addAction(bigIconAct);
      iconThemeSizeMenu->addAction(hugeIconAct);
    settingsMenu->addSeparator();
    settingsMenu->addAction(appPrefsAct);
    settingsMenu->addMenu(createProfilesMenu());
    settingsMenu->addAction(editSplashAct);
    settingsMenu->addAction(burnConfigAct);

    burnMenu = menuBar()->addMenu(tr("Read/Write"));
    burnMenu->addAction(writeEepromAct);
    burnMenu->addAction(readEepromAct);
    burnMenu->addSeparator();
    burnMenu->addAction(writeBackupToRadioAct);
    burnMenu->addAction(readBackupToFileAct);
    burnMenu->addSeparator();
    burnMenu->addAction(writeFlashAct);
    burnMenu->addAction(readFlashAct);
    burnMenu->addSeparator();
    burnMenu->addSeparator();
    EEPROMInterface *eepromInterface = GetEepromInterface();
    if (!IS_ARM(eepromInterface->getBoard())) {    
      burnMenu->addAction(burnFusesAct);
      burnMenu->addAction(burnListAct);
    }
    menuBar()->addSeparator();
    helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addSeparator();
    helpMenu->addAction(checkForUpdatesAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(openDocURLAct);
    helpMenu->addSeparator();
    helpMenu->addAction(changelogAct);
    helpMenu->addAction(fwchangelogAct);
    helpMenu->addSeparator();
    helpMenu->addAction(contributorsAct);
}
 
QMenu *MainWindow::createRecentFileMenu()
{
    QMenu *recentFileMenu = new QMenu(this);
    for ( int i = 0; i < MAX_RECENT; ++i)
      recentFileMenu->addAction(recentFileActs[i]);
    return recentFileMenu;
}

QMenu *MainWindow::createProfilesMenu()
{
    QMenu *profilesMenu=new QMenu(tr("Radio Profile"), this);
    int i;
    for ( i = 0; i < MAX_PROFILES; ++i) {
      profilesMenu->addAction(profileActs[i]);
    }
    profilesMenu->addSeparator();

    profilesMenu->addAction(createProfileAct);
    profilesMenu->setIcon(CompanionIcon("profiles.png"));
    return profilesMenu;
}

void MainWindow::createToolBars()
{
    QSize size;
    switch(g.iconSize()) {
      case 0:
        size=QSize(16,16);
        break;
      case 1:
        size=QSize(24,24);
        break;
      case 2:
        size=QSize(32,32);
        break;
      case 3:
        size=QSize(48,48);
        break;
      default:
        size=QSize(24,24);
        break;        
    }
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setIconSize(size);
    fileToolBar->setObjectName("File");
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);

    QToolButton * recentToolButton = new QToolButton;
    recentToolButton->setPopupMode(QToolButton::InstantPopup);
    recentToolButton->setFocusPolicy(Qt::NoFocus);
    recentToolButton->setMenu(createRecentFileMenu());
    recentToolButton->setIcon(CompanionIcon("recentdocument.png"));
    recentToolButton->setToolTip(tr("Recent Models+Settings"));
    recentToolButton->setStatusTip(tr("Show recent Models+Settings documents"));

    fileToolBar->addWidget(recentToolButton);
    fileToolBar->addAction(saveAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(logsAct);
    fileToolBar->addAction(fwPrefsAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(appPrefsAct);

    QToolButton * profileButton = new QToolButton;
    profileButton->setPopupMode(QToolButton::InstantPopup);
    profileButton->setFocusPolicy(Qt::NoFocus);
    profileButton->setMenu(createProfilesMenu());
    profileButton->setIcon(CompanionIcon("profiles.png"));
    profileButton->setToolTip(tr("Radio Profile"));
    profileButton->setStatusTip(tr("Show the list of radio profiles"));

    fileToolBar->addWidget(profileButton);
    fileToolBar->addAction(editSplashAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(simulateAct);
    fileToolBar->addAction(printAct);
    fileToolBar->addAction(compareAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->setIconSize(size);
    editToolBar->setObjectName("Edit");
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    
    burnToolBar = new QToolBar(tr("Write"));
    addToolBar( Qt::LeftToolBarArea, burnToolBar );
    burnToolBar->setIconSize(size);
    burnToolBar->setObjectName("Write");
    burnToolBar->addAction(writeEepromAct);
    burnToolBar->addAction(readEepromAct);
    burnToolBar->addSeparator();
    burnToolBar->addAction(writeBackupToRadioAct);
    burnToolBar->addAction(readBackupToFileAct);
    burnToolBar->addSeparator();
    burnToolBar->addAction(writeFlashAct);
    burnToolBar->addAction(readFlashAct);
    burnToolBar->addSeparator();
    burnToolBar->addAction(burnConfigAct);

    helpToolBar = addToolBar(tr("Help"));
    helpToolBar->setIconSize(size);
    helpToolBar->setObjectName("Help");
    helpToolBar->addAction(checkForUpdatesAct);
    helpToolBar->addAction(aboutAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

MdiChild *MainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
      return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
      MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
      if (mdiChild->currentFile() == canonicalFilePath)
        return window;
    }
    return 0;
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
      return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::updateRecentFileActions()
{
    int i, numRecentFiles;
 
    //  Hide all document slots
    for ( i=0 ; i < g.historySize(); i++)
      recentFileActs[i]->setVisible(false);

    // Fill slots with content and unhide them
    QStringList files = g.recentFiles();
    numRecentFiles = qMin(files.size(), g.historySize());
 
    for ( i = 0; i < numRecentFiles; i++)  {
      QString text = strippedName(files[i]);
      if (!text.trimmed().isEmpty())
      {
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
      }
    }
}

void MainWindow::updateIconSizeActions()
{
  switch (g.iconSize())
  {
    case 0:  smallIconAct->setChecked(true);  break;
    case 1:  normalIconAct->setChecked(true); break;
    case 2:  bigIconAct->setChecked(true);    break;
    case 3:  hugeIconAct->setChecked(true);   break;
  }
}

void MainWindow::updateLanguageActions()
{
  QString langId = g.locale();

  if (langId=="") 
    sysLangAct->setChecked(true);
  else if (langId=="cs_CZ") 
    czechLangAct->setChecked(true);
  else if (langId=="de_DE") 
    germanLangAct->setChecked(true);
  else if (langId=="en") 
    englishLangAct->setChecked(true);
  else if (langId=="fi_FI") 
    finnishLangAct->setChecked(true);
  else if (langId=="fr_FR") 
    frenchLangAct->setChecked(true);
  else if (langId=="it_IT") 
    italianLangAct->setChecked(true);
  else if (langId=="he_IL") 
    hebrewLangAct->setChecked(true);
  else if (langId=="pl_PL") 
    polishLangAct->setChecked(true);
  else if (langId=="pt_PT") 
    portugueseLangAct->setChecked(true);
  else if (langId=="ru_RU") 
    russianLangAct->setChecked(true);
  else if (langId=="sv_SE") 
    swedishLangAct->setChecked(true);
  else if (langId=="nl_NL") 
    dutchLangAct->setChecked(true);
}

void MainWindow::updateIconThemeActions()
{
  switch (g.theme())
  {
    case 0:  classicThemeAct->setChecked(true); break;
    case 1:  yericoThemeAct->setChecked(true);  break;
    case 2:  monoWhiteAct->setChecked(true);    break;
    case 3:  monoThemeAct->setChecked(true);    break;
    case 4:  monoBlueAct->setChecked(true);     break;
  }
}

void MainWindow::updateProfilesActions()
{
  for (int i=0; i<MAX_PROFILES; i++) 
  {
    if (g.profile[i].existsOnDisk()) 
    {
      QString text = tr("%2").arg(g.profile[i].name());
      profileActs[i]->setText(text);
      profileActs[i]->setData(i);
      profileActs[i]->setVisible(true);
      if (i == g.id())
        profileActs[i]->setChecked(true);
    } 
    else 
    {
      profileActs[i]->setVisible(false);
    }
  }
}

void MainWindow::createProfile()
{
  int i;
  for (i=0; i<MAX_PROFILES && g.profile[i].existsOnDisk(); i++)
    ;
  if (i==MAX_PROFILES)  //Failed to find free slot
    return;
 
  // Copy current profile to new and give it a new name
  g.profile[i] = g.profile[g.id()];
  g.profile[i].name(tr("New Radio"));

  g.id(i);
  updateMenus();
}

QString MainWindow::strippedName(const QString &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}

int MainWindow::getEpromVersion(QString fileName)
{
  RadioData testData;
  if (fileName.isEmpty()) {
    return -1;
  }
  QFile file(fileName);
  if (!file.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("Unable to find file %1!").arg(fileName));
    return -1;
  }
  int fileType = getFileType(fileName);
#if 0
  if (fileType==FILE_TYPE_XML) {
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
      QMessageBox::critical(this, tr("Error"),tr("Error opening file %1:\n%2.").arg(fileName).arg(file.errorString()));
      return -1;
    }
    QTextStream inputStream(&file);
    XmlInterface(inputStream).load(testData);
  }
  else
#endif
  if (fileType==FILE_TYPE_HEX || fileType==FILE_TYPE_EEPE) { //read HEX file
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
        QMessageBox::critical(this, tr("Error"),tr("Error opening file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return -1;
    }        
    QDomDocument doc(ER9X_EEPROM_FILE_TYPE);
    bool xmlOK = doc.setContent(&file);
    if(xmlOK) {
      if (!LoadEepromXml(testData, doc)){
        return -1;
      }
    }
    file.reset();
      
    QTextStream inputStream(&file);
    if (fileType==FILE_TYPE_EEPE) {  // read EEPE file header
      QString hline = inputStream.readLine();
      if (hline!=EEPE_EEPROM_FILE_HEADER) {
        file.close();
        return -1;
      }
    }
    uint8_t eeprom[EESIZE_RLC_MAX];
    int eeprom_size = HexInterface(inputStream).load(eeprom, EESIZE_RLC_MAX);
    if (!eeprom_size) {
      QMessageBox::critical(this, tr("Error"), tr("Invalid Models and Settings File %1").arg(fileName));
      file.close();
      return -1;
    }
    file.close();
    if (!LoadEeprom(testData, eeprom, eeprom_size)) {
      QMessageBox::critical(this, tr("Error"),tr("Invalid Models and Settings File %1").arg(fileName));
      return -1;
    }
  }
  else if (fileType==FILE_TYPE_BIN) { //read binary
    int eeprom_size = file.size();
    if (!file.open(QFile::ReadOnly)) {  //reading binary file   - TODO HEX support
      QMessageBox::critical(this, tr("Error"),tr("Error opening file %1:\n%2.").arg(fileName).arg(file.errorString()));
      return -1;
    }
    QByteArray eeprom(eeprom_size, 0);
    long result = file.read(eeprom.data(), eeprom_size);
    file.close();
    if (result != eeprom_size) {
      QMessageBox::critical(this, tr("Error"),tr("Error reading file %1:\n%2.").arg(fileName).arg(file.errorString()));
      return -1;
    }
    if (!LoadEeprom(testData, (uint8_t *)eeprom.data(), eeprom_size)) {
      QMessageBox::critical(this, tr("Error"),tr("Invalid binary Models and Settings File %1").arg(fileName));
      return -1;
    }
  }
  return testData.generalSettings.version;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
      event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
      return;

    QString fileName = urls.first().toLocalFile();
    if (fileName.isEmpty())
      return;
    g.eepromDir(QFileInfo(fileName).dir().absolutePath());

    QMdiSubWindow *existing = findMdiChild(fileName);
    if (existing) {
      mdiArea->setActiveSubWindow(existing);
      return;
    }

    MdiChild *child = createMdiChild();
    if (child->loadFile(fileName)) {
      statusBar()->showMessage(tr("File loaded"), 2000);
      child->show();
    }
}

void MainWindow::autoClose()
{
    this->close();
}


