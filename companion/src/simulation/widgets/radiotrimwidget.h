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

#ifndef _RADIOTRIMWIDGET_H_
#define _RADIOTRIMWIDGET_H_

#include "radiowidget.h"
#include "sliderwidget.h"

#include <QToolButton>

class RadioTrimWidget : public RadioWidget
{
  Q_OBJECT

  public:

    explicit RadioTrimWidget(Qt::Orientation orientation = Qt::Vertical, QWidget * parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
      RadioWidget(parent, f),
      m_slider(NULL)
    {
      init(orientation);
    }
    explicit RadioTrimWidget(const QString & labelText, Qt::Orientation orientation = Qt::Vertical, int value = 0, QWidget * parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
      RadioWidget(labelText, value, parent, f),
      m_slider(NULL)
    {
      init(orientation);
    }

    void setIndices(int sliderIdx = -1, int decrBtnIdx = -1, int incrBtnIdx = -1)
    {
      setIndex(sliderIdx);
      m_btnDecIndex = decrBtnIdx;
      m_btnIncIndex = incrBtnIdx;
    }

    void setTrimRange(int min, int max)
    {
      if (m_slider)
        m_slider->setRange(min, max);
    }

  signals:

    void trimButtonPressed(int index);
    void trimButtonReleased(int index);
    void trimSliderMoved(int index, int value);

  protected slots:

    void init(Qt::Orientation orientation)
    {
      m_type = RADIO_WIDGET_TRIM;
      m_slider = new SliderWidget(this);
      m_slider->setOrientation(orientation);

      setTrimRange(-125, 125);
      setIndices();

      QSize btnIcnSz(12, 12);
      QWidget * trimWidget = new QWidget(this);
      QBoxLayout * trimLayout = new QVBoxLayout(trimWidget);
      trimLayout->setSpacing(5);
      trimLayout->setContentsMargins(8, 8, 8, 8);
      QToolButton * trimBtnInc = new QToolButton(trimWidget);
      trimBtnInc->setIconSize(btnIcnSz);
      QToolButton * trimBtnDec = new QToolButton(trimWidget);
      trimBtnDec->setIconSize(btnIcnSz);

      Qt::Alignment algn;
      if (orientation == Qt::Horizontal) {
        trimWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        trimLayout->setDirection(QBoxLayout::RightToLeft);
        trimBtnInc->setArrowType(Qt::RightArrow);
        trimBtnDec->setArrowType(Qt::LeftArrow);
        algn = Qt::AlignVCenter;
      }
      else {
        trimWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        trimLayout->setDirection(QBoxLayout::TopToBottom);
        trimBtnInc->setArrowType(Qt::UpArrow);
        trimBtnDec->setArrowType(Qt::DownArrow);
        algn = Qt::AlignHCenter;
      }

      trimLayout->addWidget(trimBtnInc, 0, algn);
      trimLayout->addWidget(m_slider, 1, algn);
      trimLayout->addWidget(trimBtnDec, 0, algn);

      setWidget(trimWidget, algn);

      connect(m_slider, &SliderWidget::valueChanged, this, &RadioWidget::setValue);
      connect(this, &RadioWidget::valueChanged, this, &RadioTrimWidget::onValueChanged);

      connect(trimBtnInc, &QToolButton::pressed,  [this]() { emit trimButtonPressed(m_btnIncIndex);  });
      connect(trimBtnInc, &QToolButton::released, [this]() { emit trimButtonReleased(m_btnIncIndex); });
      connect(trimBtnDec, &QToolButton::pressed,  [this]() { emit trimButtonPressed(m_btnDecIndex);  });
      connect(trimBtnDec, &QToolButton::released, [this]() { emit trimButtonReleased(m_btnDecIndex); });
    }

    void onValueChanged(int value)
    {
      m_slider->blockSignals(true);
      m_slider->setValue(value);
      m_slider->blockSignals(false);
      emit trimSliderMoved(m_index, m_value);
    }

  protected:
    SliderWidget * m_slider;
    int m_btnDecIndex;
    int m_btnIncIndex;
};


#endif // _RADIOTRIMWIDGET_H_
