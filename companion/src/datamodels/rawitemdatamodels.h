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

#ifndef RAWITEMDATAMODELS_H
#define RAWITEMDATAMODELS_H

#include "rawsource.h"
#include "rawswitch.h"

#include <QStandardItemModel>

class GeneralSettings;
class ModelData;

class AbstractRawItemDataModel: public QStandardItemModel
{
    Q_OBJECT
  public:
    enum DataRoles { ItemIdRole = Qt::UserRole, ItemTypeRole, ItemFlagsRole, IsAvailableRole };
    Q_ENUM(DataRoles)

    enum DataGroups {
      NoneGroup     = 0x01,
      NegativeGroup = 0x02,
      PositiveGroup = 0x04
    };
    Q_ENUM(DataGroups)

    explicit AbstractRawItemDataModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent = nullptr)  :
      QStandardItemModel(parent),
      generalSettings(generalSettings),
      modelData(modelData)
    {}

  public slots:
    virtual void update() = 0;

  signals:
    void dataAboutToBeUpdated();
    void dataUpdateComplete();

  protected:
    const GeneralSettings * generalSettings;
    const ModelData * modelData;
};


class RawSourceItemModel: public AbstractRawItemDataModel
{
    Q_OBJECT
  public:
    explicit RawSourceItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent = nullptr);

  public slots:
    void update() override;

  protected:
    void setDynamicItemData(QStandardItem * item, const RawSource & src) const;
    void addItems(const RawSourceType & type, const int group, const int count, const int start = 0);
};


class RawSwitchItemModel: public AbstractRawItemDataModel
{
    Q_OBJECT
  public:
    explicit RawSwitchItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent = nullptr);

  public slots:
    void update() override;

  protected:
    void setDynamicItemData(QStandardItem * item, const RawSwitch & rsw) const;
    void addItems(const RawSwitchType & type, int count);
};


class CurveItemModel: public AbstractRawItemDataModel
{
    Q_OBJECT
  public:
    explicit CurveItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent = nullptr);

  public slots:
    void update() override;

  protected:
    void setDynamicItemData(QStandardItem * item, int index) const;
};


class CommonItemModels: public QObject
{
    Q_OBJECT
  public:
    enum RadioModelObjects {
      RMO_CHANNELS,
      RMO_CURVES,
      RMO_FLIGHT_MODES,
      RMO_GLOBAL_VARIABLES,
      RMO_INPUTS,
      RMO_LOGICAL_SWITCHES,
      RMO_SCRIPTS,
      RMO_TELEMETRY_SENSORS,
      RMO_TIMERS
    };
    Q_ENUM(RadioModelObjects)

    explicit CommonItemModels(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent = nullptr);
    ~CommonItemModels();

    void update(const RadioModelObjects radioModelObjects);
    RawSourceItemModel * rawSourceItemModel() const { return m_rawSourceItemModel; }
    RawSwitchItemModel * rawSwitchItemModel() const { return m_rawSwitchItemModel; }
    CurveItemModel * curveItemModel() const { return m_curveItemModel; }

  private:
    RawSourceItemModel *m_rawSourceItemModel;
    RawSwitchItemModel *m_rawSwitchItemModel;
    CurveItemModel *m_curveItemModel;
};

#endif // RAWITEMDATAMODELS_H
