#include "hexspinbox.h"

HexSpinBox::HexSpinBox(QWidget *parent):
  QSpinBox(parent)
{
  validator = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{1,8}"), this);
}

QValidator::State HexSpinBox::validate(QString &text, int &pos) const
{
  return validator->validate(text, pos);
}

int HexSpinBox::valueFromText(const QString &text) const
{
  return text.toInt(0, 16);
}

QString HexSpinBox::textFromValue(int value) const
{
  QString text;
  text.sprintf("%04X", value);
  return text;
}



