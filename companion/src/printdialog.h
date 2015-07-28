#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QDir>
#include "eeprominterface.h"
#include "modelprinter.h"

namespace Ui {
  class PrintDialog;
}

class PrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintDialog(QWidget *parent, Firmware * firmware, GeneralSettings *gg, ModelData *gm, QString filename="");
    
    ~PrintDialog();
    void  closeEvent(QCloseEvent *event);
    
    Firmware * firmware;
    GeneralSettings * g_eeGeneral;
    ModelData * g_model;

    QString printfilename;

private:
    Ui::PrintDialog *ui;
    unsigned int gvars;

    void printSetup();
    QString printFlightModes();
    void printInputs();
    void printMixes();
    void printLimits();
    void printCurves();
    void printGvars();
    void printLogicalSwitches();
    void printSafetySwitches();
    void printCustomFunctions();
    void printFrSky();
    void printToFile();
    
    QTextEdit * te;
    QString curvefile5;
    QString curvefile9;
    QDir *qd;

    ModelPrinter modelPrinter;
    
private slots:
    void on_printButton_clicked();
    void on_printFileButton_clicked();
    void autoClose();
};

#endif // PRINTDIALOG_H
