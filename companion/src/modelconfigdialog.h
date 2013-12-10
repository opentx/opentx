#ifndef MODELCONFIGDIALOG_H
#define MODELCONFIGDIALOG_H
#include "eeprominterface.h"
#include <QDialog>
#include <QtGui>


namespace Ui {
    class modelConfigDialog;
}

class modelConfigDialog : public QDialog {
    Q_OBJECT
public:
    modelConfigDialog(RadioData &radioData, uint64_t * result, QWidget *parent = 0);
    ~modelConfigDialog();

private:
    Ui::modelConfigDialog *ui;
    bool wingsLock;
    bool tailLock;
    bool rxLock;
    int ModelType;
    RadioData &radioData;
    uint64_t * result;
    GeneralSettings g_eeGeneral;
    QStringList ruddercolor;
    QStringList aileroncolor;
    QStringList elevatorcolor;
    QStringList throttlecolor;
    QStringList flapscolor;
    QStringList airbrakecolor;
    QStringList helicolor;
    bool rx[10];
    unsigned int ailerons;
    unsigned int rudders;
    unsigned int throttle;
    unsigned int elevators;
    unsigned int spoilers;
    unsigned int flaps;
    


private slots:
    void ConfigChanged();
    void tailConfigChanged();
    void rxUpdate();
    void resetControls();
    void on_planeButton_clicked();
    void on_heliButton_clicked();
    void on_gliderButton_clicked();
    void on_deltaButton_clicked();
    void formSetup();
    void asUpdate();
    void shrink();
    void doAction(QAbstractButton *button);
};

#endif // MODELCONFIGDIALOG_H
