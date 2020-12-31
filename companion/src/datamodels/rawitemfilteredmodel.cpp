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

FilteredItemModel::FilteredItemModel(AbstractItemModel * sourceModel, int flags) :
  QSortFilterProxyModel(nullptr),
  filterFlags(0)
{
  setFilterRole(AbstractItemModel::IsAvailableRole);
  setFilterKeyColumn(0);
  setFilterFlags(flags);
  setDynamicSortFilter(true);
  setSourceModel(sourceModel);

  AbstractDynamicItemModel * itemModel = qobject_cast<AbstractDynamicItemModel *>(sourceModel);
  if (itemModel) {
    connect(itemModel, &AbstractDynamicItemModel::aboutToBeUpdated, this, &FilteredItemModel::onAboutToBeUpdated);
    connect(itemModel, &AbstractDynamicItemModel::updateComplete, this, &FilteredItemModel::onUpdateComplete);
  }
}

void FilteredItemModel::setFilterFlags(int flags)
{
  if (filterFlags != flags) {
    filterFlags = flags;
    invalidateFilter();
  }
}

bool FilteredItemModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const
{
  const QModelIndex & srcIdx = sourceModel()->index(sourceRow, 0, sourceParent);
  if (!srcIdx.isValid() || !sourceModel()->data(srcIdx, filterRole()).toBool())
    return false;

  if (!filterFlags)
    return true;

  bool ok;
  const int flags = sourceModel()->data(srcIdx, AbstractItemModel::ItemFlagsRole).toInt(&ok);
  return (ok && (!flags || (filterFlags & flags)));
}

void FilteredItemModel::update() const
{
  AbstractDynamicItemModel * itemModel = qobject_cast<AbstractDynamicItemModel *>(sourceModel());
  if (itemModel)
    itemModel->update();
}

void FilteredItemModel::onAboutToBeUpdated()
{
  emit aboutToBeUpdated();
}

void FilteredItemModel::onUpdateComplete()
{
  emit updateComplete();
}

// static
void FilteredItemModel::dumpItemModelContents(FilteredItemModel * itemModel)
{
  if (itemModel) {
    qDebug() << "id:" << itemModel->getId() << "name:" << itemModel->getName();

    for (int i = 0; i < itemModel->rowCount(); ++i) {
      qDebug() << "row:"   << i
               << "text:"  << itemModel->data(itemModel->index(i, 0), Qt::DisplayRole).toString()
               << "id:"    << itemModel->data(itemModel->index(i, 0), AbstractItemModel::ItemIdRole).toInt()
               << "avail:" << itemModel->data(itemModel->index(i, 0), AbstractItemModel::IsAvailableRole).toBool()
               << "flags:" << itemModel->data(itemModel->index(i, 0), AbstractItemModel::ItemFlagsRole).toInt()
               << "type:"  << itemModel->data(itemModel->index(i, 0), AbstractItemModel::ItemTypeRole).toInt();
    }
  }
  else
    qDebug() << "Error: model did not cast";
}

//
// FilteredItemModelsFactory
//

FilteredItemModelsFactory::FilteredItemModelsFactory()
{
}

FilteredItemModelsFactory::~FilteredItemModelsFactory()
{
  unregisterItemModels();
}

int FilteredItemModelsFactory::registerItemModel(FilteredItemModel * itemModel, QString name)
{
  if (itemModel) {
    if (isItemModelRegistered(name))
      qDebug() << "Warning: item model already registered with name:" << name;
    const int id = registeredItemModels.count();
    itemModel->setId(id);
    itemModel->setName(name);
    registeredItemModels.push_back(itemModel);
    return id;
  }
  return 0;
}

void FilteredItemModelsFactory::unregisterItemModels()
{
  foreach (FilteredItemModel * itemModel, registeredItemModels) {
    delete itemModel;
  }
}

void FilteredItemModelsFactory::unregisterItemModel(const int id)
{
  FilteredItemModel * itemModel =  getItemModel(id);
  if (itemModel)
    delete itemModel;
}

bool FilteredItemModelsFactory::isItemModelRegistered(const int id) const
{
  return getItemModel(id);
}

bool FilteredItemModelsFactory::isItemModelRegistered(const QString name) const
{
  return getItemModel(name);
}

FilteredItemModel * FilteredItemModelsFactory::getItemModel(const int id) const
{
  foreach (FilteredItemModel * itemModel, registeredItemModels) {
    if (itemModel->getId() == id)
      return itemModel;
  }

  return nullptr;
}

FilteredItemModel * FilteredItemModelsFactory::getItemModel(const QString name) const
{
  foreach (FilteredItemModel * itemModel, registeredItemModels) {
    if (itemModel->getName() == name)
      return itemModel;
  }

  return nullptr;
}

void FilteredItemModelsFactory::update()
{
  foreach (FilteredItemModel * itemModel, registeredItemModels) {
    itemModel->update();
  }
}

void FilteredItemModelsFactory::dumpItemModelContents(const int id) const
{
  FilteredItemModel::dumpItemModelContents(getItemModel(id));
}

void FilteredItemModelsFactory::dumpItemModelContents(const QString name) const
{
  FilteredItemModel::dumpItemModelContents(getItemModel(name));
}

void FilteredItemModelsFactory::dumpAllItemModelContents() const
{
  foreach (FilteredItemModel * itemModel, registeredItemModels) {
    FilteredItemModel::dumpItemModelContents(itemModel);
  }
}
