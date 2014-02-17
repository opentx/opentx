// Companion Application Data Class.
// All temporary and permanent global variables are defined here to make 
// initialization and storage safe and visible.
// Do not access variables in QSettings directly, it is not type safe!


#ifndef APPDATA_H
#define APPDATA_H
#include <QtGui>

#define COMPANY "OpenTX Companion"
#define PRODUCT "OpenTX"
#define MAX_PROFILES 15

class DataObj
{
  public:
    void store(const QByteArray newArray, QByteArray &array, const char *tag, const QString group1="", const QString group2="" )
    {
      QSettings settings(PRODUCT, COMPANY);
      if (!group1.isEmpty()) settings.beginGroup(group1);
      if (!group2.isEmpty()) settings.beginGroup(group2);

      settings.setValue(tag, newArray);
      array = newArray;

      if (!group1.isEmpty()) settings.endGroup();
      if (!group2.isEmpty()) settings.endGroup();
    }

    void store(const QStringList newSList, QStringList &stringList, const char *tag, const QString group1="", const QString group2="" )
    {
      QSettings settings(PRODUCT, COMPANY);
      if (!group1.isEmpty()) settings.beginGroup(group1);
      if (!group2.isEmpty()) settings.beginGroup(group2);

      settings.setValue(tag, newSList);
      stringList = newSList;

      if (!group1.isEmpty()) settings.endGroup();
      if (!group2.isEmpty()) settings.endGroup();
    }

    void store(const QString newString, QString &string, const char *tag, const QString group1="", const QString group2="" )
    {
      QSettings settings(PRODUCT, COMPANY);
      if (!group1.isEmpty()) settings.beginGroup(group1);
      if (!group2.isEmpty()) settings.beginGroup(group2);

      settings.setValue(tag, newString);
      string = newString;

      if (!group1.isEmpty()) settings.endGroup();
      if (!group2.isEmpty()) settings.endGroup();
    }

    void store(const bool newTruth, bool &truth, const char *tag, const QString group1="", const QString group2="" )
    {
      QSettings settings(PRODUCT, COMPANY);
      if (!group1.isEmpty()) settings.beginGroup(group1);
      if (!group2.isEmpty()) settings.beginGroup(group2);

      settings.setValue(tag, newTruth);
      truth = newTruth;

      if (!group1.isEmpty()) settings.endGroup();
      if (!group2.isEmpty()) settings.endGroup();
    }

    void store(const int newNumber, int &number, const char *tag, const QString group1="", const QString group2="" )
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
    void retrieve( QByteArray &array, const char *tag, const char *def, const QString group1="", const QString group2="" )
    {
      QSettings settings(PRODUCT, COMPANY);
      if (!group1.isEmpty()) settings.beginGroup(group1);
      if (!group2.isEmpty()) settings.beginGroup(group2);

      array = settings.value(tag, def).toByteArray();

      if (!group1.isEmpty()) settings.endGroup();
      if (!group2.isEmpty()) settings.endGroup();
    }

    void retrieve( QStringList &stringList, const char *tag, const char *def, const QString group1="", const QString group2="" )
    {
      QSettings settings(PRODUCT, COMPANY);
      if (!group1.isEmpty()) settings.beginGroup(group1);
      if (!group2.isEmpty()) settings.beginGroup(group2);

      stringList = settings.value(tag, def).toStringList();

      if (!group1.isEmpty()) settings.endGroup();
      if (!group2.isEmpty()) settings.endGroup();
    }

    void retrieve( QString &string, const char *tag, const char *def, const QString group1="", const QString group2="" )
    {
      QSettings settings(PRODUCT, COMPANY);
      if (!group1.isEmpty()) settings.beginGroup(group1);
      if (!group2.isEmpty()) settings.beginGroup(group2);

      string = settings.value(tag, def).toString();

      if (!group1.isEmpty()) settings.endGroup();
      if (!group2.isEmpty()) settings.endGroup();
    }

    void retrieve( bool &truth, const char *tag, const bool def, const QString group1="", const QString group2="" )
    {
      QSettings settings(PRODUCT, COMPANY);
      if (!group1.isEmpty()) settings.beginGroup(group1);
      if (!group2.isEmpty()) settings.beginGroup(group2);

      truth = settings.value(tag, def).toBool();

      if (!group1.isEmpty()) settings.endGroup();
      if (!group2.isEmpty()) settings.endGroup();
    }

    void retrieve( int &number, const char *tag, const int def, const QString group1="", const QString group2="" )
    {
      QSettings settings(PRODUCT, COMPANY);
      if (!group1.isEmpty()) settings.beginGroup(group1);
      if (!group2.isEmpty()) settings.beginGroup(group2);

      number = settings.value(tag, def).toInt();

      if (!group1.isEmpty()) settings.endGroup();
      if (!group2.isEmpty()) settings.endGroup();
    }

    // Retrieve and Store functions
    void getset( QByteArray &array, const char *tag, const char *def, const QString group1="", const QString group2="" )
    {
        retrieve( array, tag, def, group1, group2);
        store(array, array, tag, group1, group2);
    }

    void getset( QStringList &stringList, const char *tag, const char *def, const QString group1="", const QString group2="" )
    {
        retrieve( stringList, tag, def, group1, group2);
        store(stringList, stringList, tag, group1, group2);
    }

    void getset( QString &string, const char *tag, const char *def, const QString group1="", const QString group2="" )
    {
        retrieve( string, tag, def, group1, group2);
        store(string, string, tag, group1, group2);
    }

    void getset( bool &truth, const char *tag, const bool def, const QString group1="", const QString group2="" )
    {
        retrieve( truth, tag, def, group1, group2);
        store(truth, truth, tag, group1, group2);
    }

    void getset( int &number, const char *tag, const int def, const QString group1="", const QString group2="" )
    {
        retrieve( number, tag, def, group1, group2);
        store(number, number, tag, group1, group2);
    }  
}; 

class Profile:DataObj
{

  private:
    // Class Internal Variable
    int index;

    // Application Variables
    QString _firmware;
    QString _Name;
    QString _sdPath;
    QString _SplashFileName;
    bool    _burnFirmware;
    bool    _rename_firmware_files;
    bool    _patchImage;
    int     _default_channel_order;
    int     _default_mode;

    // Firmware Variables
    QString _Beeper;
    QString _countryCode;    
    QString _currentCalib;
    QString _Display;
    QString _GSStickMode;
    QString _Haptic;
    QString _PPM_Multiplier;
    QString _Speaker;
    QString _StickPotCalib;
    QString _TrainerCalib;
    QString _VbatCalib;
    QString _vBatWarn;

  public:
    // All the get declarations
    QString firmware()             { return _firmware;               }
    QString Name()                 { return _Name;                   }
    QString sdPath()               { return _sdPath;                 }
    QString SplashFileName()       { return _SplashFileName;         }
    bool burnFirmware()            { return _burnFirmware;           }
    bool rename_firmware_files()   { return _rename_firmware_files;  }
    bool patchImage()              { return _patchImage;             }
    int default_channel_order()    { return _default_channel_order;  }
    int default_mode()             { return _default_mode;           }

    QString Beeper()               { return _Beeper;                 }
    QString countryCode()          { return _countryCode;            }    
    QString currentCalib()         { return _currentCalib;           }
    QString Display()              { return _Display;                }
    QString GSStickMode()          { return _GSStickMode;            }
    QString Haptic()               { return _Haptic;                 }
    QString PPM_Multiplier()       { return _PPM_Multiplier;         }
    QString Speaker()              { return _Speaker;                }
    QString StickPotCalib()        { return _StickPotCalib;          }
    QString TrainerCalib()         { return _TrainerCalib;           }
    QString VbatCalib()            { return _VbatCalib;              }
    QString vBatWarn()             { return _vBatWarn;               }


    // All the set declarations
    void Name                    (const QString str) { if (str.isEmpty())  // Name may never be empty!
                                                         store("----", _Name,        "Name"                  ,"Profiles", QString("profile%1").arg(index));
                                                       else
                                                         store(str,    _Name,        "Name"                  ,"Profiles", QString("profile%1").arg(index));}

    void firmware                (const QString str) { store(str, _firmware,         "firmware"              ,"Profiles", QString("profile%1").arg(index));}
    void sdPath                  (const QString str) { store(str, _sdPath,           "sdPath"                ,"Profiles", QString("profile%1").arg(index));}
    void SplashFileName          (const QString str) { store(str, _SplashFileName,   "SplashFileName"        ,"Profiles", QString("profile%1").arg(index));}
    void burnFirmware            (const bool bl) { store(bl, _burnFirmware,          "burnFirmware"          ,"Profiles", QString("profile%1").arg(index));}
    void rename_firmware_files   (const bool bl) { store(bl, _rename_firmware_files, "rename_firmware_files" ,"Profiles", QString("profile%1").arg(index));}
    void patchImage              (const bool bl) { store(bl, _patchImage,            "patchImage"            ,"Profiles", QString("profile%1").arg(index));}
    void default_channel_order   (const int it)  { store(it, _default_channel_order, "default_channel_order" ,"Profiles", QString("profile%1").arg(index));}
    void default_mode            (const int it)  { store(it, _default_mode,          "default_mode"          ,"Profiles", QString("profile%1").arg(index));}

    void Beeper                  (const QString str) { store(str, _Beeper,           "Beeper"                ,"Profiles", QString("profile%1").arg(index));}
    void countryCode             (const QString str) { store(str, _countryCode,      "countryCode"           ,"Profiles", QString("profile%1").arg(index));}
    void currentCalib            (const QString str) { store(str, _currentCalib,     "currentCalib"          ,"Profiles", QString("profile%1").arg(index));}
    void Display                 (const QString str) { store(str, _Display,          "Display"               ,"Profiles", QString("profile%1").arg(index));}
    void GSStickMode             (const QString str) { store(str, _GSStickMode,      "GSStickMode"           ,"Profiles", QString("profile%1").arg(index));}
    void Haptic                  (const QString str) { store(str, _Haptic,           "Haptic"                ,"Profiles", QString("profile%1").arg(index));}
    void PPM_Multiplier          (const QString str) { store(str, _PPM_Multiplier,   "PPM_Multiplier"        ,"Profiles", QString("profile%1").arg(index));}
    void Speaker                 (const QString str) { store(str, _Speaker,          "Speaker"               ,"Profiles", QString("profile%1").arg(index));}
    void StickPotCalib           (const QString str) { store(str, _StickPotCalib,    "StickPotCalib"         ,"Profiles", QString("profile%1").arg(index));}
    void TrainerCalib            (const QString str) { store(str, _TrainerCalib,     "TrainerCalib"          ,"Profiles", QString("profile%1").arg(index));}
    void VbatCalib               (const QString str) { store(str, _VbatCalib,        "VbatCalib"             ,"Profiles", QString("profile%1").arg(index));}
    void vBatWarn                (const QString str) { store(str, _vBatWarn,         "vBatWarn"              ,"Profiles", QString("profile%1").arg(index));}

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

    void flush()
    {
        // Load and store all variables. Use default values if setting values are missing
        getset( _firmware,                "firmware"                ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _Name,                    "Name"                    ,"----"  ,"Profiles", QString("profile%1").arg(index));
        getset( _sdPath,                  "sdPath"                  ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _SplashFileName,          "SplashFileName"          ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _burnFirmware,            "burnFirmware"            ,false   ,"Profiles", QString("profile%1").arg(index));
        getset( _rename_firmware_files,   "rename_firmware_files"   ,false   ,"Profiles", QString("profile%1").arg(index));
        getset( _patchImage,              "patchImage"              ,false   ,"Profiles", QString("profile%1").arg(index));
        getset( _default_channel_order,   "default_channel_order"   ,0       ,"Profiles", QString("profile%1").arg(index));
        getset( _default_mode,            "default_mode"            ,1       ,"Profiles", QString("profile%1").arg(index));

        getset( _Beeper,                  "Beeper"                  ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _countryCode,             "countryCode"             ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _currentCalib,            "currentCalib"            ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _Display,                 "Display"                 ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _GSStickMode,             "GSStickMode"             ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _Haptic,                  "Haptic"                  ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _PPM_Multiplier,          "PPM_Multiplier"          ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _Speaker,                 "Speaker"                 ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _StickPotCalib,           "StickPotCalib"           ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _TrainerCalib,            "TrainerCalib"            ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _VbatCalib,               "VbatCalib"               ,""      ,"Profiles", QString("profile%1").arg(index));
        getset( _vBatWarn,                "vBatWarn"                ,""      ,"Profiles", QString("profile%1").arg(index));     
    }

    void init(int newIndex)
    {
        index = newIndex;

        _firmware =              "";
        _Name =                  "";
        _sdPath =                "";
        _SplashFileName =        "";
        _burnFirmware =          false;
        _rename_firmware_files = false;
        _patchImage =            false;
        _default_channel_order = 0;
        _default_mode =          1;

        _Beeper =                "";
        _countryCode =           "";
        _currentCalib =          "";
        _Display =               "";
        _GSStickMode =           "";
        _Haptic =                "";
        _PPM_Multiplier =        "";
        _Speaker =               "";
        _StickPotCalib =         "";
        _TrainerCalib =          "";
        _VbatCalib =             "";
        _vBatWarn =              "";
        
        QString pName;
        retrieve( pName, "Name", "", "Profiles", QString("profile%1").arg(index));

        // Do not write empty profiles to disk except the default (0) profile.
        if ( index > 0 && !existsOnDisk())
          return;

        flush();
    }
};

class AppData:DataObj
{
    // All the global variables
  public:
    Profile pro[MAX_PROFILES];

   private:
    QStringList _recentFileList;
    QByteArray _mainWindowGeometry;
    QByteArray _mainWindowState; 
    QByteArray _modelEditGeometry;

    QString _locale;
    QString _cpu_id;
    QString _lastFw;

    QString _backupPath;
    QString _gePath;
    QString _lastDir;
    QString _lastFlashDir;
    QString _lastImagesDir;
    QString _lastLogDir;
    QString _libraryPath;
    QString _snapshotpath;

    bool _backupEnable;
    bool _backupOnFlash;
    bool _maximized;
    bool _js_support;
    bool _rev4asupport;
    bool _show_splash;
    bool _snapshot_to_clipboard;
    bool _startup_check_companion;
    bool _startup_check_fw;
    bool _simuSW;
    bool _wizardEnable;

    int _backLight;
    int _embedded_splashes;  // Shouldn't this be bool ??
    int _fwserver;
    int _generalEditTab;
    int _icon_size;
    int _js_ctrl;
    int _history_size;
    int _modelEditTab;
    int _profileId;
    int _theme;
    int _warningId;


public:
    // All the get declarations
    QStringList recentFileList()   { return _recentFileList;         }
    QByteArray mainWindowGeometry(){ return _mainWindowGeometry;     }
    QByteArray mainWindowState()   { return _mainWindowState;        }
    QByteArray modelEditGeometry() { return _modelEditGeometry;      }

    QString locale()               { return _locale;                 }
    QString cpu_id()               { return _cpu_id;                 }
    QString lastFw()               { return _lastFw;                 }

    QString backupPath()           { return _backupPath;             }
    QString gePath()               { return _gePath;                 }
    QString lastDir()              { return _lastDir;                }
    QString lastFlashDir()         { return _lastFlashDir;           }
    QString lastImagesDir()        { return _lastImagesDir;          }
    QString lastLogDir()           { return _lastLogDir;             }
    QString libraryPath()          { return _libraryPath;            }
    QString snapshotpath()         { return _snapshotpath;           }

    bool backupEnable()            { return _backupEnable;           }
    bool backupOnFlash()           { return _backupOnFlash;          }
    bool js_support()              { return _js_support;             }
    bool rev4asupport()            { return _rev4asupport;           }
    bool maximized()               { return _maximized;              }
    bool show_splash()             { return _show_splash;            }
    bool snapshot_to_clipboard()   { return _snapshot_to_clipboard;  }
    bool startup_check_companion() { return _startup_check_companion;}
    bool startup_check_fw()        { return _startup_check_fw;       }
    bool simuSW()                  { return _simuSW;                 }
    bool wizardEnable()            { return _wizardEnable;           }

    int backLight()                { return _backLight;              }
    int embedded_splashes()        { return _embedded_splashes;      }
    int fwserver()                 { return _fwserver;               }
    int generalEditTab()           { return _generalEditTab;         }
    int icon_size()                { return _icon_size;              }
    int history_size()             { return _history_size;           }
    int js_ctrl()                  { return _js_ctrl;                }
    int modelEditTab()             { return _modelEditTab;           }
    int profileId()                { return _profileId;              }
    int theme()                    { return _theme;                  }
    int warningId()                { return _warningId;              }

// All the set declarations
    void recentFileList       (const QStringList l) { store(l, _recentFileList,     "recentFileList"    );}
    void mainWindowGeometry   (const QByteArray a)  { store(a, _mainWindowGeometry, "mainWindowGeometry");}
    void mainWindowState      (const QByteArray a)  { store(a, _mainWindowState,    "mainWindowState"   );}
    void modelEditGeometry    (const QByteArray a)  { store(a, _modelEditGeometry,  "modelEditGeometry" );}

    void locale               (const QString str) { store(str, _locale,             "locale"            );}
    void cpu_id               (const QString str) { store(str, _cpu_id,             "cpu_id"            );}
    void lastFw               (const QString str) { store(str, _lastFw,             "lastFw"            );}

    void backupPath           (const QString str) { store(str, _backupPath,         "backupPath"        );}
    void gePath               (const QString str) { store(str, _gePath,             "gePath"            );}
    void lastDir              (const QString str) { store(str, _lastDir,            "lastDir"           );}
    void lastFlashDir         (const QString str) { store(str, _lastFlashDir,       "lastFlashDir"      );}
    void lastImagesDir        (const QString str) { store(str, _lastImagesDir,      "lastImagesDir"     );}
    void lastLogDir           (const QString str) { store(str, _lastLogDir,         "lastLogDir"        );}
    void libraryPath          (const QString str) { store(str, _libraryPath,        "libraryPath"       );}
    void snapshotpath         (const QString str) { store(str, _snapshotpath,       "snapshotpath"      );}

    void backupEnable            (const bool bl) { store(bl, _backupEnable,            "backupEnable"            );}
    void backupOnFlash           (const bool bl) { store(bl, _backupOnFlash,           "backupOnFlash"           );}
    void maximized               (const bool bl) { store(bl, _maximized,               "maximized"               );}
    void js_support              (const bool bl) { store(bl, _js_support,              "js_support"              );}
    void rev4asupport            (const bool bl) { store(bl, _rev4asupport,            "rev4asupport"            );}
    void show_splash             (const bool bl) { store(bl, _show_splash,             "show_splash"             );}
    void snapshot_to_clipboard   (const bool bl) { store(bl, _snapshot_to_clipboard,   "snapshot_to_clipboard"   );}
    void startup_check_companion (const bool bl) { store(bl, _startup_check_companion, "startup_check_companion" );}
    void startup_check_fw        (const bool bl) { store(bl, _startup_check_fw,        "startup_check_fw"        );}
    void simuSW                  (const bool bl) { store(bl, _simuSW,                  "simuSW"                  );}
    void wizardEnable            (const bool bl) { store(bl, _wizardEnable,            "wizardEnable"            );}

    void backLight               (const int it) { store(it, _backLight,                 "backLight"               );}
    void embedded_splashes       (const int it) { store(it, _embedded_splashes,         "embedded_splashes"       );}
    void fwserver                (const int it) { store(it, _fwserver,                  "fwserver"                );}
    void generalEditTab          (const int it) { store(it, _generalEditTab,            "generalEditTab"          );}
    void icon_size               (const int it) { store(it, _icon_size,                 "icon_size"               );}
    void history_size            (const int it) { store(it, _history_size,              "history_size"            );}
    void js_ctrl                 (const int it) { store(it, _js_ctrl,                   "js_ctrl"                 );}
    void modelEditTab            (const int it) { store(it, _modelEditTab,              "modelEditTab"            );}
    void profileId               (const int it) { store(it, _profileId,                 "profileId"               );}
    void theme                   (const int it) { store(it, _theme,                     "theme"                   );}
    void warningId               (const int it) { store(it, _warningId,                 "warningId"               );}

    // Constructor
    AppData()
    {
        //Initialize the profiles
        for (int i=0; i<MAX_PROFILES; i++)
          pro[i].init( i );

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
          pro[0].firmware(firmware);
          pro[0].Name(settings.value(                 "Name",                     ""    ).toString());
          pro[0].sdPath(settings.value(               "sdPath",                   ""    ).toString());
          pro[0].SplashFileName(settings.value(       "SplashFileName",           ""    ).toString());
          pro[0].burnFirmware(settings.value(         "burnFirmware",             false ).toBool());
          pro[0].rename_firmware_files(settings.value("rename_firmware_files",    false ).toBool());
          pro[0].patchImage(settings.value(           "patchImage",               false ).toBool());
          pro[0].default_channel_order(settings.value("default_channel_order",    "0"   ).toInt());
          pro[0].default_mode(settings.value(         "default_mode",             "1"   ).toInt());

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

        getset( _recentFileList,          "recentFileList"          ,"" );
        getset( _mainWindowGeometry,      "mainWindowGeometry"      ,"" );
        getset( _mainWindowState,         "mainWindowState"         ,"" );
        getset( _modelEditGeometry,       "modelEditGeometry"       ,"" );
        
        getset( _locale,                  "locale"                  ,"" );
        getset( _cpu_id,                  "cpu_id"                  ,"" );
        getset( _lastFw,                  "lastFw"                  ,"" );

        getset( _backupPath,              "backupPath"              ,"" );
        getset( _gePath,                  "gePath"                  ,"" );
        getset( _lastDir,                 "lastDir"                 ,"" );
        getset( _lastFlashDir,            "lastFlashDir"            ,"" );
        getset( _lastImagesDir,           "lastImagesDir"           ,"" );
        getset( _lastLogDir,              "lastLogDir"              ,"" );
        getset( _libraryPath,             "libraryPath"             ,"" );
        getset( _snapshotpath,            "snapshotpath"            ,"" );

        getset( _backupEnable,            "backupEnable"            ,false );
        getset( _backupOnFlash,           "backupOnFlash"           ,true  );
        getset( _js_support,              "js_support"              ,false );
        getset( _rev4asupport,            "rev4asupport"            ,false );
        getset( _maximized,               "maximized"               ,false );
        getset( _show_splash,             "show_splash"             ,true  );
        getset( _snapshot_to_clipboard,   "snapshot_to_clipboard"   ,false );
        getset( _startup_check_companion, "startup_check_companion" ,true  );
        getset( _startup_check_fw,        "startup_check_fw"        ,true  );
        getset( _simuSW,                  "simuSW"                  ,false );
        getset( _wizardEnable,            "wizardEnable"            ,true  );

        getset( _backLight,               "backLight"               ,0  );
        getset( _embedded_splashes,       "embedded_splashes"       ,0  );
        getset( _fwserver,                "fwserver"                ,0  );
        getset( _generalEditTab,          "generalEditTab"          ,0  );
        getset( _icon_size,               "icon_size"               ,2  );
        getset( _js_ctrl,                 "js_ctrl"                 ,0  ); 
        getset( _history_size,            "history_size"            ,10 );
        getset( _modelEditTab,            "modelEditTab"            ,0  );
        getset( _profileId,               "profileId"               ,0  );
        getset( _theme,                   "theme"                   ,1  ); 
        getset( _warningId,               "warningId"               ,0  );
    }
 };

extern AppData glob;

#endif // APPDATA_H
