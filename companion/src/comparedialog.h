#ifndef COMPAREDIALOG_H
#define COMPAREDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QDir>
#include "eeprominterface.h"
#include "modelprinter.h"

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
    ModelData g_model1;
    ModelData g_model2;
    bool model1Valid;
    bool model2Valid;
  
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

    bool ChannelHasExpo(ExpoData * expoArray, uint8_t destCh);
    bool ChannelHasMix(MixData * mixArray, uint8_t destCh);
    bool handleDroppedModel(const QMimeData * mimeData, ModelData * model, QLabel * label);
    void applyDiffFont(QString & v1, QString & v2, const QString & font = "", bool eqGrey = false);
    void diffAndAssembleTableLines(QStringList & i1, QStringList & i2, QString & o1, QString & o2);
    
    QTextEdit * te;
    QString curvefile1;
    QString curvefile2;
    QDir *qd;

    ModelPrinter modelPrinter1;
    ModelPrinter modelPrinter2;
    GeneralSettings dummySettings;
    
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
    void printInputs();
    void printMixers();
    void printCurves();
    void printGvars();
    void printLogicalSwitches();
    void printSafetySwitches();
    void printCustomFunctions();
    void printFrSky();
};

#endif // COMPAREDIALOG_H
