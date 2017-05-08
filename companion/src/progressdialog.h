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

#ifndef _PROGRESSDIALOG_H_
#define _PROGRESSDIALOG_H_

#include <QDialog>

namespace Ui
{
  class ProgressDialog;
}

class ProgressWidget;

class ProgressDialog : public QDialog
{
  Q_OBJECT

public:
  ProgressDialog(QWidget *parent, const QString &label, const QIcon &icon, bool forceOpen=false);
  ~ProgressDialog();

  ProgressWidget * progress();
  bool isEmpty() const;

public slots:
  void setProcessStarted();
  void setProcessStopped();

private slots:
  void on_closeButton_clicked();
  void on_outputProgress_detailsToggled();
  void on_outputProgress_locked(bool);
  void shrink();

private:
  Ui::ProgressDialog *ui;
  bool locked;
};

#endif // _PROGRESSDIALOG_H_
