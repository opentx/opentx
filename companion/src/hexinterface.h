/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 * 
 * Based on th9x -> http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef hex_stream_h
#define hex_stream_h

#include <inttypes.h>
#include <QTextStream>

class HexInterface {
  public:
    HexInterface(QTextStream &stream);

    int load(uint8_t *output, int maxsize);
    bool save(uint8_t *data, const int size);

  protected:

    int getValueFromLine(const QString &line, int pos, int len=2);
    QString iHEXLine(quint8 * data, quint32 addr, quint8 len);
    QString iHEXExtRec(quint8 bank);

    QTextStream & stream;
};

#endif
