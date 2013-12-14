#ifndef AVROUTPUTDIALOG_H
#define AVROUTPUTDIALOG_H

#include <QDialog>
#include <QtGui>

#define AVR_DIALOG_CLOSE_IF_SUCCESSFUL 0x00
#define AVR_DIALOG_KEEP_OPEN           0x01
#define AVR_DIALOG_FORCE_CLOSE         0x02
#define AVR_DIALOG_SHOW_DONE           0x04
#define READBUF 65536
#define BLKSIZE 512


namespace Ui {
    class avrOutputDialog;
}

class avrOutputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit avrOutputDialog(QWidget *parent, QString prog, QStringList arg, QString wTitle, int closeBehaviour=AVR_DIALOG_CLOSE_IF_SUCCESSFUL, bool displayDetails=false);
    ~avrOutputDialog();

    void addText(const QString &text);
    void runAgain(QString prog, QStringList arg, int closeBehaviour=AVR_DIALOG_CLOSE_IF_SUCCESSFUL);
    void waitForFinish();
    void addReadFuses();

protected slots:
    void doAddTextStdOut();
    void doAddTextStdErr();
    void doProcessStarted();
    void doCopy();
    void doFinished(int code);
    void on_checkBox_toggled(bool checked);
    void shrink();
    void forceClose();
    void killTimerElapsed();
    void errorWizard();

private:
    QString getProgrammer();
    Ui::avrOutputDialog *ui;

    QProcess *process;
    QTimer *kill_timer;
    QString cmdLine;
    int closeOpt;
    quint8 lfuse;
    quint8 hfuse;
    quint8 efuse;
    QString prevLine;
    QString currLine;
    QString prevStdLine;
    QString currStdLine;
    int phase;
    QString winTitle;
    bool hasErrors;
    QString sourceFile;
    QString destFile;
    
};

#endif // AVROUTPUTDIALOG_H
