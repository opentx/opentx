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
    void initSettings();

private slots:
    void on_fw_dnld_clicked();
    void on_voice_dnld_clicked();
    void on_checkFWUpdates_clicked();
};

#endif // FWPREFERENCESDIALOG_H
