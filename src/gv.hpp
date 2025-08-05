/*
 *
 *   Class for the graphics view
 *
 */

#pragma once

#include "circle.hpp"
#include "line.hpp"
#include "path.hpp"
#include "traverse.hpp"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QGraphicsItemAnimation>
#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMimeData>
#include <QMouseEvent>
#include <QObject>
#include <QPainterPath>
#include <QTimeLine>
#include <QTimer>
#include <qevent.h>

class GV : public QGraphicsView
{
    Q_OBJECT
public:
    GV(QWidget *parent = nullptr) noexcept;

    void setPixmap(const QPixmap &img) noexcept;
    QPixmap getPixmap() noexcept;
    void reset() noexcept;
    void setDuration(const double &s) noexcept;
    void setTraverse(const Traverse &t) noexcept;
    void setDrawPathMode(const bool &t) noexcept;
    void getOffset(const size_t &offset) noexcept;
    void setAudioIndex(const int &index) noexcept;
    void setObjColor(const QString &) noexcept;
    void clearDrawPath() noexcept;
    void clearPixmap() noexcept;
    QVector<QPointF> getPathDrawnPos() noexcept;
    ~GV();

signals:
    void animationFinished();
    void drawPathFinished();
    void audioIndexSet();
    void dropFile(QString filepath);
    void pixelClick(QPointF pixelLocation);

protected:
    void mousePressEvent(QMouseEvent *) noexcept override;
    void mouseReleaseEvent(QMouseEvent *) noexcept override;
    void mouseMoveEvent(QMouseEvent *) noexcept override;
    void wheelEvent(QWheelEvent *) noexcept override;
    void dropEvent(QDropEvent *e) noexcept override;
    void dragEnterEvent(QDragEnterEvent *e) noexcept override;
    void dragMoveEvent(QDragMoveEvent *e) noexcept override;

private:
    QGraphicsScene *m_scene   = new QGraphicsScene();
    QGraphicsPixmapItem *m_pi = new QGraphicsPixmapItem();
    AnimatedLineItem *m_li    = nullptr;
    AnimatedCircleItem *m_ci  = nullptr;
    AnimatedPathItem *m_pathi = nullptr;
    double m_duration_s       = 1.0f;
    Traverse m_traverse;
    bool m_draw_path_mode          = false;
    QPainterPath m_painter_path    = QPainterPath();
    QGraphicsPathItem *m_path_item = nullptr;
    QVector<QPointF> m_pathDrawnPixelsPos;
    int m_width, m_height, m_audio_index;
    double m_sqrt;

    bool m_pixel_analyser_mode = false;
    QColor m_obj_color         = QColor("#FF5000");
};
