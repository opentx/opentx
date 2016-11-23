#ifndef CUSTOMFUNCTIONS_H
#define CUSTOMFUNCTIONS_H

#include "modeledit.h"
#include "eeprominterface.h"
#include <QtMultimedia>

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
    void customFunctionEdited();
    void functionEdited();
    void fsw_customContextMenuRequested(QPoint pos);
    void refreshCustomFunction(int index, bool modified=false);
    void onChildModified();
    void playMusic();
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

    QSet<QString> tracksSet;
    QSet<QString> scriptsSet;
    int mediaPlayerCurrent;
    QComboBox * fswtchSwtch[CPN_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchFunc[CPN_MAX_CUSTOM_FUNCTIONS];
    QCheckBox * fswtchParamGV[CPN_MAX_CUSTOM_FUNCTIONS];
    QDoubleSpinBox * fswtchParam[CPN_MAX_CUSTOM_FUNCTIONS];
    QTimeEdit * fswtchParamTime[CPN_MAX_CUSTOM_FUNCTIONS];
    QPushButton * playBT[CPN_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchParamT[CPN_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchParamArmT[CPN_MAX_CUSTOM_FUNCTIONS];
    QCheckBox * fswtchEnable[CPN_MAX_CUSTOM_FUNCTIONS];
    RepeatComboBox * fswtchRepeat[CPN_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchGVmode[CPN_MAX_CUSTOM_FUNCTIONS];
    QSlider * fswtchBLcolor[CPN_MAX_CUSTOM_FUNCTIONS];
    QMediaPlayer * mediaPlayer;

    int selectedFunction;

};

#endif // CUSTOMFUNCTIONS_H
