#include "customizesplashdialog.h"
#include "ui_customizesplashdialog.h"

#include <QtGui>
#include "helpers.h"
#include "burndialog.h"
#include "splashlibrary.h"
#include "flashinterface.h"

customizeSplashDialog::customizeSplashDialog(QWidget *parent) :
QDialog(parent),
ui(new Ui::customizeSplashDialog) {
  ui->setupUi(this);
  ui->HowToLabel->clear();
  ui->HowToLabel->append("<center>" + tr("Select an original firmware file") + "</center>");
}

customizeSplashDialog::~customizeSplashDialog() {
  delete ui;
}

void customizeSplashDialog::on_FlashLoadButton_clicked()
{
  QString fileName;
  QSettings settings("companion9x", "companion9x");
  ui->ImageLoadButton->setDisabled(true);
  ui->libraryButton->setDisabled(true);
  ui->SaveFlashButton->setDisabled(true);
  ui->SaveImageButton->setDisabled(true);
  ui->ImageFileName->clear();
  ui->imageLabel->clear();
  ui->HowToLabel->clear();
  ui->HowToLabel->setStyleSheet("background:rgb(255, 255, 0)");
  fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("lastFlashDir").toString(), FLASH_FILES_FILTER);
  QFile file(fileName);
  if (!file.exists()) {
    ui->FWFileName->clear();
    ui->HowToLabel->append("<center>" + tr("Select an original firmware file") + "</center>");
    return;    
  }
  if (fileName.isEmpty()) {
    ui->FWFileName->clear();
    ui->HowToLabel->append("<center>" + tr("Select an original firmware file") + "</center>");
    return;
  }
  
  ui->FWFileName->setText(fileName);
  FlashInterface flash(fileName);
  if (flash.hasSplash()) {
    ui->HowToLabel->append("<center>" + tr("Select an image to customize your splash <br />or save actual firmware splash") + "</center>");
    ui->ImageLoadButton->setEnabled(true);
    ui->libraryButton->setEnabled(true);
    ui->SaveImageButton->setEnabled(true);
    ui->imageLabel->setPixmap(QPixmap::fromImage(flash.getSplash()));
    ui->imageLabel->setFixedSize(flash.getSplashWidth()*2,flash.getSplashHeight()*2);
  } else {
    QMessageBox::information(this, tr("Error"), tr("Could not find bitmap to replace in file"));
    return;
  }
  settings.setValue("lastFlashDir", QFileInfo(fileName).dir().absolutePath());
}

void customizeSplashDialog::on_ImageLoadButton_clicked() {
  QString supportedImageFormats;
  for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
    supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
  }

  QSettings settings("companion9x", "companion9x");
  QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image to load"), settings.value("lastImagesDir").toString(), tr("Images (%1)").arg(supportedImageFormats));

  if (!fileName.isEmpty()) {
    settings.setValue("lastImagesDir", QFileInfo(fileName).dir().absolutePath());
    QImage image(fileName);
    if (image.isNull()) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot load %1.").arg(fileName));
      return;
    }
    ui->HowToLabel->clear();
    ui->ImageFileName->setText(fileName);    
    if (ui->imageLabel->width()==424) {
      image=image.convertToFormat(QImage::Format_RGB32);
      QRgb col;
      int gray;
      int width = image.width();
      int height = image.height();
      for (int i = 0; i < width; ++i)
      {
          for (int j = 0; j < height; ++j)
          {
              col = image.pixel(i, j);
              gray = qGray(col);
              image.setPixel(i, j, qRgb(gray, gray, gray));
          }
      }      
      ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->imageLabel->width()/2, ui->imageLabel->height()/2)));
    } else {
      ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->imageLabel->width()/2, ui->imageLabel->height()/2).convertToFormat(QImage::Format_Mono)));
    }
    ui->SaveFlashButton->setEnabled(true);
    ui->HowToLabel->append("<center>" + tr("Save your custimized firmware") + "</center>");
  }
}

void customizeSplashDialog::on_libraryButton_clicked() {
  QString fileName;
  
  splashLibrary *ld = new splashLibrary(this,&fileName);
  ld->exec();
  if (!fileName.isEmpty()) {
    QImage image(fileName);
    if (image.isNull()) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot load %1.").arg(fileName));
      return;
    }
    ui->HowToLabel->clear();
    ui->ImageFileName->setText(fileName);
    if (ui->imageLabel->width()==424) {
      image=image.convertToFormat(QImage::Format_RGB32);
      QRgb col;
      int gray;
      int width = image.width();
      int height = image.height();
      for (int i = 0; i < width; ++i)
      {
          for (int j = 0; j < height; ++j)
          {
              col = image.pixel(i, j);
              gray = qGray(col);
              image.setPixel(i, j, qRgb(gray, gray, gray));
          }
      }      
      ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->imageLabel->width()/2, ui->imageLabel->height()/2)));
    } else {
      ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->imageLabel->width()/2, ui->imageLabel->height()/2).convertToFormat(QImage::Format_Mono)));
    }
    ui->SaveFlashButton->setEnabled(true);
    ui->HowToLabel->append("<center>" + tr("Save your custimized firmware") + "</center>");
  }
}

void customizeSplashDialog::on_SaveFlashButton_clicked()
{
  QString fileName;
  QSettings settings("companion9x", "companion9x");
  ui->HowToLabel->clear();
  fileName = QFileDialog::getSaveFileName(this, tr("Write to file"), settings.value("lastFlashDir").toString(), FLASH_FILES_FILTER, 0, QFileDialog::DontConfirmOverwrite);
  if (fileName.isEmpty()) {
    return;
  }
  FlashInterface flash(ui->FWFileName->text());
  if (!flash.hasSplash()) {
    QMessageBox::critical(this, tr("Error"), tr("Error reading file %1").arg(fileName));
    return;
  }
  settings.setValue("lastFlashDir", QFileInfo(fileName).dir().absolutePath());
  QImage image = ui->imageLabel->pixmap()->toImage().scaled(flash.getSplashWidth(), flash.getSplashHeight());
  flash.setSplash(image);
  if (flash.saveFlash(fileName) > 0) {
    ui->HowToLabel->setStyleSheet("background:rgb(0,255.0);");
    ui->HowToLabel->append("<center>" + tr("Firmware correctly saved.") + "</center>");
  } else {
    ui->HowToLabel->setStyleSheet("background:rgb(255.0.0);");
    ui->HowToLabel->append("<center>" + tr("Firmware not saved.") + "</center>");
  }
}

void customizeSplashDialog::on_InvertColorButton_clicked()
{
  QImage image = ui->imageLabel->pixmap()->toImage();
  image.invertPixels();
  ui->imageLabel->setPixmap(QPixmap::fromImage(image));
}

void customizeSplashDialog::on_SaveImageButton_clicked()
{
  QString fileName;
  QSettings settings("companion9x", "companion9x");

  fileName = QFileDialog::getSaveFileName(this, tr("Write to file"), settings.value("lastImagesDir").toString(), tr("PNG images (*.png);;"), 0, QFileDialog::DontConfirmOverwrite);
  if (!fileName.isEmpty()) {
    settings.setValue("lastImagesDir", QFileInfo(fileName).dir().absolutePath());
    QImage image = ui->imageLabel->pixmap()->toImage().scaled(ui->imageLabel->width()/2, ui->imageLabel->height()/2).convertToFormat(QImage::Format_Indexed8);
    image.save(fileName, "PNG");
  }
}
