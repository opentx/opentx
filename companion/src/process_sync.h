#ifndef SYNCPROCESS_H_
#define SYNCPROCESS_H_

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

#endif /* SYNCPROCESS_H_ */
