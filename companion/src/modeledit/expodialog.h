#ifndef EXPODIALOG_H
#define EXPODIALOG_H

#include <QtWidgets>
#include "eeprominterface.h"
#include "modelprinter.h"

class GVarGroup;
class CurveGroup;

namespace Ui {
  class ExpoDialog;
}

class ExpoDialog : public QDialog {
    Q_OBJECT
  public:
    ExpoDialog(QWidget *parent, ModelData & model, ExpoData *expodata, GeneralSettings & generalSettings, 
                Firmware * firmware, QString & inputName);
    ~ExpoDialog();

  protected:
    void updateScale();

  private slots:
    void valuesChanged();
    void shrink();    

  private:
    Ui::ExpoDialog * ui;
    ModelData & model;
    GeneralSettings & generalSettings;
    Firmware * firmware;
    ExpoData * ed;
    QString & inputName;
    GVarGroup * gvWeightGroup;
    GVarGroup * gvOffsetGroup;
    CurveGroup * curveGroup;
    ModelPrinter modelPrinter;
};

#endif // EXPODIALOG_H
