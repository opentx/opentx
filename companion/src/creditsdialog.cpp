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

#include "creditsdialog.h"
#include "ui_htmldialog.h"
#include "helpers.h"
#include <QFile>

CreditsDialog::CreditsDialog(QWidget * parent):
  QDialog(parent),
  ui(new Ui::HtmlDialog)
{
  ui->setupUi(this);

  setWindowTitle(tr("OpenTX Contributors"));
  setWindowIcon(CompanionIcon("contributors.png"));

  QString str = "<html>" \
                "<head>" \
                "  <style type=\"text/css\">" \
                "    .normal { font-weight:normal;color:#000000;vertical-align:top;font-size:10px;text-align:left;font-family:arial,helvetica,sans-serif; }" \
                "    .bold { font-weight:bold;color:#C00000;vertical-align:top;font-size:10px;text-align:left;font-family:arial,helvetica,sans-serif; }" \
                "    .title { font-weight:bold;color:#000000;font-size:14px;text-align:left;font-family:arial,helvetica,sans-serif; }" \
                "  </style>" \
                "</head>"
                "<body class=\"normal\">";

  foreach(CreditsSection section, readCredits()) {
    str.append(formatTable(sectionTitle(section.title), section.names, 3));
  }

  str.append("<table><tr><td class=\"normal\">&nbsp;</td></tr>" \
             "  <tr><td class=\"normal\">" + tr("Honors go to Rafal Tomczak (RadioClone), Thomas Husterer (th9x) and Erez Raviv (er9x and eePe)") + "<br/></td></tr>" \
             "</table>");

#if 0
  QFile blacklist(":/BLACKLIST.txt");
  if (blacklist.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QStringList names;
    names << blacklist.readAll();
    str.append(formatTable(tr("OpenTX Blacklist"), names, 1));
  }
#endif

  str.append("</body></html>");
  ui->textEditor->setHtml(str);
  ui->textEditor->scroll(0, 0);
  ui->textEditor->setOpenExternalLinks(true);
}

CreditsDialog::~CreditsDialog()
{
  delete ui;
}

QList<CreditsDialog::CreditsSection> CreditsDialog::readCredits()
{
  QFile credits(":/CREDITS.txt");
  QList<CreditsSection> result;
  if (credits.open(QIODevice::ReadOnly | QIODevice::Text)) {
    while (!credits.atEnd()) {
      QByteArray line = credits.readLine().trimmed();
      if (line.size() >= 2) {
        if (line.startsWith("[") && line.endsWith("]")) {
          result.push_back(CreditsSection(line.mid(1, line.size() - 2)));
        } else {
          result.back().addName(line);
        }
      }
    }
  }
  return result;
}

QString CreditsDialog::sectionTitle(const QString & title)
{
  if (title == "Main developers")
    return tr("Main developers");
  else if (title == "Translators")
    return tr("Translators");
  else if (title == "Companies and projects who have donated to OpenTX")
    return tr("Companies and projects who have donated to OpenTX");
  else if (title == "People who have donated to OpenTX")
    return tr("People who have donated to OpenTX");
  else
    return tr("Other contributors");
}

QString CreditsDialog::formatTable(const QString & title, const QStringList & names, int columns)
{
  const float cwidth = 100.0 / columns;
  QString str = "<table width=\"100%\" border=0 cellspacing=0 cellpadding=2>" \
                "  <tr><td class=\"normal\">&nbsp;</td></tr>" \
                "  <tr><td class=\"title\">" + title + "</td></tr>" \
                "</table>";

  str.append("<table width=\"100%\" border=0 cellspacing=0 cellpadding=2>");

  int column = 0;
  foreach(QString name, names) {
    if (column == 0) {
      str.append("<tr>");
    }
    QString trclass = name.contains("monthly") ? "bold" : "normal";
    str.append(QString("<td width=\"%1%\" class=\"%2\">%3</td>").arg(cwidth).arg(trclass).arg(name.trimmed().replace("monthly", tr("monthly"))));
    if (++column == columns) {
      str.append("</tr>");
      column = 0;
    }
  }

  if (column != 0) {
    str.append("</tr>");
  }

  str.append("</table>");
  return str;
}
