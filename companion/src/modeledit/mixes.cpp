#include "mixes.h"
#include "helpers.h"
// #include <QMessageBox>
// #include "expodialog.h"

MixesPanel::MixesPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings):
  ModelPanel(parent, model, generalSettings),
  mixInserted(false)
{
  QGridLayout * mixesLayout = new QGridLayout(this);

  MixerlistWidget = new MixersList(this, false); // TODO enum
  QPushButton * qbUp = new QPushButton(this);
  QPushButton * qbDown = new QPushButton(this);
  QPushButton * qbClear = new QPushButton(this);
  qbUp->setText(tr("Move Up"));
  qbUp->setIcon(CompanionIcon("moveup.png"));
  qbUp->setShortcut(QKeySequence(tr("Ctrl+Up")));
  qbDown->setText(tr("Move Down"));
  qbDown->setIcon(CompanionIcon("movedown.png"));
  qbDown->setShortcut(QKeySequence(tr("Ctrl+Down")));
  qbClear->setText(tr("Clear Mixes"));
  qbClear->setIcon(CompanionIcon("clear.png"));

  mixesLayout->addWidget(MixerlistWidget,1,1,1,3);
  mixesLayout->addWidget(qbUp,2,1);
  mixesLayout->addWidget(qbClear,2,2);
  mixesLayout->addWidget(qbDown,2,3);

  connect(MixerlistWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(mixerlistWidget_customContextMenuRequested(QPoint)));
  connect(MixerlistWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(mixerlistWidget_doubleClicked(QModelIndex)));
  connect(MixerlistWidget,SIGNAL(mimeDropped(int,const QMimeData*,Qt::DropAction)),this,SLOT(mimeMixerDropped(int,const QMimeData*,Qt::DropAction)));

  connect(qbUp,SIGNAL(pressed()),SLOT(moveMixUp()));
  connect(qbDown,SIGNAL(pressed()),SLOT(moveMixDown()));
  connect(qbClear,SIGNAL(pressed()),SLOT(clearMixes()));

  connect(MixerlistWidget,SIGNAL(keyWasPressed(QKeyEvent*)), this, SLOT(mixerlistWidget_KeyPress(QKeyEvent*)));
}

MixesPanel::~MixesPanel()
{
}

QString MixesPanel::getChannelLabel(int curDest)
{
  QString str;
  int outputs = GetEepromInterface()->getCapability(Outputs);
  if (curDest > outputs) {
    str = QObject::tr("X%1  ").arg(curDest-outputs);
  }
  else {
    str = QObject::tr("CH%1").arg(curDest);
    str.append(" ");
    str = str.left(4);
    if (GetEepromInterface()->getCapability(HasChNames)) {
      QString name = model.limitData[curDest-1].name;
      if (!name.isEmpty()) {
        name = QString("(") + name + QString(")");
      }
      name.append("        ");
      str += name.left(8);
    }
  }
  return str;
}

void MixesPanel::update()
{
  // curDest -> destination channel4
  // i -> mixer number
  QByteArray qba;
  MixerlistWidget->clear();
  unsigned int curDest = 0;
  int i;
  unsigned int outputs = GetEepromInterface()->getCapability(Outputs);
  for (i=0; i<GetEepromInterface()->getCapability(Mixes); i++) {
    MixData *md = &model.mixData[i];
    if ((md->destCh==0) || (md->destCh>outputs+(unsigned int)GetEepromInterface()->getCapability(ExtraChannels))) continue;
    QString str = "";
    while (curDest < md->destCh-1) {
      curDest++;
      str = getChannelLabel(curDest);
      qba.clear();
      qba.append((quint8)-curDest);
      QListWidgetItem *itm = new QListWidgetItem(str);
      itm->setData(Qt::UserRole,qba);
      MixerlistWidget->addItem(itm);
    }

    str = getChannelLabel(md->destCh);

    if (curDest != md->destCh) {
      curDest = md->destCh;
    }
    else {
      str.fill(' ');
    }

    switch (md->mltpx) {
      case (1): str += " *"; break;
      case (2): str += " R"; break;
      default:  str += "  "; break;
    };

    str += " " + md->srcRaw.toString();

    str += " " + tr("Weight(%1)").arg(getGVarString(md->weight, true));

    QString phasesStr = getPhasesStr(md->phases, model);
    if (!phasesStr.isEmpty()) str += " " + phasesStr;

    if (md->swtch.type != SWITCH_TYPE_NONE) {
      str += " " + tr("Switch(%1)").arg(md->swtch.toString());
    }

    if (md->carryTrim>0) {
      str += " " + tr("No Trim");
    }
    else if (md->carryTrim<0) {
      str += " " + RawSource(SOURCE_TYPE_TRIM, (-(md->carryTrim)-1)).toString();
    }

    if (md->noExpo)      str += " " + tr("No DR/Expo");
    if (md->sOffset)     str += " " + tr("Offset(%1)").arg(getGVarString(md->sOffset));
    if (md->curve.value) str += " " + md->curve.toString();

    int scale = GetEepromInterface()->getCapability(SlowScale);
    if (scale == 0)
      scale = 1;
    if (md->delayDown || md->delayUp)
      str += tr(" Delay(u%1:d%2)").arg((double)md->delayUp/scale).arg((double)md->delayDown/scale);
    if (md->speedDown || md->speedUp)
      str += tr(" Slow(u%1:d%2)").arg((double)md->speedUp/scale).arg((double)md->speedDown/scale);
    if (md->mixWarn)  str += tr(" Warn(%1)").arg(md->mixWarn);
    if (GetEepromInterface()->getCapability(HasMixerNames)) {
      QString MixerName;
      MixerName.append(md->name);
      if (!MixerName.isEmpty()) {
        str += QString("(%1)").arg(MixerName);
      }
    }
    qba.clear();
    qba.append((quint8)i);
    qba.append((const char*)md, sizeof(MixData));
    QListWidgetItem *itm = new QListWidgetItem(str);
    itm->setData(Qt::UserRole, qba);  // mix number
    MixerlistWidget->addItem(itm); //(str);
  }

  while(curDest<outputs+GetEepromInterface()->getCapability(ExtraChannels)) {
    curDest++;
    QString str = getChannelLabel(curDest);
    qba.clear();
    qba.append((quint8)-curDest);
    QListWidgetItem *itm = new QListWidgetItem(str);
    itm->setData(Qt::UserRole,qba); // add new mixer
    MixerlistWidget->addItem(itm);
  }
}


bool MixesPanel::gm_insertMix(int idx)
{
    if (idx<0 || idx>=GetEepromInterface()->getCapability(Mixes) || model.mixData[GetEepromInterface()->getCapability(Mixes)-1].destCh > 0) {
      QMessageBox::information(this, "companion", tr("Not enough available mixers!"));
      return false;
    }

    int i = model.mixData[idx].destCh;
    memmove(&model.mixData[idx+1],&model.mixData[idx],
        (GetEepromInterface()->getCapability(Mixes)-(idx+1))*sizeof(MixData) );
    memset(&model.mixData[idx],0,sizeof(MixData));
    model.mixData[idx].srcRaw = RawSource(SOURCE_TYPE_NONE);
    model.mixData[idx].destCh = i;
    model.mixData[idx].weight = 100;
    return true;
}

void MixesPanel::gm_deleteMix(int index)
{
    memmove(&model.mixData[index],&model.mixData[index+1],
            (GetEepromInterface()->getCapability(Mixes)-(index+1))*sizeof(MixData));
    memset(&model.mixData[GetEepromInterface()->getCapability(Mixes)-1],0,sizeof(MixData));
}

void MixesPanel::gm_openMix(int index)
{
    if(index<0 || index>=GetEepromInterface()->getCapability(Mixes)) return;

    MixData mixd(model.mixData[index]);
    emit modified();
    update();

    MixerDialog *g = new MixerDialog(this, model, &mixd, generalSettings);
    if(g->exec()) {
      model.mixData[index] = mixd;
      emit modified();
      update();
    } else {
      if (mixInserted) {
        gm_deleteMix(index);
      }
      mixInserted=false;
      emit modified();
      update();
    }
}

int MixesPanel::getMixerIndex(unsigned int dch)
{
    int i = 0;
    while ((model.mixData[i].destCh<=dch) && (model.mixData[i].destCh) && (i<GetEepromInterface()->getCapability(Mixes))) i++;
    if(i==GetEepromInterface()->getCapability(Mixes)) return -1;
    return i;
}

void MixesPanel::mixerlistWidget_doubleClicked(QModelIndex index)
{
    int idx= MixerlistWidget->item(index.row())->data(Qt::UserRole).toByteArray().at(0);
    if (idx<0) {
        int i = -idx;
        idx = getMixerIndex(i); //get mixer index to insert
        if (!gm_insertMix(idx))
          return;
        model.mixData[idx].destCh = i;
        mixInserted=true;
    } else {
      mixInserted=false;
    }
    gm_openMix(idx);
}

void MixesPanel::mixersDeleteList(QList<int> list)
{
    qSort(list.begin(), list.end());

    int iDec = 0;
    foreach(int idx, list) {
      gm_deleteMix(idx-iDec);
      iDec++;
    }
}

QList<int> MixesPanel::createMixListFromSelected()
{
    QList<int> list;
    foreach(QListWidgetItem *item, MixerlistWidget->selectedItems()) {
      int idx= item->data(Qt::UserRole).toByteArray().at(0);
      if(idx>=0 && idx<GetEepromInterface()->getCapability(Mixes)) list << idx;
    }
    return list;
}

// TODO duplicated code
void MixesPanel::setSelectedByMixList(QList<int> list)
{
    for(int i=0; i<MixerlistWidget->count(); i++) {
      int t = MixerlistWidget->item(i)->data(Qt::UserRole).toByteArray().at(0);
      if(list.contains(t))
          MixerlistWidget->item(i)->setSelected(true);
    }
}


void MixesPanel::mixersDelete(bool ask)
{
    QMessageBox::StandardButton ret = QMessageBox::No;

    if(ask)
      ret = QMessageBox::warning(this, "companion",
               tr("Delete Selected Mixes?"),
               QMessageBox::Yes | QMessageBox::No);


    if ((ret == QMessageBox::Yes) || (!ask)) {
      mixersDeleteList(createMixListFromSelected());
      emit modified();
      update();
    }
}

void MixesPanel::mixersCut()
{
    mixersCopy();
    mixersDelete(false);
}

void MixesPanel::mixersCopy()
{

    QList<int> list = createMixListFromSelected();

    QByteArray mxData;
    foreach(int idx, list) {
      mxData.append((char*)&model.mixData[idx],sizeof(MixData));
    }

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion-mix", mxData);
    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void MixesPanel::pasteMixerMimeData(const QMimeData * mimeData, int destIdx)
{
  if(mimeData->hasFormat("application/x-companion-mix")) {
    int idx; // mixer index
    int dch;

    if(destIdx<0) {
      dch = -destIdx;
      idx = getMixerIndex(dch) - 1; //get mixer index to insert
    } else {
      idx = destIdx;
      dch = model.mixData[idx].destCh;
    }

    QByteArray mxData = mimeData->data("application/x-companion-mix");

    int i = 0;
    while(i<mxData.size()) {
      idx++;
      if(idx==GetEepromInterface()->getCapability(Mixes)) break;

      if (!gm_insertMix(idx))
        break;
      MixData *md = &model.mixData[idx];
      memcpy(md,mxData.mid(i,sizeof(MixData)).constData(),sizeof(MixData));
      md->destCh = dch;
      i += sizeof(MixData);
    }

    emit modified();
    update();
  }
}


void MixesPanel::mixersPaste()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    QListWidgetItem *item = MixerlistWidget->currentItem();
    if (item)
      pasteMixerMimeData(mimeData, item->data(Qt::UserRole).toByteArray().at(0));
}

void MixesPanel::mixersDuplicate()
{
    mixersCopy();
    mixersPaste();
}

void MixesPanel::mixerOpen()
{
    int idx = MixerlistWidget->currentItem()->data(Qt::UserRole).toByteArray().at(0);
    if(idx<0) {
      int i = -idx;
      idx = getMixerIndex(i); //get mixer index to insert
      if (!gm_insertMix(idx))
        return;
      model.mixData[idx].destCh = i;
      mixInserted=true;
    } else {
      mixInserted=false;
    }

    gm_openMix(idx);
}

void MixesPanel::mixerAdd()
{
    if (!MixerlistWidget->currentItem())
      return;
    int index = MixerlistWidget->currentItem()->data(Qt::UserRole).toByteArray().at(0);

    if(index<0) {  // if empty then return relavent index
      int i = -index;
      index = getMixerIndex(i); //get mixer index to insert
      if (!gm_insertMix(index))
        return;
      model.mixData[index].destCh = i;
      mixInserted=true;
    } else {
      index++;
      if (!gm_insertMix(index))
        return;
      model.mixData[index].destCh = model.mixData[index-1].destCh;
      mixInserted=true;
    }
    gm_openMix(index);
}

void MixesPanel::mixerlistWidget_customContextMenuRequested(QPoint pos)
{
    QPoint globalPos = MixerlistWidget->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion-mix");

    QMenu contextMenu;
    contextMenu.addAction(CompanionIcon("add.png"), tr("&Add"),this,SLOT(mixerAdd()),tr("Ctrl+A"));
    contextMenu.addAction(CompanionIcon("edit.png"), tr("&Edit"),this,SLOT(mixerOpen()),tr("Enter"));
    contextMenu.addSeparator();
    contextMenu.addAction(CompanionIcon("clear.png"), tr("&Delete"),this,SLOT(mixersDelete()),tr("Delete"));
    contextMenu.addAction(CompanionIcon("copy.png"), tr("&Copy"),this,SLOT(mixersCopy()),tr("Ctrl+C"));
    contextMenu.addAction(CompanionIcon("cut.png"), tr("&Cut"),this,SLOT(mixersCut()),tr("Ctrl+X"));
    contextMenu.addAction(CompanionIcon("paste.png"), tr("&Paste"),this,SLOT(mixersPaste()),tr("Ctrl+V"))->setEnabled(hasData);
    contextMenu.addAction(CompanionIcon("duplicate.png"), tr("Du&plicate"),this,SLOT(mixersDuplicate()),tr("Ctrl+U"));
    contextMenu.addSeparator();
    contextMenu.addAction(CompanionIcon("moveup.png"), tr("Move Up"),this,SLOT(moveMixUp()),tr("Ctrl+Up"));
    contextMenu.addAction(CompanionIcon("movedown.png"), tr("Move Down"),this,SLOT(moveMixDown()),tr("Ctrl+Down"));

    contextMenu.exec(globalPos);
}

void MixesPanel::mimeMixerDropped(int index, const QMimeData *data, Qt::DropAction /*action*/)
{
    int idx= MixerlistWidget->item(index > 0 ? index-1 : 0)->data(Qt::UserRole).toByteArray().at(0);
    pasteMixerMimeData(data, idx);
}

void MixesPanel::mixesEdited()
{
    emit modified();
}

void MixesPanel::mixerlistWidget_KeyPress(QKeyEvent *event)
{
    if(event->matches(QKeySequence::SelectAll)) mixerAdd();  //Ctrl A
    if(event->matches(QKeySequence::Delete))    mixersDelete();
    if(event->matches(QKeySequence::Copy))      mixersCopy();
    if(event->matches(QKeySequence::Cut))       mixersCut();
    if(event->matches(QKeySequence::Paste))     mixersPaste();
    if(event->matches(QKeySequence::Underline)) mixersDuplicate();

    if(event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter) mixerOpen();
    if(event->matches(QKeySequence::MoveToNextLine))
        MixerlistWidget->setCurrentRow(MixerlistWidget->currentRow()+1);
    if(event->matches(QKeySequence::MoveToPreviousLine))
        MixerlistWidget->setCurrentRow(MixerlistWidget->currentRow()-1);
}

int MixesPanel::gm_moveMix(int idx, bool dir) //true=inc=down false=dec=up
{
    if(idx>GetEepromInterface()->getCapability(Mixes) || (idx==0 && !dir) || (idx==GetEepromInterface()->getCapability(Mixes) && dir)) return idx;

    int tdx = dir ? idx+1 : idx-1;
    MixData &src=model.mixData[idx];
    MixData &tgt=model.mixData[tdx];

    unsigned int outputs = GetEepromInterface()->getCapability(Outputs);
    if((src.destCh==0) || (src.destCh>outputs) || (tgt.destCh>outputs)) return idx;

    if (tgt.destCh!=src.destCh) {
        if ((dir)  && (src.destCh<outputs)) src.destCh++;
        if ((!dir) && (src.destCh>0)) src.destCh--;
        return idx;
    }

    //flip between idx and tgt
    MixData temp;
    memcpy(&temp,&src,sizeof(MixData));
    memcpy(&src,&tgt,sizeof(MixData));
    memcpy(&tgt,&temp,sizeof(MixData));
    return tdx;
}

void MixesPanel::moveMixUp()
{
    QList<int> list = createMixListFromSelected();
    QList<int> highlightList;
    foreach(int idx, list) {
      highlightList << gm_moveMix(idx, false);
    }
    emit modified();
    update();
    setSelectedByMixList(highlightList);
}

void MixesPanel::moveMixDown()
{
    QList<int> list = createMixListFromSelected();
    QList<int> highlightList;
    foreach(int idx, list) {
      highlightList << gm_moveMix(idx, true);
    }
    emit modified();
    update();
    setSelectedByMixList(highlightList);
}

void MixesPanel::clearMixes()
{
  if (QMessageBox::question(this, tr("Clear Mixes?"), tr("Really clear all the mixes?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
    model.clearMixes();
    emit modified();
    update();
  }
}
