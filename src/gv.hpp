#include "qgraphicsview.h"
#include <qt6/QtWidgets/QGraphicsView>
#include <qt6/QtWidgets/QGraphicsScene>
#include <qt6/QtWidgets/QGraphicsLineItem>
#include <qt6/QtCore/QPropertyAnimation>
#include "traverse.hpp"
#include "line.hpp"

class GV : public QGraphicsView
{
public:
    GV(QWidget *parent = nullptr);

    void setPixmap(QPixmap &img);
    void play();
    void reset();
    void pause();
    void setDuration(double s);
    ~GV();

private:

    void traverse(Traverse);
    void traverse_normal();
    QGraphicsScene *m_scene = new QGraphicsScene();
    QGraphicsPixmapItem *m_pi = new QGraphicsPixmapItem();

    bool m_isPlaying = false;
    QPropertyAnimation *m_anim;

    AnimatedLineItem *m_li;
    int m_duration_s = 1;
};
