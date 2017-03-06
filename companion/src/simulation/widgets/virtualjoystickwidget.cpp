/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define GBALL_SIZE  20
#define RESX        1024

#include "virtualjoystickwidget.h"

#include "boards.h"
#include "constants.h"
#include "modeledit/node.h"
#include "helpers.h"
#include "radiotrimwidget.h"

VirtualJoystickWidget::VirtualJoystickWidget(QWidget *parent, QChar side, bool showTrims, bool showBtns, bool showValues, QSize size) :
  QWidget(parent),
  stickSide(side),
  prefSize(size),
  hTrimWidget(NULL),
  vTrimWidget(NULL),
  btnHoldX(NULL),
  btnHoldY(NULL),
  btnFixX(NULL),
  btnFixY(NULL),
  nodeLabelX(NULL),
  nodeLabelY(NULL)
{
  ar = (float)size.width() / size.height();
  extraSize = QSize(0, 0);

  // 5 col x 4 rows
  layout = new QGridLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setSizePolicy(sizePolicy);

  gv = new QGraphicsView(this);
  gv->setSizePolicy(sizePolicy);
  gv->setMinimumSize(size);
  gv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  gv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  scene = new QGraphicsScene(gv);
  scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  gv->setScene(scene);

  node = new Node();
  node->setPos(-GBALL_SIZE / 2, -GBALL_SIZE / 2);
  node->setBallSize(GBALL_SIZE);
  scene->addItem(node);

  int colvt, colbb, colvx, colvy;
  if (stickSide == 'L') {
    colvt = 3;
    colbb = 1;
    colvx = 1;
    colvy = 3;
  }
  else {
    colvt = 1;
    colbb = 3;
    colvx = 3;
    colvy = 1;
  }

  if (showTrims) {
    hTrimWidget = createTrimWidget('H');
    vTrimWidget = createTrimWidget('V');

    layout->addWidget(vTrimWidget, 1, colvt, 1, 1);
    layout->addWidget(hTrimWidget, 2, 2, 1, 1);

    extraSize += QSize(vTrimWidget->sizeHint().width(), hTrimWidget->sizeHint().height());
  }
  else {
    colvx = colvy = colvt;
  }

  if (showBtns) {
    QVBoxLayout * btnbox = new QVBoxLayout();
    btnbox->setContentsMargins(9, 9, 9, 9);
    btnbox->setSpacing(2);
    btnbox->addWidget(btnHoldY = createButtonWidget(HOLD_Y));
    btnbox->addWidget(btnFixY = createButtonWidget(FIX_Y));
    btnbox->addWidget(btnFixX = createButtonWidget(FIX_X));
    btnbox->addWidget(btnHoldX = createButtonWidget(HOLD_X));

    layout->addLayout(btnbox, 1, colbb, 1, 1);

    extraSize.setWidth(extraSize.width() + btnbox->sizeHint().width());
  }

  if (showValues) {
    QLayout * valX = createNodeValueLayout('X', nodeLabelX);
    QLayout * valY = createNodeValueLayout('Y', nodeLabelY);
    if (!showTrims) {
      QVBoxLayout * vertXY = new QVBoxLayout();
      vertXY->addLayout(valX);
      vertXY->addLayout(valY);
      layout->addLayout(vertXY, 1, colvx, 1, 1);
      extraSize += QSize(nodeLabelX->sizeHint().width(), 0);
    }
    else {
      layout->addLayout(valX, 2, colvx, 1, 1);
      layout->addLayout(valY, 2, colvy, 1, 1);
    }
  }

  layout->addItem(new QSpacerItem(0, 0), 0, 0, 1, 5);  // r0 c0-4: top v spacer
  layout->addItem(new QSpacerItem(0, 0), 1, 0, 2, 1);  // r1-2 c0: left h spacer
  layout->addWidget(gv, 1, 2, 1, 1);                   // r1 c2: stick widget
  layout->addItem(new QSpacerItem(0, 0), 1, 4, 2, 1);  // r1-2 c4: right h spacer
  layout->addItem(new QSpacerItem(0, 0), 3, 0, 1, 5);  // r4 c0-4: bot v spacer

  connect(node, SIGNAL(xChanged()), this, SLOT(updateNodeValueLabels()));
  connect(node, SIGNAL(yChanged()), this, SLOT(updateNodeValueLabels()));

  setSize(prefSize, frameSize());
}

void VirtualJoystickWidget::setStickX(qreal x)
{
  node->setX(x);
}

void VirtualJoystickWidget::setStickY(qreal y)
{
  node->setY(y);
}

void VirtualJoystickWidget::setStickPos(QPointF xy)
{
  node->setPos(xy);
}

void VirtualJoystickWidget::centerStick()
{
  node->stepToCenter();
}

qreal VirtualJoystickWidget::getStickX()
{
  return getStickPos().x();
}

qreal VirtualJoystickWidget::getStickY()
{
  return getStickPos().y();
}

QPointF VirtualJoystickWidget::getStickPos()
{
  return QPointF(node->getX(), node->getY());
}

void VirtualJoystickWidget::setTrimValue(int which, int value)
{
  RadioTrimWidget * trim = getTrimWidget(which);
  if (trim) {
    trim->setValue(value);
  }
}

void VirtualJoystickWidget::setTrimRange(int which, int min, int max)
{
  RadioTrimWidget * trim = getTrimWidget(which);
  if (trim) {
    trim->setTrimRange(min, max);
  }
}

int VirtualJoystickWidget::getTrimValue(int which)
{
  RadioTrimWidget * trim = getTrimWidget(which);
  if (trim) {
    return trim->getValue();
  }
  return 0;
}

void VirtualJoystickWidget::setStickConstraint(int which, bool active)
{
  if (btnHoldX == NULL)
    return;  // no buttons

  switch (which) {
    case HOLD_X:
      btnHoldX->setChecked(active);
      break;
    case HOLD_Y:
      btnHoldY->setChecked(active);
      break;
    case FIX_X:
      btnFixX->setChecked(active);
      break;
    case FIX_Y:
      btnFixY->setChecked(active);
      break;
    default:
      break;
  }
}

void VirtualJoystickWidget::setStickColor(const QColor & color)
{
  node->setColor(color);
}

QSize VirtualJoystickWidget::sizeHint() const {
  return prefSize;
}

void VirtualJoystickWidget::resizeEvent(QResizeEvent * event)
{
  QWidget::resizeEvent(event);
  setSize(event->size(), event->oldSize());
}

void VirtualJoystickWidget::setSize(const QSize & size, const QSize &)
{
  float thisAspectRatio = (float)size.width() / size.height();
  float newGvSz, spacerSz;
  bool sized = false;

  if (thisAspectRatio > ar) {
    // constrain width
    newGvSz = (height() - extraSize.height()) * ar; // new width
    spacerSz = (width() - newGvSz - extraSize.width()) / 2; // + 0.5;
    if (spacerSz >= 0.0f) {
      layout->setRowStretch(0, 0);
      layout->setColumnStretch(0, spacerSz);
      layout->setColumnStretch(4, spacerSz);
      layout->setRowStretch(3, 0);
      sized = true;
    }
  }
  if (!sized) {
    // constrain height
    newGvSz = (width() - extraSize.width()) * ar; // new height
    spacerSz = (height() - newGvSz - extraSize.height()) / 2; // + 0.5;
    spacerSz = qMax(spacerSz, 0.0f);
    layout->setRowStretch(0, spacerSz);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(4, 0);
    layout->setRowStretch(3, spacerSz);
  }

  layout->setColumnStretch(2, newGvSz);
  layout->setRowStretch(1, newGvSz);

  //prefSize = QSize(newGvSz + extraSize.width(), newGvSz + extraSize.height());
  gv->resize(newGvSz, newGvSz);
  gv->updateGeometry();

  QRectF qr = (QRectF)gv->contentsRect();
  qreal w  = qr.width()  - GBALL_SIZE;
  qreal h  = qr.height() - GBALL_SIZE;
  qreal cx = qr.width() / 2;
  qreal cy = qr.height() / 2;
  qreal nodeX = node->getX();
  qreal nodeY = node->getY();

  scene->setSceneRect(-cx,-cy,w,h);

  node->setX(nodeX);
  node->setY(nodeY);

  //qDebug() << thisAspectRatio << size << newGvSz << spacerSz << extraSize << gv->geometry() << gv->contentsRect() << gv->frameRect() << getStickPos();
}

RadioTrimWidget * VirtualJoystickWidget::createTrimWidget(QChar type)
{

  RadioTrimWidget * trimWidget = new RadioTrimWidget(type == 'H' ? Qt::Horizontal : Qt::Vertical);
  trimWidget->setIndices(getTrimSliderType(type), getTrimButtonType(type, 0), getTrimButtonType(type, 1));

  connect(trimWidget, &RadioTrimWidget::trimButtonPressed, this, &VirtualJoystickWidget::trimButtonPressed);
  connect(trimWidget, &RadioTrimWidget::trimButtonReleased, this, &VirtualJoystickWidget::trimButtonReleased);
  connect(trimWidget, &RadioTrimWidget::trimSliderMoved, this, &VirtualJoystickWidget::trimSliderMoved);

  return trimWidget;
}

QPushButton * VirtualJoystickWidget::createButtonWidget(int type)
{
  QString btnRole, btnLabel;
  switch (type) {
    case HOLD_Y:
      btnLabel = tr("Hold Y");
      break;
    case FIX_Y:
      btnLabel = tr("Fix Y");
      break;
    case FIX_X:
      btnLabel = tr("Fix X");
      break;
    case HOLD_X:
    default:
      btnLabel = tr("Hold X");
      break;
  }
  QPushButton * btn = new QPushButton(this);
  btn->setObjectName(QString("%1_%2").arg(btnLabel.replace(" ", "_")).arg(stickSide));
  btn->setProperty("btnType", type);
  btn->setText(btnLabel);
  QFont font;
  font.setPointSize(8);
  btn->setFont(font);
  btn->setStyleSheet(QLatin1String( \
                       "QPushButton {"
                       "     background-color: #EEEEEE;"
                       "     border-style: outset;"
                       "     border-width: 1px;"
                       "     border-radius: 4px;"
                       "     border-color: black;"
                       "     padding: 2px;"
                       "}"
                       "QPushButton:checked {\n"
                       "     background-color: #4CC417;\n"
                       "     border-style: inset;\n"
                       "}" ));
  btn->setCheckable(true);

  connect(btn, SIGNAL(toggled(bool)), SLOT(onButtonChange(bool)));

  return btn;
}

QLayout *VirtualJoystickWidget::createNodeValueLayout(QChar type, QLabel *& valLabel)
{
  QLabel * lbl = new QLabel(QString("%1 %").arg(type), this);
  lbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  lbl->setAlignment(Qt::AlignCenter);
  QLabel * val = new QLabel("0");
  val->setObjectName(QString("val_%1").arg(type));
  val->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  val->setAlignment(Qt::AlignCenter);
  val->setMinimumWidth(val->fontMetrics().width("-100 "));
  QVBoxLayout * layout = new QVBoxLayout();
  layout->setContentsMargins(2, 2, 2, 2);
  layout->setSpacing(2);
  layout->addWidget(lbl);
  layout->addWidget(val);

  valLabel = val;
  return layout;
}

int VirtualJoystickWidget::getTrimSliderType(QChar type)
{
  using namespace Board;

  if (stickSide == 'L') {
    if (type == 'H')
      return TRIM_AXIS_LH;
    else
      return TRIM_AXIS_LV;
  }
  else {
    if (type == 'H')
      return TRIM_AXIS_RH;
    else
      return TRIM_AXIS_RV;
  }
}

int VirtualJoystickWidget::getTrimButtonType(QChar type, int pos)
{
  using namespace Board;

  if (stickSide == 'L') {
    if (type == 'H') {
      if (pos == 0)
        return TRIM_SW_LH_DEC;
      else
        return TRIM_SW_LH_INC;
    }
    else {
      if (pos == 0)
        return TRIM_SW_LV_DEC;
      else
        return TRIM_SW_LV_INC;
    }
  }
  // right side
  else {
    if (type == 'H') {
      if (pos == 0)
        return TRIM_SW_RH_DEC;
      else
        return TRIM_SW_RH_INC;
    }
    else {
      if (pos == 0)
        return TRIM_SW_RV_DEC;
      else
        return TRIM_SW_RV_INC;
    }
  }
}

RadioTrimWidget * VirtualJoystickWidget::getTrimWidget(int which)
{
  if (which == Board::TRIM_AXIS_LH || which == Board::TRIM_AXIS_RH)
    return hTrimWidget;
  else
    return vTrimWidget;
}

void VirtualJoystickWidget::onButtonChange(bool checked)
{
  if (!sender() || !sender()->property("btnType").isValid())
    return;

  switch (sender()->property("btnType").toInt()) {
    case HOLD_Y:
      node->setCenteringY(!checked);
      break;
    case FIX_Y:
      node->setFixedY(checked);
      break;
    case FIX_X:
      node->setFixedX(checked);
      break;
    case HOLD_X:
      node->setCenteringX(!checked);
      break;
  }
}

void VirtualJoystickWidget::updateNodeValueLabels()
{
  if (nodeLabelX)
    nodeLabelX->setText(QString("%1").arg((qreal)node->getX() *  100 + getTrimValue(0) / 5, 2, 'f', 0));
  if (nodeLabelY)
    nodeLabelY->setText(QString("%1").arg((qreal)node->getY() * -100 + getTrimValue(1) / 5, 2, 'f', 0));
}
