#ifndef FUSESDIALOG_H
#define FUSESDIALOG_H

#include <QDialog>

namespace Ui {
    class fusesDialog;
}

class fusesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit fusesDialog(QWidget *parent = 0);
    ~fusesDialog();

private:
    Ui::fusesDialog *ui;

private slots:
    void on_readFuses_clicked();
    void on_resetFuses_EEdelete_clicked();
    void on_resetFuses_EEprotect_clicked();
};

#endif // FUSESDIALOG_H
