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
    QComboBox * csw[CPN_MAX_CSW];
    QDoubleSpinBox  * cswitchValue[CPN_MAX_CSW];
    QDoubleSpinBox  * cswitchOffset[CPN_MAX_CSW];
    QDoubleSpinBox  * cswitchOffset2[CPN_MAX_CSW];
    QTimeEdit  * cswitchTOffset[CPN_MAX_CSW];
    QComboBox * cswitchAnd[CPN_MAX_CSW];
    QDoubleSpinBox  * cswitchDuration[CPN_MAX_CSW];
    QDoubleSpinBox  * cswitchDelay[CPN_MAX_CSW];
    QComboBox * cswitchSource1[CPN_MAX_CSW];
    QComboBox * cswitchSource2[CPN_MAX_CSW];
    void setSwitchWidgetVisibility(int i);
    int selectedSwitch;

    void populateCSWCB(QComboBox *b, int value);
    void populateAndSwitchCB(QComboBox *b, const RawSwitch & value);
    void updateLine(int index);
    void updateTimerParam(QDoubleSpinBox *sb, int timer, double minimum=0);

};

#endif // CUSTOMSWITCHES_H
