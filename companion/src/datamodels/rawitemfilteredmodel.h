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

#ifndef RAWITEMFILTEREDMODEL_H
#define RAWITEMFILTEREDMODEL_H

#include "rawitemdatamodels.h"

#include <QSortFilterProxyModel>

class GeneralSettings;
class ModelData;

class RawItemFilteredModel: public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    enum DataFilters {
      AllFilter = AbstractRawItemDataModel::NegativeGroup | AbstractRawItemDataModel::NoneGroup | AbstractRawItemDataModel::PositiveGroup,
      AllExcludeNoneFilter = AllFilter &~ AbstractRawItemDataModel::NoneGroup,
      NegativeFilter = AbstractRawItemDataModel::NegativeGroup | AbstractRawItemDataModel::NoneGroup,
      NegativeExcludeNoneFilter = AbstractRawItemDataModel::NegativeGroup,
      PositiveFilter = AbstractRawItemDataModel::PositiveGroup | AbstractRawItemDataModel::NoneGroup,
      PositiveExcludeNoneFilter = AbstractRawItemDataModel::PositiveGroup
    };
    Q_ENUM(DataFilters)

    explicit RawItemFilteredModel(QAbstractItemModel * sourceModel, int flags, QObject * parent = nullptr);
    explicit RawItemFilteredModel(QAbstractItemModel * sourceModel, QObject * parent = nullptr) : RawItemFilteredModel(sourceModel, 0, parent) {}

  public slots:
    void setFilterFlags(int flags);
    void update() const;
    void onDataAboutToBeUpdated();
    void onDataUpdateComplete();

  signals:
    void dataAboutToBeUpdated();
    void dataUpdateComplete();

  protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const override;

    int filterFlags;
};
#endif // RAWITEMFILTEREDMODEL_H
