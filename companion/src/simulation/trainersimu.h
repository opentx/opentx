#ifndef trainersimu_h
#define trainersimu_h


#include <QShowEvent>
#include <QCloseEvent>
#include <QDialog>
#include <QTimer>
#include "modeledit/node.h"
#include "simulatorinterface.h"

namespace Ui {
  class TrainerSimulator;
}


class TrainerSimulator : public QDialog
{
  Q_OBJECT

  public:
    explicit TrainerSimulator(QWidget * parent, SimulatorInterface * simulator);
    virtual ~TrainerSimulator();


  protected:
    virtual void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent *event);

  private:
    Ui::TrainerSimulator * ui;
    QTimer * timer;
    SimulatorInterface *simulator;

    QGraphicsView * leftStick, * rightStick;
    Node *nodeLeft;
    Node *nodeRight;

    void centerSticks();
    void setupSticks();
    void resizeEvent(QResizeEvent *event  = 0);
    void setTrainerInputs();

  private slots:
    void onTimerEvent();

};

#endif

