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

#ifndef _CURVES_H_
#define _CURVES_H_

#include "modeledit.h"
#include "eeprominterface.h"
#include <QGraphicsScene>
#include <QGraphicsView>

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

class CustomScene : public QGraphicsScene
{
  Q_OBJECT

  public:
    CustomScene(QGraphicsView * view);

  signals:
    void newPoint(int, int);

  protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event) override;
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
    void on_curveType_currentIndexChanged(int index);
    void on_curveApply_clicked();
    void onSceneNewPoint(int x, int y);
    void onPointSizeEdited();
    void onNodeDelete();

  protected:
    virtual void resizeEvent(QResizeEvent *event);
    void addTemplate(QString name, unsigned int flags, curveFunction function);

  private:
    Ui::Curves *ui;
    int currentCurve;
    bool visibleCurves[CPN_MAX_CURVES];
    QSpinBox * spnx[CPN_MAX_POINTS];
    QSpinBox * spny[CPN_MAX_POINTS];
    QVector<CurveCreatorTemplate> templates;
    void setCurrentCurve(int index);
    void updateCurve();
    void updateCurveType();
    void updateCurvePoints();
    bool allowCurveType(int points, CurveData::CurveType type);
    void setPointY(int i, int x, int y);
    CustomScene * scene;
};

#endif // _CURVES_H_
