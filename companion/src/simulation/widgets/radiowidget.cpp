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

#include "radiowidget.h"

RadioWidget::RadioWidget(QWidget * parent, Qt::WindowFlags f) :
  QWidget(parent, f),
  m_value(0),
  m_flags(0),
  m_invertValue(false),
  m_showLabel(false),
  m_labelText(""),
  m_type(RADIO_WIDGET_NONE)
{
  init();
}

RadioWidget::RadioWidget(const QString & labelText, int value, QWidget * parent, Qt::WindowFlags f) :
  QWidget(parent, f),
  m_value(value),
  m_flags(0),
  m_invertValue(false),
  m_showLabel(true),
  m_labelText(labelText),
  m_type(RADIO_WIDGET_NONE)
{
  init();
}

void RadioWidget::setIndex(int index)
{
  m_index = index;
}

void RadioWidget::setInvertValue(bool invertValue)
{
  m_invertValue = invertValue;
}

void RadioWidget::setValue(int value)
{
  if (value != m_value) {
    m_value = value;
    emit valueChanged(m_value);
  }
}

void RadioWidget::setFlags(quint16 flags)
{
  if (flags != m_flags) {
    m_flags = flags;
    emit flagsChanged(flags);
  }
}

void RadioWidget::setShowLabel(bool show)
{
  if (show != m_showLabel) {
    m_showLabel = show;
    addLabel();
  }
}

void RadioWidget::setLabelText(const QString & labelText, bool showLabel)
{
  m_labelText = labelText;
  m_showLabel = showLabel;
  addLabel();
}

void RadioWidget::setStateData(const QByteArray & data)
{
  RadioWidgetState state;
  QDataStream stream(data);
  stream >> state;

  //setIndex(state.index);
  setValue(state.value);
  setFlags(state.flags);
}

void RadioWidget::changeVisibility(bool visible)
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

int RadioWidget::getValue() const
{
  return m_value * (m_invertValue ? -1 : 1);
}

int RadioWidget::getIndex() const
{
  return m_index;
}

int RadioWidget::getType() const
{
  return m_type;
}

QByteArray RadioWidget::getStateData() const
{
  QByteArray ba;
  QDataStream stream(&ba, QIODevice::WriteOnly);
  stream << getState();
  return ba;
}

RadioWidget::RadioWidgetState RadioWidget::getState() const
{
  return RadioWidgetState(quint8(m_type), qint8(m_index), qint16(m_value), quint16(m_flags));;
}

void RadioWidget::init()
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

void RadioWidget::addLabel()
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

void RadioWidget::setWidget(QWidget * widget)
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

QDataStream & operator << (QDataStream &out, const RadioWidget::RadioWidgetState & o)
{
  out << o._version << o.type << o.index << o.value << o.flags;
  return out;
}

QDataStream & operator >> (QDataStream &in, RadioWidget::RadioWidgetState & o)
{
  if (o._version <= RADIO_WIDGET_STATE_VERSION)
    in >> o._version >> o.type >> o.index >> o.value >> o.flags;
  return in;
}

QDebug operator << (QDebug d, const RadioWidget::RadioWidgetState & o)
{
  QDebugStateSaver saver(d);
  d << "RadioWidget::RadioWidgetState: type=" << o.type << "; index=" << o.index
    << "; value=" << o.value << "; flags=0x" << hex << o.flags;
  return d;
}
