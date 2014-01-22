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

static const QColor colors[C9X_MAX_CURVES] = {
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

#if 0
#ifdef __APPLE__
    ui->curveEdit_1->setStyleSheet("color: #00007f;");
    ui->curveEdit_2->setStyleSheet("color: #007f00;");
    ui->curveEdit_3->setStyleSheet("color: #7f0000;");
    ui->curveEdit_4->setStyleSheet("color: #007f7f;");
    ui->curveEdit_5->setStyleSheet("color: #7f007f;");
    ui->curveEdit_6->setStyleSheet("color: #7f7f00;");
    ui->curveEdit_7->setStyleSheet("color: #7f7f7f;");
    ui->curveEdit_8->setStyleSheet("color: #0000ff;");
    ui->curveEdit_9->setStyleSheet("color: #007fff;");
    ui->curveEdit_10->setStyleSheet("color: #7f00ff;");
    ui->curveEdit_11->setStyleSheet("color: #00ff00;");
    ui->curveEdit_12->setStyleSheet("color: #00ff7f;");
    ui->curveEdit_13->setStyleSheet("color: #7fff00;");
    ui->curveEdit_14->setStyleSheet("color: #ff0000;");
    ui->curveEdit_15->setStyleSheet("color: #ff007f;");
    ui->curveEdit_16->setStyleSheet("color: #ff7f00;");
#else
    ui->curveEdit_1->setStyleSheet("background-color: #00007f; color: white;");
    ui->curveEdit_2->setStyleSheet("background-color: #007f00; color: white;");
    ui->curveEdit_3->setStyleSheet("background-color: #7f0000; color: white;");
    ui->curveEdit_4->setStyleSheet("background-color: #007f7f; color: white;");
    ui->curveEdit_5->setStyleSheet("background-color: #7f007f; color: white;");
    ui->curveEdit_6->setStyleSheet("background-color: #7f7f00; color: white;");
    ui->curveEdit_7->setStyleSheet("background-color: #7f7f7f; color: white;");
    ui->curveEdit_8->setStyleSheet("background-color: #0000ff; color: white;");
    ui->curveEdit_9->setStyleSheet("background-color: #007fff; color: white;");
    ui->curveEdit_10->setStyleSheet("background-color: #7f00ff; color: white;");
    ui->curveEdit_11->setStyleSheet("background-color: #00ff00; color: white;");
    ui->curveEdit_12->setStyleSheet("background-color: #00ff7f; color: white;");
    ui->curveEdit_13->setStyleSheet("background-color: #7fff00; color: white;");
    ui->curveEdit_14->setStyleSheet("background-color: #ff0000; color: white;");
    ui->curveEdit_15->setStyleSheet("background-color: #ff007f; color: white;");
    ui->curveEdit_16->setStyleSheet("background-color: #ff7f00; color: white;");
#endif
#endif

Curves::Curves(QWidget * parent, ModelData & model):
  ModelPanel(parent, model),
  ui(new Ui::Curves),
  currentCurve(0)
{
  ui->setupUi(this);

  lock = true;

  if (!GetEepromInterface()->getCapability(HasCvNames)) {
    ui->curveName->hide();
    ui->curveNameLabel->hide();
  }

  QGraphicsScene *scene = new QGraphicsScene(ui->curvePreview);
  scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  ui->curvePreview->setScene(scene);

  for (int i=0; i<GetEepromInterface()->getCapability(NumCurves); i++) {
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
    if (GetEepromInterface()->getCapability(EnhancedCurves)) {
      insert = (i >= 1);
    }
    else {
      insert = (i==2 || i==4 || i==8 || i==16);
    }
    if (insert) {
      ui->curvePoints->addItem(tr("%1 points").arg(i+1), i+1);
    }
  }

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

  if (GetEepromInterface()->getCapability(HasCvNames)) {
    ui->curveName->setText(model.curves[currentCurve].name);
  }

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

  updateCurveType();
  updateCurve();

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

  if (GetEepromInterface()->getCapability(EnhancedCurves)) {
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

  int numcurves = GetEepromInterface()->getCapability(NumCurves);
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
}

void Curves::onPointEdited()
{
  if (!lock) {
    lock = true;
    int index = sender()->property("index").toInt();
    model.curves[currentCurve].points[index].x = spnx[index]->value();
    model.curves[currentCurve].points[index].y = spny[index]->value();
    updateCurve();
    emit modified();
    lock = false;
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
  int numcurves = GetEepromInterface()->getCapability(NumCurves);

  int totalpoints = 0;
  for (int i=0; i<numcurves; i++) {
    int cvPoints = (i==currentCurve ? points : model.curves[i].count);
    CurveData::CurveType cvType = (i==currentCurve ? type : model.curves[i].type);
    totalpoints += cvPoints + (cvType==CurveData::CURVE_TYPE_CUSTOM ? cvPoints-2 : 0);
  }

  int fwpoints = GetEepromInterface()->getCapability(NumCurvePoints);
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
        model.curves[currentCurve].points[i].x = (i >= model.curves[currentCurve].count-1 ? +100 : -100 + (200*i)/(numpoints-1));
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
        model.curves[currentCurve].points[i].x = (i >= model.curves[currentCurve].count-1 ? +100 : -100 + (200*i)/(numpoints-1));
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

#if 0
void ModelEdit::on_ca_ctype_CB_currentIndexChanged()
{
    int index=ui->ca_ctype_CB->currentIndex();
    switch (index) {
      case 0:
        ui->ca_coeff_SB->hide();
        ui->ca_coeff_label->hide();
        ui->ca_ymid_SB->hide();
        ui->ca_ymid_label->hide();
        ui->ca_ymin_SB->show();
        ui->ca_ymin_label->show();
        ui->ca_ymin_SB->setValue(-100);
        ui->ca_ymax_SB->setValue(100);
        break;
      case 1:
        ui->ca_coeff_SB->show();
        ui->ca_coeff_label->show();
        ui->ca_ymid_SB->hide();
        ui->ca_ymid_label->hide();
        ui->ca_ymin_SB->show();
        ui->ca_ymin_label->show();
        ui->ca_ymin_SB->setValue(-100);
        ui->ca_ymax_SB->setValue(100);
        break;
      case 2:
        ui->ca_coeff_SB->show();
        ui->ca_coeff_label->show();
        ui->ca_ymid_SB->hide();
        ui->ca_ymid_label->hide();
        ui->ca_ymin_SB->hide();
        ui->ca_ymin_label->hide();
        ui->ca_ymax_SB->setValue(100);
        break;
      case 3:
        ui->ca_coeff_SB->show();
        ui->ca_coeff_label->show();
        ui->ca_ymid_SB->show();
        ui->ca_ymid_label->show();
        ui->ca_ymin_SB->hide();
        ui->ca_ymin_label->hide();
        ui->ca_ymid_SB->setValue(0);
        ui->ca_ymax_SB->setValue(100);
        break;
    }
}

void ModelEdit::on_ca_apply_PB_clicked()
{
    int index=ui->ca_ctype_CB->currentIndex();
    float x;
    int y;
    int invert=0;
    float a;
    if (index==0) {
      a=(ui->ca_ymax_SB->value()-ui->ca_ymin_SB->value())/200.0;
      int numpoints=model.curves[currentCurve].count;
      for (int i=0; i<numpoints; i++) {
        if (model.curves[currentCurve].type == CurveData::CURVE_TYPE_CUSTOM) {
          x=(model.curves[currentCurve].points[i].x+100);
        } else {
          x=(200.0/(numpoints-1))*i;
        }
        y=ui->ca_ymin_SB->value()+a*x;
        switch (ui->ca_side_CB->currentIndex()) {
          case 0:
            model.curves[currentCurve].points[i].y=y;
            break;
          case 1:
            if (x>=100) {
              model.curves[currentCurve].points[i].y=y;
            }
            break;
          case 2:
            if (x<100) {
              model.curves[currentCurve].points[i].y=y;
            }
            break;
        }
      }
    } else if (index==1) {
      int numpoints=model.curves[currentCurve].count;
      for (int i=0; i<numpoints; i++) {
        if (model.curves[currentCurve].type == CurveData::CURVE_TYPE_CUSTOM) {
          x=((model.curves[currentCurve].points[i].x)+100)/2.0;
        } else {
          x=(100.0/(numpoints-1))*i;
        }
        a=ui->ca_coeff_SB->value();
        if (a>=0) {
          y=round(c9xexpou(x,a)*(ui->ca_ymax_SB->value()-ui->ca_ymin_SB->value())/100.0+ui->ca_ymin_SB->value());
        } else {
          a=-a;
          x=100-x;
          y=round((100.0-c9xexpou(x,a))*(ui->ca_ymax_SB->value()-ui->ca_ymin_SB->value())/100.0+ui->ca_ymin_SB->value());
        }
        switch (ui->ca_side_CB->currentIndex()) {
          case 0:
            model.curves[currentCurve].points[i].y=y;
            break;
          case 1:
            if (x>=50) {
              model.curves[currentCurve].points[i].y=y;
            }
            break;
          case 2:
            if (x<50) {
              model.curves[currentCurve].points[i].y=y;
            }
            break;
        }
      }
    } else if (index==2) {
      int numpoints=model.curves[currentCurve].count;
      for (int i=0; i<numpoints; i++) {
        if (model.curves[currentCurve].type == CurveData::CURVE_TYPE_CUSTOM) {
          x=(model.curves[currentCurve].points[i].x);
        } else {
          x=-100.0+(200.0/(numpoints-1))*i;
        }
        a=ui->ca_coeff_SB->value();
        if (x<0) {
          x=-x;
          invert=1;
        } else {
          invert=0;
        }
        if (a>=0) {
          y=round(c9xexpou(x,a)*(ui->ca_ymax_SB->value()/100.0));
        } else {
          a=-a;
          x=100-x;
          y=round((100.0-c9xexpou(x,a))*(ui->ca_ymax_SB->value()/100.0));
        }
        switch (ui->ca_side_CB->currentIndex()) {
          case 0:
            if (invert==1) {
              model.curves[currentCurve].points[i].y=-y;
            } else {
              model.curves[currentCurve].points[i].y=y;
            }
            break;
          case 1:
            if (invert==0) {
              model.curves[currentCurve].points[i].y=y;
            }
            break;
          case 2:
            if (invert==1) {
              model.curves[currentCurve].points[i].y=-y;
            }
            break;
        }
      }
    } else if (index==3) {
      int numpoints=model.curves[currentCurve].count;
      for (int i=0; i<numpoints; i++) {
        if (model.curves[currentCurve].type == CurveData::CURVE_TYPE_CUSTOM) {
          x=(model.curves[currentCurve].points[i].x);
        } else {
          x=-100.0+(200.0/(numpoints-1))*i;
        }
        int pos=(x>=0);
        a=ui->ca_coeff_SB->value();
        if (x<0) {
          x=-x;
        }
        if (a>=0) {
          y=round(c9xexpou(x,a)*((ui->ca_ymax_SB->value()-ui->ca_ymid_SB->value())/100.0)+ui->ca_ymid_SB->value());
        } else {
          a=-a;
          x=100-x;
          y=round((100.0-c9xexpou(x,a))*((ui->ca_ymax_SB->value()-ui->ca_ymid_SB->value())/100.0)+ui->ca_ymid_SB->value());
        }
        switch (ui->ca_side_CB->currentIndex()) {
          case 0:
            model.curves[currentCurve].points[i].y=y;
            break;
          case 1:
            if (pos) {
              model.curves[currentCurve].points[i].y=y;
            }
            break;
          case 2:
            if (!pos) {
              model.curves[currentCurve].points[i].y=y;
            }
            break;
        }
      }
    }
    updateSettings();
    setCurrentCurve(currentCurve);
    drawCurve();
}

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
