#ifndef APPPREFERENCESDIALOG_H
#define APPPREFERENCESDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include "eeprominterface.h"

namespace Ui {
    class appPreferencesDialog;
}

class appPreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit appPreferencesDialog(QWidget *parent = 0);
    ~appPreferencesDialog();

private:
    Ui::appPreferencesDialog *ui;
    void initSettings();

private slots:
    void writeValues();
    void on_backupPathButton_clicked();
    void on_ge_pathButton_clicked();
};

#endif // APPPREFERENCESDIALOG_H
