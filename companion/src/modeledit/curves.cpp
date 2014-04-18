#include "curves.h"
#include "ui_curves.h"
#include "node.h"
#include "edge.h"
#include "helpers.h"
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>

#define GFX_MARGIN 16

#define CURVE_COEFF_ENABLE   1
#define CURVE_YMID_ENABLE    2
#define CURVE_YMIN_ENABLE    4

float curveLinear(float x, float coeff, float yMin, float yMid, float yMax)
{
  float a = (yMax-yMin) / 200.0;
  return yMin + a * (x+100.0);
}

float c9xexpou(float point, float coeff)
{
  float x = point*1024.0/100.0;
  float k = coeff*256.0/100.0;
  return ((k*x*x*x/(1024*1024) + x*(256-k) + 128) / 256) / 1024.0 * 100;
}

float curveExpo(float x, float coeff, float yMin, float yMid, float yMax)
{
  float a = (yMax-yMin) / 100.0;

  x += 100.0;
  x /= 2.0;

  if (coeff >= 0) {
    return round(c9xexpou(x, coeff)*a + yMin);
  }
  else {
    coeff = -coeff;
    x = 100 - x;
    return round((100.0 - c9xexpou(x, coeff))*a + yMin);
  }
}

float curveSymmetricalY(float x, float coeff, float yMin, float yMid, float yMax)
{
  bool invert;
  if (x<0) {
    x = -x;
    invert = 1;
  }
  else {
    invert = 0;
  }

  float y;
  if (coeff >= 0) {
    y = round(c9xexpou(x, coeff) * (yMax/100.0));
  }
  else {
    coeff = -coeff;
    x = 100.0 - x;
    y = round((100.0-c9xexpou(x, coeff)) * (yMax/100.0));
  }

  if (invert) {
    y = -y;
  }

  return y;
}

float curveSymmetricalX(float x, float coeff, float yMin, float yMid, float yMax)
{
  float a = (yMax-yMid) / 100.0;

  if (x<0)
    x = -x;

  float y;
  if (coeff >= 0) {
    y = round(c9xexpou(x, coeff) * a + yMid);
  }
  else {
    coeff = -coeff;
    x = 100-x;
    y = round((100.0-c9xexpou(x, coeff)) * a + yMid);
  }

  return y;
}

Curves::Curves(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  ui(new Ui::Curves),
  currentCurve(0)
{
  ui->setupUi(this);

  lock = true;

  if (!firmware->getCapability(HasCvNames)) {
    ui->curveName->hide();
    ui->curveNameLabel->hide();
  }

  QGraphicsScene *scene = new QGraphicsScene(ui->curvePreview);
  scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  ui->curvePreview->setScene(scene);

  for (int i=0; i<firmware->getCapability(NumCurves); i++) {
    visibleCurves[i] = false;

    // The reset curve button
    QPushButton * reset = new QPushButton(this);
    reset->setProperty("index", i);
    reset->setMinimumSize(QSize(0, 0));
    reset->setIcon(CompanionIcon("clear.png"));
    reset->setIconSize(QSize(14, 14));
    connect(reset, SIGNAL(clicked()), this, SLOT(resetCurve()));
    ui->curvesLayout->addWidget(reset, i, 0, 1, 1);

    // The edit curve button
    QPushButton * edit = new QPushButton(this);
    edit->setProperty("index", i);
    QPalette palette;
    palette.setBrush(QPalette::Active, QPalette::Button, QBrush(colors[i]));
    palette.setBrush(QPalette::Active, QPalette::ButtonText, QBrush(Qt::white));
#ifdef __APPLE__
    edit->setStyleSheet(QString("color: %1;").arg(colors[i].name()));
#else
    edit->setStyleSheet(QString("background-color: %1; color: white;").arg(colors[i].name()));
#endif
    edit->setPalette(palette);
    edit->setText(tr("Curve %1").arg(i+1));
    connect(edit, SIGNAL(clicked()), this, SLOT(editCurve()));
    ui->curvesLayout->addWidget(edit, i, 1, 1, 1);

    // The curve plot checkbox
    QCheckBox * plot = new QCheckBox(this);
    plot->setProperty("index", i);
    plot->setPalette(palette);
    connect(plot, SIGNAL(toggled(bool)), this, SLOT(plotCurve(bool)));
    ui->curvesLayout->addWidget(plot, i, 2, 1, 1);
  }

  for (int i=0; i<C9X_MAX_POINTS; i++) {
    spny[i] = new QSpinBox(this);
    spny[i]->setProperty("index", i);
    spny[i]->setMinimum(-100);
    spny[i]->setMaximum(+100);
    spny[i]->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    spny[i]->setAccelerated(true);
    connect(spny[i], SIGNAL(valueChanged(int)), this, SLOT(onPointEdited()));
    ui->pointsLayout->addWidget(spny[i], i, 0, 1, 1);

    spnx[i] = new QSpinBox(this);
    spnx[i]->setProperty("index", i);
    spnx[i]->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    spnx[i]->setAccelerated(true);
    connect(spnx[i], SIGNAL(valueChanged(int)), this, SLOT(onPointEdited()));
    ui->pointsLayout->addWidget(spnx[i], i, 1, 1, 1);

    bool insert;
    if (firmware->getCapability(EnhancedCurves)) {
      insert = (i >= 1);
    }
    else {
      insert = (i==2 || i==4 || i==8 || i==16);
    }
    if (insert) {
      ui->curvePoints->addItem(tr("%1 points").arg(i+1), i+1);
    }
  }

  addTemplate(tr("Linear"), CURVE_YMIN_ENABLE, curveLinear);
  addTemplate(tr("Single Expo"), CURVE_COEFF_ENABLE | CURVE_YMIN_ENABLE, curveExpo);
  addTemplate(tr("Symmetrical f(x)=-f(-x)"), CURVE_COEFF_ENABLE, curveSymmetricalY);
  addTemplate(tr("Symmetrical f(x)=f(-x)"), CURVE_COEFF_ENABLE | CURVE_YMID_ENABLE, curveSymmetricalX);

  lock = false;
}

Curves::~Curves()
{
  delete ui;
}

void Curves::editCurve()
{
  QPushButton *button = (QPushButton *)sender();
  int index = button->property("index").toInt();
  setCurrentCurve(index);
  update();
}

void Curves::resetCurve()
{
  QPushButton *button = (QPushButton *)sender();
  int index = button->property("index").toInt();
  int res = QMessageBox::question(this, "companion", tr("Are you sure you want to reset curve %1 ?").arg(index+1), QMessageBox::Yes | QMessageBox::No);
  if (res == QMessageBox::Yes) {
    model.curves[index].clear(5);
    update();
    emit modified();
  }
}

void Curves::plotCurve(bool checked)
{
  QCheckBox *chk = (QCheckBox *)sender();
  int index = chk->property("index").toInt();
  visibleCurves[index] = checked;
  updateCurve();
}

void Curves::update()
{
  lock = true;

  if (firmware->getCapability(HasCvNames)) {
    ui->curveName->setText(model.curves[currentCurve].name);
  }

  updateCurveType();
  updateCurve();
  updateCurvePoints();

  lock = false;
}

void Curves::setCurrentCurve(int index)
{
  currentCurve = index;
}

void Curves::updateCurveType()
{
  lock = true;

  int index = 0;

  if (firmware->getCapability(EnhancedCurves)) {
    index = model.curves[currentCurve].count - 2;
  }
  else {
    if (model.curves[currentCurve].count == 5)
      index += 2;
    else if (model.curves[currentCurve].count == 9)
      index += 4;
    else if (model.curves[currentCurve].count == 17)
      index += 6;
  }

  ui->curvePoints->setCurrentIndex(index);
  ui->curveCustom->setCurrentIndex(model.curves[currentCurve].type);
  ui->curveSmooth->setCurrentIndex(model.curves[currentCurve].smooth);

  lock = false;
}

void Curves::updateCurve()
{
  lock = true;

  Node * nodel = 0;
  Node * nodex = 0;
  QColor color;

  QGraphicsScene *scene = ui->curvePreview->scene();
  scene->clear();

  qreal width  = scene->sceneRect().width();
  qreal height = scene->sceneRect().height();

  qreal centerX = scene->sceneRect().left() + width/2; //center X
  qreal centerY = scene->sceneRect().top() + height/2; //center Y

  QGraphicsSimpleTextItem *ti = scene->addSimpleText(tr("Editing curve %1").arg(currentCurve+1));
  ti->setPos(3, 3);

  scene->addLine(centerX, GFX_MARGIN, centerX, height+GFX_MARGIN);
  scene->addLine(GFX_MARGIN, centerY, width+GFX_MARGIN, centerY);

  QPen pen;
  pen.setWidth(1);
  pen.setStyle(Qt::SolidLine);

  int numcurves = firmware->getCapability(NumCurves);
  for (int k=0; k<numcurves; k++) {
    pen.setColor(colors[k]);
    if (currentCurve!=k && visibleCurves[k]) {
      int numpoints = model.curves[k].count;
      for (int i=0; i<numpoints-1; i++) {
        if (model.curves[k].type == CurveData::CURVE_TYPE_CUSTOM)
          scene->addLine(centerX + (qreal)model.curves[k].points[i].x*width/200,centerY - (qreal)model.curves[k].points[i].y*height/200,centerX + (qreal)model.curves[k].points[i+1].x*width/200,centerY - (qreal)model.curves[k].points[i+1].y*height/200, pen);
        else
          scene->addLine(GFX_MARGIN + i*width/(numpoints-1),centerY - (qreal)model.curves[k].points[i].y*height/200,GFX_MARGIN + (i+1)*width/(numpoints-1),centerY - (qreal)model.curves[k].points[i+1].y*height/200, pen);
      }
    }
  }

  int numpoints = model.curves[currentCurve].count;
  for (int i=0; i<numpoints; i++) {
    nodel = nodex;
    nodex = new Node();
    nodex->setProperty("index", i);
    nodex->setColor(colors[currentCurve]);
    if (model.curves[currentCurve].type == CurveData::CURVE_TYPE_CUSTOM) {
      if (i>0 && i<numpoints-1) {
        nodex->setFixedX(false);
        nodex->setMinX(model.curves[currentCurve].points[i-1].x);
        nodex->setMaxX(model.curves[currentCurve].points[i+1].x);
      }
      else {
        nodex->setFixedX(true);
      }
      nodex->setPos(centerX + (qreal)model.curves[currentCurve].points[i].x*width/200,centerY - (qreal)model.curves[currentCurve].points[i].y*height/200);
    }
    else {
      nodex->setFixedX(true);
      nodex->setPos(GFX_MARGIN + i*width/(numpoints-1), centerY - (qreal)model.curves[currentCurve].points[i].y*height/200);
    }
    connect(nodex, SIGNAL(moved(int, int)), this, SLOT(onNodeMoved(int, int)));
    connect(nodex, SIGNAL(focus()), this, SLOT(onNodeFocus()));
    connect(nodex, SIGNAL(unfocus()), this, SLOT(onNodeUnfocus()));
    scene->addItem(nodex);
    if (i>0) scene->addItem(new Edge(nodel, nodex));
  }

  lock = false;
}

void Curves::updateCurvePoints()
{
  lock = true;

  int count = model.curves[currentCurve].count;
  for (int i=0; i<count; i++) {
    spny[i]->show();
    spny[i]->setValue(model.curves[currentCurve].points[i].y);
    if (model.curves[currentCurve].type == CurveData::CURVE_TYPE_CUSTOM) {
      spnx[i]->show();
      if (i==0 || i==model.curves[currentCurve].count-1) {
        spnx[i]->setDisabled(true);
        spnx[i]->setMaximum(+100);
        spnx[i]->setMinimum(-100);
      }
      else {
        spnx[i]->setMaximum(model.curves[currentCurve].points[i+1].x);
        spnx[i]->setMinimum(model.curves[currentCurve].points[i-1].x);
      }
      spnx[i]->setValue(model.curves[currentCurve].points[i].x);
    }
    else {
      spnx[i]->hide();
    }
  }
  for (int i=count; i<C9X_MAX_POINTS; i++) {
    spny[i]->hide();
    spnx[i]->hide();
  }

  lock = false;
}

void Curves::onPointEdited()
{
  if (!lock) {
    int index = sender()->property("index").toInt();
    model.curves[currentCurve].points[index].x = spnx[index]->value();
    model.curves[currentCurve].points[index].y = spny[index]->value();
    updateCurve();
    emit modified();
  }
}

void Curves::onNodeMoved(int x, int y)
{
  if (!lock) {
    lock = true;
    int index = sender()->property("index").toInt();
    model.curves[currentCurve].points[index].x = x;
    model.curves[currentCurve].points[index].y = y;
    spnx[index]->setValue(x);
    spny[index]->setValue(y);
    emit modified();
    lock = false;
  }
}

void Curves::onNodeFocus()
{
  int index = sender()->property("index").toInt();
  spny[index]->setFocus();
}

void Curves::onNodeUnfocus()
{
  int index = sender()->property("index").toInt();
  spny[index]->clearFocus();
  updateCurve();
}

bool Curves::allowCurveType(int points, CurveData::CurveType type)
{
  int numcurves = firmware->getCapability(NumCurves);

  int totalpoints = 0;
  for (int i=0; i<numcurves; i++) {
    int cvPoints = (i==currentCurve ? points : model.curves[i].count);
    CurveData::CurveType cvType = (i==currentCurve ? type : model.curves[i].type);
    totalpoints += cvPoints + (cvType==CurveData::CURVE_TYPE_CUSTOM ? cvPoints-2 : 0);
  }

  int fwpoints = firmware->getCapability(NumCurvePoints);
  if (totalpoints > fwpoints) {
    QMessageBox::warning(this, "companion", tr("Not enough free points in EEPROM to store the curve."));
    return false;
  }
  else {
    return true;
  }
}

void Curves::on_curvePoints_currentIndexChanged(int index)
{
  if (!lock) {
    int numpoints = ((QComboBox *)sender())->itemData(index).toInt();

    if (allowCurveType(numpoints, model.curves[currentCurve].type)) {
      model.curves[currentCurve].count = numpoints;

      // TODO something better + reuse!
      for (int i=0; i<C9X_MAX_POINTS; i++) {
        model.curves[currentCurve].points[i].x = (i >= numpoints-1 ? +100 : -100 + (200*i)/(numpoints-1));
        model.curves[currentCurve].points[i].y = 0;
      }

      update();
      emit modified();
    }
    else {
      updateCurveType();
    }
  }
}

void Curves::on_curveCustom_currentIndexChanged(int index)
{
  if (!lock) {
    CurveData::CurveType type = (CurveData::CurveType)index;
    int numpoints = ui->curvePoints->itemData(ui->curvePoints->currentIndex()).toInt();
    if (allowCurveType(model.curves[currentCurve].count, type)) {
      model.curves[currentCurve].type = type;

      // TODO something better + reuse!
      for (int i=0; i<C9X_MAX_POINTS; i++) {
        model.curves[currentCurve].points[i].x = (i >= numpoints-1 ? +100 : -100 + (200*i)/(numpoints-1));
        model.curves[currentCurve].points[i].y = 0;
      }

      update();
      emit modified();
    }
    else {
      updateCurveType();
    }
  }
}

void Curves::on_curveSmooth_currentIndexChanged(int index)
{
  model.curves[currentCurve].smooth = index;
  update();
}

void Curves::on_curveName_editingFinished()
{
  memset(model.curves[currentCurve].name, 0, sizeof(model.curves[currentCurve].name));
  strcpy(model.curves[currentCurve].name, ui->curveName->text().toAscii());
  emit modified();
}

void Curves::resizeEvent(QResizeEvent *event)
{
  QRect qr = ui->curvePreview->contentsRect();
  ui->curvePreview->scene()->setSceneRect(GFX_MARGIN, GFX_MARGIN, qr.width()-GFX_MARGIN*2, qr.height()-GFX_MARGIN*2);
  updateCurve();
  ModelPanel::resizeEvent(event);
}

void Curves::on_curveType_currentIndexChanged()
{
  int index = ui->curveType->currentIndex();
  unsigned int flags = templates[index].flags;
  ui->curveCoeffLabel->setVisible(flags & CURVE_COEFF_ENABLE);
  ui->curveCoeff->setVisible(flags & CURVE_COEFF_ENABLE);
  ui->yMax->setValue(100);
  ui->yMidLabel->setVisible(flags & CURVE_YMID_ENABLE);
  ui->yMid->setVisible(flags & CURVE_YMID_ENABLE);
  ui->yMid->setValue(0);
  ui->yMinLabel->setVisible(flags & CURVE_YMIN_ENABLE);
  ui->yMin->setVisible(flags & CURVE_YMIN_ENABLE);
  ui->yMin->setValue(-100);
}

void Curves::addTemplate(QString name, unsigned int flags, curveFunction function)
{
  CurveCreatorTemplate tmpl;
  tmpl.name = name;
  tmpl.flags = flags;
  tmpl.function = function;
  templates.append(tmpl);
  ui->curveType->addItem(name);
}

void Curves::on_curveApply_clicked()
{
  int index = ui->curveType->currentIndex();
  int numpoints = model.curves[currentCurve].count;

  for (int i=0; i<numpoints; i++) {
    float x;
    if (model.curves[currentCurve].type == CurveData::CURVE_TYPE_CUSTOM)
      x = model.curves[currentCurve].points[i].x;
    else
      x = -100.0 + (200.0/(numpoints-1))*i;

    bool apply = false;
    switch (ui->curveSide->currentIndex()) {
      case 0:
        apply = true;
        break;
      case 1:
        if (x>=0)
          apply = true;
        break;
      case 2:
        if (x<0)
          apply = true;
        break;
    }

    if (apply) {
      model.curves[currentCurve].points[i].y = templates[index].function(x, ui->curveCoeff->value(), ui->yMin->value(), ui->yMid->value(), ui->yMax->value());
    }
  }

  updateCurve();
  updateCurvePoints();
  emit modified();
}

#if 0
void ModelEdit::clearCurves(bool ask)
{
    if (ask) {
      int res = QMessageBox::question(this, tr("Clear Curves?"), tr("Really clear all the curves?"), QMessageBox::Yes | QMessageBox::No);
      if (res!=QMessageBox::Yes) return;
    }
    curvesLock=true;
    for (int j=0; j<16; j++) {
      model.curves[j].count = 5;
      model.curves[j].custom = false;
      memset(model.curves[j].name, 0, sizeof(model.curves[j].name));
      for (int i=0; i<17; i++) {
        model.curves[j].points[i].x = 0;
        model.curves[j].points[i].y = 0;
      }
    }
    for (int i=0; i<17; i++) {
      spnx[i]->setMinimum(-100);
      spnx[i]->setMaximum(100);
      spnx[i]->setValue(0);
      spny[i]->setValue(0);
    }
    currentCurve=0;
    curvesLock=false;
    ui->curvetype_CB->setCurrentIndex(2);
    ui->curveName->clear();
    updateSettings();
    drawCurve();
}

void ModelEdit::ControlCurveSignal(bool flag)
{
    foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("curvePt[0-9]+"))) {
      sb->blockSignals(flag);
    }
}
#endif
