#ifndef AUTOHEXSPINBOX_H_
#define AUTOHEXSPINBOX_H_

#include "hexspinbox.h"
#include "modeledit/modeledit.h"

class AutoHexSpinBox: public HexSpinBox
{
  Q_OBJECT

  public:
    explicit AutoHexSpinBox(QWidget *parent = 0):
      HexSpinBox(parent),
      field(NULL),
      panel(NULL),
      lock(false)
    {
      connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
    }

    void setField(unsigned int & field, ModelPanel * panel=NULL)
    {
      this->field = &field;
      this->panel = panel;
      updateValue();
    }

    void updateValue()
    {
      if (field) {
        setValue(*field);
      }
    }

  protected slots:
    void onValueChanged(int value)
    {
      if (field && !lock) {
        *field = value;
        if (panel) {
          emit panel->modified();
        }
      }
    }

  protected:
    unsigned int * field;
    ModelPanel * panel;
    bool lock;
};

#endif /* AUTOHEXSPINBOX_H_ */
