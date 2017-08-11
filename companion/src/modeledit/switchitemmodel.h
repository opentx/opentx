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
#include "radiodata.h"

enum SwitchContext
{
    LogicalSwitchesContext,
    SpecialFunctionsContext,
    GlobalFunctionsContext,
    TimersContext,
    MixesContext
};

class RawSwitchItemModel: public QStandardItemModel {
  public:
    RawSwitchItemModel(const GeneralSettings * const generalSettings, const ModelData * const modelData);
    void update();

  protected:
    void add(const RawSwitchType & type, int count);

    const GeneralSettings * generalSettings;
    const ModelData * modelData;
};

class RawSwitchFilterItemModel: public QSortFilterProxyModel {
  public:
    RawSwitchFilterItemModel(const GeneralSettings * const generalSettings = NULL, const ModelData * const modelData = NULL, SwitchContext context = LogicalSwitchesContext);
    void update();

  protected:
    const GeneralSettings * generalSettings;
    const ModelData * modelData;
    SwitchContext context;
    RawSwitchItemModel * parent;

    bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const override;
};
