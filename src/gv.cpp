#include "gv.hpp"
GV::GV(QWidget *parent) : QGraphicsView(parent)
{
    this->setScene(m_scene);
    m_scene->addItem(m_pi);
    this->show();
}

void GV::setTraverse(Traverse t)
{
    m_traverse = t;

    if (m_anim)
    {
        disconnect(m_anim, &QPropertyAnimation::finished, this, nullptr);
        delete m_anim;
        m_anim = nullptr;
    }

    switch(t)
    {
        case Traverse::LEFT_TO_RIGHT:
            if (m_li)
                m_scene->removeItem(m_li);
            if (m_ci)
                m_scene->removeItem(m_ci);
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
            if(m_li)
                m_scene->removeItem(m_li);
            if (m_ci)
                m_scene->removeItem(m_ci);
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
            if (m_li)
                m_scene->removeItem(m_li);
            if (m_ci)
                m_scene->removeItem(m_ci);
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
            if (m_li)
                m_scene->removeItem(m_li);
            if (m_ci)
                m_scene->removeItem(m_ci);
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
            if (m_li)
                m_scene->removeItem(m_li);
            if (m_ci)
                m_scene->removeItem(m_ci);

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
            if (m_li)
                m_scene->removeItem(m_li);
            if (m_ci)
            {
                m_scene->removeItem(m_ci);
            }
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
            if (m_li)
                m_scene->removeItem(m_li);
            if (m_ci)
                m_scene->removeItem(m_ci);
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
            if (m_li)
                m_scene->removeItem(m_li);
            if (m_ci)
                m_scene->removeItem(m_ci);
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

    }
}

void GV::setDuration(double s)
{
    m_duration_s = s;

    if (m_anim)
        m_anim->setDuration(m_duration_s * 1000);
}

void GV::setPixmap(QPixmap &pix)
{
    if (!pix) return;

    m_pi->setPixmap(pix);
}

void GV::play()
{
    m_isPlaying = true;

    m_anim->start();

    if (m_anim->state() == QPropertyAnimation::State::Paused)
        m_anim->setPaused(false);
}

void GV::reset()
{
    m_isPlaying = false;
    if (m_li)
        m_li->reset();
    if (m_ci)
        m_ci->reset();
    m_anim->stop();
}

void GV::pause()
{
    m_isPlaying = false;

    if (m_anim->state() == QPropertyAnimation::State::Running)
        m_anim->setPaused(true);
}

GV::~GV()
{}
