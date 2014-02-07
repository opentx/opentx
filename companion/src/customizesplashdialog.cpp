#include "customizesplashdialog.h"
#include "ui_customizesplashdialog.h"

#include <QtGui>
#include "helpers.h"
#include "burndialog.h"
#include "splashlibrary.h"
#include "flashinterface.h"

Side::Side(){
  imageLabel = 0;
  fileNameEdit = 0;
  saveButton = 0;
  source=new Source;
  *source = UNDEFINED;
}

void Side::copyImage( Side side ){
  if ((*source!=UNDEFINED) && (*side.source!=UNDEFINED))
    imageLabel->setPixmap(*side.imageLabel->pixmap());
}

customizeSplashDialog::customizeSplashDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::customizeSplashDialog)
{
  ui->setupUi(this);
  ui->leftLibraryButton->setIcon(CompanionIcon("library.png"));
  ui->rightLibraryButton->setIcon(CompanionIcon("library.png"));
  
  left.imageLabel =  ui->leftImageLabel;
  right.imageLabel = ui->rightImageLabel;
  left.fileNameEdit =  ui->leftFileNameEdit;
  right.fileNameEdit = ui->rightFileNameEdit;
  left.saveButton =  ui->leftSaveButton;
  right.saveButton =  ui->rightSaveButton;
  left.libraryButton = ui->leftLibraryButton;
  right.libraryButton = ui->rightLibraryButton;
  left.invertButton = ui->leftInvertButton;
  right.invertButton = ui->rightInvertButton;

  resize(0,0);
}

customizeSplashDialog::~customizeSplashDialog()
{
  delete ui;
}

void customizeSplashDialog::on_copyRightToLeftButton_clicked() {
  left.copyImage(right);
}
void customizeSplashDialog::on_copyLeftToRightButton_clicked() {
right.copyImage(left);
}


void customizeSplashDialog::on_leftLoadFwButton_clicked() {loadFwButton_clicked(left);}
void customizeSplashDialog::on_rightLoadFwButton_clicked() {loadFwButton_clicked(right);}
void customizeSplashDialog::loadFwButton_clicked(Side side)
{
  QSettings settings;
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("lastFlashDir").toString(), FLASH_FILES_FILTER);
  if (!fileName.isEmpty()) {
    QFile file(fileName);
    if (fileName.isEmpty() || !file.exists())
      return;

    FlashInterface flash(fileName);
    if (!flash.hasSplash()) {
      QMessageBox::information(this, tr("Error"), tr("Could not find bitmap to replace in file"));
      return;
    }

    side.saveButton->setEnabled(true);
    side.imageLabel->setPixmap(QPixmap::fromImage(flash.getSplash()));
    side.imageLabel->setFixedSize(flash.getSplashWidth()*2,flash.getSplashHeight()*2);
    side.fileNameEdit->setText(fileName);
    settings.setValue("lastFlashDir", QFileInfo(fileName).dir().absolutePath());
    *side.source=FW;
    side.saveButton->setEnabled(true);
    side.libraryButton->setEnabled(true);
    side.invertButton->setEnabled(true);
  }
}

void customizeSplashDialog::on_leftLoadPictButton_clicked() {loadPictButton_clicked(left);}
void customizeSplashDialog::on_rightLoadPictButton_clicked() {loadPictButton_clicked(right);}
void customizeSplashDialog::loadPictButton_clicked(Side side)
{
  QString supportedImageFormats;
  for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
    supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
  }
  QSettings settings;
  QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image to load"), settings.value("lastImagesDir").toString(), tr("Images (%1)").arg(supportedImageFormats));

  if (fileName.isEmpty()){
    return;
  }
  QImage image(fileName);
  if (image.isNull()) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot load %1.").arg(fileName));
    return;
  }
  if (side.imageLabel->width()==424) {
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
    side.imageLabel->setPixmap(QPixmap::fromImage(image.scaled(side.imageLabel->width()/2, side.imageLabel->height()/2)));
  } else {
    side.imageLabel->setPixmap(QPixmap::fromImage(image.scaled(side.imageLabel->width()/2, side.imageLabel->height()/2).convertToFormat(QImage::Format_Mono)));
  }

  settings.setValue("lastImagesDir", QFileInfo(fileName).dir().absolutePath());
  side.fileNameEdit->setText(fileName);
  *side.source=PICT;
  side.saveButton->setEnabled(true);
  side.libraryButton->setEnabled(true);
  side.invertButton->setEnabled(true);
}

void customizeSplashDialog::on_leftLibraryButton_clicked(){libraryButton_clicked(left);}
void customizeSplashDialog::on_rightLibraryButton_clicked(){libraryButton_clicked(right);}
void customizeSplashDialog::libraryButton_clicked( Side side )
{
  QString fileName;
  
  splashLibrary *ld = new splashLibrary(this,&fileName);
  ld->exec();
  if (!fileName.isEmpty()) {
    QImage image(fileName);
    if (image.isNull()) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot load %1.").arg(fileName));
      return;
    }
    if (side.imageLabel->width()==424) {
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
      side.imageLabel->setPixmap(QPixmap::fromImage(image.scaled(side.imageLabel->width()/2, side.imageLabel->height()/2)));
    } else {
      side.imageLabel->setPixmap(QPixmap::fromImage(image.scaled(side.imageLabel->width()/2, side.imageLabel->height()/2).convertToFormat(QImage::Format_Mono)));
    }
  }
}

void customizeSplashDialog::on_leftSaveButton_clicked(){saveButton_clicked(left);}
void customizeSplashDialog::on_rightSaveButton_clicked(){saveButton_clicked(right);}
void customizeSplashDialog::saveButton_clicked(Side side)
{
  QSettings settings;
  QString fileName = side.fileNameEdit->text();

  if (*side.source == FW)
  {
    FlashInterface flash(fileName);
    if (!flash.hasSplash()) {
      QMessageBox::critical(this, tr("Error"), tr("Could not store image in firmware file %1").arg(fileName));
      return;
    }
    QImage image = side.imageLabel->pixmap()->toImage().scaled(flash.getSplashWidth(), flash.getSplashHeight());
    if (flash.setSplash(image) && (flash.saveFlash(fileName) > 0))
      settings.setValue("lastFlashDir", QFileInfo(fileName).dir().absolutePath());
    else
      QMessageBox::critical(this, tr("Error"), tr("Could not store image in firmware file %1").arg(fileName));
  }
  else if (*side.source == PICT)
  {
    if (!fileName.isEmpty()) {
      QImage image = side.imageLabel->pixmap()->toImage().scaled(side.imageLabel->width()/2, side.imageLabel->height()/2).convertToFormat(QImage::Format_Indexed8);
      if (image.save(fileName))
        settings.setValue("lastImagesDir", QFileInfo(fileName).dir().absolutePath());
      else
        QMessageBox::critical(this, tr("Error"), tr("The image file %1 could not be stored").arg(fileName));      
    }
  }
}

void customizeSplashDialog::on_leftInvertButton_clicked(){invertButton_clicked(left);}
void customizeSplashDialog::on_rightInvertButton_clicked(){invertButton_clicked(right);}
void customizeSplashDialog::invertButton_clicked(Side side)
{
  QImage image = side.imageLabel->pixmap()->toImage();
  image.invertPixels();
  side.imageLabel->setPixmap(QPixmap::fromImage(image));
}


