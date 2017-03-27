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

#ifndef _SIMULATORINTERFACE_H_
#define _SIMULATORINTERFACE_H_

#include "boards.h"
#include "constants.h"

#include <algorithm>
#include <inttypes.h>

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDir>
#include <QLibrary>
#include <QMap>

struct TxInputs
{
    int  sticks[CPN_MAX_STICKS]; /* lh lv rv rh */
    int  pots[CPN_MAX_POTS];
    int  switches[CPN_MAX_SWITCHES];
    bool keys[CPN_MAX_KEYS];
    bool rotenc;
    bool trims[CPN_MAX_TRIM_SW];
};

class TxOutputs
{
  public:
    TxOutputs() { memset(this, 0, sizeof(TxOutputs)); }
    int chans[CPN_MAX_CHNOUT];
    bool vsw[CPN_MAX_LOGICAL_SWITCHES];
    int gvars[CPN_MAX_FLIGHT_MODES][CPN_MAX_GVARS];
    unsigned int beep;
    // uint8_t phase;
};

struct Trims
{
  int values[CPN_MAX_TRIMS]; /* lh lv rv rh t5 t6 */
  bool extended;
};

class SimulatorInterface
{
  public:

    virtual ~SimulatorInterface() {}

    virtual void setSdPath(const QString & sdPath = "", const QString & settingsPath = "") { }

    virtual void setVolumeGain(int value) { }

    virtual void start(QByteArray & eeprom, bool tests=true) = 0;

    virtual void start(const char * filename, bool tests=true) = 0;

    virtual void stop() = 0;

    virtual void readEepromData(QByteArray & dest) = 0;

    virtual bool timer10ms() = 0;

    virtual uint8_t * getLcd() = 0;

    virtual bool lcdChanged(bool &lightEnable) = 0;

    virtual void setValues(TxInputs &inputs) = 0;

    virtual void getValues(TxOutputs &outputs) = 0;

    virtual void setTrim(unsigned int idx, int value) = 0;

    virtual void getTrims(Trims &trims) = 0;

    virtual unsigned int getPhase() = 0;

    virtual const char * getPhaseName(unsigned int phase) = 0;

    virtual void wheelEvent(int steps) { }

    virtual const char * getError() = 0;

    virtual void sendTelemetry(uint8_t * data, unsigned int len) = 0;

    virtual uint8_t getSensorInstance(uint16_t id, uint8_t defaultValue = 0) = 0;

    virtual uint16_t getSensorRatio(uint16_t id) = 0;

    virtual void setTrainerInput(unsigned int inputNumber, int16_t value) = 0;

    virtual void installTraceHook(void (*callback)(const char *)) = 0;

    virtual void setLuaStateReloadPermanentScripts() = 0;
};

class SimulatorFactory {

  public:

    virtual ~SimulatorFactory() { }

    virtual QString name() = 0;

    virtual Board::Type type() = 0;

    virtual SimulatorInterface *create() = 0;
};

class SimulatorLoader
{
  public:
    static void registerSimulators();
    static void unregisterSimulators();
    static QStringList getAvailableSimulators();
    static QString findSimulatorByFirmwareName(const QString & name);
    static SimulatorInterface * loadSimulator(const QString & name);
    static bool unloadSimulator(const QString & name);

  protected:
    typedef SimulatorFactory * (*RegisterSimulator)();

    static int registerSimulators(const QDir & dir);
    static QMap<QString, QLibrary *> registeredSimulators;
};

#endif // _SIMULATORINTERFACE_H_
