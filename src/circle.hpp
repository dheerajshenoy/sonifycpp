#pragma once

#include "traverse.hpp"
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QObject>

// A custom QGraphicsLineItem to allow for animation
class AnimatedCircleItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)
public:
    AnimatedCircleItem(QColor color) noexcept
    {
        this->setPen(QPen(color, 2));
        this->setZValue(1);
    }

    using QGraphicsEllipseItem::QGraphicsEllipseItem;

    inline void setImageWidth(const qreal& w) noexcept
    {
        m_imgw = w;
        m_rect = this->rect();
    }

    inline void setImageHeight(const qreal& h) noexcept
    {
        m_imgh = h;
        m_rect = this->rect();
    }
    
    inline void setCenter() noexcept
    {

        centerX = m_imgw / 2.0;
        centerY = m_imgh / 2.0;
        setPos(centerX, centerY);
        setTransformOriginPoint(centerX, centerY);
    }

    inline qreal radius() noexcept
    {
        return m_w;
    }

    inline void setRadius(const qreal& r) noexcept
    {
        prepareGeometryChange();
        setRect(this->x() - r - m_imgw / 2.0, this->y() - r - m_imgh / 2.0, r * 2, r * 2);
    }

    inline void reset() noexcept
    {
        this->setRect(m_rect);
    }

private:
    qreal m_x, m_y, m_h, m_w, m_imgh, m_imgw, centerX, centerY;
    QRectF m_rect;
};
