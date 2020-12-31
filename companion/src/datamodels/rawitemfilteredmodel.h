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

#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>

class FilteredItemModel: public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    enum DataFilters {
      AllFilter = AbstractItemModel::NegativeGroup | AbstractItemModel::NoneGroup | AbstractItemModel::PositiveGroup,
      AllExcludeNoneFilter = AllFilter &~ AbstractItemModel::NoneGroup,
      NegativeFilter = AbstractItemModel::NegativeGroup | AbstractItemModel::NoneGroup,
      NegativeExcludeNoneFilter = AbstractItemModel::NegativeGroup,
      PositiveFilter = AbstractItemModel::PositiveGroup | AbstractItemModel::NoneGroup,
      PositiveExcludeNoneFilter = AbstractItemModel::PositiveGroup
    };
    Q_ENUM(DataFilters)

    explicit FilteredItemModel(AbstractItemModel * sourceModel, int flags);
    explicit FilteredItemModel(AbstractItemModel * sourceModel) :
      FilteredItemModel(sourceModel, 0) {}
    virtual ~FilteredItemModel() {};

    void setId(int id) { m_id = id; }
    int getId() const { return m_id; };
    void setName(QString name) { m_name = name; }
    QString getName() const { return m_name; }

    static void dumpItemModelContents(FilteredItemModel * itemModel);

  public slots:
    void setFilterFlags(int flags);
    void update() const;
    void onAboutToBeUpdated();
    void onUpdateComplete();

  signals:
    void aboutToBeUpdated();
    void updateComplete();

  protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const override;

    int filterFlags;

  private:
    int m_id = 0;
    QString m_name = "";
};

class FilteredItemModelsFactory
{
  public:
    FilteredItemModelsFactory();
    virtual ~FilteredItemModelsFactory();

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


#define FILTEREDITEMMODELCONNECT(fltrmodel) \
  connect(fltrmodel, SIGNAL(aboutToBeUpdated()), this, SLOT(onItemModelAboutToBeUpdated())); \
  connect(fltrmodel, SIGNAL(updateComplete()), this, SLOT(onItemModelUpdateComplete()));

#define FILTEREDITEMMODEL(fltrmodel, class, modelid, flags) \
  fltrmodel = new FilteredItemModel(sharedItemModels->getItemModel(AbstractItemModel::modelid), flags); \
  FILTEREDITEMMODELCONNECT(fltrmodel)

#define FILTEREDITEMMODELNOFLAGS(fltrmodel, class, modelid) \
  fltrmodel = new FilteredItemModel(sharedItemModels->getItemModel(AbstractItemModel::modelid)); \
  FILTEREDITEMMODELCONNECT(fltrmodel)

#endif // RAWITEMFILTEREDMODEL_H
