#include "customfunctions.h"
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include "helpers.h"

CustomFunctionsPanel::CustomFunctionsPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings):
  ModelPanel(parent, model),
  generalSettings(generalSettings),
  phononLock(false)
{
  QGridLayout * gridLayout = new QGridLayout(this);

  int col = 1;
  addLabel(gridLayout, tr("Switch"), col++);
  addLabel(gridLayout, tr("Function"), col++);
  addLabel(gridLayout, tr("Parameters"), col++);
  addLabel(gridLayout, tr("Enable"), col++);

  lock = true;
  int num_fsw = GetEepromInterface()->getCapability(CustomFunctions);

  QStringList paramarmList;
  if (!GetEepromInterface()->getCapability(VoicesAsNumbers)) {
    for (int i=0; i<num_fsw; i++) {
      if (model.funcSw[i].func==FuncPlayPrompt || model.funcSw[i].func==FuncBackgroundMusic) {
        QString temp = model.funcSw[i].paramarm;
        if (!temp.isEmpty()) {
          if (!paramarmList.contains(temp)) {
            paramarmList.append(temp);
          }
        }
      }
    }

    QSettings settings("companion", "companion");
    QString path = settings.value("sdPath", ".").toString();
    path.append("/SOUNDS/");
    QString lang = generalSettings.ttsLanguage;
    if (lang.isEmpty())
      lang="en";
    path.append(lang);
    QDir qd(path);
    int vml= GetEepromInterface()->getCapability(VoicesMaxLength)+4;
    if (qd.exists()) {
      QStringList filters;
      filters << "*.wav" << "*.WAV";
      foreach ( QString file, qd.entryList(filters, QDir::Files) ) {
        QFileInfo fi(file);
        QString temp=fi.completeBaseName();
        if (!paramarmList.contains(temp) && temp.length()<=vml) {
          paramarmList.append(temp);
        }
      }
    }
  }

  for (int i=0; i<num_fsw; i++) {
    AssignFunc func = model.funcSw[i].func;

    // The label
    QLabel * label = new QLabel(this);
    label->setContextMenuPolicy(Qt::CustomContextMenu);
    label->setMouseTracking(true);
    label->setProperty("index", i);
    label->setText(tr("CF%1").arg(i+1));
    gridLayout->addWidget(label, i+1, 0);
    connect(label, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(fsw_customContextMenuRequested(QPoint)));

    // The switch
    fswtchSwtch[i] = new QComboBox(this);
    fswtchSwtch[i]->setProperty("index", i);
    connect(fswtchSwtch[i], SIGNAL(currentIndexChanged(int)), this, SLOT(customFunctionEdited()));
    gridLayout->addWidget(fswtchSwtch[i], i+1, 1);
    populateSwitchCB(fswtchSwtch[i], model.funcSw[i].swtch, POPULATE_MSWITCHES|POPULATE_ONOFF);

    // The function
    fswtchFunc[i] = new QComboBox(this);
    fswtchFunc[i]->setProperty("index", i);
    connect(fswtchFunc[i], SIGNAL(currentIndexChanged(int)), this, SLOT(customFunctionEdited()));
    gridLayout->addWidget(fswtchFunc[i], i+1, 2);
    populateFuncCB(fswtchFunc[i], model.funcSw[i].func);

    QHBoxLayout *paramLayout = new QHBoxLayout();
    gridLayout->addLayout(paramLayout, i+1, 3);

    fswtchGVmode[i] = new QComboBox(this);
    fswtchGVmode[i]->setProperty("index", i);
    connect(fswtchGVmode[i], SIGNAL(currentIndexChanged(int)), this, SLOT(customFunctionEdited()));
    paramLayout->addWidget(fswtchGVmode[i]);
    populateGVmodeCB(fswtchGVmode[i], model.funcSw[i].adjustMode);

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

    fswtchParamT[i] = new QComboBox(this);
    fswtchParamT[i]->setProperty("index", i);
    paramLayout->addWidget(fswtchParamT[i]);
    populateFuncParamCB(fswtchParamT[i], func, model.funcSw[i].param, model.funcSw[i].adjustMode);
    connect(fswtchParamT[i], SIGNAL(currentIndexChanged(int)), this, SLOT(customFunctionEdited()));

    fswtchParamArmT[i] = new QComboBox(this);
    fswtchParamArmT[i]->setProperty("index", i);
    populateFuncParamArmTCB(fswtchParamArmT[i],&model, model.funcSw[i].paramarm, paramarmList);
    fswtchParamArmT[i]->setEditable(true);
    paramLayout->addWidget(fswtchParamArmT[i]);

    connect(fswtchParamArmT[i], SIGNAL(currentIndexChanged(int)), this, SLOT(customFunctionEdited()));
    connect(fswtchParamArmT[i], SIGNAL(editTextChanged ( const QString)), this, SLOT(customFunctionEdited()));

#ifdef PHONON
    QSettings settings("companion", "companion");
    int theme_set=settings.value("theme", 1).toInt();
    QString Theme;
    switch(theme_set) {
      case 0:
        Theme="classic";
        break;
      default:
        Theme="monochrome";
        break;          
    }
    QIcon PlayIcon;
    populate_icon(&PlayIcon,Theme,"play.png");
    playBT[i] = new QPushButton(this);
    playBT[i]->setObjectName(QString("play_%1").arg(i));
    playBT[i]->setIcon(PlayIcon);
    paramLayout->addWidget(playBT[i]);
    connect(playBT[i], SIGNAL(pressed()), this, SLOT(playMusic()));
#endif

    QHBoxLayout *repeatLayout = new QHBoxLayout();
    gridLayout->addLayout(repeatLayout, i+1, 4);

    fswtchRepeat[i] = new QComboBox(this);
    fswtchRepeat[i]->setProperty("index", i);
    connect(fswtchRepeat[i], SIGNAL(currentIndexChanged(int)), this, SLOT(customFunctionEdited()));
    repeatLayout->addWidget(fswtchRepeat[i], i+1);
    populateRepeatCB(fswtchRepeat[i], model.funcSw[i].repeatParam);

    fswtchEnable[i] = new QCheckBox(this);
    fswtchEnable[i]->setProperty("index", i);
    fswtchEnable[i]->setText(tr("ON"));
    repeatLayout->addWidget(fswtchEnable[i], i+1);
    fswtchEnable[i]->setChecked(model.funcSw[i].enabled);
    connect(fswtchEnable[i], SIGNAL(stateChanged(int)), this, SLOT(customFunctionEdited()));
  }

  lock = false;
}

CustomFunctionsPanel::~CustomFunctionsPanel()
{
}

#ifdef PHONON

void CustomFunctionsPanel::mediaPlayer_state(Phonon::State newState, Phonon::State oldState)
{
    if (phononLock)
      return;
    QSettings settings("companion", "companion");
    int theme_set=settings.value("theme", 1).toInt();
    QString Theme;
    switch(theme_set) {
      case 0:
        Theme="classic";
        break;
      default:
        Theme="monochrome";
        break;          
    }
    QIcon PlayIcon;
    populate_icon(&PlayIcon,Theme,"play.png");
    
    phononLock=true;
    if ((newState==Phonon::StoppedState || newState==Phonon::PausedState)  && oldState==Phonon::PlayingState) {
      clickObject->stop();
      clickObject->clearQueue();
      clickObject->clear();
      for (int i=0; i<GetEepromInterface()->getCapability(CustomFunctions); i++) {
        playBT[i]->setObjectName(QString("play_%1").arg(i));
        playBT[i]->setIcon(PlayIcon);
      }
    }
    if (newState==Phonon::ErrorState) {
      clickObject->stop();
      clickObject->clearQueue();
      clickObject->clear();
      for (int i=0; i<GetEepromInterface()->getCapability(CustomFunctions); i++) {
        playBT[i]->setObjectName(QString("play_%1").arg(i));
        playBT[i]->setIcon(PlayIcon);
      }
    }

    phononLock=false;
}
#endif

void CustomFunctionsPanel::playMusic()
{
    QPushButton *playButton = qobject_cast<QPushButton*>(sender());
    int index=playButton->objectName().mid(5,2).toInt();
    QString function=playButton->objectName().left(4);
    QSettings settings("companion", "companion");
    int theme_set=settings.value("theme", 1).toInt();
    QString Theme;
    switch(theme_set) {
      case 0:
        Theme="classic";
        break;
      default:
        Theme="monochrome";
        break;          
    }
    QIcon PlayIcon;
    populate_icon(&PlayIcon,Theme,"play.png");
    QIcon StopIcon;
    populate_icon(&StopIcon,Theme,"stop.png");
      
    QString path=settings.value("sdPath", ".").toString();
    QDir qd(path);
    QString track;
    if (qd.exists()) {
      if (GetEepromInterface()->getCapability(VoicesAsNumbers)) {
        track=path+QString("/%1.wav").arg(int(fswtchParam[index]->value()),4,10,(const QChar)'0');
      } else {
        path.append("/SOUNDS/");
        QString lang = generalSettings.ttsLanguage;
        if (lang.isEmpty())
          lang="en";
        path.append(lang);
        if (fswtchParamArmT[index]->currentText()!="----") {
          track=path+"/"+fswtchParamArmT[index]->currentText()+".wav";
        }
      }
      QFile file(track);
      if (!file.exists()) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to find sound file %1!").arg(track));
        track.clear();
      }
#ifdef PHONON
      if (function=="play" && !track.isEmpty()) {
        clickObject->clear();
        clickObject->setCurrentSource(Phonon::MediaSource(track));
        clickObject->play();
        playBT[index]->setObjectName(QString("stop_%1").arg(index));
        playBT[index]->setIcon(StopIcon);
      } else {
        clickObject->stop();
        clickObject->clear();
        playBT[index]->setObjectName(QString("play_%1").arg(index));
        playBT[index]->setIcon(PlayIcon);
      }
#endif
    }
}

#define CUSTOM_FUNCTION_NUMERIC_PARAM  (1<<0)
#define CUSTOM_FUNCTION_SOURCE_PARAM   (1<<1)
#define CUSTOM_FUNCTION_FILE_PARAM     (1<<2)
#define CUSTOM_FUNCTION_GV_MODE        (1<<3)
#define CUSTOM_FUNCTION_GV_TOOGLE      (1<<4)
#define CUSTOM_FUNCTION_ENABLE         (1<<5)
#define CUSTOM_FUNCTION_REPEAT         (1<<6)
#define CUSTOM_FUNCTION_PLAY           (1<<7)

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

void CustomFunctionsPanel::refreshCustomFunction(int i, bool modified)
{
    unsigned int widgetsMask = 0;
    if (modified) {
      model.funcSw[i].swtch = RawSwitch(fswtchSwtch[i]->itemData(fswtchSwtch[i]->currentIndex()).toInt());
      model.funcSw[i].func = (AssignFunc)fswtchFunc[i]->currentIndex();
      model.funcSw[i].enabled =fswtchEnable[i]->isChecked();
      model.funcSw[i].repeatParam = (AssignFunc)fswtchRepeat[i]->currentIndex();
      model.funcSw[i].adjustMode = (AssignFunc)fswtchGVmode[i]->currentIndex();
    }

    int index = fswtchFunc[i]->currentIndex();

    if (index>=FuncSafetyCh1 && index<=FuncSafetyCh16) {
      fswtchParam[i]->setDecimals(0);
      fswtchParam[i]->setSingleStep(1);
      fswtchParam[i]->setMinimum(-125);
      fswtchParam[i]->setMaximum(125);
      if (modified) {
        model.funcSw[i].param = fswtchParam[i]->value();
      }
      fswtchParam[i]->setValue(model.funcSw[i].param);
      widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM + CUSTOM_FUNCTION_ENABLE;
    } else if (index==FuncLogs) {
      fswtchParam[i]->setDecimals(1);
      fswtchParam[i]->setMinimum(0);
      fswtchParam[i]->setMaximum(25.5);
      fswtchParam[i]->setSingleStep(0.1);
      if (modified) model.funcSw[i].param = fswtchParam[i]->value()*10.0;
      fswtchParam[i]->setValue(model.funcSw[i].param/10.0);
      widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM + CUSTOM_FUNCTION_ENABLE;
    }
    else if (index>=FuncAdjustGV1 && index<=FuncAdjustGVLast) {
      if (modified) model.funcSw[i].adjustMode = fswtchGVmode[i]->currentIndex();
      widgetsMask |= CUSTOM_FUNCTION_GV_MODE + CUSTOM_FUNCTION_ENABLE;
      if (model.funcSw[i].adjustMode==0) {
        if (modified) model.funcSw[i].param = fswtchParam[i]->value();
        fswtchParam[i]->setDecimals(0);
        fswtchParam[i]->setSingleStep(1);
        fswtchParam[i]->setMinimum(-125);
        fswtchParam[i]->setMaximum(125);
        fswtchParam[i]->setValue(model.funcSw[i].param);
        widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM;
      } else {
        if (modified) model.funcSw[i].param = fswtchParamT[i]->itemData(fswtchParamT[i]->currentIndex()).toInt();
        populateFuncParamCB(fswtchParamT[i], index, model.funcSw[i].param, model.funcSw[i].adjustMode);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
      }
    } else if (index==FuncReset) {
      if (modified) model.funcSw[i].param = (uint8_t)fswtchParamT[i]->currentIndex();
      populateFuncParamCB(fswtchParamT[i], index, model.funcSw[i].param);
      widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
    } else if (index==FuncVolume) {
      if (modified) model.funcSw[i].param = fswtchParamT[i]->itemData(fswtchParamT[i]->currentIndex()).toInt();
      populateFuncParamCB(fswtchParamT[i], index, model.funcSw[i].param);
      widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM + CUSTOM_FUNCTION_ENABLE;
    } else if (index==FuncPlaySound || index==FuncPlayHaptic || index==FuncPlayValue || index==FuncPlayPrompt || index==FuncPlayBoth || index==FuncBackgroundMusic) {
      if (modified) model.funcSw[i].repeatParam = fswtchRepeat[i]->itemData(fswtchRepeat[i]->currentIndex()).toInt();
      if (index != FuncBackgroundMusic) {
        if (GetEepromInterface()->getCapability(HasFuncRepeat)) {
          widgetsMask |= CUSTOM_FUNCTION_REPEAT;
        }
      }
      if (index==FuncPlayValue) {
        if (modified) model.funcSw[i].param = fswtchParamT[i]->itemData(fswtchParamT[i]->currentIndex()).toInt();
        populateFuncParamCB(fswtchParamT[i], index, model.funcSw[i].param);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM + CUSTOM_FUNCTION_REPEAT;
      } else if (index==FuncPlayPrompt || index==FuncPlayBoth) {
        if (GetEepromInterface()->getCapability(VoicesAsNumbers)) {
          fswtchParam[i]->setDecimals(0);
          fswtchParam[i]->setSingleStep(1);
          fswtchParam[i]->setMinimum(0);
          if (index==FuncPlayPrompt) {
            widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM + CUSTOM_FUNCTION_REPEAT + CUSTOM_FUNCTION_GV_TOOGLE;
          } else {
            widgetsMask |= CUSTOM_FUNCTION_NUMERIC_PARAM + CUSTOM_FUNCTION_REPEAT;
            fswtchParamGV[i]->setChecked(false);
          }
          fswtchParam[i]->setMaximum(index==FuncPlayBoth ? 254 : 255);
          if (modified) {
            if (fswtchParamGV[i]->isChecked()) {
              fswtchParam[i]->setMinimum(1);
              model.funcSw[i].param = std::min(fswtchParam[i]->value(),5.0)+(fswtchParamGV[i]->isChecked() ? 250 : 0);
            } else {
              model.funcSw[i].param = fswtchParam[i]->value();
            }
          }
          if (model.funcSw[i].param>250 && (index!=FuncPlayBoth)) {
            fswtchParamGV[i]->setChecked(true);
            fswtchParam[i]->setValue(model.funcSw[i].param-250);
            fswtchParam[i]->setMaximum(5);
          } else {
            fswtchParamGV[i]->setChecked(false);
            fswtchParam[i]->setValue(model.funcSw[i].param);
          }
          if (model.funcSw[i].param < 251)
            widgetsMask |= CUSTOM_FUNCTION_PLAY;
        } else {
          widgetsMask |= CUSTOM_FUNCTION_FILE_PARAM;
          if (modified) {
            memset(model.funcSw[i].paramarm, 0, sizeof(model.funcSw[i].paramarm));
            int vml=GetEepromInterface()->getCapability(VoicesMaxLength);
            if (fswtchParamArmT[i]->currentText() != "----") {
              widgetsMask |= CUSTOM_FUNCTION_PLAY;
              for (int j=0; j<std::min(fswtchParamArmT[i]->currentText().length(),vml); j++) {
                model.funcSw[i].paramarm[j] = fswtchParamArmT[i]->currentText().toAscii().at(j);
              }
            }
          }
        }
      } else if (index==FuncBackgroundMusic) {
        widgetsMask |= CUSTOM_FUNCTION_FILE_PARAM;
        if (modified) {
          memset(model.funcSw[i].paramarm, 0, sizeof(model.funcSw[i].paramarm));
          int vml=GetEepromInterface()->getCapability(VoicesMaxLength);
          if (fswtchParamArmT[i]->currentText() != "----") {
            widgetsMask |= CUSTOM_FUNCTION_PLAY;
            for (int j=0; j<std::min(fswtchParamArmT[i]->currentText().length(),vml); j++) {
              model.funcSw[i].paramarm[j] = fswtchParamArmT[i]->currentText().toAscii().at(j);
            }
          }
        }
      } else if (index==FuncPlaySound) {
        if (modified) model.funcSw[i].param = (uint8_t)fswtchParamT[i]->currentIndex();
        populateFuncParamCB(fswtchParamT[i], index, model.funcSw[i].param);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
      } else if (index==FuncPlayHaptic) {
        if (modified) model.funcSw[i].param = (uint8_t)fswtchParamT[i]->currentIndex();
        populateFuncParamCB(fswtchParamT[i], index, model.funcSw[i].param);
        widgetsMask |= CUSTOM_FUNCTION_SOURCE_PARAM;
      }
   } else if (model.funcSw[i].swtch.type!=SWITCH_TYPE_NONE) {
      if (modified) model.funcSw[i].param = fswtchParam[i]->value();
      fswtchParam[i]->setDecimals(0);
      fswtchParam[i]->setSingleStep(1);
      fswtchParam[i]->setValue(model.funcSw[i].param);
      if (index<=FuncInstantTrim) {
        widgetsMask |= CUSTOM_FUNCTION_ENABLE;
      }
    }

    fswtchParam[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_NUMERIC_PARAM);
    fswtchParamGV[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_GV_TOOGLE);
    fswtchParamT[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_SOURCE_PARAM);
    fswtchParamArmT[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_FILE_PARAM);
    fswtchEnable[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_ENABLE);
    if (!(widgetsMask & CUSTOM_FUNCTION_ENABLE)) fswtchEnable[i]->setChecked(false);
    fswtchRepeat[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_REPEAT);
    fswtchGVmode[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_GV_MODE);
#ifdef PHONON
    playBT[i]->setVisible(widgetsMask & CUSTOM_FUNCTION_PLAY);
#endif
}

void CustomFunctionsPanel::update()
{
  for (int i=0; i<GetEepromInterface()->getCapability(CustomFunctions); i++) {
    refreshCustomFunction(i);
  }
}

void CustomFunctionsPanel::fswPaste()
{
  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();
  if (mimeData->hasFormat("application/x-companion9x-fsw")) {
    QByteArray fswData = mimeData->data("application/x-companion9x-fsw");

    FuncSwData *fsw = &model.funcSw[selectedFunction];
    memcpy(fsw, fswData.mid(0, sizeof(FuncSwData)).constData(), sizeof(FuncSwData));
    // TODO update switch and func
    populateSwitchCB(fswtchSwtch[selectedFunction], model.funcSw[selectedFunction].swtch, POPULATE_MSWITCHES|POPULATE_ONOFF);
    populateFuncCB(fswtchFunc[selectedFunction], model.funcSw[selectedFunction].func);
    refreshCustomFunction(selectedFunction);
    emit modified();
  }
}

void CustomFunctionsPanel::fswDelete()
{
  model.funcSw[selectedFunction].clear();
  // TODO update switch and func
  populateSwitchCB(fswtchSwtch[selectedFunction], model.funcSw[selectedFunction].swtch, POPULATE_MSWITCHES|POPULATE_ONOFF);
  populateFuncCB(fswtchFunc[selectedFunction], model.funcSw[selectedFunction].func);
  refreshCustomFunction(selectedFunction);
  emit modified();
}

void CustomFunctionsPanel::fswCopy()
{
    QByteArray fswData;
    fswData.append((char*)&model.funcSw[selectedFunction],sizeof(FuncSwData));
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion9x-fsw", fswData);
    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
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
    QSettings settings("companion", "companion");
    int theme_set=settings.value("theme", 1).toInt();
    QString Theme;
    switch(theme_set) {
      case 0:
        Theme="classic";
        break;
      default:
        Theme="monochrome";
        break;          
    }
    QIcon ClearIcon;
    populate_icon(&ClearIcon,Theme,"clear.png");
    QIcon CopyIcon;
    populate_icon(&CopyIcon,Theme,"copy.png");
    QIcon CutIcon;
    populate_icon(&CutIcon,Theme,"cut.png");
    QIcon PasteIcon;
    populate_icon(&PasteIcon,Theme,"paste.png");

    QPoint globalPos = label->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion9x-fsw");

    QMenu contextMenu;
    contextMenu.addAction(ClearIcon, tr("&Delete"),this,SLOT(fswDelete()),tr("Delete"));
    contextMenu.addAction(CopyIcon, tr("&Copy"),this,SLOT(fswCopy()),tr("Ctrl+C"));
    contextMenu.addAction(CutIcon, tr("&Cut"),this,SLOT(fswCut()),tr("Ctrl+X"));
    contextMenu.addAction(PasteIcon, tr("&Paste"),this,SLOT(fswPaste()),tr("Ctrl+V"))->setEnabled(hasData);

    contextMenu.exec(globalPos);
}
