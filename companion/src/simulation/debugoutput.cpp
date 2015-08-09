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
