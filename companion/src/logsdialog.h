#ifndef LOGSDIALOG_H
#define LOGSDIALOG_H

#include <QtCore>
#include <QtGui>
#include "qcustomplot.h"

struct coords {
  QVector<double> x, y;
  double min_y;
  double max_y;
  bool secondRange;
  QString name;
};

struct plotsCollection {
  QVarLengthArray<struct coords> coords;
  double min_x;
  double max_x;
  bool twoRanges;
  double rangeOneMin;
  double rangeOneMax;
  double rangeTwoMin;
  double rangeTwoMax;
};

namespace Ui {
    class logsDialog;
}

class logsDialog : public QDialog
{
    Q_OBJECT

public:
  explicit logsDialog(QWidget *parent = 0);
  ~logsDialog();

private slots:
  void titleDoubleClick();
  void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
  void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
  void selectionChanged();
  void mousePress();
  void mouseWheel();
  void removeSelectedGraph();
  void removeAllGraphs();
  void moveLegend();
  void plotLogs();
  void plotValue(int index, int plot, int numplots);
  void plottableItemDoubleClick(QCPAbstractPlottable *  plottable, QMouseEvent * event);
  // void graphClicked(QCPAbstractPlottable *plottable);
  void on_fileOpen_BT_clicked();
  void on_sessions_CB_currentIndexChanged(int index);
  void on_mapsButton_clicked();
  void setRangeyAxis2(QCPRange range);

private:
  QList<QStringList> csvlog;
  Ui::logsDialog *ui;
  bool cvsFileParse();
  double GetScale(QString channel);
  QList<QColor> palette;
  bool plotLock;
  QString logFilename;

  double rangeRatio;
  double rangeyAxisMin;
  double rangeyAxisMax;
  double rangeyAxis2Min;
  double rangeyAxis2Max;
  bool newPlotLogs();
};

#endif // LOGSDIALOG_H
