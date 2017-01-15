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

#include "releasenotesdialog.h"
#include "ui_htmldialog.h"
#include <QFile>

ReleaseNotesDialog::ReleaseNotesDialog(QWidget * parent) :
  QDialog(parent),
  ui(new Ui::HtmlDialog)
{
  ui->setupUi(this);

  setWindowTitle(tr("Companion Release Notes"));
  setWindowIcon(CompanionIcon("changelog.png"));

  QFile file(":/releasenotes.txt");
  if (file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    ui->textEditor->setHtml(file.readAll());
    ui->textEditor->setOpenExternalLinks(true);
  }
  ui->textEditor->scroll(0, 0);
  ui->textEditor->setOpenExternalLinks(true);
}

ReleaseNotesDialog::~ReleaseNotesDialog()
{
  delete ui;
}
