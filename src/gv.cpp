#include "gv.hpp"
#include "qcontainerfwd.h"
#include "qtimeline.h"
GV::GV(QWidget *parent) : QGraphicsView(parent)
{
    this->setScene(m_scene);
    m_scene->addItem(m_pi);
    this->show();
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

    if (m_anim)
    {
        disconnect(m_anim, &QPropertyAnimation::finished, this, nullptr);
        delete m_anim;
        m_anim = nullptr;
    }

    if (m_anim_item)
    {
        delete m_anim_item;
        m_anim_item = nullptr;
    }

    switch(t)
    {
        case Traverse::LEFT_TO_RIGHT:
            m_li = new AnimatedLineItem();
            m_li->setImageHeight(m_pi->pixmap().height());
            m_li->setImageWidth(m_pi->pixmap().width());
            m_scene->addItem(m_li);
            m_anim = new QPropertyAnimation(m_li, "x");
            connect(m_anim, &QPropertyAnimation::finished, this, [&]() {
                emit animationFinished();
            });
            m_anim->setStartValue(0);
            m_anim->setEndValue(m_pi->pixmap().width());
        break;

        case Traverse::RIGHT_TO_LEFT:
            m_li = new AnimatedLineItem();
            m_li->setImageHeight(m_pi->pixmap().height());
            m_li->setImageWidth(m_pi->pixmap().width());
            m_scene->addItem(m_li);
            m_anim = new QPropertyAnimation(m_li, "x");
            connect(m_anim, &QPropertyAnimation::finished, this, [&]() {
                emit animationFinished();
            });
            m_anim->setStartValue(m_pi->pixmap().width());
            m_anim->setEndValue(0);
        break;

        case Traverse::TOP_TO_BOTTOM:
            m_li = new AnimatedLineItem();
            m_li->setImageHeight(m_pi->pixmap().height());
            m_li->setImageWidth(m_pi->pixmap().width());
            m_scene->addItem(m_li);
            m_anim = new QPropertyAnimation(m_li, "y");
            connect(m_anim, &QPropertyAnimation::finished, this, [&]() {
                emit animationFinished();
            });
            m_anim->setStartValue(0);
            m_anim->setEndValue(m_pi->pixmap().height());
        break;

        case Traverse::BOTTOM_TO_TOP:
            m_li = new AnimatedLineItem();
            m_li->setImageHeight(m_pi->pixmap().height());
            m_li->setImageWidth(m_pi->pixmap().width());
            m_scene->addItem(m_li);
            m_anim = new QPropertyAnimation(m_li, "y");
            connect(m_anim, &QPropertyAnimation::finished, this, [&]() {
                emit animationFinished();
            });
            m_anim->setStartValue(m_pi->pixmap().height());
            m_anim->setEndValue(0);
        break;

        case Traverse::CIRCLE_OUTWARDS:
            m_ci = new AnimatedCircleItem();
            m_ci->setImageHeight(m_pi->pixmap().height());
            m_ci->setImageWidth(m_pi->pixmap().width());
            m_ci->setCenter();
            m_scene->addItem(m_ci);

            m_anim = new QPropertyAnimation(m_ci, "radius");
            connect(m_anim, &QPropertyAnimation::finished, this, [&]() {
                emit animationFinished();
            });
            m_anim->setStartValue(0);
            m_anim->setEndValue(std::max(m_pi->pixmap().width() / 2.0, m_pi->pixmap().height() / 2.0));
        break;

        case Traverse::CIRCLE_INWARDS:
            m_ci = new AnimatedCircleItem();
            m_ci->setImageHeight(m_pi->pixmap().height());
            m_ci->setImageWidth(m_pi->pixmap().width());
            m_ci->setCenter();
            m_scene->addItem(m_ci);
            m_anim = new QPropertyAnimation(m_ci, "radius");
            connect(m_anim, &QPropertyAnimation::finished, this, [&]() {
                emit animationFinished();
            });
            m_anim->setEndValue(0);
            m_anim->setStartValue(std::max(m_pi->pixmap().width(), m_pi->pixmap().height()) / 2);
        break;

        case Traverse::CLOCKWISE:
            m_li = new AnimatedLineItem();
            m_li->setImageHeight(m_pi->pixmap().height());
            m_li->setImageWidth(m_pi->pixmap().width());
            m_scene->addItem(m_li);
            m_li->setLength();
            m_anim = new QPropertyAnimation(m_li, "angle");
            connect(m_anim, &QPropertyAnimation::finished, this, [&]() {
                emit animationFinished();
            });
            m_anim->setStartValue(0);
            m_anim->setEndValue(360);
        break;

        case Traverse::ANTICLOCKWISE:
            m_li = new AnimatedLineItem();
            m_li->setImageHeight(m_pi->pixmap().height());
            m_li->setImageWidth(m_pi->pixmap().width());
            m_scene->addItem(m_li);
            m_li->setLength();
            m_anim = new QPropertyAnimation(m_li, "angle");
            connect(m_anim, &QPropertyAnimation::finished, this, [&]() {
                emit animationFinished();
            });
            m_anim->setStartValue(360);
            m_anim->setEndValue(0);
        break;

        case Traverse::PATH:
            m_pathi = new AnimatedPathItem();
            m_scene->addItem(m_pathi);
            /*m_anim = new QPropertyAnimation(m_pathi, "index");*/
            m_anim_item = new QGraphicsItemAnimation();
            m_anim_item->setItem(m_pathi);
            m_timeline->setFrameRange(0, 100);
            connect(m_timeline, &QTimeLine::finished, this, [&]() {
                emit animationFinished();
            });
            m_anim_item->setTimeLine(m_timeline);
            for (int i = 0; i < m_pathDrawnPixelsPos.size(); ++i)
                m_anim_item->setPosAt(i / static_cast<double>(m_pathDrawnPixelsPos.size()), m_pathDrawnPixelsPos[i]);
    }
}

void GV::setDuration(double s)
{
    m_duration_s = s;

    if (m_anim)
        m_anim->setDuration(s * 1000);

    if (!m_timeline)
    {
        m_timeline = new QTimeLine(s * 1000);
    }
    else {
        m_timeline->setDuration(s * 1000);
    }

}

void GV::setPixmap(QPixmap &pix)
{
    if (!pix) return;

    m_pi->setPixmap(pix);
}

void GV::play()
{
    m_isPlaying = true;

    if (m_anim)
    {
        switch(m_anim->state())
        {
            case QPropertyAnimation::State::Stopped:
                m_anim->start();
            break;

            case QPropertyAnimation::State::Paused:
                m_anim->setPaused(false);
            break;
        }
    }

    if (m_timeline)
    {

        switch(m_timeline->state())
        {
            case QTimeLine::State::NotRunning:
                m_timeline->start();
            break;

            case QTimeLine::State::Paused:
                m_timeline->setPaused(false);
            break;
        }
    }
}


void GV::reset()
{
    m_isPlaying = false;
    if (m_li)
        m_li->reset();
    if (m_ci)
        m_ci->reset();
    if (m_anim)
        m_anim->stop();
    if (m_timeline)
        m_timeline->stop();
}

void GV::pause()
{
    m_isPlaying = false;

    if (m_anim && m_anim->state() == QPropertyAnimation::State::Running)
        m_anim->setPaused(true);

    if (m_anim_item && m_timeline && m_timeline->state() == QTimeLine::State::Running)
        m_timeline->setPaused(true);
}

void GV::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::MouseButton::LeftButton)
    {
        if(m_draw_path_mode)
        {
            QPointF s = mapToScene(e->pos());
            qDebug() << s << "\n";
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
        qDebug() << s << "\n";
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
