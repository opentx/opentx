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
        QString str;
        str.append("<html><head>");
        str.append("<style type=\"text/css\">\n");
        str.append(".mycss\n{\nfont-weight:normal;\ncolor:#000000;vertical-align: top;font-size:10px;text-align:left;font-family:arial, helvetica, sans-serif;\n}\n");
        str.append(".mycssb\n{\nfont-weight:bold;\ncolor:#C00000;vertical-align: top;font-size:10px;text-align:left;font-family:arial, helvetica, sans-serif;\n}\n");
        str.append(".myhead\n{\nfont-weight:bold;\ncolor:#000000;font-size:14px;text-align:left;font-family:arial, helvetica, sans-serif;\n}\n");
        str.append("</style>\n</head><body class=\"mycss\"><table width=\"100%\" border=0 cellspacing=0 cellpadding=2>");
        str.append("<tr><td class=\"myhead\">"+tr("People who have contributed to this project")+"</td></tr>");
        str.append("</table>");
        str.append("<table width=\"100%\" border=0 cellspacing=0 cellpadding=2>");
        if(file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
          int columns=6;
          float cwidth=100.0/columns;
          while (!file.atEnd()) {
            str.append("<tr>");
            for (int i=0; i<columns; i++) {
              str.append(QString("<td width=\"%1%\" ").arg(cwidth));
              if (!file.atEnd()) {
                QByteArray line = file.readLine();
                if (line.contains("monthly") || line.contains("mensual")) {
                  str.append("class=\"mycssb\">");
                } else {
                  str.append("class=\"mycss\">");
                }
                str.append(line.trimmed()+"</td>");
              } else {
                str.append("class=\"mycss\">&nbsp;</td>");
              }
            }
            str.append("</tr>");
          }
        }
        str.append("</table>");
        QFile file2(":/CREDITS.txt");
        str.append("<table width=\"100%\" border=0 cellspacing=0 cellpadding=2>");
        str.append("<tr><td class=\"mycss\">&nbsp;</td></tr>");
        str.append("<tr><td class=\"myhead\">"+tr("Coders")+"</td></tr>");
        str.append("</table>");
        str.append("<table width=\"100%\" border=0 cellspacing=0 cellpadding=2>");
        if(file2.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
          while (!file2.atEnd()) {
            str.append("<tr>");
            for (int i=0; i<3; i++) {
              str.append("<td width=\"33.33%\" class=\"mycss\">");
              if (!file2.atEnd()) {
                QByteArray line = file2.readLine();
                str.append(line.trimmed());
              } else {
                str.append("&nbsp;");
              }
              str.append("</td>");
            }
            str.append("</tr>");
          }
        }
        str.append("<tr><td class=\"mycss\">&nbsp;</td></tr>");
        str.append("<tr><td colspan=3 class=\"mycss\">"+tr("Honors go to Rafal Tomczak (RadioClone) and Thomas Husterer (th9x) \nof course. Also to Erez Raviv (er9x) and it's fantastic eePe, from which\ncompanion9x was forked out.")+"</td></tr>");
        str.append("<tr><td colspan=3 class=\"mycss\">"+tr("Thank you all !!!")+"</td></tr>");
        str.append("</table>");
        str.append("</body></html>");        
        ui->textEditor->setHtml(str);
        ui->textEditor->scroll(0,0);
        this->setWindowTitle(tr("Contributors"));
        }
        break;
      
      case 1:{
        QFile file(":/releasenotes.txt");
        if(file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
          ui->textEditor->setHtml(file.readAll());
        }
        ui->textEditor->scroll(0,0);
        this->setWindowTitle(tr("Companion Release Notes"));
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
    ui->textEditor->scroll(0,0);
}

contributorsDialog::~contributorsDialog()
{
    delete ui;
}

void contributorsDialog::replyFinished(QNetworkReply * reply)
{
    ui->textEditor->setHtml(reply->readAll());
}

void contributorsDialog::forceClose() {
    accept();;
}
