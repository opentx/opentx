#ifndef SPLASHLIBRARY_H
#define SPLASHLIBRARY_H

#include <QtGui>
#include <QDialog>

namespace Ui {
    class splashLibrary;
}

class splashLibrary : public QDialog
{
    Q_OBJECT

public:
    explicit splashLibrary(QWidget *parent = 0, QString * fileName=NULL);
    ~splashLibrary();

private slots:
    void shrink() ;
    void dclose();
    void onButtonPressed(int button);
    void on_nextPage_clicked();
    void on_prevPage_clicked();
 
private:
    void getFileList();
    void setupPage(int page);
    Ui::splashLibrary *ui;
    QString * splashFileName;
    QString libraryPath;
    QStringList imageList;
    int page;
};

#endif // SPLASHLIBRARY_H
