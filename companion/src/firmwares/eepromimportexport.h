/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _EEPROMIMPORTEXPORT_H_
#define _EEPROMIMPORTEXPORT_H_

#include "customdebug.h"

#include <QtCore>
#include <QBitArray>

class DataField {
  Q_DECLARE_TR_FUNCTIONS(DataField)

  public:
    DataField(DataField * parent, const char * name=""):
      parent(parent),
      name(name)
    {
    }

    virtual ~DataField()
    {
    }

    virtual const QString & getName()
    {
      return name;
    }

    virtual unsigned int size() = 0;
    virtual void ExportBits(QBitArray & output) = 0;
    virtual void ImportBits(const QBitArray & input) = 0;

    QBitArray bytesToBits(QByteArray bytes)
    {
      QBitArray bits(bytes.count()*8);
      // Convert from QByteArray to QBitArray
      for (int i=0; i<bytes.count(); ++i)
        for (int b=0; b<8; ++b)
          bits.setBit(i*8+b, bytes.at(i)&(1<<b));
      return bits;
    }

    QByteArray bitsToBytes(QBitArray bits, int offset=0)
    {
      QByteArray bytes;
      bytes.resize((offset+bits.count()+7)/8);
      bytes.fill(0);
      // Convert from QBitArray to QByteArray
      for (int b=0; b<bits.count(); ++b)
        bytes[(b+offset)/8] = ( bytes.at((b+offset)/8) | ((bits[b]?1:0)<<((b+offset)%8)));
      return bytes;
    }

    int Export(QByteArray & output)
    {
      QBitArray result;
      ExportBits(result);
      output = bitsToBytes(result);
      return 0;
    }

    int Import(const QByteArray & input)
    {
      QBitArray bits = bytesToBits(input);
      if ((unsigned int)bits.size() < size()) {
        qDebug() << QString("Error importing %1: size to small %2/%3").arg(getName()).arg(input.size()).arg(size());
        return -1;
      }
      ImportBits(bits);
      return 0;
    }

    virtual int Dump(int level=0, int offset=0)
    {
      QBitArray bits;
      ExportBits(bits);
      QByteArray bytes = bitsToBytes(bits);
      int result = (offset+bits.count()) % 8;
      for (int i=0; i<level; i++) printf("  ");
      if (bits.count() % 8 == 0)
        printf("%s (%dbytes) ", getName().toLatin1().constData(), bytes.count());
      else
        printf("%s (%dbits) ", getName().toLatin1().constData(), bits.count());
      for (int i=0; i<bytes.count(); i++) {
        unsigned char c = bytes[i];
        if ((i==0 && offset) || (i==bytes.count()-1 && result!=0))
          printf("(%02x) ", c);
        else
          printf("%02x ", c);
      }
      printf("\n"); fflush(stdout);
      return result;
    }

  protected:
    virtual void setError(const QString & error)
    {
      if (parent) {
        parent->setError(error);
      }
      else {
        qWarning() << qPrintable(error);
      }
    }

    DataField * parent;
    QString name;
};

class ProxyField: public DataField {
  public:
    explicit ProxyField(DataField * parent):
      DataField(parent, "Proxy")
    {
    }

    virtual DataField * getField() = 0;

};

template<class container, int N>
class BaseUnsignedField: public DataField {
  public:
    BaseUnsignedField(DataField * parent, container & field):
      DataField(parent, "Unsigned"),
      field(field),
      min(0),
      max(std::numeric_limits<container>::max())
    {
    }

    BaseUnsignedField(DataField * parent, container & field, const char * name):
      DataField(parent, name),
      field(field),
      min(0),
      max(std::numeric_limits<container>::max())
    {
    }

    BaseUnsignedField(DataField * parent, container & field, container min, container max, const char * name="Unsigned"):
      DataField(parent, name),
      field(field),
      min(min),
      max(max)
    {
    }

    virtual void ExportBits(QBitArray & output)
    {
      container value = field;
      if (value > max) value = max;
      if (value < min) value = min;

      output.resize(N);
      for (int i=0; i<N; i++) {
        if (value & ((container)1<<i)) {
          output.setBit(i);
        }
      }
    }

    virtual void ImportBits(const QBitArray & input)
    {
      field = 0;
      for (int i=0; i<N; i++) {
        if (input[i])
          field |= ((container)1<<i);
      }
      qCDebug(eepromImport) << QString("\timported %1<%2>: 0x%3(%4)").arg(name).arg(N).arg(field, 0, 16).arg(field);
    }

    virtual unsigned int size()
    {
      return N;
    }

  protected:
    container & field;
    container min;
    container max;

  private:
    BaseUnsignedField();
};

template <int N>
class UnsignedField : public BaseUnsignedField<unsigned int, N>
{
  public:
    UnsignedField(DataField * parent, unsigned int & field):
      BaseUnsignedField<unsigned int, N>(parent, field)
    {
    }

    UnsignedField(DataField * parent, unsigned int & field, const char *name):
      BaseUnsignedField<unsigned int, N>(parent, field, name)
    {
    }

    UnsignedField(DataField * parent, unsigned int & field, unsigned int min, unsigned int max, const char *name="Unsigned"):
      BaseUnsignedField<unsigned int, N>(parent, field, min, max, name)
    {
    }
};

template<int N>
class BoolField: public DataField {
  public:
    explicit BoolField(DataField * parent, bool & field):
      DataField(parent, "Bool"),
      field(field)
    {
    }

    virtual void ExportBits(QBitArray & output)
    {
      output.resize(N);
      if (field) {
        output.setBit(0);
      }
    }

    virtual void ImportBits(const QBitArray & input)
    {
      field = input[0] ? true : false;
      qCDebug(eepromImport) << QString("\timported %1<%2>: 0x%3(%4)").arg(name).arg(N).arg(field, 0, 16).arg(field);
    }

    virtual unsigned int size()
    {
      return N;
    }

  protected:
    bool & field;

  private:
    BoolField();
};

template<int N>
class SignedField: public DataField {
  public:
    SignedField(DataField * parent, int & field):
      DataField(parent, "Signed"),
      field(field),
      min(INT_MIN),
      max(INT_MAX)
    {
    }

    SignedField(DataField * parent, int & field, const char *name):
      DataField(parent, name),
      field(field),
      min(INT_MIN),
      max(INT_MAX)
    {
    }

    SignedField(DataField * parent, int & field, int min, int max, const char *name="Signed"):
      DataField(parent, name),
      field(field),
      min(min),
      max(max)
    {
    }

    virtual void ExportBits(QBitArray & output)
    {
      int value = field;
      if (value > max) value = max;
      if (value < min) value = min;

      output.resize(N);
      for (int i=0; i<N; i++) {
        if (((unsigned int)value) & (1<<i))
          output.setBit(i);
      }
    }

    virtual void ImportBits(const QBitArray & input)
    {
      unsigned int value = 0;
      for (int i=0; i<N; i++) {
        if (input[i])
          value |= (1<<i);
      }

      if (input[N-1]) {
        for (unsigned int i=N; i<8*sizeof(int); i++) {
          value |= (1<<i);
        }
      }

      field = (int)value;
      qCDebug(eepromImport) << QString("\timported %1<%2>: 0x%3(%4)").arg(name).arg(N).arg(field, 0, 16).arg(field);
    }

    virtual unsigned int size()
    {
      return N;
    }

  protected:
    int & field;
    int min;
    int max;
};

template<int N>
class SpareBitsField: public UnsignedField<N> {
  public:
    SpareBitsField(DataField * parent):
      UnsignedField<N>(parent, spare, 0, 0, "Spare"),
      spare(0)
    {
    }
  protected:
    unsigned int spare;
};

template<int N>
class CharField: public DataField {
  public:
    CharField(DataField * parent, char * field, bool truncate=true, const char * name="Char"):
      DataField(parent, name),
      field(field),
      truncate(truncate)
    {
    }

    virtual void ExportBits(QBitArray & output)
    {
      output.resize(N*8);
      int b = 0;
      int len = truncate ? strlen(field) : N;
      for (int i=0; i<N; i++) {
        int idx = (i>=len ? 0 : field[i]);
        for (int j=0; j<8; j++, b++) {
          if (idx & (1<<j))
            output.setBit(b);
        }
      }
    }

    virtual void ImportBits(const QBitArray & input)
    {
      unsigned int b = 0;
      for (int i=0; i<N; i++) {
        int8_t idx = 0;
        for (int j=0; j<8; j++) {
          if (input[b++])
            idx |= (1<<j);
        }
        field[i] = idx;
      }
      qCDebug(eepromImport) << QString("\timported %1<%2>: '%3'").arg(name).arg(N).arg(field);
    }

    virtual unsigned int size()
    {
      return 8*N;
    }

  protected:
    char * field;
    bool truncate;
};

static const char specialCharsTab[] = "_-.,";
static inline int8_t char2idx(char c)
{
  if (c==' ') return 0;
  if (c>='A' && c<='Z') return 1+c-'A';
  if (c>='a' && c<='z') return -1-c+'a';
  if (c>='0' && c<='9') return 27+c-'0';
  for (int8_t i=0;;i++) {
    char cc = specialCharsTab[i];
    if (cc==0) return 0;
    if (cc==c) return 37+i;
  }
}

#define ZCHAR_MAX 40
static inline char idx2char(int8_t idx)
{
  if (idx == 0) return ' ';
  if (idx < 0) {
    if (idx > -27) return 'a' - idx - 1;
    idx = -idx;
  }
  if (idx < 27) return 'A' + idx - 1;
  if (idx < 37) return '0' + idx - 27;
  if (idx <= ZCHAR_MAX) return specialCharsTab[idx-37];
  return ' ';
}

template<int N>
class ZCharField: public DataField {
  public:
    ZCharField(DataField * parent, char * field, const char * name = "ZChar"):
      DataField(parent, name),
      field(field)
    {
    }

    virtual void ExportBits(QBitArray & output)
    {
      output.resize(N*8);
      int b = 0;
      int len = strlen(field);
      for (int i=0; i<N; i++) {
        int idx = i>=len ? 0 : char2idx(field[i]);
        for (int j=0; j<8; j++, b++) {
          if (idx & (1<<j))
            output.setBit(b);
        }
      }
    }

    virtual void ImportBits(const QBitArray & input)
    {
      unsigned int b = 0;
      for (int i=0; i<N; i++) {
        int8_t idx = 0;
        for (int j=0; j<8; j++) {
          if (input[b++])
            idx |= (1<<j);
        }
        field[i] = idx2char(idx);
      }

      field[N] = '\0';
      for (int i=N-1; i>=0; i--) {
        if (field[i] == ' ')
          field[i] = '\0';
        else
          break;
      }
      qCDebug(eepromImport) << QString("\timported %1<%2>: '%3'").arg(name).arg(N).arg(field);
    }

    virtual unsigned int size()
    {
      return 8*N;
    }

  protected:
    char * field;
};

class StructField: public DataField {
  public:
    StructField(DataField * parent, const char * name="Struct"):
      DataField(parent, name)
    {
    }

    ~StructField() {
      foreach(DataField * field, fields) {
        delete field;
      }
    }

    inline void Append(DataField * field) {
      //qCDebug(eepromImport) << QString("StructField(%1) appending field: %2").arg(name).arg(field->getName());
      fields.append(field);
    }

    virtual void ExportBits(QBitArray & output)
    {
      int offset = 0;
      output.resize(size());
      foreach(DataField *field, fields) {
        QBitArray bits;
        field->ExportBits(bits);
        for (int i=0; i<bits.size(); i++)
          output[offset++] = bits[i];
      }
    }

    virtual void ImportBits(const QBitArray & input)
    {
      qCDebug(eepromImport) << QString("\timporting %1[%2]:").arg(name).arg(fields.size());
      int offset = 0;
      foreach(DataField *field, fields) {
        unsigned int size = field->size();
        QBitArray bits(size);
        for (unsigned int i=0; i<size; i++) {
          bits[i] = input[offset++];
        }
        field->ImportBits(bits);
      }
    }

    virtual unsigned int size()
    {
      unsigned int result = 0;
      foreach(DataField *field, fields) {
        result += field->size();
      }
      return result;
    }

    virtual int Dump(int level=0, int offset=0)
    {
      for (int i=0; i<level; i++) printf("  ");
      printf("%s (%d bytes)\n", getName().toLatin1().constData(), size()/8);
      foreach(DataField *field, fields) {
        offset = field->Dump(level+1, offset);
      }
      return offset;
    }

  protected:
    QList<DataField *> fields;
};

class TransformedField: public DataField {
  public:
    TransformedField(DataField * parent, DataField & field):
      DataField(parent),
      field(field)
    {
    }

    virtual ~TransformedField()
    {
    }

    virtual void ExportBits(QBitArray & output)
    {
      beforeExport();
      field.ExportBits(output);
    }

    virtual void ImportBits(const QBitArray & input)
    {
      qCDebug(eepromImport) << QString("\timporting TransformedField %1:").arg(field.getName());
      field.ImportBits(input);
      afterImport();
    }


    virtual const QString & getName()
    {
      return field.getName();
    }

    virtual unsigned int size()
    {
      return field.size();
    }

    virtual void beforeExport() = 0;

    virtual void afterImport() = 0;

    virtual int Dump(int level=0, int offset=0)
    {
      beforeExport();
      return field.Dump(level, offset);
    }

  protected:
    DataField & field;
};

class ConversionTable {

  public:
    bool exportValue(const int before, int &after)
    {
      after = 0;

      for (std::list<ConversionTuple>::iterator it=exportTable.begin(); it!=exportTable.end(); it++) {
        ConversionTuple & tuple = *it;
        if (before == tuple.a) {
          after = tuple.b;
          return true;
        }
      }

      return false;
    }

    bool importValue(const int before, int &after)
    {
      after = 0;

      for (std::list<ConversionTuple>::iterator it=importTable.begin(); it!=importTable.end(); it++) {
        ConversionTuple & tuple = *it;
        if (before == tuple.b) {
          after = tuple.a;
          return true;
        }
      }

      return false;
    }

  protected:

    class ConversionTuple {
      public:
        ConversionTuple(const int a, const int b):
          a(a),
          b(b)
        {
        }

        int a;
        int b;
    };

    void addConversion(const int a, const int b)
    {
      ConversionTuple conversion(a, b);
      importTable.push_back(conversion);
      exportTable.push_back(conversion);
    }

    void addImportConversion(const int a, const int b)
    {
      importTable.push_back(ConversionTuple(a, b));
    }

    void addExportConversion(const int a, const int b)
    {
      exportTable.push_back(ConversionTuple(a, b));
    }

    std::list<ConversionTuple> importTable;
    std::list<ConversionTuple> exportTable;
};

template<class T>
class ConversionField: public TransformedField {
  public:
    ConversionField(DataField * parent, int & field, ConversionTable *table, const char *name, const QString & error = ""):
      TransformedField(parent, internalField),
      internalField(this, _field, name),
      field(field),
      _field(0),
      table(table),
      shift(0),
      scale(1),
      min(INT_MIN),
      max(INT_MAX),
      exportFunc(NULL),
      importFunc(NULL),
      error(error)
    {
    }

    ConversionField(DataField * parent, unsigned int & field, ConversionTable *table, const char *name, const QString & error = ""):
      TransformedField(parent, internalField),
      internalField(this, (unsigned int &)_field, name),
      field((int &)field),
      _field(0),
      table(table),
      shift(0),
      scale(0),
      min(INT_MIN),
      max(INT_MAX),
      exportFunc(NULL),
      importFunc(NULL),
      error(error)
    {
    }

    ConversionField(DataField * parent, int & field, int (*exportFunc)(int), int (*importFunc)(int)):
      TransformedField(parent, internalField),
      internalField(this, _field),
      field(field),
      _field(0),
      table(NULL),
      shift(0),
      scale(0),
      min(INT_MIN),
      max(INT_MAX),
      exportFunc(exportFunc),
      importFunc(importFunc),
      error("")
    {
    }

    ConversionField(DataField * parent, int & field, int shift, int scale=0, int min=INT_MIN, int max=INT_MAX, const char *name = "Signed shifted"):
      TransformedField(parent, internalField),
      internalField(this, _field, name),
      field(field),
      _field(0),
      table(NULL),
      shift(shift),
      scale(scale),
      min(min),
      max(max),
      exportFunc(NULL),
      importFunc(NULL),
      error("")
    {
    }

    ConversionField(DataField * parent, unsigned int & field, int shift, int scale=0):
      TransformedField(parent, internalField),
      internalField(this, (unsigned int &)_field),
      field((int &)field),
      _field(0),
      table(NULL),
      shift(shift),
      scale(scale),
      min(INT_MIN),
      max(INT_MAX),
      exportFunc(NULL),
      importFunc(NULL),
      error("")
    {
    }

    virtual void beforeExport()
    {
      _field = field;

      if (scale) {
        _field /= scale;
      }

      if (table) {
        if (!table->exportValue(_field, _field)) {
          setError(error.isEmpty() ? DataField::tr("Conversion error on field %1").arg(name) : error);
        }
        return;
      }

      if (shift) {
        if (_field < min)
          _field = min + shift;
        else if (_field > max)
          _field = max + shift;
        else
          _field += shift;
      }

      if (exportFunc) {
        _field = exportFunc(_field);
      }
    }

    virtual void afterImport()
    {
      field = _field;

      if (table) {
        if (table->importValue(field, field))
          return;
      }

      if (shift) {
        field -= shift;
      }

      if (importFunc) {
        field = importFunc(field);
      }

      if (scale) {
        field *= scale;
      }
      qCDebug(eepromImport) << QString("\timported ConversionField<%1>:").arg(internalField.getName()) << QString(" before: %1, after: %2").arg(_field).arg(field);
    }

  protected:
    T internalField;
    int & field;
    int _field;
    ConversionTable * table;
    int shift;
    int scale;
    int min;
    int max;
    int (*exportFunc)(int);
    int (*importFunc)(int);
    const QString error;
};

#endif // _EEPROMIMPORTEXPORT_H_
