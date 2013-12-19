#ifndef MODELPANEL_H
#define MODELPANEL_H

#include <QWidget>
#include <QGridLayout>
#include "eeprominterface.h"

class ModelPanel : public QWidget
{
    Q_OBJECT

  public:
    ModelPanel(QWidget *parent, ModelData & model);
    virtual ~ModelPanel();

  signals:
    void modified();

  public slots:
    virtual void update();

  protected:
    ModelData & model;
    bool lock;
    void addLabel(QGridLayout * gridLayout, QString text, int col);

};

#endif // CHANNELS_H
