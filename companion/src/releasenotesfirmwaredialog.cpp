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

#include "releasenotesfirmwaredialog.h"
#include "ui_htmldialog.h"
#include "helpers.h"

ReleaseNotesFirmwareDialog::ReleaseNotesFirmwareDialog(QWidget * parent, const QString & rnurl):
  QDialog(parent),
  ui(new Ui::HtmlDialog)
{
  ui->setupUi(this);

  setWindowTitle(tr("OpenTX Release Notes"));
  setWindowIcon(CompanionIcon("changelog.png"));

  manager = new QNetworkAccessManager(this);
  connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
  QUrl url(rnurl);
  QNetworkRequest request(url);
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
  manager->get(request);
}

ReleaseNotesFirmwareDialog::~ReleaseNotesFirmwareDialog()
{
  delete ui;
}

void ReleaseNotesFirmwareDialog::replyFinished(QNetworkReply * reply)
{
  ui->textEditor->setHtml(reply->readAll());
  ui->textEditor->setOpenExternalLinks(true);
  reply->deleteLater();
}

