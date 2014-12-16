#ifndef COPYPROCESS_H_
#define COPYPROCESS_H_

#include <QObject>
#include <QString>
#include <QStringList>

class ProgressWidget;

class CopyProcess : public QObject
{
  Q_OBJECT

public:
  CopyProcess(const QString &source, const QString &destination, ProgressWidget *progress);
  bool run();

signals:
  void finished();

protected slots:
  void onTimer();

protected:
  ProgressWidget *progress;
  const QString source;
  const QString destination;
  bool result;
};

#endif // COPYPROCESS_H_
