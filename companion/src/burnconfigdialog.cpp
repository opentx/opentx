/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "burnconfigdialog.h"
#include "ui_burnconfigdialog.h"
#include "eeprominterface.h"
#include "helpers.h"
#include "appdata.h"
#include "progressdialog.h"
#include "process_flash.h"

#if !defined _MSC_VER && defined __GNUC__
#include <unistd.h>
#endif

burnConfigDialog::burnConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::burnConfigDialog)
{
    ui->setupUi(this);
    setWindowIcon(CompanionIcon("configure.png"));
    ui->avrdude_programmer->model()->sort(0);

    getSettings();
    populateProgrammers();
    Board::Type board = getCurrentBoard();
    if (IS_STM32(board)) {
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
    }
    else if (IS_SKY9X(board)) {
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
    }
    else {
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
    connect(this, SIGNAL(accepted()), this, SLOT(putSettings()));
}

burnConfigDialog::~burnConfigDialog()
{
    delete ui;
}

QStringList burnConfigDialog::getAvrdudeArgs()
{
    QStringList args = avrArgs;
    if (!avrPort.isEmpty())
      args << "-P" << avrPort;

#if defined(__APPLE__)
    args << "-C" << QFileInfo(QApplication::applicationDirPath() + "/../Resources/avrdude.conf").absoluteFilePath();
#endif
    return args;
}

void burnConfigDialog::getSettings()
{
    avrLoc   = g.avrdudeLocation();
    sambaLoc = g.sambaLocation();
    dfuLoc =   g.dfuLocation();

#if defined WIN32 || !defined __GNUC__
    if ( avrLoc.isEmpty())
      avrLoc = QFileInfo("avrdude.exe").absoluteFilePath();
    if ( sambaLoc.isEmpty())
      sambaLoc = QFileInfo("sam-ba.exe").absoluteFilePath();
    if ( dfuLoc.isEmpty())
      dfuLoc =  QFileInfo("dfu-util.exe").absoluteFilePath();
#elif defined __APPLE__
    if ( avrLoc.isEmpty())
      avrLoc = QFileInfo(QApplication::applicationDirPath() + "/../Resources/avrdude").absoluteFilePath();
    if ( sambaLoc.isEmpty())
      sambaLoc = "/usr/local/bin/sam-ba";
    if ( dfuLoc.isEmpty())
      dfuLoc = QFileInfo(QApplication::applicationDirPath() + "/../Resources/dfu-util").absoluteFilePath();
#elif defined __FreeBSD__
    if (avrLoc.isEmpty())
      avrLoc = "/usr/local/bin/avrdude";
    if (dfuLoc.isEmpty())
      dfuLoc = "/usr/local/bin/dfu-util";
#else
    if ( avrLoc.isEmpty())
      avrLoc = "/usr/bin/avrdude";
    if ( sambaLoc.isEmpty())
      sambaLoc = "/usr/bin/sam-ba";
    if ( dfuLoc.isEmpty())
      dfuLoc =  QFileInfo("/usr/bin/dfu-util").absoluteFilePath();
#endif

    dfuArgs = g.dfuArguments().split(" ", QString::SkipEmptyParts);
    avrArgs = g.avrArguments().split(" ", QString::SkipEmptyParts);
    avrProgrammer =  g.programmer();
    avrPort = g.avrPort();
    avrMCU = g.mcu();
    armMCU = g.armMcu();
    sambaPort = g.sambaPort();

    ui->avrdude_location->setText(getAVRDUDE());
    ui->avrArgs->setText(avrArgs.join(" "));

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
    g.avrdudeLocation( avrLoc );
    g.programmer( avrProgrammer);
    g.mcu( avrMCU );
    g.avrPort( avrPort );
    g.avrArguments( avrArgs.join(" ") );
    g.sambaLocation( sambaLoc );
    g.sambaPort( sambaPort );
    g.armMcu( armMCU );
    g.dfuLocation( dfuLoc );
    g.dfuArguments( dfuArgs.join(" ") );
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

void burnConfigDialog::listAvrdudeProgrammers()
{
  ProgressDialog progressDialog(this, tr("List available programmers"), CompanionIcon("list.png"), true);
  FlashProcess flashProcess(ui->avrdude_location->text(), QStringList() << "-c?", progressDialog.progress());
  flashProcess.run();
}

// TODO choose better name when no merge in progress....
void burnConfigDialog::on_pushButton_3_clicked()
{
  listAvrdudeProgrammers();
}

// TODO choose better name when no merge in progress....
void burnConfigDialog::on_pushButton_4_clicked()
{
  ProgressDialog progressDialog(this, tr("Avrdude help"), CompanionIcon("configure.png"), true);
  FlashProcess flashProcess(ui->avrdude_location->text(), QStringList() << "-?", progressDialog.progress());
  flashProcess.run();
}

void burnConfigDialog::on_advCtrChkB_toggled(bool checked)
{
  Board::Type board = getCurrentBoard();
  if (checked) {
    if (IS_STM32(board)) {
      ui->label_dfu2->show();
      ui->dfuArgs->show();
    }
    else if (IS_SKY9X(board)) {
      ui->label_sb2->show();
      ui->arm_mcu->show();
    }
    else {
      ui->label_av3->show();
      ui->avrdude_mcu->show();
      QMessageBox::warning(this, CPN_STR_APP_NAME,
        tr("<b><u>WARNING!</u></b><br>Normally CPU type is automatically selected according to the chosen firmware.<br>If you change the CPU type the resulting eeprom could be inconsistent."),
        QMessageBox::Ok);
    }
  }
  else {
    if (IS_STM32(board)) {
      ui->label_dfu2->hide();
      ui->dfuArgs->hide();
    }
    else if (IS_SKY9X(board)) {
      ui->label_sb2->hide();
      ui->arm_mcu->hide();
    }
    else {
      ui->label_av3->hide();
      ui->avrdude_mcu->hide();
    }
  }
  QTimer::singleShot(0, this, SLOT(shrink()));
}


void burnConfigDialog::shrink()
{
  resize(0, 0);
}



