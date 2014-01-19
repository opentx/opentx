#include "inputs.h"
#include <QMessageBox>
#include "expodialog.h"
#include "helpers.h"

InputsPanel::InputsPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings):
  ModelPanel(parent, model),
  generalSettings(generalSettings),
  expoInserted(false)
{
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

  QGridLayout * exposLayout = new QGridLayout(this);

  ExposlistWidget = new MixersList(this, true);
  QPushButton * qbUp = new QPushButton(this);
  QPushButton * qbDown = new QPushButton(this);
  QPushButton * qbClear = new QPushButton(this);
  QIcon qbUpIcon;
  populate_icon(&qbUpIcon,Theme,"moveup.png");
  qbUp->setText(tr("Move Up"));
  qbUp->setIcon(qbUpIcon);
  qbUp->setShortcut(QKeySequence(tr("Ctrl+Up")));
  QIcon qbDownIcon;
  populate_icon(&qbDownIcon,Theme,"movedown.png");
  qbDown->setText(tr("Move Down"));
  qbDown->setIcon(qbDownIcon);
  qbDown->setShortcut(QKeySequence(tr("Ctrl+Down")));
  QIcon qbClearIcon;
  populate_icon(&qbClearIcon,Theme,"clear.png");
  qbClear->setText(tr("Clear Expo Settings"));
  qbClear->setIcon(qbClearIcon);

  exposLayout->addWidget(ExposlistWidget,1,1,1,3);
  exposLayout->addWidget(qbUp,2,1);
  exposLayout->addWidget(qbClear,2,2);
  exposLayout->addWidget(qbDown,2,3);

  connect(ExposlistWidget, SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(expolistWidget_customContextMenuRequested(QPoint)));
  connect(ExposlistWidget, SIGNAL(doubleClicked(QModelIndex)),this,SLOT(expolistWidget_doubleClicked(QModelIndex)));
  connect(ExposlistWidget, SIGNAL(mimeDropped(int,const QMimeData*,Qt::DropAction)),this,SLOT(mimeExpoDropped(int,const QMimeData*,Qt::DropAction)));

  connect(qbUp, SIGNAL(pressed()),SLOT(moveExpoUp()));
  connect(qbDown, SIGNAL(pressed()),SLOT(moveExpoDown()));
  connect(qbClear, SIGNAL(pressed()),SLOT(clearExpos()));

  connect(ExposlistWidget, SIGNAL(keyWasPressed(QKeyEvent*)), this, SLOT(expolistWidget_KeyPress(QKeyEvent*)));
}

InputsPanel::~InputsPanel()
{
}

void InputsPanel::update()
{
  lock = true;

  int inputsCount = GetEepromInterface()->getCapability(VirtualInputs);
  if (inputsCount == 0)
    inputsCount = NUM_STICKS;

  // curDest -> destination channel
  // i -> mixer number
  QByteArray qba;
  ExposlistWidget->clear();
  int curDest = -1;

  for(int i=0; i<C9X_MAX_EXPOS; i++) {
    ExpoData *md = &model.expoData[i];

    if (md->mode==0) break;

    QString str;

    while (curDest<(int)md->chn-1) {
      curDest++;
      str = getInputStr(model, curDest);
      qba.clear();
      qba.append((quint8)-curDest-1);
      QListWidgetItem *itm = new QListWidgetItem(str);
      itm->setData(Qt::UserRole,qba);
      ExposlistWidget->addItem(itm);
    }

    if (curDest!=(int)md->chn) {
      if (GetEepromInterface()->getCapability(VirtualInputs))
        str = QString("%1").arg(getInputStr(model, md->chn), -8, ' ');
      else
        str = getInputStr(model, md->chn);
      curDest = md->chn;
    }
    else {
      str = "   ";
    }

    if (GetEepromInterface()->getCapability(VirtualInputs)) {
      str += " " + tr("Source(%1)").arg(md->srcRaw.toString());
    }
    else {
      switch (md->mode) {
        case (1): str += " <-"; break;
        case (2): str += " ->"; break;
        default:  str += "   "; break;
      };
    }

    str += " " + tr("Weight(%1)").arg(getGVarString(md->weight));

    if (md->curve.value) str += " " + md->curve.toString();

    QString phasesStr = getPhasesStr(md->phases, model);
    if (!phasesStr.isEmpty()) str += " " + phasesStr;

    if (md->swtch.type != SWITCH_TYPE_NONE) str += " " + tr("Switch(%1)").arg(md->swtch.toString());

    if (GetEepromInterface()->getCapability(HasExpoNames)) {
      QString expoName = md->name;
      if (!expoName.isEmpty()) str += QString(" [%1]").arg(expoName);
    }

    qba.clear();
    qba.append((quint8)i);
    qba.append((const char*)md, sizeof(ExpoData));
    QListWidgetItem *itm = new QListWidgetItem(str);
    itm->setData(Qt::UserRole,qba);  // expo number
    ExposlistWidget->addItem(itm);   //(str);
  }

  while (curDest<inputsCount-1) {
    curDest++;
    QString str = getInputStr(model, curDest);
    qba.clear();
    qba.append((quint8)-curDest-1);
    QListWidgetItem *itm = new QListWidgetItem(str);
    itm->setData(Qt::UserRole,qba); // add new expo
    ExposlistWidget->addItem(itm);
  }

  lock = false;
}


bool InputsPanel::gm_insertExpo(int idx)
{
    if (idx<0 || idx>=C9X_MAX_EXPOS || model.expoData[C9X_MAX_EXPOS-1].mode > 0) {
      QMessageBox::information(this, "companion9x", tr("Not enough available expos!"));
      return false;
    }

    int chn = model.expoData[idx].chn;
    memmove(&model.expoData[idx+1],&model.expoData[idx],
            (C9X_MAX_EXPOS-(idx+1))*sizeof(ExpoData) );
    memset(&model.expoData[idx],0,sizeof(ExpoData));
    model.expoData[idx].chn = chn;
    model.expoData[idx].weight = 100;
    model.expoData[idx].mode = 3 /* TODO enum */;
    return true;
}

void InputsPanel::gm_deleteExpo(int index)
{
    memmove(&model.expoData[index],&model.expoData[index+1],
              (C9X_MAX_EXPOS-(index+1))*sizeof(ExpoData));
    memset(&model.expoData[C9X_MAX_EXPOS-1],0,sizeof(ExpoData));
}

void InputsPanel::gm_openExpo(int index)
{
    if(index<0 || index>=C9X_MAX_EXPOS) return;

    ExpoData mixd(model.expoData[index]);
    emit modified();
    update();

    ExpoDialog *g = new ExpoDialog(this, model, &mixd, generalSettings.stickMode);
    if (g->exec())  {
      model.expoData[index] = mixd;
      emit modified();
      update();
    }
    else {
      if (expoInserted) {
        gm_deleteExpo(index);
      }
      expoInserted=false;
      emit modified();
      update();
    }
}

int InputsPanel::getExpoIndex(unsigned int dch)
{
    unsigned int i = 0;
    while (model.expoData[i].chn<=dch && model.expoData[i].mode && i<C9X_MAX_EXPOS) i++;
    if(i==C9X_MAX_EXPOS) return -1;
    return i;
}


QList<int> InputsPanel::createExpoListFromSelected()
{
    QList<int> list;
    foreach(QListWidgetItem *item, ExposlistWidget->selectedItems()) {
      int idx= item->data(Qt::UserRole).toByteArray().at(0);
      if(idx>=0 && idx<C9X_MAX_EXPOS) list << idx;
    }
    return list;
}


void InputsPanel::setSelectedByExpoList(QList<int> list)
{
    for(int i=0; i<ExposlistWidget->count(); i++) {
      int t = ExposlistWidget->item(i)->data(Qt::UserRole).toByteArray().at(0);
      if(list.contains(t))
        ExposlistWidget->item(i)->setSelected(true);
    }
}

void InputsPanel::exposDelete(bool ask)
{
    QMessageBox::StandardButton ret = QMessageBox::No;

    if(ask)
      ret = QMessageBox::warning(this, "companion9x",
               tr("Delete Selected Expos?"),
               QMessageBox::Yes | QMessageBox::No);


    if ((ret == QMessageBox::Yes) || (!ask)) {
        exposDeleteList(createExpoListFromSelected());
        emit modified();
        update();
    }
}

void InputsPanel::exposCut()
{
    exposCopy();
    exposDelete(false);
}

void InputsPanel::exposCopy()
{
    QList<int> list = createExpoListFromSelected();

    QByteArray mxData;
    foreach(int idx, list) {
      mxData.append((char*)&model.expoData[idx],sizeof(ExpoData));
    }

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion9x-expo", mxData);
    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void InputsPanel::mimeExpoDropped(int index, const QMimeData *data, Qt::DropAction /*action*/)
{
    int idx = ExposlistWidget->item(index > 0 ? index-1 : 0)->data(Qt::UserRole).toByteArray().at(0);
    pasteExpoMimeData(data, idx);
}

#include <iostream>
#include <QtGui/qwidget.h>
void InputsPanel::pasteExpoMimeData(const QMimeData * mimeData, int destIdx)
{
  if (mimeData->hasFormat("application/x-companion9x-expo")) {
    int idx; // mixer index
    int dch;

    if (destIdx < 0) {
      dch = -destIdx - 1;
      idx = getExpoIndex(dch) - 1; //get expo index to insert
    } else {
      idx = destIdx;
      dch = model.expoData[idx].chn;
    }

    QByteArray mxData = mimeData->data("application/x-companion9x-expo");

    int i = 0;
    while (i < mxData.size()) {
      idx++;
      if (!gm_insertExpo(idx))
        break;
      ExpoData *md = &model.expoData[idx];
      memcpy(md, mxData.mid(i, sizeof(ExpoData)).constData(), sizeof(ExpoData));
      md->chn = dch;
      i += sizeof(ExpoData);
    }

    emit modified();
    update();
  }
}


void InputsPanel::exposPaste()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    QListWidgetItem *item = ExposlistWidget->currentItem();
    if (item)
      pasteExpoMimeData(mimeData, item->data(Qt::UserRole).toByteArray().at(0));
}

void InputsPanel::exposDuplicate()
{
    exposCopy();
    exposPaste();
}


void InputsPanel::expoOpen(QListWidgetItem *item)
{
    if (!item)
      item = ExposlistWidget->currentItem();

    int idx = item->data(Qt::UserRole).toByteArray().at(0);
    if (idx<0) {
      int ch = -idx-1;
      idx = getExpoIndex(ch); // get expo index to insert
      if (!gm_insertExpo(idx))
        return;
      model.expoData[idx].chn = ch;
      expoInserted=true;
    } else {
        expoInserted=false;
    }
    gm_openExpo(idx);
}

void InputsPanel::expoAdd()
{
    int index = ExposlistWidget->currentItem()->data(Qt::UserRole).toByteArray().at(0);

    if(index<0) {  // if empty then return relevant index
      expoOpen();
    }  else {
      index++;
      if (!gm_insertExpo(index))
        return;
      model.expoData[index].chn = model.expoData[index-1].chn;
    }
    gm_openExpo(index);
}


void InputsPanel::expolistWidget_customContextMenuRequested(QPoint pos)
{
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
    QIcon AddIcon;
    populate_icon(&AddIcon,Theme,"add.png");
    QIcon EditIcon;
    populate_icon(&EditIcon,Theme,"edit.png");
    QIcon ClearIcon;
    populate_icon(&ClearIcon,Theme,"clear.png");
    QIcon CopyIcon;
    populate_icon(&CopyIcon,Theme,"copy.png");
    QIcon CutIcon;
    populate_icon(&CutIcon,Theme,"cut.png");
    QIcon PasteIcon;
    populate_icon(&PasteIcon,Theme,"paste.png");
    QIcon DuplicateIcon;
    populate_icon(&DuplicateIcon,Theme,"duplicate.png");
    QIcon MoveUpIcon;
    populate_icon(&MoveUpIcon,Theme,"moveup.png");
    QIcon MoveDownIcon;
    populate_icon(&MoveDownIcon,Theme,"movedown.png");
    QPoint globalPos = ExposlistWidget->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion9x-expo");

    QMenu contextMenu;
    contextMenu.addAction(AddIcon, tr("&Add"),this,SLOT(expoAdd()),tr("Ctrl+A"));
    contextMenu.addAction(EditIcon, tr("&Edit"),this,SLOT(expoOpen()),tr("Enter"));
    contextMenu.addSeparator();
    contextMenu.addAction(ClearIcon, tr("&Delete"),this,SLOT(exposDelete()),tr("Delete"));
    contextMenu.addAction(CopyIcon, tr("&Copy"),this,SLOT(exposCopy()),tr("Ctrl+C"));
    contextMenu.addAction(CutIcon, tr("&Cut"),this,SLOT(exposCut()),tr("Ctrl+X"));
    contextMenu.addAction(PasteIcon, tr("&Paste"),this,SLOT(exposPaste()),tr("Ctrl+V"))->setEnabled(hasData);
    contextMenu.addAction(DuplicateIcon, tr("Du&plicate"),this,SLOT(exposDuplicate()),tr("Ctrl+U"));
    contextMenu.addSeparator();
    contextMenu.addAction(MoveUpIcon, tr("Move Up"),this,SLOT(moveExpoUp()),tr("Ctrl+Up"));
    contextMenu.addAction(MoveDownIcon, tr("Move Down"),this,SLOT(moveExpoDown()),tr("Ctrl+Down"));

    contextMenu.exec(globalPos);
}


void InputsPanel::expolistWidget_KeyPress(QKeyEvent *event)
{
    if(event->matches(QKeySequence::SelectAll)) expoAdd();  //Ctrl A
    if(event->matches(QKeySequence::Delete))    exposDelete();
    if(event->matches(QKeySequence::Copy))      exposCopy();
    if(event->matches(QKeySequence::Cut))       exposCut();
    if(event->matches(QKeySequence::Paste))     exposPaste();
    if(event->matches(QKeySequence::Underline)) exposDuplicate();

    if(event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter) expoOpen();
    if(event->matches(QKeySequence::MoveToNextLine))
        ExposlistWidget->setCurrentRow(ExposlistWidget->currentRow()+1);
    if(event->matches(QKeySequence::MoveToPreviousLine))
        ExposlistWidget->setCurrentRow(ExposlistWidget->currentRow()-1);
}


int InputsPanel::gm_moveExpo(int idx, bool dir) //true=inc=down false=dec=up
{
    if(idx>C9X_MAX_EXPOS || (idx==C9X_MAX_EXPOS && dir)) return idx;

    int tdx = dir ? idx+1 : idx-1;
    ExpoData temp;
    temp.clear();
    ExpoData &src=model.expoData[idx];
    ExpoData &tgt=model.expoData[tdx];
    if (!dir && tdx<0 && src.chn>0) {
      src.chn--;
      return idx;
    } else if (!dir && tdx<0) {
      return idx;
    }

    if(memcmp(&src,&temp,sizeof(ExpoData))==0) return idx;
    bool tgtempty=(memcmp(&tgt,&temp,sizeof(ExpoData))==0 ? 1:0);
    if(tgt.chn!=src.chn || tgtempty) {
        if ((dir)  && (src.chn<(NUM_STICKS-1))) src.chn++;
        if ((!dir) && (src.chn>0)) src.chn--;
        return idx;
    }

    //flip between idx and tgt
    memcpy(&temp,&src,sizeof(ExpoData));
    memcpy(&src,&tgt,sizeof(ExpoData));
    memcpy(&tgt,&temp,sizeof(ExpoData));
    return tdx;
}

void InputsPanel::moveExpoUp()
{
    QList<int> list = createExpoListFromSelected();
    QList<int> highlightList;
    foreach(int idx, list) {
      highlightList << gm_moveExpo(idx, false);
    }
    emit modified();
    update();
    setSelectedByExpoList(highlightList);
}

void InputsPanel::moveExpoDown()
{
    QList<int> list = createExpoListFromSelected();
    QList<int> highlightList;
    foreach(int idx, list) {
      highlightList << gm_moveExpo(idx, true);
    }
    emit modified();
    update();
    setSelectedByExpoList(highlightList);
}

void InputsPanel::expolistWidget_doubleClicked(QModelIndex index)
{
    expoOpen(ExposlistWidget->item(index.row()));
}

void InputsPanel::exposDeleteList(QList<int> list)
{
    qSort(list.begin(), list.end());

    int iDec = 0;
    foreach(int idx, list) {
      gm_deleteExpo(idx-iDec);
      iDec++;
    }
}

void InputsPanel::clearExpos()
{
  if (QMessageBox::question(this, tr("Clear Expos?"), tr("Really clear all the expos?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
    model.clearInputs();
    emit modified();
    update();
  }
}
