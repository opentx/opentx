/*
 * Author - Romolo Manfredini <romolo.manfredini@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef FLASHINTERFACE_H
#define FLASHINTERFACE_H

#include <QDialog>
#include <QtGui>
#include <inttypes.h>
#include "file.h"

#define MAX_FSIZE (512*1024)
#define SPLASH_WIDTH (128)
#define SPLASH_HEIGHT (64)
#define SPLASHX9D_WIDTH (212)
#define SPLASHX9D_HEIGHT (64)
#define SPLASH_SIZE_MAX   (SPLASHX9D_WIDTH*SPLASHX9D_HEIGHT/2)
#define ERSPLASH_MARKER "Splash"
#define ERSPLASH_OFFSET (10)
#define ERSKY9X_SPS "SPS"
#define ERSKY9X_SPE "SPE"
#define ERSKY9X_OFFSET (7)

#define FILE_TYPE_BIN  1
#define FILE_TYPE_HEX  2
#define FILE_TYPE_EEPE 3
#define FILE_TYPE_EEPM 4
#define FILE_TYPE_XML  5

int getFileType(const QString &fullFileName);

class FlashInterface
{
  public:
    FlashInterface(QString filename);
    inline QString getDate() { return date; }
    inline QString getTime() { return time; }
    inline QString getSvn() { return svn; }
    int getSize() { return flash_size; }
    inline QString getVersion() { return version; }
    inline QString getEEprom() { return eeprom; }
    QImage getSplash();
    bool setSplash(const QImage & newsplash);
    bool hasSplash();
    int getSplashWidth();
    uint getSplashHeight();
    QImage::Format getSplashFormat();
    uint saveFlash(QString fileName);
    bool isValid();

  private:
    QByteArray flash;
    uint flash_size;
    QString seekString(const QString & string);
    QString seekLabel(const QString & label);
    void SeekSplash();
    bool SeekSplash(QByteArray sps, QByteArray spe, int size);
    bool SeekSplash(QByteArray splash);
    QString filename;
    QString date;
    QString time;
    QString svn;
    QString version;
    QString eeprom;
    QByteArray splash;
    uint splash_offset;
    uint splash_size;
    uint splash_width;
    uint splash_height;
    QImage::Format splash_format;
    bool isValidFlag;
};

#endif /* FLASHINTERFACE_H */

