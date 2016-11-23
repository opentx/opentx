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

#ifndef _STORAGE_SDCARD_H_
#define _STORAGE_SDCARD_H_

#include <QList>
#include <QByteArray>
#include <QString>
#include <QDir>

/*
  This class implements a storage model used for radios that
  store all settings on the SD card (currently only Hours)

  Disk storage format:
   * uses extension OTX
   * its a normal ZIP archive containing the same directory 
     structure as the SD card in the radio. Only the relevant
     directories and files are included.
*/

class StorageSdcardModelNotFound {};
class StorageSdcardReadFileError {};
class StorageSdcardWriteFileError {};

// representation of modelXX.bin file
struct ModelFile {
  QString filename;   // file name (without path)
  QByteArray data;    // file contents
};


class StorageSdcard {
  public:
    /*
      Reads models and radio settings from:
       * if a file is specified in path, from that file (OTX archive format)
       * if a path is specified, from that path (where it expects to find files like on the SD card)
    */
    int read(const QString & path);

    /*
      |Writes models and radio settings to:
       * if a file is specified in path, to that file (OTX archive format)
       * if a path is specified, to that path (layout created is like on the SD card)
    */
    int write(const QString & path);

    /*
      Returns a model data for specified model
    */
    const QByteArray & getModelData(const QString & filename) const;

    /*
      Returns a model iterator for specified model
    */
    QList<ModelFile>::iterator getModelIterator(const QString & filename);
    QList<ModelFile>::const_iterator getModelIterator(const QString & filename) const;

    /*
      Returns a list of all model bin files (their filenames)
    */
    QList<QString> getModelsFileNames() const;

    QString lastErrorMessage;
    QByteArray radio;         // radio settings (radio.bin)
    QByteArray modelList;     // model names and categories (models.txt)
    QList<ModelFile> models;  // collection of model data (modelXX.bin)

  private:
    int readOtx(const QString & path);
    int readSdcard(const QString & path);
    int writeOtx(const QString & path);
    int writeSdcard(const QString & path);

    void readFile(QByteArray & data, const QString & path);
    void writeFile(const QByteArray & data, const QString & path);

};

#endif // _STORAGE_SDCARD_H_
