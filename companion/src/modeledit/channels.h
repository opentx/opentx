#ifndef CHANNELS_H
#define CHANNELS_H

#include "modelpanel.h"
#include <QSpinBox>

class GVarGroup;

class LimitsGroup
{
  public:
    LimitsGroup(FirmwareInterface * firmware, QGridLayout *gridLayout, int row, int col, int & value, int min, int max, int deflt);
    ~LimitsGroup();

    void updateMinMax(int max);

  protected:
    FirmwareInterface * firmware;
    QDoubleSpinBox * spinbox;
    GVarGroup * gvarGroup;
    int & value;
    double displayStep;
};

class Channels : public ModelPanel
{
    Q_OBJECT

  public:
    Channels(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware);
    ~Channels();
    
  private:
    QVector<LimitsGroup *> limitsGroups;

  public slots:
    void refreshExtendedLimits();

  private slots:
    void symlimitsEdited();
    void nameEdited();
    void invEdited();
    void curveEdited();
    void ppmcenterEdited();

};

#endif // CHANNELS_H
