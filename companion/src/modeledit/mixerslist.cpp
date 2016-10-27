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


/**
    @brief Override to give us different mime type for mixers and inputs
*/
QStringList MixersList::mimeTypes () const
{
    QStringList types;
    if (expo) types << "application/x-companion-expo-item";
    else types << "application/x-companion-mix-item";
    return types;
}

void MixersList::dropEvent(QDropEvent *event)
{
    QList<int> list;
    foreach(QListWidgetItem *item, selectedItems()) {
      int idx= item->data(Qt::UserRole).toByteArray().at(0);
      if(idx >= 0) list << idx;
    }
    if (list.count()<1) return;
    event->acceptProposedAction();
    dropMimeData(indexAt(event->pos()).row(),event->mimeData(),event->dropAction());
}

bool MixersList::dropMimeData( int index, const QMimeData * data, Qt::DropAction action )
{
    // qDebug() << "MixersList::dropMimeData() index:" << index << "formats" << data->formats();
    QByteArray dropData = data->data(expo ? "application/x-companion-expo-item" : "application/x-companion-mix-item");
    if (dropData.isNull() ) return false;
    QDataStream stream(&dropData, QIODevice::ReadOnly);
    QByteArray qba;

    while (!stream.atEnd()) {
        int r,c;
        QMap<int, QVariant> v;
        stream >> r >> c >> v;
        QList<QVariant> lsVars;
        lsVars = v.values();
        qba.append(lsVars.at(1).toByteArray().mid(1));
        // qDebug() << "MixersList::dropMimeData() added" << lsVars.count() << "items, data:" << lsVars;
    }

    if(qba.length()>0) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setData(expo ? "application/x-companion-expo" : "application/x-companion-mix", qba);
        emit mimeDropped(index, mimeData, action);
        delete mimeData;      //is this correct? is mimeData freed elswere??
    }

    return true;
}

/**
    @brief Paints our HTML formated list item text
*/
void MixersDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem options = option;
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
    QStyleOptionViewItem options = option;
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

void MixersDelegate::SetupDocument(QTextDocument & doc, const QStyleOptionViewItem & options) const
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
