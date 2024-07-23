#include <qt6/QtWidgets/QGraphicsLineItem>
#include <qt6/QtGui/QPen>
#include <qt6/QtCore/QObject>

// A custom QGraphicsLineItem to allow for animation
class AnimatedLineItem : public QObject, public QGraphicsLineItem {
    Q_OBJECT
    Q_PROPERTY(qreal x READ x WRITE setX)

public:
    AnimatedLineItem()
    {
        this->setPen(QPen(Qt::red, 2));
        this->setVisible(false);
        this->setZValue(1);
    }

    using QGraphicsLineItem::QGraphicsLineItem;

    qreal x() {
        return line().p1().x();
    }

    void setImageHeight(qreal h)
    {
        m_h = h;
        this->setLine(QLineF(0, 0, 0, m_h));
    }

    void setX(qreal x) {
        QLineF F = line();
        F.setLine(x, 0, x, m_h);
        setLine(F);
    }



private:
    qreal m_h;
};
