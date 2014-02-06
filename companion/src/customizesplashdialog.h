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
  void on_leftLoadButton_clicked();
  void on_leftLibraryButton_clicked();
  void on_leftSaveButton_clicked();
  void on_leftInvertButton_clicked();

  void on_rightLoadButton_clicked();
  void on_rightLibraryButton_clicked();
  void on_rightSaveButton_clicked();
  void on_rightInvertButton_clicked();

private:
  Ui::customizeSplashDialog *ui;
};

#endif // CUSTOMIZESPLASHDIALOG_H
