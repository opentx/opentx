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

#include <QGridLayout>
#include <QDebug>
#include <QLabel>
#include <QWidget>

#define RADIO_WIDGET_STATE_VERSION    1

class RadioWidget : public QWidget
{
  Q_OBJECT

  public:

    // These are saved to persistent user settings in RadioWidgetState,
    //   so only append entires here, do not change index value if it can be avoided.
    enum RadioWidgetType {
      RADIO_WIDGET_NONE = 0,
      RADIO_WIDGET_SWITCH,
      RADIO_WIDGET_KNOB,
      RADIO_WIDGET_FADER,
      RADIO_WIDGET_STICK   // actually one axis of a stick
    };

    struct RadioWidgetState {
      public:
        RadioWidgetState(quint8 type = 0, qint8 index = 0, qint16 value = 0, quint16 flags = 0) :
          type(type), index(index), value(value), flags(flags),
          _version(RADIO_WIDGET_STATE_VERSION)
        { }

        quint8 type;
        qint8 index;
        qint16 value;
        quint16 flags;

        friend QDataStream & operator << (QDataStream &out, const RadioWidgetState & o);
        friend QDataStream & operator >> (QDataStream &in, RadioWidgetState & o);
        friend QDebug operator << (QDebug d, const RadioWidgetState & o);

      private:
        quint8 _version = RADIO_WIDGET_STATE_VERSION;  // structure definition version
    };

    explicit RadioWidget(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    explicit RadioWidget(const QString &labelText, int value = 0, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

    void setIndex(int index);
    void setInvertValue(bool invertValue);
    void setValue(int value);
    void setFlags(quint16 flags);
    void setShowLabel(bool show);
    void setLabelText(const QString &labelText, bool showLabel = true);
    void setStateData(const QByteArray & data);
    void changeVisibility(bool visible);


    virtual int getValue() const;
    int getIndex() const;
    int getType()  const;
    QByteArray getStateData() const;
    RadioWidgetState getState() const;

  protected:

    void init();
    void addLabel();
    void setWidget(QWidget * widget = NULL);

    int m_value;
    int m_index;
    quint16 m_flags;
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
    void flagsChanged(quint16 flags);

};

#endif // _RADIOWIDGET_H_
