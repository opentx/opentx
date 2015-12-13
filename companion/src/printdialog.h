#ifndef _PRINTDIALOG_H
#define _PRINTDIALOG_H

#include <QDialog>
#include "eeprominterface.h"
#include "multimodelprinter.h"

namespace Ui {
  class PrintDialog;
}

class PrintDialog : public QDialog
{
    Q_OBJECT

  public:
    PrintDialog(QWidget * parent, Firmware * firmware, GeneralSettings & generalSettings, ModelData & model, const QString & filename="");
    ~PrintDialog();
    void closeEvent(QCloseEvent * event);
    
    Firmware * firmware;
    GeneralSettings & generalSettings;
    ModelData & model;

    QString printfilename;

  protected:
    Ui::PrintDialog *ui;
    MultiModelPrinter multimodelprinter; // TODO multimodelPrinter

    void printToFile();

  private slots:
    void on_printButton_clicked();
    void on_printFileButton_clicked();
    void autoClose();
};

#endif // _PRINTDIALOG_H
