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
#include "progresswidget.h"
#include <QDirIterator>
#include <QDateTime>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

SyncProcess::SyncProcess(const QString & folder1, const QString & folder2, ProgressWidget * progress):
  folder1(folder1),
  folder2(folder2),
  progress(progress),
  index(0),
  count(0),
  closed(false)
{
  connect(progress, SIGNAL(stopped()),this, SLOT(onClosed()));
}

void SyncProcess::onClosed()
{
  closed = true;
}

bool SyncProcess::run()
{
  if (!QFile::exists(folder1)) {
    QMessageBox::warning(NULL, QObject::tr("Synchronization error"), QObject::tr("The directory '%1' doesn't exist!").arg(folder1));
    return true;
  }

  if (!QFile::exists(folder2)) {
    QMessageBox::warning(NULL, QObject::tr("Synchronization error"), QObject::tr("The directory '%1' doesn't exist!").arg(folder2));
    return true;
  }

  count = getFilesCount(folder1) + getFilesCount(folder2);
  progress->setMaximum(count);

  QStringList errors = updateDir(folder1, folder2) + updateDir(folder2, folder1);
  if (errors.count() > 0) {
    QMessageBox::warning(NULL, QObject::tr("Synchronization error"), errors.join("\n"));
  }

  // don't close the window unless the user wanted
  return closed;
}

int SyncProcess::getFilesCount(const QString & directory)
{
  int result = 0;
  QDirIterator it(directory, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    it.next();
    result++;
  }
  return result;
}

QStringList SyncProcess::updateDir(const QString & source, const QString & destination)
{
  QDirIterator it(source, QDirIterator::Subdirectories);
  while (!closed && it.hasNext()) {
    QEventLoop loop;
    QTimer::singleShot(10, &loop, SLOT(quit()));
    loop.exec();
    index++;
    progress->setInfo(tr("%1/%2 files").arg(index).arg(count));
    progress->setValue(index);
    QString result = updateEntry(it.next(), source, destination);
    if (!result.isEmpty()) {
      errors << result;
    }
  }
  return errors;
}

QString SyncProcess::updateEntry(const QString & path, const QDir & source, const QDir & destination)
{
  QFileInfo sourceInfo(path);
  QString relativePath = source.relativeFilePath(path);
  QString destinationPath = destination.absoluteFilePath(relativePath);
  QFileInfo destinationInfo(destinationPath);
  if (sourceInfo.isDir()) {
    if (!destinationInfo.exists()) {
      progress->addText(tr("Create directory %1\n").arg(destinationPath));
      if (!destination.mkdir(relativePath)) {
        return QObject::tr("Create '%1' failed").arg(destinationPath);
      }
    }
  }
  else {
    if (!destinationInfo.exists()) {
      // qDebug() << "Copy" << path << "to" << destinationPath;
      progress->addText(tr("Copy %1 to %2").arg(path).arg(destinationPath) + "\n");
      if (!QFile::copy(path, destinationPath)) {
        return QObject::tr("Copy '%1' to '%2' failed").arg(path).arg(destinationPath);
      }
    }
    else if (sourceInfo.lastModified() > destinationInfo.lastModified()) {
      // retrieve source contents
      QFile sourceFile(path);
      if (!sourceFile.open(QFile::ReadOnly)) {
        return QObject::tr("Open '%1' failed").arg(path);
      }
      QString sourceContents = sourceFile.readAll();
      sourceFile.close();
      // try to retrieve destination contents
      QFile destinationFile(destinationPath);
      if (destinationFile.open(QFile::ReadOnly)) {
        QString destinationContents = destinationFile.readAll();
        destinationFile.close();
        if (sourceContents == destinationContents) {
          // qDebug() << "Skip" << path;
          return QString();
        }
      }
      if (!destinationFile.open(QFile::WriteOnly)) {
        return QObject::tr("Write '%1' failed").arg(destinationPath);
      }
      progress->addText(tr("Write %1").arg(destinationPath) + "\n");
      // qDebug() << "Write" << destinationPath;
      QTextStream destinationStream(&destinationFile);
      destinationStream << sourceContents;
      destinationFile.close();
    }
  }
  return QString();
}
