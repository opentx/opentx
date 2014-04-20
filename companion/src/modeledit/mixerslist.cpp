#include "mixerslist.h"

MixersList::MixersList(QWidget *parent, bool expo) :
    QListWidget(parent),
    expo(expo)
{
    setFont(QFont("Courier New",12));
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    //setDragDropMode(QAbstractItemView::InternalMove);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(true);
    setItemDelegate(new MixersDelegate(parent));
}

void MixersList::keyPressEvent(QKeyEvent *event)
{
    emit keyWasPressed(event);
}

bool MixersList::dropMimeData( int index, const QMimeData * data, Qt::DropAction action )
{    
    qDebug() << "MixersList::dropMimeData() " << index << data->formats();
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
        qDebug() << "MixersList::dropMimeData() added data " << lsVars.count() << lsVars;
        if(itemString.isEmpty()) {};
    }

    qDebug() << "MixersList::dropMimeData() qba.length() " << qba.length();
    if(qba.length()>0)
    {
        QMimeData *mimeData = new QMimeData;
        if (expo)
          mimeData->setData("application/x-companion-expo", qba);
        else
          mimeData->setData("application/x-companion-mix", qba);

        qDebug() << "MixersList::dropMimeData() emit mimeDropped " << index << mimeData << action;
        emit mimeDropped(index, mimeData, action);
    }


    return true;
}


void MixersList::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "MixersList::mousePressEvent" ;
    QListWidgetItem *item = currentItem();
    if (item) {
        qDebug() << "MixersList::mousePressEvent" << item->text();    
    }
    if (event->button() == Qt::LeftButton)
    {
        //startPos = event->pos();
    }
    QListWidget::mousePressEvent(event);
 
}


#define     ADDED_SPACE     20

void MixersDelegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    painter->save();

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setDefaultFont(option.font);

    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    if ( index.model()->data(index, Qt::UserRole + 2).toInt() > 0 ) {
        //qDebug() << "MixersDelegate::sizeHint() detected channel head"; 
        painter->translate(options.rect.left(), options.rect.top() + ADDED_SPACE);
        QRect clip(0, 0, options.rect.width(), options.rect.height() - ADDED_SPACE);
        doc.drawContents(painter, clip);
    }
    else {
        painter->translate(options.rect.left(), options.rect.top());
        QRect clip(0, 0, options.rect.width(), options.rect.height());
        doc.drawContents(painter, clip);
    }

    painter->restore();

}



QSize MixersDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setDefaultFont(option.font);
    doc.setTextWidth(options.rect.width());

    //qDebug() << "MixersDelegate::sizeHint() UserRole-> " << index.model()->data(index, Qt::UserRole + 2);

    int height = doc.size().height();
    if ( index.model()->data(index, Qt::UserRole + 2).toInt() > 0 ) {
        //qDebug() << "MixersDelegate::sizeHint() detected channel head"; 
        height = doc.size().height() + ADDED_SPACE;
    }

    //qDebug() << "MixersDelegate::sizeHint() options.rect " << options.rect;
    //qDebug() << "MixersDelegate::sizeHint() result " << QSize(doc.idealWidth(), height);

    return QSize(doc.idealWidth(), height);
}