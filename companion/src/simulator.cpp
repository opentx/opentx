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
#include <QLocale>
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QSplashScreen>
#include <QThread>
#include <QDebug>
#include <QTextStream>
#if defined(JOYSTICKS) || defined(SIMU_AUDIO)
  #include <SDL.h>
  #undef main
#endif
#include "simulatordialog.h"
#include "eeprominterface.h"
#include "appdata.h"
#include "qxtcommandoptions.h"

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


void showMessage(const QString & message, enum QMessageBox::Icon icon = QMessageBox::NoIcon) {
  QMessageBox msgBox;
  msgBox.setText(message);
  msgBox.setIcon(icon);
  msgBox.exec();
}

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(companion);

  QApplication app(argc, argv);
  app.setApplicationName("OpenTX Simulator");
  app.setOrganizationName("OpenTX");
  app.setOrganizationDomain("open-tx.org");

  g.init();

#ifdef __APPLE__
  app.setStyle(new MyProxyStyle);
#endif

  /* QTranslator companionTranslator;
  companionTranslator.load(":/companion_" + locale);
  QTranslator qtTranslator;
  qtTranslator.load((QString)"qt_" + locale.left(2), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&companionTranslator);
  app.installTranslator(&qtTranslator);
*/

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
    showMessage(QObject::tr("WARNING: couldn't initialize SDL:\n%1").arg(SDL_GetError()), QMessageBox::Warning);
  }
#endif

  SimulatorDialog *dialog;
  QString eepromFileName;
  QDir eedir;
  QFile file;

  registerSimulators();
  registerOpenTxFirmwares();

  eedir = QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  if (!eedir.exists("OpenTX")) {
    if (!eedir.mkpath("OpenTX")) {
      showMessage(QObject::tr("WARNING: couldn't create directory for EEPROM:\n%1").arg(eedir.absoluteFilePath("OpenTX")), QMessageBox::Warning);
    }
  }
  eedir.cd("OpenTX");

  QStringList firmwareIds;
  int currentIdx = 0;
  foreach(SimulatorFactory *factory, registered_simulators) {
    firmwareIds << factory->name();
    if (factory->name() == g.lastSimulator()) {
      currentIdx = firmwareIds.size() - 1;
    }
  }

  QxtCommandOptions options;
  options.add("radio", "radio to simulate", QxtCommandOptions::ValueRequired);
  options.alias("radio", "r");
  options.add("help", "show this help text");
  options.alias("help", "h");
  options.parse(QCoreApplication::arguments());
  if (options.count("help") || options.showUnrecognizedWarning()) {
    QString msg;
    QTextStream stream(&msg);
    stream << "Usage: simulator [OPTION]... [EEPROM.BIN FILE] " << endl << endl;
    stream << "Options:" << endl;
    options.showUsage(false, stream);
    // list all available radios
    stream << endl << "Available radios:" << endl;
    foreach(QString name, firmwareIds) {
      stream << "\t" << name << endl;
    }
    // display
    showMessage(msg, QMessageBox::Information);
    return 1;
  }


  bool ok = false;
  QString firmwareId;
  if (options.count("radio") == 1) {
    firmwareId = options.value("radio").toString();
    if (firmwareIds.contains(firmwareId)) {
      ok = true;
    }
  }
  if (!ok) {
    firmwareId = QInputDialog::getItem(0, QObject::tr("Radio type"),
                                                QObject::tr("Which radio type do you want to simulate?"),
                                                firmwareIds, currentIdx, false, &ok);
  }
  qDebug() << "firmwareId" << firmwareId;

  if (ok && !firmwareId.isEmpty()) {
    if (firmwareId != g.lastSimulator()) {
      g.lastSimulator(firmwareId);
    }
    QString radioId;
    int pos = firmwareId.indexOf("-");
    if (pos > 0) {
      radioId = firmwareId.mid(pos+1);
      pos = radioId.lastIndexOf("-");
      if (pos > 0) {
        radioId = radioId.mid(0, pos);
      }
    }
    qDebug() << "radioId" << radioId;
    current_firmware_variant = GetFirmware(firmwareId);
    qDebug() << "current_firmware_variant" << current_firmware_variant->getName();

    if (options.positional().isEmpty()) {
      eepromFileName = QString("eeprom-%1.bin").arg(radioId);
      eepromFileName = eedir.filePath(eepromFileName.toLatin1());
    }
    else {
      eepromFileName = options.positional()[0];
    }
    qDebug() << "eepromFileName" << eepromFileName;
    // TODO display used eeprom filename somewhere

    SimulatorFactory * factory = getSimulatorFactory(firmwareId);
    if (!factory) {
      showMessage(QObject::tr("ERROR: Simulator %1 not found").arg(firmwareId), QMessageBox::Critical);
      return 2;
    }
    if (factory->type() == BOARD_HORUS)
      dialog = new SimulatorDialogHorus(NULL, factory->create());
    else if (factory->type() == BOARD_FLAMENCO)
      dialog = new SimulatorDialogFlamenco(NULL, factory->create());
    else if (factory->type() == BOARD_TARANIS_X9D || factory->type() == BOARD_TARANIS_X9DP || factory->type() == BOARD_TARANIS_X9E)
      dialog = new SimulatorDialogTaranis(NULL, factory->create(), SIMULATOR_FLAGS_S1|SIMULATOR_FLAGS_S2);
    else
      dialog = new SimulatorDialog9X(NULL, factory->create());
  }
  else {
    return 0;
  }

  dialog->show();
  dialog->start(eepromFileName.toLatin1().constData());

  int result = app.exec();

  delete dialog;

  unregisterSimulators();
  unregisterOpenTxFirmwares();

#if defined(JOYSTICKS) || defined(SIMU_AUDIO)
  SDL_Quit();
#endif

  return result;
}
