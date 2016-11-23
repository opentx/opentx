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

#ifndef _COMPAREDIALOG_H_
#define _COMPAREDIALOG_H_

#include <QtWidgets>
#include "multimodelprinter.h"

namespace Ui {
  class CompareDialog;
}

class CompareDialog : public QDialog
{
  Q_OBJECT

  public:
    CompareDialog(QWidget *parent, Firmware * firmware);
    ~CompareDialog();
    
  protected:
    MultiModelPrinter multimodelprinter;
    ModelData model1;
    ModelData model2;
    bool model1Valid;
    bool model2Valid;
    Ui::CompareDialog * ui;
    
  protected slots:
    void on_printButton_clicked();
    void on_printFileButton_clicked();
    
  protected:
    void closeEvent(QCloseEvent * event);
    bool handleDroppedModel(const QMimeData * mimeData, ModelData & model, QLabel * label);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void printDiff();
};

#endif // _COMPAREDIALOG_H_
