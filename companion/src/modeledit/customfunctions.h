#ifndef CUSTOMFUNCTIONS_H
#define CUSTOMFUNCTIONS_H

#include "modelpanel.h"
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
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

  signals:
    void modified();

  private slots:
    void onIndexChanged(int);

  protected:
    int & repeatParam;
};

class CustomFunctionsPanel : public ModelPanel
{
    Q_OBJECT

  public:
    CustomFunctionsPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings);
    ~CustomFunctionsPanel();

    virtual void update();

  private slots:
    void customFunctionEdited();
    void fsw_customContextMenuRequested(QPoint pos);
    void refreshCustomFunction(int index, bool modified=false);
    void playMusic();
    void onChildModified();
#ifdef PHONON
    void mediaPlayer_state(Phonon::State newState, Phonon::State oldState);
#endif
    void fswDelete();
    void fswCopy();
    void fswPaste();
    void fswCut();

  private:
    GeneralSettings & generalSettings;
    bool phononLock;
    QComboBox * fswtchSwtch[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchFunc[C9X_MAX_CUSTOM_FUNCTIONS];
    QCheckBox * fswtchParamGV[C9X_MAX_CUSTOM_FUNCTIONS];
    QDoubleSpinBox * fswtchParam[C9X_MAX_CUSTOM_FUNCTIONS];
    QPushButton * playBT[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchParamT[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchParamArmT[C9X_MAX_CUSTOM_FUNCTIONS];
    QCheckBox * fswtchEnable[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchRepeat[C9X_MAX_CUSTOM_FUNCTIONS];
    QComboBox * fswtchGVmode[C9X_MAX_CUSTOM_FUNCTIONS];
#ifdef PHONON
    Phonon::MediaObject *clickObject;
    Phonon::AudioOutput *clickOutput;
#endif

    int selectedFunction;

};

#endif // CUSTOMFUNCTIONS_H
