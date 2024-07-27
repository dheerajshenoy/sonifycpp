#include "gv.hpp"
#include "qcontainerfwd.h"
#include "qtimeline.h"
GV::GV(QWidget *parent) : QGraphicsView(parent)
{
    this->setScene(m_scene);
    m_scene->addItem(m_pi);
    this->show();
}

void GV::setAudioIndex(int index)
{
    m_audio_index = index;
    emit audioIndexSet();
}

void GV::setDrawPathMode(bool t)
{
    m_draw_path_mode = t;

    if (m_draw_path_mode)
    {
        m_pathDrawnPixelsPos.clear();
        if (!m_path_item)
        {
            m_path_item = new QGraphicsPathItem();
            m_path_item->setPen(QPen(Qt::red, 2));
            m_scene->addItem(m_path_item);
        }
    }
}
void GV::setTraverse(Traverse t)
{
    m_traverse = t;


    // Clean the canvas
    if (m_li && m_scene)
    {
        m_scene->removeItem(m_li);
    }
    if (m_ci && m_scene)
    {
        m_scene->removeItem(m_ci);
    }

    if (m_pathi && m_scene)
    {
        m_scene->removeItem(m_pathi);
    }

    switch(t)
    {
        case Traverse::LEFT_TO_RIGHT:
            if (m_li)
                disconnect(m_li, 0, 0, 0);

            if (!m_li)
            {
                m_li = new AnimatedLineItem();
                m_scene->addItem(m_li);
            }

            connect(this, &GV::audioIndexSet, m_li, [&]() {
                m_li->setLine(m_audio_index, 0, m_audio_index, m_height);
            });
            break;

        case Traverse::RIGHT_TO_LEFT:
            if (m_li)
                disconnect(m_li, 0, 0, 0);

            if (!m_li)
            {
                m_li = new AnimatedLineItem();
                m_scene->addItem(m_li);
            }
            connect(this, &GV::audioIndexSet, m_li, [&]() {
                m_li->setLine(m_width - m_audio_index, 0, m_width - m_audio_index, m_height);
            });
            break;

        case Traverse::TOP_TO_BOTTOM:
            if (m_li)
                disconnect(m_li, 0, 0, 0);

            if (!m_li)
            {
                m_li = new AnimatedLineItem();
                m_scene->addItem(m_li);
            }
            connect(this, &GV::audioIndexSet, m_li, [&]() {
                m_li->setLine(0, m_audio_index, m_width, m_audio_index);
            });
            break;

        case Traverse::BOTTOM_TO_TOP:
            if (m_li)
                disconnect(m_li, 0, 0, 0);

            if (!m_li)
            {
                m_li = new AnimatedLineItem();
                m_scene->addItem(m_li);
            }
            connect(this, &GV::audioIndexSet, m_li, [&]() {
                m_li->setLine(0, m_height - m_audio_index, m_width, m_height - m_audio_index);
            });
            break;

        case Traverse::CIRCLE_OUTWARDS:
            if (m_ci)
                disconnect(m_ci, 0, 0, 0);

            if (!m_ci)
            {
                m_ci = new AnimatedCircleItem();
                m_ci->setImageHeight(m_height);
                m_ci->setImageWidth(m_width);
                m_ci->setCenter();
                m_scene->addItem(m_ci);
            }
            connect(this, &GV::audioIndexSet, m_ci, [&]() {
                m_ci->setRadius(m_audio_index);
            });
            break;

        case Traverse::CIRCLE_INWARDS:
            if (m_ci)
                disconnect(m_ci, 0, 0, 0);

            if (!m_ci)
            {
                m_ci = new AnimatedCircleItem();
                m_ci->setImageHeight(m_height);
                m_ci->setImageWidth(m_width);
                m_ci->setCenter();
                m_scene->addItem(m_ci);
            }
            connect(this, &GV::audioIndexSet, m_ci, [&]() {
                m_ci->setRadius(m_sqrt - m_audio_index);
            });
            break;

        case Traverse::CLOCKWISE:
            if (m_li)
                disconnect(m_li, 0, 0, 0);

            if (!m_li)
            {
                m_li = new AnimatedLineItem();
                m_li->setImageHeight(m_height);
                m_li->setImageWidth(m_width);
                m_li->setLength();
                m_scene->addItem(m_li);
            }
            connect(this, &GV::audioIndexSet, m_li, [&]() {
                m_li->setAngle(m_audio_index);
            });
            break;

        case Traverse::ANTICLOCKWISE:
            if (m_li)
                disconnect(m_li, 0, 0, 0);

            if (!m_li)
            {
                m_li = new AnimatedLineItem();
                m_li->setImageHeight(m_height);
                m_li->setImageWidth(m_width);
                m_li->setLength();
                m_scene->addItem(m_li);
            }
            connect(this, &GV::audioIndexSet, m_li, [&]() {
                m_li->setAngle(360 - m_audio_index);
            });
            break;

        case Traverse::PATH:
            if (m_pathi)
                disconnect(m_pathi, 0, 0, 0);

            if (!m_pathi)
            {
                m_pathi = new AnimatedPathItem();
                m_scene->addItem(m_pathi);
            }
            connect(this, &GV::audioIndexSet, m_pathi, [&]() {
                m_pathi->setPos(m_pathDrawnPixelsPos.at(m_audio_index));
            });
    }
}

void GV::setDuration(double s)
{
    m_duration_s = s;
}

void GV::setPixmap(QPixmap &pix)
{
    if (!pix) return;

    m_pi->setPixmap(pix);

    m_width = pix.width();
    m_height = pix.height();

    m_sqrt = sqrt(m_width * m_width / 4.0 + m_height * m_height / 4.0);
}

void GV::reset()
{
    if (m_li)
        m_li->reset();
    if (m_ci)
        m_ci->reset();

}
void GV::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::MouseButton::LeftButton)
    {
        if(m_draw_path_mode)
        {
            QPointF s = mapToScene(e->pos());
            m_painter_path.moveTo(s);
            m_path_item->setPath(m_painter_path);
            m_pathDrawnPixelsPos.push_back(s);
        }
    }
    QGraphicsView::mousePressEvent(e);
}

void GV::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::MouseButton::LeftButton)
    {
        if(m_draw_path_mode)
        {
            setDrawPathMode(false);
            emit drawPathFinished();
        }
    }
    QGraphicsView::mouseReleaseEvent(e);
}

void GV::mouseMoveEvent(QMouseEvent *e)
{
    if(m_draw_path_mode)
    {
        QPointF s = mapToScene(e->pos());
        m_painter_path.lineTo(s);
        m_path_item->setPath(m_painter_path);
        m_pathDrawnPixelsPos.push_back(s);
    }
    QGraphicsView::mouseMoveEvent(e);
}

GV::~GV()
{}

QVector<QPointF> GV::getPathDrawnPos()
{
    return m_pathDrawnPixelsPos;
}
