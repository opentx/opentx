#ifndef HELI_H
#define HELI_H

#include "modeledit.h"

namespace Ui {
  class Heli;
}

class HeliPanel : public ModelPanel
{
    Q_OBJECT

  public:
    HeliPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    ~HeliPanel();
    void update();

  private slots:
    void edited();

  private:
    Ui::Heli *ui;
};

#endif // HELI_H
