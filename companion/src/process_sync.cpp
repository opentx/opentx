#include "process_sync.h"
#include "progresswidget.h"
#include <QDirIterator>
#include <QDateTime>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

SyncProcess::SyncProcess(const QString &folder1, const QString &folder2, ProgressWidget *progress):
folder1(folder1),
folder2(folder2),
progress(progress),
simulation(false),
index(0)
{
}

void SyncProcess::run()
{
  simulation = true;
  index = 0;
  if (synchronize()) {
    int count = index;
    progress->setMaximum(count);
    simulation = false;
    index = 0;
    synchronize();
    progress->setValue(count);
  }
}

bool SyncProcess::synchronize()
{
  if (!QFile::exists(folder1)) {
    QMessageBox::warning(NULL, QObject::tr("Synchronization error"), QObject::tr("The directory '%1' doesn't exist!").arg(folder1));
    return false;
  }
  if (!QFile::exists(folder2)) {
    QMessageBox::warning(NULL, QObject::tr("Synchronization error"), QObject::tr("The directory '%1' doesn't exist!").arg(folder2));
    return false;
  }
  QStringList errors = updateDir(folder1, folder2) + updateDir(folder2, folder1);
  if (errors.count() > 0) {
    QMessageBox::warning(NULL, QObject::tr("Synchronization error"), errors.join("\n"));
    return false;
  }
  return true;
}

QStringList SyncProcess::updateDir(const QDir &source, const QDir &destination)
{
  QDirIterator it(source, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    if (!simulation) {
      progress->setValue(index);
    }
    QString path = it.next();
    // qDebug() << path;
    QFileInfo sourceInfo(path);
    QString relativePath = source.relativeFilePath(path);
    QString destinationPath = destination.absoluteFilePath(relativePath);
    QFileInfo destinationInfo(destinationPath);
    if (sourceInfo.isDir()) {
      if (!destinationInfo.exists()) {
        ++index;
        if (!simulation) {
          progress->addText(tr("Create directory %1\n").arg(destinationPath));
          if (!destination.mkdir(relativePath)) {
            errors << QObject::tr("Create '%1' failed").arg(destinationPath);
            continue;
          }
        }
      }
    }
    else {
      if (!destinationInfo.exists()) {
        // qDebug() << "Copy" << path << "to" << destinationPath;
        ++index;
        if (!simulation) {
          progress->addText(tr("Copy %1 to %2\n").arg(path).arg(destinationPath));
          if (!QFile::copy(path, destinationPath)) {
            errors << QObject::tr("Copy '%1' to '%2' failed").arg(path).arg(destinationPath);
            continue;
          }
        }
      }
      else if (sourceInfo.lastModified() > destinationInfo.lastModified()) {
        ++index;
        if (!simulation) {
          progress->addText(tr("Read %1\n").arg(path));
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
          progress->addText(tr("Write %1\n").arg(destinationPath));
          // qDebug() << "Write" << destinationPath;
          QTextStream destinationStream(&destinationFile);
          destinationStream << sourceContents;
          destinationFile.close();
        }
      }
    }
  }
  return errors;
}
