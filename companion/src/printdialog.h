#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QDir>
#include "eeprominterface.h"

namespace Ui {
    class printDialog;
}

class printDialog : public QDialog
{
    Q_OBJECT

public:
    explicit printDialog(QWidget *parent = 0, GeneralSettings *gg = 0, ModelData *gm = 0, QString filename="");
    
    ~printDialog();
    void  closeEvent(QCloseEvent *event);
    
    ModelData *g_model;
    GeneralSettings *g_eeGeneral;
    EEPROMInterface *eepromInterface;
    QString printfilename;

private:
    Ui::printDialog *ui;

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
