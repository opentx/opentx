/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef NODE_H
#define NODE_H

#include <QtWidgets>

#define DEFAULT_BALL_SIZE 10
#define BALL_HEIGHT 2

class Edge;
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class Node : public QGraphicsObject
{
  Q_OBJECT

  public:
    Node();

    void addEdge(Edge *edge);
    QList<Edge *> edges() const;

    enum { Type = UserType + 1 };
    int type() const { return Type; }
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setBallSize(int size);
    void stepToCenter(qreal step=10);
    int  getBallSize() {return ballSize;}
    qreal getX();
    qreal getY();
    void setX(qreal newX);
    void setY(qreal newX);

    void setCenteringX(bool val) {centerX = val;}
    void setCenteringY(bool val) {centerY = val;}
    void setFixedX(bool val) {fixedX = val;}
    void setFixedY(bool val) {fixedY = val;}
    bool getFixedX() {return fixedX;}
    bool getFixedY() {return fixedY;}
    void setMinX(int val) {minX = val;};
    void setMaxX(int val) {maxX = val;};
    void setColor(const QColor & color);
    
  signals:
    void moved(int x, int y);
    void focus();
    void unfocus();

  protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
  private:
    bool bPressed;
    bool centerX;
    bool centerY;
    bool fixedX;
    bool fixedY;
    int  ballSize;
    int minX;
    int maxX;
    QList<Edge *> edgeList;
    QPointF newPos;
    QColor nodecolor;
};

#endif
