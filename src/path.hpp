#include "qtmetamacros.h"
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
        this->setRect(0, 0, 1, 1);
    }
    using QGraphicsEllipseItem::QGraphicsEllipseItem;

};
