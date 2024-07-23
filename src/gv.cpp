#include "gv.hpp"
GV::GV(QWidget *parent) : QGraphicsView(parent)
{
    this->setScene(m_scene);
    m_li = new AnimatedLineItem();
    m_scene->addItem(m_li);
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
    m_anim = new QPropertyAnimation(m_li, "x");
    connect(m_anim, &QPropertyAnimation::finished, this, [&]() {
        emit animationFinished();
    });

    switch(t)
    {
        case Traverse::LEFT_TO_RIGHT:
            m_anim->setStartValue(0);
            m_anim->setEndValue(m_pi->pixmap().width());
        break;

        case Traverse::RIGHT_TO_LEFT:
            m_anim->setStartValue(m_pi->pixmap().width());
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
    m_li->setImageHeight(m_pi->pixmap().height());
    m_li->setImageWidth(m_pi->pixmap().width());
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
    m_li->reset();
    m_anim->stop();
    /*m_li->setVisible(false);*/
}

void GV::pause()
{
    m_isPlaying = false;
    /*m_li->setVisible(false);*/

    if (m_anim->state() == QPropertyAnimation::State::Running)
        m_anim->setPaused(true);
}

GV::~GV()
{}
