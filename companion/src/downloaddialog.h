#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QtWidgets>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>

namespace Ui {
    class downloadDialog;
}

class downloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit downloadDialog(QWidget *parent = 0, QString src = "", QString tgt = "");
    ~downloadDialog();

private slots:
    void fileError();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

private:
#if 0
    void closeEvent( QCloseEvent * event);
#endif
    Ui::downloadDialog *ui;

    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QFile *file;

};

#endif // DOWNLOADDIALOG_H
