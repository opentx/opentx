#ifndef FLASHEEPROMDIALOG_H_
#define FLASHEEPROMDIALOG_H_

#include <QtWidgets>

#define ER9X_EEPROM_FILE_TYPE          "ER9X_EEPROM_FILE"
#define EEPE_EEPROM_FILE_HEADER        "EEPE EEPROM FILE"
#define HEX_FILES_FILTER               "HEX files (*.hex);;"
#define BIN_FILES_FILTER               "BIN files (*.bin);;"
#define EXTERNAL_EEPROM_FILES_FILTER   "EEPROM files (*.bin *.hex);;" BIN_FILES_FILTER HEX_FILES_FILTER

namespace Ui
{
  class FlashEEpromDialog;
}

class RadioData;

class FlashEEpromDialog : public QDialog
{
  Q_OBJECT

public:
  explicit FlashEEpromDialog(QWidget *parent, const QString &fileName="");
  ~FlashEEpromDialog();

private slots:
  void on_eepromLoad_clicked();
  void on_burnButton_clicked();
  void on_cancelButton_clicked();
  void shrink();

protected:
  void updateUI();
  bool isValidEEprom(const QString &filename);
  int getEEpromVersion(const QString &filename);
  bool patchCalibration();
  bool patchHardwareSettings();

private:
  Ui::FlashEEpromDialog *ui;
  QString eepromFilename;
  RadioData *radioData;
};

#endif // FLASHEEPROMDIALOG_H_
