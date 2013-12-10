#ifndef SIMULATORDIALOG_H
#define SIMULATORDIALOG_H

#include <QDialog>
#include "node.h"
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
    class simulatorDialog;
}

class simulatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit simulatorDialog(QWidget *parent = 0);
    ~simulatorDialog();

    void loadParams(RadioData &radioData, const int model_idx=-1);

private:
    Ui::simulatorDialog *ui;
    Node *nodeLeft;
    Node *nodeRight;
    QTimer *timer;
    QString windowName;
    int backLight;
    bool lightOn;
    int switches;
#ifdef JOYSTICKS
    Joystick *joystick;
    int jscal[8][4];
    int jsmap[8];
#endif

    
    /* quint16 s_timeCumTot;
    quint16 s_timeCumAbs;
    quint16 s_timeCumSw;
    quint16 s_timeCumThr;
    quint16 s_timeCum16ThrP;
    quint8  s_timerState;
    quint8  beepAgain;
    quint16 g_LightOffCounter;
    qint16  s_timerVal;
    quint16 s_time;
    quint16 s_cnt;
    quint16 s_sum;
    quint8  sw_toggled; */

    EEPROMInterface *txInterface;
    SimulatorInterface *simulator;
    RadioData g_radioData;
    int g_modelIdx;
    
    void setupSticks();
    void setupTimer();
    void resizeEvent(QResizeEvent *event  = 0);

    void getValues();
    void setValues();
    void centerSticks();
    // void timerTick();

    bool keyState(EnumKeys key);
    int getValue(qint8 i);
    bool getSwitch(int swtch, bool nc, qint8 level=0);
    void setTrims();

    int beepVal;
    int beepShow;

protected:
    virtual void closeEvent(QCloseEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void wheelEvent(QWheelEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    static int screenshotIdx;
    static uint32_t switchstatus;
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
    void dialChanged();
#ifdef JOYSTICKS
    void onjoystickAxisValueChanged(int axis, int value);
#endif
 
};

#endif // SIMULATORDIALOG_H
