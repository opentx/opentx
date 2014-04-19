#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QDir>
#include "eeprominterface.h"

namespace Ui {
  class PrintDialog;
}

class PrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintDialog(QWidget *parent, FirmwareInterface * firmware, GeneralSettings *gg, ModelData *gm, QString filename="");
    
    ~PrintDialog();
    void  closeEvent(QCloseEvent *event);
    
    FirmwareInterface * firmware;
    GeneralSettings * g_eeGeneral;
    ModelData * g_model;

    QString printfilename;

private:
    Ui::PrintDialog *ui;

    void printSetup();
    QString printPhases();
    void printExpo();
    void printMixes();
    void printLimits();
    void printCurves();
    void printGvars();
    void printSwitches();
    void printSafetySwitches();
    void printFSwitches();
    void printFrSky();
    void printToFile();
    
    QString fv(const QString name, const QString value);
    
    QTextEdit * te;
    QString curvefile5;
    QString curvefile9;
    QDir *qd;
    
private slots:
    void on_printButton_clicked();
    void on_printFileButton_clicked();
    void autoClose();
};

#endif // PRINTDIALOG_H
