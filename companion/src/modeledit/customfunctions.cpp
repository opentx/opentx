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
#include "helpers.h"
#include "appdata.h"

RepeatComboBox::RepeatComboBox(QWidget *parent, int & repeatParam):
  QComboBox(parent),
  repeatParam(repeatParam)
{
  unsigned int step = IS_ARM(GetEepromInterface()->getBoard()) ? 1 : 10;
  int value = repeatParam/step;

  if (step == 1) {
    addItem(QObject::tr("Played once, not during startup"), -1);
    value++;
  }

  addItem(QObject::tr("No repeat"), 0);

  for (unsigned int i=step; i<=60; i+=step) {
    addItem(QObject::tr("%1s").arg(i), i);
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
  unsigned int step = IS_ARM(GetEepromInterface()->getBoard()) ? 1 : 10;
  int value = repeatParam/step;
  if (step == 1) {
    value++;
  }
  setCurrentIndex(value);
}

CustomFunctionsPanel::CustomFunctionsPanel(QWidget * parent, ModelData * model, GeneralSettings & generalSettings, Firmware * firmware):
  GenericPanel(parent, model, generalSettings, firmware),
  functions(model ? model->customFn : generalSettings.customFn),
  mediaPlayerCurrent(-1),
  mediaPlayer(NULL)
{
  Stopwatch s1("CustomFunctionsPanel - populate"); 
  lock = true;
  int num_fsw = model ? firmware->getCapability(CustomFunctions) : firmware->getCapability(GlobalFunctions);

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

  if (IS_TARANIS(firmware->getBoard())) {
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
    fswtchSwtch[i]->setProperty("index", i);
    populateSwitchCB(fswtchSwtch[i], functions[i].swtch, generalSettings, model ? SpecialFunctionsContext : GlobalFunctionsContext);
    fswtchSwtch[i]->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    fswtchSwtch[i]->setMaxVisibleItems(10);
    connect(fswtchSwtch[i], SIGNAL(currentIndexChanged(int)), this, SLOT(customFunctionEdited()));
    tableLayout->addWidget(i, 1, fswtchSwtch[i]);
    // s1.report("switch");

    // The function
    fswtchFunc[i] = new QComboBox(this);
    fswtchFunc[i]->setProperty("index", i);
    populateFuncCB(fswtchFunc[i], functions[i].func);
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

    fswtchParamTime[i] = new QTimeEdit(this);
    fswtchParamTime[i]->setProperty("index", i);
    fswtchParamTime[i]->setAccelerated(true);
    fswtchParamTime[i]->setDisplayFormat("hh:mm:ss");
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

    playBT[i] = new QPushButton(this);
    playBT[i]->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    playBT[i]->setProperty("index", i);
    playBT[i]->setIcon(playIcon);
    paramLayout->addWidget(playBT[i]);
    connect(playBT[i], SIGNAL(pressed()), this, SLOT(playMusic()));

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
}

void CustomFunctionsPanel::onMediaPlayerStateChanged(QMediaPlayer::State state)
{
  if (!lock) {
    lock = true;
    if (state==QMediaPlayer::StoppedState || state==QMediaPlayer::PausedState) {
      mediaPlayer->stop();
      if (mediaPlayerCurrent >= 0) {
        playBT[mediaPlayerCurrent]->setIcon(CompanionIcon("play.png"));
        mediaPlayerCurrent = -1;
      }
    }
    lock = false;
  }
}

void CustomFunctionsPanel::onMediaPlayerError(QMediaPlayer::Error error)
{
  if (!lock) {
    lock = true;
    if (mediaPlayerCurrent >= 0) {
      playBT[mediaPlayerCurrent]->setIcon(CompanionIcon("play.png"));
      mediaPlayerCurrent = -1;
    }
    lock = false;
  }
}

void CustomFunctionsPanel::playMusic()
{
  if (!mediaPlayer) {
    mediaPlayer = new QMediaPlayer(this);
    connect(mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(onMediaPlayerStateChanged(QMediaPlayer::State)));
    connect(mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onMediaPlayerError(QMediaPlayer::Error)));
  }

  int index = sender()->property("index").toInt();
  QString path = g.profile[g.id()].sdPath();
  QDir qd(path);
  QString track;
  if (qd.exists()) {
    if (firmware->getCapability(VoicesAsNumbers)) {
      track = path + QString("/%1.wav").arg(int(fswtchParam[index]->value()), 4, 10, (const QChar)'0');
    }
    else {
      path.append("/SOUNDS/");
      QString lang = generalSettings.ttsLanguage;
      if (lang.isEmpty())
        lang = "en";
      path.append(lang);
      if (fswtchParamArmT[index]->currentText() != "----") {
        track = path + "/" + fswtchParamArmT[index]->currentText() + ".wav";
      }
    }
    QFile file(track);
    if (!file.exists()) {
      QMessageBox::critical(this, tr("Error"), tr("Unable to find sound file %1!").arg(track));
      return;
    }

    if (mediaPlayerCurrent == index) {
      mediaPlayer->stop();
      playBT[index]->setIcon(CompanionIcon("play.png"));
      mediaPlayerCurrent = -1;
    }
    else {
      if (mediaPlayerCurrent >= 0) {
        playBT[mediaPlayerCurrent]->setIcon(CompanionIcon("play.png"));
      }
      mediaPlayerCurrent = index;
      mediaPlayer->setMedia(QUrl::fromLocalFile(track));
      mediaPlayer->play();
      playBT[index]->setIcon(CompanionIcon("stop.png"));
    }
  }
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
    fswtchParamArmT[index]->setCurrentIndex(0);
    refreshCustomFunction(index, true);
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
    AssignFunc func = (AssignFunc)fswtchFunc[i]->itemData(fswtchFunc[i]->currentIndex()).toInt();

    unsigned int widgetsMask = 0;
    if (modified) {
      cfn.swtch = RawSwitch(fswtchSwtch[i]->itemData(fswtchSwtch[i]->currentIndex()).toInt());
      cfn.func = func;
      cfn.enabled = fswtchEnable[i]->isChecked();
      cfn.adjustMode = (AssignFunc)fswtchGVmode[i]->currentIndex();
    }

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
        widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM + CUSTOM_FUNCTION_ENABLE;
      }
    }
    else if (func==FuncLogs) {
      fswtchParam[i]->setDecimals(1);
      fswtchParam[i]->setMinimum(0);
      fswtchParam[i]->setMaximum(25.5);
      fswtchParam[i]->setSingleStep(0.1);
      if (modified) cfn.param = fswtchParam[i]->value()*10.0;
      fswtchParam[i]->setValue(cfn.param/10.0);
      widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM;
    }
    else if (func>=FuncAdjustGV1 && func<=FuncAdjustGVLast) {
      if (modified) cfn.adjustMode = fswtchGVmode[i]->currentIndex();
      widgetsMask |= CUSTOM_FUNCTION_GV_MODE | CUSTOM_FUNCTION_ENABLE;
      if (cfn.adjustMode==FUNC_ADJUST_GVAR_CONSTANT || cfn.adjustMode==FUNC_ADJUST_GVAR_INCDEC) {
        if (modified) cfn.param = fswtchParam[i]->value();
        fswtchParam[i]->setDecimals(0);
        fswtchParam[i]->setSingleStep(1);
        if (IS_ARM(GetEepromInterface()->getBoard())) {
          fswtchParam[i]->setMinimum(-500);
          fswtchParam[i]->setMaximum(500);
        }
        else {
          fswtchParam[i]->setMinimum(-125);
          fswtchParam[i]->setMaximum(125);
        }
        fswtchParam[i]->setValue(cfn.param);
        widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM;
      }
      else {
        if (modified) cfn.param = fswtchParamT[i]->itemData(fswtchParamT[i]->currentIndex()).toInt();
        populateFuncParamCB(fswtchParamT[i], func, cfn.param, cfn.adjustMode);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
      }
    }
    else if (func==FuncReset) {
      if (modified) cfn.param = fswtchParamT[i]->itemData(fswtchParamT[i]->currentIndex()).toInt();
      populateFuncParamCB(fswtchParamT[i], func, cfn.param);
      widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM | CUSTOM_FUNCTION_ENABLE;
    }
    else if (func>=FuncSetTimer1 && func<=FuncSetTimer3) {
      if (modified) cfn.param = QTimeS(fswtchParamTime[i]->time()).seconds();
      fswtchParamTime[i]->setMinimumTime(QTime(0, 0, 0));
      fswtchParamTime[i]->setMaximumTime(firmware->getMaxTimerStart());
      fswtchParamTime[i]->setTime(QTimeS(cfn.param));
      widgetsMask |= CUSTOM_FUNCTION_TIME_PARAM + CUSTOM_FUNCTION_ENABLE;
    }
    else if (func>=FuncSetFailsafeInternalModule && func<=FuncBindExternalModule) {
      widgetsMask |= CUSTOM_FUNCTION_ENABLE;
    }
    else if (func==FuncVolume) {
      if (modified) cfn.param = fswtchParamT[i]->itemData(fswtchParamT[i]->currentIndex()).toInt();
      populateFuncParamCB(fswtchParamT[i], func, cfn.param);
      widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM + CUSTOM_FUNCTION_ENABLE;
    }
    else if (func==FuncPlaySound || func==FuncPlayHaptic || func==FuncPlayValue || func==FuncPlayPrompt || func==FuncPlayBoth || func==FuncBackgroundMusic) {
      if (func != FuncBackgroundMusic) {
        widgetsMask |= CUSTOM_FUNCTION_REPEAT;
        fswtchRepeat[i]->update();
      }
      if (func==FuncPlayValue) {
        if (modified) cfn.param = fswtchParamT[i]->itemData(fswtchParamT[i]->currentIndex()).toInt();
        populateFuncParamCB(fswtchParamT[i], func, cfn.param);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM + CUSTOM_FUNCTION_REPEAT;
      }
      else if (func==FuncPlayPrompt || func==FuncPlayBoth) {
        if (firmware->getCapability(VoicesAsNumbers)) {
          fswtchParam[i]->setDecimals(0);
          fswtchParam[i]->setSingleStep(1);
          fswtchParam[i]->setMinimum(0);
          if (func==FuncPlayPrompt) {
            widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM + CUSTOM_FUNCTION_REPEAT + CUSTOM_FUNCTION_GV_TOOGLE;
          }
          else {
            widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM + CUSTOM_FUNCTION_REPEAT;
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
            getFileComboBoxValue(fswtchParamArmT[i], cfn.paramarm, firmware->getCapability(VoicesMaxLength));
          }
          populateFileComboBox(fswtchParamArmT[i], tracksSet, cfn.paramarm);
          if (fswtchParamArmT[i]->currentText() != "----") {
            widgetsMask |= CUSTOM_FUNCTION_PLAY;
          }
        }
      }
      else if (func==FuncBackgroundMusic) {
        widgetsMask |= CUSTOM_FUNCTION_FILE_PARAM;
        if (modified) {
          getFileComboBoxValue(fswtchParamArmT[i], cfn.paramarm, firmware->getCapability(VoicesMaxLength));
        }
        populateFileComboBox(fswtchParamArmT[i], tracksSet, cfn.paramarm);
        if (fswtchParamArmT[i]->currentText() != "----") {
          widgetsMask |= CUSTOM_FUNCTION_PLAY;
        }
      }
      else if (func==FuncPlaySound) {
        if (modified) cfn.param = (uint8_t)fswtchParamT[i]->currentIndex();
        populateFuncParamCB(fswtchParamT[i], func, cfn.param);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
      }
      else if (func==FuncPlayHaptic) {
        if (modified) cfn.param = (uint8_t)fswtchParamT[i]->currentIndex();
        populateFuncParamCB(fswtchParamT[i], func, cfn.param);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
      }
    }
    else if (func==FuncPlayScript) {
      widgetsMask |= CUSTOM_FUNCTION_FILE_PARAM;
      if (modified) {
        getFileComboBoxValue(fswtchParamArmT[i], cfn.paramarm, 8);
      }
      populateFileComboBox(fswtchParamArmT[i], scriptsSet, cfn.paramarm);
    }
    else if (func==FuncBacklight && IS_TARANIS_PLUS(GetEepromInterface()->getBoard())) {
      if (modified) cfn.param = (uint8_t)fswtchBLcolor[i]->value();
      fswtchBLcolor[i]->setValue(cfn.param);
      widgetsMask |= CUSTOM_FUNCTION_BL_COLOR;
    }
    else {
      if (modified) cfn.param = fswtchParam[i]->value();
      fswtchParam[i]->setDecimals(0);
      fswtchParam[i]->setSingleStep(1);
      fswtchParam[i]->setValue(cfn.param);
      if (func <= FuncInstantTrim) {
        widgetsMask |= CUSTOM_FUNCTION_ENABLE;
      }
    }

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
    populateSwitchCB(fswtchSwtch[selectedFunction], functions[selectedFunction].swtch, generalSettings, model ? SpecialFunctionsContext : GlobalFunctionsContext);
    populateFuncCB(fswtchFunc[selectedFunction], functions[selectedFunction].func);
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
  populateSwitchCB(fswtchSwtch[selectedFunction], functions[selectedFunction].swtch, generalSettings, model ? SpecialFunctionsContext : GlobalFunctionsContext);
  populateFuncCB(fswtchFunc[selectedFunction], functions[selectedFunction].func);
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
    contextMenu.addAction(CompanionIcon("copy.png"), tr("&Copy"),this,SLOT(fswCopy()),tr("Ctrl+C"));
    contextMenu.addAction(CompanionIcon("cut.png"), tr("&Cut"),this,SLOT(fswCut()),tr("Ctrl+X"));
    contextMenu.addAction(CompanionIcon("paste.png"), tr("&Paste"),this,SLOT(fswPaste()),tr("Ctrl+V"))->setEnabled(hasData);
    contextMenu.addAction(CompanionIcon("clear.png"), tr("&Delete"),this,SLOT(fswDelete()),tr("Delete"));

    contextMenu.exec(globalPos);
}

void CustomFunctionsPanel::populateFuncCB(QComboBox *b, unsigned int value)
{
  b->clear();
  for (unsigned int i=0; i<FuncCount; i++) {
    if (((i>=FuncOverrideCH1 && i<=FuncOverrideCH32) && (!model || !firmware->getCapability(SafetyChannelCustomFunction))) ||
        ((i==FuncVolume || i==FuncBackgroundMusic || i==FuncBackgroundMusicPause) && !firmware->getCapability(HasVolume)) ||
        ((i==FuncPlayScript && !IS_TARANIS(firmware->getBoard()))) ||
        ((i==FuncPlayHaptic) && !firmware->getCapability(Haptic)) ||
        ((i==FuncPlayBoth) && !firmware->getCapability(HasBeeper)) ||
        ((i==FuncLogs) && !firmware->getCapability(HasSDLogs)) ||
        ((i==FuncSetTimer3) && firmware->getCapability(Timers) < 3) ||
        ((i>=FuncRangeCheckInternalModule && i<=FuncBindExternalModule) && (!model || !firmware->getCapability(DangerousFunctions))) ||
        ((i>=FuncAdjustGV1 && i<=FuncAdjustGVLast) && !firmware->getCapability(Gvars))
        ) {
      // skipped
      // b->addItem(CustomFunctionData(AssignFunc(i)).funcToString(), i);
      // QModelIndex index = b->model()->index(i, 0);
      // QVariant v(0);
      // b->model()->setData(index, v, Qt::UserRole - 1);
    }
    else {
      b->addItem(CustomFunctionData(AssignFunc(i)).funcToString(), i);
      if (i == value) {
        b->setCurrentIndex(b->count()-1);
      }
    }
  }
}

void CustomFunctionsPanel::populateGVmodeCB(QComboBox *b, unsigned int value)
{
  b->clear();
  b->addItem(QObject::tr("Value"));
  b->addItem(QObject::tr("Source"));
  b->addItem(QObject::tr("GVAR"));
  b->addItem(QObject::tr("Increment"));
  b->setCurrentIndex(value);
}

void CustomFunctionsPanel::populateFuncParamCB(QComboBox *b, uint function, unsigned int value, unsigned int adjustmode)
{
  QStringList qs;
  b->clear();
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
    populateSourceCB(b, RawSource(value), generalSettings, model, POPULATE_NONE|POPULATE_SOURCES|POPULATE_VIRTUAL_INPUTS|POPULATE_TRIMS|POPULATE_SWITCHES);
  }
  else if (function==FuncPlayValue) {
    populateSourceCB(b, RawSource(value), generalSettings, model, POPULATE_NONE|POPULATE_SOURCES|POPULATE_VIRTUAL_INPUTS|POPULATE_SWITCHES|POPULATE_GVARS|POPULATE_TRIMS|POPULATE_TELEMETRY|POPULATE_TELEMETRYEXT|POPULATE_SCRIPT_OUTPUTS);
  }
  else if (function>=FuncAdjustGV1 && function<=FuncAdjustGVLast) {
    switch (adjustmode) {
      case 1:
        populateSourceCB(b, RawSource(value), generalSettings, model, POPULATE_NONE|POPULATE_SOURCES|POPULATE_VIRTUAL_INPUTS|POPULATE_TRIMS|POPULATE_SWITCHES);
        break;
      case 2:
        populateSourceCB(b, RawSource(value), generalSettings, model, POPULATE_GVARS);
        break;
    }
  }
  else {
    b->hide();
  }
}
