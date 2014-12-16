#ifndef SYNCPROCESS_H_
#define SYNCPROCESS_H_

#include <QObject>
#include <QString>
#include <QStringList>

class QDir;
class ProgressWidget;

class SyncProcess : public QObject
{
  Q_OBJECT

public:
  SyncProcess(const QString &folder1, const QString &folder2, ProgressWidget *progress);
  void run();

protected:
  bool synchronize();
  QStringList updateDir(const QDir &source, const QDir &destination);
  QString folder1;
  QString folder2;
  ProgressWidget *progress;
  QStringList errors;
  bool simulation;
  int index;
};

#endif /* SYNCPROCESS_H_ */
