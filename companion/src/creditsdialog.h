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

#ifndef _CREDITSDIALOG_H_
#define _CREDITSDIALOG_H_

#include <QDialog>

namespace Ui {
  class HtmlDialog;
}

class CreditsDialog : public QDialog
{
  Q_OBJECT

  class CreditsSection
  {
    public:
      CreditsSection(const QString & title):
        title(title)
      {
      }

      void addName(const QString & name)
      {
        names.push_back(name);
      }

      QString title;
      QStringList names;
  };

  public:
    explicit CreditsDialog(QWidget * parent = 0);
    ~CreditsDialog();

  private:
    Ui::HtmlDialog * ui;
    QList<CreditsSection> readCredits();
    QString sectionTitle(const QString & title);
    QString formatTable(const QString & title, const QStringList & names, int colums);
};

#endif // _CREDITSDIALOG_H_
