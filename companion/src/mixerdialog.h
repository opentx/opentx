#ifndef MIXERDIALOG_H
#define MIXERDIALOG_H

#include <QDialog>
#include "eeprominterface.h"

namespace Ui {
    class MixerDialog;
}

class MixerDialog : public QDialog {
    Q_OBJECT
public:
    MixerDialog(QWidget *parent, MixData *mixdata, int stickMode);
    ~MixerDialog();

protected:
    void changeEvent(QEvent *e);

private slots:
    void widgetChanged();
    void valuesChanged();
    void shrink();


private:
    Ui::MixerDialog *ui;
    MixData *md;
};

#endif // MIXERDIALOG_H
