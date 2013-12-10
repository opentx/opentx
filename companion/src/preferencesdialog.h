#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include "eeprominterface.h"

namespace Ui {
    class preferencesDialog;
}

class Joystick;

class preferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit preferencesDialog(QWidget *parent = 0);
    ~preferencesDialog();
    Joystick *joystick;

private:
    Ui::preferencesDialog *ui;
    QList<QCheckBox *> optionsCheckBoxes;
    bool updateLock;

    void populateLocale();
    void populateFirmwareOptions(const FirmwareInfo *);
    FirmwareVariant getFirmwareVariant();
    void initSettings();
    QCheckBox * voice;


private slots:
    void shrink();
    void writeValues();
    void firmwareLangChanged();
    void baseFirmwareChanged();
    void firmwareOptionChanged(bool state);
    void firmwareChanged();
    void on_fw_dnld_clicked();
    void on_voice_dnld_clicked();
    void on_SplashSelect_clicked();
    void on_InvertPixels_clicked();
    void on_clearImageButton_clicked();
    void on_libraryPathButton_clicked();
    void on_snapshotPathButton_clicked();
    void on_snapshotClipboardCKB_clicked();
    void on_backupPathButton_clicked();
    void on_sdPathButton_clicked();
    void on_splashLibraryButton_clicked();
    void on_checkFWUpdates_clicked();
    void on_ProfSlot_SB_valueChanged();
    void on_ProfSave_PB_clicked();
    void on_export_PB_clicked();
    void on_import_PB_clicked();
#ifdef JOYSTICKS
    void on_joystickChkB_clicked();
    void on_joystickcalButton_clicked();
#endif
};

#endif // PREFERENCESDIALOG_H
