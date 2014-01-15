#include "heli.h"
#include "ui_heli.h"
#include "helpers.h"

HeliPanel::HeliPanel(QWidget *parent, ModelData & model):
  ModelPanel(parent, model),
  ui(new Ui::Heli)
{
  ui->setupUi(this);

  connect(ui->swashTypeCB, SIGNAL(currentIndexChanged(int)), this, SLOT(edited()));
  connect(ui->swashCollectiveCB, SIGNAL(currentIndexChanged(int)), this, SLOT(edited()));
  connect(ui->swashRingValSB, SIGNAL(editingFinished()), this, SLOT(edited()));
  connect(ui->swashInvertELE, SIGNAL(stateChanged(int)), this, SLOT(edited()));
  connect(ui->swashInvertAIL, SIGNAL(stateChanged(int)), this, SLOT(edited()));
  connect(ui->swashInvertCOL, SIGNAL(stateChanged(int)), this, SLOT(edited()));
}

HeliPanel::~HeliPanel()
{
  delete ui;
}

void HeliPanel::update()
{
  lock = true;

  ui->swashTypeCB->setCurrentIndex(model.swashRingData.type);
  populateSourceCB(ui->swashCollectiveCB, model.swashRingData.collectiveSource, POPULATE_SOURCES | POPULATE_SWITCHES | POPULATE_TRIMS);
  ui->swashRingValSB->setValue(model.swashRingData.value);
  ui->swashInvertELE->setChecked(model.swashRingData.invertELE);
  ui->swashInvertAIL->setChecked(model.swashRingData.invertAIL);
  ui->swashInvertCOL->setChecked(model.swashRingData.invertCOL);

  lock = false;
}

void HeliPanel::edited()
{
  if (!lock) {
    model.swashRingData.type  = ui->swashTypeCB->currentIndex();
    model.swashRingData.collectiveSource = ui->swashCollectiveCB->itemData(ui->swashCollectiveCB->currentIndex()).toInt();
    model.swashRingData.value = ui->swashRingValSB->value();
    model.swashRingData.invertELE = ui->swashInvertELE->isChecked();
    model.swashRingData.invertAIL = ui->swashInvertAIL->isChecked();
    model.swashRingData.invertCOL = ui->swashInvertCOL->isChecked();
    emit modified();
  }
}
