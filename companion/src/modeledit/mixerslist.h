#ifndef MIXERSLIST_H
#define MIXERSLIST_H

#include <QtWidgets>

#define MIX_ROW_HEIGHT_INCREASE     8               //how much space is added above mixer row (for new channel), if 0 space adding is disabled
const int  GroupHeaderRole = (Qt::UserRole+2);      //defines new user role for list items. If value is > 0, then space is added before that item

class MixersList : public QListWidget
{
    Q_OBJECT

  public:
    explicit MixersList(QWidget *parent, bool expo);
    void keyPressEvent(QKeyEvent *event);

  signals:
    void mimeDropped(int index, const QMimeData *data, Qt::DropAction action);
    void keyWasPressed(QKeyEvent *event);

  public slots:
    bool dropMimeData(int index, const QMimeData *data, Qt::DropAction action);

  protected:
    virtual QStringList mimeTypes() const;

  private:
    QPoint dragStartPosition;
    bool expo;

};

/**
  @brief Provides sizing and painting of items for our mixes and inputs lists

  This class draws HTML formated strings into QListWidget class
*/
class MixersDelegate : public QStyledItemDelegate
{
    Q_OBJECT
  public:
    inline MixersDelegate(QObject *parent) : QStyledItemDelegate(parent) {};

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

  private:
    void SetupDocument(QTextDocument & doc, const QStyleOptionViewItemV4 & options) const;

};

#endif // MIXERSLIST_H
