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

#include "mainwindow.h"
#include "mdichild.h"
#include "burnconfigdialog.h"
#include "comparedialog.h"
#include "logsdialog.h"
#include "apppreferencesdialog.h"
#include "fwpreferencesdialog.h"
#include "firmwareinterface.h"
#include "fusesdialog.h"
#include "downloaddialog.h"
#include "printdialog.h"
#include "version.h"
#include "creditsdialog.h"
#include "releasenotesdialog.h"
#include "releasenotesfirmwaredialog.h"
#include "customizesplashdialog.h"
#include "flasheepromdialog.h"
#include "flashfirmwaredialog.h"
#include "hexinterface.h"
#include "warnings.h"
#include "helpers.h"
#include "appdata.h"
#include "radionotfound.h"
#include "process_sync.h"
#include "radiointerface.h"
#include "progressdialog.h"
#include "progresswidget.h"
#include "storage.h"
#include "translations.h"

#include <QtGui>
#include <QNetworkProxyFactory>
#include <QFileInfo>
#include <QDesktopServices>

#define OPENTX_DOWNLOADS_PAGE_URL         "http://www.open-tx.org/downloads"
#define DONATE_STR                        "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=QUZ48K4SEXDP2"

#ifdef __APPLE__
  #define COMPANION_STAMP                 "companion-macosx.stamp"
  #define COMPANION_INSTALLER             "macosx/opentx-companion-%1.dmg"
  #define COMPANION_FILEMASK              QT_TRANSLATE_NOOP("MainWindow", "Diskimage (*.dmg)")
  #define COMPANION_INSTALL_QUESTION      QT_TRANSLATE_NOOP("MainWindow", "Would you like to open the disk image to install the new version?")
#elif WIN32
  #define COMPANION_STAMP                 "companion-windows.stamp"
  #define COMPANION_INSTALLER             "windows/companion-windows-%1.exe"
  #define COMPANION_FILEMASK              QT_TRANSLATE_NOOP("MainWindow", "Executable (*.exe)")
  #define COMPANION_INSTALL_QUESTION      QT_TRANSLATE_NOOP("MainWindow", "Would you like to launch the installer?")
#else
  #define COMPANION_STAMP                 "companion-linux.stamp"
  #define COMPANION_INSTALLER             "" // no automated updates for linux
  #define COMPANION_FILEMASK              "*.*"
  #define COMPANION_INSTALL_QUESTION      QT_TRANSLATE_NOOP("MainWindow", "Would you like to launch the installer?")
#endif

const char * const OPENTX_COMPANION_DOWNLOAD_URL[] = {
  "https://downloads.open-tx.org/2.2/release/companion",
  "https://downloads.open-tx.org/2.2/rc/companion",
  "https://downloads.open-tx.org/2.2/nightlies/companion"
};

MainWindow::MainWindow():
  downloadDialog_forWait(nullptr),
  checkForUpdatesState(0),
  networkManager(nullptr),
  windowsListActions(new QActionGroup(this))
{
  // setUnifiedTitleAndToolBarOnMac(true);
  this->setWindowIcon(QIcon(":/icon.png"));
  QNetworkProxyFactory::setUseSystemConfiguration(true);
  setAcceptDrops(true);

  mdiArea = new QMdiArea(this);
  mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setTabsClosable(true);
  mdiArea->setTabsMovable(true);
  mdiArea->setDocumentMode(true);
  connect(mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::updateMenus);

  setCentralWidget(mdiArea);

  createActions();
  createMenus();
  createToolBars();
  retranslateUi();
  updateMenus();

  setIconThemeSize(g.iconSize());
  restoreGeometry(g.mainWinGeo());
  restoreState(g.mainWinState());
  setTabbedWindows(g.tabbedMdi());

  connect(windowsListActions, &QActionGroup::triggered, this, &MainWindow::onChangeWindowAction);

  // give time to the splash to disappear and main window to open before starting updates
  int updateDelay = 1000;
  bool showSplash = g.showSplash();
  if (showSplash) {
    updateDelay += (SPLASH_TIME*1000);
  }

  if (g.isFirstUse()) {
    g.warningId(g.warningId() | AppMessages::MSG_WELCOME);
    QTimer::singleShot(updateDelay-500, this, SLOT(appPrefs()));  // must be shown before warnings dialog but after splash
  }
  else {
    if (!g.previousVersion().isEmpty())
      g.warningId(g.warningId() | AppMessages::MSG_UPGRADED);
    QTimer::singleShot(updateDelay, this, SLOT(doAutoUpdates()));
  }
  QTimer::singleShot(updateDelay, this, SLOT(displayWarnings()));

  QStringList strl = QApplication::arguments();
  QString str;
  QString printfilename;
  int printing = strl.contains("--print");
  int model = -1;
  int count = 0;
  foreach(QString arg, strl) {
    count++;
    if (arg.contains("--model")) {
      model = strl[count].toInt() - 1;
    }
    if (arg.contains("--filename")) {
      printfilename = strl[count];
    }
  }
  if (strl.count() > 1)
    str = strl[1];
  if (!str.isEmpty()) {
    int fileType = getStorageType(str);

    if (fileType==STORAGE_TYPE_EEPE || fileType==STORAGE_TYPE_EEPM || fileType==STORAGE_TYPE_BIN || fileType==STORAGE_TYPE_OTX) {
      MdiChild * child = createMdiChild();
      if (child->loadFile(str)) {
        if (!(printing && model >= 0 && (getCurrentFirmware()->getCapability(Models) == 0 || model<getCurrentFirmware()->getCapability(Models)) && !printfilename.isEmpty())) {
          statusBar()->showMessage(tr("File loaded"), 2000);
          child->show();
        }
        else {
          child->show();
          child->print(model, printfilename);
          child->close();
        }
      }
      else {
        child->closeFile(true);
      }
    }
  }
  if (printing) {
    QTimer::singleShot(0, this, SLOT(autoClose()));
  }
}

MainWindow::~MainWindow()
{
  if (windowsListActions) {
    delete windowsListActions;
    windowsListActions = NULL;
  }
}

void MainWindow::displayWarnings()
{
  using namespace AppMessages;
  static uint shownMsgs = 0;
  int showMsgs = g.warningId();
  int msgId;
  QString infoTxt;

  if ((showMsgs & MSG_WELCOME) && !(shownMsgs & MSG_WELCOME)) {
    infoTxt = CPN_STR_MSG_WELCOME.arg(VERSION);
    msgId = MSG_WELCOME;
  }
  else if ((showMsgs & MSG_UPGRADED) && !(shownMsgs & MSG_UPGRADED)) {
    infoTxt = CPN_STR_MSG_UPGRADED.arg(VERSION);
    msgId = MSG_UPGRADED;
  }
  else {
    return;
  }

  QMessageBox msgBox(this);
  msgBox.setWindowTitle(CPN_STR_APP_NAME);
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setInformativeText(infoTxt);
  QCheckBox * cb = new QCheckBox(tr("Show this message again at next startup?"), &msgBox);
  msgBox.setCheckBox(cb);

  msgBox.exec();

  shownMsgs |= msgId;
  if (!cb->isChecked())
    g.warningId(showMsgs & ~msgId);

  displayWarnings();  // in case more warnings need showing
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

QString MainWindow::getCompanionUpdateBaseUrl()
{
  return OPENTX_COMPANION_DOWNLOAD_URL[g.boundedOpenTxBranch()];
}

void MainWindow::checkForUpdates()
{
  if (!checkForUpdatesState) {
    closeUpdatesWaitDialog();
    if (networkManager) {
      networkManager->deleteLater();
      networkManager = nullptr;
    }
    return;
  }

  if (checkForUpdatesState & SHOW_DIALOG_WAIT) {
    checkForUpdatesState -= SHOW_DIALOG_WAIT;
    downloadDialog_forWait = new downloadDialog(NULL, tr("Checking for updates"));
    downloadDialog_forWait->show();
  }

  if (networkManager)
    disconnect(networkManager, 0, this, 0);
  else
    networkManager = new QNetworkAccessManager(this);

  QUrl url;
  if (checkForUpdatesState & CHECK_COMPANION) {
    checkForUpdatesState -= CHECK_COMPANION;
    url.setUrl(QString("%1/%2").arg(getCompanionUpdateBaseUrl()).arg(COMPANION_STAMP));
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::checkForCompanionUpdateFinished);
    qDebug() << "Checking for Companion update " << url.url();
  }
  else if (checkForUpdatesState & CHECK_FIRMWARE) {
    checkForUpdatesState -= CHECK_FIRMWARE;
    const QString stamp = getCurrentFirmware()->getStampUrl();
    if (!stamp.isEmpty()) {
      url.setUrl(stamp);
      connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::checkForFirmwareUpdateFinished);
      qDebug() << "Checking for firmware update " << url.url();
    }
  }
  if (!url.isValid()) {
    checkForUpdates();
    return;
  }

  QNetworkRequest request(url);
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
  networkManager->get(request);
}

void MainWindow::onUpdatesError()
{
  checkForUpdatesState = 0;
  closeUpdatesWaitDialog();
  QMessageBox::warning(this, CPN_STR_APP_NAME, tr("Unable to check for updates."));
}

void MainWindow::closeUpdatesWaitDialog()
{
  if (downloadDialog_forWait) {
    downloadDialog_forWait->close();
    delete downloadDialog_forWait;
    downloadDialog_forWait = NULL;
  }
}

QString MainWindow::seekCodeString(const QByteArray & qba, const QString & label)
{
  int posLabel = qba.indexOf(label);
  if (posLabel < 0)
    return QString::null;
  int start = qba.indexOf("\"", posLabel + label.length());
  if (start < 0)
    return QString::null;
  int end = qba.indexOf("\"", start + 1);
  if (end < 0)
    return QString::null;
  return qba.mid(start + 1, end - start - 1);
}

void MainWindow::checkForCompanionUpdateFinished(QNetworkReply * reply)
{
  QByteArray qba = reply->readAll();
  reply->deleteLater();
  QString version = seekCodeString(qba, "VERSION");
  if (version.isNull())
    return onUpdatesError();

  int webVersion = version2index(version);

  int ownVersion = version2index(VERSION);

  if (ownVersion < webVersion) {
#if defined WIN32 || defined __APPLE__
    int ret = QMessageBox::question(this, CPN_STR_APP_NAME, tr("A new version of Companion is available (version %1)<br>"
                                                        "Would you like to download it?").arg(version) ,
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
      QDir dir(g.updatesDir());
      QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), dir.absoluteFilePath(QString(COMPANION_INSTALLER).arg(version)), tr(COMPANION_FILEMASK));

      if (!fileName.isEmpty()) {
        g.updatesDir(QFileInfo(fileName).dir().absolutePath());
        downloadDialog * dd = new downloadDialog(this, QString("%1/%2").arg(getCompanionUpdateBaseUrl()).arg(QString(COMPANION_INSTALLER).arg(version)), fileName);
        installer_fileName = fileName;
        connect(dd, SIGNAL(accepted()), this, SLOT(updateDownloaded()));
        dd->exec();
      }
    }
#else
    QMessageBox::warning(this, tr("New release available"), tr("A new release of Companion is available, please check the <a href='%1'>OpenTX website!</a>").arg(OPENTX_DOWNLOADS_PAGE_URL));
#endif
  }
  else {
    if (downloadDialog_forWait && checkForUpdatesState==0) {
      QMessageBox::information(this, CPN_STR_APP_NAME, tr("No updates available at this time."));
    }
  }

  checkForUpdates();
}

void MainWindow::updateDownloaded()
{
  int ret = QMessageBox::question(this, CPN_STR_APP_NAME, tr(COMPANION_INSTALL_QUESTION), QMessageBox::Yes | QMessageBox::No);
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
    QMessageBox::critical(this, CPN_STR_TTL_ERROR,
        tr("Error opening file %1:\n%2.")
        .arg(g.profile[g.id()].fwName())
        .arg(file.errorString()));
    return;
  }
  file.reset();
  QTextStream inputStream(&file);
  QString hline = inputStream.readLine();
  if (hline.startsWith("ERROR:")) {
    int errnum = hline.mid(6).toInt();
    switch(errnum) {
      case 1:
        errormsg = tr("Not enough flash available on this board for all the selected options");
        break;
      case 2:
        errormsg = tr("Compilation server temporary failure, try later");
        break;
      case 3:
        errormsg = tr("Compilation server too busy, try later");
        break;
      case 4:
        errormsg = tr("Compilation error");
        break;
      case 5:
        errormsg = tr("Invalid firmware");
        break;
      case 6:
        errormsg = tr("Invalid board");
        break;
      case 7:
        errormsg = tr("Invalid language");
        break;
      default:
        errormsg = tr("Unknown server failure, try later");
        break;
    }
    file.close();
    file.remove();
    QMessageBox::critical(this, CPN_STR_TTL_ERROR, errormsg);
    return;
  }
  file.close();
  g.fwRev.set(Firmware::getCurrentVariant()->getId(), version2index(firmwareVersionString));
  if (g.profile[g.id()].burnFirmware()) {
    int ret = QMessageBox::question(this, CPN_STR_APP_NAME, tr("Do you want to write the firmware to the radio now ?"), QMessageBox::Yes | QMessageBox::No);
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
  reply->deleteLater();
  QString versionString = seekCodeString(qba, "VERSION");
  QString dateString = seekCodeString(qba, "DATE");
  if (versionString.isNull() || dateString.isNull())
    return onUpdatesError();

  long version = version2index(versionString);
  if (version <= 0)
    return onUpdatesError();

  QString fullVersionString = QString("%1 (%2)").arg(versionString).arg(dateString);

  if (checkForUpdatesState & AUTOMATIC_DOWNLOAD) {
    checkForUpdatesState -= AUTOMATIC_DOWNLOAD;
    download = true;
  }
  else {
    int currentVersion = g.fwRev.get(Firmware::getCurrentVariant()->getId());
    QString currentVersionString = index2version(currentVersion);

    QMessageBox msgBox;
    msgBox.setWindowTitle(CPN_STR_APP_NAME);
    QSpacerItem * horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout * layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    if (currentVersion == 0) {
      QString rn = getCurrentFirmware()->getReleaseNotesUrl();
      QAbstractButton *rnButton = NULL;
      msgBox.setText(tr("Firmware %1 does not seem to have ever been downloaded.\nVersion %2 is available.\nDo you want to download it now?\n\nWe recommend you view the release notes using the button below to learn about any changes that may be important to you.")
                     .arg(Firmware::getCurrentVariant()->getId()).arg(fullVersionString));
      QAbstractButton *YesButton = msgBox.addButton(trUtf8("Yes"), QMessageBox::YesRole);
      msgBox.addButton(trUtf8("No"), QMessageBox::NoRole);
      if (!rn.isEmpty()) {
        rnButton = msgBox.addButton(trUtf8("Release Notes"), QMessageBox::ActionRole);
      }
      msgBox.setIcon(QMessageBox::Question);
      msgBox.resize(0, 0);
      msgBox.exec();
      if (msgBox.clickedButton() == rnButton) {
        ReleaseNotesFirmwareDialog * dialog = new ReleaseNotesFirmwareDialog(this, rn);
        dialog->exec();
        int ret2 = QMessageBox::question(this, CPN_STR_APP_NAME, tr("Do you want to download version %1 now ?").arg(fullVersionString), QMessageBox::Yes | QMessageBox::No);
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
      QString rn = getCurrentFirmware()->getReleaseNotesUrl();
      QAbstractButton *rnButton = NULL;
      msgBox.setText(tr("A new version of %1 firmware is available:\n  - current is %2\n  - newer is %3\n\nDo you want to download it now?\n\nWe recommend you view the release notes using the button below to learn about any changes that may be important to you.")
                     .arg(Firmware::getCurrentVariant()->getId()).arg(currentVersionString).arg(fullVersionString));
      QAbstractButton *YesButton = msgBox.addButton(trUtf8("Yes"), QMessageBox::YesRole);
      msgBox.addButton(trUtf8("No"), QMessageBox::NoRole);
      if (!rn.isEmpty()) {
        rnButton = msgBox.addButton(trUtf8("Release Notes"), QMessageBox::ActionRole);
      }
      msgBox.setIcon(QMessageBox::Question);
      msgBox.resize(0,0);
      msgBox.exec();
      if( msgBox.clickedButton() == rnButton ) {
        ReleaseNotesFirmwareDialog * dialog = new ReleaseNotesFirmwareDialog(this, rn);
        dialog->exec();
        int ret2 = QMessageBox::question(this, CPN_STR_APP_NAME, tr("Do you want to download version %1 now ?").arg(fullVersionString),
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
        QMessageBox::information(this, CPN_STR_APP_NAME, tr("No updates available at this time."));
      }
    }
  }

  if (ignore) {
    int res = QMessageBox::question(this, CPN_STR_APP_NAME, tr("Ignore this version %1?").arg(fullVersionString), QMessageBox::Yes | QMessageBox::No);
    if (res==QMessageBox::Yes)   {
      g.fwRev.set(Firmware::getCurrentVariant()->getId(), version);
    }
  }
  else if (download == true) {
    firmwareVersionString = versionString;
    startFirmwareDownload();
  }

  checkForUpdates();
}

void MainWindow::startFirmwareDownload()
{
  QString url = Firmware::getCurrentVariant()->getFirmwareUrl();
  qDebug() << "Downloading firmware" << url;
  QString ext = url.mid(url.lastIndexOf("."));
  QString defaultFilename = g.flashDir() + "/" + Firmware::getCurrentVariant()->getId();
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
  g.tabbedMdi(actTabbedWindows->isChecked());
  QApplication::closeAllWindows();
  mdiArea->closeAllSubWindows();
  if (mdiArea->currentSubWindow()) {
    event->ignore();
  }
  else {
    event->accept();
  }
}

void MainWindow::changeEvent(QEvent * e)
{
  QMainWindow::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      retranslateUi(true);
      break;
    default:
      break;
  }
}

void MainWindow::setLanguage(const QString & langString)
{
  g.locale(langString);
  Translations::installTranslators();
}

void MainWindow::onLanguageChanged(QAction * act)
{
  QString lang = act->property("locale").toString();
  if (!lang.isNull())
    setLanguage(lang);
}

void  MainWindow::setTheme(int index)
{
  g.theme(index);
  QMessageBox::information(this, CPN_STR_APP_NAME, tr("The new theme will be loaded the next time you start Companion."));
}

void MainWindow::onThemeChanged(QAction * act)
{
  bool ok;
  int id = act->property("themeId").toInt(&ok);
  if (ok && id >= 0 && id < 5)
    setTheme(id);
}

void  MainWindow::setIconThemeSize(int index)
{
  if (index != g.iconSize())
    g.iconSize(index);

  QSize size;
  switch(g.iconSize()) {
    case 0:
      size=QSize(16,16);
      break;
    case 2:
      size=QSize(32,32);
      break;
    case 3:
      size=QSize(48,48);
      break;
    case 1:
    default:
      size=QSize(24,24);
      break;
  }
  this->setIconSize(size);
}

void MainWindow::onIconSizeChanged(QAction * act)
{
  bool ok;
  int id = act->property("sizeId").toInt(&ok);
  if (ok && id >= 0 && id < 4)
    setIconThemeSize(id);
}

void MainWindow::setTabbedWindows(bool on)
{
  mdiArea->setViewMode(on ? QMdiArea::TabbedView : QMdiArea::SubWindowView);
  if (actTileWindows)
    actTileWindows->setDisabled(on);
  if (actCascadeWindows)
    actCascadeWindows->setDisabled(on);

  if (actTabbedWindows->isChecked() != on)
    actTabbedWindows->setChecked(on);
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

void MainWindow::openFile(const QString & fileName, bool updateLastUsedDir)
{
  if (!fileName.isEmpty()) {
    if (updateLastUsedDir) {
      g.eepromDir(QFileInfo(fileName).dir().absolutePath());
    }

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
    else {
      child->closeFile(true);
    }
  }
}

void MainWindow::openFile()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open Models and Settings file"), g.eepromDir(), EEPROM_FILES_FILTER);
  openFile(fileName);
}

void MainWindow::save()
{
  if (activeMdiChild() && activeMdiChild()->save()) {
    statusBar()->showMessage(tr("File saved"), 2000);
  }
}

void MainWindow::saveAs()
{
  if (activeMdiChild() && activeMdiChild()->saveAs()) {
    statusBar()->showMessage(tr("File saved"), 2000);
  }
}

void MainWindow::saveAll()
{
  foreach (QMdiSubWindow * window, mdiArea->subWindowList()) {
    MdiChild * child;
    if ((child = qobject_cast<MdiChild *>(window->widget())) && child->isWindowModified())
      child->save();
  }
}

void MainWindow::closeFile()
{
  if (mdiArea->activeSubWindow())
    mdiArea->activeSubWindow()->close();
}

void MainWindow::openRecentFile()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) {
    QString fileName = action->data().toString();
    openFile(fileName, false);
  }
}

bool MainWindow::loadProfileId(const unsigned pid)  // TODO Load all variables - Also HW!
{
  if (pid >= MAX_PROFILES)
    return false;

  Firmware * newFw = Firmware::getFirmwareForId(g.profile[pid].fwType());
  // warn if we're switching between incompatible board types and any files have been modified
  if (!Boards::isBoardCompatible(Firmware::getCurrentVariant()->getBoard(), newFw->getBoard()) && anyChildrenDirty()) {
    if (QMessageBox::question(this, CPN_STR_APP_NAME,
                              tr("There are unsaved file changes which you may lose when switching radio types.\n\nDo you wish to continue?"),
                              (QMessageBox::Yes | QMessageBox::No), QMessageBox::No) != QMessageBox::Yes) {
      updateProfilesActions();
      return false;
    }
  }

  // Set the new profile number
  g.id(pid);
  Firmware::setCurrentVariant(newFw);
  emit firmwareChanged();
  updateMenus();
  return true;
}

void MainWindow::loadProfile()
{
  QAction * action = qobject_cast<QAction *>(sender());
  if (action) {
    bool ok = false;
    unsigned profnum = action->data().toUInt(&ok);
    if (ok)
      loadProfileId(profnum);
  }
}

void MainWindow::appPrefs()
{
  AppPreferencesDialog * dialog = new AppPreferencesDialog(this);
  dialog->setMainWinHasDirtyChild(anyChildrenDirty());
  connect(dialog, &AppPreferencesDialog::firmwareProfileAboutToChange, this, &MainWindow::saveAll);
  connect(dialog, &AppPreferencesDialog::firmwareProfileChanged, this, &MainWindow::loadProfileId);
  dialog->exec();
  dialog->deleteLater();
}

void MainWindow::fwPrefs()
{
  FirmwarePreferencesDialog * dialog = new FirmwarePreferencesDialog(this);
  dialog->exec();
  dialog->deleteLater();
}

void MainWindow::contributors()
{
  CreditsDialog * dialog = new CreditsDialog(this);
  dialog->exec();
  dialog->deleteLater();
}

// Create a widget with a line edit and folder select button and handles all interactions. Features autosuggest
//   path hints while typing, invalid paths shown in red. The label string is only for dialog title, not a QLabel.
// This should probably be moved some place more reusable, esp. the QFileSystemModel.
QWidget * folderSelectorWidget(QString * path, const QString & label, QWidget * parent)
{
  static QFileSystemModel fileModel;
  static bool init = false;
  if (!init) {
    init = true;
    fileModel.setFilter(QDir::Dirs);
    fileModel.setRootPath("/");
  }

  QWidget * fsw = new QWidget(parent);
  QLineEdit * le = new QLineEdit(parent);
  QCompleter * fsc = new QCompleter(fsw);
  fsc->setModel(&fileModel);
  //fsc->setCompletionMode(QCompleter::InlineCompletion);
  le->setCompleter(fsc);

  QToolButton * btn = new QToolButton(fsw);
  btn->setIcon(CompanionIcon("open.png"));
  QHBoxLayout * l = new QHBoxLayout(fsw);
  l->setContentsMargins(0,0,0,0);
  l->setSpacing(3);
  l->addWidget(le);
  l->addWidget(btn);

  QObject::connect(btn, &QToolButton::clicked, [=]() {
    QString dir = QFileDialog::getExistingDirectory(parent, label, le->text(), 0);
    if (!dir.isEmpty()) {
      le->setText(QDir::toNativeSeparators(dir));
      le->setFocus();
    }
  });

  QObject::connect(le, &QLineEdit::textChanged, [=](const QString & text) {
    *path = text;
    if (QFile::exists(text))
      le->setStyleSheet("");
    else
      le->setStyleSheet("QLineEdit {color: red;}");
  });
  le->setText(QDir::toNativeSeparators(*path));

  return fsw;
}

void MainWindow::sdsync()
{
  const QString dlgTtl = tr("Synchronize SD");
  const QIcon dlgIcn = CompanionIcon("sdsync.png");
  const QString srcArw = CPN_STR_SW_INDICATOR_UP % " ";
  const QString dstArw = CPN_STR_SW_INDICATOR_DN % " ";
  QStringList errorMsgs;

  // remember user-selectable options for duration of session
  static QString sourcePath;
  static QString destPath;
  static int syncDirection = SyncProcess::SYNC_A2B_B2A;
  static int compareType = SyncProcess::OVERWR_NEWER_IF_DIFF;
  static int maxFileSize = 2 * 1024 * 1024;  // Bytes
  static bool dryRun = false;

  if (sourcePath.isEmpty())
    sourcePath = g.profile[g.id()].sdPath();
  if (destPath.isEmpty())
    destPath = findMassstoragePath("SOUNDS").replace(QRegExp("[/\\\\]?SOUNDS"), "");

  if (sourcePath.isEmpty())
    errorMsgs << tr("No local SD structure path configured!");
  if (destPath.isEmpty())
    errorMsgs << tr("No Radio or SD card detected!");

  QDialog dlg(this);
  dlg.setWindowTitle(dlgTtl % tr(" :: Options"));
  dlg.setWindowIcon(dlgIcn);
  dlg.setSizeGripEnabled(true);
  dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);

  QLabel * lblSrc = new QLabel(tr("Local Folder:"), &dlg);
  QWidget * wdgSrc = folderSelectorWidget(&sourcePath, lblSrc->text(), &dlg);

  QLabel * lblDst = new QLabel(tr("Radio Folder:"), &dlg);
  QWidget * wdgDst = folderSelectorWidget(&destPath, lblDst->text(), &dlg);

  QLabel * lbDir = new QLabel(tr("Sync. Direction:"), &dlg);
  QComboBox * syncDir = new QComboBox(&dlg);
  syncDir->addItem(tr("%1%2 Both directions, to radio folder first").arg(dstArw, srcArw), SyncProcess::SYNC_A2B_B2A);
  syncDir->addItem(tr("%1%2 Both directions, to local folder first").arg(srcArw, dstArw), SyncProcess::SYNC_B2A_A2B);
  syncDir->addItem(tr(" %1  Only from local folder to radio folder").arg(dstArw), SyncProcess::SYNC_A2B);
  syncDir->addItem(tr(" %1  Only from radio folder to local folder").arg(srcArw), SyncProcess::SYNC_B2A);
  syncDir->setCurrentIndex(-1);  // we set the default option later

  QLabel * lbMode = new QLabel(tr("Existing Files:"), &dlg);
  QComboBox * copyMode = new QComboBox(&dlg);
  copyMode->setToolTip(tr("How to handle overwriting files which already exist in the destination folder."));
  copyMode->addItem(tr("Copy only if newer and different (compare contents)"), SyncProcess::OVERWR_NEWER_IF_DIFF);
  copyMode->addItem(tr("Copy only if newer (do not compare contents)"), SyncProcess::OVERWR_NEWER_ALWAYS);
  copyMode->addItem(tr("Copy only if different (ignore file time stamps)"), SyncProcess::OVERWR_IF_DIFF);
  copyMode->addItem(tr("Always copy (force overwite existing files)"), SyncProcess::OVERWR_ALWAYS);

  QLabel * lbSize = new QLabel(tr("Max. File Size:"), &dlg);
  QSpinBox * maxSize = new QSpinBox(&dlg);
  maxSize->setRange(0, 100 * 1024);
  maxSize->setAccelerated(true);
  maxSize->setSpecialValueText(tr("Any size"));
  maxSize->setToolTip(tr("Skip files larger than this size. Enter zero for unlimited."));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
  maxSize->setGroupSeparatorShown(true);
#endif

  QCheckBox * testRun = new QCheckBox(tr("Test-run only"), &dlg);
  testRun->setToolTip(tr("Run as normal but do not actually copy anything. Useful for verifying results before real sync."));
  connect(testRun, &QCheckBox::toggled, [=](bool on) { dryRun = on; });

  // layout to hold size spinbox and checkbox option(s)
  QHBoxLayout * hlay1 = new QHBoxLayout();
  hlay1->addWidget(maxSize, 1);
  hlay1->addWidget(testRun);

  // dialog OK/Cancel buttons
  QDialogButtonBox * bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);

  // Create main layout and add everything
  QGridLayout * dlgL = new QGridLayout(&dlg);
  dlgL->setSizeConstraint(QLayout::SetFixedSize);
  int row = 0;
  if (errorMsgs.size()) {
    QLabel * lblWarn = new QLabel(QString(errorMsgs.join('\n')), &dlg);
    lblWarn->setStyleSheet("QLabel { color: red; }");
    dlgL->addWidget(lblWarn, row++, 0, 1, 2);
  }
  dlgL->addWidget(lblSrc, row, 0);
  dlgL->addWidget(wdgSrc, row++, 1);
  dlgL->addWidget(lblDst, row, 0);
  dlgL->addWidget(wdgDst, row++, 1);
  dlgL->addWidget(lbDir, row, 0);
  dlgL->addWidget(syncDir, row++, 1);
  dlgL->addWidget(lbMode, row, 0);
  dlgL->addWidget(copyMode, row++, 1);
  dlgL->addWidget(lbSize, row, 0);
  dlgL->addLayout(hlay1, row++, 1);
  dlgL->addWidget(bb, row++, 0, 1, 2);
  dlgL->setRowStretch(row, 1);

  connect(copyMode, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int) {
    compareType = copyMode->currentData().toInt();
  });

  // function to dis/enable the OVERWR_ALWAYS option depending on sync direction
  connect(syncDir, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int) {
    int dir = syncDir->currentData().toInt();
    int idx = copyMode->findData(SyncProcess::OVERWR_ALWAYS);
    int flg = (dir == SyncProcess::SYNC_A2B || dir == SyncProcess::SYNC_B2A) ? 33 : 0;
    if (!flg && idx == copyMode->currentIndex())
      copyMode->setCurrentIndex(copyMode->findData(SyncProcess::OVERWR_NEWER_IF_DIFF));
    copyMode->setItemData(idx, flg, Qt::UserRole - 1);
    syncDirection = dir;
  });

  // function to set magnitude of file size spinbox, KB or MB
  connect(maxSize, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value) {
    int multi = maxSize->property("multi").isValid() ? maxSize->property("multi").toInt() : 0;
    maxSize->blockSignals(true);
    if (value >= 10 * 1024 && multi != 1024 * 1024) {
      // KB -> MB
      multi = 1024 * 1024;
      maxSize->setValue(value / 1024);
      maxSize->setMaximum(100);
      maxSize->setSingleStep(1);
      maxSize->setSuffix(tr(" MB"));
    }
    else if ((value < 10 && multi != 1024) || !multi) {
      // MB -> KB
      if (multi)
        value *= 1024;
      multi = 1024;
      if (value == 9 * 1024)
        value += 1024 - 32;  // avoid large jump when stepping from 10MB to 10,208KB
      maxSize->setMaximum(100 * 1024);
      maxSize->setValue(value);
      maxSize->setSingleStep(32);
      maxSize->setSuffix(tr(" KB"));
    }
    maxSize->setProperty("multi", multi);
    maxSize->blockSignals(false);
    maxFileSize = value * multi;
  });

  copyMode->setCurrentIndex(copyMode->findData(compareType));
  syncDir->setCurrentIndex(syncDir->findData(syncDirection));
  maxSize->setValue(maxFileSize / 1024);
  testRun->setChecked(dryRun);

  connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
  connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

  // to restart dialog on error/etc
  openDialog:

  // show the modal options dialog
  if (dlg.exec() == QDialog::Rejected)
    return;

  // validate
  errorMsgs.clear();
  if (sourcePath == destPath)
    errorMsgs << tr("Source and destination folders are the same!");
  if (sourcePath.isEmpty() || !QFile::exists(sourcePath))
    errorMsgs << tr("Source folder not found: %1").arg(sourcePath);
  if (destPath.isEmpty() || !QFile::exists(destPath))
    errorMsgs << tr("Destination folder not found: %1").arg(destPath);

  if (!errorMsgs.isEmpty()) {
    QMessageBox::warning(this, dlgTtl % tr(" :: Error"), errorMsgs.join('\n'));
    goto openDialog;
  }

  // set up the progress dialog and the sync process worker
  ProgressDialog * progressDlg = new ProgressDialog(this, dlgTtl % tr(" :: Progress"), dlgIcn);
  progressDlg->setAttribute(Qt::WA_DeleteOnClose, true);
  ProgressWidget * progWidget = progressDlg->progress();
  SyncProcess * syncProcess = new SyncProcess(sourcePath, destPath, syncDirection, compareType, maxFileSize, dryRun);

  // move sync process to separate thread, we only use signals/slots from here on...
  QThread * syncThread = new QThread(this);
  syncProcess->moveToThread(syncThread);

  // ...and quite a few of them!
  connect(this,        &MainWindow::startSync,         syncProcess, &SyncProcess::run);
  connect(syncThread,  &QThread::finished,             syncProcess, &SyncProcess::deleteLater);
  connect(syncProcess, &SyncProcess::finished,         syncThread,  &QThread::quit);
  connect(syncProcess, &SyncProcess::destroyed,        syncThread,  &QThread::quit);
  connect(syncProcess, &SyncProcess::destroyed,        syncThread,  &QThread::deleteLater);
  connect(syncProcess, &SyncProcess::fileCountChanged, progWidget,  &ProgressWidget::setMaximum);
  connect(syncProcess, &SyncProcess::progressStep,     progWidget,  &ProgressWidget::setValue);
  connect(syncProcess, &SyncProcess::progressMessage,  progWidget,  &ProgressWidget::addMessage);
  connect(syncProcess, &SyncProcess::statusMessage,    progWidget,  &ProgressWidget::setInfo);
  connect(syncProcess, &SyncProcess::started,          progressDlg, &ProgressDialog::setProcessStarted);
  connect(syncProcess, &SyncProcess::finished,         progressDlg, &ProgressDialog::setProcessStopped);
  connect(syncProcess, &SyncProcess::finished,         [=]()        { QApplication::alert(this); });
  connect(progressDlg, &ProgressDialog::rejected,      syncProcess, &SyncProcess::stop);
  connect(progressDlg, &ProgressDialog::rejected,      syncProcess, &SyncProcess::deleteLater);

  // go (finally)
  syncThread->start();
  emit startSync();
}

void MainWindow::changelog()
{
  Firmware * firmware = getCurrentFirmware();
  QString url = firmware->getReleaseNotesUrl();
  if (url.isEmpty()) {
    QMessageBox::information(this, tr("Release notes"), tr("Cannot retrieve release notes from the server."));
  }
  else {
    ReleaseNotesFirmwareDialog * dialog = new ReleaseNotesFirmwareDialog(this, url);
    dialog->exec();
    dialog->deleteLater();
  }
}

void MainWindow::customizeSplash()
{
  CustomizeSplashDialog * dialog = new CustomizeSplashDialog(this);
  dialog->exec();
  dialog->deleteLater();
}

void MainWindow::writeEeprom()
{
  if (activeMdiChild()) activeMdiChild()->writeEeprom();
}

void MainWindow::readEeprom()
{
  Board::Type board = getCurrentBoard();
  QString tempFile;
  if (IS_HORUS(board))
    tempFile = generateProcessUniqueTempFileName("temp.otx");
  else if (IS_ARM(board))
    tempFile = generateProcessUniqueTempFileName("temp.bin");
  else
    tempFile = generateProcessUniqueTempFileName("temp.hex");

  qDebug() << "MainWindow::readEeprom(): using temp file: " << tempFile;

  if (readEepromFromRadio(tempFile)) {
    MdiChild * child = createMdiChild();
    child->newFile(false);
    child->loadFile(tempFile, false);
    child->show();
    qunlink(tempFile);
  }
}

bool MainWindow::readFirmwareFromRadio(const QString & filename)
{
  ProgressDialog progressDialog(this, tr("Read Firmware from Radio"), CompanionIcon("read_flash.png"));
  bool result = readFirmware(filename, progressDialog.progress());
  if (!result && !progressDialog.isEmpty()) {
    progressDialog.exec();
  }
  return result;
}

bool MainWindow::readEepromFromRadio(const QString & filename)
{
  ProgressDialog progressDialog(this, tr("Read Models and Settings from Radio"), CompanionIcon("read_eeprom.png"));
  bool result = ::readEeprom(filename, progressDialog.progress());
  if (!result) {
    if (!progressDialog.isEmpty()) {
      progressDialog.exec();
    }
  }
  else {
    statusBar()->showMessage(tr("Models and Settings read"), 2000);
  }
  return result;
}

void MainWindow::writeBackup()
{
  if (IS_HORUS(getCurrentBoard())) {
    QMessageBox::information(this, CPN_STR_APP_NAME, tr("This function is not yet implemented"));
    return;
    // TODO implementation
  }
  FlashEEpromDialog *cd = new FlashEEpromDialog(this);
  cd->exec();
}

void MainWindow::writeFlash(QString fileToFlash)
{
  FlashFirmwareDialog * cd = new FlashFirmwareDialog(this);
  cd->exec();
}

void MainWindow::readBackup()
{
  if (IS_HORUS(getCurrentBoard())) {
    QMessageBox::information(this, CPN_STR_APP_NAME, tr("This function is not yet implemented"));
    return;
    // TODO implementation
  }
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Radio Backup to File"), g.eepromDir(), EXTERNAL_EEPROM_FILES_FILTER);
  if (!fileName.isEmpty()) {
    if (!readEepromFromRadio(fileName))
      return;
  }
}

void MainWindow::readFlash()
{
  QString fileName = QFileDialog::getSaveFileName(this,tr("Read Radio Firmware to File"), g.flashDir(), FLASH_FILES_FILTER);
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
  burnConfigDialog bcd(this);
  bcd.listAvrdudeProgrammers();
}

void MainWindow::burnFuses()
{
  FusesDialog *fd = new FusesDialog(this);
  fd->exec();
  delete fd;
}

void MainWindow::compare()
{
  CompareDialog *fd = new CompareDialog(this,getCurrentFirmware());
  fd->setAttribute(Qt::WA_DeleteOnClose, true);
  fd->show();
}

void MainWindow::logFile()
{
  LogsDialog *fd = new LogsDialog(this);
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
  aboutStr.append(QString("Version %1, %2").arg(VERSION).arg(__DATE__));
  aboutStr.append("<br/><br/>");
  aboutStr.append(tr("Copyright OpenTX Team") + "<br/>&copy; 2011-2017<br/>");
  QMessageBox msgBox(this);
  msgBox.setWindowIcon(CompanionIcon("information.png"));
  msgBox.setWindowTitle(tr("About Companion"));
  msgBox.setText(aboutStr);
  msgBox.exec();
}

void MainWindow::updateMenus()
{
  QMdiSubWindow * activeChild = mdiArea->activeSubWindow();

  newAct->setEnabled(true);
  openAct->setEnabled(true);
  saveAct->setEnabled(activeChild);
  saveAsAct->setEnabled(activeChild);
  closeAct->setEnabled(activeChild);
  compareAct->setEnabled(activeChild);
  writeEepromAct->setEnabled(activeChild);
  readEepromAct->setEnabled(true);
  writeBUToRadioAct->setEnabled(true);
  readBUToFileAct->setEnabled(true);
  editSplashAct->setDisabled(IS_HORUS(getCurrentBoard()));

  foreach (QAction * act, fileWindowActions) {
    if (!act)
      continue;
    if (fileMenu && fileMenu->actions().contains(act))
      fileMenu->removeAction(act);
    if (fileToolBar && fileToolBar->actions().contains(act)) {
      fileToolBar->removeAction(act);
    }
    if (act->isSeparator() && act->parent() == this)
      delete act;
  }
  fileWindowActions.clear();
  if (activeChild) {
    editMenu->clear();
    editMenu->addActions(activeMdiChild()->getEditActions());
    editMenu->addSeparator();
    editMenu->addActions(activeMdiChild()->getModelActions());  // maybe separate menu/toolbar?
    editMenu->setEnabled(true);

    editToolBar->clear();
    editToolBar->addActions(activeMdiChild()->getEditActions());
    editToolBar->setEnabled(true);
    if (activeMdiChild()->getAction(MdiChild::ACT_MDL_MOV)) {
      // workaround for default split button appearance of action with menu  :-/
      QToolButton * btn;
      if ((btn = qobject_cast<QToolButton *>(editToolBar->widgetForAction(activeMdiChild()->getAction(MdiChild::ACT_MDL_MOV)))))
        btn->setPopupMode(QToolButton::InstantPopup);
    }

    fileWindowActions = activeMdiChild()->getGeneralActions();
    QAction *sep = new QAction(this);
    sep->setSeparator(true);
    fileWindowActions.append(sep);
    fileMenu->insertActions(logsAct, fileWindowActions);
    fileToolBar->insertActions(logsAct, fileWindowActions);
  }
  else {
    editToolBar->setDisabled(true);
    editMenu->setDisabled(true);
  }

  foreach (QAction * act, windowsListActions->actions()) {
    if (act->property("window_ptr").canConvert<QMdiSubWindow *>() &&
        act->property("window_ptr").value<QMdiSubWindow *>() == activeChild) {
      act->setChecked(true);
      break;
    }
  }

  updateRecentFileActions();
  updateProfilesActions();
  setWindowTitle(tr("OpenTX Companion %1 - Radio: %2 - Profile: %3").arg(VERSION).arg(getCurrentFirmware()->getName()).arg(g.profile[g.id()].name()));
}

MdiChild * MainWindow::createMdiChild()
{
  QMdiSubWindow * win = new QMdiSubWindow();
  MdiChild * child = new MdiChild(this, win);
  win->setAttribute(Qt::WA_DeleteOnClose);
  win->setWidget(child);
  mdiArea->addSubWindow(win);
  if (g.mdiWinGeo().size() < 10 && g.mdiWinGeo() == "maximized")
    win->showMaximized();

  connect(this, &MainWindow::firmwareChanged, child, &MdiChild::onFirmwareChanged);
  connect(child, &MdiChild::windowTitleChanged, this, &MainWindow::onSubwindowTitleChanged);
  connect(child, &MdiChild::modified, this, &MainWindow::onSubwindowModified);
  connect(child, &MdiChild::newStatusMessage, statusBar(), &QStatusBar::showMessage);
  connect(child, &MdiChild::destroyed, win, &QMdiSubWindow::close);
  connect(win, &QMdiSubWindow::destroyed, this, &MainWindow::updateWindowActions);

  updateWindowActions();
  return child;
}

QAction * MainWindow::addAct(const QString & icon, const char *slot, const QKeySequence & shortcut, QObject *slotObj, const char * signal)
{
  QAction * newAction = new QAction( this );
  newAction->setMenuRole(QAction::NoRole);
  if (!icon.isEmpty())
    newAction->setIcon(CompanionIcon(icon));
  if (!shortcut.isEmpty())
    newAction->setShortcut(shortcut);
  if (slot) {
    if (!slotObj)
      slotObj = this;
    if (!signal)
      connect(newAction, SIGNAL(triggered()), slotObj, slot);
    else
      connect(newAction, signal, slotObj, slot);
  }
  return newAction;
}

QAction * MainWindow::addActToGroup(QActionGroup * aGroup, const QString & sName, const QString & lName, const char * propName, const QVariant & propValue, const QVariant & dfltValue, const QKeySequence & shortcut)
{
  QAction * act = aGroup->addAction(sName);
  act->setMenuRole(QAction::NoRole);
  act->setStatusTip(lName);
  act->setCheckable(true);
  if (!shortcut.isEmpty())
    act->setShortcut(shortcut);
  if (propName) {
    act->setProperty(propName, propValue);
    if (propValue == dfltValue)
      act->setChecked(true);
  }
  return act;
}

void MainWindow::trAct(QAction * act, const QString & text, const QString & descript)
{
  if (!text.isEmpty())
    act->setText(text);
  if (!descript.isEmpty())
    act->setStatusTip(descript);
}

void MainWindow::retranslateUi(bool showMsg)
{
  trAct(newAct,    tr("New"),        tr("Create a new Models and Settings file"));
  trAct(openAct,   tr("Open..."),    tr("Open Models and Settings file"));
  trAct(saveAct,   tr("Save"),       tr("Save Models and Settings file"));
  trAct(saveAsAct, tr("Save As..."), tr("Save Models and Settings file"));
  trAct(closeAct,  tr("Close"),      tr("Close Models and Settings file"));
  trAct(exitAct,   tr("Exit"),       tr("Exit the application"));
  trAct(aboutAct,  tr("About..."),   tr("Show the application's About box"));

  trAct(recentFilesAct,     tr("Recent Files"),               tr("List of recently used files"));
  trAct(profilesMenuAct,    tr("Radio Profiles"),             tr("Create or Select Radio Profiles"));
  trAct(logsAct,            tr("View Log File..."),           tr("Open and view log file"));
  trAct(appPrefsAct,        tr("Settings..."),                tr("Edit Settings"));
  trAct(fwPrefsAct,         tr("Download..."),                tr("Download firmware and voice files"));
  trAct(checkForUpdatesAct, tr("Check for Updates..."),       tr("Check OpenTX and Companion updates"));
  trAct(changelogAct,       tr("Release notes..."),           tr("Show release notes"));
  trAct(compareAct,         tr("Compare Models..."),          tr("Compare models"));
  trAct(editSplashAct,      tr("Edit Radio Splash Image..."), tr("Edit the splash image of your Radio"));
  trAct(burnListAct,        tr("List programmers..."),        tr("List available programmers"));
  trAct(burnFusesAct,       tr("Fuses..."),                   tr("Show fuses dialog"));
  trAct(readFlashAct,       tr("Read Firmware from Radio"),   tr("Read firmware from Radio"));
  trAct(writeFlashAct,      tr("Write Firmware to Radio"),    tr("Write firmware to Radio"));
  trAct(sdsyncAct,          tr("Synchronize SD"),             tr("SD card synchronization"));

  trAct(openDocURLAct,      tr("Manuals and other Documents"),         tr("Open the OpenTX document page in a web browser"));
  trAct(writeEepromAct,     tr("Write Models and Settings To Radio"),  tr("Write Models and Settings to Radio"));
  trAct(readEepromAct,      tr("Read Models and Settings From Radio"), tr("Read Models and Settings from Radio"));
  trAct(burnConfigAct,      tr("Configure Communications..."),         tr("Configure software for communicating with the Radio"));
  trAct(writeBUToRadioAct,  tr("Write Backup to Radio"),               tr("Write Backup from file to Radio"));
  trAct(readBUToFileAct,    tr("Backup Radio to File"),                tr("Save a complete backup file of all settings and model data in the Radio"));
  trAct(contributorsAct,    tr("Contributors..."),                     tr("A tribute to those who have contributed to OpenTX and Companion"));

  trAct(createProfileAct,   tr("Add Radio Profile"),               tr("Create a new Radio Settings Profile"));
  trAct(copyProfileAct,     tr("Copy Current Radio Profile"),      tr("Duplicate current Radio Settings Profile"));
  trAct(deleteProfileAct,   tr("Delete Current Radio Profile..."), tr("Delete the current Radio Settings Profile"));

  trAct(actTabbedWindows,   tr("Tabbed Windows"),    tr("Use tabs to arrange open windows."));
  trAct(actTileWindows,     tr("Tile Windows"),      tr("Arrange open windows across all the available space."));
  trAct(actCascadeWindows,  tr("Cascade Windows"),   tr("Arrange all open windows in a stack."));
  trAct(actCloseAllWindows, tr("Close All Windows"), tr("Closes all open files (prompts to save if necessary."));

  editMenu->setTitle(tr("Edit"));
  fileMenu->setTitle(tr("File"));
  settingsMenu->setTitle(tr("Settings"));
  themeMenu->setTitle(tr("Set Icon Theme"));
  iconThemeSizeMenu->setTitle(tr("Set Icon Size"));
  burnMenu->setTitle(tr("Read/Write"));
  windowMenu->setTitle(tr("Window"));
  helpMenu->setTitle(tr("Help"));

  fileToolBar->setWindowTitle(tr("File"));
  editToolBar->setWindowTitle(tr("Edit"));
  burnToolBar->setWindowTitle(tr("Write"));
  helpToolBar->setWindowTitle(tr("Help"));

  showReadyStatus();

  if (showMsg)
    QMessageBox::information(this, CPN_STR_APP_NAME, tr("Some text will not be translated until the next time you start Companion. Please note that some translations may not be complete."));
}

void MainWindow::createActions()
{
  newAct =             addAct("new.png",    SLOT(newFile()),                  QKeySequence::New);
  openAct =            addAct("open.png",   SLOT(openFile()),                 QKeySequence::Open);
  saveAct =            addAct("save.png",   SLOT(save()),                     QKeySequence::Save);
  saveAsAct =          addAct("saveas.png", SLOT(saveAs()),                   tr("Ctrl+Shift+S"));       // Windows doesn't have "native" save-as key, Lin/OSX both use this one anyway
  closeAct =           addAct("clear.png",  SLOT(closeFile())             /*, QKeySequence::Close*/);    // setting/showing this shortcut interferes with the system one (Ctrl+W/Ctrl-F4)
  exitAct =            addAct("exit.png",   SLOT(closeAllWindows()),          QKeySequence::Quit, qApp);

  logsAct =            addAct("logs.png",           SLOT(logFile()),          tr("Ctrl+Alt+L"));
  appPrefsAct =        addAct("apppreferences.png", SLOT(appPrefs()),         QKeySequence::Preferences);
  fwPrefsAct =         addAct("fwpreferences.png",  SLOT(fwPrefs()),          tr("Ctrl+Alt+D"));
  compareAct =         addAct("compare.png",        SLOT(compare()),          tr("Ctrl+Alt+R"));
  sdsyncAct =          addAct("sdsync.png",         SLOT(sdsync()));

  editSplashAct =      addAct("paintbrush.png",        SLOT(customizeSplash()));
  burnListAct =        addAct("list.png",              SLOT(burnList()));
  burnFusesAct =       addAct("fuses.png",             SLOT(burnFuses()));
  readFlashAct =       addAct("read_flash.png",        SLOT(readFlash()));
  writeFlashAct =      addAct("write_flash.png",       SLOT(writeFlash()));
  writeEepromAct =     addAct("write_eeprom.png",      SLOT(writeEeprom()));
  readEepromAct =      addAct("read_eeprom.png",       SLOT(readEeprom()));
  burnConfigAct =      addAct("configure.png",         SLOT(burnConfig()));
  writeBUToRadioAct =  addAct("write_eeprom_file.png", SLOT(writeBackup()));
  readBUToFileAct =    addAct("read_eeprom_file.png",  SLOT(readBackup()));

  createProfileAct =   addAct("new.png",   SLOT(createProfile()));
  copyProfileAct   =   addAct("copy.png",  SLOT(copyProfile()));
  deleteProfileAct =   addAct("clear.png", SLOT(deleteCurrentProfile()));

  actTabbedWindows =   addAct("", SLOT(setTabbedWindows(bool)), 0, this, SIGNAL(triggered(bool)));
  actTileWindows =     addAct("", SLOT(tileSubWindows()),       0, mdiArea);
  actCascadeWindows =  addAct("", SLOT(cascadeSubWindows()),    0, mdiArea);
  actCloseAllWindows = addAct("", SLOT(closeAllSubWindows()),   0, mdiArea);

  checkForUpdatesAct = addAct("update.png",         SLOT(doUpdates()));
  aboutAct =           addAct("information.png",    SLOT(about()));
  openDocURLAct =      addAct("changelog.png",      SLOT(openDocURL()));
  changelogAct =       addAct("changelog.png",      SLOT(changelog()));
  contributorsAct =    addAct("contributors.png",   SLOT(contributors()));

  // these two get assigned menus in createMenus()
  recentFilesAct =     addAct("recentdocument.png");
  profilesMenuAct =    addAct("profiles.png");

  exitAct->setMenuRole(QAction::QuitRole);
  aboutAct->setMenuRole(QAction::AboutRole);
  appPrefsAct->setMenuRole(QAction::PreferencesRole);
  contributorsAct->setMenuRole(QAction::ApplicationSpecificRole);
  openDocURLAct->setMenuRole(QAction::ApplicationSpecificRole);
  checkForUpdatesAct->setMenuRole(QAction::ApplicationSpecificRole);
  changelogAct->setMenuRole(QAction::ApplicationSpecificRole);

  actTabbedWindows->setCheckable(true);
  compareAct->setEnabled(false);
}

void MainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu("");
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addAction(closeAct);
  fileMenu->addAction(recentFilesAct);
  fileMenu->addSeparator();
  fileMenu->addAction(logsAct);
  fileMenu->addAction(fwPrefsAct);
  fileMenu->addAction(compareAct);
  fileMenu->addAction(sdsyncAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  editMenu = menuBar()->addMenu("");

  settingsMenu = menuBar()->addMenu("");
  settingsMenu->addMenu(createLanguageMenu(settingsMenu));

  themeMenu = settingsMenu->addMenu("");
  QActionGroup * themeGroup = new QActionGroup(themeMenu);
  addActToGroup(themeGroup, tr("Classical"),  tr("The classic companion9x icon theme"),  "themeId", 0, g.theme());
  addActToGroup(themeGroup, tr("Yerico"),     tr("Yellow round honey sweet icon theme"), "themeId", 1, g.theme());
  addActToGroup(themeGroup, tr("Monochrome"), tr("A monochrome black icon theme"),       "themeId", 3, g.theme());
  addActToGroup(themeGroup, tr("MonoBlue"),   tr("A monochrome blue icon theme"),        "themeId", 4, g.theme());
  addActToGroup(themeGroup, tr("MonoWhite"),  tr("A monochrome white icon theme"),       "themeId", 2, g.theme());
  connect(themeGroup, &QActionGroup::triggered, this, &MainWindow::onThemeChanged);
  themeMenu->addActions(themeGroup->actions());

  iconThemeSizeMenu = settingsMenu->addMenu("");
  QActionGroup * szGroup = new QActionGroup(iconThemeSizeMenu);
  addActToGroup(szGroup, tr("Small"),  tr("Use small toolbar icons"),       "sizeId", 0, g.iconSize());
  addActToGroup(szGroup, tr("Normal"), tr("Use normal size toolbar icons"), "sizeId", 1, g.iconSize());
  addActToGroup(szGroup, tr("Big"),    tr("Use big toolbar icons"),         "sizeId", 2, g.iconSize());
  addActToGroup(szGroup, tr("Huge"),   tr("Use huge toolbar icons"),        "sizeId", 3, g.iconSize());
  connect(szGroup, &QActionGroup::triggered, this, &MainWindow::onIconSizeChanged);
  iconThemeSizeMenu->addActions(szGroup->actions());

  settingsMenu->addSeparator();
  settingsMenu->addAction(appPrefsAct);
  settingsMenu->addAction(profilesMenuAct);
  settingsMenu->addAction(editSplashAct);
  settingsMenu->addAction(burnConfigAct);

  burnMenu = menuBar()->addMenu("");
  burnMenu->addAction(writeEepromAct);
  burnMenu->addAction(readEepromAct);
  burnMenu->addSeparator();
  burnMenu->addAction(writeBUToRadioAct);
  burnMenu->addAction(readBUToFileAct);
  burnMenu->addSeparator();
  burnMenu->addAction(writeFlashAct);
  burnMenu->addAction(readFlashAct);
  burnMenu->addSeparator();
  burnMenu->addSeparator();
  if (!IS_ARM(getCurrentBoard())) {
    burnMenu->addAction(burnFusesAct);
    burnMenu->addAction(burnListAct);
  }

  windowMenu = menuBar()->addMenu("");
  windowMenu->addAction(actTabbedWindows);
  windowMenu->addAction(actTileWindows);
  windowMenu->addAction(actCascadeWindows);
  windowMenu->addAction(actCloseAllWindows);
  windowMenu->addSeparator();

  helpMenu = menuBar()->addMenu("");
  helpMenu->addSeparator();
  helpMenu->addAction(checkForUpdatesAct);
  helpMenu->addSeparator();
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(openDocURLAct);
  helpMenu->addSeparator();
  helpMenu->addAction(changelogAct);
  helpMenu->addSeparator();
  helpMenu->addAction(contributorsAct);

  recentFilesMenu = new QMenu(this);
  recentFilesMenu->setToolTipsVisible(true);
  for ( int i = 0; i < g.historySize(); ++i) {
    recentFileActs.append(recentFilesMenu->addAction(""));
    recentFileActs[i]->setVisible(false);
    connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
  }
  recentFilesAct->setMenu(recentFilesMenu);

  profilesMenu = new QMenu(this);
  QActionGroup *profilesGroup = new QActionGroup(this);
  for (int i=0; i < MAX_PROFILES; i++) {
    profileActs.append(profilesMenu->addAction(""));
    profileActs[i]->setVisible(false);
    profileActs[i]->setCheckable(true);
    connect(profileActs[i], SIGNAL(triggered()), this, SLOT(loadProfile()));
    profilesGroup->addAction(profileActs[i]);
  }
  profilesMenu->addSeparator();
  profilesMenu->addAction(createProfileAct);
  profilesMenu->addAction(copyProfileAct);
  profilesMenu->addAction(deleteProfileAct);
  profilesMenuAct->setMenu(profilesMenu);
}

void MainWindow::createToolBars()
{
  fileToolBar = addToolBar("");
  fileToolBar->setObjectName("File");
  fileToolBar->addAction(newAct);
  fileToolBar->addAction(openAct);
  fileToolBar->addAction(recentFilesAct);
  fileToolBar->addAction(saveAct);
  fileToolBar->addAction(closeAct);
  fileToolBar->addSeparator();
  fileToolBar->addAction(logsAct);
  fileToolBar->addAction(fwPrefsAct);
  fileToolBar->addSeparator();
  fileToolBar->addAction(appPrefsAct);
  fileToolBar->addAction(profilesMenuAct);
  fileToolBar->addAction(editSplashAct);
  fileToolBar->addAction(editSplashAct);
  fileToolBar->addSeparator();
  fileToolBar->addAction(compareAct);
  fileToolBar->addAction(sdsyncAct);

  // workaround for default split button appearance of action with menu  :-/
  QToolButton * btn;
  if ((btn = qobject_cast<QToolButton *>(fileToolBar->widgetForAction(recentFilesAct))))
    btn->setPopupMode(QToolButton::InstantPopup);
  if ((btn = qobject_cast<QToolButton *>(fileToolBar->widgetForAction(profilesMenuAct))))
    btn->setPopupMode(QToolButton::InstantPopup);

  // gets populated later
  editToolBar = addToolBar("");
  editToolBar->setObjectName("Edit");

  burnToolBar = new QToolBar(this);
  addToolBar( Qt::LeftToolBarArea, burnToolBar );
  burnToolBar->setObjectName("Write");
  burnToolBar->addAction(writeEepromAct);
  burnToolBar->addAction(readEepromAct);
  burnToolBar->addSeparator();
  burnToolBar->addAction(writeBUToRadioAct);
  burnToolBar->addAction(readBUToFileAct);
  burnToolBar->addSeparator();
  burnToolBar->addAction(writeFlashAct);
  burnToolBar->addAction(readFlashAct);
  burnToolBar->addSeparator();
  burnToolBar->addAction(burnConfigAct);

  helpToolBar = addToolBar("");
  helpToolBar->setObjectName("Help");
  helpToolBar->addAction(checkForUpdatesAct);
  helpToolBar->addAction(aboutAct);
}

QMenu * MainWindow::createLanguageMenu(QWidget * parent)
{
  QMenu * menu = new QMenu(tr("Set Menu Language"), parent);
  QActionGroup * actGroup = new QActionGroup(menu);
  QString lName;

  addActToGroup(actGroup, tr("System language"), tr("Use default system language."), "locale", QString(""), g.locale());
  foreach (const QString & lang, Translations::getAvailableTranslations()) {
    QLocale locale(lang);
    lName = locale.nativeLanguageName();
    addActToGroup(actGroup, lName.left(1).toUpper() % lName.mid(1), tr("Use %1 language (some translations may not be complete).").arg(lName), "locale", lang, g.locale());
  }
  if (!actGroup->checkedAction())
    actGroup->actions().first()->setChecked(true);

  connect(actGroup, &QActionGroup::triggered, this, &MainWindow::onLanguageChanged);
  menu->addActions(actGroup->actions());
  return menu;
}

void MainWindow::showReadyStatus()
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

bool MainWindow::anyChildrenDirty()
{
  foreach (QMdiSubWindow * window, mdiArea->subWindowList()) {
    MdiChild * child;
    if ((child = qobject_cast<MdiChild *>(window->widget())) && child->isWindowModified())
      return true;
  }
  return false;
}

void MainWindow::updateRecentFileActions()
{
  QStringList files = g.recentFiles();
  for (int i=0; i < recentFileActs.size(); i++) {
    if (i < files.size() && !files.at(i).isEmpty()) {
      recentFileActs[i]->setText(QFileInfo(files.at(i)).fileName());
      recentFileActs[i]->setData(files.at(i));
      recentFileActs[i]->setStatusTip(QDir::toNativeSeparators(files.at(i)));
      recentFileActs[i]->setToolTip(recentFileActs[i]->statusTip());
      recentFileActs[i]->setVisible(true);
    }
    else {
      recentFileActs[i]->setVisible(false);
    }
  }
}

void MainWindow::updateProfilesActions()
{
  for (int i=0; i < qMin(profileActs.size(), MAX_PROFILES); i++) {
    if (g.profile[i].existsOnDisk()) {
      QString text = tr("%2").arg(g.profile[i].name());
      profileActs[i]->setText(text);
      profileActs[i]->setData(i);
      profileActs[i]->setVisible(true);
      if (i == g.id())
        profileActs[i]->setChecked(true);
    }
    else {
      profileActs[i]->setVisible(false);
    }
  }
}

void MainWindow::updateWindowActions()
{
  if (!windowsListActions)
    return;

  foreach (QAction * act, windowsListActions->actions()) {
    windowsListActions->removeAction(act);
    if (windowMenu->actions().contains(act))
      windowMenu->removeAction(act);
    delete act;
  }
  int count = 0;
  foreach (QMdiSubWindow * win, mdiArea->subWindowList()) {
    QString scut;
    if (++count < 10)
      scut = tr("Alt+%1").arg(count);
    QAction * act = addActToGroup(windowsListActions, "", "", "window_ptr", qVariantFromValue(win), QVariant(), scut);
    act->setChecked(win == mdiArea->activeSubWindow());
    updateWindowActionTitle(win, act);
  }
  windowMenu->addActions(windowsListActions->actions());
}

void MainWindow::updateWindowActionTitle(const QMdiSubWindow * win, QAction * act)
{
  MdiChild * child = qobject_cast<MdiChild *>(win->widget());
  if (!child)
    return;

  if (!act) {
    foreach (QAction * a, windowsListActions->actions()) {
      if (a->property("window_ptr").canConvert<QMdiSubWindow *>() &&
          a->property("window_ptr").value<QMdiSubWindow *>() == win) {
        act = a;
        break;
      }
    }
  }
  if (!act)
    return;

  QString ttl = child->userFriendlyCurrentFile();
  if (child->isWindowModified())
    ttl.prepend("* ");
  act->setText(ttl);
}

void MainWindow::onSubwindowTitleChanged()
{
  QMdiSubWindow * win = NULL;
  if ((win = qobject_cast<QMdiSubWindow *>(sender()->parent())))
    updateWindowActionTitle(win);
}

void MainWindow::onSubwindowModified()
{
  onSubwindowTitleChanged();
}

void MainWindow::onChangeWindowAction(QAction * act)
{
  if (!act->isChecked())
    return;

  QMdiSubWindow * win = NULL;
  if (act->property("window_ptr").canConvert<QMdiSubWindow *>())
    win = act->property("window_ptr").value<QMdiSubWindow *>();
  if (win)
    mdiArea->setActiveSubWindow(win);
}

int MainWindow::newProfile(bool loadProfile)
{
  int i;
  for (i=0; i < MAX_PROFILES && g.profile[i].existsOnDisk(); i++)
    ;
  if (i == MAX_PROFILES)  //Failed to find free slot
    return -1;

  g.profile[i].init(i);
  g.profile[i].name(tr("New Radio"));

  if (loadProfile) {
    if (loadProfileId(i))
      appPrefs();
  }

  return i;
}

void MainWindow::createProfile()
{
  newProfile(true);
}

void MainWindow::copyProfile()
{
  int newId = newProfile(false);

  if (newId > -1) {
    g.profile[newId] = g.profile[g.id()];
    g.profile[newId].name(g.profile[newId].name() + tr(" - Copy"));
    if (loadProfileId(newId))
      appPrefs();
  }
}

void MainWindow::deleteProfile(const int pid)
{
  if (pid == g.id() && anyChildrenDirty()) {
    QMessageBox::warning(this, tr("Companion :: Open files warning"), tr("Please save or close modified file(s) before deleting the active profile."));
    return;
  }
  if (pid == 0) {
    QMessageBox::warning(this, tr("Not possible to remove profile"), tr("The default profile can not be removed."));
    return;
  }
  int ret = QMessageBox::question(this,
                                  tr("Confirm Delete Profile"),
                                  tr("Are you sure you wish to delete the \"%1\" radio profile? There is no way to undo this action!").arg(g.profile[pid].name()));
  if (ret != QMessageBox::Yes)
    return;

  g.profile[pid].remove();
  loadProfileId(0);
}

void MainWindow::deleteCurrentProfile()
{
  deleteProfile(g.id());
}

QString MainWindow::strippedName(const QString &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasFormat("text/uri-list"))
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
  QList<QUrl> urls = event->mimeData()->urls();
  if (urls.isEmpty()) return;
  QString fileName = urls.first().toLocalFile();
  openFile(fileName);
}

void MainWindow::autoClose()
{
  this->close();
}
