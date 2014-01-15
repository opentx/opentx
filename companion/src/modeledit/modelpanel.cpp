#include "modelpanel.h"
#include <QLabel>

ModelPanel::ModelPanel(QWidget * parent, ModelData & model):
  QWidget(parent),
  model(model),
  lock(false)
{
}

ModelPanel::~ModelPanel()
{
}

void ModelPanel::update()
{
}

void ModelPanel::addLabel(QGridLayout * gridLayout, QString text, int col)
{
  QLabel *label = new QLabel(this);
  label->setFrameShape(QFrame::Panel);
  label->setFrameShadow(QFrame::Raised);
  label->setMidLineWidth(0);
  label->setAlignment(Qt::AlignCenter);
  label->setMargin(3);
  label->setText(text);
  gridLayout->addWidget(label, 0, col, 1, 1);
}

