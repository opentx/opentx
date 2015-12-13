#ifndef RADIOINTERFACE_H_
#define RADIOINTERFACE_H_

#include <QString>
#include <QStringList>

class ProgressWidget;

QString getRadioInterfaceCmd();

QString findMassstoragePath(const QString &filename);

QStringList getAvrdudeArgs(const QString &cmd, const QString &filename);
QStringList getSambaArgs(const QString &tcl);
QStringList getDfuArgs(const QString &cmd, const QString &filename);

void readAvrdudeFuses(ProgressWidget *progress);
void resetAvrdudeFuses(bool eepromProtect, ProgressWidget *progress);

QStringList getReadEEpromCmd(const QString &filename);
QStringList getWriteEEpromCmd(const QString &filename);
QStringList getReadFirmwareArgs(const QString &filename);
QStringList getWriteFirmwareArgs(const QString &filename);

bool readFirmware(const QString &filename, ProgressWidget *progress);
bool writeFirmware(const QString &filename, ProgressWidget *progress);
bool readEeprom(const QString &filename, ProgressWidget *progress);
bool writeEeprom(const QString &filename, ProgressWidget *progress);

#endif /* RADIOINTERFACE_H_ */
