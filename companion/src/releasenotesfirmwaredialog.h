#ifndef RELEASENOTESFIRMWAREDIALOG_H
#define RELEASENOTESFIRMWAREDIALOG_H

#include <QDialog>
#include "eeprominterface.h"
#include "downloaddialog.h"

namespace Ui {
  class HtmlDialog;
}

class ReleaseNotesFirmwareDialog : public QDialog
{
  Q_OBJECT

  public:
    explicit ReleaseNotesFirmwareDialog(QWidget *parent, const QString & rnurl);
    ~ReleaseNotesFirmwareDialog();

  private:
    Ui::HtmlDialog *ui;
    QNetworkAccessManager *manager;

  private slots:
    void replyFinished(QNetworkReply * reply);   
    
};

#endif // RELEASENOTESFIRMWAREDIALOG_H
