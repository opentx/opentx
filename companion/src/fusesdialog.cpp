#include "fusesdialog.h"
#include "ui_fusesdialog.h"
#include "helpers.h"
#include "progressdialog.h"
#include "radiointerface.h"

FusesDialog::FusesDialog(QWidget *parent) :
QDialog(parent),
ui(new Ui::FusesDialog)
{
  ui->setupUi(this);
  setWindowIcon(CompanionIcon("fuses.png"));
}

FusesDialog::~FusesDialog()
{
  delete ui;
}

void FusesDialog::on_resetFuses_EEprotect_clicked()
{
  ProgressDialog progressDialog(this, tr("Reset Radio Fuses"), CompanionIcon("fuses.png"), true);
  return resetAvrdudeFuses(true, progressDialog.progress());
}

void FusesDialog::on_resetFuses_EEdelete_clicked()
{
  ProgressDialog progressDialog(this, tr("Reset Radio Fuses"), CompanionIcon("fuses.png"), true);
  return resetAvrdudeFuses(false, progressDialog.progress());
}

void FusesDialog::on_readFuses_clicked()
{
  ProgressDialog progressDialog(this, tr("Read Fuses from Radio"), CompanionIcon("fuses.png"), true);
  return readAvrdudeFuses(progressDialog.progress());
}
