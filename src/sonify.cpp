#include "sonify.hpp"


Sonify::Sonify(QWidget *parent)
    : QMainWindow(parent)
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

    /*m_sonify_btn->setEnabled(false);*/
    m_play_btn->setEnabled(false);
    m_reset_btn->setEnabled(false);

    this->setCentralWidget(m_widget);
    this->show();

    initMenu();
    initConnections();

    /*Open("/home/neo/Gits/sonifycpp/test2.png");*/
}

void Sonify::initMenu()
{
    m_menu_bar = new QMenuBar();

    m_file_menu = new QMenu("File");
    m_audio_menu = new QMenu("Audio");
    m_about_menu = new QMenu("About");

    m_menu_bar->addMenu(m_file_menu);
    m_menu_bar->addMenu(m_audio_menu);
    m_menu_bar->addMenu(m_about_menu);

    this->setMenuBar(m_menu_bar);

    m_file__open = new QAction("Open");
    m_file__exit = new QAction("Exit");

    m_audio__save = new QAction("Save");

    m_file_menu->addAction(m_file__open);
    m_file_menu->addAction(m_file__exit);

    m_audio_menu->addAction(m_audio__save);
}

void Sonify::PlayAudio()
{
    if (m_isAudioPlaying)
    {
        m_play_btn->setText("Play");
        gv->pause();
        sonification->pause();
        m_reset_btn->setEnabled(true);
    }
    else  
    {
        m_play_btn->setText("Pause");
        gv->play();
        sonification->play();
        m_reset_btn->setEnabled(false);
    }
    m_isAudioPlaying = !m_isAudioPlaying;
}

void Sonify::initConnections()
{

    connect(m_sonify_btn, &QPushButton::clicked, this, &Sonify::doSonify);
    connect(m_play_btn, &QPushButton::clicked, this, &Sonify::PlayAudio);
    connect(m_reset_btn, &QPushButton::clicked, this, &Sonify::Reset);

    connect(m_file__exit, &QAction::triggered, this, []() {
        QApplication::exit();
    });

    connect(m_file__open, &QAction::triggered, this, [&]() { Sonify::Open(); });

    connect(m_audio__save, &QAction::triggered, this, [&]() { Sonify::Save(); });

    connect(gv, &GV::animationFinished, this, [&]() {
        m_play_btn->setText("Play");
        m_isAudioPlaying = false;
        sonification->reset();
    });
}

bool Sonify::Save(QString filename)
{
    if (filename.isEmpty())
    {
        QFileDialog fd;
        QString filename = fd.getSaveFileName(this, "Save Audio", nullptr, "Audio Files (*.wav)");

        if (filename.isEmpty()) return false;

        // Handle file format
        return sonification->save(filename);
    }

    return false;
}

bool Sonify::Open(QString filename)
{
    if (filename.isEmpty())
    {
        QFileDialog fd;
        QStringList files = fd.getOpenFileNames(this, "Open Image", nullptr, "Image Files (*.png *.jpeg *.jpg)");

        if (files.empty())
            return false;
        m_pix = QPixmap(files[0]);
        if (!m_pix.isNull())
            m_pix = m_pix.scaled(720, 480, Qt::KeepAspectRatio);
        else
            return false;
    }

    else {
        m_pix = QPixmap(filename);
        if (!m_pix.isNull())
            m_pix = m_pix.scaled(720, 480, Qt::KeepAspectRatio);
        else
            return false;
    }
    gv->setPixmap(m_pix);

    return true;

}

void Sonify::doSonify()
{
    if (!m_pix)
    {
        if (Open())
        {
            sonification->Sonify(m_pix);
            fprintf(stderr, "%lf", sonification->getDuration());
            gv->setDuration(sonification->getDuration());
        }
        return;
    }

    sonification->Sonify(m_pix);
    fprintf(stderr, "%lf", sonification->getDuration());
    gv->setDuration(sonification->getDuration());

    m_play_btn->setEnabled(true);
    m_reset_btn->setEnabled(true);
}

void Sonify::Reset()
{
    m_play_btn->setText("Play");
    m_isAudioPlaying = false;
    gv->reset();
    sonification->reset();
}

Sonify::~Sonify()
{}
