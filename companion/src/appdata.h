// Companion Application Data Class.
// All temporary and permanent global variables are defined here to make 
// initialization and storage safe and visible.
// Do not access variables in QSettings directly, it is not type safe!


#ifndef APPDATA_H
#define APPDATA_H
#include <QtGui>

#define COMPANY "OpenTX Companion"
#define PRODUCT "OpenTX"

class AppData
{
  private:
    // All the global variables
    QStringList _recentFileList;
    QByteArray _mainWindowGeometry;
    QByteArray _mainWindowState; 

    QString _firmware;
    QString _locale;
    QString _cpu_id;
    QString _SplashImage;
    QString _Name;
    QString _SplashFileName;
    QString _modelEditGeometry;

    QString _backupPath;
    QString _gePath;
    QString _lastDir;
    QString _lastFlashDir;
    QString _lastImagesDir;
    QString _libraryPath;
    QString _sdPath;
    QString _snapshotpath;

    bool _backupEnable;
    bool _backupOnFlash;
    bool _burnFirmware;
    bool _maximized;
    bool _js_support;
    bool _rename_firmware_files;
    bool _show_splash;
    bool _snapshot_to_clipboard;
    bool _startup_check_companion;
    bool _startup_check_fw;
    bool _simuSW;
    bool _wizardEnable;

    int _backLight;
    int _default_channel_order;
    int _default_mode;
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

    // Storage functions

    void store(const QByteArray newArray, QByteArray &array, const char *tag)
    {
      QSettings settings(PRODUCT, COMPANY);
      settings.setValue(tag, newArray);
      array = newArray;
    }

    void store(const QStringList newSList, QStringList &stringList, const char *tag)
    {
      QSettings settings(PRODUCT, COMPANY);
      settings.setValue(tag, newSList);
      stringList = newSList;
    }

    void store(const QString newString, QString &string, const char *tag)
    {
      QSettings settings(PRODUCT, COMPANY);
      settings.setValue(tag, newString);
      string = newString;
    }

    void store(const bool newTruth, bool &truth, const char *tag)
    {
      QSettings settings(PRODUCT, COMPANY);
      settings.setValue(tag, newTruth);
      truth = newTruth;
    }

    void store(const int newNumber, int &number, const char *tag)
    {
      QSettings settings(PRODUCT, COMPANY);
      settings.setValue(tag, newNumber);
      number = newNumber;
    }

    // Retrieval functions
    void retrieve( QByteArray &array, const char *tag, const char *def)
    {
        QSettings settings(PRODUCT, COMPANY);
        array = settings.value(tag, def).toByteArray();
    }
    void retrieve( QStringList &stringList, const char *tag, const char *def)
    {
        QSettings settings(PRODUCT, COMPANY);
        stringList = settings.value(tag, def).toStringList();
    }

    void retrieve( QString &string, const char *tag, const char *def)
    {
        QSettings settings(PRODUCT, COMPANY);
        string = settings.value(tag, def).toString();
    }

    void retrieve( bool &truth, const char *tag, const bool def)
    {
        QSettings settings(PRODUCT, COMPANY);
        truth = settings.value(tag, def).toBool();
    }

    void retrieve( int &number, const char *tag, const int def)
    {
        QSettings settings(PRODUCT, COMPANY);
        number = settings.value(tag, def).toInt();
    }

    // Retrieve and Store functions
    void getset( QByteArray &array, const char *tag, const char *def)
    {
        retrieve( array, tag, def);
        store(array, array, tag);
    }

    void getset( QStringList &stringList, const char *tag, const char *def)
    {
        retrieve( stringList, tag, def);
        store(stringList, stringList, tag);
    }

    void getset( QString &string, const char *tag, const char *def)
    {
        retrieve( string, tag, def);
        store(string, string, tag);
    }

    void getset( bool &truth, const char *tag, const bool def)
    {
        retrieve( truth, tag, def);
        store(truth, truth, tag);
    }

    void getset( int &number, const char *tag, const int def)
    {
        retrieve( number, tag, def);
        store(number, number, tag);
    }

public:
    // All the get declarations
    QStringList recentFileList()   { return _recentFileList;         }
    QByteArray mainWindowGeometry(){ return _mainWindowGeometry;     }
    QByteArray mainWindowState()   { return _mainWindowState;        }

    QString firmware()             { return _firmware;               }
    QString locale()               { return _locale;                 }
    QString cpu_id()               { return _cpu_id;                 }
    QString SplashImage()          { return _SplashImage;            }
    QString Name()                 { return _Name;                   }
    QString SplashFileName()       { return _SplashFileName;         }
    QString modelEditGeometry()    { return _modelEditGeometry;      }

    QString backupPath()           { return _backupPath;             }
    QString gePath()               { return _gePath;                 }
    QString lastDir()              { return _lastDir;                }
    QString lastFlashDir()         { return _lastFlashDir;           }
    QString lastImagesDir()        { return _lastImagesDir;          }
    QString libraryPath()          { return _libraryPath;            }
    QString sdPath()               { return _sdPath;                 }
    QString snapshotpath()         { return _snapshotpath;           }

    bool backupEnable()            { return _backupEnable;           }
    bool backupOnFlash()           { return _backupOnFlash;          }
    bool burnFirmware()            { return _burnFirmware;           }
    bool js_support()              { return _js_support;             }
    bool maximized()               { return _maximized;              }
    bool rename_firmware_files()   { return _rename_firmware_files;  }
    bool show_splash()             { return _show_splash;            }
    bool snapshot_to_clipboard()   { return _snapshot_to_clipboard;  }
    bool startup_check_companion() { return _startup_check_companion;}
    bool startup_check_fw()        { return _startup_check_fw;       }
    bool simuSW()                  { return _simuSW;                 }
    bool wizardEnable()            { return _wizardEnable;           }

    int backLight()                { return _backLight;              }
    int default_channel_order()    { return _default_channel_order;  }
    int default_mode()             { return _default_mode;           }
    int embedded_splashes()        { return _embedded_splashes;      }
    int fwserver()                 { return _fwserver;               }
    int generalEditTab()           { return _generalEditTab;         }
    int icon_size()                { return _icon_size;              }
    int history_size()             { return _history_size;           }
    int js_ctr()                   { return _js_ctrl;                }
    int modelEditTab()             { return _modelEditTab;           }
    int profileId()                { return _profileId;              }
    int theme()                    { return _theme;                  }
    int warningId()                { return _warningId;              }

    // All the set declarations
    void recentFileList       (const QStringList l) { store(l, _recentFileList,     "recentFileList"    );}
    void mainWindowGeometry   (const QByteArray a)  { store(a, _mainWindowGeometry, "mainWindowGeometry");}
    void mainWindowState      (const QByteArray a)  { store(a, _mainWindowState,    "mainWindowState"   );}

    void firmware             (const QString str) { store(str, _firmware,           "firmware"          );}
    void locale               (const QString str) { store(str, _locale,             "locale"            );}
    void cpu_id               (const QString str) { store(str, _cpu_id,             "cpu_id"            );}
    void SplashImage          (const QString str) { store(str, _SplashImage,        "SplashImage"       );}
    void Name                 (const QString str) { store(str, _Name,               "Name"              );}
    void SplashFileName       (const QString str) { store(str, _SplashFileName,     "SplashFileName"    );}
    void modelEditGeometry    (const QString str) { store(str, _modelEditGeometry,  "modelEditGeometry" );}

    void backupPath           (const QString str) { store(str, _backupPath,         "backupPath"        );}
    void gePath               (const QString str) { store(str, _gePath,             "gePath"            );}
    void lastDir              (const QString str) { store(str, _lastDir,            "lastDir"           );}
    void lastFlashDir         (const QString str) { store(str, _lastFlashDir,       "lastFlashDir"      );}
    void lastImagesDir        (const QString str) { store(str, _lastImagesDir,      "lastImagesDir"     );}
    void libraryPath          (const QString str) { store(str, _libraryPath,        "libraryPath"       );}
    void sdPath               (const QString str) { store(str, _sdPath,             "sdPath"            );}
    void snapshotpath         (const QString str) { store(str, _snapshotpath,       "snapshotpath"      );}

    void backupEnable            (const bool bl) { store(bl, _backupEnable,            "backupEnable"            );}
    void backupOnFlash           (const bool bl) { store(bl, _backupOnFlash,           "backupOnFlash"           );}
    void burnFirmware            (const bool bl) { store(bl, _burnFirmware,            "burnFirmware"            );}
    void maximized               (const bool bl) { store(bl, _maximized,               "maximized"               );}
    void js_support              (const bool bl) { store(bl, _js_support,              "js_support"              );}
    void rename_firmware_files   (const bool bl) { store(bl, _rename_firmware_files,   "rename_firmware_files"   );}
    void show_splash             (const bool bl) { store(bl, _show_splash,             "show_splash"             );}
    void snapshot_to_clipboard   (const bool bl) { store(bl, _snapshot_to_clipboard,   "snapshot_to_clipboard"   );}
    void startup_check_companion (const bool bl) { store(bl, _startup_check_companion, "startup_check_companion" );}
    void startup_check_fw        (const bool bl) { store(bl, _startup_check_fw,        "startup_check_fw"        );}
    void simuSW                  (const bool bl) { store(bl, _simuSW,                  "simuSW"                  );}
    void wizardEnable            (const bool bl) { store(bl, _wizardEnable,            "wizardEnable"            );}

    void backLight               (const int it) { store(it, _backLight,                 "backLight"               );}
    void default_channel_order   (const int it) { store(it, _default_channel_order,     "default_channel_order"   );}
    void default_mode            (const int it) { store(it, _default_mode,              "default_mode"            );}
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
        // Start by hijacking all left over settings from companion9x, but only do it one time
        // Use "location" tag as an indicator that the settings are missing 
        QSettings c9x_settings("companion9x", "companion9x");
        QSettings settings(PRODUCT, COMPANY);
        if (!settings.contains("locale")) {
          QStringList keys = c9x_settings.allKeys();
          for (QStringList::iterator i=keys.begin(); i!=keys.end(); i++) {
            settings.setValue(*i, c9x_settings.value(*i));
          }
        }

        // Load and store all variables. Use default values if setting values are missing

        getset( _recentFileList,          "recentFileList"          ,"" );
        getset( _mainWindowGeometry,      "mainWindowGeometry"      ,"" );
        getset( _mainWindowState,         "mainWindowState"         ,"" );
        
        getset( _firmware,                "firmware"                ,"" );
        getset( _locale,                  "locale"                  ,"" );
        getset( _cpu_id,                  "cpu_id"                  ,"" );
        getset( _SplashImage,             "SplashImage"             ,"" );
        getset( _Name,                    "Name"                    ,"" );
        getset( _SplashFileName,          "SplashFileName"          ,"" );
        getset( _modelEditGeometry,       "modelEditGeometry"       ,"" );

        getset( _backupPath,              "backupPath"              ,"" );
        getset( _gePath,                  "gePath"                  ,"" );
        getset( _lastDir,                 "lastDir"                 ,"" );
        getset( _lastFlashDir,            "lastFlashDir"            ,"" );
        getset( _lastImagesDir,           "lastImagesDir"           ,"" );
        getset( _libraryPath,             "libraryPath"             ,"" );
        getset( _sdPath,                  "sdPath"                  ,"" );
        getset( _snapshotpath,            "snapshotpath"            ,"" );

        getset( _backupEnable,            "backupEnable"            ,false );
        getset( _backupOnFlash,           "backupOnFlash"           ,true  );
        getset( _burnFirmware,            "burnFirmware"            ,false );
        getset( _js_support,              "js_support"              ,false );
        getset( _maximized,               "maximized"               ,false );
        getset( _rename_firmware_files,   "rename_firmware_files"   ,false );
        getset( _show_splash,             "show_splash"             ,true  );
        getset( _snapshot_to_clipboard,   "snapshot_to_clipboard"   ,false );
        getset( _startup_check_companion, "startup_check_companion" ,true  );
        getset( _startup_check_fw,        "startup_check_fw"        ,true  );
        getset( _simuSW,                  "simuSW"                  ,false );
        getset( _wizardEnable,            "wizardEnable"            ,true  );

        getset( _backLight,               "backLight"               ,0 );
        getset( _default_channel_order,   "default_channel_order"   ,0 );
        getset( _default_mode,            "default_mode"            ,1 );
        getset( _embedded_splashes,       "embedded_splashes"       ,0 );
        getset( _fwserver,                "fwserver"                ,0 );
        getset( _generalEditTab,          "generalEditTab"          ,0 );
        getset( _icon_size,               "icon_size"               ,2 );
        getset( _js_ctrl,                 "js_ctrl"                 ,0 ); 
        getset( _history_size,            "history_size"            ,6 );
        getset( _modelEditTab,            "modelEditTab"            ,0 );
        getset( _profileId,               "profileId"               ,0 );
        getset( _theme,                   "theme"                   ,1 );
        getset( _warningId,               "warningId"               ,0 );
    }
 };

extern AppData glob;

#endif // APPDATA_H
