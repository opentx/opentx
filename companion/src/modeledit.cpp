#include "modeledit.h"
#include "ui_modeledit.h"
#include "helpers.h"
#include "edge.h"
#include "node.h"
#include "expodialog.h"
#include "mixerdialog.h"
#include "simulatordialog.h"
#include "xsimulatordialog.h"
#include "modelconfigdialog.h"
#include <assert.h>
#include <QtGui>
#include <QApplication>

#define BC_BIT_RUD (0x01)
#define BC_BIT_ELE (0x02)
#define BC_BIT_THR (0x04)
#define BC_BIT_AIL (0x08)
#define BC_BIT_P1  (0x10)
#define BC_BIT_P2  (0x20)
#define BC_BIT_P3  (0x40)
#define BC_BIT_P4  (0x80)
#define BC_BIT_REA  (0x80)
#define BC_BIT_REB  (0x100)

#define RUD  (1)
#define ELE  (2)
#define THR  (3)
#define AIL  (4)

#define GFX_MARGIN 16

ModelEdit::ModelEdit(RadioData &radioData, uint8_t id, bool openWizard, bool isNew, QWidget *parent) :
    QDialog(parent),
    redrawCurve(true),
    openWizard(openWizard),
    isNew(isNew),
    ui(new Ui::ModelEdit),
    radioData(radioData),   
    id_model(id),
    g_model(radioData.models[id]),
    g_eeGeneral(radioData.generalSettings)
{
    ui->setupUi(this);

    setupMixerListWidget();
    setupExposListWidget();

    QSettings settings("companion9x", "companion9x");
    ui->tabWidget->setCurrentIndex(settings.value("modelEditTab", 0).toInt());

    QRegExp rx(CHAR_FOR_NAMES_REGEX);
    ui->modelNameLE->setValidator(new QRegExpValidator(rx, this));
    ui->phase0Name->setValidator(new QRegExpValidator(rx, this));
    ui->phase1Name->setValidator(new QRegExpValidator(rx, this));
    ui->phase2Name->setValidator(new QRegExpValidator(rx, this));
    ui->phase3Name->setValidator(new QRegExpValidator(rx, this));
    ui->phase4Name->setValidator(new QRegExpValidator(rx, this));
    ui->phase5Name->setValidator(new QRegExpValidator(rx, this));
    ui->phase6Name->setValidator(new QRegExpValidator(rx, this));
    ui->phase7Name->setValidator(new QRegExpValidator(rx, this));
    ui->phase8Name->setValidator(new QRegExpValidator(rx, this));
    ui->cname_LE->setValidator(new QRegExpValidator(rx, this));
  #ifdef PHONON
    phononLock=false;
    clickObject = new Phonon::MediaObject(this);
    clickOutput = new Phonon::AudioOutput(Phonon::NoCategory, this);
    Phonon::createPath(clickObject, clickOutput);
    connect(clickObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),  this, SLOT(mediaPlayer_state(Phonon::State,Phonon::State)));
  #endif

    tabModelEditSetup();
    tabPhases();
    tabExpos();
    tabMixes();
    tabLimits();
    tabCurves();
    tabCustomSwitches();
    selectedSwitch=0;
    int telTab=10;
    int fswTab=9;

    if (GetEepromInterface()->getCapability(FSSwitch)) {
      ui->tabSafetySwitches->setDisabled(true);
      ui->tabWidget->removeTab(8);
      telTab--;
      fswTab--;
    } else {
      tabSafetySwitches();
    }

    if (GetEepromInterface()->getCapability(CustomFunctions)==0 ) {
      ui->tabCustomFunctions->setDisabled(true);
      ui->tabWidget->removeTab(fswTab);
      telTab--;
    } else {
      tabCustomFunctions();
      selectedFunction=0;
    }
    tabTemplates();
    tabHeli();
    if (GetEepromInterface()->getCapability(Telemetry) & TM_HASTELEMETRY) {
      tabTelemetry();
    } else {
      ui->tabTelemetry->setDisabled(true);
      ui->tabWidget->removeTab(telTab);
    }
    ui->tabWidget->setCurrentIndex(0);
    ui->curvePreview->setMinimumWidth(240);
    ui->curvePreview->setMinimumHeight(240);
    resizeEvent();
    QTimer::singleShot(0, this, SLOT(shrink()));
    if (openWizard) {
      QTimer::singleShot(1, this, SLOT(wizard()));
    }
}

ModelEdit::~ModelEdit()
{
    delete ui;
}


float ModelEdit::getBarStep(int barId) 
{
    switch (barId-1) {
      case TELEMETRY_SOURCE_TX_BATT:
        return  0.1;
        break;
      case TELEMETRY_SOURCE_TIMER1:
      case TELEMETRY_SOURCE_TIMER2:
        return 3;
        break;
      case TELEMETRY_SOURCE_A1:
      case TELEMETRY_SOURCE_A1_MIN:
        return (ui->a1RatioSB->value()/255);
        break;
      case TELEMETRY_SOURCE_A2:
      case TELEMETRY_SOURCE_A2_MIN:
        return (ui->a2RatioSB->value()/255);
        break;
      case TELEMETRY_SOURCE_ALT:
      case TELEMETRY_SOURCE_GPS_ALT:
      case TELEMETRY_SOURCE_ALT_MAX:
      case TELEMETRY_SOURCE_ALT_MIN:
        return 8;
        break;
      case TELEMETRY_SOURCE_RPM:
      case TELEMETRY_SOURCE_RPM_MAX:
        return 50;
        break;
      case TELEMETRY_SOURCE_CELLS_SUM:
      case TELEMETRY_SOURCE_VFAS:
        return  0.1;
        break;
      case TELEMETRY_SOURCE_CELL:
        return  0.02;
        break;      
      case TELEMETRY_SOURCE_HDG:
        return  2;
        break;
      case TELEMETRY_SOURCE_DIST:
      case TELEMETRY_SOURCE_DIST_MAX:
        return  8;
        break;
      case TELEMETRY_SOURCE_CURRENT_MAX:
      case TELEMETRY_SOURCE_CURRENT:
        return  0.5;
        break;
      case TELEMETRY_SOURCE_POWER:
        return  5;
        break;
      case TELEMETRY_SOURCE_CONSUMPTION:
        return  20;
        break;
      case TELEMETRY_SOURCE_SPEED:
      case TELEMETRY_SOURCE_SPEED_MAX:
        if (g_model.frsky.imperial==1) {
          return 1;
        } else {
          return 1.852;
        }
        break;
      default:
        return  1;
        break;
    }
}

void ModelEdit::setupExposListWidget()
{
    ExposlistWidget = new MixersList(this, true);
    QPushButton * qbUp = new QPushButton(this);
    QPushButton * qbDown = new QPushButton(this);
    QPushButton * qbClear = new QPushButton(this);

    qbUp->setText(tr("Move Up"));
    qbUp->setIcon(QIcon(":/images/moveup.png"));
    qbUp->setShortcut(QKeySequence(tr("Ctrl+Up")));
    qbDown->setText(tr("Move Down"));
    qbDown->setIcon(QIcon(":/images/movedown.png"));
    qbDown->setShortcut(QKeySequence(tr("Ctrl+Down")));
    qbClear->setText(tr("Clear Expo Settings"));
    qbClear->setIcon(QIcon(":/images/clear.png"));

    ui->exposLayout->addWidget(ExposlistWidget,1,1,1,3);
    ui->exposLayout->addWidget(qbUp,2,1);
    ui->exposLayout->addWidget(qbClear,2,2);
    ui->exposLayout->addWidget(qbDown,2,3);

    connect(ExposlistWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(expolistWidget_customContextMenuRequested(QPoint)));
    connect(ExposlistWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(expolistWidget_doubleClicked(QModelIndex)));
    connect(ExposlistWidget,SIGNAL(mimeDropped(int,const QMimeData*,Qt::DropAction)),this,SLOT(mimeExpoDropped(int,const QMimeData*,Qt::DropAction)));

    connect(qbUp,SIGNAL(pressed()),SLOT(moveExpoUp()));
    connect(qbDown,SIGNAL(pressed()),SLOT(moveExpoDown()));
    connect(qbClear,SIGNAL(pressed()),SLOT(clearExpos()));

    connect(ExposlistWidget,SIGNAL(keyWasPressed(QKeyEvent*)), this, SLOT(expolistWidget_KeyPress(QKeyEvent*)));
}

void ModelEdit::setupMixerListWidget()
{
    MixerlistWidget = new MixersList(this, false); // TODO enum
    QPushButton * qbUp = new QPushButton(this);
    QPushButton * qbDown = new QPushButton(this);
    QPushButton * qbClear = new QPushButton(this);

    qbUp->setText(tr("Move Up"));
    qbUp->setIcon(QIcon(":/images/moveup.png"));
    qbUp->setShortcut(QKeySequence(tr("Ctrl+Up")));
    qbDown->setText(tr("Move Down"));
    qbDown->setIcon(QIcon(":/images/movedown.png"));
    qbDown->setShortcut(QKeySequence(tr("Ctrl+Down")));
    qbClear->setText(tr("Clear Mixes"));
    qbClear->setIcon(QIcon(":/images/clear.png"));

    ui->mixersLayout->addWidget(MixerlistWidget,1,1,1,3);
    ui->mixersLayout->addWidget(qbUp,2,1);
    ui->mixersLayout->addWidget(qbClear,2,2);
    ui->mixersLayout->addWidget(qbDown,2,3);

    connect(MixerlistWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(mixerlistWidget_customContextMenuRequested(QPoint)));
    connect(MixerlistWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(mixerlistWidget_doubleClicked(QModelIndex)));
    connect(MixerlistWidget,SIGNAL(mimeDropped(int,const QMimeData*,Qt::DropAction)),this,SLOT(mimeMixerDropped(int,const QMimeData*,Qt::DropAction)));

    connect(qbUp,SIGNAL(pressed()),SLOT(moveMixUp()));
    connect(qbDown,SIGNAL(pressed()),SLOT(moveMixDown()));
    connect(qbClear,SIGNAL(pressed()),SLOT(clearMixes()));

    connect(MixerlistWidget,SIGNAL(keyWasPressed(QKeyEvent*)), this, SLOT(mixerlistWidget_KeyPress(QKeyEvent*)));
}

void ModelEdit::resizeEvent(QResizeEvent *event)
{

    if(ui->curvePreview->scene())
    {
        QRect qr = ui->curvePreview->contentsRect();
        ui->curvePreview->scene()->setSceneRect(GFX_MARGIN, GFX_MARGIN, qr.width()-GFX_MARGIN*2, qr.height()-GFX_MARGIN*2);
        drawCurve();
    }

    QDialog::resizeEvent(event);

}

void ModelEdit::applyBaseTemplate()
{
    clearMixes(false);
    applyTemplate(0);
    updateSettings();
    tabMixes();
}

void ModelEdit::updateSettings()
{
    radioData.models[id_model] = g_model;
    emit modelValuesChanged();
}

void ModelEdit::on_tabWidget_currentChanged(int index)
{
    QSettings settings("companion9x", "companion9x");
    settings.setValue("modelEditTab",index);//ui->tabWidget->currentIndex());
    resizeEvent();
}

void ModelEdit::fssldValueChanged()
{
    if (fsLock)
      return;
    fsLock=true;
    QSpinBox * fssb1[] = { ui->fsm1SB_1, ui->fsm1SB_2,ui->fsm1SB_3,ui->fsm1SB_4,ui->fsm1SB_5,ui->fsm1SB_6,ui->fsm1SB_7,ui->fsm1SB_8,
                                 ui->fsm1SB_9, ui->fsm1SB_10,ui->fsm1SB_11,ui->fsm1SB_12,ui->fsm1SB_13,ui->fsm1SB_14,ui->fsm1SB_15,ui->fsm1SB_16, NULL };
    QSpinBox * fssb2[] = { ui->fsm2SB_1, ui->fsm2SB_2,ui->fsm2SB_3,ui->fsm2SB_4,ui->fsm2SB_5,ui->fsm2SB_6,ui->fsm2SB_7,ui->fsm2SB_8,
                                 ui->fsm2SB_9, ui->fsm2SB_10,ui->fsm2SB_11,ui->fsm2SB_12,ui->fsm2SB_13,ui->fsm2SB_14,ui->fsm2SB_15,ui->fsm2SB_16, NULL };
    QSlider *sl = qobject_cast<QSlider*>(sender());
    int fsId=sl->objectName().mid(sl->objectName().lastIndexOf("_")+1).toInt()-1;
    int moduleid=sl->objectName().mid(3,1).toInt();
    if (moduleid==1) {
      fssb1[fsId]->setValue(sl->value());
    } else {
      fssb2[fsId]->setValue(sl->value());
    }
    fsLock=false;
}

void ModelEdit::fssbValueChanged()
{
    if (fsLock)
      return;
    fsLock=true;
    QSlider * fssld1[] = { ui->fsm1SL_1, ui->fsm1SL_2,ui->fsm1SL_3,ui->fsm1SL_4,ui->fsm1SL_5,ui->fsm1SL_6,ui->fsm1SL_7,ui->fsm1SL_8,
                                 ui->fsm1SL_9, ui->fsm1SL_10,ui->fsm1SL_11,ui->fsm1SL_12,ui->fsm1SL_13,ui->fsm1SL_14,ui->fsm1SL_15,ui->fsm1SL_16, NULL };
    QSlider * fssld2[] = { ui->fsm2SL_1, ui->fsm2SL_2,ui->fsm2SL_3,ui->fsm2SL_4,ui->fsm2SL_5,ui->fsm2SL_6,ui->fsm2SL_7,ui->fsm2SL_8,
                                 ui->fsm2SL_9, ui->fsm2SL_10,ui->fsm2SL_11,ui->fsm2SL_12,ui->fsm2SL_13,ui->fsm2SL_14,ui->fsm2SL_15,ui->fsm2SL_16, NULL };
    QSpinBox *sb = qobject_cast<QSpinBox*>(sender());
    int fsId=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
    int moduleid=sb->objectName().mid(3,1).toInt();
    if (moduleid==1) {
      fssld1[fsId]->setValue(sb->value());
    } else {
      fssld2[fsId]->setValue(sb->value());
    }
    fsLock=false;
}

void ModelEdit::fssldEdited()
{
    QSlider *sl = qobject_cast<QSlider*>(sender());
    int fsId=sl->objectName().mid(sl->objectName().lastIndexOf("_")+1).toInt()-1;
    int moduleid=sl->objectName().mid(3,1).toInt();
    if (moduleid==1) {
      g_model.moduleData[0].failsafeChannels[fsId]=sl->value();
    } else {
      g_model.moduleData[1].failsafeChannels[fsId]=sl->value();
    }
    updateSettings();
}

void ModelEdit::fssbEdited()
{
    QSpinBox *sb = qobject_cast<QSpinBox*>(sender());
    int fsId=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
    int moduleid=sb->objectName().mid(3,1).toInt();
    if (moduleid==1) {
      g_model.moduleData[0].failsafeChannels[fsId]=sb->value();
    } else {
      g_model.moduleData[1].failsafeChannels[fsId]=sb->value();
    }
    updateSettings();
}

void ModelEdit::tabModelEditSetup()
{
    //name
    QLabel * pmsl[] = {ui->swwarn_label, ui->swwarn0_label, ui->swwarn4_label, NULL};
    QCheckBox * pmchkb[] = {ui->swwarn1_ChkB,ui->swwarn2_ChkB,ui->swwarn3_ChkB,ui->swwarn5_ChkB,ui->swwarn6_ChkB, NULL};
    QSlider * tpmsld[] = {ui->chkSA, ui->chkSB, ui->chkSC, ui->chkSD, ui->chkSE, ui->chkSF, ui->chkSG, NULL};
    QSlider * fssld1[] = { ui->fsm1SL_1, ui->fsm1SL_2,ui->fsm1SL_3,ui->fsm1SL_4,ui->fsm1SL_5,ui->fsm1SL_6,ui->fsm1SL_7,ui->fsm1SL_8,
                                 ui->fsm1SL_9, ui->fsm1SL_10,ui->fsm1SL_11,ui->fsm1SL_12,ui->fsm1SL_13,ui->fsm1SL_14,ui->fsm1SL_15,ui->fsm1SL_16, NULL };
    QSlider * fssld2[] = { ui->fsm2SL_1, ui->fsm2SL_2,ui->fsm2SL_3,ui->fsm2SL_4,ui->fsm2SL_5,ui->fsm2SL_6,ui->fsm2SL_7,ui->fsm2SL_8,
                                 ui->fsm2SL_9, ui->fsm2SL_10,ui->fsm2SL_11,ui->fsm2SL_12,ui->fsm2SL_13,ui->fsm2SL_14,ui->fsm2SL_15,ui->fsm2SL_16, NULL };
    QSpinBox * fssb1[] = { ui->fsm1SB_1, ui->fsm1SB_2,ui->fsm1SB_3,ui->fsm1SB_4,ui->fsm1SB_5,ui->fsm1SB_6,ui->fsm1SB_7,ui->fsm1SB_8,
                                 ui->fsm1SB_9, ui->fsm1SB_10,ui->fsm1SB_11,ui->fsm1SB_12,ui->fsm1SB_13,ui->fsm1SB_14,ui->fsm1SB_15,ui->fsm1SB_16, NULL };
    QSpinBox * fssb2[] = { ui->fsm2SB_1, ui->fsm2SB_2,ui->fsm2SB_3,ui->fsm2SB_4,ui->fsm2SB_5,ui->fsm2SB_6,ui->fsm2SB_7,ui->fsm2SB_8,
                                 ui->fsm2SB_9, ui->fsm2SB_10,ui->fsm2SB_11,ui->fsm2SB_12,ui->fsm2SB_13,ui->fsm2SB_14,ui->fsm2SB_15,ui->fsm2SB_16, NULL };
    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      ui->modelNameLE->setMaxLength(12);
    } else {
      ui->modelNameLE->setMaxLength(10);
    }
    ui->modelNameLE->setText(g_model.name);
    fsLock=true;
    if (GetEepromInterface()->getCapability(NumModules)<2) {
      ui->rf2_GB->hide();
    }

    if (!GetEepromInterface()->getCapability(HasFailsafe)) {
      ui->FSGB_1 ->hide();
      ui->FSGB_2->hide();
      ui->ModelSetupTab->setTabEnabled(1,0);
    } else {
      if (GetEepromInterface()->getCapability(HasFailsafe)<32) {
        ui->FSGB_2->hide();      
      }

      for (int i=0; fssld1[i]; i++) {
        fssld1[i]->setValue(g_model.moduleData[0].failsafeChannels[i]);
        fssld2[i]->setValue(g_model.moduleData[1].failsafeChannels[i]);
        fssb1[i]->setValue(g_model.moduleData[0].failsafeChannels[i]);
        fssb2[i]->setValue(g_model.moduleData[1].failsafeChannels[i]);
        connect(fssld1[i],SIGNAL(valueChanged(int)),this,SLOT(fssldValueChanged()));
        connect(fssld2[i],SIGNAL(valueChanged(int)),this,SLOT(fssldValueChanged()));
        connect(fssb1[i],SIGNAL(valueChanged(int)),this,SLOT(fssbValueChanged()));
        connect(fssb2[i],SIGNAL(valueChanged(int)),this,SLOT(fssbValueChanged()));
        connect(fssld1[i],SIGNAL(sliderReleased()),this,SLOT(fssldEdited()));
        connect(fssld2[i],SIGNAL(sliderReleased()),this,SLOT(fssldEdited()));
        connect(fssb1[i],SIGNAL(editingFinished()),this,SLOT(fssbEdited()));
        connect(fssb2[i],SIGNAL(editingFinished()),this,SLOT(fssbEdited()));
      }
    }
    fsLock=false;
    //timer1 mode direction value
    populateTimerSwitchCB(ui->timer1ModeCB,g_model.timers[0].mode,GetEepromInterface()->getCapability(TimerTriggerB));
    int min = g_model.timers[0].val/60;
    int sec = g_model.timers[0].val%60;
    ui->timer1ValTE->setTime(QTime(0,min,sec));
    ui->timer1DirCB->setCurrentIndex(g_model.timers[0].dir);
    if (!GetEepromInterface()->getCapability(ModelVoice)) {
      ui->modelVoice_SB->hide();
      ui->modelVoice_label->hide();
    } else {
      ui->modelVoice_SB->setValue(g_model.modelVoice+260);
    }
    if (!GetEepromInterface()->getCapability(PerModelThrottleInvert)) {
      ui->label_thrrev->hide();
      ui->thrrevChkB->hide();
    } else {
      ui->thrrevChkB->setChecked(g_model.throttleReversed);
    }
    if (!GetEepromInterface()->getCapability(ModelImage)) {
      ui->modelImage_CB->hide();
      ui->modelImage_label->hide();
      ui->modelImage_image->hide();
    } else {
      modelImageLock=true;
      QStringList items;
      items.append("");
      QSettings settings("companion9x", "companion9x");
      QString path=settings.value("sdPath", ".").toString();
      path.append("/BMP/");
      QDir qd(path);
      int vml= GetEepromInterface()->getCapability(VoicesMaxLength)+4;
      if (qd.exists()) {
        QStringList filters;
        filters << "*.bmp" << "*.bmp";
        foreach ( QString file, qd.entryList(filters, QDir::Files) ) {
          QFileInfo fi(file);
          QString temp=fi.completeBaseName();
          if (!items.contains(temp) && temp.length()<=vml) {
            items.append(temp);
          }
        }
      }
      if (!items.contains(g_model.bitmap)) {
        items.append(g_model.bitmap);      
      }
      items.sort();
      ui->modelImage_CB->clear();
      foreach ( QString file, items ) {
        ui->modelImage_CB->addItem(file);
        if (file==g_model.bitmap) {
          ui->modelImage_CB->setCurrentIndex(ui->modelImage_CB->count()-1);
          QString fileName=path;
          fileName.append(g_model.bitmap);
          fileName.append(".bmp");
          QImage image(fileName);
          if (image.isNull()) {
            fileName=path;
            fileName.append(g_model.bitmap);
            fileName.append(".BMP");
            image.load(fileName);
          }
          if (!image.isNull()) {
            ui->modelImage_image->setPixmap(QPixmap::fromImage(image.scaled( 64,32)));;
          }
        }
      }
      modelImageLock=false;
    }

    if (!GetEepromInterface()->getCapability(pmSwitchMask)) {
      for (int i=0; pmsl[i]; i++) {
        pmsl[i]->hide();
      }
      for (int i=0; pmchkb[i]; i++) {
        pmchkb[i]->hide();
      }
      ui->tswwarn0_CB->hide(); 
      for (int i=0; tpmsld[i]; i++) {
        tpmsld[i]->hide();
      }
      ui->swwarn0_line->hide();
      ui->swwarn0_line->hide();
      ui->swwarn0_CB->hide();
      ui->swwarn4_CB->hide();
      ui->swwarn_line0->hide();
      ui->swwarn_line1->hide();
      ui->swwarn_line2->hide();
      ui->swwarn_line3->hide();
      ui->swwarn_line4->hide();
      ui->swwarn_line5->hide();
      ui->tswwarn0_label->hide();
      ui->tswwarn1_label->hide();
      ui->tswwarn2_label->hide();
      ui->tswwarn3_label->hide();
      ui->tswwarn4_label->hide();
      ui->tswwarn5_label->hide();
      ui->tswwarn6_label->hide();
      ui->tswwarn7_label->hide();
    } else {
      if (GetEepromInterface()->getCapability(Pots)==3) {
        ui->swwarn0_CB->setCurrentIndex(g_model.switchWarningStates & 0x01);
        ui->swwarn1_ChkB->setChecked(checkbit(g_model.switchWarningStates, 1));
        ui->swwarn2_ChkB->setChecked(checkbit(g_model.switchWarningStates, 2));
        ui->swwarn3_ChkB->setChecked(checkbit(g_model.switchWarningStates, 3));
        ui->swwarn4_CB->setCurrentIndex((g_model.switchWarningStates & 0x30)>>4);
        ui->swwarn5_ChkB->setChecked(checkbit(g_model.switchWarningStates, 6));
        ui->swwarn6_ChkB->setChecked(checkbit(g_model.switchWarningStates, 7));
        for (int i=0; pmchkb[i]; i++) {
          connect(pmchkb[i], SIGNAL(stateChanged(int)),this,SLOT(startupSwitchEdited()));
        }
        connect(ui->swwarn0_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(startupSwitchEdited()));
        connect(ui->swwarn4_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(startupSwitchEdited()));
      } else {
        ui->tswwarn0_CB->setCurrentIndex(g_model.switchWarningStates & 0x01);
        uint16_t switchstate=(g_model.switchWarningStates>>1);
        ui->chkSA->setValue(switchstate & 0x3);
        switchstate >>= 2;
        ui->chkSB->setValue(switchstate & 0x3);
        switchstate >>= 2;
        ui->chkSC->setValue(switchstate & 0x3);
        switchstate >>= 2;
        ui->chkSD->setValue(switchstate & 0x3);
        switchstate >>= 2;
        ui->chkSE->setValue(switchstate & 0x3);
        switchstate >>= 2;
        ui->chkSF->setValue((switchstate & 0x3)/2);
        switchstate >>= 2;
        ui->chkSG->setValue(switchstate & 0x3);
        connect(ui->tswwarn0_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(startupSwitchEdited()));
        for (int i=0; tpmsld[i]; i++) {
          connect(tpmsld[i], SIGNAL(valueChanged(int)),this,SLOT(startupSwitchEdited()));
        }      
      }
    }
    int ppmmax=GetEepromInterface()->getCapability(PPMFrameLength);
    if (ppmmax>0) {
      ui->ppmFrameLengthDSB->setMaximum(ppmmax);
    }
    if (!GetEepromInterface()->getCapability(InstantTrimSW)) {
      ui->instantTrim_label->hide();
      ui->instantTrim_CB->setDisabled(true);
      ui->instantTrim_CB->hide();
    } else {
      switchEditLock=true;
      int found=false;
      for (int i=0; i< C9X_MAX_CUSTOM_FUNCTIONS; i++) {
        if (g_model.funcSw[i].func==FuncInstantTrim) {
          populateSwitchCB(ui->instantTrim_CB,g_model.funcSw[i].swtch,POPULATE_MSWITCHES & POPULATE_ONOFF);
          found=true;
          break;
        }
      }
      if (found==false) {
        populateSwitchCB(ui->instantTrim_CB,RawSwitch(),POPULATE_MSWITCHES & POPULATE_ONOFF);
      }
      switchEditLock=false;
    }
    if (GetEepromInterface()->getCapability(NoTimerDirs)) {
      ui->timer1DirCB->hide();
      ui->timer2DirCB->hide();
    }
    if (GetEepromInterface()->getCapability(NoThrExpo)) {
      ui->label_thrExpo->hide();
      ui->thrExpoChkB->hide();
    }
    if (!(GetEepromInterface()->getCapability(ExtendedTrims)>0)) {
      ui->extendedTrimsChkB->hide();
      ui->extendedTrims_label->hide();
    }
    if (!GetEepromInterface()->getCapability(HasTTrace)) {
      ui->label_ttrace->hide();
      ui->ttraceCB->hide();
    } else {
      populateTTraceCB(ui->ttraceCB,g_model.thrTraceSrc);
    }
    if (GetEepromInterface()->getCapability(RotaryEncoders)==0) {
      ui->bcREaChkB->hide();
      ui->bcREbChkB->hide();
    }
    if (GetEepromInterface()->getCapability(Pots)==3) {
      ui->tswwarn0_CB->hide();
      ui->tswwarn0_label->hide();
      ui->tswwarn1_label->hide();
      ui->tswwarn2_label->hide();
      ui->tswwarn3_label->hide();
      ui->tswwarn4_label->hide();
      ui->tswwarn5_label->hide();
      ui->tswwarn6_label->hide();
      ui->tswwarn7_label->hide();
      ui->chkSA->hide();
      ui->chkSB->hide();
      ui->chkSC->hide();
      ui->chkSD->hide();
      ui->chkSE->hide();
      ui->chkSF->hide();
      ui->chkSG->hide();
      this->layout()->removeItem(ui->TaranisSwitchStartup);
      ui->bcP4ChkB->hide();
    } else {
      ui->swwarn0_CB->hide();
      ui->swwarn0_label->hide();
      ui->swwarn0_line->hide();
      ui->swwarn1_ChkB->hide();
      ui->swwarn1_line->hide();
      ui->swwarn2_ChkB->hide();
      ui->swwarn3_ChkB->hide();
      ui->swwarn4_CB->hide();
      ui->swwarn4_label->hide();
      ui->swwarn5_ChkB->hide();
      ui->swwarn6_ChkB->hide();
      ui->swwarn_line0->hide();
      ui->swwarn_line1->hide();
      ui->swwarn_line2->hide();
      ui->swwarn_line3->hide();
      ui->swwarn_line4->hide();
      ui->swwarn_line5->hide();
      this->layout()->removeItem(ui->StockSwitchStartup);
      ui->bcP1ChkB->setText(tr("S1"));
      ui->bcP2ChkB->setText(tr("S2"));
      ui->bcP3ChkB->setText(tr("LS"));
    }

    if (!GetEepromInterface()->getCapability(PerModelThrottleWarning)) {
      ui->thrwarnChkB->setDisabled(true);
      ui->thrwarnChkB->hide();
      ui->thrwarnLabel->hide();
    } else {
      switchEditLock=true;
      ui->thrwarnChkB->setChecked(g_model.disableThrottleWarning);
      switchEditLock=false;
    }
    if (!GetEepromInterface()->getCapability(TimerTriggerB)) {
      ui->timer1ModeBCB->hide();
      ui->timer1ModeB_label->hide();
      ui->timer2ModeBCB->hide();
      ui->timer2ModeB_label->hide();
    } else {
      populateTimerSwitchBCB(ui->timer1ModeBCB,g_model.timers[0].modeB,GetEepromInterface()->getCapability(TimerTriggerB));
      populateTimerSwitchBCB(ui->timer2ModeBCB,g_model.timers[1].modeB,GetEepromInterface()->getCapability(TimerTriggerB));
    }

    int index=0;
    int selindex;
    int selindex2;
    protocolEditLock=true; 
    protocol2EditLock=true; 
    ui->protocolCB->clear();
    for (int i=0; i<PROTO_LAST; i++) {
      if (GetEepromInterface()->isAvailable((Protocol)i)) {
        ui->protocolCB->addItem(getProtocolStr(i), (QVariant)i);
        if (g_model.moduleData[0].protocol == i) {
          selindex = index;
        }
        index++;
      }
    }
    if (GetEepromInterface()->getCapability(NumModules)>1) {
      index=0;
      ui->protocolCB_2->clear();
      for (int i=0; i<PROTO_LAST; ++i) {
        if (GetEepromInterface()->isAvailable((Protocol)i, 1)) {
          ui->protocolCB_2->addItem(getProtocolStr(i), (QVariant)i);
          if (g_model.moduleData[1].protocol == i) {
            selindex2 = index;
          }
          index++;
        }
      }
    }
    if (GetEepromInterface()->getCapability(ModelTrainerEnable)) {
      trainerEditLock=true;
      if (!(g_model.trainerMode||g_model.traineron)) {
        ui->protocolCB_3->setCurrentIndex(0);
        ui->label_PPM_3->hide();
        ui->ppmDelaySB_3->hide();
        ui->label_PPMCH_3->hide();
        ui->label_pulsePol_3->hide();
        ui->pulsePolCB_3->hide();
        ui->numChannelsSB_3->hide();
        ui->label_ppmFrameLength_3->hide();
        ui->ppmFrameLengthDSB_3->hide();
        ui->label_numChannelsStart_3->hide();
        ui->numChannelsStart_3->hide();      
      } else {
        ui->protocolCB_3->setCurrentIndex(1);
        ui->label_PPM_3->show();
        ui->ppmDelaySB_3->show();
        ui->label_PPMCH_3->show();
        ui->label_pulsePol_3->show();
        ui->pulsePolCB_3->show();
        ui->numChannelsSB_3->show();
        ui->label_ppmFrameLength_3->show();
        ui->ppmFrameLengthDSB_3->show();
        ui->label_numChannelsStart_3->show();
        ui->numChannelsStart_3->show();      
      }
      trainerEditLock=false;
      on_protocolCB_3_currentIndexChanged(g_model.traineron||g_model.trainerMode);
    } else {
      ui->rf3_GB->hide();
    }

    ui->label_PPM->hide();
    ui->ppmDelaySB->hide();
    ui->label_PPMCH->hide();
    ui->label_pulsePol->hide();
    ui->pulsePolCB->hide();
    ui->numChannelsSB->hide();
    ui->label_ppmFrameLength->hide();
    ui->ppmFrameLengthDSB->hide();
    ui->label_DSM->hide();
    ui->DSM_Type->hide();
    ui->label_PXX->hide();
    ui->pxxRxNum->hide();
    ui->label_numChannelsStart->hide();
    ui->numChannelsStart->hide();
    ui->pxxRxNum->setEnabled(false);
    ui->protocolCB->setCurrentIndex(selindex);
    protocolEditLock=false;    
    on_protocolCB_currentIndexChanged(selindex);
    if (GetEepromInterface()->getCapability(NumModules)>1) {
      ui->label_PPM_2->hide();
      ui->ppmDelaySB_2->hide();
      ui->label_PPMCH_2->hide();
      ui->label_pulsePol_2->hide();
      ui->pulsePolCB_2->hide();
      ui->numChannelsSB_2->hide();
      ui->label_ppmFrameLength_2->hide();
      ui->ppmFrameLengthDSB_2->hide();
      ui->label_DSM_2->hide();
      ui->DSM_Type_2->hide();
      ui->label_PXX_2->hide();
      ui->pxxRxNum_2->hide();
      ui->label_numChannelsStart_2->hide();
      ui->numChannelsStart_2->hide();
      ui->pxxRxNum_2->setEnabled(false);
      ui->protocolCB_2->setCurrentIndex(selindex2);
      protocol2EditLock=false;
      on_protocolCB_2_currentIndexChanged(selindex2);
    }

    //timer2 mode direction value
    if (GetEepromInterface()->getCapability(Timers)<2) {
      ui->timer2DirCB->hide();
      ui->timer2ValTE->hide();
      ui->timer2DirCB->hide();
      ui->timer2ModeCB->hide();
      ui->timer2ModeBCB->hide();
      ui->timer2ModeB_label->hide();
      ui->label_timer2->hide();
    } else {
      populateTimerSwitchCB(ui->timer2ModeCB,g_model.timers[1].mode,GetEepromInterface()->getCapability(TimerTriggerB));
      min = g_model.timers[1].val/60;
      sec = g_model.timers[1].val%60;
      ui->timer2ValTE->setTime(QTime(0,min,sec));
      ui->timer2DirCB->setCurrentIndex(g_model.timers[1].dir);
    }
    if (!GetEepromInterface()->getCapability(PermTimers)) {
      ui->timer1Perm->hide();
      ui->timer2Perm->hide();
      ui->timer1PermValue->hide();
      ui->timer2PermValue->hide();
    } else {
      int sign=1;
      int pvalue=g_model.timers[0].pvalue;
      if (pvalue<0) {
        pvalue=-pvalue;
        sign=-1;
      }
      int hours=pvalue/3600;
      pvalue-=hours*3600;
      int minutes = pvalue/60;
      int seconds = pvalue%60;
      ui->timer1PermValue->setText(QString(" %1(%2:%3:%4)").arg(sign<0 ? "-" :" ").arg(hours,2,10,QLatin1Char('0')).arg(minutes,2,10,QLatin1Char('0')).arg(seconds,2,10,QLatin1Char('0')));
      // QString QString::arg ( int a, int fieldWidth = 0, int base = 10, const QChar & fillChar = QLatin1Char( ' ' ) ) const
      sign=1;
      pvalue=g_model.timers[1].pvalue;
      if (pvalue<0) {
        pvalue=-pvalue;
        sign=-1;
      }    
      hours=pvalue/3600;
      pvalue-=hours*3600;
      minutes = pvalue/60;
      seconds = pvalue%60;
      ui->timer2PermValue->setText(QString(" %1(%2:%3:%4)").arg(sign<0 ? "-" :" ").arg(hours,2,10,QLatin1Char('0')).arg(minutes,2,10,QLatin1Char('0')).arg(seconds,2,10,QLatin1Char('0')));
      ui->timer1Perm->setChecked(g_model.timers[0].persistent);
      ui->timer2Perm->setChecked(g_model.timers[1].persistent);
    }
    if (!GetEepromInterface()->getCapability(minuteBeep)) {
      ui->timer1Minute->hide();
      ui->timer2Minute->hide();
    } else {
      ui->timer1Minute->setChecked(g_model.timers[0].minuteBeep);
      ui->timer2Minute->setChecked(g_model.timers[1].minuteBeep);
    }

    if (!GetEepromInterface()->getCapability(countdownBeep)) {
      ui->timer1CountDownBeep->hide();
      ui->timer2CountDownBeep->hide();
    } else {
      ui->timer1CountDownBeep->setChecked(g_model.timers[0].countdownBeep);
      ui->timer2CountDownBeep->setChecked(g_model.timers[1].countdownBeep);
    }

    //trim inc, thro trim, thro expo, instatrim
    ui->trimIncCB->setCurrentIndex(g_model.trimInc);
    ui->thrExpoChkB->setChecked(g_model.thrExpo);
    ui->thrTrimChkB->setChecked(g_model.thrTrim);

    //center beep
    ui->bcRUDChkB->setChecked(g_model.beepANACenter & BC_BIT_RUD);
    ui->bcELEChkB->setChecked(g_model.beepANACenter & BC_BIT_ELE);
    ui->bcTHRChkB->setChecked(g_model.beepANACenter & BC_BIT_THR);
    ui->bcAILChkB->setChecked(g_model.beepANACenter & BC_BIT_AIL);
    ui->bcP1ChkB->setChecked(g_model.beepANACenter & BC_BIT_P1);
    ui->bcP2ChkB->setChecked(g_model.beepANACenter & BC_BIT_P2);
    ui->bcP3ChkB->setChecked(g_model.beepANACenter & BC_BIT_P3);
    ui->bcP4ChkB->setChecked(g_model.beepANACenter & BC_BIT_P4);
    ui->bcREaChkB->setChecked(g_model.beepANACenter & BC_BIT_REA);
    ui->bcREbChkB->setChecked(g_model.beepANACenter & BC_BIT_REB);

    // PPM settings fields
    ui->ppmDelaySB->setEnabled(g_model.moduleData[0].protocol == PPM);
    ui->pulsePolCB->setCurrentIndex(g_model.moduleData[0].ppmPulsePol);
    ui->ppmDelaySB->setEnabled(g_model.moduleData[0].protocol == PPM);
    ui->ppmDelaySB->setValue(g_model.moduleData[0].ppmDelay);
    // TODO? ui->numChannelsSB->setEnabled(g_model.moduleData[0].protocol == PPM);

    ui->extendedLimitsChkB->setChecked(g_model.extendedLimits);
    ui->T2ThrTrgChkB->setChecked(g_model.t2throttle);
    if (!GetEepromInterface()->getCapability(Timer2ThrTrig)) {
      ui->T2ThrTrg->hide();
      ui->T2ThrTrgChkB->hide();
    }
    ui->ppmFrameLengthDSB->setValue(22.5+((double)g_model.moduleData[0].ppmFrameLength)*0.5);
    if (!GetEepromInterface()->getCapability(PPMExtCtrl)) {
      ui->ppmFrameLengthDSB->hide();
      ui->label_ppmFrameLength->hide();
    }
    switch (g_model.moduleData[0].protocol) {
      case PXX_DJT:
      case PXX_XJT_X16:
      case PXX_XJT_D8:
      case PXX_XJT_LR12:
        ui->pxxRxNum->setMinimum(0);
        ui->pxxRxNum->setValue((g_model.modelId));
        break;
      case DSM2:
        if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
          ui->pxxRxNum->setValue(1);
        }
        else {
          ui->pxxRxNum->setMinimum(0);
          ui->pxxRxNum->setValue((g_model.modelId));
        }
        ui->numChannelsSB->setValue(8);
        break;
      default:
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        ui->DSM_Type->setEnabled(false);
        ui->label_PXX->hide();
        ui->pxxRxNum->hide();
        ui->pxxRxNum->setEnabled(false);
        ui->numChannelsSB->setValue(g_model.moduleData[0].channelsCount);
        break;
    }
}

void ModelEdit::on_modelVoice_SB_editingFinished()
{
    g_model.modelVoice=ui->modelVoice_SB->value()-260;
    updateSettings();
}

void ModelEdit::on_timer1Perm_toggled(bool checked)
{
    g_model.timers[0].persistent=checked;
    updateSettings();
}

void ModelEdit::on_timer2Perm_toggled(bool checked)
{
    g_model.timers[1].persistent=checked;
    updateSettings();
}

void ModelEdit::on_timer1Minute_toggled(bool checked)
{
    g_model.timers[0].minuteBeep=checked;
    updateSettings();
}

void ModelEdit::on_timer2Minute_toggled(bool checked)
{
    g_model.timers[1].minuteBeep=checked;
    updateSettings();
}

void ModelEdit::on_timer1CountDownBeep_toggled(bool checked)
{
    g_model.timers[0].countdownBeep=checked;
    updateSettings();
}

void ModelEdit::on_timer2CountDownBeep_toggled(bool checked)
{
    g_model.timers[1].countdownBeep=checked;
    updateSettings();
}

void ModelEdit::displayOnePhaseOneTrim(unsigned int phase_idx, unsigned int chn, QComboBox *trimUse, QSpinBox *trimVal, QSlider *trimSlider)
{
    PhaseData *phase = &g_model.phaseData[phase_idx];
    int trim = phase->trim[chn];
    if (trimUse) populateTrimUseCB(trimUse, phase_idx);
    if (phase->trimRef[chn] >= 0) {
      if (trimUse) trimUse->setCurrentIndex(1 + phase->trimRef[chn] - (phase->trimRef[chn] >= (int)phase_idx ? 1 : 0));
      trim = g_model.phaseData[g_model.getTrimFlightPhase(chn, phase_idx)].trim[chn];
      trimVal->setEnabled(false);
      trimSlider->setEnabled(false);
    } else {
      trimVal->setEnabled(true);
      trimSlider->setEnabled(true);
      if (trimUse) trimUse->setCurrentIndex(0);
    }
    int trimsMax = GetEepromInterface()->getCapability(ExtendedTrims);
    if (trimsMax == 0 || !g_model.extendedTrims) {
      trimsMax = 125;
    }
    /*else if (trimsMax == 500)
      trimSlider->setTickInterval(50);*/
    trimSlider->setRange(-trimsMax, +trimsMax);
    trimVal->setRange(-trimsMax, +trimsMax);
    trimVal->setValue(trim);
    trimSlider->setValue(trim);
    if (chn == 2 && g_eeGeneral.throttleReversed)
      trimSlider->setInvertedAppearance(true);
}

void ModelEdit::displayOnePhase(unsigned int phase_idx, QLineEdit *name, QComboBox *sw, QDoubleSpinBox *fadeIn, QDoubleSpinBox *fadeOut, 
                                                      QComboBox *trim1Use, QSpinBox *trim1, QLabel *trim1Label, QSlider *trim1Slider, 
                                                      QComboBox *trim2Use, QSpinBox *trim2, QLabel *trim2Label, QSlider *trim2Slider, 
                                                      QComboBox *trim3Use, QSpinBox *trim3, QLabel *trim3Label, QSlider *trim3Slider, 
                                                      QComboBox *trim4Use, QSpinBox *trim4, QLabel *trim4Label, QSlider *trim4Slider, 
                                                      QLabel *gv1Label, QComboBox *gv1Use,QSpinBox *gv1Value,
                                                      QLabel *gv2Label, QComboBox *gv2Use,QSpinBox *gv2Value,
                                                      QLabel *gv3Label, QComboBox *gv3Use,QSpinBox *gv3Value,
                                                      QLabel *gv4Label, QComboBox *gv4Use,QSpinBox *gv4Value,
                                                      QLabel *gv5Label, QComboBox *gv5Use,QSpinBox *gv5Value,
                                                      QLabel *re1Label, QComboBox *re1Use,QSpinBox *re1Value,
                                                      QLabel *re2Label, QComboBox *re2Use,QSpinBox *re2Value,bool doConnect=false)
{
    phasesLock=true;
    PhaseData *phase = &g_model.phaseData[phase_idx];
    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      if (name) name->setMaxLength(10);
    } else {
      if (name) name->setMaxLength(6);
    }

    if (name) name->setText(phase->name);
    if (sw) populateSwitchCB(sw, phase->swtch);

    int scale=GetEepromInterface()->getCapability(SlowScale);
    float range=GetEepromInterface()->getCapability(SlowRange);
    int fades=GetEepromInterface()->getCapability(FlightPhasesHaveFades);
    if (fades && fadeIn) {
      fadeIn->setEnabled(true);
      fadeOut->setEnabled(true);
      fadeIn->setMaximum(range/scale);
      fadeIn->setSingleStep(1.0/scale);
      fadeIn->setDecimals((scale==1 ? 0 :1) );
      if (fadeIn) {
        fadeIn->setValue((double)phase->fadeIn/scale);
      }
      fadeOut->setMaximum(range/scale);
      fadeOut->setSingleStep(1.0/scale);
      fadeOut->setDecimals((scale==1 ? 0 :1));
      if (fadeOut) {
        fadeOut->setValue((double)phase->fadeOut/scale);
      }
    } else if ( fadeIn ) {
      fadeIn->setDisabled(true);
      fadeOut->setDisabled(true);
    }
    displayOnePhaseOneTrim(phase_idx, CONVERT_MODE(1)-1, trim1Use, trim1, trim1Slider);
    displayOnePhaseOneTrim(phase_idx, CONVERT_MODE(2)-1, trim2Use, trim2, trim2Slider);
    displayOnePhaseOneTrim(phase_idx, CONVERT_MODE(3)-1, trim3Use, trim3, trim3Slider);
    displayOnePhaseOneTrim(phase_idx, CONVERT_MODE(4)-1, trim4Use, trim4, trim4Slider);

    if (gv1Use) { 
      populateGvarUseCB(gv1Use, phase_idx);
      if (phase->gvars[0]>1024) {
        gv1Use->setCurrentIndex(phase->gvars[0]-1024);
      }
    }
    gv1Value->setMinimum(-1024);
    gv1Value->setMaximum(1024);
    if (phase->gvars[0]<1024) {
      gv1Value->setValue(phase->gvars[0]);
      gv1Value->setEnabled(true);
    } else {
      uint index=phase->gvars[0]-1025;
      if (index>=phase_idx) {
        index++;
      }
      PhaseData *phasegvar = &g_model.phaseData[index];
      gv1Value->setValue(phasegvar->gvars[0]);
      gv1Value->setDisabled(true);
    }
    if (gv2Use) {
      populateGvarUseCB(gv2Use, phase_idx);
      if (phase->gvars[1]>1024) {
        gv2Use->setCurrentIndex(phase->gvars[1]-1024);
      }    
    }
    gv2Value->setMinimum(-1024);
    gv2Value->setMaximum(1024);
    if (phase->gvars[1]<1024) {
      gv2Value->setValue(phase->gvars[1]);
      gv2Value->setEnabled(true);
    } else {
      uint index=phase->gvars[1]-1025;
      if (index>=phase_idx) {
        index++;
      }
      PhaseData *phasegvar = &g_model.phaseData[index];
      gv2Value->setValue(phasegvar->gvars[1]);
      gv2Value->setDisabled(true);
    }
    if (gv3Use) {
      populateGvarUseCB(gv3Use, phase_idx);
      if (phase->gvars[2]>1024) {
        gv3Use->setCurrentIndex(phase->gvars[2]-1024);
      }    
    }
    gv3Value->setMinimum(-1024);
    gv3Value->setMaximum(1024);
    if (phase->gvars[2]<1024) {
      gv3Value->setValue(phase->gvars[2]);
      gv3Value->setEnabled(true);
    } else {
      uint index=phase->gvars[2]-1025;
      if (index>=phase_idx) {
        index++;
      }
      PhaseData *phasegvar = &g_model.phaseData[index];
      gv3Value->setValue(phasegvar->gvars[2]);
      gv3Value->setDisabled(true);
    }  
    if (gv4Use) {
      populateGvarUseCB(gv4Use, phase_idx);
      if (phase->gvars[3]>1024) {
        gv4Use->setCurrentIndex(phase->gvars[3]-1024);
      }    
    }
    gv4Value->setMinimum(-1024);
    gv4Value->setMaximum(1024);
    if (phase->gvars[3]<1024) {
      gv4Value->setValue(phase->gvars[3]);
      gv4Value->setEnabled(true);
    } else {
      uint index=phase->gvars[3]-1025;
      if (index>=phase_idx) {
        index++;
      }
      PhaseData *phasegvar = &g_model.phaseData[index];
      gv4Value->setValue(phasegvar->gvars[3]);
      gv4Value->setDisabled(true);
    }    
    if (gv5Use) {
      populateGvarUseCB(gv5Use, phase_idx);
      if (phase->gvars[4]>1024) {
        gv5Use->setCurrentIndex(phase->gvars[4]-1024);
      }    
    }
    gv5Value->setMinimum(-1024);
    gv5Value->setMaximum(1024);
    if (phase->gvars[4]<1024) {
      gv5Value->setValue(phase->gvars[4]);
      gv5Value->setEnabled(true);
    } else {
      uint index=phase->gvars[4]-1025;
      if (index>=phase_idx) {
        index++;
      }
      PhaseData *phasegvar = &g_model.phaseData[index];
      gv5Value->setValue(phasegvar->gvars[4]);
      gv5Value->setDisabled(true);
    }    
    if (re1Value) {
      re1Value->setMinimum(-1024);
      re1Value->setMaximum(1024);
    }
    if (re2Value) {
      re2Value->setMinimum(-1024);
      re2Value->setMaximum(1024);
    }  
    if (re1Use) {
      populateGvarUseCB(re1Use, phase_idx);
      if (phase->rotaryEncoders[0]>1024) {
        re1Use->setCurrentIndex(phase->rotaryEncoders[0]-1024);
      }    
    }
    if (phase->rotaryEncoders[0]<1024) {
      re1Value->setValue(phase->rotaryEncoders[0]);
      re1Value->setEnabled(true);
    } else {
      uint index=phase->rotaryEncoders[0]-1025;
      if (index>=phase_idx) {
        index++;
      }
      PhaseData *phasegvar = &g_model.phaseData[index];
      re1Value->setValue(phasegvar->rotaryEncoders[0]);
      re1Value->setDisabled(true);
    }  
    if (re2Use) {
      populateGvarUseCB(re2Use, phase_idx);
      if (phase->rotaryEncoders[0]>1024) {
        re1Use->setCurrentIndex(phase->rotaryEncoders[0]-1024);
      }    
    }
    if (phase->rotaryEncoders[1]<1024) {
      re2Value->setValue(phase->rotaryEncoders[1]);
      re2Value->setEnabled(true);
    } else {
      uint index=phase->rotaryEncoders[1]-1025;
      if (index>=phase_idx) {
        index++;
      }
      PhaseData *phasegvar = &g_model.phaseData[index];
      re2Value->setValue(phasegvar->rotaryEncoders[1]);
      re2Value->setDisabled(true);
    }  
    if (GetEepromInterface()->getCapability(RotaryEncoders)<2) {
      if (re2Label)
        re2Label->hide();
      if (re2Use)
        re2Use->hide();
      re2Value->hide();
    }
    if (GetEepromInterface()->getCapability(RotaryEncoders)<1) {
      if (re1Label)
        re1Label->hide();
      if (re1Use)
        re1Use->hide();
      re1Value->hide();
    }
    QString labels[] = { tr("Rud"), tr("Ele"), tr("Thr"), tr("Ail") };
    if (trim1Label) {
      trim1Label->setText(labels[CONVERT_MODE(1)-1]);
      trim2Label->setText(labels[CONVERT_MODE(2)-1]);
      trim3Label->setText(labels[CONVERT_MODE(3)-1]);
      trim4Label->setText(labels[CONVERT_MODE(4)-1]);
    }
    // the connects
    if (doConnect) {
      connect(name, SIGNAL(editingFinished()), this, SLOT(phaseName_editingFinished()));
      if (sw!=NULL) {
        connect(sw,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseSwitch_currentIndexChanged()));
        connect(trim1Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseTrimUse_currentIndexChanged()));
        connect(trim2Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseTrimUse_currentIndexChanged()));
        connect(trim3Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseTrimUse_currentIndexChanged()));
        connect(trim4Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseTrimUse_currentIndexChanged()));
      }
      if (gv1Use) connect(gv1Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseGVUse_currentIndexChanged()));
      if (gv2Use) connect(gv2Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseGVUse_currentIndexChanged()));
      if (gv3Use) connect(gv3Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseGVUse_currentIndexChanged()));
      if (gv4Use) connect(gv4Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseGVUse_currentIndexChanged()));
      if (gv5Use) connect(gv5Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseGVUse_currentIndexChanged()));
      if (re1Use) connect(re1Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseREUse_currentIndexChanged()));
      if (re2Use) connect(re2Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseREUse_currentIndexChanged()));
      connect(gv1Value,SIGNAL(editingFinished()),this,SLOT(phaseGVValue_editingFinished()));
      connect(gv2Value,SIGNAL(editingFinished()),this,SLOT(phaseGVValue_editingFinished()));
      connect(gv3Value,SIGNAL(editingFinished()),this,SLOT(phaseGVValue_editingFinished()));
      connect(gv4Value,SIGNAL(editingFinished()),this,SLOT(phaseGVValue_editingFinished()));
      connect(gv5Value,SIGNAL(editingFinished()),this,SLOT(phaseGVValue_editingFinished()));
      connect(re1Value,SIGNAL(editingFinished()),this,SLOT(phaseREValue_editingFinished()));
      connect(re2Value,SIGNAL(editingFinished()),this,SLOT(phaseREValue_editingFinished()));

      connect(fadeIn,SIGNAL(editingFinished()),this,SLOT(phaseFadeIn_editingFinished()));
      connect(fadeOut,SIGNAL(editingFinished()),this,SLOT(phaseFadeOut_editingFinished()));
      connect(trim1,SIGNAL(valueChanged(int)),this,SLOT(phaseTrim_valueChanged()));
      connect(trim2,SIGNAL(valueChanged(int)),this,SLOT(phaseTrim_valueChanged()));
      connect(trim3,SIGNAL(valueChanged(int)),this,SLOT(phaseTrim_valueChanged()));
      connect(trim4,SIGNAL(valueChanged(int)),this,SLOT(phaseTrim_valueChanged()));
      connect(trim1Slider,SIGNAL(valueChanged(int)),this,SLOT(phaseTrimSlider_valueChanged()));
      connect(trim2Slider,SIGNAL(valueChanged(int)),this,SLOT(phaseTrimSlider_valueChanged()));
      connect(trim3Slider,SIGNAL(valueChanged(int)),this,SLOT(phaseTrimSlider_valueChanged()));
      connect(trim4Slider,SIGNAL(valueChanged(int)),this,SLOT(phaseTrimSlider_valueChanged()));
      phasesLock=false;  
    }
}

void ModelEdit::tabPhases()
{
    QSpinBox * tmp[9][5]={
      {ui->phase0GV1Value,ui->phase0GV2Value,ui->phase0GV3Value,ui->phase0GV4Value,ui->phase0GV5Value} ,
      {ui->phase1GV1Value,ui->phase1GV2Value,ui->phase1GV3Value,ui->phase1GV4Value,ui->phase1GV5Value} ,
      {ui->phase2GV1Value,ui->phase2GV2Value,ui->phase2GV3Value,ui->phase2GV4Value,ui->phase2GV5Value} ,
      {ui->phase3GV1Value,ui->phase3GV2Value,ui->phase3GV3Value,ui->phase3GV4Value,ui->phase3GV5Value} ,
      {ui->phase4GV1Value,ui->phase4GV2Value,ui->phase4GV3Value,ui->phase4GV4Value,ui->phase4GV5Value} ,
      {ui->phase5GV1Value,ui->phase5GV2Value,ui->phase5GV3Value,ui->phase5GV4Value,ui->phase5GV5Value} ,
      {ui->phase6GV1Value,ui->phase6GV2Value,ui->phase6GV3Value,ui->phase6GV4Value,ui->phase6GV5Value} ,
      {ui->phase7GV1Value,ui->phase7GV2Value,ui->phase7GV3Value,ui->phase7GV4Value,ui->phase7GV5Value} ,
      {ui->phase8GV1Value,ui->phase8GV2Value,ui->phase8GV3Value,ui->phase8GV4Value,ui->phase8GV5Value}
    };

    QSpinBox * tmp2[9][2]={
      {ui->phase0RE1Value,ui->phase0RE2Value} ,
      {ui->phase1RE1Value,ui->phase1RE2Value} ,
      {ui->phase2RE1Value,ui->phase2RE2Value} ,
      {ui->phase3RE1Value,ui->phase3RE2Value} ,
      {ui->phase4RE1Value,ui->phase4RE2Value} ,
      {ui->phase5RE1Value,ui->phase5RE2Value} ,
      {ui->phase6RE1Value,ui->phase6RE2Value} ,
      {ui->phase7RE1Value,ui->phase7RE2Value} ,
      {ui->phase8RE1Value,ui->phase8RE2Value} ,    
    };

    memcpy(gvarsSB, tmp, sizeof(gvarsSB));
    memcpy(reSB, tmp2, sizeof(reSB));

    phasesLock = true;
    int gvars=0;
    if (GetEepromInterface()->getCapability(HasVariants)) {
      if (GetCurrentFirmwareVariant() & GVARS_VARIANT)
        gvars=1;
    } else {
      gvars=GetEepromInterface()->getCapability(Gvars);
    }
    if (!GetEepromInterface()->getCapability(RotaryEncoders)) {
      ui->phase0reGB->hide();
      ui->phase1reGB->hide();
      ui->phase2reGB->hide();
      ui->phase3reGB->hide();
      ui->phase4reGB->hide();
      ui->phase5reGB->hide();
      ui->phase6reGB->hide();
      ui->phase7reGB->hide();
      ui->phase8reGB->hide();
    }
    if (!GetEepromInterface()->getCapability(GvarsFlightPhases) || gvars==0) {
      if (!GetEepromInterface()->getCapability(Gvars) || gvars==0) {
        ui->phase0gvGB->hide();
      }
      ui->phase1gvGB->hide();
      ui->phase2gvGB->hide();
      ui->phase3gvGB->hide();
      ui->phase4gvGB->hide();
      ui->phase5gvGB->hide();
      ui->phase6gvGB->hide();
      ui->phase7gvGB->hide();
      ui->phase8gvGB->hide();
    }

    if (!GetEepromInterface()->getCapability(GvarsAreNamed)) {
      ui->phase0GV1Name->hide();
      ui->phase0GV2Name->hide();
      ui->phase0GV3Name->hide();
      ui->phase0GV4Name->hide();
      ui->phase0GV5Name->hide();  
      ui->phase0GV6Name->hide();
      ui->phase0GV7Name->hide();  
    }

    if (GetEepromInterface()->getCapability(Gvars) &&  gvars==1) {
      if (!GetEepromInterface()->getCapability(GvarsHaveSources)) {
        ui->phase0GV1Source->hide();
        ui->phase0GV2Source->hide();
        ui->phase0GV3Source->hide();
        ui->phase0GV4Source->hide();
        ui->phase0GV5Source->hide();
        ui->phase0GV6Source->hide();
        ui->phase0GV7Source->hide();
      } else {
        populateGvSourceCB(ui->phase0GV1Source, g_model.gvsource[0]);
        populateGvSourceCB(ui->phase0GV2Source, g_model.gvsource[1]);
        populateGvSourceCB(ui->phase0GV3Source, g_model.gvsource[2]);
        populateGvSourceCB(ui->phase0GV4Source, g_model.gvsource[3]);
        populateGvSourceCB(ui->phase0GV5Source, g_model.gvsource[4]);  
        connect(ui->phase0GV1Source,SIGNAL(currentIndexChanged(int)),this,SLOT(GVSource_currentIndexChanged()));
        connect(ui->phase0GV2Source,SIGNAL(currentIndexChanged(int)),this,SLOT(GVSource_currentIndexChanged()));
        connect(ui->phase0GV3Source,SIGNAL(currentIndexChanged(int)),this,SLOT(GVSource_currentIndexChanged()));
        connect(ui->phase0GV4Source,SIGNAL(currentIndexChanged(int)),this,SLOT(GVSource_currentIndexChanged()));
        connect(ui->phase0GV5Source,SIGNAL(currentIndexChanged(int)),this,SLOT(GVSource_currentIndexChanged()));
      }
      if (IS_TARANIS(GetEepromInterface()->getBoard())) {
        ui->phase0GV1Name->setMaxLength(10);
        ui->phase0GV2Name->setMaxLength(10);
        ui->phase0GV3Name->setMaxLength(10);
        ui->phase0GV4Name->setMaxLength(10);
        ui->phase0GV5Name->setMaxLength(10);
      } else {
        ui->phase0GV1Name->setMaxLength(6);
        ui->phase0GV2Name->setMaxLength(6);
        ui->phase0GV3Name->setMaxLength(6);
        ui->phase0GV4Name->setMaxLength(6);
        ui->phase0GV5Name->setMaxLength(6);      
      }

      ui->phase0GV1Name->setText(g_model.gvars_names[0]);
      ui->phase0GV2Name->setText(g_model.gvars_names[1]);
      ui->phase0GV3Name->setText(g_model.gvars_names[2]);
      ui->phase0GV4Name->setText(g_model.gvars_names[3]);
      ui->phase0GV5Name->setText(g_model.gvars_names[4]);
      connect(ui->phase0GV1Name,SIGNAL(editingFinished()),this,SLOT(GVName_editingFinished()));
      connect(ui->phase0GV2Name,SIGNAL(editingFinished()),this,SLOT(GVName_editingFinished()));
      connect(ui->phase0GV3Name,SIGNAL(editingFinished()),this,SLOT(GVName_editingFinished()));
      connect(ui->phase0GV4Name,SIGNAL(editingFinished()),this,SLOT(GVName_editingFinished()));
      connect(ui->phase0GV5Name,SIGNAL(editingFinished()),this,SLOT(GVName_editingFinished()));
    }
    if (gvars) {
      if (GetEepromInterface()->getCapability(GvarsNum)<7) {
        ui->phase0GV6Name->hide();
        ui->phase0GV7Name->hide();
        ui->phase0GV6Source->hide();
        ui->phase0GV7Source->hide();
        ui->phase0GV6Value->hide();
        ui->phase0GV7Value->hide();
        ui->phase0GV6_Label->hide();
        ui->phase0GV7_Label->hide();
      } else {
        populateGvSourceCB(ui->phase0GV6Source, g_model.gvsource[5]);
        populateGvSourceCB(ui->phase0GV7Source, g_model.gvsource[6]);  
        connect(ui->phase0GV6Source,SIGNAL(currentIndexChanged(int)),this,SLOT(GVSource_currentIndexChanged()));
        connect(ui->phase0GV7Source,SIGNAL(currentIndexChanged(int)),this,SLOT(GVSource_currentIndexChanged()));
        ui->phase0GV6Name->setText(g_model.gvars_names[5]);
        ui->phase0GV7Name->setText(g_model.gvars_names[6]);
        connect(ui->phase0GV6Name,SIGNAL(editingFinished()),this,SLOT(GVName_editingFinished()));
        connect(ui->phase0GV7Name,SIGNAL(editingFinished()),this,SLOT(GVName_editingFinished()));
        ui->phase0GV6Value->setValue(g_model.phaseData[0].gvars[5]);
        ui->phase0GV7Value->setValue(g_model.phaseData[0].gvars[6]);
        connect(ui->phase0GV6Value,SIGNAL(editingFinished()),this,SLOT(phaseGVValue_editingFinished()));
        connect(ui->phase0GV7Value,SIGNAL(editingFinished()),this,SLOT(phaseGVValue_editingFinished()));
      }
    }

    displayOnePhase(0, ui->phase0Name, NULL,                   ui->phase0FadeIn, ui->phase0FadeOut, NULL,                      ui->phase0Trim1Value, ui->phase0Trim1Label, ui->phase0Trim1Slider, NULL,                       ui->phase0Trim2Value, ui->phase0Trim2Label, ui->phase0Trim2Slider, NULL,                       ui->phase0Trim3Value, ui->phase0Trim3Label, ui->phase0Trim3Slider, NULL,                       ui->phase0Trim4Value, ui->phase0Trim4Label, ui->phase0Trim4Slider, ui->phase0GV1_Label, NULL                    , ui->phase0GV1Value, ui->phase0GV2_Label, NULL                    , ui->phase0GV2Value, ui->phase0GV3_Label, NULL                    , ui->phase0GV3Value, ui->phase0GV4_Label, NULL                    , ui->phase0GV4Value, ui->phase0GV5_Label, NULL                    , ui->phase0GV5Value,ui->phase0REA_Label,NULL, ui->phase0RE1Value,ui->phase0REB_Label, NULL, ui->phase0RE2Value, true);
    displayOnePhase(1, ui->phase1Name, ui->phase1Switch, ui->phase1FadeIn, ui->phase1FadeOut, ui->phase1Trim1Use, ui->phase1Trim1Value, ui->phase1Trim1Label, ui->phase1Trim1Slider, ui->phase1Trim2Use, ui->phase1Trim2Value, ui->phase1Trim2Label, ui->phase1Trim2Slider, ui->phase1Trim3Use, ui->phase1Trim3Value, ui->phase1Trim3Label, ui->phase1Trim3Slider, ui->phase1Trim4Use, ui->phase1Trim4Value, ui->phase1Trim4Label, ui->phase1Trim4Slider, ui->phase1GV1_Label, ui->phase1GV1Use, ui->phase1GV1Value, ui->phase1GV2_Label, ui->phase1GV2Use, ui->phase1GV2Value, ui->phase1GV3_Label, ui->phase1GV3Use, ui->phase1GV3Value, ui->phase1GV4_Label, ui->phase1GV4Use, ui->phase1GV4Value, ui->phase1GV5_Label, ui->phase1GV5Use, ui->phase1GV5Value,ui->phase1REA_Label, ui->phase1RE1Use, ui->phase1RE1Value,ui->phase1REB_Label, ui->phase1RE2Use, ui->phase1RE2Value, true);
    displayOnePhase(2, ui->phase2Name, ui->phase2Switch, ui->phase2FadeIn, ui->phase2FadeOut, ui->phase2Trim1Use, ui->phase2Trim1Value, ui->phase2Trim1Label, ui->phase2Trim1Slider, ui->phase2Trim2Use, ui->phase2Trim2Value, ui->phase2Trim2Label, ui->phase2Trim2Slider, ui->phase2Trim3Use, ui->phase2Trim3Value, ui->phase2Trim3Label, ui->phase2Trim3Slider, ui->phase2Trim4Use, ui->phase2Trim4Value, ui->phase2Trim4Label, ui->phase2Trim4Slider, ui->phase2GV1_Label, ui->phase2GV1Use, ui->phase2GV1Value, ui->phase2GV2_Label, ui->phase2GV2Use, ui->phase2GV2Value, ui->phase2GV3_Label, ui->phase2GV3Use, ui->phase2GV3Value, ui->phase2GV4_Label, ui->phase2GV4Use, ui->phase2GV4Value, ui->phase2GV5_Label, ui->phase2GV5Use, ui->phase2GV5Value,ui->phase2REA_Label, ui->phase2RE1Use, ui->phase2RE1Value,ui->phase2REB_Label, ui->phase2RE2Use, ui->phase2RE2Value, true);
    displayOnePhase(3, ui->phase3Name, ui->phase3Switch, ui->phase3FadeIn, ui->phase3FadeOut, ui->phase3Trim1Use, ui->phase3Trim1Value, ui->phase3Trim1Label, ui->phase3Trim1Slider, ui->phase3Trim2Use, ui->phase3Trim2Value, ui->phase3Trim2Label, ui->phase3Trim2Slider, ui->phase3Trim3Use, ui->phase3Trim3Value, ui->phase3Trim3Label, ui->phase3Trim3Slider, ui->phase3Trim4Use, ui->phase3Trim4Value, ui->phase3Trim4Label, ui->phase3Trim4Slider, ui->phase3GV1_Label, ui->phase3GV1Use, ui->phase3GV1Value, ui->phase3GV2_Label, ui->phase3GV2Use, ui->phase3GV2Value, ui->phase3GV3_Label, ui->phase3GV3Use, ui->phase3GV3Value, ui->phase3GV4_Label, ui->phase3GV4Use, ui->phase3GV4Value, ui->phase3GV5_Label, ui->phase3GV5Use, ui->phase3GV5Value,ui->phase3REA_Label, ui->phase3RE1Use, ui->phase3RE1Value,ui->phase3REB_Label, ui->phase3RE2Use, ui->phase3RE2Value, true);
    displayOnePhase(4, ui->phase4Name, ui->phase4Switch, ui->phase4FadeIn, ui->phase4FadeOut, ui->phase4Trim1Use, ui->phase4Trim1Value, ui->phase4Trim1Label, ui->phase4Trim1Slider, ui->phase4Trim2Use, ui->phase4Trim2Value, ui->phase4Trim2Label, ui->phase4Trim2Slider, ui->phase4Trim3Use, ui->phase4Trim3Value, ui->phase4Trim3Label, ui->phase4Trim3Slider, ui->phase4Trim4Use, ui->phase4Trim4Value, ui->phase4Trim4Label, ui->phase4Trim4Slider, ui->phase4GV1_Label, ui->phase4GV1Use, ui->phase4GV1Value, ui->phase4GV2_Label, ui->phase4GV2Use, ui->phase4GV2Value, ui->phase4GV3_Label, ui->phase4GV3Use, ui->phase4GV3Value, ui->phase4GV4_Label, ui->phase4GV4Use, ui->phase4GV4Value, ui->phase4GV5_Label, ui->phase4GV5Use, ui->phase4GV5Value,ui->phase4REA_Label, ui->phase4RE1Use, ui->phase4RE1Value,ui->phase4REB_Label, ui->phase4RE2Use, ui->phase4RE2Value, true);
    displayOnePhase(5, ui->phase5Name, ui->phase5Switch, ui->phase5FadeIn, ui->phase5FadeOut, ui->phase5Trim1Use, ui->phase5Trim1Value, ui->phase5Trim1Label, ui->phase5Trim1Slider, ui->phase5Trim2Use, ui->phase5Trim2Value, ui->phase5Trim2Label, ui->phase5Trim2Slider, ui->phase5Trim3Use, ui->phase5Trim3Value, ui->phase5Trim3Label, ui->phase5Trim3Slider, ui->phase5Trim4Use, ui->phase5Trim4Value, ui->phase5Trim4Label, ui->phase5Trim4Slider, ui->phase5GV1_Label, ui->phase5GV1Use, ui->phase5GV1Value, ui->phase5GV2_Label, ui->phase5GV2Use, ui->phase5GV2Value, ui->phase5GV3_Label, ui->phase5GV3Use, ui->phase5GV3Value, ui->phase5GV4_Label, ui->phase5GV4Use, ui->phase5GV4Value, ui->phase5GV5_Label, ui->phase5GV5Use, ui->phase5GV5Value,ui->phase5REA_Label, ui->phase5RE1Use, ui->phase5RE1Value,ui->phase5REB_Label, ui->phase5RE2Use, ui->phase5RE2Value, true);
    displayOnePhase(6, ui->phase6Name, ui->phase6Switch, ui->phase6FadeIn, ui->phase6FadeOut, ui->phase6Trim1Use, ui->phase6Trim1Value, ui->phase6Trim1Label, ui->phase6Trim1Slider, ui->phase6Trim2Use, ui->phase6Trim2Value, ui->phase6Trim2Label, ui->phase6Trim2Slider, ui->phase6Trim3Use, ui->phase6Trim3Value, ui->phase6Trim3Label, ui->phase6Trim3Slider, ui->phase6Trim4Use, ui->phase6Trim4Value, ui->phase6Trim4Label, ui->phase6Trim4Slider, ui->phase6GV1_Label, ui->phase6GV1Use, ui->phase6GV1Value, ui->phase6GV2_Label, ui->phase6GV2Use, ui->phase6GV2Value, ui->phase6GV3_Label, ui->phase6GV3Use, ui->phase6GV3Value, ui->phase6GV4_Label, ui->phase6GV4Use, ui->phase6GV4Value, ui->phase6GV5_Label, ui->phase6GV5Use, ui->phase6GV5Value,ui->phase6REA_Label, ui->phase6RE1Use, ui->phase6RE1Value,ui->phase6REB_Label, ui->phase6RE2Use, ui->phase6RE2Value, true);
    displayOnePhase(7, ui->phase7Name, ui->phase7Switch, ui->phase7FadeIn, ui->phase7FadeOut, ui->phase7Trim1Use, ui->phase7Trim1Value, ui->phase7Trim1Label, ui->phase7Trim1Slider, ui->phase7Trim2Use, ui->phase7Trim2Value, ui->phase7Trim2Label, ui->phase7Trim2Slider, ui->phase7Trim3Use, ui->phase7Trim3Value, ui->phase7Trim3Label, ui->phase7Trim3Slider, ui->phase7Trim4Use, ui->phase7Trim4Value, ui->phase7Trim4Label, ui->phase7Trim4Slider, ui->phase7GV1_Label, ui->phase7GV1Use, ui->phase7GV1Value, ui->phase7GV2_Label, ui->phase7GV2Use, ui->phase7GV2Value, ui->phase7GV3_Label, ui->phase7GV3Use, ui->phase7GV3Value, ui->phase7GV4_Label, ui->phase7GV4Use, ui->phase7GV4Value, ui->phase7GV5_Label, ui->phase7GV5Use, ui->phase7GV5Value,ui->phase7REA_Label, ui->phase7RE1Use, ui->phase7RE1Value,ui->phase7REB_Label, ui->phase7RE2Use, ui->phase7RE2Value, true);
    displayOnePhase(8, ui->phase8Name, ui->phase8Switch, ui->phase8FadeIn, ui->phase8FadeOut, ui->phase8Trim1Use, ui->phase8Trim1Value, ui->phase8Trim1Label, ui->phase8Trim1Slider, ui->phase8Trim2Use, ui->phase8Trim2Value, ui->phase8Trim2Label, ui->phase8Trim2Slider, ui->phase8Trim3Use, ui->phase8Trim3Value, ui->phase8Trim3Label, ui->phase8Trim3Slider, ui->phase8Trim4Use, ui->phase8Trim4Value, ui->phase8Trim4Label, ui->phase8Trim4Slider, ui->phase8GV1_Label, ui->phase8GV1Use, ui->phase8GV1Value, ui->phase8GV2_Label, ui->phase8GV2Use, ui->phase8GV2Value, ui->phase8GV3_Label, ui->phase8GV3Use, ui->phase8GV3Value, ui->phase8GV4_Label, ui->phase8GV4Use, ui->phase8GV4Value, ui->phase8GV5_Label, ui->phase8GV5Use, ui->phase8GV5Value,ui->phase8REA_Label, ui->phase8RE1Use, ui->phase8RE1Value,ui->phase8REB_Label, ui->phase8RE2Use, ui->phase8RE2Value, true);

    QSlider * tmpsliders[9][4]={
      {ui->phase0Trim1Slider,ui->phase0Trim2Slider,ui->phase0Trim3Slider,ui->phase0Trim4Slider},
      {ui->phase1Trim1Slider,ui->phase1Trim2Slider,ui->phase1Trim3Slider,ui->phase1Trim4Slider},
      {ui->phase2Trim1Slider,ui->phase2Trim2Slider,ui->phase2Trim3Slider,ui->phase2Trim4Slider},
      {ui->phase3Trim1Slider,ui->phase3Trim2Slider,ui->phase3Trim3Slider,ui->phase3Trim4Slider},
      {ui->phase4Trim1Slider,ui->phase4Trim2Slider,ui->phase4Trim3Slider,ui->phase4Trim4Slider},
      {ui->phase5Trim1Slider,ui->phase5Trim2Slider,ui->phase5Trim3Slider,ui->phase5Trim4Slider},
      {ui->phase6Trim1Slider,ui->phase6Trim2Slider,ui->phase6Trim3Slider,ui->phase6Trim4Slider},
      {ui->phase7Trim1Slider,ui->phase7Trim2Slider,ui->phase7Trim3Slider,ui->phase7Trim4Slider},
      {ui->phase8Trim1Slider,ui->phase8Trim2Slider,ui->phase8Trim3Slider,ui->phase8Trim4Slider}
    };
    QSpinBox * tmpspinbox[9][4]={
      {ui->phase0Trim1Value,ui->phase0Trim2Value,ui->phase0Trim3Value,ui->phase0Trim4Value},
      {ui->phase1Trim1Value,ui->phase1Trim2Value,ui->phase1Trim3Value,ui->phase1Trim4Value},
      {ui->phase2Trim1Value,ui->phase2Trim2Value,ui->phase2Trim3Value,ui->phase2Trim4Value},
      {ui->phase3Trim1Value,ui->phase3Trim2Value,ui->phase3Trim3Value,ui->phase3Trim4Value},
      {ui->phase4Trim1Value,ui->phase4Trim2Value,ui->phase4Trim3Value,ui->phase4Trim4Value},
      {ui->phase5Trim1Value,ui->phase5Trim2Value,ui->phase5Trim3Value,ui->phase5Trim4Value},
      {ui->phase6Trim1Value,ui->phase6Trim2Value,ui->phase6Trim3Value,ui->phase6Trim4Value},
      {ui->phase7Trim1Value,ui->phase7Trim2Value,ui->phase7Trim3Value,ui->phase7Trim4Value},
      {ui->phase8Trim1Value,ui->phase8Trim2Value,ui->phase8Trim3Value,ui->phase8Trim4Value}
    };
    memcpy(phasesTrimSliders,tmpsliders,sizeof(phasesTrimSliders));
    memcpy(phasesTrimValues,tmpspinbox,sizeof(phasesTrimValues));

    int phases = GetEepromInterface()->getCapability(FlightPhases);
    if (phases < 9) {
      ui->phase8->setDisabled(true);
      ui->phases->removeTab(8);
    }
    if (phases < 8) {
      ui->phase7->setDisabled(true);
      ui->phases->removeTab(7);
    }
    if (phases < 7) {
      ui->phase6->setDisabled(true);
      ui->phases->removeTab(6);
    }
    if (phases < 6) {
      ui->phase5->setDisabled(true);
      ui->phases->removeTab(5);
    }
    if (phases < 5) {
      ui->phase4->setDisabled(true);
      ui->phases->removeTab(4);
    }
    if (phases < 4) {
      ui->phase3->setDisabled(true);
      ui->phases->removeTab(3);
    }
    if (phases < 3) {
      ui->phase2->setDisabled(true);
      ui->phases->removeTab(2);
    }
    if (phases < 2) {
      ui->phase1->setDisabled(true);
      ui->phases->removeTab(1);
      ui->phase0Name->setDisabled(true);
      ui->phase0FadeIn->setDisabled(true);
      ui->phase0FadeOut->setDisabled(true);
    }
    if ( GetEepromInterface()->getCapability(FlightPhasesAreNamed) ) {
      for (int i=0; i < phases; i++) {
        QString PhaseName=g_model.phaseData[i].name;
        QString TabName;
        if (i==0) { 
          TabName.append(QObject::tr("Flight Mode 0 (Default)"));
        } else {
          TabName.append(QObject::tr("FM %1").arg(i));
        }
        if (!PhaseName.isEmpty()) {
          TabName.append(" (");
          TabName.append(PhaseName);
          TabName.append(")");
        }
        ui->phases->setTabText(i,TabName);
      }
    } else {
      QLineEdit * tmp[]= { ui->phase0Name,ui->phase1Name,ui->phase2Name,ui->phase3Name,ui->phase4Name,ui->phase5Name,ui->phase6Name,ui->phase7Name,ui->phase8Name};
      QLabel * tmp2[] = { ui->label_fm0name,ui->label_fm1name,ui->label_fm2name,ui->label_fm3name,ui->label_fm4name,ui->label_fm5name,ui->label_fm6name,ui->label_fm7name,ui->label_fm8name};
      for (int i=0; i < phases; i++) {
        tmp[i]->hide();
        tmp2[i]->hide();
      }
    }
    ui->phases->setCurrentIndex(0);
    phasesLock = false;
}

void ModelEdit::on_phases_currentChanged(int index)
{
    phasesLock = true;
    switch(index) {
      case 1:
        displayOnePhase(1, NULL, NULL, NULL, NULL, NULL, ui->phase1Trim1Value, NULL, ui->phase1Trim1Slider, NULL, ui->phase1Trim2Value, NULL, ui->phase1Trim2Slider, NULL, ui->phase1Trim3Value, NULL, ui->phase1Trim3Slider, NULL, ui->phase1Trim4Value, NULL, ui->phase1Trim4Slider, NULL, NULL, ui->phase1GV1Value, NULL, NULL, ui->phase1GV2Value, NULL, NULL, ui->phase1GV3Value, NULL, NULL, ui->phase1GV4Value, NULL, NULL, ui->phase1GV5Value,NULL,NULL, ui->phase1RE1Value,NULL,NULL, ui->phase1RE2Value);
        break;
      case 2:
        displayOnePhase(2, NULL, NULL, NULL, NULL, NULL, ui->phase2Trim1Value, NULL, ui->phase2Trim1Slider, NULL, ui->phase2Trim2Value, NULL, ui->phase2Trim2Slider, NULL, ui->phase2Trim3Value, NULL, ui->phase2Trim3Slider, NULL, ui->phase2Trim4Value, NULL, ui->phase2Trim4Slider, NULL, NULL, ui->phase2GV1Value, NULL, NULL, ui->phase2GV2Value, NULL, NULL, ui->phase2GV3Value, NULL, NULL, ui->phase2GV4Value, NULL, NULL, ui->phase2GV5Value,NULL,NULL, ui->phase2RE1Value,NULL,NULL, ui->phase2RE2Value);
        break;
      case 3:
        displayOnePhase(3, NULL, NULL, NULL, NULL, NULL, ui->phase3Trim1Value, NULL, ui->phase3Trim1Slider, NULL, ui->phase3Trim2Value, NULL, ui->phase3Trim2Slider, NULL, ui->phase3Trim3Value, NULL, ui->phase3Trim3Slider, NULL, ui->phase3Trim4Value, NULL, ui->phase3Trim4Slider, NULL, NULL, ui->phase3GV1Value, NULL, NULL, ui->phase3GV2Value, NULL, NULL, ui->phase3GV3Value, NULL, NULL, ui->phase3GV4Value, NULL, NULL, ui->phase3GV5Value,NULL,NULL, ui->phase3RE1Value,NULL,NULL, ui->phase3RE2Value);
        break;
      case 4:
        displayOnePhase(4, NULL, NULL, NULL, NULL, NULL, ui->phase4Trim1Value, NULL, ui->phase4Trim1Slider, NULL, ui->phase4Trim2Value, NULL, ui->phase4Trim2Slider, NULL, ui->phase4Trim3Value, NULL, ui->phase4Trim3Slider, NULL, ui->phase4Trim4Value, NULL, ui->phase4Trim4Slider, NULL, NULL, ui->phase4GV1Value, NULL, NULL, ui->phase4GV2Value, NULL, NULL, ui->phase4GV3Value, NULL, NULL, ui->phase4GV4Value, NULL, NULL, ui->phase4GV5Value,NULL,NULL, ui->phase4RE1Value,NULL,NULL, ui->phase4RE2Value);
        break;
      case 5:
        displayOnePhase(5, NULL, NULL, NULL, NULL, NULL, ui->phase5Trim1Value, NULL, ui->phase5Trim1Slider, NULL, ui->phase5Trim2Value, NULL, ui->phase5Trim2Slider, NULL, ui->phase5Trim3Value, NULL, ui->phase5Trim3Slider, NULL, ui->phase5Trim4Value, NULL, ui->phase5Trim4Slider, NULL, NULL, ui->phase5GV1Value, NULL, NULL, ui->phase5GV2Value, NULL, NULL, ui->phase5GV3Value, NULL, NULL, ui->phase5GV4Value, NULL, NULL, ui->phase5GV5Value,NULL,NULL, ui->phase5RE1Value,NULL,NULL, ui->phase5RE2Value);
        break;
      case 6:
        displayOnePhase(6, NULL, NULL, NULL, NULL, NULL, ui->phase6Trim1Value, NULL, ui->phase6Trim1Slider, NULL, ui->phase6Trim2Value, NULL, ui->phase6Trim2Slider, NULL, ui->phase6Trim3Value, NULL, ui->phase6Trim3Slider, NULL, ui->phase6Trim4Value, NULL, ui->phase6Trim4Slider, NULL, NULL, ui->phase6GV1Value, NULL, NULL, ui->phase6GV2Value, NULL, NULL, ui->phase6GV3Value, NULL, NULL, ui->phase6GV4Value, NULL, NULL, ui->phase6GV5Value,NULL,NULL, ui->phase6RE1Value,NULL,NULL, ui->phase6RE2Value);
        break;
      case 7:
        displayOnePhase(7, NULL, NULL, NULL, NULL, NULL, ui->phase7Trim1Value, NULL, ui->phase7Trim1Slider, NULL, ui->phase7Trim2Value, NULL, ui->phase7Trim2Slider, NULL, ui->phase7Trim3Value, NULL, ui->phase7Trim3Slider, NULL, ui->phase7Trim4Value, NULL, ui->phase7Trim4Slider, NULL, NULL, ui->phase7GV1Value, NULL, NULL, ui->phase7GV2Value, NULL, NULL, ui->phase7GV3Value, NULL, NULL, ui->phase7GV4Value, NULL, NULL, ui->phase7GV5Value,NULL,NULL, ui->phase7RE1Value,NULL,NULL, ui->phase7RE2Value);
        break;
      case 8:
        displayOnePhase(8, NULL, NULL, NULL, NULL, NULL, ui->phase8Trim1Value, NULL, ui->phase8Trim1Slider, NULL, ui->phase8Trim2Value, NULL, ui->phase8Trim2Slider, NULL, ui->phase8Trim3Value, NULL, ui->phase8Trim3Slider, NULL, ui->phase8Trim4Value, NULL, ui->phase8Trim4Slider, NULL, NULL, ui->phase8GV1Value, NULL, NULL, ui->phase8GV2Value, NULL, NULL, ui->phase8GV3Value, NULL, NULL, ui->phase8GV4Value, NULL, NULL, ui->phase8GV5Value,NULL,NULL, ui->phase8RE1Value,NULL,NULL, ui->phase8RE2Value);
        break;
    }
    phasesLock = false;
}

void ModelEdit::tabExpos()
{
    // curDest -> destination channel
    // i -> mixer number
    QByteArray qba;
    ExposlistWidget->clear();
    int curDest = -1;

    for(int i=0; i<C9X_MAX_EXPOS; i++) {
      ExpoData *md = &g_model.expoData[i];

      if (md->mode==0) break;
      QString str = "";
      while(curDest<(int)md->chn-1) {
        curDest++;
        str = getStickStr(curDest);
        qba.clear();
        qba.append((quint8)-curDest-1);
        QListWidgetItem *itm = new QListWidgetItem(str);
        itm->setData(Qt::UserRole,qba);
        ExposlistWidget->addItem(itm);
      }

      if(curDest!=(int)md->chn) {
        str = getStickStr(md->chn);
        curDest = md->chn;
      } else {
        str = "   ";
      }

      switch (md->mode) {
        case (1): str += " <-"; break;
        case (2): str += " ->"; break;
        default:  str += "   "; break;
      };

      str += tr("Weight") + getGVarString(md->weight).rightJustified(6, ' ');
      if (!GetEepromInterface()->getCapability(ExpoIsCurve)) {
        if (md->expo!=0)
          str += " " + tr("Expo") + getGVarString(md->expo, true).rightJustified(7, ' ');
      } else {
        if (md->curveMode==0 && md->curveParam!=0)  
          str += " " + tr("Expo") + getGVarString(md->curveParam, true).rightJustified(7, ' ');
      }
      if (GetEepromInterface()->getCapability(FlightPhases)) {
        if(md->phases) {
          if (md->phases!=(unsigned int)(1<<GetEepromInterface()->getCapability(FlightPhases))-1) {
            int mask=1;
            int first=0;
            for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases);i++) {
              if (!(md->phases & mask)) {
                first++;
              }
              mask <<=1;
            }
            if (first>1) {
              str += " " + tr("Flight modes") + QString("(");
            } else {
              str += " " + tr("Flight mode") + QString("(");
            }
            mask=1;
            first=1;
            for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases);i++) {
              if (!(md->phases & mask)) {
                if (!first) {
                  str += QString(", ")+ QString("%1").arg(getPhaseName(i+1, g_model.phaseData[i].name));
                } else {
                  str += QString("%1").arg(getPhaseName(i+1,g_model.phaseData[i].name));
                  first=0;
                }
              }
              mask <<=1;
            }
            str += QString(")");
          } else {
            str += tr("DISABLED")+QString(" !!!");
          }
        }
      } 
      if (md->swtch.type != SWITCH_TYPE_NONE) str += " " + tr("Switch") + QString("(%1)").arg(md->swtch.toString());
      if (md->curveMode)
        if (md->curveParam) str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(md->curveParam));
      if (GetEepromInterface()->getCapability(HasExpoNames)) {
        QString ExpoName;
        ExpoName.append(md->name);
        if (!ExpoName.isEmpty()) {
          str+=QString("(%1)").arg(ExpoName);
        }
      }
      qba.clear();
      qba.append((quint8)i);
      qba.append((const char*)md, sizeof(ExpoData));
      QListWidgetItem *itm = new QListWidgetItem(str);
      itm->setData(Qt::UserRole,qba);  // expo number
      ExposlistWidget->addItem(itm);   //(str);
    }

    while(curDest<NUM_STICKS-1) {
      curDest++;
      QString str = getStickStr(curDest);
      qba.clear();
      qba.append((quint8)-curDest-1);
      QListWidgetItem *itm = new QListWidgetItem(str);
      itm->setData(Qt::UserRole,qba); // add new expo
      ExposlistWidget->addItem(itm);
    }
}

void ModelEdit::exposEdited()
{
    updateSettings();
}

void ModelEdit::tabMixes()
{
    // curDest -> destination channel4
    // i -> mixer number
    QByteArray qba;
    MixerlistWidget->clear();
    unsigned int curDest = 0;
    int i;
    unsigned int outputs = GetEepromInterface()->getCapability(Outputs);
    int showNames=ui->showNames_Ckb->isChecked();
    for(i=0; i<GetEepromInterface()->getCapability(Mixes); i++) {
      MixData *md = &g_model.mixData[i];
      if ((md->destCh==0) || (md->destCh>outputs+(unsigned int)GetEepromInterface()->getCapability(ExtraChannels))) continue; 
      QString str = "";
      while(curDest<(md->destCh-1)) {
        curDest++;
        if (curDest > outputs) {
          str = tr("X%1  ").arg(curDest-outputs);
        } else {
          str = tr("CH%1%2").arg(curDest/10).arg(curDest%10);
          if (GetEepromInterface()->getCapability(HasChNames) && showNames) {
            QString name=g_model.limitData[curDest-1].name;
            if (!name.isEmpty()) {
              name.append("     ");
              str=name.left(6);
            }
          }
        }
        qba.clear();
        qba.append((quint8)-curDest);
        QListWidgetItem *itm = new QListWidgetItem(str);
        itm->setData(Qt::UserRole,qba);
        MixerlistWidget->addItem(itm);
      }

      if (md->destCh > outputs) {
        str = tr("X%1  ").arg(md->destCh-outputs);
      } else {
        str = tr("CH%1%2").arg(md->destCh/10).arg(md->destCh%10);
        str.append("  ");
        if (GetEepromInterface()->getCapability(HasChNames) && showNames) {
          QString name=g_model.limitData[md->destCh-1].name;
          if (!name.isEmpty()) {
            name.append("     ");
            str=name.left(6);
          }
        }
      }
      if (curDest != md->destCh) {
        curDest = md->destCh;
      } else {
        str.fill(' ');
      }

      switch(md->mltpx) {
        case (1): str += " *"; break;
        case (2): str += " R"; break;
        default:  str += "  "; break;
      };

      str += " " + getGVarString(md->weight, true).rightJustified(6, ' ');
      str += md->srcRaw.toString();
      unsigned int fpCount = GetEepromInterface()->getCapability(FlightPhases);
      if (GetEepromInterface()->getCapability(FlightPhases)) {
        if(md->phases) {
          if (md->phases!=(unsigned int)(1<<fpCount)-1) {
            int mask=1;
            int first=0;
            for (unsigned int i=0; i<fpCount; i++) {
              if (!(md->phases & mask)) {
                first++;
              }
              mask <<=1;
            }
            if (first>1) {
              str += " " + tr("Flight modes") + QString("(");
            } else {
              str += " " + tr("Flight mode") + QString("(");
            }
            mask=1;
            first=1;
            for (unsigned int i=0; i<fpCount; i++) {
              if (!(md->phases & mask)) {
                if (!first) {
                  str += QString(", ")+ QString("%1").arg(getPhaseName(i+1, g_model.phaseData[i].name));
                }
                else {
                  str += QString("%1").arg(getPhaseName(i+1,g_model.phaseData[i].name));
                  first=0;
                }
              }
              mask <<=1;
            }
            str += QString(")");
          } else {
            str += tr("DISABLED")+QString(" !!!");
          }
        }
      }
      if(md->swtch.type != SWITCH_TYPE_NONE) str += " " + tr("Switch") + QString("(%1)").arg(md->swtch.toString());
      if(md->carryTrim>0) {
        str += " " +tr("No Trim");
      } else if (md->carryTrim<0) {
        str += " " + RawSource(SOURCE_TYPE_TRIM, (-(md->carryTrim)-1)).toString();
      }
      if(md->noExpo) {
        str += " " +tr("No DR/Expo");
      } 
      if (GetEepromInterface()->getCapability(MixFmTrim) && md->enableFmTrim==1) {
        if (md->sOffset) str += " " + tr("FMTrim") + QString("(%1%)").arg(md->sOffset);
      } else {
        if (md->sOffset) str += " " + tr("Offset") + getGVarString(md->sOffset);
      }
      if (md->differential) str += " " + tr("Diff") + getGVarString(md->differential);
      if (md->curve) str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(md->curve));
      int scale=GetEepromInterface()->getCapability(SlowScale);
      if (scale==0)
        scale=1;
      if (md->delayDown || md->delayUp)
        str += tr(" Delay(u%1:d%2)").arg((double)md->delayUp/scale).arg((double)md->delayDown/scale);
      if (md->speedDown || md->speedUp)
        str += tr(" Slow(u%1:d%2)").arg((double)md->speedUp/scale).arg((double)md->speedDown/scale);
      if (md->mixWarn)  str += tr(" Warn(%1)").arg(md->mixWarn);
      if (GetEepromInterface()->getCapability(HasMixerNames)) {
        QString MixerName;
        MixerName.append(md->name);
        if (!MixerName.isEmpty()) {
          str+=QString("(%1)").arg(MixerName);
        }
      }
      qba.clear();
      qba.append((quint8)i);
      qba.append((const char*)md, sizeof(MixData));
      QListWidgetItem *itm = new QListWidgetItem(str);
      itm->setData(Qt::UserRole,qba);  // mix number
      MixerlistWidget->addItem(itm);//(str);
    }

    while(curDest<outputs+GetEepromInterface()->getCapability(ExtraChannels)) {
      curDest++;
      QString str;

      if (curDest > outputs) {
        str = tr("X%1  ").arg(curDest-outputs);
      } else {
        str = tr("CH%1%2").arg(curDest/10).arg(curDest%10);
        if (GetEepromInterface()->getCapability(HasChNames) && showNames) {
          QString name=g_model.limitData[curDest-1].name;
          if (!name.isEmpty()) {
            name.append("     ");
            str=name.left(6);
          }
        }
      }
      qba.clear();
      qba.append((quint8)-curDest);
      QListWidgetItem *itm = new QListWidgetItem(str);
      itm->setData(Qt::UserRole,qba); // add new mixer
      MixerlistWidget->addItem(itm);
    }
}

void ModelEdit::mixesEdited()
{
    updateSettings();
}


void ModelEdit::tabHeli()
{
    updateHeliTab();

    connect(ui->swashTypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(heliEdited()));
    connect(ui->swashCollectiveCB,SIGNAL(currentIndexChanged(int)),this,SLOT(heliEdited()));
    connect(ui->swashRingValSB,SIGNAL(editingFinished()),this,SLOT(heliEdited()));
    connect(ui->swashInvertELE,SIGNAL(stateChanged(int)),this,SLOT(heliEdited()));
    connect(ui->swashInvertAIL,SIGNAL(stateChanged(int)),this,SLOT(heliEdited()));
    connect(ui->swashInvertCOL,SIGNAL(stateChanged(int)),this,SLOT(heliEdited()));
}

void ModelEdit::updateHeliTab()
{
    heliEditLock = true;

    ui->swashTypeCB->setCurrentIndex(g_model.swashRingData.type);
    populateSourceCB(ui->swashCollectiveCB, g_model.swashRingData.collectiveSource, POPULATE_SOURCES | POPULATE_SWITCHES | POPULATE_TRIMS);
    ui->swashRingValSB->setValue(g_model.swashRingData.value);
    ui->swashInvertELE->setChecked(g_model.swashRingData.invertELE);
    ui->swashInvertAIL->setChecked(g_model.swashRingData.invertAIL);
    ui->swashInvertCOL->setChecked(g_model.swashRingData.invertCOL);

    heliEditLock = false;
}

void ModelEdit::heliEdited()
{
    if(heliEditLock) return;
    g_model.swashRingData.type  = ui->swashTypeCB->currentIndex();
    g_model.swashRingData.collectiveSource = ui->swashCollectiveCB->itemData(ui->swashCollectiveCB->currentIndex()).toInt();
    g_model.swashRingData.value = ui->swashRingValSB->value();
    g_model.swashRingData.invertELE = ui->swashInvertELE->isChecked();
    g_model.swashRingData.invertAIL = ui->swashInvertAIL->isChecked();
    g_model.swashRingData.invertCOL = ui->swashInvertCOL->isChecked();
    updateSettings();
}

void ModelEdit::tabLimits()
{
    limitEditLock=true;
    int chnames=GetEepromInterface()->getCapability(HasChNames);
    QRegExp rx(CHAR_FOR_NAMES_REGEX);
    foreach(QLineEdit *le, findChildren<QLineEdit *>(QRegExp("CHName_[0-9]+"))) {
      int len=le->objectName().mid(le->objectName().lastIndexOf("_")+1).toInt()-1;
      if (chnames ) {
        QString name=g_model.limitData[len].name;
        if (name.trimmed().isEmpty()) {
          le->setText(tr("CH %1").arg(len+1));
        } else {
          le->setText(name);
        }
        le->setValidator(new QRegExpValidator(rx, this));
        le->setEnabled(true);
        le->setReadOnly(false);
        connect(le, SIGNAL(editingFinished()), this, SLOT(limitNameEdited()));
      } else {
        le->setText(tr("CH %1").arg(len+1));
        le->setDisabled(true);
        le->setReadOnly(true);
      }
    }
    limitEditLock=false;
    foreach(QDoubleSpinBox *sb, findChildren<QDoubleSpinBox *>(QRegExp("offsetDSB_[0-9]+"))) {
      int sbn=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
      sb->setValue(g_model.limitData[sbn].offset/10.0);
      connect(sb, SIGNAL(editingFinished()), this, SLOT(limitOffsetEdited()));
    }

    foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("minSB_[0-9]+"))) {
      int sbn=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
      sb->setValue(g_model.limitData[sbn].min);
      connect(sb, SIGNAL(editingFinished()), this, SLOT(limitEdited()));
    }

    foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("maxSB_[0-9]+"))) {
      int sbn=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
      sb->setValue(g_model.limitData[sbn].max);
      connect(sb, SIGNAL(editingFinished()), this, SLOT(limitEdited()));
    }

    foreach(QComboBox *cb, findChildren<QComboBox *>(QRegExp("chInvCB_[0-9]+"))) {
      int cbn=cb->objectName().mid(cb->objectName().lastIndexOf("_")+1).toInt()-1;
      cb->setCurrentIndex((g_model.limitData[cbn].revert) ? 1 : 0);
      connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(limitInvEdited()));
    }
    if (GetEepromInterface()->getCapability(PPMCenter)) {
      foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("ppmcenter_[0-9]+"))) {
        int sbn=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
        sb->setValue(g_model.limitData[sbn].ppmCenter+1500);
        connect(sb, SIGNAL(editingFinished()), this, SLOT(ppmcenterEdited()));
      }
    } else {
      foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("ppmcenter_[0-9]+"))) {
        sb->hide();
      }
      ui->ppmc_label1->hide();
      ui->ppmc_label2->hide();
    }

    if (GetEepromInterface()->getCapability(SYMLimits)) {
      foreach(QCheckBox *ckb, findChildren<QCheckBox *>(QRegExp("symckb_[0-9]+"))) {
        int ckbn=ckb->objectName().mid(ckb->objectName().lastIndexOf("_")+1).toInt()-1;
        ckb->setChecked(g_model.limitData[ckbn].symetrical);
        connect(ckb, SIGNAL(toggled(bool)), this, SLOT(limitSymEdited()));
      }
    } else {
      foreach(QCheckBox *ckb, findChildren<QCheckBox *>(QRegExp("symckb_[0-9]+"))) {
        ckb->hide();
      }
      ui->label_sym01->hide();
      ui->label_sym02->hide();
    }

    if (GetEepromInterface()->getCapability(Outputs)<17) {
      ui->limitGB2->hide();
    }
    setLimitMinMax();
}

void ModelEdit::updateCurvesTab()
{
    ControlCurveSignal(true);
    foreach(QCheckBox *ChkB, findChildren<QCheckBox *>(QRegExp("plotCB_[0-9]+"))) {
      int ChkBn=ChkB->objectName().mid(ChkB->objectName().lastIndexOf("_")+1).toInt()-1;
      ChkB->setChecked(plot_curve[ChkBn]);
    }
    ControlCurveSignal(false);
}


void ModelEdit::tabCurves()
{
    QSpinBox* tmpy[17] = {
      ui->curvePt01_y, ui->curvePt02_y, ui->curvePt03_y, ui->curvePt04_y, ui->curvePt05_y,
      ui->curvePt06_y, ui->curvePt07_y, ui->curvePt08_y, ui->curvePt09_y, ui->curvePt10_y,
      ui->curvePt11_y, ui->curvePt12_y, ui->curvePt13_y, ui->curvePt14_y, ui->curvePt15_y,
      ui->curvePt16_y, ui->curvePt17_y };
    QSpinBox* tmpx[17] = {
      ui->curvePt01_x, ui->curvePt02_x, ui->curvePt03_x, ui->curvePt04_x, ui->curvePt05_x,
      ui->curvePt06_x, ui->curvePt07_x, ui->curvePt08_x, ui->curvePt09_x, ui->curvePt10_x,
      ui->curvePt11_x, ui->curvePt12_x, ui->curvePt13_x, ui->curvePt14_x, ui->curvePt15_x,
      ui->curvePt16_x, ui->curvePt17_x };

    QPushButton * editb[16]= {
      ui->curveEdit_1,ui->curveEdit_2,ui->curveEdit_3,ui->curveEdit_4,
      ui->curveEdit_5,ui->curveEdit_6,ui->curveEdit_7,ui->curveEdit_8,
      ui->curveEdit_9,ui->curveEdit_10,ui->curveEdit_11,ui->curveEdit_12,
      ui->curveEdit_13,ui->curveEdit_14,ui->curveEdit_15,ui->curveEdit_16 };

    QPushButton * resetb[16]= {
      ui->resetCurve_1,ui->resetCurve_2,ui->resetCurve_3,ui->resetCurve_4,
      ui->resetCurve_5,ui->resetCurve_6,ui->resetCurve_7,ui->resetCurve_8,
      ui->resetCurve_9,ui->resetCurve_10,ui->resetCurve_11,ui->resetCurve_12,
      ui->resetCurve_13,ui->resetCurve_14,ui->resetCurve_15,ui->resetCurve_16 };

    QCheckBox * plotcb[16]= {
      ui->plotCB_1,ui->plotCB_2,ui->plotCB_3,ui->plotCB_4,
      ui->plotCB_5,ui->plotCB_6,ui->plotCB_7,ui->plotCB_8,
      ui->plotCB_9,ui->plotCB_10,ui->plotCB_11,ui->plotCB_12,
      ui->plotCB_13,ui->plotCB_14,ui->plotCB_15,ui->plotCB_16 };

    memcpy(spny, tmpy, sizeof(spny));
    memcpy(spnx, tmpx, sizeof(spnx));
    int numcurves=GetEepromInterface()->getCapability(NumCurves);
    if (numcurves==0) {
      numcurves=16;
    }

    if (!GetEepromInterface()->getCapability(CustomCurves)){
      ui->curvetype_CB->setDisabled(true);
      int count=0;
      for (int i=0; i< GetEepromInterface()->getCapability(NumCurves3); i++) {
          g_model.curves[count].count=3;
          g_model.curves[count].custom=false;
          count++;
      }
      for (int i=0; i< GetEepromInterface()->getCapability(NumCurves5); i++) {
          g_model.curves[count].count=5;
          g_model.curves[count].custom=false;
          count++;
      }
      for (int i=0; i< GetEepromInterface()->getCapability(NumCurves9); i++) {
          g_model.curves[count].count=9;
          g_model.curves[count].custom=false;
          count++;
      }
      for (int i=count; i< 16; i++) {
          editb[i]->hide();
          plotcb[i]->hide();
          resetb[i]->hide();
      }

    } else {
      ui->curvetype_CB->setEnabled(true);
    }
    if (!GetEepromInterface()->getCapability(HasCvNames)){
      ui->cname_LE->hide();
      ui->cname_label->hide();
    } else {    
      ui->cname_LE->setText(g_model.curves[0].name);
    }

    for (int i=numcurves; i<16;i++) {
      editb[i]->hide();
      resetb[i]->hide();
      plotcb[i]->hide();
    }


    for (int i=0; i<16;i++) {
      plot_curve[i]=FALSE;
    }
    redrawCurve=true;
    drawing=false;
#ifdef __APPLE__
    ui->curveEdit_1->setStyleSheet("color: #00007f;");
    ui->curveEdit_2->setStyleSheet("color: #007f00;");
    ui->curveEdit_3->setStyleSheet("color: #7f0000;");
    ui->curveEdit_4->setStyleSheet("color: #007f7f;");
    ui->curveEdit_5->setStyleSheet("color: #7f007f;");
    ui->curveEdit_6->setStyleSheet("color: #7f7f00;");
    ui->curveEdit_7->setStyleSheet("color: #7f7f7f;");
    ui->curveEdit_8->setStyleSheet("color: #0000ff;");
    ui->curveEdit_9->setStyleSheet("color: #007fff;");
    ui->curveEdit_10->setStyleSheet("color: #7f00ff;");
    ui->curveEdit_11->setStyleSheet("color: #00ff00;");
    ui->curveEdit_12->setStyleSheet("color: #00ff7f;");
    ui->curveEdit_13->setStyleSheet("color: #7fff00;");
    ui->curveEdit_14->setStyleSheet("color: #ff0000;");
    ui->curveEdit_15->setStyleSheet("color: #ff007f;");
    ui->curveEdit_16->setStyleSheet("color: #ff7f00;");
#else  
    ui->curveEdit_1->setStyleSheet("background-color: #00007f; color: white;");
    ui->curveEdit_2->setStyleSheet("background-color: #007f00; color: white;");
    ui->curveEdit_3->setStyleSheet("background-color: #7f0000; color: white;");
    ui->curveEdit_4->setStyleSheet("background-color: #007f7f; color: white;");
    ui->curveEdit_5->setStyleSheet("background-color: #7f007f; color: white;");
    ui->curveEdit_6->setStyleSheet("background-color: #7f7f00; color: white;");
    ui->curveEdit_7->setStyleSheet("background-color: #7f7f7f; color: white;");
    ui->curveEdit_8->setStyleSheet("background-color: #0000ff; color: white;");
    ui->curveEdit_9->setStyleSheet("background-color: #007fff; color: white;");
    ui->curveEdit_10->setStyleSheet("background-color: #7f00ff; color: white;");
    ui->curveEdit_11->setStyleSheet("background-color: #00ff00; color: white;");
    ui->curveEdit_12->setStyleSheet("background-color: #00ff7f; color: white;");
    ui->curveEdit_13->setStyleSheet("background-color: #7fff00; color: white;");
    ui->curveEdit_14->setStyleSheet("background-color: #ff0000; color: white;");
    ui->curveEdit_15->setStyleSheet("background-color: #ff007f; color: white;");
    ui->curveEdit_16->setStyleSheet("background-color: #ff7f00; color: white;");
#endif
    updateCurvesTab();

    QGraphicsScene *scene = new QGraphicsScene(ui->curvePreview);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    ui->curvePreview->setScene(scene);
    currentCurve = 0;

    connect(ui->clearMixesPB,SIGNAL(pressed()),this,SLOT(clearCurves()));

    foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("curvePt[0-9]+"))) {
      connect(sb, SIGNAL(valueChanged(int)), this, SLOT(curvePointEdited()));
    }

    foreach(QPushButton *pb, findChildren<QPushButton *>(QRegExp("resetCurve_[0-9]+"))) {
      connect(pb, SIGNAL(clicked()), this, SLOT(resetCurve()));
    }

    foreach(QPushButton *pb, findChildren<QPushButton *>(QRegExp("curveEdit_[0-9]+"))) {
      connect(pb, SIGNAL(clicked()), this, SLOT(editCurve()));
    }

    foreach(QCheckBox *ChkB, findChildren<QCheckBox *>(QRegExp("plotCB_[0-9]+"))) {
      connect(ChkB, SIGNAL(toggled(bool)), this, SLOT(plotCurve(bool)));
    }
    setCurrentCurve(currentCurve);
    ui->ca_coeff_SB->hide();
    ui->ca_coeff_label->hide();
    ui->ca_ymid_SB->hide();
    ui->ca_ymid_label->hide();
}

void ModelEdit::on_ca_ctype_CB_currentIndexChanged()
{
    int index=ui->ca_ctype_CB->currentIndex();
    switch (index) {
      case 0:
        ui->ca_coeff_SB->hide();
        ui->ca_coeff_label->hide();
        ui->ca_ymid_SB->hide();
        ui->ca_ymid_label->hide();
        ui->ca_ymin_SB->show();
        ui->ca_ymin_label->show();
        ui->ca_ymin_SB->setValue(-100);
        ui->ca_ymax_SB->setValue(100);
        break;
      case 1:
        ui->ca_coeff_SB->show();
        ui->ca_coeff_label->show();
        ui->ca_ymid_SB->hide();
        ui->ca_ymid_label->hide();
        ui->ca_ymin_SB->show();
        ui->ca_ymin_label->show();
        ui->ca_ymin_SB->setValue(-100);
        ui->ca_ymax_SB->setValue(100);
        break;
      case 2:
        ui->ca_coeff_SB->show();
        ui->ca_coeff_label->show();
        ui->ca_ymid_SB->hide();
        ui->ca_ymid_label->hide();
        ui->ca_ymin_SB->hide();
        ui->ca_ymin_label->hide();
        ui->ca_ymax_SB->setValue(100);      
        break;
      case 3:
        ui->ca_coeff_SB->show();
        ui->ca_coeff_label->show();
        ui->ca_ymid_SB->show();
        ui->ca_ymid_label->show();
        ui->ca_ymin_SB->hide();
        ui->ca_ymin_label->hide();
        ui->ca_ymid_SB->setValue(0);
        ui->ca_ymax_SB->setValue(100);      
        break;
    }
}

void ModelEdit::on_ca_apply_PB_clicked()
{
    int index=ui->ca_ctype_CB->currentIndex();
    float x;
    int y;
    int invert=0;
    float a;
    if (index==0) {
      a=(ui->ca_ymax_SB->value()-ui->ca_ymin_SB->value())/200.0;
      int numpoints=g_model.curves[currentCurve].count;
      for (int i=0; i<numpoints; i++) {
        if (g_model.curves[currentCurve].custom) {
          x=(g_model.curves[currentCurve].points[i].x+100);
        } else {
          x=(200.0/(numpoints-1))*i;
        }
        y=ui->ca_ymin_SB->value()+a*x;
        switch (ui->ca_side_CB->currentIndex()) {
          case 0:
            g_model.curves[currentCurve].points[i].y=y;
            break;
          case 1:
            if (x>=100) {
              g_model.curves[currentCurve].points[i].y=y;
            }
            break;
          case 2:
            if (x<100) {
              g_model.curves[currentCurve].points[i].y=y;
            }
            break;
        }
      }
    } else if (index==1) {
      int numpoints=g_model.curves[currentCurve].count;
      for (int i=0; i<numpoints; i++) {
        if (g_model.curves[currentCurve].custom) {
          x=((g_model.curves[currentCurve].points[i].x)+100)/2.0;
        } else {
          x=(100.0/(numpoints-1))*i;
        }
        a=ui->ca_coeff_SB->value();
        if (a>=0) {
          y=round(c9xexpou(x,a)*(ui->ca_ymax_SB->value()-ui->ca_ymin_SB->value())/100.0+ui->ca_ymin_SB->value());
        } else {
          a=-a;
          x=100-x;
          y=round((100.0-c9xexpou(x,a))*(ui->ca_ymax_SB->value()-ui->ca_ymin_SB->value())/100.0+ui->ca_ymin_SB->value());
        }
        switch (ui->ca_side_CB->currentIndex()) {
          case 0:
            g_model.curves[currentCurve].points[i].y=y;
            break;
          case 1:
            if (x>=50) {
              g_model.curves[currentCurve].points[i].y=y;
            }
            break;
          case 2:
            if (x<50) {
              g_model.curves[currentCurve].points[i].y=y;
            }
            break;
        }
      }
    } else if (index==2) {
      int numpoints=g_model.curves[currentCurve].count;
      for (int i=0; i<numpoints; i++) {
        if (g_model.curves[currentCurve].custom) {
          x=(g_model.curves[currentCurve].points[i].x);
        } else {
          x=-100.0+(200.0/(numpoints-1))*i;
        }
        a=ui->ca_coeff_SB->value();
        if (x<0) {
          x=-x;
          invert=1; 
        } else {
          invert=0;
        }
        if (a>=0) {
          y=round(c9xexpou(x,a)*(ui->ca_ymax_SB->value()/100.0));
        } else {
          a=-a;
          x=100-x;
          y=round((100.0-c9xexpou(x,a))*(ui->ca_ymax_SB->value()/100.0));
        }
        switch (ui->ca_side_CB->currentIndex()) {
          case 0:
            if (invert==1) {
              g_model.curves[currentCurve].points[i].y=-y;
            } else {
              g_model.curves[currentCurve].points[i].y=y;
            }
            break;
          case 1:
            if (invert==0) {
              g_model.curves[currentCurve].points[i].y=y;
            }
            break;
          case 2:
            if (invert==1) {
              g_model.curves[currentCurve].points[i].y=-y;
            }
            break;
        }      
      }
    } else if (index==3) {
      int numpoints=g_model.curves[currentCurve].count;
      for (int i=0; i<numpoints; i++) {
        if (g_model.curves[currentCurve].custom) {
          x=(g_model.curves[currentCurve].points[i].x);
        } else {
          x=-100.0+(200.0/(numpoints-1))*i;
        }
        int pos=(x>=0);
        a=ui->ca_coeff_SB->value();
        if (x<0) {
          x=-x;
        }
        if (a>=0) {
          y=round(c9xexpou(x,a)*((ui->ca_ymax_SB->value()-ui->ca_ymid_SB->value())/100.0)+ui->ca_ymid_SB->value());
        } else {
          a=-a;
          x=100-x;
          y=round((100.0-c9xexpou(x,a))*((ui->ca_ymax_SB->value()-ui->ca_ymid_SB->value())/100.0)+ui->ca_ymid_SB->value());
        }
        switch (ui->ca_side_CB->currentIndex()) {
          case 0:
            g_model.curves[currentCurve].points[i].y=y;
            break;
          case 1:
            if (pos) {
              g_model.curves[currentCurve].points[i].y=y;
            }
            break;
          case 2:
            if (!pos) {
              g_model.curves[currentCurve].points[i].y=y;
            }
            break;
        }
      }
    }  
    updateSettings();
    setCurrentCurve(currentCurve);
    drawCurve();
}

void ModelEdit::limitSymEdited()
{
    QCheckBox *ckb = qobject_cast<QCheckBox*>(sender());
    int limitId=ckb->objectName().mid(ckb->objectName().lastIndexOf("_")+1).toInt()-1;
    g_model.limitData[limitId].symetrical = (ckb->checkState() ? 1 : 0);
    updateSettings(); 
}

void ModelEdit::limitNameEdited()
{
    if(limitEditLock) return;
    limitEditLock=true;
    QLineEdit *le = qobject_cast<QLineEdit*>(sender());
    int limitId=le->objectName().mid(le->objectName().lastIndexOf("_")+1).toInt()-1;
    int i=0;
    if (le->text()==tr("CH %1").arg(limitId+1)) {
      le->setText("");
    }
    for (i=0; i<le->text().toAscii().length(); i++) {
      g_model.limitData[limitId].name[i]=le->text().toAscii().at(i);
    }
    if (le->text().trimmed().isEmpty()) {
      le->setText(tr("CH %1").arg(limitId+1));
    }
    g_model.limitData[limitId].name[i]=0;
    updateSettings();
    tabMixes();
    limitEditLock=false;
}


void ModelEdit::limitOffsetEdited()
{
    QDoubleSpinBox *dsb = qobject_cast<QDoubleSpinBox*>(sender());
    int limitId=dsb->objectName().mid(dsb->objectName().lastIndexOf("_")+1).toInt()-1;
    g_model.limitData[limitId].offset = round(dsb->value()*10);
    updateSettings(); 
}

void ModelEdit::limitEdited()
{
    QSpinBox *sb = qobject_cast<QSpinBox*>(sender());
    int limitId=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
    if (sb->objectName().indexOf("max")!=-1) {
      g_model.limitData[limitId].max = sb->value();
    }
    else {
      g_model.limitData[limitId].min = sb->value();
    }
    updateSettings();
}

void ModelEdit::limitInvEdited()
{
    QComboBox *cb = qobject_cast<QComboBox*>(sender());
    int limitId=cb->objectName().mid(cb->objectName().lastIndexOf("_")+1).toInt()-1;
    g_model.limitData[limitId].revert = cb->currentIndex();
    updateSettings();
}

void ModelEdit::ppmcenterEdited()
{
    QSpinBox *sb = qobject_cast<QSpinBox*>(sender());
    int limitId=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
    g_model.limitData[limitId].ppmCenter = sb->value()-1500;
    updateSettings();
}

void ModelEdit::startupSwitchEdited()
{
    if (GetEepromInterface()->getCapability(Pots)==3) {
      uint8_t i= 0;
      i|=(uint8_t)ui->swwarn0_CB->currentIndex();
      if (i==1) {
        ui->swwarn1_ChkB->setDisabled(true) ;
        ui->swwarn2_ChkB->setDisabled(true) ;
        ui->swwarn3_ChkB->setDisabled(true) ;
        ui->swwarn4_CB->setDisabled(true) ;
        ui->swwarn5_ChkB->setDisabled(true) ;
        ui->swwarn6_ChkB->setDisabled(true) ;
      } else {
        ui->swwarn1_ChkB->setEnabled(true) ;
        ui->swwarn2_ChkB->setEnabled(true) ;
        ui->swwarn3_ChkB->setEnabled(true) ;
        ui->swwarn4_CB->setEnabled(true) ;
        ui->swwarn5_ChkB->setEnabled(true) ;
        ui->swwarn6_ChkB->setEnabled(true) ;
        i|=(ui->swwarn1_ChkB->isChecked() ? 1 : 0)<<1;
        i|=(ui->swwarn2_ChkB->isChecked() ? 1 : 0)<<2;
        i|=(ui->swwarn3_ChkB->isChecked() ? 1 : 0)<<3;
        i|=((uint8_t)ui->swwarn4_CB->currentIndex() & 0x03)<<4;
        i|=(ui->swwarn5_ChkB->isChecked() ? 1 : 0)<<6;
        i|=(ui->swwarn6_ChkB->isChecked() ? 1 : 0)<<7;
      }
      g_model.switchWarningStates=i;
    } else {
      uint16_t i= 0;
      i|=(uint16_t)ui->tswwarn0_CB->currentIndex();
      if (i==1) {
        ui->chkSA->setDisabled(true);
        ui->chkSB->setDisabled(true);
        ui->chkSC->setDisabled(true);
        ui->chkSD->setDisabled(true);
        ui->chkSE->setDisabled(true);
        ui->chkSF->setDisabled(true);
        ui->chkSG->setDisabled(true);
      } else {
        ui->chkSA->setEnabled(true);
        ui->chkSB->setEnabled(true);
        ui->chkSC->setEnabled(true);
        ui->chkSD->setEnabled(true);
        ui->chkSE->setEnabled(true);
        ui->chkSF->setEnabled(true);
        ui->chkSG->setEnabled(true);
        i|=(((uint16_t)ui->chkSA->value())<<1);
        i|=(((uint16_t)ui->chkSB->value())<<3);
        i|=(((uint16_t)ui->chkSC->value())<<5);
        i|=(((uint16_t)ui->chkSD->value())<<7);
        i|=(((uint16_t)ui->chkSE->value())<<9);
        i|=(((uint16_t)ui->chkSF->value())<<12);
        i|=(((uint16_t)ui->chkSG->value())<<13);
      }
      g_model.switchWarningStates=i;
    }
    updateSettings();
}

void ModelEdit::setCurrentCurve(int curveId)
{
    currentCurve = curveId;
    curvesLock=true;
    QString ss = "QSpinBox { background-color:rgb(255, 255, 127);}";
    for (int i=0; i<g_model.curves[currentCurve].count;i++) {
      spny[i]->show();
      spny[i]->setValue(g_model.curves[currentCurve].points[i].y);
      if (!g_model.curves[currentCurve].custom || i==0 || i==(g_model.curves[currentCurve].count-1)) {
        spnx[i]->hide();
        if (i==0) {
            spnx[i]->setValue(-100);
        }
        if (i==(g_model.curves[currentCurve].count-1)) {
            spnx[i]->setValue(100);
        }
      } else {
        spnx[i]->show();
        spnx[i]->setMaximum(g_model.curves[currentCurve].points[i+1].x-1);
        spnx[i]->setMinimum(g_model.curves[currentCurve].points[i-1].x+1);
        spnx[i]->setValue(g_model.curves[currentCurve].points[i].x);
      }
    }
    for (int i=g_model.curves[currentCurve].count; i<17;i++) {
      spny[i]->hide();
      spnx[i]->hide();
    }
    int index=0;
    if (g_model.curves[currentCurve].count==3) {
      index=0;
    } else if (g_model.curves[currentCurve].count==5) {
      index=2;
    } else if (g_model.curves[currentCurve].count==9) {
      index=4;
    }  else if (g_model.curves[currentCurve].count==17) {
      index=6;
    }
    if (g_model.curves[currentCurve].custom) {
      index++;
    }
    if (!GetEepromInterface()->getCapability(HasCvNames)){
      ui->cname_LE->hide();
      ui->cname_label->hide();
    } else {
      ui->cname_LE->setText(g_model.curves[currentCurve].name);
    }

    ui->curvetype_CB->setCurrentIndex(index);
    curvesLock=false;
}

void ModelEdit::curvePointEdited()
{
    if (curvesLock) {
      return;
    }
  /*  for (int i=1; i< g_model.curves[currentCurve].count-1; i++) {
      if (g_model.curves[currentCurve].points[i].x>(100-g_model.curves[currentCurve].count+i)) {
        g_model.curves[currentCurve].points[i].x=(100-g_model.curves[currentCurve].count+i);
      }
      if (g_model.curves[currentCurve].points[i].x<=g_model.curves[currentCurve].points[i-1].x) {
        spnx[i]->setValue(spnx[i-1]->value()+1);
      }
    }
  */ 
    for (int i=0; i< 17; i++) {
      g_model.curves[currentCurve].points[i].x=spnx[i]->value();
      g_model.curves[currentCurve].points[i].y=spny[i]->value();
    }
    if (redrawCurve)
      drawCurve();
    updateSettings();
}


void ModelEdit::setSwitchWidgetVisibility(int i)
{
    RawSource source=RawSource(g_model.customSw[i].val1);
    switch (getCSFunctionFamily(g_model.customSw[i].func))
    {
      case CS_FAMILY_VOFS:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(false);
        cswitchValue[i]->setVisible(false);
        cswitchOffset[i]->setVisible(true);
        populateSourceCB(cswitchSource1[i], source, POPULATE_SOURCES | (GetEepromInterface()->getCapability(ExtraTrims) ? POPULATE_TRIMS : 0) | POPULATE_SWITCHES | POPULATE_TELEMETRY | (GetEepromInterface()->getCapability(GvarsInCS) ? POPULATE_GVARS : 0));
        cswitchOffset[i]->setDecimals(source.getDecimals(g_model));
        cswitchOffset[i]->setSingleStep(source.getStep(g_model));
        if (g_model.customSw[i].func>CS_FN_ELESS && g_model.customSw[i].func<CS_FN_VEQUAL) {
          cswitchOffset[i]->setMinimum(source.getStep(g_model)*-127);
          cswitchOffset[i]->setMaximum(source.getStep(g_model)*127);
          cswitchOffset[i]->setValue(source.getStep(g_model)*g_model.customSw[i].val2);
        } else {
          cswitchOffset[i]->setMinimum(source.getMin(g_model));
          cswitchOffset[i]->setMaximum(source.getMax(g_model));
          cswitchOffset[i]->setValue(source.getStep(g_model)*(g_model.customSw[i].val2+source.getRawOffset(g_model))+source.getOffset(g_model));
        }
        break;
      case CS_FAMILY_VBOOL:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(true);
        cswitchValue[i]->setVisible(false);
        cswitchOffset[i]->setVisible(false);
        populateSwitchCB(cswitchSource1[i], RawSwitch(g_model.customSw[i].val1));
        populateSwitchCB(cswitchSource2[i], RawSwitch(g_model.customSw[i].val2));
        break;
      case CS_FAMILY_VCOMP:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(true);
        cswitchValue[i]->setVisible(false);
        cswitchOffset[i]->setVisible(false);
        populateSourceCB(cswitchSource1[i], RawSource(g_model.customSw[i].val1), POPULATE_SOURCES | (GetEepromInterface()->getCapability(ExtraTrims) ? POPULATE_TRIMS : 0) | POPULATE_SWITCHES | POPULATE_TELEMETRY | (GetEepromInterface()->getCapability(GvarsInCS) ? POPULATE_GVARS : 0));
        populateSourceCB(cswitchSource2[i], RawSource(g_model.customSw[i].val2), POPULATE_SOURCES | (GetEepromInterface()->getCapability(ExtraTrims) ? POPULATE_TRIMS : 0) | POPULATE_SWITCHES | POPULATE_TELEMETRY | (GetEepromInterface()->getCapability(GvarsInCS) ? POPULATE_GVARS : 0));
        break;
      case CS_FAMILY_TIMERS:
        cswitchSource1[i]->setVisible(false);
        cswitchSource2[i]->setVisible(false);
        cswitchValue[i]->setVisible(true);
        cswitchOffset[i]->setVisible(true);
        cswitchOffset[i]->setDecimals(1);
        cswitchOffset[i]->setMinimum(0.1);
        cswitchOffset[i]->setMaximum(175);
        float value=ValToTim(g_model.customSw[i].val2);
        cswitchOffset[i]->setSingleStep(0.1);
        if (value>60) {
           cswitchOffset[i]->setSingleStep(1);
        } else if (value>2) {
          cswitchOffset[i]->setSingleStep(0.5);
        }
        cswitchOffset[i]->setValue(value);
        
        cswitchValue[i]->setDecimals(1);
        cswitchValue[i]->setMinimum(0.1);
        cswitchValue[i]->setMaximum(175);
        value=ValToTim(g_model.customSw[i].val1);
        cswitchValue[i]->setSingleStep(0.1);
        if (value>60) {
           cswitchValue[i]->setSingleStep(1);
        } else if (value>2) {
          cswitchValue[i]->setSingleStep(0.5);
        }
        cswitchValue[i]->setValue(value);
        break;
    }
    cswitchAnd[i]->setVisible(true);
    populateSwitchCB(cswitchAnd[i], RawSwitch(g_model.customSw[i].andsw), POPULATE_AND_SWITCHES);
    if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
      cswitchDuration[i]->setVisible(true);
      cswitchDuration[i]->setValue(g_model.customSw[i].duration/2.0);
      cswitchDelay[i]->setVisible(true);
      cswitchDelay[i]->setValue(g_model.customSw[i].delay/2.0);
    }
}

void ModelEdit::updateSwitchesTab()
{
    switchEditLock = true;
    int num_csw=GetEepromInterface()->getCapability(CustomSwitches);
    for (int i=0; i< num_csw; i++) {
      populateCSWCB(csw[i], g_model.customSw[i].func);
      setSwitchWidgetVisibility(i);
    }
    switchEditLock = false;
}

void ModelEdit::tabCustomSwitches()
{
    switchEditLock = true;
    QComboBox* tmpcsw[C9X_NUM_CSW] = {ui->cswitchFunc_1, ui->cswitchFunc_2, ui->cswitchFunc_3, ui->cswitchFunc_4,
      ui->cswitchFunc_5, ui->cswitchFunc_6, ui->cswitchFunc_7, ui->cswitchFunc_8,
      ui->cswitchFunc_9, ui->cswitchFunc_10, ui->cswitchFunc_11, ui->cswitchFunc_12,
      ui->cswitchFunc_13, ui->cswitchFunc_14, ui->cswitchFunc_15, ui->cswitchFunc_16,
      ui->cswitchFunc_17, ui->cswitchFunc_18, ui->cswitchFunc_19, ui->cswitchFunc_20,
      ui->cswitchFunc_21, ui->cswitchFunc_22, ui->cswitchFunc_23, ui->cswitchFunc_24,
      ui->cswitchFunc_25, ui->cswitchFunc_26, ui->cswitchFunc_27, ui->cswitchFunc_28,
      ui->cswitchFunc_29, ui->cswitchFunc_30, ui->cswitchFunc_31, ui->cswitchFunc_32 };
    QLabel* cswlabel[C9X_NUM_CSW] = { ui->cswlabel_01, ui->cswlabel_02, ui->cswlabel_03, ui->cswlabel_04,
      ui->cswlabel_05, ui->cswlabel_06, ui->cswlabel_07, ui->cswlabel_08,
      ui->cswlabel_09, ui->cswlabel_10, ui->cswlabel_11, ui->cswlabel_12,
      ui->cswlabel_13, ui->cswlabel_14, ui->cswlabel_15, ui->cswlabel_16,
      ui->cswlabel_17, ui->cswlabel_18, ui->cswlabel_19, ui->cswlabel_20,
      ui->cswlabel_21, ui->cswlabel_22, ui->cswlabel_23, ui->cswlabel_24, 
      ui->cswlabel_25, ui->cswlabel_26, ui->cswlabel_27, ui->cswlabel_28, 
      ui->cswlabel_29, ui->cswlabel_30, ui->cswlabel_31, ui->cswlabel_32};
    
    memcpy(csw, tmpcsw, sizeof(csw));
    int num_csw=GetEepromInterface()->getCapability(CustomSwitches);
    for(int i=0; i<16; i++) {
      if (i<num_csw) {
        tmpcsw[i]->setProperty("index",i);
        cswitchSource1[i] = new QComboBox(this);
        cswitchSource1[i]->setProperty("index",i);
        connect(cswitchSource1[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
        ui->gridLayout_21->addWidget(cswitchSource1[i],i+1,2);
        cswitchSource1[i]->setVisible(false);
        cswitchValue[i] = new QDoubleSpinBox(this);
        cswitchValue[i]->setMaximum(125);
        cswitchValue[i]->setMinimum(-125);
        cswitchValue[i]->setAccelerated(true);
        cswitchValue[i]->setDecimals(0);
        cswitchValue[i]->setProperty("index",i);
        connect(cswitchValue[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
        ui->gridLayout_21->addWidget(cswitchValue[i],i+1,2);
        cswitchValue[i]->setVisible(false);

        cswitchSource2[i] = new QComboBox(this);
        cswitchSource2[i]->setProperty("index",i);
        connect(cswitchSource2[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
        ui->gridLayout_21->addWidget(cswitchSource2[i],i+1,3);
        cswitchSource2[i]->setVisible(false);

        cswitchOffset[i] = new QDoubleSpinBox(this);
        cswitchOffset[i]->setProperty("index",i);
        cswitchOffset[i]->setMaximum(125);
        cswitchOffset[i]->setMinimum(-125);
        cswitchOffset[i]->setAccelerated(true);
        cswitchOffset[i]->setDecimals(0);
        connect(cswitchOffset[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
        ui->gridLayout_21->addWidget(cswitchOffset[i],i+1,3);
        cswitchOffset[i]->setVisible(false);
        cswitchAnd[i] = new QComboBox(this);
        cswitchAnd[i]->setProperty("index",i);
        connect(cswitchAnd[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
        ui->gridLayout_21->addWidget(cswitchAnd[i],i+1,4);
        cswitchAnd[i]->setVisible(false);
        if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
          cswitchDuration[i] = new QDoubleSpinBox(this);
          cswitchDuration[i]->setProperty("index",i);
          cswitchDuration[i]->setSingleStep(0.5);
          cswitchDuration[i]->setMaximum(50);
          cswitchDuration[i]->setMinimum(0);
          cswitchDuration[i]->setAccelerated(true);
          cswitchDuration[i]->setDecimals(1);
          connect(cswitchDuration[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
          ui->gridLayout_21->addWidget(cswitchDuration[i],i+1,5);
          cswitchDuration[i]->setVisible(false);

          cswitchDelay[i] = new QDoubleSpinBox(this);
          cswitchDelay[i]->setProperty("index",i);
          cswitchDelay[i]->setSingleStep(0.5);
          cswitchDelay[i]->setMaximum(50);
          cswitchDelay[i]->setMinimum(0);
          cswitchDelay[i]->setAccelerated(true);
          cswitchDelay[i]->setDecimals(1);
          connect(cswitchDelay[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
          ui->gridLayout_21->addWidget(cswitchDelay[i],i+1,6);
          cswitchDelay[i]->setVisible(false);
        } else {
          ui->cswCol3->hide();
          ui->cswCol4->hide();
        }
        connect(cswlabel[i],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(csw_customContextMenuRequested(QPoint)));
      } else {
        csw[i]->hide();
        cswlabel[i]->hide();
      }
    }
    if (num_csw>16) {
      for(int i=16; i<32; i++) {
        if (i<=num_csw) {
          tmpcsw[i]->setProperty("index",i);
          cswitchSource1[i] = new QComboBox(this);
          cswitchSource1[i]->setProperty("index",i);
          connect(cswitchSource1[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
          ui->gridLayout_22->addWidget(cswitchSource1[i],i-15,2);
          cswitchSource1[i]->setVisible(false);
          cswitchValue[i] = new QDoubleSpinBox(this);
          cswitchValue[i]->setProperty("index",i);
          cswitchValue[i]->setMaximum(125);
          cswitchValue[i]->setMinimum(-125);
          cswitchValue[i]->setAccelerated(true);
          cswitchValue[i]->setDecimals(0);
          connect(cswitchValue[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
          ui->gridLayout_22->addWidget(cswitchValue[i],i-15,2);
          cswitchValue[i]->setVisible(false);

          cswitchSource2[i] = new QComboBox(this);
          cswitchSource2[i]->setProperty("index",i);
          connect(cswitchSource2[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
          ui->gridLayout_22->addWidget(cswitchSource2[i],i-15,3);
          cswitchSource2[i]->setVisible(false);

          cswitchOffset[i] = new QDoubleSpinBox(this);
          cswitchOffset[i]->setProperty("index",i);
          cswitchOffset[i]->setMaximum(125);
          cswitchOffset[i]->setMinimum(-125);
          cswitchOffset[i]->setAccelerated(true);
          cswitchOffset[i]->setDecimals(0);
          connect(cswitchOffset[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
          ui->gridLayout_22->addWidget(cswitchOffset[i],i-15,3);
          cswitchOffset[i]->setVisible(false);
          cswitchAnd[i] = new QComboBox(this);
          cswitchAnd[i]->setProperty("index",i);
          connect(cswitchAnd[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
          ui->gridLayout_22->addWidget(cswitchAnd[i],i-15,4);
          cswitchAnd[i]->setVisible(false);
          if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
            cswitchDuration[i] = new QDoubleSpinBox(this);
            cswitchDuration[i]->setProperty("index",i);
            cswitchDuration[i]->setSingleStep(0.5);
            cswitchDuration[i]->setMaximum(50);
            cswitchDuration[i]->setMinimum(0);
            cswitchDuration[i]->setAccelerated(true);
            cswitchDuration[i]->setDecimals(1);
            connect(cswitchDuration[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
            ui->gridLayout_22->addWidget(cswitchDuration[i],i-15,5);
            cswitchDuration[i]->setVisible(false);

            cswitchDelay[i] = new QDoubleSpinBox(this);
            cswitchDelay[i]->setProperty("index",i);
            cswitchDelay[i]->setSingleStep(0.5);
            cswitchDelay[i]->setMaximum(50);
            cswitchDelay[i]->setMinimum(0);
            cswitchDelay[i]->setAccelerated(true);
            cswitchDelay[i]->setDecimals(1);
            connect(cswitchDelay[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
            ui->gridLayout_22->addWidget(cswitchDelay[i],i-15,6);
            cswitchDelay[i]->setVisible(false);
          } else {
            ui->cswCol3_2->hide();
            ui->cswCol4_2->hide();
          }
          connect(cswlabel[i],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(csw_customContextMenuRequested(QPoint)));        
        } else {
          csw[i]->hide();
          cswlabel[i]->hide();
        }
      }
    } else {
      ui->cswitchGB2->hide();
    }
    updateSwitchesTab();
    //connects
    for (int i=0; i<num_csw; i++) {
      connect(csw[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    }
    switchEditLock = false;
}

void ModelEdit::tabCustomFunctions()
{
  switchEditLock = true;
  int num_fsw = GetEepromInterface()->getCapability(CustomFunctions);

  QStringList paramarmList;
  if (!GetEepromInterface()->getCapability(VoicesAsNumbers)) {
    for (int i=0; i<num_fsw; i++) {
      if (g_model.funcSw[i].func==FuncPlayPrompt || g_model.funcSw[i].func==FuncBackgroundMusic) {
        QString temp = g_model.funcSw[i].paramarm;
        if (!temp.isEmpty()) {
          if (!paramarmList.contains(temp)) {
            paramarmList.append(temp);
          }
        }
      }
    }

    QSettings settings("companion9x", "companion9x");
    QString path=settings.value("sdPath", ".").toString();
    path.append("/SOUNDS/");
    QString lang=radioData.generalSettings.ttsLanguage;
    if (lang.isEmpty())
      lang="en";
    path.append(lang);
    QDir qd(path);
    int vml= GetEepromInterface()->getCapability(VoicesMaxLength)+4;
    if (qd.exists()) {
      QStringList filters;
      filters << "*.wav" << "*.WAV";
      foreach ( QString file, qd.entryList(filters, QDir::Files) ) {
        QFileInfo fi(file);
        QString temp=fi.completeBaseName();
        if (!paramarmList.contains(temp) && temp.length()<=vml) {
          paramarmList.append(temp);
        }
      }
    }
  }

  for (int i=0; i<num_fsw; i++) {
    AssignFunc func = g_model.funcSw[i].func;
    QGridLayout *layout = i >= 16 ? ui->fswitchlayout2 : ui->fswitchlayout1;

    fswLabel[i] = new QLabel(this);
    fswLabel[i]->setFrameStyle(QFrame::Panel | QFrame::Raised);
    fswLabel[i]->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    fswLabel[i]->setContextMenuPolicy(Qt::CustomContextMenu);
    fswLabel[i]->setMouseTracking(true);
    fswLabel[i]->setProperty("FunctionId", i+1);
    fswLabel[i]->setText(tr("CF%1").arg(i+1));
    layout->addWidget(fswLabel[i],(i%16)+1,0);
    connect(fswLabel[i],SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(fsw_customContextMenuRequested(QPoint)));

    fswtchSwtch[i] = new QComboBox(this);
    fswtchSwtch[i]->setProperty("functionIndex", i);
    connect(fswtchSwtch[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customFunctionEdited()));
    layout->addWidget(fswtchSwtch[i],(i%16)+1,1);
    populateSwitchCB(fswtchSwtch[i], g_model.funcSw[i].swtch, POPULATE_MSWITCHES|POPULATE_ONOFF);

    fswtchFunc[i] = new QComboBox(this);
    fswtchFunc[i]->setProperty("functionIndex", i);
    connect(fswtchFunc[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customFunctionEdited()));
    layout->addWidget(fswtchFunc[i],(i%16)+1,2);
    populateFuncCB(fswtchFunc[i], g_model.funcSw[i].func);

    QHBoxLayout *paramLayout = new QHBoxLayout();
    layout->addLayout(paramLayout, (i%16)+1, 3);

    fswtchGVmode[i] = new QComboBox(this);
    fswtchGVmode[i]->setProperty("functionIndex", i);
    connect(fswtchGVmode[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customFunctionEdited()));
    paramLayout->addWidget(fswtchGVmode[i]);
    populateGVmodeCB(fswtchGVmode[i], g_model.funcSw[i].adjustMode);

    fswtchParamGV[i] = new QCheckBox(this);
    fswtchParamGV[i]->setProperty("functionIndex", i);
    fswtchParamGV[i]->setText("GV");
    fswtchParamGV[i]->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    connect(fswtchParamGV[i],SIGNAL(stateChanged(int)),this,SLOT(customFunctionEdited()));
    paramLayout->addWidget(fswtchParamGV[i]);
    
    fswtchParam[i] = new QDoubleSpinBox(this);
    fswtchParam[i]->setProperty("functionIndex", i);
    fswtchParam[i]->setAccelerated(true);
    fswtchParam[i]->setDecimals(0);
    connect(fswtchParam[i],SIGNAL(editingFinished()),this,SLOT(customFunctionEdited()));
    paramLayout->addWidget(fswtchParam[i]);

    fswtchParamT[i] = new QComboBox(this);
    fswtchParamT[i]->setProperty("functionIndex", i);
    paramLayout->addWidget(fswtchParamT[i]);
    populateFuncParamCB(fswtchParamT[i], func, g_model.funcSw[i].param, g_model.funcSw[i].adjustMode);
    connect(fswtchParamT[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customFunctionEdited()));

    fswtchParamArmT[i] = new QComboBox(this);
    fswtchParamArmT[i]->setProperty("functionIndex", i);
    populateFuncParamArmTCB(fswtchParamArmT[i],&g_model, g_model.funcSw[i].paramarm, paramarmList);
    fswtchParamArmT[i]->setEditable(true);
    paramLayout->addWidget(fswtchParamArmT[i]);

    connect(fswtchParamArmT[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customFunctionEdited()));
    connect(fswtchParamArmT[i],SIGNAL(editTextChanged ( const QString)),this,SLOT(customFunctionEdited()));

#ifdef PHONON    
    playBT[i] = new QPushButton(this);
    playBT[i]->setObjectName(QString("play_%1").arg(i));
    playBT[i]->setIcon(QIcon(":/images/play.png"));
    paramLayout->addWidget(playBT[i]);
    connect(playBT[i],SIGNAL(pressed()),this,SLOT(playMusic()));
#endif    

    QHBoxLayout *repeatLayout = new QHBoxLayout();
    layout->addLayout(repeatLayout, (i%16)+1, 4);

    fswtchRepeat[i] = new QComboBox(this);
    fswtchRepeat[i]->setProperty("functionIndex", i);
    connect(fswtchRepeat[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customFunctionEdited()));
    repeatLayout->addWidget(fswtchRepeat[i],(i%16)+1);
    populateRepeatCB(fswtchRepeat[i], g_model.funcSw[i].repeatParam);

    fswtchEnable[i] = new QCheckBox(this);
    fswtchEnable[i]->setProperty("functionIndex", i);
    fswtchEnable[i]->setText(tr("ON"));
    repeatLayout->addWidget(fswtchEnable[i],(i%16)+1);
    fswtchEnable[i]->setChecked(g_model.funcSw[i].enabled);
    connect(fswtchEnable[i],SIGNAL(stateChanged(int)),this,SLOT(customFunctionEdited()));

    refreshCustomFunction(i);
  }

  if (num_fsw <= 16) {
    ui->FSwitchGB2->hide();
  }

  switchEditLock = false;
}

void ModelEdit::tabSafetySwitches()
{
    for(int i=0; i<NUM_SAFETY_CHNOUT; i++)
    {
        safetySwitchSwtch[i] = new QComboBox(this);
        populateSwitchCB(safetySwitchSwtch[i],g_model.safetySw[i].swtch);
        ui->grid_tabSafetySwitches->addWidget(safetySwitchSwtch[i],i+1,1);
        connect(safetySwitchSwtch[i],SIGNAL(currentIndexChanged(int)),this,SLOT(safetySwitchesEdited()));

        safetySwitchValue[i] = new QSpinBox(this);
        safetySwitchValue[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        safetySwitchValue[i]->setMaximum(125);
        safetySwitchValue[i]->setMinimum(-125);
        safetySwitchValue[i]->setAccelerated(true);
        safetySwitchValue[i]->setValue(g_model.safetySw[i].val);
        ui->grid_tabSafetySwitches->addWidget(safetySwitchValue[i],i+1,2);
        connect(safetySwitchValue[i],SIGNAL(editingFinished()),this,SLOT(safetySwitchesEdited()));
    }
}

void ModelEdit::customFieldEdited()
{
  if (telemetryLock) return;
  int cols=GetEepromInterface()->getCapability(TelemetryColsCSFields);
  if (cols==0) cols=2;

  for (int i=0; i<GetEepromInterface()->getCapability(TelemetryCSFields); i++) {
    int screen=i/(4*cols);
    int r=((i%(4*cols))%4);
    int c=((i%(4*cols))/4);
    if (g_model.frsky.screens[screen].type==0) {
      g_model.frsky.screens[screen].body.lines[r].source[c]=csf[i]->currentIndex();
    }
  }
  updateSettings();
}

void ModelEdit::customSwitchesEdited()
{
    if(switchEditLock) return;
        
    switchEditLock = true;
    int i=-1;
    QString indextext=sender()->property("index").toString();
    if (!indextext.isEmpty()) {
      i=indextext.toInt();
    } else {
      // should not happen
      switchEditLock = false;
      return;
    }
    bool chAr;
    float value, step;
    int newval;
    chAr = (getCSFunctionFamily(g_model.customSw[i].func) != getCSFunctionFamily(csw[i]->itemData(csw[i]->currentIndex()).toInt()));
    g_model.customSw[i].func = csw[i]->itemData(csw[i]->currentIndex()).toInt();
    if(chAr) {
      if (getCSFunctionFamily(g_model.customSw[i].func)==CS_FAMILY_TIMERS) {
        g_model.customSw[i].val1 = -119;
        g_model.customSw[i].val2 = -119;
      } else {
        g_model.customSw[i].val1 = 0;
        g_model.customSw[i].val2 = 0;        
      }
      g_model.customSw[i].andsw = 0;
      setSwitchWidgetVisibility(i);
    }
    if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
      g_model.customSw[i].duration= (uint8_t)round(cswitchDuration[i]->value()*2);
      g_model.customSw[i].delay= (uint8_t)round(cswitchDelay[i]->value()*2);
    }
    RawSource source;
    switch (getCSFunctionFamily(g_model.customSw[i].func))
    {
      case (CS_FAMILY_VOFS):
        if (g_model.customSw[i].val1 != cswitchSource1[i]->itemData(cswitchSource1[i]->currentIndex()).toInt()) {
          source=RawSource(g_model.customSw[i].val1);
          g_model.customSw[i].val1 = cswitchSource1[i]->itemData(cswitchSource1[i]->currentIndex()).toInt();
          RawSource newSource = RawSource(g_model.customSw[i].val1);
          if (newSource.type == SOURCE_TYPE_TELEMETRY) {
            if (g_model.customSw[i].func>CS_FN_ELESS && g_model.customSw[i].func<CS_FN_VEQUAL) {
              g_model.customSw[i].val2 = 0;
            } else {
              g_model.customSw[i].val2 = -128;
            }
          } else {
            if (g_model.customSw[i].func>CS_FN_ELESS && g_model.customSw[i].func<CS_FN_VEQUAL) {
              g_model.customSw[i].val2 = (cswitchOffset[i]->value()/source.getStep(g_model));
            } else {
              g_model.customSw[i].val2 = ((cswitchOffset[i]->value()-source.getOffset(g_model))/source.getStep(g_model))-source.getRawOffset(g_model);
            }
          }
          setSwitchWidgetVisibility(i);
       } else {
          source=RawSource(g_model.customSw[i].val1);
          if (g_model.customSw[i].func>CS_FN_ELESS && g_model.customSw[i].func<CS_FN_VEQUAL) {
            g_model.customSw[i].val2 = (cswitchOffset[i]->value()/source.getStep(g_model));
            cswitchOffset[i]->setValue(g_model.customSw[i].val2*source.getStep(g_model));
          } else {
            g_model.customSw[i].val2 = ((cswitchOffset[i]->value()-source.getOffset(g_model))/source.getStep(g_model))-source.getRawOffset(g_model);
            cswitchOffset[i]->setValue((g_model.customSw[i].val2 +source.getRawOffset(g_model))*source.getStep(g_model)+source.getOffset(g_model));            
          }
        }
        break;
      case (CS_FAMILY_TIMERS): {
        value=cswitchOffset[i]->value();
        newval=TimToVal(value);
        if (newval>g_model.customSw[i].val2) {
          if (value >=60) {
            value=round(value);
            step=1;
          } else if (value>=2) {
            value=(round(value*2.0)/2);
            step=0.5;
          } else {
            step=0.1;
          }
        } else {
          if (value <=2) {
            step=0.1;
          } else if (value<=60) {
            value=(round(value*2.0)/2);
            step=0.5;
          } else {
            value=round(value);
            step=1;            
          }
        }
        g_model.customSw[i].val2=TimToVal(value);
        value=ValToTim(g_model.customSw[i].val2);
        cswitchOffset[i]->setValue(value);
        cswitchOffset[i]->setSingleStep(step);

        value=cswitchValue[i]->value();
        newval=TimToVal(value);
        if (newval>g_model.customSw[i].val1) {
          if (value >=60) {
            value=round(value);
            step=1;
          } else if (value>=2) {
            value=(round(value*2.0)/2);
            step=0.5;
          } else {
            step=0.1;
          }
        } else {
          if (value <=2) {
            step=0.1;
          } else if (value<=60) {
            value=(round(value*2.0)/2);
            step=0.5;
          } else {
            value=round(value);
            step=1;            
          }
        }
        g_model.customSw[i].val1=TimToVal(value);
        value=ValToTim(g_model.customSw[i].val1);
        cswitchValue[i]->setValue(value);
        cswitchValue[i]->setSingleStep(step);
        break;
        }          
      case (CS_FAMILY_VBOOL):
      case (CS_FAMILY_VCOMP):
        g_model.customSw[i].val1 = cswitchSource1[i]->itemData(cswitchSource1[i]->currentIndex()).toInt();
        g_model.customSw[i].val2 = cswitchSource2[i]->itemData(cswitchSource2[i]->currentIndex()).toInt();
        break;
    }
    g_model.customSw[i].andsw = cswitchAnd[i]->itemData(cswitchAnd[i]->currentIndex()).toInt();
    updateSettings();
    switchEditLock = false;
}

#ifdef PHONON

void ModelEdit::mediaPlayer_state(Phonon::State newState, Phonon::State oldState)
{
    if (phononLock)
      return;
    phononLock=true;
    if ((newState==Phonon::StoppedState || newState==Phonon::PausedState)  && oldState==Phonon::PlayingState) {
      clickObject->stop();
      clickObject->clearQueue();
      clickObject->clear();
      for (int i=0; i<GetEepromInterface()->getCapability(CustomFunctions); i++) {
        playBT[i]->setObjectName(QString("play_%1").arg(i));
        playBT[i]->setIcon(QIcon(":/images/play.png"));   
      }
    }
    if (newState==Phonon::ErrorState) {
      clickObject->stop();
      clickObject->clearQueue();
      clickObject->clear();
      for (int i=0; i<GetEepromInterface()->getCapability(CustomFunctions); i++) {
        playBT[i]->setObjectName(QString("play_%1").arg(i));
        playBT[i]->setIcon(QIcon(":/images/play.png"));   
      }
    }

    phononLock=false;
}
#endif

void ModelEdit::playMusic()
{
    QPushButton *playButton = qobject_cast<QPushButton*>(sender());
    int index=playButton->objectName().mid(5,2).toInt();
    QString function=playButton->objectName().left(4);
    QSettings settings("companion9x", "companion9x");
    QString path=settings.value("sdPath", ".").toString();
    QDir qd(path);
    QString track;
    if (qd.exists()) {
      if (GetEepromInterface()->getCapability(VoicesAsNumbers)) {
        track=path+QString("/%1.wav").arg(int(fswtchParam[index]->value()),4,10,(const QChar)'0');
      } else {
        path.append("/SOUNDS/");
        QString lang=radioData.generalSettings.ttsLanguage;
        if (lang.isEmpty())
          lang="en";
        path.append(lang);
        if (fswtchParamArmT[index]->currentText()!="----") {
          track=path+"/"+fswtchParamArmT[index]->currentText()+".wav";
        }
      }
      QFile file(track);
      if (!file.exists()) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to find sound file %1!").arg(track));
        track.clear();
      }
#ifdef PHONON
      if (function=="play" && !track.isEmpty()) {
        clickObject->clear();
        clickObject->setCurrentSource(Phonon::MediaSource(track));
        clickObject->play();
        playBT[index]->setObjectName(QString("stop_%1").arg(index));
        playBT[index]->setIcon(QIcon(":/images/stop.png"));
      } else {
        clickObject->stop();
        clickObject->clear();
        playBT[index]->setObjectName(QString("play_%1").arg(index));
        playBT[index]->setIcon(QIcon(":/images/play.png"));      
      }
#endif
    }
}

#define CUSTOM_FUNCTION_NUMERIC_PARAM  (1<<0)
#define CUSTOM_FUNCTION_SOURCE_PARAM   (1<<1)
#define CUSTOM_FUNCTION_FILE_PARAM     (1<<2)
#define CUSTOM_FUNCTION_GV_MODE        (1<<3)
#define CUSTOM_FUNCTION_GV_TOOGLE      (1<<4)
#define CUSTOM_FUNCTION_ENABLE         (1<<5)
#define CUSTOM_FUNCTION_REPEAT         (1<<6)
#define CUSTOM_FUNCTION_PLAY           (1<<7)

void ModelEdit::customFunctionEdited()
{
    if (switchEditLock) return;
    switchEditLock = true;

    int index = sender()->property("functionIndex").toInt();
    refreshCustomFunction(index, true);

    updateSettings();
    switchEditLock = false;
}

void ModelEdit::refreshCustomFunction(int i, bool modified)
{
    unsigned int widgetsMask = 0;
    if (modified) {
      g_model.funcSw[i].swtch = RawSwitch(fswtchSwtch[i]->itemData(fswtchSwtch[i]->currentIndex()).toInt());
      g_model.funcSw[i].func = (AssignFunc)fswtchFunc[i]->currentIndex();
      g_model.funcSw[i].enabled =fswtchEnable[i]->isChecked();
      g_model.funcSw[i].repeatParam = (AssignFunc)fswtchRepeat[i]->currentIndex();
      g_model.funcSw[i].adjustMode = (AssignFunc)fswtchGVmode[i]->currentIndex();
    }

    int index = fswtchFunc[i]->currentIndex();

    if (index>=FuncSafetyCh1 && index<=FuncSafetyCh16) {
      fswtchParam[i]->setDecimals(0);
      fswtchParam[i]->setSingleStep(1);
      fswtchParam[i]->setMinimum(-125);
      fswtchParam[i]->setMaximum(125);
      if (modified) {
        g_model.funcSw[i].param = fswtchParam[i]->value();
      }
      fswtchParam[i]->setValue(g_model.funcSw[i].param);
      widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM + CUSTOM_FUNCTION_ENABLE;
    } else if (index==FuncLogs) {
      fswtchParam[i]->setDecimals(1);
      fswtchParam[i]->setMinimum(0);
      fswtchParam[i]->setMaximum(25.5);
      fswtchParam[i]->setSingleStep(0.1);    
      if (modified) g_model.funcSw[i].param = fswtchParam[i]->value()*10.0;
      fswtchParam[i]->setValue(g_model.funcSw[i].param/10.0);
      widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM + CUSTOM_FUNCTION_ENABLE;
    } else if (index>=FuncAdjustGV1 && index<=FuncAdjustGV5) {
      if (modified) g_model.funcSw[i].adjustMode = fswtchGVmode[i]->currentIndex();
      widgetsMask |= CUSTOM_FUNCTION_GV_MODE + CUSTOM_FUNCTION_ENABLE;
      if (g_model.funcSw[i].adjustMode==0) {
        if (modified) g_model.funcSw[i].param = fswtchParam[i]->value();
        fswtchParam[i]->setDecimals(0);
        fswtchParam[i]->setSingleStep(1);
        fswtchParam[i]->setMinimum(-125);
        fswtchParam[i]->setMaximum(125);
        fswtchParam[i]->setValue(g_model.funcSw[i].param);
        widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM;
      } else {
        if (modified) g_model.funcSw[i].param = fswtchParamT[i]->itemData(fswtchParamT[i]->currentIndex()).toInt();
        populateFuncParamCB(fswtchParamT[i], index, g_model.funcSw[i].param, g_model.funcSw[i].adjustMode);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
      }
    } else if (index==FuncReset) {
      if (modified) g_model.funcSw[i].param = (uint8_t)fswtchParamT[i]->currentIndex();
      populateFuncParamCB(fswtchParamT[i], index, g_model.funcSw[i].param);
      widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
    } else if (index==FuncVolume) {
      if (modified) g_model.funcSw[i].param = fswtchParamT[i]->itemData(fswtchParamT[i]->currentIndex()).toInt();
      populateFuncParamCB(fswtchParamT[i], index, g_model.funcSw[i].param);
      widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM + CUSTOM_FUNCTION_ENABLE;
    } else if (index==FuncPlaySound || index==FuncPlayHaptic || index==FuncPlayValue || index==FuncPlayPrompt || index==FuncPlayBoth || index==FuncBackgroundMusic) {
      if (modified) g_model.funcSw[i].repeatParam = fswtchRepeat[i]->itemData(fswtchRepeat[i]->currentIndex()).toInt();
      if (index != FuncBackgroundMusic) {
        if (GetEepromInterface()->getCapability(HasFuncRepeat)) {
          widgetsMask |= CUSTOM_FUNCTION_REPEAT;
        }
      }
      if (index==FuncPlayValue) {
        if (modified) g_model.funcSw[i].param = fswtchParamT[i]->itemData(fswtchParamT[i]->currentIndex()).toInt();
        populateFuncParamCB(fswtchParamT[i], index, g_model.funcSw[i].param);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM + CUSTOM_FUNCTION_REPEAT;
      } else if (index==FuncPlayPrompt || index==FuncPlayBoth) {
        if (GetEepromInterface()->getCapability(VoicesAsNumbers)) {
          fswtchParam[i]->setDecimals(0);
          fswtchParam[i]->setSingleStep(1);
          fswtchParam[i]->setMinimum(0);
          if (index==FuncPlayPrompt) {
            widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM + CUSTOM_FUNCTION_REPEAT + CUSTOM_FUNCTION_GV_TOOGLE;
          } else {
            widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM + CUSTOM_FUNCTION_REPEAT;
            fswtchParamGV[i]->setChecked(false);
          }
          fswtchParam[i]->setMaximum(index==FuncPlayBoth ? 254 : 255);
          if (modified) {
            if (fswtchParamGV[i]->isChecked()) {
              fswtchParam[i]->setMinimum(1);
              g_model.funcSw[i].param = std::min(fswtchParam[i]->value(),5.0)+(fswtchParamGV[i]->isChecked() ? 250 : 0);
            } else {
              g_model.funcSw[i].param = fswtchParam[i]->value();
            }
          }
          if (g_model.funcSw[i].param>250 && (index!=FuncPlayBoth)) {
            fswtchParamGV[i]->setChecked(true);
            fswtchParam[i]->setValue(g_model.funcSw[i].param-250);
            fswtchParam[i]->setMaximum(5);
          } else {
            fswtchParamGV[i]->setChecked(false);
            fswtchParam[i]->setValue(g_model.funcSw[i].param);
          }
          if (g_model.funcSw[i].param < 251)
            widgetsMask |= CUSTOM_FUNCTION_PLAY;
        } else {
          widgetsMask |= CUSTOM_FUNCTION_FILE_PARAM;
          if (modified) {
            memset(g_model.funcSw[i].paramarm, 0, sizeof(g_model.funcSw[i].paramarm));
            int vml=GetEepromInterface()->getCapability(VoicesMaxLength);
            if (fswtchParamArmT[i]->currentText() != "----") {
              widgetsMask |= CUSTOM_FUNCTION_PLAY;
              for (int j=0; j<std::min(fswtchParamArmT[i]->currentText().length(),vml); j++) {
                g_model.funcSw[i].paramarm[j] = fswtchParamArmT[i]->currentText().toAscii().at(j);
              }
            }
          }
        }
      } else if (index==FuncBackgroundMusic) {
        widgetsMask |= CUSTOM_FUNCTION_FILE_PARAM;
        if (modified) {
          memset(g_model.funcSw[i].paramarm, 0, sizeof(g_model.funcSw[i].paramarm));
          int vml=GetEepromInterface()->getCapability(VoicesMaxLength);
          if (fswtchParamArmT[i]->currentText() != "----") {
            widgetsMask |= CUSTOM_FUNCTION_PLAY;
            for (int j=0; j<std::min(fswtchParamArmT[i]->currentText().length(),vml); j++) {
              g_model.funcSw[i].paramarm[j] = fswtchParamArmT[i]->currentText().toAscii().at(j);
            }
          }
        }      
      } else if (index==FuncPlaySound) {
        if (modified) g_model.funcSw[i].param = (uint8_t)fswtchParamT[i]->currentIndex();
        populateFuncParamCB(fswtchParamT[i], index, g_model.funcSw[i].param);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
      } else if (index==FuncPlayHaptic) {
        if (modified) g_model.funcSw[i].param = (uint8_t)fswtchParamT[i]->currentIndex();
        populateFuncParamCB(fswtchParamT[i], index, g_model.funcSw[i].param);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
      }
   } else if (g_model.funcSw[i].swtch.type!=SWITCH_TYPE_NONE) {
      if (modified) g_model.funcSw[i].param = fswtchParam[i]->value();
      fswtchParam[i]->setDecimals(0);
      fswtchParam[i]->setSingleStep(1);
      fswtchParam[i]->setValue(g_model.funcSw[i].param);
      if (index<=FuncInstantTrim) {
        widgetsMask |= CUSTOM_FUNCTION_ENABLE;
      }
    }

    fswtchParam[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_NUMERIC_PARAM);
    fswtchParamGV[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_GV_TOOGLE);
    fswtchParamT[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_SOURCE_PARAM);
    fswtchParamArmT[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_FILE_PARAM);
    fswtchEnable[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_ENABLE);
    if (!(widgetsMask & CUSTOM_FUNCTION_ENABLE)) fswtchEnable[i]->setChecked(false);
    fswtchRepeat[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_REPEAT);
    fswtchGVmode[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_GV_MODE);
#ifdef PHONON
    playBT[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_PLAY);
#endif
}

void ModelEdit::tabTelemetry()
{
    QSettings settings("companion9x", "companion9x");
    QString firmware_id = settings.value("firmware", default_firmware_variant.id).toString();
    float a1ratio;
    float a2ratio;

    QGroupBox* barsgb[3] = { ui->CS1Bars, ui->CS2Bars, ui->CS3Bars };
    QGroupBox* numsgb[3] = { ui->CS1Nums, ui->CS2Nums, ui->CS3Nums };

    QComboBox* barscb[12] = { ui->telBarCS1B1_CB, ui->telBarCS1B2_CB,  ui->telBarCS1B3_CB,  ui->telBarCS1B4_CB,
                              ui->telBarCS2B1_CB, ui->telBarCS2B2_CB,  ui->telBarCS2B3_CB,  ui->telBarCS2B4_CB,
                              ui->telBarCS3B1_CB, ui->telBarCS3B2_CB,  ui->telBarCS3B3_CB,  ui->telBarCS3B4_CB};
    QDoubleSpinBox* minsb[12] = { ui->telMinCS1SB1,  ui->telMinCS1SB2,  ui->telMinCS1SB3,  ui->telMinCS1SB4,
                                  ui->telMinCS2SB1,  ui->telMinCS2SB2,  ui->telMinCS2SB3,  ui->telMinCS2SB4,
                                  ui->telMinCS3SB1,  ui->telMinCS3SB2,  ui->telMinCS3SB3,  ui->telMinCS3SB4};
    QDoubleSpinBox* maxsb[12] = { ui->telMaxCS1SB1,  ui->telMaxCS1SB2,  ui->telMaxCS1SB3,  ui->telMaxCS1SB4,
                                  ui->telMaxCS2SB1,  ui->telMaxCS2SB2,  ui->telMaxCS2SB3,  ui->telMaxCS2SB4,
                                  ui->telMaxCS3SB1,  ui->telMaxCS3SB2,  ui->telMaxCS3SB3,  ui->telMaxCS3SB4};
    QComboBox* tmp[36] = { ui->telemetryCS1F1_CB, ui->telemetryCS1F2_CB, ui->telemetryCS1F3_CB, ui->telemetryCS1F4_CB, ui->telemetryCS1F5_CB, ui->telemetryCS1F6_CB, ui->telemetryCS1F7_CB, ui->telemetryCS1F8_CB, ui->telemetryCS1F9_CB, ui->telemetryCS1F10_CB, ui->telemetryCS1F11_CB, ui->telemetryCS1F12_CB,
                           ui->telemetryCS2F1_CB, ui->telemetryCS2F2_CB, ui->telemetryCS2F3_CB, ui->telemetryCS2F4_CB, ui->telemetryCS2F5_CB, ui->telemetryCS2F6_CB, ui->telemetryCS2F7_CB, ui->telemetryCS2F8_CB, ui->telemetryCS2F9_CB, ui->telemetryCS2F10_CB, ui->telemetryCS2F11_CB, ui->telemetryCS2F12_CB,
                           ui->telemetryCS3F1_CB, ui->telemetryCS3F2_CB, ui->telemetryCS3F3_CB, ui->telemetryCS3F4_CB, ui->telemetryCS3F5_CB, ui->telemetryCS3F6_CB, ui->telemetryCS3F7_CB, ui->telemetryCS3F8_CB, ui->telemetryCS3F9_CB, ui->telemetryCS3F10_CB, ui->telemetryCS3F11_CB, ui->telemetryCS3F12_CB};

    memcpy(barsGB, barsgb, sizeof(barsGB));
    memcpy(numsGB, numsgb, sizeof(numsGB));
    memcpy(barsCB, barscb, sizeof(barsCB));
    memcpy(maxSB, maxsb, sizeof(maxSB));
    memcpy(minSB, minsb, sizeof(minSB));
    memcpy(csf, tmp, sizeof(csf));

    telemetryLock=true;
    ui->telemetryCSType1->setCurrentIndex(g_model.frsky.screens[0].type);
    ui->telemetryCSType2->setCurrentIndex(g_model.frsky.screens[1].type);
    ui->telemetryCSType3->setCurrentIndex(g_model.frsky.screens[2].type);

    for (int i=0; i<3; i++) {
      bool isNum = (g_model.frsky.screens[i].type==0);
      barsGB[i]->setVisible(!isNum);
      numsGB[i]->setVisible(isNum);
    }

    connect(ui->telemetryCSType1,SIGNAL(currentIndexChanged(int)),this,SLOT(ScreenTypeCBcurrentIndexChanged(int)));
    connect(ui->telemetryCSType2,SIGNAL(currentIndexChanged(int)),this,SLOT(ScreenTypeCBcurrentIndexChanged(int)));
    connect(ui->telemetryCSType3,SIGNAL(currentIndexChanged(int)),this,SLOT(ScreenTypeCBcurrentIndexChanged(int)));

    //frsky Settings
    if (!GetEepromInterface()->getCapability(TelemetryRSSIModel) ) {
      ui->RSSIGB->hide();
    }
    ui->rssiAlarm1SB->setValue(g_model.frsky.rssiAlarms[0].value);
    ui->rssiAlarm2SB->setValue(g_model.frsky.rssiAlarms[1].value);
    ui->rssiAlarm1CB->setCurrentIndex(g_model.frsky.rssiAlarms[0].level);
    ui->rssiAlarm2CB->setCurrentIndex(g_model.frsky.rssiAlarms[1].level);

    if (!GetEepromInterface()->getCapability(HasAltitudeSel)) {
      ui->AltitudeGPS_ChkB->hide();
    } else {
      ui->AltitudeGPS_ChkB->setChecked(g_model.frsky.FrSkyGpsAlt);
    }
    int varioCap=GetEepromInterface()->getCapability(HasVario);
  //  if (IS_TARANIS(GetEepromInterface()->getBoard())) {   
    if (false) {   
      ui->AltitudeToolbar_ChkB->setChecked(g_model.frsky.altitudeDisplayed);
    } else {
      ui->AltitudeToolbar_ChkB->hide();  
    }
    if (!varioCap) {
      ui->varioLimitMax_DSB->hide();
      ui->varioLimitMinOff_ChkB->hide();
      ui->varioLimitMin_DSB->hide();
      ui->varioLimitCenterMin_DSB->hide();
      ui->varioLimitCenterMax_DSB->hide();
      ui->varioLimit_label->hide();
      ui->VarioLabel_1->hide();
      ui->VarioLabel_2->hide();
      ui->VarioLabel_3->hide();
      ui->VarioLabel_4->hide();
      ui->varioSourceCB->hide();
      ui->varioSource_label->hide();
    } else {
      if (!GetEepromInterface()->getCapability(HasVarioSink)) {
        ui->varioLimitMinOff_ChkB->hide();
        ui->varioLimitMin_DSB->hide();
        ui->varioLimitCenterMin_DSB->hide();
        ui->VarioLabel_1->hide();
        ui->VarioLabel_2->hide();      
      }
      ui->varioLimitMin_DSB->setValue(g_model.frsky.varioMin-10);
      ui->varioLimitMax_DSB->setValue(g_model.frsky.varioMax+10);
      ui->varioLimitCenterMax_DSB->setValue((g_model.frsky.varioCenterMax/10.0)+0.5);
      ui->varioSourceCB->setCurrentIndex(g_model.frsky.varioSource);
      if (g_model.frsky.varioCenterMin==-16) {
        ui->varioLimitMinOff_ChkB->setChecked(true);
        ui->varioLimitCenterMin_DSB->setValue(-2.0);
        ui->varioLimitCenterMin_DSB->setDisabled(true);
      } else {
        ui->varioLimitMinOff_ChkB->setChecked(false);
        ui->varioLimitCenterMin_DSB->setValue((g_model.frsky.varioCenterMin/10.0)-0.5);
      }
      int mask=1;
      for (int i=0; i< ui->varioSourceCB->count(); i++) {
        if (!(varioCap&mask)) {
          QModelIndex index = ui->varioSourceCB->model()->index(i, 0);
          QVariant v(0);
          ui->varioSourceCB->model()->setData(index, v, Qt::UserRole - 1);
        }
        mask <<=1;
      }
    }

    if (!(GetEepromInterface()->getCapability(HasAltitudeSel)||GetEepromInterface()->getCapability(HasVario))) {
      ui->altimetryGB->hide();
    }
    if (GetEepromInterface()->getCapability(NoTelemetryProtocol)) {
      g_model.frsky.usrProto=1;
      ui->frskyProtoCB->setDisabled(true);
    } else {
      ui->frskyProtoCB->setEnabled(true);
    }

    if (GetEepromInterface()->getCapability(TelemetryCSFields)==0) {
      ui->groupBox_5->hide();
    } else {
      if (GetEepromInterface()->getCapability(TelemetryCSFields)==8) {
        ui->tabCsView->removeTab(1);
        ui->tabCsView->removeTab(2);
      }
      if (GetEepromInterface()->getCapability(TelemetryCSFields)==16) {
        ui->tabCsView->removeTab(2);
      }
      int cols=GetEepromInterface()->getCapability(TelemetryColsCSFields);
      if (cols==0) cols=2;
      for (int screen=0; screen<(GetEepromInterface()->getCapability(TelemetryCSFields)/(4*cols)); screen++) {
        for (int c=0; c<cols; c++) {
          for (int r=0; r<4; r++) {
            int index=screen*12+c*4+r;
            if (g_model.frsky.screens[screen].type==0) {
              populateCustomScreenFieldCB(csf[index], g_model.frsky.screens[screen].body.lines[r].source[c], (r<4), g_model.frsky.usrProto);
            } else {
              populateCustomScreenFieldCB(csf[index], 0, (r<4), g_model.frsky.usrProto);
            }
            connect(csf[index],SIGNAL(currentIndexChanged(int)),this,SLOT(customFieldEdited()));  
          }
        }
        for (int c=cols; c<3; c++) {
          for (int r=0; r<4; r++) {
            int index=screen*12+c*4+r;
            csf[index]->hide();
          }
        }
      }   
    }

    if (!GetEepromInterface()->getCapability(TelemetryUnits)) {
      ui->frskyUnitsCB->setDisabled(true);
      int index=0;
      if (firmware_id.contains("imperial")) {
        index=1;
      }
      ui->frskyUnitsCB->setCurrentIndex(index);
    }
    if ((GetEepromInterface()->getCapability(Telemetry)&TM_HASWSHH)) {
      ui->frskyProtoCB->addItem(tr("Winged Shadow How High"));
    } else {
      ui->frskyProtoCB->addItem(tr("Winged Shadow How High (not supported)"));
    }
    ui->a1UnitCB->setCurrentIndex(g_model.frsky.channels[0].type);
    if (g_model.frsky.channels[0].type==0 || g_model.frsky.channels[0].type==1 || g_model.frsky.channels[0].type==2) {
      a1ratio=(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier)/10.0;
      ui->a1RatioSB->setDecimals(1);
      ui->a1RatioSB->setMaximum(25.5*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
    } else {
      a1ratio=g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier;
      ui->a1RatioSB->setDecimals(0);
      ui->a1RatioSB->setMaximum(255*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
    }
    ui->a1RatioSB->setValue(a1ratio);

    updateA1Fields();

    if (g_model.frsky.channels[1].type==0 || g_model.frsky.channels[1].type==1 || g_model.frsky.channels[1].type==2) {
      a2ratio=(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier)/10.0;
      ui->a2RatioSB->setDecimals(1);
      ui->a2RatioSB->setMaximum(25.5*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
    } else {
      a2ratio=g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier;
      ui->a2RatioSB->setDecimals(0);
      ui->a2RatioSB->setMaximum(255*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
    }
    ui->a2RatioSB->setValue(a2ratio);
    updateA2Fields();

    ui->a11LevelCB->setCurrentIndex(g_model.frsky.channels[0].alarms[0].level);
    ui->a11GreaterCB->setCurrentIndex(g_model.frsky.channels[0].alarms[0].greater);
    ui->a12LevelCB->setCurrentIndex(g_model.frsky.channels[0].alarms[1].level);
    ui->a12GreaterCB->setCurrentIndex(g_model.frsky.channels[0].alarms[1].greater);
    ui->a2UnitCB->setCurrentIndex(g_model.frsky.channels[1].type);
    ui->a21LevelCB->setCurrentIndex(g_model.frsky.channels[1].alarms[0].level);
    ui->a21GreaterCB->setCurrentIndex(g_model.frsky.channels[1].alarms[0].greater);
    ui->a22LevelCB->setCurrentIndex(g_model.frsky.channels[1].alarms[1].level);
    ui->a22GreaterCB->setCurrentIndex(g_model.frsky.channels[1].alarms[1].greater);
    if (!(GetEepromInterface()->getCapability(Telemetry)&TM_HASOFFSET)) {
      ui->a1CalibSB->hide();
      ui->a2CalibSB->hide();
      ui->a1CalibLabel->hide();
      ui->a2CalibLabel->hide();
    } else {
      ui->label_A1Max->setText(tr("Range"));
      ui->label_A2Max->setText(tr("Range"));
    }
    ui->frskyProtoCB->setCurrentIndex(g_model.frsky.usrProto);
    ui->frskyUnitsCB->setCurrentIndex(g_model.frsky.imperial);
    ui->frskyBladesCB->setCurrentIndex(g_model.frsky.blades);
    ui->frskyCurrentCB->setCurrentIndex(g_model.frsky.currentSource);
    ui->frskyVoltCB->setCurrentIndex(g_model.frsky.voltsSource);

    for (int screen=0; screen<2;screen++) {
      for (int rows=0; rows<4; rows++) {
        for (int cols=0; cols<3; cols++) {
          int index=screen*12+cols*4+rows;
          populateCustomScreenFieldCB(csf[index], g_model.frsky.screens[screen].body.lines[rows].source[cols], (rows<4), g_model.frsky.usrProto);
        }
      }
    }

    for (int j=0; j<12; j++) {
      int screen = j/4;
      int field = j%4;
      populateCustomScreenFieldCB(barsCB[j], g_model.frsky.screens[screen].body.bars[field].source, false, g_model.frsky.usrProto);
      switch (g_model.frsky.screens[screen].body.bars[field].source-1) {
        case TELEMETRY_SOURCE_TX_BATT:
        case TELEMETRY_SOURCE_A1:
        case TELEMETRY_SOURCE_A1_MIN:
        case TELEMETRY_SOURCE_A2:
        case TELEMETRY_SOURCE_A2_MIN:
        case TELEMETRY_SOURCE_CELLS_SUM:
        case TELEMETRY_SOURCE_VFAS:
        case TELEMETRY_SOURCE_CURRENT_MAX:
        case TELEMETRY_SOURCE_CURRENT:
          minsb[j]->setDecimals(1);
          maxsb[j]->setDecimals(1);
          break;
        case TELEMETRY_SOURCE_CELL:
          minsb[j]->setDecimals(2);
          maxsb[j]->setDecimals(2);
          break;
        default:
          minsb[j]->setDecimals(0);
          maxsb[j]->setDecimals(0);
      }
      minsb[j]->setMinimum(getBarValue(g_model.frsky.screens[screen].body.bars[field].source, 0, &g_model.frsky));
      minsb[j]->setMaximum(getBarValue(g_model.frsky.screens[screen].body.bars[field].source, 255, &g_model.frsky));
      minsb[j]->setSingleStep(getBarStep(g_model.frsky.screens[screen].body.bars[field].source));
      minsb[j]->setValue(getBarValue(g_model.frsky.screens[screen].body.bars[field].source, g_model.frsky.screens[screen].body.bars[field].barMin, &g_model.frsky));
      maxsb[j]->setMinimum(getBarValue(g_model.frsky.screens[screen].body.bars[field].source, 0, &g_model.frsky));
      maxsb[j]->setMaximum(getBarValue(g_model.frsky.screens[screen].body.bars[field].source, 255, &g_model.frsky));
      maxsb[j]->setSingleStep(getBarStep(g_model.frsky.screens[screen].body.bars[field].source));
      maxsb[j]->setValue(getBarValue(g_model.frsky.screens[screen].body.bars[field].source, (255-g_model.frsky.screens[screen].body.bars[field].barMax), &g_model.frsky));

      if (g_model.frsky.screens[screen].body.bars[field].source==0 || g_model.frsky.screens[screen].type==0) {
        minsb[j]->setDisabled(true);
        maxsb[j]->setDisabled(true);
      }
      connect(barsCB[j],SIGNAL(currentIndexChanged(int)),this,SLOT(telBarCBcurrentIndexChanged(int)));
      connect(maxSB[j],SIGNAL(editingFinished()),this,SLOT(telMaxSBeditingFinished()));
      connect(minSB[j],SIGNAL(editingFinished()),this,SLOT(telMinSBeditingFinished()));
    }
    telemetryLock=false;
}

void ModelEdit::updateA1Fields()
{
    float a1ratio,a11value, a12value;
    if (g_model.frsky.channels[0].ratio==0) {
      ui->a11ValueSB->setMinimum(0);
      ui->a11ValueSB->setMaximum(0);
      ui->a12ValueSB->setMinimum(0);
      ui->a12ValueSB->setMaximum(0);     
      ui->a1CalibSB->setMinimum(0);
      ui->a1CalibSB->setMaximum(0);
      ui->a1CalibSB->setValue(0);
      ui->a11ValueSB->setValue(0);
      ui->a12ValueSB->setValue(0);
      return;
    }

    if (g_model.frsky.channels[0].type==0 || g_model.frsky.channels[0].type==1 || g_model.frsky.channels[0].type==2) {
      a1ratio=(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier)/10.0;
      ui->a11ValueSB->setDecimals(2);
      ui->a11ValueSB->setSingleStep(a1ratio/255.0);
      ui->a12ValueSB->setDecimals(2);
      ui->a12ValueSB->setSingleStep(a1ratio/255.0);
      ui->a11ValueSB->setMinimum((g_model.frsky.channels[0].ratio*1.0*g_model.frsky.channels[0].offset)/2550.0);
      ui->a11ValueSB->setMaximum(a1ratio+(g_model.frsky.channels[0].ratio*1.0*g_model.frsky.channels[0].offset)/2550.0);
      ui->a12ValueSB->setMinimum((g_model.frsky.channels[0].offset*1.0*g_model.frsky.channels[0].ratio)/2550.0);
      ui->a12ValueSB->setMaximum(a1ratio+(g_model.frsky.channels[0].offset*1.0*g_model.frsky.channels[0].ratio)/2550.0);
    } else {
      a1ratio=g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier;
      ui->a11ValueSB->setDecimals(2);
      ui->a11ValueSB->setSingleStep(a1ratio/255.0);
      ui->a12ValueSB->setDecimals(2);
      ui->a12ValueSB->setSingleStep(a1ratio/255.0);
      ui->a11ValueSB->setMinimum((g_model.frsky.channels[0].offset*1.0*g_model.frsky.channels[0].ratio)/255.0);
      ui->a11ValueSB->setMaximum(a1ratio+(g_model.frsky.channels[0].offset*1.0*g_model.frsky.channels[0].ratio)/255.0);
      ui->a12ValueSB->setMinimum((g_model.frsky.channels[0].offset*1.0*g_model.frsky.channels[0].ratio)/255.0);
      ui->a12ValueSB->setMaximum(a1ratio+(g_model.frsky.channels[0].offset*1.0*g_model.frsky.channels[0].ratio)/255.0);
    }  
    ui->a1CalibSB->setDecimals(2);
    ui->a1CalibSB->setMaximum((a1ratio*127)/255.0);
    ui->a1CalibSB->setMinimum((-a1ratio*128)/255.0);
    ui->a1CalibSB->setSingleStep(a1ratio/255.0);
    ui->a1CalibSB->setValue((g_model.frsky.channels[0].offset*a1ratio)/255);
    a11value=a1ratio*(g_model.frsky.channels[0].alarms[0].value/255.0+g_model.frsky.channels[0].offset/255.0);
    a12value=a1ratio*(g_model.frsky.channels[0].alarms[1].value/255.0+g_model.frsky.channels[0].offset/255.0);;
    ui->a11ValueSB->setValue(a11value);
    ui->a12ValueSB->setValue(a12value);
}

void ModelEdit::updateA2Fields()
{
    float a2ratio,a21value, a22value;
    if (g_model.frsky.channels[1].ratio==0) {
      ui->a21ValueSB->setMinimum(0);
      ui->a21ValueSB->setMaximum(0);
      ui->a22ValueSB->setMinimum(0);
      ui->a22ValueSB->setMaximum(0);     
      ui->a2CalibSB->setMinimum(0);
      ui->a2CalibSB->setMaximum(0);
      ui->a2CalibSB->setValue(0);
      ui->a21ValueSB->setValue(0);
      ui->a22ValueSB->setValue(0);
      return;
    }
    if (g_model.frsky.channels[1].type==0) {
      a2ratio=g_model.frsky.channels[1].ratio/10.0;
      ui->a21ValueSB->setDecimals(2);
      ui->a21ValueSB->setSingleStep(a2ratio/255.0);
      ui->a22ValueSB->setDecimals(2);
      ui->a22ValueSB->setSingleStep(a2ratio/255.0);
      ui->a21ValueSB->setMinimum((g_model.frsky.channels[1].offset*1.0*g_model.frsky.channels[1].ratio)/2550.0);
      ui->a21ValueSB->setMaximum(a2ratio+(g_model.frsky.channels[1].offset*1.0*g_model.frsky.channels[1].ratio)/2550.0);
      ui->a22ValueSB->setMinimum((g_model.frsky.channels[1].offset*1.0*g_model.frsky.channels[1].ratio*1.0)/2550.0);
      ui->a22ValueSB->setMaximum(a2ratio+(g_model.frsky.channels[1].offset*1.0*g_model.frsky.channels[1].ratio)/2550.0);
    } else {
      a2ratio=g_model.frsky.channels[1].ratio;
      ui->a21ValueSB->setDecimals(2);
      ui->a21ValueSB->setSingleStep(a2ratio/255.0);
      ui->a22ValueSB->setDecimals(2);
      ui->a22ValueSB->setSingleStep(a2ratio/255.0);
      ui->a21ValueSB->setMinimum((g_model.frsky.channels[1].offset*1.0*g_model.frsky.channels[1].ratio)/255.0);
      ui->a21ValueSB->setMaximum(a2ratio+(g_model.frsky.channels[1].offset*1.0*g_model.frsky.channels[1].ratio)/255.0);
      ui->a22ValueSB->setMinimum((g_model.frsky.channels[1].offset*1.0*g_model.frsky.channels[1].ratio)/255.0);
      ui->a22ValueSB->setMaximum(a2ratio+(g_model.frsky.channels[1].offset*1.0*g_model.frsky.channels[1].ratio)/255.0);
    }  
    ui->a2CalibSB->setDecimals(2);
    ui->a2CalibSB->setMaximum((a2ratio*127)/255.0);
    ui->a2CalibSB->setMinimum((-a2ratio*128)/255.0);
    ui->a2CalibSB->setSingleStep(a2ratio/255.0);
    ui->a2CalibSB->setValue((g_model.frsky.channels[1].offset*a2ratio)/255.0);
    a21value=a2ratio*(g_model.frsky.channels[1].alarms[0].value/255.0+g_model.frsky.channels[1].offset/255.0);
    a22value=a2ratio*(g_model.frsky.channels[1].alarms[1].value/255.0+g_model.frsky.channels[1].offset/255.0);
    ui->a21ValueSB->setValue(a21value);
    ui->a22ValueSB->setValue(a22value);
}

void ModelEdit::tabTemplates()
{
    ui->templateList->clear();
    ui->templateList->addItem(tr("Simple 4-CH"));
    ui->templateList->addItem(tr("T-Cut"));
    ui->templateList->addItem(tr("Sticky T-Cut"));
    ui->templateList->addItem(tr("V-Tail"));
    ui->templateList->addItem(tr("Elevon\\Delta"));
    ui->templateList->addItem(tr("Heli Setup"));
    ui->templateList->addItem(tr("Heli Setup with gyro gain control"));
    ui->templateList->addItem(tr("Gyro gain control"));
    ui->templateList->addItem(tr("Heli Setup (Futaba's channel assignment style)"));
    ui->templateList->addItem(tr("Heli Setup with gyro gain control (Futaba's channel assignment style)"));
    ui->templateList->addItem(tr("Gyro gain control (Futaba's channel assignment style)"));
    ui->templateList->addItem(tr("Servo Test"));
    ui->templateList->addItem(tr("MultiCopter"));
    ui->templateList->addItem(tr("Use Model Config Wizard"));
}

void ModelEdit::on_modelNameLE_editingFinished()
{
    int length=ui->modelNameLE->maxLength();
    strncpy(g_model.name, ui->modelNameLE->text().toAscii(), length);
    updateSettings();
}

void ModelEdit::on_modelImage_CB_currentIndexChanged(int index)
{
    if (modelImageLock)
      return;
    strncpy(g_model.bitmap, ui->modelImage_CB->currentText().toAscii(), GetEepromInterface()->getCapability(VoicesMaxLength));
    QSettings settings("companion9x", "companion9x");
    QString path=settings.value("sdPath", ".").toString();
    path.append("/BMP/");
    QDir qd(path);
    if (qd.exists()) {
      QString fileName=path;
      fileName.append(g_model.bitmap);
      fileName.append(".bmp");
      QImage image(fileName);
      if (image.isNull()) {
        fileName=path;
        fileName.append(g_model.bitmap);
        fileName.append(".BMP");
        image.load(fileName);
      }
      if (!image.isNull()) {
        ui->modelImage_image->setPixmap(QPixmap::fromImage(image.scaled( 64,32)));;
      } else {
        ui->modelImage_image->clear();
      }
    } else {
      ui->modelImage_image->clear();
    }
    updateSettings();
}

void ModelEdit::phaseName_editingFinished()
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
    int phase = lineEdit->objectName().mid(5,1).toInt();
    QString PhaseName=lineEdit->text();
    QString TabName;
    if (phase==0) { 
      TabName.append(QObject::tr("Flight Mode 0 (Default)"));
    } else {
      TabName.append(QObject::tr("FM %1").arg(phase));
    }

    if (!PhaseName.isEmpty()) {
      TabName.append(" (");
      TabName.append(PhaseName);
      TabName.append(")");
    }
    ui->phases->setTabText(phase,TabName);
    int length=lineEdit->maxLength();
    strncpy(g_model.phaseData[phase].name, lineEdit->text().toAscii(), length);
    updateSettings();
}

void ModelEdit::phaseSwitch_currentIndexChanged()
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int phase = comboBox->objectName().mid(5,1).toInt();
    g_model.phaseData[phase].swtch = RawSwitch(comboBox->itemData(comboBox->currentIndex()).toInt());
    updateSettings();
}

void ModelEdit::phaseFadeIn_editingFinished()
{
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int phase = spinBox->objectName().mid(5,1).toInt();
    int scale=GetEepromInterface()->getCapability(SlowScale);  
    g_model.phaseData[phase].fadeIn = round(spinBox->value()*scale);
    updateSettings();
}

void ModelEdit::phaseFadeOut_editingFinished()
{
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int phase = spinBox->objectName().mid(5,1).toInt();
    int scale=GetEepromInterface()->getCapability(SlowScale);  
    g_model.phaseData[phase].fadeOut = round(spinBox->value()*scale);
    updateSettings();
}

void ModelEdit::on_timer1ModeCB_currentIndexChanged(int index)
{
    g_model.timers[0].mode = TimerMode(ui->timer1ModeCB->itemData(index).toInt());
    updateSettings();
}

void ModelEdit::on_timer1DirCB_currentIndexChanged(int index)
{
    g_model.timers[0].dir = index;
    updateSettings();
}

void ModelEdit::on_timer1ValTE_editingFinished()
{
    g_model.timers[0].val = ui->timer1ValTE->time().minute()*60 + ui->timer1ValTE->time().second();
    updateSettings();
}

void ModelEdit::on_timer1ModeBCB_currentIndexChanged(int index)
{
    g_model.timers[0].modeB = ui->timer1ModeBCB->itemData(index).toInt();
    updateSettings();
}

void ModelEdit::on_timer2ModeCB_currentIndexChanged(int index)
{
    g_model.timers[1].mode = TimerMode(ui->timer2ModeCB->itemData(index).toInt());
    updateSettings();
}

void ModelEdit::on_timer2DirCB_currentIndexChanged(int index)
{
    g_model.timers[1].dir = index;
    updateSettings();
}

void ModelEdit::on_timer2ValTE_editingFinished()
{
    g_model.timers[1].val = ui->timer2ValTE->time().minute()*60 + ui->timer2ValTE->time().second();
    updateSettings();
}

void ModelEdit::on_timer2ModeBCB_currentIndexChanged(int index)
{
    g_model.timers[1].modeB = ui->timer2ModeBCB->itemData(index).toInt();
    updateSettings();
}

void ModelEdit::on_trimIncCB_currentIndexChanged(int index)
{
    g_model.trimInc = index;
    updateSettings();
}

void ModelEdit::on_ttraceCB_currentIndexChanged(int index)
{
    g_model.thrTraceSrc = index;
    updateSettings();
}

void ModelEdit::on_pulsePolCB_currentIndexChanged(int index)
{
    g_model.moduleData[0].ppmPulsePol = index;
    updateSettings();
}

void ModelEdit::on_pulsePolCB_2_currentIndexChanged(int index)
{
    g_model.moduleData[1].ppmPulsePol = index;
    updateSettings();
}

void ModelEdit::on_pulsePolCB_3_currentIndexChanged(int index)
{
    g_model.moduleData[2].ppmPulsePol = index;
    updateSettings();
}

void ModelEdit::on_protocolCB_currentIndexChanged(int index)
{
  if (!protocolEditLock) {
    protocolEditLock=true;
    
    g_model.moduleData[0].protocol=(Protocol)ui->protocolCB->itemData(index).toInt();
    int protocol=g_model.moduleData[0].protocol;
  //  g_model.protocol = (Protocol)index;
    if (protocol==PXX_XJT_D8 || protocol==OFF) {
      ui->FSGB_1->hide();
    } else {
      ui->fsm1CB->setCurrentIndex(g_model.moduleData[0].failsafeMode);
      on_fsm1CB_currentIndexChanged(g_model.moduleData[0].failsafeMode);
      ui->FSGB_1->show();
    }
    updateSettings();

    ui->ppmDelaySB->setEnabled(!protocol);
    ui->numChannelsSB->setEnabled(!protocol);

    if (GetEepromInterface()->getCapability(HasPPMStart)) {
      ui->numChannelsSB->setSingleStep(1);
    } else {
      ui->numChannelsSB->setSingleStep(2);
    }

    switch (protocol) {
      case OFF:
        ui->label_PPM->hide();
        ui->ppmDelaySB->hide();
        ui->label_PPMCH->hide();
        ui->label_pulsePol->hide();
        ui->pulsePolCB->hide();
        ui->numChannelsSB->hide();
        ui->label_ppmFrameLength->hide();
        ui->ppmFrameLengthDSB->hide();
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        ui->label_PXX->hide();
        ui->pxxRxNum->hide();
        ui->label_numChannelsStart->hide();
        ui->numChannelsStart->hide();
        break;    
      case PXX_XJT_X16:
      case PXX_XJT_D8:
      case PXX_XJT_LR12:
      case PXX_DJT:
        ui->label_PPM->hide();
        ui->ppmDelaySB->hide();
        ui->ppmDelaySB->setEnabled(false);
        ui->label_PPMCH->show();
        ui->label_pulsePol->hide();
        ui->pulsePolCB->hide();
        ui->numChannelsSB->show();
        ui->numChannelsSB->setEnabled(true);
        ui->label_ppmFrameLength->hide();
        ui->ppmFrameLengthDSB->hide();
        ui->ppmFrameLengthDSB->setEnabled(false);
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        ui->DSM_Type->setEnabled(false);
        ui->label_PXX->show();
        ui->pxxRxNum->setMinimum(0);
        ui->pxxRxNum->show();
        ui->pxxRxNum->setEnabled(true);
        ui->pxxRxNum->setValue((g_model.moduleData[0].channelsCount-8)/2+1);
        ui->label_numChannelsStart->show();
        ui->numChannelsStart->show();
        ui->numChannelsStart->setValue(g_model.moduleData[0].channelsStart+1);
        ui->numChannelsSB->setMinimum(g_model.moduleData[0].channelsStart+4);
        ui->numChannelsSB->setValue(g_model.moduleData[0].channelsStart+g_model.moduleData[0].channelsCount);
        break;
      case LP45:
      case DSM2:
      case DSMX:
        ui->label_pulsePol->hide();
        ui->pulsePolCB->hide();
        ui->label_PPM->hide();
        ui->ppmDelaySB->hide();
        ui->ppmDelaySB->setEnabled(false);
        ui->label_PPMCH->hide();
        ui->label_numChannelsStart->hide();
        ui->numChannelsStart->hide();
        ui->numChannelsSB->hide();
        ui->numChannelsSB->setEnabled(false);
        ui->label_ppmFrameLength->hide();
        ui->ppmFrameLengthDSB->hide();
        ui->ppmFrameLengthDSB->setEnabled(false);
        if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
          ui->label_PXX->hide();
          ui->pxxRxNum->hide();
          ui->pxxRxNum->setEnabled(false);
        }
        else {
          ui->pxxRxNum->setMinimum(0);
          ui->pxxRxNum->setValue(g_model.modelId);
          ui->label_PXX->show();
          ui->pxxRxNum->show();
          ui->pxxRxNum->setEnabled(true);         
        }
        ui->DSM_Type->setEnabled(false);
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        break;
      default:
        if (GetEepromInterface()->getCapability(HasPPMStart)) {
          ui->label_numChannelsStart->show();
          ui->numChannelsStart->show();
        }
        else {
          ui->label_numChannelsStart->hide();
          ui->numChannelsStart->hide();
        }
        ui->numChannelsStart->setValue(g_model.moduleData[0].channelsStart+1);
        ui->numChannelsSB->setMinimum(g_model.moduleData[0].channelsStart+4);
        ui->numChannelsSB->setValue(g_model.moduleData[0].channelsStart + g_model.moduleData[0].channelsCount);
        ui->label_pulsePol->show();
        ui->pulsePolCB->show();
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        ui->DSM_Type->setEnabled(false);
        ui->label_PXX->hide();
        ui->pxxRxNum->hide();
        ui->pxxRxNum->setEnabled(false);
        ui->label_PPM->show();
        ui->ppmDelaySB->show();        
        ui->ppmDelaySB->setEnabled(true);
        ui->label_PPMCH->show();
        ui->numChannelsSB->show();        
        ui->numChannelsSB->setEnabled(true);
        ui->ppmFrameLengthDSB->setEnabled(true);
        ui->ppmFrameLengthDSB->setMinimum(g_model.moduleData[0].channelsCount*(g_model.extendedLimits ? 2.250 :2)+3.5);
        if (GetEepromInterface()->getCapability(PPMExtCtrl)) {
          ui->ppmFrameLengthDSB->show();
          ui->label_ppmFrameLength->show();
        }

        break;
    }
    protocolEditLock=false;
  }
}

void ModelEdit::on_protocolCB_2_currentIndexChanged(int index)
{
  if (!protocol2EditLock) {
    protocol2EditLock=true;
    
    g_model.moduleData[1].protocol=(Protocol)ui->protocolCB_2->itemData(index).toInt();
    int protocol=g_model.moduleData[1].protocol;
    if (protocol==PXX_XJT_X16 || protocol==PXX_XJT_LR12) {
      ui->FSGB_2->show();
      ui->fsm2CB->setCurrentIndex(g_model.moduleData[1].failsafeMode);
      on_fsm2CB_currentIndexChanged(g_model.moduleData[1].failsafeMode);
    } else {
      ui->FSGB_2->hide();
    }
  //  g_model.protocol = (Protocol)index;
    updateSettings();

    ui->ppmDelaySB_2->setEnabled(!protocol);
    ui->numChannelsSB_2->setEnabled(!protocol);

    if (GetEepromInterface()->getCapability(HasPPMStart)) {
      ui->numChannelsSB_2->setSingleStep(1);
    } else {
      ui->numChannelsSB_2->setSingleStep(2);
    }

    switch (protocol) {
      case OFF:
        ui->label_PPM_2->hide();
        ui->ppmDelaySB_2->hide();
        ui->label_PPMCH_2->hide();
        ui->label_pulsePol_2->hide();
        ui->pulsePolCB_2->hide();
        ui->numChannelsSB_2->hide();
        ui->label_ppmFrameLength_2->hide();
        ui->ppmFrameLengthDSB_2->hide();
        ui->label_DSM_2->hide();
        ui->DSM_Type_2->hide();
        ui->label_PXX_2->hide();
        ui->pxxRxNum_2->hide();
        ui->label_numChannelsStart_2->hide();
        ui->numChannelsStart_2->hide();
        break;    
      case PXX_XJT_X16:
      case PXX_XJT_D8:
      case PXX_XJT_LR12:
      case PXX_DJT:
        ui->label_PPM_2->hide();
        ui->ppmDelaySB_2->hide();
        ui->ppmDelaySB_2->setEnabled(false);
        ui->label_PPMCH_2->show();
        ui->label_pulsePol_2->hide();
        ui->pulsePolCB_2->hide();
        ui->numChannelsSB_2->show();
        ui->numChannelsSB_2->setEnabled(true);
        ui->label_ppmFrameLength_2->hide();
        ui->ppmFrameLengthDSB_2->hide();
        ui->ppmFrameLengthDSB_2->setEnabled(false);
        ui->label_DSM_2->hide();
        ui->DSM_Type_2->hide();
        ui->DSM_Type_2->setEnabled(false);
        ui->label_PXX_2->show();
        ui->pxxRxNum_2->setMinimum(0);
        ui->pxxRxNum_2->show();
        ui->pxxRxNum_2->setEnabled(true);
        ui->pxxRxNum_2->setValue(g_model.modelId);
        ui->label_numChannelsStart_2->show();
        ui->numChannelsStart_2->show();
        ui->numChannelsStart_2->setValue(g_model.moduleData[1].channelsStart+1);
        ui->numChannelsSB_2->setMinimum(g_model.moduleData[1].channelsStart+4);
        ui->numChannelsSB_2->setValue(g_model.moduleData[1].channelsStart+g_model.moduleData[1].channelsCount);
        break;
      case LP45:
      case DSM2:
      case DSMX:
        ui->label_pulsePol_2->hide();
        ui->pulsePolCB_2->hide();
        ui->label_PPM_2->hide();
        ui->ppmDelaySB_2->hide();
        ui->ppmDelaySB_2->setEnabled(false);
        ui->label_PPMCH_2->hide();
        ui->numChannelsSB_2->hide();
        ui->numChannelsSB_2->setEnabled(false);
        ui->label_ppmFrameLength_2->hide();
        ui->ppmFrameLengthDSB_2->hide();
        ui->ppmFrameLengthDSB_2->setEnabled(false);
        if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
          ui->label_PXX_2->hide();
          ui->pxxRxNum_2->hide();
          ui->pxxRxNum_2->setEnabled(false);
        } else {
          ui->pxxRxNum_2->setMinimum(0);
          ui->pxxRxNum_2->setValue(g_model.modelId);
          ui->label_PXX_2->show();
          ui->pxxRxNum_2->show();
          ui->pxxRxNum_2->setEnabled(true);         
        }
        ui->DSM_Type_2->setEnabled(false);
        ui->label_DSM_2->hide();
        ui->DSM_Type_2->hide();
        break;
      default:
        if (GetEepromInterface()->getCapability(HasPPMStart)) {
          ui->label_numChannelsStart_2->show();
          ui->numChannelsStart_2->show();
        } else {
          ui->label_numChannelsStart_2->hide();
          ui->numChannelsStart_2->hide();
        }
        ui->numChannelsStart_2->setValue(g_model.moduleData[1].channelsStart+1);
        ui->numChannelsSB_2->setMinimum(g_model.moduleData[1].channelsStart+4);
        ui->numChannelsSB_2->setValue(g_model.moduleData[1].channelsStart+g_model.moduleData[1].channelsCount);
        ui->label_pulsePol_2->show();
        ui->pulsePolCB_2->show();
        ui->pulsePolCB_2->setCurrentIndex(g_model.moduleData[1].ppmPulsePol);
        ui->label_DSM_2->hide();
        ui->DSM_Type_2->hide();
        ui->DSM_Type_2->setEnabled(false);
        ui->label_PXX_2->hide();
        ui->pxxRxNum_2->hide();
        ui->pxxRxNum_2->setEnabled(false);
        ui->label_PPM_2->show();
        ui->ppmDelaySB_2->show();        
        ui->ppmDelaySB_2->setEnabled(true);
        ui->ppmDelaySB_2->setValue(g_model.moduleData[1].ppmDelay);
        ui->label_PPMCH_2->show();
        ui->numChannelsSB_2->show();        
        ui->numChannelsSB_2->setEnabled(true);
        ui->ppmFrameLengthDSB_2->setEnabled(true);
        ui->ppmFrameLengthDSB_2->setMinimum(g_model.moduleData[1].channelsCount*(g_model.extendedLimits ? 2.250 :2)+3.5);
        if (GetEepromInterface()->getCapability(PPMExtCtrl)) {
          ui->ppmFrameLengthDSB_2->show();
          ui->label_ppmFrameLength_2->show();
          ui->ppmFrameLengthDSB_2->setValue(g_model.moduleData[1].ppmFrameLength/2.0+22.5);
        }
        break;
    }
    protocol2EditLock=false;
  }
}

void ModelEdit::on_protocolCB_3_currentIndexChanged(int index)
{
  if (!trainerEditLock) {
    trainerEditLock=true;
    
    g_model.moduleData[2].protocol=ui->protocolCB_3->currentIndex();
    g_model.trainerMode=ui->protocolCB_3->currentIndex();
    updateSettings();
    switch (index) {
      case 0:
        ui->label_PPM_3->hide();
        ui->ppmDelaySB_3->hide();
        ui->label_PPMCH_3->hide();
        ui->label_pulsePol_3->hide();
        ui->pulsePolCB_3->hide();
        ui->numChannelsSB_3->hide();
        ui->label_ppmFrameLength_3->hide();
        ui->ppmFrameLengthDSB_3->hide();
        ui->label_numChannelsStart_3->hide();
        ui->numChannelsStart_3->hide();
        break;    
      default:
        ui->label_PPM_3->show();
        ui->ppmDelaySB_3->show();
        ui->label_PPMCH_3->show();
        ui->label_pulsePol_3->show();
        ui->pulsePolCB_3->show();
        ui->numChannelsSB_3->show();
        ui->label_ppmFrameLength_3->show();
        ui->ppmFrameLengthDSB_3->show();
        ui->label_numChannelsStart_3->show();
        ui->numChannelsStart_3->show();
        ui->numChannelsStart_3->setValue(g_model.moduleData[2].channelsStart+1);
        ui->numChannelsSB_3->setMinimum(g_model.moduleData[2].channelsStart+4);
        ui->numChannelsSB_3->setValue(g_model.moduleData[2].channelsStart+g_model.moduleData[2].channelsCount);
        ui->pulsePolCB_3->setCurrentIndex(g_model.moduleData[2].ppmPulsePol);
        ui->ppmDelaySB_3->setValue(g_model.moduleData[2].ppmDelay);
        ui->ppmFrameLengthDSB_3->setMinimum(g_model.moduleData[2].channelsCount*(g_model.extendedLimits ? 2.250 :2)+3.5);
        if (GetEepromInterface()->getCapability(PPMExtCtrl)) {
          ui->ppmFrameLengthDSB_3->show();
          ui->label_ppmFrameLength_3->show();
          ui->ppmFrameLengthDSB_3->setValue(g_model.moduleData[2].ppmFrameLength/2.0+22.5);
        }
        break;
    }
    trainerEditLock=false;
  }
}


void ModelEdit::on_numChannelsSB_editingFinished()
{
    // TODO only accept valid values
    g_model.moduleData[0].channelsCount = 1+ui->numChannelsSB->value()-ui->numChannelsStart->value();
    ui->ppmFrameLengthDSB->setMinimum(g_model.moduleData[0].channelsCount*(g_model.extendedLimits ? 2.250 :2)+3.5);
    updateSettings();
}

void ModelEdit::on_numChannelsSB_2_editingFinished()
{
    // TODO only accept valid values
    g_model.moduleData[1].channelsCount = 1+ui->numChannelsSB_2->value()-ui->numChannelsStart_2->value();
    ui->ppmFrameLengthDSB_2->setMinimum(g_model.moduleData[1].channelsCount*(g_model.extendedLimits ? 2.250 :2.0)+3.5);
    updateSettings();
}

void ModelEdit::on_numChannelsSB_3_editingFinished()
{
    // TODO only accept valid values
    g_model.moduleData[2].channelsCount = 1+ui->numChannelsSB_3->value()-ui->numChannelsStart_3->value();
    ui->ppmFrameLengthDSB_3->setMinimum(g_model.moduleData[2].channelsCount*(g_model.extendedLimits ? 2.250 :2)+3.5);
    updateSettings();
}

void ModelEdit::on_numChannelsStart_editingFinished()
{
    // TODO only accept valid values
    g_model.moduleData[0].channelsStart = ui->numChannelsStart->value()-1;
    ui->numChannelsSB->setMinimum(g_model.moduleData[0].channelsStart+4);
    ui->numChannelsSB->setValue(g_model.moduleData[0].channelsStart+g_model.moduleData[0].channelsCount);
    updateSettings();
}

void ModelEdit::on_numChannelsStart_2_editingFinished()
{
    // TODO only accept valid values
    g_model.moduleData[1].channelsStart = ui->numChannelsStart_2->value()-1;
    ui->numChannelsSB_2->setMinimum(g_model.moduleData[1].channelsStart+4);
    ui->numChannelsSB_2->setValue(g_model.moduleData[1].channelsStart+g_model.moduleData[1].channelsCount);
    updateSettings();
}

void ModelEdit::on_numChannelsStart_3_editingFinished()
{
    // TODO only accept valid values
    g_model.moduleData[2].channelsStart = ui->numChannelsStart_3->value()-1;
    ui->numChannelsSB_3->setMinimum(g_model.moduleData[2].channelsStart+4);
    ui->numChannelsSB_3->setValue(g_model.moduleData[2].channelsStart+g_model.moduleData[2].channelsCount);
    updateSettings();
}

void ModelEdit::on_ppmDelaySB_editingFinished()
{
    if(protocolEditLock) return;
    // TODO only accept valid values
    g_model.moduleData[0].ppmDelay = ui->ppmDelaySB->value();
    updateSettings();
}

void ModelEdit::on_ppmDelaySB_2_editingFinished()
{
    if(protocol2EditLock) return;
    // TODO only accept valid values
    g_model.moduleData[1].ppmDelay = ui->ppmDelaySB_2->value();
    updateSettings();
}

void ModelEdit::on_ppmDelaySB_3_editingFinished()
{
    if(trainerEditLock) return;
    // TODO only accept valid values
    g_model.moduleData[2].ppmDelay = ui->ppmDelaySB_3->value();
    updateSettings();
}

void ModelEdit::on_DSM_Type_currentIndexChanged(int index)
{
    if(protocolEditLock) return;
    // g_model.moduleData[0].channelsCount = (index*2)+8;
    updateSettings();
}

void ModelEdit::on_DSM_Type_2_currentIndexChanged(int index)
{
    if(protocol2EditLock) return;
    // g_model.moduleData[1].channelsCount = (index*2)+8;
    updateSettings();
}

void ModelEdit::on_pxxRxNum_editingFinished()
{
    if(protocolEditLock)
      return;

    if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
      // g_model.moduleData[0].channelsCount = (ui->pxxRxNum->value()-1)*2+8;
    } else {
      g_model.modelId = ui->pxxRxNum->value();
    }
    updateSettings();
}

void ModelEdit::on_pxxRxNum_2_editingFinished()
{
    if(protocol2EditLock)
      return;

    /* if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
      g_model.moduleData[1].channelsCount = (ui->pxxRxNum_2->value()-1)*2+8;
    } */
    updateSettings();
}

void ModelEdit::on_ppmFrameLengthDSB_editingFinished()
{
    g_model.moduleData[0].ppmFrameLength = (ui->ppmFrameLengthDSB->value()-22.5)/0.5;
    updateSettings();
}

void ModelEdit::on_ppmFrameLengthDSB_2_editingFinished()
{
    g_model.moduleData[1].ppmFrameLength = (ui->ppmFrameLengthDSB_2->value()-22.5)/0.5;
    updateSettings();
}

void ModelEdit::on_ppmFrameLengthDSB_3_editingFinished()
{
    g_model.moduleData[2].ppmFrameLength = (ui->ppmFrameLengthDSB_3->value()-22.5)/0.5;
    updateSettings();
}

void ModelEdit::on_fsm1CB_currentIndexChanged(int index)
{
    QSpinBox * fssb[] = { ui->fsm1SB_1, ui->fsm1SB_2,ui->fsm1SB_3,ui->fsm1SB_4,ui->fsm1SB_5,ui->fsm1SB_6,ui->fsm1SB_7,ui->fsm1SB_8,
                                 ui->fsm1SB_9, ui->fsm1SB_10,ui->fsm1SB_11,ui->fsm1SB_12,ui->fsm1SB_13,ui->fsm1SB_14,ui->fsm1SB_15,ui->fsm1SB_16, NULL };
    QSlider * fssld[] = { ui->fsm1SL_1, ui->fsm1SL_2,ui->fsm1SL_3,ui->fsm1SL_4,ui->fsm1SL_5,ui->fsm1SL_6,ui->fsm1SL_7,ui->fsm1SL_8,
                                 ui->fsm1SL_9, ui->fsm1SL_10,ui->fsm1SL_11,ui->fsm1SL_12,ui->fsm1SL_13,ui->fsm1SL_14,ui->fsm1SL_15,ui->fsm1SL_16, NULL };
    g_model.moduleData[0].failsafeMode=index;
    for (int i=0; fssb[i]; i++) {
     if (index==1) {
        fssb[i]->setEnabled(true);
        fssld[i]->setEnabled(true);
      } else {
        fssb[i]->setDisabled(true);
        fssld[i]->setDisabled(true);     
      }
    }
    updateSettings();
}

void ModelEdit::on_fsm2CB_currentIndexChanged(int index)
{
    QSpinBox * fssb[] = { ui->fsm2SB_1, ui->fsm2SB_2,ui->fsm2SB_3,ui->fsm2SB_4,ui->fsm2SB_5,ui->fsm2SB_6,ui->fsm2SB_7,ui->fsm2SB_8,
                                 ui->fsm2SB_9, ui->fsm2SB_10,ui->fsm2SB_11,ui->fsm2SB_12,ui->fsm2SB_13,ui->fsm2SB_14,ui->fsm2SB_15,ui->fsm2SB_16, NULL };
    QSlider * fssld[] = { ui->fsm2SL_1, ui->fsm2SL_2,ui->fsm2SL_3,ui->fsm2SL_4,ui->fsm2SL_5,ui->fsm2SL_6,ui->fsm2SL_7,ui->fsm2SL_8,
                                 ui->fsm2SL_9, ui->fsm2SL_10,ui->fsm2SL_11,ui->fsm2SL_12,ui->fsm2SL_13,ui->fsm2SL_14,ui->fsm2SL_15,ui->fsm2SL_16, NULL };  
    g_model.moduleData[1].failsafeMode=index;
    for (int i=0; fssb[i]; i++) {
     if (index==1) {
        fssb[i]->setEnabled(true);
        fssld[i]->setEnabled(true);
      } else {
        fssb[i]->setDisabled(true);
        fssld[i]->setDisabled(true);     
      }
    }
    updateSettings();
}

void ModelEdit::on_instantTrim_CB_currentIndexChanged(int index)
{
    if(switchEditLock) return;
    bool found=false;
    for (int i=0; i< C9X_MAX_CUSTOM_FUNCTIONS; i++) {
      if (g_model.funcSw[i].func==FuncInstantTrim) {
        g_model.funcSw[i].swtch = RawSwitch(ui->instantTrim_CB->itemData(ui->instantTrim_CB->currentIndex()).toInt());
        found=true;
      }
    }
    if (found==false) {
      for (int i=0; i< C9X_MAX_CUSTOM_FUNCTIONS; i++) {
        if (g_model.funcSw[i].swtch==RawSwitch()) {
          g_model.funcSw[i].swtch = RawSwitch(ui->instantTrim_CB->itemData(ui->instantTrim_CB->currentIndex()).toInt());
          g_model.funcSw[i].func = FuncInstantTrim;
          break;
        }
      }
    }
    updateSettings();
}

void ModelEdit::on_a1UnitCB_currentIndexChanged(int index) {
    float a1ratio;
    g_model.frsky.channels[0].type=index;
    switch (index) {
      case 0:
      case 1:
      case 2:
        ui->a1RatioSB->setDecimals(1);
        ui->a1RatioSB->setMaximum(25.5*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
        a1ratio=(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier)/10.0;
        break;
      default:
        ui->a1RatioSB->setDecimals(0);
        ui->a1RatioSB->setMaximum(255*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
        a1ratio=(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier);
        break;
    }
    ui->a1RatioSB->setValue(a1ratio);
    updateA1Fields();
    updateSettings();
}

void ModelEdit::on_a1RatioSB_valueChanged()
{
    if (telemetryLock) return;
    if (g_model.frsky.channels[0].type==0 || g_model.frsky.channels[0].type==1 || g_model.frsky.channels[0].type==2) {
      g_model.frsky.channels[0].multiplier = findmult(ui->a1RatioSB->value(), 25.5);
      float singlestep =(1<<g_model.frsky.channels[0].multiplier)/10.0;
      telemetryLock=true;
      ui->a1RatioSB->setSingleStep(singlestep);
      ui->a1RatioSB->setValue(round(ui->a1RatioSB->value()/singlestep)*singlestep);
      telemetryLock=false;
    } else {
      g_model.frsky.channels[0].multiplier = findmult(ui->a1RatioSB->value(), 255);
      float singlestep =(1<<g_model.frsky.channels[0].multiplier);
      telemetryLock=true;
      ui->a1RatioSB->setSingleStep(singlestep);
      ui->a1RatioSB->setValue(round(ui->a1RatioSB->value()/singlestep)*singlestep);
      telemetryLock=false;
    }  
}

void ModelEdit::on_a1RatioSB_editingFinished()
{
    float a1ratio, a1calib, a11value,a12value;
    if (telemetryLock) return;
    if (g_model.frsky.channels[0].type==0 || g_model.frsky.channels[0].type==1 || g_model.frsky.channels[0].type==2) {
      g_model.frsky.channels[0].multiplier = findmult(ui->a1RatioSB->value(), 25.5);
      ui->a1CalibSB->setSingleStep((1<<g_model.frsky.channels[0].multiplier)/10.0);
      ui->a11ValueSB->setSingleStep((1<<g_model.frsky.channels[0].multiplier)/10.0);
      ui->a12ValueSB->setSingleStep((1<<g_model.frsky.channels[0].multiplier)/10.0);
      g_model.frsky.channels[0].ratio = ((int)(round(ui->a1RatioSB->value()*10))/(1 <<g_model.frsky.channels[0].multiplier));
    }  else {
      g_model.frsky.channels[0].multiplier = findmult(ui->a1RatioSB->value(), 255);
      ui->a1CalibSB->setSingleStep(1<<g_model.frsky.channels[0].multiplier);
      ui->a11ValueSB->setSingleStep(1<<g_model.frsky.channels[0].multiplier);
      ui->a12ValueSB->setSingleStep(1<<g_model.frsky.channels[0].multiplier);
      g_model.frsky.channels[0].ratio = (ui->a1RatioSB->value()/(1 << g_model.frsky.channels[0].multiplier));
    }
    ui->a1CalibSB->setMaximum((ui->a1RatioSB->value()*127)/255);
    ui->a1CalibSB->setMinimum((ui->a1RatioSB->value()*-128)/255);
    ui->a11ValueSB->setMaximum(ui->a1RatioSB->value());
    ui->a12ValueSB->setMaximum(ui->a1RatioSB->value());
    repaint();
    a1ratio=g_model.frsky.channels[0].ratio * (1 << g_model.frsky.channels[0].multiplier);
    a1calib=ui->a1CalibSB->value();
    a11value=ui->a11ValueSB->value();
    a12value=ui->a12ValueSB->value();
    if (g_model.frsky.channels[0].type==0) {
      a1calib*=10;
      a11value*=10;
      a12value*=10; 
    }
    if (a1calib>0) {
      if (a1calib>((a1ratio*127)/255)) {
        g_model.frsky.channels[0].offset=127;
      } else {
        g_model.frsky.channels[0].offset=round(a1calib*255/a1ratio);
      }
    }
    if (a1calib<0) {
      if (a1calib<((a1ratio*-128)/255)) {
        g_model.frsky.channels[0].offset=-128;
      } else {
        g_model.frsky.channels[0].offset=round(a1calib*255/a1ratio);
      }
    }
    g_model.frsky.channels[0].alarms[0].value=round((a11value*255-g_model.frsky.channels[0].offset*(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier))/(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier));
    g_model.frsky.channels[0].alarms[1].value=round((a12value*255-g_model.frsky.channels[0].offset*(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier))/(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier)); 
    updateA1Fields();
    telBarUpdate();
    updateSettings();
}

void ModelEdit::on_a1CalibSB_editingFinished()
{
    float a1ratio,a1calib,a11value,a12value;
    if (g_model.frsky.channels[0].type==0 || g_model.frsky.channels[0].type==1 || g_model.frsky.channels[0].type==2) {
      a1ratio=(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier)/10.0;
    } else {
      a1ratio=g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier;
    }
    if (a1ratio!=0) {
      g_model.frsky.channels[0].offset = round((255*ui->a1CalibSB->value()/a1ratio));
      a1calib=a1ratio*g_model.frsky.channels[0].offset/255.0;
      a11value=ui->a11ValueSB->value();
      a12value=ui->a12ValueSB->value();
      if (a11value<a1calib) {
        a11value=a1calib;
      } else if (a11value>(a1ratio+a1calib)) {
        a11value=a1ratio+a1calib;
      }
      if (a12value<a1calib) {
        a12value=a1calib;
      } else if (a12value>(a1ratio+a1calib)) {
        a12value=a1ratio+a1calib;
      }
      g_model.frsky.channels[0].alarms[0].value=round(((a11value-a1calib)*255)/a1ratio);
      g_model.frsky.channels[0].alarms[1].value=round(((a12value-a1calib)*255)/a1ratio);
    } else {
      g_model.frsky.channels[0].offset=0;
      g_model.frsky.channels[0].alarms[0].value=0;
      g_model.frsky.channels[0].alarms[1].value=0;
    }
    updateA1Fields();
    telBarUpdate();
    updateSettings();
}

void ModelEdit::on_a11LevelCB_currentIndexChanged(int index)
{
    g_model.frsky.channels[0].alarms[0].level = index;
    updateSettings();
}

void ModelEdit::on_frskyUnitsCB_currentIndexChanged(int index)
{
    g_model.frsky.imperial=index;
    updateSettings();
}

void ModelEdit::on_frskyBladesCB_currentIndexChanged(int index)
{
    g_model.frsky.blades=index;
    updateSettings();
}

void ModelEdit::on_frskyCurrentCB_currentIndexChanged(int index)
{
    g_model.frsky.currentSource=index;
    updateSettings();
}

void ModelEdit::on_frskyVoltCB_currentIndexChanged(int index)
{
    g_model.frsky.voltsSource=index;
    updateSettings();
}

void ModelEdit::on_frskyProtoCB_currentIndexChanged(int index)
{
    if (telemetryLock) return;
    int bindex[12];
    telemetryLock=true;
    for (int i=0; i<12; i++) {
      bindex[i]=barsCB[i]->currentIndex();
      g_model.frsky.usrProto=index;
      populateCustomScreenFieldCB(barsCB[i], bindex[i], false, g_model.frsky.usrProto);
    }
    if (!GetEepromInterface()->getCapability(TelemetryCSFields)) {
      ui->groupBox_5->hide();
    } else {
      for (int screen=0; screen<2;screen++) {
        for (int rows=0; rows<4; rows++) {
          for (int cols=0; cols<3; cols++) {
            int index=screen*12+cols*4+rows;
            populateCustomScreenFieldCB(csf[index], g_model.frsky.screens[screen].body.lines[rows].source[cols], (rows<4), g_model.frsky.usrProto);
          }
        }
      }
    }
    telemetryLock=false;
    if (index==0) {
      for (int i=0; i<12; i++) {
        if (bindex[i]>2) {
          barsCB[i]->setCurrentIndex(0);
        } else {
          barsCB[i]->setCurrentIndex(bindex[i]);
        }
      }
    } else if (index==2) {
      for (int i=0; i<12; i++) {
        if (bindex[i]>3) {
          barsCB[i]->setCurrentIndex(0);
        } else {
          barsCB[i]->setCurrentIndex(bindex[i]);
        }
      }
    } else {
      for (int i=0; i<12; i++) {
        barsCB[i]->setCurrentIndex(bindex[i]);
      }
    }
    updateSettings();
}

void ModelEdit::on_a11GreaterCB_currentIndexChanged(int index)
{
    g_model.frsky.channels[0].alarms[0].greater = index;
    updateSettings();
}

void ModelEdit::on_a11ValueSB_editingFinished()
{
    float a1ratio, a1calib, a11value;
    a1ratio=g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier;
    a1calib=g_model.frsky.channels[0].offset;
    a11value=ui->a11ValueSB->value();
    if (g_model.frsky.channels[0].type==0) {
      a1ratio/=10;
    }
    if (a11value<((a1calib*a1ratio)/255)) {
      g_model.frsky.channels[0].alarms[0].value=0;
    } else if (a11value>(a1ratio+(a1calib*a1ratio)/255)) {
      g_model.frsky.channels[0].alarms[0].value=255;
    } else {
      g_model.frsky.channels[0].alarms[0].value = round((a11value-((a1calib*a1ratio)/255))/a1ratio*255);
    }
    updateA1Fields();
    updateSettings();
}

void ModelEdit::on_a12LevelCB_currentIndexChanged(int index)
{
    g_model.frsky.channels[0].alarms[1].level = index;
    updateSettings();
}

void ModelEdit::on_a12GreaterCB_currentIndexChanged(int index)
{
    g_model.frsky.channels[0].alarms[1].greater = index;
    updateSettings();
}

void ModelEdit::on_a12ValueSB_editingFinished()
{
    float a1ratio, a1calib, a12value;
    a1ratio=g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier;
    a1calib=g_model.frsky.channels[0].offset;
    a12value=ui->a12ValueSB->value();
    if (g_model.frsky.channels[0].type==0) {
      a1ratio/=10;
    }
    if (a12value<((a1calib*a1ratio)/255)) {
      g_model.frsky.channels[0].alarms[1].value=0;
    } else if (a12value>(a1ratio+(a1calib*a1ratio)/255)) {
      g_model.frsky.channels[0].alarms[1].value=255;
    } else {
      g_model.frsky.channels[0].alarms[1].value = round((a12value-((a1calib*a1ratio)/255))/a1ratio*255);
    }
    updateA1Fields();
    updateSettings();
}

void ModelEdit::on_a2UnitCB_currentIndexChanged(int index) {
    float a2ratio;
    g_model.frsky.channels[1].type=index;
    switch (index) {
      case 0:
      case 1:
      case 2:
        ui->a2RatioSB->setDecimals(1);
        ui->a2RatioSB->setMaximum(25.5*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
        a2ratio=(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier)/10.0;
        break;
      default:
        ui->a2RatioSB->setDecimals(0);
        ui->a2RatioSB->setMaximum(255*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
        a2ratio=g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier;
        break;
    }
    ui->a2RatioSB->setValue(a2ratio);
    updateA2Fields();
    updateSettings();
}

void ModelEdit::on_a2RatioSB_valueChanged()
{
    if (telemetryLock) return;
    if (g_model.frsky.channels[1].type==0 || g_model.frsky.channels[1].type==1 || g_model.frsky.channels[1].type==2) {
      g_model.frsky.channels[1].multiplier = findmult(ui->a2RatioSB->value(), 25.5);
      float singlestep =(1<<g_model.frsky.channels[1].multiplier)/10.0;
      telemetryLock=true;
      ui->a2RatioSB->setSingleStep(singlestep);
      ui->a2RatioSB->setValue(round(ui->a2RatioSB->value()/singlestep)*singlestep);
      telemetryLock=false;
    } else {
      g_model.frsky.channels[1].multiplier = findmult(ui->a2RatioSB->value(), 255);
      float singlestep =(1<<g_model.frsky.channels[1].multiplier);
      telemetryLock=true;
      ui->a2RatioSB->setSingleStep(singlestep);
      ui->a2RatioSB->setValue(round(ui->a2RatioSB->value()/singlestep)*singlestep);
      telemetryLock=false;
    }  
}

void ModelEdit::on_a2RatioSB_editingFinished()
{
    float a2ratio, a2calib, a21value,a22value;
    if (telemetryLock) return;
    if (g_model.frsky.channels[1].type==0 || g_model.frsky.channels[1].type==1 || g_model.frsky.channels[1].type==2) {
      g_model.frsky.channels[1].multiplier = findmult(ui->a2RatioSB->value(), 25.5);
      ui->a2CalibSB->setSingleStep((1<<g_model.frsky.channels[1].multiplier)/10.0);
      ui->a21ValueSB->setSingleStep((1<<g_model.frsky.channels[1].multiplier)/10.0);
      ui->a22ValueSB->setSingleStep((1<<g_model.frsky.channels[1].multiplier)/10.0);
      g_model.frsky.channels[1].ratio = ((round(ui->a2RatioSB->value()*10))/(1 << g_model.frsky.channels[1].multiplier));
    } else {
      g_model.frsky.channels[1].multiplier = findmult(ui->a2RatioSB->value(), 255);
      ui->a2CalibSB->setSingleStep(1<<g_model.frsky.channels[1].multiplier);
      ui->a21ValueSB->setSingleStep(1<<g_model.frsky.channels[1].multiplier);
      ui->a22ValueSB->setSingleStep(1<<g_model.frsky.channels[1].multiplier);
      g_model.frsky.channels[1].ratio = (ui->a2RatioSB->value()/(1 << g_model.frsky.channels[1].multiplier));
    }
    ui->a2CalibSB->setMaximum((ui->a2RatioSB->value()*127)/255);
    ui->a2CalibSB->setMinimum((ui->a2RatioSB->value()*-128)/255);
    ui->a21ValueSB->setMaximum(ui->a2RatioSB->value());
    ui->a22ValueSB->setMaximum(ui->a2RatioSB->value());
    repaint();
    a2ratio=g_model.frsky.channels[1].ratio * (1 << g_model.frsky.channels[1].multiplier);
    a2calib=ui->a2CalibSB->value();
    a21value=ui->a21ValueSB->value();
    a22value=ui->a22ValueSB->value();
    if (g_model.frsky.channels[1].type==0 || g_model.frsky.channels[1].type==1 || g_model.frsky.channels[1].type==2) {
      a2calib*=10;
      a21value*=10;
      a22value*=10; 
    }
    if (a2calib>0) {
      if (a2calib>((a2ratio*127)/255)) {
        g_model.frsky.channels[1].offset=127;
      } else {
        g_model.frsky.channels[1].offset=round(a2calib*255/a2ratio);
      }
    }
    if (a2calib<0) {
      if (a2calib<((a2ratio*-128)/255)) {
        g_model.frsky.channels[1].offset=-128;
      } else {
        g_model.frsky.channels[1].offset=round(a2calib*255/a2ratio);
      }
    }
    g_model.frsky.channels[1].alarms[0].value=round((a21value*255-g_model.frsky.channels[1].offset*(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier))/(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier));
    g_model.frsky.channels[1].alarms[1].value=round((a22value*255-g_model.frsky.channels[1].offset*(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier))/(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier)); 
    updateA2Fields();
    telBarUpdate();
    updateSettings();
  }

  void ModelEdit::on_a2CalibSB_editingFinished()
  {
    float a2ratio,a2calib,a21value,a22value;
    if (g_model.frsky.channels[1].type==0 || g_model.frsky.channels[1].type==1 || g_model.frsky.channels[1].type==2) {
      a2ratio=(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier)/10.0;
    } else {
      a2ratio=g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier;
    }
    if (a2ratio!=0) {
      g_model.frsky.channels[1].offset = round((255*ui->a2CalibSB->value()/a2ratio));
      a2calib=a2ratio*g_model.frsky.channels[1].offset/255.0;
      a21value=ui->a21ValueSB->value();
      a22value=ui->a22ValueSB->value();
      if (a21value<a2calib) {
        a21value=a2calib;
      } else if (a21value>(a2ratio+a2calib)) {
        a21value=a2ratio+a2calib;
      }
      if (a22value<a2calib) {
        a22value=a2calib;
      } else if (a22value>(a2ratio+a2calib)) {
        a22value=a2ratio+a2calib;
      }
      g_model.frsky.channels[1].alarms[0].value=round(((a21value-a2calib)*255)/a2ratio);
      g_model.frsky.channels[1].alarms[1].value=round(((a22value-a2calib)*255)/a2ratio);
    } else {
      g_model.frsky.channels[1].offset=0;
      g_model.frsky.channels[1].alarms[0].value=0;
      g_model.frsky.channels[1].alarms[1].value=0;
    }
    updateA2Fields();
    telBarUpdate();
    updateSettings();
}

void ModelEdit::on_a21LevelCB_currentIndexChanged(int index)
{
    g_model.frsky.channels[1].alarms[0].level = index;
    updateSettings();
}

void ModelEdit::on_a21GreaterCB_currentIndexChanged(int index)
{
    g_model.frsky.channels[1].alarms[0].greater = index;
    updateSettings();
}

void ModelEdit::on_a21ValueSB_editingFinished()
{
    float a2ratio, a2calib, a21value;
    a2ratio=g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier;
    a2calib=g_model.frsky.channels[1].offset;
    a21value=ui->a21ValueSB->value();
    if (g_model.frsky.channels[1].type==0) {
      a2ratio/=10;
    }
    if (a21value<((a2calib*a2ratio)/255)) {
      g_model.frsky.channels[1].alarms[0].value=0;
    } else if (a21value>(a2ratio+(a2calib*a2ratio)/255)) {
      g_model.frsky.channels[1].alarms[0].value=255;
    } else {
      g_model.frsky.channels[1].alarms[0].value = round((a21value-((a2calib*a2ratio)/255))/a2ratio*255);
    }
    updateA2Fields();
    updateSettings();
}

void ModelEdit::on_a22LevelCB_currentIndexChanged(int index)
{
    g_model.frsky.channels[1].alarms[1].level = index;
    updateSettings();
}

void ModelEdit::on_a22GreaterCB_currentIndexChanged(int index)
{
    g_model.frsky.channels[1].alarms[1].greater = index;
    updateSettings();
}

void ModelEdit::on_a22ValueSB_editingFinished()
{
    float a2ratio, a2calib, a22value;
    a2ratio=g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier;
    a2calib=g_model.frsky.channels[1].offset;
    a22value=ui->a22ValueSB->value();
    if (g_model.frsky.channels[1].type==0) {
      a2ratio/=10;
    }
    if (a22value<((a2calib*a2ratio)/255)) {
      g_model.frsky.channels[1].alarms[1].value=0;
    } else if (a22value>(a2ratio+(a2calib*a2ratio)/255)) {
      g_model.frsky.channels[1].alarms[1].value=255;
    } else {
      g_model.frsky.channels[1].alarms[1].value = round((a22value-((a2calib*a2ratio)/255))/a2ratio*255);
    }
    updateA2Fields();
    updateSettings();
}

void ModelEdit::on_rssiAlarm1CB_currentIndexChanged(int index) {
    if (telemetryLock) return;
    g_model.frsky.rssiAlarms[0].level=index;
    updateSettings();
}

void ModelEdit::on_rssiAlarm2CB_currentIndexChanged(int index) {
    if (telemetryLock) return;
    g_model.frsky.rssiAlarms[1].level=index;
    updateSettings();
}

void ModelEdit::on_rssiAlarm1SB_editingFinished() {
    if (telemetryLock) return;
    g_model.frsky.rssiAlarms[0].value=(ui->rssiAlarm1SB->value());
    updateSettings();  
}

void ModelEdit::on_rssiAlarm2SB_editingFinished() {
    if (telemetryLock) return;
    g_model.frsky.rssiAlarms[1].value=(ui->rssiAlarm2SB->value());
    updateSettings();    
}

void ModelEdit::on_AltitudeGPS_ChkB_toggled(bool checked)
{
    if (telemetryLock) return;
    g_model.frsky.FrSkyGpsAlt = checked;
    updateSettings();
    //AltitudeGPS_CB
}

void ModelEdit::on_showNames_Ckb_toggled(bool checked)
{
    tabMixes();
}
void ModelEdit::on_varioSourceCB_currentIndexChanged(int index)
{
    if (telemetryLock) return;
    g_model.frsky.varioSource = index;
    updateSettings();
}

void ModelEdit::on_varioLimitMin_DSB_editingFinished()
{
    if (telemetryLock) return;
    g_model.frsky.varioMin= round(ui->varioLimitMin_DSB->value()+10);
    updateSettings();    
}

void ModelEdit::on_varioLimitMax_DSB_editingFinished()
{
    if (telemetryLock) return;
    g_model.frsky.varioMax= round(ui->varioLimitMax_DSB->value()-10);
    updateSettings();    
}

void ModelEdit::on_varioLimitCenterMin_DSB_editingFinished()
{
    if (telemetryLock) return;
    if (ui->varioLimitCenterMin_DSB->value()>ui->varioLimitCenterMax_DSB->value()) {
      ui->varioLimitCenterMax_DSB->setValue(ui->varioLimitCenterMin_DSB->value());
    }
    g_model.frsky.varioCenterMin= round((ui->varioLimitCenterMin_DSB->value()+0.5)*10);
    updateSettings();    
}

void ModelEdit::on_varioLimitMinOff_ChkB_toggled(bool checked)
{
    if (telemetryLock) return;
    g_model.frsky.varioCenterMin = -16;
    if (!checked) {
      telemetryLock=true;
      ui->varioLimitCenterMin_DSB->setValue(-2.0);
      ui->varioLimitCenterMin_DSB->setEnabled(true);
      telemetryLock=false;
    } else {
      ui->varioLimitCenterMin_DSB->setValue(-2.0);
      ui->varioLimitCenterMin_DSB->setDisabled(true);
    }
    updateSettings();
}

void ModelEdit::on_varioLimitCenterMax_DSB_editingFinished()
{
    if (telemetryLock) return;
    if (ui->varioLimitCenterMin_DSB->value()>ui->varioLimitCenterMax_DSB->value()) {
      ui->varioLimitCenterMax_DSB->setValue(ui->varioLimitCenterMin_DSB->value());
    }
    g_model.frsky.varioCenterMax= round((ui->varioLimitCenterMax_DSB->value()-0.5)*10);
    updateSettings();    
}

void ModelEdit::telBarUpdate() 
{
    int index;
    telemetryLock=true;
    for (int i=0; i<12; i++) {
      int screen=i/4;
      index=barsCB[i]->currentIndex();
      if (index==TELEMETRY_SOURCE_A1 || index==TELEMETRY_SOURCE_A1 || index==TELEMETRY_SOURCE_A1_MIN || index==TELEMETRY_SOURCE_A2_MIN) {
        minSB[i]->setMinimum(getBarValue(index, 0, &g_model.frsky));
        minSB[i]->setMaximum(getBarValue(index, 255, &g_model.frsky));
        minSB[i]->setSingleStep(getBarStep(index));
        maxSB[i]->setMinimum(getBarValue(index, 0, &g_model.frsky));
        maxSB[i]->setMaximum(getBarValue(index, 255, &g_model.frsky));
        maxSB[i]->setSingleStep(getBarStep(index));
        minSB[i]->setValue(getBarValue(index, g_model.frsky.screens[screen].body.bars[i%4].barMin, &g_model.frsky));
        maxSB[i]->setValue(getBarValue(index, 255-g_model.frsky.screens[screen].body.bars[i%4].barMax, &g_model.frsky));
      }
    }
    telemetryLock=false;
}

void ModelEdit::ScreenTypeCBcurrentIndexChanged(int index)
{
    if (telemetryLock) return;

    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int screen = comboBox->objectName().right(1).toInt() -1;
    telemetryLock=true;
    g_model.frsky.screens[screen].type=index;

    for (int i=0; i<3; i++) {
      bool isNum = (g_model.frsky.screens[i].type==0);
      barsGB[i]->setVisible(!isNum);
      numsGB[i]->setVisible(isNum);
    }

    telemetryLock=false;
    updateSettings();
}

void ModelEdit::telBarCBcurrentIndexChanged(int index)
{
    if (telemetryLock) return;
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int screenId = comboBox->objectName().mid(8,1).toInt() - 1;
    int barId = comboBox->objectName().mid(10,1).toInt() - 1;
    int bar=barId+screenId*4;
    g_model.frsky.screens[screenId].body.bars[barId].source=index;
    telemetryLock=true;
    if (index==0) {
      g_model.frsky.screens[screenId].body.bars[barId].barMin=0;
      g_model.frsky.screens[screenId].body.bars[barId].barMax=0;
      minSB[bar]->setDisabled(true);
      maxSB[bar]->setDisabled(true);
    } else {
      minSB[bar]->setEnabled(true);
      maxSB[bar]->setEnabled(true);    
    }
    switch (index-1) {
      case TELEMETRY_SOURCE_TX_BATT:
      case TELEMETRY_SOURCE_A1:
      case TELEMETRY_SOURCE_A1_MIN:
      case TELEMETRY_SOURCE_A2:
      case TELEMETRY_SOURCE_A2_MIN:
      case TELEMETRY_SOURCE_CELLS_SUM:
      case TELEMETRY_SOURCE_VFAS:
      case TELEMETRY_SOURCE_CURRENT_MAX:
      case TELEMETRY_SOURCE_CURRENT:
        minSB[bar]->setDecimals(1);
        maxSB[bar]->setDecimals(1);
        break;
      case TELEMETRY_SOURCE_CELL:
        minSB[bar]->setDecimals(2);
        maxSB[bar]->setDecimals(2);
        break;
      default:
        minSB[bar]->setDecimals(0);
        maxSB[bar]->setDecimals(0);
    }  
    minSB[bar]->setMinimum(getBarValue(index, 0, &g_model.frsky));
    minSB[bar]->setMaximum(getBarValue(index, 255, &g_model.frsky));
    minSB[bar]->setSingleStep(getBarStep(index));
    maxSB[bar]->setMinimum(getBarValue(index, 0, &g_model.frsky));
    maxSB[bar]->setMaximum(getBarValue(index, 255, &g_model.frsky));
    maxSB[bar]->setSingleStep(getBarStep(index));
    minSB[bar]->setValue(getBarValue(index, g_model.frsky.screens[screenId].body.bars[barId].barMin, &g_model.frsky));
    maxSB[bar]->setValue(getBarValue(index, 255-g_model.frsky.screens[screenId].body.bars[barId].barMax, &g_model.frsky));
    telemetryLock=false;
    updateSettings();
}

void ModelEdit::telMinSBeditingFinished()
{
    if (telemetryLock) return;
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int screenId = spinBox->objectName().mid(8,1).toInt() - 1;
    int barId = spinBox->objectName().right(1).toInt() - 1;
    int minId = barId+screenId*4;
    telemetryLock=true;
    if (g_model.frsky.screens[screenId].body.bars[barId].source==TELEMETRY_SOURCE_A1 || g_model.frsky.screens[screenId].body.bars[barId].source==TELEMETRY_SOURCE_A1_MIN) {
     g_model.frsky.screens[screenId].body.bars[barId].barMin=round((minSB[minId]->value()-ui->a1CalibSB->value())/getBarStep(g_model.frsky.screens[screenId].body.bars[barId].source));
    } else if (g_model.frsky.screens[screenId].body.bars[minId].source==TELEMETRY_SOURCE_A2 || g_model.frsky.screens[screenId].body.bars[minId].source==TELEMETRY_SOURCE_A2_MIN) {
     g_model.frsky.screens[screenId].body.bars[barId].barMin=round((minSB[minId]->value()-ui->a2CalibSB->value())/getBarStep(g_model.frsky.screens[screenId].body.bars[barId].source));
    } else {
     g_model.frsky.screens[screenId].body.bars[barId].barMin=round((minSB[minId]->value()-getBarValue(g_model.frsky.screens[screenId].body.bars[barId].source, 0, &g_model.frsky))/getBarStep(g_model.frsky.screens[screenId].body.bars[barId].source));
    }
    spinBox->setValue(getBarValue(g_model.frsky.screens[screenId].body.bars[barId].source, g_model.frsky.screens[screenId].body.bars[barId].barMin, &g_model.frsky));
    if (maxSB[minId]->value()<minSB[minId]->value()) {
      g_model.frsky.screens[screenId].body.bars[minId].barMax=(255-g_model.frsky.screens[screenId].body.bars[barId].barMin+1);
      maxSB[minId]->setValue(getBarValue(g_model.frsky.screens[screenId].body.bars[barId].source, 255-g_model.frsky.screens[screenId].body.bars[barId].barMax, &g_model.frsky));
    }
    maxSB[minId]->setMinimum(getBarValue(g_model.frsky.screens[screenId].body.bars[barId].source, (g_model.frsky.screens[screenId].body.bars[barId].barMin+1), &g_model.frsky));
    telemetryLock=false;
    updateSettings();
}

void ModelEdit::telMaxSBeditingFinished()
{
    if (telemetryLock) return;
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int screenId = spinBox->objectName().mid(8,1).toInt() - 1;
    int barId = spinBox->objectName().right(1).toInt() - 1;
    telemetryLock=true;
    if (g_model.frsky.screens[screenId].body.bars[barId].source==5) {
      g_model.frsky.screens[screenId].body.bars[barId].barMax = (255-round((spinBox->value()-ui->a1CalibSB->value())/getBarStep(g_model.frsky.screens[screenId].body.bars[barId].source)));
    } else if (g_model.frsky.screens[screenId].body.bars[barId].source==6) {
      g_model.frsky.screens[screenId].body.bars[barId].barMax = (255-round((spinBox->value()-ui->a2CalibSB->value())/getBarStep(g_model.frsky.screens[screenId].body.bars[barId].source)));
    } else {
      g_model.frsky.screens[screenId].body.bars[barId].barMax = (255-round((spinBox->value()-getBarValue(g_model.frsky.screens[screenId].body.bars[barId].source, 0, &g_model.frsky))/getBarStep(g_model.frsky.screens[screenId].body.bars[barId].source) ));
    }
    spinBox->setValue(getBarValue(g_model.frsky.screens[screenId].body.bars[barId].source, (255-g_model.frsky.screens[screenId].body.bars[barId].barMax), &g_model.frsky));
    telemetryLock=false;
    updateSettings();
}

void ModelEdit::on_thrTrimChkB_toggled(bool checked)
{
    g_model.thrTrim = checked;
    updateSettings();
}

void ModelEdit::on_thrExpoChkB_toggled(bool checked)
{
    g_model.thrExpo = checked;
    updateSettings();
}

void ModelEdit::on_AltitudeToolbar_ChkB_toggled(bool checked)
{
    g_model.frsky.altitudeDisplayed = checked;
    updateSettings();
}

void ModelEdit::on_bcRUDChkB_toggled(bool checked)
{
    if(checked) {
      g_model.beepANACenter |= BC_BIT_RUD;
    } else {
      g_model.beepANACenter &= ~BC_BIT_RUD;
    }
    updateSettings();
}

void ModelEdit::on_bcELEChkB_toggled(bool checked)
{
    if(checked) {
      g_model.beepANACenter |= BC_BIT_ELE;
    } else {
      g_model.beepANACenter &= ~BC_BIT_ELE;
    }
    updateSettings();
}

void ModelEdit::on_bcTHRChkB_toggled(bool checked)
{
    if(checked) {
      g_model.beepANACenter |= BC_BIT_THR;
    } else {
      g_model.beepANACenter &= ~BC_BIT_THR;
    }
    updateSettings();
}

void ModelEdit::on_bcAILChkB_toggled(bool checked)
{
    if(checked) {
      g_model.beepANACenter |= BC_BIT_AIL;
    } else {
      g_model.beepANACenter &= ~BC_BIT_AIL;
    }
    updateSettings();
}

void ModelEdit::on_bcP1ChkB_toggled(bool checked)
{
    if(checked) {
      g_model.beepANACenter |= BC_BIT_P1;
    } else {
      g_model.beepANACenter &= ~BC_BIT_P1;
    }
    updateSettings();
}

void ModelEdit::on_bcP2ChkB_toggled(bool checked)
{
    if(checked) {
      g_model.beepANACenter |= BC_BIT_P2;
    } else {
      g_model.beepANACenter &= ~BC_BIT_P2;
    }
    updateSettings();
}

void ModelEdit::on_bcP3ChkB_toggled(bool checked)
{
    if(checked) {
      g_model.beepANACenter |= BC_BIT_P3;
    } else {
      g_model.beepANACenter &= ~BC_BIT_P3;
    }
    updateSettings();
}

void ModelEdit::on_bcP4ChkB_toggled(bool checked)
{
    if(checked) {
      g_model.beepANACenter |= BC_BIT_P4;
    } else {
      g_model.beepANACenter &= ~BC_BIT_P4;
    }
    updateSettings();
}

void ModelEdit::on_bcREaChkB_toggled(bool checked)
{
    if(checked) {
      g_model.beepANACenter |= BC_BIT_REA;
    } else {
      g_model.beepANACenter &= ~BC_BIT_REA;
    }
    updateSettings();
}

void ModelEdit::on_bcREbChkB_toggled(bool checked)
{
    if(checked) {
      g_model.beepANACenter |= BC_BIT_REB;
    } else {
      g_model.beepANACenter &= ~BC_BIT_REB;
    }
    updateSettings();
}

void ModelEdit::phaseGVValue_editingFinished()
{
    if (phasesLock) return;
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    int phase = spinBox->objectName().mid(5,1).toInt();
    int gvar = spinBox->objectName().mid(8,1).toInt()-1;
    g_model.phaseData[phase].gvars[gvar] = spinBox->value();
    updateSettings();
}

void ModelEdit::GVName_editingFinished()
{
    if (phasesLock) return;
    QLineEdit *lineedit = qobject_cast<QLineEdit*>(sender());
    int gvar = lineedit->objectName().mid(8,1).toInt()-1;
    memset(&g_model.gvars_names[gvar],0,sizeof(g_model.gvars_names[gvar]));
    const char * le=lineedit->text().toAscii();
    strncpy(g_model.gvars_names[gvar], le, sizeof(g_model.gvars_names[gvar])-1);
    updateSettings();
}

void ModelEdit::GVSource_currentIndexChanged()
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int gvar = comboBox->objectName().mid(8,1).toInt()-1;
    int index=comboBox->currentIndex();
    g_model.gvsource[gvar]=index;
    updateSettings();
}

void ModelEdit::phaseGVUse_currentIndexChanged()
{
    if (phasesLock) return;
    phasesLock=true;
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int phase = comboBox->objectName().mid(5,1).toInt();
    int gvar = comboBox->objectName().mid(8,1).toInt()-1;
    int index=comboBox->currentIndex();
    if (index == 0) {
      int value=g_model.phaseData[phase].gvars[gvar];
      if (value>1024) {
        value=0;
      }
      gvarsSB[phase][gvar]->setValue(value);
      gvarsSB[phase][gvar]->setEnabled(true);
      g_model.phaseData[phase].gvars[gvar]=value;
    } else {
      g_model.phaseData[phase].gvars[gvar]=1024+index;
      int value=gvarsSB[index+(index>phase ?0 :-1)][gvar]->value();
      gvarsSB[phase][gvar]->setValue(value);
      gvarsSB[phase][gvar]->setDisabled(true);
    }
    phasesLock=false;
    updateSettings();
}

void ModelEdit::phaseREValue_editingFinished()
{
    if (phasesLock) return;
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    int phase = spinBox->objectName().mid(5,1).toInt();
    int gvar = spinBox->objectName().mid(8,1).toInt()-1;
    g_model.phaseData[phase].rotaryEncoders[gvar] = spinBox->value();
    updateSettings();
}

void ModelEdit::phaseREUse_currentIndexChanged()
{
    if (phasesLock) return;
    phasesLock=true;
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int phase = comboBox->objectName().mid(5,1).toInt();
    int re = comboBox->objectName().mid(8,1).toInt()-1;
    int index=comboBox->currentIndex();
    if (index == 0) {
      int value=g_model.phaseData[phase].rotaryEncoders[re];
      if (value>1024) {
        value=0;
      }
      reSB[phase][re]->setValue(value);
      reSB[phase][re]->setEnabled(true);
      g_model.phaseData[phase].rotaryEncoders[re]=value;
    } else {
      g_model.phaseData[phase].rotaryEncoders[re]=1024+index;
      int value=reSB[index+(index>phase ?0 :-1)][re]->value();
      reSB[phase][re]->setValue(value);
      reSB[phase][re]->setDisabled(true)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ;
    }
    phasesLock=false;
    updateSettings();
}

void ModelEdit::phaseTrimUse_currentIndexChanged()
{
    if (phasesLock) return;
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int phase = comboBox->objectName().mid(5,1).toInt();
    int trim = comboBox->objectName().mid(10,1).toInt();

    int chn = CONVERT_MODE(trim)-1;
    int index=comboBox->currentIndex();

    if (index == 0) {
      g_model.phaseData[phase].trim[chn] = g_model.phaseData[g_model.getTrimFlightPhase(chn, phase)].trim[chn];
      g_model.phaseData[phase].trimRef[chn] = -1;
    } else {
      g_model.phaseData[phase].trim[chn] = 0;
      g_model.phaseData[phase].trimRef[chn] = index - 1 + (index > (int)phase ? 1 : 0);
    }

    phasesLock = true;
    displayOnePhaseOneTrim(phase, chn, NULL, phasesTrimValues[phase][trim-1], phasesTrimSliders[phase][trim-1]);
    phasesLock = false;
    updateSettings();
}

void ModelEdit::phaseTrim_valueChanged()
{
    if (phasesLock) return;
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    int phase = spinBox->objectName().mid(5,1).toInt();
    int trim = spinBox->objectName().mid(10,1).toInt();

    int chn = CONVERT_MODE(trim)-1;
    g_model.phaseData[phase].trim[chn] = spinBox->value();
    phasesLock=true;
    phasesTrimSliders[phase][trim-1]->setValue(spinBox->value());
    phasesLock=false;
    updateSettings();
}

void ModelEdit::phaseTrimSlider_valueChanged()
{
    if (phasesLock) return;
    QSlider *slider = qobject_cast<QSlider*>(sender());
    int phase = slider->objectName().mid(5,1).toInt();
    int trim = slider->objectName().mid(10,1).toInt();

    int chn = CONVERT_MODE(trim)-1;
    g_model.phaseData[phase].trim[chn] = slider->value();
    phasesLock=true;
    phasesTrimValues[phase][trim-1]->setValue(slider->value());
    phasesLock=false;
    updateSettings();
}

QSpinBox *ModelEdit::getNodeSB(int i)   // get the SpinBox that corresponds to the selected node
{
    return spny[i];
}

QSpinBox *ModelEdit::getNodeSBX(int i)   // get the SpinBox that corresponds to the selected node
{
     return spnx[i];
}

void ModelEdit::drawCurve()
{
    
    if (drawing)
        return;
    drawing=true;
    curvesLock=true;
    int k,i;
    QColor * plot_color[16];

    plot_color[0]=new QColor(0,0,127);
    plot_color[1]=new QColor(0,127,0);
    plot_color[2]=new QColor(127,0,0);
    plot_color[3]=new QColor(0,127,127);
    plot_color[4]=new QColor(127,0,127);
    plot_color[5]=new QColor(127,127,0);
    plot_color[6]=new QColor(127,127,127);
    plot_color[7]=new QColor(0,0,255);
    plot_color[8]=new QColor(0,127,255);
    plot_color[9]=new QColor(127,0,255);
    plot_color[10]=new QColor(0,255,0);
    plot_color[11]=new QColor(0,255,127);
    plot_color[12]=new QColor(127,255,0);
    plot_color[13]=new QColor(255,0,0);
    plot_color[14]=new QColor(255,0,127);
    plot_color[15]=new QColor(255,127,0);

    if(currentCurve<0 || currentCurve>15) return;

    Node *nodel = 0;
    Node *nodex = 0;

    QGraphicsScene *scene = ui->curvePreview->scene();
    QPen pen;
    QColor color;
    scene->clear();

    qreal width  = scene->sceneRect().width();
    qreal height = scene->sceneRect().height();

    qreal centerX = scene->sceneRect().left() + width/2; //center X
    qreal centerY = scene->sceneRect().top() + height/2; //center Y

    QGraphicsSimpleTextItem *ti;
    ti = scene->addSimpleText(tr("Editing curve %1").arg(currentCurve+1));
    ti->setPos(3,3);

    scene->addLine(centerX,GFX_MARGIN,centerX,height+GFX_MARGIN);
    scene->addLine(GFX_MARGIN,centerY,width+GFX_MARGIN,centerY);
    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);
    int numcurves=GetEepromInterface()->getCapability(NumCurves);
    if (numcurves==0) {
      numcurves=16;
    }
    
    for(k=0; k<numcurves; k++) {
      pen.setColor(*plot_color[k]);
      if ((currentCurve!=k) && (plot_curve[k])) {
        int numpoints=g_model.curves[k].count;
        for(i=0; i<(numpoints-1); i++) {
          if (!g_model.curves[k].custom) {
            scene->addLine(GFX_MARGIN + i*width/(numpoints-1),centerY - (qreal)g_model.curves[k].points[i].y*height/200,GFX_MARGIN + (i+1)*width/(numpoints-1),centerY - (qreal)g_model.curves[k].points[i+1].y*height/200,pen);
          } else {
            scene->addLine(centerX + (qreal)g_model.curves[k].points[i].x*width/200,centerY - (qreal)g_model.curves[k].points[i].y*height/200,centerX + (qreal)g_model.curves[k].points[i+1].x*width/200,centerY - (qreal)g_model.curves[k].points[i+1].y*height/200,pen);
          }
        }
      }
    }
    int numpoints=g_model.curves[currentCurve].count;

    for(int i=0; i<numpoints; i++) {
      nodel = nodex;
      nodex = new Node(getNodeSB(i),getNodeSBX(i));
      nodex->setColor(*plot_color[currentCurve]);
      if (!g_model.curves[currentCurve].custom) {
        nodex->setFixedX(true);
        nodex->setPos(GFX_MARGIN + i*width/(numpoints-1),centerY - (qreal)g_model.curves[currentCurve].points[i].y*height/200);
      } else {
        if (i>0 && i<(numpoints-1)) {
          nodex->setFixedX(false);
          nodex->setMinX(g_model.curves[currentCurve].points[i-1].x+1);
          nodex->setMaxX(g_model.curves[currentCurve].points[i+1].x-1);
          spnx[i]->setMaximum(g_model.curves[currentCurve].points[i+1].x-1);
          spnx[i]->setMinimum(g_model.curves[currentCurve].points[i-1].x+1);
        } else {
          nodex->setFixedX(true);
        }
        nodex->setPos(centerX + (qreal)g_model.curves[currentCurve].points[i].x*width/200,centerY - (qreal)g_model.curves[currentCurve].points[i].y*height/200);
      }
      scene->addItem(nodex);
      if(i>0) scene->addItem(new Edge(nodel, nodex));
    }
    curvesLock=false;
    drawing=false;
}


bool ModelEdit::gm_insertMix(int idx)
{
    if (idx<0 || idx>=GetEepromInterface()->getCapability(Mixes) || g_model.mixData[GetEepromInterface()->getCapability(Mixes)-1].destCh > 0) {
      QMessageBox::information(this, "companion9x", tr("Not enough available mixers!"));
      return false;
    }

    int i = g_model.mixData[idx].destCh;
    memmove(&g_model.mixData[idx+1],&g_model.mixData[idx],
        (GetEepromInterface()->getCapability(Mixes)-(idx+1))*sizeof(MixData) );
    memset(&g_model.mixData[idx],0,sizeof(MixData));
    g_model.mixData[idx].srcRaw = RawSource(SOURCE_TYPE_NONE);
    g_model.mixData[idx].destCh = i;
    g_model.mixData[idx].weight = 100;
    return true;
}

void ModelEdit::gm_deleteMix(int index)
{
    memmove(&g_model.mixData[index],&g_model.mixData[index+1],
            (GetEepromInterface()->getCapability(Mixes)-(index+1))*sizeof(MixData));
    memset(&g_model.mixData[GetEepromInterface()->getCapability(Mixes)-1],0,sizeof(MixData));
}

void ModelEdit::gm_openMix(int index)
{
    if(index<0 || index>=GetEepromInterface()->getCapability(Mixes)) return;

    MixData mixd(g_model.mixData[index]);
    updateSettings();
    tabMixes();

    MixerDialog *g = new MixerDialog(this, &mixd, g_eeGeneral.stickMode);
    if(g->exec()) {
      g_model.mixData[index] = mixd;
      updateSettings();
      tabMixes();
    } else {
      if (mixInserted) {
        gm_deleteMix(index);
      }
      mixInserted=false;
      updateSettings();
      tabMixes();
    }
}

int ModelEdit::getMixerIndex(unsigned int dch)
{
    int i = 0;
    while ((g_model.mixData[i].destCh<=dch) && (g_model.mixData[i].destCh) && (i<GetEepromInterface()->getCapability(Mixes))) i++;
    if(i==GetEepromInterface()->getCapability(Mixes)) return -1;
    return i;
}

bool ModelEdit::gm_insertExpo(int idx)
{
    if (idx<0 || idx>=C9X_MAX_EXPOS || g_model.expoData[C9X_MAX_EXPOS-1].mode > 0) {
      QMessageBox::information(this, "companion9x", tr("Not enough available expos!"));
      return false;
    }

    int chn = g_model.expoData[idx].chn;
    memmove(&g_model.expoData[idx+1],&g_model.expoData[idx],
            (C9X_MAX_EXPOS-(idx+1))*sizeof(ExpoData) );
    memset(&g_model.expoData[idx],0,sizeof(ExpoData));
    g_model.expoData[idx].chn = chn;
    g_model.expoData[idx].weight = 100;
    g_model.expoData[idx].mode = 3 /* TODO enum */;
    return true;
}

void ModelEdit::gm_deleteExpo(int index)
{
    memmove(&g_model.expoData[index],&g_model.expoData[index+1],
              (C9X_MAX_EXPOS-(index+1))*sizeof(ExpoData));
    memset(&g_model.expoData[C9X_MAX_EXPOS-1],0,sizeof(ExpoData));
}

void ModelEdit::gm_openExpo(int index)
{
    if(index<0 || index>=C9X_MAX_EXPOS) return;

    ExpoData mixd(g_model.expoData[index]);
    updateSettings();
    tabExpos();

    ExpoDialog *g = new ExpoDialog(this, &mixd, g_eeGeneral.stickMode);
    if(g->exec())  {
      g_model.expoData[index] = mixd;
      updateSettings();
      tabExpos();
    } else {
      if (expoInserted) {
        gm_deleteExpo(index);
      }
      expoInserted=false;
      updateSettings();
      tabExpos();
    }
}

int ModelEdit::getExpoIndex(unsigned int dch)
{
    unsigned int i = 0;
    while (g_model.expoData[i].chn<=dch && g_model.expoData[i].mode && i<C9X_MAX_EXPOS) i++;
    if(i==C9X_MAX_EXPOS) return -1;
    return i;
}

void ModelEdit::mixerlistWidget_doubleClicked(QModelIndex index)
{
    int idx= MixerlistWidget->item(index.row())->data(Qt::UserRole).toByteArray().at(0);
    if (idx<0) {
        int i = -idx;
        idx = getMixerIndex(i); //get mixer index to insert
        if (!gm_insertMix(idx))
          return;
        g_model.mixData[idx].destCh = i;
        mixInserted=true;
    } else {
      mixInserted=false;
    }
    gm_openMix(idx);
}


void ModelEdit::expolistWidget_doubleClicked(QModelIndex index)
{
    expoOpen(ExposlistWidget->item(index.row()));
}

void ModelEdit::exposDeleteList(QList<int> list)
{
    qSort(list.begin(), list.end());

    int iDec = 0;
    foreach(int idx, list) {
      gm_deleteExpo(idx-iDec);
      iDec++;
    }
}

void ModelEdit::mixersDeleteList(QList<int> list)
{
    qSort(list.begin(), list.end());

    int iDec = 0;
    foreach(int idx, list) {
      gm_deleteMix(idx-iDec);
      iDec++;
    }
}

QList<int> ModelEdit::createMixListFromSelected()
{
    QList<int> list;
    foreach(QListWidgetItem *item, MixerlistWidget->selectedItems()) {
      int idx= item->data(Qt::UserRole).toByteArray().at(0);
      if(idx>=0 && idx<GetEepromInterface()->getCapability(Mixes)) list << idx;
    }
    return list;
}

QList<int> ModelEdit::createExpoListFromSelected()
{
    QList<int> list;
    foreach(QListWidgetItem *item, ExposlistWidget->selectedItems()) {
      int idx= item->data(Qt::UserRole).toByteArray().at(0);
      if(idx>=0 && idx<C9X_MAX_EXPOS) list << idx;
    }
    return list;
}

// TODO duplicated code
void ModelEdit::setSelectedByMixList(QList<int> list)
{
    for(int i=0; i<MixerlistWidget->count(); i++) {
      int t = MixerlistWidget->item(i)->data(Qt::UserRole).toByteArray().at(0);
      if(list.contains(t))
          MixerlistWidget->item(i)->setSelected(true);
    }
}

void ModelEdit::setSelectedByExpoList(QList<int> list)
{
    for(int i=0; i<ExposlistWidget->count(); i++) {
      int t = ExposlistWidget->item(i)->data(Qt::UserRole).toByteArray().at(0);
      if(list.contains(t))
        ExposlistWidget->item(i)->setSelected(true);
    }
}

void ModelEdit::mixersDelete(bool ask)
{
    QMessageBox::StandardButton ret = QMessageBox::No;

    if(ask)
      ret = QMessageBox::warning(this, "companion9x",
               tr("Delete Selected Mixes?"),
               QMessageBox::Yes | QMessageBox::No);


    if ((ret == QMessageBox::Yes) || (!ask)) {
      mixersDeleteList(createMixListFromSelected());
      updateSettings();
      tabMixes();
    }
}

void ModelEdit::mixersCut()
{
    mixersCopy();
    mixersDelete(false);
}

void ModelEdit::mixersCopy()
{

    QList<int> list = createMixListFromSelected();

    QByteArray mxData;
    foreach(int idx, list) {
      mxData.append((char*)&g_model.mixData[idx],sizeof(MixData));
    }

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion9x-mix", mxData);
    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void ModelEdit::exposDelete(bool ask)
{
    QMessageBox::StandardButton ret = QMessageBox::No;

    if(ask)
      ret = QMessageBox::warning(this, "companion9x",
               tr("Delete Selected Expos?"),
               QMessageBox::Yes | QMessageBox::No);


    if ((ret == QMessageBox::Yes) || (!ask)) {
        exposDeleteList(createExpoListFromSelected());
        updateSettings();
        tabExpos();
    }
}

void ModelEdit::exposCut()
{
    exposCopy();
    exposDelete(false);
}

void ModelEdit::exposCopy()
{
    QList<int> list = createExpoListFromSelected();

    QByteArray mxData;
    foreach(int idx, list) {
      mxData.append((char*)&g_model.expoData[idx],sizeof(ExpoData));
    }

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion9x-expo", mxData);
    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void ModelEdit::mimeExpoDropped(int index, const QMimeData *data, Qt::DropAction /*action*/)
{
    int idx = ExposlistWidget->item(index > 0 ? index-1 : 0)->data(Qt::UserRole).toByteArray().at(0);
    pasteExpoMimeData(data, idx);
}

void ModelEdit::mimeMixerDropped(int index, const QMimeData *data, Qt::DropAction /*action*/)
{
    int idx= MixerlistWidget->item(index > 0 ? index-1 : 0)->data(Qt::UserRole).toByteArray().at(0);
    pasteMixerMimeData(data, idx);
}

void ModelEdit::pasteMixerMimeData(const QMimeData * mimeData, int destIdx)
{
  if(mimeData->hasFormat("application/x-companion9x-mix")) {
    int idx; // mixer index
    int dch;

    if(destIdx<0) {
      dch = -destIdx;
      idx = getMixerIndex(dch) - 1; //get mixer index to insert
    } else {
      idx = destIdx;
      dch = g_model.mixData[idx].destCh;
    }

    QByteArray mxData = mimeData->data("application/x-companion9x-mix");

    int i = 0;
    while(i<mxData.size()) {
      idx++;
      if(idx==GetEepromInterface()->getCapability(Mixes)) break;

      if (!gm_insertMix(idx))
        break;
      MixData *md = &g_model.mixData[idx];
      memcpy(md,mxData.mid(i,sizeof(MixData)).constData(),sizeof(MixData));
      md->destCh = dch;
      i += sizeof(MixData);
    }

    updateSettings();
    tabMixes();
  }
}
#include <iostream>
#include <QtGui/qwidget.h>
void ModelEdit::pasteExpoMimeData(const QMimeData * mimeData, int destIdx)
{
  if (mimeData->hasFormat("application/x-companion9x-expo")) {
    int idx; // mixer index
    int dch;

    if (destIdx < 0) {
      dch = -destIdx - 1;
      idx = getExpoIndex(dch) - 1; //get expo index to insert
    } else {
      idx = destIdx;
      dch = g_model.expoData[idx].chn;
    }

    QByteArray mxData = mimeData->data("application/x-companion9x-expo");

    int i = 0;
    while (i < mxData.size()) {
      idx++;
      if (!gm_insertExpo(idx))
        break;
      ExpoData *md = &g_model.expoData[idx];
      memcpy(md, mxData.mid(i, sizeof(ExpoData)).constData(), sizeof(ExpoData));
      md->chn = dch;
      i += sizeof(ExpoData);
    }

    updateSettings();
    tabExpos();
  }
}

void ModelEdit::mixersPaste()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    QListWidgetItem *item = MixerlistWidget->currentItem();
    if (item)
      pasteMixerMimeData(mimeData, item->data(Qt::UserRole).toByteArray().at(0));
}

void ModelEdit::mixersDuplicate()
{
    mixersCopy();
    mixersPaste();
}

void ModelEdit::exposPaste()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    QListWidgetItem *item = ExposlistWidget->currentItem();
    if (item)
      pasteExpoMimeData(mimeData, item->data(Qt::UserRole).toByteArray().at(0));
}

void ModelEdit::exposDuplicate()
{
    exposCopy();
    exposPaste();
}

void ModelEdit::mixerOpen()
{
    int idx = MixerlistWidget->currentItem()->data(Qt::UserRole).toByteArray().at(0);
    if(idx<0) {
      int i = -idx;
      idx = getMixerIndex(i); //get mixer index to insert
      if (!gm_insertMix(idx))
        return;
      g_model.mixData[idx].destCh = i;
      mixInserted=true;
    } else {
      mixInserted=false;
    }
    
    gm_openMix(idx);
}

void ModelEdit::mixerAdd()
{
    if (!MixerlistWidget->currentItem())
      return;
    int index = MixerlistWidget->currentItem()->data(Qt::UserRole).toByteArray().at(0);

    if(index<0) {  // if empty then return relavent index
      int i = -index;
      index = getMixerIndex(i); //get mixer index to insert
      if (!gm_insertMix(index))
        return;
      g_model.mixData[index].destCh = i;
      mixInserted=true;
    } else {
      index++;
      if (!gm_insertMix(index))
        return;
      g_model.mixData[index].destCh = g_model.mixData[index-1].destCh;
      mixInserted=true;
    }
    gm_openMix(index);
}

void ModelEdit::expoOpen(QListWidgetItem *item)
{
    if (!item)
      item = ExposlistWidget->currentItem();

    int idx = item->data(Qt::UserRole).toByteArray().at(0);
    if (idx<0) {
      int ch = -idx-1;
      idx = getExpoIndex(ch); // get expo index to insert
      if (!gm_insertExpo(idx))
        return;
      g_model.expoData[idx].chn = ch;
      expoInserted=true;
    } else {
        expoInserted=false;
    }
    gm_openExpo(idx);
}

void ModelEdit::expoAdd()
{
    int index = ExposlistWidget->currentItem()->data(Qt::UserRole).toByteArray().at(0);

    if(index<0) {  // if empty then return relevant index
      expoOpen();
    }  else {
      index++;
      if (!gm_insertExpo(index))
        return;
      g_model.expoData[index].chn = g_model.expoData[index-1].chn;
    }
    gm_openExpo(index);
}

void ModelEdit::mixerlistWidget_customContextMenuRequested(QPoint pos)
{
    QPoint globalPos = MixerlistWidget->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion9x-mix");

    QMenu contextMenu;
    contextMenu.addAction(QIcon(":/images/add.png"), tr("&Add"),this,SLOT(mixerAdd()),tr("Ctrl+A"));
    contextMenu.addAction(QIcon(":/images/edit.png"), tr("&Edit"),this,SLOT(mixerOpen()),tr("Enter"));
    contextMenu.addSeparator();
    contextMenu.addAction(QIcon(":/images/clear.png"), tr("&Delete"),this,SLOT(mixersDelete()),tr("Delete"));
    contextMenu.addAction(QIcon(":/images/copy.png"), tr("&Copy"),this,SLOT(mixersCopy()),tr("Ctrl+C"));
    contextMenu.addAction(QIcon(":/images/cut.png"), tr("&Cut"),this,SLOT(mixersCut()),tr("Ctrl+X"));
    contextMenu.addAction(QIcon(":/images/paste.png"), tr("&Paste"),this,SLOT(mixersPaste()),tr("Ctrl+V"))->setEnabled(hasData);
    contextMenu.addAction(QIcon(":/images/duplicate.png"), tr("Du&plicate"),this,SLOT(mixersDuplicate()),tr("Ctrl+U"));
    contextMenu.addSeparator();
    contextMenu.addAction(QIcon(":/images/moveup.png"), tr("Move Up"),this,SLOT(moveMixUp()),tr("Ctrl+Up"));
    contextMenu.addAction(QIcon(":/images/movedown.png"), tr("Move Down"),this,SLOT(moveMixDown()),tr("Ctrl+Down"));

    contextMenu.exec(globalPos);
}

void ModelEdit::expolistWidget_customContextMenuRequested(QPoint pos)
{
    QPoint globalPos = ExposlistWidget->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion9x-expo");

    QMenu contextMenu;
    contextMenu.addAction(QIcon(":/images/add.png"), tr("&Add"),this,SLOT(expoAdd()),tr("Ctrl+A"));
    contextMenu.addAction(QIcon(":/images/edit.png"), tr("&Edit"),this,SLOT(expoOpen()),tr("Enter"));
    contextMenu.addSeparator();
    contextMenu.addAction(QIcon(":/images/clear.png"), tr("&Delete"),this,SLOT(exposDelete()),tr("Delete"));
    contextMenu.addAction(QIcon(":/images/copy.png"), tr("&Copy"),this,SLOT(exposCopy()),tr("Ctrl+C"));
    contextMenu.addAction(QIcon(":/images/cut.png"), tr("&Cut"),this,SLOT(exposCut()),tr("Ctrl+X"));
    contextMenu.addAction(QIcon(":/images/paste.png"), tr("&Paste"),this,SLOT(exposPaste()),tr("Ctrl+V"))->setEnabled(hasData);
    contextMenu.addAction(QIcon(":/images/duplicate.png"), tr("Du&plicate"),this,SLOT(exposDuplicate()),tr("Ctrl+U"));
    contextMenu.addSeparator();
    contextMenu.addAction(QIcon(":/images/moveup.png"), tr("Move Up"),this,SLOT(moveExpoUp()),tr("Ctrl+Up"));
    contextMenu.addAction(QIcon(":/images/movedown.png"), tr("Move Down"),this,SLOT(moveExpoDown()),tr("Ctrl+Down"));

    contextMenu.exec(globalPos);
}

void ModelEdit::cswPaste()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();  
    if (mimeData->hasFormat("application/x-companion9x-csw")) {
      QByteArray cswData = mimeData->data("application/x-companion9x-csw");

      CustomSwData *csw = &g_model.customSw[selectedSwitch];
      memcpy(csw, cswData.mid(0, sizeof(CustomSwData)).constData(), sizeof(CustomSwData));
      updateSettings();
      updateSelectedSwitch();
    }
}

void ModelEdit::cswDelete()
{
    g_model.customSw[selectedSwitch].clear();
    updateSettings();
    updateSelectedSwitch();
}

void ModelEdit::cswCopy()
{
    QByteArray cswData;
    cswData.append((char*)&g_model.customSw[selectedSwitch],sizeof(CustomSwData));
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion9x-csw", cswData);
    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void ModelEdit::updateSelectedSwitch()
{
    switchEditLock = true;
    populateCSWCB(csw[selectedSwitch], g_model.customSw[selectedSwitch].func);
    setSwitchWidgetVisibility(selectedSwitch);
    switchEditLock = false;
}

void ModelEdit::cswCut()
{
  cswCopy();
  cswDelete();
}

void ModelEdit::csw_customContextMenuRequested(QPoint pos)
{
    QLabel *label = (QLabel *)sender();
    if (!label)
      return;
    QString name=label->objectName();
    if (!name.startsWith("cswlabel"))
      return;
    selectedSwitch=name.mid(9,2).toInt()-1;
    
    QPoint globalPos = label->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion9x-csw");

    QMenu contextMenu;
    contextMenu.addAction(QIcon(":/images/clear.png"), tr("&Delete"),this,SLOT(cswDelete()),tr("Delete"));
    contextMenu.addAction(QIcon(":/images/copy.png"), tr("&Copy"),this,SLOT(cswCopy()),tr("Ctrl+C"));
    contextMenu.addAction(QIcon(":/images/cut.png"), tr("&Cut"),this,SLOT(cswCut()),tr("Ctrl+X"));
    contextMenu.addAction(QIcon(":/images/paste.png"), tr("&Paste"),this,SLOT(cswPaste()),tr("Ctrl+V"))->setEnabled(hasData);

    contextMenu.exec(globalPos);
}

void ModelEdit::fsw_customContextMenuRequested(QPoint pos)
{
    QLabel *label = (QLabel *)sender();
    if (!label)
      return;
    int fsw=label->property("FunctionId").toInt();
    if (!(fsw>0 && fsw<=32))
      return;
    selectedFunction=fsw-1;
    
    QPoint globalPos = label->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion9x-fsw");

    QMenu contextMenu;
    contextMenu.addAction(QIcon(":/images/clear.png"), tr("&Delete"),this,SLOT(fswDelete()),tr("Delete"));
    contextMenu.addAction(QIcon(":/images/copy.png"), tr("&Copy"),this,SLOT(fswCopy()),tr("Ctrl+C"));
    contextMenu.addAction(QIcon(":/images/cut.png"), tr("&Cut"),this,SLOT(fswCut()),tr("Ctrl+X"));
    contextMenu.addAction(QIcon(":/images/paste.png"), tr("&Paste"),this,SLOT(fswPaste()),tr("Ctrl+V"))->setEnabled(hasData);

    contextMenu.exec(globalPos);
}

void ModelEdit::fswPaste()
{
  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();  
  if (mimeData->hasFormat("application/x-companion9x-fsw")) {
    QByteArray fswData = mimeData->data("application/x-companion9x-fsw");
    
    FuncSwData *fsw = &g_model.funcSw[selectedFunction];
    memcpy(fsw, fswData.mid(0, sizeof(FuncSwData)).constData(), sizeof(FuncSwData));
    updateSettings();
    tabCustomFunctions();
  }
}

void ModelEdit::fswDelete()
{
  g_model.funcSw[selectedFunction].clear();
  updateSettings();
  tabCustomFunctions();
}

void ModelEdit::fswCopy()
{
    QByteArray fswData;
    fswData.append((char*)&g_model.funcSw[selectedFunction],sizeof(FuncSwData));
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion9x-fsw", fswData);
    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void ModelEdit::fswCut()
{
    fswCopy();
    fswDelete();
}

void ModelEdit::mixerlistWidget_KeyPress(QKeyEvent *event)
{
    if(event->matches(QKeySequence::SelectAll)) mixerAdd();  //Ctrl A
    if(event->matches(QKeySequence::Delete))    mixersDelete();
    if(event->matches(QKeySequence::Copy))      mixersCopy();
    if(event->matches(QKeySequence::Cut))       mixersCut();
    if(event->matches(QKeySequence::Paste))     mixersPaste();
    if(event->matches(QKeySequence::Underline)) mixersDuplicate();

    if(event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter) mixerOpen();
    if(event->matches(QKeySequence::MoveToNextLine))
        MixerlistWidget->setCurrentRow(MixerlistWidget->currentRow()+1);
    if(event->matches(QKeySequence::MoveToPreviousLine))
        MixerlistWidget->setCurrentRow(MixerlistWidget->currentRow()-1);
}

void ModelEdit::expolistWidget_KeyPress(QKeyEvent *event)
{
    if(event->matches(QKeySequence::SelectAll)) expoAdd();  //Ctrl A
    if(event->matches(QKeySequence::Delete))    exposDelete();
    if(event->matches(QKeySequence::Copy))      exposCopy();
    if(event->matches(QKeySequence::Cut))       exposCut();
    if(event->matches(QKeySequence::Paste))     exposPaste();
    if(event->matches(QKeySequence::Underline)) exposDuplicate();

    if(event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter) expoOpen();
    if(event->matches(QKeySequence::MoveToNextLine))
        ExposlistWidget->setCurrentRow(ExposlistWidget->currentRow()+1);
    if(event->matches(QKeySequence::MoveToPreviousLine))
        ExposlistWidget->setCurrentRow(ExposlistWidget->currentRow()-1);
}

int ModelEdit::gm_moveMix(int idx, bool dir) //true=inc=down false=dec=up
{
    if(idx>GetEepromInterface()->getCapability(Mixes) || (idx==0 && !dir) || (idx==GetEepromInterface()->getCapability(Mixes) && dir)) return idx;

    int tdx = dir ? idx+1 : idx-1;
    MixData &src=g_model.mixData[idx];
    MixData &tgt=g_model.mixData[tdx];

    unsigned int outputs = GetEepromInterface()->getCapability(Outputs);
    if((src.destCh==0) || (src.destCh>outputs) || (tgt.destCh>outputs)) return idx;

    if(tgt.destCh!=src.destCh) {
        if ((dir)  && (src.destCh<outputs)) src.destCh++;
        if ((!dir) && (src.destCh>0)) src.destCh--;
        return idx;
    }

    //flip between idx and tgt
    MixData temp;
    memcpy(&temp,&src,sizeof(MixData));
    memcpy(&src,&tgt,sizeof(MixData));
    memcpy(&tgt,&temp,sizeof(MixData));
    return tdx;
}

void ModelEdit::moveMixUp()
{
    QList<int> list = createMixListFromSelected();
    QList<int> highlightList;
    foreach(int idx, list) {
      highlightList << gm_moveMix(idx, false);
    }
    updateSettings();
    tabMixes();
    setSelectedByMixList(highlightList);
}

void ModelEdit::moveMixDown()
{
    QList<int> list = createMixListFromSelected();
    QList<int> highlightList;
    foreach(int idx, list) {
      highlightList << gm_moveMix(idx, true);
    }
    updateSettings();
    tabMixes();
    setSelectedByMixList(highlightList);
}

int ModelEdit::gm_moveExpo(int idx, bool dir) //true=inc=down false=dec=up
{
    if(idx>C9X_MAX_EXPOS || (idx==C9X_MAX_EXPOS && dir)) return idx;
    
    int tdx = dir ? idx+1 : idx-1;
    ExpoData temp;
    temp.clear();    
    ExpoData &src=g_model.expoData[idx];
    ExpoData &tgt=g_model.expoData[tdx];
    if (!dir && tdx<0 && src.chn>0) {
      src.chn--;
      return idx;
    } else if (!dir && tdx<0) {
      return idx;
    }
    
    if(memcmp(&src,&temp,sizeof(ExpoData))==0) return idx;
    bool tgtempty=(memcmp(&tgt,&temp,sizeof(ExpoData))==0 ? 1:0);
    if(tgt.chn!=src.chn || tgtempty) {
        if ((dir)  && (src.chn<(NUM_STICKS-1))) src.chn++;
        if ((!dir) && (src.chn>0)) src.chn--;
        return idx;
    }

    //flip between idx and tgt
    memcpy(&temp,&src,sizeof(ExpoData));
    memcpy(&src,&tgt,sizeof(ExpoData));
    memcpy(&tgt,&temp,sizeof(ExpoData));
    return tdx;
}

void ModelEdit::moveExpoUp()
{
    QList<int> list = createExpoListFromSelected();
    QList<int> highlightList;
    foreach(int idx, list) {
      highlightList << gm_moveExpo(idx, false);
    }
    updateSettings();
    tabExpos();
    setSelectedByExpoList(highlightList);
}

void ModelEdit::moveExpoDown()
{
    QList<int> list = createExpoListFromSelected();
    QList<int> highlightList;
    foreach(int idx, list) {
      highlightList << gm_moveExpo(idx, true);
    }
    updateSettings();
    tabExpos();
    setSelectedByExpoList(highlightList);
}

void ModelEdit::launchSimulation()
{
    if (GetEepromInterface()->getSimulator()) {
      RadioData *simuData = new RadioData;
      *simuData = radioData;
      simuData->models[id_model] = g_model;
      if (GetEepromInterface()->getCapability(SimulatorType)) {
        xsimulatorDialog sd(this);
        sd.loadParams(*simuData, id_model);
        sd.exec();
      } else {
        simulatorDialog sd(this);
        sd.loadParams(*simuData, id_model);
        sd.exec();
      }
    } else {
      QMessageBox::warning(NULL,
          QObject::tr("Warning"),
          QObject::tr("Simulator for this firmware is not yet available"));
    }
}

void ModelEdit::on_pushButton_clicked()
{
    launchSimulation();
}

void ModelEdit::resetCurve()
{
    QPushButton *button = (QPushButton *)sender();
    int btn=button->objectName().mid(button->objectName().lastIndexOf("_")+1).toInt()-1;
    if (btn!=currentCurve) {
      int res = QMessageBox::question(this, "companion9x",tr("Are you sure you want to delete curve %1 ?").arg(btn+1),QMessageBox::Yes | QMessageBox::No);
      if (res == QMessageBox::No) {
        return;
      }
    }
    memset(g_model.curves[btn].name,0,sizeof(g_model.curves[btn].name));
    ui->cname_LE->clear();
    g_model.curves[btn].count=5;
    g_model.curves[btn].custom=false;
    curvesLock=true;
    for (int i=0; i<17; i++) {
      g_model.curves[btn].points[i].x=0;
      g_model.curves[btn].points[i].y=0;
      spnx[i]->setMinimum(-100);
      spnx[i]->setMaximum(100);
      spnx[i]->setValue(0);
      spny[i]->setValue(0);
    }
    curvesLock=false;
    updateCurvesTab();
    if (btn==currentCurve) {
      ui->curvetype_CB->setCurrentIndex(2);
    }
    updateSettings();
    drawCurve();
}

void ModelEdit::editCurve()
{
    QPushButton *button = (QPushButton *)sender();
    int btn=button->objectName().mid(button->objectName().lastIndexOf("_")+1).toInt()-1;
    setCurrentCurve(btn);
    drawCurve();
}

void ModelEdit::plotCurve(bool checked)
{
    QCheckBox *chk = (QCheckBox *)sender();
    int btn=chk->objectName().mid(chk->objectName().lastIndexOf("_")+1).toInt()-1;
    plot_curve[btn] = checked;
    drawCurve();
}

void ModelEdit::on_T2ThrTrgChkB_toggled(bool checked)
{
    g_model.t2throttle = checked;
    updateSettings();
}

void ModelEdit::on_extendedLimitsChkB_toggled(bool checked)
{
    g_model.extendedLimits = checked;
    setLimitMinMax();
    updateSettings();
}

void ModelEdit::on_thrwarnChkB_toggled(bool checked)
{
    if (switchEditLock)
      return;
    g_model.disableThrottleWarning = checked;
    updateSettings();
}

void ModelEdit::on_thrrevChkB_toggled(bool checked)
{
    g_model.throttleReversed = checked;
    updateSettings();
}

void ModelEdit::on_extendedTrimsChkB_toggled(bool checked)
{
    g_model.extendedTrims = checked;
    tabPhases();
    updateSettings();
}

void ModelEdit::setLimitMinMax()
{
    on_numChannelsSB_editingFinished();
    on_numChannelsSB_2_editingFinished();
    on_numChannelsSB_3_editingFinished();
    int v = g_model.extendedLimits ? 125 : 100;
    foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("minSB_[0-9]+"))) {
      sb->setMaximum(25);
      sb->setMinimum(-v);
    }
    foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("maxSB_[0-9]+"))) {
      sb->setMaximum(v);
      sb->setMinimum(-25);
    }
}


void ModelEdit::safetySwitchesEdited()
{
    for(int i=0; i<NUM_SAFETY_CHNOUT; i++) {
        g_model.safetySw[i].swtch = RawSwitch(safetySwitchSwtch[i]->itemData(safetySwitchSwtch[i]->currentIndex()).toInt());
        g_model.safetySw[i].val   = safetySwitchValue[i]->value();
    }
    updateSettings();
}



void ModelEdit::on_templateList_doubleClicked(QModelIndex index)
{
    QString text = ui->templateList->item(index.row())->text();
    if (index.row()==13) {
      uint64_t result=0xffffffff;
      modelConfigDialog *mcw = new modelConfigDialog(radioData, &result, this);
      mcw->exec();
      if (result!=0xffffffff) {
        applyNumericTemplate(result);
        updateSettings();
        tabMixes();
      }
    } else {
      int res = QMessageBox::question(this,tr("Apply Template?"),tr("Apply template \"%1\"?").arg(text),QMessageBox::Yes | QMessageBox::No);
      if(res!=QMessageBox::Yes) return;
      applyTemplate(index.row());
      updateSettings();
      tabMixes();
   }
}


MixData* ModelEdit::setDest(uint8_t dch)
{
    uint8_t i = 0;
    while ((g_model.mixData[i].destCh<=dch) && (g_model.mixData[i].destCh) && (i<GetEepromInterface()->getCapability(Mixes))) i++;
    if(i==GetEepromInterface()->getCapability(Mixes)) return &g_model.mixData[0];

    memmove(&g_model.mixData[i+1],&g_model.mixData[i],
            (GetEepromInterface()->getCapability(Mixes)-(i+1))*sizeof(MixData) );
    memset(&g_model.mixData[i],0,sizeof(MixData));
    g_model.mixData[i].destCh = dch;
    return &g_model.mixData[i];
}

void ModelEdit::clearExpos(bool ask)
{
    if(ask) {
      int res = QMessageBox::question(this,tr("Clear Expos?"),tr("Really clear all the expos?"),QMessageBox::Yes | QMessageBox::No);
      if(res!=QMessageBox::Yes) return;
    }
    memset(g_model.expoData,0,sizeof(g_model.expoData)); //clear all expos
    updateSettings();
    tabExpos();
}

void ModelEdit::clearMixes(bool ask)
{
    if(ask) {
      int res = QMessageBox::question(this,tr("Clear Mixes?"),tr("Really clear all the mixes?"),QMessageBox::Yes | QMessageBox::No);
      if(res!=QMessageBox::Yes) return;
    }
    memset(g_model.mixData,0,sizeof(g_model.mixData)); //clear all mixes
    updateSettings();
    tabMixes();
}

void ModelEdit::clearCurves(bool ask)
{
    if(ask) {
      int res = QMessageBox::question(this,tr("Clear Curves?"),tr("Really clear all the curves?"),QMessageBox::Yes | QMessageBox::No);
      if(res!=QMessageBox::Yes) return;
    }
    curvesLock=true;
    if (!GetEepromInterface()->getCapability(CustomCurves)){
      ui->curvetype_CB->setDisabled(true);
      int count=0;
      for (int j=0; j< GetEepromInterface()->getCapability(NumCurves3); j++) {
          g_model.curves[count].count=3;
          g_model.curves[count].custom=false;
          memset(g_model.curves[j].name,0,sizeof(g_model.curves[j].name)); 
          for (int i=0; i<17; i++) {
            g_model.curves[count].points[i].x=0;
            g_model.curves[count].points[i].y=0;
          }
          count++;
      }
      for (int j=0; j< GetEepromInterface()->getCapability(NumCurves5); j++) {
          g_model.curves[count].count=5;
          g_model.curves[count].custom=false;
          memset(g_model.curves[j].name,0,sizeof(g_model.curves[j].name)); 
          for (int i=0; i<17; i++) {
            g_model.curves[count].points[i].x=0;
            g_model.curves[count].points[i].y=0;
          }
          count++;
      }
      for (int j=0; j< GetEepromInterface()->getCapability(NumCurves9); j++) {
          g_model.curves[count].count=9;
          g_model.curves[count].custom=false;
          memset(g_model.curves[j].name,0,sizeof(g_model.curves[j].name)); 
          for (int i=0; i<17; i++) {
            g_model.curves[count].points[i].x=0;
            g_model.curves[count].points[i].y=0;
          }
          count++;
      }
      for (int j=count; j<16; j++) {
          g_model.curves[j].count=5;
          g_model.curves[j].custom=false;
          memset(g_model.curves[j].name,0,sizeof(g_model.curves[j].name)); 
          for (int i=0; i<17; i++) {
            g_model.curves[j].points[i].x=0;
            g_model.curves[j].points[i].y=0;
          }
      }
    } else {
      for (int j=0; j<16; j++) {
          g_model.curves[j].count=5;
          g_model.curves[j].custom=false;
          memset(g_model.curves[j].name,0,sizeof(g_model.curves[j].name));
          for (int i=0; i<17; i++) {
            g_model.curves[j].points[i].x=0;
            g_model.curves[j].points[i].y=0;
          }
      }
    }
    for (int i=0; i<17; i++) {
      spnx[i]->setMinimum(-100);
      spnx[i]->setMaximum(100);
      spnx[i]->setValue(0);
      spny[i]->setValue(0);
    }
    currentCurve=0;
    curvesLock=false;
    if (GetEepromInterface()->getCapability(NumCurves3)>0) {
      ui->curvetype_CB->setCurrentIndex(0);
    } else {
      ui->curvetype_CB->setCurrentIndex(2);  
    }
    ui->cname_LE->clear();
    updateSettings();
    drawCurve();
}

void ModelEdit::setCurve(uint8_t c, int8_t ar[])
{
    int len=sizeof(ar)/sizeof(int8_t);
    if (GetEepromInterface()->getCapability(CustomCurves)) {
      if (GetEepromInterface()->getCapability(NumCurves)>c) {
        if (len<9) {
          g_model.curves[c].count=5;
          g_model.curves[c].custom=false;
          for (int i=0; i< 5; i++) {
            g_model.curves[c].points[i].y=ar[i];
          }
        } else {
          g_model.curves[c].count=5;
          g_model.curves[c].custom=false;
          for (int i=0; i< 5; i++) {
            g_model.curves[c].points[i].y=ar[i];
          }
        }
      }
    } else {
      if (len<9) {
        g_model.curves[c].count=5;
        g_model.curves[c].custom=false;
        for (int i=0; i< 5; i++) {
          g_model.curves[c].points[i].y=ar[i];
        }
      } else {
        g_model.curves[c+8].count=5;
        g_model.curves[c+8].custom=false;
        for (int i=0; i< 5; i++) {
          g_model.curves[c+8].points[i].y=ar[i];
        }
      }
    }
}

void ModelEdit::setSwitch(unsigned int idx, unsigned int func, int v1, int v2)
{
    g_model.customSw[idx-1].func = func;
    g_model.customSw[idx-1].val1   = v1;
    g_model.customSw[idx-1].val2   = v2;
}

void ModelEdit::applyNumericTemplate(uint64_t tpl)
{
    clearCurves(false);
    clearExpos(false);
    clearMixes(false);
    int8_t heli_ar1[] = {-100, -20, 30, 70, 90};
    int8_t heli_ar2[] = {80, 70, 60, 70, 100};
    int8_t heli_ar3[] = {100, 90, 80, 90, 100};
    int8_t heli_ar4[] = {-30,  -15, 0, 50, 100};
    int8_t heli_ar5[] = {-100, -50, 0, 50, 100};
    bool rx[10];
    for (int i=0; i<10 ; i++) {
      rx[i]=false;
    }
    int thrsw=GetEepromInterface()->getCapability(GetThrSwitch);
    MixData *md = &g_model.mixData[0];
    uint8_t spo2ch=(tpl & 0x0F);
    tpl>>=4;
    uint8_t spo1ch=(tpl & 0x0F);
    tpl>>=4;
    uint8_t fla2ch=(tpl & 0x0F);
    tpl>>=4;
    uint8_t fla1ch=(tpl & 0x0F);
    tpl>>=4;
    uint8_t rud2ch=(tpl & 0x0F);
    tpl>>=4;
    uint8_t ele2ch=(tpl & 0x0F);
    tpl>>=4;
    uint8_t ail2ch=(tpl & 0x0F);
    tpl>>=4;
    uint8_t chstyle=(tpl & 0x03);
    tpl>>=2;
    uint8_t gyro=(tpl & 0x03);
    tpl>>=2;
    uint8_t tailtype=(tpl & 0x03);
    tpl>>=2;
    uint8_t swashtype=(tpl & 0x07);
    tpl>>=3;
    uint8_t ruddertype=(tpl & 0x03);
    tpl>>=2;
    uint8_t spoilertype=(tpl & 0x3);
    tpl>>=2;
    uint8_t flaptype=(tpl & 0x03);
    tpl>>=2;
    uint8_t ailerontype=(tpl & 0x03);
    tpl>>=2;
    uint8_t enginetype=(tpl & 0x03);
    tpl>>=2;
    uint8_t modeltype=(tpl & 0x03);

  #define ICC(x) icc[(x)-1]
    uint8_t icc[4] = {0};
    for(uint8_t i=1; i<=4; i++) //generate inverse array
      for(uint8_t j=1; j<=4; j++) if(CC(i)==j) icc[j-1]=i;

    int ailerons;
    int flaps;
    int throttle;
    int spoilers;
    int elevators;
    int rudders;
    int sign;
    uint8_t rxch;
    switch (modeltype) {
      case 0:
        ailerons=ailerontype;
        flaps=flaptype;
        throttle=1;
        switch (tailtype) {
          case 0:
          case 1:
            rudders=1;
            elevators=1;
            break;
          case 2:
            rudders=1;
            elevators=2;
            break;
        }
        rxch=ICC(STK_RUD);
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
        if (tailtype==1) {
          md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
        }
        rx[rxch-1]=true;
        rxch=ICC(STK_ELE);
        if (tailtype==1) {
          md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
        }
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
        rx[rxch-1]=true;
        rxch=ICC(STK_THR);
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("THR").toAscii().data(),6);
        rx[rxch-1]=true;
        if (ail2ch > 0) {
          rx[ail2ch-1]=true;
        }
        if (ele2ch > 0) {
          rx[ele2ch-1]=true;
        }
        if (fla1ch > 0) {
          rx[fla1ch-1]=true;
        }
        if (fla2ch > 0) {
          rx[fla2ch-1]=true;
        }
        if (ailerons>0) {
          rxch=ICC(STK_AIL);
          md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
          rx[rxch-1]=true;
        }
        if (ailerons>1) {
          if (ail2ch==0) {
            for (int j=0; j<10 ; j++) {
              if (!rx[j]) {
                md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL2").toAscii().data(),6);
                rx[j]=true;
                break;
              }
            }
          } else {
            md=setDest(ail2ch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL2").toAscii().data(),6);
          }
        }
        if (elevators>1) {
          if (ele2ch==0) {
            for (int j=0; j<10 ; j++) {
              if (!rx[j]) {
                md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();;strncpy(md->name, tr("ELE2").toAscii().data(),6);
                rx[j]=true;
                break;
              }
            }
          }else{
            md=setDest(ele2ch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();;strncpy(md->name, tr("ELE2").toAscii().data(),6);
          }
        }
        if (flaps>0) {
              md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6); md->speedUp=4; md->speedDown=4;
              md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6); md->speedUp=4; md->speedDown=4;
        }
        sign=-1;
        for (uint8_t i=0; i< flaps; i++) {
          sign*=-1;
          int index;
          if (i==0) {
            index=fla1ch;
          } else {
            index=fla2ch;
          }
          if (index==0) {
            for (int j=0; j<10 ; j++) {
              if (!rx[j]) {
                md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight=100*sign; md->sOffset=0; md->swtch=RawSwitch();strncpy(md->name, tr("FLAP%1").arg(i+1).toAscii().data(),6);
                rx[j]=true;
                break;
              }
            }
          } else {
            md=setDest(index);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight=100*sign; md->sOffset=0; md->swtch=RawSwitch();strncpy(md->name, tr("FLAP%1").arg(i+1).toAscii().data(),6);
          }
        }      
        break;
      case 1:
        setCurve(CURVE5(1),heli_ar1);
        setCurve(CURVE5(2),heli_ar2);
        setCurve(CURVE5(3),heli_ar3);
        setCurve(CURVE5(4),heli_ar4);
        setCurve(CURVE5(5),heli_ar5);
        setCurve(CURVE5(6),heli_ar5);
        switch (swashtype) {
          case 0:
            g_model.swashRingData.type = HELI_SWASH_TYPE_90;
            break;
          case 1:
            g_model.swashRingData.type = HELI_SWASH_TYPE_120;
            break;
          case 2:
            g_model.swashRingData.type = HELI_SWASH_TYPE_120X;
            break;
          case 3:
            g_model.swashRingData.type = HELI_SWASH_TYPE_140;
            break;
          case 4:
            g_model.swashRingData.type = HELI_SWASH_TYPE_NONE;
            break;
        }
        g_model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

        if (chstyle==0) {
          if (swashtype!=4) {
            md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 0);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
            md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 1);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
            md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 2);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("PITCH").toAscii().data(),6);
          } else {
            md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
            md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
            md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("PITCH").toAscii().data(),6);
          }
          md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0); md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
          md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
          md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
          md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
          md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw); md->mltpx=MLTPX_REP;
          switch (gyro) {
            case 1:
              md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA);strncpy(md->name, tr("GYRO").toAscii().data(),6);
              md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA);strncpy(md->name, tr("GYRO").toAscii().data(),6);
              break;
            case 2:
              md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA); md->sOffset=100;strncpy(md->name, tr("GYRO").toAscii().data(),6);
              md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA); md->sOffset=100;strncpy(md->name, tr("GYRO").toAscii().data(),6);
              break;
          }
        } else {
          if (swashtype!=4) {
            md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 1);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
            md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 0);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
            md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 2);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("PITCH").toAscii().data(),6);
          } else {
            md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
            md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
            md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("PITCH").toAscii().data(),6);          
          }
          md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0); md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
          md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
          md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
          md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
          md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw); md->mltpx=MLTPX_REP;
          switch (gyro) {
            case 1:
              md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA);strncpy(md->name, tr("GYRO").toAscii().data(),6);
              md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA);strncpy(md->name, tr("GYRO").toAscii().data(),6);
              break;
            case 2:
              md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA); md->sOffset=100;strncpy(md->name, tr("GYRO").toAscii().data(),6);
              md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA); md->sOffset=100;strncpy(md->name, tr("GYRO").toAscii().data(),6);
              break;
          }     
        }
        // collective
        md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
        md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
        md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;
        break;
      case 2:
        ailerons=ailerontype;
        flaps=flaptype;
        spoilers=spoilertype;
        throttle=enginetype;
        switch (tailtype) {
          case 0:
          case 1:
            rudders=1;
            elevators=1;
            break;
          case 2:
            rudders=1;
            elevators=2;
            break;
        }
        if (throttle==1) {
         rxch=ICC(STK_THR);
         md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("THR").toAscii().data(),6);
         rx[rxch-1]=true;
        }
        rxch=ICC(STK_RUD);
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
        if (tailtype==1) {
          md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
        }
        rx[rxch-1]=true;
        rxch=ICC(STK_ELE);
        if (tailtype==1) {
          md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
        }
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
        rx[rxch-1]=true;
        if (ail2ch > 0) {
          rx[ail2ch-1]=true;
        }
        if (ele2ch > 0) {
          rx[ele2ch-1]=true;
        }
        if (fla1ch > 0) {
          rx[fla1ch-1]=true;
        }
        if (fla2ch > 0) {
          rx[fla2ch-1]=true;
        }
        if (spo1ch > 0) {
          rx[spo1ch-1]=true;
        }
        if (spo2ch > 0) {
          rx[spo2ch-1]=true;
        }

        if (ailerons>0) {
          rxch=ICC(STK_AIL);
          md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
          rx[rxch-1]=true;
        }
        if (ailerons>1) {
          if (ail2ch==0) {
            for (int j=0; j<10 ; j++) {
              if (!rx[j]) {
                md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL2").toAscii().data(),6);
                rx[j]=true;
                break;
              }
            }
          } else {
            md=setDest(ail2ch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL2").toAscii().data(),6);
          }
        }
        if (elevators>1) {
          if (ele2ch==0) {
            for (int j=0; j<10 ; j++) {
              if (!rx[j]) {
                md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE2").toAscii().data(),6);
                rx[j]=true;
                break;
              }
            }
          } else {
            md=setDest(ele2ch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE2").toAscii().data(),6);          
          }
        }
        if (flaps>0) {
              md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6);md->speedUp=4; md->speedDown=4; 
              md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6);md->speedUp=4; md->speedDown=4; 
        }
        sign=-1;
        for (uint8_t i=0; i< flaps; i++) {
          sign*=-1;
          int index;
          if (i==0) {
            index=fla1ch;
          } else {
            index=fla2ch;
          }
          if (index==0) {
            for (int j=0; j<10 ; j++) {
              if (!rx[j]) {
                md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight=100*sign; md->sOffset=0; md->swtch=RawSwitch();strncpy(md->name, tr("FLAP%1").arg(i+1).toAscii().data(),6);
                rx[j]=true;
                break;
              }
            }
          } else {
            md=setDest(index);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight=100*sign; md->sOffset=0; md->swtch=RawSwitch();strncpy(md->name, tr("FLAP%1").arg(i+1).toAscii().data(),6);
          }
        }      
        if (spoilers>0) {
              md=setDest(12);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA);strncpy(md->name, tr("SPOIL").toAscii().data(),6); md->speedUp=4;;md->speedDown=4;
              md=setDest(12);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA);strncpy(md->name, tr("SPOIL").toAscii().data(),6);md->speedUp=4;md->speedDown=4;
        }
        sign=-1;
        for (uint8_t i=0; i< spoilers; i++) {
          sign*=-1;
          int index;
          if (i==0) {
            index=spo1ch;
          } else {
            index=spo2ch;
          }
          if (index==0) {
            for (int j=0; j<10 ; j++) {
              if (!rx[j]) {
                md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 11); md->weight=100*sign;  md->sOffset=0; md->swtch=RawSwitch();strncpy(md->name, tr("SPOIL%1").arg(i+1).toAscii().data(),6);
                rx[j]=true;
                break;
              }
            }
          } else {
            md=setDest(index);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 11);  md->weight=100*sign;  md->sOffset=0; md->swtch=RawSwitch();strncpy(md->name, tr("SPOIL%1").arg(i+1).toAscii().data(),6);
          }
        }
        break;
      case 3:
        flaps=flaptype;
        throttle=enginetype;
        rudders=ruddertype;
        if (throttle==1) {
         rxch=ICC(STK_THR);
         md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("THR").toAscii().data(),6);
         rx[rxch-1]=true;
        }
        rxch=ICC(STK_ELE);
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
        rx[rxch-1]=true;
        rxch=ICC(STK_AIL);
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
        rx[rxch-1]=true;
        if (rudders>0) {
          rxch=ICC(STK_RUD);
          md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
          rx[rxch-1]=true;
        }
        if (rud2ch > 0) {
          rx[rud2ch-1]=true;
        }
        if (fla1ch > 0) {
          rx[fla1ch-1]=true;
        }
        if (fla2ch > 0) {
          rx[fla2ch-1]=true;
        }
        if (rudders>1) {
          if (rud2ch==0) {
            for (int j=0; j<10 ; j++) {
              if (!rx[j]) {
                md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD2").toAscii().data(),6);
                rx[j]=true;
                break;
              }
            }
          } else {
            md=setDest(rud2ch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD2").toAscii().data(),6);
          }
        }
        if (flaps>0) {
              md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6); md->sOffset=0; md->speedUp=4;
              md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6); md->sOffset=0; md->speedUp=4;
        }
        sign=-1;
        for (uint8_t i=0; i< flaps; i++) {
          sign*=-1;
          int index;
          if (i==0) {
            index=fla1ch;
          } else {
            index=fla2ch;
          }
          if (index==0) {
            for (int j=0; j<10 ; j++) {
              if (!rx[j]) {
                md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight=100*sign; md->sOffset=0; md->speedUp=4; md->speedDown=4; md->swtch=RawSwitch();strncpy(md->name, tr("FLAP%1").arg(i+1).toAscii().data(),6);
                rx[j]=true;
                break;
              }
            }
          } else {
            md=setDest(index);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight=100*sign; md->sOffset=0; md->speedUp=4; md->speedDown=4; md->swtch=RawSwitch();strncpy(md->name, tr("FLAP%1").arg(i+1).toAscii().data(),6);
          }
        }      
        break;
    }
    updateHeliTab();
    updateCurvesTab();
    if (modeltype==1 && swashtype!=4) {
      ui->tabWidget->setCurrentIndex(1);
    } else {
      ui->tabWidget->setCurrentIndex(4);
    }
    resizeEvent();
}

void ModelEdit::applyTemplate(uint8_t idx)
{
    int8_t heli_ar1[] = {-100, -20, 30, 70, 90};
    int8_t heli_ar2[] = {80, 70, 60, 70, 100};
    int8_t heli_ar3[] = {100, 90, 80, 90, 100};
    int8_t heli_ar4[] = {-30,  -15, 0, 50, 100};
    int8_t heli_ar5[] = {-100, -50, 0, 50, 100};

    int thrsw=GetEepromInterface()->getCapability(GetThrSwitch);
    MixData *md = &g_model.mixData[0];

    //CC(STK)   -> vSTK
    //ICC(vSTK) -> STK
  #define ICC(x) icc[(x)-1]
    uint8_t icc[4] = {0};
    for(uint8_t i=1; i<=4; i++) //generate inverse array
      for(uint8_t j=1; j<=4; j++) if(CC(i)==j) icc[j-1]=i;


    uint8_t j = 0;

    //Simple 4-Ch
    if(idx==j++) {
      if (md->destCh)
        clearMixes();
      md=setDest(ICC(STK_RUD));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_ELE));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_THR));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_AIL));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();
    }

    //T-Cut
    if(idx==j++) {
        md=setDest(ICC(STK_THR));  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100;  md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw);  md->mltpx=MLTPX_REP;
    }

    //sticky t-cut
    if(idx==j++) {
      md=setDest(ICC(STK_THR));  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100;  md->swtch=RawSwitch(SWITCH_TYPE_VIRTUAL, 12);  md->mltpx=MLTPX_REP;
      md=setDest(14); md->srcRaw=RawSource(SOURCE_TYPE_CH, 13); md->weight= 100; md->swtch=RawSwitch();
      md=setDest(14); md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100;  md->swtch=RawSwitch(SWITCH_TYPE_VIRTUAL, 11);  md->mltpx=MLTPX_REP;
      md=setDest(14); md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight= 100;  md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw);  md->mltpx=MLTPX_REP;
      setSwitch(0xB, CS_FN_VNEG, RawSource(SOURCE_TYPE_STICK, 2).toValue(), -99);
      setSwitch(0xC, CS_FN_VPOS, RawSource(SOURCE_TYPE_CH, 13).toValue(), 0);
      updateSwitchesTab();
    }

    //V-Tail
    if(idx==j++) {
      clearMixes();
      md=setDest(ICC(STK_THR));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_AIL));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_RUD));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight= 100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_RUD));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_ELE));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight= 100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_ELE));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight= 100; md->swtch=RawSwitch();
    }

    //Elevon\\Delta
    if(idx==j++) {
      clearMixes();
      md=setDest(ICC(STK_THR));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_RUD));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_ELE));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight= 100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_ELE));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight= 100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_AIL));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();
      md=setDest(ICC(STK_AIL));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();
    }

    //Heli Setup  
    if(idx==j++)  {
      clearMixes();  //This time we want a clean slate
      clearCurves();

      // Set up Mixes
      // 3 cyclic channels
      md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 0);  md->weight= 100; md->swtch=RawSwitch();
      md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 1);  md->weight= 100; md->swtch=RawSwitch();
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 2);  md->weight= 100; md->swtch=RawSwitch();

      // rudder
      md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0); md->weight=100; md->swtch=RawSwitch();

      // throttle
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw); md->mltpx=MLTPX_REP;

      // gyro gain
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA);
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA);

      // collective
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;

      g_model.swashRingData.type = HELI_SWASH_TYPE_120;
      g_model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

      // set up Curves
      setCurve(CURVE5(1),heli_ar1);
      setCurve(CURVE5(2),heli_ar2);
      setCurve(CURVE5(3),heli_ar3);
      setCurve(CURVE5(4),heli_ar4);
      setCurve(CURVE5(5),heli_ar5);
      setCurve(CURVE5(6),heli_ar5);

      // make sure curves are redrawn
      updateHeliTab();
      updateCurvesTab();
      resizeEvent();
    }

    //Heli Setup  gyro gain control
    if(idx==j++)  {
      clearMixes();  //This time we want a clean slate
      clearCurves();

      // Set up Mixes
      // 3 cyclic channels
      md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 0);  md->weight= 100; md->swtch=RawSwitch();
      md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 1);  md->weight= 100; md->swtch=RawSwitch();
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 2);  md->weight= 100; md->swtch=RawSwitch();

      // rudder
      md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0); md->weight=100; md->swtch=RawSwitch();

      // throttle
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw); md->mltpx=MLTPX_REP;

      // gyro gain
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA); md->sOffset=100;
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA); md->sOffset=100;

      // collective
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;

      g_model.swashRingData.type = HELI_SWASH_TYPE_120;
      g_model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

      // set up Curves
      setCurve(CURVE5(1),heli_ar1);
      setCurve(CURVE5(2),heli_ar2);
      setCurve(CURVE5(3),heli_ar3);
      setCurve(CURVE5(4),heli_ar4);
      setCurve(CURVE5(5),heli_ar5);
      setCurve(CURVE5(6),heli_ar5);

      // make sure curves are redrawn
      updateHeliTab();
      updateCurvesTab();
      resizeEvent();
    }

    // gyro gain control 
    if(idx==j++)  {
      int res = QMessageBox::question(this,tr("Clear Mixes?"),tr("Really clear existing mixes on CH6?"),QMessageBox::Yes | QMessageBox::No);
      if(res!=QMessageBox::Yes) return;
      // first clear mix on ch6
      bool found=true;
      while (found) {
        found=false;
        for (int i=0; i< GetEepromInterface()->getCapability(Mixes); i++) {
          if (g_model.mixData[i].destCh==6) {
            gm_deleteMix(i);
            found=true;
            break;
          }
        }
      }
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA); md->sOffset=100;
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA); md->sOffset=100;    
    }

    //Heli Setup  futaba style
    if(idx==j++)  {
      clearMixes();  //This time we want a clean slate
      clearCurves();

      // Set up Mixes
      // 3 cyclic channels
      md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 1);  md->weight= 100; md->swtch=RawSwitch();
      md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 0);  md->weight= 100; md->swtch=RawSwitch();
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 2);  md->weight= 100; md->swtch=RawSwitch();

      // rudder
      md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0); md->weight=100; md->swtch=RawSwitch();

      // throttle
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw); md->mltpx=MLTPX_REP;

      // gyro gain
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA);
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA);

      // collective
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;

      g_model.swashRingData.type = HELI_SWASH_TYPE_120;
      g_model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

      // set up Curves
      setCurve(CURVE5(1),heli_ar1);
      setCurve(CURVE5(2),heli_ar2);
      setCurve(CURVE5(3),heli_ar3);
      setCurve(CURVE5(4),heli_ar4);
      setCurve(CURVE5(5),heli_ar5);
      setCurve(CURVE5(6),heli_ar5);

      // make sure curves are redrawn
      updateHeliTab();
      updateCurvesTab();
      resizeEvent();
    }

    // Heli setup futaba style with gyro gain control
    if(idx==j++)  {
      clearMixes();  //This time we want a clean slate
      clearCurves();

      // Set up Mixes
      // 3 cyclic channels
      md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 1);  md->weight= 100; md->swtch=RawSwitch();
      md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 0);  md->weight= 100; md->swtch=RawSwitch();
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 2);  md->weight= 100; md->swtch=RawSwitch();

      // rudder
      md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0); md->weight=100; md->swtch=RawSwitch();

      // throttle
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw); md->mltpx=MLTPX_REP;

      // gyro gain
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA); md->sOffset=100;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA); md->sOffset=100;

      // collective
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;

      g_model.swashRingData.type = HELI_SWASH_TYPE_120;
      g_model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

      // set up Curves
      setCurve(CURVE5(1),heli_ar1);
      setCurve(CURVE5(2),heli_ar2);
      setCurve(CURVE5(3),heli_ar3);
      setCurve(CURVE5(4),heli_ar4);
      setCurve(CURVE5(5),heli_ar5);
      setCurve(CURVE5(6),heli_ar5);

      // make sure curves are redrawn
      updateHeliTab();
      updateCurvesTab();
      resizeEvent();
    }

    // gyro gain control  futaba style
    if(idx==j++)  {
      int res = QMessageBox::question(this,tr("Clear Mixes?"),tr("Really clear existing mixes on CH5?"),QMessageBox::Yes | QMessageBox::No);
      if(res!=QMessageBox::Yes) return;
      // first clear mix on ch6
      bool found=true;
      while (found) {
        found=false;
        for (int i=0; i< GetEepromInterface()->getCapability(Mixes); i++) {
          if (g_model.mixData[i].destCh==5) {
            gm_deleteMix(i);
            found=true;
            break;
          }
        }
      }
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA); md->sOffset=100;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA); md->sOffset=100;    
    }

    //Servo Test
    if(idx==j++) {
      md=setDest(15); md->srcRaw=RawSource(SOURCE_TYPE_CH, 15);   md->weight= 100; md->speedUp = 8; md->speedDown = 8; md->swtch=RawSwitch();
      md=setDest(16); md->srcRaw=RawSource(SOURCE_TYPE_CUSTOM_SWITCH, 0); md->weight= 110; md->swtch=RawSwitch();
      md=setDest(16); md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-110; md->swtch=RawSwitch(SWITCH_TYPE_VIRTUAL, 2); md->mltpx=MLTPX_REP;
      md=setDest(16); md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight= 110; md->swtch=RawSwitch(SWITCH_TYPE_VIRTUAL, 3); md->mltpx=MLTPX_REP;
      setSwitch(1, CS_FN_LESS, RawSource(SOURCE_TYPE_CH, 14).toValue(), RawSource(SOURCE_TYPE_CH, 15).toValue());
      setSwitch(2, CS_FN_VPOS, RawSource(SOURCE_TYPE_CH, 14).toValue(), 105);
      setSwitch(3, CS_FN_VNEG, RawSource(SOURCE_TYPE_CH, 14).toValue(), -105);

      // redraw switches tab
      updateSwitchesTab();
    }

    //MultiCopter
    if(idx==j++) {
      if (md->destCh)
        clearMixes();
      md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=50; md->swtch=RawSwitch(); //CH1 AIL
      md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-50; md->swtch=RawSwitch(); //CH2 ELE
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(); //CH3 THR
      md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch(); //CH4 RUD
    }
}

void ModelEdit::ControlCurveSignal(bool flag)
{
    foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("curvePt[0-9]+"))) {
      sb->blockSignals(flag);
    }
}

void ModelEdit::shrink() {
#if defined __APPLE__
    QSettings settings("companion9x", "companion9x");
    QPoint pos = settings.value("mepos", QPoint(0, 0)).toPoint();
    QSize size = settings.value("mesize", QSize(800, 625)).toSize();
    move(pos);
    resize(size);    
# else
    const int height = QApplication::desktop()->height();
    QSettings settings("companion9x", "companion9x");
    QPoint pos = settings.value("mepos", QPoint(100, 100)).toPoint();
    QSize size = settings.value("mesize", QSize(800, 625)).toSize();
    if (size.height() < height) {
        move(pos);
        resize(size);    
    } else {
      resize(0,0);
    }
#endif  
  ui->curvePreview->repaint();
}

void ModelEdit::on_cname_LE_editingFinished() {
    if (GetEepromInterface()->getCapability(HasCvNames)) {
      int i=0;
      for (i=0; i<ui->cname_LE->text().toAscii().length(); i++) {
        g_model.curves[currentCurve].name[i]=ui->cname_LE->text().toAscii().at(i);
      }
      g_model.curves[currentCurve].name[i]=0;
    }
    updateSettings();
}

void ModelEdit::on_curvetype_CB_currentIndexChanged(int index) {
    if (curvesLock)
      return;
    int numcurves=GetEepromInterface()->getCapability(NumCurves);
    if (numcurves==0) {
      numcurves=16;
    }
    int numpoint[]={3,3,5,5,9,9,17,17};
    bool custom[]={false,true,false,true,false,true,false,true};
    int currpoints=g_model.curves[currentCurve].count;
    bool currcustom=g_model.curves[currentCurve].custom;
    curvesLock=true;
    int totalpoints=0;
    for (int i=0; i< numcurves; i++) {
      if (i!=currentCurve) {
        totalpoints+=g_model.curves[i].count;
        if (g_model.curves[i].custom) {
          totalpoints+=g_model.curves[i].count-2;
        }
      }
    }
    totalpoints+=numpoint[index];
    if (currcustom) {
      totalpoints+=numpoint[index]-2;
    }
    int fwpoints=GetEepromInterface()->getCapability(NumCurvePoints);
    if (fwpoints!=0) {
      if (fwpoints<totalpoints) {
        QMessageBox::warning(this, "companion9x", tr("Not enough free points in eeprom to store the curve."));
        int oldindex=0;
        if (currpoints==3) {
          oldindex=0;
        } else if (currpoints==5) {
          oldindex=2;
        } else if (currpoints==9) {
          oldindex=4;
        }  else if (currpoints==17) {
          oldindex=6;
        }
        if (currcustom) {
          index++;
        }
        ui->curvetype_CB->setCurrentIndex(oldindex);
        curvesLock=false;
        return;
      }
    }
    // let's be sure that for standard curves X values are set correctly.
    if (!currcustom) {
      for (int i=0; i< currpoints; i++) {
        spnx[i]->setMinimum(-100);
        spnx[i]->setMaximum(100);        
        spnx[i]->setValue(-100+((200*i)/(currpoints-1)));
      }    
    }
    if (numpoint[index]==currpoints) {
      for (int i=0; i< currpoints; i++) {
        spnx[i]->setMinimum(-100);
        spnx[i]->setMaximum(100);        
        spnx[i]->setValue(-100+((200*i)/(currpoints-1)));
      }
      for (int i=currpoints; i< 17; i++) {
        spnx[i]->setMinimum(-100);
        spnx[i]->setMaximum(100);
        spnx[i]->setValue(0);
        spny[i]->setValue(0);
      }
    } else if (numpoint[index]>currpoints) {
      for (int i=0; i< 17; i++) {
        g_model.curves[currentCurve].points[i].x=spnx[i]->value();
        g_model.curves[currentCurve].points[i].y=spny[i]->value();
        spnx[i]->setMinimum(-100);
        spnx[i]->setMaximum(100);
      }
      int currintervals=currpoints-1;
      int diffpoints=numpoint[index]-currpoints;
      int skip=diffpoints/currintervals;
      for (int i=0; i< currpoints; i++) {
        if (custom[index]) {
          spnx[i+(skip*i)]->setValue(g_model.curves[currentCurve].points[i].x);
        } else {
          spnx[i+(skip*i)]->setValue(-100+(200*i)/currintervals);
        }
        spny[i+(skip*i)]->setValue(g_model.curves[currentCurve].points[i].y);
        if (i>0) {
            int diffx=spnx[i+(skip*i)]->value()-spnx[(i-1)+(skip*(i-1))]->value();
            int diffy=spny[i+(skip*i)]->value()-spny[(i-1)+(skip*(i-1))]->value();
            for (int j=1; j<= skip; j++) {
                spny[(i-1)+skip*(i-1)+j]->setValue(spny[(i-1)+(skip*(i-1))]->value()+((diffy*j)/(skip+1)));
                spnx[(i-1)+skip*(i-1)+j]->setValue(spnx[(i-1)+(skip*(i-1))]->value()+((diffx*j)/(skip+1)));
            }
        }
      }    
    } else {
      int intervals=numpoint[index]-1;
      int diffpoints=currpoints-numpoint[index];
      int skip=diffpoints/intervals;
      for (int i=0; i< numpoint[index]; i++) {
        spnx[i]->setMinimum(-100);
        spnx[i]->setMaximum(100);
        if (custom[index]) {
          spnx[i]->setValue(spnx[i+skip*i]->value());
        } else {
          spnx[i]->setValue(-100+(200*i)/intervals);
        }
        spny[i]->setValue(spny[i+skip*i]->value());
      }
      for (int i=numpoint[index]; i< 17; i++) {
        spnx[i]->setValue(0);
        spny[i]->setValue(0);
      }
    }
    curvesLock=false;
    for (int i=0; i< 17; i++) {
      g_model.curves[currentCurve].points[i].x=spnx[i]->value();
      g_model.curves[currentCurve].points[i].y=spny[i]->value();
    }
    g_model.curves[currentCurve].count=numpoint[index];
    g_model.curves[currentCurve].custom=custom[index];
    setCurrentCurve(currentCurve);
    if (redrawCurve)
      drawCurve();
    updateSettings();
}

void ModelEdit::closeEvent(QCloseEvent *event)
{
    QSettings settings("companion9x", "companion9x");
    settings.setValue("mepos", pos());
    settings.setValue("mesize", size());
    event->accept();
}

void ModelEdit::wizard()
{
    uint64_t result=0xffffffff;
    modelConfigDialog *mcw = new modelConfigDialog(radioData, &result, this);
    mcw->exec();
    if (result!=0xffffffff) {
      applyNumericTemplate(result);
      updateSettings();
      tabMixes();
    } else if (this->isNew) {
      memset(&g_model, 0, sizeof(ModelData));
      accept();
    }
}
