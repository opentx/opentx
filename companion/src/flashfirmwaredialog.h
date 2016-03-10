#ifndef FLASH_FIRMWARE_DIALOG_H
#define FLASH_FIRMWARE_DIALOG_H

#include <QtWidgets>
#include "eeprominterface.h"
#include "firmwareinterface.h"
#include "xmlinterface.h"

#define HEX_FILES_FILTER              "HEX files (*.hex);;"
#define BIN_FILES_FILTER              "BIN files (*.bin);;"
#define EEPE_FILES_FILTER             "EEPE EEPROM files (*.eepe);;"
#define EEPROM_FILES_FILTER           "EEPE files (*.eepe *.bin *.hex);;" EEPE_FILES_FILTER BIN_FILES_FILTER HEX_FILES_FILTER
#define FLASH_FILES_FILTER            "FLASH files (*.bin *.hex);;" BIN_FILES_FILTER HEX_FILES_FILTER
#define EXTERNAL_EEPROM_FILES_FILTER  "EEPROM files (*.bin *.hex);;" BIN_FILES_FILTER HEX_FILES_FILTER
#define ER9X_EEPROM_FILE_TYPE         "ER9X_EEPROM_FILE"
#define EEPE_EEPROM_FILE_HEADER       "EEPE EEPROM FILE"
#define EEPE_MODEL_FILE_HEADER        "EEPE MODEL FILE"

namespace Ui
{
  class FlashFirmwareDialog;
}

class FlashFirmwareDialog : public QDialog
{
  Q_OBJECT

public:
  FlashFirmwareDialog(QWidget *parent = 0);
  ~FlashFirmwareDialog();

private slots:
  void on_firmwareLoad_clicked();
  void on_burnButton_clicked();
  void on_cancelButton_clicked();
  void on_useProfileSplash_clicked();
  void on_useFirmwareSplash_clicked();
  void on_useLibrarySplash_clicked();
  void on_useExternalSplash_clicked();
  void shrink();

protected:
  void updateUI();
  void startFlash(const QString &filename);

private:
  Ui::FlashFirmwareDialog *ui;
  QString fwName;
  RadioData radioData;
  enum ImageSource {FIRMWARE, PROFILE, LIBRARY, EXTERNAL};
  ImageSource imageSource;
  QString imageFile;
};

#endif // FLASH_FIRMWARE_DIALOG_H
