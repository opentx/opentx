#ifndef AUTODOUBLESPINBOX_H_
#define AUTODOUBLESPINBOX_H_

#include <QDoubleSpinBox>
#include "modeledit/modeledit.h"
#if __GNUC__
  #include <math.h>
#else
  #include "../targets/windows/winbuild.h"
#endif

class AutoDoubleSpinBox: public QDoubleSpinBox
{
  Q_OBJECT

  public:
    explicit AutoDoubleSpinBox(QWidget *parent = 0):
      QDoubleSpinBox(parent),
      field(NULL),
      panel(NULL),
      lock(false)
    {
      connect(this, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
    }

    void setField(int & field, ModelPanel * panel=NULL)
    {
      this->field = &field;
      this->panel = panel;
      updateValue();
    }

    void setField(unsigned int & field, ModelPanel * panel=NULL)
    {
      this->field = (int *)&field;
      this->panel = panel;
      updateValue();
    }

    void updateValue()
    {
      if (field) {
        setValue(float(*field)/multiplier());
      }
    }

    void setDecimals(int prec)
    {
      QDoubleSpinBox::setDecimals(prec);
      updateValue();
    }

  protected:
    int multiplier()
    {
      switch (decimals()) {
        case 1:
          return 10;
        case 2:
          return 100;
        default:
          return 1;
       }
     }

  protected slots:
    void onValueChanged(double value)
    {
      if (field && !lock) {
        *field = round(value * multiplier());
        if (panel) {
          emit panel->modified();
        }
      }
    }

  protected:
    int * field;
    ModelPanel * panel;
    bool lock;
};

#endif /* AUTODOUBLESPINBOX_H_ */
