#ifndef FLASHPROCESS_H_
#define FLASHPROCESS_H_

#include <QObject>
#include <QString>
#include <QStringList>

class ProgressWidget;
class QProcess;

class FlashProcess : public QObject
{
  Q_OBJECT

public:
  FlashProcess(const QString &cmd, const QStringList &args, ProgressWidget *progress);
  ~FlashProcess();
  bool run();

signals:
  void finished();

protected slots:
  void onStarted();
  void onReadyReadStandardOutput();
  void onReadyReadStandardError();
  void onFinished(int);

protected:
  void analyseStandardOutput(const QString &text);
  void analyseStandardError(const QString &text);
  void errorWizard();
  void addReadFuses();
  ProgressWidget *progress;
  const QString cmd;
  const QStringList args;
  QProcess *process;
  bool hasErrors;
  QString currStdoutLine;
  QString currStderrLine;
  unsigned int lfuse;
  unsigned int hfuse;
  unsigned int efuse;
  enum FlashPhase { READING, WRITING, VERIFYING };
  FlashPhase flashPhase;
};

#endif // FLASHPROCESS_H_
