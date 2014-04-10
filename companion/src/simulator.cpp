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

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QSplashScreen>
#include <QThread>
#include <iostream>
#include "simulatordialog.h"
#include "eeprominterface.h"

#if defined WIN32 || !defined __GNUC__
#include <windows.h>
#define sleep(x) Sleep(x*1000)
#else
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <QProxyStyle> 

class MyProxyStyle : public QProxyStyle
 {
   public:
    void polish ( QWidget * w ) {
      QMenu* mn = dynamic_cast<QMenu*>(w);
      QPushButton* pb = dynamic_cast<QPushButton*>(w);
      if(!(mn || pb) && !w->testAttribute(Qt::WA_MacNormalSize))
          w->setAttribute(Qt::WA_MacSmallSize);
    }
 };
#endif

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(companion);
  QApplication app(argc, argv);
  app.setApplicationName("OpenTX Simulator");
  app.setOrganizationName("OpenTX");
  app.setOrganizationDomain("open-tx.org");

#ifdef __APPLE__
  app.setStyle(new MyProxyStyle);
#endif

  QString dir;
  if (argc) dir = QFileInfo(argv[0]).canonicalPath() + "/lang";

  /* QTranslator companionTranslator;
  companionTranslator.load(":/companion_" + locale);
  QTranslator qtTranslator;
  qtTranslator.load((QString)"qt_" + locale.left(2), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&companionTranslator);
  app.installTranslator(&qtTranslator);
*/

  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

  RegisterFirmwares();
  SimulatorDialog *dialog;
  const char * eepromFileName;
  QString fileName;
  QByteArray path;
  QDir eedir;
  QFile file;

  QMessageBox msgBox;
  msgBox.setWindowTitle("Radio type");
  msgBox.setText("Which radio type do you want to simulate?");
  msgBox.setIcon(QMessageBox::Question);
  QAbstractButton *taranisButton = msgBox.addButton("Taranis", QMessageBox::ActionRole);
  msgBox.addButton("9X", QMessageBox::ActionRole);
  QPushButton *exitButton = msgBox.addButton(QMessageBox::Close);

  eedir = QDir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
  if (!eedir.exists("OpenTX")) {
    eedir.mkdir("OpenTX");
  }
  eedir.cd("OpenTX");

  msgBox.exec();
  
  if (msgBox.clickedButton() == exitButton)
    return 0;
  else if (msgBox.clickedButton() == taranisButton) {
    current_firmware_variant = GetFirmwareVariant("opentx-taranis-en");
    fileName = eedir.filePath("eeprom-taranis.bin");
    path = fileName.toAscii();
    eepromFileName = path.data();
    dialog = new SimulatorDialogTaranis();
  }
  else {
    fileName = eedir.filePath("eeprom-9x.bin");
    path = fileName.toAscii();
    eepromFileName = path.data();
    dialog = new SimulatorDialog9X();
  }

  dialog->show();
  dialog->start(eepromFileName);

  int result = app.exec();

  delete dialog;

  return result;
}
