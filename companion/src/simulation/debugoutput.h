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

namespace Ui {
  class DebugOutput;
}

class QAbstractButton;

using namespace Simulator;

class DebugOutput : public QWidget
{
  Q_OBJECT

  public:
    explicit DebugOutput(QWidget * parent, SimulatorInterface * simulator);
    virtual ~DebugOutput();
    void start();
    void stop();
    void traceCallback(const char * text);

    static QRegularExpression makeRegEx(const QString & input, bool * isExlusive = NULL);

  protected slots:
    void saveState();
    void restoreState();
    void processBytesReceived();
    void onFilterTextEdited();
    void onFilterTextChanged(const QString &);
    void on_bufferSize_editingFinished();
    void on_actionWordWrap_toggled(bool checked);
    void on_actionClearScr_triggered();
    void on_actionShowFilterHelp_triggered();

  protected:
    Ui::DebugOutput * ui;
    SimulatorInterface * m_simulator;
    QTimer * m_tmrDataPrint;
    QStringList m_dataBuffer;
    QMutex m_mtxDataBuffer;
    QRegularExpression m_filterRegEx;

    int m_radioProfileId;
    int m_dataPrintFreq;
    bool m_running;
    bool m_filterExclude;

    const static int m_dataBufferMaxSize;
    const static int m_dataPrintFreqDefault;
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

