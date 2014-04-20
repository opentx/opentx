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
    setItemDelegate(new MixersDelegate(parent));     //set custom paint handler
}

void MixersList::keyPressEvent(QKeyEvent *event)
{
    emit keyWasPressed(event);
}

bool MixersList::dropMimeData( int index, const QMimeData * data, Qt::DropAction action )
{    
    // qDebug() << "MixersList::dropMimeData() index:" << index << "formats" << data->formats();
    QByteArray dropData = data->data("application/x-qabstractitemmodeldatalist");
    QDataStream stream(&dropData, QIODevice::ReadOnly);
    QByteArray qba;

    while (!stream.atEnd()) {
        int r,c;
        QMap<int, QVariant> v;
        stream >> r >> c >> v;
        QList<QVariant> lsVars;
        lsVars = v.values();
        //QString itemString = lsVars.at(0).toString();
        qba.append(lsVars.at(1).toByteArray().mid(1));
        // qDebug() << "MixersList::dropMimeData() added" << lsVars.count() << "items, data:" << lsVars;

        //if(itemString.isEmpty()) {};
    }

    if(qba.length()>0) {
        QMimeData *mimeData = new QMimeData;
        if (expo)
          mimeData->setData("application/x-companion-expo", qba);
        else
          mimeData->setData("application/x-companion-mix", qba);

        emit mimeDropped(index, mimeData, action);
    }


    return true;
}

/**
    @brief Paints our HTML formated list item text
*/
void MixersDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    painter->save();

    //setup html document
    QTextDocument doc;
    SetupDocument(doc, options);

    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    if ( index.model()->data(index, GroupHeaderRole).toInt() > 0 ) {
        //paint with added space
        painter->translate(options.rect.left(), options.rect.top() + MIX_ROW_HEIGHT_INCREASE);
        QRect clip(0, 0, options.rect.width(), options.rect.height() - MIX_ROW_HEIGHT_INCREASE);
        doc.drawContents(painter, clip);
    }
    else {
        painter->translate(options.rect.left(), options.rect.top());
        QRect clip(0, 0, options.rect.width(), options.rect.height());
        doc.drawContents(painter, clip);
    }
    painter->restore();
}


/**
    @brief Returns needed size for our HTML formated list item text
*/
QSize MixersDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    //setup html document
    QTextDocument doc;
    SetupDocument(doc, options);

    //qDebug() << "MixersDelegate::sizeHint() UserRole-> " << index.model()->data(index, Qt::UserRole + 2);
    int height = doc.size().height();
    if ( index.model()->data(index, Qt::UserRole + 2).toInt() > 0 ) {
        //qDebug() << "MixersDelegate::sizeHint() detected channel head"; 
        height = doc.size().height() + MIX_ROW_HEIGHT_INCREASE;
    }
    //qDebug() << "MixersDelegate::sizeHint() options.rect " << options.rect;
    //qDebug() << "MixersDelegate::sizeHint() result " << QSize(doc.idealWidth(), height) << options.rect.width();
    return QSize(doc.idealWidth(), height);
}

void MixersDelegate::SetupDocument(QTextDocument & doc, const QStyleOptionViewItemV4 & options) const
{
    //setup html document
    doc.setHtml(options.text);
    doc.setDefaultFont(options.font);

    //minimize margins (default margins look ugly)
    QTextFrame *tf = doc.rootFrame();
    QTextFrameFormat tff = tf->frameFormat();    
    tff.setMargin(0);
    tf->setFrameFormat(tff);      
}
