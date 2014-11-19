#ifndef FOLDERSYNC_H_
#define FOLDERSYNC_H_

#include <QString>
#include <QStringList>

class QDir;

class FoldersSyncTask
{
  public:
    FoldersSyncTask(const QString &folder1, const QString &folder2);
    void run();

  protected:
    QStringList updateDir(const QDir &source, const QDir &destination);
    QString folders[2];
};

#endif /* FOLDERSYNC_H_ */
