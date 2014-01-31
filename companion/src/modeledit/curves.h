#ifndef CURVES_H
#define CURVES_H

#include "modelpanel.h"
#include <QSpinBox>

namespace Ui {
  class Curves;
}

class Curves : public ModelPanel
{
    Q_OBJECT

  public:
    Curves(QWidget *parent, ModelData & model);
    virtual ~Curves();

    virtual void update();

  private slots:
    void resetCurve();
    void editCurve();
    void plotCurve(bool checked);
    void on_curveName_editingFinished();
    void on_curvePoints_currentIndexChanged(int index);
    void on_curveCustom_currentIndexChanged(int index);
    void on_curveSmooth_currentIndexChanged(int index);
    void onPointEdited();
    void onNodeMoved(int x, int y);
    void onNodeFocus();
    void onNodeUnfocus();

  protected:
    virtual void resizeEvent(QResizeEvent *event);

  private:
    Ui::Curves *ui;
    int currentCurve;
    bool visibleCurves[C9X_MAX_CURVES];
    QSpinBox * spnx[C9X_MAX_POINTS];
    QSpinBox * spny[C9X_MAX_POINTS];
    void setCurrentCurve(int index);
    void updateCurve();
    void updateCurveType();
    bool allowCurveType(int points, CurveData::CurveType type);

};

#endif // CURVES_H
