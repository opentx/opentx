#ifndef CONTRIBUTORSDIALOG_H
#define CONTRIBUTORSDIALOG_H

#include <QDialog>
#include "eeprominterface.h"
#include "downloaddialog.h"

namespace Ui {
    class contributorsDialog;
}

class contributorsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit contributorsDialog(QWidget *parent = 0, int contest = 0, QString rnurl = QString() );
    ~contributorsDialog();

private:
    Ui::contributorsDialog *ui;
    QNetworkAccessManager *manager;

    void showEvent ( QShowEvent * );

private slots:
    void forceClose();
    void replyFinished(QNetworkReply * reply);   
    
};

#endif // CONTRIBUTORSDIALOG_H
