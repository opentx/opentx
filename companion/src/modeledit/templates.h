#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <QtWidgets>
#include "eeprominterface.h"

class Templates : public QWidget
{
    Q_OBJECT

  public:
    Templates(QWidget *parent, ModelData & model);
    ~Templates();

  private slots:
    void onDoubleClicked(QModelIndex index);

  private:
    void applyNumericTemplate(uint64_t tpl);
    ModelData & model;
};

#endif // TEMPLATES_H
