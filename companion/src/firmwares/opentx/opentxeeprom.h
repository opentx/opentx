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

#ifndef _OPENTXEEPROM_H_
#define _OPENTXEEPROM_H_

#include <inttypes.h>
#include "eeprominterface.h"
#include "eepromimportexport.h"
#include <qbytearray.h>

#define GVARS_VARIANT                  0x0001
#define FRSKY_VARIANT                  0x0002
#define MAVLINK_VARIANT                0x0008
#define M128_VARIANT                   0x8000
#define TARANIS_X9E_VARIANT            0x8000
#define TARANIS_X7_VARIANT             0x4000
#define TARANIS_XLITE_VARIANT          0x2000

#define SIMU_STOCK_VARIANTS            (GVARS_VARIANT|FRSKY_VARIANT)
#define SIMU_M128_VARIANTS             (M128_VARIANT|SIMU_STOCK_VARIANTS)

class OpenTxGeneralData: public TransformedField {
  public:
    OpenTxGeneralData(GeneralSettings & generalData, Board::Type board, unsigned int version, unsigned int variant=0);

    virtual const QString & getName() { return internalField.getName(); }

    QStringList errors()
    {
      return _errors;
    }

  protected:
    virtual void beforeExport();
    virtual void afterImport();

    virtual void setError(const QString & error)
    {
      qWarning() << error;
      _errors << error;
    }

    StructField internalField;
    GeneralSettings & generalData;
    Board::Type board;
    unsigned int version;
    int inputsCount;
    unsigned int chkSum;
    QStringList _errors;
};

class ProtocolsConversionTable: public ConversionTable
{
  public:
    ProtocolsConversionTable(Board::Type board)
    {
      int val = 0;
      if (IS_ARM(board)) {
        addConversion(PULSES_OFF, val++);
      }
      addConversion(PULSES_PPM, val++);
      if (!IS_ARM(board)) {
        addConversion(PULSES_PPM16, val++);
        addConversion(PULSES_PPMSIM, val++);
      }
      if (IS_ARM(board)) {
        addConversion(PULSES_PXX_XJT_X16, val);
        addConversion(PULSES_PXX_XJT_D8, val);
        addConversion(PULSES_PXX_XJT_LR12, val++);
      }
      else {
        addConversion(PULSES_PXX_DJT, val++);
      }
      if (IS_ARM(board)) {
        addConversion(PULSES_LP45, val);
        addConversion(PULSES_DSM2, val);
        addConversion(PULSES_DSMX, val++);
      }
      else {
        addConversion(PULSES_LP45, val++);
        addConversion(PULSES_DSM2, val++);
        addConversion(PULSES_DSMX, val++);
      }
      if (IS_ARM(board)) {
        addConversion(PULSES_CROSSFIRE, val++);
        addConversion(PULSES_MULTIMODULE, val++);
        addConversion(PULSES_PXX_R9M, val++);
        addConversion(PULSES_SBUS, val++);
      }
    }
};

class ChannelsConversionTable: public ConversionTable
{
  public:
    ChannelsConversionTable()
    {
      for (int i=4; i<=16; i+= 2)
        addConversion(i, -4+i/2);
    }
};


class OpenTxModelData: public TransformedField {
  public:
    OpenTxModelData(ModelData & modelData, Board::Type board, unsigned int version, unsigned int variant);

    virtual const QString & getName() { return internalField.getName(); }

    QStringList errors()
    {
      return _errors;
    }

  protected:
    virtual void beforeExport();
    virtual void afterImport();

    virtual void setError(const QString & error)
    {
      qWarning() << error;
      _errors << error;
    }

    StructField internalField;
    ModelData & modelData;
    Board::Type board;
    unsigned int version;
    unsigned int variant;
    char name[256];
    int subprotocols[CPN_MAX_MODULES+1/*trainer*/];
    ProtocolsConversionTable protocolsConversionTable;
    ChannelsConversionTable channelsConversionTable;
    QStringList _errors;
};

void OpenTxEepromCleanup(void);
#endif // _OPENTXEEPROM_H_
