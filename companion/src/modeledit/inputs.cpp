#include "inputs.h"
#include <QMessageBox>
#include "expodialog.h"
#include "helpers.h"

InputsPanel::InputsPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  expoInserted(false),
  modelPrinter(firmware, generalSettings, model)
{
  inputsCount = firmware->getCapability(VirtualInputs);
  if (inputsCount == 0)
    inputsCount = NUM_STICKS;

  QGridLayout * exposLayout = new QGridLayout(this);

  ExposlistWidget = new MixersList(this, true);
  QPushButton * qbUp = new QPushButton(this);
  QPushButton * qbDown = new QPushButton(this);
  QPushButton * qbClear = new QPushButton(this);
  
  qbUp->setText(tr("Move Up"));
  qbUp->setIcon(CompanionIcon("moveup.png"));
  qbUp->setShortcut(QKeySequence(tr("Ctrl+Up")));
  qbDown->setText(tr("Move Down"));
  qbDown->setIcon(CompanionIcon("movedown.png"));
  qbDown->setShortcut(QKeySequence(tr("Ctrl+Down")));
  qbClear->setText(tr("Clear All Inputs"));
  qbClear->setIcon(CompanionIcon("clear.png"));

  exposLayout->addWidget(ExposlistWidget,1,1,1,3);
  exposLayout->addWidget(qbUp,2,1);
  exposLayout->addWidget(qbClear,2,2);
  exposLayout->addWidget(qbDown,2,3);

  connect(ExposlistWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(expolistWidget_customContextMenuRequested(QPoint)));
  connect(ExposlistWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(expolistWidget_doubleClicked(QModelIndex)));
  connect(ExposlistWidget, SIGNAL(mimeDropped(int,const QMimeData*,Qt::DropAction)), this, SLOT(mimeExpoDropped(int, const QMimeData*, Qt::DropAction)));

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

  // curDest -> destination channel
  // i -> mixer number
  QByteArray qba;
  ExposlistWidget->clear();
  firstLine = true;
  int curDest = -1;

  for (int i=0; i<C9X_MAX_EXPOS; i++) {
    ExpoData *md = &model->expoData[i];

    if (md->mode==0) break;

    QString str;

    while (curDest<(int)md->chn-1) {
      curDest++;
      AddInputLine(-curDest-1);
    }
    if (AddInputLine(i)) {
      curDest++;
    }

  }

  while (curDest<inputsCount-1) {
    curDest++;
    AddInputLine(-curDest-1);
  }

  lock = false;
}


/**
  @brief Creates new input line (list item) and adds it to the list widget

  @note Input lines are now HTML formated because they use same widget as mixers.

  @param[in] dest   defines which input line to create. 
                    If dest < 0 then create empty input slot for input -dest ( dest=-6 -> Input05)
                    if dest >=0 then create used input based on model input data from slot dest (dest=4 -> model expoData[4])

  @retval true      created input number is different from the previous list item
          false     created input number is the same as previous list item
*/
bool InputsPanel::AddInputLine(int dest) 
{
  bool new_ch;
  QString str = getInputText(dest, &new_ch);
  QListWidgetItem *itm = new QListWidgetItem(str);
  QByteArray qba(1, (quint8)dest);
  if (dest >= 0) {
    //add input data
    ExpoData *md = &model->expoData[dest];
    qba.append((const char*)md, sizeof(ExpoData));
  }
  itm->setData(Qt::UserRole, qba);  
#if MIX_ROW_HEIGHT_INCREASE > 0
  if (new_ch && !firstLine) {
    //increase size of this row
    itm->setData(GroupHeaderRole, 1);  
  }
#endif
  ExposlistWidget->addItem(itm);
  firstLine = false;
  //qDebug() << "InputsPanel::AddInputLine(): dest" << dest << "text" << str;
  return new_ch;
}


/**
  @brief Returns HTML formated input representation

 @param[in] dest   defines which input line to create. 
                    If dest < 0 then create empty input slot for input -dest ( dest=-6 -> Input05)
                    if dest >=0 then create used input based on model input data from slot dest (dest=4 -> model expoData[4])

  @retval string    input line in HTML  
*/
QString InputsPanel::getInputText(int dest, bool * new_ch)
{
  QString str;
  if (new_ch) *new_ch = 0;
  if (dest < 0) {
    str = modelPrinter.printInputName(-dest-1);
    if (new_ch) *new_ch = 1;
  }
  else {
    ExpoData & input = model->expoData[dest];

    if ((dest == 0) || (model->expoData[dest-1].chn != input.chn)) {
      if (new_ch) *new_ch = 1;
      if (firmware->getCapability(VirtualInputs))
        str += QString("%1").arg(modelPrinter.printInputName(input.chn), -10, ' ');
      else
        str = modelPrinter.printInputName(input.chn);
    }
    else {
      if (firmware->getCapability(VirtualInputs))
        str = "          ";
      else
        str = "   ";
    }
    str.replace(" ", "&nbsp;");
    str += modelPrinter.printInputLine(input);
  }
  return str;
}

bool InputsPanel::gm_insertExpo(int idx)
{
  if (idx<0 || idx>=C9X_MAX_EXPOS || model->expoData[C9X_MAX_EXPOS-1].mode > 0) {
    QMessageBox::information(this, "Companion", tr("Not enough available inputs!"));
    return false;
  }

  int chn = model->expoData[idx].chn;

  ExpoData * newExpo = model->insertInput(idx);
  newExpo->chn = chn;
  newExpo->weight = 100;
  newExpo->mode = INPUT_MODE_BOTH;

  return true;
}

void InputsPanel::gm_deleteExpo(int index)
{
  model->removeInput(index);
}

void InputsPanel::gm_openExpo(int index)
{
    if(index<0 || index>=C9X_MAX_EXPOS) return;

    ExpoData mixd(model->expoData[index]);

    QString inputName;
    if (firmware->getCapability(VirtualInputs))
      inputName = model->inputNames[mixd.chn];

    ExpoDialog *g = new ExpoDialog(this, *model, &mixd, generalSettings, firmware, inputName);
    if (g->exec())  {
      model->expoData[index] = mixd;
      if (firmware->getCapability(VirtualInputs))
        strncpy(model->inputNames[mixd.chn], inputName.toAscii().data(), 4);
      emit modified();
      update();
    }
    else {
      if (expoInserted) {
        gm_deleteExpo(index);
      }
      expoInserted=false;
      update();
    }
}

int InputsPanel::getExpoIndex(unsigned int dch)
{
    unsigned int i = 0;
    while (model->expoData[i].chn<=dch && model->expoData[i].mode && i<C9X_MAX_EXPOS) i++;
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
    QList<int> list = createExpoListFromSelected();
    if(list.isEmpty()) return;

    QMessageBox::StandardButton ret = QMessageBox::No;

    if(ask)
      ret = QMessageBox::warning(this, "companion",
               tr("Delete Selected Inputs?"),
               QMessageBox::Yes | QMessageBox::No);


    if ((ret == QMessageBox::Yes) || (!ask)) {
        exposDeleteList(list);
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
      mxData.append((char*)&model->expoData[idx], sizeof(ExpoData));
    }

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion-expo", mxData);
    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void InputsPanel::mimeExpoDropped(int index, const QMimeData *data, Qt::DropAction action)
{
    int idx = ExposlistWidget->item(index > 0 ? index-1 : 0)->data(Qt::UserRole).toByteArray().at(0);
  if (action==Qt::CopyAction) {
    pasteExpoMimeData(data, idx);
  }
  else if (action==Qt::MoveAction) {
    QList<int> list = createExpoListFromSelected();
    exposDeleteList(list);
    pasteExpoMimeData(data, idx);
  }
}

#include <iostream>
#include <QtGui/qwidget.h>
void InputsPanel::pasteExpoMimeData(const QMimeData * mimeData, int destIdx)
{
  if (mimeData->hasFormat("application/x-companion-expo")) {
    int idx; // mixer index
    int dch;

    if (destIdx < 0) {
      dch = -destIdx - 1;
      idx = getExpoIndex(dch) - 1; //get expo index to insert
    } else {
      idx = destIdx;
      dch = model->expoData[idx].chn;
    }

    QByteArray mxData = mimeData->data("application/x-companion-expo");

    int i = 0;
    while (i < mxData.size()) {
      idx++;
      if (!gm_insertExpo(idx))
        break;
      ExpoData *md = &model->expoData[idx];
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
      model->expoData[idx].chn = ch;
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
      model->expoData[index].chn = model->expoData[index-1].chn;
    }
    gm_openExpo(index);
}


void InputsPanel::expolistWidget_customContextMenuRequested(QPoint pos)
{
    QPoint globalPos = ExposlistWidget->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion-expo");

    QMenu contextMenu;
    contextMenu.addAction(CompanionIcon("add.png"), tr("&Add"),this,SLOT(expoAdd()),tr("Ctrl+A"));
    contextMenu.addAction(CompanionIcon("edit.png"), tr("&Edit"),this,SLOT(expoOpen()),tr("Enter"));
    contextMenu.addSeparator();
    contextMenu.addAction(CompanionIcon("clear.png"), tr("&Delete"),this,SLOT(exposDelete()),tr("Delete"));
    contextMenu.addAction(CompanionIcon("copy.png"), tr("&Copy"),this,SLOT(exposCopy()),tr("Ctrl+C"));
    contextMenu.addAction(CompanionIcon("cut.png"), tr("&Cut"),this,SLOT(exposCut()),tr("Ctrl+X"));
    contextMenu.addAction(CompanionIcon("paste.png"), tr("&Paste"),this,SLOT(exposPaste()),tr("Ctrl+V"))->setEnabled(hasData);
    contextMenu.addAction(CompanionIcon("duplicate.png"), tr("Du&plicate"),this,SLOT(exposDuplicate()),tr("Ctrl+U"));
    contextMenu.addSeparator();
    contextMenu.addAction(CompanionIcon("moveup.png"), tr("Move Up"),this,SLOT(moveExpoUp()),tr("Ctrl+Up"));
    contextMenu.addAction(CompanionIcon("movedown.png"), tr("Move Down"),this,SLOT(moveExpoDown()),tr("Ctrl+Down"));

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
    ExpoData &src=model->expoData[idx];
    ExpoData &tgt=model->expoData[tdx];
    if (!dir && tdx<0 && src.chn>0) {
      src.chn--;
      return idx;
    }
    else if (!dir && tdx<0) {
      return idx;
    }

    if (memcmp(&src, &temp, sizeof(ExpoData)) == 0) {
      return idx;
    }

    bool tgtempty = (memcmp(&tgt, &temp, sizeof(ExpoData)) == 0);

    if (tgt.chn!=src.chn || tgtempty) {
      if ((dir)  && (src.chn<unsigned(inputsCount-1))) src.chn++;
      if ((!dir) && (src.chn>0)) src.chn--;
      return idx;
    }

    // flip between idx and tgt
    memcpy(&temp, &src, sizeof(ExpoData));
    memcpy(&src, &tgt, sizeof(ExpoData));
    memcpy(&tgt, &temp, sizeof(ExpoData));
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
  if (QMessageBox::question(this, tr("Clear Inputs?"), tr("Really clear all the inputs?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
    model->clearInputs();
    emit modified();
    update();
  }
}
