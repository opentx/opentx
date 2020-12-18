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

#include "curvereference.h"
#include "adjustmentreference.h"
#include "helpers.h"
#include "modeldata.h"
#include "rawitemfilteredmodel.h"

const QString CurveReference::toString(const ModelData * model, bool verbose) const
{
  if (value == 0)
    return CPN_STR_NONE_ITEM;

  QString ret;
  unsigned idx = abs(value) - 1;

  switch(type) {
    case CURVE_REF_DIFF:
    case CURVE_REF_EXPO:
      ret = AdjustmentReference(value).toString(model);
      break;
    case CURVE_REF_FUNC:
      ret = functionToString(value);
      break;
    case CURVE_REF_CUSTOM:
      if (model)
        ret = model->curves[idx].nameToString(idx);
      else
        ret = CurveData().nameToString(idx);
      if (value < 0)
        ret.prepend(CPN_STR_SW_INDICATOR_REV);
      break;
    default:
      return CPN_STR_UNKNOWN_ITEM;
  }

  if (verbose)
    ret = tr(qPrintable(QString(typeToString(type) + "(%1)").arg(ret)));

  return ret;
}

const bool CurveReference::isValueNumber() const
{
  return (type == CURVE_REF_DIFF || type == CURVE_REF_EXPO) && AdjustmentReference(value).type == AdjustmentReference::ADJUST_REF_VALUE;
}

//  static
int CurveReference::getDefaultValue(const CurveRefType type, const bool isGVar)
{
  if (isGVar && (type == CURVE_REF_DIFF || type == CURVE_REF_EXPO))
    return AdjustmentReference(AdjustmentReference::ADJUST_REF_GVAR, 1).toValue();
  else if (type == CURVE_REF_FUNC)
    return 1;
  else
    return 0;
}

//  static
QString CurveReference::typeToString(const CurveRefType type)
{
  const QStringList strl = { "Diff", "Expo" , "Func", "Curve" };
  int idx = (int)type;

  if (idx < 0 || idx >= strl.count())
    return CPN_STR_UNKNOWN_ITEM;

  return strl.at(idx);
}

constexpr int MAX_CURVE_REF_FUNC { 6 };

//  static
QString CurveReference::functionToString(const int value)
{
  const QStringList strl = { "x>0", "x<0", "|x|", "f>0", "f<0", "|f|" };
  int idx = value - 1;

  if (idx < 0 || idx >= strl.count())
    return CPN_STR_UNKNOWN_ITEM;

  return strl.at(idx);
}

/*
 * CurveReferenceUIManager
*/

constexpr int CURVE_REF_UI_HIDE_DIFF             { 0x01 };
constexpr int CURVE_REF_UI_HIDE_EXPO             { 0x02 };
constexpr int CURVE_REF_UI_HIDE_NEGATIVE_CURVES  { 0x04 };

//  static
bool CurveReferenceUIManager::firsttime { true };
int CurveReferenceUIManager::flags { 0 };
bool CurveReferenceUIManager::hasCapabilityGvars { false };
int CurveReferenceUIManager::numCurves { 0 };
RawItemFilteredModel * CurveReferenceUIManager::curveItemModel { nullptr };
QStandardItemModel * CurveReferenceUIManager::tempModel { nullptr };

CurveReferenceUIManager::CurveReferenceUIManager(QComboBox * curveValueCB, CurveReference & curve, const ModelData & model,
                                                 RawItemFilteredModel * curveItemModel, QObject * parent) :
  QObject(parent),
  curveTypeCB(nullptr),
  curveGVarCB(nullptr),
  curveValueSB(nullptr),
  curveValueCB(curveValueCB),
  curve(curve),
  model(model),
  lock(false)
{
  init(curveItemModel);
}

CurveReferenceUIManager::CurveReferenceUIManager(QComboBox * curveTypeCB, QCheckBox * curveGVarCB, QSpinBox * curveValueSB,
                                                 QComboBox * curveValueCB, CurveReference & curve, const ModelData & model,
                                                 RawItemFilteredModel * curveItemModel, QObject * parent) :
  QObject(parent),
  curveTypeCB(curveTypeCB),
  curveGVarCB(curveGVarCB),
  curveValueSB(curveValueSB),
  curveValueCB(curveValueCB),
  curve(curve),
  model(model),
  lock(false)
{
  init(curveItemModel);
}

CurveReferenceUIManager::~CurveReferenceUIManager()
{
  delete tempModel;
}

void CurveReferenceUIManager::init(RawItemFilteredModel * curveModel)
{
  tempModel = new QStandardItemModel();

  if (firsttime) {
    firsttime = false;
    Firmware * fw = getCurrentFirmware();
    hasCapabilityGvars = fw->getCapability(Gvars);
    numCurves = fw->getCapability(NumCurves);
    curveItemModel = curveModel;

    if (!fw->getCapability(HasInputDiff))
      flags |= (CURVE_REF_UI_HIDE_DIFF | CURVE_REF_UI_HIDE_NEGATIVE_CURVES);

    if (!fw->getCapability(HasMixerExpo))
      flags |= CURVE_REF_UI_HIDE_EXPO;
  }

  if (curveTypeCB) {
    populateTypeCB(curveTypeCB, curve);
    connect(curveTypeCB, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));
  }

  if (curveGVarCB)
    connect(curveGVarCB, SIGNAL(stateChanged(int)), this, SLOT(gvarCBChanged(int)));

  if (curveValueSB) {
    curveValueSB->setMinimum(-100);
    curveValueSB->setMaximum(100);
    connect(curveValueSB, SIGNAL(editingFinished()), this, SLOT(valueSBChanged()));
  }

  curveValueCB->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  curveValueCB->setMaxVisibleItems(10);
  connect(curveValueCB, SIGNAL(currentIndexChanged(int)), this, SLOT(valueCBChanged()));

  update();
}

#define CURVE_REF_UI_GVAR_SHOW  (1<<0)
#define CURVE_REF_UI_VALUE_SHOW (1<<1)
#define CURVE_REF_UI_REF_SHOW   (1<<2)

void CurveReferenceUIManager::update()
{
  lock = true;
  int widgetsMask = 0;

  if (curve.type == CurveReference::CURVE_REF_DIFF || curve.type == CurveReference::CURVE_REF_EXPO) {
    if (hasCapabilityGvars)
      widgetsMask |= CURVE_REF_UI_GVAR_SHOW;
    if (curve.isValueNumber()) {
      curveGVarCB->setChecked(false);
      curveValueSB->setValue(curve.value);
      widgetsMask |= CURVE_REF_UI_VALUE_SHOW;
    }
    else {
      curveGVarCB->setChecked(true);
      widgetsMask |= CURVE_REF_UI_REF_SHOW;
    }
  }
  else {
    widgetsMask |= CURVE_REF_UI_REF_SHOW;
  }

  if(curveTypeCB) {
    curveTypeCB->setCurrentIndex(curveTypeCB->findData(curve.type));
    curveTypeCB->show();
  }
  if(curveGVarCB)
    curveGVarCB->setVisible(widgetsMask & CURVE_REF_UI_GVAR_SHOW);
  if(curveValueSB)
    curveValueSB->setVisible(widgetsMask & CURVE_REF_UI_VALUE_SHOW);
  if(curveValueCB) {
    if (curve.isValueReference())
      populateValueCB(curveValueCB, curve, &model);
    curveValueCB->setVisible(widgetsMask & CURVE_REF_UI_REF_SHOW);
  }

  lock = false;
}

void CurveReferenceUIManager::gvarCBChanged(int state)
{
  if (!lock) {
    curve.value = CurveReference::getDefaultValue(curve.type, state);
    update();
  }
}

void CurveReferenceUIManager::typeChanged(int value)
{
  if (!lock) {
    CurveReference::CurveRefType type = (CurveReference::CurveRefType)curveTypeCB->itemData(curveTypeCB->currentIndex()).toInt();
    curve = CurveReference(type, CurveReference::getDefaultValue(type));
    update();
  }
}

void CurveReferenceUIManager::valueSBChanged()
{
  if (!lock) {
    curve.value = curveValueSB->value();
    update();
  }
}

void CurveReferenceUIManager::valueCBChanged()
{
  if (!lock) {
    curve.value = curveValueCB->itemData(curveValueCB->currentIndex()).toInt();
    update();
  }
}

//  static
void CurveReferenceUIManager::populateTypeCB(QComboBox * cb, const CurveReference & curveRef)
{
  if (cb) {
    cb->clear();
    for (int i = 0; i <= CurveReference::MAX_CURVE_REF_TYPE; i++) {
      if ((curveRef.type == CurveReference::CURVE_REF_DIFF && !(flags & CURVE_REF_UI_HIDE_DIFF)) ||
          (curveRef.type == CurveReference::CURVE_REF_EXPO && !(flags & CURVE_REF_UI_HIDE_EXPO)) ||
          (curveRef.type != CurveReference::CURVE_REF_DIFF && curveRef.type != CurveReference::CURVE_REF_EXPO))
        cb->addItem(CurveReference::typeToString((CurveReference::CurveRefType)i), i);
    }

    cb->setCurrentIndex(cb->findData((int)curveRef.type));
  }
}

//  static
void CurveReferenceUIManager::populateValueCB(QComboBox * cb, const CurveReference & curveRef, const ModelData * model)
{
  if (cb) {
    cb->setModel(tempModel);  //  do not want to clear/alter the shared curves model and set to nullptr is invalid

    switch (curveRef.type) {
      case CurveReference::CURVE_REF_DIFF:
      case CurveReference::CURVE_REF_EXPO:
        cb->clear();
        Helpers::populateGVCB(*cb, curveRef.value, *model);
        break;
      case CurveReference::CURVE_REF_FUNC:
        cb->clear();
        for (int i = 1; i <= MAX_CURVE_REF_FUNC; i++) {
          cb->addItem(CurveReference::functionToString(i), i);
        }
        break;
      case CurveReference::CURVE_REF_CUSTOM:
        cb->setModel(curveItemModel);
        break;
      default:
        break;
    }

    cb->setCurrentIndex(cb->findData(curveRef.value));
  }
}
