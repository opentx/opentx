#include "progressdialog.h"
#include "ui_progressdialog.h"
#include "appdata.h"
#include <QTimer>

ProgressDialog::ProgressDialog(QWidget *parent, const QString &title, const QIcon &icon, bool forceOpen):
QDialog(parent),
ui(new Ui::ProgressDialog),
locked(false)
{
  ui->setupUi(this);
  setWindowTitle(title);
  setWindowIcon(icon);
  if (forceOpen) {
    ui->outputProgress->forceOpen();
  }
  resize(0, 0);
  show();
}

ProgressDialog::~ProgressDialog()
{
  delete ui;
}

ProgressWidget * ProgressDialog::progress()
{
  return ui->outputProgress;
}

void ProgressDialog::on_closeButton_clicked()
{
  if (!locked) {
    ui->outputProgress->stop();
    close();
  }
}

void ProgressDialog::on_outputProgress_detailsToggled()
{
  QTimer::singleShot(0, this, SLOT(shrink()));
}

void ProgressDialog::on_outputProgress_locked(bool lock)
{
  ui->closeButton->setEnabled(!lock);
  locked = lock;
}

void ProgressDialog::shrink()
{
  resize(0, 0);
}
