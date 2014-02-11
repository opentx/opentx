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
#include "preferencesdialog.h"
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
#include "firmwares/opentx/opentxinterface.h" // TODO get rid of this include

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
  downloadDialog_forWait(NULL)
{
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));

    MaxRecentFiles=MAX_RECENT;
    QSettings settings;
    int icon_size=settings.value("icon_size", 1).toInt();
    switch (icon_size) {
      case 0:
        ISize="16";
        break;
      case 1:
        ISize="24";
        break;
      case 2:
        ISize="32";
        break;
      case 3:
        ISize="48";
        break;
      default:
        ISize="24";
        break;        
    }
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    readSettings();
    setUnifiedTitleAndToolBarOnMac(true);
    this->setWindowIcon(QIcon(":/icon.png"));
    this->setIconSize(QSize(32,32));
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    setAcceptDrops(true);
    
    // give time to the splash to disappear and main window to open before starting updates
    int updateDelay = 1000;
    bool showSplash = settings.value("show_splash", true).toBool();
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
    if(strl.count()>1) str = strl[1];
    if(!str.isEmpty()) {
      int fileType = getFileType(str);

      if(fileType==FILE_TYPE_HEX) {
        burnToFlash(str);
      }

      if(fileType==FILE_TYPE_EEPE || fileType==FILE_TYPE_EEPM || fileType==FILE_TYPE_BIN) {
        MdiChild *child = createMdiChild();
        if (child->loadFile(str)) {
          if (!(printing && (model >=0 && model<GetEepromInterface()->getMaxModels()) && !printfilename.isEmpty()  )) {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
          } else {
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
  QSettings settings;
  int warnId=settings.value("warningId", 0 ).toInt();
  if (warnId<WARNING_ID && warnId!=0) {
    int res=0;
    if (WARNING_LEVEL>0) {
      QMessageBox::warning(this, "Companion", WARNING);
      res = QMessageBox::question(this, "Companion",tr("Display previous warning again at startup ?"),QMessageBox::Yes | QMessageBox::No);
    } else {
      QMessageBox::about(this, "Companion", WARNING);
      res = QMessageBox::question(this, "Companion",tr("Display previous message again at startup ?"),QMessageBox::Yes | QMessageBox::No);
    }
    if (res == QMessageBox::No) {
      settings.setValue("warningId", WARNING_ID);
    }
  } else if (warnId==0) {
    settings.setValue("warningId", WARNING_ID);    
  }
}

void MainWindow::doAutoUpdates()
{
    checkForUpdates(false, current_firmware_variant.id);
}

void MainWindow::doUpdates()
{
    checkForUpdates(true, current_firmware_variant.id);
}

void MainWindow::checkForUpdates(bool ignoreSettings, QString & fwId)
{
    showcheckForUpdatesResult = ignoreSettings;
    check1done = true;
    check2done = true;
    QSettings settings;
    fwToUpdate = fwId;
    QString stamp = GetFirmware(fwToUpdate)->stamp;

    if (!stamp.isEmpty()) {
      if (checkFW || ignoreSettings) {
        check1done=false;
        manager1 = new QNetworkAccessManager(this);
        connect(manager1, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply1Finished(QNetworkReply*)));
        QUrl url(stamp);
        QNetworkRequest request(url);
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
        manager1->get(request);
      }
    }
    
    if (checkCompanion9x || ignoreSettings) {
      check2done = false;
      manager2 = new QNetworkAccessManager(this);
      connect(manager2, SIGNAL(finished(QNetworkReply*)),this, SLOT(checkForUpdateFinished(QNetworkReply*)));
      QNetworkRequest request(QUrl(C9X_STAMP));
      request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
      manager2->get(request);
    }

    if(ignoreSettings) {
      downloadDialog_forWait = new downloadDialog(this, tr("Checking for updates"));
      downloadDialog_forWait->show();
    } else {
      downloadDialog_forWait = NULL; // TODO needed?
    }
}

void MainWindow::checkForUpdateFinished(QNetworkReply * reply)
{
    check2done = true;
    if(check1done && check2done && downloadDialog_forWait) {
      downloadDialog_forWait->close();
      // TODO delete downloadDialog_forWait?
    }

    QByteArray qba = reply->readAll();
    int i = qba.indexOf("C9X_VERSION");

    if (i>0) {
      QString version = qba.mid(i+14, 4);

      if (version.isNull()) {
          QMessageBox::warning(this, "Companion", tr("Unable to check for updates."));
          return;
      }
      double vnum=version.toDouble();
      QString c9xversion=QString(C9X_VERSION);
      int i;
      for (i=0; i<c9xversion.length();i++) {
        if (!(c9xversion.at(i).isDigit() || c9xversion.at(i).isPunct()))
          break;
      }
      double c9xver=(c9xversion.left(i).toDouble());
      if (c9xver< vnum) {
#if defined WIN32 || !defined __GNUC__ // || defined __APPLE__  // OSX should only notify of updates since no update packages are available. 
        showcheckForUpdatesResult = false; // update is available - do not show dialog
        int ret = QMessageBox::question(this, "Companion", tr("A new version of Companion is available (version %1)<br>"
                                                            "Would you like to download it?").arg(version) ,
                                        QMessageBox::Yes | QMessageBox::No);

        QSettings settings;

        if (ret == QMessageBox::Yes) {
#if defined __APPLE__          
          QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), settings.value("lastUpdatesDir").toString() + QString(C9X_INSTALLER).arg(version));
#else            
          QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), settings.value("lastUpdatesDir").toString() + QString(C9X_INSTALLER).arg(version), tr("Executable (*.exe)"));
#endif
          if (!fileName.isEmpty()) {
            settings.setValue("lastUpdatesDir", QFileInfo(fileName).dir().absolutePath());
            downloadDialog * dd = new downloadDialog(this, QString(OPENTX_COMPANION_DOWNLOADS C9X_INSTALLER).arg(version), fileName);
            installer_fileName = fileName;
            connect(dd, SIGNAL(accepted()), this, SLOT(updateDownloaded()));
            dd->show();
          }
        }
#else
        QMessageBox::warning(this, tr("New release available"), tr("A new release of Companion is available please check the OpenTX website!"));
#endif            
      } else {
        if (showcheckForUpdatesResult && check1done && check2done)
          QMessageBox::information(this, "Companion", tr("No updates available at this time."));
      }
    } else {
      if(check1done && check2done)
        QMessageBox::warning(this, "Companion", tr("Unable to check for updates."));
    }
}

void MainWindow::updateDownloaded()
{
    int ret = QMessageBox::question(this, "Companion", tr("Would you like to launch the installer?") ,
                                     QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
      if(QDesktopServices::openUrl(QUrl::fromLocalFile(installer_fileName)))
        QApplication::exit();
    }
}

void MainWindow::downloadLatestFW(FirmwareInfo * firmware, const QString & firmwareId)
{
    QString url, ext, cpuid;
    QSettings settings;
    url = firmware->getUrl(firmwareId);
    cpuid=settings.value("cpuid","").toString();
    ext = url.mid(url.lastIndexOf("."));
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), settings.value("lastFlashDir").toString() + "/" + firmwareId + ext);
    if (!fileName.isEmpty()) {
      downloadedFW = firmwareId;
      needRename=true;
      downloadedFWFilename = fileName;
      if (!cpuid.isEmpty()) {
        url.append("&cpuid=");
        url.append(cpuid);
      }
      settings.setValue("lastFlashDir", QFileInfo(fileName).dir().absolutePath());
      downloadDialog * dd = new downloadDialog(this, url, fileName);
      connect(dd, SIGNAL(accepted()), this, SLOT(reply1Accepted()));
      dd->exec();
    }
}

void MainWindow::reply1Accepted()
{
    QString errormsg;
    QSettings settings;
    bool autoflash=settings.value("burnFirmware", true).toBool();
    bool addversion=settings.value("rename_firmware_files", false).toBool();
    settings.beginGroup("FwRevisions");
    if (downloadedFWFilename.isEmpty()) {
      if (!(downloadedFW.isEmpty())) {
        QFile file(downloadedFW);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
        QMessageBox::critical(this, tr("Error"),
                                tr("Error opening file %1:\n%2.")
                                .arg(downloadedFW)
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
              errormsg=tr("Firmware does not longer fit in the Tx, due to selected firmware options");
              break;
            case 2:
              errormsg=tr("Compilation server temporary failure, try later");
              break;
            case 3:
              errormsg=tr("Compilation server too busy, try later");
              break;
            case 4:
              errormsg=tr("Compilation server requires registration, please check OpenTX web site");
              break;
            default:
              errormsg=tr("Unknown server failure, try later");
              break;          
          }
          file.close();
          QMessageBox::critical(this, tr("Error"), errormsg);
          settings.endGroup();
          return;
        }
        file.close();
        currentFWrev = currentFWrev_temp;
        settings.setValue(downloadedFW, currentFWrev);
      }
    } else {
      QFile file(downloadedFWFilename);
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
      QMessageBox::critical(this, tr("Error"),
                              tr("Error opening file %1:\n%2.")
                              .arg(downloadedFWFilename)
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
            errormsg=tr("Firmware does not fit in the Tx, due to selected firmware options");
            break;
          case 2:
            errormsg=tr("Compilation server termporary failure, try later");
            break;
          case 3:
            errormsg=tr("Compilation server too busy, try later");
            break;
          case 4:
            errormsg=tr("Compilation server requires registration, please check OpenTX web site");
            break;
          default:
            errormsg=tr("Unknown server failure, try later");
            break;          
        }
        file.close();
        QMessageBox::critical(this, tr("Error"), errormsg);
        settings.endGroup();
        return;
      }
      file.close();
      FlashInterface flash(downloadedFWFilename);
      QString rev=flash.getSvn();
      int pos=rev.lastIndexOf("-r");
      if (pos>0) {
        currentFWrev=rev.mid(pos+2).toInt();
        if (addversion && needRename) {
          QFileInfo fi(downloadedFWFilename);
          QString path=fi.path()+QDir::separator ();
          path.append(fi.completeBaseName());
          path.append(rev.mid(pos));
          path.append(".");
          path.append(fi.suffix());
          QDir qd;
          qd.remove(path);
          qd.rename(downloadedFWFilename,path);
          downloadedFWFilename=path;
        }
        settings.setValue(downloadedFW, currentFWrev);
        if (autoflash) {
          int ret = QMessageBox::question(this, "Companion", tr("Do you want to write the firmware to the transmitter now ?"), QMessageBox::Yes | QMessageBox::No);
          if (ret == QMessageBox::Yes) {
            burnToFlash(downloadedFWFilename);
          }
        }
      }
    }
    settings.endGroup();
}

void MainWindow::reply1Finished(QNetworkReply * reply)
{
    int OldFwRev;
    check1done = true;
    bool download = false;
    bool ignore = false;
    QString cpuid;
    
    if(check1done && check2done && downloadDialog_forWait) {
      downloadDialog_forWait->close();
      // TODO delete downloadDialog_forWait?
    }
    
    QSettings settings;
    cpuid=settings.value("cpuid","").toString();
    QByteArray qba = reply->readAll();
    int i = qba.indexOf("SVN_VERS");
    int warning = qba.indexOf("WARNING");

    if(i>0) {
      bool cres;
      int k = qba.indexOf("-r", i);
      int l = qba.indexOf('"', k);
      int rev = QString::fromAscii(qba.mid(k+2, l-k-2)).toInt(&cres);
      QString newrev=qba.mid(k).replace('"', "").trimmed();

      if(!cres) {
        QMessageBox::warning(this, "Companion", tr("Unable to check for updates."));
        int server = settings.value("fwserver",0).toInt();
        server++;
        settings.setValue("fwserver",server);
        return;
      }

      if(rev>0) {
        NewFwRev=rev;
        settings.beginGroup("FwRevisions");
        OldFwRev = settings.value(fwToUpdate, 0).toInt();
        settings.endGroup();
        QMessageBox msgBox;
        QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout* layout = (QGridLayout*)msgBox.layout();
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());            
        if (OldFwRev == 0) {
          showcheckForUpdatesResult = false; // update is available - do not show dialog
          QString rn = GetFirmware(fwToUpdate)->rnurl;
          QAbstractButton *rnButton;
          msgBox.setWindowTitle("Companion");
          msgBox.setInformativeText(tr("Firmware %1 does not seem to have ever been downloaded.\nVersion %2 is available.\nDo you want to download it now ?").arg(fwToUpdate).arg(NewFwRev));
          QAbstractButton *YesButton = msgBox.addButton(trUtf8("Yes"), QMessageBox::YesRole);
          msgBox.addButton(trUtf8("No"), QMessageBox::NoRole);
          if (!rn.isEmpty()) {
            rnButton = msgBox.addButton(trUtf8("Release Notes"), QMessageBox::ActionRole);
          }
          msgBox.setIcon(QMessageBox::Question);
          msgBox.resize(0,0);
          msgBox.exec();
          if( msgBox.clickedButton() == rnButton ) {
            contributorsDialog *cd = new contributorsDialog(this,2,rn);
            cd->exec();
            int ret2 = QMessageBox::question(this, "Companion", tr("Do you want to download release %1 now ?").arg(NewFwRev),
                  QMessageBox::Yes | QMessageBox::No);
            if (ret2 == QMessageBox::Yes) {
              download = true;
            } else {
              ignore = true;
            }                                    
          } else if (msgBox.clickedButton() == YesButton ) {
            download = true;
          } else {
            ignore = true;
          }
        } else if (NewFwRev > OldFwRev) {
          showcheckForUpdatesResult = false; // update is available - do not show dialog
          QString rn = GetFirmware(fwToUpdate)->rnurl;
          QAbstractButton *rnButton;
          msgBox.setText("Companion");
          msgBox.setInformativeText(tr("A new version of %1 firmware is available (current %2 - newer %3).\nDo you want to download it now ?").arg(fwToUpdate).arg(OldFwRev).arg(NewFwRev));
          QAbstractButton *YesButton = msgBox.addButton(trUtf8("Yes"), QMessageBox::YesRole);
          msgBox.addButton(trUtf8("No"), QMessageBox::NoRole);
          if (!rn.isEmpty()) {
            rnButton = msgBox.addButton(trUtf8("Release Notes"), QMessageBox::ActionRole);
          }
          msgBox.setIcon(QMessageBox::Question);
          msgBox.resize(0,0);
          msgBox.exec(); 
          if( msgBox.clickedButton() == rnButton ) {
            warning=0;
            contributorsDialog *cd = new contributorsDialog(this,2,rn);
            cd->exec();
            int ret2 = QMessageBox::question(this, "Companion", tr("Do you want to download release %1 now ?").arg(NewFwRev),
                  QMessageBox::Yes | QMessageBox::No);
            if (ret2 == QMessageBox::Yes) {
              download = true;
            } else {
              ignore = true;
            }                                    
          } else if (msgBox.clickedButton() == YesButton ) {
            download = true;
          } else {
            ignore = true;
          }
        } else {
          if(showcheckForUpdatesResult && check1done && check2done)
            QMessageBox::information(this, "Companion", tr("No updates available at this time."));
        }
        if (ignore) {
          int res = QMessageBox::question(this, "Companion",tr("Ignore this version (r%1)?").arg(rev), QMessageBox::Yes | QMessageBox::No);
          if (res==QMessageBox::Yes)   {
            settings.beginGroup("FwRevisions");
            settings.setValue(fwToUpdate, NewFwRev);
            settings.endGroup();
          }
        } else if (download == true) {
          if (warning>0) {
            QString rn = GetFirmware(fwToUpdate)->rnurl;
            if (!rn.isEmpty()) {
              int ret2 = QMessageBox::warning(this, "Companion", tr("Release notes contain very important informations. Do you want to see them now ?"), QMessageBox::Yes | QMessageBox::No);
              if (ret2 == QMessageBox::Yes) {
                contributorsDialog *cd = new contributorsDialog(this,2,rn);
                cd->exec();
              }
            }
          }
          downloadedFW = fwToUpdate;
          QString url = GetFirmware(fwToUpdate)->getUrl(fwToUpdate);
          QString ext = url.mid(url.lastIndexOf("."));
          needRename=false;
          bool addversion=settings.value("rename_firmware_files", false).toBool();
          QString fileName;
          if (addversion) {
            fileName = QFileDialog::getSaveFileName(this, tr("Save As"), settings.value("lastFlashDir").toString() + "/" + fwToUpdate + newrev + ext);
          } else {
            fileName = QFileDialog::getSaveFileName(this, tr("Save As"), settings.value("lastFlashDir").toString() + "/" + fwToUpdate + ext);
          }
          if (!fileName.isEmpty()) {
            if (!cpuid.isEmpty()) {
              url.append("&cpuid=");
              url.append(cpuid);
            }              
            downloadedFWFilename = fileName;
            settings.setValue("lastFlashDir", QFileInfo(fileName).dir().absolutePath());
            downloadDialog * dd = new downloadDialog(this, url, fileName);
            currentFWrev_temp = NewFwRev;
            connect(dd, SIGNAL(accepted()), this, SLOT(reply1Accepted()));
            dd->exec();
            downloadedFW = fwToUpdate;
          }
        }
      } else {
        QMessageBox::warning(this, "Companion", tr("Unable to check for updates."));
      }
    } else {
      if(check1done && check2done) {
        QMessageBox::warning(this, "Companion", tr("Unable to check for updates."));
        int server = settings.value("fwserver",0).toInt();
        server++;
        settings.setValue("fwserver",server);
        return;
      }
    }    
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
      event->ignore();
    }
    else {
      event->accept();
    }
}

void  MainWindow::setLanguage(QString langString)
{    
    QSettings settings;
    settings.setValue("locale", langString );
    
    QMessageBox msgBox;
    msgBox.setText(tr("The selected language will be used the next time you start Companion."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    msgBox.exec();
}

void  MainWindow::setTheme(int index)
{    
    QSettings settings;
    settings.setValue("theme", index );
    
    QMessageBox msgBox;
    msgBox.setText(tr("The new theme will be loaded the next time you start Companion."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    msgBox.exec();
}

void  MainWindow::setIconThemeSize(int index)
{    
    QSettings settings;
    settings.setValue("icon_size", index );

    QMessageBox msgBox;
    msgBox.setText(tr("The icon size will be used the next time you start Companion."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    msgBox.exec();
}

void MainWindow::newFile()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();
}

void MainWindow::openDocumentURL()
{
  QString link = "http://www.open-tx.org/documents.html";
  QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::openFile()
{
    QSettings settings;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("lastDir").toString(),tr(EEPROM_FILES_FILTER));
    if (!fileName.isEmpty()) {
      settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());

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
    QSettings settings;
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
      QString fileName=action->data().toString();
      // settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());

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

void MainWindow::loadProfile()
{
    QSettings settings;
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
      int profnum=action->data().toInt();
      QSettings settings;
      settings.setValue("profileId",profnum);
      settings.beginGroup("Profiles");
      QString profile=QString("profile%1").arg(profnum);
      settings.beginGroup(profile);
      QString profileName=settings.value("Name", "").toString();
      int chord=settings.value("default_channel_order", 0).toInt();
      int defmod=settings.value("default_mode", 0).toInt();
      bool burnfw=settings.value("burnFirmware", false).toBool();
      QString sdPath=settings.value("sdPath", ".").toString();
      bool renfw=settings.value("rename_firmware_files", false).toBool();
      QString SplashFileName=settings.value("SplashFileName","").toString();
      QString SplashImage=settings.value("SplashImage", "").toString();            
      QString firmware_id=settings.value("firmware", default_firmware_variant.id).toString();
      firmware_id.replace("open9x","opentx");
      firmware_id.replace("x9da","taranis");
      settings.setValue("firmware", firmware_id);
      settings.endGroup();
      settings.endGroup();
      settings.setValue("Name", profileName );
      settings.setValue("default_channel_order", chord);
      settings.setValue("default_mode", defmod);
      settings.setValue("burnFirmware", burnfw);
      settings.setValue("rename_firmware_files", renfw);
      settings.setValue("sdPath", sdPath);
      settings.setValue("SplashFileName", SplashFileName);
      settings.setValue("SplashImage", SplashImage);
      settings.setValue("firmware", firmware_id);
      current_firmware_variant = GetFirmwareVariant(firmware_id);

      foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        mdiChild->eepromInterfaceChanged();
      }

      updateMenus();
    }
}

void MainWindow::appPreferences()
{
    appPreferencesDialog *pd = new appPreferencesDialog(this);
    pd->exec();
    updateMenus();
}

void MainWindow::fwPreferences()
{
    fwPreferencesDialog *pd = new fwPreferencesDialog(this);
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
    FirmwareInfo *currfirm = GetCurrentFirmware();
    QString rn=currfirm->rnurl;
    if (rn.isEmpty()) {
      QMessageBox::information(this, tr("Firmware updates"), tr("Current firmware does not provide release notes informations."));
    } else {
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

void MainWindow::burnTo()
{
    if (activeMdiChild())
      activeMdiChild()->burnTo();
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
    } else if (eepromInterface->getBoard()==BOARD_SKY9X) {
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
  //    return NULL; // to be implemented
    } else if (eepromInterface->getBoard() == BOARD_SKY9X) {
      ret=GetSambaArguments(QString("SERIALFLASH::Init 0\n") + "receive_file {SerialFlash AT25} \"" + filename + "\" 0x0 0x80000 0\n");
    } else {
      ret=GetAvrdudeArguments("eeprom:r:", filename);
    }
    return ret;
}

QStringList MainWindow::GetSendEEpromCommand(const QString &filename)
{
    QStringList ret;
    EEPROMInterface *eepromInterface = GetEepromInterface();
    if (IS_TARANIS(eepromInterface->getBoard())) {
  //    return NULL;  // to be implemented
    } else if (eepromInterface->getBoard() == BOARD_SKY9X) {
      ret=GetSambaArguments(QString("SERIALFLASH::Init 0\n") + "send_file {SerialFlash AT25} \"" + filename + "\" 0x0 0\n");
    } else {
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
    } else if (eepromInterface->getBoard() == BOARD_SKY9X) {
      ret=GetSambaArguments(QString("send_file {Flash} \"") + filename + "\" 0x400000 0\n" + "FLASH::ScriptGPNMV 2\n");
    } else {
      ret=GetAvrdudeArguments("flash:w:", filename);
    }
    return ret;
}

QStringList MainWindow::GetReceiveFlashCommand(const QString &filename)
{
    EEPROMInterface *eepromInterface = GetEepromInterface();
    if (IS_TARANIS(eepromInterface->getBoard())) {
      return GetDFUUtilArguments("-U", filename);    
    } else if (eepromInterface->getBoard() == BOARD_SKY9X) {
      return GetSambaArguments(QString("receive_file {Flash} \"") + filename + "\" 0x400000 0x40000 0\n");
    } else {
      return GetAvrdudeArguments("flash:r:", filename);
    }
}

QString MainWindow::FindTaranisPath()
{
    int pathcount=0;
    QString path;
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
      if(ret) {
        QString vName=QString::fromUtf16 ( (const ushort *) szVolumeName) ;
        if (vName.contains("TARANIS")) {
          eepromfile=drive.absolutePath();
          eepromfile.append("/TARANIS.BIN");
          if (QFile::exists(eepromfile)) {
            pathcount++;
            path=eepromfile;
          }
        }
      }
    }
#else
    struct mount_entry *entry;
    entry = read_file_system_list(true); 
    while (entry != NULL) {
      if (!drives.contains(entry->me_devname)) {
        drives.append(entry->me_devname);
        eepromfile=entry->me_mountdir;

        eepromfile.append("/TARANIS.BIN");
  #if !defined __APPLE__ && !defined WIN32
        QString fstype=entry->me_type;
        qDebug() << fstype;
        if (QFile::exists(eepromfile) && fstype.contains("fat") ) {
  #else
        if (QFile::exists(eepromfile)) {
  #endif 
          pathcount++;
          path=eepromfile;
        }
      }
      entry = entry->me_next; ;
    }
#endif
    if (pathcount==1) {
      return path;
    } else {
      return "";
    }
}


void MainWindow::burnFrom()
{
    QString tempDir = QDir::tempPath();
    QString tempFile;
    int res=0;
    EEPROMInterface *eepromInterface = GetEepromInterface();
    if (IS_ARM(eepromInterface->getBoard())) 
      tempFile = tempDir + "/temp.bin";
    else
      tempFile = tempDir + "/temp.hex";
    if (QFile::exists(tempFile)) {
      unlink(tempFile.toAscii());
    }

    if (IS_TARANIS(eepromInterface->getBoard())) {
      QString path=FindTaranisPath();
      if (path.isEmpty()) {
        QMessageBox::warning(this, tr("Taranis radio not found"), tr("Impossible to identify the radio on your system, please verify the eeprom disk is connected."));
        res=false;
      } else {
        QStringList str;
        str << path << tempFile;
        avrOutputDialog *ad = new avrOutputDialog(this,"", str, tr("Read Models and Settings From Tx")); //, AVR_DIALOG_KEEP_OPEN);
        ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
        res = ad->exec();
        sleep(1);
      }
    } else {    
      QStringList str = GetReceiveEEpromCommand(tempFile);
      avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Read Models and Settings From Tx")); //, AVR_DIALOG_KEEP_OPEN);
      ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
      res = ad->exec();
    }
    if (QFileInfo(tempFile).exists() && res) {
      MdiChild *child = createMdiChild();
      child->newFile();
      child->loadFile(tempFile, false);
      child->show();
    }
}

void MainWindow::burnExtenalToEEPROM()
{
    QSettings settings;
    QString fileName;
    bool backup = false;
    burnDialog *cd = new burnDialog(this, 1, &fileName, &backup);
    cd->exec();
    if (!fileName.isEmpty()) {
      settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());
      int ret = QMessageBox::question(this, "Companion", tr("Write Models and settings from %1 to the Tx?").arg(QFileInfo(fileName).fileName()), QMessageBox::Yes | QMessageBox::No);
      if (ret != QMessageBox::Yes) return;
      if (!isValidEEPROM(fileName))
        ret = QMessageBox::question(this, "Companion", tr("The file %1\nhas not been recognized as a valid Models and Settings file\nWrite anyway ?").arg(QFileInfo(fileName).fileName()), QMessageBox::Yes | QMessageBox::No);
      if (ret != QMessageBox::Yes) return;
      bool backupEnable = settings.value("backupEnable", true).toBool();
      QString backupPath = settings.value("backupPath", "").toString();
      if (!backupPath.isEmpty()) {
        if (!QDir(backupPath).exists()) {
          if (backupEnable) {
            QMessageBox::warning(this, tr("Backup is impossible"), tr("The backup dir set in preferences does not exist"));
          }
          backupEnable = false;
        }
      } else {
        backupEnable = false;
      }
      EEPROMInterface *eepromInterface = GetEepromInterface();
      if (backup) {
        if (backupEnable) {
          QString backupFile = backupPath + "/backup-" + QDateTime().currentDateTime().toString("yyyy-MM-dd-HHmmss") + ".bin";
          if (IS_TARANIS(eepromInterface->getBoard())) {
            QString path=FindTaranisPath();
            if (path.isEmpty()) {
              QMessageBox::warning(this, tr("Taranis radio not found"), tr("Impossible to identify the radio on your system, please verify the eeprom disk is connected."));
              return;
            } else {
              QStringList str;
              str << path << backupFile;
              avrOutputDialog *ad = new avrOutputDialog(this,"", str, tr("Backup Models and Settings From Tx")); //, AVR_DIALOG_KEEP_OPEN);
              ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
              ad->exec();
              sleep(1);
            }
          } else {
            QStringList str = GetReceiveEEpromCommand(backupFile);
            avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Backup Models and Settings From Tx"));
            ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
            ad->exec();
            sleep(1);
          }
        }
        int oldrev = getEpromVersion(fileName);
        QString tempDir = QDir::tempPath();
        QString tempFlash = tempDir + "/flash.bin";
        QStringList str = GetReceiveFlashCommand(tempFlash);
        avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, "Read Firmware From Tx");
        ad->setWindowIcon(CompanionIcon("read_flash.png"));
        ad->exec();
        sleep(1);
        QString restoreFile = tempDir + "/compat.bin";
        if (!convertEEPROM(fileName, restoreFile, tempFlash)) {
         int ret = QMessageBox::question(this, "Error", tr("Cannot check Models and Settings compatibility! Continue anyway?") ,
                                              QMessageBox::Yes | QMessageBox::No);
         if (ret==QMessageBox::No)
           return;
        } else {
          int rev = getEpromVersion(restoreFile);
          if ((rev / 100) != (oldrev / 100)) {
            QMessageBox::warning(this, tr("Warning"), tr("The transmitter firmware belongs to another product family, check file and preferences!"));
          }
          else if (rev < oldrev) {
            QMessageBox::warning(this, tr("Warning"), tr("The transmitter firmware is outdated, please upgrade!"));
          }
          fileName = restoreFile;
        }
        QByteArray ba = tempFlash.toLatin1();
        char *name = ba.data();
        unlink(name);
      } else {
        if (backupEnable) {
          QString backupFile = backupPath + "/backup-" + QDateTime().currentDateTime().toString("yyyy-MM-dd-hhmmss") + ".bin";
          if (IS_TARANIS(eepromInterface->getBoard())) {
            QString path=FindTaranisPath();
            if (path.isEmpty()) {
              QMessageBox::warning(this, tr("Taranis radio not found"), tr("Impossible to identify the radio on your system, please verify the eeprom disk is connected."));
              return;
            } else {
              QStringList str;
              str << path << backupFile;
              avrOutputDialog *ad = new avrOutputDialog(this,"", str, tr("Backup Models and Settings From Tx")); //, AVR_DIALOG_KEEP_OPEN);
              ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
              ad->exec();
              delete ad;
              sleep(1);
            }
          }
          else {
            QStringList str = ((MainWindow *)this->parent())->GetReceiveEEpromCommand(backupFile);
            avrOutputDialog *ad = new avrOutputDialog(this, ((MainWindow *)this->parent())->GetAvrdudeLocation(), str, tr("Backup Models and Settings From Tx"));
            ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
            ad->exec();
            delete ad;
            sleep(1);
          }
        }
      }
      if (IS_TARANIS(eepromInterface->getBoard())) {
        QString path=FindTaranisPath();
        if (path.isEmpty()) {
          QMessageBox::warning(this, tr("Taranis radio not found"), tr("Impossible to identify the radio on your system, please verify the eeprom disk is connected."));
          return;
        }
        else {
          QStringList str;
          str << fileName << path;
          avrOutputDialog *ad = new avrOutputDialog(this,"", str, tr("Write Models and Settings To Tx")); //, AVR_DIALOG_KEEP_OPEN);
          ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
          ad->exec();
          delete ad;
          sleep(1);
        }      
      }
      else {
        QStringList str = GetSendEEpromCommand(fileName);
        avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, "Write Models and Settings To Tx", AVR_DIALOG_SHOW_DONE);
        ad->setWindowIcon(CompanionIcon("write_eeprom.png"));
        ad->exec();
        delete ad;
      }
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
      uint8_t *eeprom = (uint8_t *)malloc(eeprom_size);
      QTextStream inputStream(&file);
      eeprom_size = HexInterface(inputStream).load(eeprom, eeprom_size);
      if (!eeprom_size) {
        free(eeprom);
        return false;
      }
      file.close();
      RadioData * radioData = new RadioData();
      bool result = LoadEeprom(*radioData, eeprom, eeprom_size);
      free(eeprom);
      delete radioData;
      return result;
    }
    else if (fileType==FILE_TYPE_BIN) { //read binary
      if (!file.open(QFile::ReadOnly))
        return false;
      eeprom_size = file.size();
      uint8_t *eeprom = (uint8_t *)malloc(eeprom_size);
      memset(eeprom, 0, eeprom_size);
      long read = file.read((char*)eeprom, eeprom_size);
      file.close();
      if (read != eeprom_size) {
        free(eeprom);
        return false;
      }
      RadioData * radioData = new RadioData();
      bool result = LoadEeprom(*radioData, eeprom, eeprom_size);
      free(eeprom);
      delete radioData;
      return result;
    }
    return false;
}

bool MainWindow::convertEEPROM(QString backupFile, QString restoreFile, QString flashFile)
{
    FirmwareInfo *firmware = GetCurrentFirmware();
    FlashInterface flash(flashFile);
    if (!flash.isValid())
      return false;

    QString fwSvn = flash.getSvn();
    if (fwSvn.isEmpty() || !fwSvn.contains("-r"))
      return false;
    QStringList svnTags = fwSvn.split("-r", QString::SkipEmptyParts);
    fwSvn = svnTags.back();
    if (fwSvn.endsWith('M'))
      fwSvn = fwSvn.mid(0, fwSvn.size()-1);
    int revision = fwSvn.toInt();
    if (!revision)
      return false;

    unsigned int version = 0;
    unsigned int variant = 0;

    if ((svnTags.at(0) == "open9x")||(svnTags.at(0) == "opentx")) {
      if (revision > 1464) {
        QString fwBuild = flash.getBuild();
        if (fwBuild.contains("-")) {
          QStringList buildTags = fwBuild.split("-", QString::SkipEmptyParts);
          if (buildTags.size() >= 1)
            version = buildTags.at(0).toInt();
          if (buildTags.size() >= 2)
            variant = buildTags.at(1).toInt();
        } else {
          version = fwBuild.toInt(); // TODO changer le nom de la variable
        }
      }
      else {
        version = ((Open9xFirmware *)firmware)->getEepromVersion(revision);
      }
    }

    QFile file(backupFile);
    int eeprom_size = file.size();
    if (!eeprom_size)
      return false;

    if (!file.open(QIODevice::ReadOnly))
      return false;

    uint8_t *eeprom = (uint8_t *)malloc(eeprom_size);
    long result = file.read((char*)eeprom, eeprom_size);
    file.close();

    QSharedPointer<RadioData> radioData = QSharedPointer<RadioData>(new RadioData());
    if (!LoadEeprom(*radioData, eeprom, eeprom_size) || !firmware->saveEEPROM(eeprom, *radioData, variant, version))
      return false;

    QFile file2(restoreFile);
    if (!file2.open(QIODevice::WriteOnly))
      return false;

    result = file2.write((char*)eeprom, eeprom_size);
    file2.close();
    if (result != eeprom_size)
      return false;

    free(eeprom);
    return true;
}

void MainWindow::burnToFlash(QString fileToFlash)
{
    QSettings settings;
    QString fileName;
    bool backup = settings.value("backupOnFlash", false).toBool();
    if(!fileToFlash.isEmpty())
      fileName = fileToFlash;
    burnDialog *cd = new burnDialog(this, 2, &fileName, &backup);
    cd->exec();
    if (IS_TARANIS(GetEepromInterface()->getBoard()))
      backup=false;
    if (!fileName.isEmpty()) {
      settings.setValue("backupOnFlash", backup);
      if (backup) {
        QString tempDir    = QDir::tempPath();
        QString backupFile = tempDir + "/backup.bin";
        bool backupEnable=settings.value("backupEnable", true).toBool();
        QString backupPath=settings.value("backupPath", "").toString();
        if (!backupPath.isEmpty() && !IS_TARANIS(GetEepromInterface()->getBoard())) {
          if (!QDir(backupPath).exists()) {
            if (backupEnable) {
              QMessageBox::warning(this, tr("Backup is impossible"), tr("The backup dir set in preferences does not exist"));
            }
            backupEnable=false;
          }
        } else {
          backupEnable=false;
        }
        if (backupEnable) {
          QDateTime datetime;
          backupFile.clear();
          backupFile=backupPath+"/backup-"+QDateTime().currentDateTime().toString("yyyy-MM-dd-hhmmss")+".bin";
        }
        QStringList str = GetReceiveEEpromCommand(backupFile);
        avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Backup Models and Settings From Tx"), AVR_DIALOG_CLOSE_IF_SUCCESSFUL);
        ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
        int res = ad->exec();
        delete ad;
        if (QFileInfo(backupFile).exists() && res) {
          sleep(1);
          QStringList str = GetSendFlashCommand(fileName);
          avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Write Firmware To Tx"), AVR_DIALOG_CLOSE_IF_SUCCESSFUL);
          CompanionIcon iconw("write_eeprom.png");
          ad->setWindowIcon(iconw);
          int res = ad->exec();
          delete ad;
          if (res) {
            QString restoreFile = tempDir + "/restore.bin";
            if (!convertEEPROM(backupFile, restoreFile, fileName)) {
              QMessageBox::warning(this, tr("Conversion failed"), tr("Cannot convert Models and Settings for use with this firmware, original data will be used"));
              restoreFile = backupFile;
            }
            sleep(1);
            QStringList str = GetSendEEpromCommand(restoreFile);
            avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Restore Models and Settings To Tx"), AVR_DIALOG_CLOSE_IF_SUCCESSFUL);
            ad->setWindowIcon(iconw);
            res = ad->exec();
            delete ad;
            if (!res) {
              QMessageBox::warning(this, tr("Restore failed"), tr("Could not restore Models and Settings to TX. The models and settings data file can be found at: %1").arg(backupFile));
            }
          }
          else {
            QMessageBox::warning(this, tr("Firmware write failed"), tr("Could not write firmware to to transmitter. The models and settings data file can be found at: %1").arg(backupFile));
          }
        }
        else {
          QMessageBox::warning(this, tr("Backup failed"), tr("Cannot backup existing Models and Settings from TX. Firmware write process aborted"));
        }
      }
      else {
        bool backupEnable=settings.value("backupEnable", true).toBool();
        QString backupPath=settings.value("backupPath", "").toString();
        if (!QDir(backupPath).exists()) {
          if (backupEnable) {
            QMessageBox::warning(this, tr("Backup is impossible"), tr("The backup dir set in preferences does not exist"));
          }
          backupEnable=false;
        }
        if (backupEnable && !IS_TARANIS(GetEepromInterface()->getBoard())) {
          QDateTime datetime;
          QString backupFile=backupPath+"/backup-"+QDateTime().currentDateTime().toString("yyyy-MM-dd-hhmmss")+".bin";
          QStringList str = GetReceiveEEpromCommand(backupFile);
          avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Backup Models and Settings From Tx"));
          ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
          ad->exec();
          delete ad;
          sleep(1);
        }

        QStringList str = GetSendFlashCommand(fileName);
        avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Write Firmware To Tx"), AVR_DIALOG_SHOW_DONE);
        ad->setWindowIcon(CompanionIcon("write_flash.png"));
        ad->exec();
        delete ad;
      }
    }
}

void MainWindow::burnExtenalFromEEPROM()
{
    QSettings settings;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save transmitter Models and Settings to File"), settings.value("lastDir").toString(), tr(EXTERNAL_EEPROM_FILES_FILTER));
    if (!fileName.isEmpty()) {
      EEPROMInterface *eepromInterface = GetEepromInterface();
      if (IS_TARANIS(eepromInterface->getBoard())) {
        QString path=FindTaranisPath();
        if (path.isEmpty()) {
          QMessageBox::warning(this, tr("Taranis radio not found"), tr("Impossible to identify the radio on your system, please verify that the eeprom disk is connected."));
          return;
        }
        else {
          QStringList str;            
          str << path << fileName;
          avrOutputDialog *ad = new avrOutputDialog(this,"", str, tr("Read Models and Settings From Tx")); //, AVR_DIALOG_KEEP_OPEN);
          ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
          ad->exec();
          delete ad;
        }
      }
      else {
        settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());
        QStringList str = GetReceiveEEpromCommand(fileName);
        avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Read Models and Settings From Tx"));
        ad->setWindowIcon(CompanionIcon("read_eeprom.png"));
        ad->exec();
        delete ad;
      }
    }
}

void MainWindow::burnFromFlash()
{
    QSettings settings;
    QString fileName = QFileDialog::getSaveFileName(this,tr("Read Tx Firmware to File"), settings.value("lastFlashDir").toString(),tr(FLASH_FILES_FILTER));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.exists()) {
          file.remove();
        }
        settings.setValue("lastFlashDir",QFileInfo(fileName).dir().absolutePath());
        QStringList str = GetReceiveFlashCommand(fileName);
        avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, "Read Firmware From Tx");
        ad->setWindowIcon(CompanionIcon("read_flash.png"));
        ad->exec();
        delete ad;
    }
}

void MainWindow::burnConfig()
{
    burnConfigDialog *bcd = new burnConfigDialog(this);
    bcd->exec();
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
}

void MainWindow::compare()
{
    compareDialog *fd = new compareDialog(this);
    fd->show();
}

void MainWindow::logFile()
{
    logsDialog *fd = new logsDialog(this);
    fd->show();
}

void MainWindow::about()
{
    QString aboutStr = "<center><img src=\":/images/companion-title.png\"></center><br/>";
    aboutStr.append(tr("OpenTX Home Page: <a href='%1'>%1</a>").arg("http://www.open-tx.org"));
    aboutStr.append("<br/><br/>");
    aboutStr.append(tr("The OpenTX Companion project was originally forked from <a href='%2'>eePe</a>").arg("http://code.google.com/p/eepe"));
    aboutStr.append("<br/><br/>");
    aboutStr.append(tr("If you've found this program useful, please support by <a href='%1'>donating</a>").arg(DONATE_STR));
    aboutStr.append("<br/><br/>");
    aboutStr.append(tr("Version %1, %3").arg(C9X_VERSION).arg(__DATE__));
    aboutStr.append("<br/><br/>");
    aboutStr.append(tr("Copyright") + " Bertrand Songis & Romolo Manfredini<br/>&copy; 2011-2014<br/>");
    QMessageBox msgBox(this);
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
    burnToAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);
    
    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
    simulateAct->setEnabled(hasSelection);
    printAct->setEnabled(hasSelection);
    loadbackupAct->setEnabled(hasMdiChild);
    compareAct->setEnabled(activeMdiChild());
    updateRecentFileActions();
    updateProfilesActions();

    QSettings settings;
    setWindowTitle(tr("OpenTX Companion - FW: %1 - Profile: %2").arg(GetCurrentFirmware()->name).arg(settings.value("profileId").toString()));
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;
    mdiArea->addSubWindow(child);
    if(!child->parentWidget()->isMaximized() && !child->parentWidget()->isMinimized())
      child->parentWidget()->resize(400, 400);

    connect(child, SIGNAL(copyAvailable(bool)),cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),copyAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),simulateAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),printAct, SLOT(setEnabled(bool)));

    return child;
}

void MainWindow::createActions()
{
    newAct = new QAction(CompanionIcon("new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(CompanionIcon("open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));
    
    loadbackupAct = new QAction(CompanionIcon("open.png"), tr("&loadBackup..."), this);
    loadbackupAct->setStatusTip(tr("Load backup from file"));
    connect(loadbackupAct, SIGNAL(triggered()), this, SLOT(loadBackup()));

    saveAct = new QAction(CompanionIcon("save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(CompanionIcon("saveas.png"), tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    logsAct = new QAction(CompanionIcon("logs.png"), tr("Lo&gs"), this);
    logsAct->setShortcut(tr("Ctrl+G"));
    logsAct->setStatusTip(tr("Open log file"));
    connect(logsAct, SIGNAL(triggered()), this, SLOT(logFile()));
    
    appPreferencesAct = new QAction(CompanionIcon("apppreferences.png"), tr("&Setting..."), this);
    appPreferencesAct->setStatusTip(tr("Edit Settings"));
    connect(appPreferencesAct, SIGNAL(triggered()), this, SLOT(appPreferences()));

    fwPreferencesAct = new QAction(CompanionIcon("fwpreferences.png"), tr("&Downloads..."), this);
    fwPreferencesAct->setStatusTip(tr("Download firmware and voice files"));
    connect(fwPreferencesAct, SIGNAL(triggered()), this, SLOT(fwPreferences()));

    checkForUpdatesAct = new QAction(CompanionIcon("update.png"), tr("&Check for updates..."), this);
    checkForUpdatesAct->setStatusTip(tr("Check for new version of Companion"));
    connect(checkForUpdatesAct, SIGNAL(triggered()), this, SLOT(doUpdates()));

    contributorsAct = new QAction(CompanionIcon("contributors.png"), tr("Contributors &List..."), this);
    contributorsAct->setStatusTip(tr("Show Companion contributors list"));
    connect(contributorsAct, SIGNAL(triggered()), this, SLOT(contributors()));

    changelogAct = new QAction(CompanionIcon("changelog.png"), tr("ChangeLog..."), this);
    changelogAct->setStatusTip(tr("Show Companion changelog"));
    connect(changelogAct, SIGNAL(triggered()), this, SLOT(changelog()));

    fwchangelogAct = new QAction(CompanionIcon("changelog.png"), tr("Firmware ChangeLog..."), this);
    fwchangelogAct->setStatusTip(tr("Show firmware changelog"));
    connect(fwchangelogAct, SIGNAL(triggered()), this, SLOT(fwchangelog()));
    
    compareAct = new QAction(CompanionIcon("compare.png"), tr("Compare..."), this);
    compareAct->setStatusTip(tr("Compare models"));
    compareAct->setEnabled(false);
    connect(compareAct, SIGNAL(triggered()), this, SLOT(compare()));
    
    customizeSplashAct = new QAction(CompanionIcon("paintbrush.png"), tr("Configure transmitter start screen..."), this);
    customizeSplashAct->setStatusTip(tr("Customize the splash screen of your TX"));
    connect(customizeSplashAct, SIGNAL(triggered()), this, SLOT(customizeSplash()));
    
    exitAct = new QAction(CompanionIcon("exit.png"), tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    cutAct = new QAction(CompanionIcon("cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(CompanionIcon("copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(CompanionIcon("paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    burnToAct = new QAction(CompanionIcon("write_eeprom.png"), tr("&Write Models and Settings To Tx"), this);
    burnToAct->setShortcut(tr("Ctrl+Alt+W"));
    burnToAct->setStatusTip(tr("Write Models and Settings to transmitter"));
    connect(burnToAct,SIGNAL(triggered()),this,SLOT(burnTo()));

    burnFromAct = new QAction(CompanionIcon("read_eeprom.png"), tr("&Read Models and Settings From Tx"), this);
    burnFromAct->setShortcut(tr("Ctrl+Alt+R"));
    burnFromAct->setStatusTip(tr("Read Models and Settings from transmitter"));
    connect(burnFromAct,SIGNAL(triggered()),this,SLOT(burnFrom()));

    burnToFlashAct = new QAction(CompanionIcon("write_flash.png"), tr("Write Firmware"), this);
    burnToFlashAct->setStatusTip(tr("Write firmware to transmitter"));
    connect(burnToFlashAct,SIGNAL(triggered()),this,SLOT(burnToFlash()));

    burnExtenalToEEPROMAct = new QAction(CompanionIcon("write_eeprom_file.png"), tr("Write Models and Settings from file to Tx"), this);
    burnExtenalToEEPROMAct->setStatusTip(tr("Write Models and Settings from file to transmitter"));
    connect(burnExtenalToEEPROMAct,SIGNAL(triggered()),this,SLOT(burnExtenalToEEPROM()));

    burnExtenalFromEEPROMAct = new QAction(CompanionIcon("read_eeprom_file.png"), tr("Save transmitter Models and Settings to file"), this);
    burnExtenalFromEEPROMAct->setStatusTip(tr("Save the Models and Settings from the transmitter to a file"));
    connect(burnExtenalFromEEPROMAct,SIGNAL(triggered()),this,SLOT(burnExtenalFromEEPROM()));

    burnFromFlashAct = new QAction(CompanionIcon("read_flash.png"), tr("Read Firmware"), this);
    burnFromFlashAct->setStatusTip(tr("Read firmware from transmitter"));
    connect(burnFromFlashAct,SIGNAL(triggered()),this,SLOT(burnFromFlash()));

    burnConfigAct = new QAction(CompanionIcon("configure.png"), tr("&Configure connection software..."), this);
    burnConfigAct->setStatusTip(tr("Configure software for reading from and writing to the transmitter"));
    connect(burnConfigAct,SIGNAL(triggered()),this,SLOT(burnConfig()));
    EEPROMInterface *eepromInterface = GetEepromInterface();
    if (!IS_ARM(eepromInterface->getBoard())) {
      burnListAct = new QAction(CompanionIcon("list.png"), tr("&List programmers"), this);
      burnListAct->setStatusTip(tr("List available programmers"));
      connect(burnListAct,SIGNAL(triggered()),this,SLOT(burnList()));

      burnFusesAct = new QAction(CompanionIcon("fuses.png"), tr("&Fuses..."), this);
      burnFusesAct->setStatusTip(tr("Show fuses dialog"));
      connect(burnFusesAct,SIGNAL(triggered()),this,SLOT(burnFuses()));
    }

    simulateAct = new QAction(CompanionIcon("simulate.png"), tr("&Simulate"), this);
    simulateAct->setShortcut(tr("Alt+S"));
    simulateAct->setStatusTip(tr("Simulate selected model."));
    simulateAct->setEnabled(false);
    connect(simulateAct,SIGNAL(triggered()),this,SLOT(simulate()));

    printAct = new QAction(CompanionIcon("print.png"), tr("&Print"), this);
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setStatusTip(tr("Print current model."));
    printAct->setEnabled(false);
    connect(printAct,SIGNAL(triggered()),this,SLOT(print()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(CompanionIcon("information.png"), tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    for (int i = 0; i < MaxRecentFiles; ++i)  {
      recentFileActs[i] = new QAction(this);
      recentFileActs[i]->setVisible(false);
      connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
    updateRecentFileActions();

    for (int i=0; i<MAX_PROFILES; i++) {
      profileActs[i] = new QAction(this);
      profileActs[i]->setVisible(false);
      connect(profileActs[i], SIGNAL(triggered()), this, SLOT(loadProfile()));
    }
    updateProfilesActions();
    createProfileAct = new QAction(tr("New Profile"), this);
    createProfileAct->setStatusTip(tr("Create a new Radio Setting Profile"));
    connect(createProfileAct, SIGNAL(triggered()), this, SLOT(createProfile()));

    classicThemeAct = new QAction(tr("Classic"), this);
    classicThemeAct->setStatusTip(tr("The multicolor classical Companion icon theme"));
    connect(classicThemeAct, SIGNAL(triggered()), this, SLOT(setClassicTheme()));
    monoThemeAct = new QAction("Monochrome",this);
    monoThemeAct->setStatusTip(tr("A monochrome black icon theme"));
    connect(monoThemeAct, SIGNAL(triggered()), this, SLOT(setMonochromeTheme()));
    monoWhiteThemeAct = new QAction("MonoWhite",this);
    monoWhiteThemeAct->setStatusTip(tr("A monochrome white icon theme"));
    connect(monoWhiteThemeAct, SIGNAL(triggered()), this, SLOT(setMonoWhiteTheme()));
    monoBlueThemeAct = new QAction("MonoBlue",this);
    monoBlueThemeAct->setStatusTip(tr("A monochrome blue icon theme"));
    connect(monoBlueThemeAct, SIGNAL(triggered()), this, SLOT(setMonoBlueTheme()));

    smallIconAct = new QAction(tr("Small"),this);
    smallIconAct->setStatusTip(tr("Use small toolbar icons"));
    connect(smallIconAct, SIGNAL(triggered()), this, SLOT(setSmallIconThemeSize()));
    normalIconAct = new QAction(this);
    normalIconAct->setStatusTip(tr("Use normal size toolbar icons"));
    normalIconAct->setText(tr("Normal"));
    connect(normalIconAct, SIGNAL(triggered()), this, SLOT(setNormalIconThemeSize()));
    bigIconAct = new QAction(this);
    bigIconAct->setStatusTip(tr("Use big toolbar icons"));
    bigIconAct->setText(tr("Big"));
    connect(bigIconAct, SIGNAL(triggered()), this, SLOT(setBigIconThemeSize()));
    hugeIconAct = new QAction(this);    
    hugeIconAct->setStatusTip(tr("Use huge toolbar icons"));
    hugeIconAct->setText(tr("Huge"));   
    connect(hugeIconAct, SIGNAL(triggered()), this, SLOT(setHugeIconThemeSize()));

    defaultLanguageAct = new QAction(tr("System default language"),this);
    defaultLanguageAct->setStatusTip(tr("Use system default language in menus"));
    connect(defaultLanguageAct, SIGNAL(triggered()), this, SLOT(setDefaultLanguage()));

    czechLanguageAct = new QAction(tr("Czech"),this);
    czechLanguageAct->setStatusTip(tr("Use Czech in menus"));
    connect(czechLanguageAct, SIGNAL(triggered()), this, SLOT(setCZLanguage()));

    germanLanguageAct = new QAction(tr("German"),this);
    germanLanguageAct->setStatusTip(tr("Use German in menus"));
    connect(germanLanguageAct, SIGNAL(triggered()), this, SLOT(setDELanguage()));

    englishLanguageAct = new QAction(tr("English"),this);
    englishLanguageAct->setStatusTip(tr("Use English in menus"));
    connect(englishLanguageAct, SIGNAL(triggered()), this, SLOT(setENLanguage()));

    frenchLanguageAct = new QAction(tr("French"),this);
    frenchLanguageAct->setStatusTip(tr("Use French in menus"));
    connect(frenchLanguageAct, SIGNAL(triggered()), this, SLOT(setFRLanguage()));

    italianLanguageAct = new QAction(tr("Italian"),this);
    italianLanguageAct->setStatusTip(tr("Use Italian in menus"));
    connect(italianLanguageAct, SIGNAL(triggered()), this, SLOT(setITLanguage()));

    hebrewLanguageAct = new QAction(tr("Hebrew"),this);
    hebrewLanguageAct->setStatusTip(tr("Use Hebrew in menus"));
    connect(hebrewLanguageAct, SIGNAL(triggered()), this, SLOT(setHELanguage()));

    polishLanguageAct = new QAction(tr("Polish"),this);
    polishLanguageAct->setStatusTip(tr("Use Polish in menus"));
    connect(polishLanguageAct, SIGNAL(triggered()), this, SLOT(setPLLanguage()));

    portugueseLanguageAct = new QAction(tr("Portuguese"),this);
    portugueseLanguageAct->setStatusTip(tr("Use Portuguese in menus"));
    connect(portugueseLanguageAct, SIGNAL(triggered()), this, SLOT(setPTLanguage()));

    swedishLanguageAct = new QAction(tr("Swedish"),this);
    swedishLanguageAct->setStatusTip(tr("Use Swedish in menus"));
    connect(swedishLanguageAct, SIGNAL(triggered()), this, SLOT(setSELanguage()));

    russianLanguageAct = new QAction(tr("Russian"),this);
    russianLanguageAct->setStatusTip(tr("Use Russian in menus"));
    connect(russianLanguageAct, SIGNAL(triggered()), this, SLOT(setRULanguage()));

    openDocumentURLAct = new QAction(tr("Manuals and other Documents"),this);
    openDocumentURLAct->setStatusTip(tr("Open the OpenTX document page in a web browser"));
    connect(openDocumentURLAct, SIGNAL(triggered()), this, SLOT(openDocumentURL()));

}

void MainWindow::createMenus()

{
    QMenu *recentFileMenu=new QMenu(tr("Recent Files"));
    QMenu *languageMenu=new QMenu(tr("Set Menu Language"));
    QMenu *themeMenu=new QMenu(tr("Set Icon Theme"));
    QMenu *iconThemeSizeMenu=new QMenu(tr("Set Icon Size"));
    
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addMenu(recentFileMenu);
    recentFileMenu->setIcon(CompanionIcon("recentdocument.png"));
    for (int i=0; i<MaxRecentFiles; ++i)
      recentFileMenu->addAction(recentFileActs[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(logsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(simulateAct);
    fileMenu->addAction(printAct);
    fileMenu->addAction(compareAct);
    fileMenu->addSeparator();
    fileMenu->addAction(fwPreferencesAct);
    fileMenu->addMenu(createProfilesMenu());
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    settingsMenu = menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addMenu(languageMenu);
      languageMenu->addAction(defaultLanguageAct);
      languageMenu->addAction(englishLanguageAct);
      languageMenu->addAction(czechLanguageAct);
      languageMenu->addAction(germanLanguageAct);
      languageMenu->addAction(frenchLanguageAct);
      languageMenu->addAction(hebrewLanguageAct);
      languageMenu->addAction(italianLanguageAct);
      languageMenu->addAction(polishLanguageAct);
      languageMenu->addAction(portugueseLanguageAct);
      languageMenu->addAction(swedishLanguageAct);
      languageMenu->addAction(russianLanguageAct);
    settingsMenu->addMenu(themeMenu);
      themeMenu->addAction(classicThemeAct);
      themeMenu->addAction(monoThemeAct);
      themeMenu->addAction(monoBlueThemeAct);
      themeMenu->addAction(monoWhiteThemeAct);
    settingsMenu->addMenu(iconThemeSizeMenu);
      iconThemeSizeMenu->addAction(smallIconAct);
      iconThemeSizeMenu->addAction(normalIconAct);
      iconThemeSizeMenu->addAction(bigIconAct);
      iconThemeSizeMenu->addAction(hugeIconAct);
    settingsMenu->addSeparator();
    settingsMenu->addAction(appPreferencesAct);
    settingsMenu->addAction(customizeSplashAct);
    settingsMenu->addAction(burnConfigAct);

    burnMenu = menuBar()->addMenu(tr("&Read/Write"));
    burnMenu->addAction(burnToAct);
    burnMenu->addAction(burnFromAct);
    burnMenu->addSeparator();
    burnMenu->addAction(burnExtenalToEEPROMAct);
    burnMenu->addAction(burnExtenalFromEEPROMAct);
    burnMenu->addSeparator();
    burnMenu->addAction(burnToFlashAct);
    burnMenu->addAction(burnFromFlashAct);
    burnMenu->addSeparator();
    burnMenu->addSeparator();
    EEPROMInterface *eepromInterface = GetEepromInterface();
    if (!IS_ARM(eepromInterface->getBoard())) {    
      burnMenu->addAction(burnFusesAct);
      burnMenu->addAction(burnListAct);
    }
    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addSeparator();
    helpMenu->addAction(checkForUpdatesAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(openDocumentURLAct);
    helpMenu->addSeparator();
    helpMenu->addAction(changelogAct);
    helpMenu->addAction(fwchangelogAct);
    helpMenu->addSeparator();
    helpMenu->addAction(contributorsAct);
}
 
QMenu *MainWindow::createRecentFileMenu()
{
    QMenu *recentFileMenu = new QMenu(this);
    for ( int i = 0; i < MaxRecentFiles; ++i)
      recentFileMenu->addAction(recentFileActs[i]);
    return recentFileMenu;
}

QMenu *MainWindow::createProfilesMenu()
{
    QMenu *profilesMenu=new QMenu(tr("Radio Settings Profiles"));
    int i;
    for ( i = 0; i < MAX_PROFILES; ++i) {
      profilesMenu->addAction(profileActs[i]);
    }
    if ( i>0 )
      profilesMenu->addSeparator();
    profilesMenu->addAction(createProfileAct);
    profilesMenu->setIcon(CompanionIcon("profiles.png"));
    return profilesMenu;
}

void MainWindow::createToolBars()
{
    QSettings settings;
    int icon_size=settings.value("icon_size",2 ).toInt();
    QSize size;
    switch(icon_size) {
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
      recentToolButton->setMenu(createRecentFileMenu());
      recentToolButton->setIcon(CompanionIcon("recentdocument.png"));
      recentToolButton->setToolTip(tr("Recent Files"));
    fileToolBar->addWidget(recentToolButton);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(logsAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(appPreferencesAct);
    fileToolBar->addAction(fwPreferencesAct);
    fileToolBar->addAction(customizeSplashAct);
      profileButton = new QToolButton;
      profileButton->setPopupMode(QToolButton::InstantPopup);
      profileButton->setMenu(createProfilesMenu());
      profileButton->setIcon(CompanionIcon("profiles.png"));
      profileButton->setToolTip(tr("Firmware Profiles"));
    fileToolBar->addWidget(profileButton);
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
    burnToolBar->addAction(burnToAct);
    burnToolBar->addAction(burnFromAct);
    burnToolBar->addSeparator();
    burnToolBar->addAction(burnExtenalToEEPROMAct);
    burnToolBar->addAction(burnExtenalFromEEPROMAct);
    burnToolBar->addSeparator();
    burnToolBar->addAction(burnToFlashAct);
    burnToolBar->addAction(burnFromFlashAct);
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

void MainWindow::readSettings()
{
    QSettings settings;
    restoreState(settings.value("mainWindowState").toByteArray());
    checkCompanion9x = settings.value("startup_check_companion", true).toBool();
    checkFW = settings.value("startup_check_fw", true).toBool();
    MaxRecentFiles =settings.value("history_size",10).toInt();
    if (settings.value("profileId",0).toInt() == 0)
    {
      createProfile();
      settings.setValue("profileId", "1");
    }
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
    int i,j, numRecentFiles;
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
 
    numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
 
    for ( i = 0; i < numRecentFiles; ++i)  {
      QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
      recentFileActs[i]->setText(text);
      recentFileActs[i]->setData(files[i]);
      recentFileActs[i]->setVisible(true);
    }
    for ( j = numRecentFiles; j < MaxRecentFiles; ++j)
      recentFileActs[j]->setVisible(false);
 
    // separatorAct->setVisible(numRecentFiles > 0);
}

void MainWindow::updateProfilesActions()
 {
    int i;
    QSettings settings;
    int activeProfile = settings.value("profileId").toInt();

    settings.beginGroup("Profiles");
    for (i=0; i<MAX_PROFILES; i++) {
      QString profile=QString("profile%1").arg(i+1);
      settings.beginGroup(profile);
      QString name=settings.value("Name","").toString();
      if (!name.isEmpty()) {
        QString text = tr("&%1 %2").arg(i + 1).arg(name);
        profileActs[i]->setText(text);
        profileActs[i]->setData(i+1);
        profileActs[i]->setVisible(true);
        if ((i+1) == activeProfile)
          profileActs[i]->setIcon(CompanionIcon("arrow-right.png"));
        else
          profileActs[i]->setIcon(CompanionIcon(""));

      } else {
        profileActs[i]->setVisible(false);
      }
      settings.endGroup();
    }
}

void MainWindow::createProfile()
{
  int firstFreeIndex = 0;
  QSettings settings;
  settings.beginGroup("Profiles");
  for (int i=0; firstFreeIndex ==0 && i<MAX_PROFILES; i++) {
    QString profile=QString("profile%1").arg(i+1);
    settings.beginGroup(profile);  
    QString name=settings.value("Name","").toString();
    if (name.isEmpty())
      firstFreeIndex = i+1;
    settings.endGroup();
  }
  settings.endGroup();
  if (firstFreeIndex == 0)  // Could not find free index
    return;

  settings.beginGroup("Profiles");
  settings.beginGroup(QString("profile%1").arg(firstFreeIndex));
  settings.setValue("Name",QString("profile%1").arg(firstFreeIndex));
  settings.endGroup();
  settings.endGroup();

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
  if (fileType==FILE_TYPE_XML) {
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
      QMessageBox::critical(this, tr("Error"),tr("Error opening file %1:\n%2.").arg(fileName).arg(file.errorString()));
      return -1;
    }
    QTextStream inputStream(&file);
    XmlInterface(inputStream).load(testData);
  }
  else if (fileType==FILE_TYPE_HEX || fileType==FILE_TYPE_EEPE) { //read HEX file
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
    uint8_t *eeprom = (uint8_t *)malloc(eeprom_size);
    memset(eeprom, 0, eeprom_size);
    long result = file.read((char*)eeprom, eeprom_size);
    file.close();
    if (result != eeprom_size) {
      QMessageBox::critical(this, tr("Error"),tr("Error reading file %1:\n%2.").arg(fileName).arg(file.errorString()));
      return -1;
    }
    if (!LoadEeprom(testData, eeprom, eeprom_size)) {
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
    QSettings settings;
    settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());

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


