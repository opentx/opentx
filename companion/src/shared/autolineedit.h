#ifndef AUTOLINEEDIT_H_
#define AUTOLINEEDIT_H_

#include <QLineEdit>
#include <QRegExpValidator>
#include "genericpanel.h"

#define CHAR_FOR_NAMES_REGEX "[ A-Za-z0-9_.-,]*"

class AutoLineEdit: public QLineEdit
{
  Q_OBJECT

  public:
    explicit AutoLineEdit(QWidget *parent = 0):
      QLineEdit(parent),
      field(NULL),
      panel(NULL),
      lock(false)
    {
      QRegExp rx(CHAR_FOR_NAMES_REGEX);
      setValidator(new QRegExpValidator(rx, this));
      connect(this, SIGNAL(editingFinished()), this, SLOT(onEdited()));
    }

    void setField(char * field, int len, GenericPanel * panel=NULL)
    {
      this->field = field;
      this->panel = panel;
      setMaxLength(len);
      updateValue();
    }

    void updateValue()
    {
      lock = true;
      if (field) {
        setText(field);
      }
      lock = false;
    }

  protected slots:
    void onEdited()
    {
      if (field && !lock) {
        strcpy(field, text().toLatin1());
        if (panel) {
          emit panel->modified();
        }
      }
    }

  protected:
    char * field;
    GenericPanel * panel;
    bool lock;
};

#endif /* AUTOLINEEDIT_H_ */
