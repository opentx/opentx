#include "splashlibrary.h"
#include "ui_splashlibrary.h"

#include <QtGui>
#include "helpers.h"
//#include "splashlabel.h"
#include "flashinterface.h"

splashLibrary::splashLibrary(QWidget *parent, QString * fileName) : QDialog(parent), ui(new Ui::splashLibrary) {
  splashFileName = fileName;
  ui->setupUi(this);
  page = 0;
  getFileList();
  if (imageList.size() > 20) {
    ui->nextPage->setEnabled(true);
  }
  setupPage(page);

  foreach(splashLabel *sl, findChildren<splashLabel *>()) {
    connect(sl, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  }
  resize(0, 0);
}

splashLibrary::~splashLibrary() {
  delete ui;
}

void splashLibrary::setupPage(int page) {
  splashLabel * sl[] = { ui->FwImage_01, ui->FwImage_02, ui->FwImage_03, ui->FwImage_04 ,
        ui->FwImage_05, ui->FwImage_06, ui->FwImage_07, ui->FwImage_08 ,
        ui->FwImage_09, ui->FwImage_10, ui->FwImage_11, ui->FwImage_12 ,
        ui->FwImage_13, ui->FwImage_14, ui->FwImage_15, ui->FwImage_16 ,
        ui->FwImage_17, ui->FwImage_18, ui->FwImage_19, ui->FwImage_20 };
  for(int i=0; i<20; i++) {
    if ((i + 20 * page) < imageList.size()) {
      QImage image(imageList.at(i + 20 * page));
      if (!image.isNull()) {
        sl[i]->setPixmap(QPixmap::fromImage(image.scaled(SPLASH_WIDTH, SPLASH_HEIGHT).convertToFormat(QImage::Format_Mono)));
        sl[i]->setEnabled(true);
        sl[i]->setId((i + 20 * page));
        sl[i]->setStyleSheet("border:1px solid; border-color:#999999;");
      }
      else {
        sl[i]->clear();
        sl[i]->setDisabled(true);
        sl[i]->setStyleSheet("border:1px;");
        sl[i]->setId(-1);
      }
    }
    else {
      sl[i]->clear();
      sl[i]->setDisabled(true);
      sl[i]->setStyleSheet("border:1px;");
      sl[i]->setId(-1);
    }
  }
  setWindowTitle(tr("Splash Library - page %1 of %2").arg(page + 1).arg(ceil((float) imageList.size() / 20.0)));
}

void splashLibrary::getFileList() {
  QSettings settings("companion9x", "companion9x");
  imageList.clear();
  if (settings.value("embedded_splashes", 0).toInt() == 0) {
    QDir myRes(":/images/library");
    QStringList tmp = myRes.entryList();
    for (int i = 0; i < tmp.size(); i++) {
      QFileInfo fileInfo = tmp.at(i);
      imageList.append(":/images/library/" + fileInfo.fileName());
    }
  }
  QString libraryPath = settings.value("libraryPath", "").toString();
  if (!libraryPath.isEmpty()) {
    if (QDir(libraryPath).exists()) {
      QStringList supportedImageFormats;
      for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
        supportedImageFormats << QLatin1String("*.") + QImageReader::supportedImageFormats()[formatIndex];
      }
      QDir myDir(libraryPath);
      myDir.setNameFilters(supportedImageFormats);
      QStringList tmp = myDir.entryList();
      for (int i = 0; i < tmp.size(); i++) {
        QFileInfo fileInfo = tmp.at(i);
        QString filename = libraryPath + "/" + fileInfo.fileName();
        QImage image(filename);
        if (!image.isNull()) {
          imageList.append(filename);
        }
        else {
          QMessageBox::information(this, tr("Warning"), tr("Invalid image in library %1").arg(filename));
        }
      }
    }
  }
  if (imageList.size() == 0) {
    QMessageBox::information(this, tr("Information"), tr("No valid image found in library, check your settings"));
    QTimer::singleShot(0, this, SLOT(dclose()));
  }
  else {
    imageList.sort();
  }
}

void splashLibrary::shrink() {
  resize(0, 0);
}

void splashLibrary::dclose() {
  close();
}

void splashLibrary::onButtonPressed(int button) {
  splashLabel * myLabel = qobject_cast<splashLabel *>(sender());

  foreach(splashLabel *sl, findChildren<splashLabel *>()) {
    if (sl->isEnabled()) {
      sl->setStyleSheet("border:1px solid; border-color:#999999;");
    }
  }
  if (button == Qt::Key_Enter) {
    int id = myLabel->getId();
    splashFileName->clear();
    splashFileName->append(imageList.at(id));
    close();
  }
  myLabel->setStyleSheet("border:1px solid; border-color:#00ffff");
}

void splashLibrary::on_nextPage_clicked() {
  page++;
  if (page >= (imageList.size() / 20)) {
    ui->nextPage->setDisabled(true);
  }
  ui->prevPage->setEnabled(true);
  setupPage(page);
}

void splashLibrary::on_prevPage_clicked() {
  page--;
  if (page == 0) {
    ui->prevPage->setDisabled(true);
  }
  if (imageList.size() > 20) {
    ui->nextPage->setEnabled(true);
  }
  setupPage(page);
}