#ifndef QAUTOCOMBOBOX_H_
#define QAUTOCOMBOBOX_H_

#include <QComboBox>
#include "modeledit/modeledit.h"

class QUnsignedAutoComboBox: public QComboBox
{
  Q_OBJECT

  public:
    explicit QUnsignedAutoComboBox(QWidget *parent = 0):
      QComboBox(parent),
      field(NULL),
      panel(NULL),
      lock(false)
    {
      connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    }

    void addItem(const QString & item, unsigned int value)
    {
      lock = true;
      QComboBox::addItem(item, value);
      if (field && *field==value)
        setCurrentIndex(count()-1);
      lock = false;
    }

    void setField(unsigned int * field, ModelPanel * panel=NULL)
    {
      this->field = field;
      this->panel = panel;
    }

  protected slots:
    void onCurrentIndexChanged(int index)
    {
      if (field && !lock) {
        *field = itemData(index).toInt();
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

#endif /* QAUTOCOMBOBOX_H_ */
