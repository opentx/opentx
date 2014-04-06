#ifndef SIMULATORDIALOG_H
#define SIMULATORDIALOG_H

#include <QDialog>
#include "modeledit/node.h"
#include "eeprominterface.h"

#ifdef JOYSTICKS
#include "joystick.h"
#endif

#define TMR_OFF     0
#define TMR_RUNNING 1
#define TMR_BEEPING 2
#define TMR_STOPPED 3

#define FLASH_DURATION 10

namespace Ui {
  class SimulatorDialog9X;
  class SimulatorDialogTaranis;
}

// TODO rename + move?
class lcdWidget;
class mySlider;

#define SIMULATOR_FLAGS_NOTX            1
#define SIMULATOR_FLAGS_STICK_MODE_LEFT 2

class SimulatorDialog : public QDialog
{
  Q_OBJECT

  public:
    explicit SimulatorDialog(QWidget * parent = NULL, unsigned int flags=0);
    virtual ~SimulatorDialog();

    void start(const char * filename);
    void start(QByteArray & eeprom);

  protected:
    template <class T> void initUi(T * ui);
    virtual void setLightOn(bool enable) { }
    virtual void updateBeepButton() { }

    unsigned int flags;
    lcdWidget * lcd;
    QGraphicsView * leftStick, * rightStick;
    QDial * dialP_1, * dialP_2, * dialP_3, * dialP_4;
    mySlider * trimHLeft, * trimVLeft, * trimHRight, * trimVRight;
    QLabel * leftXPerc, * rightXPerc, * leftYPerc, * rightYPerc;
    QTabWidget * tabWidget;
    QGridLayout * logicalSwitchesLayout;
    QGridLayout * channelsLayout;
    QVector<QLabel *> logicalSwitchLabels;
    QVector<QSlider *> channelSliders;
    QVector<QLabel *> channelValues;

    void init();
    Node *nodeLeft;
    Node *nodeRight;
    QTimer *timer;
    QString windowName;
    unsigned int backLight;
    bool lightOn;
    int switches;
#ifdef JOYSTICKS
    Joystick *joystick;
    int jscal[8][4];
    int jsmap[8];
#endif

    EEPROMInterface *txInterface;
    SimulatorInterface *simulator;

    void setupSticks();
    void setupTimer();
    void resizeEvent(QResizeEvent *event  = 0);

    virtual void getValues() = 0;
    void setValues();
    void centerSticks();
    // void timerTick();

    bool keyState(EnumKeys key);
    int getValue(qint8 i);
    bool getSwitch(int swtch, bool nc, qint8 level=0);
    void setTrims();

    int beepVal;

    int lcdWidth;
    int lcdDepth;

  protected:
    virtual void closeEvent(QCloseEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void wheelEvent(QWheelEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    static int screenshotIdx;
    int buttonPressed;
    bool middleButtonPressed;

  private slots:
    void onButtonPressed(int value);
    void on_FixRightY_clicked(bool checked);
    void on_FixRightX_clicked(bool checked);
    void on_FixLeftY_clicked(bool checked);
    void on_FixLeftX_clicked(bool checked);
    void on_holdRightY_clicked(bool checked);
    void on_holdRightX_clicked(bool checked);
    void on_holdLeftY_clicked(bool checked);
    void on_holdLeftX_clicked(bool checked);
    void on_trimHLeft_valueChanged(int);
    void on_trimVLeft_valueChanged(int);
    void on_trimHRight_valueChanged(int);
    void on_trimVRight_valueChanged(int);
    void onTimerEvent();

#ifdef JOYSTICKS
    void onjoystickAxisValueChanged(int axis, int value);
#endif
 
};

class SimulatorDialog9X: public SimulatorDialog
{
  Q_OBJECT

  public:
    explicit SimulatorDialog9X(QWidget * parent = NULL, unsigned int flags=0);
    virtual ~SimulatorDialog9X();

  protected:
    virtual void getValues();
    virtual void setLightOn(bool enable);
    virtual void updateBeepButton();

  private slots:
    void dialChanged();

  private:
    Ui::SimulatorDialog9X * ui;
    int beepShow;

};

class SimulatorDialogTaranis: public SimulatorDialog
{
  Q_OBJECT

  public:
    explicit SimulatorDialogTaranis(QWidget * parent = NULL, unsigned int flags=0);
    virtual ~SimulatorDialogTaranis();

  protected:
    int trimPressed;
    virtual void getValues();

  private:
    Ui::SimulatorDialogTaranis * ui;

  private slots:
    void onTrimPressed();
    void onTrimReleased();
};

#endif // SIMULATORDIALOG_H
