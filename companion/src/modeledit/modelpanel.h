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
    void addLabel(QGridLayout * gridLayout, QString text, int col, bool mimimize=false);
    void addEmptyLabel(QGridLayout * gridLayout, int col);
    void addHSpring(QGridLayout *, int col, int row);
    void addVSpring(QGridLayout *, int col, int row);
    void addDoubleSpring(QGridLayout *, int col, int row);
};

#endif // CHANNELS_H
