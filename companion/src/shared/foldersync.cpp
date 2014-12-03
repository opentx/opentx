#include "foldersync.h"
#include <QDirIterator>
#include <QDateTime>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

FoldersSyncTask::FoldersSyncTask(const QString &folder1, const QString &folder2)
{
  folders[0] = folder1;
  folders[1] = folder2;
}

void FoldersSyncTask::run()
{
  if (!QFile::exists(folders[0])) {
    QMessageBox::warning(NULL, QObject::tr("Synchronization error"), QObject::tr("The directory '%1' doesn't exist!").arg(folders[0]));
    return;
  }
  if (!QFile::exists(folders[1])) {
    QMessageBox::warning(NULL, QObject::tr("Synchronization error"), QObject::tr("The directory '%1' doesn't exist!").arg(folders[1]));
    return;
  }
  QStringList errors = updateDir(folders[0], folders[1]) + updateDir(folders[1], folders[0]);
  if (errors.count() > 0) {
    QMessageBox::warning(NULL, QObject::tr("Synchronization error"), errors.join("\n"));
  }
}

QStringList FoldersSyncTask::updateDir(const QDir &source, const QDir &destination)
{
  QStringList errors;
  QDirIterator it(source, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    QString path = it.next();
    // qDebug() << path;
    QFileInfo sourceInfo(path);
    QString relativePath = source.relativeFilePath(path);
    QString destinationPath = destination.absoluteFilePath(relativePath);
    QFileInfo destinationInfo(destinationPath);
    if (sourceInfo.isDir()) {
      if (!destinationInfo.exists()) {
        if (!destination.mkdir(relativePath)) {
          errors << QObject::tr("Create '%1' failed").arg(destinationPath);
          continue;
        }
      }
    }
    else {
      if (!destinationInfo.exists()) {
        qDebug() << "Copy" << path << "to" << destinationPath;
        if (!QFile::copy(path, destinationPath)) {
          errors << QObject::tr("Copy '%1' to '%2' failed").arg(path).arg(destinationPath);
          continue;
        }
      }
      else if (sourceInfo.lastModified() > destinationInfo.lastModified()) {
        // retrieve source contents
        QFile sourceFile(path);
        if (!sourceFile.open(QFile::ReadOnly)) {
          errors << QObject::tr("Open '%1' failed").arg(path);
          continue;
        }
        QString sourceContents = sourceFile.readAll();
        sourceFile.close();
        // try to retrieve destination contents
        QFile destinationFile(path);
        if (destinationFile.open(QFile::ReadOnly)) {
          QString destinationContents = destinationFile.readAll();
          destinationFile.close();
          if (sourceContents == destinationContents) {
            // qDebug() << "Skip" << path;
            continue;
          }
        }
        if (!destinationFile.open(QFile::WriteOnly)) {
          errors << QObject::tr("Write '%1' failed").arg(destinationPath);
          continue;
        }
        qDebug() << "Write" << destinationPath;
        QTextStream destinationStream(&destinationFile);
        destinationStream << sourceContents;
        destinationFile.close();
      }
    }
  }
  return errors;
}
