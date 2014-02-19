// Companion Application Data Class.
// Author Kjell Kernen

// All temporary and permanent global variables are defined here to make 
// initialization and storage safe and visible.
// Do not access variables in QSettings directly, it is not type safe!


#ifndef APPDATA_H
#define APPDATA_H
#include <QtGui>

#define COMPANY "OpenTX Companion"
#define PRODUCT "OpenTX"
#define MAX_PROFILES 15
#define MAX_JOYSTICKS 8

class DataObj
{
public:
	void clear (const QString tag1="", const QString tag2="", const QString tag3="")
	{
		QSettings settings(PRODUCT, COMPANY);
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

	void store(const QByteArray newArray, QByteArray &array, const QString tag, const QString group1="", const QString group2="" )
	{
		QSettings settings(PRODUCT, COMPANY);
		if (!group1.isEmpty()) settings.beginGroup(group1);
		if (!group2.isEmpty()) settings.beginGroup(group2);

		settings.setValue(tag, newArray);
		array = newArray;

		if (!group1.isEmpty()) settings.endGroup();
		if (!group2.isEmpty()) settings.endGroup();
	}

	void store(const QStringList newSList, QStringList &stringList, const QString tag, const QString group1="", const QString group2="" )
	{
		QSettings settings(PRODUCT, COMPANY);
		if (!group1.isEmpty()) settings.beginGroup(group1);
		if (!group2.isEmpty()) settings.beginGroup(group2);

		settings.setValue(tag, newSList);
		stringList = newSList;

		if (!group1.isEmpty()) settings.endGroup();
		if (!group2.isEmpty()) settings.endGroup();
	}

	void store(const QString newString, QString &string, const QString tag, const QString group1="", const QString group2="" )
	{
		QSettings settings(PRODUCT, COMPANY);
		if (!group1.isEmpty()) settings.beginGroup(group1);
		if (!group2.isEmpty()) settings.beginGroup(group2);

		settings.setValue(tag, newString);
		string = newString;

		if (!group1.isEmpty()) settings.endGroup();
		if (!group2.isEmpty()) settings.endGroup();
	}

	void store(const bool newTruth, bool &truth, const QString tag, const QString group1="", const QString group2="" )
	{
		QSettings settings(PRODUCT, COMPANY);
		if (!group1.isEmpty()) settings.beginGroup(group1);
		if (!group2.isEmpty()) settings.beginGroup(group2);

		settings.setValue(tag, newTruth);
		truth = newTruth;

		if (!group1.isEmpty()) settings.endGroup();
		if (!group2.isEmpty()) settings.endGroup();
	}

	void store(const int newNumber, int &number, const QString tag, const QString group1="", const QString group2="" )
	{
		QSettings settings(PRODUCT, COMPANY);
		if (!group1.isEmpty()) settings.beginGroup(group1);
		if (!group2.isEmpty()) settings.beginGroup(group2);

		settings.setValue(tag, newNumber);
		number = newNumber;

		if (!group1.isEmpty()) settings.endGroup();
		if (!group2.isEmpty()) settings.endGroup();
	}

	// Retrieval functions
	void retrieve( QByteArray &array, const QString tag, const QString def, const QString group1="", const QString group2="" )
	{
		QSettings settings(PRODUCT, COMPANY);
		if (!group1.isEmpty()) settings.beginGroup(group1);
		if (!group2.isEmpty()) settings.beginGroup(group2);

		array = settings.value(tag, def).toByteArray();

		if (!group1.isEmpty()) settings.endGroup();
		if (!group2.isEmpty()) settings.endGroup();
	}

	void retrieve( QStringList &stringList, const QString tag, const QString def, const QString group1="", const QString group2="" )
	{
		QSettings settings(PRODUCT, COMPANY);
		if (!group1.isEmpty()) settings.beginGroup(group1);
		if (!group2.isEmpty()) settings.beginGroup(group2);

		stringList = settings.value(tag, def).toStringList();

		if (!group1.isEmpty()) settings.endGroup();
		if (!group2.isEmpty()) settings.endGroup();
	}

	void retrieve( QString &string, const QString tag, const QString def, const QString group1="", const QString group2="" )
	{
		QSettings settings(PRODUCT, COMPANY);
		if (!group1.isEmpty()) settings.beginGroup(group1);
		if (!group2.isEmpty()) settings.beginGroup(group2);

		string = settings.value(tag, def).toString();

		if (!group1.isEmpty()) settings.endGroup();
		if (!group2.isEmpty()) settings.endGroup();
	}

	void retrieve( bool &truth, const QString tag, const bool def, const QString group1="", const QString group2="" )
	{
		QSettings settings(PRODUCT, COMPANY);
		if (!group1.isEmpty()) settings.beginGroup(group1);
		if (!group2.isEmpty()) settings.beginGroup(group2);

		truth = settings.value(tag, def).toBool();

		if (!group1.isEmpty()) settings.endGroup();
		if (!group2.isEmpty()) settings.endGroup();
	}

	void retrieve( int &number, const QString tag, const int def, const QString group1="", const QString group2="" )
	{
		QSettings settings(PRODUCT, COMPANY);
		if (!group1.isEmpty()) settings.beginGroup(group1);
		if (!group2.isEmpty()) settings.beginGroup(group2);

		number = settings.value(tag, def).toInt();

		if (!group1.isEmpty()) settings.endGroup();
		if (!group2.isEmpty()) settings.endGroup();
	}

	// Retrieve and Store functions
	void getset( QByteArray &array, const QString tag, const QString def, const QString group1="", const QString group2="" )
	{
		retrieve( array, tag, def, group1, group2);
		store(array, array, tag, group1, group2);
	}

	void getset( QStringList &stringList, const QString tag, const QString def, const QString group1="", const QString group2="" )
	{
		retrieve( stringList, tag, def, group1, group2);
		store(stringList, stringList, tag, group1, group2);
	}

	void getset( QString &string, const QString tag, const QString def, const QString group1="", const QString group2="" )
	{
		retrieve( string, tag, def, group1, group2);
		store(string, string, tag, group1, group2);
	}

	void getset( bool &truth, const QString tag, const bool def, const QString group1="", const QString group2="" )
	{
		retrieve( truth, tag, def, group1, group2);
		store(truth, truth, tag, group1, group2);
	}

	void getset( int &number, const QString tag, const int def, const QString group1="", const QString group2="" )
	{
		retrieve( number, tag, def, group1, group2);
		store(number, number, tag, group1, group2);
	}
}; 

class FwRevision:DataObj
{
public:
	int get( const QString fwName )
	{
		QString result;
		retrieve( result, fwName, "", "FwRevisions" );
		return result.toInt();
	}

	void set( const QString fwName, const int fwRevision )
	{
		QString tempString= QString("%1").arg(fwRevision);
		store( tempString, tempString, fwName, "FwRevisions" );
	}

	void remove( const QString tag )
	{
		clear( "FwRevisions", tag );
	}
};

class JStickData:DataObj
{
private:
	int index;

	int _stickAxe;
	int _stickMin;
	int _stickMed;
	int _stickMax;
	int _stickInv;

public:
public:
	// All the get declarations
	int stick_axe() { return _stickAxe; }
	int stick_min() { return _stickMin; }
	int stick_med() { return _stickMed; }
	int stick_max() { return _stickMax; }
	int stick_inv() { return _stickInv; }

	// All the set declarations
	void stick_axe(const int it) { store( it, _stickAxe, QString("stick%1_axe").arg(index), "JsCalibration" );}
	void stick_min(const int it) { store( it, _stickMin, QString("stick%1_min").arg(index), "JsCalibration" );}
	void stick_med(const int it) { store( it, _stickMed, QString("stick%1_med").arg(index), "JsCalibration" );}
	void stick_max(const int it) { store( it, _stickMax, QString("stick%1_max").arg(index), "JsCalibration" );}
	void stick_inv(const int it) { store( it, _stickInv, QString("stick%1_inv").arg(index), "JsCalibration" );}

	// Constructor
	JStickData()
	{
		index = -1;
	}

	void remove()
	{
		// Remove all JStickData values from settings file
		QSettings settings(PRODUCT, COMPANY);
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

	bool existsOnDisk()
	{
		QSettings settings(PRODUCT, COMPANY);
		settings.beginGroup("JsCalibration");
		int axe = settings.value( QString("stick%1_axe").arg(index), -1 ).toInt();
		settings.endGroup();

		return (axe > -1);
	}

	void init(int newIndex)
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

	void flush()
	{
		getset( _stickAxe, QString("stick%1_axe").arg(index), -1,     "JsCalibration" );
		getset( _stickMin, QString("stick%1_min").arg(index), -32767, "JsCalibration" );
		getset( _stickMed, QString("stick%1_med").arg(index),  0,     "JsCalibration" );
		getset( _stickMax, QString("stick%1_max").arg(index),  0,     "JsCalibration" );
		getset( _stickInv, QString("stick%1_inv").arg(index),  0,     "JsCalibration" );
	}
};

class Profile:DataObj
{
private:
	// Class Internal Variable
	int index;

	// Application Variables
	QString _firmware;
	QString _name;
	QString _sdPath;
	QString _splashFile;
	bool    _burnFirmware;
	bool    _renameFwFiles;
	bool    _patchImage;
	int     _channelOrder;
	int     _defaultMode;

	// Firmware Variables
	QString _beeper;
	QString _countryCode;
	QString _display;
	QString _haptic;
	QString _speaker;
	QString _stickPotCalib;
	QString _trainerCalib;
	int     _currentCalib;
	int     _gsStickMode;
	int     _ppmMultiplier;
	int     _vBatCalib;
	int     _vBatWarn;

public:
	// All the get declarations
	QString firmware()      { return _firmware;      }
	QString name()          { return _name;          }
	QString sdPath()        { return _sdPath;        }
	QString splashFile()    { return _splashFile;    }
	bool    burnFirmware()  { return _burnFirmware;  }
	bool    renameFwFiles() { return _renameFwFiles; }
	bool    patchImage()    { return _patchImage;    }
	int     channelOrder()  { return _channelOrder;  }
	int     defaultMode()   { return _defaultMode;   }

	QString beeper()        { return _beeper;        }
	QString countryCode()   { return _countryCode;   }
	QString display()       { return _display;       }
	QString haptic()        { return _haptic;        }
	QString speaker()       { return _speaker;       }
	QString stickPotCalib() { return _stickPotCalib; }
	QString trainerCalib()  { return _trainerCalib;  }
	int     currentCalib()  { return _currentCalib;  }
	int     gsStickMode()   { return _gsStickMode;   }
	int     ppmMultiplier() { return _ppmMultiplier; }
	int     vBatCalib()     { return _vBatCalib;     }
	int     vBatWarn()      { return _vBatWarn;      }

	// All the set declarations
	void name          (const QString x) { if (x.isEmpty())  // Name may never be empty!
		                                   store("----", _name,     "Name"                  ,"Profiles", QString("profile%1").arg(index));
	                                    else
		                                   store(x, _name,          "Name"                  ,"Profiles", QString("profile%1").arg(index));}

	void firmware      (const QString x) { store(x, _firmware,      "firmware"              ,"Profiles", QString("profile%1").arg(index));}
	void sdPath        (const QString x) { store(x, _sdPath,        "sdPath"                ,"Profiles", QString("profile%1").arg(index));}
	void splashFile    (const QString x) { store(x, _splashFile,    "SplashFileName"        ,"Profiles", QString("profile%1").arg(index));}
	void burnFirmware  (const bool    x) { store(x, _burnFirmware,  "burnFirmware"          ,"Profiles", QString("profile%1").arg(index));}
	void renameFwFiles (const bool    x) { store(x, _renameFwFiles, "rename_firmware_files" ,"Profiles", QString("profile%1").arg(index));}
	void patchImage    (const bool    x) { store(x, _patchImage,    "patchImage"            ,"Profiles", QString("profile%1").arg(index));}
	void channelOrder  (const int     x) { store(x, _channelOrder,  "default_channel_order" ,"Profiles", QString("profile%1").arg(index));}
	void defaultMode   (const int     x) { store(x, _defaultMode,   "default_mode"          ,"Profiles", QString("profile%1").arg(index));}

	void beeper        (const QString x) { store(x, _beeper,        "Beeper"                ,"Profiles", QString("profile%1").arg(index));}
	void countryCode   (const QString x) { store(x, _countryCode,   "countryCode"           ,"Profiles", QString("profile%1").arg(index));}
	void display       (const QString x) { store(x, _display,       "Display"               ,"Profiles", QString("profile%1").arg(index));}
	void haptic        (const QString x) { store(x, _haptic,        "Haptic"                ,"Profiles", QString("profile%1").arg(index));}
	void speaker       (const QString x) { store(x, _speaker,       "Speaker"               ,"Profiles", QString("profile%1").arg(index));}
	void stickPotCalib (const QString x) { store(x, _stickPotCalib, "StickPotCalib"         ,"Profiles", QString("profile%1").arg(index));}
	void trainerCalib  (const QString x) { store(x, _trainerCalib,  "TrainerCalib"          ,"Profiles", QString("profile%1").arg(index));}
	void currentCalib  (const int     x) { store(x, _currentCalib,  "currentCalib"          ,"Profiles", QString("profile%1").arg(index));}
	void gsStickMode   (const int     x) { store(x, _gsStickMode,   "GSStickMode"           ,"Profiles", QString("profile%1").arg(index));}
	void ppmMultiplier (const int     x) { store(x, _ppmMultiplier, "PPM_Multiplier"        ,"Profiles", QString("profile%1").arg(index));}
	void vBatCalib     (const int     x) { store(x, _vBatCalib,     "VbatCalib"             ,"Profiles", QString("profile%1").arg(index));}
	void vBatWarn      (const int     x) { store(x, _vBatWarn,      "vBatWarn"              ,"Profiles", QString("profile%1").arg(index));}

	// Constructor
	Profile()
	{
		index = -1;
	}

	void remove()
	{
		// Remove all profile values from settings file
		QSettings settings(PRODUCT, COMPANY);
		settings.beginGroup("Profiles");
		settings.remove(QString("profile%1").arg(index));
		settings.endGroup();

		// Reset all profile variables to initial values
		init(index);
	}

	bool existsOnDisk()
	{
		QSettings settings(PRODUCT, COMPANY);
		settings.beginGroup("Profiles");
		settings.beginGroup(QString("profile%1").arg(index));
		QStringList keyList = settings.allKeys();
		settings.endGroup();
		settings.endGroup();

		return (keyList.length() > 0);
	}

	void init(int newIndex)
	{
		index = newIndex;

		_firmware =      "";
		_name =          "";
		_sdPath =        "";
		_splashFile =    "";
		_burnFirmware =  false;
		_renameFwFiles = false;
		_patchImage =    false;
		_channelOrder =  0;
		_defaultMode =   1;

		_beeper =        "";
		_countryCode =   "";
		_display =       "";
		_haptic =        "";
		_speaker =       "";
		_stickPotCalib = "";
		_trainerCalib =  "";

		_currentCalib =  0;
		_gsStickMode =   0;
		_ppmMultiplier = 0;
		_vBatCalib =     0;
		_vBatWarn =      0;

		// Do not write empty profiles to disk except the default (0) profile.
		if ( index > 0 && !existsOnDisk())
			return;

		flush();
	}

	void flush()
	{
		// Load and store all variables. Use default values if setting values are missing
		getset( _firmware,      "firmware"              ,""     ,"Profiles", QString("profile%1").arg(index));
		getset( _name,          "Name"                  ,"----" ,"Profiles", QString("profile%1").arg(index));
		getset( _sdPath,        "sdPath"                ,""     ,"Profiles", QString("profile%1").arg(index));
		getset( _splashFile,    "SplashFileName"        ,""     ,"Profiles", QString("profile%1").arg(index));
		getset( _burnFirmware,  "burnFirmware"          ,false  ,"Profiles", QString("profile%1").arg(index));
		getset( _renameFwFiles, "rename_firmware_files" ,false  ,"Profiles", QString("profile%1").arg(index));
		getset( _patchImage,    "patchImage"            ,false  ,"Profiles", QString("profile%1").arg(index));
		getset( _channelOrder,  "default_channel_order" ,0      ,"Profiles", QString("profile%1").arg(index));
		getset( _defaultMode,   "default_mode"          ,1      ,"Profiles", QString("profile%1").arg(index));

		getset( _beeper,        "Beeper"                ,""     ,"Profiles", QString("profile%1").arg(index));
		getset( _countryCode,   "countryCode"           ,""     ,"Profiles", QString("profile%1").arg(index));
		getset( _display,       "Display"               ,""     ,"Profiles", QString("profile%1").arg(index));
		getset( _haptic,        "Haptic"                ,""     ,"Profiles", QString("profile%1").arg(index));
		getset( _speaker,       "Speaker"               ,""     ,"Profiles", QString("profile%1").arg(index));
		getset( _stickPotCalib, "StickPotCalib"         ,""     ,"Profiles", QString("profile%1").arg(index));
		getset( _trainerCalib,  "TrainerCalib"          ,""     ,"Profiles", QString("profile%1").arg(index));
		getset( _currentCalib,  "currentCalib"          ,0      ,"Profiles", QString("profile%1").arg(index));
		getset( _gsStickMode,   "GSStickMode"           ,0      ,"Profiles", QString("profile%1").arg(index));
		getset( _ppmMultiplier, "PPM_Multiplier"        ,0      ,"Profiles", QString("profile%1").arg(index));
		getset( _vBatCalib,     "VbatCalib"             ,0      ,"Profiles", QString("profile%1").arg(index));
		getset( _vBatWarn,      "vBatWarn"              ,0      ,"Profiles", QString("profile%1").arg(index));
	}
};

class AppData:DataObj
{
	// All the global variables
public:
	Profile    profile[MAX_PROFILES];
	JStickData joystick[MAX_JOYSTICKS];
	FwRevision fwRev;

private:
	QStringList _recentFiles;
	QByteArray _mainWinGeo;
	QByteArray _mainWinState;
	QByteArray _modelEditGeo;

	QString _armMcu;
	QString _avrArguments;
	QString _avrPort;
	QString _avrdudeLocation;
	QString _cpuId;
	QString _dfuArguments;
	QString _dfuLocation;
	QString _lastFw;
	QString _locale;
	QString _mcu;
	QString _programmer;
	QString _sambaLocation;
	QString _sambaPort;

	QString _backupDir;
	QString _compileServer;
	QString _gePath;
	QString _lastDir;
	QString _flashDir;
	QString _imagesDir;
	QString _logDir;
	QString _libDir;
	QString _snapshotDir;

	bool _enableBackup;
	bool _backupOnFlash;
	bool _maximized;
	bool _jsSupport;
	bool _rev4aSupport;
	bool _showSplash;
	bool _snapToClpbrd;
	bool _autoCheckApp;
	bool _autoCheckFw;
	bool _simuSW;
	bool _enableWizard;

	int _backLight;
	int _embedSplashes;
	int _fwServerFails;
	int _generalEditTab;
	int _iconSize;
	int _jsCtrl;
	int _historySize;
	int _modelEditTab;
	int _id;
	int _theme;
	int _warningId;


public:
	// All the get declarations
	QStringList recentFiles() { return _recentFiles;     }
	QByteArray mainWinGeo()   { return _mainWinGeo;      }
	QByteArray mainWinState() { return _mainWinState;    }
	QByteArray modelEditGeo() { return _modelEditGeo;    }

	QString armMcu()          { return _armMcu;          }
	QString avrArguments()    { return _avrArguments;    }
	QString avrPort()         { return _avrPort;         }
	QString avrdudeLocation() { return _avrdudeLocation; }
	QString cpuId()           { return _cpuId;           }
	QString dfuArguments()    { return _dfuArguments;    }
	QString dfuLocation()     { return _dfuLocation;     }
	QString lastFw()          { return _lastFw;          }
	QString locale()          { return _locale;          }
	QString mcu()             { return _mcu;             }
	QString programmer()      { return _programmer;      }
	QString sambaLocation()   { return _sambaLocation;   }
	QString sambaPort()       { return _sambaPort;       }

	QString backupDir()       { return _backupDir;       }
	QString compileServer()   { return _compileServer;   }
	QString gePath()          { return _gePath;          }
	QString lastDir()         { return _lastDir;         }
	QString flashDir()        { return _flashDir;        }
	QString imagesDir()       { return _imagesDir;       }
	QString logDir()          { return _logDir;          }
	QString libDir()          { return _libDir;          }
	QString snapshotDir()     { return _snapshotDir;     }

	bool enableBackup()       { return _enableBackup;    }
	bool backupOnFlash()      { return _backupOnFlash;   }
	bool jsSupport()          { return _jsSupport;       }
	bool rev4aSupport()       { return _rev4aSupport;    }
	bool maximized()          { return _maximized;       }
	bool showSplash()         { return _showSplash;      }
	bool snapToClpbrd()       { return _snapToClpbrd;    }
	bool autoCheckApp()       { return _autoCheckApp;    }
	bool autoCheckFw()        { return _autoCheckFw;     }
	bool simuSW()             { return _simuSW;          }
	bool enableWizard()       { return _enableWizard;    }

	int backLight()           { return _backLight;       }
	int embedSplashes()       { return _embedSplashes;   }
	int fwServerFails()       { return _fwServerFails;   }
	int generalEditTab()      { return _generalEditTab;  }
	int iconSize()            { return _iconSize;        }
	int historySize()         { return _historySize;     }
	int jsCtrl()              { return _jsCtrl;          }
	int modelEditTab()        { return _modelEditTab;    }
	int id()                  { return _id;              }
	int theme()               { return _theme;           }
	int warningId()           { return _warningId;       }

	// All the set declarations
	void recentFiles     (const QStringList x) { store(x, _recentFiles,     "recentFileList"          );}
	void mainWinGeo      (const QByteArray  x) { store(x, _mainWinGeo,      "mainWindowGeometry"      );}
	void mainWinState    (const QByteArray  x) { store(x, _mainWinState,    "mainWindowState"         );}
	void modelEditGeo    (const QByteArray  x) { store(x, _modelEditGeo,    "modelEditGeometry"       );}

	void armMcu          (const QString     x) { store(x, _armMcu,          "arm_mcu"                 );}
	void avrArguments    (const QString     x) { store(x, _avrArguments,    "avr_arguments"           );}
	void avrPort         (const QString     x) { store(x, _avrPort,         "avr_port"                );}
	void avrdudeLocation (const QString     x) { store(x, _avrdudeLocation, "avrdudeLocation"         );}
	void cpuId           (const QString     x) { store(x, _cpuId,           "cpu_id"                  );}
	void dfuArguments    (const QString     x) { store(x, _dfuArguments,    "dfu_arguments"           );}
	void dfuLocation     (const QString     x) { store(x, _dfuLocation,     "dfu_location"            );}
	void lastFw          (const QString     x) { store(x, _lastFw,          "lastFw"                  );}
	void locale          (const QString     x) { store(x, _locale,          "locale"                  );}
	void mcu             (const QString     x) { store(x, _mcu,             "mcu"                     );}
	void programmer      (const QString     x) { store(x, _programmer,      "programmer"              );}
	void sambaLocation   (const QString     x) { store(x, _sambaLocation,   "samba_location"          );}
	void sambaPort       (const QString     x) { store(x, _sambaPort,       "samba_port"              );}

	void backupDir       (const QString     x) { store(x, _backupDir,       "backupPath"              );}
	void compileServer   (const QString     x) { store(x, _compileServer,   "compilation-server"      );}
	void gePath          (const QString     x) { store(x, _gePath,          "gePath"                  );}
	void lastDir         (const QString     x) { store(x, _lastDir,         "lastDir"                 );}
	void flashDir        (const QString     x) { store(x, _flashDir,        "lastFlashDir"            );}
	void imagesDir       (const QString     x) { store(x, _imagesDir,       "lastImagesDir"           );}
	void logDir          (const QString     x) { store(x, _logDir,          "lastLogDir"              );}
	void libDir          (const QString     x) { store(x, _libDir,          "libraryPath"             );}
	void snapshotDir     (const QString     x) { store(x, _snapshotDir,     "snapshotpath"            );}

	void enableBackup    (const bool        x) { store(x, _enableBackup,    "backupEnable"            );}
	void backupOnFlash   (const bool        x) { store(x, _backupOnFlash,   "backupOnFlash"           );}
	void maximized       (const bool        x) { store(x, _maximized,       "maximized"               );}
	void jsSupport       (const bool        x) { store(x, _jsSupport,       "js_support"              );}
	void rev4aSupport    (const bool        x) { store(x, _rev4aSupport,    "rev4asupport"            );}
	void showSplash      (const bool        x) { store(x, _showSplash,      "show_splash"             );}
	void snapToClpbrd    (const bool        x) { store(x, _snapToClpbrd,    "snapshot_to_clipboard"   );}
	void autoCheckApp    (const bool        x) { store(x, _autoCheckApp,    "startup_check_companion" );}
	void autoCheckFw     (const bool        x) { store(x, _autoCheckFw,     "startup_check_fw"        );}
	void simuSW          (const bool        x) { store(x, _simuSW,          "simuSW"                  );}
	void enableWizard    (const bool        x) { store(x, _enableWizard,    "wizardEnable"            );}

	void backLight       (const int         x) { store(x, _backLight,       "backLight"               );}
	void embedSplashes   (const int         x) { store(x, _embedSplashes,   "embedded_splashes"       );}
	void fwServerFails   (const int         x) { store(x, _fwServerFails,   "fwserver"                );}
	void generalEditTab  (const int         x) { store(x, _generalEditTab,  "generalEditTab"          );}
	void iconSize        (const int         x) { store(x, _iconSize,        "icon_size"               );}
	void historySize     (const int         x) { store(x, _historySize,     "history_size"            );}
	void jsCtrl          (const int         x) { store(x, _jsCtrl,          "js_ctrl"                 );}
	void modelEditTab    (const int         x) { store(x, _modelEditTab,    "modelEditTab"            );}
	void id              (const int         x) { store(x, _id,              "profileId"               );}
	void theme           (const int         x) { store(x, _theme,           "theme"                   );}
	void warningId       (const int         x) { store(x, _warningId,       "warningId"               );}

	// Constructor
	AppData()
	{
		//Initialize the profiles
		for (int i=0; i<MAX_PROFILES; i++)
			profile[i].init( i );

		//Initialize the joysticks
		for (int i=0; i<MAX_JOYSTICKS; i++)
			joystick[i].init( i );

		// Import settings from companion9x, but only do it one time.
		// Use "location" tag as an indicator that the settings are missing
		QSettings c9x_settings("companion9x", "companion9x");
		QSettings settings(PRODUCT, COMPANY);
		if (!settings.contains("locale"))
		{
			QStringList keys = c9x_settings.allKeys();
			for (QStringList::iterator i=keys.begin(); i!=keys.end(); i++)
			{
				settings.setValue(*i, c9x_settings.value(*i));
			}
			// Convert firmware name
			QString firmware = settings.value("firmware", "").toString();
			firmware.replace("open9x","opentx");
			firmware.replace("x9da","taranis");

			// Move Companion9x settings to profile0, the new default profile
			profile[0].firmware( firmware );
			profile[0].name( settings.value(          "Name",                  ""    ).toString());
			profile[0].sdPath( settings.value(        "sdPath",                ""    ).toString());
			profile[0].splashFile( settings.value(    "SplashFileName",        ""    ).toString());
			profile[0].burnFirmware( settings.value(  "burnFirmware",          false ).toBool());
			profile[0].renameFwFiles( settings.value( "rename_firmware_files", false ).toBool());
			profile[0].patchImage( settings.value(    "patchImage",            false ).toBool());
			profile[0].channelOrder( settings.value(  "default_channel_order", "0"   ).toInt());
			profile[0].defaultMode( settings.value(   "default_mode",          "1"   ).toInt());

			// Delete unused settings
			settings.remove("firmware");
			settings.remove("Name");
			settings.remove("sdPath");
			settings.remove("SplashFileName");
			settings.remove("burnFirmware");
			settings.remove("rename_firmware_files");
			settings.remove("patchImage");
			settings.remove("default_channel_order");
			settings.remove("default_mode");
		}

		// Load and store all variables. Use default values if setting values are missing
		getset( _recentFiles,     "recentFileList"          ,"" );
		getset( _mainWinGeo,      "mainWindowGeometry"      ,"" );
		getset( _mainWinState,    "mainWindowState"         ,"" );
		getset( _modelEditGeo,    "modelEditGeometry"       ,"" );

		getset( _armMcu,          "arm_mcu"                 ,"at91sam3s4-9x" );
		getset( _avrArguments,    "avr_arguments"           ,"" );
		getset( _avrPort,         "avr_port"                ,"" );
		getset( _avrdudeLocation, "avrdudeLocation"        ,"" );
		getset( _cpuId,           "cpu_id"                  ,"" );
		getset( _dfuArguments,    "dfu_arguments"           ,"-a 0" );
		getset( _dfuLocation,     "dfu_location"            ,"" );
		getset( _lastFw,          "lastFw"                  ,"" );
		getset( _locale,          "locale"                  ,"" );
		getset( _mcu,             "mcu"                     ,"m64" );
		getset( _programmer,      "programmer"              ,"usbasp" );
		getset( _sambaLocation,   "samba_location"          ,"" );
		getset( _sambaPort,       "samba_port"              ,"\\USBserial\\COM23" );

		getset( _backupDir,       "backupPath"              ,"" );
		getset( _compileServer,   "compilation-server"      ,"" );
		getset( _gePath,          "gePath"                  ,"" );
		getset( _lastDir,         "lastDir"                 ,"" );
		getset( _flashDir,        "lastFlashDir"            ,"" );
		getset( _imagesDir,       "lastImagesDir"           ,"" );
		getset( _logDir,          "lastLogDir"              ,"" );
		getset( _libDir,          "libraryPath"             ,"" );
		getset( _snapshotDir,     "snapshotpath"            ,"" );

		getset( _enableBackup,    "backupEnable"            ,false );
		getset( _backupOnFlash,   "backupOnFlash"           ,true  );
		getset( _jsSupport,       "js_support"              ,false );
		getset( _rev4aSupport,    "rev4asupport"            ,false );
		getset( _maximized,       "maximized"               ,false );
		getset( _showSplash,      "show_splash"             ,true  );
		getset( _snapToClpbrd,    "snapshot_to_clipboard"   ,false );
		getset( _autoCheckApp,    "startup_check_companion" ,true  );
		getset( _autoCheckFw,     "startup_check_fw"        ,true  );
		getset( _simuSW,          "simuSW"                  ,false );
		getset( _enableWizard,    "wizardEnable"            ,true  );

		getset( _backLight,       "backLight"               ,0  );
		getset( _embedSplashes,   "embedded_splashes"       ,0  );
		getset( _fwServerFails,   "fwserver"                ,0  );
		getset( _generalEditTab,  "generalEditTab"          ,0  );
		getset( _iconSize,        "icon_size"               ,2  );
		getset( _jsCtrl,          "js_ctrl"                 ,0  );
		getset( _historySize,     "history_size"            ,10 );
		getset( _modelEditTab,    "modelEditTab"            ,0  );
		getset( _id,              "profileId"               ,0  );
		getset( _theme,           "theme"                   ,1  );
		getset( _warningId,       "warningId"               ,0  );
	}
};

extern AppData g;

#endif // APPDATA_H
