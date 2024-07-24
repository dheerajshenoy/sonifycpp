#include "sonify.hpp"
#include "qcustomplot.h"


Sonify::Sonify(QWidget *parent)
    : QMainWindow(parent)
{

    m_widget = new QWidget();
    m_side_panel = new QWidget();
    m_side_panel_layout = new QGridLayout();
    m_layout = new QVBoxLayout();
    m_sonify_btn = new QPushButton("Sonify");
    m_play_btn = new QPushButton("Play");
    m_reset_btn = new QPushButton("Reset");
    m_traverse_combo = new QComboBox();
    m_duration_label = new QLabel("Duration: ");
    m_traverse_label = new QLabel("Traversal Mode: ");
    m_num_samples_label = new QLabel("Samples: ");
    m_num_samples_spinbox = new QSpinBox();
    m_num_samples_spinbox->setMinimum(1);
    m_num_samples_spinbox->setMaximum(4000);
    m_num_samples_spinbox->setValue(1024);
    m_splitter = new QSplitter();

    m_status_bar = new QWidget();
    m_status_bar->setMaximumHeight(30);
    m_status_bar_layout = new QHBoxLayout();
    m_statusbar_msg_label = new QLabel();

    m_status_bar_layout->addWidget(m_statusbar_msg_label);
    m_status_bar_layout->addWidget(m_duration_label);

    m_status_bar->setLayout(m_status_bar_layout);

    m_traverse_combo->addItem("Left to Right");
    m_traverse_combo->addItem("Right to Left");
    m_traverse_combo->addItem("Top to Bottom");
    m_traverse_combo->addItem("Bottom to Top");
    m_traverse_combo->addItem("Circle Outwards");
    m_traverse_combo->addItem("Circle Inwards");

    m_widget->setLayout(m_layout);

    waveformplot->addGraph();
    m_layout->addWidget(m_splitter);

    m_splitter->addWidget(m_side_panel);
    m_splitter->addWidget(gv);


    m_side_panel->setFixedWidth(300);
    m_side_panel->setLayout(m_side_panel_layout);

    gv->setAlignment(Qt::AlignmentFlag::AlignCenter);

    m_side_panel_layout->addWidget(m_sonify_btn, 0, 0, 1, 2);
    m_side_panel_layout->addWidget(m_play_btn, 1, 0);
    m_side_panel_layout->addWidget(m_reset_btn, 1, 1);
    m_side_panel_layout->addWidget(m_traverse_label, 2, 0);
    m_side_panel_layout->addWidget(m_traverse_combo, 2, 1);
    m_side_panel_layout->addWidget(m_num_samples_label, 3, 0);
    m_side_panel_layout->addWidget(m_num_samples_spinbox, 3, 1);
    QLabel *m_separator = new QLabel();
    m_side_panel_layout->addWidget(m_separator, 4, 0, 1, 1, Qt::AlignCenter);


    m_layout->addWidget(waveformplot);
    m_layout->addWidget(m_status_bar);
    waveformplot->setVisible(false);
    /*m_sonify_btn->setEnabled(false);*/
    m_play_btn->setEnabled(false);
    m_reset_btn->setEnabled(false);

    this->setCentralWidget(m_widget);
    this->show();

    initMenu();
    initConnections();

    /*Open("/home/neo/Gits/sonifycpp/test2.png");*/
}

void Sonify::setMsg(QString msg, int s)
{
    if (!msg.isEmpty())
    {
        if (s > 0)
        {
            m_statusbar_msg_label->setText(msg);

            QTimer::singleShot(s * 1000, [&]() {
                m_statusbar_msg_label->clear();
            });

            return;
        }
        else {
            m_statusbar_msg_label->setText(msg);
        }
    }
}

void Sonify::initMenu()
{
    m_menu_bar = new QMenuBar();

    m_file_menu = new QMenu("File");
    m_audio_menu = new QMenu("Audio");
    m_view_menu = new QMenu("View");
    m_about_menu = new QMenu("About");

    m_menu_bar->addMenu(m_file_menu);
    m_menu_bar->addMenu(m_view_menu);
    m_menu_bar->addMenu(m_audio_menu);
    m_menu_bar->addMenu(m_about_menu);


    m_file__open = new QAction("Open");
    m_file__exit = new QAction("Exit");

    m_audio__save = new QAction("Save");

    m_file_menu->addAction(m_file__open);
    m_file_menu->addAction(m_file__exit);

    m_audio_menu->addAction(m_audio__save);

    m_view__waveform = new QAction("Waveform");
    m_view__waveform->setCheckable(true);

    m_view_menu->addAction(m_view__waveform);
    this->setMenuBar(m_menu_bar);
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

    connect(m_view__waveform, &QAction::triggered, this, &Sonify::viewWaveform);
}

QVector<double> convertToDouble(const QVector<short>& shortData) {
    QVector<double> doubleData;
    doubleData.reserve(shortData.size());
    for (short value : shortData) {
        doubleData.append(static_cast<double>(value / 255.0));
    }
    return doubleData;
}

void Sonify::viewWaveform(bool state)
{
    if (state)
    {
        /*QVector<double> y = convertToDouble(sonification->getAudioData());*/
        /**/
        /*QVector<double> x;*/
        /*x.resize(y.size());*/
        /**/
        /*for(int i=0; i < y.size(); i++)*/
        /*{*/
        /*    x[i] = i;*/
        /*}*/
        /**/
        /*waveformplot->graph(0)->setPen(QPen(QColor(Qt::red), 2));*/
        /*waveformplot->graph(0)->setData(x, y);*/
        /*waveformplot->xAxis->setRange(0, y.size());*/
        waveformplot->yAxis->setRange(-1, 1);
        waveformplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        waveformplot->xAxis->setLabel("x");
        waveformplot->yAxis->setLabel("y");
        waveformplot->replot();
    }

    waveformplot->setVisible(state);
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
    sonification->setNumSamples(m_num_samples_spinbox->value());
    if (!m_pix)
    {
        if (Open())
        {

            if (m_traverse_combo->currentText() == "Left to Right")
            {
                sonification->Sonify(m_pix, Traverse::LEFT_TO_RIGHT);
                gv->setTraverse(Traverse::LEFT_TO_RIGHT);
            }
            else if (m_traverse_combo->currentText() == "Right to Left")
            {
                sonification->Sonify(m_pix, Traverse::RIGHT_TO_LEFT);
                gv->setTraverse(Traverse::RIGHT_TO_LEFT);
            }

            else if (m_traverse_combo->currentText() == "Top to Bottom")
            {
                sonification->Sonify(m_pix, Traverse::TOP_TO_BOTTOM);
                gv->setTraverse(Traverse::TOP_TO_BOTTOM);
            }

            else if (m_traverse_combo->currentText() == "Bottom to Top")
            {

                sonification->Sonify(m_pix, Traverse::BOTTOM_TO_TOP);
                gv->setTraverse(Traverse::BOTTOM_TO_TOP);
            }

            else if (m_traverse_combo->currentText() == "Circle Outwards")
            {
                sonification->Sonify(m_pix, Traverse::CIRCLE_OUTWARDS);
                gv->setTraverse(Traverse::CIRCLE_OUTWARDS);
            }

            else if (m_traverse_combo->currentText() == "Circle Inwards")
            {
                sonification->Sonify(m_pix, Traverse::CIRCLE_INWARDS);
                gv->setTraverse(Traverse::CIRCLE_INWARDS);
            }

            gv->setDuration(sonification->getDuration());
            m_duration_label->setText("Duration: " + QString::number(sonification->getDuration()) + "s");
            m_play_btn->setEnabled(true);
            m_reset_btn->setEnabled(true);
        }
        return;
    }

    if (m_traverse_combo->currentText() == "Left to Right")
    {
        sonification->Sonify(m_pix, Traverse::LEFT_TO_RIGHT);
        gv->setTraverse(Traverse::LEFT_TO_RIGHT);
    }
    else if (m_traverse_combo->currentText() == "Right to Left")
    {
        sonification->Sonify(m_pix, Traverse::RIGHT_TO_LEFT);
        gv->setTraverse(Traverse::RIGHT_TO_LEFT);
    }

    else if (m_traverse_combo->currentText() == "Top to Bottom")
    {
        sonification->Sonify(m_pix, Traverse::TOP_TO_BOTTOM);
        gv->setTraverse(Traverse::TOP_TO_BOTTOM);
    }

    else if (m_traverse_combo->currentText() == "Bottom to Top")
    {

        sonification->Sonify(m_pix, Traverse::BOTTOM_TO_TOP);
        gv->setTraverse(Traverse::BOTTOM_TO_TOP);
    }

    else if (m_traverse_combo->currentText() == "Circle Outwards")
    {

        sonification->Sonify(m_pix, Traverse::CIRCLE_OUTWARDS);
        gv->setTraverse(Traverse::CIRCLE_OUTWARDS);
    }

    else if (m_traverse_combo->currentText() == "Circle Inwards")
    {

        sonification->Sonify(m_pix, Traverse::CIRCLE_INWARDS);
        gv->setTraverse(Traverse::CIRCLE_INWARDS);
    }

    gv->setDuration(sonification->getDuration());
    m_duration_label->setText("Duration: " + QString::number(sonification->getDuration()) + "s");
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
