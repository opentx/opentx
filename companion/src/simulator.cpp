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
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>
#if defined(JOYSTICKS) || defined(SIMU_AUDIO)
  #include <SDL.h>
  #undef main
#endif

#include "appdata.h"
#include "constants.h"
#include "eeprominterface.h"
#include "simulator.h"
#include "simulatormainwindow.h"
#include "simulatorstartupdialog.h"
#include "storage.h"
#include "qxtcommandoptions.h"
#include "version.h"

#ifdef WIN32
  #include <windows.h>
  #ifdef _MSC_VER
    #define sleep(x) Sleep(x*1000)
  #endif
#endif
#if !defined(_MSC_VER) || defined(__GNUC__)
#include <unistd.h>
#endif

using namespace Simulator;

int finish(int exitCode);

void showMessage(const QString & message, enum QMessageBox::Icon icon = QMessageBox::NoIcon)
{
  QMessageBox msgBox;
  msgBox.setText(message);
  msgBox.setIcon(icon);
  msgBox.exec();
}

void sharedHelpText(QTextStream &stream)
{
  // list all available profiles
  stream << endl << QObject::tr("Available profiles:") << endl;
  QMapIterator<int, QString> pi(g.getActiveProfiles());
  while (pi.hasNext()) {
    pi.next();
    stream << "\t" << QObject::tr("ID: ") << pi.key() << QObject::tr(" Name: ") << pi.value() << endl;
  }
  // list all available radios
  stream << endl << QObject::tr("Available radios:") << endl;
  foreach(QString name, registered_simulators.keys()) {
    stream << "\t" << name << endl;
  }
}

QString findFirmwareId(int profileId)
{
  SimulatorFactory * sf = getSimulatorFactory(g.profile[profileId].fwType());
  if (sf)
    return sf->name();
  else
    return QString();
}

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(companion);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if defined(WIN32) && defined(WIN_USE_CONSOLE_STDIO)
  if (!GetConsoleWindow()) {
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
  }
  SetConsoleTitle(APP_SIMULATOR " Console");
#endif

  QApplication app(argc, argv);
  app.setApplicationName(APP_SIMULATOR);
  app.setOrganizationName(COMPANY);
  app.setOrganizationDomain(COMPANY_DOMAIN);

  g.init();

  QTranslator companionTranslator;
  companionTranslator.load(":/companion_" + g.locale());
  QTranslator qtTranslator;
  qtTranslator.load((QString)"qt_" + g.locale().left(2), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&companionTranslator);
  app.installTranslator(&qtTranslator);

#if defined(JOYSTICKS) || defined(SIMU_AUDIO)
  uint32_t sdlFlags = 0;
  #ifdef JOYSTICKS
    sdlFlags |= SDL_INIT_JOYSTICK;
  #endif
  #ifdef SIMU_AUDIO
    sdlFlags |= SDL_INIT_AUDIO;
  #endif
  if (SDL_Init(sdlFlags) < 0) {
    showMessage(QObject::tr("WARNING: couldn't initialize SDL:\n%1").arg(SDL_GetError()), QMessageBox::Warning);
  }
#endif

  QxtCommandOptions cliOptions;
  bool cliOptsFound = false;
  int profileId = (g.simuLastProfId() > -1 ? g.simuLastProfId() : g.id());
  SimulatorOptions simOptions = g.profile[profileId].simulatorOptions();

  registerStorageFactories();
  registerOpenTxFirmwares();
  registerSimulators();

  if (!registered_simulators.size()) {
    showMessage(QObject::tr("ERROR: No simulator libraries available."), QMessageBox::Critical);
    return finish(3);
  }

  cliOptions.add("profile", QObject::tr("Radio profile ID or Name to use for simulator."), QxtCommandOptions::ValueRequired);
  cliOptions.alias("profile", "p");
  cliOptions.add("radio", QObject::tr("Radio type to simulate (usually defined in profile)."), QxtCommandOptions::ValueRequired);
  cliOptions.alias("radio", "r");
  cliOptions.add("help", QObject::tr("show this help text"));
  cliOptions.alias("help", "h");
  cliOptions.parse(QCoreApplication::arguments());
  if (cliOptions.count("help") || cliOptions.showUnrecognizedWarning()) {
    QString msg;
    QTextStream stream(&msg);
    stream << QObject::tr("Usage: simulator [OPTION]... [EEPROM.BIN FILE OR DATA FOLDER] ") << endl << endl;
    stream << QObject::tr("Options:") << endl;
    cliOptions.showUsage(false, stream);
    sharedHelpText(stream);
    // display
    showMessage(msg, QMessageBox::Information);
    return finish(1);
  }

  // TODO : defaults should be set in Profile::init()
  if (simOptions.firmwareId.isEmpty())
    simOptions.firmwareId = findFirmwareId(profileId);
  if (simOptions.dataFolder.isEmpty())
    simOptions.dataFolder = g.eepromDir();
  if (simOptions.sdPath.isEmpty())
    simOptions.sdPath = g.profile[profileId].sdPath();

  if (cliOptions.count("profile") == 1) {
    bool chk;
    int pid = cliOptions.value("profile").toInt(&chk);
    if (chk)
      profileId = pid;
    else
      profileId = g.getActiveProfiles().key(cliOptions.value("profile").toString(), -1);

    if (!g.getActiveProfiles().contains(profileId)) {
      fprintf(stderr, "Error: Profile ID %d was not found.", profileId);
      return finish(1);
    }

    simOptions = g.profile[profileId].simulatorOptions();
    cliOptsFound = true;
  }

  if (cliOptions.count("radio") == 1) {
    simOptions.firmwareId = cliOptions.value("radio").toString();
    cliOptsFound = true;
  }

  if (cliOptsFound || (simOptions.dataFile.isEmpty() && !simOptions.firmwareId.isEmpty()))
    simOptions.dataFile = SimulatorStartupDialog::radioEepromFileName(simOptions.firmwareId, g.eepromDir());

  if (!cliOptions.positional().isEmpty()) {
    if (QString(cliOptions.positional().at(0)).contains(QRegExp(".*\\.[\\w]{2,6}$"))) {
      simOptions.dataFile = cliOptions.positional()[0];
      simOptions.startupDataType = SimulatorOptions::START_WITH_FILE;
    }
    else {
      simOptions.dataFolder = cliOptions.positional()[0];
      simOptions.startupDataType = SimulatorOptions::START_WITH_FOLDER;
    }
    cliOptsFound = true;
  }

  if (!cliOptsFound || profileId == -1 || simOptions.firmwareId.isEmpty() || (simOptions.dataFile.isEmpty() && simOptions.dataFolder.isEmpty())) {
    SimulatorStartupDialog * dlg = new SimulatorStartupDialog(&simOptions, &profileId);
    int ret = dlg->exec();
    dlg->deleteLater();
    if (ret != QDialog::Accepted) {
      return finish(0);
    }
  }
  qDebug() << __FILE__ << __LINE__ << "profileId=" << profileId << simOptions;

  if (profileId < 0 || simOptions.firmwareId.isEmpty() || (simOptions.dataFile.isEmpty() && simOptions.dataFolder.isEmpty())) {
    showMessage(QObject::tr("ERROR: Couldn't start simulator, missing radio/profile/data file/folder.\n  Profile ID: [%1]; Radio ID: [%2];\nData File: [%3]")
                .arg(profileId).arg(simOptions.firmwareId, simOptions.dataFile), QMessageBox::Critical);
    return finish(1);
  }
  if (!g.getActiveProfiles().contains(profileId) || !registered_simulators.keys().contains(simOptions.firmwareId)) {
    QString msg;
    QTextStream stream(&msg);
    stream << QObject::tr("ERROR: Radio profile or simulator firmware not found.\nProfile ID: [%1]; Radio ID: [%2]")
                    .arg(profileId).arg(simOptions.firmwareId);
    sharedHelpText(stream);
    showMessage(msg, QMessageBox::Critical);
    return finish(2);
  }

  SimulatorFactory * factory = getSimulatorFactory(simOptions.firmwareId);
  if (!factory) {
    showMessage(QObject::tr("ERROR: Simulator %1 not found").arg(simOptions.firmwareId), QMessageBox::Critical);
    return finish(2);
  }
  SimulatorInterface * simulator = factory->create();
  if (!simulator) {
    showMessage(QObject::tr("ERROR: Failed to create simulator interface, possibly missing or bad library."), QMessageBox::Critical);
    return finish(2);
  }

  current_firmware_variant = getFirmware(simOptions.firmwareId);

  g.sessionId(profileId);
  g.simuLastProfId(profileId);

  int result = 1;
  SimulatorMainWindow * mainWindow = new SimulatorMainWindow(NULL, simulator, SIMULATOR_FLAGS_STANDALONE);
  if (mainWindow->setOptions(simOptions, true)) {
    mainWindow->start();
    mainWindow->show();
    result = app.exec();
  }
  else {
    result = 3;
  }
  delete mainWindow;
  delete simulator;

  return finish(result);
}

int finish(int exitCode)
{
  qDebug() << "SIMULATOR EXIT" << exitCode;
  unregisterSimulators();
  unregisterOpenTxFirmwares();

#if defined(JOYSTICKS) || defined(SIMU_AUDIO)
  SDL_Quit();
#endif
#if defined(WIN32) && defined(WIN_USE_CONSOLE_STDIO)
  FreeConsole();
#endif

  return exitCode;
}
