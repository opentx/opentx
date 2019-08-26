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

#include <QSortFilterProxyModel>

class GeneralSettings;
class ModelData;
class RawSourceItemModel;
class RawSwitchItemModel;

class RawItemFilteredModel: public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    explicit RawItemFilteredModel(QAbstractItemModel * sourceModel, int flags, QObject * parent = nullptr);
    explicit RawItemFilteredModel(QAbstractItemModel * sourceModel, QObject * parent = nullptr) : RawItemFilteredModel(sourceModel, 0, parent) {}

  public slots:
    void setFilterFlags(int flags);
    void update() const;

  protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const override;

    int filterFlags;
};


// The specialized "convenience" types below can go away once centralalized RawSource/RawSwitch item models are established.
// These proxy classes will automatically create a source model of the corresponding type.

class RawSwitchFilterItemModel: public RawItemFilteredModel
{
    Q_OBJECT
  public:
    using RawItemFilteredModel::RawItemFilteredModel;

    explicit RawSwitchFilterItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, int context, QObject * parent = nullptr);
};


class RawSourceFilterItemModel: public RawItemFilteredModel
{
    Q_OBJECT
  public:
    using RawItemFilteredModel::RawItemFilteredModel;

    explicit RawSourceFilterItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, int flags, QObject * parent = nullptr);

    explicit RawSourceFilterItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData, QObject * parent = nullptr) :
      RawSourceFilterItemModel(generalSettings, modelData, 0, parent) {}
};

#endif // RAWITEMFILTEREDMODEL_H
