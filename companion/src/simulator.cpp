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
//#include <QTranslator>
#include <QLocale>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QToolButton>
#if defined(JOYSTICKS) || defined(SIMU_AUDIO)
  #include <SDL.h>
  #undef main
#endif

#include "appdata.h"
#include "constants.h"
#include "eeprominterface.h"
#include "simulatordialog.h"
#include "storage.h"
#include "qxtcommandoptions.h"

#ifdef WIN32
  #include <windows.h>
  #ifdef _MSC_VER
    #define sleep(x) Sleep(x*1000)
  #endif
#endif
#if !defined(_MSC_VER) || defined(__GNUC__)
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

typedef struct
{
  int profileId;
  QString firmwareId;
  QString eepromFileName;
  QString dataFolder;
  bool useFolder;
} simulatorOptions_t;


QDir g_eepromDirectory;

int finish(int exitCode);

void showMessage(const QString & message, enum QMessageBox::Icon icon = QMessageBox::NoIcon)
{
  QMessageBox msgBox;
  msgBox.setText(message);
  msgBox.setIcon(icon);
  msgBox.exec();
}

bool usesCategorizedStorage(const QString &name)
{
  return name.contains("horus", Qt::CaseInsensitive);
}
QString radioEepromFileName(QString firmwareId)
{
  QString eepromFileName = "", ext = "bin";
  QString radioId = firmwareId;
  int pos = firmwareId.indexOf("-");
  if (pos > 0) {
    radioId = firmwareId.mid(pos+1);
    pos = radioId.lastIndexOf("-");
    if (pos > 0) {
      radioId = radioId.mid(0, pos);
    }
  }
  if (usesCategorizedStorage(radioId))
    ext = "otx";

  eepromFileName = QString("eeprom-%1.%2").arg(radioId, ext);
  eepromFileName = g_eepromDirectory.filePath(eepromFileName.toLatin1());
  // qDebug() << "radioId" << radioId << "eepromFileName" << eepromFileName;

  return eepromFileName;
}

bool startupOptionsDialog(simulatorOptions_t &opts)
{
  bool ret = false;
  QString label;

  QDialog * dialog = new QDialog();
  dialog->setWindowFlags(dialog->windowFlags() & (~ Qt::WindowContextHelpButtonHint));

  QFormLayout * form = new QFormLayout(dialog);
  form->addRow(new QLabel(QObject::tr("Simulator Startup Options:")));

  label = QObject::tr("Profile:");
  QComboBox * cbProf = new QComboBox();
  cbProf->setToolTip(QObject::tr("Existing radio profiles are shown here.<br/>" \
                                 "Create or edit profiles using the Companion application."));
  QMapIterator<int, QString> pi(g.getActiveProfiles());
  while (pi.hasNext()) {
    pi.next();
    cbProf->addItem(pi.value(), pi.key());
    if (pi.key() == opts.profileId)
      cbProf->setCurrentIndex(cbProf->count() - 1);
  }
  form->addRow(label, cbProf);

  label = QObject::tr("Radio Type:");
  QComboBox * cbType = new QComboBox();
  cbType->setToolTip(QObject::tr("Existing radio simulators are shown here.<br/>" \
                                 "The radio type specified in the selected profile is used by default."));
  cbType->addItems(registered_simulators.keys());
  cbType->setCurrentIndex(cbType->findText(opts.firmwareId));
  if (cbType->currentIndex() == -1 && cbProf->currentIndex() > -1){
    SimulatorFactory * sf = getSimulatorFactory(g.profile[cbProf->currentIndex()].fwType());
    if (sf)
      cbType->setCurrentIndex(cbType->findText(sf->name()));
  }
  form->addRow(label, cbType);

  label = QObject::tr("Data File:");
  QLineEdit * fwFile = new QLineEdit(opts.eepromFileName, dialog);
  fwFile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  fwFile->setToolTip(QObject::tr("Radio data (.bin/.eeprom/.otx) image file to use. A new file with a default image will be created if necessary.<br />" \
                                 "<b>NOTE</b>: any existing EEPROM data incompatible with the selected radio type may be overwritten!"));
  QToolButton * fwBtn = new QToolButton(dialog);
  fwBtn->setText("...");
  fwBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
  fwBtn->setToolTip(QObject::tr("Select radio data..."));
  QWidget * fw = new QWidget(dialog);
  QHBoxLayout * hl = new QHBoxLayout(fw);
  hl->setContentsMargins(0, 0, 0, 0);
  hl->setSpacing(2);
  hl->addWidget(fwFile, 2);
  hl->addWidget(fwBtn, 0);
  form->addRow(label, fw);

  bool show = usesCategorizedStorage(cbType->currentText());

  label = QObject::tr("Data Folder:");
  QLineEdit * fwFolder = new QLineEdit(opts.dataFolder, dialog);
  fwFolder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  fwFolder->setToolTip(QObject::tr("Directory containing RADIO and MODELS folders."));
  QToolButton * fwFolderBtn = new QToolButton(dialog);
  fwFolderBtn->setText("...");
  fwFolderBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
  fwFolderBtn->setToolTip(QObject::tr("Select data folder..."));
  QWidget * fwf = new QWidget(dialog);
  QHBoxLayout * hlf = new QHBoxLayout(fwf);
  hlf->setContentsMargins(0, 0, 0, 0);
  hlf->setSpacing(2);
  hlf->addWidget(fwFolder, 2);
  hlf->addWidget(fwFolderBtn, 0);
  form->addRow(label, fwf);
  fwf->setVisible(show);
  form->labelForField(fwf)->setVisible(show);

  label = QObject::tr("Date Source:");
  QRadioButton * dsfileBtn = new QRadioButton(QObject::tr("File"), dialog);
  QRadioButton * dsdirBtn = new QRadioButton(QObject::tr("Folder"), dialog);
  dsfileBtn->setChecked(true);
  QButtonGroup * btngrp = new QButtonGroup(dialog);
  btngrp->setExclusive(true);
  btngrp->addButton(dsfileBtn);
  btngrp->addButton(dsdirBtn);
  QWidget * btnwdgt = new QWidget(dialog);
  QHBoxLayout * btnlo = new QHBoxLayout(btnwdgt);
  btnlo->setContentsMargins(0, 0, 0, 0);
  btnlo->setSpacing(9);
  btnlo->addWidget(dsfileBtn);
  btnlo->addWidget(dsdirBtn);
  btnlo->addStretch(1);
  form->addRow(label, btnwdgt);
  btnwdgt->setVisible(show);
  form->labelForField(btnwdgt)->setVisible(show);

  QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
  form->addRow(&buttonBox);

  dialog->resize(400, dialog->sizeHint().height());

  QObject::connect(&buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
  QObject::connect(&buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

  // set new default radio type when profile choice changes
  QObject::connect(cbProf, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [cbProf, cbType](int index) {
    if (index < 0)
      return;
    SimulatorFactory * sf = getSimulatorFactory(g.profile[index].fwType());
    if (sf) {
      int i = cbType->findText(sf->name());
      if (i > -1) {
        cbType->setCurrentIndex(i);
      }
    }
  });

  // set new default firmware file when radio type changes
  QObject::connect(cbType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [cbType, fwFile, btnwdgt, fwf, form](int index) {
    if (index < 0)
      return;
    QString cbtxt = cbType->currentText();
    bool show = usesCategorizedStorage(cbtxt);
    fwFile->setText(radioEepromFileName(cbtxt));
    btnwdgt->setVisible(show);
    form->labelForField(btnwdgt)->setVisible(show);
    fwf->setVisible(show);
    form->labelForField(fwf)->setVisible(show);
  });

  // connect button to file selector dialog
  QObject::connect(fwBtn, &QToolButton::clicked, [dialog, fwFile, cbType, opts, dsfileBtn](bool) {
    QString filter = QObject::tr((usesCategorizedStorage(cbType->currentText()) ? OTX_FILES_FILTER : EEPROM_FILES_FILTER));
    filter += QObject::tr("All files (*.*)");
    QString file = QFileDialog::getSaveFileName(dialog, QObject::tr("Select EEPROM image"), fwFile->text(),
                                                filter, NULL, QFileDialog::DontConfirmOverwrite);
    if (!file.isEmpty()) {
      fwFile->setText(file);
      dsfileBtn->setChecked(true);
    }
  });

  // connect button to folder selector dialog
  QObject::connect(fwFolderBtn, &QToolButton::clicked, [dialog, fwFolder, cbType, opts, dsdirBtn](bool) {
    QString folder = QFileDialog::getExistingDirectory(dialog, QObject::tr("Select Data Directory"), fwFolder->text(), QFileDialog::DontUseNativeDialog);
    if (!folder.isEmpty()) {
      fwFolder->setText(folder);
      dsdirBtn->setChecked(true);
    }
  });

  // go
  if (dialog->exec() == QDialog::Accepted) {
    opts.profileId = cbProf->currentData().toInt();
    opts.firmwareId = cbType->currentText();
    opts.eepromFileName = fwFile->text();
    opts.dataFolder = fwFolder->text();
    opts.useFolder = dsdirBtn->isChecked();
    ret = true;
  }

  dialog->deleteLater();
  return ret;
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

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(companion);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if defined(WIN32) && defined(WIN_USE_CONSOLE_STDIO)
  AllocConsole();
  SetConsoleTitle("Simulator Console");
  freopen("conin$", "r", stdin);
  freopen("conout$", "w", stdout);
  freopen("conout$", "w", stderr);
#endif

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

  simulatorOptions_t simOptions;
  QxtCommandOptions cliOptions;
  bool cliOptsFound = false;

  g_eepromDirectory = QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  if (!g_eepromDirectory.exists("OpenTX")) {
    if (!g_eepromDirectory.mkpath("OpenTX")) {
      showMessage(QObject::tr("WARNING: couldn't create directory for EEPROM:\n%1").arg(g_eepromDirectory.absoluteFilePath("OpenTX")), QMessageBox::Warning);
    }
  }
  g_eepromDirectory.cd("OpenTX");

  registerSimulators();
  registerOpenTxFirmwares();

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

  if (cliOptions.count("radio") == 1) {
    simOptions.firmwareId = cliOptions.value("radio").toString();
    cliOptsFound = true;
  }
  else {
    simOptions.firmwareId = g.lastSimulator();
  }

  if (cliOptions.count("profile") == 1) {
    bool chk;
    int pid = cliOptions.value("profile").toInt(&chk);
    if (chk) {
      simOptions.profileId = pid;
    }
    else {
      simOptions.profileId = g.getActiveProfiles().key(cliOptions.value("profile").toString(), -1);
    }
    // load default radio for this profile if not already passed on command line
    if (!cliOptsFound) {
      SimulatorFactory * sf = getSimulatorFactory(g.profile[simOptions.profileId].fwType());
      if (sf)
        simOptions.firmwareId = sf->name();
    }
    cliOptsFound = true;
  }
  else if (g.simuLastProfId() != -1) {
    simOptions.profileId = g.simuLastProfId();
  }
  else {
    simOptions.profileId = g.id();
  }

  if (!cliOptions.positional().isEmpty()) {
    if (QString(cliOptions.positional().at(0)).contains(QRegExp(".*\\.[\\w]{2,6}$"))) {
      simOptions.eepromFileName = cliOptions.positional()[0];
    }
    else {
      simOptions.dataFolder = cliOptions.positional()[0];
      simOptions.useFolder = true;
    }
    cliOptsFound = true;
  }
  else if (cliOptsFound || g.simuLastEepe().isEmpty()) {
    simOptions.eepromFileName = radioEepromFileName(simOptions.firmwareId);
    simOptions.dataFolder = g_eepromDirectory.absolutePath();
  }
  else {
    simOptions.eepromFileName = g.simuLastEepe();
    simOptions.dataFolder = g.simuLastFolder();
  }

  if (!cliOptsFound || simOptions.profileId == -1 || simOptions.firmwareId.isEmpty() || (simOptions.eepromFileName.isEmpty() && simOptions.dataFolder.isEmpty())) {
    if (!startupOptionsDialog(simOptions)) {
      return finish(0);
    }
  }
  qDebug() << "firmwareId" << simOptions.firmwareId << "profileId" << simOptions.profileId << "eepromFileName" \
           << simOptions.eepromFileName << "dataFolder" << simOptions.dataFolder << "useFolder" << simOptions.useFolder;

  if (simOptions.profileId < 0 || simOptions.firmwareId.isEmpty() || (simOptions.eepromFileName.isEmpty() && simOptions.dataFolder.isEmpty())) {
    showMessage(QObject::tr("ERROR: Couldn't start simulator, missing radio/profile/data file/folder.\nProfile ID: [%1]; Radio ID: [%2];\nData File: [%3]")
                .arg(simOptions.profileId).arg(simOptions.firmwareId).arg(simOptions.eepromFileName), QMessageBox::Critical);
    return finish(1);
  }
  if (!g.getActiveProfiles().contains(simOptions.profileId) || !registered_simulators.keys().contains(simOptions.firmwareId)) {
    QString msg;
    QTextStream stream(&msg);
    stream << QObject::tr("ERROR: Radio profile or simulator firmware not found.\nProfile ID: [%1]; Radio ID: [%2]")
                    .arg(simOptions.profileId).arg(simOptions.firmwareId);
    sharedHelpText(stream);
    showMessage(msg, QMessageBox::Critical);
    return finish(2);
  }

  SimulatorFactory * factory = getSimulatorFactory(simOptions.firmwareId);
  if (!factory) {
    showMessage(QObject::tr("ERROR: Simulator %1 not found").arg(simOptions.firmwareId), QMessageBox::Critical);
    return finish(2);
  }

  current_firmware_variant = getFirmware(simOptions.firmwareId);
  registerStorageFactories();

  int oldProfId = g.id();
  g.id(simOptions.profileId);
  g.simuLastProfId(simOptions.profileId);
  g.lastSimulator(simOptions.firmwareId);
  g.simuLastEepe(simOptions.eepromFileName);
  g.simuLastFolder(simOptions.dataFolder);

  uint32_t flags = SIMULATOR_FLAGS_STANDALONE;
  SimulatorUiFlavor uiflav;

  switch(factory->type()) {
    case BOARD_HORUS :
      uiflav = SIMULATOR_UI_X12;
      break;
    case BOARD_TARANIS_X9E :
    case BOARD_TARANIS_X9D :
    case BOARD_TARANIS_X9DP :
      uiflav = SIMULATOR_UI_X9;
      break;
    default:
      uiflav = SIMULATOR_UI_9X;
  }

  SimulatorDialog * dialog = new SimulatorDialog(NULL, factory->create(), uiflav, flags);
  dialog->setRadioProfileId(simOptions.profileId);
  if (simOptions.useFolder && !simOptions.dataFolder.isEmpty()) {
    dialog->setDataPath(simOptions.dataFolder);
    dialog->setEepromData();
  }
  else {
    dialog->setEepromData(simOptions.eepromFileName.toLocal8Bit(), true);
  }
  dialog->start();
  dialog->show();
  int result = app.exec();

  g.id(oldProfId);
  dialog->deleteLater();

  return finish(result);
}

int finish(int exitCode)
{
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
