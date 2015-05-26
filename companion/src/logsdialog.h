#ifndef LOGSDIALOG_H
#define LOGSDIALOG_H

#include <QtCore>
#include <QtGui>
#include "qcustomplot/qcustomplot.h"

#define INVALID_MIN 999999
#define INVALID_MAX -999999

enum yaxes_t {
  firstLeft = 0,
  firstRight,
  secondLeft,
  secondRight,
  AXES_LIMIT // = 4
};

struct coords {
  QVector<double> x, y;
  double min_y;
  double max_y;
  yaxes_t yaxis;
  QString name;
};

struct minMax {
  double min;
  double max;
};

struct plotsCollection {
  QVarLengthArray<struct coords> coords;
  double min_x;
  double max_x;
  bool tooManyRanges;
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
  void titleDoubleClick(QMouseEvent *evt, QCPPlotTitle *title);
  void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
  void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
  void selectionChanged();
  void mousePress();
  void mouseWheel();
  void removeAllGraphs();
  void plotLogs();
  void on_fileOpen_BT_clicked();
  void on_sessions_CB_currentIndexChanged(int index);
  void on_mapsButton_clicked();
  void yAxisChangeRanges(QCPRange range);

private:
  QList<QStringList> csvlog;
  Ui::logsDialog *ui;
  QCPAxisRect *axisRect;
  QCPLegend *rightLegend;
  bool cvsFileParse();
  bool plotLock;
  QString logFilename;

  QVarLengthArray<Qt::GlobalColor> colors;
  QPen pen;

  double yAxesRatios[AXES_LIMIT];
  minMax yAxesRanges[AXES_LIMIT];
};

#endif // LOGSDIALOG_H
