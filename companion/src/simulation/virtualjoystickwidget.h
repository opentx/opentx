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

#ifndef VIRTUALJOYSTICKWIDGET_H
#define VIRTUALJOYSTICKWIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QBoxLayout>
#include <QGridLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QLabel>

class Node;
class SliderWidget;

#define GBALL_SIZE  20
#define RESX        1024

class VirtualJoystickWidget : public QWidget
{
  Q_OBJECT

  public:
    enum TrimAxes {
      TRIM_AXIS_L_X = 0,
      TRIM_AXIS_L_Y,
      TRIM_AXIS_R_Y,
      TRIM_AXIS_R_X,
    };

    enum ConstraintTypes {
      HOLD_X = 0,
      HOLD_Y,
      FIX_X,
      FIX_Y
    };

    explicit VirtualJoystickWidget(QWidget * parent = NULL, QChar side = 'L', bool showTrims = true, bool showBtns = true, bool showValues = true, QSize size = QSize(245, 245));

    void setStickX(qreal x);
    void setStickY(qreal y);
    void setStickPos(QPointF xy);
    void centerStick();
    qreal getStickX();
    qreal getStickY();
    QPointF getStickPos();

    void setTrimValue(int which, int value);
    void setTrimRange(int which, int min, int max);
    int getTrimValue(int which);

    void setStickConstraint(int which, bool active);

    virtual QSize sizeHint() const
    {
      return prefSize;
    }

    virtual void resizeEvent(QResizeEvent *event)
    {
      QWidget::resizeEvent(event);
      setSize(event->size());
    }

  signals:
    void trimButtonPressed(int which);
    void trimButtonReleased();
    void trimSliderMoved(int which, int value);

  protected slots:
    void onTrimPressed();
    void onSliderChange(int value);
    void onButtonChange(bool checked);
    void updateNodeValueLabels();

  protected:
    void setSize(QSize size);
    void repositionNode();
    QWidget * createTrimWidget(QChar type);
    QPushButton * createButtonWidget(int type);
    QLayout * createNodeValueLayout(QChar type, QLabel *& valLabel);
    int getTrimSliderType(QChar type);
    int getTrimButtonType(QChar type, int pos);
    SliderWidget * getTrimSlider(int which);

    QChar stickSide;
    QSize prefSize;
    QGridLayout * layout;
    QGraphicsView * gv;
    QGraphicsScene * scene;
    Node * node;
    SliderWidget * hTrimSlider, * vTrimSlider;
    QPushButton * btnHoldX, * btnHoldY;
    QPushButton * btnFixX, * btnFixY;
    QLabel * nodeLabelX, * nodeLabelY;
    QSize extraSize;
    float ar;  // aspect ratio

};

#endif // VIRTUALJOYSTICKWIDGET_H
