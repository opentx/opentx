#include "releasenotesdialog.h"
#include "ui_htmldialog.h"

ReleaseNotesDialog::ReleaseNotesDialog(QWidget * parent) :
  QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
  ui(new Ui::HtmlDialog)
{
  ui->setupUi(this);

  setWindowTitle(tr("Companion Release Notes"));
  setWindowIcon(CompanionIcon("changelog.png"));

  QFile file(":/releasenotes.txt");
  if (file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    ui->textEditor->setHtml(file.readAll());
    ui->textEditor->setOpenExternalLinks(true);
  }
  ui->textEditor->scroll(0, 0);
  ui->textEditor->setOpenExternalLinks(true);
}

ReleaseNotesDialog::~ReleaseNotesDialog()
{
  delete ui;
}
