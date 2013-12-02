#ifndef MODELEDIT_H
#define MODELEDIT_H

#include <QDialog>
#include <QtGui>
#include <QPen>
#include "eeprominterface.h"
#include "mixerslist.h"
#ifdef PHONON
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>
#endif
namespace Ui {
    class ModelEdit;
}

class ModelEdit : public QDialog
{
    Q_OBJECT

public:
    explicit ModelEdit(RadioData &radioData, uint8_t id , bool openWizard =false, bool inNew =false, QWidget *parent = 0);
    ~ModelEdit();

    void applyBaseTemplate();
    bool redrawCurve;
    void drawCurve();
    bool drawing;
    bool mixInserted;
    bool expoInserted;
    bool openWizard;
    bool isNew;
private:
    Ui::ModelEdit *ui;

    QSpinBox* spny[17];
    QSpinBox* spnx[17];
    QSpinBox* gvarsSB[9][5];
    QSpinBox* reSB[9][2];
    QGroupBox* barsGB[3];
    QGroupBox* numsGB[3];
    QComboBox* barsCB[12];
    QDoubleSpinBox* minSB[12];
    QDoubleSpinBox* maxSB[12];
    QComboBox* csf[36];
    QComboBox* csw[C9X_NUM_CSW];
    MixersList *ExposlistWidget;

    MixersList *MixerlistWidget;

    RadioData &radioData;
    int       id_model;
    ModelData g_model;
    GeneralSettings g_eeGeneral;
    bool protocolEditLock;
    bool protocol2EditLock;
    bool trainerEditLock;
    bool switchEditLock;
    bool heliEditLock;
    bool limitEditLock;
    bool phasesLock;
    bool telemetryLock;
    bool curvesLock;
    bool phononLock;
    bool fsLock;
    bool modelImageLock;
    bool plot_curve[16];
    int selectedSwitch;
    int selectedFunction;
    QDoubleSpinBox  * cswitchValue[C9X_NUM_CSW];
    QDoubleSpinBox  * cswitchOffset[C9X_NUM_CSW];
    QComboBox * cswitchAnd[C9X_NUM_CSW];
    QDoubleSpinBox  * cswitchDuration[C9X_NUM_CSW];
    QDoubleSpinBox  * cswitchDelay[C9X_NUM_CSW];
    QComboBox * cswitchSource1[C9X_NUM_CSW];
    QComboBox * cswitchSource2[C9X_NUM_CSW];
    QLabel * fswLabel[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchSwtch[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchFunc[C9X_MAX_CUSTOM_FUNCTIONS];
    QCheckBox * fswtchParamGV[C9X_MAX_CUSTOM_FUNCTIONS];
    QDoubleSpinBox * fswtchParam[C9X_MAX_CUSTOM_FUNCTIONS];
    QPushButton * playBT[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchParamT[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchParamArmT[C9X_MAX_CUSTOM_FUNCTIONS];
    QCheckBox * fswtchEnable[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchRepeat[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchGVmode[C9X_MAX_CUSTOM_FUNCTIONS];
    
    QSpinBox  * safetySwitchValue[C9X_NUM_CHNOUT];
    QComboBox * safetySwitchSwtch[C9X_NUM_CHNOUT];
    QSlider * phasesTrimSliders[9][4];
    QSpinBox * phasesTrimValues[9][4];

    void setupExposListWidget();
    void setupMixerListWidget();
    float getBarStep(int barId);
    void telBarUpdate();
    
    void updateSettings();
    void tabModelEditSetup();
    void tabPhases();
    void tabExpos();
    void tabMixes();
    void tabHeli();
    void tabLimits();
    void tabCurves();
    void tabCustomSwitches();
    void tabSafetySwitches();
    void tabCustomFunctions();
    void tabTelemetry();
    void tabTemplates();
    void updateCurvesTab();
    void setSwitchWidgetVisibility(int i);
    void setLimitMinMax();
    void updateSelectedSwitch();
    void updateSwitchesTab();
    void updateHeliTab();
    void updateA1Fields();
    void updateA2Fields();

    void launchSimulation();
    void resizeEvent(QResizeEvent *event  = 0);


    int currentCurve;
    void setCurrentCurve(int curveId);

    QSpinBox *getNodeSB(int i);
    QSpinBox *getNodeSBX(int i);

    int getExpoIndex(unsigned int dch);
    bool gm_insertExpo(int idx);
    void gm_deleteExpo(int index);
    void gm_openExpo(int index);
    int gm_moveExpo(int idx, bool dir);
    void exposDeleteList(QList<int> list);
    QList<int> createExpoListFromSelected();
    void setSelectedByExpoList(QList<int> list);

    int getMixerIndex(unsigned int dch);
    bool gm_insertMix(int idx);
    void gm_deleteMix(int index);
    void gm_openMix(int index);
    int gm_moveMix(int idx, bool dir);
    void mixersDeleteList(QList<int> list);
    QList<int> createMixListFromSelected();
    void setSelectedByMixList(QList<int> list);

    void applyTemplate(uint8_t idx);
    void applyNumericTemplate(uint64_t tpl);
    MixData* setDest(uint8_t dch);
    void setCurve(uint8_t c, int8_t ar[]);
    void setSwitch(unsigned int idx, unsigned int func, int v1, int v2);
    void on_phaseSwitch_currentIndexChanged(unsigned int phase, int index);
    void on_phaseFadeIn_valueChanged(unsigned int phase, int value);
    void on_phaseFadeOut_valueChanged(unsigned int phase, int value);
    void on_phaseTrim_valueChanged(unsigned int phase, unsigned int stick, int value);
    void on_phaseTrimUse_currentIndexChanged(unsigned int phase, unsigned int stick, int index, QSpinBox *trim, QSlider *slider);
    void displayOnePhaseOneTrim(unsigned int phase_idx, unsigned int trim_idx, QComboBox *trimUse, QSpinBox *trimVal, QSlider *trimSlider);
    void displayOnePhase(unsigned int phase_idx, QLineEdit *name, QComboBox *sw, QDoubleSpinBox *fadeIn, QDoubleSpinBox *fadeOut, QComboBox *trim1Use, QSpinBox *trim1, QLabel *trim1Label, QSlider *trim1Slider, QComboBox *trim2Use, QSpinBox *trim2, QLabel *trim2Label, QSlider *trim2Slider, QComboBox *trim3Use, QSpinBox *trim3, QLabel *trim3Label, QSlider *trim3Slider, QComboBox *trim4Use, QSpinBox *trim4, QLabel *trim4Label, QSlider *trim4Slider, QLabel *gv1Label, QComboBox *gv1Use,QSpinBox *gv1Value, QLabel *gv2Label, QComboBox *gv2Use,QSpinBox *gv2Value, QLabel *gv3Label, QComboBox *gv3Use,QSpinBox *gv3Value, QLabel *gv4Label, QComboBox *gv4Use,QSpinBox *gv4Value, QLabel *gv5Label, QComboBox *gv5Use,QSpinBox *gv5Value, QLabel *re1Label, QComboBox *re1Use,QSpinBox *re1Value, QLabel *re2Label, QComboBox *re2Use,QSpinBox *re2Value,bool doConnect);
    
    void incSubtrim(uint8_t idx, int16_t inc);
#ifdef PHONON
    Phonon::MediaObject *clickObject;
    Phonon::AudioOutput *clickOutput;
#endif

signals:
    void modelValuesChanged();

private slots:
    void clearExpos(bool ask=true);
    void clearMixes(bool ask=true);
    void clearCurves(bool ask=true);
    void on_modelVoice_SB_editingFinished();
    void on_T2ThrTrgChkB_toggled(bool checked);
    void on_ttraceCB_currentIndexChanged(int index);
    void on_instantTrim_CB_currentIndexChanged(int index);
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
    void resetCurve();
    void editCurve();
    void playMusic();
#ifdef PHONON
    void mediaPlayer_state(Phonon::State newState, Phonon::State oldState);
#endif
    
    void plotCurve(bool checked);
//phases slots
    void phaseName_editingFinished();
    void phaseSwitch_currentIndexChanged();
    void phaseFadeIn_editingFinished();
    void phaseFadeOut_editingFinished();
    void phaseTrimUse_currentIndexChanged();
    void phaseTrim_valueChanged();
    void phaseTrimSlider_valueChanged();
    void GVName_editingFinished();
    void GVSource_currentIndexChanged();
    void phaseGVValue_editingFinished();
    void phaseGVUse_currentIndexChanged();
    void phaseREValue_editingFinished();
    void phaseREUse_currentIndexChanged();
    
    void mimeMixerDropped(int index, const QMimeData *data, Qt::DropAction action);
    void pasteMixerMimeData(const QMimeData * mimeData, int destIdx);

    void mimeExpoDropped(int index, const QMimeData *data, Qt::DropAction action);
    void pasteExpoMimeData(const QMimeData * mimeData, int destIdx);

    void on_pushButton_clicked();

    void exposDelete(bool ask=true);
    void exposCut();
    void exposCopy();
    void exposPaste();
    void exposDuplicate();
    void expoOpen(QListWidgetItem *item = NULL);
    void expoAdd();
    
    void cswDelete();
    void cswCopy();
    void cswPaste();
    void cswCut();

    void fswDelete();
    void fswCopy();
    void fswPaste();
    void fswCut();
    
    void moveExpoUp();
    void moveExpoDown();

    void mixersDelete(bool ask=true);
    void mixersCut();
    void mixersCopy();
    void mixersPaste();
    void mixersDuplicate();
    void mixerOpen();
    void mixerAdd();
    void moveMixUp();
    void moveMixDown();

    void expolistWidget_customContextMenuRequested(QPoint pos);
    void expolistWidget_doubleClicked(QModelIndex index);
    void expolistWidget_KeyPress(QKeyEvent *event);


    void mixerlistWidget_customContextMenuRequested(QPoint pos);
    void mixerlistWidget_doubleClicked(QModelIndex index);
    void mixerlistWidget_KeyPress(QKeyEvent *event);

    void csw_customContextMenuRequested(QPoint pos);
    void fsw_customContextMenuRequested(QPoint pos);
    
    
    void curvePointEdited();
    void on_ca_ctype_CB_currentIndexChanged();
    void on_ca_apply_PB_clicked();
    
    void on_cname_LE_editingFinished();
    void limitOffsetEdited();
    void limitSymEdited();
    void limitEdited();
    void limitNameEdited();
    void limitInvEdited();
    void ppmcenterEdited();
    void customSwitchesEdited();
    void safetySwitchesEdited();
    void customFunctionEdited();
    void refreshCustomFunction(int index, bool modified=false);
    void startupSwitchEdited();
    void exposEdited();
    void mixesEdited();
    void heliEdited();

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

    void on_thrExpoChkB_toggled(bool checked);
    void on_thrTrimChkB_toggled(bool checked);
    void on_a1UnitCB_currentIndexChanged(int index);
    void on_a2UnitCB_currentIndexChanged(int index);
    void on_a1RatioSB_editingFinished();
    void on_a1RatioSB_valueChanged();
    void on_a1CalibSB_editingFinished();
    void on_a11LevelCB_currentIndexChanged(int index);
    void on_a11GreaterCB_currentIndexChanged(int index);
    void on_a11ValueSB_editingFinished();
    void on_a12LevelCB_currentIndexChanged(int index);
    void on_a12GreaterCB_currentIndexChanged(int index);
    void on_a12ValueSB_editingFinished();
    void on_a2RatioSB_editingFinished();
    void on_a2RatioSB_valueChanged();
    void on_a2CalibSB_editingFinished();
    void on_a21LevelCB_currentIndexChanged(int index);
    void on_a21GreaterCB_currentIndexChanged(int index);
    void on_a21ValueSB_editingFinished();
    void on_a22LevelCB_currentIndexChanged(int index);
    void on_a22GreaterCB_currentIndexChanged(int index);
    void on_a22ValueSB_editingFinished();
    void on_frskyProtoCB_currentIndexChanged(int index);
    void on_frskyUnitsCB_currentIndexChanged(int index);
    void on_frskyBladesCB_currentIndexChanged(int index);
    void on_frskyCurrentCB_currentIndexChanged(int index);
    void on_frskyVoltCB_currentIndexChanged(int index);
    void on_AltitudeToolbar_ChkB_toggled(bool checked);
    void on_rssiAlarm1CB_currentIndexChanged(int index);
    void on_rssiAlarm2CB_currentIndexChanged(int index);
    void on_rssiAlarm1SB_editingFinished();
    void on_rssiAlarm2SB_editingFinished();
    void on_AltitudeGPS_ChkB_toggled(bool checked);
    void on_showNames_Ckb_toggled(bool checked);
    void on_varioSourceCB_currentIndexChanged(int index);
    void on_varioLimitMin_DSB_editingFinished();
    void on_varioLimitMax_DSB_editingFinished();
    void on_varioLimitCenterMin_DSB_editingFinished();
    void on_varioLimitMinOff_ChkB_toggled(bool checked);
    void on_varioLimitCenterMax_DSB_editingFinished();
    void telBarCBcurrentIndexChanged(int index);
    void ScreenTypeCBcurrentIndexChanged(int index);
    void telMaxSBeditingFinished();
    void telMinSBeditingFinished();
    void customFieldEdited();
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
    void on_protocolCB_currentIndexChanged(int index);
    void on_protocolCB_2_currentIndexChanged(int index);
    void on_protocolCB_3_currentIndexChanged(int index);
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
    void on_fsm1CB_currentIndexChanged(int index);
    void on_fsm2CB_currentIndexChanged(int index);
    
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
    void on_phases_currentChanged(int index);
    void on_tabWidget_currentChanged(int index);
    void on_templateList_doubleClicked(QModelIndex index);
    void ControlCurveSignal(bool flag);
    void shrink();
    void wizard();
    void closeEvent(QCloseEvent *event);
    void on_curvetype_CB_currentIndexChanged(int index);
};

#endif // MODELEDIT_H
