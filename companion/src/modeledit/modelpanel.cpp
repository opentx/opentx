#include "modelpanel.h"
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>

ModelPanel::ModelPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware):
  GenericPanel(parent),
  model(model),
  generalSettings(generalSettings),
  firmware(firmware)
{
}

ModelPanel::~ModelPanel()
{
}

