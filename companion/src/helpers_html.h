#ifndef HELPERS_HTML_H_
#define HELPERS_HTML_H_

#include <QString>

QString doTC(const QString &s, const QString &color="", bool bold=false);
QString doTR(const QString &s, const QString &color="", bool bold=false);
QString doTL(const QString &s, const QString &color="", bool bold=false);
QString fv(const QString &name, const QString &value, const QString &color="green");

#endif /* HELPERS_HTML_H_ */
