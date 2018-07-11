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

#ifndef RAWSOURCEITEMMODEL_H
#define RAWSOURCEITEMMODEL_H

#include "rawsource.h"

#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class GeneralSettings;
class ModelData;

class RawSourceItemModel: public QStandardItemModel
{
    Q_OBJECT
  public:
    enum DataRoles { ItemIdRole = Qt::UserRole, ItemTypeRole, ItemGroupRole, IsAvailableRole };
    Q_ENUM(DataRoles)

    RawSourceItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent = nullptr);

  public slots:
    void update();

  protected:
    void setDynamicItemData(QStandardItem * item, const RawSource & src);
    void addItems(const RawSourceType & type, const unsigned group, const int count, const int start = 0);

    const GeneralSettings * generalSettings;
    const ModelData * modelData;
};


class RawSourceFilterItemModel: public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    explicit RawSourceFilterItemModel(QAbstractItemModel * sourceModel, unsigned groups, QObject * parent = nullptr);
    explicit RawSourceFilterItemModel(QAbstractItemModel * sourceModel, QObject * parent = nullptr) :
      RawSourceFilterItemModel(sourceModel, RawSource::AllSourceGroups, parent)
    {}

    explicit RawSourceFilterItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, unsigned groups, QObject * parent = nullptr);
    explicit RawSourceFilterItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent = nullptr) :
      RawSourceFilterItemModel(generalSettings, modelData, RawSource::AllSourceGroups, parent)
    {}

  public slots:
    void setFilterGroups(unsigned groups);
    void update();

  protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const override;

    unsigned int filterGroups;
};

#endif // RAWSOURCEITEMMODEL_H
