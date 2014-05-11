#ifndef TARANISNOTFOUNDDIALOG_H
#define TARANISNOTFOUNDDIALOG_H

#include <QDialog>

namespace Ui {
    class taranisNotFoundDialog;
}

class taranisNotFoundDialog : public QDialog
{
    Q_OBJECT
public:
    explicit taranisNotFoundDialog(QWidget *parent = 0);
    ~taranisNotFoundDialog();
private:
    Ui::taranisNotFoundDialog *ui;
private slots:
    void on_OK_clicked();
};

#endif // TARANISNOTFOUNDDIALOG_H
