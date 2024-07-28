/*
*
*   Class for the graphics view
*
*/
#ifndef GV_HPP
#define GV_HPP

#include "qevent.h"
#include "qgraphicsview.h"
#include <qt6/QtWidgets/QGraphicsView>
#include <qt6/QtWidgets/QGraphicsScene>
#include <qt6/QtWidgets/QGraphicsLineItem>
#include <qt6/QtWidgets/QGraphicsPathItem>
#include <qt6/QtGui/QPainterPath>
#include <qt6/QtGui/QMouseEvent>
#include <qt6/QtCore/QTimeLine>
#include <qt6/QtCore/QTimer>
#include <qt6/QtWidgets/QGraphicsItemAnimation>
#include <qt6/QtCore/QObject>
#include <qt6/QtCore/QMimeData>
#include <qt6/QtGui/QDropEvent>
#include <qt6/QtGui/QDragEnterEvent>
#include <qt6/QtGui/QDragMoveEvent>
#include "traverse.hpp"
#include "line.hpp"
#include "circle.hpp"
#include "path.hpp"

class GV : public QGraphicsView
{
    Q_OBJECT
public:
    GV(QWidget *parent = nullptr);

    void setPixmap(QPixmap &img);
    void reset();
    void setDuration(double s);
    void setTraverse(Traverse t);
    void setDrawPathMode(bool t);
    void getOffset(size_t &offset);
    void setAudioIndex(int index);
    QVector<QPointF> getPathDrawnPos();
    ~GV();

signals:
    void animationFinished();
    void drawPathFinished();
    void audioIndexSet();
    void dropFile(QString filepath);

protected:
    void mousePressEvent(QMouseEvent *) override; 
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void dropEvent(QDropEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;

private:

    QGraphicsScene *m_scene = new QGraphicsScene();
    QGraphicsPixmapItem *m_pi = new QGraphicsPixmapItem();
    AnimatedLineItem *m_li = nullptr;
    AnimatedCircleItem *m_ci = nullptr;
    AnimatedPathItem *m_pathi = nullptr;
    double m_duration_s = 1.0f;
    Traverse m_traverse;
    bool m_draw_path_mode = false;
    QPainterPath m_painter_path = QPainterPath();
    QGraphicsPathItem *m_path_item = nullptr;
    QVector<QPointF> m_pathDrawnPixelsPos;
    int m_width, m_height, m_audio_index;
    double m_sqrt;
};

#endif
