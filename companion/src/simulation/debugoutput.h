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

#ifndef _DEBUGOUTPUT_H_
#define _DEBUGOUTPUT_H_

#include "simulator.h"
#include "simulatorinterface.h"

#include <QMutex>
#include <QTimer>
#include <QValidator>
#include <QWidget>

// NOTE : The buffer sizes need to be large enough to handle the flood of data when X12/X10 simulator starts up (almost 40K!).
#ifndef DEBUG_OUTPUT_WIDGET_OUT_BUFF_SIZE
  // This buffer holds received and processed data until it can be printed to our console.
  #define DEBUG_OUTPUT_WIDGET_OUT_BUFF_SIZE    (40 * 1024)  // [bytes]
#endif
#ifndef DEBUG_OUTPUT_WIDGET_INP_BUFF_SIZE
  // This buffer is active if line filter is enabled and holds received data until it can be filtered and placed in output buffer.
  #define DEBUG_OUTPUT_WIDGET_INP_BUFF_SIZE    (30 * 1024)  // [bytes]
#endif

namespace Ui {
  class DebugOutput;
}

class QAbstractButton;
class FilteredTextBuffer;

using namespace Simulator;

class DebugOutput : public QWidget
{
  Q_OBJECT

  public:
    explicit DebugOutput(QWidget * parent, SimulatorInterface * simulator);
    virtual ~DebugOutput();

    static QRegularExpression makeRegEx(const QString & input, bool * isExlusive = NULL);

    static FilteredTextBuffer * m_dataBufferDevice;

  signals:
    void filterExprChanged(const QRegularExpression & expr);
    void filterEnabledChanged(const bool enabled);
    void filterExclusiveChanged(const bool exlusive);
    void filterChanged(bool enable, bool exclusive, const QRegularExpression & expr);

  protected slots:
    void saveState();
    void restoreState();
    void processBytesReceived();
    void onDataBufferOverflow(const qint64 len);
    void onFilterStateChanged();
    void onFilterTextChanged(const QString &);
    void onFilterToggled(bool enable);
    void on_bufferSize_editingFinished();
    void on_actionWordWrap_toggled(bool checked);
    void on_actionClearScr_triggered();
    void on_actionShowFilterHelp_triggered();

  protected:
    Ui::DebugOutput * ui;
    SimulatorInterface * m_simulator;
    int m_radioProfileId;
    bool m_filterEnable;
    bool m_filterExclude;

    const static quint16 m_savedViewStateVersion;
};

class DebugOutputFilterValidator : public QValidator
{
  public:
    DebugOutputFilterValidator(QObject *parent = Q_NULLPTR) : QValidator(parent) { }
    virtual State validate(QString & input, int &) const;
};

class DeleteComboBoxItemEventFilter : public QObject
{
  Q_OBJECT
  protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // _DEBUGOUTPUT_H_

