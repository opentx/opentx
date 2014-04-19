#ifndef FWPREFERENCESDIALOG_H
#define FWPREFERENCESDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include "eeprominterface.h"

namespace Ui {
  class FirmwarePreferencesDialog;
}

class Joystick;

class FirmwarePreferencesDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit FirmwarePreferencesDialog(QWidget *parent = 0);
    ~FirmwarePreferencesDialog();

  private:
    Ui::FirmwarePreferencesDialog *ui;
    void initSettings();

  private slots:
    void on_fw_dnld_clicked();
    void on_voice_dnld_clicked();
    void on_checkFWUpdates_clicked();
};

#endif // FWPREFERENCESDIALOG_H
