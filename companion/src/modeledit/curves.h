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

class CompoundItemModelFactory;

constexpr char MIMETYPE_CURVE[] = "application/x-companion-curve";

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

class CurvesPanel : public ModelPanel
{
    Q_OBJECT

  public:
    CurvesPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware, CompoundItemModelFactory * sharedItemModels);
    virtual ~CurvesPanel();

    virtual void update();

  private slots:
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
    void on_curveType_currentIndexChanged(int index);
    void on_curveApply_clicked();
    void onSceneNewPoint(int x, int y);
    void onPointSizeEdited();
    void onNodeDelete();
    void onCustomContextMenuRequested(QPoint pos);
    void cmClear(bool prompt = true);
    void cmClearAll();
    void cmCopy();
    void cmCut();
    void cmDelete();
    void cmInsert();
    void cmPaste();
    void cmMoveDown();
    void cmMoveUp();

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
    int maxCurves;
    int hasNames;
    int hasEnhanced;
    int maxPoints;
    int selectedIndex;
    bool hasClipboardData(QByteArray * data = nullptr) const;
    bool insertAllowed() const;
    bool moveDownAllowed() const;
    bool moveUpAllowed() const;
    void swapData(int idx1, int idx2);
    CompoundItemModelFactory * sharedItemModels;
    void updateItemModels();
};

#endif // _CURVES_H_
