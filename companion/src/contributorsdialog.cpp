#include "contributorsdialog.h"
#include "ui_contributorsdialog.h"
#include <QtGui>

contributorsDialog::contributorsDialog(QWidget *parent, int contest, QString rnurl) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    ui(new Ui::contributorsDialog)
{
    ui->setupUi(this);
    switch (contest) {
      case 0: {
        QFile file(":/DONATIONS.txt");
        QString htmlString = "<table>";
        htmlString += "<tr><td colspan=\"5\"><h3>People who have contributed to this project</h3></td></tr><tr>";
        if(file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
             int i = 0;
             while (!file.atEnd()) {
                   QByteArray line = file.readLine(); 
                   htmlString += QString("<td>") + line + QString("</td>");
                   i++;
                   if (!(i%5)){
                       htmlString += "</tr><tr>";
                   }
              }
             htmlString += "</tr>";
        }

        htmlString += "<tr></tr><tr><td colspan=\"5\"><h3>Coders</h3></td></tr><tr>";
        QFile file2(":/CREDITS.txt");
        if(file2.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
             int i = 0;
             while (!file2.atEnd()) {
                   QByteArray line = file2.readLine();
                   htmlString += QString("<td>") + line + QString("</td>");
                   i++;
                   if (!(i%5)){
                       htmlString += "</tr><tr>";
                   }
              }
        }
        htmlString += "</tr></table>";
        ui->textBrowser->insertHtml(htmlString);

        ui->textBrowser->insertPlainText("\n\n");
        ui->textBrowser->insertPlainText(tr("Honors go to Rafal Tomczak (RadioClone) and Thomas Husterer (th9x) \nof course. Also to Erez Raviv (er9x) and it's fantastic eePe, from which\ncompanion9x was forked out."));
        ui->textBrowser->insertPlainText("\n\n");
        ui->textBrowser->insertPlainText(tr("Thank you all !!!"));
        ui->textBrowser->setReadOnly(true);
        ui->textBrowser->verticalScrollBar()->setValue(0);
        this->setWindowTitle(tr("Contributors"));
        }
        break;
      
      case 1:{
        QFile file(":/releasenotes.txt");
        if(file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
          ui->textBrowser->insertHtml(file.readAll());
        }
        ui->textBrowser->setReadOnly(true);
        ui->textBrowser->verticalScrollBar()->setValue(0);
        this->setWindowTitle(tr("Companion9x Release Notes"));
        }
        break;
      case 2:{
        if (!rnurl.isEmpty()) {
          this->setWindowTitle(tr("OpenTX Release Notes"));
          manager = new QNetworkAccessManager(this);
          connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
          QUrl url(rnurl);
          QNetworkRequest request(url);
          request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
          manager->get(request);
        } else {
          QTimer::singleShot(0, this, SLOT(forceClose()));                
        }
        break;
      }
    }
}

void contributorsDialog::showEvent ( QShowEvent * )
{
    ui->textBrowser->verticalScrollBar()->setValue(0);
}

contributorsDialog::~contributorsDialog()
{
    delete ui;
}

void contributorsDialog::replyFinished(QNetworkReply * reply)
{
    ui->textBrowser->insertHtml(reply->readAll());
}

void contributorsDialog::forceClose() {
    accept();;
}
