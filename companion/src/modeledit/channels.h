#ifndef CHANNELS_H
#define CHANNELS_H

#include "modelpanel.h"
#include <QSpinBox>

class Channels : public ModelPanel
{
    Q_OBJECT

  public:
    Channels(QWidget *parent, ModelData & model);
    ~Channels();
    
  private:
    QVector<QDoubleSpinBox *> minSpins;
    QVector<QDoubleSpinBox *> maxSpins;

  public slots:
    void refreshExtendedLimits();

  private slots:
    void symlimitsEdited();
    void nameEdited();
    void offsetEdited();
    void minEdited();
    void maxEdited();
    void invEdited();
    void ppmcenterEdited();

};

#endif // CHANNELS_H
