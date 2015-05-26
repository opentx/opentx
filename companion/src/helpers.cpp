#include <QtGui>
#if defined WIN32
  #include <io.h>
  #include <stdio.h>
#endif
#if !defined WIN32 && defined __GNUC__
  #include <unistd.h>
#endif 
#include "appdata.h"
#include "helpers.h"
#include "simulatordialog.h"
#include "simulatorinterface.h"
#include "firmwareinterface.h"

const QColor colors[C9X_MAX_CURVES] = {
  QColor(0,0,127),
  QColor(0,127,0),
  QColor(127,0,0),
  QColor(0,127,127),
  QColor(127,0,127),
  QColor(127,127,0),
  QColor(127,127,127),
  QColor(0,0,255),
  QColor(0,127,255),
  QColor(127,0,255),
  QColor(0,255,0),
  QColor(0,255,127),
  QColor(127,255,0),
  QColor(255,0,0),
  QColor(255,0,127),
  QColor(255,127,0),
  QColor(0,0,127),
  QColor(0,127,0),
  QColor(127,0,0),
  QColor(0,127,127),
  QColor(127,0,127),
  QColor(127,127,0),
  QColor(127,127,127),
  QColor(0,0,255),
  QColor(0,127,255),
  QColor(127,0,255),
  QColor(0,255,0),
  QColor(0,255,127),
  QColor(127,255,0),
  QColor(255,0,0),
  QColor(255,0,127),
  QColor(255,127,0),
};

QString getPhaseName(int val, const char * phasename)
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

QString getInputStr(ModelData * model, int index)
{
  QString result;

  if (GetCurrentFirmware()->getCapability(VirtualInputs)) {
    if (strlen(model->inputNames[index]) > 0) {
      result = QObject::tr("[I%1]").arg(index+1);
      result += QString(model->inputNames[index]);
    }
    else {
      result = QObject::tr("Input%1").arg(index+1, 2, 10, QChar('0'));
    }
  }
  else {
    result = RawSource(SOURCE_TYPE_STICK, index).toString(model);
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
  for (int i=-GetCurrentFirmware()->getCapability(FlightModes); i<=GetCurrentFirmware()->getCapability(FlightModes); i++) {
    if (i < 0)
      b->addItem(QObject::tr("!Flight mode %1").arg(-i-1), i);
    else if (i > 0)
      b->addItem(QObject::tr("Flight mode %1").arg(i-1), i);
    else
      b->addItem(QObject::tr("----"), 0);
  }
  b->setCurrentIndex(value + GetCurrentFirmware()->getCapability(FlightModes));
}

GVarGroup::GVarGroup(QCheckBox *weightGV, QAbstractSpinBox *weightSB, QComboBox *weightCB, int & weight, const int deflt, const int mini, const int maxi, const double step, bool allowGvars):
  QObject(),
  weightGV(weightGV),
  weightSB(weightSB),
  sb(dynamic_cast<QSpinBox *>(weightSB)),
  dsb(dynamic_cast<QDoubleSpinBox *>(weightSB)),
  weightCB(weightCB),
  weight(weight),
  step(step),
  lock(true)
{
  if (allowGvars && GetCurrentFirmware()->getCapability(Gvars)) {
    populateGVCB(weightCB, weight);
    connect(weightGV, SIGNAL(stateChanged(int)), this, SLOT(gvarCBChanged(int)));
    connect(weightCB, SIGNAL(currentIndexChanged(int)), this, SLOT(valuesChanged()));
  }
  else {
    weightGV->hide();
    if (weight > maxi || weight < mini) {
      weight = deflt;
    }
  }

  int val;

  if (weight>maxi || weight<mini) {
    val = deflt;
    weightGV->setChecked(true);
    weightSB->hide();
    weightCB->show();
  }
  else {
    val = weight;
    weightGV->setChecked(false);
    weightSB->show();
    weightCB->hide();
  }

  if (sb) {
    sb->setMinimum(mini);
    sb->setMaximum(maxi);
    sb->setValue(val);
  }
  else {
    dsb->setMinimum(mini*step);
    dsb->setMaximum(maxi*step);
    dsb->setValue(val*step);
  }

  connect(weightSB, SIGNAL(editingFinished()), this, SLOT(valuesChanged()));

  lock = false;
}

void GVarGroup::gvarCBChanged(int state)
{
  weightCB->setVisible(state);
  if (weightSB)
    weightSB->setVisible(!state);
  else
    weightSB->setVisible(!state);
  valuesChanged();
}

void GVarGroup::valuesChanged()
{
  if (!lock) {
    if (weightGV->isChecked())
      weight = weightCB->itemData(weightCB->currentIndex()).toInt();
    else if (sb)
      weight = sb->value();
    else
      weight = dsb->value()/step;
  }
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
  if (!(flags & HIDE_DIFF)) curveTypeCB->addItem(tr("Diff"), 0);
  if (!(flags & HIDE_EXPO)) curveTypeCB->addItem(tr("Expo"), 1);
  curveTypeCB->addItem(tr("Func"), 2);
  curveTypeCB->addItem(tr("Curve"), 3);

  curveValueCB->setMaxVisibleItems(10);

  connect(curveTypeCB, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));
  connect(curveGVarCB, SIGNAL(stateChanged(int)), this, SLOT(gvarCBChanged(int)));
  connect(curveValueCB, SIGNAL(currentIndexChanged(int)), this, SLOT(valuesChanged()));
  connect(curveValueSB, SIGNAL(editingFinished()), this, SLOT(valuesChanged()));

  update();
}

void CurveGroup::update()
{
  lock = true;

  int found = curveTypeCB->findData(curve.type);
  if (found < 0) found = 0;
  curveTypeCB->setCurrentIndex(found);

  if (curve.type == CurveReference::CURVE_REF_DIFF || curve.type == CurveReference::CURVE_REF_EXPO) {
    curveGVarCB->setVisible(GetCurrentFirmware()->getCapability(Gvars));
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
        int numcurves = GetCurrentFirmware()->getCapability(NumCurves);
        if (lastType != curve.type) {
          lastType = curve.type;
          curveValueCB->clear();
          for (int i= ((flags & HIDE_NEGATIVE_CURVES) ? 0 : -numcurves); i<=numcurves; i++) {
            curveValueCB->addItem(CurveReference(CurveReference::CURVE_REF_CUSTOM, i).toString(), i);
            if (i == curve.value) {
              curveValueCB->setCurrentIndex(curveValueCB->count() - 1);
            }
          }
        }
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
      curve.value = 10000+1; // TODO constant in EEpromInterface ...
    }
    else {
      curve.value = 0; // TODO could be better
    }

    update();
  }
}

void CurveGroup::typeChanged(int value)
{
  if (!lock) {
    int type = curveTypeCB->itemData(curveTypeCB->currentIndex()).toInt();
    switch (type) {
      case 0:
        curve = CurveReference(CurveReference::CURVE_REF_DIFF, 0);
        break;
      case 1:
        curve = CurveReference(CurveReference::CURVE_REF_EXPO, 0);
        break;
      case 2:
        curve = CurveReference(CurveReference::CURVE_REF_FUNC, 0);
        break;
      case 3:
        curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, 0);
        break;
    }

    update();
  }
}

void CurveGroup::valuesChanged()
{
  if (!lock) {
    switch (curveTypeCB->itemData(curveTypeCB->currentIndex()).toInt()) {
      case 0:
      case 1:
      {
        int value;
        if (curveGVarCB->isChecked())
          value = curveValueCB->itemData(curveValueCB->currentIndex()).toInt();
        else
          value = curveValueSB->value();
        curve = CurveReference(curveTypeCB->itemData(curveTypeCB->currentIndex()).toInt() == 0 ? CurveReference::CURVE_REF_DIFF : CurveReference::CURVE_REF_EXPO, value);
        break;
      }
      case 2:
        curve = CurveReference(CurveReference::CURVE_REF_FUNC, curveValueCB->currentIndex());
        break;
      case 3:
        curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, curveValueCB->itemData(curveValueCB->currentIndex()).toInt());
        break;
    }

    update();
  }
}

void populateGvarUseCB(QComboBox *b, unsigned int phase)
{
  b->addItem(QObject::tr("Own value"));
  for (int i=0; i<GetCurrentFirmware()->getCapability(FlightModes); i++) {
    if (i != (int)phase) {
      b->addItem(QObject::tr("Flight mode %1 value").arg(i));
    }
  }
}

void populateSwitchCB(QComboBox *b, const RawSwitch & value, const GeneralSettings & generalSettings, SwitchContext context)
{
  RawSwitch item;

  b->clear();

  if (context != MixesContext && context != GlobalFunctionsContext) {
    // !FMx
    if (IS_ARM(GetCurrentFirmware()->getBoard())) {
      for (int i=-GetCurrentFirmware()->getCapability(FlightModes); i<0; i++) {
        item = RawSwitch(SWITCH_TYPE_FLIGHT_MODE, i);
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  if (context != GlobalFunctionsContext) {
    for (int i=-GetCurrentFirmware()->getCapability(LogicalSwitches); i<0; i++) {
      item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  for (int i=-GetCurrentFirmware()->getCapability(RotaryEncoders); i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_ROTARY_ENCODER, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=-8; i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_TRIM, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=GetCurrentFirmware()->getCapability(MultiposPots)-1; i>=0; i--) {
    if (generalSettings.potConfig[i] == GeneralSettings::POT_MULTIPOS_SWITCH) {
      for (int j=-GetCurrentFirmware()->getCapability(MultiposPotsPositions); j<0; j++) {
        item = RawSwitch(SWITCH_TYPE_MULTIPOS_POT, -i*GetCurrentFirmware()->getCapability(MultiposPotsPositions)+j);
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  for (int i=-GetCurrentFirmware()->getCapability(SwitchesPositions); i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_SWITCH, i);
    if (IS_TARANIS(GetCurrentFirmware()->getBoard()) && !generalSettings.switchPositionAllowedTaranis(i)){
      continue;
    }
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  if (context == TimersContext) {
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

  for (int i=1; i<=GetCurrentFirmware()->getCapability(SwitchesPositions); i++) {
    item = RawSwitch(SWITCH_TYPE_SWITCH, i);
    if (IS_TARANIS(GetCurrentFirmware()->getBoard()) && !generalSettings.switchPositionAllowedTaranis(i)){
      continue;
    }
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=0; i<GetCurrentFirmware()->getCapability(MultiposPots); i++) {
    if (generalSettings.potConfig[i] == GeneralSettings::POT_MULTIPOS_SWITCH) {
      for (int j=1; j<=GetCurrentFirmware()->getCapability(MultiposPotsPositions); j++) {
        item = RawSwitch(SWITCH_TYPE_MULTIPOS_POT, i*GetCurrentFirmware()->getCapability(MultiposPotsPositions)+j);
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  for (int i=1; i<=8; i++) {
    item = RawSwitch(SWITCH_TYPE_TRIM, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=1; i<=GetCurrentFirmware()->getCapability(RotaryEncoders); i++) {
    item = RawSwitch(SWITCH_TYPE_ROTARY_ENCODER, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  if (context != GlobalFunctionsContext) {
    for (int i=1; i<=GetCurrentFirmware()->getCapability(LogicalSwitches); i++) {
      item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  if (context == SpecialFunctionsContext || context == GlobalFunctionsContext) {
    // ON
    item = RawSwitch(SWITCH_TYPE_ON);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
    // One
    item = RawSwitch(SWITCH_TYPE_ONE, 1);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  // FMx
  if (context != MixesContext && context != GlobalFunctionsContext) {
    if (IS_ARM(GetCurrentFirmware()->getBoard())) {
      for (int i=1; i<=GetCurrentFirmware()->getCapability(FlightModes); i++) {
        item = RawSwitch(SWITCH_TYPE_FLIGHT_MODE, i);
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  b->setMaxVisibleItems(10);
}

void populateGVCB(QComboBox *b, int value)
{
  bool selected = false;

  b->clear();

  int pgvars = GetCurrentFirmware()->getCapability(Gvars);
  for (int i=-pgvars; i<=-1; i++) {
    int16_t gval = (int16_t)(-10000+i);
    b->addItem(QObject::tr("-GV%1").arg(-i), gval);
    if (value == gval) {
      b->setCurrentIndex(b->count()-1);
      selected = true;
    }
  }

  for (int i=1; i<=pgvars; i++) {
    int16_t gval = (int16_t)(10000+i);
    b->addItem(QObject::tr("GV%1").arg(i), gval);
    if (value == gval) {
      b->setCurrentIndex(b->count()-1);
      selected = true;
    }
  }

  if (!selected) {
    b->setCurrentIndex(pgvars);
  }
}

void populateSourceCB(QComboBox *b, const RawSource & source, const GeneralSettings generalSettings, const ModelData * model, unsigned int flags)
{
  BoardEnum board = GetCurrentFirmware()->getBoard();
  RawSource item;

  b->clear();

  if (flags & POPULATE_NONE) {
    item = RawSource(SOURCE_TYPE_NONE);
    b->addItem(item.toString(model), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }

  if (flags & POPULATE_SCRIPT_OUTPUTS) {
    for (int i=0; i<GetCurrentFirmware()->getCapability(LuaScripts); i++) {
      for (int j=0; j<GetCurrentFirmware()->getCapability(LuaOutputsPerScript); j++) {
        item = RawSource(SOURCE_TYPE_LUA_OUTPUT, i*16+j);
        b->addItem(item.toString(model), item.toValue());
        if (item == source) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  if (model && (flags & POPULATE_VIRTUAL_INPUTS)) {
    int virtualInputs = GetCurrentFirmware()->getCapability(VirtualInputs);
    for (int i=0; i<virtualInputs; i++) {
      if (model->isInputValid(i)) {
        item = RawSource(SOURCE_TYPE_VIRTUAL_INPUT, i);
        b->addItem(item.toString(model), item.toValue());
        if (item == source) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  if (flags & POPULATE_SOURCES) {
    for (int i=0; i<4+GetCurrentFirmware()->getCapability(Pots); i++) {
      item = RawSource(SOURCE_TYPE_STICK, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
    for (int i=0; i<GetCurrentFirmware()->getCapability(RotaryEncoders); i++) {
      item = RawSource(SOURCE_TYPE_ROTARY_ENCODER, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_TRIMS) {
    for (int i=0; i<4; i++) {
      item = RawSource(SOURCE_TYPE_TRIM, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_SOURCES) {
    item = RawSource(SOURCE_TYPE_MAX);
    b->addItem(item.toString(model), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }
  
  if (flags & POPULATE_SWITCHES) {
    for (int i=0; i<GetCurrentFirmware()->getCapability(Switches); i++) {
      item = RawSource(SOURCE_TYPE_SWITCH, i);
      b->addItem(item.toString(model), item.toValue());
      if (IS_TARANIS(GetCurrentFirmware()->getBoard()) && !generalSettings.switchSourceAllowedTaranis(i)) {
        QModelIndex index = b->model()->index(b->count()-1, 0);
        QVariant v(0);
        b->model()->setData(index, v, Qt::UserRole - 1);
      }
      if (item == source) b->setCurrentIndex(b->count()-1);
    }

    for (int i=0; i<GetCurrentFirmware()->getCapability(LogicalSwitches); i++) {
      item = RawSource(SOURCE_TYPE_CUSTOM_SWITCH, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_SOURCES) {
    for (int i=0; i<NUM_CYC; i++) {
      item = RawSource(SOURCE_TYPE_CYC, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }

    for (int i=0; i<GetCurrentFirmware()->getCapability(TrainerInputs); i++) {
      item = RawSource(SOURCE_TYPE_PPM, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }

    for (int i=0; i<GetCurrentFirmware()->getCapability(Outputs); i++) {
      item = RawSource(SOURCE_TYPE_CH, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_TELEMETRY) {
    if (IS_ARM(GetCurrentFirmware()->getBoard())) {
      for (int i=0; i<5; ++i) {
        item = RawSource(SOURCE_TYPE_SPECIAL, i);
        b->addItem(item.toString(model), item.toValue());
        if (item == source) b->setCurrentIndex(b->count()-1);
      }
      for (int i=0; i<C9X_MAX_SENSORS; ++i) {
        if (model->sensorData[i].isAvailable()) {
          for (int j=0; j<3; ++j) {
            item = RawSource(SOURCE_TYPE_TELEMETRY, 3*i+j);
            b->addItem(item.toString(model), item.toValue());
            // qDebug() << item.toString(model) << source.toString(model);
            if (item == source) b->setCurrentIndex(b->count()-1);
          }
        }
      }
    }
    else {
      for (int i=0; i<(flags & POPULATE_TELEMETRYEXT ? TELEMETRY_SOURCES_STATUS_COUNT : TELEMETRY_SOURCES_COUNT); i++) {
        if (i==TELEMETRY_SOURCE_RSSI_TX && IS_TARANIS(board))
          continue;
        if (i==TELEMETRY_SOURCE_TX_TIME && !GetCurrentFirmware()->getCapability(RtcTime))
          continue;
        if (i==TELEMETRY_SOURCE_SWR && !GetCurrentFirmware()->getCapability(SportTelemetry))
          continue;
        if (i==TELEMETRY_SOURCE_TIMER3 && !IS_ARM(board))
          continue;
        item = RawSource(SOURCE_TYPE_TELEMETRY, i);
        b->addItem(item.toString(model), item.toValue());
        if (item == source) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  if (flags & POPULATE_GVARS) {
    for (int i=0; i<GetCurrentFirmware()->getCapability(Gvars); i++) {
      item = RawSource(SOURCE_TYPE_GVAR, i);
      b->addItem(item.toString(model), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  b->setMaxVisibleItems(10);
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
      if ((GetCurrentFirmware()->getCapability(Telemetry) & TM_HASWSHH))
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
    case -2:
      return QObject::tr("Exponential");
    case -1:
      return QObject::tr("Extra Fine");
    case 0:
      return QObject::tr("Fine");
    case 1:
      return QObject::tr("Medium");
    case 2:
      return QObject::tr("Coarse");
    default:
      return QObject::tr("Unknown");
  }
}

QString getTimerStr(TimerData & timer)
{
  QString result = QObject::tr("%1:%2").arg(timer.val/60, 2, 10, QChar('0')).arg(timer.val%60, 2, 10, QChar('0'));
  result += QString(", ") + timer.mode.toString();
  if (timer.persistent)
    result += QObject::tr(", Persistent");
  if (timer.minuteBeep)
    result += QObject::tr(", MinuteBeep");
  if (timer.countdownBeep == 1)
    result += QObject::tr(", CountDown(Beeps)");
  else if (timer.countdownBeep == 2)
    result += QObject::tr(", CountDown(Voice)");
  return result;
}

QString getProtocol(ModuleData & module)
{
  QString str = getProtocolStr(module.protocol);

  if (module.protocol == PPM)
    str.append(QObject::tr(": Channel start: %1, %2 Channels, %3usec Delay, Pulse polarity %4").arg(module.channelsStart+1).arg(module.channelsCount).arg(module.ppmDelay).arg(module.polarityToString()));
  else
    str.append(QObject::tr(": Channel start: %1, %2 Channels").arg(module.channelsStart+1).arg(module.channelsCount));
  return str;
}

QString getTrainerMode(const int trainermode, ModuleData & module)
{
  QString result;
  switch (trainermode) {
    case 1:
      result=QObject::tr("Slave/Jack")+QObject::tr(": Channel start: %1, %2 Channels, %3usec Delay, Pulse polarity %4").arg(module.channelsStart+1).arg(module.channelsCount).arg(module.ppmDelay).arg(module.polarityToString());
      break;
    case 2:
      result=QObject::tr("Master/SBUS Module");
      break;
    case 3:
      result=QObject::tr("Master/CPPM Module");
      break;
    case 4:
      result=QObject::tr("Master/SBUS in battery compartment");
      break;
    default:
      result=QObject::tr("Master/Jack");
  }
  return result;
}

QString getPhasesStr(unsigned int phases, ModelData * model)
{
  int numphases = GetCurrentFirmware()->getCapability(FlightModes);

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
          str += getPhaseName(i+1, model->flightModeData[i].name);
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

QString getCenterBeepStr(ModelData * g_model)
{
  QStringList strl;
  if (g_model->beepANACenter & 0x01)
    strl << QObject::tr("Rudder");
  if (g_model->beepANACenter & 0x02)
    strl << QObject::tr("Elevator");
  if (g_model->beepANACenter & 0x04)
    strl << QObject::tr("Throttle");
  if (g_model->beepANACenter & 0x08)
    strl << QObject::tr("Aileron");
  if (IS_TARANIS(GetCurrentFirmware()->getBoard())) {
    if (g_model->beepANACenter & 0x10)
      strl << "S1";
    if (g_model->beepANACenter & 0x20)
      strl << "S2";
    if (g_model->beepANACenter & 0x40)
      strl << "S3";
    if (g_model->beepANACenter & 0x80)
      strl << "LS";
    if (g_model->beepANACenter & 0x100)
      strl << "RS";
  }
  else {
    if (g_model->beepANACenter & 0x10)
      strl << "P1";
    if (g_model->beepANACenter & 0x20)
      strl << "P2";
    if (g_model->beepANACenter & 0x40)
      strl << "P3";
  }
  return strl.join(", ");
}

QString getTheme()
{
  int theme_set = g.theme();
  QString Theme;
  switch(theme_set) {
    case 0:
      Theme="classic";
      break;
    case 2:
      Theme="monowhite";
      break;
    case 3:
      Theme="monochrome";
      break;
    case 4:
      Theme="monoblue";
      break;
    default:
      Theme="yerico";
      break;          
  }
  return Theme;
}

CompanionIcon::CompanionIcon(const QString &baseimage)
{
  static QString theme = getTheme();
  addFile(":/themes/"+theme+"/16/"+baseimage, QSize(16,16));
  addFile(":/themes/"+theme+"/24/"+baseimage, QSize(24,24));
  addFile(":/themes/"+theme+"/32/"+baseimage, QSize(32,32));
  addFile(":/themes/"+theme+"/48/"+baseimage, QSize(48,48));
}

void startSimulation(QWidget * parent, RadioData & radioData, int modelIdx)
{
  SimulatorInterface * si = GetCurrentFirmwareSimulator();
  if (si) {
    RadioData * simuData = new RadioData(radioData);
    unsigned int flags = 0;
    if (modelIdx >= 0) {
      flags |= SIMULATOR_FLAGS_NOTX;
      simuData->generalSettings.currModel = modelIdx;
    }
    if (radioData.generalSettings.stickMode & 1) {
      flags |= SIMULATOR_FLAGS_STICK_MODE_LEFT;
    }
    BoardEnum board = GetCurrentFirmware()->getBoard();
    SimulatorDialog * sd;
    if (IS_TARANIS(board)) {
      for (int i=0; i<GetCurrentFirmware()->getCapability(Pots); i++) {
        if (radioData.generalSettings.potConfig[i] != GeneralSettings::POT_NONE) {
          flags |= (SIMULATOR_FLAGS_S1 << i);
          if (radioData.generalSettings.potConfig[1] == GeneralSettings::POT_MULTIPOS_SWITCH ) {
            flags |= (SIMULATOR_FLAGS_S1_MULTI << i);
          }
        }
      }
      sd = new SimulatorDialogTaranis(parent, si, flags);
    }
    else {
      sd = new SimulatorDialog9X(parent, si, flags);
    }
    QByteArray eeprom(GetEepromInterface()->getEEpromSize(), 0);
    GetEepromInterface()->save((uint8_t *)eeprom.data(), *simuData, GetCurrentFirmware()->getCapability(SimulatorVariant));
    delete simuData;
    sd->start(eeprom);
    sd->exec();
    delete sd;
  }
  else {
    QMessageBox::warning(NULL,
      QObject::tr("Warning"),
      QObject::tr("Simulator for this firmware is not yet available"));
  }
}

QPixmap makePixMap( QImage image, QString firmwareType )
{
  if (firmwareType.contains( "taranis" )) {
    image = image.convertToFormat(QImage::Format_RGB32);
    QRgb col;
    int gray;
    for (int i = 0; i < image.width(); ++i) {
      for (int j = 0; j < image.height(); ++j) {
        col = image.pixel(i, j);
        gray = qGray(col);
        image.setPixel(i, j, qRgb(gray, gray, gray));
      }
    }
    image = image.scaled(SPLASHX9D_WIDTH, SPLASHX9D_HEIGHT); 
  } 
  else {
    image = image.scaled(SPLASH_WIDTH, SPLASH_HEIGHT).convertToFormat(QImage::Format_Mono);
  }
  return(QPixmap::fromImage(image));
}

int version2index(QString version)
{
  QStringList parts = version.split('.');
  int result = 0;
  if (parts.size() > 2)
    result = parts[2].toInt();
  if (parts.size() > 1)
    result += 100 * parts[1].toInt();
  if (parts.size() > 0)
    result += 10000 * parts[0].toInt();
  return result;
}

QString index2version(int index)
{
  if (index >= 19900) {
    int revision = index % 100;
    index /= 100;
    int minor = index % 100;
    int major = index / 100;
    return QString("%1.%2.%3").arg(major).arg(minor).arg(revision);
  }
  else {
    return QString();
  }
}

int qunlink(const QString & fileName)
{
  QByteArray ba = fileName.toLatin1();
  return unlink(ba.constData());
}

QString generateProcessUniqueTempFileName(const QString &fileName)
{
  QString sanitizedFileName = fileName;
  sanitizedFileName.remove('/');
  return QDir::tempPath() + QString("/%1-").arg(QCoreApplication::applicationPid()) + sanitizedFileName;
}

bool isTempFileName(const QString & fileName)
{
  return fileName.startsWith(QDir::tempPath());
}

QString getSoundsPath(const GeneralSettings &generalSettings)
{
  QString path = g.profile[g.id()].sdPath() + "/SOUNDS/";
  QString lang = generalSettings.ttsLanguage;
  if (lang.isEmpty())
    lang = "en";
  path.append(lang);
  return path;
}

QSet<QString> getFilesSet(const QString &path, const QStringList &filter, int maxLen)
{
  QSet<QString> result;
  QDir dir(path);
  if (dir.exists()) {
    foreach (QString filename, dir.entryList(filter, QDir::Files)) {
      QFileInfo file(filename);
      QString name = file.completeBaseName();
      if (name.length() <= maxLen) {
        result.insert(name);
      }
    }
  }
  return result;
}

bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
  return s1.toLower() < s2.toLower();
}
