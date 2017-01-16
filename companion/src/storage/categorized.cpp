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

  if (!loadRadioSettingsFromByteArray(radioData.generalSettings, radioSettingsBuffer)) {
    return false;
  }
  
  QByteArray modelsListBuffer;
  if (!loadFile(modelsListBuffer, "RADIO/models.txt")) {
    setError(QObject::tr("Can't extract RADIO/models.txt"));
    return false;
  }
  
  QList<QByteArray> lines = modelsListBuffer.split('\n');
  int modelIndex = 0;
  int categoryIndex = -1;
  foreach (const QByteArray & line, lines) {
    if (!line.isEmpty()) {
      if (line.startsWith('[') && line.endsWith(']')) {
        QString name = line.mid(1, line.size() - 2);
        CategoryData category(name.toStdString().c_str());
        radioData.categories.push_back(category);
        categoryIndex++;
      }
      else {
        qDebug() << "Loading" << line;
        QByteArray modelBuffer;
        if (!loadFile(modelBuffer, QString("MODELS/%1").arg(QString(line)))) {
          setError(QObject::tr("Can't extract %1").arg(QString(line)));
          return false;
        }
        if ((int)radioData.models.size() <= modelIndex) {
          radioData.models.resize(modelIndex + 1);
        }
        if (!loadModelFromByteArray(radioData.models[modelIndex], modelBuffer)) {
          return false;
        }
        strncpy(radioData.models[modelIndex].filename, line.data(), sizeof(radioData.models[modelIndex].filename));
        if (strcmp(radioData.generalSettings.currModelFilename, line.data()) == 0) {
          radioData.generalSettings.currModelIndex = modelIndex;
        }
        radioData.models[modelIndex].category = categoryIndex;
        radioData.models[modelIndex].used = true;
        modelIndex++;
      }
    }
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
  
  // all models
  for (unsigned int i=0; i<radioData.models.size(); i++) {
    const ModelData & model = radioData.models[i];
    if (!model.isEmpty()) {
      QString modelFilename = QString("MODELS/%1").arg(model.filename);
      QByteArray modelData;
      writeModelToByteArray(model, modelData);
      if (!writeFile(modelData, modelFilename)) {
        return false;
      }
      int categoryIndex = model.category;
      if (currentCategoryIndex != categoryIndex) {
        modelsList.append(QString().sprintf("[%s]\n", radioData.categories[model.category].name));
        currentCategoryIndex = categoryIndex;
      }
      modelsList.append(QString(model.filename) + "\n");
    }
  }
  
  if (!writeFile(modelsList, "RADIO/models.txt")) {
    return false;
  }

  return true;
}
