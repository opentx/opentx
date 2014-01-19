#ifndef EXPODIALOG_H
#define EXPODIALOG_H

#include <QDialog>
#include "eeprominterface.h"

class GVarGroup;
class CurveGroup;

namespace Ui {
  class ExpoDialog;
}

class ExpoDialog : public QDialog {
    Q_OBJECT
  public:
    ExpoDialog(QWidget *parent, ModelData & model, ExpoData *expodata, int stickMode);
    ~ExpoDialog();

  protected:
    void changeEvent(QEvent *e);

  private slots:
    void valuesChanged();
    void shrink();    

  private:
    Ui::ExpoDialog * ui;
    ModelData & model;
    ExpoData * ed;
    GVarGroup * gvGroup;
    CurveGroup * curveGroup;
};

#endif // EXPODIALOG_H
