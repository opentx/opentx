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

#include "process_sync.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QMutexLocker>
#include <QDirIterator>
#include <QDebug>

#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0))
#define QtInfoMsg    QtMsgType(4)
#endif

#define PRINT_INFO(str)       emit progressMessage((str), QtInfoMsg)
#define PRINT_CREATE(str)     emit progressMessage((str), QtInfoMsg)
#define PRINT_REPLACE(str)    emit progressMessage((str), QtWarningMsg)
#define PRINT_DELETE(str)     emit progressMessage((str), QtCriticalMsg)
#define PRINT_ERROR(str)      emit progressMessage((str), QtFatalMsg)
//#define PRINT_SKIP(str)     emit progressMessage((str), QtDebugMsg)  // mostly useless noise (maybe make an option later)
#define PRINT_SKIP(str)
#define PRINT_SEP()           PRINT_INFO(QString(80, '='))

#define SYNC_MAX_ERRORS         50  // give up after this many errors per destination

SyncProcess::SyncProcess(const QString & folderA, const QString & folderB, const int & syncDirection, const int & compareType, const qint64 & maxFileSize, const bool dryRun):
  folder1(folderA),
  folder2(folderB),
  direction((SyncDirection)syncDirection),
  ctype((SyncCompareType)compareType),
  maxFileSize(qMax<qint64>(0, maxFileSize)),
  dryRun(dryRun),
  stopping(false)
{
  if (direction == SYNC_B2A_A2B) {
    folder1 = folderB;
    folder2 = folderA;
    direction = SYNC_A2B_B2A;
  }

  if (ctype == OVERWR_ALWAYS && direction == SYNC_A2B_B2A)
    ctype = OVERWR_IF_DIFF;

  dirFilters = QDir::Filters(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

  reportTemplate = tr("New: <b>%1</b>; Updated: <b>%2</b>; Skipped: <b>%3</b>; Errors: <font color=%5><b>%4</b></font>;");
  if (dryRun)
    testRunStr = tr("[TEST RUN] ");
}

void SyncProcess::stop()
{
  QMutexLocker locker(&stopReqMutex);
  stopping = true;
}

bool SyncProcess::isStopRequsted()
{
  QMutexLocker locker(&stopReqMutex);
  return stopping;
}

void SyncProcess::run()
{
  count = index = created = updated = skipped = errored = 0;

  emit started();
  emit progressStep(index);
  emit statusMessage(tr("Gathering file information..."));

  if (direction == SYNC_A2B_B2A || direction == SYNC_A2B)
    count += getFilesCount(folder1);

  if (isStopRequsted()) {
    finish();
    return;
  }

  if (direction == SYNC_A2B_B2A || direction == SYNC_B2A)
    count += getFilesCount(folder2);

  if (isStopRequsted()) {
    finish();
    return;
  }

  emit fileCountChanged(count);

  if (!count) {
    QString nf = tr("Synchronization failed, nothing found to copy.");
    emit statusMessage(nf);
    PRINT_ERROR(nf);
    emit finished();
    return;
  }

  if (direction == SYNC_A2B_B2A || direction == SYNC_A2B)
    updateDir(folder1, folder2);

  if (isStopRequsted()) {
    finish();
    return;
  }

  if (direction == SYNC_A2B_B2A || direction == SYNC_B2A)
    updateDir(folder2, folder1);

  finish();
}

void SyncProcess::finish()
{
  QString endStr = testRunStr % tr("Synchronization finished. ") % reportTemplate;
  emit statusMessage(endStr.arg(created).arg(updated).arg(skipped).arg(errored).arg(errored ? "red" : "black"));
  emit finished();
}

int SyncProcess::getFilesCount(const QString & directory)
{
  if (!QFile::exists(directory))
    return 0;

  int result = 0;
  QDirIterator it(directory, dirFilters, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
  while (it.hasNext() && !isStopRequsted()) {
    it.next();
    result++;
    QApplication::processEvents();
  }
  return result;
}

void SyncProcess::updateDir(const QString & source, const QString & destination)
{
  int counts[4] = { created, updated, skipped, errored };
  QString statusStr =  testRunStr % tr("Synchronizing %1 -&gt; %2: %3").arg(source, destination, "<b>%1</b>|<b>%2</b> (%3)");

  PRINT_INFO(testRunStr % tr("Starting synchronization: %1 -&gt; %2<br>").arg(source, destination));

  QDirIterator it(source, dirFilters, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
  while (it.hasNext() && !isStopRequsted()) {
    it.next();
    ++index;
    emit statusMessage(statusStr.arg(index).arg(count).arg(reportTemplate.arg(created).arg(updated).arg(skipped).arg(errored).arg(errored ? "red" : "black")));
    emit progressStep(index);
    if (maxFileSize && it.fileInfo().isFile() && it.fileInfo().size() > maxFileSize) {
      PRINT_SKIP(tr("Skipping large file: %1 (%2KB)").arg(it.fileName()).arg(int(it.fileInfo().size() / 1024)));
      ++skipped;
    }
    else {
      updateEntry(it.filePath(), source, destination);
      if (errored - counts[3] > SYNC_MAX_ERRORS) {
        PRINT_ERROR(tr("<br><b>Too many errors, giving up.<b>"));
        break;
      }
    }
    QApplication::processEvents();
  }

  QString endStr = "<br>" % testRunStr % tr("Finished synchronizing %1 -&gt; %2 :<br>&nbsp;&nbsp;&nbsp;&nbsp; %3").arg(source, destination, reportTemplate);
  PRINT_INFO(endStr.arg(created-counts[0]).arg(updated-counts[1]).arg(skipped-counts[2]).arg(errored-counts[3]).arg(errored-counts[3] ? "red" : "black"));
  PRINT_SEP();
}

bool SyncProcess::updateEntry(const QString & entry, const QDir & source, const QDir & destination)
{
  QString srcPath = source.toNativeSeparators(source.absoluteFilePath(entry));
  QString relPath = source.relativeFilePath(entry);
  QString destPath = destination.toNativeSeparators(destination.absoluteFilePath(relPath));
  QFileInfo sourceInfo(srcPath);
  QFileInfo destInfo(destPath);

  if (sourceInfo.isDir()) {
    if (!destInfo.exists()) {
      PRINT_CREATE(tr("Creating directory: %1").arg(destPath));
      if (!dryRun && !destination.mkpath(destPath)) {
        PRINT_ERROR(tr("Could not create directory: %1").arg(destPath));
        ++errored;
        return false;
      }
      ++created;
    }
    else {
      PRINT_SKIP(tr("Destination directory exists: %1").arg(destPath));
      ++skipped;
    }
    return true;
  }

  QFile sourceFile(srcPath);
  QFile destinationFile(destPath);
  bool destExists = destInfo.exists();
  bool checkDate = (ctype == OVERWR_NEWER_IF_DIFF || ctype == OVERWR_NEWER_ALWAYS);
  bool checkContent = (ctype == OVERWR_NEWER_IF_DIFF || ctype == OVERWR_IF_DIFF);
  bool existed = false;

  if (destExists && checkDate) {
    if (sourceInfo.lastModified() <= destInfo.lastModified()) {
      PRINT_SKIP(tr("Skipping older file: %1").arg(srcPath));
      ++skipped;
      return true;
    }
    checkDate = false;
  }

  if (destExists && checkContent) {
    if (!sourceFile.open(QFile::ReadOnly)) {
      PRINT_ERROR(tr("Could not open source file '%1': %2").arg(srcPath, sourceFile.errorString()));
      ++errored;
      return false;
    }
    if (!destinationFile.open(QFile::ReadOnly)) {
      PRINT_ERROR(tr("Could not open destination file '%1': %2").arg(destPath, destinationFile.errorString()));
      ++errored;
      return false;
    }

    bool skip =  QCryptographicHash::hash(sourceFile.readAll(), QCryptographicHash::Md5) == QCryptographicHash::hash(destinationFile.readAll(), QCryptographicHash::Md5);
    sourceFile.close();
    destinationFile.close();
    if (skip) {
      PRINT_SKIP(tr("Skipping identical file: %1").arg(srcPath));
      ++skipped;
      return true;
    }
    checkContent = false;
  }

  if (!destExists || (!checkDate && !checkContent)) {
    if (destInfo.exists()) {
      existed = true;
      PRINT_REPLACE(tr("Replacing destination file: %1").arg(destPath));
      if (!dryRun && !destinationFile.remove()) {
        PRINT_ERROR(tr("Could not delete destination file '%1': %2").arg(destPath, destinationFile.errorString()));
        ++errored;
        return false;
      }
    }
    else {
      PRINT_CREATE(tr("Creating destination file: %1").arg(destPath));
    }
    if (!dryRun && !sourceFile.copy(destPath)) {
      PRINT_ERROR(tr("Copy failed: '%1' to '%2': %3").arg(srcPath, destPath, sourceFile.errorString()));
      ++errored;
      return false;
    }

    if (existed)
      ++updated;
    else
      ++created;
  }

  return true;
}
