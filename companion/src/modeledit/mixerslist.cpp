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
        qba.append(lsVars.last().toByteArray().mid(1));
        qDebug() << "MixersList::dropMimeData() added data " << lsVars;
        if(itemString.isEmpty()) {};
    }

    if(qba.length()>0)
    {
        QMimeData *mimeData = new QMimeData;
        if (expo)
          mimeData->setData("application/x-companion-expo", qba);
        else
          mimeData->setData("application/x-companion-mix", qba);

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

#define QFIXED_MAX (INT_MAX/256)

QRect __textLayoutBounds(const QStyleOptionViewItemV2 &option)
{
    QRect rect = option.rect;
    const bool wrapText = option.features & QStyleOptionViewItemV2::WrapText;
    switch (option.decorationPosition) {
    case QStyleOptionViewItem::Left:
    case QStyleOptionViewItem::Right:
        rect.setWidth(wrapText && rect.isValid() ? rect.width() : (QFIXED_MAX));
        break;
    case QStyleOptionViewItem::Top:
    case QStyleOptionViewItem::Bottom:
        rect.setWidth(wrapText ? option.decorationSize.width() : (QFIXED_MAX));
        break;
    }

    return rect;
}

void MixersDelegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    //Q_D(const QItemDelegate);
    Q_ASSERT(index.isValid());

    QStyleOptionViewItemV4 opt = setOptions(index, option);
    //qDebug() << "MixersDelegate::paint" << opt.rect;

    const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
    opt.features = v2 ? v2->features
                    : QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
    const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
    opt.locale = v3 ? v3->locale : QLocale();
    opt.widget = v3 ? v3->widget : 0;

    // prepare
    painter->save();
    if (false /*d->clipPainting*/)
        painter->setClipRect(opt.rect);

    // get the data and the rectangles

    QVariant value;


    QString text;
    QRect displayRect;
    value = index.data(Qt::DisplayRole);
    if (value.isValid() && !value.isNull()) {
        text = value.toString();
        displayRect = textRectangle(painter, __textLayoutBounds(opt), opt.font, text);
    }

    QRect checkRect;
    Qt::CheckState checkState = Qt::Unchecked;
    value = index.data(Qt::CheckStateRole);
    if (value.isValid()) {
        checkState = static_cast<Qt::CheckState>(value.toInt());
        checkRect = check(opt, opt.rect, value);
    }

    // do the layout
    QRect dummy;
    


    doLayout(opt, &checkRect, &dummy, &displayRect, false);

    // draw the item

    drawBackground(painter, opt, index);
    drawCheck(painter, opt, checkRect, checkState);
    //drawDecoration(painter, opt, decorationRect, pixmap);
    drawDisplay(painter, opt, displayRect, text);
    drawFocus(painter, opt, displayRect);

    // done
    painter->restore();
}

void MixersDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
                                const QRect &rect, const QString &text) const
{

    painter->save();

    QTextDocument doc;
    doc.setHtml(text);
    doc.setDefaultFont(option.font);

    painter->translate(rect.left(), rect.top());
    QRect clip(0, 0, rect.width(), rect.height());
    doc.drawContents(painter, clip);

    painter->restore();
}



// QSize MixersDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
// {
//     QStyleOptionViewItemV4 options = option;
//     initStyleOption(&options, index);

//     QTextDocument doc;
//     doc.setHtml(options.text);
//     doc.setDefaultFont(option.font);
//     doc.setTextWidth(options.rect.width());
//     return QSize(doc.idealWidth(), doc.size().height());
// }