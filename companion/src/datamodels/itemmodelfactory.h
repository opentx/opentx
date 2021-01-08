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

#ifndef ITEMMODELFACTORY_H
#define ITEMMODELFACTORY_H

class GeneralSettings;
class ModelData;
class AbstractItemModel;
class FilteredItemModel;

class ItemModelFactory
{
  public:
    ItemModelFactory(const GeneralSettings * const generalSettings, const ModelData * const modelData);
    virtual ~ItemModelFactory();

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

class FilteredItemModelFactory
{
  public:
    FilteredItemModelFactory();
    virtual ~FilteredItemModelFactory();

    int registerItemModel(FilteredItemModel * itemModel, QString name);
    void unregisterItemModels();
    void unregisterItemModel(const int id);
    bool isItemModelRegistered(const int id) const;
    bool isItemModelRegistered(const QString name) const;
    FilteredItemModel * getItemModel(const int id) const;
    FilteredItemModel * getItemModel(const QString name) const;
    void update();

    void dumpItemModelContents(const int id) const;
    void dumpItemModelContents(const QString name) const;
    void dumpAllItemModelContents() const;

  protected:
    QVector<FilteredItemModel *> registeredItemModels;
};

#endif // ITEMMODELFACTORY_H
