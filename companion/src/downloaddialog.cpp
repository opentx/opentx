#include "downloaddialog.h"
#include "ui_downloaddialog.h"
#include <QMessageBox>
#include <QtGui>

downloadDialog::downloadDialog(QWidget *parent, QString src, QString tgt) :
    QDialog(parent),
    ui(new Ui::downloadDialog)
{
    ui->setupUi(this);

    ui->progressBar->setValue(1);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);

    if(tgt.isEmpty())
    {
        setWindowTitle(src);
        return;  // just show wait dialog.
    }

    file = new QFile(tgt);
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "companion9x",
                              tr("Unable to save the file %1: %2.")
                              .arg(tgt).arg(file->errorString()));
        QTimer::singleShot(0, this, SLOT(fileError()));
    } else {

        reply = qnam.get(QNetworkRequest(QUrl(src)));
        connect(reply, SIGNAL(finished()),
                this, SLOT(httpFinished()));
        connect(reply, SIGNAL(readyRead()),
                this, SLOT(httpReadyRead()));
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
                this, SLOT(updateDataReadProgress(qint64,qint64)));
    
    }
}
downloadDialog::~downloadDialog()
{
    delete ui;
}

void downloadDialog::httpFinished()
{
    file->flush();
    file->close();

    bool ok = true;
    if (reply->error())
    {
        file->remove();
        QMessageBox::information(this, tr("companion9x"),
                                 tr("Download failed: %1.")
                                 .arg(reply->errorString()));
        ok = false;
    }

    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;

    if(ok)
        accept();
    else
        reject();
}

void downloadDialog::httpReadyRead()
{
    if (file)
        file->write(reply->readAll());
}

void downloadDialog::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);
}

void downloadDialog::fileError()
{
    delete file;
    file = 0;
    reject();
}
