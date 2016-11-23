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

#ifndef _PROCESS_SYNC_H_
#define _PROCESS_SYNC_H_

#include <QObject>
#include <QString>
#include <QStringList>

class QDir;
class ProgressWidget;

class SyncProcess : public QObject
{
    Q_OBJECT

  public:
    SyncProcess(const QString & folder1, const QString & folder2, ProgressWidget * progress);
    bool run();

  protected slots:
    void onClosed();

  protected:
    int getFilesCount(const QString & directory);
    QStringList updateDir(const QString & source, const QString & destination);
    QString updateEntry(const QString & path, const QDir & source, const QDir & destination);
    QString folder1;
    QString folder2;
    ProgressWidget * progress;
    QStringList errors;
    int index;
    int count;
    bool closed;
};

#endif // _PROCESS_SYNC_H_
