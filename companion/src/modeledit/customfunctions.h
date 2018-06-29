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

class RawSwitchFilterItemModel;
class TimerEdit;

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
    CustomFunctionsPanel(QWidget *parent, ModelData * mode, GeneralSettings & generalSettings, Firmware * firmware);
    ~CustomFunctionsPanel();

    virtual void update();

  protected:
    CustomFunctionData * functions;

  private slots:
    void setDataModels();
    void customFunctionEdited();
    void functionEdited();
    void fsw_customContextMenuRequested(QPoint pos);
    void refreshCustomFunction(int index, bool modified=false);
    void onChildModified();
    bool playSound(int index);
    void stopSound(int index);
    void toggleSound(bool play);
    void onMediaPlayerStateChanged(QMediaPlayer::State state);
    void onMediaPlayerError(QMediaPlayer::Error error);
    void fswDelete();
    void fswCopy();
    void fswPaste();
    void fswCut();

  private:
    void populateFuncCB(QComboBox *b, unsigned int value);
    void populateGVmodeCB(QComboBox *b, unsigned int value);
    void populateFuncParamCB(QComboBox *b, uint function, unsigned int value, unsigned int adjustmode=0);
    RawSwitchFilterItemModel * rawSwitchItemModel;
    QStandardItemModel * rawSrcInputsItemModel;
    QStandardItemModel * rawSrcAllItemModel;

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

    int selectedFunction;

};

#endif // _CUSTOMFUNCTIONS_H_
