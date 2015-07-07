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
#include <QSplashScreen>
#include <QThread>
#include <iostream>
#if defined(JOYSTICKS) || defined(SIMU_AUDIO)
  #include <SDL.h>
  #undef main
#endif
#include "mainwindow.h"
#include "eeprominterface.h"
#include "appdata.h"

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
  app.setApplicationName("OpenTX Companion");
  app.setOrganizationName("OpenTX");
  app.setOrganizationDomain("open-tx.org");
  app.setAttribute(Qt::AA_DontShowIconsInMenus, false);

#ifdef __APPLE__
  app.setStyle(new MyProxyStyle);
#endif

  QTranslator companionTranslator;
  companionTranslator.load(":/companion_" + g.locale());
  QTranslator qtTranslator;
  qtTranslator.load((QString)"qt_" + g.locale().left(2), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&companionTranslator);
  app.installTranslator(&qtTranslator);

  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

#if defined(JOYSTICKS) || defined(SIMU_AUDIO)
  uint32_t sdlFlags = 0;
  #ifdef JOYSTICKS
    sdlFlags |= SDL_INIT_JOYSTICK;
  #endif
  #ifdef SIMU_AUDIO
    sdlFlags |= SDL_INIT_AUDIO;
  #endif
  if (SDL_Init(sdlFlags) < 0) {
    fprintf(stderr, "ERROR: couldn't initialize SDL: %s\n", SDL_GetError());
  }
#endif

  registerEEpromInterfaces();
  registerOpenTxFirmwares();
  registerSimulators();

  if (g.profile[g.id()].fwType().isEmpty()){
    g.profile[g.id()].fwType(default_firmware_variant->getId());
    g.profile[g.id()].fwName("");
  }

  QString splashScreen;
  if ( g.profile[g.id()].fwType().contains("taranis"))     splashScreen = ":/images/splash-taranis.png";
  else if ( g.profile[g.id()].fwType().contains("9xrpro")) splashScreen = ":/images/splash-9xrpro.png";
  else if ( g.profile[g.id()].fwType().contains("9xr"))    splashScreen = ":/images/splash-9xr.png";
  else  splashScreen = ":/images/splash-9x.png";

  QPixmap pixmap = QPixmap(splashScreen);
  QSplashScreen *splash = new QSplashScreen(pixmap);

  current_firmware_variant = GetFirmware(g.profile[g.id()].fwType());

  MainWindow *mainWin = new MainWindow();
  if (g.showSplash()) {
    splash->show();
    QTimer::singleShot(1000*SPLASH_TIME, splash, SLOT(close()));
    QTimer::singleShot(1000*SPLASH_TIME, mainWin, SLOT(show()));
  }
  else {
    mainWin->show();
  }

  int result = app.exec();

  delete splash;
  delete mainWin;

  unregisterFirmwares();
  unregisterEEpromInterfaces();

#if defined(JOYSTICKS) || defined(SIMU_AUDIO)
  SDL_Quit();
#endif

  return result;
}
