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

#pragma once

#include "genericpanel.h"

#include <QComboBox>
#include <QString>

class AutoPrecisionComboBox: public QComboBox
{
    Q_OBJECT
    //! \property minDecimals Determines the first list item
    Q_PROPERTY(unsigned int minDecimals READ minDecimals WRITE setMinDecimals)
    //! \property maxDecimals Determines the last list item
    Q_PROPERTY(unsigned int maxDecimals READ maxDecimals WRITE setMaxDecimals)
    //! \property padding If true, add trailing underscore placeholders to each list item
    Q_PROPERTY(bool padding READ padding WRITE setPadding)
    //! \property suffix Optional text suffix to each list value
    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix)

  public:
    explicit AutoPrecisionComboBox(QWidget * parent = nullptr):
      QComboBox(parent)
    {
      init();
    }

    explicit AutoPrecisionComboBox(unsigned int & field, unsigned int minDecimals = 0, unsigned int maxDecimals = 1, bool padding = false, QString suffix = "", QWidget * parent = nullptr, GenericPanel * panel = nullptr):
      QComboBox(parent)
    {
      setField(field, minDecimals, maxDecimals, padding, suffix, panel);
      init();
    }

    explicit AutoPrecisionComboBox(int & field, unsigned int minDecimals = 0, unsigned int maxDecimals = 1, bool padding = false, QString suffix = "", QWidget * parent = nullptr, GenericPanel * panel = nullptr):
      QComboBox(parent)
    {
      setField(field, minDecimals, maxDecimals, padding, suffix, panel);
      init();
    }

    inline int     minDecimals() const { return m_minDecimals; }
    inline int     maxDecimals() const { return m_maxDecimals; }
    inline bool    padding()     const { return m_padding; }
    inline QString suffix()      const { return m_suffix; }

  public slots:
    void setField(int & field, unsigned int minDecimals = 0, unsigned int maxDecimals = 1, bool padding = false, QString suffix = "", GenericPanel * panel = nullptr)
    {
      m_field = reinterpret_cast<unsigned int *>(&field);
      initField(minDecimals, maxDecimals, padding, suffix);
      setPanel(panel);
    }

    void setField(unsigned int & field, unsigned int minDecimals = 0, unsigned int maxDecimals = 1, bool padding = false, QString suffix = "", GenericPanel * panel = nullptr)
    {
      m_field = &field;
      initField(minDecimals, maxDecimals, padding, suffix);
      setPanel(panel);
    }

    void setMinDecimals(unsigned int minDecimals)
    {
      m_minDecimals = rangecheckDecimals(minDecimals);
      if (m_minDecimals > m_maxDecimals)
        m_minDecimals = m_maxDecimals;
      if (!m_lock)
        updateList();
    }

    void setMaxDecimals(unsigned int maxDecimals)
    {
      m_maxDecimals = rangecheckDecimals(maxDecimals);
      if (m_maxDecimals < m_minDecimals)
        m_maxDecimals = m_minDecimals;
      if (!m_lock)
        updateList();
    }

    void setPadding(bool padding)
    {
      m_padding = padding;
      if (!m_lock)
        updateList();
    }

    void setSuffix(QString suffix)
    {
      m_suffix = suffix.trimmed();
      if (!m_lock)
        updateList();
    }

    void setPanel(GenericPanel * panel)
    {
      m_panel = panel;
    }

    void setValue(int value)
    {
      if (!m_field)
        return;
      const unsigned int val = (unsigned int)value;
      if (*m_field != val) {
        *m_field = rangecheckDecimals(val);
        updateValue();
        emit valueChanged();
      }
    }

    void setValue(unsigned int value)
    {
      if (!m_field)
        return;
      if (*m_field != value) {
        *m_field = rangecheckDecimals(value);
        updateValue();
        emit valueChanged();
      }
    }

    void updateValue()
    {
      if (!m_field)
        return;
      if (!isValidDecimals(*m_field))
        return;
      if (*m_field > m_maxDecimals) {
        setMaxDecimals(*m_field);
      }
      const bool savedlock = setLocked(true);
      setCurrentIndex(findData(*m_field));
      setLocked(savedlock);
    }

    void setAutoIndexes()
    {
      for (int i = 0; i < count(); ++i)
        setItemData(i, i);
      updateValue();
    }

  signals:
    void valueChanged();

  protected slots:
    void init()
    {
      connect(this, QOverload<int>::of(&AutoPrecisionComboBox::currentIndexChanged), this, &AutoPrecisionComboBox::onCurrentIndexChanged);
    }

    void initField(unsigned int minDecimals, unsigned int maxDecimals, bool padding, QString suffix)
    {
      const bool savedlock = setLocked(true);
      setMinDecimals(minDecimals);
      setMaxDecimals(maxDecimals);
      setPadding(padding);
      setSuffix(suffix);
      setLocked(savedlock);
      updateList();
    }

    void updateList()
    {
      const bool savedlock = setLocked(true);
      unsigned int i;
      int j;
      clear();

      for (i = APCB_DECIMALS_MIN, j = (int)m_maxDecimals; j >= 0; i++, j--) {
        if (i >= m_minDecimals) {
          QString str = QString("0.%1").arg(QString(i, '0'));
          if (m_padding)
            str.append(QString(j, '_'));
          if (m_suffix != "")
            str.append(QString(" %1").arg(m_suffix));
          addItem(str, i);
        }
      }

      setSizeAdjustPolicy(QComboBox::AdjustToContents);
      setLocked(savedlock);
    }

    void onCurrentIndexChanged(int index)
    {
      if (index < 0)
        return;
      if (m_field && !m_lock) {
        *m_field = itemData(index).toUInt();
        if (m_panel)
          emit m_panel->modified();
        emit valueChanged();
      }
    }

    unsigned int rangecheckDecimals(unsigned int decimals)
    {
      unsigned int ret;
      if (decimals < APCB_DECIMALS_MIN)
        ret = APCB_DECIMALS_MIN;
      else if (decimals > APCB_DECIMALS_MAX)
        ret = APCB_DECIMALS_MAX;
      else
        ret = decimals;
      return ret;
    }

    bool isValidDecimals(unsigned int value)
    {
      if (value >= APCB_DECIMALS_MIN && value <= APCB_DECIMALS_MAX)
        return true;
      else
        return false;
    }

    bool setLocked(bool lock)
    {
      const bool ret = m_lock;
      m_lock = lock;
      return ret;
    }

  protected:
    constexpr static unsigned int APCB_DECIMALS_MIN = {0};
    constexpr static unsigned int APCB_DECIMALS_MAX = {6};

    unsigned int * m_field = nullptr;
    GenericPanel * m_panel = nullptr;
    unsigned int m_minDecimals = 0;
    unsigned int m_maxDecimals = 0;
    bool m_padding = false;
    QString m_suffix = "";
    bool m_lock = false;
};
