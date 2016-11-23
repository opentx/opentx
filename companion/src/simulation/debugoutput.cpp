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

#include "debugoutput.h"
#include "ui_debugoutput.h"

DebugOutput::DebugOutput(QWidget * parent):
  QDialog(parent),
  ui(new Ui::DebugOutput)
{
  ui->setupUi(this);

#ifdef __APPLE__
  QFont newFont("Courier", 13);
  ui->Output->setFont(newFont);
  ui->Output->setAttribute(Qt::WA_MacNormalSize);
#endif
#if defined WIN32 || !defined __GNUC__
  QFont newFont("Courier", 9);
  ui->Output->setFont(newFont);
#endif
}

DebugOutput::~DebugOutput()
{
  delete ui;
}

void DebugOutput::traceCallback(const QString & text)
{
  // ui->Output->appendPlainText(text);
  QTextCursor cursor(ui->Output->textCursor());

  // is the scrollbar at the end?
  bool atEnd = (ui->Output->verticalScrollBar()->value() == ui->Output->verticalScrollBar()->maximum());

  cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1);
  cursor.insertText(text);

  if (atEnd) {
    ui->Output->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
  }
}
