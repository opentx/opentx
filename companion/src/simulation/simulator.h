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

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QColor>
#include <QDataStream>
#include <QDebug>
#include <QPair>
#include <QString>

#define SIMULATOR_OPTIONS_VERSION    1

namespace Simulator {

typedef QPair<QString, QString> keymapHelp_t;

}

struct SimulatorOptions
{
    enum StartupDataTypes {
      START_WITH_FILE = 0,
      START_WITH_FOLDER,
      START_WITH_SDPATH,
      START_WITH_RAWDATA
    };

  private:
    quint16 _version = SIMULATOR_OPTIONS_VERSION;  // structure definition version

  public:
    // v1 fields
    quint8  startupDataType = START_WITH_FILE;
    QString firmwareId;
    QString dataFile;
    QString dataFolder;
    QString sdPath;
    QByteArray windowGeometry;
    QList<QByteArray> controlsState;  // saved switch/pot/stick settings
    QColor lcdColor;

    friend QDataStream & operator << (QDataStream &out, const SimulatorOptions & o)
    {
      out << quint16(SIMULATOR_OPTIONS_VERSION) << o.startupDataType << o.firmwareId << o.dataFile << o.dataFolder
          << o.sdPath << o.windowGeometry << o.controlsState << o.lcdColor;
      return out;
    }

    friend QDataStream & operator >> (QDataStream &in, SimulatorOptions & o)
    {
      if (o._version <= SIMULATOR_OPTIONS_VERSION) {
        in >> o._version >> o.startupDataType >> o.firmwareId >> o.dataFile >> o.dataFolder
           >> o.sdPath >> o.windowGeometry >> o.controlsState >> o.lcdColor;
      }
      return in;
    }

    friend QDebug operator << (QDebug d, const SimulatorOptions & o)
    {
      QDebugStateSaver saver(d);
      d.nospace() << "SimulatorOptions: firmwareId=" << o.firmwareId << "; dataFile=" << o.dataFile << "; dataFolder=" << o.dataFolder
                  << "; sdPath=" << o.sdPath << "; startupDataType=" << o.startupDataType;
      return d;
    }
};

Q_DECLARE_METATYPE(SimulatorOptions)


#endif // SIMULATOR_H
