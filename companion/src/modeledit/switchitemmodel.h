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

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include "rawswitch.h"

class GeneralSettings;
class ModelData;

class RawSwitchItemModel: public QStandardItemModel
{
    Q_OBJECT
  public:
    enum DataRoles { ItemIdRole = Qt::UserRole, ItemTypeRole, ContextRole, IsAvailableRole };
    Q_ENUM(DataRoles)

    RawSwitchItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent = nullptr);

  public slots:
    void update() const;

  protected:
    void setDynamicItemData(QStandardItem * item, const RawSwitch & rsw) const;
    void add(const RawSwitchType & type, int count);

    const GeneralSettings * generalSettings;
    const ModelData * modelData;
};

class RawSwitchFilterItemModel: public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    RawSwitchFilterItemModel(QAbstractItemModel * sourceModel, RawSwitch::SwitchContext context, QObject * parent = nullptr);
    RawSwitchFilterItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, RawSwitch::SwitchContext context, QObject * parent = nullptr);

  public slots:
    void setSwitchContext(RawSwitch::SwitchContext ctxt);
    void update() const;

  protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const override;

    RawSwitch::SwitchContext context = RawSwitch::NoSwitchContext;
};
