#ifndef JOYSTICKDIALOG_H
#define JOYSTICKDIALOG_H

#include "joystick.h"
#include <QDialog>

namespace Ui {
    class JoystickDialog;
}

class JoystickDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoystickDialog(QWidget *parent = 0, int stick=-1);
    ~JoystickDialog();
    Joystick *joystick;

public slots:
    void onjoystickAxisValueChanged(int axis, int value);
    
private:
    Ui::JoystickDialog *ui;
    void joystickOpen(int stick);
    int jscal[8][3];
    int step;
    
private slots:
    void on_nextButton_clicked();
    void on_cancelButton_clicked();
    void on_okButton_clicked();
    
};

#endif // JOYSTICKDIALOG_H
