#include "burnconfigdialog.h"
#include "ui_burnconfigdialog.h"
#include "avroutputdialog.h"
#include "eeprominterface.h"
#include <QtGui>

#if !defined WIN32 && defined __GNUC__
#include <unistd.h>
#endif

burnConfigDialog::burnConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::burnConfigDialog)
{
    ui->setupUi(this);
    ui->avrdude_programmer->model()->sort(0);

    getSettings();
    populateProgrammers();
    EEPROMInterface *eepromInterface = GetEepromInterface();
    if (IS_TARANIS(eepromInterface->getBoard())) {
      setWindowTitle(tr("DFU-UTIL Configuration"));
      ui->avrArgs->hide();
      ui->avrdude_location->hide();
      ui->avrdude_port->hide();
      ui->avrdude_programmer->hide();
      ui->label_av1->hide();
      ui->label_av2->hide();
      ui->label_av4->hide();
      ui->label_av5->hide();
      ui->pushButton->hide();
      ui->pushButton_3->hide();
      ui->pushButton_4->hide();
      ui->label_sb1->hide();
      ui->label_sb3->hide();
      ui->samba_location->hide();
      ui->samba_port->hide();      
      ui->sb_browse->hide();
    } else if (eepromInterface->getBoard()==BOARD_SKY9X) {
      setWindowTitle(tr("SAM-BA Configuration"));
      ui->avrArgs->hide();
      ui->avrdude_location->hide();
      ui->avrdude_port->hide();
      ui->avrdude_programmer->hide();
      ui->label_av1->hide();
      ui->label_av2->hide();
      ui->label_av4->hide();
      ui->label_av5->hide();
      ui->pushButton->hide();
      ui->pushButton_3->hide();
      ui->pushButton_4->hide();
      ui->label_dfu1->hide();
      ui->dfu_location->hide();
      ui->dfu_browse->hide();
    } else {
      setWindowTitle(tr("AVRDUDE Configuration"));
      ui->label_sb1->hide();
      ui->label_sb3->hide();
      ui->samba_location->hide();
      ui->samba_port->hide();
      ui->sb_browse->hide();
      ui->label_dfu1->hide();
      ui->label_dfu2->hide();
      ui->dfu_location->hide();
      ui->dfu_browse->hide();
    }
    ui->label_av3->hide();
    ui->avrdude_mcu->hide();
    ui->label_sb2->hide();
    ui->arm_mcu->hide();
    ui->label_dfu2->hide();
    ui->dfuArgs->hide();

    QTimer::singleShot(0, this, SLOT(shrink()));
    connect(this,SIGNAL(accepted()),this,SLOT(putSettings()));
}

burnConfigDialog::~burnConfigDialog()
{
    delete ui;
}

void burnConfigDialog::getSettings()
{
    QSettings settings("companion9x", "companion9x");
#if defined WIN32 || !defined __GNUC__
    avrLoc   = settings.value("avrdude_location", QFileInfo("avrdude.exe").absoluteFilePath()).toString();
    sambaLoc = settings.value("samba_location", QFileInfo("sam-ba.exe").absoluteFilePath()).toString();
    dfuLoc = settings.value("dfu_location", QFileInfo("dfu-util.exe").absoluteFilePath()).toString();
#elif defined __APPLE__
    avrLoc   = settings.value("avrdude_location", "/usr/local/bin/avrdude").toString();
    sambaLoc = settings.value("samba_location", "/usr/local/bin/sam-ba").toString();
    dfuLoc = settings.value("dfu_location", QFileInfo("/opt/local/bin/dfu-util").absoluteFilePath()).toString();
#else
    avrLoc   = settings.value("avrdude_location", "/usr/bin/avrdude").toString();
    sambaLoc = settings.value("samba_location", "/usr/bin/sam-ba").toString();
    dfuLoc = settings.value("dfu_location", QFileInfo("/usr/bin/dfu-util").absoluteFilePath()).toString();
#endif
    QString str = settings.value("avr_arguments").toString();
    avrArgs = str.split(" ", QString::SkipEmptyParts);

    avrProgrammer =  settings.value("programmer", QString("usbasp")).toString();

    avrMCU = settings.value("mcu", QString("m64")).toString();
    armMCU = settings.value("arm_mcu", QString("at91sam3s4-9x")).toString();

    avrPort   = settings.value("avr_port", "").toString();
    sambaPort = settings.value("samba_port", "\\USBserial\\COM23").toString();
    
    str = settings.value("dfu_arguments", "-a 0").toString();
    dfuArgs = str.split(" ", QString::SkipEmptyParts);
    ui->avrdude_location->setText(getAVRDUDE());
    ui->avrArgs->setText(getAVRArgs().join(" "));

    ui->samba_location->setText(getSAMBA());
    ui->samba_port->setText(getSambaPort());

    ui->dfu_location->setText(getDFU());
    ui->dfuArgs->setText(getDFUArgs().join(" "));

    int idx1 = ui->avrdude_programmer->findText(getProgrammer());
    int idx2 = ui->avrdude_port->findText(getPort());
    int idx3 = ui->avrdude_mcu->findText(getMCU());
    int idx4 = ui->arm_mcu->findText(getArmMCU());
    if(idx1>=0) ui->avrdude_programmer->setCurrentIndex(idx1);
    if(idx2>=0) ui->avrdude_port->setCurrentIndex(idx2);
    if(idx3>=0) ui->avrdude_mcu->setCurrentIndex(idx3);
    if(idx4>=0) ui->arm_mcu->setCurrentIndex(idx4);
    QFile file;
    if (file.exists(avrLoc)) {
      ui->pushButton_3->setEnabled(true);
    } else {
      ui->pushButton_3->setDisabled(true);
    }    
}

void burnConfigDialog::putSettings()
{
    QSettings settings("companion9x", "companion9x");
    settings.setValue("avrdude_location", avrLoc);
    settings.setValue("programmer", avrProgrammer);
    settings.setValue("mcu", avrMCU);
    settings.setValue("avr_port", avrPort);
    settings.setValue("avr_arguments", avrArgs.join(" "));
    settings.setValue("samba_location", sambaLoc);
    settings.setValue("samba_port", sambaPort);
    settings.setValue("arm_mcu", armMCU);
    settings.setValue("dfu_location", dfuLoc);
    settings.setValue("dfu_arguments", dfuArgs.join(" "));
}

void burnConfigDialog::populateProgrammers()
{
    QString fileName = QFileInfo(avrLoc).canonicalPath() + "/avrdude.conf"; //for windows
    if(!QFileInfo(fileName).exists()) fileName = "/etc/avrdude.conf"; //for linux
    if(!QFileInfo(fileName).exists()) fileName = "/etc/avrdude/avrdude.conf"; //for linux
    if(!QFileInfo(fileName).exists()) return; // not found avrdude.conf - returning

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QStringList items;
    QString line = "";
    QString prevline = "";
    QTextStream in(&file);
    while (!in.atEnd())
    {
        prevline = line;
        line = in.readLine();

        if(prevline.left(10).toLower()=="programmer")
            items << line.section('"',1,1);
    }
    file.close();

    items.sort();

    QString avrProgrammer_temp = avrProgrammer;
    ui->avrdude_programmer->clear();
    ui->avrdude_programmer->addItems(items);
    int idx1 = ui->avrdude_programmer->findText(avrProgrammer_temp);
    if(idx1>=0) ui->avrdude_programmer->setCurrentIndex(idx1);
}

void burnConfigDialog::on_avrdude_programmer_currentIndexChanged(QString )
{
    avrProgrammer = ui->avrdude_programmer->currentText();
}

void burnConfigDialog::on_avrdude_mcu_currentIndexChanged(QString )
{
    avrMCU = ui->avrdude_mcu->currentText();
}

void burnConfigDialog::on_avrdude_location_editingFinished()
{
    avrLoc = ui->avrdude_location->text();
    if (avrLoc.isEmpty()) {
      ui->pushButton_3->setDisabled(true);
    } else {
      QFile file;
      if (file.exists(avrLoc)) {
        ui->pushButton_3->setEnabled(true);
      }
    }
}

void burnConfigDialog::on_avrArgs_editingFinished()
{
    avrArgs = ui->avrArgs->text().split(" ", QString::SkipEmptyParts);
}

void burnConfigDialog::on_avrdude_port_currentIndexChanged(QString )
{
    avrPort = ui->avrdude_port->currentText();
}

void burnConfigDialog::on_samba_location_editingFinished()
{
    sambaLoc = ui->samba_location->text();
}

void burnConfigDialog::on_samba_port_editingFinished()
{
    sambaPort = ui->samba_port->text();
}

void burnConfigDialog::on_arm_mcu_currentIndexChanged(QString )
{
    armMCU = ui->arm_mcu->currentText();
}

void burnConfigDialog::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Location"),ui->avrdude_location->text());

    if(!fileName.isEmpty())
    {
        ui->avrdude_location->setText(fileName);
        avrLoc = fileName;
    }
}

void burnConfigDialog::on_sb_browse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Location"),ui->samba_location->text());
    if(!fileName.isEmpty())
    {
        ui->samba_location->setText(fileName);
        sambaLoc = fileName;
    }
}

void burnConfigDialog::on_dfu_browse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Location"),ui->dfu_location->text());

    if(!fileName.isEmpty())
    {
        ui->dfu_location->setText(fileName);
        dfuLoc = fileName;
    }
}

void burnConfigDialog::on_dfu_location_editingFinished()
{
    dfuLoc = ui->dfu_location->text();
}

void burnConfigDialog::on_dfuArgs_editingFinished()
{
    dfuArgs = ui->dfuArgs->text().split(" ", QString::SkipEmptyParts);
}

void burnConfigDialog::listProgrammers()
{
    QStringList arguments;
    arguments << "-c?";
    avrOutputDialog *ad = new avrOutputDialog(this, ui->avrdude_location->text(), arguments, "List available programmers", AVR_DIALOG_KEEP_OPEN, TRUE);
    ad->setWindowIcon(QIcon(":/images/list.png"));
    ad->show();
}

void burnConfigDialog::on_pushButton_3_clicked()
{
    listProgrammers();
}



void burnConfigDialog::on_pushButton_4_clicked()
{
    QStringList arguments;
    arguments << "-?";

    avrOutputDialog *ad = new avrOutputDialog(this, ui->avrdude_location->text(), arguments, "Show help", AVR_DIALOG_KEEP_OPEN,TRUE);
    ad->setWindowIcon(QIcon(":/images/configure.png"));
    ad->show();
}


void burnConfigDialog::readFuses()
{
    QStringList args   = avrArgs;
    if(!avrPort.isEmpty()) args << "-P" << avrPort;

    QStringList str;
    str << "-U" << "lfuse:r:-:i" << "-U" << "hfuse:r:-:i" << "-U" << "efuse:r:-:i";

    QStringList arguments;
    arguments << "-c" << avrProgrammer << "-p" << avrMCU << args << str;

    avrOutputDialog *ad = new avrOutputDialog(this, avrLoc, arguments, "Read Fuses",AVR_DIALOG_KEEP_OPEN,TRUE);
    ad->setWindowIcon(QIcon(":/images/fuses.png"));
    ad->show();
}

void burnConfigDialog::restFuses(bool eeProtect)
{
    //fuses
    //avrdude -c usbasp -p m64 -U lfuse:w:<0x0E>:m
    //avrdude -c usbasp -p m64 -U hfuse:w:<0x89>:m  0x81 for eeprom protection
    //avrdude -c usbasp -p m64 -U efuse:w:<0xFF>:m

    QMessageBox::StandardButton ret = QMessageBox::No;

    ret = QMessageBox::warning(this, tr("companion9x"),
                               tr("<b><u>WARNING!</u></b><br>This will reset the fuses of  %1 to the factory settings.<br>Writing fuses can mess up your radio.<br>Do this only if you are sure they are wrong!<br>Are you sure you want to continue?").arg(avrMCU),
                               QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
    {
        QStringList args   = avrArgs;
        if(!avrPort.isEmpty()) args << "-P" << avrPort;
        QStringList str;
        if (avrMCU=="m2560") {
          args << "-B8";
          QString erStr = eeProtect ? "hfuse:w:0x11:m" : "hfuse:w:0x19:m";
          str << "-U" << "lfuse:w:0xD7:m" << "-U" << erStr << "-U" << "efuse:w:0xFC:m";
          //use hfuse = 0x81 to prevent eeprom being erased with every flashing          
        } else {
          QString tempDir    = QDir::tempPath();
          QString tempFile;
          QString lfuses;
          tempFile = tempDir + "/ftemp.bin";
          QStringList argread;
          argread << "-c" << avrProgrammer << "-p" << avrMCU << args  <<"-U" << "lfuse:r:"+tempFile+":r" ;
          avrOutputDialog *ad = new avrOutputDialog(this, avrLoc, argread, "Reset Fuses",AVR_DIALOG_CLOSE_IF_SUCCESSFUL,FALSE);
          ad->setWindowIcon(QIcon(":/images/fuses.png"));
          ad->exec();
          QFile file(tempFile);
          if (file.exists() && file.size()==1) {
            file.open(QIODevice::ReadOnly);
            char bin_flash[1];
            file.read(bin_flash, 1);
            if (bin_flash[0]==0x0E) {
              lfuses="lfuse:w:0x0E:m";
            } else {
              lfuses="lfuse:w:0x3F:m";
            } 
            file.close();
            unlink(tempFile.toAscii());
          } else {
            lfuses="lfuse:w:0x3F:m";
          }
          
          QString erStr = eeProtect ? "hfuse:w:0x81:m" : "hfuse:w:0x89:m";
          str << "-U" << lfuses << "-U" << erStr << "-U" << "efuse:w:0xFF:m";
          //use hfuse = 0x81 to prevent eeprom being erased with every flashing
        }
        QStringList arguments;
        if (avrMCU=="m2560") {
          arguments << "-c" << avrProgrammer << "-p" << avrMCU << args << "-u" << str;
        } else {
          arguments << "-c" << avrProgrammer << "-p" << avrMCU << args << "-B" << "100" << "-u" << str;          
        }
        avrOutputDialog *ad = new avrOutputDialog(this, avrLoc, arguments, "Reset Fuses",AVR_DIALOG_KEEP_OPEN,TRUE);
        ad->setWindowIcon(QIcon(":/images/fuses.png"));
        ad->show();
    }

}

void burnConfigDialog::on_advCtrChkB_toggled(bool checked)
{
  EEPROMInterface *eepromInterface = GetEepromInterface();
  if (checked) {
    if (IS_TARANIS(eepromInterface->getBoard())) {
      ui->label_dfu2->show();
      ui->dfuArgs->show();
    } else if (eepromInterface->getBoard()==BOARD_SKY9X) {
      ui->label_sb2->show();
      ui->arm_mcu->show();
    } else {
      ui->label_av3->show();
      ui->avrdude_mcu->show();
      QMessageBox::warning(this, tr("companion9x"),
        tr("<b><u>WARNING!</u></b><br>Normally CPU type is automatically selected according to the chosen firmware.<br>If you change the CPU type the resulting eeprom could be inconsistent."),
        QMessageBox::Ok);
    }
  } else {
    if (IS_TARANIS(eepromInterface->getBoard())) {
      ui->label_dfu2->hide();
      ui->dfuArgs->hide();
    } else if (eepromInterface->getBoard()==BOARD_SKY9X) {
      ui->label_sb2->hide();
      ui->arm_mcu->hide();
    } else {
      ui->label_av3->hide();
      ui->avrdude_mcu->hide();
    }
  }
  QTimer::singleShot(0, this, SLOT(shrink()));
}


void burnConfigDialog::shrink()
{
  resize(0,0);
}



