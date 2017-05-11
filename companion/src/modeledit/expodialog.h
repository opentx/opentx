#ifndef EXPODIALOG_H
#define EXPODIALOG_H

#include <QDialog>
#include <QCheckBox>
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
    void label_phases_customContextMenuRequested(const QPoint & pos);
    void fmClearAll();
    void fmSetAll();
    void fmInvertAll();

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
    bool lock;
    QCheckBox * cb_fp[C9X_MAX_FLIGHT_MODES-1];
};

#endif // EXPODIALOG_H
