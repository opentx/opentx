#ifndef AUTOCHECKBOX_H_
#define AUTOCHECKBOX_H_

#include <QCheckBox>
#include "modeledit/modeledit.h"

class AutoCheckBox: public QCheckBox
{
  Q_OBJECT

  public:
    explicit AutoCheckBox(QWidget *parent = 0):
      QCheckBox(parent),
      field(NULL),
      panel(NULL),
      lock(false)
    {
      connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
    }

    void setField(bool & field, ModelPanel * panel=NULL)
    {
      this->field = &field;
      this->panel = panel;
      updateValue();
    }

    void updateValue()
    {
      lock = true;
      if (field) {
        setChecked(*field);
      }
      lock = false;
    }

  protected slots:
    void onToggled(bool checked)
    {
      if (field && !lock) {
        *field = checked;
        if (panel) {
          emit panel->modified();
        }
      }
    }

  protected:
    bool * field;
    ModelPanel * panel;
    bool lock;
};

#endif /* AUTOCHECKBOX_H_ */
