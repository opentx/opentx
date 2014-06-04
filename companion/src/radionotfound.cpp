#include "radionotfound.h"
#include "ui_radionotfound.h"

RadioNotFoundDialog::RadioNotFoundDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RadioNotFoundDialog)
{
  ui->setupUi(this);
}

RadioNotFoundDialog::~RadioNotFoundDialog()
{
  delete ui;
}

void RadioNotFoundDialog::on_OK_clicked()
{
  accept();
}
