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

#include "comparedialog.h"
#include "ui_comparedialog.h"
#include "helpers.h"
#include <QPrinter>
#include <QPrintDialog>

// #if !defined WIN32 && defined __GNUC__
// #include <unistd.h>
// #endif

class DragDropHeader {
  public:
    DragDropHeader():
      general_settings(false),
      models_count(0)
    {
    }
    bool general_settings;
    uint8_t models_count;
    uint8_t models[CPN_MAX_MODELS];
};

CompareDialog::CompareDialog(QWidget * parent, Firmware * firmware):
  QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
  multimodelprinter(firmware),
  model1Valid(false),
  model2Valid(false),
  ui(new Ui::CompareDialog)
{
  ui->setupUi(this);

  setWindowIcon(CompanionIcon("compare.png"));
  setAcceptDrops(true);

  //setDragDropOverwriteMode(true);
  //setDropIndicatorShown(true);
  // TODO scroll to top ... ui->textEdit->scrollToAnchor("1");
}

void CompareDialog::dragMoveEvent(QDragMoveEvent * event)
{
  if (event->mimeData()->hasFormat("application/x-companion")) {   
    event->acceptProposedAction();
  }
  else {
    event->ignore();
  }
}

void CompareDialog::dragEnterEvent(QDragEnterEvent * event)
{
  // accept just text/uri-list mime format
  if (event->mimeData()->hasFormat("application/x-companion")) {   
    event->acceptProposedAction();
  }
  else {
    event->ignore();
  }
}

void CompareDialog::dragLeaveEvent(QDragLeaveEvent * event)
{
  event->accept();
}

bool CompareDialog::handleDroppedModel(const QMimeData * mimeData, ModelData & model, QLabel * label)
{
  if (mimeData->hasFormat("application/x-companion")) {
    QByteArray gmData = mimeData->data("application/x-companion");
    DragDropHeader * header = (DragDropHeader *)gmData.data();
    if (!header->general_settings) {
      char * gData = gmData.data() + sizeof(DragDropHeader);
      if (gData[0] == 'M') {
        ModelData * modeltemp = (ModelData *)(gData + 1);
        if (modeltemp->used) {
          memcpy(&model, modeltemp, sizeof(ModelData));
          QString name(model.name);
          if (!name.isEmpty())
            label->setText(name);
          else
            label->setText(tr("No name"));
          return true;
        }
      }
    }
  }
  return false;
}

void CompareDialog::dropEvent(QDropEvent *event)
{
  QLabel * child = qobject_cast<QLabel*>(childAt(event->pos()));
  if (!child) return;
  if (child->objectName().contains("label_1")) {   
    model1Valid = handleDroppedModel(event->mimeData(), model1, ui->label_1);
  }
  else if (child->objectName().contains("label_2")) {
    model2Valid = handleDroppedModel(event->mimeData(), model2, ui->label_2);
  }
  event->accept();
  if (model1Valid && model2Valid) {
    multimodelprinter.setModel(0, model1);
    multimodelprinter.setModel(1, model2);
    ui->textEdit->setHtml(multimodelprinter.print(ui->textEdit->document()));
  }
}

void CompareDialog::closeEvent(QCloseEvent *event) 
{
}

CompareDialog::~CompareDialog()
{
  delete ui;
}

void CompareDialog::on_printButton_clicked()
{
  QPrinter printer;
  printer.setPageMargins(10.0, 10.0, 10.0, 10.0, printer.Millimeter);
  QPrintDialog * dialog = new QPrintDialog(&printer, this);
  dialog->setWindowTitle(tr("Print Document"));
  if (dialog->exec() != QDialog::Accepted)
    return;
  ui->textEdit->print(&printer);
}

void CompareDialog::on_printFileButton_clicked()
{
  QPrinter printer;
  QString filename = QFileDialog::getSaveFileName(this, tr("Select PDF output file"), QString(), "Pdf File(*.pdf)");
  printer.setPageMargins(10.0, 10.0, 10.0, 10.0, printer.Millimeter);
  printer.setOutputFormat(QPrinter::PdfFormat);
  printer.setOrientation(QPrinter::Landscape);
  printer.setColorMode(QPrinter::Color);
  if (!filename.isEmpty()) {
    if (QFileInfo(filename).suffix().isEmpty())
      filename.append(".pdf");
    printer.setOutputFileName(filename);
    ui->textEdit->print(&printer);
  }
}
