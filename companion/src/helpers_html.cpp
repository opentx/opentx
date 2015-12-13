#include "helpers_html.h"

QString tdAlign(const QString &s, const QString &align, const QString &color, bool bold)
{
  QString str = s;
  if (bold) str = "<b>" + str + "</b>";
  if (!color.isEmpty()) str = "<font color=" + color + ">" + str + "</font>";
  return "<td align=" + align + ">" + str + "</td>";
}

QString doTC(const QString &s, const QString &color, bool bold)
{
  return tdAlign(s, "center", color, bold);
}

QString doTR(const QString &s, const QString &color, bool bold)
{
  return tdAlign(s, "right", color, bold);
}

QString doTL(const QString &s, const QString &color, bool bold)
{
  return tdAlign(s, "left", color, bold);
}

QString fv(const QString &name, const QString &value, const QString &color)
{
  return "<b>" + name + ": </b><font color=" +color + ">" + value + "</font><br>";
}


