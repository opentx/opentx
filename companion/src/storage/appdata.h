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

// All temporary and permanent global variables are defined here to make
// initialization and storage safe and visible.
// Do not access variables in QSettings directly, it is not type safe!


#ifndef _APPDATA_H_
#define _APPDATA_H_

#include <QByteArray>
#include <QStringList>
#include <QString>
#include <QSettings>
#include <QStandardPaths>

#include "constants.h"
#include "simulator.h"

#define COMPANY            "OpenTX"
#define COMPANY_DOMAIN     "open-tx.org"
#define PRODUCT            "Companion 2.2"
#define APP_COMPANION      "OpenTX Companion"
#define APP_SIMULATOR      "OpenTX Simulator"

#define MAX_PROFILES 15
#define MAX_JOYSTICKS 8

class CompStoreObj
{
  public:
    void clear (const QString & tag1 = "", const QString &tag2 = "", const QString &tag3 = "");

    // NOTE: OBJ_T can only be a standard or registered Qt type. To use custom types,
    //   either Q_DECLARE_METATYPE() them or add additional logic in retrieve() to handle QMetaType::User.

    template <typename OBJ_T, typename TAG_T, typename GP1_T = QString, typename GP2_T = QString>
    void store(const OBJ_T newValue, OBJ_T & destValue, const TAG_T tag, const GP1_T group1 = "", const GP2_T group2 = "");

    template <typename OBJ_T, typename TAG_T, typename DEF_T, typename GP1_T = QString, typename GP2_T = QString>
    void retrieve(OBJ_T & destValue, const TAG_T tag, const DEF_T def, const GP1_T group1 = "", const GP2_T group2 = "");

    template <typename OBJ_T, typename TAG_T, typename DEF_T, typename GP1_T = QString, typename GP2_T = QString>
    void getset(OBJ_T & value, const TAG_T tag, const DEF_T def, const GP1_T group1 = "", const GP2_T group2 = "" );

    // this specialization is needed because QVariant::fromValue(const char *) fails
    template <typename OBJ_T, typename TAG_T, typename GP1_T = QString, typename GP2_T = QString>
    void getset(OBJ_T & value, const TAG_T tag, const char * def, const GP1_T group1 = "", const GP2_T group2 = "" );

};

class FwRevision: protected CompStoreObj
{
  public:
    int get( const QString);
    void set( const QString, const int );
    void remove( const QString );
};

class JStickData: protected CompStoreObj
{
  private:
    int index;

    int _stickAxe;
    int _stickMin;
    int _stickMed;
    int _stickMax;
    int _stickInv;

  public:
    // All the get definitions
    int stick_axe();
    int stick_min();
    int stick_med();
    int stick_max();
    int stick_inv();

    // All the set definitions
    void stick_axe(const int);
    void stick_min(const int);
    void stick_med(const int);
    void stick_max(const int);
    void stick_inv(const int);

    JStickData();
    void remove();
    bool existsOnDisk();
    void init(int index);
    void flush();
};

class Profile: protected CompStoreObj
{
  private:
    // Class Internal Variable
    int index;

    // Application Variables
    QString _fwName;
    QString _fwType;
    QString _name;
    QString _sdPath;
    int     _volumeGain;
    QString _pBackupDir;
    QString _splashFile;
    bool    _burnFirmware;
    bool    _penableBackup;
    bool    _renameFwFiles;
    int     _channelOrder;
    int     _defaultMode;

    // Firmware Variables
    QString _beeper;
    QString _countryCode;
    QString _display;
    QString _haptic;
    QString _speaker;
    QString _stickPotCalib;
    QString _timeStamp;
    QString _trainerCalib;
    QString _controlTypes;
    QString _controlNames;
    int     _gsStickMode;
    int     _ppmMultiplier;
    int     _vBatWarn;
    int     _vBatMin;
    int     _vBatMax;
    int     _txCurrentCalibration;
    int     _txVoltageCalibration;

    // Simulator variables
    SimulatorOptions _simulatorOptions;

  public:
    // All the get definitions
    QString fwName() const;
    QString fwType() const;
    QString name() const;
    QString sdPath() const;
    int     volumeGain() const;
    QString pBackupDir() const;
    QString splashFile() const;
    bool    burnFirmware() const;
    bool    renameFwFiles() const;
    bool    penableBackup() const;
    int     channelOrder() const;
    int     defaultMode() const;

    QString beeper() const;
    QString countryCode() const;
    QString display() const;
    QString haptic() const;
    QString speaker() const;
    QString stickPotCalib() const;
    QString timeStamp() const;
    QString trainerCalib() const;
    QString controlTypes() const;
    QString controlNames() const;
    int     txCurrentCalibration() const;
    int     gsStickMode() const;
    int     ppmMultiplier() const;
    int     txVoltageCalibration() const;
    int     vBatWarn() const;
    int     vBatMin() const;
    int     vBatMax() const;

    SimulatorOptions simulatorOptions() const;

    // All the set definitions
    void name          (const QString);
    void fwName        (const QString);
    void fwType        (const QString);
    void volumeGain    (const int);
    void sdPath        (const QString);
    void pBackupDir    (const QString);
    void splashFile    (const QString);
    void burnFirmware  (const bool);
    void renameFwFiles (const bool);
    void penableBackup (const bool);
    void channelOrder  (const int);
    void defaultMode   (const int);

    void beeper        (const QString);
    void countryCode   (const QString);
    void display       (const QString);
    void haptic        (const QString);
    void speaker       (const QString);
    void stickPotCalib (const QString);
    void timeStamp     (const QString);
    void trainerCalib  (const QString);
    void controlTypes  (const QString);
    void controlNames  (const QString);
    void txCurrentCalibration  (const int);
    void gsStickMode   (const int);
    void ppmMultiplier (const int);
    void txVoltageCalibration     (const int);
    void vBatWarn      (const int);
    void vBatMin       (const int);
    void vBatMax       (const int);

    void simulatorOptions (const SimulatorOptions &);

    Profile();
    Profile& operator=(const Profile&);
    void remove();
    bool existsOnDisk();
    void init(int newIndex);
    void initFwVariables();
    void flush();
    QString groupId();
};


#define PROPERTY4(type, name, key, dflt)                            \
    public:                                                         \
      inline type name() { return _ ## name; }                      \
      void name(const type val) { store(val, _ ## name, # key); }   \
    private:                                                        \
      void name ## _init() { getset(_ ## name, # key, dflt); }      \
      type _ ## name;

#define PROPERTY(type, name, dflt)    PROPERTY4(type, name, name, dflt)

class AppData: protected CompStoreObj
{
  PROPERTY(bool, enableBackup,               false)
  PROPERTY(bool, backupOnFlash,              true)
  PROPERTY(bool, outputDisplayDetails,       false)
  PROPERTY(bool, checkHardwareCompatibility, true)
  PROPERTY(bool, removeModelSlots,           true)
  PROPERTY(bool, maximized,   false)
  PROPERTY(bool, simuSW,      false)
  PROPERTY(bool, tabbedMdi,   false)
  PROPERTY(bool, appDebugLog, false)
  PROPERTY(bool, fwTraceLog,  false)

  PROPERTY(unsigned, OpenTxBranch, BRANCH_RELEASE_STABLE);

  PROPERTY4(bool, jsSupport,       js_support              ,false)
  PROPERTY4(bool, showSplash,      show_splash             ,true)
  PROPERTY4(bool, snapToClpbrd,    snapshot_to_clipboard   ,false)
  PROPERTY4(bool, autoCheckApp,    startup_check_companion ,true)
  PROPERTY4(bool, autoCheckFw,     startup_check_fw        ,true)

  // All the global variables
  public:
    Profile    profile[MAX_PROFILES];
    JStickData joystick[MAX_JOYSTICKS];
    FwRevision fwRev;

  private:
    QStringList _recentFiles;
    QStringList _simuDbgFilters;

    QByteArray _mainWinGeo;
    QByteArray _mainWinState;
    QByteArray _modelEditGeo;
    QByteArray _mdiWinGeo;
    QByteArray _mdiWinState;
    QByteArray _compareWinGeo;

    QString _armMcu;
    QString _avrArguments;
    QString _avrPort;
    QString _avrdudeLocation;
    QString _dfuArguments;
    QString _dfuLocation;
    QString _locale;
    QString _mcu;
    QString _programmer;
    QString _sambaLocation;
    QString _sambaPort;

    QString _backupDir;
    QString _gePath;
    QString _eepromDir;
    QString _flashDir;
    QString _imagesDir;
    QString _logDir;
    QString _libDir;
    QString _snapshotDir;
    QString _updatesDir;
    PROPERTY(QString, appLogsDir,  QString(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) % "/" COMPANY "/DebugLogs"))

    int _newModelAction;  // 0=no action; 1=model wizard; 2=model edit
    int _backLight;
    int _embedSplashes;
    int _fwServerFails;
    int _generalEditTab;
    int _iconSize;
    int _jsCtrl;
    int _historySize;
    int _id;
    int _theme;
    int _warningId;
    int _simuLastProfId;
    // currently loaded radio profile ID, NOT saved to persistent storage
    int _sessionId;

  public:
    // All the get definitions
    QStringList recentFiles();
    QStringList simuDbgFilters();

    QByteArray mainWinGeo();
    QByteArray mainWinState();
    QByteArray modelEditGeo();
    QByteArray mdiWinGeo();
    QByteArray mdiWinState();
    QByteArray compareWinGeo();

    QString armMcu();
    QString avrArguments();
    QString avrPort();
    QString avrdudeLocation();
    QString dfuArguments();
    QString dfuLocation();
    QString lastFw();
    QString locale();
    QString mcu();
    QString programmer();
    QString sambaLocation();
    QString sambaPort();

    QString backupDir();
    QString gePath();
    QString eepromDir();
    QString flashDir();
    QString imagesDir();
    QString logDir();
    QString libDir();
    QString snapshotDir();
    QString updatesDir();

    int newModelAction();
    int backLight();
    int embedSplashes();
    int fwServerFails();
    int generalEditTab();
    int iconSize();
    int historySize();
    int jsCtrl();
    int id();
    int theme();
    int warningId();
    int simuLastProfId();
    int sessionId();

    // All the set definitions
    void recentFiles     (const QStringList x);
    void simuDbgFilters  (const QStringList x);

    void mainWinGeo      (const QByteArray);
    void mainWinState    (const QByteArray);
    void modelEditGeo    (const QByteArray);
    void mdiWinGeo       (const QByteArray);
    void mdiWinState     (const QByteArray);
    void compareWinGeo   (const QByteArray);

    void armMcu          (const QString);
    void avrArguments    (const QString);
    void avrPort         (const QString);
    void avrdudeLocation (const QString);
    void dfuArguments    (const QString);
    void dfuLocation     (const QString);
    void lastFw          (const QString);
    void locale          (const QString);
    void mcu             (const QString);
    void programmer      (const QString);
    void sambaLocation   (const QString);
    void sambaPort       (const QString);

    void backupDir       (const QString);
    void gePath          (const QString);
    void eepromDir       (const QString);
    void flashDir        (const QString);
    void imagesDir       (const QString);
    void logDir          (const QString);
    void libDir          (const QString);
    void snapshotDir     (const QString);
    void updatesDir      (const QString);

    void newModelAction  (const int);
    void backLight       (const int);
    void embedSplashes   (const int);
    void fwServerFails   (const int);
    void generalEditTab  (const int);
    void iconSize        (const int);
    void historySize     (const int);
    void jsCtrl          (const int);
    void modelEditTab    (const int);
    void id              (const int);
    void theme           (const int);
    void warningId       (const int);
    void simuLastProfId  (const int);
    void sessionId       (const int);

    // Constructor
    AppData();
    void init();

    QMap<int, QString> getActiveProfiles();
    inline bool isFirstUse() const { return firstUse; }
    inline QString previousVersion() const { return upgradeFromVersion; }
    bool hasCurrentSettings();
    bool findPreviousVersionSettings(QString * version);
    bool importSettings(QString fromVersion);

    inline DownloadBranchType boundedOpenTxBranch() {
#if defined(ALLOW_NIGHTLY_BUILDS)
      return qBound(BRANCH_RELEASE_STABLE, DownloadBranchType(OpenTxBranch()), BRANCH_NIGHTLY_UNSTABLE);
#else
      return qBound(BRANCH_RELEASE_STABLE, DownloadBranchType(OpenTxBranch()), BRANCH_RC_TESTING);
#endif
    }

  protected:
    void convertSettings(QSettings & settings);

    bool firstUse;
    QString upgradeFromVersion;
};

extern AppData g;

#endif // _APPDATA_H_
