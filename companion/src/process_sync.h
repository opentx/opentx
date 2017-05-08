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
#include <QDir>
#include <QMutex>
#include <QString>

class SyncProcess : public QObject
{
    Q_OBJECT

  public:
    enum SyncDirection {
      SYNC_A2B_B2A,
      SYNC_B2A_A2B,
      SYNC_A2B,
      SYNC_B2A
    };

    enum SyncCompareType {
      OVERWR_NEWER_IF_DIFF,
      OVERWR_NEWER_ALWAYS,
      OVERWR_IF_DIFF,
      OVERWR_ALWAYS
    };

    SyncProcess(const QString & folderA,
                const QString & folderB,
                const int & syncDirection = SYNC_A2B_B2A,
                const int & compareType = OVERWR_NEWER_IF_DIFF,
                const qint64 & maxFileSize = 5*1024*1024,
                const bool dryRun = false);

  public slots:
    void run();
    void stop();

  signals:
    void started();
    void finished();
    void fileCountChanged(int count);
    void progressStep(int step);
    void progressMessage(const QString & text, const int & type);
    void statusMessage(const QString & text);

  protected:
    bool isStopRequsted();
    void finish();
    int getFilesCount(const QString & directory);
    void updateDir(const QString & source, const QString & destination);
    bool updateEntry(const QString & entry, const QDir & source, const QDir & destination);

    QString folder1;
    QString folder2;
    SyncDirection direction;
    SyncCompareType ctype;
    qint64 maxFileSize;  // Bytes
    QDir::Filters dirFilters;
    QMutex stopReqMutex;
    QString reportTemplate;
    QString testRunStr;
    int index;
    int count;
    int created;
    int updated;
    int skipped;
    int errored;
    bool dryRun;
    bool stopping;
};

#endif // _PROCESS_SYNC_H_
