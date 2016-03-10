#ifndef CHANNELS_H
#define CHANNELS_H

#include "helpers.h"
#include "modeledit.h"

class GVarGroup;

class LimitsGroup
{
  public:
    LimitsGroup(Firmware * firmware, TableLayout *tableLayout, int row, int col, int & value, int min, int max, int deflt);
    ~LimitsGroup();

    void updateMinMax(int max);

  protected:
    Firmware * firmware;
    QDoubleSpinBox * spinbox;
    GVarGroup * gvarGroup;
    int & value;
    double displayStep;
};

class Channels : public ModelPanel
{
    Q_OBJECT

  public:
    Channels(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
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
