#ifndef _LOGSDIALOG_H_
#define _LOGSDIALOG_H_

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
  class LogsDialog;
}

class LogsDialog : public QDialog
{
    Q_OBJECT

public:
  explicit LogsDialog(QWidget *parent = 0);
  ~LogsDialog();

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
  Ui::LogsDialog *ui;
  QCPAxisRect *axisRect;
  QCPLegend *rightLegend;
  bool plotLock;
  QString logFilename;

  QVarLengthArray<Qt::GlobalColor> colors;
  QPen pen;

  double yAxesRatios[AXES_LIMIT];
  minMax yAxesRanges[AXES_LIMIT];

  bool cvsFileParse();
  QList<QStringList> filterGePoints(const QList<QStringList> & input);
  void exportToGoogleEarth();
  QDateTime getRecordTimeStamp(int index);
  QString generateDuration(const QDateTime & start, const QDateTime & end);
  void setFlightSessions();

};

#endif // _LOGSDIALOG_H_
