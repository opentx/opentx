#ifndef FWPREFERENCESDIALOG_H
#define FWPREFERENCESDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include "eeprominterface.h"

namespace Ui {
    class fwPreferencesDialog;
}

class Joystick;

class fwPreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit fwPreferencesDialog(QWidget *parent = 0);
    ~fwPreferencesDialog();

private:
    Ui::fwPreferencesDialog *ui;
    QList<QCheckBox *> optionsCheckBoxes;
    bool updateLock;

    void showVoice(bool);
    void showVoice();
    void hideVoice();
    void populateLocale();
    void populateFirmwareOptions(const FirmwareInterface *);
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
    void on_checkFWUpdates_clicked();
};

#endif // FWPREFERENCESDIALOG_H
