#include "burndialog.h"
#include "ui_burndialog.h"

#include <QtGui>
#include "eeprominterface.h"
#include "helpers.h"
#include "splashlibrary.h"
#include "flashinterface.h"
#include "hexinterface.h"

burnDialog::burnDialog(QWidget *parent, int Type, QString * fileName, bool * backupEE, QString DocName):
  QDialog(parent),
  ui(new Ui::burnDialog),
  hexfileName(fileName),
  backup(backupEE),
  hexType(Type)
{
  ui->setupUi(this);
  ui->SplashFrame->hide();
  ui->FramFWInfo->hide();
  ui->EEbackupCB->hide();
  ui->EEbackupCB->setCheckState(*backup ? Qt::Checked : Qt::Unchecked);
  if (Type == 2) {
    ui->EEpromCB->hide();
    ui->profile_label->hide();
    ui->patchcalib_CB->hide();
    ui->patchhw_CB->hide();
    ui->InvertColorButton->setDisabled(true);
    this->setWindowTitle(tr("Write firmware to TX"));
    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      ui->EEbackupCB->hide();
    }
  } else {
    ui->FlashLoadButton->setText(tr("Load eEprom"));
    ui->profile_label->hide();
    ui->patchcalib_CB->hide();
    ui->patchhw_CB->hide();
    ui->EEpromCB->hide();
    ui->ImageLoadButton->setDisabled(true);
    ui->libraryButton->setDisabled(true);
    ui->InvertColorButton->setDisabled(true);
    ui->BurnFlashButton->setDisabled(true);
    ui->ImageFileName->clear();
    ui->FwImage->clear();
    ui->FWFileName->clear();
    ui->DateField->clear();
    ui->SVNField->clear();
    ui->ModField->clear();
    ui->FramFWInfo->hide();
    ui->SplashFrame->hide();
    ui->BurnFlashButton->setDisabled(true);
    ui->EEbackupCB->hide();
    if (DocName.isEmpty()) {
        this->setWindowTitle(tr("Write models to TX"));
    } else {
        this->setWindowTitle(tr("Write %1 to TX").arg(DocName));
    }
    QSettings settings("companion9x", "companion9x");
    int profileid=settings.value("profileId", 1).toInt();
    settings.beginGroup("Profiles");
    QString profile=QString("profile%1").arg(profileid);
    settings.beginGroup(profile);
    QString Name=settings.value("Name","").toString();
    settings.endGroup();
    settings.endGroup();
    ui->profile_label->setText(tr("Current profile")+QString(": ")+Name);
  }
  if (!hexfileName->isEmpty()) {
    ui->FWFileName->setText(*hexfileName);
    if (Type==2) {
      checkFw(*hexfileName);
    } else {
      burnraw=false;
      if (checkeEprom(*hexfileName)) {
        QSettings settings("companion9x", "companion9x");
        int profileid=settings.value("profileId", 1).toInt();
        settings.beginGroup("Profiles");
        QString profile=QString("profile%1").arg(profileid);
        settings.beginGroup(profile);
        QString Name=settings.value("Name","").toString();
        QString calib=settings.value("StickPotCalib","").toString();
        QString trainercalib=settings.value("TrainerCalib","").toString();
        QString DisplaySet=settings.value("Display","").toString();
        QString BeeperSet=settings.value("Beeper","").toString();
        QString HapticSet=settings.value("Haptic","").toString();
        QString SpeakerSet=settings.value("Speaker","").toString();
        settings.endGroup();
        settings.endGroup();
        if (!Name.isEmpty()) {
          ui->profile_label->show();
          ui->patchcalib_CB->show();
          ui->patchhw_CB->show();
          // TODO I hardcode the number of pots here, should be dependant on the board?
          if (!((calib.length()==(NUM_STICKS+3)*12) && (trainercalib.length()==16))) {
            ui->patchcalib_CB->setDisabled(true);
          }
          if (!((DisplaySet.length()==6) && (BeeperSet.length()==4) && (HapticSet.length()==6) && (SpeakerSet.length()==6))) {
            ui->patchhw_CB->setDisabled(true);
          }
        } else {
          ui->profile_label->hide();
        }
        if (!IS_TARANIS(GetEepromInterface()->getBoard())) {
          ui->EEpromCB->show();
        } else {
          ui->EEpromCB->setChecked(false);
        }
        ui->BurnFlashButton->setEnabled(true);
      }
    }
    ui->FWFileName->hide();
    ui->FlashLoadButton->hide();   
    hexfileName->clear();
  }
  else if (Type==2) {
    QSettings settings("companion9x", "companion9x");
    QString FileName;
    FileName = settings.value("lastFw").toString();
    QFile file(FileName);
    if (file.exists()) {
      checkFw(FileName);
    }
  }
  resize(0, 0);
}

burnDialog::~burnDialog() {
  delete ui;
}

void burnDialog::on_FlashLoadButton_clicked()
{
  QString fileName;
  QSettings settings("companion9x", "companion9x");
  ui->ImageLoadButton->setDisabled(true);
  ui->libraryButton->setDisabled(true);
  ui->InvertColorButton->setDisabled(true);
  ui->BurnFlashButton->setDisabled(true);
  ui->ImageFileName->clear();
  ui->FwImage->clear();
  ui->FWFileName->clear();
  ui->DateField->clear();
  ui->SVNField->clear();
  ui->ModField->clear();
  ui->FramFWInfo->hide();
  ui->SplashFrame->hide();
  ui->BurnFlashButton->setDisabled(true);
  ui->EEbackupCB->hide();
  QTimer::singleShot(0, this, SLOT(shrink()));
  if (hexType==2) {
    fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("lastFlashDir").toString(), FLASH_FILES_FILTER);
    checkFw(fileName);
  } else {
    QString fileName = QFileDialog::getOpenFileName(this,tr("Choose file to write to EEPROM memory"), settings.value("lastDir").toString(), tr(EXTERNAL_EEPROM_FILES_FILTER));
    if (checkeEprom(fileName)) {
      if (burnraw==false) {
        ui->BurnFlashButton->setEnabled(true);
        ui->profile_label->show();
        ui->patchcalib_CB->show();
        ui->patchhw_CB->show();
        if (!IS_TARANIS(GetEepromInterface()->getBoard())) {
          ui->EEpromCB->show();
        } else {
          ui->EEpromCB->setChecked(false);
        }
      } else {
        ui->BurnFlashButton->setEnabled(true);
        ui->profile_label->hide();
        ui->patchcalib_CB->setChecked(false);
        ui->patchhw_CB->setChecked(false);
        ui->patchhw_CB->hide();
        ui->patchcalib_CB->hide();        
      }
      QTimer::singleShot(0, this, SLOT(shrink()));
    }
  }
}

void burnDialog::checkFw(QString fileName)
{
  if (fileName.isEmpty()) {
    return;
  }
  QSettings settings("companion9x", "companion9x");
  if (!IS_TARANIS(GetEepromInterface()->getBoard())) {
    ui->EEbackupCB->show();
  } else {
    ui->EEbackupCB->setChecked(false);
    *backup=false;
  }
  ui->FWFileName->setText(fileName);
  FlashInterface flash(fileName);
  if (flash.isValid()) {
    ui->FramFWInfo->show();
    ui->DateField->setText(flash.getDate() + " " + flash.getTime());
    ui->SVNField->setText(flash.getSvn());
    ui->ModField->setText(flash.getBuild());
    ui->BurnFlashButton->setEnabled(true);
    ui->BurnFlashButton->setText(tr("Burn to TX"));
    if (flash.hasSplash()) {
      ui->SplashFrame->show();
      ui->ImageLoadButton->setEnabled(true);
      ui->libraryButton->setEnabled(true);
      ui->FwImage->setFixedSize(flash.getSplashWidth(),flash.getSplashHeight());
      ui->imageLabel->setFixedSize(flash.getSplashWidth(),flash.getSplashHeight());
      ui->FwImage->show();
      ui->FwImage->setPixmap(QPixmap::fromImage(flash.getSplash()));
      QString ImageStr = settings.value("SplashImage", "").toString();
      bool PatchFwCB = settings.value("patchImage", false).toBool();
      if (!ImageStr.isEmpty()) {
        QImage Image = qstring2image(ImageStr);
        ui->imageLabel->setPixmap(QPixmap::fromImage(Image.convertToFormat(flash.getSplashFormat())));
        ui->InvertColorButton->setEnabled(true);
        ui->PreferredImageCB->setChecked(true);
        ui->PatchFWCB->setChecked(PatchFwCB);
      }
      else {
        QString fileName=ui->ImageFileName->text();
        if (!fileName.isEmpty()) {
          QImage image(fileName);
          if (!image.isNull()) {
            ui->InvertColorButton->setEnabled(true);
            ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->imageLabel->width(), ui->imageLabel->height()).convertToFormat(flash.getSplashFormat())));
            ui->PatchFWCB->setEnabled(true);
            ui->PatchFWCB->setChecked(PatchFwCB);
          } else {
            ui->PatchFWCB->setDisabled(true);
            ui->PatchFWCB->setChecked(false);
            ui->PreferredImageCB->setDisabled(true);         
          }
        } else {
          ui->PatchFWCB->setDisabled(true);
          ui->PatchFWCB->setChecked(false);
          ui->PreferredImageCB->setDisabled(true);
        }
      }
    } else {
      ui->FwImage->hide();
      ui->ImageFileName->setText("");
      ui->SplashFrame->hide();
    }
  } else {
    QMessageBox::warning(this, tr("Warning"), tr("%1 is not a known firmware").arg(fileName));
    ui->BurnFlashButton->setText(tr("Burn anyway !"));
    ui->BurnFlashButton->setEnabled(true);
  }  
  QTimer::singleShot(0, this, SLOT(shrink()));
  settings.setValue("lastFlashDir", QFileInfo(fileName).dir().absolutePath());
}

bool burnDialog::checkeEprom(QString fileName)
{
  if (fileName.isEmpty()) {
    return false;
  }
  QFile file(fileName);
  if (!file.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("Unable to find file %1!").arg(fileName));
    return false;
  }
  burnraw=false;
  int fileType = getFileType(fileName);
  if (fileType==FILE_TYPE_XML) {
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
      QMessageBox::critical(this, tr("Error"),tr("Error opening file %1:\n%2.").arg(fileName).arg(file.errorString()));
      return false;
    }
    QTextStream inputStream(&file);
    XmlInterface(inputStream).load(radioData);
  } else if (fileType==FILE_TYPE_HEX || fileType==FILE_TYPE_EEPE) { //read HEX file
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
        QMessageBox::critical(this, tr("Error"),tr("Error opening file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }        
    QDomDocument doc(ER9X_EEPROM_FILE_TYPE);
    bool xmlOK = doc.setContent(&file);
    if(xmlOK) {
      if (!LoadEepromXml(radioData, doc)){
        return false;
      }
    }
    file.reset();
      
    QTextStream inputStream(&file);
    if (fileType==FILE_TYPE_EEPE) {  // read EEPE file header
      QString hline = inputStream.readLine();
      if (hline!=EEPE_EEPROM_FILE_HEADER) {
        file.close();
        return false;
      }
    }
    uint8_t eeprom[EESIZE_RLC_MAX];
    int eeprom_size = HexInterface(inputStream).load(eeprom, EESIZE_RLC_MAX);
    if (!eeprom_size) {
      int res = QMessageBox::question(this, "companion9x",tr("Invalid binary EEPROM File %1, Proceed anyway ?").arg(fileName),QMessageBox::Yes | QMessageBox::No);
      if (res == QMessageBox::No) {
        return false;
      }
      burnraw=true;
      ui->FWFileName->setText(fileName);
      return true;
    }
    file.close();
    if (!LoadEeprom(radioData, eeprom, eeprom_size)) {
      int res = QMessageBox::question(this, "companion9x",tr("Invalid binary EEPROM File %1, Proceed anyway ?").arg(fileName),QMessageBox::Yes | QMessageBox::No);
      if (res == QMessageBox::No) {
        return false;
      }
      burnraw=true;
      ui->FWFileName->setText(fileName);
      return true;
    }
  } else if (fileType==FILE_TYPE_BIN) { //read binary
    int eeprom_size = file.size();
    if (!file.open(QFile::ReadOnly)) {  //reading binary file   - TODO HEX support
        QMessageBox::critical(this, tr("Error"),tr("Error opening file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }
    uint8_t *eeprom = (uint8_t *)malloc(eeprom_size);
    memset(eeprom, 0, eeprom_size);
    long result = file.read((char*)eeprom, eeprom_size);
    file.close();
    if (result != eeprom_size) {
        QMessageBox::critical(this, tr("Error"),tr("Error reading file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }
    if (!LoadEeprom(radioData, eeprom, eeprom_size)) {
      int res = QMessageBox::question(this, "companion9x",tr("Invalid binary EEPROM File %1, Proceed anyway ?").arg(fileName),QMessageBox::Yes | QMessageBox::No);
      if (res == QMessageBox::No) {
        return false;
      }
      burnraw=true;
    }
  }
  ui->FWFileName->setText(fileName);
  return true;
}

void burnDialog::on_ImageLoadButton_clicked()
{
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
      ui->PatchFWCB->setDisabled(true);
      ui->PatchFWCB->setChecked(false);
      ui->InvertColorButton->setDisabled(true);
      return;
    }
    ui->ImageFileName->setText(fileName);
    ui->InvertColorButton->setEnabled(true);
    if (ui->imageLabel->width()!=128) {
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
      ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->imageLabel->width(), ui->imageLabel->height())));
    } else {
      ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->imageLabel->width(), ui->imageLabel->height()).convertToFormat(QImage::Format_Mono)));
    }
    ui->PatchFWCB->setEnabled(true);
  }
}

void burnDialog::on_libraryButton_clicked()
{
  QString fileName;
  splashLibrary *ld = new splashLibrary(this,&fileName);
  ld->exec();
  if (!fileName.isEmpty()) {
    QImage image(fileName);
    if (image.isNull()) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot load %1.").arg(fileName));
      ui->PatchFWCB->setDisabled(true);
      ui->PatchFWCB->setChecked(false);
      ui->InvertColorButton->setDisabled(true);
      return;
    }
    ui->ImageFileName->setText(fileName);
    ui->InvertColorButton->setEnabled(true);
    if (ui->imageLabel->width()!=128) {
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
      ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->imageLabel->width(), ui->imageLabel->height())));
    } else {
      ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->imageLabel->width(), ui->imageLabel->height()).convertToFormat(QImage::Format_Mono)));
    }
    ui->PatchFWCB->setEnabled(true);
  }
}

void burnDialog::on_BurnFlashButton_clicked()
{
  if (hexType==2) {
    QString fileName=ui->FWFileName->text();
    if (!fileName.isEmpty()) {
      QSettings settings("companion9x", "companion9x");
      settings.setValue("lastFlashDir", QFileInfo(fileName).dir().absolutePath());
      settings.setValue("lastFw", fileName);
      if (ui->PatchFWCB->isChecked()) {
        settings.setValue("patchImage", true);
        QImage image = ui->imageLabel->pixmap()->toImage().scaled(ui->imageLabel->width(), ui->imageLabel->height());
        if (!image.isNull()) {
          QString tempDir    = QDir::tempPath();
          QString tempFile;
          if (getFileType(fileName) == FILE_TYPE_HEX)
            tempFile = tempDir + "/flash.hex";
          else
            tempFile = tempDir + "/flash.bin";
          FlashInterface flash(fileName);
          flash.setSplash(image);
          if (flash.saveFlash(tempFile) > 0) {
            hexfileName->clear();
            hexfileName->append(tempFile);
          } else {
            hexfileName->clear();
            QMessageBox::critical(this, tr("Warning"), tr("Cannot save customized firmware"));
          }
        } else {
          hexfileName->clear();
          QMessageBox::critical(this, tr("Warning"), tr("Custom image not found"));
        }
      } else {
            settings.setValue("patchImage", false);
            hexfileName->clear();
            hexfileName->append(fileName);
      }
    } else {
      QMessageBox::critical(this, tr("Warning"), tr("No firmware selected"));
      hexfileName->clear();     
    }
  }
  if (hexType==1) {
    QSettings settings("companion9x", "companion9x");
    int profileid=settings.value("profileId", 1).toInt();
    settings.beginGroup("Profiles");
    QString profile=QString("profile%1").arg(profileid);
    settings.beginGroup(profile);
    QString calib=settings.value("StickPotCalib","").toString();
    QString trainercalib=settings.value("TrainerCalib","").toString();
    int potsnum=GetEepromInterface()->getCapability(Pots);
    int8_t vBatCalib=(int8_t)settings.value("VbatCalib", radioData.generalSettings.vBatCalib).toInt();
    int8_t currentCalib=(int8_t)settings.value("currentCalib", radioData.generalSettings.currentCalib).toInt();
    int8_t PPM_Multiplier=(int8_t)settings.value("PPM_Multiplier", radioData.generalSettings.PPM_Multiplier).toInt();
    uint8_t GSStickMode=(uint8_t)settings.value("GSStickMode", radioData.generalSettings.stickMode).toUInt();
    uint8_t vBatWarn=(uint8_t)settings.value("vBatWarn",radioData.generalSettings.vBatWarn).toUInt();
    
    QString DisplaySet=settings.value("Display","").toString();
    QString BeeperSet=settings.value("Beeper","").toString();
    QString HapticSet=settings.value("Haptic","").toString();
    QString SpeakerSet=settings.value("Speaker","").toString();
    settings.endGroup();
    settings.endGroup();
    bool patch=false;
    if (ui->patchcalib_CB->isChecked()) {
      if ((calib.length()==(NUM_STICKS+potsnum)*12) && (trainercalib.length()==16)) {
        QString Byte;
        int16_t byte16;
        bool ok;
        for (int i=0; i<(NUM_STICKS+potsnum); i++) {
          Byte=calib.mid(i*12,4);
          byte16=(int16_t)Byte.toInt(&ok,16);
          if (ok)
            radioData.generalSettings.calibMid[i]=byte16;
          Byte=calib.mid(4+i*12,4);
          byte16=(int16_t)Byte.toInt(&ok,16);
          if (ok)
            radioData.generalSettings.calibSpanNeg[i]=byte16;
          Byte=calib.mid(8+i*12,4);
          byte16=(int16_t)Byte.toInt(&ok,16);
          if (ok)
            radioData.generalSettings.calibSpanPos[i]=byte16;
        }
        for (int i=0; i<4; i++) {
          Byte=trainercalib.mid(i*4,4);
          byte16=(int16_t)Byte.toInt(&ok,16);
          if (ok)
            radioData.generalSettings.trainer.calib[i]=byte16;
        }
        radioData.generalSettings.currentCalib=currentCalib;
        radioData.generalSettings.vBatCalib=vBatCalib;
        radioData.generalSettings.PPM_Multiplier=PPM_Multiplier;
        patch=true;
      } else {
        QMessageBox::critical(this, tr("Warning"), tr("Wrong radio calibration data in profile, eeprom not patched"));
      }
    }
    if (ui->patchhw_CB->isChecked()) {
      if ((DisplaySet.length()==6) && (BeeperSet.length()==4) && (HapticSet.length()==6) && (SpeakerSet.length()==6)) {
        radioData.generalSettings.vBatWarn=vBatWarn;
        radioData.generalSettings.stickMode=GSStickMode;
        uint8_t byte8u;
        int8_t byte8;
        bool ok;
        byte8=(int8_t)DisplaySet.mid(0,2).toInt(&ok,16);
        if (ok)
          radioData.generalSettings.optrexDisplay=(byte8==1 ? true : false);
        byte8u=(uint8_t)DisplaySet.mid(2,2).toUInt(&ok,16);
        if (ok)
          radioData.generalSettings.contrast=byte8u;

        byte8u=(uint8_t)DisplaySet.mid(4,2).toUInt(&ok,16);
        if (ok)
          radioData.generalSettings.backlightBright=byte8u;

        byte8u=(uint8_t)BeeperSet.mid(0,2).toUInt(&ok,16);
        if (ok)
          radioData.generalSettings.beeperMode=(BeeperMode)byte8u;

        byte8=(int8_t)BeeperSet.mid(2,2).toInt(&ok,16);
        if (ok)
          radioData.generalSettings.beeperLength=byte8;

        byte8u=(uint8_t)HapticSet.mid(0,2).toUInt(&ok,16);
        if (ok)
          radioData.generalSettings.hapticMode=(BeeperMode)byte8u;

        byte8u=(uint8_t)HapticSet.mid(2,2).toUInt(&ok,16);
        if (ok)
          radioData.generalSettings.hapticStrength=byte8u;

        byte8=(int8_t)HapticSet.mid(4,2).toInt(&ok,16);
        if (ok)
          radioData.generalSettings.hapticLength=byte8;

        byte8u=(uint8_t)SpeakerSet.mid(0,2).toUInt(&ok,16);
        if (ok)
          radioData.generalSettings.speakerMode=byte8u;

        byte8u=(uint8_t)SpeakerSet.mid(2,2).toUInt(&ok,16);
        if (ok)
          radioData.generalSettings.speakerPitch=byte8u;

        byte8u=(uint8_t)SpeakerSet.mid(4,2).toUInt(&ok,16);
        if (ok)
          radioData.generalSettings.speakerVolume=byte8u;
        patch=true;
      } else {
        QMessageBox::critical(this, tr("Warning"), tr("Wrong radio setting data in profile, eeprom not patched"));
      }
    
      QString fileName;
      if (patch) {
        QString tempDir    = QDir::tempPath();
        fileName = tempDir + "/temp.bin";
        QFile file(fileName);

        uint8_t *eeprom = (uint8_t*)malloc(GetEepromInterface()->getEEpromSize());
        int eeprom_size = 0;

        eeprom_size = GetEepromInterface()->save(eeprom, radioData, GetCurrentFirmwareVariant());
        if (!eeprom_size) {
          QMessageBox::warning(this, tr("Error"),tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));
          hexfileName->clear();
        }

        if (!file.open(QIODevice::WriteOnly)) {
          QMessageBox::warning(this, tr("Error"),tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));
          hexfileName->clear();
        }

        QTextStream outputStream(&file);

        long result = file.write((char*)eeprom, eeprom_size);
        if(result!=eeprom_size) {
          QMessageBox::warning(this, tr("Error"),tr("Error writing file %1:\n%2.").arg(fileName).arg(file.errorString()));
          hexfileName->clear();
        }
        hexfileName->clear();
        hexfileName->append(fileName);
      } else {
        hexfileName->clear();
        hexfileName->append(ui->FWFileName->text());        
      }
    } else {
      hexfileName->clear();
      hexfileName->append(ui->FWFileName->text());        
    }
  }
  this->close();
}

void burnDialog::on_cancelButton_clicked()
{
  hexfileName->clear();     
  this->close();  
}

void burnDialog::on_InvertColorButton_clicked()
{
  if (ui->imageLabel->pixmap()) {
    QImage image = ui->imageLabel->pixmap()->toImage();
    image.invertPixels();
    ui->imageLabel->setPixmap(QPixmap::fromImage(image));
  }
}

void burnDialog::on_EEpromCB_toggled(bool checked)
{
  if (ui->EEpromCB->isChecked()) {
    *backup=true;
  } else {
    *backup=false;
  }
}

void burnDialog::on_PreferredImageCB_toggled(bool checked)
{
  QString tmpFileName;
  if (checked) {
    QSettings settings("companion9x", "companion9x");
    QString ImageStr = settings.value("SplashImage", "").toString();
    if (!ImageStr.isEmpty()) {
      QImage Image = qstring2image(ImageStr);
      if (ui->imageLabel->width()!=128) {
        Image=Image.convertToFormat(QImage::Format_RGB32);
        QRgb col;
        int gray;
        int width = Image.width();
        int height = Image.height();
        for (int i = 0; i < width; ++i)
        {
            for (int j = 0; j < height; ++j)
            {
                col = Image.pixel(i, j);
                gray = qGray(col);
                Image.setPixel(i, j, qRgb(gray, gray, gray));
            }
        }
      } else {
         Image=Image.convertToFormat(QImage::Format_Mono);
      }
      ui->imageLabel->setPixmap(QPixmap::fromImage(Image));
      ui->InvertColorButton->setEnabled(true);
      ui->PreferredImageCB->setChecked(true);
      ui->ImageFileName->setDisabled(true);
      ui->ImageLoadButton->setDisabled(true);
      ui->libraryButton->setDisabled(true);
      ui->PatchFWCB->setEnabled(true);
    }
  } else {
    ui->imageLabel->clear();
    ui->ImageLoadButton->setEnabled(true);
    ui->libraryButton->setEnabled(true);
    tmpFileName = ui->ImageFileName->text();
    if (!tmpFileName.isEmpty()) {
      QImage image(tmpFileName);
      if (!image.isNull()) {
        if (ui->imageLabel->width()!=128) {
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
        } else {
           image=image.convertToFormat(QImage::Format_Mono);
        }        
        ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->imageLabel->width(), ui->imageLabel->height())));
        ui->InvertColorButton->setEnabled(true);
        ui->ImageFileName->setEnabled(true);
        ui->PatchFWCB->setDisabled(false);
      } else {
        ui->InvertColorButton->setDisabled(true);
        ui->PatchFWCB->setDisabled(true);
        ui->PatchFWCB->setChecked(false);
      }
    } else {
      ui->InvertColorButton->setDisabled(true);
      ui->PatchFWCB->setDisabled(true);
      ui->PatchFWCB->setChecked(false);
    }
  }
}

void burnDialog::shrink()
{
    resize(0,0);
}

void burnDialog::on_EEbackupCB_clicked()
{
  if (ui->EEbackupCB->isChecked()) {
    *backup=true;
  } else {
    *backup=false;
  }
}
