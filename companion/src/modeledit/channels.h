#ifndef CHANNELS_H
#define CHANNELS_H

#include "modelpanel.h"
#include <QSpinBox>

class GVarGroup;

class Channels : public ModelPanel
{
    Q_OBJECT

  public:
    Channels(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware);
    ~Channels();
    
  private:
    QVector<QDoubleSpinBox *> minSpins;
    QVector<QDoubleSpinBox *> maxSpins;
    GVarGroup * ofsGroup;
    GVarGroup * minGroup;
    GVarGroup * maxGroup;

  public slots:
    void refreshExtendedLimits();

  private slots:
    void symlimitsEdited();
    void nameEdited();
    void offsetEdited();
    void minEdited();
    void maxEdited();
    void invEdited();
    void curveEdited();
    void ppmcenterEdited();

};

#endif // CHANNELS_H
