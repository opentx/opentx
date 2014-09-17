#ifndef MODELPANEL_H
#define MODELPANEL_H

#include "eeprominterface.h"
#include "helpers.h"

class ModelPanel : public GenericPanel
{
  Q_OBJECT

  public:
    ModelPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware);
    virtual ~ModelPanel();

  protected:
    ModelData & model;
    GeneralSettings & generalSettings;
    FirmwareInterface * firmware;
};

#endif // CHANNELS_H
