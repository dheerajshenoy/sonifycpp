#pragma once

#include <QPainterPath>
#include <QPen>
#include <QColor>
#include <QGraphicsEllipseItem>

class AnimatedPathItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    AnimatedPathItem(QColor color) noexcept
    {
        this->setPen(QPen(color, 2));
        this->setBrush(QBrush(color));
        this->setRect(0, 0, 10, 10);
    }
    using QGraphicsEllipseItem::QGraphicsEllipseItem;

};
