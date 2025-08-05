#pragma once

#include "traverse.hpp"

#include <QGraphicsLineItem>
#include <QObject>
#include <QPen>

// A custom QGraphicsLineItem to allow for animation
class AnimatedLineItem : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    AnimatedLineItem(QColor color) noexcept
    {
        this->setPen(QPen(color, 2));
        this->setZValue(1);
    }

    using QGraphicsLineItem::QGraphicsLineItem;

    inline void setLeftToRight() noexcept
    {
        this->setLine(QLineF(0, 0, m_w, m_h));
        m_line = line();
    }

    inline void setRightToLeft() noexcept
    {
        this->setLine(QLineF(m_w, 0, m_w, m_h));
        m_line = line();
    }

    inline void setTopToBottom() noexcept
    {
        this->setLine(QLineF(0, 0, m_w, 0));
        m_line = line();
    }

    inline void setBottomToTop() noexcept
    {
        this->setLine(QLineF(0, m_h, m_w, m_h));
        m_line = line();
    }

    inline void setImageWidth(const qreal &w) noexcept
    {
        m_w = w;
    }

    inline void setImageHeight(const qreal &h) noexcept
    {
        m_h = h;
    }

    inline void reset() noexcept
    {
        this->setLine(m_line);
    }

    inline void setLength() noexcept
    {
        auto centerX = m_w / 2.0;
        auto centerY = m_h / 2.0;
        m_l          = sqrt(centerX * centerX + centerY * centerY);
    }

    void setAngle(const qreal &angle) noexcept
    {
        QLineF line = this->line();
        qreal rad   = qDegreesToRadians(static_cast<qreal>(angle));
        auto x      = m_w / 2.0 + m_l * cos(rad);
        auto y      = m_h / 2.0 + m_l * sin(rad);
        line.setP1(QPointF(m_w / 2.0, m_h / 2.0));
        line.setP2(QPointF(x, y));
        this->setLine(line);
        m_angle = angle;
    }

    qreal angle() noexcept
    {
        return m_angle;
    }

private:
    qreal m_h, m_w, m_angle, m_l;
    QLineF m_line;
};
