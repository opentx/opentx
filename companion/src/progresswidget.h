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

#ifndef _PROGRESSWIDGET_H_
#define _PROGRESSWIDGET_H_

#include <QWidget>

#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0))
  #define QtInfoMsg    QtMsgType(4)
#endif

namespace Ui {
  class ProgressWidget;
}

class ProgressWidget : public QWidget
{
  Q_OBJECT

  public:
    explicit ProgressWidget(QWidget *parent);
    ~ProgressWidget();
    void lock(bool lock);
    void addText(const QString &text, const bool richText = false);
    void addHtml(const QString &text);
    void addMessage(const QString & text, const int & type = QtInfoMsg);
    QString getText();
    void setInfo(const QString &text);
    void setMaximum(int value);
    int maximum();
    void setValue(int value);
    void setProgressColor(const QColor &color);
    void addSeparator();
    void forceOpen();
    void stop();
    bool isEmpty() const;

  signals:
    void detailsToggled();
    void locked(bool);
    void stopped();

  protected slots:
    void on_checkBox_toggled(bool checked);
    void shrink();

  private:
    Ui::ProgressWidget *ui;
};

#endif // _PROGRESSWIDGET_H_
