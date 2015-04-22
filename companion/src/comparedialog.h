#ifndef COMPAREDIALOG_H
#define COMPAREDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QDir>
#include "eeprominterface.h"

namespace Ui {
  class CompareDialog;
}

class CompareDialog : public QDialog
{
    Q_OBJECT

  public:
    CompareDialog(QWidget *parent, Firmware * firmware);
    ~CompareDialog();
    Firmware * firmware;
    
  protected:
    GeneralSettings * settings;
    ModelData * g_model1;
    ModelData * g_model2;
    ModelData * modeltemp;
    int model1;
    int model2;
  
  private:
    void  closeEvent(QCloseEvent *event);
    Ui::CompareDialog *ui;
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
    QString curvefile1;
    QString curvefile2;
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
