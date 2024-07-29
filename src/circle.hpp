#ifndef CIRCLE_HPP
#define CIRCLE_HPP
#include "traverse.hpp"
#include <qt6/QtWidgets/QGraphicsEllipseItem>
#include <qt6/QtGui/QPen>
#include <qt6/QtCore/QObject>

// A custom QGraphicsLineItem to allow for animation
class AnimatedCircleItem : public QObject, public QGraphicsEllipseItem {
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)

public:
    AnimatedCircleItem(QColor color)
    {
        this->setPen(QPen(color, 2));
        this->setZValue(1);
    }

    using QGraphicsEllipseItem::QGraphicsEllipseItem;

    void setImageWidth(qreal w)
    {
        m_imgw = w;
        m_rect = this->rect();
    }

    void setImageHeight(qreal h)
    {
        m_imgh = h;
        m_rect = this->rect();
    }
    
    void setCenter()
    {

        centerX = m_imgw / 2.0;
        centerY = m_imgh / 2.0;
        setPos(centerX, centerY);
        setTransformOriginPoint(centerX, centerY);
    }

    qreal radius()
    {
        return m_w;
    }

    void setRadius(qreal r)
    {
        prepareGeometryChange();
        setRect(this->x() - r - m_imgw / 2.0, this->y() - r - m_imgh / 2.0, r * 2, r * 2);
    }

    void reset() {
        this->setRect(m_rect);
    }

private:
    qreal m_x, m_y, m_h, m_w, m_imgh, m_imgw, centerX, centerY;
    QRectF m_rect;
};

#endif
