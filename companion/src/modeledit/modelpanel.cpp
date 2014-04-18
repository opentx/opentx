#include "modelpanel.h"
#include <QLabel>

ModelPanel::ModelPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware):
  QWidget(parent),
  model(model),
  generalSettings(generalSettings),
  firmware(firmware),
  lock(false)
{
}

ModelPanel::~ModelPanel()
{
}

void ModelPanel::update()
{
}

void ModelPanel::addLabel(QGridLayout * gridLayout, QString text, int col, bool minimize)
{
  QLabel *label = new QLabel(this);
  label->setFrameShape(QFrame::Panel);
  label->setFrameShadow(QFrame::Raised);
  label->setMidLineWidth(0);
  label->setAlignment(Qt::AlignCenter);
  label->setMargin(5);
  label->setText(text);
  if (!minimize)
    label->setMinimumWidth(100);
  gridLayout->addWidget(label, 0, col, 1, 1);
}

void ModelPanel::addEmptyLabel(QGridLayout * gridLayout, int col)
{
  QLabel *label = new QLabel(this);
  label->setText("");
  gridLayout->addWidget(label, 0, col, 1, 1);
}

void ModelPanel::addHSpring(QGridLayout * gridLayout, int col, int row)
{
    QSpacerItem * spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    gridLayout->addItem(spacer, row, col);
}

void ModelPanel::addVSpring(QGridLayout * gridLayout, int col, int row)
{
    QSpacerItem * spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
    gridLayout->addItem(spacer, row, col);
}

void ModelPanel::addDoubleSpring(QGridLayout * gridLayout, int col, int row)
{
    QSpacerItem * spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );
    gridLayout->addItem(spacer, row, col);
}

