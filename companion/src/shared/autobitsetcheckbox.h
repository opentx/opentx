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

#include <QCheckBox>
#include "genericpanel.h"

class AutoBitsetCheckBox: public QCheckBox
{
    Q_OBJECT
    //! \property bitmask The value (shifted bit(s)) to set/clear based on checked state.
    Q_PROPERTY(int bitmask READ bitmask WRITE setBitmask)
    //! \property inverted If \e true, then the \p bitmask is cleared when the checkbox is checked, and vice versa. Default behavior is to set the bitmask when checked.
    Q_PROPERTY(bool inverted READ inverted WRITE setInverted)
    //! \property toggleMask If > 0, then set this value when clearing the \p bitmask value (toggles between the two masks being set). \p inverted is ignored if \p toggleMask is set.
    Q_PROPERTY(int toggleMask READ toggleMask WRITE setToggleMask)

  public:
    explicit AutoBitsetCheckBox(QWidget *parent = nullptr) :
      AutoBitsetCheckBox(QString(), parent) {}
    explicit AutoBitsetCheckBox(const QString &text, QWidget *parent = nullptr) :
      QCheckBox(text, parent)
    {
      init();
    }

    // int field constructors
    explicit AutoBitsetCheckBox(int & field, int bitmask, const QString &text = QString(), QWidget *parent = nullptr) :
      AutoBitsetCheckBox(field, bitmask, false, text, parent) {}
    explicit AutoBitsetCheckBox(int & field, int bitmask, bool invert, const QString &text = QString(), QWidget *parent = nullptr) :
      QCheckBox(text, parent)
    {
      setField(field, bitmask, invert);
      init();
    }
    explicit AutoBitsetCheckBox(int & field, int bitmask, int toggleMask, const QString &text = QString(), QWidget *parent = nullptr) :
      QCheckBox(text, parent)
    {
      setField(field, bitmask, false, toggleMask);
      init();
    }

    // unsigned field constructors
    explicit AutoBitsetCheckBox(unsigned & field, int bitmask, const QString &text = QString(), QWidget *parent = nullptr) :
      AutoBitsetCheckBox(field, bitmask, false, text, parent) {}
    explicit AutoBitsetCheckBox(unsigned & field, int bitmask, bool invert, const QString &text = QString(), QWidget *parent = nullptr) :
      QCheckBox(text, parent)
    {
      setField(field, bitmask, invert);
      init();
    }
    explicit AutoBitsetCheckBox(unsigned & field, int bitmask, int toggleMask, const QString &text = QString(), QWidget *parent = nullptr) :
      QCheckBox(text, parent)
    {
      setField(field, bitmask, false, toggleMask);
      init();
    }

    inline int  bitmask()    const { return m_bitmask; }
    inline int  toggleMask() const { return m_toggleMask; }
    inline bool inverted()   const { return m_toggleMask ? false : m_invert; }

  public slots:
    void setField(int & field, int bitmask = 0, bool invert = false, int toggleMask = 0)
    {
      m_field = &field;
      initField(bitmask, invert, toggleMask);
    }

    void setField(unsigned & field, int bitmask = 0, bool invert = false, int toggleMask = 0)
    {
      m_field = reinterpret_cast<int *>(&field);
      initField(bitmask, invert, toggleMask);
    }

    void setBitmask(int bitmask)
    {
      m_bitmask = bitmask;
      updateValue();
    }

    void setToggleMask(int toggleMask)
    {
      m_toggleMask = toggleMask;
      updateValue();
    }

    void setInverted(bool invert)
    {
      m_invert = invert;
      updateValue();
    }

    void updateValue()
    {
      if (m_panel && m_panel->lock)
        return;
      if (!m_field)
        return;
      const bool oldLock = setLocked(true);
      setChecked((m_invert && !m_toggleMask) != bool(*m_field & m_bitmask));
      setLocked(oldLock);
    }

    void setPanel(GenericPanel * panel)
    {
      m_panel = panel;
    }

  protected slots:
    void init()
    {
      connect(this, &AutoBitsetCheckBox::toggled, this, &AutoBitsetCheckBox::onToggled);
    }

    void initField(int bitmask, bool invert, int toggleMask)
    {
      m_bitmask = bitmask;
      m_toggleMask = toggleMask;
      m_invert = toggleMask ? false : invert;
      updateValue();
    }

    void onToggled(bool checked)
    {
      if (!m_field)
        return;

      if (m_toggleMask) {
        if (checked) {
          *m_field &= ~m_toggleMask;
          *m_field |= m_bitmask;
        }
        else {
          *m_field &= ~m_bitmask;
          *m_field |= m_toggleMask;
        }
      }
      else {
        if (m_invert)
          checked = !checked;
        if (checked)
          *m_field |= m_bitmask;
        else
          *m_field &= ~m_bitmask;
      }

      if (m_panel && !m_lock)
        emit m_panel->modified();
    }

    bool setLocked(bool lock)
    {
      const bool ret = m_lock;
      m_lock = lock;
      return ret;
    }

  protected:
    int * m_field = nullptr;
    int m_bitmask = 0;
    int m_toggleMask = 0;
    bool m_invert = false;
    bool m_lock = false;
    GenericPanel * m_panel = nullptr;
};
