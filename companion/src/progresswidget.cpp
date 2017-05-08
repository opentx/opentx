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

#include "progresswidget.h"
#include "ui_progresswidget.h"
#include "appdata.h"
#include <QDebug>
#include <QTimer>
#include <QScrollBar>

ProgressWidget::ProgressWidget(QWidget *parent):
QWidget(parent),
ui(new Ui::ProgressWidget)
{
  ui->setupUi(this);
  ui->info->hide();
  ui->checkBox->hide();
  ui->textEdit->hide();

  QFont newFont(ui->textEdit->font());
  newFont.setFamily("Courier");
#ifdef __APPLE__
  newFont.setPointSize(13);
  ui->textEdit->setAttribute(Qt::WA_MacNormalSize);
#elif defined WIN32 || !defined __GNUC__
  newFont.setPointSize(9);
#endif
  ui->textEdit->setFont(newFont);
}

ProgressWidget::~ProgressWidget()
{
  delete ui;
}

void ProgressWidget::stop()
{
  emit stopped();
}

void ProgressWidget::forceOpen()
{
  ui->checkBox->hide();
  ui->textEdit->show();
}

void ProgressWidget::setInfo(const QString &text)
{
  ui->info->show();
  ui->info->setText(text);
}

void ProgressWidget::setMaximum(int value)
{
  ui->progressBar->setMaximum(value);
}

int ProgressWidget::maximum()
{
  return ui->progressBar->maximum();
}

void ProgressWidget::setValue(int value)
{
  ui->progressBar->setValue(value);
}

void ProgressWidget::addText(const QString &text, const bool richText)
{
  ui->checkBox->setVisible(true);
  ui->checkBox->setChecked(g.outputDisplayDetails());

  QTextCursor cursor(ui->textEdit->textCursor());

  // is the scrollbar at the end?
  bool atEnd = (ui->textEdit->verticalScrollBar()->value() == ui->textEdit->verticalScrollBar()->maximum());

  cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1);
  if (richText)
    cursor.insertHtml(text);
  else
    cursor.insertText(text);

  if (atEnd) {
    ui->textEdit->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
  }
}

void ProgressWidget::addHtml(const QString & text)
{
  addText(text, true);
}

void ProgressWidget::addMessage(const QString & text, const int & type)
{
  QString color;
  switch (type) {
    case QtDebugMsg:
      color = "dimgrey";  // not important messages, may be filtered out
      break;
    case QtWarningMsg:    // use warning level as emphasis
      color = "darkblue";
      break;
    case QtCriticalMsg:   // use critical as a warning
      color = "#ff7900";
      break;
    case QtFatalMsg:      // fatal for hard errors
      color = "red";
      break;
    case QtInfoMsg:       // default plain text
    default:
      break;
  }
  if (color.isEmpty()) {
    if (text.contains(QRegExp("<[^>]+>")))  // detect html (rouhgly)
      addHtml(text % "<br>");
    else
      addText(text % "\n", false);
  }
  else
    addHtml(QString("<font color=%1>").arg(color) % text % "</font><br>");
}

QString ProgressWidget::getText()
{
  return ui->textEdit->toPlainText();
}

bool ProgressWidget::isEmpty() const
{
  return ui->textEdit->toPlainText().isEmpty();
}

void ProgressWidget::setProgressColor(const QColor &color)
{
  ui->progressBar->setStyleSheet(QString("QProgressBar  {text-align: center;} QProgressBar::chunk { background-color: %1; text-align:center;}:").arg(color.name()));
}

#define HLINE_SEPARATOR "================================================================================="

void ProgressWidget::addSeparator()
{
  addText("\n" HLINE_SEPARATOR "\n");
}

void ProgressWidget::on_checkBox_toggled(bool checked)
{
  g.outputDisplayDetails(checked);
  ui->textEdit->setVisible(checked);
  QTimer::singleShot(0, this, SLOT(shrink()));
}

void ProgressWidget::lock(bool lock)
{
  emit locked(lock);
}

void ProgressWidget::shrink()
{
  emit detailsToggled();
}


