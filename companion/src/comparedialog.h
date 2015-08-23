#ifndef _COMPAREDIALOG_H
#define _COMPAREDIALOG_H

#include <QDialog>
#include <QLabel>
#include "multimodelprinter.h"

namespace Ui {
  class CompareDialog;
}

class CompareDialog : public QDialog
{
  Q_OBJECT

  public:
    CompareDialog(QWidget *parent, Firmware * firmware);
    ~CompareDialog();
    
  protected:
    MultiModelPrinter multimodelprinter;
    ModelData model1;
    ModelData model2;
    bool model1Valid;
    bool model2Valid;
    Ui::CompareDialog * ui;
    
  protected slots:
    void on_printButton_clicked();
    void on_printFileButton_clicked();
    
  protected:
    void closeEvent(QCloseEvent * event);
    bool handleDroppedModel(const QMimeData * mimeData, ModelData & model, QLabel * label);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void printDiff();
};

#endif // _COMPAREDIALOG_H
