#include "mixerslist.h"

MixersList::MixersList(QWidget *parent, bool expo) :
    QListWidget(parent),
    expo(expo)
{
    setFont(QFont("Courier New",12));
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
}

void MixersList::keyPressEvent(QKeyEvent *event)
{
    emit keyWasPressed(event);
}

bool MixersList::dropMimeData( int index, const QMimeData * data, Qt::DropAction action )
{    
    QByteArray dropData = data->data("application/x-qabstractitemmodeldatalist");
    QDataStream stream(&dropData, QIODevice::ReadOnly);
    QByteArray qba;

    while (!stream.atEnd())
    {
        int r,c;
        QMap<int, QVariant> v;
        stream >> r >> c >> v;
        QList<QVariant> lsVars;
        lsVars = v.values();
        QString itemString = lsVars.at(0).toString();
        qba.append(lsVars.at(1).toByteArray().mid(1));

        if(itemString.isEmpty()) {};
    }

    if(qba.length()>0)
    {
        QMimeData *mimeData = new QMimeData;
        if (expo)
          mimeData->setData("application/x-companion9x-expo", qba);
        else
          mimeData->setData("application/x-companion9x-mix", qba);

        emit mimeDropped(index, mimeData, action);
    }


    return true;
}


