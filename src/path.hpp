#ifndef PATH_HPP
#define PATH_HPP

#include <qt6/QtGui/QPainterPath>
#include <qt6/QtGui/QPen>
#include <qt6/QtGui/QColor>
#include <qt6/QtWidgets/QGraphicsEllipseItem>

class AnimatedPathItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    AnimatedPathItem(QColor color)
    {
        this->setPen(QPen(color, 2));
        this->setBrush(QBrush(color));
        this->setRect(0, 0, 10, 10);
    }
    using QGraphicsEllipseItem::QGraphicsEllipseItem;

};

#endif
