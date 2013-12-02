#ifndef EXPODIALOG_H
#define EXPODIALOG_H

#include <QDialog>
#include "eeprominterface.h"

namespace Ui {
    class ExpoDialog;
}

class ExpoDialog : public QDialog {
    Q_OBJECT
public:
    ExpoDialog(QWidget *parent, ExpoData *mixdata, int stickMode);
    ~ExpoDialog();

protected:
    void changeEvent(QEvent *e);

private slots:
    void valuesChanged();
    void widgetChanged();
    void shrink();    

private:
    Ui::ExpoDialog *ui;
    ExpoData *ed;
};

#endif // EXPODIALOG_H
