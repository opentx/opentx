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

#ifndef TIMEREDIT_H
#define TIMEREDIT_H

#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QDebug>

class TimerEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(int minimumTime READ minimumTime WRITE setMinimumTime)
    Q_PROPERTY(int maximumTime READ maximumTime WRITE setMaximumTime)
    Q_PROPERTY(bool showSeconds READ showSeconds WRITE setShowSeconds)
    Q_PROPERTY(unsigned int singleStep READ singleStep WRITE setSingleStep)
    Q_PROPERTY(unsigned int pageStep READ pageStep WRITE setPageStep)


  public:
    enum TimeParts {Whole, Polarity, Hours, Minutes, Seconds};

    TimerEdit(QWidget * parent = Q_NULLPTR);

    int timeInSeconds() const;
    int timepart(const QString & input, TimeParts part) const;

    bool showSeconds() const { return m_showSeconds; }
    int minimumTime() const { return m_minTime; }
    int maximumTime() const { return m_maxTime; }
    unsigned int singleStep() const { return m_singleStep; }
    unsigned int pageStep() const { return m_pageStep; }

  public slots:
    void setTime(int seconds);
    void incrDecr(int seconds);
    void setTimeRange(int minSeconds, int maxSeconds);
    void setMinimumTime(int minSeconds);
    void setMaximumTime(int maxSeconds);
    void setShowSeconds(bool showSeconds);

    void setSingleStep(unsigned int step) { m_singleStep = step; }
    void setPageStep(unsigned int pageStep) { m_pageStep = pageStep; }

  protected:
    void textEditedHandler();
    void setupFormat();
    void emitValueChanged();
    void keyPressEvent(QKeyEvent * event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;

    bool m_showSeconds;
    int m_minTime;  // seconds
    int m_maxTime;  // seconds
    unsigned m_singleStep;  // seconds
    unsigned m_pageStep;    // seconds
    short m_hourDigits;
    QRegularExpressionValidator * m_validator;
};

#endif // TIMEREDIT_H
