#ifndef CURVES_H
#define CURVES_H

#include "modeledit.h"
#include "eeprominterface.h"

enum CopyAction {
  CURVE_COPY,
  CURVE_PASTE,
  CURVE_RESET,
  CURVE_RESETALL
};

namespace Ui {
  class Curves;
}

typedef float (*curveFunction) (float x, float coeff, float yMin, float yMid, float yMax);

struct CurveCreatorTemplate {
  QString name;
  unsigned int flags;
  curveFunction function;
};

class Curves : public ModelPanel
{
    Q_OBJECT

  public:
    Curves(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~Curves();

    virtual void update();

  private slots:
    void editCurve();
    void ShowContextMenu(const QPoint& pos);
    void plotCurve(bool checked);
    void on_curveName_editingFinished();
    void on_curvePoints_currentIndexChanged(int index);
    void on_curveCustom_currentIndexChanged(int index);
    void on_curveSmooth_currentIndexChanged(int index);
    void onPointEdited();
    void onNodeMoved(int x, int y);
    void onNodeFocus();
    void onNodeUnfocus();
    void on_curveType_currentIndexChanged();
    void on_curveApply_clicked();

  protected:
    virtual void resizeEvent(QResizeEvent *event);
    void addTemplate(QString name, unsigned int flags, curveFunction function);

  private:
    Ui::Curves *ui;
    int currentCurve;
    bool visibleCurves[C9X_MAX_CURVES];
    QSpinBox * spnx[C9X_MAX_POINTS];
    QSpinBox * spny[C9X_MAX_POINTS];
    QVector<CurveCreatorTemplate> templates;
    void setCurrentCurve(int index);
    void updateCurve();
    void updateCurveType();
    void updateCurvePoints();
    bool allowCurveType(int points, CurveData::CurveType type);
    void setPointY(int i, int x, int y);

};

#endif // CURVES_H
