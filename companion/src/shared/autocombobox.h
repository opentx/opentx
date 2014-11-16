#ifndef AUTOCOMBOBOX_H_
#define AUTOCOMBOBOX_H_

#include <QComboBox>
#include "genericpanel.h"

class AutoComboBox: public QComboBox
{
  Q_OBJECT

  public:
    explicit AutoComboBox(QWidget *parent = 0):
      QComboBox(parent),
      field(NULL),
      panel(NULL),
      lock(false)
    {
      connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    }

    void clear()
    {
      lock = true;
      QComboBox::clear();
      lock = false;
    }

    void addItem(const QString & item, int value)
    {
      lock = true;
      QComboBox::addItem(item, value);
      if (field && *field==value) {
        setCurrentIndex(count()-1);
      }
      lock = false;
    }

    void setField(unsigned int & field, GenericPanel * panel=NULL)
    {
      this->field = (int *)&field;
      this->panel = panel;
      for (int i=0; i<count(); ++i) {
        setItemData(i, i);
        if ((int)field == i)
          setCurrentIndex(i);
      }
    }

    void setField(int & field, GenericPanel * panel=NULL)
    {
      this->field = &field;
      this->panel = panel;
      for (int i=0; i<count(); ++i) {
        setItemData(i, i);
        if ((int)field == i)
          setCurrentIndex(i);
      }
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
    int * field;
    GenericPanel * panel;
    bool lock;
};

#endif /* AUTOCOMBOBOX_H_ */
