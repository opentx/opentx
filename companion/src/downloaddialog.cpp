/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "downloaddialog.h"
#include "ui_downloaddialog.h"
#include "constants.h"
#include "helpers.h"

#include <QTime>

downloadDialog::downloadDialog(QWidget *parent, QString src, QString tgt):
  QDialog(parent),
  ui(new Ui::downloadDialog),
  file(NULL)
{
    ui->setupUi(this);
    setWindowIcon(CompanionIcon("fwpreferences.png"));
    ui->progressBar->setValue(1);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);

    if (tgt.isEmpty()) {
      setWindowTitle(src);
      return;  // just show wait dialog.
    }

    file = new QFile(tgt);
    if (!file->open(QIODevice::WriteOnly)) {
      QMessageBox::critical(this, CPN_STR_APP_NAME,
          tr("Unable to save the file %1: %2.")
          .arg(tgt).arg(file->errorString()));
      QTimer::singleShot(0, this, SLOT(fileError()));
    }
    else {
      reply = qnam.get(QNetworkRequest(QUrl(src)));
      connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
      connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
      connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDataReadProgress(qint64,qint64)));
    }
}

downloadDialog::~downloadDialog()
{
  delete ui;
  delete file;
}

void downloadDialog::httpFinished()
{
    file->flush();
    file->close();

    bool ok = true;
    if (reply->error()) {
        file->remove();
        QMessageBox::information(this, CPN_STR_APP_NAME,
                                 tr("Download failed: %1.")
                                 .arg(reply->errorString()));
        ok = false;
    }

    reply->deleteLater();
    reply = 0;
    delete file;
    file = NULL;

    if (ok)
      accept();
    else
      reject();
}

void downloadDialog::httpReadyRead()
{
  if (file) {
    file->write(reply->readAll());
  }
}

void downloadDialog::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
  ui->progressBar->setMaximum(totalBytes);
  ui->progressBar->setValue(bytesRead);
}

void downloadDialog::fileError()
{
  delete file;
  file = NULL;
  reject();
}

#if 0
void downloadDialog::closeEvent( QCloseEvent * event)
{
  // Delay closing 2 seconds to avoid unpleasant flashing download dialogs
  QTime closeTime= QTime::currentTime().addSecs(2);
  while( QTime::currentTime() < closeTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

  event->accept();
}
#endif
