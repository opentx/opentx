#include "fusesdialog.h"
#include "ui_fusesdialog.h"

#include "burnconfigdialog.h"

fusesDialog::fusesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fusesDialog)
{
    ui->setupUi(this);
}

fusesDialog::~fusesDialog()
{
    delete ui;
}

void fusesDialog::on_resetFuses_EEprotect_clicked()
{
    burnConfigDialog *bcd = new burnConfigDialog(this);
    bcd->restFuses(true);
}

void fusesDialog::on_resetFuses_EEdelete_clicked()
{
    burnConfigDialog *bcd = new burnConfigDialog(this);
    bcd->restFuses(false);
}

void fusesDialog::on_readFuses_clicked()
{
    burnConfigDialog *bcd = new burnConfigDialog(this);
    bcd->readFuses();
}
