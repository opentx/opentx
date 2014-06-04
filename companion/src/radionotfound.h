#ifndef _RADIONOTFOUNDDIALOG_H
#define _RADIONOTFOUNDDIALOG_H

#include <QDialog>

namespace Ui {
    class RadioNotFoundDialog;
}

class RadioNotFoundDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RadioNotFoundDialog(QWidget *parent = 0);
    ~RadioNotFoundDialog();
private:
    Ui::RadioNotFoundDialog *ui;
private slots:
    void on_OK_clicked();
};

#endif // _RADIONOTFOUNDDIALOG_H
