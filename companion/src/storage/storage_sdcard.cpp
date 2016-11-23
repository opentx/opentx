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

#include <QFileInfo>
#include <QObject>
#include <QDebug>
#include <QRegularExpression>
#include "storage_sdcard.h"

// defines for miniz
#include "miniz.c"

int readFromArchive(QByteArray & data, mz_zip_archive * zip_archive, unsigned int index)
{
  size_t uncompSize;
  void * uncompData = mz_zip_reader_extract_to_heap(zip_archive, index, &uncompSize, 0);
  if (!uncompData) {
    return -1;
  }
  qDebug() << "Extracted file with index" << index << ",size" << uncompSize;
  data.append((const char *)uncompData, uncompSize);
  mz_free(uncompData);
  return 0;
}

int StorageSdcard::read(const QString & path)
{
  return QFileInfo(path).isDir() ? readSdcard(path) : readOtx(path);
}

int StorageSdcard::write(const QString & path)
{
  return QFileInfo(path).isDir() ? writeSdcard(path) : writeOtx(path);
}

int StorageSdcard::readOtx(const QString & path)
{
  QFile file(path);
  if (!file.open(QFile::ReadOnly)) {
    lastErrorMessage = QObject::tr("Error opening file %1:\n%2.").arg(path).arg(file.errorString());
    return -1;
  }
  QByteArray archiveContents = file.readAll();
  file.close();
  qDebug() << "File" << path << "read, size:" << archiveContents.size();

  // open zip file
  mz_zip_archive zip_archive;
  memset(&zip_archive, 0, sizeof(zip_archive));
  if (!mz_zip_reader_init_mem(&zip_archive, archiveContents.data(), archiveContents.size(), 0)) {
    lastErrorMessage = QObject::tr("Error opening OTX archive %1.").arg(path);
    qDebug() << lastErrorMessage;
    return -1;
  }

  // go trough all files in an archive
  QRegularExpression regexModel("MODELS/\\w+.bin", QRegularExpression::CaseInsensitiveOption);
  for (unsigned int i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++)
  {
    mz_zip_archive_file_stat file_stat;
    if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
       lastErrorMessage = QObject::tr("mz_zip_reader_file_stat() failed!");
       qDebug() << lastErrorMessage;
       mz_zip_reader_end(&zip_archive);
       return -1;
    }
    // printf("Filename: \"%s\", Comment: \"%s\", Uncompressed size: %u, Compressed size: %u, Is Dir: %u\n", file_stat.m_filename, file_stat.m_comment, (uint)file_stat.m_uncomp_size, (uint)file_stat.m_comp_size, mz_zip_reader_is_file_a_directory(&zip_archive, i));

    if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) continue;

    QString filename(file_stat.m_filename);
    if (regexModel.match(filename).hasMatch()) {
      qDebug() << "found model:" << filename;
      models.append(ModelFile());
      ModelFile & newModel = models.last();
      newModel.filename = QFileInfo(filename).fileName();
      if (readFromArchive(newModel.data, &zip_archive, i) < 0) {
        lastErrorMessage = QObject::tr("Can't extract file %1").arg(filename);
        qDebug() << lastErrorMessage;
        mz_zip_reader_end(&zip_archive);
        return -1;
      }
    }
    else if (QString::compare(filename, "RADIO/radio.bin", Qt::CaseInsensitive) == 0) {
      if (readFromArchive(radio, &zip_archive, i) < 0) {
        lastErrorMessage = QObject::tr("Can't extract file %1").arg(filename);
        qDebug() << lastErrorMessage;
        mz_zip_reader_end(&zip_archive);
        return -1;
      }
    }
    else if (QString::compare(filename, "RADIO/models.txt", Qt::CaseInsensitive) == 0) {
      if (readFromArchive(modelList, &zip_archive, i) < 0) {
        lastErrorMessage = QObject::tr("Can't extract file %1").arg(filename);
        qDebug() << lastErrorMessage;
        mz_zip_reader_end(&zip_archive);
        return -1;
      }
    }
    else {
      qDebug() << "Unknown file " << filename;
    }

  }
  mz_zip_reader_end(&zip_archive);
  return 0;
}

void StorageSdcard::readFile(QByteArray & data, const QString & path)
{
  QFile file(path);
  if (!file.open(QFile::ReadOnly)) {
    lastErrorMessage = QObject::tr("Error opening file %1:\n%2.").arg(path).arg(file.errorString());
    throw StorageSdcardReadFileError();
  }
  data = file.readAll();
  file.close();
  qDebug() << "File" << path << "read, size:" << data.size();
}

int StorageSdcard::readSdcard(const QString & path)
{
  try {
    readFile(radio, path + "/RADIO/radio.bin");
    readFile(modelList, path + "RADIO/models.txt");

    QDir dir(path + "MODELS/");
    QStringList filters;
    filters << "*.bin";
    foreach(QString filename, dir.entryList(filters, QDir::Files)) {
      qDebug() << "found" << filename;
      models.append(ModelFile());
      ModelFile & newModel = models.last();
      newModel.filename = QFileInfo(filename).fileName();
      readFile(newModel.data, path + "MODELS/" + filename);
    }
  }
  catch (StorageSdcardReadFileError) {
    return -1;
  }
  return 0;
}

void StorageSdcard::writeFile(const QByteArray & data, const QString & path)
{
  QFile file(path);
  if (!file.open(QFile::WriteOnly)) {
    lastErrorMessage = QObject::tr("Error opening file %1:\n%2.").arg(path).arg(file.errorString());
    throw StorageSdcardWriteFileError();
  }
  file.write(data.data(), data.size());
  file.close();
  qDebug() << "File" << path << "written, size:" << data.size();
}

int StorageSdcard::writeSdcard(const QString & path)
{
  try {
    QDir dir(path);
    dir.mkdir("RADIO");
    writeFile(radio, path + "/RADIO/radio.bin");
    writeFile(modelList, path + "RADIO/models.txt");

    dir.mkdir("MODELS");
    for (QList<ModelFile>::const_iterator i = models.begin(); i != models.end(); ++i) {
      qDebug() << "writing" << i->filename;
      writeFile(i->data, path + "MODELS/" + i->filename);
    }
  }
  catch (StorageSdcardWriteFileError) {
    return -1;
  }
  return 0;
}

#define MZ_ALLOCATION_SIZE    (32*1024)

int StorageSdcard::writeOtx(const QString & path)
{
  qDebug() << "Saving to archive" << path;
  mz_zip_archive zip_archive;
  memset(&zip_archive, 0, sizeof(zip_archive));

  if (!mz_zip_writer_init_heap(&zip_archive, 0, MZ_ALLOCATION_SIZE)) {
    lastErrorMessage = QObject::tr("Error initializing OTX archive writer");
    qDebug() << lastErrorMessage;
    return -1;
  }

  // add radio.bin
  if (!mz_zip_writer_add_mem(&zip_archive, "RADIO/radio.bin", radio.data(), radio.size(), MZ_DEFAULT_LEVEL)) {
    lastErrorMessage = QObject::tr("Error adding %1 to OTX archive").arg("RADIO/radio.bin");
    qDebug() << lastErrorMessage;
    mz_zip_writer_end(&zip_archive);
    return -1;
  }

  // add models.txt
  if (!mz_zip_writer_add_mem(&zip_archive, "RADIO/models.txt", modelList.data(), modelList.size(), MZ_DEFAULT_LEVEL)) {
    lastErrorMessage = QObject::tr("Error adding %1 to OTX archive").arg("RADIO/models.txt");
    qDebug() << lastErrorMessage;
    mz_zip_writer_end(&zip_archive);
    return -1;
  }

  // add all models
  for (QList<ModelFile>::iterator i = models.begin(); i != models.end(); ++i) {
    QString filename = "MODELS/" + i->filename;
    qDebug() << "\tadding model:" << filename << "size" << i->data.size();
    if (!mz_zip_writer_add_mem(&zip_archive, filename.toLatin1(), i->data.data(), i->data.size(), MZ_DEFAULT_LEVEL)) {
      lastErrorMessage = QObject::tr("Error adding %1 to OTX archive").arg(filename);
      qDebug() << lastErrorMessage;
      mz_zip_writer_end(&zip_archive);
      return -1;
    }
  }

  // finalize archive and get contents
  char * archiveContents;
  size_t archiveSize;
  if (!mz_zip_writer_finalize_heap_archive(&zip_archive, (void **)&archiveContents, &archiveSize)) {
    lastErrorMessage = QObject::tr("Error creating OTX archive");
    qDebug() << lastErrorMessage;
    mz_zip_writer_end(&zip_archive);
    return -1;
  }
  qDebug() << "Archive size" << archiveSize;

  // write contents to file
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly)) {
    lastErrorMessage = QObject::tr("Error creating OTX file %1:\n%2.").arg(path).arg(file.errorString());
    qDebug() << lastErrorMessage;
    mz_zip_writer_end(&zip_archive);
    return -1;
  }

  qint64 result = file.write(archiveContents, archiveSize);
  if(result != (qint64)archiveSize) {
    lastErrorMessage = QObject::tr("Error writing file %1:\n%2.").arg(path).arg(file.errorString());
    mz_zip_writer_end(&zip_archive);
    return -1;
  }
  file.close();

  mz_zip_writer_end(&zip_archive);
  return 0;
}

const QByteArray & StorageSdcard::getModelData(const QString & filename) const
{
  for (QList<ModelFile>::const_iterator i = models.begin(); i != models.end(); ++i) {
    if (filename == i->filename) return i->data;
  }
  throw StorageSdcardModelNotFound();
}


QList<ModelFile>::iterator StorageSdcard::getModelIterator(const QString & filename)
{
  for (QList<ModelFile>::iterator i = models.begin(); i != models.end(); ++i) {
    if (filename == i->filename) return i;
  }
  return models.end();
}

QList<ModelFile>::const_iterator StorageSdcard::getModelIterator(const QString & filename) const
{
  for (QList<ModelFile>::const_iterator i = models.begin(); i != models.end(); ++i) {
    if (filename == i->filename) return i;
  }
  return models.end();
}

QList<QString> StorageSdcard::getModelsFileNames() const
{
  QList<QString> result;
  for (QList<ModelFile>::const_iterator i = models.begin(); i != models.end(); ++i) {
    result.append(i->filename);
  }
  return result;
}

