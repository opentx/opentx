#ifndef MIXERDIALOG_H
#define MIXERDIALOG_H

#include <QDialog>
#include "eeprominterface.h"

class GVarGroup;
class CurveGroup;

namespace Ui {
  class MixerDialog;
}

class MixerDialog : public QDialog {
    Q_OBJECT
  public:
    MixerDialog(QWidget *parent, ModelData & model, MixData *mixdata, int stickMode);
    ~MixerDialog();

  protected:
    void changeEvent(QEvent *e);

  private slots:
    void valuesChanged();
    void shrink();

  private:
    Ui::MixerDialog *ui;
    ModelData & model;
    MixData *md;
    bool lock;
    GVarGroup * gvWeightGroup;
    GVarGroup * gvOffsetGroup;
    CurveGroup * curveGroup;
};

#endif // MIXERDIALOG_H
