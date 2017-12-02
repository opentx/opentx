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

#include "appdata.h"

// Global data and storage object
AppData g;

// ** CompStoreObj class********************
void CompStoreObj::clear (const QString &tag1, const QString &tag2, const QString &tag3)
{
  QSettings settings(COMPANY, PRODUCT);
  if (tag2.isEmpty())
  {
    settings.remove(tag1);
  }
  else if (tag3.isEmpty())
  {
    settings.beginGroup(tag1);
    settings.remove(tag2);
    settings.endGroup();
  }
  else
  {
    settings.beginGroup(tag1);
    settings.beginGroup(tag2);
    settings.remove(tag3);
    settings.endGroup();
    settings.endGroup();
  }
}

template <typename OBJ_T, typename TAG_T, typename GP1_T, typename GP2_T>
void CompStoreObj::store(const OBJ_T newValue, OBJ_T & destValue, const TAG_T tag, const GP1_T group1, const GP2_T group2)
{
  QSettings settings(COMPANY, PRODUCT);
  QString g1 = group1;  // these may come in as const char * or QString
  QString g2 = group2;
  if (!g1.isEmpty()){
    settings.beginGroup(g1);
    if (!g2.isEmpty())
      settings.beginGroup(g2);
  }

  settings.setValue(tag, QVariant::fromValue(newValue));
  destValue = newValue;

  if (!g1.isEmpty()){
    settings.endGroup();
    if (!g2.isEmpty())
      settings.endGroup();
  }
}

template <typename OBJ_T, typename TAG_T, typename DEF_T, typename GP1_T, typename GP2_T>
void CompStoreObj::retrieve(OBJ_T & destValue, const TAG_T tag, const DEF_T def, const GP1_T group1, const GP2_T group2)
{
  QSettings settings(COMPANY, PRODUCT);
  QString g1 = group1;  // these may come in as const char * or QString
  QString g2 = group2;
  if (!g1.isEmpty()){
    settings.beginGroup(g1);
    if (!g2.isEmpty())
      settings.beginGroup(g2);
  }

  QVariant val = settings.value(QString(tag), QVariant::fromValue(def));
  if (val.canConvert<OBJ_T>())
    destValue = val.value<OBJ_T>();

  if (!g1.isEmpty()){;
    settings.endGroup();
    if (!g2.isEmpty())
      settings.endGroup();
  }
}

template <typename OBJ_T, typename TAG_T, typename DEF_T, typename GP1_T, typename GP2_T>
void CompStoreObj::getset(OBJ_T & value, const TAG_T tag, const DEF_T def, const GP1_T group1, const GP2_T group2 )
{
  retrieve(value, tag, def, group1, group2);
  store(value, value, tag, group1, group2);
}

template <typename OBJ_T, typename TAG_T, typename GP1_T, typename GP2_T>
void CompStoreObj::getset(OBJ_T & value, const TAG_T tag, const char * def, const GP1_T group1, const GP2_T group2 )
{
  getset(value, tag, QString(def), group1, group2);
}


// ** FwRevision class********************
int FwRevision::get( const QString fwType )
{
    QString result;
    retrieve( result, fwType, QString(""), "FwRevisions" );
    return result.toInt();
}

void FwRevision::set( const QString fwType, const int fwRevision )
{
    QString tempString = QString("%1").arg(fwRevision);
    store( tempString, tempString, fwType, "FwRevisions" );
}

void FwRevision::remove( const QString tag )
{
    clear( "FwRevisions", tag );
}

// ** JStickData class********************
// Get declarations
int JStickData::stick_axe() { return _stickAxe; }
int JStickData::stick_min() { return _stickMin; }
int JStickData::stick_med() { return _stickMed; }
int JStickData::stick_max() { return _stickMax; }
int JStickData::stick_inv() { return _stickInv; }

// Set declarations
void JStickData::stick_axe(const int it) { store( it, _stickAxe, QString("stick%1_axe").arg(index), "JsCalibration" );}
void JStickData::stick_min(const int it) { store( it, _stickMin, QString("stick%1_min").arg(index), "JsCalibration" );}
void JStickData::stick_med(const int it) { store( it, _stickMed, QString("stick%1_med").arg(index), "JsCalibration" );}
void JStickData::stick_max(const int it) { store( it, _stickMax, QString("stick%1_max").arg(index), "JsCalibration" );}
void JStickData::stick_inv(const int it) { store( it, _stickInv, QString("stick%1_inv").arg(index), "JsCalibration" );}

// Constructor
JStickData::JStickData()
{
    index = -1;
}

void JStickData::remove()
{
    // Remove all JStickData values from settings file
    QSettings settings(COMPANY, PRODUCT);
    settings.beginGroup( "JsCalibration" );
    settings.remove( QString( "stick%1_axe").arg(index) );
    settings.remove( QString( "stick%1_min").arg(index) );
    settings.remove( QString( "stick%1_med").arg(index) );
    settings.remove( QString( "stick%1_max").arg(index) );
    settings.remove( QString( "stick%1_inv").arg(index) );
    settings.endGroup();

    // Reset all JStickData variables to initial values
    init(index);
}

bool JStickData::existsOnDisk()
{
    QSettings settings(COMPANY, PRODUCT);
    settings.beginGroup("JsCalibration");
    int axe = settings.value( QString("stick%1_axe").arg(index), -1 ).toInt();
    settings.endGroup();

    return (axe > -1);
}

void JStickData::init(int newIndex)
{
    index = newIndex;
    _stickAxe = -1;
    _stickMin = -32767;
    _stickMed = 0;
    _stickMax = 0;
    _stickInv = 0;

    // Do not write empty joystick calibrations to disk.
    if ( !existsOnDisk() )
        return;

    flush();
}

void JStickData::flush()
{
    getset( _stickAxe, QString("stick%1_axe").arg(index), -1,     "JsCalibration" );
    getset( _stickMin, QString("stick%1_min").arg(index), -32767, "JsCalibration" );
    getset( _stickMed, QString("stick%1_med").arg(index),  0,     "JsCalibration" );
    getset( _stickMax, QString("stick%1_max").arg(index),  0,     "JsCalibration" );
    getset( _stickInv, QString("stick%1_inv").arg(index),  0,     "JsCalibration" );
}

// ** Profile class********************
// Get declarations
QString Profile::fwName()        const { return _fwName;        }
QString Profile::fwType()        const { return _fwType;        }
QString Profile::name()          const { return _name;          }
QString Profile::sdPath()        const { return _sdPath;        }
int     Profile::volumeGain()    const { return _volumeGain;    }
QString Profile::pBackupDir()    const { return _pBackupDir;    }
QString Profile::splashFile()    const { return _splashFile;    }
bool    Profile::burnFirmware()  const { return _burnFirmware;  }
bool    Profile::penableBackup() const { return _penableBackup; }
bool    Profile::renameFwFiles() const { return _renameFwFiles; }
int     Profile::channelOrder()  const { return _channelOrder;  }
int     Profile::defaultMode()   const { return _defaultMode;   }

QString Profile::beeper()        const { return _beeper;        }
QString Profile::countryCode()   const { return _countryCode;   }
QString Profile::display()       const { return _display;       }
QString Profile::haptic()        const { return _haptic;        }
QString Profile::speaker()       const { return _speaker;       }
QString Profile::stickPotCalib() const { return _stickPotCalib; }
QString Profile::timeStamp()     const { return _timeStamp;     }
QString Profile::trainerCalib()  const { return _trainerCalib;  }
QString Profile::controlTypes()  const { return _controlTypes;  }
QString Profile::controlNames()  const { return _controlNames;  }
int     Profile::gsStickMode()   const { return _gsStickMode;   }
int     Profile::ppmMultiplier() const { return _ppmMultiplier; }
int     Profile::vBatWarn()      const { return _vBatWarn;      }
int     Profile::vBatMin()       const { return _vBatMin;       }
int     Profile::vBatMax()       const { return _vBatMax;       }
int     Profile::txCurrentCalibration()  const { return _txCurrentCalibration; }
int     Profile::txVoltageCalibration()  const { return _txVoltageCalibration; }

SimulatorOptions Profile::simulatorOptions() const { return _simulatorOptions; }

// Set declarations
void Profile::name          (const QString x) { store(x, _name,          "Name"                  ,"Profiles", groupId());}
void Profile::fwName        (const QString x) { store(x, _fwName,        "fwName"                ,"Profiles", groupId());}
void Profile::fwType        (const QString x) { store(x, _fwType,        "fwType"                ,"Profiles", groupId());}
void Profile::sdPath        (const QString x) { store(x, _sdPath,        "sdPath"                ,"Profiles", groupId());}
void Profile::volumeGain    (const int     x) { store(x, _volumeGain,    "volumeGain"            ,"Profiles", groupId());}
void Profile::pBackupDir    (const QString x) { store(x, _pBackupDir,    "pBackupDir"            ,"Profiles", groupId());}
void Profile::splashFile    (const QString x) { store(x, _splashFile,    "SplashFileName"        ,"Profiles", groupId());}
void Profile::burnFirmware  (const bool    x) { store(x, _burnFirmware,  "burnFirmware"          ,"Profiles", groupId());}
void Profile::renameFwFiles (const bool    x) { store(x, _renameFwFiles, "rename_firmware_files" ,"Profiles", groupId());}
void Profile::penableBackup (const bool    x) { store(x, _penableBackup, "penableBackup"         ,"Profiles", groupId());}
void Profile::channelOrder  (const int     x) { store(x, _channelOrder,  "default_channel_order" ,"Profiles", groupId());}
void Profile::defaultMode   (const int     x) { store(x, _defaultMode,   "default_mode"          ,"Profiles", groupId());}
void Profile::beeper        (const QString x) { store(x, _beeper,        "Beeper"                ,"Profiles", groupId());}
void Profile::countryCode   (const QString x) { store(x, _countryCode,   "countryCode"           ,"Profiles", groupId());}
void Profile::display       (const QString x) { store(x, _display,       "Display"               ,"Profiles", groupId());}
void Profile::haptic        (const QString x) { store(x, _haptic,        "Haptic"                ,"Profiles", groupId());}
void Profile::speaker       (const QString x) { store(x, _speaker,       "Speaker"               ,"Profiles", groupId());}
void Profile::stickPotCalib (const QString x) { store(x, _stickPotCalib, "StickPotCalib"         ,"Profiles", groupId());}
void Profile::timeStamp     (const QString x) { store(x, _timeStamp,     "TimeStamp"             ,"Profiles", groupId());}
void Profile::trainerCalib  (const QString x) { store(x, _trainerCalib,  "TrainerCalib"          ,"Profiles", groupId());}
void Profile::controlTypes  (const QString x) { store(x, _controlTypes,  "ControlTypes"          ,"Profiles", groupId());}
void Profile::controlNames  (const QString x) { store(x, _controlNames,  "ControlNames"          ,"Profiles", groupId());}
void Profile::gsStickMode   (const int     x) { store(x, _gsStickMode,   "GSStickMode"           ,"Profiles", groupId());}
void Profile::ppmMultiplier (const int     x) { store(x, _ppmMultiplier, "PPM_Multiplier"        ,"Profiles", groupId());}
void Profile::vBatWarn      (const int     x) { store(x, _vBatWarn,      "vBatWarn"              ,"Profiles", groupId());}
void Profile::vBatMin       (const int     x) { store(x, _vBatMin,       "VbatMin"               ,"Profiles", groupId());}
void Profile::vBatMax       (const int     x) { store(x, _vBatMax,       "VbatMax"               ,"Profiles", groupId());}
void Profile::txCurrentCalibration (const int x) { store(x, _txCurrentCalibration, "currentCalib","Profiles", groupId());}
void Profile::txVoltageCalibration (const int x) { store(x, _txVoltageCalibration, "VbatCalib"   ,"Profiles", groupId());}

void Profile::simulatorOptions(const SimulatorOptions & x) { store(x, _simulatorOptions, "simulatorOptions" ,"Profiles", groupId()); }

// Constructor
Profile::Profile()
{
    index = -1;
}

// The default copy operator can not be used since the index variable would be destroyed
Profile& Profile::operator=(const Profile& rhs)
{
    name         ( rhs.name()          );
    fwName       ( rhs.fwName()        );
    fwType       ( rhs.fwType()        );
    sdPath       ( rhs.sdPath()        );
    volumeGain   ( rhs.volumeGain()    );
    pBackupDir   ( rhs.pBackupDir()    );
    splashFile   ( rhs.splashFile()    );
    burnFirmware ( rhs.burnFirmware()  );
    renameFwFiles( rhs.renameFwFiles() );
    penableBackup( rhs.penableBackup() );
    channelOrder ( rhs.channelOrder()  );
    defaultMode  ( rhs.defaultMode()   );
    beeper       ( rhs.beeper()        );
    countryCode  ( rhs.countryCode()   );
    display      ( rhs.display()       );
    haptic       ( rhs.haptic()        );
    speaker      ( rhs.speaker()       );
    stickPotCalib( rhs.stickPotCalib() );
    trainerCalib ( rhs.trainerCalib()  );
    controlTypes ( rhs.controlTypes()  );
    controlNames ( rhs.controlNames()  );
    gsStickMode  ( rhs.gsStickMode()   );
    ppmMultiplier( rhs.ppmMultiplier() );
    vBatWarn     ( rhs.vBatWarn()      );
    vBatMin      ( rhs.vBatMin()       );
    vBatMax      ( rhs.vBatMax()       );
    txCurrentCalibration ( rhs.txCurrentCalibration() );
    txVoltageCalibration ( rhs.txVoltageCalibration() );
    simulatorOptions     ( rhs.simulatorOptions()     );

    return *this;
}

void Profile::remove()
{
    // Remove all profile values from settings file
    QSettings settings(COMPANY, PRODUCT);
    settings.beginGroup("Profiles");
    settings.remove(groupId());
    settings.endGroup();

    // Reset all profile variables to initial values
    init(index);
}

bool Profile::existsOnDisk()
{
    QSettings settings(COMPANY, PRODUCT);
    settings.beginGroup("Profiles");
    settings.beginGroup(groupId());
    QStringList keyList = settings.allKeys();
    settings.endGroup();
    settings.endGroup();

    return (keyList.length() > 0);
}

void Profile::initFwVariables()
{
    _beeper =        "";
    _countryCode =   "";
    _display =       "";
    _haptic =        "";
    _speaker =       "";
    _stickPotCalib = "";
    _timeStamp =     "";
    _trainerCalib =  "";
    _controlTypes =  "";
    _controlNames =  "";

    _txCurrentCalibration =  0;
    _gsStickMode =   0;
    _ppmMultiplier = 0;
    _txVoltageCalibration =     0;
    _vBatWarn =      0;
    _vBatMin =       0;
    _vBatMax =       0;
}

void Profile::init(int newIndex)
{
    index = newIndex;

    _fwName =        "";
    _fwType =        "";
    _name =          "";
    _sdPath =        "";
    _volumeGain =    10;
    _pBackupDir =    "";
    _splashFile =    "";
    _burnFirmware =  false;
    _renameFwFiles = false;
    _penableBackup = false;
    _channelOrder =  0;
    _defaultMode =   1;

    _simulatorOptions = SimulatorOptions();

    initFwVariables();

    // Do not write empty profiles to disk except the default (0) profile.
    if ( index > 0 && !existsOnDisk())
        return;

    flush();
}

void Profile::flush()
{
    // Load and store all variables. Use default values if setting values are missing
    getset( _fwName,        "fwName"                ,""     ,"Profiles", groupId());
    getset( _fwType,        "fwType"                ,""     ,"Profiles", groupId());
    getset( _name,          "Name"                  ,""     ,"Profiles", groupId());
    getset( _sdPath,        "sdPath"                ,""     ,"Profiles", groupId());
    getset( _volumeGain,    "volumeGain"            ,10     ,"Profiles", groupId());
    getset( _pBackupDir,    "pBackupDir"            ,""     ,"Profiles", groupId());
    getset( _splashFile,    "SplashFileName"        ,""     ,"Profiles", groupId());
    getset( _burnFirmware,  "burnFirmware"          ,false  ,"Profiles", groupId());
    getset( _penableBackup, "penableBackup"         ,false  ,"Profiles", groupId());
    getset( _renameFwFiles, "rename_firmware_files" ,false  ,"Profiles", groupId());
    getset( _channelOrder,  "default_channel_order" ,0      ,"Profiles", groupId());
    getset( _defaultMode,   "default_mode"          ,1      ,"Profiles", groupId());

    getset( _beeper,        "Beeper"                ,""     ,"Profiles", groupId());
    getset( _countryCode,   "countryCode"           ,""     ,"Profiles", groupId());
    getset( _display,       "Display"               ,""     ,"Profiles", groupId());
    getset( _haptic,        "Haptic"                ,""     ,"Profiles", groupId());
    getset( _speaker,       "Speaker"               ,""     ,"Profiles", groupId());
    getset( _stickPotCalib, "StickPotCalib"         ,""     ,"Profiles", groupId());
    getset( _timeStamp,     "TimeStamp"             ,""     ,"Profiles", groupId());
    getset( _trainerCalib,  "TrainerCalib"          ,""     ,"Profiles", groupId());
    getset( _controlTypes,  "ControlTypes"          ,""     ,"Profiles", groupId());
    getset( _controlNames,  "ControlNames"          ,""     ,"Profiles", groupId());
    getset( _gsStickMode,   "GSStickMode"           ,0      ,"Profiles", groupId());
    getset( _ppmMultiplier, "PPM_Multiplier"        ,0      ,"Profiles", groupId());
    getset( _vBatWarn,      "vBatWarn"              ,0      ,"Profiles", groupId());
    getset( _vBatMin,       "VbatMin"               ,0      ,"Profiles", groupId());
    getset( _vBatMax,       "VbatMax"               ,0      ,"Profiles", groupId());
    getset( _txCurrentCalibration,  "currentCalib"  ,0      ,"Profiles", groupId());
    getset( _txVoltageCalibration,  "VbatCalib"     ,0      ,"Profiles", groupId());

    getset( _simulatorOptions, "simulatorOptions", SimulatorOptions(), "Profiles", groupId());

}

QString Profile::groupId()
{
  return QString("profile%1").arg(index);
}


// ** AppData class********************

// Get declarations
QStringList AppData::recentFiles()    { return _recentFiles;     }
QStringList AppData::simuDbgFilters() { return _simuDbgFilters;  }

QByteArray AppData::mainWinGeo()   { return _mainWinGeo;      }
QByteArray AppData::mainWinState() { return _mainWinState;    }
QByteArray AppData::modelEditGeo() { return _modelEditGeo;    }
QByteArray AppData::mdiWinGeo()    { return _mdiWinGeo;       }
QByteArray AppData::mdiWinState()  { return _mdiWinState;     }
QByteArray AppData::compareWinGeo(){ return _compareWinGeo;   }

QString AppData::armMcu()          { return _armMcu;          }
QString AppData::avrArguments()    { return _avrArguments;    }
QString AppData::avrPort()         { return _avrPort;         }
QString AppData::avrdudeLocation() { return _avrdudeLocation; }
QString AppData::dfuArguments()    { return _dfuArguments;    }
QString AppData::dfuLocation()     { return _dfuLocation;     }
QString AppData::locale()          { return _locale;          }
QString AppData::mcu()             { return _mcu;             }
QString AppData::programmer()      { return _programmer;      }
QString AppData::sambaLocation()   { return _sambaLocation;   }
QString AppData::sambaPort()       { return _sambaPort;       }

QString AppData::backupDir()       { return _backupDir;       }
QString AppData::gePath()          { return _gePath;          }
QString AppData::eepromDir()       { return _eepromDir;       }
QString AppData::flashDir()        { return _flashDir;        }
QString AppData::imagesDir()       { return _imagesDir;       }
QString AppData::logDir()          { return _logDir;          }
QString AppData::libDir()          { return _libDir;          }
QString AppData::snapshotDir()     { return _snapshotDir;     }
QString AppData::updatesDir()      { return _updatesDir;      }

int AppData::newModelAction()      { return _newModelAction;  }
int AppData::backLight()           { return _backLight;       }
int AppData::embedSplashes()       { return _embedSplashes;   }
int AppData::fwServerFails()       { return _fwServerFails;   }
int AppData::generalEditTab()      { return _generalEditTab;  }
int AppData::iconSize()            { return _iconSize;        }
int AppData::historySize()         { return _historySize;     }
int AppData::jsCtrl()              { return _jsCtrl;          }
int AppData::id()                  { return _id;              }
int AppData::theme()               { return _theme;           }
int AppData::warningId()           { return _warningId;       }
int AppData::simuLastProfId()      { return _simuLastProfId;  }
int AppData::sessionId()           { return _sessionId;       }

// Set declarations
void AppData::recentFiles     (const QStringList x) { store(x, _recentFiles,     "recentFileList"          );}
void AppData::simuDbgFilters  (const QStringList x) { store(x, _simuDbgFilters,  "simuDbgFilters"          );}

void AppData::mainWinGeo      (const QByteArray  x) { store(x, _mainWinGeo,      "mainWindowGeometry"      );}
void AppData::mainWinState    (const QByteArray  x) { store(x, _mainWinState,    "mainWindowState"         );}
void AppData::modelEditGeo    (const QByteArray  x) { store(x, _modelEditGeo,    "modelEditGeometry"       );}
void AppData::mdiWinGeo       (const QByteArray  x) { store(x, _mdiWinGeo,       "mdiWinGeo"               );}
void AppData::mdiWinState     (const QByteArray  x) { store(x, _mdiWinState,     "mdiWinState"             );}
void AppData::compareWinGeo   (const QByteArray  x) { store(x, _compareWinGeo,   "compareWinGeo"           );}

void AppData::armMcu          (const QString     x) { store(x, _armMcu,          "arm_mcu"                 );}
void AppData::avrArguments    (const QString     x) { store(x, _avrArguments,    "avr_arguments"           );}
void AppData::avrPort         (const QString     x) { store(x, _avrPort,         "avr_port"                );}
void AppData::avrdudeLocation (const QString     x) { store(x, _avrdudeLocation, "avrdudeLocation"         );}
void AppData::dfuArguments    (const QString     x) { store(x, _dfuArguments,    "dfu_arguments"           );}
void AppData::dfuLocation     (const QString     x) { store(x, _dfuLocation,     "dfu_location"            );}
void AppData::locale          (const QString     x) { store(x, _locale,          "locale"                  );}
void AppData::mcu             (const QString     x) { store(x, _mcu,             "mcu"                     );}
void AppData::programmer      (const QString     x) { store(x, _programmer,      "programmer"              );}
void AppData::sambaLocation   (const QString     x) { store(x, _sambaLocation,   "samba_location"          );}
void AppData::sambaPort       (const QString     x) { store(x, _sambaPort,       "samba_port"              );}

void AppData::backupDir       (const QString     x) { store(x, _backupDir,       "backupPath"              );}
void AppData::gePath          (const QString     x) { store(x, _gePath,          "gePath"                  );}
void AppData::eepromDir       (const QString     x) { store(x, _eepromDir,       "lastDir"                 );}
void AppData::flashDir        (const QString     x) { store(x, _flashDir,        "lastFlashDir"            );}
void AppData::imagesDir       (const QString     x) { store(x, _imagesDir,       "lastImagesDir"           );}
void AppData::logDir          (const QString     x) { store(x, _logDir,          "lastLogDir"              );}
void AppData::libDir          (const QString     x) { store(x, _libDir,          "libraryPath"             );}
void AppData::snapshotDir     (const QString     x) { store(x, _snapshotDir,     "snapshotpath"            );}
void AppData::updatesDir      (const QString     x) { store(x, _updatesDir,      "lastUpdatesDir"          );}

void AppData::newModelAction  (const int         x) { store(x, _newModelAction,  "newModelAction"          );}
void AppData::backLight       (const int         x) { store(x, _backLight,       "backLight"               );}
void AppData::embedSplashes   (const int         x) { store(x, _embedSplashes,   "embedded_splashes"       );}
void AppData::fwServerFails   (const int         x) { store(x, _fwServerFails,   "fwserver"                );}
void AppData::generalEditTab  (const int         x) { store(x, _generalEditTab,  "generalEditTab"          );}
void AppData::iconSize        (const int         x) { store(x, _iconSize,        "icon_size"               );}
void AppData::historySize     (const int         x) { store(x, _historySize,     "history_size"            );}
void AppData::jsCtrl          (const int         x) { store(x, _jsCtrl,          "js_ctrl"                 );}
void AppData::theme           (const int         x) { store(x, _theme,           "theme"                   );}
void AppData::warningId       (const int         x) { store(x, _warningId,       "warningId"               );}
void AppData::simuLastProfId  (const int         x) { store(x, _simuLastProfId,  "simuLastProfId"          );}

void AppData::id(const int x)
{
  store(x, _id, "profileId");
  sessionId(x);
}

// currently loaded radio profile ID, NOT saved to persistent storage
void AppData::sessionId       (const int         x) { _sessionId = x; }

// Constructor
AppData::AppData()
{
}

void AppData::init()
{
    qRegisterMetaTypeStreamOperators<SimulatorOptions>("SimulatorOptions");

    firstUse = false;

    QSettings settings(COMPANY, PRODUCT);
    if (!settings.contains("settings_version")) {
      firstUse = true;
    }
    convertSettings(settings);

    // Initialize the profiles
    for (int i=0; i<MAX_PROFILES; i++)
        profile[i].init( i );

    // Initialize the joysticks
    for (int i=0; i<MAX_JOYSTICKS; i++)
        joystick[i].init( i );

    // Load and store all variables. Use default values if setting values are missing
    QString _tempString;                                          // Do not touch. Do not change the settings version before a new verson update!
    getset( _tempString,      "settings_version"        ,"220" ); // This is a version marker. Will be used to upgrade the settings later on.

    getset( _recentFiles,     "recentFileList"          ,"" );
    getset( _simuDbgFilters,  "simuDbgFilters"          ,"" );

    getset( _mainWinGeo,      "mainWindowGeometry"      ,"" );
    getset( _mainWinState,    "mainWindowState"         ,"" );
    getset( _modelEditGeo,    "modelEditGeometry"       ,"" );
    getset( _mdiWinGeo,       "mdiWinGeo"               ,"" );
    getset( _mdiWinState,     "mdiWinState"             ,"" );
    getset( _compareWinGeo,   "compareWinGeo"           ,"" );

    getset( _armMcu,          "arm_mcu"                 ,"at91sam3s4-9x" );
    getset( _avrArguments,    "avr_arguments"           ,"" );
    getset( _avrPort,         "avr_port"                ,"" );
    getset( _avrdudeLocation, "avrdudeLocation"        ,"" );
    getset( _dfuArguments,    "dfu_arguments"           ,"-a 0" );
    getset( _dfuLocation,     "dfu_location"            ,"" );
    getset( _locale,          "locale"                  ,"" );
    getset( _mcu,             "mcu"                     ,"m64" );
    getset( _programmer,      "programmer"              ,"usbasp" );
    getset( _sambaLocation,   "samba_location"          ,"" );
    getset( _sambaPort,       "samba_port"              ,"\\USBserial\\COM23" );

    getset( _backupDir,       "backupPath"              ,"" );
    getset( _gePath,          "gePath"                  ,"" );
    getset( _eepromDir,       "lastDir"                 ,"" );
    getset( _flashDir,        "lastFlashDir"            ,"" );
    getset( _imagesDir,       "lastImagesDir"           ,"" );
    getset( _logDir,          "lastLogDir"              ,"" );
    getset( _libDir,          "libraryPath"             ,"" );
    getset( _snapshotDir,     "snapshotpath"            ,"" );
    getset( _updatesDir,      "lastUpdatesDir"          ,"" );

    getset( _OpenTxBranch, "OpenTxBranch", BRANCH_RELEASE_STABLE);
    
    appLogsDir_init();
    enableBackup_init();
    backupOnFlash_init();
    outputDisplayDetails_init();
    checkHardwareCompatibility_init();
    removeModelSlots_init();
    maximized_init();
    simuSW_init();
    tabbedMdi_init();
    appDebugLog_init();
    fwTraceLog_init();
    jsSupport_init();
    showSplash_init();
    snapToClpbrd_init();
    autoCheckApp_init();
    autoCheckFw_init();

    getset( _newModelAction,  "newModelAction"          ,1  );
    getset( _backLight,       "backLight"               ,0  );
    getset( _embedSplashes,   "embedded_splashes"       ,0  );
    getset( _fwServerFails,   "fwserver"                ,0  );
    getset( _generalEditTab,  "generalEditTab"          ,0  );
    getset( _iconSize,        "icon_size"               ,2  );
    getset( _jsCtrl,          "js_ctrl"                 ,0  );
    getset( _historySize,     "history_size"            ,10 );
    getset( _id,              "profileId"               ,0  );
    getset( _theme,           "theme"                   ,1  );
    getset( _warningId,       "warningId"               ,0  );
    getset( _simuLastProfId,  "simuLastProfId"          ,-1 );

    sessionId(id());
}

QMap<int, QString> AppData::getActiveProfiles()
{
  QMap<int, QString> active;
  for (int i=0; i<MAX_PROFILES; i++) {
    if (g.profile[i].existsOnDisk())
      active.insert(i, g.profile[i].name());
  }
  return active;
}

void AppData::convertSettings(QSettings & settings)
{
  // convert old settings to new
  if (settings.contains("useWizard")) {
    if (!settings.contains("newModelAction")) {
      newModelAction(settings.value("useWizard").toBool() ? 1 : 2);
    }
    settings.remove("useWizard");
  }
  if (settings.contains("warningId") && settings.value("warningId").toInt() == 7) {
    // meaning of warningId changed during v2.2 development but value of "7" indicates old setting, removing it will restore defaults
    settings.remove("warningId");
  }
}

bool AppData::hasCurrentSettings()
{
  QSettings settings(COMPANY, PRODUCT);
  if (!settings.contains("settings_version")) {
    return false;
  }
  return true;
}

bool AppData::findPreviousVersionSettings(QString * version)
{
  QSettings * fromSettings = NULL;
  *version = "";

  QSettings settings21("OpenTX", "Companion 2.1");
  if (settings21.contains("settings_version")) {
    fromSettings = &settings21;
    *version = "2.1";
  }
  else {
    settings21.clear();
  }

  QSettings settings20("OpenTX", "Companion 2.0");
  if (settings20.contains("settings_version")) {
    if (!fromSettings) {
      fromSettings = &settings20;
      *version = "2.0";
    }
  }
  else {
    settings20.clear();
  }

  QSettings settings16("OpenTX", "OpenTX Companion");
  if (settings16.contains("settings_version")) {
    if (!fromSettings) {
      fromSettings = &settings16;
      *version = "1.x";
    }
  }
  else {
    settings16.clear();
  }

  if (!fromSettings)
    return false;

  return true;
}

bool AppData::importSettings(QString fromVersion)
{
  QSettings toSettings(COMPANY, PRODUCT);

  QString fromCompany;
  QString fromProduct;

  upgradeFromVersion = "";

  if (fromVersion == "2.1") {
    fromCompany = "OpenTX";
    fromProduct = "Companion 2.1";
  }
  else if (fromVersion == "2.0") {
    fromCompany = "OpenTX";
    fromProduct = "Companion 2.0";
  }
  else if (fromVersion == "1.x") {
    fromCompany = "OpenTX";
    fromProduct = "OpenTX Companion";
  }
  else
    return false;

  upgradeFromVersion = fromVersion;

  QSettings fromSettings(fromCompany, fromProduct);

  // do not copy these settings
  QStringList excludeKeys = QStringList() << "compilation-server";
#ifdef WIN32
  // locations of tools which come with Companion distros
  excludeKeys << "avrdude_location" << "avrdudeLocation" << "dfu_location";
  // install-specific keys;  "." is the "default" key which may contain install path
  excludeKeys << "Start Menu Folder" << ".";
#endif

  // import settings
  foreach (const QString & key, fromSettings.allKeys()) {
    if (fromSettings.value(key).isValid() && !excludeKeys.contains(key)) {
      toSettings.setValue(key, fromSettings.value(key));
    }
  }

  return true;
}
