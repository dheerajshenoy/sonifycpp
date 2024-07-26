#ifndef LINE_HPP
#define LINE_HPP

#include "traverse.hpp"
#include <qt6/QtWidgets/QGraphicsLineItem>
#include <qt6/QtGui/QPen>
#include <qt6/QtCore/QObject>

// A custom QGraphicsLineItem to allow for animation
class AnimatedLineItem : public QObject, public QGraphicsLineItem {
    Q_OBJECT
    Q_PROPERTY(qreal x READ x WRITE setX)
    Q_PROPERTY(qreal y READ y WRITE setY)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle)

public:
    AnimatedLineItem()
    {
        this->setPen(QPen(Qt::red, 2));
        this->setZValue(1);
    }

    using QGraphicsLineItem::QGraphicsLineItem;

    void setLeftToRight()
    {
        this->setLine(QLineF(0, 0, 0, m_h));
        m_line = line();
    }
    void setRightToLeft()
    {
        this->setLine(QLineF(m_w, 0, m_w, m_h));
        m_line = line();
    }

    void setTopToBottom()
    {
        this->setLine(QLineF(0, 0, m_w, 0));
        m_line = line();
    }

    void setBottomToTop()
    {
        this->setLine(QLineF(0, m_h, m_w, m_h));
        m_line = line();
    }

    void setImageWidth(qreal w)
    {
        m_w = w;
    }

    void setImageHeight(qreal h)
    {
        m_h = h;
    }

    qreal x() {
        return line().p1().x();
    }

    void setX(qreal x) {
        QLineF F = line();
        F.setLine(x, 0, x, m_h);
        setLine(F);
    }

    qreal y() {
        return line().p1().y();
    }

    void setY(qreal y) {
        QLineF F = line();
        F.setLine(0, y, m_w, y);
        setLine(F);
    }

    void reset() {
        this->setLine(m_line);
    }

    void setLength()
    {
        auto centerX = m_w / 2.0;
        auto centerY = m_h / 2.0;
        m_l = sqrt(centerX * centerX + centerY * centerY);
    }

    void setAngle(qreal angle)
    {
        QLineF line = this->line();
        qreal rad = qDegreesToRadians(static_cast<qreal>(angle));
        auto x = m_w / 2.0 + m_l * cos(rad);
        auto y = m_h / 2.0 + m_l * sin(rad);
        line.setP1(QPointF(m_w / 2.0, m_h / 2.0));
        line.setP2(QPointF(x, y));
        this->setLine(line);
        m_angle = angle;
    }

    qreal angle()
    {
        return m_angle;
    }

private:
    qreal m_h, m_w, m_angle, m_l;
    QLineF m_line;
};

#endif
