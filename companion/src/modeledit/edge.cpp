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

#include <QPainter>

#include "edge.h"
#include "node.h"

#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;

Edge::Edge(Node *sourceNode, Node *destNode)
    : arrowSize(10)
{
  setAcceptedMouseButtons(0);
  source = sourceNode;
  dest = destNode;
  source->addEdge(this);
  dest->addEdge(this);
  adjust();
}

Edge::~Edge()
{
}

Node *Edge::sourceNode() const
{
  return source;
}

void Edge::setSourceNode(Node *node)
{
  source = node;
  adjust();
}

Node *Edge::destNode() const
{
  return dest;
}

void Edge::setDestNode(Node *node)
{
  dest = node;
  adjust();
}

void Edge::adjust()
{
  if (!source || !dest)
    return;

  QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
  qreal length = line.length();
  prepareGeometryChange();
  int ballSize = source->getBallSize();
  if (length > qreal(ballSize)) {
    QPointF edgeOffset((line.dx() * ballSize / 2) / length, (line.dy() * ballSize / 2) / length);
    sourcePoint = line.p1() + edgeOffset;
    destPoint = line.p2() - edgeOffset;
  } else {
    sourcePoint = destPoint = line.p1();
  }
}

QRectF Edge::boundingRect() const 
{
  if (!source || !dest)
    return QRectF();

  qreal penWidth = 1;
  qreal extra = (penWidth + arrowSize) / 2.0;

  return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
          destPoint.y() - sourcePoint.y()))
          .normalized()
          .adjusted(-extra, -extra, extra, extra);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  if (!source || !dest)
    return;

  QLineF line(sourcePoint, destPoint);
  if (qFuzzyCompare(line.length(), qreal(0.)))
    return;

  // Draw the line itself
  painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter->drawLine(line);
}
