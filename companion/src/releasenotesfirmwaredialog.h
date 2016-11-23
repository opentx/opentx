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

#ifndef _RELEASENOTESFIRMWAREDIALOG_H_
#define _RELEASENOTESFIRMWAREDIALOG_H_

#include <QDialog>
#include "eeprominterface.h"
#include "downloaddialog.h"

namespace Ui {
  class HtmlDialog;
}

class ReleaseNotesFirmwareDialog : public QDialog
{
  Q_OBJECT

  public:
    explicit ReleaseNotesFirmwareDialog(QWidget *parent, const QString & rnurl);
    ~ReleaseNotesFirmwareDialog();

  private:
    Ui::HtmlDialog *ui;
    QNetworkAccessManager *manager;

  private slots:
    void replyFinished(QNetworkReply * reply);   
    
};

#endif // _RELEASENOTESFIRMWAREDIALOG_H_
