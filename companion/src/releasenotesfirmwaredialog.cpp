#include "releasenotesfirmwaredialog.h"
#include "ui_htmldialog.h"
#include "helpers.h"

ReleaseNotesFirmwareDialog::ReleaseNotesFirmwareDialog(QWidget * parent, const QString & rnurl):
  QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
  ui(new Ui::HtmlDialog)
{
  ui->setupUi(this);

  setWindowTitle(tr("OpenTX Release Notes"));
  setWindowIcon(CompanionIcon("changelog.png"));

  manager = new QNetworkAccessManager(this);
  connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
  QUrl url(rnurl);
  QNetworkRequest request(url);
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
  manager->get(request);
}

ReleaseNotesFirmwareDialog::~ReleaseNotesFirmwareDialog()
{
  delete ui;
}

void ReleaseNotesFirmwareDialog::replyFinished(QNetworkReply * reply)
{
  ui->textEditor->setHtml(reply->readAll());
  ui->textEditor->setOpenExternalLinks(true);
}

