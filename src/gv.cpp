#include "gv.hpp"
GV::GV(QWidget *parent) : QGraphicsView(parent)
{
    this->setScene(m_scene);
    m_li = new AnimatedLineItem();
    m_scene->addItem(m_li);
    this->show();
}

void GV::setDuration(double s)
{
    m_duration_s = s;
    if (m_anim)
        m_anim->setDuration(m_duration_s * 1000);
}

void GV::setPixmap(QPixmap &pix)
{
    m_pi->setPixmap(pix);
    m_scene->addItem(m_pi);
    
    m_li->setImageHeight(pix.height());
    m_anim = new QPropertyAnimation(m_li, "x");
    m_anim->setStartValue(0);
    m_anim->setEndValue(pix.width());

}

void GV::play()
{
    m_isPlaying = true;
    traverse(Traverse::NORMAL);

    m_li->setVisible(true);

    if (m_anim->state() == QPropertyAnimation::State::Paused)
        m_anim->setPaused(false);
}

void GV::reset()
{
    m_isPlaying = false;
    /*m_li->setLine(0, 0, 0, m_pix.height());*/
    m_anim->stop();
    m_li->setVisible(false);
    m_anim->setPaused(true);
}

void GV::pause()
{
    m_isPlaying = false;
    m_li->setVisible(false);

    if (m_anim->state() == QPropertyAnimation::State::Running)
        m_anim->setPaused(true);
}

void GV::traverse(Traverse mode)
{
    if (m_isPlaying)
    {
        switch(mode)
        {

            case Traverse::NORMAL:
                traverse_normal();
                break;
        }

    }
}

void GV::traverse_normal()
{
    m_anim->start();
}

GV::~GV()
{}
