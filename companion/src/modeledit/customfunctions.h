#ifndef CUSTOMFUNCTIONS_H
#define CUSTOMFUNCTIONS_H

#include "modeledit.h"
#include "eeprominterface.h"
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTimeEdit>
#ifdef PHONON
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>
#endif

class RepeatComboBox: public QComboBox
{
    Q_OBJECT

  public:
    RepeatComboBox(QWidget *parent, int & repeatParam);
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
#ifdef PHONON
    void playMusic();
    void mediaPlayer_state(Phonon::State newState, Phonon::State oldState);
#endif
    void fswDelete();
    void fswCopy();
    void fswPaste();
    void fswCut();

  private:
    void populateFuncCB(QComboBox *b, unsigned int value);
    void populateGVmodeCB(QComboBox *b, unsigned int value);
    void populateFuncParamCB(QComboBox *b, uint function, unsigned int value, unsigned int adjustmode=0);

    bool initialized;
    QSet<QString> tracksSet;
    QSet<QString> scriptsSet;
    int phononCurrent;
    QComboBox * fswtchSwtch[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchFunc[C9X_MAX_CUSTOM_FUNCTIONS];
    QCheckBox * fswtchParamGV[C9X_MAX_CUSTOM_FUNCTIONS];
    QDoubleSpinBox * fswtchParam[C9X_MAX_CUSTOM_FUNCTIONS];
    QTimeEdit * fswtchParamTime[C9X_MAX_CUSTOM_FUNCTIONS];
    QPushButton * playBT[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchParamT[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchParamArmT[C9X_MAX_CUSTOM_FUNCTIONS];
    QCheckBox * fswtchEnable[C9X_MAX_CUSTOM_FUNCTIONS];
    RepeatComboBox * fswtchRepeat[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchGVmode[C9X_MAX_CUSTOM_FUNCTIONS];
    QSlider * fswtchBLcolor[C9X_MAX_CUSTOM_FUNCTIONS];
#ifdef PHONON
    Phonon::MediaObject *clickObject;
    Phonon::AudioOutput *clickOutput;
#endif

    int selectedFunction;

};

#endif // CUSTOMFUNCTIONS_H
