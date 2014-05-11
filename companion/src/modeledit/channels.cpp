#include "channels.h"
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>

Channels::Channels(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware):
  ModelPanel(parent, model, generalSettings, firmware)
{
  QGridLayout * gridLayout = new QGridLayout(this);
  bool minimize = false;

  int col = 1;
  if (firmware->getCapability(ChannelsName))
  {
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
    QDoubleSpinBox * offset = new QDoubleSpinBox(this);
    offset->setProperty("index", i);
    offset->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    offset->setAccelerated(true);
    offset->setDecimals(1);
    offset->setMinimum(-100);
    offset->setSingleStep(0.1);
    offset->setValue(float(model.limitData[i].offset) / 10);
    connect(offset, SIGNAL(editingFinished()), this, SLOT(offsetEdited()));
    gridLayout->addWidget(offset, i+1, col++, 1, 1);

    // Channel min
    QDoubleSpinBox * minSB = new QDoubleSpinBox(this);
    minSB->setProperty("index", i);
    minSB->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    minSB->setAccelerated(true);
    minSB->setDecimals(1);
    minSB->setMinimum(model.extendedLimits ? -125 : -100);
    minSB->setSingleStep(0.1);
    minSB->setMaximum(0);
    minSB->setValue(float(model.limitData[i].min) / 10);
    connect(minSB, SIGNAL(editingFinished()), this, SLOT(minEdited()));
    gridLayout->addWidget(minSB, i+1, col++, 1, 1);
    minSpins << minSB;

    // Channel max
    QDoubleSpinBox * maxSB = new QDoubleSpinBox(this);
    maxSB->setProperty("index", i);
    maxSB->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    maxSB->setAccelerated(true);
    maxSB->setDecimals(1);
    maxSB->setMinimum(0);
    maxSB->setSingleStep(0.1);
    maxSB->setMaximum(model.extendedLimits ? 125 : 100);
    maxSB->setValue(float(model.limitData[i].max) / 10);
    connect(maxSB, SIGNAL(editingFinished()), this, SLOT(maxEdited()));
    gridLayout->addWidget(maxSB, i+1, col++, 1, 1);
    maxSpins << maxSB;

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
    if (firmware->getCapability(PPMCenter)) {
      QSpinBox * center = new QSpinBox(this);
      center->setProperty("index", i);
      center->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
      center->setMinimum(1375);
      center->setMaximum(1625);
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
}

Channels::~Channels()
{
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

void Channels::offsetEdited()
{
  QDoubleSpinBox *dsb = qobject_cast<QDoubleSpinBox*>(sender());
  int index = dsb->property("index").toInt();
  model.limitData[index].offset = round(dsb->value() * 10);
  emit modified();
}

void Channels::minEdited()
{
  QDoubleSpinBox *sb = qobject_cast<QDoubleSpinBox*>(sender());
  int index = sb->property("index").toInt();
  model.limitData[index].min = round(sb->value() * 10);
  emit modified();
}

void Channels::maxEdited()
{
  QDoubleSpinBox *sb = qobject_cast<QDoubleSpinBox*>(sender());
  int index = sb->property("index").toInt();
  model.limitData[index].max = round(sb->value() * 10);
  emit modified();
}

void Channels::refreshExtendedLimits()
{
  for (int i=0; i<firmware->getCapability(Outputs); i++) {
    QDoubleSpinBox * minDSB = minSpins[i];
    QDoubleSpinBox * maxDSB = maxSpins[i];
    
    minDSB->setMinimum(model.extendedLimits ? -125 : -100);
    maxDSB->setMaximum(model.extendedLimits ? 125 : 100);
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

