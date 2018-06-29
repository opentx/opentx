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

#include "customfunctions.h"
#include "switchitemmodel.h"
#include "helpers.h"
#include "appdata.h"

#include <TimerEdit>

RepeatComboBox::RepeatComboBox(QWidget *parent, int & repeatParam):
  QComboBox(parent),
  repeatParam(repeatParam)
{
  unsigned int step = IS_ARM(getCurrentBoard()) ? 1 : 10;
  int value = repeatParam/step;

  setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

  if (step == 1) {
    addItem(tr("Played once, not during startup"), -1);
    value++;
  }

  addItem(tr("No repeat"), 0);

  for (unsigned int i=step; i<=60; i+=step) {
    addItem(tr("%1s").arg(i), i);
  }

  setCurrentIndex(value);

  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChanged(int)));
}

void RepeatComboBox::onIndexChanged(int index)
{
  repeatParam = itemData(index).toInt();
  emit modified();
}

void RepeatComboBox::update()
{
  unsigned int step = IS_ARM(getCurrentBoard()) ? 1 : 10;
  int value = repeatParam/step;
  if (step == 1) {
    value++;
  }
  setCurrentIndex(value);
}

CustomFunctionsPanel::CustomFunctionsPanel(QWidget * parent, ModelData * model, GeneralSettings & generalSettings, Firmware * firmware):
  GenericPanel(parent, model, generalSettings, firmware),
  functions(model ? model->customFn : generalSettings.customFn),
  rawSwitchItemModel(NULL),
  rawSrcInputsItemModel(NULL),
  rawSrcAllItemModel(NULL),
  mediaPlayerCurrent(-1),
  mediaPlayer(NULL)
{
  Stopwatch s1("CustomFunctionsPanel - populate");
  lock = true;
  int num_fsw = model ? firmware->getCapability(CustomFunctions) : firmware->getCapability(GlobalFunctions);

  setDataModels();

  if (!firmware->getCapability(VoicesAsNumbers)) {
    tracksSet = getFilesSet(getSoundsPath(generalSettings), QStringList() << "*.wav" << "*.WAV", firmware->getCapability(VoicesMaxLength));
    for (int i=0; i<num_fsw; i++) {
      if (functions[i].func==FuncPlayPrompt || functions[i].func==FuncBackgroundMusic) {
        QString temp = functions[i].paramarm;
        if (!temp.isEmpty()) {
          tracksSet.insert(temp);
        }
      }
    }
  }

  s1.report("get tracks");

  if (IS_STM32(firmware->getBoard())) {
    scriptsSet = getFilesSet(g.profile[g.id()].sdPath() + "/SCRIPTS/FUNCTIONS", QStringList() << "*.lua", firmware->getCapability(VoicesMaxLength));
    for (int i=0; i<num_fsw; i++) {
      if (functions[i].func==FuncPlayScript) {
        QString temp = functions[i].paramarm;
        if (!temp.isEmpty()) {
          scriptsSet.insert(temp);
        }
      }
    }
  }
  s1.report("get scripts");

  CompanionIcon playIcon("play.png");
  playIcon.addImage("stop.png", QIcon::Normal, QIcon::On);

  QStringList headerLabels;
  headerLabels << "#" << tr("Switch") << tr("Action") << tr("Parameters") << tr("Enable");
  TableLayout * tableLayout = new TableLayout(this, num_fsw, headerLabels);

  for (int i=0; i<num_fsw; i++) {
    // The label
    QLabel * label = new QLabel(this);
    label->setContextMenuPolicy(Qt::CustomContextMenu);
    label->setMouseTracking(true);
    label->setProperty("index", i);
    if (model)
      label->setText(tr("SF%1").arg(i+1));
    else
      label->setText(tr("GF%1").arg(i+1));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    connect(label, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(fsw_customContextMenuRequested(QPoint)));
    tableLayout->addWidget(i, 0, label);
    // s1.report("label");

    // The switch
    fswtchSwtch[i] = new QComboBox(this);
    fswtchSwtch[i]->setModel(rawSwitchItemModel);
    fswtchSwtch[i]->setCurrentIndex(fswtchSwtch[i]->findData(functions[i].swtch.toValue()));
    fswtchSwtch[i]->setProperty("index", i);
    fswtchSwtch[i]->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    fswtchSwtch[i]->setMaxVisibleItems(10);
    connect(fswtchSwtch[i], SIGNAL(currentIndexChanged(int)), this, SLOT(customFunctionEdited()));
    tableLayout->addWidget(i, 1, fswtchSwtch[i]);
    // s1.report("switch");

    // The function
    fswtchFunc[i] = new QComboBox(this);
    if (!i) {
      populateFuncCB(fswtchFunc[i], functions[i].func);
    }
    else {
      fswtchFunc[i]->setModel(fswtchFunc[0]->model());
      fswtchFunc[i]->setCurrentIndex(fswtchFunc[i]->findData(functions[i].func));
    }
    fswtchFunc[i]->setProperty("index", i);
    connect(fswtchFunc[i], SIGNAL(currentIndexChanged(int)), this, SLOT(functionEdited()));
    tableLayout->addWidget(i, 2, fswtchFunc[i]);
    // s1.report("func");

    // The parameters
    QHBoxLayout * paramLayout = new QHBoxLayout();
    tableLayout->addLayout(i, 3, paramLayout);

    fswtchGVmode[i] = new QComboBox(this);
    fswtchGVmode[i]->setProperty("index", i);
    populateGVmodeCB(fswtchGVmode[i], functions[i].adjustMode);
    fswtchGVmode[i]->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(fswtchGVmode[i], SIGNAL(currentIndexChanged(int)), this, SLOT(customFunctionEdited()));
    paramLayout->addWidget(fswtchGVmode[i]);

    fswtchParamGV[i] = new QCheckBox(this);
    fswtchParamGV[i]->setProperty("index", i);
    fswtchParamGV[i]->setText("GV");
    fswtchParamGV[i]->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    connect(fswtchParamGV[i], SIGNAL(stateChanged(int)), this, SLOT(customFunctionEdited()));
    paramLayout->addWidget(fswtchParamGV[i]);

    fswtchParam[i] = new QDoubleSpinBox(this);
    fswtchParam[i]->setProperty("index", i);
    fswtchParam[i]->setAccelerated(true);
    fswtchParam[i]->setDecimals(0);
    connect(fswtchParam[i], SIGNAL(editingFinished()), this, SLOT(customFunctionEdited()));
    paramLayout->addWidget(fswtchParam[i]);

    fswtchParamTime[i] = new TimerEdit(this);
    fswtchParamTime[i]->setProperty("index", i);
    connect(fswtchParamTime[i], SIGNAL(editingFinished()), this, SLOT(customFunctionEdited()));
    paramLayout->addWidget(fswtchParamTime[i]);

    fswtchParamT[i] = new QComboBox(this);
    fswtchParamT[i]->setProperty("index", i);
    populateFuncParamCB(fswtchParamT[i], functions[i].func, functions[i].param, functions[i].adjustMode);
    paramLayout->addWidget(fswtchParamT[i]);
    fswtchParamT[i]->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(fswtchParamT[i], SIGNAL(currentIndexChanged(int)), this, SLOT(customFunctionEdited()));

    fswtchParamArmT[i] = new QComboBox(this);
    fswtchParamArmT[i]->setProperty("index", i);
    fswtchParamArmT[i]->setEditable(true);
    fswtchParamArmT[i]->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    paramLayout->addWidget(fswtchParamArmT[i]);

    connect(fswtchParamArmT[i], SIGNAL(currentIndexChanged(int)), this, SLOT(customFunctionEdited()));
    connect(fswtchParamArmT[i], SIGNAL(editTextChanged ( const QString)), this, SLOT(customFunctionEdited()));

    fswtchBLcolor[i] = new QSlider(this);
    fswtchBLcolor[i]->setProperty("index", i);
    fswtchBLcolor[i]->setMinimum(0);
    fswtchBLcolor[i]->setMaximum(100);
    fswtchBLcolor[i]->setSingleStep(1);
    fswtchBLcolor[i]->setOrientation(Qt::Horizontal);
    paramLayout->addWidget(fswtchBLcolor[i]);
    connect(fswtchBLcolor[i], SIGNAL(sliderReleased()), this, SLOT(customFunctionEdited()));

    playBT[i] = new QToolButton(this);
    playBT[i]->setProperty("index", i);
    playBT[i]->setIcon(playIcon);
    playBT[i]->setCheckable(true);
    paramLayout->addWidget(playBT[i]);
    connect(playBT[i], &QToolButton::clicked, this, &CustomFunctionsPanel::toggleSound);

    QHBoxLayout * repeatLayout = new QHBoxLayout();
    tableLayout->addLayout(i, 4, repeatLayout);
    fswtchRepeat[i] = new RepeatComboBox(this, functions[i].repeatParam);
    repeatLayout->addWidget(fswtchRepeat[i], i+1);
    connect(fswtchRepeat[i], SIGNAL(modified()), this, SLOT(onChildModified()));

    fswtchEnable[i] = new QCheckBox(this);
    fswtchEnable[i]->setProperty("index", i);
    fswtchEnable[i]->setText(tr("ON"));
    fswtchEnable[i]->setFixedWidth(200);
    repeatLayout->addWidget(fswtchEnable[i], i+1);
    connect(fswtchEnable[i], SIGNAL(stateChanged(int)), this, SLOT(customFunctionEdited()));
  }

  s1.report("add items");

  disableMouseScrolling();
  s1.report("disableMouseScrolling");

  lock = false;

  update();
  s1.report("update");
  tableLayout->resizeColumnsToContents();
  s1.report("resizeColumnsToContents");
  tableLayout->setColumnWidth(3, 300);
  tableLayout->pushRowsUp(num_fsw+1);
  s1.report("end");
}

CustomFunctionsPanel::~CustomFunctionsPanel()
{
  if (mediaPlayer)
    stopSound(mediaPlayerCurrent);
}

void CustomFunctionsPanel::setDataModels()
{
  if (rawSwitchItemModel)
    rawSwitchItemModel->update();
  else
    rawSwitchItemModel = new RawSwitchFilterItemModel(&generalSettings, model, model ? SpecialFunctionsContext : GlobalFunctionsContext);

  // The RawSource item models have to be reloaded on every update().  TODO: convert to filtered model like for RawSwitches

  if (rawSrcInputsItemModel)
    rawSrcInputsItemModel->deleteLater();
  if (rawSrcAllItemModel)
    rawSrcAllItemModel->deleteLater();

  rawSrcInputsItemModel = Helpers::getRawSourceItemModel(&generalSettings, model, POPULATE_NONE|POPULATE_SOURCES|POPULATE_VIRTUAL_INPUTS|POPULATE_TRIMS|POPULATE_SWITCHES);
  rawSrcInputsItemModel->setParent(this);
  rawSrcAllItemModel = Helpers::getRawSourceItemModel(&generalSettings, model, POPULATE_NONE|POPULATE_SOURCES|POPULATE_VIRTUAL_INPUTS|POPULATE_SWITCHES|POPULATE_GVARS|POPULATE_TRIMS|POPULATE_TELEMETRY|POPULATE_TELEMETRYEXT|POPULATE_SCRIPT_OUTPUTS);
  rawSrcAllItemModel->setParent(this);
}

void CustomFunctionsPanel::onMediaPlayerStateChanged(QMediaPlayer::State state)
{
  if (state != QMediaPlayer::PlayingState)
    stopSound(mediaPlayerCurrent);
}

void CustomFunctionsPanel::onMediaPlayerError(QMediaPlayer::Error error)
{
  stopSound(mediaPlayerCurrent);
  QMessageBox::critical(this, CPN_STR_TTL_ERROR, tr("Error occurred while trying to play sound, possibly the file is already opened. (Err: %1 [%2])").arg(mediaPlayer ? mediaPlayer->errorString() : "").arg(error));
}

bool CustomFunctionsPanel::playSound(int index)
{
  QString path = g.profile[g.id()].sdPath();
  if (!QDir(path).exists())
    return false;  // unlikely

  if (mediaPlayer)
    stopSound(mediaPlayerCurrent);

  if (firmware->getCapability(VoicesAsNumbers)) {  // AVR
    path.append(QString("/%1.wav").arg(int(fswtchParam[index]->value()), 4, 10, QChar('0')));
  }
  else {
    QString lang(generalSettings.ttsLanguage);
    if (lang.isEmpty())
      lang = "en";
    path.append(QString("/SOUNDS/%1/%2.wav").arg(lang).arg(fswtchParamArmT[index]->currentText()));
  }
  if (!QFileInfo::exists(path) || !QFileInfo(path).isReadable()) {
    QMessageBox::critical(this, CPN_STR_TTL_ERROR, tr("Unable to find or open sound file:\n%1").arg(path));
    return false;
  }

  mediaPlayer = new QMediaPlayer(this, QMediaPlayer::LowLatency);
  mediaPlayer->setMedia(QUrl::fromLocalFile(path));
  connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &CustomFunctionsPanel::onMediaPlayerStateChanged);
  connect(mediaPlayer, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error), this, &CustomFunctionsPanel::onMediaPlayerError);
  mediaPlayerCurrent = index;
  mediaPlayer->play();
  return true;
}

void CustomFunctionsPanel::stopSound(int index)
{
  if (index > -1 && index < (int)DIM(playBT))
    playBT[index]->setChecked(false);
  mediaPlayerCurrent = -1;
  if (mediaPlayer) {
    disconnect(mediaPlayer, 0, this, 0);
    mediaPlayer->stop();
    mediaPlayer->deleteLater();
    mediaPlayer = nullptr;
  }
}

void CustomFunctionsPanel::toggleSound(bool play)
{
  if (!sender() || !sender()->property("index").isValid())
    return;
  const int index = sender()->property("index").toInt();
  if (play)
    playBT[index]->setChecked(playSound(index));
  else
    stopSound(index);
}

#define CUSTOM_FUNCTION_NUMERIC_PARAM  (1<<0)
#define CUSTOM_FUNCTION_SOURCE_PARAM   (1<<1)
#define CUSTOM_FUNCTION_FILE_PARAM     (1<<2)
#define CUSTOM_FUNCTION_TIME_PARAM     (1<<3)
#define CUSTOM_FUNCTION_GV_MODE        (1<<4)
#define CUSTOM_FUNCTION_GV_TOOGLE      (1<<5)
#define CUSTOM_FUNCTION_ENABLE         (1<<6)
#define CUSTOM_FUNCTION_REPEAT         (1<<7)
#define CUSTOM_FUNCTION_PLAY           (1<<8)
#define CUSTOM_FUNCTION_BL_COLOR       (1<<9)
#define CUSTOM_FUNCTION_SHOW_FUNC      (1<<10)

void CustomFunctionsPanel::customFunctionEdited()
{
  if (!lock) {
    lock = true;
    int index = sender()->property("index").toInt();
    refreshCustomFunction(index, true);
    emit modified();
    lock = false;
  }
}

void CustomFunctionsPanel::functionEdited()
{
  if (!lock) {
    lock = true;
    int index = sender()->property("index").toInt();
    RawSwitch swtch = functions[index].swtch;
    functions[index].clear();
    functions[index].swtch = swtch;
    functions[index].func = (AssignFunc)fswtchFunc[index]->currentData().toInt();
    refreshCustomFunction(index);
    emit modified();
    lock = false;
  }
}

void CustomFunctionsPanel::onChildModified()
{
  emit modified();
}

void CustomFunctionsPanel::refreshCustomFunction(int i, bool modified)
{
    CustomFunctionData & cfn = functions[i];
    AssignFunc func = (AssignFunc)fswtchFunc[i]->currentData().toInt();

    unsigned int widgetsMask = 0;
    if (modified) {
      cfn.swtch = RawSwitch(fswtchSwtch[i]->currentData().toInt());
      cfn.func = func;
      cfn.enabled = fswtchEnable[i]->isChecked();
      cfn.adjustMode = (AssignFunc)fswtchGVmode[i]->currentIndex();
    }

    if (!cfn.isEmpty()) {
      widgetsMask |= CUSTOM_FUNCTION_SHOW_FUNC;

      if (func>=FuncOverrideCH1 && func<=FuncOverrideCH32) {
        if (model) {
          int channelsMax = model->getChannelsMax(true);
          fswtchParam[i]->setDecimals(0);
          fswtchParam[i]->setSingleStep(1);
          fswtchParam[i]->setMinimum(-channelsMax);
          fswtchParam[i]->setMaximum(channelsMax);
          if (modified) {
            cfn.param = fswtchParam[i]->value();
          }
          fswtchParam[i]->setValue(cfn.param);
          widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM | CUSTOM_FUNCTION_ENABLE;
        }
      }
      else if (func==FuncLogs) {
        fswtchParam[i]->setDecimals(1);
        fswtchParam[i]->setMinimum(0);
        fswtchParam[i]->setMaximum(25.5);
        fswtchParam[i]->setSingleStep(0.1);
        if (modified)
          cfn.param = fswtchParam[i]->value()*10.0;
        fswtchParam[i]->setValue(cfn.param/10.0);
        widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM;
      }
      else if (func>=FuncAdjustGV1 && func<=FuncAdjustGVLast) {
        int gvidx = func - FuncAdjustGV1;
        if (modified)
          cfn.adjustMode = fswtchGVmode[i]->currentIndex();
        widgetsMask |= CUSTOM_FUNCTION_GV_MODE | CUSTOM_FUNCTION_ENABLE;
        if (cfn.adjustMode==FUNC_ADJUST_GVAR_CONSTANT || cfn.adjustMode==FUNC_ADJUST_GVAR_INCDEC) {
          if (modified)
            cfn.param = fswtchParam[i]->value() * model->gvarData[gvidx].multiplierSet();
          if (IS_ARM(getCurrentBoard())) {
            fswtchParam[i]->setDecimals(model->gvarData[gvidx].prec);
            fswtchParam[i]->setSingleStep(model->gvarData[gvidx].multiplierGet());
            fswtchParam[i]->setSuffix(model->gvarData[gvidx].unitToString());
            fswtchParam[i]->setMinimum(model->gvarData[gvidx].getMinPrec());
            fswtchParam[i]->setMaximum(model->gvarData[gvidx].getMaxPrec());
            fswtchParam[i]->setValue(cfn.param * model->gvarData[gvidx].multiplierGet());
          }
          else {
            fswtchParam[i]->setDecimals(0);
            fswtchParam[i]->setSingleStep(1);
            fswtchParam[i]->setMinimum(-125);
            fswtchParam[i]->setMaximum(125);
            fswtchParam[i]->setValue(cfn.param);
          }
          widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM;
        }
        else {
          if (modified)
            cfn.param = fswtchParamT[i]->currentData().toInt();
          populateFuncParamCB(fswtchParamT[i], func, cfn.param, cfn.adjustMode);
          widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
        }
      }
      else if (func==FuncReset) {
        if (modified)
          cfn.param = fswtchParamT[i]->currentData().toInt();
        populateFuncParamCB(fswtchParamT[i], func, cfn.param);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM | CUSTOM_FUNCTION_ENABLE;
      }
      else if (func>=FuncSetTimer1 && func<=FuncSetTimer3) {
        if (modified)
          cfn.param = fswtchParamTime[i]->timeInSeconds();
        RawSourceRange range = RawSource(SOURCE_TYPE_SPECIAL, func - FuncSetTimer1 + 2).getRange(model, generalSettings);
        fswtchParamTime[i]->setTimeRange((int)range.min, (int)range.max);
        fswtchParamTime[i]->setTime(cfn.param);
        widgetsMask |= CUSTOM_FUNCTION_TIME_PARAM | CUSTOM_FUNCTION_ENABLE;
      }
      else if (func>=FuncSetFailsafeInternalModule && func<=FuncBindExternalModule) {
        widgetsMask |= CUSTOM_FUNCTION_ENABLE;
      }
      else if (func==FuncVolume) {
        if (modified)
          cfn.param = fswtchParamT[i]->currentData().toInt();
        populateFuncParamCB(fswtchParamT[i], func, cfn.param);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM | CUSTOM_FUNCTION_ENABLE;
      }
      else if (func==FuncPlaySound || func==FuncPlayHaptic || func==FuncPlayValue || func==FuncPlayPrompt || func==FuncPlayBoth || func==FuncBackgroundMusic) {
        if (func != FuncBackgroundMusic) {
          widgetsMask |= CUSTOM_FUNCTION_REPEAT;
          fswtchRepeat[i]->update();
        }
        if (func==FuncPlayValue) {
          if (modified)
            cfn.param = fswtchParamT[i]->currentData().toInt();
          populateFuncParamCB(fswtchParamT[i], func, cfn.param);
          widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM | CUSTOM_FUNCTION_REPEAT;
        }
        else if (func==FuncPlayPrompt || func==FuncPlayBoth) {
          if (firmware->getCapability(VoicesAsNumbers)) {
            fswtchParam[i]->setDecimals(0);
            fswtchParam[i]->setSingleStep(1);
            fswtchParam[i]->setMinimum(0);
            if (func==FuncPlayPrompt) {
              widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM | CUSTOM_FUNCTION_REPEAT | CUSTOM_FUNCTION_GV_TOOGLE;
            }
            else {
              widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM | CUSTOM_FUNCTION_REPEAT;
              fswtchParamGV[i]->setChecked(false);
            }
            fswtchParam[i]->setMaximum(func==FuncPlayBoth ? 254 : 255);
            if (modified) {
              if (fswtchParamGV[i]->isChecked()) {
                fswtchParam[i]->setMinimum(1);
                cfn.param = std::min(fswtchParam[i]->value(),5.0)+(fswtchParamGV[i]->isChecked() ? 250 : 0);
              }
              else {
                cfn.param = fswtchParam[i]->value();
              }
            }
            if (cfn.param>250 && (func!=FuncPlayBoth)) {
              fswtchParamGV[i]->setChecked(true);
              fswtchParam[i]->setValue(cfn.param-250);
              fswtchParam[i]->setMaximum(5);
            }
            else {
              fswtchParamGV[i]->setChecked(false);
              fswtchParam[i]->setValue(cfn.param);
            }
            if (cfn.param < 251)
              widgetsMask |= CUSTOM_FUNCTION_PLAY;
          }
          else {
            widgetsMask |= CUSTOM_FUNCTION_FILE_PARAM;
            if (modified) {
              Helpers::getFileComboBoxValue(fswtchParamArmT[i], cfn.paramarm, firmware->getCapability(VoicesMaxLength));
            }
            Helpers::populateFileComboBox(fswtchParamArmT[i], tracksSet, cfn.paramarm);
            if (fswtchParamArmT[i]->currentText() != "----") {
              widgetsMask |= CUSTOM_FUNCTION_PLAY;
            }
          }
        }
        else if (func==FuncBackgroundMusic) {
          widgetsMask |= CUSTOM_FUNCTION_FILE_PARAM;
          if (modified) {
            Helpers::getFileComboBoxValue(fswtchParamArmT[i], cfn.paramarm, firmware->getCapability(VoicesMaxLength));
          }
          Helpers::populateFileComboBox(fswtchParamArmT[i], tracksSet, cfn.paramarm);
          if (fswtchParamArmT[i]->currentText() != "----") {
            widgetsMask |= CUSTOM_FUNCTION_PLAY;
          }
        }
        else if (func==FuncPlaySound) {
          if (modified)
            cfn.param = (uint8_t)fswtchParamT[i]->currentIndex();
          populateFuncParamCB(fswtchParamT[i], func, cfn.param);
          widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
        }
        else if (func==FuncPlayHaptic) {
          if (modified)
            cfn.param = (uint8_t)fswtchParamT[i]->currentIndex();
          populateFuncParamCB(fswtchParamT[i], func, cfn.param);
          widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
        }
      }
      else if (func==FuncPlayScript) {
        widgetsMask |= CUSTOM_FUNCTION_FILE_PARAM;
        if (modified) {
          Helpers::getFileComboBoxValue(fswtchParamArmT[i], cfn.paramarm, 8);
        }
        Helpers::populateFileComboBox(fswtchParamArmT[i], scriptsSet, cfn.paramarm);
      }
      else if (func==FuncBacklight && IS_TARANIS_PLUS(getCurrentBoard())) {
        if (modified)
          cfn.param = (uint8_t)fswtchBLcolor[i]->value();
        fswtchBLcolor[i]->setValue(cfn.param);
        widgetsMask |= CUSTOM_FUNCTION_BL_COLOR;
      }
      else {
        if (modified)
          cfn.param = fswtchParam[i]->value();
        fswtchParam[i]->setDecimals(0);
        fswtchParam[i]->setSingleStep(1);
        fswtchParam[i]->setValue(cfn.param);
        if (func <= FuncInstantTrim) {
          widgetsMask |= CUSTOM_FUNCTION_ENABLE;
        }
      }

    }

    fswtchFunc[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_SHOW_FUNC);
    fswtchParam[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_NUMERIC_PARAM);
    fswtchParamTime[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_TIME_PARAM);
    fswtchParamGV[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_GV_TOOGLE);
    fswtchParamT[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_SOURCE_PARAM);
    fswtchParamArmT[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_FILE_PARAM);
    fswtchEnable[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_ENABLE);
    if (widgetsMask & CUSTOM_FUNCTION_ENABLE)
      fswtchEnable[i]->setChecked(cfn.enabled);
    else
      fswtchEnable[i]->setChecked(false);
    fswtchRepeat[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_REPEAT);
    fswtchGVmode[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_GV_MODE);
    fswtchBLcolor[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_BL_COLOR);
    playBT[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_PLAY);
}

void CustomFunctionsPanel::update()
{
  setDataModels();

  lock = true;
  int num_fsw = model ? firmware->getCapability(CustomFunctions) : firmware->getCapability(GlobalFunctions);
  for (int i=0; i<num_fsw; i++) {
    refreshCustomFunction(i);
  }
  lock = false;
}

void CustomFunctionsPanel::fswPaste()
{
  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();
  if (mimeData->hasFormat("application/x-companion-fsw")) {
    QByteArray fswData = mimeData->data("application/x-companion-fsw");
    CustomFunctionData *fsw = &functions[selectedFunction];
    memcpy(fsw, fswData.constData(), sizeof(CustomFunctionData));
    lock = true;
    fswtchSwtch[selectedFunction]->setCurrentIndex(fswtchSwtch[selectedFunction]->findData(functions[selectedFunction].swtch.toValue()));
    fswtchFunc[selectedFunction]->setCurrentIndex(fswtchFunc[selectedFunction]->findData(functions[selectedFunction].func));
    populateGVmodeCB(fswtchGVmode[selectedFunction], functions[selectedFunction].adjustMode);
    populateFuncParamCB(fswtchParamT[selectedFunction], functions[selectedFunction].func, functions[selectedFunction].param, functions[selectedFunction].adjustMode);
    refreshCustomFunction(selectedFunction);
    lock = false;
    emit modified();
  }
}

void CustomFunctionsPanel::fswDelete()
{
  functions[selectedFunction].clear();
  // TODO update switch and func
  lock = true;
  fswtchSwtch[selectedFunction]->setCurrentIndex(fswtchSwtch[selectedFunction]->findData(functions[selectedFunction].swtch.toValue()));
  fswtchFunc[selectedFunction]->setCurrentIndex(fswtchFunc[selectedFunction]->findData(functions[selectedFunction].func));
  refreshCustomFunction(selectedFunction);
  lock = false;
  emit modified();
}

void CustomFunctionsPanel::fswCopy()
{
  QByteArray fswData;
  fswData.append((char*)&functions[selectedFunction], sizeof(CustomFunctionData));
  QMimeData *mimeData = new QMimeData;
  mimeData->setData("application/x-companion-fsw", fswData);
  QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);
}

void CustomFunctionsPanel::fswCut()
{
  fswCopy();
  fswDelete();
}

void CustomFunctionsPanel::fsw_customContextMenuRequested(QPoint pos)
{
    QLabel *label = (QLabel *)sender();
    selectedFunction = label->property("index").toInt();

    QPoint globalPos = label->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion-fsw");

    QMenu contextMenu;
    contextMenu.addAction(CompanionIcon("copy.png"), tr("&Copy"),this,SLOT(fswCopy()));
    contextMenu.addAction(CompanionIcon("cut.png"), tr("&Cut"),this,SLOT(fswCut()));
    contextMenu.addAction(CompanionIcon("paste.png"), tr("&Paste"),this,SLOT(fswPaste()))->setEnabled(hasData);
    contextMenu.addAction(CompanionIcon("clear.png"), tr("&Delete"),this,SLOT(fswDelete()));

    contextMenu.exec(globalPos);
}

void CustomFunctionsPanel::populateFuncCB(QComboBox *b, unsigned int value)
{
  b->clear();
  for (unsigned int i=0; i<FuncCount; i++) {
    if (((i>=FuncOverrideCH1 && i<=FuncOverrideCH32) && (!model || !firmware->getCapability(SafetyChannelCustomFunction))) ||
        ((i==FuncVolume || i==FuncBackgroundMusic || i==FuncBackgroundMusicPause) && !firmware->getCapability(HasVolume)) ||
        ((i==FuncPlayScript && !IS_HORUS_OR_TARANIS(firmware->getBoard()))) ||
        ((i==FuncPlayHaptic) && !firmware->getCapability(Haptic)) ||
        ((i==FuncPlayBoth) && !firmware->getCapability(HasBeeper)) ||
        ((i==FuncLogs) && !firmware->getCapability(HasSDLogs)) ||
        ((i==FuncSetTimer1 || i==FuncSetTimer2) && !IS_ARM(firmware->getBoard())) ||
        ((i==FuncSetTimer3) && firmware->getCapability(Timers) < 3) ||
        ((i==FuncScreenshot) && !IS_TARANIS(firmware->getBoard())) ||
        ((i>=FuncRangeCheckInternalModule && i<=FuncBindExternalModule) && (!model || !firmware->getCapability(DangerousFunctions))) ||
        ((i>=FuncAdjustGV1 && i<=FuncAdjustGVLast) && (!model || !firmware->getCapability(Gvars)))
        ) {
      // skipped
      continue;
    }
    else {
      b->addItem(CustomFunctionData(AssignFunc(i)).funcToString(model), i);
      if (i == value) {
        b->setCurrentIndex(b->count()-1);
      }
    }
  }
}

void CustomFunctionsPanel::populateGVmodeCB(QComboBox *b, unsigned int value)
{
  b->clear();
  b->addItem(tr("Value"));
  b->addItem(tr("Source"));
  b->addItem(tr("GVAR"));
  b->addItem(tr("Increment"));
  b->setCurrentIndex(value);
}

void CustomFunctionsPanel::populateFuncParamCB(QComboBox *b, uint function, unsigned int value, unsigned int adjustmode)
{
  QStringList qs;
  b->setModel(new QStandardItemModel(b));  // clear combo box but not any shared item model
  if (function==FuncPlaySound) {
    CustomFunctionData::populatePlaySoundParams(qs);
    b->addItems(qs);
    b->setCurrentIndex(value);
  }
  else if (function==FuncPlayHaptic) {
    CustomFunctionData::populateHapticParams(qs);
    b->addItems(qs);
    b->setCurrentIndex(value);
  }
  else if (function==FuncReset) {
    CustomFunctionData::populateResetParams(model, b, value);
  }
  else if (function==FuncVolume) {
    b->setModel(rawSrcInputsItemModel);
    b->setCurrentIndex(b->findData(value));
  }
  else if (function==FuncPlayValue) {
    b->setModel(rawSrcAllItemModel);
    b->setCurrentIndex(b->findData(value));
  }
  else if (function>=FuncAdjustGV1 && function<=FuncAdjustGVLast) {
    switch (adjustmode) {
      case 1:
        b->setModel(rawSrcInputsItemModel);
        b->setCurrentIndex(b->findData(value));
        break;
      case 2:
        b->setModel(Helpers::getRawSourceItemModel(&generalSettings, model, POPULATE_GVARS));
        b->setCurrentIndex(b->findData(value));
        break;
    }
  }
  else {
    b->hide();
  }
}
