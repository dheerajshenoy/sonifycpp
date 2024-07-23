#include "sonify.hpp"


Sonify::Sonify(QWidget *parent)
    : QMainWindow(parent)
{

    initGUI();

}

void Sonify::initGUI()
{
    m_widget = new QWidget();
    m_layout = new QVBoxLayout();
    m_sonify_btn = new QPushButton("Sonify");
    m_play_btn = new QPushButton("Play");
    m_reset_btn = new QPushButton("Reset");

    m_widget->setLayout(m_layout);
    m_layout->addWidget(gv);

    gv->setAlignment(Qt::AlignmentFlag::AlignCenter);
    m_layout->addWidget(m_sonify_btn);
    m_layout->addWidget(m_play_btn);
    m_layout->addWidget(m_reset_btn);

    this->setCentralWidget(m_widget);
    this->show();

    initConnections();
}

void Sonify::PlayAudio()
{
    if (m_isAudioPlaying)
    {
        m_play_btn->setText("Play");
        gv->pause();
        sonification->pause();
    }
    else  
    {
        m_play_btn->setText("Pause");
        gv->play();
        sonification->play();
    }
    m_isAudioPlaying = !m_isAudioPlaying;
}

void Sonify::initConnections()
{

    connect(m_sonify_btn, &QPushButton::clicked, this, &Sonify::doSonify);
    connect(m_play_btn, &QPushButton::clicked, this, &Sonify::PlayAudio);
    connect(m_reset_btn, &QPushButton::clicked, this, &Sonify::Reset);
}

void Sonify::doSonify()
{
    QPixmap pix = QPixmap("/home/neo/Gits/sonifycpp/download.jpeg");
    pix = pix.scaled(720, 480, Qt::KeepAspectRatio);
    gv->setPixmap(pix);


    sonification->Sonify(pix);
    gv->setDuration(sonification->getDuration());
}

void Sonify::Reset()
{
    gv->reset();
    sonification->reset();
}

Sonify::~Sonify()
{}
