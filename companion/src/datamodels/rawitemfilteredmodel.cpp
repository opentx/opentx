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

#include "rawitemfilteredmodel.h"
#include "rawitemdatamodels.h"

RawItemFilteredModel::RawItemFilteredModel(QAbstractItemModel * sourceModel, int flags, QObject * parent) :
  QSortFilterProxyModel(parent),
  filterFlags(0)
{
  setFilterRole(AbstractRawItemDataModel::IsAvailableRole);
  setFilterKeyColumn(0);
  setFilterFlags(flags);
  setDynamicSortFilter(true);
  setSourceModel(sourceModel);
}

void RawItemFilteredModel::setFilterFlags(int flags)
{
  if (filterFlags != flags) {
    filterFlags = flags;
    invalidateFilter();
  }
}

bool RawItemFilteredModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const
{
  const QModelIndex & srcIdx = sourceModel()->index(sourceRow, 0, sourceParent);
  if (!srcIdx.isValid() || !sourceModel()->data(srcIdx, filterRole()).toBool())
    return false;

  if (!filterFlags)
    return true;

  bool ok;
  const int flags = sourceModel()->data(srcIdx, AbstractRawItemDataModel::ItemFlagsRole).toInt(&ok);
  return (ok && (!flags || (filterFlags & flags)));
}

void RawItemFilteredModel::update() const
{
  AbstractRawItemDataModel * model = qobject_cast<AbstractRawItemDataModel *>(sourceModel());
  if (model)
    model->update();
}


RawSourceFilterItemModel::RawSourceFilterItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, int flags, QObject * parent) :
  RawItemFilteredModel(new RawSourceItemModel(generalSettings, modelData, parent), flags, parent)
{
}


RawSwitchFilterItemModel::RawSwitchFilterItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, int context, QObject * parent) :
  RawItemFilteredModel(new RawSwitchItemModel(generalSettings, modelData, parent), context, parent)
{
}
