#ifndef CUSTOMSWITCHES_H
#define CUSTOMSWITCHES_H

#include "modeledit.h"
#include "eeprominterface.h"

class LogicalSwitchesPanel : public ModelPanel
{
    Q_OBJECT

  public:
    LogicalSwitchesPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~LogicalSwitchesPanel();

    virtual void update();

  private slots:
    void edited();
    void v1Edited(int value);
    void v2Edited(int value);
    void andEdited(int value);
    void durationEdited(double duration);
    void delayEdited(double delay);
    void csw_customContextMenuRequested(QPoint pos);
    void cswDelete();
    void cswCopy();
    void cswPaste();
    void cswCut();

  private:
    QComboBox * csw[C9X_NUM_CSW];
    QDoubleSpinBox  * cswitchValue[C9X_NUM_CSW];
    QDoubleSpinBox  * cswitchOffset[C9X_NUM_CSW];
    QDoubleSpinBox  * cswitchOffset2[C9X_NUM_CSW];
    QTimeEdit  * cswitchTOffset[C9X_NUM_CSW];
    QComboBox * cswitchAnd[C9X_NUM_CSW];
    QDoubleSpinBox  * cswitchDuration[C9X_NUM_CSW];
    QDoubleSpinBox  * cswitchDelay[C9X_NUM_CSW];
    QComboBox * cswitchSource1[C9X_NUM_CSW];
    QComboBox * cswitchSource2[C9X_NUM_CSW];
    void setSwitchWidgetVisibility(int i);
    int selectedSwitch;

    void populateCSWCB(QComboBox *b, int value);
    void populateAndSwitchCB(QComboBox *b, const RawSwitch & value);
    void updateLine(int index);
    void updateTimerParam(QDoubleSpinBox *sb, int timer, double minimum=0);

};

#endif // CUSTOMSWITCHES_H
