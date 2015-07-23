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

#ifndef FIRMWAREINTERFACE_H_
#define FIRMWAREINTERFACE_H_

#include <QString>
#include <QImage>
#include <QByteArray>

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

class FirmwareInterface
{
  public:
    FirmwareInterface(const QString &filename);
    inline QString getDate() { return date; }
    inline QString getTime() { return time; }
    int getSize() { return flashSize; }
    QString getFlavour() const;
    bool isHardwareCompatible(const FirmwareInterface &previousFirmware) const;
    inline QString getVersion() { return version; }
    unsigned int getVersionId() { return versionId; }
    inline int getEEpromVersion() { return eepromVersion; }
    inline int getEEpromVariant() { return eepromVariant; }
    inline QString getEEpromId() { return eepromId; }
    QImage getSplash();
    bool setSplash(const QImage & newsplash);
    bool hasSplash();
    int getSplashWidth();
    uint getSplashHeight();
    QImage::Format getSplashFormat();
    unsigned int save(QString fileName);
    bool isValid();

  private:
    QByteArray flash;
    uint flashSize;
    QString seekString(const QString & string);
    QString seekLabel(const QString & label);
    void SeekSplash();
    bool SeekSplash(QByteArray sps, QByteArray spe, int size);
    bool SeekSplash(QByteArray splash);
    QString filename;
    QString date;
    QString time;
    QString flavour;
    QString version;
    int versionId;
    QString eepromId;
    int eepromVersion;
    int eepromVariant;
    QByteArray splash;
    uint splashOffset;
    uint splashSize;
    uint splashWidth;
    uint splashHeight;
    QImage::Format splash_format;
    bool isValidFlag;
};

#endif /* FIRMWAREINTERFACE_H_ */
