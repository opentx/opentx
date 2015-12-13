#ifndef SPLASHLIBRARY_H
#define SPLASHLIBRARY_H

#include <QtGui>
#include <QDialog>

namespace Ui {
    class SplashLibraryDialog;
}

class SplashLibraryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SplashLibraryDialog(QWidget *parent = 0, QString * fileName=NULL);
    ~SplashLibraryDialog();

private slots:
    void dclose();
    void onButtonPressed(int button);
    void on_nextPage_clicked();
    void on_prevPage_clicked();
 
private:
    void getFileList();
    void setupPage(int page);
    Ui::SplashLibraryDialog *ui;
    QString * splashFileName;
    QString libraryPath;
    QStringList imageList;
    int page;
};

#endif // SPLASHLIBRARY_H
