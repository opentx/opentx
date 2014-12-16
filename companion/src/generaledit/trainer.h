#ifndef TRAINER_H
#define TRAINER_H

#include "generaledit.h"
#include "eeprominterface.h"

namespace Ui {
  class Trainer;
}

class TrainerPanel : public GeneralPanel
{
    Q_OBJECT

  public:
    TrainerPanel(QWidget *parent, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~TrainerPanel();

  private slots:
    void on_trnMode_1_currentIndexChanged(int index);
    void on_trnChn_1_currentIndexChanged(int index);
    void on_trnWeight_1_editingFinished();
    void on_trnMode_2_currentIndexChanged(int index);
    void on_trnChn_2_currentIndexChanged(int index);
    void on_trnWeight_2_editingFinished();
    void on_trnMode_3_currentIndexChanged(int index);
    void on_trnChn_3_currentIndexChanged(int index);
    void on_trnWeight_3_editingFinished();
    void on_trnMode_4_currentIndexChanged(int index);
    void on_trnChn_4_currentIndexChanged(int index);
    void on_trnWeight_4_editingFinished();

  private:
    Ui::Trainer *ui;

    void setValues();
};

#endif // TRAINER_H
