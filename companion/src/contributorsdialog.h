#ifndef CONTRIBUTORSDIALOG_H
#define CONTRIBUTORSDIALOG_H

#include <QDialog>

namespace Ui {
  class HtmlDialog;
}

class ContributorsDialog : public QDialog
{
  Q_OBJECT

  public:
    explicit ContributorsDialog(QWidget * parent = 0);
    ~ContributorsDialog();

  private:
    Ui::HtmlDialog * ui;
    QString formatTable(const QString & title, const QStringList & names, int colums);
};

#endif // CONTRIBUTORSDIALOG_H
