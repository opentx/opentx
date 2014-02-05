#ifndef APPPREFERENCESDIALOG_H
#define APPPREFERENCESDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include "eeprominterface.h"

namespace Ui {
    class appPreferencesDialog;
}

class Joystick;

class appPreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit appPreferencesDialog(QWidget *parent = 0);
    ~appPreferencesDialog();
    Joystick *joystick;

private:
    Ui::appPreferencesDialog *ui;
    void initSettings();

private slots:
    void writeValues();
    void on_snapshotPathButton_clicked();
    void on_snapshotClipboardCKB_clicked();
    void on_backupPathButton_clicked();
    void on_ge_pathButton_clicked();
#ifdef JOYSTICKS
    void on_joystickChkB_clicked();
    void on_joystickcalButton_clicked();
#endif
};

#endif // APPPREFERENCESDIALOG_H
