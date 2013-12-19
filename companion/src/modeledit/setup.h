#ifndef SETUP_H
#define SETUP_H

#include "modelpanel.h"

namespace Ui {
  class Setup;
}

class Setup : public ModelPanel
{
    Q_OBJECT

  public:
    Setup(QWidget *parent, ModelData & model);
    virtual ~Setup();

  private slots:
    void on_protocolCB_currentIndexChanged(int index);
    void on_protocolCB_2_currentIndexChanged(int index);
    void on_protocolCB_3_currentIndexChanged(int index);
    void on_fsm1CB_currentIndexChanged(int index);
    void on_fsm2CB_currentIndexChanged(int index);
    void on_modelVoice_SB_editingFinished();
    void on_T2ThrTrgChkB_toggled(bool checked);
    void on_ttraceCB_currentIndexChanged(int index);
    void on_instantTrim_CB_currentIndexChanged(int index);
    void on_thrExpoChkB_toggled(bool checked);
    void on_thrTrimChkB_toggled(bool checked);
    void on_extendedLimitsChkB_toggled(bool checked);
    void on_extendedTrimsChkB_toggled(bool checked);
    void on_thrwarnChkB_toggled(bool checked);
    void on_thrrevChkB_toggled(bool checked);
    void on_timer1Perm_toggled(bool checked);
    void on_timer2Perm_toggled(bool checked);
    void on_timer1Minute_toggled(bool checked);
    void on_timer2Minute_toggled(bool checked);
    void on_timer1CountDownBeep_toggled(bool checked);
    void on_timer2CountDownBeep_toggled(bool checked);
    void fssldEdited();
    void fssbEdited();
    void fssldValueChanged();
    void fssbValueChanged();
    void on_ppmDelaySB_editingFinished();
    void on_ppmDelaySB_2_editingFinished();
    void on_ppmDelaySB_3_editingFinished();
    void on_numChannelsSB_editingFinished();
    void on_numChannelsSB_2_editingFinished();
    void on_numChannelsSB_3_editingFinished();
    void on_numChannelsStart_editingFinished();
    void on_numChannelsStart_2_editingFinished();
    void on_numChannelsStart_3_editingFinished();
//    void void ModelEdit::on_numChannelsStart_3_editingFinished();
    void on_pulsePolCB_currentIndexChanged(int index);
    void on_pulsePolCB_2_currentIndexChanged(int index);
    void on_pulsePolCB_3_currentIndexChanged(int index);
    void on_ppmFrameLengthDSB_editingFinished();
    void on_ppmFrameLengthDSB_2_editingFinished();
    void on_ppmFrameLengthDSB_3_editingFinished();
    void on_DSM_Type_currentIndexChanged(int index);
    void on_DSM_Type_2_currentIndexChanged(int index);
    void on_pxxRxNum_editingFinished();
    void on_pxxRxNum_2_editingFinished();

    // TODO void on_trimSWCB_currentIndexChanged(int index);
    void on_trimIncCB_currentIndexChanged(int index);
    void on_timer1DirCB_currentIndexChanged(int index);
    void on_timer1ModeCB_currentIndexChanged(int index);
    void on_timer1ValTE_editingFinished();
    void on_timer1ModeBCB_currentIndexChanged(int index);
    void on_timer2DirCB_currentIndexChanged(int index);
    void on_timer2ModeCB_currentIndexChanged(int index);
    void on_timer2ValTE_editingFinished();
    void on_timer2ModeBCB_currentIndexChanged(int index);
    void on_modelNameLE_editingFinished();
    void on_modelImage_CB_currentIndexChanged(int index);

    void on_bcRUDChkB_toggled(bool checked);
    void on_bcELEChkB_toggled(bool checked);
    void on_bcTHRChkB_toggled(bool checked);
    void on_bcAILChkB_toggled(bool checked);
    void on_bcP1ChkB_toggled(bool checked);
    void on_bcP2ChkB_toggled(bool checked);
    void on_bcP3ChkB_toggled(bool checked);
    void on_bcP4ChkB_toggled(bool checked);
    void on_bcREaChkB_toggled(bool checked);
    void on_bcREbChkB_toggled(bool checked);

    void startupSwitchEdited();

  private:
    Ui::Setup *ui;
    void tabModelEditSetup();
};

#endif // SETUP_H
