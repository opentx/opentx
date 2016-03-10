#include "templates.h"

#define CHANNEL_ORDER(x) generalSettings.getDefaultStick(x)
#define CC(x) (CHANNEL_ORDER(x)) //need to invert this to work with dest
// TODO ICC is GeneralSettings::getDefaultChannel(unsigned int stick)

void ModelEdit::setCurve(uint8_t c, int8_t ar[])
{
  int len=sizeof(ar)/sizeof(int8_t);

  if (firmware->getCapability(NumCurves)>c) {
    if (len<9) {
      model.curves[c].count=5;
      model.curves[c].custom=false;
      for (int i=0; i< 5; i++) {
        model.curves[c].points[i].y=ar[i];
      }
    }
    else {
      model.curves[c].count=5;
      model.curves[c].custom=false;
      for (int i=0; i< 5; i++) {
        model.curves[c].points[i].y=ar[i];
      }
    }
  }
}

Templates::Templates(QWidget * parent, ModelData & model):
  QWidget(parent),
  model(model)
{
  QGridLayout * gridLayout = new QGridLayout(this);
  QListWidget * templateList = new QListWidget(this);
  gridLayout->addWidget(templateList, 0, 0, 1, 1);

  templateList->addItem(tr("Simple 4-CH"));
  templateList->addItem(tr("T-Cut"));
  templateList->addItem(tr("Sticky T-Cut"));
  templateList->addItem(tr("V-Tail"));
  templateList->addItem(tr("Elevon\\Delta"));
  templateList->addItem(tr("Heli Setup"));
  templateList->addItem(tr("Heli Setup with gyro gain control"));
  templateList->addItem(tr("Gyro gain control"));
  templateList->addItem(tr("Heli Setup (Futaba's channel assignment style)"));
  templateList->addItem(tr("Heli Setup with gyro gain control (Futaba's channel assignment style)"));
  templateList->addItem(tr("Gyro gain control (Futaba's channel assignment style)"));
  templateList->addItem(tr("Servo Test"));
  templateList->addItem(tr("MultiCopter"));
  templateList->addItem(tr("Use Model Config Wizard"));

  connect(templateList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
}

Templates::~Templates()
{
}

void Templates::setSwitch(unsigned int idx, unsigned int func, int v1, int v2)
{
    g_model.logicalSw[idx-1].func = func;
    g_model.logicalSw[idx-1].val1   = v1;
    g_model.logicalSw[idx-1].val2   = v2;
}

void Templates::onDoubleClicked(QModelIndex index)
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

void Templates::applyNumericTemplate(uint64_t tpl)
{
    clearCurves();
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
    int thrsw=firmware->getCapability(GetThrSwitch);
    MixData *md = &model.mixData[0];
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
              md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6); md->speedUp=4; md->speedDown=4;
              md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6); md->speedUp=4; md->speedDown=4;
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
            model.swashRingData.type = HELI_SWASH_TYPE_90;
            break;
          case 1:
            model.swashRingData.type = HELI_SWASH_TYPE_120;
            break;
          case 2:
            model.swashRingData.type = HELI_SWASH_TYPE_120X;
            break;
          case 3:
            model.swashRingData.type = HELI_SWASH_TYPE_140;
            break;
          case 4:
            model.swashRingData.type = HELI_SWASH_TYPE_NONE;
            break;
        }
        model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

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
          md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
          md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
          md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
          md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw); md->mltpx=MLTPX_REP;
          switch (gyro) {
            case 1:
              md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_GEA);strncpy(md->name, tr("GYRO").toAscii().data(),6);
              md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_GEA);strncpy(md->name, tr("GYRO").toAscii().data(),6);
              break;
            case 2:
              md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_GEA); md->sOffset=100;strncpy(md->name, tr("GYRO").toAscii().data(),6);
              md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_GEA); md->sOffset=100;strncpy(md->name, tr("GYRO").toAscii().data(),6);
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
          md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
          md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
          md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
          md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw); md->mltpx=MLTPX_REP;
          switch (gyro) {
            case 1:
              md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_GEA);strncpy(md->name, tr("GYRO").toAscii().data(),6);
              md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_GEA);strncpy(md->name, tr("GYRO").toAscii().data(),6);
              break;
            case 2:
              md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_GEA); md->sOffset=100;strncpy(md->name, tr("GYRO").toAscii().data(),6);
              md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_GEA); md->sOffset=100;strncpy(md->name, tr("GYRO").toAscii().data(),6);
              break;
          }
        }
        // collective
        md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
        md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
        md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;
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
              md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6);md->speedUp=4; md->speedDown=4;
              md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6);md->speedUp=4; md->speedDown=4;
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
              md=setDest(12);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_GEA);strncpy(md->name, tr("SPOIL").toAscii().data(),6); md->speedUp=4;;md->speedDown=4;
              md=setDest(12);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_GEA);strncpy(md->name, tr("SPOIL").toAscii().data(),6);md->speedUp=4;md->speedDown=4;
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
              md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6); md->sOffset=0; md->speedUp=4;
              md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6); md->sOffset=0; md->speedUp=4;
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

void Templates::applyTemplate(uint8_t idx)
{
    int8_t heli_ar1[] = {-100, -20, 30, 70, 90};
    int8_t heli_ar2[] = {80, 70, 60, 70, 100};
    int8_t heli_ar3[] = {100, 90, 80, 90, 100};
    int8_t heli_ar4[] = {-30,  -15, 0, 50, 100};
    int8_t heli_ar5[] = {-100, -50, 0, 50, 100};

    int thrsw=firmware->getCapability(GetThrSwitch);
    MixData *md = &model.mixData[0];

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
      setSwitch(0xB, LS_FN_VNEG, RawSource(SOURCE_TYPE_STICK, 2).toValue(), -99);
      setSwitch(0xC, LS_FN_VPOS, RawSource(SOURCE_TYPE_CH, 13).toValue(), 0);
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
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw); md->mltpx=MLTPX_REP;

      // gyro gain
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_GEA);
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_GEA);

      // collective
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;

      model.swashRingData.type = HELI_SWASH_TYPE_120;
      model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

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
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw); md->mltpx=MLTPX_REP;

      // gyro gain
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_GEA); md->sOffset=100;
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_GEA); md->sOffset=100;

      // collective
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;

      model.swashRingData.type = HELI_SWASH_TYPE_120;
      model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

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
        for (int i=0; i< firmware->getCapability(Mixes); i++) {
          if (model.mixData[i].destCh==6) {
            gm_deleteMix(i);
            found=true;
            break;
          }
        }
      }
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_GEA); md->sOffset=100;
      md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_GEA); md->sOffset=100;
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
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw); md->mltpx=MLTPX_REP;

      // gyro gain
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_GEA);
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_GEA);

      // collective
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;

      model.swashRingData.type = HELI_SWASH_TYPE_120;
      model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

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
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
      md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,thrsw); md->mltpx=MLTPX_REP;

      // gyro gain
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_GEA); md->sOffset=100;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_GEA); md->sOffset=100;

      // collective
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;

      model.swashRingData.type = HELI_SWASH_TYPE_120;
      model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

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
        for (int i=0; i< firmware->getCapability(Mixes); i++) {
          if (model.mixData[i].destCh==5) {
            gm_deleteMix(i);
            found=true;
            break;
          }
        }
      }
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-SWITCH_GEA); md->sOffset=100;
      md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,SWITCH_GEA); md->sOffset=100;
    }

    //Servo Test
    if(idx==j++) {
      md=setDest(15); md->srcRaw=RawSource(SOURCE_TYPE_CH, 15);   md->weight= 100; md->speedUp = 8; md->speedDown = 8; md->swtch=RawSwitch();
      md=setDest(16); md->srcRaw=RawSource(SOURCE_TYPE_CUSTOM_SWITCH, 0); md->weight= 110; md->swtch=RawSwitch();
      md=setDest(16); md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-110; md->swtch=RawSwitch(SWITCH_TYPE_VIRTUAL, 2); md->mltpx=MLTPX_REP;
      md=setDest(16); md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight= 110; md->swtch=RawSwitch(SWITCH_TYPE_VIRTUAL, 3); md->mltpx=MLTPX_REP;
      setSwitch(1, LS_FN_LESS, RawSource(SOURCE_TYPE_CH, 14).toValue(), RawSource(SOURCE_TYPE_CH, 15).toValue());
      setSwitch(2, LS_FN_VPOS, RawSource(SOURCE_TYPE_CH, 14).toValue(), 105);
      setSwitch(3, LS_FN_VNEG, RawSource(SOURCE_TYPE_CH, 14).toValue(), -105);

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

MixData* Templates::setDest(uint8_t dch)
{
    uint8_t i = 0;
    while ((g_model.mixData[i].destCh<=dch) && (g_model.mixData[i].destCh) && (i<firmware->getCapability(Mixes))) i++;
    if(i==firmware->getCapability(Mixes)) return &g_model.mixData[0];

    memmove(&g_model.mixData[i+1],&g_model.mixData[i],
            (firmware->getCapability(Mixes)-(i+1))*sizeof(MixData) );
    memset(&g_model.mixData[i],0,sizeof(MixData));
    g_model.mixData[i].destCh = dch;
    return &g_model.mixData[i];
}
