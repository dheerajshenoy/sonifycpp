#include "qgraphicsview.h"
#include <qt6/QtWidgets/QGraphicsView>
#include <qt6/QtWidgets/QGraphicsScene>
#include <qt6/QtWidgets/QGraphicsLineItem>
#include <qt6/QtCore/QPropertyAnimation>
#include <qt6/QtCore/QObject>
#include "traverse.hpp"
#include "line.hpp"
#include "circle.hpp"

class GV : public QGraphicsView
{
    Q_OBJECT
public:
    GV(QWidget *parent = nullptr);

    void setPixmap(QPixmap &img);
    void play();
    void reset();
    void pause();
    void setDuration(double s);
    void setTraverse(Traverse t);
    ~GV();

signals:
    void animationFinished();

private:

    QGraphicsScene *m_scene = new QGraphicsScene();
    QGraphicsPixmapItem *m_pi = new QGraphicsPixmapItem();

    bool m_isPlaying = false;
    QPropertyAnimation *m_anim;

    AnimatedLineItem *m_li;
    AnimatedCircleItem *m_ci;
    double m_duration_s = 1.0f;

    Traverse m_traverse = Traverse::LEFT_TO_RIGHT;
};
