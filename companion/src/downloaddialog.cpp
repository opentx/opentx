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

DownloadDialog::DownloadDialog(QWidget *parent, QString src, QString tgt):
  QDialog(parent),
  ui(new Ui::DownloadDialog),
  reply(nullptr),
  file(nullptr),
  aborted(false)
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
      connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateDataReadProgress(qint64, qint64)));
    }
}

DownloadDialog::~DownloadDialog()
{
  delete ui;
  delete file;
}

void DownloadDialog::reject()
{
  if (reply && reply->isRunning()) {
    aborted = true;
    reply->abort();  // this will call QNetworkReply::finished()
    return;
  }

  QDialog::reject();
}

void DownloadDialog::httpFinished()
{
  file->flush();
  file->close();

  const bool ok = !(reply->error() || aborted);
  if (!ok) {
    file->remove();
    if (!aborted)
      QMessageBox::information(this, CPN_STR_APP_NAME, tr("Download failed: %1.").arg(reply->errorString()));
  }

  reply->deleteLater();
  reply = nullptr;
  file->deleteLater();
  file = nullptr;

  if (ok)
    accept();
  else
    reject();
}

void DownloadDialog::httpReadyRead()
{
  if (file) {
    file->write(reply->readAll());
  }
}

void DownloadDialog::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
  ui->progressBar->setMaximum(totalBytes);
  ui->progressBar->setValue(bytesRead);
}

void DownloadDialog::fileError()
{
  delete file;
  file = nullptr;
  reject();
}

#if 0
void DownloadDialog::closeEvent( QCloseEvent * event)
{
  // Delay closing 2 seconds to avoid unpleasant flashing download dialogs
  QTime closeTime= QTime::currentTime().addSecs(2);
  while( QTime::currentTime() < closeTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

  event->accept();
}
#endif
