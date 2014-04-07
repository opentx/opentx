#ifndef MIXERSLIST_H
#define MIXERSLIST_H

#include <QtGui>

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

  private:
    QPoint dragStartPosition;
    bool expo;

};

#endif // MIXERSLIST_H
