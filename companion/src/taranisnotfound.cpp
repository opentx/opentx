#include "taranisnotfound.h"
#include "ui_taranisnotfound.h"

taranisNotFoundDialog::taranisNotFoundDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::taranisNotFoundDialog)
{
  ui->setupUi(this);
}

taranisNotFoundDialog::~taranisNotFoundDialog()
{
  delete ui;
}

void taranisNotFoundDialog::on_OK_clicked()
{
  accept();
}
