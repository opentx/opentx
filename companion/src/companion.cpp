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

#include <QApplication>
#include <QSplashScreen>
#if defined(JOYSTICKS) || defined(SIMU_AUDIO)
  #include <SDL.h>
  #undef main
#endif

#include "appdebugmessagehandler.h"
#include "customdebug.h"
#include "mainwindow.h"
#include "version.h"
#include "appdata.h"
#include "storage.h"
#include "translations.h"

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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
  /* From doc: This attribute must be set before Q(Gui)Application is constructed. */
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QApplication app(argc, argv);
  app.setApplicationName(APP_COMPANION);
  app.setOrganizationName(COMPANY);
  app.setOrganizationDomain(COMPANY_DOMAIN);
  app.setAttribute(Qt::AA_DontShowIconsInMenus, false);

  Q_INIT_RESOURCE(companion);

  if (AppDebugMessageHandler::instance())
    AppDebugMessageHandler::instance()->installAppMessageHandler();

  CustomDebug::setFilterRules();

  g.init();

  QStringList strl = QApplication::arguments();
  if (strl.contains("--version")) {
    printf("%s\n", VERSION);
    fflush(stdout);
    exit(0);
  }

#ifdef __APPLE__
  app.setStyle(new MyProxyStyle);
#endif

  Translations::installTranslators();

  // QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

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

  registerStorageFactories();
  registerOpenTxFirmwares();
  SimulatorLoader::registerSimulators();

  if (g.profile[g.id()].fwType().isEmpty()){
    g.profile[g.id()].fwType(Firmware::getDefaultVariant()->getId());
    g.profile[g.id()].fwName("");
  }

  QString splashScreen;
  splashScreen = ":/images/splash.png";

  QPixmap pixmap = QPixmap(splashScreen);
  QSplashScreen *splash = new QSplashScreen(pixmap);

  Firmware::setCurrentVariant(Firmware::getFirmwareForId(g.profile[g.id()].fwType()));

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

  SimulatorLoader::unregisterSimulators();
  unregisterOpenTxFirmwares();
  unregisterStorageFactories();

#if defined(JOYSTICKS) || defined(SIMU_AUDIO)
  SDL_Quit();
#endif

  qDebug() << "COMPANION EXIT" << result;
  return result;
}
