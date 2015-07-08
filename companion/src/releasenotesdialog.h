#ifndef RELEASENOTESDIALOG_H
#define RELEASENOTESDIALOG_H

#include <QDialog>
#include "helpers.h"

namespace Ui {
  class HtmlDialog;
}

class ReleaseNotesDialog : public QDialog
{
  Q_OBJECT

  public:
    explicit ReleaseNotesDialog(QWidget *parent = 0);
    ~ReleaseNotesDialog();

  private:
    Ui::HtmlDialog * ui;
};

#endif // RELEASENOTESDIALOG_H
