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

#ifndef _RADIOWIDGET_H_
#define _RADIOWIDGET_H_

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

class RadioWidget : public QWidget
{
  Q_OBJECT

  public:
    enum RadioWidgetType { RADIO_WIDGET_NONE, RADIO_WIDGET_SWITCH, RADIO_WIDGET_KNOB, RADIO_WIDGET_FADER };

    explicit RadioWidget(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
      QWidget(parent, f),
      m_value(0),
      m_invertValue(false),
      m_showLabel(false),
      m_labelText(""),
      m_type(RADIO_WIDGET_NONE)
    {
      init();
    }
    explicit RadioWidget(const QString &labelText, int value = 0, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
      QWidget(parent, f),
      m_value(value),
      m_invertValue(false),
      m_showLabel(true),
      m_labelText(labelText),
      m_type(RADIO_WIDGET_NONE)
    {
      init();
    }

    void setIndex(int index)              { m_index = index; }
    void setInvertValue(bool invertValue) { m_invertValue = invertValue; }

    void setValue(int value)
    {
      if (value != m_value) {
        m_value = value;
        emit valueChanged(m_value);
      }
    }

    void setShowLabel(bool show)
    {
      if (show != m_showLabel) {
        m_showLabel = show;
        addLabel();
      }
    }

    void setLabelText(const QString &labelText, bool showLabel = true)
    {
      m_labelText = labelText;
      m_showLabel = showLabel;
      addLabel();
    }

    void changeVisibility(bool visible)
    {
      static QSizePolicy oldSP = sizePolicy();
      setVisible(visible);
      if (visible) {
        setSizePolicy(oldSP);
      }
      else {
        oldSP = sizePolicy();
        setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
      }
    }

    int getValue() const { return m_value * (m_invertValue ? -1 : 1); }
    int getIndex() const { return m_index; }
    int getType()  const { return m_type;  }


  protected:
    void init()
    {
      setIndex(0);
      m_controlWidget = NULL;
      m_nameLabel = NULL;

      m_gridLayout= new QGridLayout(this);
      m_gridLayout->setContentsMargins(0, 0, 0, 0);
      m_gridLayout->setVerticalSpacing(4);
      m_gridLayout->setHorizontalSpacing(0);

      addLabel();
    }

    void addLabel()
    {
      if (m_nameLabel) {
        m_gridLayout->removeWidget(m_nameLabel);
        m_nameLabel->deleteLater();
      }
      if (m_showLabel && !m_labelText.isEmpty()) {
        m_nameLabel = new QLabel(m_labelText, this);
        m_nameLabel->setAlignment(Qt::AlignCenter);
        m_nameLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        m_gridLayout->addWidget(m_nameLabel, 1, 0, 1, 1, Qt::AlignTop);
        m_gridLayout->setRowStretch(1, 0);
      }
    }

    void setWidget(QWidget * widget = NULL)
    {
      if (m_controlWidget) {
        m_gridLayout->removeWidget(m_controlWidget);
        m_controlWidget->deleteLater();
      }
      m_controlWidget = widget;
      if (widget) {
        m_gridLayout->addWidget(widget, 0, 0, 1, 1, Qt::AlignHCenter);
        m_gridLayout->setRowStretch(0, 1);
      }
    }

    int m_value;
    int m_index;
    bool m_invertValue;
    bool m_showLabel;
    QString m_labelText;
    RadioWidgetType m_type;

  private:
    QGridLayout * m_gridLayout;
    QWidget * m_controlWidget;
    QLabel * m_nameLabel;

  signals:
    void valueChanged(int m_value);

};

#endif // _RADIOWIDGET_H_






