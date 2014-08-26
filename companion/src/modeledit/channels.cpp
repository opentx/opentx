#include "channels.h"
#include "helpers.h"
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>

LimitsGroup::LimitsGroup(FirmwareInterface * firmware, QGridLayout *gridLayout, int row, int col, int & value, int min, int max, int deflt):
  firmware(firmware),
  spinbox(new QDoubleSpinBox()),
  value(value),
  step(1.0)
{

  bool allowGVars = false;

  spinbox->setProperty("index", row);
  spinbox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  spinbox->setAccelerated(true);

  if (firmware->getCapability(PPMUnitMicroseconds)) {
    step = 5.12;
    spinbox->setDecimals(1);
    spinbox->setSuffix("us");
  }
  else {
    spinbox->setDecimals(0);
    spinbox->setSuffix("%");
  }

  if (IS_TARANIS(firmware->getBoard()) || deflt == 0 /*it's the offset*/) {
    min *= 10;
    max *= 10;
    deflt *= 10;
    step /= 10;
    spinbox->setDecimals(1);
    allowGVars = true;
  }

  spinbox->setSingleStep(step);

  QHBoxLayout * horizontalLayout = new QHBoxLayout();
  QCheckBox * gv = new QCheckBox(QObject::tr("GV"));
  horizontalLayout->addWidget(gv);
  QComboBox * cb = new QComboBox();
  horizontalLayout->addWidget(cb);
  horizontalLayout->addWidget(spinbox);
  gridLayout->addLayout(horizontalLayout, row, col, 1, 1);
  gvarGroup = new GVarGroup(gv, spinbox, cb, value, deflt, min, max, step, allowGVars);
}

LimitsGroup::~LimitsGroup()
{
  delete gvarGroup;
}

void LimitsGroup::updateMinMax(int max)
{
  if (IS_TARANIS(firmware->getBoard())) {
    max *= 10;
  }
  if (spinbox->maximum() == 0) {
    spinbox->setMinimum(-step*max);
    if (value < -max) {
      value = -max;
    }
  }
  if (spinbox->minimum() == 0) {
    spinbox->setMaximum(step*max);
    if (value > max) {
      value = max;
    }
  }
}

Channels::Channels(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware):
  ModelPanel(parent, model, generalSettings, firmware)
{
  QGridLayout * gridLayout = new QGridLayout(this);
  bool minimize = false;

  int col = 1;
  if (firmware->getCapability(ChannelsName)) {
    minimize=true;
    addLabel(gridLayout, tr("Name"), col++);
  }
  addLabel(gridLayout, tr("Subtrim"), col++, minimize);
  addLabel(gridLayout, tr("Min"), col++, minimize);
  addLabel(gridLayout, tr("Max"), col++, minimize);
  addLabel(gridLayout, tr("Direction"), col++, minimize);
  if (IS_TARANIS(GetEepromInterface()->getBoard()))
    addLabel(gridLayout, tr("Curve"), col++, minimize);
  if (firmware->getCapability(PPMCenter))
    addLabel(gridLayout, tr("PPM Center"), col++, minimize);
  if (firmware->getCapability(SYMLimits))
    addLabel(gridLayout, tr("Linear Subtrim"), col++, true);

  for (int i=0; i<firmware->getCapability(Outputs); i++) {
    col = 0;

    // Channel label
    QLabel *label = new QLabel(this);
    label->setText(tr("Channel %1").arg(i+1));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    gridLayout->addWidget(label, i+1, col++, 1, 1);

    // Channel name
    int nameLen = firmware->getCapability(ChannelsName);
    if (nameLen > 0) {
      QLineEdit * name = new QLineEdit(this);
      name->setProperty("index", i);
      name->setMaxLength(nameLen);
      QRegExp rx(CHAR_FOR_NAMES_REGEX);
      name->setValidator(new QRegExpValidator(rx, this));
      name->setText(model.limitData[i].name);
      connect(name, SIGNAL(editingFinished()), this, SLOT(nameEdited()));
      gridLayout->addWidget(name, i+1, col++, 1, 1);
    }

    // Channel offset
    limitsGroups << new LimitsGroup(firmware, gridLayout, i+1, col++, model.limitData[i].offset, -100, 100, 0);

    // Channel min
    limitsGroups << new LimitsGroup(firmware, gridLayout, i+1, col++, model.limitData[i].min, -model.getChannelsMax(), 0, -100);

    // Channel max
    limitsGroups << new LimitsGroup(firmware, gridLayout, i+1, col++, model.limitData[i].max, 0, model.getChannelsMax(), 100);

    // Channel inversion
    QComboBox * invCB = new QComboBox(this);
    invCB->insertItems(0, QStringList() << tr("---") << tr("INV"));
    invCB->setProperty("index", i);
    invCB->setCurrentIndex((model.limitData[i].revert) ? 1 : 0);
    connect(invCB, SIGNAL(currentIndexChanged(int)), this, SLOT(invEdited()));
    gridLayout->addWidget(invCB, i+1, col++, 1, 1);

    // Curve
    if (IS_TARANIS(GetEepromInterface()->getBoard())) {
      QComboBox * curveCB = new QComboBox(this);
      curveCB->setProperty("index", i);
      int numcurves = firmware->getCapability(NumCurves);
      for (int j=-numcurves; j<=numcurves; j++) {
        curveCB->addItem(CurveReference(CurveReference::CURVE_REF_CUSTOM, j).toString(), j);
      }
      curveCB->setCurrentIndex(model.limitData[i].curve.value+numcurves);
      connect(curveCB, SIGNAL(currentIndexChanged(int)), this, SLOT(curveEdited()));
      gridLayout->addWidget(curveCB, i+1, col++, 1, 1);
    }

    // PPM center
    int ppmCenterMax = firmware->getCapability(PPMCenter);
    if (ppmCenterMax) {
      QSpinBox * center = new QSpinBox(this);
      center->setProperty("index", i);
      center->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
      center->setSuffix("us");
      center->setMinimum(1500-ppmCenterMax);
      center->setMaximum(1500+ppmCenterMax);
      center->setValue(1500);
      center->setValue(model.limitData[i].ppmCenter + 1500);
      connect(center, SIGNAL(editingFinished()), this, SLOT(ppmcenterEdited()));
      gridLayout->addWidget(center, i+1, col++, 1, 1);
    }

    // Symetrical limits
    if (firmware->getCapability(SYMLimits)) {
      QCheckBox * symlimits = new QCheckBox(this);
      symlimits->setProperty("index", i);
      symlimits->setChecked(model.limitData[i].symetrical);
      connect(symlimits, SIGNAL(toggled(bool)), this, SLOT(symlimitsEdited()));
      gridLayout->addWidget(symlimits, i+1, col++, 1, 1);
    }
  }

  // Push the rows up
  addVSpring(gridLayout, 0,firmware->getCapability(Outputs)+1);

  disableMouseScrolling();
}

Channels::~Channels()
{
  foreach(LimitsGroup *group, limitsGroups) {
    delete group;
  }
}

void Channels::symlimitsEdited()
{
  QCheckBox * ckb = qobject_cast<QCheckBox*>(sender());
  int index = ckb->property("index").toInt();
  model.limitData[index].symetrical = (ckb->checkState() ? 1 : 0);
  emit modified();
}

void Channels::nameEdited()
{
  if (!lock) {
    lock = true;
    QLineEdit *le = qobject_cast<QLineEdit*>(sender());
    int index = le->property("index").toInt();
    strcpy(model.limitData[index].name, le->text().toAscii());
    lock = false;
    emit modified();
  }
}

void Channels::refreshExtendedLimits()
{
  int channelMax = model.getChannelsMax();

  foreach(LimitsGroup *group, limitsGroups) {
    group->updateMinMax(channelMax);
  }

  emit modified(); 
}

void Channels::invEdited()
{
  QComboBox *cb = qobject_cast<QComboBox*>(sender());
  int index = cb->property("index").toInt();
  model.limitData[index].revert = cb->currentIndex();
  emit modified();
}

void Channels::curveEdited()
{
  QComboBox *cb = qobject_cast<QComboBox*>(sender());
  int index = cb->property("index").toInt();
  model.limitData[index].curve = CurveReference(CurveReference::CURVE_REF_CUSTOM, cb->itemData(cb->currentIndex()).toInt());
  emit modified();
}

void Channels::ppmcenterEdited()
{
  QSpinBox *sb = qobject_cast<QSpinBox*>(sender());
  int index = sb->property("index").toInt();
  model.limitData[index].ppmCenter = sb->value() - 1500;
  emit modified();
}

