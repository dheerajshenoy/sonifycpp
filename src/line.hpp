#include "traverse.hpp"
#include <qt6/QtWidgets/QGraphicsLineItem>
#include <qt6/QtGui/QPen>
#include <qt6/QtCore/QObject>

// A custom QGraphicsLineItem to allow for animation
class AnimatedLineItem : public QObject, public QGraphicsLineItem {
    Q_OBJECT
    Q_PROPERTY(qreal x READ x WRITE setX)
    Q_PROPERTY(qreal y READ y WRITE setY)

public:
    AnimatedLineItem()
    {
        this->setPen(QPen(Qt::red, 2));
        this->setZValue(1);
    }

    using QGraphicsLineItem::QGraphicsLineItem;

    void setImageWidth(qreal w)
    {
        m_w = w;
        this->setLine(QLineF(m_w, 0, m_w, m_h));
        m_line = line();
    }

    void setImageHeight(qreal h)
    {
        m_h = h;
        this->setLine(QLineF(0, 0, 0, m_h));
        m_line = line();
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

private:
    qreal m_h, m_w;
    QLineF m_line;
};
