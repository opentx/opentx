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

#include "radiointerface.h"
#include "appdata.h"
#include "eeprominterface.h"
#include "process_flash.h"
#include "radionotfound.h"
#include "burnconfigdialog.h"
#include "helpers.h"
#include "process_copy.h"
#include "storage.h"
#include "progresswidget.h"

QString getRadioInterfaceCmd()
{
  burnConfigDialog bcd;
  Board::Type board = getCurrentBoard();
  if (IS_STM32(board)) {
    return bcd.getDFU();
  }
  else if (IS_SKY9X(board)) {
    return bcd.getSAMBA();
  }
  else {
    return bcd.getAVRDUDE();
  }
}

QStringList getAvrdudeArgs(const QString & cmd, const QString & filename)
{
  QStringList args;
  burnConfigDialog bcd;
  QString programmer = bcd.getProgrammer();
  QString mcu = bcd.getMCU();
  Board::Type board = getCurrentBoard();

  args << "-c" << programmer << "-p";
  if (IS_2560(board))
    args << "m2560";
  else if (board == Board::BOARD_M128)
    args << "m128";
  else
    args << mcu;

  args << bcd.getAvrdudeArgs();

  QString fullcmd = cmd + filename;
  if (QFileInfo(filename).suffix().toUpper() == "HEX")
    fullcmd += ":i";
  else if (QFileInfo(filename).suffix().toUpper()=="BIN")
    fullcmd += ":r";
  else
    fullcmd += ":a";

  args << "-U" << fullcmd;

  return args;
}

QStringList getDfuArgs(const QString & cmd, const QString & filename)
{
  QStringList args;
  burnConfigDialog bcd;
  args << bcd.getDFUArgs();
  if (!filename.endsWith(".dfu"))
    args << "--dfuse-address" << "0x08000000";
  if (cmd == "-U")
    args.last().append(":" % QString::number(Boards::getFlashSize(getCurrentBoard())));
  args << "--device" << "0483:df11";
  args << "" << cmd % filename;
  return args;
}

QStringList getSambaArgs(const QString & tcl)
{
  QStringList result;

  QString tclFilename = generateProcessUniqueTempFileName("temp.tcl");
  if (QFile::exists(tclFilename)) {
    qunlink(tclFilename);
  }
  QFile tclFile(tclFilename);
  if (!tclFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(NULL, CPN_STR_TTL_ERROR,
                         QCoreApplication::translate("RadioInterface", "Cannot write file %1:\n%2.").arg(tclFilename).arg(tclFile.errorString()));
    return result;
  }

  QTextStream outputStream(&tclFile);
  outputStream << tcl;

  burnConfigDialog bcd;
  result << bcd.getSambaPort() << bcd.getArmMCU() << tclFilename ;
  return result;
}

QStringList getReadEEpromCmd(const QString & filename)
{
  QStringList result;
  EEPROMInterface *eepromInterface = getCurrentEEpromInterface();
  if (IS_STM32(eepromInterface->getBoard())) {
    // impossible
  }
  else if (IS_SKY9X(eepromInterface->getBoard())) {
    result = getSambaArgs(QString("SERIALFLASH::Init 0\n") + "receive_file {SerialFlash AT25} \"" + filename + "\" 0x0 0x80000 0\n");
  }
  else {
    result = getAvrdudeArgs("eeprom:r:", filename);
  }
  return result;
}

QStringList getWriteEEpromCmd(const QString & filename)
{
  Board::Type board = getCurrentBoard();
  if (IS_STM32(board)) {
    // impossible
    return QStringList();
  }
  else if (IS_SKY9X(board)) {
    return getSambaArgs(QString("SERIALFLASH::Init 0\n") + "send_file {SerialFlash AT25} \"" + filename + "\" 0x0 0\n");
  }
  else {
    return getAvrdudeArgs("eeprom:w:", filename);
  }
}

QStringList getWriteFirmwareArgs(const QString & filename)
{
  Board::Type board = getCurrentBoard();
  if (IS_STM32(board)) {
    return getDfuArgs("-D", filename);
  }
  else if (board == Board::BOARD_SKY9X) {
    return getSambaArgs(QString("send_file {Flash} \"") + filename + "\" 0x400000 0\n" + "FLASH::ScriptGPNMV 2\n");
  }
  else if (board == Board::BOARD_9XRPRO || board == Board::BOARD_AR9X) {
    return getSambaArgs(QString("send_file {Flash} \"") + filename + "\" 0x400000 0\n" + "FLASH::ScriptGPNMV 2\n");
  }
  else {
    return getAvrdudeArgs("flash:w:", filename);
  }
}

QStringList getReadFirmwareArgs(const QString & filename)
{
  Board::Type board = getCurrentBoard();
  if (IS_STM32(board)) {
    return getDfuArgs("-U", filename);
  }
  else if (board == Board::BOARD_SKY9X) {
    return getSambaArgs(QString("receive_file {Flash} \"") + filename + "\" 0x400000 0x40000 0\n");
  }
  else if (board == Board::BOARD_9XRPRO) {
    return getSambaArgs(QString("receive_file {Flash} \"") + filename + "\" 0x400000 0x80000 0\n");
  }
  else {
    return getAvrdudeArgs("flash:r:", filename);
  }
}

void readAvrdudeFuses(ProgressWidget * progress)
{
  burnConfigDialog bcd;
  QStringList args;
  args << "-c" << bcd.getProgrammer() << "-p" << bcd.getMCU() << bcd.getAvrdudeArgs() << "-U" << "lfuse:r:-:i" << "-U" << "hfuse:r:-:i" << "-U" << "efuse:r:-:i";
  FlashProcess flashProcess(bcd.getAVRDUDE(), args, progress);
  flashProcess.run();
}

void resetAvrdudeFuses(bool eepromProtect, ProgressWidget * progress)
{
  //fuses
  //avrdude -c usbasp -p m64 -U lfuse:w:<0x0E>:m
  //avrdude -c usbasp -p m64 -U hfuse:w:<0x89>:m  0x81 for eeprom protection
  //avrdude -c usbasp -p m64 -U efuse:w:<0xFF>:m

  burnConfigDialog bcd;
  QMessageBox::StandardButton ret = QMessageBox::No;
  ret = QMessageBox::warning(NULL, CPN_STR_APP_NAME,
                             QCoreApplication::translate("RadioInterface",
                                                         "<b><u>WARNING!</u></b>" \
                                                         "<br>This will reset the fuses of  %1 to the factory settings.<br>"
                                                         "Writing fuses can mess up your radio.<br>Do this only if you are sure they are wrong!<br>"
                                                         "Are you sure you want to continue?").arg(bcd.getMCU()),
                             QMessageBox::Yes | QMessageBox::No);
  if (ret == QMessageBox::Yes) {
    QStringList args = bcd.getAvrdudeArgs();
    QStringList str;
    if (bcd.getMCU() == "m2560") {
      args << "-B8";
      QString erStr = eepromProtect ? "hfuse:w:0x51:m" : "hfuse:w:0x59:m";
      str << "-U" << "lfuse:w:0xD7:m" << "-U" << erStr << "-U" << "efuse:w:0xFC:m";
      //use hfuse = 0x81 to prevent eeprom being erased with every flashing
    }
    else {
      QString lfuses;
      QString tempFile = generateProcessUniqueTempFileName("ftemp.bin");
      QStringList argread;
      argread << "-c" << bcd.getProgrammer() << "-p" << bcd.getMCU() << args  <<"-U" << "lfuse:r:"+tempFile+":r";
      FlashProcess flashProcess(bcd.getAVRDUDE(), argread, progress);
      flashProcess.run();
      QFile file(tempFile);
      if (file.exists() && file.size()==1) {
        file.open(QIODevice::ReadOnly);
        char bin_flash[1];
        file.read(bin_flash, 1);
        if (bin_flash[0]==0x0E) {
          lfuses = "lfuse:w:0x0E:m";
        }
        else {
          lfuses = "lfuse:w:0x3F:m";
        }
        file.close();
        qunlink(tempFile);
      }
      else {
        lfuses = "lfuse:w:0x3F:m";
      }

      QString erStr = eepromProtect ? "hfuse:w:0xC1:m" : "hfuse:w:0xC9:m";
      str << "-U" << lfuses << "-U" << erStr << "-U" << "efuse:w:0xFF:m";
      //use hfuse = 0x81 to prevent eeprom being erased with every flashing
    }

    QStringList arguments;
    if (bcd.getMCU() == "m2560") {
      arguments << "-c" << bcd.getProgrammer() << "-p" << bcd.getMCU() << args << "-u" << str;
    }
    else {
      arguments << "-c" << bcd.getProgrammer() << "-p" << bcd.getMCU() << args << "-B" << "100" << "-u" << str;
    }
    FlashProcess flashProcess(bcd.getAVRDUDE(), arguments, progress);
    flashProcess.run();
  }
}


bool readFirmware(const QString & filename, ProgressWidget * progress)
{
  bool result = false;

  QFile file(filename);
  if (file.exists() && !file.remove()) {
    QMessageBox::warning(NULL, CPN_STR_TTL_ERROR,
                         QCoreApplication::translate("RadioInterface", "Could not delete temporary file: %1").arg(filename));
    return false;
  }

  if (IS_ARM(getCurrentBoard())) {
    QString path = findMassstoragePath("FIRMWARE.BIN");
    if (!path.isEmpty()) {
      qDebug() << "readFirmware: reading" << path << "into" << filename;
      CopyProcess copyProcess(path, filename, progress);
      result = copyProcess.run();
    }
  }

  if (result == false) {
    qDebug() << "readFirmware: reading" << filename << "with" << getRadioInterfaceCmd() << getReadFirmwareArgs(filename);
    FlashProcess flashProcess(getRadioInterfaceCmd(), getReadFirmwareArgs(filename), progress);
    result = flashProcess.run();
  }

  if (!QFileInfo(filename).exists()) {
    result = false;
  }

  return result;
}

bool writeFirmware(const QString & filename, ProgressWidget * progress)
{
  if (IS_ARM(getCurrentBoard())) {
    QString path = findMassstoragePath("FIRMWARE.BIN");
    if (!path.isEmpty()) {
      qDebug() << "writeFirmware: writing" << path << "from" << filename;
      CopyProcess copyProcess(filename, path, progress);
      return copyProcess.run();
    }
  }

  qDebug() << "writeFirmware: writing" << filename << "with" << getRadioInterfaceCmd() << getWriteFirmwareArgs(filename);
  FlashProcess flashProcess(getRadioInterfaceCmd(), getWriteFirmwareArgs(filename), progress);
  return flashProcess.run();
}

bool readEeprom(const QString & filename, ProgressWidget * progress)
{
  Board::Type board = getCurrentBoard();

  QFile file(filename);
  if (file.exists() && !file.remove()) {
    QMessageBox::warning(NULL, CPN_STR_TTL_ERROR,
                         QCoreApplication::translate("RadioInterface", "Could not delete temporary file: %1").arg(filename));
    return false;
  }

  if (IS_HORUS(board)) {
    QString radioPath = findMassstoragePath("RADIO", true);
    qDebug() << "Searching for SD card, found" << radioPath;
    if (radioPath.isEmpty()) {
      QMessageBox::critical(progress, CPN_STR_TTL_ERROR,
                            QCoreApplication::translate("RadioInterface", "Unable to find Horus radio SD card!"));
      return false;
    }
    RadioData radioData;
    Storage inputStorage(radioPath);
    if (!inputStorage.load(radioData)) {
      QMessageBox::critical(progress, CPN_STR_TTL_ERROR, inputStorage.error());
      return false;
    }
    Storage outputStorage(filename);
    if (!outputStorage.write(radioData)) {
      QMessageBox::critical(progress, CPN_STR_TTL_ERROR, outputStorage.error());
      return false;
    }
  }
  else {
    if (IS_ARM(board)) {
      QString path = findMassstoragePath("EEPROM.BIN");
      if (path.isEmpty()) {
        // On previous OpenTX we called the EEPROM file "TARANIS.BIN" :(
        path = findMassstoragePath("TARANIS.BIN");
      }
      if (path.isEmpty()) {
        // Mike's bootloader calls the EEPROM file "ERSKY9X.BIN" :(
        path = findMassstoragePath("ERSKY9X.BIN");
      }
      if (path.isEmpty()) {
        RadioNotFoundDialog dialog(progress);
        dialog.exec();
        return false;
      }
      CopyProcess copyProcess(path, filename, progress);
      if (!copyProcess.run()) {
        return false;
      }
    }

    if (!IS_STM32(board)) {
      FlashProcess flashProcess(getRadioInterfaceCmd(), getReadEEpromCmd(filename), progress);
      if (!flashProcess.run()) {
        return false;
      }
    }
  }

  return QFileInfo(filename).exists();
}

bool writeEeprom(const QString & filename, ProgressWidget * progress)
{
  Board::Type board = getCurrentBoard();

  if (IS_ARM(board)) {
    QString path = findMassstoragePath("EEPROM.BIN");
    if (path.isEmpty()) {
      // On previous OpenTX we called the EEPROM file "TARANIS.BIN" :(
      path = findMassstoragePath("TARANIS.BIN");
    }
    if (path.isEmpty()) {
      // Mike's bootloader calls the EEPROM file "ERSKY9X.BIN" :(
      path = findMassstoragePath("ERSKY9X.BIN");
    }
    if (!path.isEmpty()) {
      CopyProcess copyProcess(filename, path, progress);
      return copyProcess.run();
    }
  }

  if (!IS_TARANIS(board)) {
    FlashProcess flashProcess(getRadioInterfaceCmd(), getWriteEEpromCmd(filename), progress);
    return flashProcess.run();
  }

  if (IS_ARM(board)) {
    RadioNotFoundDialog dialog(progress);
    dialog.exec();
  }

  return false;
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 4, 0))
#if defined WIN32 || !defined __GNUC__
#include <windows.h>
bool isRemovableMedia(const QString & vol)
{
  char szDosDeviceName[MAX_PATH];
  QString volume = vol;
  UINT driveType = GetDriveType(volume.replace("/", "\\").toLatin1());
  if (driveType != DRIVE_REMOVABLE)
    return false;
  QueryDosDevice(volume.replace("/", "").toLatin1(), szDosDeviceName, MAX_PATH);
  if (strstr(szDosDeviceName, "\\Floppy") != NULL) { // it's a floppy
    return false;
  }
  return true;
}
#else
  #include "mountlist.h"
#endif  // defined WIN32 || !defined __GNUC__
#endif  // (QT_VERSION < QT_VERSION_CHECK(5, 4, 0))

QString findMassstoragePath(const QString & filename, bool onlyPath)
{
  QString temppath;
  QString eepromfile;
  bool found = false;
  QRegularExpression fstypeRe("^(v?fat|msdos)", QRegularExpression::CaseInsensitiveOption);  // Linux: "vfat"; macOS: "msdos"; Win: "FAT32"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
  foreach(const QStorageInfo & si, QStorageInfo::mountedVolumes()) {
    //qDebug() << si.rootPath() << si.name() << si.device() << si.displayName() << si.fileSystemType() << si.isReady();
    if (!si.isReady() || !QString(si.fileSystemType()).contains(fstypeRe))
      continue;
    temppath = si.rootPath();
    eepromfile = temppath % "/" % filename;
    qDebug() << "Searching for" << eepromfile;
    if (QFile::exists(eepromfile)) {
      found = true;
      break;
    }
  }
#elif defined WIN32 || !defined __GNUC__
  static QStringList blacklist;
  foreach(const QFileInfo & drive, QDir::drives()) {
    temppath = drive.absolutePath();
    if (blacklist.contains(temppath))
      continue;
    if (!isRemovableMedia(temppath)) {
      blacklist.append(temppath);
      continue;
    }
    WCHAR szVolumeName[256] ;
    WCHAR szFileSystemName[256];
    DWORD dwSerialNumber = 0;
    DWORD dwMaxFileNameLength=256;
    DWORD dwFileSystemFlags=0;
    if (!GetVolumeInformationW( (WCHAR *) drive.absolutePath().utf16(),szVolumeName,256,&dwSerialNumber,&dwMaxFileNameLength,&dwFileSystemFlags,szFileSystemName,256))
      continue;
    eepromfile = temppath % "/" % filename;
    qDebug() << "Searching for" << eepromfile;
    if (QFile::exists(eepromfile)) {
      found = true;
      break;
    }
  }
#else
  const static QStringList blacklist = QStringList() << "/" << "/net" << "/proc" << "/run";
  QStringList drives;
  struct mount_entry *entry;
  struct mount_entry *firstEntry;
  firstEntry = entry = read_file_system_list(true);
  while (entry != NULL) {
    temppath = entry->me_mountdir;
    if (!drives.contains(entry->me_devname) && !blacklist.contains(temppath) && QString(entry->me_type).contains(fstypeRe)) {
      drives.append(entry->me_devname);
      eepromfile = temppath % "/" % filename;
      qDebug() << "Searching for" << eepromfile;
      if (QFile::exists(eepromfile)) {
        found = true;
        break;
      }
    }
    entry = entry->me_next;
  }
  free_file_system_list(firstEntry);
#endif

  if (found)
    return onlyPath ? temppath : eepromfile;
  else
    return QString();
}
