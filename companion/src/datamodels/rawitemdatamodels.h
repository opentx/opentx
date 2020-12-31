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
class AdjustmentReference;

class AbstractItemModel: public QStandardItemModel
{
    Q_OBJECT
  public:
    enum ItemModelId {
      UnknownId,
      RawSourceId,
      RawSwitchId,
      CurveId,
      GVarRefId,
      ThrSourceId,
      CustomFuncActionId,
      CustomFuncResetParamId,
      TeleSourceId,
      CurveRefTypeId,
      CurveRefFuncId
    };
    Q_ENUM(ItemModelId)

    enum DataRoles {
      ItemIdRole = Qt::UserRole,
      ItemTypeRole,
      ItemFlagsRole,
      IsAvailableRole
    };
    Q_ENUM(DataRoles)

    enum DataGroups {
      NoneGroup     = 0x01,
      NegativeGroup = 0x02,
      PositiveGroup = 0x04
    };
    Q_ENUM(DataGroups)

    enum UpdateTrigger {
      SystemRefresh           = 1 << 0,
      ChannelsUpdated         = 1 << 1,
      CurvesUpdated           = 1 << 2,
      FlightModesUpdated      = 1 << 3,
      GVarsUpdated            = 1 << 4,
      InputsUpdated           = 1 << 5,
      LogicalSwitchesUpdated  = 1 << 6,
      ScriptsUpdated          = 1 << 7,
      TeleSensorsUpdated      = 1 << 8,
      TimersUpdated           = 1 << 9,
      AllTriggers = SystemRefresh | ChannelsUpdated | CurvesUpdated | FlightModesUpdated | GVarsUpdated | InputsUpdated |
                    LogicalSwitchesUpdated | ScriptsUpdated | TeleSensorsUpdated | TimersUpdated,
      NoTriggers = 0
    };
    Q_ENUM(UpdateTrigger)

    explicit AbstractItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                               Firmware * firmware, const Boards * const board, const Board::Type boardType) :
      QStandardItemModel(nullptr),
      generalSettings(generalSettings),
      modelData(modelData),
      firmware(firmware),
      board(board),
      boardType(boardType)
    {}

    virtual ~AbstractItemModel() {};

    void setId(ItemModelId id) { m_id = id; }
    ItemModelId getId () const { return m_id; }

    void setUpdateMask(const int mask) { m_updateMask = mask; }
    int getUpdateMask() const { return m_updateMask; }
    inline bool doUpdate(const UpdateTrigger trigger) const { return m_updateMask & (int)trigger; }

    AbstractItemModel * getItemModel(const ItemModelId id) const;

    static void dumpItemModelContents(AbstractItemModel * itemModel);

  public slots:
    virtual void update(const UpdateTrigger trigger = SystemRefresh) = 0;

  protected:
    const GeneralSettings * generalSettings;
    const ModelData * modelData;
    Firmware * firmware;
    const Boards * board;
    const Board::Type boardType;

  private:
    ItemModelId m_id = UnknownId;
    int m_updateMask = 0;
};

class AbstractStaticItemModel: public AbstractItemModel
{
    Q_OBJECT
  public:
    explicit AbstractStaticItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                                     Firmware * firmware, const Boards * const board, const Board::Type boardType) :
      AbstractItemModel(generalSettings, modelData, firmware, board, boardType) {}
    virtual ~AbstractStaticItemModel() {};

  public slots:
    virtual void update(const UpdateTrigger trigger = SystemRefresh) override final {}
};

class AbstractDynamicItemModel: public AbstractItemModel
{
    Q_OBJECT
  public:
    explicit AbstractDynamicItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                                      Firmware * firmware, const Boards * const board, const Board::Type boardType) :
      AbstractItemModel(generalSettings, modelData, firmware, board, boardType) {}
    virtual ~AbstractDynamicItemModel() {};

  public slots:
    virtual void update(const UpdateTrigger trigger = SystemRefresh) {}

  signals:
    void aboutToBeUpdated();
    void updateComplete();
};

class RawSourceItemModel: public AbstractDynamicItemModel
{
    Q_OBJECT
  public:
    explicit RawSourceItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                                Firmware * firmware, const Boards * const board, const Board::Type boardType);
    virtual ~RawSourceItemModel() {};

  public slots:
    virtual void update(const UpdateTrigger trigger = SystemRefresh) override;

  protected:
    virtual void setDynamicItemData(QStandardItem * item, const RawSource & src) const;
    void addItems(const RawSourceType & type, const int group, const int count, const int start = 0);
};

class RawSwitchItemModel: public AbstractDynamicItemModel
{
    Q_OBJECT
  public:
    explicit RawSwitchItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                                Firmware * firmware, const Boards * const board, const Board::Type boardType);
    virtual ~RawSwitchItemModel() {};

  public slots:
    virtual void update(const UpdateTrigger trigger = SystemRefresh) override;

  protected:
    virtual void setDynamicItemData(QStandardItem * item, const RawSwitch & rsw) const;
    void addItems(const RawSwitchType & type, int count);
};

class CurveItemModel: public AbstractDynamicItemModel
{
    Q_OBJECT
  public:
    explicit CurveItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                            Firmware * firmware, const Boards * const board, const Board::Type boardType);
    virtual ~CurveItemModel() {};

  public slots:
    virtual void update(const UpdateTrigger trigger = SystemRefresh) override;

  protected:
    virtual void setDynamicItemData(QStandardItem * item, const int value) const;
};

class GVarReferenceItemModel: public AbstractDynamicItemModel
{
    Q_OBJECT
  public:
    explicit GVarReferenceItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                                    Firmware * firmware, const Boards * const board, const Board::Type boardType);
    virtual ~GVarReferenceItemModel() {};

  public slots:
    virtual void update(const UpdateTrigger trigger = SystemRefresh) override;

  protected:
    virtual void setDynamicItemData(QStandardItem * item, const AdjustmentReference & ar) const;
    void addItems(int count);
};

class ThrottleSourceItemModel: public AbstractDynamicItemModel
{
    Q_OBJECT
  public:
    explicit ThrottleSourceItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                                     Firmware * firmware, const Boards * const board, const Board::Type boardType);
    virtual ~ThrottleSourceItemModel() {};

  public slots:
    virtual void update(const UpdateTrigger trigger = SystemRefresh) override;

  protected:
    virtual void setDynamicItemData(QStandardItem * item, const int value) const;
};

class CustomFuncActionItemModel: public AbstractDynamicItemModel
{
    Q_OBJECT
  public:
    explicit CustomFuncActionItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                                       Firmware * firmware, const Boards * const board, const Board::Type boardType);
    virtual ~CustomFuncActionItemModel() {};

  public slots:
    virtual void update(const UpdateTrigger trigger = SystemRefresh) override;

  protected:
    virtual void setDynamicItemData(QStandardItem * item, const int value) const;
};

class CustomFuncResetParamItemModel: public AbstractDynamicItemModel
{
    Q_OBJECT
  public:
    explicit CustomFuncResetParamItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                                           Firmware * firmware, const Boards * const board, const Board::Type boardType);
    virtual ~CustomFuncResetParamItemModel() {};

  public slots:
    virtual void update(const UpdateTrigger trigger = SystemRefresh) override;

  protected:
    virtual void setDynamicItemData(QStandardItem * item, const int value) const;
};

class TelemetrySourceItemModel: public AbstractDynamicItemModel
{
    Q_OBJECT
  public:
    explicit TelemetrySourceItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                                      Firmware * firmware, const Boards * const board, const Board::Type boardType);
    virtual ~TelemetrySourceItemModel() {};

  public slots:
    virtual void update(const UpdateTrigger trigger = SystemRefresh) override;

  protected:
    virtual void setDynamicItemData(QStandardItem * item, const int value) const;
};

class CurveRefTypeItemModel : public AbstractStaticItemModel
{
    Q_OBJECT
  public:
    explicit CurveRefTypeItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                                   Firmware * firmware, const Boards * const board, const Board::Type boardType);
    virtual ~CurveRefTypeItemModel() {};
};

class CurveRefFuncItemModel : public AbstractStaticItemModel
{
    Q_OBJECT
  public:
    explicit CurveRefFuncItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData,
                                   Firmware * firmware, const Boards * const board, const Board::Type boardType);
    virtual ~CurveRefFuncItemModel() {};
};

class ItemModelsFactory
{
  public:
    ItemModelsFactory(const GeneralSettings * const generalSettings, const ModelData * const modelData);
    virtual ~ItemModelsFactory();

    void addItemModel(const AbstractItemModel::ItemModelId id);
    void registerItemModel(AbstractItemModel * itemModel);
    void unregisterItemModels();
    void unregisterItemModel(const AbstractItemModel::ItemModelId id);
    AbstractItemModel * getItemModel(const AbstractItemModel::ItemModelId id) const;
    void update(const AbstractItemModel::UpdateTrigger trigger = AbstractItemModel::SystemRefresh);
    void dumpAllItemModelContents() const;

  protected:
    const GeneralSettings * generalSettings;
    const ModelData * modelData;
    Firmware * firmware;
    Boards * board;
    Board::Type boardType;
    QVector<AbstractItemModel *> registeredItemModels;
};

#endif // RAWITEMDATAMODELS_H
