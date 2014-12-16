#ifndef FUSESDIALOG_H_
#define FUSESDIALOG_H_

#include <QDialog>

namespace Ui {
  class FusesDialog;
}

class FusesDialog : public QDialog
{
  Q_OBJECT

public:
  explicit FusesDialog(QWidget *parent = 0);
  ~FusesDialog();

private slots:
  void on_readFuses_clicked();
  void on_resetFuses_EEdelete_clicked();
  void on_resetFuses_EEprotect_clicked();

private:
  Ui::FusesDialog *ui;

};

#endif // FUSESDIALOG_H_
