#ifndef PROGRESS_DIALOG_H_
#define PROGRESS_DIALOG_H_

#include <QDialog>

namespace Ui
{
  class ProgressDialog;
}

class ProgressWidget;

class ProgressDialog : public QDialog
{
  Q_OBJECT

public:
  ProgressDialog(QWidget *parent, const QString &label, const QIcon &icon, bool forceOpen=false);
  ~ProgressDialog();

  ProgressWidget * progress();

private slots:
  void on_closeButton_clicked();
  void on_outputProgress_detailsToggled();
  void on_outputProgress_locked(bool);
  void shrink();

private:
  Ui::ProgressDialog *ui;
  bool locked;
};

#endif // PROGRESS_DIALOG_H_
