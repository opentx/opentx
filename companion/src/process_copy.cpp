#include "process_copy.h"
#include "progresswidget.h"
#include <QEventLoop>
#include <QFile>
#include <QMessageBox>
#include <QTimer>

#define BLKSIZE 512

CopyProcess::CopyProcess(const QString &source, const QString &destination, ProgressWidget *progress):
progress(progress),
source(source),
destination(destination),
result(true)
{
}

bool CopyProcess::run()
{
  progress->lock(true);
  // progress->setInfo(tr("Copying file..."));

  QEventLoop loop;
  connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
  QTimer::singleShot(500, this, SLOT(onTimer()));
  loop.exec();

  return result;
}

void CopyProcess::onTimer()
{
  char buf[BLKSIZE];

  QFile sourceFile(source);
  int blocks = (sourceFile.size() + BLKSIZE - 1) / BLKSIZE;
  progress->setMaximum(blocks-1);

  if (sourceFile.open(QIODevice::ReadOnly)) {
    QFile destinationFile(destination);
    if (destinationFile.open(QIODevice::ReadWrite)) {
      // progress->addText(tr("Writing file: "));
      for (int i=0; i<blocks; i++) {
        int read = sourceFile.read(buf, BLKSIZE);
        if (destinationFile.write(buf, read) == read) {
          destinationFile.flush();
          progress->setValue(i);
        }
        else {
          QMessageBox::warning(NULL, tr("Error"), tr("Write error"));
          result = false;
          break;
        }
      }
      destinationFile.close();
    }
    else {
      QMessageBox::warning(NULL, tr("Error"),tr("Cannot write %1 (reason: %2)").arg(destinationFile.fileName()).arg(sourceFile.errorString()));
      result = false;
    }
  }
  else {
    QMessageBox::warning(NULL, tr("Error"),tr("Cannot open %1 (reason: %2)").arg(sourceFile.fileName()).arg(sourceFile.errorString()));
    result = false;
  }

  sourceFile.close();

  progress->lock(false);
  emit finished();
}
