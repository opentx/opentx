#include "logicalswitches.h"
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include "helpers.h"

LogicalSwitchesPanel::LogicalSwitchesPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  selectedSwitch(0)
{
  QGridLayout * gridLayout = new QGridLayout(this);

  int col = 1;
  addLabel(gridLayout, tr("Function"), col++);
  addLabel(gridLayout, tr("V1"), col++);
  addLabel(gridLayout, tr("V2"), col++);
  addLabel(gridLayout, tr("AND Switch"), col++);
  if (firmware->getCapability(LogicalSwitchesExt)) {
    addLabel(gridLayout, tr("Duration"), col++);
    addLabel(gridLayout, tr("Delay"), col++);
  }

  lock = true;
  for (int i=0; i<firmware->getCapability(LogicalSwitches); i++) {
    // The label
    QLabel * label = new QLabel(this);
    label->setProperty("index", i);
    label->setText(tr("L%1").arg(i+1));
    label->setContextMenuPolicy(Qt::CustomContextMenu);
    label->setMouseTracking(true);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    connect(label, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(csw_customContextMenuRequested(QPoint)));
    gridLayout->addWidget(label, i+1, 0);

    // The function
    csw[i] = new QComboBox(this);
    csw[i]->setProperty("index", i);
    connect(csw[i], SIGNAL(currentIndexChanged(int)), this, SLOT(edited()));
    gridLayout->addWidget(csw[i], i+1, 1);

    // V1
    cswitchSource1[i] = new QComboBox(this);
    cswitchSource1[i]->setProperty("index",i);
    connect(cswitchSource1[i], SIGNAL(currentIndexChanged(int)), this, SLOT(v1Edited(int)));
    gridLayout->addWidget(cswitchSource1[i], i+1, 2);
    cswitchSource1[i]->setVisible(false);
    cswitchValue[i] = new QDoubleSpinBox(this);
    cswitchValue[i]->setMaximum(125);
    cswitchValue[i]->setMinimum(-125);
    cswitchValue[i]->setAccelerated(true);
    cswitchValue[i]->setDecimals(0);
    cswitchValue[i]->setProperty("index", i);
    connect(cswitchValue[i], SIGNAL(valueChanged(double)), this, SLOT(edited()));
    gridLayout->addWidget(cswitchValue[i], i+1, 2);
    cswitchValue[i]->setVisible(false);

    // V2
    QHBoxLayout *v2Layout = new QHBoxLayout();
    cswitchSource2[i] = new QComboBox(this);
    cswitchSource2[i]->setProperty("index", i);
    connect(cswitchSource2[i], SIGNAL(currentIndexChanged(int)), this, SLOT(v2Edited(int)));
    v2Layout->addWidget(cswitchSource2[i]);
    cswitchSource2[i]->setVisible(false);
    cswitchOffset[i] = new QDoubleSpinBox(this);
    cswitchOffset[i]->setProperty("index",i);
    cswitchOffset[i]->setMaximum(125);
    cswitchOffset[i]->setMinimum(-125);
    cswitchOffset[i]->setAccelerated(true);
    cswitchOffset[i]->setDecimals(0);
    connect(cswitchOffset[i], SIGNAL(valueChanged(double)), this, SLOT(edited()));
    cswitchOffset[i]->setVisible(false);
    v2Layout->addWidget(cswitchOffset[i]);
    cswitchOffset2[i] = new QDoubleSpinBox(this);
    cswitchOffset2[i]->setProperty("index",i);
    cswitchOffset2[i]->setMaximum(125);
    cswitchOffset2[i]->setMinimum(-125);
    cswitchOffset2[i]->setAccelerated(true);
    cswitchOffset2[i]->setDecimals(0);
    connect(cswitchOffset2[i], SIGNAL(valueChanged(double)), this, SLOT(edited()));
    cswitchOffset2[i]->setVisible(false);
    v2Layout->addWidget(cswitchOffset2[i]);
    cswitchTOffset[i] = new QTimeEdit(this);
    cswitchTOffset[i]->setProperty("index",i);
    cswitchTOffset[i]->setAccelerated(true);
    connect(cswitchTOffset[i],SIGNAL(editingFinished()),this,SLOT(edited()));
    v2Layout->addWidget(cswitchTOffset[i]);
    cswitchTOffset[i]->setVisible(false);
    gridLayout->addLayout(v2Layout, i+1, 3);

    // AND
    cswitchAnd[i] = new QComboBox(this);
    cswitchAnd[i]->setProperty("index", i);
    connect(cswitchAnd[i], SIGNAL(currentIndexChanged(int)), this, SLOT(andEdited(int)));
    gridLayout->addWidget(cswitchAnd[i], i+1, 4);

    if (firmware->getCapability(LogicalSwitchesExt)) {
      // Duration
      cswitchDuration[i] = new QDoubleSpinBox(this);
      cswitchDuration[i]->setProperty("index", i);
      cswitchDuration[i]->setSingleStep(0.1);
      cswitchDuration[i]->setMaximum(25);
      cswitchDuration[i]->setMinimum(0);
      cswitchDuration[i]->setAccelerated(true);
      cswitchDuration[i]->setDecimals(1);
      connect(cswitchDuration[i], SIGNAL(valueChanged(double)), this, SLOT(durationEdited(double)));
      gridLayout->addWidget(cswitchDuration[i], i+1, 5);

      // Delay
      cswitchDelay[i] = new QDoubleSpinBox(this);
      cswitchDelay[i]->setProperty("index", i);
      cswitchDelay[i]->setSingleStep(0.1);
      cswitchDelay[i]->setMaximum(25);
      cswitchDelay[i]->setMinimum(0);
      cswitchDelay[i]->setAccelerated(true);
      cswitchDelay[i]->setDecimals(1);
      connect(cswitchDelay[i], SIGNAL(valueChanged(double)), this, SLOT(delayEdited(double)));
      gridLayout->addWidget(cswitchDelay[i], i+1, 6);
    }
  }
  // Push rows upward
  addVSpring(gridLayout,0,firmware->getCapability(LogicalSwitches)+1);

  disableMouseScrolling();

  lock = false;
}

LogicalSwitchesPanel::~LogicalSwitchesPanel()
{
}

void LogicalSwitchesPanel::v1Edited(int value)
{
  if (!lock) {
    int i = sender()->property("index").toInt();
    model.customSw[i].val1 = cswitchSource1[i]->itemData(value).toInt();
    if (model.customSw[i].getFunctionFamily() == LS_FAMILY_VOFS) {
      RawSource source = RawSource(model.customSw[i].val1);
      RawSourceRange range = source.getRange(model, generalSettings);
      if (model.customSw[i].isDeltaFunction()) {
        model.customSw[i].val2 = (cswitchOffset[i]->value() / range.step);
      }
      else {
        model.customSw[i].val2 = (cswitchOffset[i]->value() - range.offset) / range.step;
      }
      setSwitchWidgetVisibility(i);
    }
    emit modified();
  }
}

void LogicalSwitchesPanel::v2Edited(int value)
{
  if (!lock) {
    int i = sender()->property("index").toInt();
    model.customSw[i].val2 = cswitchSource2[i]->itemData(value).toInt();
    emit modified();
  }
}

void LogicalSwitchesPanel::andEdited(int value)
{
  if (!lock) {
    int index = sender()->property("index").toInt();
    model.customSw[index].andsw = cswitchAnd[index]->itemData(value).toInt();
    emit modified();
  }
}

void LogicalSwitchesPanel::durationEdited(double duration)
{
  int index = sender()->property("index").toInt();
  model.customSw[index].duration = (uint8_t)round(duration*10);
  emit modified();
}

void LogicalSwitchesPanel::delayEdited(double delay)
{
  int index = sender()->property("index").toInt();
  model.customSw[index].delay = (uint8_t)round(delay*10);
  emit modified();
}

void LogicalSwitchesPanel::edited()
{
  if (!lock) {
    lock = true;
    int i = sender()->property("index").toInt();
    int newFunc = csw[i]->itemData(csw[i]->currentIndex()).toInt();
    bool chAr = (model.customSw[i].getFunctionFamily() != LogicalSwitchData(newFunc).getFunctionFamily());
    bool chDelta = (model.customSw[i].isDeltaFunction() ^ LogicalSwitchData(newFunc).isDeltaFunction());
    model.customSw[i].func = newFunc;
    if (chAr) {
      if (model.customSw[i].getFunctionFamily() == LS_FAMILY_TIMER) {
        model.customSw[i].val1 = -119;
        model.customSw[i].val2 = -119;
      }
      else if (model.customSw[i].getFunctionFamily() == LS_FAMILY_STAY) {
        model.customSw[i].val1 = 0;
        model.customSw[i].val2 = -129;
        model.customSw[i].val3 = 0;
      }
      else {
        model.customSw[i].val1 = 0;
        model.customSw[i].val2 = 0;
      }
      model.customSw[i].andsw = 0;
      setSwitchWidgetVisibility(i);
    }
    if (chDelta) {
      model.customSw[i].val2 = 0;
      setSwitchWidgetVisibility(i);
    }

    RawSource source;
    switch (model.customSw[i].getFunctionFamily())
    {
      case LS_FAMILY_VOFS:
      {
        source = RawSource(model.customSw[i].val1);
        RawSourceRange range = source.getRange(model, generalSettings);
        double value = source.isTimeBased() ? QTimeS(cswitchTOffset[i]->time()).seconds() : cswitchOffset[i]->value();
        if (model.customSw[i].isDeltaFunction()) {
          /*TODO: is this delta function value set correctly*/
          model.customSw[i].val2 = round(value/range.step);
          value=model.customSw[i].val2*range.step;
        }
        else {
          model.customSw[i].val2 = round((value-range.offset)/range.step);;
          value = model.customSw[i].val2*range.step + range.offset;
        }
        if (source.isTimeBased())
          cswitchTOffset[i]->setTime(QTimeS(round(value)));
        else
          cswitchOffset[i]->setValue(value);
        break;
      }
      case LS_FAMILY_TIMER:
        model.customSw[i].val1 = TimToVal(cswitchValue[i]->value());
        model.customSw[i].val2 = TimToVal(cswitchOffset[i]->value());
        updateTimerParam(cswitchValue[i], model.customSw[i].val1, 0.1);
        updateTimerParam(cswitchOffset[i], model.customSw[i].val2, 0.1);
        break;
      case LS_FAMILY_STAY:
        if (sender() == cswitchOffset[i]) {
          model.customSw[i].val2 = TimToVal(cswitchOffset[i]->value());
          updateTimerParam(cswitchOffset[i], model.customSw[i].val2, 0.0);
        }
        else {
          model.customSw[i].val3 = TimToVal(cswitchOffset2[i]->value()) - model.customSw[i].val2;
        }
        updateTimerParam(cswitchOffset2[i], model.customSw[i].val2+model.customSw[i].val3, cswitchOffset[i]->value());
        if (model.customSw[i].val3 == 0) {
          cswitchOffset2[i]->setSuffix(tr("(infinite)"));
          cswitchOffset2[i]->clear();
        }
        break;
      default:
        break;
    }
    emit modified();
    lock = false;
  }
}

void LogicalSwitchesPanel::updateTimerParam(QDoubleSpinBox *sb, int timer, double minimum)
{
  sb->setVisible(true);
  sb->setDecimals(1);
  sb->setMinimum(minimum);
  sb->setMaximum(175);
  float value = ValToTim(timer);
  if (value>=60)
    sb->setSingleStep(1);
  else if (value>=2)
    sb->setSingleStep(0.5);
  else
    sb->setSingleStep(0.1);
  sb->setValue(value);
  sb->setSuffix("");
}

#define SOURCE1_VISIBLE  0x1
#define SOURCE2_VISIBLE  0x2
#define VALUE1_VISIBLE   0x4
#define VALUE2_VISIBLE   0x8
#define VALUE3_VISIBLE   0x10
#define VALUE_TO_VISIBLE 0x20

void LogicalSwitchesPanel::setSwitchWidgetVisibility(int i)
{
  lock = true;

  unsigned int mask = 0;
  RawSource source = RawSource(model.customSw[i].val1);
  RawSourceRange range = source.getRange(model, generalSettings);

  switch (model.customSw[i].getFunctionFamily())
  {
    case LS_FAMILY_VOFS:
      mask |= SOURCE1_VISIBLE;
      populateSourceCB(cswitchSource1[i], source, model, POPULATE_SOURCES | POPULATE_VIRTUAL_INPUTS | POPULATE_TRIMS | POPULATE_SWITCHES | POPULATE_TELEMETRY | (firmware->getCapability(GvarsInCS) ? POPULATE_GVARS : 0));
      cswitchOffset[i]->setDecimals(range.decimals);
      cswitchOffset[i]->setSingleStep(range.step);
      if (source.isTimeBased()) {
        mask |= VALUE_TO_VISIBLE;
        int maxTime;
        int value;
        if (model.customSw[i].isDeltaFunction()) {
          /*TODO: is this delta function value set correctly*/
          maxTime = round(range.step*127);
          value = round(range.step*model.customSw[i].val2);
        } 
        else {
          maxTime = round(range.max);
          value = round(range.step*(model.customSw[i].val2/* TODO+source.getRawOffset(model)*/)+range.offset);
        }
        cswitchTOffset[i]->setMaximumTime(QTimeS(maxTime));
        cswitchTOffset[i]->setDisplayFormat((maxTime>=3600)?"hh:mm:ss":"mm:ss");
        cswitchTOffset[i]->setTime(QTimeS(value));
      }
      else {
        mask |= VALUE2_VISIBLE;
        if (range.unit.isEmpty())
          cswitchOffset[i]->setSuffix("");
        else
          cswitchOffset[i]->setSuffix(" " + range.unit);
        if (model.customSw[i].isDeltaFunction()) {
          /*TODO: is this delta function value set correctly*/
          cswitchOffset[i]->setMinimum(range.step*-127);
          cswitchOffset[i]->setMaximum(range.step*127);
          cswitchOffset[i]->setValue(range.step*model.customSw[i].val2);
        }
        else {
          cswitchOffset[i]->setMinimum(range.min);
          cswitchOffset[i]->setMaximum(range.max);
          cswitchOffset[i]->setValue(range.step*(model.customSw[i].val2/* TODO+source.getRawOffset(model)*/)+range.offset);
        }
      }
      break;
    case LS_FAMILY_VBOOL:
    case LS_FAMILY_STICKY:
      mask |= SOURCE1_VISIBLE | SOURCE2_VISIBLE;
      populateSwitchCB(cswitchSource1[i], RawSwitch(model.customSw[i].val1), generalSettings);
      populateSwitchCB(cswitchSource2[i], RawSwitch(model.customSw[i].val2), generalSettings);
      break;
    case LS_FAMILY_STAY:
      mask |= SOURCE1_VISIBLE | VALUE2_VISIBLE | VALUE3_VISIBLE;
      populateSwitchCB(cswitchSource1[i], RawSwitch(model.customSw[i].val1), generalSettings);
      updateTimerParam(cswitchOffset[i], model.customSw[i].val2, 0.0);
      updateTimerParam(cswitchOffset2[i], model.customSw[i].val2+model.customSw[i].val3, cswitchOffset[i]->value());
      if (model.customSw[i].val3 == 0) {
        cswitchOffset2[i]->setSuffix(tr("(infinite)"));
        cswitchOffset2[i]->clear();
      }
      break;
    case LS_FAMILY_VCOMP:
      mask |= SOURCE1_VISIBLE | SOURCE2_VISIBLE;
      populateSourceCB(cswitchSource1[i], RawSource(model.customSw[i].val1), model, POPULATE_SOURCES | POPULATE_VIRTUAL_INPUTS | POPULATE_TRIMS | POPULATE_SWITCHES | POPULATE_TELEMETRY | (firmware->getCapability(GvarsInCS) ? POPULATE_GVARS : 0));
      populateSourceCB(cswitchSource2[i], RawSource(model.customSw[i].val2), model, POPULATE_SOURCES | POPULATE_TRIMS | POPULATE_VIRTUAL_INPUTS | POPULATE_SWITCHES | POPULATE_TELEMETRY | (firmware->getCapability(GvarsInCS) ? POPULATE_GVARS : 0));
      break;
    case LS_FAMILY_TIMER:
      mask |= VALUE1_VISIBLE | VALUE2_VISIBLE;
      updateTimerParam(cswitchValue[i], model.customSw[i].val1, 0.1);
      updateTimerParam(cswitchOffset[i], model.customSw[i].val2, 0.1);
      break;
  }

  cswitchSource1[i]->setVisible(mask & SOURCE1_VISIBLE);
  cswitchSource2[i]->setVisible(mask & SOURCE2_VISIBLE);
  cswitchValue[i]->setVisible(mask & VALUE1_VISIBLE);
  cswitchOffset[i]->setVisible(mask & VALUE2_VISIBLE);
  cswitchOffset2[i]->setVisible(mask & VALUE3_VISIBLE);
  cswitchTOffset[i]->setVisible(mask & VALUE_TO_VISIBLE);
  lock = false;
}

void LogicalSwitchesPanel::populateCSWCB(QComboBox *b, int value)
{
  int order[] = {
    LS_FN_OFF,
    LS_FN_VEQUAL, // added at the end to avoid everything renumbered
    LS_FN_VALMOSTEQUAL, // added at the end to avoid everything renumbered
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
    if (func == LS_FN_NEQUAL || func == LS_FN_EGREATER || func == LS_FN_ELESS)
      continue;
    if (!IS_ARM(firmware->getBoard())) {
      if (func == LS_FN_VEQUAL || func == LS_FN_STAY)
        continue;
    }
    b->addItem(LogicalSwitchData(func).funcToString(), func);
    if (value == func) {
      b->setCurrentIndex(b->count()-1);
    }
  }
  b->setMaxVisibleItems(10);
}

void LogicalSwitchesPanel::updateLine(int i)
{
  lock = true;
  populateCSWCB(csw[i], model.customSw[i].func);
  lock = true;
  setSwitchWidgetVisibility(i);
  lock = true;
  populateAndSwitchCB(cswitchAnd[i], RawSwitch(model.customSw[i].andsw));
  if (firmware->getCapability(LogicalSwitchesExt)) {
    cswitchDuration[i]->setValue(model.customSw[i].duration/10.0);
    cswitchDelay[i]->setValue(model.customSw[i].delay/10.0);
  }
  lock = false;
}

void LogicalSwitchesPanel::update()
{
  for (int i=0; i<firmware->getCapability(LogicalSwitches); i++) {
    updateLine(i);
  }
}

void LogicalSwitchesPanel::cswPaste()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if (mimeData->hasFormat("application/x-companion-csw")) {
      QByteArray cswData = mimeData->data("application/x-companion-csw");
      LogicalSwitchData *csw = &model.customSw[selectedSwitch];
      memcpy(csw, cswData.mid(0, sizeof(LogicalSwitchData)).constData(), sizeof(LogicalSwitchData));
      emit modified();
      updateLine(selectedSwitch);
    }
}

void LogicalSwitchesPanel::cswDelete()
{
  model.customSw[selectedSwitch].clear();
  emit modified();
  updateLine(selectedSwitch);
}

void LogicalSwitchesPanel::cswCopy()
{
  QByteArray cswData;
  cswData.append((char*)&model.customSw[selectedSwitch],sizeof(LogicalSwitchData));
  QMimeData *mimeData = new QMimeData;
  mimeData->setData("application/x-companion-csw", cswData);
  QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void LogicalSwitchesPanel::cswCut()
{
  cswCopy();
  cswDelete();
}

// TODO make something generic here!
void LogicalSwitchesPanel::csw_customContextMenuRequested(QPoint pos)
{
    QLabel *label = (QLabel *)sender();
    selectedSwitch = label->property("index").toInt();

    QPoint globalPos = label->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion-csw");

    QMenu contextMenu;
    contextMenu.addAction(CompanionIcon("copy.png"), tr("&Copy"),this,SLOT(cswCopy()),tr("Ctrl+C"));
    contextMenu.addAction(CompanionIcon("cut.png"), tr("&Cut"),this,SLOT(cswCut()),tr("Ctrl+X"));
    contextMenu.addAction(CompanionIcon("paste.png"), tr("&Paste"),this,SLOT(cswPaste()),tr("Ctrl+V"))->setEnabled(hasData);
    contextMenu.addAction(CompanionIcon("clear.png"), tr("&Delete"),this,SLOT(cswDelete()),tr("Delete"));

    contextMenu.exec(globalPos);
}
