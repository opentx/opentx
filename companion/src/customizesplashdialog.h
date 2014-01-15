#ifndef CUSTOMIZESPLASHDIALOG_H
#define CUSTOMIZESPLASHDIALOG_H

#include <QtGui>
#include <QDialog>
#include "flashinterface.h"

namespace Ui
{
  class customizeSplashDialog;
}

class customizeSplashDialog : public QDialog
{
  Q_OBJECT

public:
  explicit customizeSplashDialog(QWidget *parent = 0);
  ~customizeSplashDialog();

private slots:
  void on_FlashLoadButton_clicked();
  void on_ImageLoadButton_clicked();
  void on_libraryButton_clicked();
  void on_SaveFlashButton_clicked();
  void on_InvertColorButton_clicked();
  void on_SaveImageButton_clicked();

private:
  Ui::customizeSplashDialog *ui;
};

#endif // CUSTOMIZESPLASHDIALOG_H
