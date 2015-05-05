#ifndef APPPREFERENCESDIALOG_H
#define APPPREFERENCESDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include "eeprominterface.h"

namespace Ui {
  class AppPreferencesDialog;
}

class Joystick;

class AppPreferencesDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit AppPreferencesDialog(QWidget *parent = 0);
    ~AppPreferencesDialog();
    Joystick *joystick;

  private:
    QList<QCheckBox *> optionsCheckBoxes;
    bool updateLock;
    void showVoice(bool);
    void showVoice();
    void hideVoice();
    void populateLocale();
    void populateFirmwareOptions(const Firmware *);
    Firmware * getFirmwareVariant();
    QCheckBox * voice;

    Ui::AppPreferencesDialog *ui;
    void initSettings();
    bool displayImage( QString fileName );
    void loadProfileString(QString profile, QString label);
    void loadFromProfile();

  protected slots:
    void shrink();  
    void baseFirmwareChanged();
    void firmwareOptionChanged(bool state);

    void writeValues();
    void on_libraryPathButton_clicked();
    void on_snapshotPathButton_clicked();
    void on_snapshotClipboardCKB_clicked();
    void on_backupPathButton_clicked();
    void on_ProfilebackupPathButton_clicked();
    void on_ge_pathButton_clicked();
    
    void on_sdPathButton_clicked();
    void on_removeProfileButton_clicked();
    void on_SplashSelect_clicked();
    void on_clearImageButton_clicked();

#ifdef JOYSTICKS
    void on_joystickChkB_clicked();
    void on_joystickcalButton_clicked();
#endif
};

#endif // APPPREFERENCESDIALOG_H
