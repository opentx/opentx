#include "burndialog.h"
#include "ui_burndialog.h"

#include <QtGui>
#include "eeprominterface.h"
#include "helpers.h"
#include "splashlibrary.h"
#include "flashinterface.h"
#include "hexinterface.h"
#include "appdata.h"

// Type 1 = Burn EEPROM, Type 2= Burn Flash
burnDialog::burnDialog(QWidget *parent, int Type, QString * fileName, bool * backupEE, QString DocName):
  QDialog(parent),
  ui(new Ui::burnDialog),
  hexfileName(fileName),
  backup(backupEE),
  hexType(Type)
{
  ui->setupUi(this);
  setWindowIcon(CompanionIcon("write_flash.png"));

  if (!g.profile[g.id()].splashFile().isEmpty()){
    imageSource=PROFILE;
    imageFile=g.profile[g.id()].splashFile();
  }
  else {
    ui->useProfileImageCB->setDisabled(true);
    imageSource=FIRMWARE;
    imageFile="";
  }
  ui->SplashFrame->hide();
  ui->FramFWInfo->hide();
  ui->EEbackupCB->hide();
  ui->EEbackupCB->setCheckState(*backup ? Qt::Checked : Qt::Unchecked);
  if (Type == FLASH_FILE_TYPE ) {
    ui->EEpromCB->hide();
    ui->profile_label->hide();
    ui->patchcalib_CB->hide();
    ui->patchhw_CB->hide();
    setWindowTitle(tr("Write firmware to Radio"));
    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      ui->EEbackupCB->hide();
    }
  }
  else {
    ui->FlashLoadButton->setText(tr("Browse for file"));
    ui->profile_label->hide();
    ui->patchcalib_CB->hide();
    ui->patchhw_CB->hide();
    ui->EEpromCB->hide();
    ui->BurnFlashButton->setDisabled(true);
    ui->FWFileName->clear();
    ui->DateField->clear();
    ui->versionField->clear();
    ui->ModField->clear();
    ui->FramFWInfo->hide();
    ui->SplashFrame->hide();
    ui->BurnFlashButton->setDisabled(true);
    ui->EEbackupCB->hide();
    if (DocName.isEmpty()) {
      setWindowTitle(tr("Write Backup to Radio"));
    }
    else {
      setWindowTitle(tr("Write Backup from %1 to Radio").arg(DocName));
    }
    ui->profile_label->setText(tr("Current profile")+QString(": ")+g.profile[g.id()].name());
  }
  if (!hexfileName->isEmpty()) {
    ui->FWFileName->setText(*hexfileName);
    if (Type==FLASH_FILE_TYPE) {
      checkFw(*hexfileName);
    }
    else {
      burnraw=false;
      if (checkeEprom(*hexfileName)) {
        QString Name = g.profile[g.id()].name();
        QString calib = g.profile[g.id()].stickPotCalib();
        QString trainercalib = g.profile[g.id()].trainerCalib();
        QString DisplaySet = g.profile[g.id()].display();
        QString BeeperSet = g.profile[g.id()].beeper();
        QString HapticSet = g.profile[g.id()].haptic();
        QString SpeakerSet = g.profile[g.id()].speaker();
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
        }
        else {
          ui->profile_label->hide();
        }
        if (!IS_TARANIS(GetEepromInterface()->getBoard())) {
          ui->EEpromCB->show();
        }
        else {
          ui->EEpromCB->setChecked(false);
        }
        ui->BurnFlashButton->setEnabled(true);
      }
    }
    ui->FWFileName->hide();
    ui->FlashLoadButton->hide();   
    hexfileName->clear();
  }
  else if (Type==FLASH_FILE_TYPE) {
    QString FileName = g.profile[g.id()].fwName();
    QFile file(FileName);
    if (file.exists()) {
      checkFw(FileName);
    }
  }
  updateUI();
  resize(0, 0);
}

burnDialog::~burnDialog()
{
  delete ui;
}

void burnDialog::on_FlashLoadButton_clicked()
{
  QString fileName;

  ui->BurnFlashButton->setDisabled(true);
  ui->FWFileName->clear();
  ui->DateField->clear();
  ui->versionField->clear();
  ui->ModField->clear();
  ui->FramFWInfo->hide();
  ui->SplashFrame->hide();
  ui->BurnFlashButton->setDisabled(true);
  ui->EEbackupCB->hide();
  QTimer::singleShot(0, this, SLOT(shrink()));
  if (hexType==FLASH_FILE_TYPE) {
    fileName = QFileDialog::getOpenFileName(this, tr("Open Firmware File"), g.flashDir(), FLASH_FILES_FILTER);
    if(fileName.isEmpty())
      return;
    checkFw(fileName);
  }
  else {
    QString fileName = QFileDialog::getOpenFileName(this,tr("Choose Radio Backup file"), g.eepromDir(), tr(EXTERNAL_EEPROM_FILES_FILTER));
    if (checkeEprom(fileName)) {
      if (burnraw==false) {
        ui->BurnFlashButton->setEnabled(true);
        ui->profile_label->show();
        ui->patchcalib_CB->show();
        ui->patchhw_CB->show();
        if (!IS_TARANIS(GetEepromInterface()->getBoard())) {
          ui->EEpromCB->show();
        }
        else {
          ui->EEpromCB->setChecked(false);
        }
      }
      else {
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
  updateUI();
}

void burnDialog::checkFw(QString fileName)
{
  if (fileName.isEmpty()) {
    return;
  }

  if (!IS_TARANIS(GetEepromInterface()->getBoard())) {
    ui->EEbackupCB->show();
  }
  else {
    ui->EEbackupCB->setChecked(false);
    *backup=false;
  }
  ui->FWFileName->setText(fileName);
  FlashInterface flash(fileName);
  if (flash.isValid()) {
    ui->FramFWInfo->show();
    ui->DateField->setText(flash.getDate() + " " + flash.getTime());
    ui->versionField->setText(flash.getVersion());
    ui->ModField->setText(flash.getEEpromId());

    ui->SplashFrame->hide();
    if (flash.hasSplash()) {
      ui->SplashFrame->show();
      ui->imageLabel->setFixedSize(flash.getSplashWidth(), flash.getSplashHeight());
    }
  }
  else {
    QMessageBox::warning(this, tr("Warning"), tr("%1 may not be a valid firmware file").arg(fileName));
  }  
  ui->BurnFlashButton->setEnabled(true);
  QTimer::singleShot(0, this, SLOT(shrink()));
  g.flashDir( QFileInfo(fileName).dir().absolutePath() );
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
#if 0
  if (fileType==FILE_TYPE_XML) {
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
      QMessageBox::critical(this, tr("Error"),tr("Error opening file %1:\n%2.").arg(fileName).arg(file.errorString()));
      return false;
    }
    QTextStream inputStream(&file);
    XmlInterface(inputStream).load(radioData);
  }
  else
#endif
  if (fileType==FILE_TYPE_HEX || fileType==FILE_TYPE_EEPE) { //read HEX file
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
      int res = QMessageBox::question(this, "Companion",tr("Invalid binary Models and Settings File %1, Proceed anyway ?").arg(fileName),QMessageBox::Yes | QMessageBox::No);
      if (res == QMessageBox::No) {
        return false;
      }
      burnraw=true;
      ui->FWFileName->setText(fileName);
      return true;
    }
    file.close();
    if (!LoadEeprom(radioData, eeprom, eeprom_size)) {
      int res = QMessageBox::question(this, "Companion",tr("Invalid binary Models and Settings File %1, Proceed anyway ?").arg(fileName),QMessageBox::Yes | QMessageBox::No);
      if (res == QMessageBox::No) {
        return false;
      }
      burnraw=true;
      ui->FWFileName->setText(fileName);
      return true;
    }
  }
  else if (fileType==FILE_TYPE_BIN) { //read binary
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
      int res = QMessageBox::question(this, "Companion",tr("Invalid binary Models and Settings File %1, Proceed anyway ?").arg(fileName),QMessageBox::Yes | QMessageBox::No);
      if (res == QMessageBox::No) {
        return false;
      }
      burnraw=true;
    }
  }
  ui->FWFileName->setText(fileName);
  return true;
}

void burnDialog::displaySplash()
{
  QImage image;
  if (imageSource == FIRMWARE){
    FlashInterface flash(ui->FWFileName->text());
    image = flash.getSplash();
  }
  else {
    image.load(imageFile);
  }
  if (image.isNull()) {
    return;
  }
  ui->imageLabel->setPixmap( makePixMap( image, g.profile[g.id()].fwType()));
}

void burnDialog::updateUI()
{
  if (hexType==EEPROM_FILE_TYPE)
    return;

  ui->useProfileImageCB->setChecked( imageSource == PROFILE );
  ui->useFwImageCB->setChecked( imageSource == FIRMWARE );
  ui->useLibraryImageCB->setChecked( imageSource == LIBRARY );
  ui->useAnotherImageCB->setChecked( imageSource == ANOTHER );
  
  displaySplash();
}

void burnDialog::on_useFwImageCB_clicked()
{
  QString fileName = ui->FWFileName->text();
  FlashInterface flash(fileName);
  if (!flash.isValid()) {
    QMessageBox::critical(this, tr("Error"), tr( "The firmware file is not valid." ));
  }
  else {
    if (!flash.hasSplash()) {
      QMessageBox::critical(this, tr("Error"), tr( "There is no start screen image in the firmware file." ));
    }
    else{
      imageSource = FIRMWARE;
      imageFile = fileName;
    }
  }
  updateUI();
}

void burnDialog::on_useProfileImageCB_clicked()
{
  QString fileName = g.profile[g.id()].splashFile();
  if (!fileName.isEmpty()){
    QImage image(fileName);
    if (image.isNull()) {
      QMessageBox::critical(this, tr("Error"), tr("Profile image %1 is invalid.").arg(fileName));
    }
    else {
      imageSource = PROFILE;
      imageFile = fileName;
    }
  }
  updateUI();
}

void burnDialog::on_useAnotherImageCB_clicked()
{
  QString supportedImageFormats;
  for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
    supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
  }
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open image file to use as radio start screen"), g.imagesDir(), tr("Images (%1)").arg(supportedImageFormats));
  if (!fileName.isEmpty()){
    g.imagesDir( QFileInfo(fileName).dir().absolutePath() );
    QImage image(fileName);
    if (image.isNull()) {
      QMessageBox::critical(this, tr("Error"), tr("Image could not be loaded from %1").arg(fileName));
    }
    else{
      imageSource = ANOTHER;
      imageFile = fileName;
    }  
  }
  updateUI();
}

void burnDialog::on_useLibraryImageCB_clicked()
{
  QString fileName;
  splashLibrary *ld = new splashLibrary(this,&fileName);
  ld->exec();
  if (!fileName.isEmpty()) {
    QImage image(fileName);
    if (image.isNull()) {
      QMessageBox::critical(this, tr("Error"), tr("The library image could not be loaded"));
    }
    else{
      imageSource = LIBRARY;
      imageFile = fileName;
    }
  }
  updateUI();
}

void burnDialog::on_BurnFlashButton_clicked()
{
  if (hexType==FLASH_FILE_TYPE) {
    QString fileName=ui->FWFileName->text();
    if (!fileName.isEmpty()) {
      g.flashDir( QFileInfo(fileName).dir().absolutePath() );
      if (!ui->useFwImageCB->isChecked()) {
        const QPixmap * pixmap = ui->imageLabel->pixmap();
        QImage image;
        if (pixmap) {
          image = pixmap->toImage().scaled(ui->imageLabel->width(), ui->imageLabel->height());
        }
        if (!image.isNull()) {
          QString tempFile;
          if (getFileType(fileName) == FILE_TYPE_HEX)
            tempFile = generateProcessUniqueTempFileName("flash.hex");
          else
            tempFile = generateProcessUniqueTempFileName("flash.bin");
          FlashInterface flash(fileName);
          flash.setSplash(image);
          if (flash.saveFlash(tempFile) > 0) {
            hexfileName->clear();
            hexfileName->append(tempFile);
          }
          else {
            hexfileName->clear();
            QMessageBox::critical(this, tr("Warning"), tr("Cannot save customized firmware"));
          }
        }
        else {
          hexfileName->clear();
          QMessageBox::critical(this, tr("Warning"), tr("Custom image not found"));
        }
      }
      else {
        hexfileName->clear();
        hexfileName->append(fileName);
      }
    }
    else {
      QMessageBox::critical(this, tr("Warning"), tr("No firmware selected"));
      hexfileName->clear();     
    }
  }

  if (hexType==EEPROM_FILE_TYPE) {
    QString calib = g.profile[g.id()].stickPotCalib();
    QString trainercalib = g.profile[g.id()].trainerCalib();
    int potsnum=GetCurrentFirmware()->getCapability(Pots);
    int8_t vBatCalib=(int8_t) g.profile[g.id()].vBatCalib();
    int8_t currentCalib=(int8_t) g.profile[g.id()].currentCalib();
    int8_t PPM_Multiplier=(int8_t) g.profile[g.id()].ppmMultiplier();
    uint8_t GSStickMode=(uint8_t) g.profile[g.id()].gsStickMode();
    uint8_t vBatWarn=(uint8_t) g.profile[g.id()].vBatWarn();
    
    QString DisplaySet= g.profile[g.id()].display();
    QString BeeperSet= g.profile[g.id()].beeper();
    QString HapticSet= g.profile[g.id()].haptic();
    QString SpeakerSet= g.profile[g.id()].speaker();
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
      }
      else {
        QMessageBox::critical(this, tr("Warning"), tr("Wrong radio calibration data in profile, Settings not patched"));
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
          radioData.generalSettings.beeperMode=(GeneralSettings::BeeperMode)byte8u;

        byte8=(int8_t)BeeperSet.mid(2,2).toInt(&ok,16);
        if (ok)
          radioData.generalSettings.beeperLength=byte8;

        byte8u=(uint8_t)HapticSet.mid(0,2).toUInt(&ok,16);
        if (ok)
          radioData.generalSettings.hapticMode=(GeneralSettings::BeeperMode)byte8u;

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
      }
      else {
        QMessageBox::critical(this, tr("Warning"), tr("Wrong radio setting data in profile, Settings not patched"));
      }
    
      if (patch) {
        QString fileName = generateProcessUniqueTempFileName("temp.bin");
        QFile file(fileName);

        uint8_t *eeprom = (uint8_t*)malloc(GetEepromInterface()->getEEpromSize());
        int eeprom_size = GetEepromInterface()->save(eeprom, radioData, GetCurrentFirmware()->getVariantNumber());
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
        if (result != eeprom_size) {
          QMessageBox::warning(this, tr("Error"),tr("Error writing file %1:\n%2.").arg(fileName).arg(file.errorString()));
        }

        hexfileName->clear();
        hexfileName->append(fileName);
      }
      else {
        hexfileName->clear();
        hexfileName->append(ui->FWFileName->text());        
      }
    }
    else {
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

void burnDialog::on_EEpromCB_toggled(bool checked)
{
  *backup = ui->EEpromCB->isChecked();
}

void burnDialog::shrink()
{
    resize(0,0);
}

void burnDialog::on_EEbackupCB_clicked()
{
  *backup = ui->EEbackupCB->isChecked();
}
