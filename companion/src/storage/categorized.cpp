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

#include "categorized.h"
#include "firmwares/opentx/opentxinterface.h"

bool CategorizedStorageFormat::load(RadioData & radioData)
{
  QByteArray radioSettingsBuffer;
  if (!loadFile(radioSettingsBuffer, "RADIO/radio.bin")) {
    setError(QObject::tr("Can't extract RADIO/radio.bin"));
    return false;
  }

  OpenTxEepromInterface * loadInterface = loadRadioSettingsFromByteArray(radioData.generalSettings, radioSettingsBuffer);
  if (!loadInterface) {
    return false;
  }

  board = loadInterface->getBoard();

  QByteArray modelsListBuffer;
  if (!loadFile(modelsListBuffer, "RADIO/models.txt")) {
    setError(QObject::tr("Can't extract RADIO/models.txt"));
    return false;
  }

  QList<QByteArray> lines = modelsListBuffer.split('\n');
  int modelIndex = 0;
  int categoryIndex = -1;
  foreach (const QByteArray & lineArray, lines) {
    QString line = QString(lineArray).trimmed();
    if (line.isEmpty()) continue;
    // qDebug() << "parsing line" << line;

    if (line.startsWith('[') && line.endsWith(']')) {
      // ignore categories for boards that do not support them
      if (getCurrentFirmware()->getCapability(HasModelCategories)) {
        QString name = line.mid(1, line.size() - 2);
        CategoryData category(qPrintable(name));
        radioData.categories.push_back(category);
        categoryIndex++;
        qDebug() << "added category" << name;
      }
      continue;
    }

    // determine if we have a model number
    QStringList parts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if (parts.size() == 2) {
      // parse model number
      int modelNumber = parts[0].toInt();
      if (modelNumber > 0 && modelNumber > modelIndex && modelNumber < getCurrentFirmware()->getCapability(Models)) {
        modelIndex = modelNumber;
        qDebug() << "advanced model number to" << modelIndex;
      }
      else {
        if (modelNumber != modelIndex) qDebug() << "Invalid model number" << parts[0];
      }
      parts.removeFirst();
    }
    if (parts.size() == 1) {
      // parse model file name and load
      QString fileName = parts[0];
      qDebug() << "Loading model from file" << fileName << "into slot" << modelIndex;
      QByteArray modelBuffer;
      if (!loadFile(modelBuffer, QString("MODELS/%1").arg(fileName))) {
        setError(QObject::tr("Can't extract %1").arg(fileName));
        return false;
      }
      if ((int)radioData.models.size() <= modelIndex) {
        radioData.models.resize(modelIndex + 1);
      }
      if (!loadModelFromByteArray(radioData.models[modelIndex], modelBuffer)) {
        return false;
      }
      strncpy(radioData.models[modelIndex].filename, qPrintable(fileName), sizeof(radioData.models[modelIndex].filename));
      if (IS_HORUS(board) && !strcmp(radioData.generalSettings.currModelFilename, qPrintable(fileName))) {
        radioData.generalSettings.currModelIndex = modelIndex;
        qDebug() << "currModelIndex =" << modelIndex;
      }
      if (getCurrentFirmware()->getCapability(HasModelCategories)) {
        radioData.models[modelIndex].category = categoryIndex;
      }
      radioData.models[modelIndex].used = true;
      modelIndex++;
      continue;
    }

    // invalid line
    // TODO add to parsing report
    qDebug() << "Invalid line" <<line;
    continue;
  }
  return true;
}

bool CategorizedStorageFormat::write(const RadioData & radioData)
{
  // models.txt
  QByteArray modelsList;
  int currentCategoryIndex = -1;

  // radio.bin
  QByteArray radioSettingsData;
  writeRadioSettingsToByteArray(radioData.generalSettings, radioSettingsData);
  if (!writeFile(radioSettingsData, "RADIO/radio.bin")) {
    return false;
  }

  // Iterate through all models, writing to their .bin files. Because radioData
  // isn't sorted w.r.t categories, we have to build up sortedModels so ww can
  // write models.txt in the correct order. We use category index '-1' to handle
  // all the models that have no category.
  unsigned int numModels = radioData.models.size();
  unsigned int numCategories = radioData.categories.size();
  std::map<int, std::vector<int>> sortedModels;

  for (unsigned int m=0; m<numModels; m++) {
    const ModelData & model = radioData.models[m];
    if (model.isEmpty()) continue;

    QString modelFilename = QString("MODELS/%1").arg(model.filename);
    QByteArray modelData;
    writeModelToByteArray(model, modelData);
    if (!writeFile(modelData, modelFilename)) {
      return false;
    }

    currentCategoryIndex = getCurrentFirmware()->getCapability(HasModelCategories) ?
                  model.category : -1;
    sortedModels[currentCategoryIndex].push_back(m);
  }

  for (int c= -1; c < (int)numCategories; c++) {

    if (c > -1 && getCurrentFirmware()->getCapability(HasModelCategories)) {
      modelsList.append(QString().sprintf("[%s]\n", radioData.categories[c].name));
    }

    numModels = sortedModels[c].size();
    for (unsigned int m=0; m<numModels; m++) {
      const ModelData & model = radioData.models[sortedModels[c][m]];
      QString line;
      if (IS_HORUS(getCurrentBoard())) {
        line = QString("%1\n").arg(model.filename);
      }
      else {
        // use format with model number and file name
        // this is needed, because this kind of radios can have unused model slots
        // NOTE: The model number will only be unique within a category
        line = QString("%1\n").arg(model.filename);

      }
      modelsList.append(line);
    }
  }

  if (!writeFile(modelsList, "RADIO/models.txt")) {
    return false;
  }

  return true;
}
