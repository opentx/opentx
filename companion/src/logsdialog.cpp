#include "logsdialog.h"
#include "ui_logsdialog.h"
#include "qcustomplot.h"
#if defined WIN32 || !defined __GNUC__
#include <windows.h>
#else
#include <unistd.h>
#endif

logsDialog::logsDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    ui(new Ui::logsDialog)
{
  csvlog.clear();
  srand(QDateTime::currentDateTime().toTime_t());
  ui->setupUi(this);
  palette.clear();
  plotLock=false;
  for (int i=0; i< 60; i++) 
        palette << QColor(rand()%245+10, rand()%245+10, rand()%245+10);
  ui->customPlot->setInteractions(QCustomPlot::iRangeDrag | QCustomPlot::iRangeZoom | QCustomPlot::iSelectAxes |
                                  QCustomPlot::iSelectLegend | QCustomPlot::iSelectPlottables | QCustomPlot::iSelectTitle | QCustomPlot::iSelectItems);
  ui->customPlot->setRangeDrag(Qt::Horizontal|Qt::Vertical);
  ui->customPlot->setRangeZoom(Qt::Horizontal|Qt::Vertical);
  ui->customPlot->yAxis->setRange(-1100, 1100);
  ui->customPlot->setupFullAxesBox();
  ui->customPlot->setTitle(tr("Telemetry logs"));
  ui->customPlot->xAxis->setLabel(tr("Time"));
  ui->customPlot->legend->setVisible(true);
  ui->customPlot->yAxis->setTickLabels(false);
  ui->customPlot->yAxis->setAutoTickCount(10);
  ui->customPlot->xAxis2->setTicks(false);
  ui->customPlot->yAxis2->setTicks(false);
  ui->customPlot->setRangeZoomFactor(2, 2);
  QFont legendFont = font();
  legendFont.setPointSize(10);
  ui->customPlot->legend->setFont(legendFont);
  ui->customPlot->legend->setSelectedFont(legendFont);
  ui->customPlot->legend->setSelectable(QCPLegend::spItems); // legend box shall not be selectable, only legend items
  ui->customPlot->legend->setVisible(false);
  QSettings settings("companion9x", "companion9x");
  QString Path=settings.value("gePath", "").toString();
  if (Path.isEmpty() || !QFile(Path).exists()) {
    ui->mapsButton->hide();
  }  
  
  // connect slot that ties some axis selections together (especially opposite axes):
  connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
  // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
  connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
  connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
 
  // make bottom and left axes transfer their ranges to top and right axes:
  connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
  
  // connect some interaction slots:
  connect(ui->customPlot, SIGNAL(titleDoubleClick(QMouseEvent*)), this, SLOT(titleDoubleClick()));
  connect(ui->customPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
  connect(ui->customPlot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
  connect(ui->customPlot, SIGNAL(plottableDoubleClick(QCPAbstractPlottable *, QMouseEvent *)), this, SLOT(plottableItemDoubleClick(QCPAbstractPlottable *, QMouseEvent *)));
  connect(ui->FieldsTW, SIGNAL(itemSelectionChanged()), this, SLOT(plotLogs()));
  connect(ui->logTable, SIGNAL(itemSelectionChanged()), this, SLOT(plotLogs()));
}

logsDialog::~logsDialog()
{
  delete ui;
}

double logsDialog::GetScale(QString channel) {
  QString Analog="Rud,Ele,Thr,Ail,P1,P2,P3";
  QString Switches="THR,RUD,ELE,ID0,ID1,ID2,AIL,GEA,TRN";
  if (Analog.contains(channel)) {
    return 1.0;
  }
  if (Switches.contains(channel)) {
    return 0.001;
  }
  return -1;
}

void logsDialog::titleDoubleClick()
{
  // Set the plot title by double clicking on it
  
  bool ok;
  QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, ui->customPlot->title(), &ok);
  if (ok)
  {
    ui->customPlot->setTitle(newTitle);
    ui->customPlot->replot();
  }
}

void logsDialog::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
  // Set an axis label by double clicking on it
  
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
    if (ok)
    {
      axis->setLabel(newLabel);
      ui->customPlot->replot();
    }
  }
}

void logsDialog::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
  // Rename a graph by double clicking on its legend item
  
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      ui->customPlot->replot();
    }
  }
}
void logsDialog::plottableItemDoubleClick(QCPAbstractPlottable *  plottable, QMouseEvent * event)
{
//   qDebug() << plottable->
}

void logsDialog::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.
   
   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.
   
   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */
  if (plotLock)
    return;
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->xAxis->selected().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selected().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->xAxis2->selected().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selected().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->xAxis2->setSelected(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->xAxis->setSelected(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->yAxis->selected().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selected().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->yAxis2->selected().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selected().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->yAxis2->setSelected(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->yAxis->setSelected(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  
  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<ui->customPlot->graphCount(); ++i)
  {
    QCPGraph *graph = ui->customPlot->graph(i);
    QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelected(true);
    }
  }
}

void logsDialog::on_mapsButton_clicked() {
  int n = csvlog.count(); // number of points in graph
  if (n==0) return;
  int latcol=0, longcol=0, altcol=0, coursecol=0, speedcol=0;
  int itemSelected=0.;
  bool rangeSelected=false;
  QSettings settings("companion9x", "companion9x");
  QString gePath=settings.value("gePath", "").toString();
  if (gePath.isEmpty() || !QFile(gePath).exists()) {
    return;
  }  
  for (int i=1; i<csvlog.at(0).count(); i++) {
    //Long,Lat,Course,GPS Speed,GPS Alt
    if (csvlog.at(0).at(i).contains("Long")) {
      longcol=i;
    }
    if (csvlog.at(0).at(i).contains("Lat")) {
      latcol=i;
    }
    if (csvlog.at(0).at(i).contains("GPS Alt")) {
      altcol=i;
    }
    if (csvlog.at(0).at(i).contains("GPS Speed")) {
      speedcol=i;
    }
    if (csvlog.at(0).at(i).contains("Course")) {
      coursecol=i;
    }
  }
  if (longcol==0 || latcol==0 || altcol==0) {
    return;
  }
  for (int i=1; i<n; i++) {
    if (ui->logTable->item(i-1,1)->isSelected()) {
      rangeSelected=true;
      itemSelected++;
    }
  }
  if (itemSelected==0) {
    itemSelected=n-1;
  }
  QString geFilename = QDir::tempPath() + "/flight.kml";
  if (QFile::exists(geFilename)) {
    unlink(geFilename.toAscii());
  }
  QFile geFile(geFilename);
  if (!geFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Error"),
        tr("Cannot write file %1:\n%2.")
        .arg(geFilename)
        .arg(geFile.errorString()));
    return;
  }

  QTextStream outputStream(&geFile);
  outputStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">\n";
  outputStream << "<Document>\n\t<name>flight.kml</name>\n\t<Placemark><name>My Flight</name>\n\t\t<LineString>\n";
  outputStream << "\t\t\t<tessellate>1</tessellate>\n\t\t\t<gx:altitudeMode>relativeToGround</gx:altitudeMode>\n\t\t\t<coordinates>\n\t\t\t\t";
  for (int i=1; i<n; i++) {
    if ((ui->logTable->item(i-1,1)->isSelected() &&rangeSelected) || !rangeSelected) {
      outputStream << csvlog.at(i).at(longcol) << "," << csvlog.at(i).at(latcol) << "," << csvlog.at(i).at(altcol) << " " ;
    }
  }
  outputStream << "\n\t\t\t</coordinates>\n\t\t</LineString>\n\t</Placemark>\n</Document>\n</kml>\n";
  geFile.close();
  
  QStringList parameters; 
  parameters << geFilename;
  QProcess *process = new QProcess(this);
  process->startDetached(gePath, parameters);
}


void logsDialog::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged
  
  if (ui->customPlot->xAxis->selected().testFlag(QCPAxis::spAxis))
    ui->customPlot->setRangeDrag(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selected().testFlag(QCPAxis::spAxis))
    ui->customPlot->setRangeDrag(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void logsDialog::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed
  int orientation=0;
  if (ui->ZoomX_ChkB->isChecked()) {
    orientation|=Qt::Horizontal;
  }
  if (ui->ZoomY_ChkB->isChecked()) {
    orientation|=Qt::Vertical;
  }
  if (orientation) {
    ui->customPlot->setRangeZoom((Qt::Orientation)orientation);
  }
}

void logsDialog::plotValue(int index, int plot, int numplots)
{
  if (plotLock)
    return;
  int n = csvlog.count(); // number of points in graph
  bool rangeSelected=false;
  uint minx;
  uint maxx;
  uint itemSelected=0;
  uint itemCount=0;
  double miny=9999;
  double maxy=-9999;
  minx=-1;
  maxx=0;
  double tmpval,yscale;
  for (int i=1; i<n; i++) {
    if (ui->logTable->item(i-1,1)->isSelected()) {
      rangeSelected=true;
      itemSelected++;
    }
  }
  if (itemSelected==0) {
    itemSelected=n-1;
  }
  QVector<double> x(itemSelected), y(itemSelected);
  if (numplots<3) {
    for (int i=1; i<n; i++) {
      if ((ui->logTable->item(i-1,1)->isSelected() &&rangeSelected) || !rangeSelected) {
        double tmp;
        QString tstamp=csvlog.at(i).at(0)+QString(" ")+csvlog.at(i).at(1);
        if (csvlog.at(i).at(1).contains(".")) {
          tmp=QDateTime::fromString(tstamp, "yyyy-MM-dd HH:mm:ss.zzz").toTime_t();
          tmp+=csvlog.at(i).at(1).mid(csvlog.at(i).at(1).indexOf(".")).toDouble();
        } else {
          tmp=QDateTime::fromString(tstamp, "yyyy-MM-dd HH:mm:ss").toTime_t();
        }
        if (minx>tmp) {
          minx=tmp;
        }
        if (maxx<tmp) {
          maxx=tmp;
        }
        tmpval = csvlog.at(i).at(index).toDouble();
        if (tmpval>maxy) {
          maxy=tmpval;
        }
        if (tmpval<miny) {
          miny=tmpval;
        }
      }
    }
    yscale=1;
  } else {
    yscale=GetScale(csvlog.at(0).at(index));
    for (int i=1; i<n; i++)
    {
      if ((ui->logTable->item(i-1,1)->isSelected() &&rangeSelected) || !rangeSelected) {
        double tmp;
        QString tstamp=csvlog.at(i).at(0)+QString(" ")+csvlog.at(i).at(1);
        if (csvlog.at(i).at(1).contains(".")) {
          tmp=QDateTime::fromString(tstamp, "yyyy-MM-dd HH:mm:ss.zzz").toTime_t();
          tmp+=csvlog.at(i).at(1).mid(csvlog.at(i).at(1).indexOf(".")).toDouble();
        } else {
          tmp=QDateTime::fromString(tstamp, "yyyy-MM-dd HH:mm:ss").toTime_t();
        }
        if (minx>tmp) {
          minx=tmp;
        }
        if (maxx<tmp) {
          maxx=tmp;
        }
        if (yscale<0) {
          tmpval = csvlog.at(i).at(index).toDouble();
          if (tmpval>maxy) {
            maxy=tmpval;
          }
          if (tmpval<miny) {
            miny=tmpval;
          }
        }
      }
    }
    if (yscale<0) {
      if (miny<0) {
        miny=-miny;
      }
      if (maxy<0) {
        maxy=-maxy;
      }
      if (miny>maxy) {
        yscale=miny/1000.0;
      } else {
        yscale=maxy/1000.0;
      }
      if (yscale==0) {
        yscale=1;
      }
    }
  }
  for (int i=1; i<n; i++) {
    if ((ui->logTable->item(i-1,1)->isSelected() &&rangeSelected) || !rangeSelected) {
      double tmp;
      QString tstamp=csvlog.at(i).at(0)+QString(" ")+csvlog.at(i).at(1);
      if (csvlog.at(i).at(1).contains(".")) {
        tmp=QDateTime::fromString(tstamp, "yyyy-MM-dd HH:mm:ss.zzz").toTime_t();
        tmp+=csvlog.at(i).at(1).mid(csvlog.at(i).at(1).indexOf(".")).toDouble();
      } else {
        tmp=QDateTime::fromString(tstamp, "yyyy-MM-dd HH:mm:ss").toTime_t();
      }
      x[itemCount] = tmp-minx;
      y[itemCount] = csvlog.at(i).at(index).toDouble()/yscale;
      itemCount++;
    }
  }
  QPen graphPen;
  QColor color=palette.at(index % 60);
  graphPen.setColor(color);
  graphPen.setWidthF(1.5);
  if (numplots<3) {
    if (plot==1) {
      ui->customPlot->xAxis->setRange(0, maxx-minx);
      ui->customPlot->yAxis->setRange(miny,maxy);
      ui->customPlot->yAxis->setLabelColor(color);
      ui->customPlot->yAxis->setLabel(csvlog.at(0).at(index));
      ui->customPlot->yAxis->setTickLabels(true);
      ui->customPlot->yAxis->setVisible(true);
      ui->customPlot->yAxis2->setVisible(false);
      ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);
      ui->customPlot->graph(0)->setName(csvlog.at(0).at(index));
      ui->customPlot->graph(0)->setData(x, y);
      ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
      ui->customPlot->graph(0)->setScatterStyle(QCP::ssNone);
      //ui->customPlot->graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1));
      ui->customPlot->graph(0)->setPen(graphPen);
    } else {
      ui->customPlot->xAxis2->setRange(0, maxx-minx);
      ui->customPlot->yAxis2->setTickLabels(true);
      ui->customPlot->yAxis2->setRange(miny,maxy);
      ui->customPlot->yAxis2->setVisible(true);
      ui->customPlot->yAxis2->setLabelColor(color);
      ui->customPlot->yAxis2->setLabel(csvlog.at(0).at(index));
      ui->customPlot->addGraph(ui->customPlot->xAxis2, ui->customPlot->yAxis2);
      ui->customPlot->graph(1)->setName(csvlog.at(0).at(index));
      ui->customPlot->graph(1)->setData(x, y);
      ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsLine);
      ui->customPlot->graph(1)->setScatterStyle(QCP::ssNone);
      //ui->customPlot->graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1));
      ui->customPlot->graph(1)->setPen(graphPen);
    }
  } else {
      ui->customPlot->yAxis->setTickLabels(false);
      ui->customPlot->yAxis2->setTickLabels(false);
      ui->customPlot->yAxis->setRange(-1100, 1100);
      ui->customPlot->yAxis->setVisible(false);
      ui->customPlot->yAxis2->setVisible(false);
      ui->customPlot->addGraph();
      ui->customPlot->graph()->setName(csvlog.at(0).at(index));
      ui->customPlot->graph()->setData(x, y);
      ui->customPlot->graph()->setLineStyle(QCPGraph::lsLine);
      ui->customPlot->graph()->setScatterStyle(QCP::ssNone);
      //ui->customPlot->graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1));
      ui->customPlot->graph()->setPen(graphPen);
  }
}

void logsDialog::removeSelectedGraph()
{
  if (ui->customPlot->selectedGraphs().size() > 0)
  {
    ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
    ui->customPlot->replot();
  }
}

void logsDialog::removeAllGraphs()
{
  ui->customPlot->clearGraphs();
  ui->customPlot->replot();
}

void logsDialog::moveLegend()
{
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
  {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok)
    {
      ui->customPlot->legend->setPositionStyle((QCPLegend::PositionStyle)dataInt);
      ui->customPlot->replot();
    }
  }
}

void logsDialog::on_fileOpen_BT_clicked()
{
  QSettings settings("companion9x", "companion9x");
  QString fileName = QFileDialog::getOpenFileName(this,tr("Select your log file"), settings.value("lastLogDir").toString());
  if (!fileName.isEmpty()) {
    settings.setValue("lastLogDir", fileName);
    ui->FileName_LE->setText(fileName);
    if (cvsFileParse()) {
      ui->FieldsTW->clear();
      ui->logTable->clear();
      ui->FieldsTW->setShowGrid(false);
      ui->FieldsTW->setContentsMargins(0,0,0,0);
      ui->FieldsTW->setRowCount(csvlog.at(0).count()-2);
      ui->FieldsTW->setColumnCount(1);
      ui->FieldsTW->setHorizontalHeaderLabels(QStringList(tr("Available fields")));
      ui->logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
      for (int i=2; i<csvlog.at(0).count(); i++) {
        QTableWidgetItem* item= new QTableWidgetItem(csvlog.at(0).at(i));
        ui->FieldsTW->setItem(0,i-2,item);
      }
      ui->FieldsTW->resizeRowsToContents();
      ui->logTable->setColumnCount(csvlog.at(0).count());
      ui->logTable->setRowCount(csvlog.count()-1);
      ui->logTable->setHorizontalHeaderLabels(csvlog.at(0));
      for (int i=1; i<csvlog.count(); i++) {
        for (int j=0; j<csvlog.at(0).count(); j++) {
          QTableWidgetItem* item= new QTableWidgetItem(csvlog.at(i).at(j));
          if (j>1) {
            item->setTextAlignment(Qt::AlignRight);
          } else {
            item->setTextAlignment(Qt::AlignCenter);
          }
          ui->logTable->setItem(i-1,j,item );
        }
      }     
      ui->logTable->resizeColumnsToContents();
      ui->logTable->resizeRowsToContents();
      // Hack - add some pixel of space to columns as Qt resize them too small
      for (int j=0; j<csvlog.at(0).count(); j++) {
        int width=ui->logTable->columnWidth(j);
        ui->logTable->setColumnWidth(j,width+5);
      }
    }
  }
}

bool logsDialog::cvsFileParse() 
{
  QFile file(ui->FileName_LE->text());
  int errors=0;
  int lines=-1;

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { //reading HEX TEXT file
    return false;
  } else {
    ui->sessions_CB->clear();
    csvlog.clear();
    QTextStream inputStream(&file);
    QRegExp rx2("(?:\"([^\"]*)\";?)|(?:([^,]*),?)");
    QStringList list;
    QString buffer = file.readLine();
    if (buffer.startsWith("Date,Time")) {
      file.reset();
    } else {
      return false;
    }
    int numfields=-1;
    while (!file.atEnd()) {
      int pos2 = 0;
      QString buffer = file.readLine();
      QString line=buffer.trimmed();
      list.clear();
      if(line.size()<1){
        list << "";		
      } else {
        while (line.size()>pos2 && (pos2 = rx2.indexIn(line, pos2)) != -1) {
          QString col;
          if(rx2.cap(1).size()>0) {
            col = rx2.cap(1);
          } else if(rx2.cap(2).size()>0) {
            col = rx2.cap(2);
          }
          if (col.contains(".")) {
            if (col.indexOf(".")==(col.length()-3)) {
              col.append("0");
            }
          }
          list<<col;
          if(col.size()) {
            pos2 += rx2.matchedLength();
          } else {
            pos2++;
          }
        }
      }
      if (numfields==-1) {
        numfields=list.count();
      }
      if (list.count()==numfields) {
        csvlog.append(list);
      } else {
        errors++;
      }
      lines++;
    }
  }
  
  file.close();
  if (errors>1) {
    QMessageBox::warning(this, "companion9x", tr("The selected logfile contains %1 invalid lines out of  %2 total lines").arg(errors).arg(lines));
  }
  plotLock=true;
  int n=csvlog.count();
  if (n==1) {
    csvlog.clear();
    return false;
  }
  int lastvalue=0;
  int tmp;
  ui->sessions_CB->addItem("---");
  for (int i=1; i<n; i++) {
    QString tstamp=csvlog.at(i).at(0)+QString(" ")+csvlog.at(i).at(1);
    if (csvlog.at(i).at(1).contains(".")) {
      tmp=QDateTime::fromString(tstamp, "yyyy-MM-dd HH:mm:ss.zzz").toTime_t();
      tmp+=csvlog.at(i).at(1).mid(csvlog.at(i).at(1).indexOf(".")).toDouble();
    } else {
      tmp=QDateTime::fromString(tstamp, "yyyy-MM-dd HH:mm:ss").toTime_t();
    }
    if (tmp>(lastvalue+60)) {
      ui->sessions_CB->addItem(tstamp, tmp);
      lastvalue=tmp;
    } else {
      lastvalue=tmp;
    }
  }
  plotLock=false;
  return true;
}


void logsDialog::on_sessions_CB_currentIndexChanged(int index)
{
  if (plotLock)
     return;
  plotLock=true;
  int start=0;
  int stop=-1;  
  if (index!=0) {
    start=ui->sessions_CB->itemData(index,Qt::UserRole).toInt();
    if (index<(ui->sessions_CB->count()-1)) {
      stop=ui->sessions_CB->itemData(index+1,Qt::UserRole).toInt();
    }
  }
//    if (ui->logTable->item(i-1,1)->isSelected()) {
  ui->logTable->clearSelection();
  if (start>0) {
    int n=csvlog.count();
    int tmp;
    ui->logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    for (int i=1; i<n; i++) {
      QString tstamp=csvlog.at(i).at(0)+QString(" ")+csvlog.at(i).at(1);
      if (csvlog.at(i).at(1).contains(".")) {
        tmp=QDateTime::fromString(tstamp, "yyyy-MM-dd HH:mm:ss.zzz").toTime_t();
        tmp+=csvlog.at(i).at(1).mid(csvlog.at(i).at(1).indexOf(".")).toDouble();
      } else {
        tmp=QDateTime::fromString(tstamp, "yyyy-MM-dd HH:mm:ss").toTime_t();
      }
      if (stop>0) {
        if (tmp==start) {
          ui->logTable->selectionModel()->select(ui->logTable->model()->index(i-1,0), QItemSelectionModel::SelectCurrent|QItemSelectionModel::Rows);
        } else if (tmp>start && tmp<stop) {
          ui->logTable->selectionModel()->select(ui->logTable->model()->index(i-1,0), QItemSelectionModel::Select|QItemSelectionModel::Rows);
        }
      } else {
        if (tmp==start) {
          ui->logTable->selectionModel()->select(ui->logTable->model()->index(i-1,0), QItemSelectionModel::SelectCurrent|QItemSelectionModel::Rows);
        } else if (tmp>start) {
          ui->logTable->selectionModel()->select(ui->logTable->model()->index(i-1,0), QItemSelectionModel::Select|QItemSelectionModel::Rows);
        }
      }
    }    
  }
  plotLock=false;
  plotLogs();
}

void logsDialog::plotLogs()
{
  if (plotLock)
    return;
  int n = csvlog.at(0).count(); // number of points in graph
  removeAllGraphs();
  int numplots=0;
  int plots=0;
  for (int i=0; i<n-2; i++) {
    if (ui->FieldsTW->item(0,i)->isSelected()) {
      numplots++;
    }
  }
  for (int i=0; i<n-2; i++) {
    if (ui->FieldsTW->item(0,i)->isSelected()) {
      plots++;
      plotValue(i+2,plots, numplots);
    }
  }
  ui->customPlot->legend->setVisible((numplots>2));
  ui->customPlot->replot();  
}

