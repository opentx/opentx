#ifndef AFHDS3_H
#define AFHDS3_H

#include <QtCore>

class Afhds3Data
{
  Q_DECLARE_TR_FUNCTIONS(Afhds3)
public:
  static QString protocolToString(unsigned int protocol);
};
#endif // AFHDS3_H
