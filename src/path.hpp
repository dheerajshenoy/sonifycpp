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
    AnimatedPathItem()
    {
        this->setPen(QPen(Qt::yellow, 2));
        this->setBrush(QBrush(Qt::yellow));
        this->setRect(0, 0, 10, 10);
    }
    using QGraphicsEllipseItem::QGraphicsEllipseItem;

};

#endif
