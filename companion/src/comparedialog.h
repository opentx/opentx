#ifndef COMPAREDIALOG_H
#define COMPAREDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QDir>
#include "eeprominterface.h"

namespace Ui {
    class compareDialog;
}

class compareDialog : public QDialog
{
    Q_OBJECT

public:
    explicit compareDialog(QWidget *parent = 0, GeneralSettings *gg = 0);
    
    ~compareDialog();
    void  closeEvent(QCloseEvent *event);
    
    ModelData * g_model1;
    ModelData * g_model2;
    ModelData * modeltemp;
    int model1;
    int model2;
    GeneralSettings *g_eeGeneral;
    EEPROMInterface *eepromInterface;
  
private:
    Ui::compareDialog *ui;
    QString doTC(const QString s, const QString color, bool bold);
    QString doTR(const QString s, const QString color, bool bold);
    QString doTL(const QString s, const QString color, bool bold);
    QString fv(const QString name, const QString value,const QString color);
    template<class T> QString getColor1(T val1, T val2) {
      if (val1!=val2)
        return "green";
      else
        return "grey";
    }
    template<class T> QString getColor2(T val1, T val2) {
      if (val1!=val2)
        return "red";
      else
        return "grey";
    }
    QString getColor1(char * val1, char * val2) {
      if (strcmp(val1,val2)!=0)
        return "green";
      else
        return "grey";
    }
    QString getColor2(char * val1, char * val2) {
      if (strcmp(val1,val2)!=0)
        return "red";
      else
        return "grey";
    }

    int ModelHasExpo(ExpoData * ExpoArray, ExpoData expo, bool * expoused);
    bool ChannelHasExpo(ExpoData * expoArray, uint8_t destCh);
    int ModelHasMix(MixData * mixArray, MixData mix, bool * mixused);
    bool ChannelHasMix(MixData * mixArray, uint8_t destCh);
    
    QTextEdit * te;
    QString curvefile5;
    QString curvefile9;
    QDir *qd;
    
private slots:
    void on_printButton_clicked();
    void on_printFileButton_clicked();
    
protected:
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void printDiff();
    void printSetup();
    void printPhases();
    void printLimits();
    void printExpos();
    void printMixers();
    void printCurves();
    void printGvars();
    void printSwitches();
    void printSafetySwitches();
    void printFSwitches();
    void printFrSky();
};

#endif // COMPAREDIALOG_H
