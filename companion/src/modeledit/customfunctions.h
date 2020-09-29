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

#ifndef _CUSTOMFUNCTIONS_H_
#define _CUSTOMFUNCTIONS_H_

#include "modeledit.h"
#include "eeprominterface.h"

#include <QMediaPlayer>

class CommonItemModels;
class RawItemFilteredModel;
class TimerEdit;

constexpr char MIMETYPE_CUSTOM_FUNCTION[] = "application/x-companion-custom-function";

class RepeatComboBox: public QComboBox
{
    Q_OBJECT

  public:
    RepeatComboBox(QWidget * parent, int & repeatParam);
    void update();

  signals:
    void modified();

  private slots:
    void onIndexChanged(int);

  protected:
    int & repeatParam;
};

class CustomFunctionsPanel : public GenericPanel
{
  Q_OBJECT

  public:
    CustomFunctionsPanel(QWidget *parent, ModelData * model, GeneralSettings & generalSettings, Firmware * firmware, CommonItemModels * commonItemModels);
    ~CustomFunctionsPanel();

    virtual void update();

  protected:
    CustomFunctionData * functions;

  private slots:
    void customFunctionEdited();
    void functionEdited();
    void onCustomContextMenuRequested(QPoint pos);
    void refreshCustomFunction(int index, bool modified=false);
    void onRepeatModified();
    bool playSound(int index);
    void stopSound(int index);
    void toggleSound(bool play);
    void onMediaPlayerStateChanged(QMediaPlayer::State state);
    void onMediaPlayerError(QMediaPlayer::Error error);
    void cmDelete();
    void cmCopy();
    void cmPaste();
    void cmCut();
    void cmMoveUp();
    void cmMoveDown();
    void cmInsert();
    void cmClear(bool prompt = true);
    void cmClearAll();
    void onModelDataAboutToBeUpdated();
    void onModelDataUpdateComplete();

  private:
    void populateFuncCB(QComboBox *b, unsigned int value);
    void populateGVmodeCB(QComboBox *b, unsigned int value);
    void populateFuncParamCB(QComboBox *b, uint function, unsigned int value, unsigned int adjustmode=0);
    bool hasClipboardData(QByteArray * data = nullptr) const;
    bool insertAllowed() const;
    bool moveDownAllowed() const;
    bool moveUpAllowed() const;
    void swapData(int idx1, int idx2);
    void resetCBsAndRefresh(int idx);
    CommonItemModels * commonItemModels;
    RawItemFilteredModel * rawSwitchFilteredModel;
    RawItemFilteredModel * rawSourceAllModel;
    RawItemFilteredModel * rawSourceInputsModel;
    RawItemFilteredModel * rawSourceGVarsModel;

    QSet<QString> tracksSet;
    QSet<QString> scriptsSet;
    int mediaPlayerCurrent;
    QComboBox * fswtchSwtch[CPN_MAX_SPECIAL_FUNCTIONS];
    QComboBox * fswtchFunc[CPN_MAX_SPECIAL_FUNCTIONS];
    QCheckBox * fswtchParamGV[CPN_MAX_SPECIAL_FUNCTIONS];
    QDoubleSpinBox * fswtchParam[CPN_MAX_SPECIAL_FUNCTIONS];
    TimerEdit * fswtchParamTime[CPN_MAX_SPECIAL_FUNCTIONS];
    QToolButton * playBT[CPN_MAX_SPECIAL_FUNCTIONS];
    QComboBox * fswtchParamT[CPN_MAX_SPECIAL_FUNCTIONS];
    QComboBox * fswtchParamArmT[CPN_MAX_SPECIAL_FUNCTIONS];
    QCheckBox * fswtchEnable[CPN_MAX_SPECIAL_FUNCTIONS];
    RepeatComboBox * fswtchRepeat[CPN_MAX_SPECIAL_FUNCTIONS];
    QComboBox * fswtchGVmode[CPN_MAX_SPECIAL_FUNCTIONS];
    QSlider * fswtchBLcolor[CPN_MAX_SPECIAL_FUNCTIONS];
    QMediaPlayer * mediaPlayer;

    int selectedIndex;
    int fswCapability;
    int modelsUpdateCnt;
};

#endif // _CUSTOMFUNCTIONS_H_
