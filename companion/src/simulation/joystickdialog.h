#ifndef JOYSTICKDIALOG_H
#define JOYSTICKDIALOG_H

#include <QtWidgets>
#include "joystick.h"

namespace Ui {
    class joystickDialog;
}

class joystickDialog : public QDialog
{
    Q_OBJECT

public:
    explicit joystickDialog(QWidget *parent = 0, int stick=-1);
    ~joystickDialog();
    Joystick *joystick;

public slots:
    void onjoystickAxisValueChanged(int axis, int value);
    
private:
    Ui::joystickDialog *ui;
    void joystickOpen(int stick);
    int jscal[8][3];
    int step;
    
private slots:
    void on_nextButton_clicked();
    void on_cancelButton_clicked();
    void on_okButton_clicked();
    
};

#endif // JOYSTICKDIALOG_H
