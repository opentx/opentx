#include <QtGui>
#include "helpers.h"

QString getPhaseName(int val, char * phasename)
{
  if (!val) return "---";
  if (!phasename) {
    return QString(val < 0 ? "!" : "") + QObject::tr("FM%1").arg(abs(val) - 1);
  }
  else {
    QString phaseName;
    phaseName.append(phasename);
    if (phaseName.isEmpty()) {
      return QString(val < 0 ? "!" : "") + QObject::tr("FM%1").arg(abs(val) - 1);
    }
    else {
      return QString(val < 0 ? "!" : "") + phaseName;
    }
  }
}

QString getInputStr(ModelData & model, int index)
{
  QString result;

  if (GetEepromInterface()->getCapability(VirtualInputs)) {
    result = model.inputNames[index];
    if (result.isEmpty()) {
      result = QObject::tr("Input%1").arg(index+1, 2, 10, QChar('0'));
    }
  }
  else {
    result = RawSource(SOURCE_TYPE_STICK, index).toString();
  }

  return result;
}

void populateGvSourceCB(QComboBox *b, int value)
{
  QString strings[] = { QObject::tr("---"), QObject::tr("Rud Trim"), QObject::tr("Ele Trim"), QObject::tr("Thr Trim"), QObject::tr("Ail Trim"), QObject::tr("Rot Enc"), QObject::tr("Rud"), QObject::tr("Ele"), QObject::tr("Thr"), QObject::tr("Ail"), QObject::tr("P1"), QObject::tr("P2"), QObject::tr("P3")};
  b->clear();
  for (int i=0; i<= 12; i++) {
    b->addItem(strings[i]);
  }
  b->setCurrentIndex(value);
}

void populateVoiceLangCB(QComboBox *b, QString language)
{
  QString strings[] = { QObject::tr("English"), QObject::tr("French"), QObject::tr("Italian"), QObject::tr("German"), QObject::tr("Czech"), QObject::tr("Slovak"), QObject::tr("Spanish"), QObject::tr("Polish"), QObject::tr("Portuguese"), QObject::tr("Swedish"), NULL};
  QString langcode[] = { "en", "fr", "it", "de", "cz", "sk", "es", "pl", "pt", "se", NULL};
  
  b->clear();
  for (int i=0; strings[i]!=NULL; i++) {
    b->addItem(strings[i],langcode[i]);
    if (language==langcode[i]) {
      b->setCurrentIndex(b->count()-1);
    }
  }
}

void populateTTraceCB(QComboBox *b, int value)
{
  const QString strings9x[] = { QObject::tr("THR"), QObject::tr("P1"), QObject::tr("P2"), QObject::tr("P3")};
  const QString stringstaranis[] = { QObject::tr("THR"), QObject::tr("S1"), QObject::tr("S2"), QObject::tr("LS"), QObject::tr("RS")};
  b->clear();
  if (IS_TARANIS(GetEepromInterface()->getBoard())) {
    for (int i=0; i< 5; i++) {
      b->addItem(stringstaranis[i]);
    }
  } else {
    for (int i=0; i< 4; i++) {
      b->addItem(strings9x[i]);
    }
  }
  int channels=(IS_ARM(GetEepromInterface()->getBoard()) ? 32 : 16);
  for (int i=1; i<= channels; i++) {
    b->addItem(QObject::tr("CH%1").arg(i, 2, 10, QChar('0')));
  }
  b->setCurrentIndex(value);
}

void populateRotEncCB(QComboBox *b, int value, int renumber)
{
  QString strings[] = { QObject::tr("No"), QObject::tr("RotEnc A"), QObject::tr("Rot Enc B"), QObject::tr("Rot Enc C"), QObject::tr("Rot Enc D"), QObject::tr("Rot Enc E")};
  
  b->clear();
  for (int i=0; i<= renumber; i++) {
    b->addItem(strings[i]);
  }
  b->setCurrentIndex(value);
}

void populateCustomScreenFieldCB(QComboBox *b, unsigned int value, bool last=false, int hubproto=0)
{
  int telem_hub[] = {0,0,0,0,0,0,0,0,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,0,0,2,2,1,1,1,1,1,1};
  b->clear();

  b->addItem(RawSource(SOURCE_TYPE_NONE, 0).toString());

  for (unsigned int i = 0; i < (last ? TELEMETRY_SOURCES_DISPLAY_COUNT : TELEMETRY_SOURCES_STATUS_COUNT); i++) {
    b->addItem(RawSource(SOURCE_TYPE_TELEMETRY, i).toString());
    if (!(i>=sizeof(telem_hub)/sizeof(int) || telem_hub[i]==0 || ((telem_hub[i]>=hubproto) && hubproto!=0))) {
      QModelIndex index = b->model()->index(i, 0);
      QVariant v(0);
      b->model()->setData(index, v, Qt::UserRole - 1);
    }
  }

  if (value>=sizeof(telem_hub)/sizeof(int))
    b->setCurrentIndex(0);
  else if (telem_hub[value]==0 || ((telem_hub[value]>=hubproto) && hubproto!=0)) {
    b->setCurrentIndex(value);
  }

  b->setMaxVisibleItems(10);
}

QString getProtocolStr(const int proto)
{
  static const char *strings[] = { "OFF",
                                   "PPM",
                                   "Silverlit A", "Silverlit B", "Silverlit C",
                                   "CTP1009",
                                   "LP45", "DSM2", "DSMX",
                                   "PPM16", "PPMsim",
                                   "FrSky XJT - D16", "FrSky XJT - D8", "FrSky XJT - LR12", "FrSky DJT",
  };

  return CHECK_IN_ARRAY(strings, proto);
}

void populatePhasesCB(QComboBox *b, int value)
{
  for (int i=-GetEepromInterface()->getCapability(FlightPhases); i<=GetEepromInterface()->getCapability(FlightPhases); i++) {
    if (i < 0)
      b->addItem(QObject::tr("!Flight mode %1").arg(-i-1), i);
    else if (i > 0)
      b->addItem(QObject::tr("Flight mode %1").arg(i-1), i);
    else
      b->addItem(QObject::tr("----"), 0);
  }
  b->setCurrentIndex(value + GetEepromInterface()->getCapability(FlightPhases));
}

bool gvarsEnabled()
{
  int gvars=0;
  if (GetEepromInterface()->getCapability(HasVariants)) {
    if ((GetCurrentFirmwareVariant() & GVARS_VARIANT)) {
      gvars=1;
    }
  }
  else {
    gvars=1;
  }
  return gvars;
}

GVarGroup::GVarGroup(QCheckBox *weightGV, QSpinBox *weightSB, QComboBox *weightCB, int & weight, const int deflt, const int mini, const int maxi, const unsigned int flags):
  QObject(),
  weightGV(weightGV),
  weightSB(weightSB),
  weightCB(weightCB),
  weight(weight),
  flags(flags),
  lock(false)
{
  lock = true;

  if (gvarsEnabled()) {
    populateGVCB(weightCB, weight);
    connect(weightGV, SIGNAL(stateChanged(int)), this, SLOT(gvarCBChanged(int)));
    connect(weightCB, SIGNAL(currentIndexChanged(int)), this, SLOT(valuesChanged()));
  }
  else {
    weightGV->hide();
    if (weight > maxi || weight < -mini) {
      weight = deflt;
    }
  }

  weightSB->setMinimum(mini);
  weightSB->setMaximum(maxi);

  if (weight>maxi || weight<mini) {
    weightGV->setChecked(true);
    weightSB->hide();
    weightCB->show();
  }
  else {
    weightGV->setChecked(false);
    weightSB->setValue(weight);
    weightSB->show();
    weightCB->hide();
  }

  connect(weightSB, SIGNAL(editingFinished()), this, SLOT(valuesChanged()));

  lock = false;
}

void GVarGroup::gvarCBChanged(int state)
{
  weightCB->setVisible(state);
  weightSB->setVisible(!state);
  valuesChanged();
}

void GVarGroup::valuesChanged()
{
  if (weightGV->isChecked())
    weight = weightCB->itemData(weightCB->currentIndex()).toInt();
  else
    weight = weightSB->value();
}

CurveGroup::CurveGroup(QComboBox *curveTypeCB, QCheckBox *curveGVarCB, QComboBox *curveValueCB, QSpinBox *curveValueSB, CurveReference & curve, unsigned int flags):
  QObject(),
  curveTypeCB(curveTypeCB),
  curveGVarCB(curveGVarCB),
  curveValueCB(curveValueCB),
  curveValueSB(curveValueSB),
  curve(curve),
  flags(flags),
  lock(false),
  lastType(-1)
{
  curveTypeCB->addItem(tr("Diff"));
  curveTypeCB->addItem(tr("Expo"));
  curveTypeCB->addItem(tr("Func"));
  curveTypeCB->addItem(tr("Curve"));

  curveValueCB->setMaxVisibleItems(10);

  connect(curveTypeCB, SIGNAL(currentIndexChanged(int)), this, SLOT(valuesChanged()));
  connect(curveGVarCB, SIGNAL(stateChanged(int)), this, SLOT(gvarCBChanged(int)));
  connect(curveValueCB, SIGNAL(currentIndexChanged(int)), this, SLOT(valuesChanged()));
  connect(curveValueSB, SIGNAL(editingFinished()), this, SLOT(valuesChanged()));

  update();
}

void CurveGroup::update()
{
  lock = true;

  curveTypeCB->setCurrentIndex(curve.type);

  if (curve.type == CurveReference::CURVE_REF_DIFF || curve.type == CurveReference::CURVE_REF_EXPO) {
    curveGVarCB->show();
    if (curve.value > 100 || curve.value < -100) {
      curveGVarCB->setChecked(true);
      if (lastType != CurveReference::CURVE_REF_DIFF && lastType != CurveReference::CURVE_REF_EXPO) {
        lastType = curve.type;
        populateGVCB(curveValueCB, curve.value);
      }
      curveValueCB->show();
      curveValueSB->hide();
    }
    else {
      curveGVarCB->setChecked(false);
      curveValueSB->setMinimum(-100);
      curveValueSB->setMaximum(100);
      curveValueSB->setValue(curve.value);
      curveValueSB->show();
      curveValueCB->hide();
    }
  }
  else {
    curveGVarCB->hide();
    curveValueSB->hide();
    curveValueCB->show();
    switch (curve.type) {
      case CurveReference::CURVE_REF_FUNC:
        if (lastType != curve.type) {
          lastType = curve.type;
          curveValueCB->clear();
          for (int i=0; i<=6/*TODO constant*/; i++) {
            curveValueCB->addItem(CurveReference(CurveReference::CURVE_REF_FUNC, i).toString());
          }
        }
        curveValueCB->setCurrentIndex(curve.value);
        break;
      case CurveReference::CURVE_REF_CUSTOM:
      {
        int numcurves = GetEepromInterface()->getCapability(NumCurves);
        if (lastType != curve.type) {
          lastType = curve.type;
          curveValueCB->clear();
          for (int i=-numcurves; i<numcurves; i++) {
            curveValueCB->addItem(CurveReference(CurveReference::CURVE_REF_CUSTOM, i).toString());
          }
        }
        curveValueCB->setCurrentIndex(curve.value+numcurves);
        break;
      }
      default:
        break;
    }
  }

  lock = false;
}

void CurveGroup::gvarCBChanged(int state)
{
  if (!lock) {
    if (state) {
      curve.value = 10000; // TODO constant in EEpromInterface ...
    }
    else {
      curve.value = 0; // TODO could be better
    }

    update();
  }
}

void CurveGroup::valuesChanged()
{
  if (!lock) {
    switch (curveTypeCB->currentIndex()) {
      case 0:
      case 1:
      {
        int value;
        if (curveGVarCB->isChecked())
          value = curveValueCB->itemData(curveValueCB->currentIndex()).toInt();
        else
          value = curveValueSB->value();
        curve = CurveReference(curveTypeCB->currentIndex() == 0 ? CurveReference::CURVE_REF_DIFF : CurveReference::CURVE_REF_EXPO, value);
        break;
      }
      case 2:
        curve = CurveReference(CurveReference::CURVE_REF_FUNC, curveValueCB->currentIndex());
        break;
      case 3:
        curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, curveValueCB->currentIndex() - GetEepromInterface()->getCapability(NumCurves));
        break;
    }

    update();
  }
}

void populateGvarUseCB(QComboBox *b, unsigned int phase)
{
  b->addItem(QObject::tr("Own value"));
  for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
    if (i != (int)phase) {
      b->addItem(QObject::tr("Flight mode %1 value").arg(i));
    }
  }
}

void populateBacklightCB(QComboBox *b, const uint8_t value)
{
  QString strings[] = { QObject::tr("OFF"), QObject::tr("Keys"), QObject::tr("Sticks"), QObject::tr("Keys + Sticks"), QObject::tr("ON"), NULL };

  b->clear();

  for (int i=0; !strings[i].isNull(); i++) {
    b->addItem(strings[i], 0);
    if (value == i) b->setCurrentIndex(b->count()-1);
  }
}

void populateAndSwitchCB(QComboBox *b, const RawSwitch & value)
{
  if (IS_ARM(GetEepromInterface()->getBoard())) {
    populateSwitchCB(b, value);
  }
  else {
    RawSwitch item;

    b->clear();

    item = RawSwitch(SWITCH_TYPE_NONE);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);

    for (int i=1; i<=GetEepromInterface()->getCapability(SwitchesPositions); i++) {
      item = RawSwitch(SWITCH_TYPE_SWITCH, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }

    for (int i=1; i<=6; i++) {
      item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }
}

void populateSwitchCB(QComboBox *b, const RawSwitch & value, unsigned long attr)
{
  RawSwitch item;

  b->clear();

  if (attr & POPULATE_ONOFF) {
    item = RawSwitch(SWITCH_TYPE_OFF);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=-GetEepromInterface()->getCapability(LogicalSwitches); i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=-GetEepromInterface()->getCapability(RotaryEncoders); i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_ROTARY_ENCODER, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=-8; i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_TRIM, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=-GetEepromInterface()->getCapability(MultiposPots) * GetEepromInterface()->getCapability(MultiposPotsPositions); i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_MULTIPOS_POT, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=-GetEepromInterface()->getCapability(SwitchesPositions); i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_SWITCH, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  if (attr & POPULATE_TIMER_MODES) {
    for (int i=0; i<5; i++) {
      item = RawSwitch(SWITCH_TYPE_TIMER_MODE, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }
  else {
    item = RawSwitch(SWITCH_TYPE_NONE);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=1; i<=GetEepromInterface()->getCapability(SwitchesPositions); i++) {
    item = RawSwitch(SWITCH_TYPE_SWITCH, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=1; i<=GetEepromInterface()->getCapability(MultiposPots) * GetEepromInterface()->getCapability(MultiposPotsPositions); i++) {
    item = RawSwitch(SWITCH_TYPE_MULTIPOS_POT, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=1; i<=8; i++) {
    item = RawSwitch(SWITCH_TYPE_TRIM, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=1; i<=GetEepromInterface()->getCapability(RotaryEncoders); i++) {
    item = RawSwitch(SWITCH_TYPE_ROTARY_ENCODER, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=1; i<=GetEepromInterface()->getCapability(LogicalSwitches); i++) {
    item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  if (attr & POPULATE_ONOFF) {
    item = RawSwitch(SWITCH_TYPE_ON);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  b->setMaxVisibleItems(10);
}

void populateGVCB(QComboBox *b, int value)
{
  int selected=0;
  int nullitem;

  b->clear();

  int pgvars = GetEepromInterface()->getCapability(Gvars);
  for (int i=-pgvars; i<=-1; i++) {
    int16_t gval = (int16_t)(-10000+i);
    b->addItem(QObject::tr("-GV%1").arg(-i), gval);
    if (value == gval) {
      b->setCurrentIndex(b->count()-1);
      selected=1;
    }
  }

  b->addItem("---", 0);

  nullitem=b->count()-1;
  if (value == 0) {
    b->setCurrentIndex(b->count()-1);
    selected=1;
  }

  for (int i=1; i<=pgvars; i++) {
    int16_t gval = (int16_t)(10000+i);
    b->addItem(QObject::tr("GV%1").arg(i), gval);
    if (value == gval) {
      b->setCurrentIndex(b->count()-1);
      selected=1;
    }
  }

  if (selected==0)
    b->setCurrentIndex(nullitem);
}

void populateSourceCB(QComboBox *b, const RawSource & source, const ModelData & model, unsigned int flags)
{
  RawSource item;

  b->clear();

  if (flags & POPULATE_SOURCES) {
    item = RawSource(SOURCE_TYPE_NONE);
    b->addItem(item.toString(), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }

  if (flags & POPULATE_VIRTUAL_INPUTS) {
    int virtualInputs = GetEepromInterface()->getCapability(VirtualInputs);
    for (int i=0; i<virtualInputs; i++) {
      item = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, i, &model);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_SOURCES) {
    for (int i=0; i<4+GetEepromInterface()->getCapability(Pots); i++) {
      item = RawSource(SOURCE_TYPE_STICK, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
    for (int i=0; i<GetEepromInterface()->getCapability(RotaryEncoders); i++) {
      item = RawSource(SOURCE_TYPE_ROTARY_ENCODER, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_TRIMS) {
    for (int i=0; i<4; i++) {
      item = RawSource(SOURCE_TYPE_TRIM, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_SOURCES) {
    item = RawSource(SOURCE_TYPE_MAX);
    b->addItem(item.toString(), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }
  
  if (flags & POPULATE_SWITCHES) {
    for (int i=0; i<GetEepromInterface()->getCapability(Switches); i++) {
      item = RawSource(SOURCE_TYPE_SWITCH, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }

    for (int i=0; i<GetEepromInterface()->getCapability(LogicalSwitches); i++) {
      item = RawSource(SOURCE_TYPE_CUSTOM_SWITCH, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_SOURCES) {
    for (int i=0; i<NUM_CYC; i++) {
      item = RawSource(SOURCE_TYPE_CYC, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }

    for (int i=0; i<NUM_PPM; i++) {
      item = RawSource(SOURCE_TYPE_PPM, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }

    for (int i=0; i<GetEepromInterface()->getCapability(Outputs)+GetEepromInterface()->getCapability(ExtraChannels); i++) {
      item = RawSource(SOURCE_TYPE_CH, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_TELEMETRYEXT) {
    for (int i=0; i<TELEMETRY_SOURCE_ACC; i++) {
      item = RawSource(SOURCE_TYPE_TELEMETRY, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }
  else if (flags & POPULATE_TELEMETRY) {
    for (int i=0; i<TELEMETRY_SOURCES_COUNT; i++) {
      item = RawSource(SOURCE_TYPE_TELEMETRY, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_GVARS) {
    for (int i=0; i<GetEepromInterface()->getCapability(Gvars); i++) {
      item = RawSource(SOURCE_TYPE_GVAR, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  b->setMaxVisibleItems(10);
}

float ValToTim(int value)
{
   return ((value < -109 ? 129+value : (value < 7 ? (113+value)*5 : (53+value)*10))/10.0);   
}

int TimToVal(float value)
{
  int temp;
  if (value>60) {
    temp=136+round((value-60));
  } else if (value>2) {
    temp=20+round((value-2.0)*2.0);
  } else {
    temp=round(value*10.0);
  }
  return (temp-129);
}  

void populateCSWCB(QComboBox *b, int value)
{
  int order[] = {
    LS_FN_OFF,
    LS_FN_VEQUAL, // added at the end to avoid everything renumbered
    LS_FN_VPOS,
    LS_FN_VNEG,
    // LS_FN_RANGE,
    LS_FN_APOS,
    LS_FN_ANEG,
    LS_FN_AND,
    LS_FN_OR,
    LS_FN_XOR,
    LS_FN_STAY,
    LS_FN_EQUAL,
    LS_FN_NEQUAL,
    LS_FN_GREATER,
    LS_FN_LESS,
    LS_FN_EGREATER,
    LS_FN_ELESS,
    LS_FN_DPOS,
    LS_FN_DAPOS,
    LS_FN_TIMER,
    LS_FN_STICKY
  };

  b->clear();
  for (int i=0; i<LS_FN_MAX; i++) {
    int func = order[i];
    b->addItem(LogicalSwitchData(func).funcToString(), func);
//    if (i>GetEepromInterface()->getCapability(CSFunc)) {
//      QModelIndex index = b->model()->index(i, 0);
//      QVariant v(0);
//    }
    if (value == func) {
      b->setCurrentIndex(b->count()-1);
    }
  }
  b->setMaxVisibleItems(10);
}

QString getSignedStr(int value)
{
  return value > 0 ? QString("+%1").arg(value) : QString("%1").arg(value);
}

QString getGVarString(int16_t val, bool sign)
{
  if (val >= -10000 && val <= 10000) {
    if (sign)
      return QString("%1%").arg(getSignedStr(val));
    else
      return QString("%1%").arg(val);
  }
  else {
    if (val<0)
      return QObject::tr("-GV%1").arg(-val-10000);
    else
      return QObject::tr("GV%1").arg(val-10000);
  }
}

QString image2qstring(QImage image)
{   
    if (image.isNull())
      return "";
    QBuffer buffer;
    image.save(&buffer, "PNG");
    QString ImageStr;
    int b=0;
    int size=buffer.data().size();
    for (int j = 0; j < size; j++) {
      b=buffer.data().at(j);
      ImageStr += QString("%1").arg(b&0xff, 2, 16, QChar('0'));
    }
    return ImageStr;
}

QImage qstring2image(QString imagestr)
{
  bool ok;
  bool failed=false;
  QImage Image;
  int len = imagestr.length();
  char b=0;
  QBuffer buffer;
  buffer.open(QIODevice::ReadWrite);
  buffer.seek(0);
  for (int i = 0; i < len/2; i++) {
    QString Byte;
    Byte = imagestr.mid((i * 2), 2);
    b = Byte.toUInt(&ok, 16);
    if (!ok) {
      failed = true;
    }
    buffer.putChar(b);
  }
  buffer.seek(0);
  if (!failed) {
    Image.load(&buffer,"PNG");
  }  
  return Image;
}

int findmult(float value, float base)
{
  int vvalue = value*10;
  int vbase = base*10;
  vvalue--;

  int mult = 0;
  for (int i=8; i>=0; i--) {
    if (vvalue/vbase >= (1<<i)) {
      mult = i+1;
      break;
    }
  }
  
  return mult;
}

QString getFrSkyAlarmType(int alarm)
{
  switch (alarm) {
    case 1:
      return QObject::tr("Yellow");
    case 2:
      return QObject::tr("Orange");
    case 3:
      return QObject::tr("Red");
    default:
      return "----";
  }
}

QString getFrSkyBlades(int blades)
{
  switch (blades) {
    case 1:
      return "3";
    case 2:
      return "4";
    default:
      return "2";
  }
}


QString getFrSkyUnits(int units)
{
  switch(units) {
    case 1:
      return QObject::tr("---");
    default:
      return "V";
  }
}

QString getFrSkyProtocol(int protocol)
{
  switch(protocol) {
    case 2:
      if ((GetEepromInterface()->getCapability(Telemetry) & TM_HASWSHH))
        return QObject::tr("Winged Shadow How High");
      else
        return QObject::tr("Winged Shadow How High (not supported)");
    case 1:
      return QObject::tr("FrSky Sensor Hub");
    default:
      return QObject::tr("None");
  }
}

QString getFrSkyMeasure(int units)
{
  switch(units) {
    case 1:
      return QObject::tr("Imperial");
    default:
      return QObject::tr("Metric");
  }
}

QString getFrSkySrc(int index)
{
  return RawSource(SOURCE_TYPE_TELEMETRY, index-1).toString();
}

QString getTrimInc(ModelData * g_model)
{
    switch (g_model->trimInc) {
      case (1): return QObject::tr("Extra Fine");
      case (2): return QObject::tr("Fine");
      case (3): return QObject::tr("Medium");
      case (4): return QObject::tr("Coarse");
      default: return QObject::tr("Exponential");
    }
}

QString getTimerStr(TimerData & timer)
{
  QString str = ", " + (timer.dir ? QObject::tr("Count Up") : QObject::tr("Count Down"));
  return QObject::tr("%1:%2, ").arg(timer.val/60, 2, 10, QChar('0')).arg(timer.val%60, 2, 10, QChar('0')) + timer.mode.toString() + str;
}

QString getProtocol(ModelData * g_model)
{
  QString str = getProtocolStr(g_model->moduleData[0].protocol);

  if (g_model->moduleData[0].protocol == PPM)
    str.append(QObject::tr(": %1 Channels, %2usec Delay").arg(g_model->moduleData[0].channelsCount).arg(g_model->moduleData[0].ppmDelay));

  return str;
}

QString getPhasesStr(unsigned int phases, ModelData & model)
{
  int numphases = GetEepromInterface()->getCapability(FlightPhases);

  if (numphases && phases) {
    QString str;
    int count = 0;
    if (phases == (unsigned int)(1<<numphases) - 1) {
      str = QObject::tr("None");
    }
    if (phases) {
      for (int i=0; i<numphases;i++) {
        if (!(phases & (1<<i))) {
          if (count++ > 0) str += QString(", ");
          str += getPhaseName(i+1, model.phaseData[i].name);
        }
      }
    }
    if (count > 1)
      return QObject::tr("Flight modes(%1)").arg(str);
    else
      return QObject::tr("Flight mode(%1)").arg(str);
  }
  else {
    return "";
  }
}

QString getCenterBeep(ModelData * g_model)
{
  //RETA123
  QStringList strl;
  if(g_model->beepANACenter & 0x01) strl << QObject::tr("Rudder");
  if(g_model->beepANACenter & 0x02) strl << QObject::tr("Elevator");
  if(g_model->beepANACenter & 0x04) strl << QObject::tr("Throttle");
  if(g_model->beepANACenter & 0x08) strl << QObject::tr("Aileron");
  if(g_model->beepANACenter & 0x10) strl << "P1";
  if(g_model->beepANACenter & 0x20) strl << "P2";
  if(g_model->beepANACenter & 0x40) strl << "P3";
  if(g_model->beepANACenter & 0x80) strl << "LS";
  return strl.join(", ");
}

QString getTheme()
{
  QSettings settings;
  int theme_set = settings.value("theme", 1).toInt();
  QString Theme;
  switch(theme_set) {
    case 0:
      Theme="classic";
      break;
    case 2:
      Theme="monowhite";
      break;
    case 3:
      Theme="monoblue";
      break;
    default:
      Theme="monochrome";
      break;          
  }
  return Theme;
}

CompanionIcon::CompanionIcon(QString baseimage)
{
  static QString theme = getTheme();
  addFile(":/themes/"+theme+"/16/"+baseimage, QSize(16,16));
  addFile(":/themes/"+theme+"/24/"+baseimage, QSize(24,24));
  addFile(":/themes/"+theme+"/32/"+baseimage, QSize(32,32));
  addFile(":/themes/"+theme+"/48/"+baseimage, QSize(48,48));
}

