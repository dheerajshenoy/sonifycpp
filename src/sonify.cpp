#include "sonify.hpp"


QVector<short> readWAVFile(const QString filename)
{
    SF_INFO sfinfo;

    SNDFILE *file = sf_open(filename.toStdString().c_str(), SFM_READ, &sfinfo);

    if(!file)
    {
        fprintf(stderr, "Could not open the file");
        return {};
    }

    QVector<short> data;
    data.resize(sfinfo.frames);

    sf_read_short(file, data.data(), sfinfo.frames);

    sf_close(file);

    return data;
}

Sonify::Sonify(QWidget *parent)
    : QMainWindow(parent)
{
    initWidgets();
    initStatusbar();
    initSidePanel();
    initMenu();
    initConnections();
    initChart();
    this->show();
    /*Open("/home/neo/Gits/sonifycpp/test2.png");*/

    WaveformSaveDialog wd(this);
    wd.exec();
}

void Sonify::initChart()
{
    m_wf_widget->setLayout(m_wf_widget_layout);
    m_wf_widget_layout->addWidget(m_wf_plot);
    m_wf_plot->addGraph();
    m_wf_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    m_wf_plot->yAxis->setRange(-1, 1);

    m_wf_plot->addLayer("Line", 0, QCustomPlot::LayerInsertMode::limAbove);
    m_wf_plot->setCurrentLayer("Line");
    m_wf_vertline_layer = m_wf_plot->layer("Line");
    m_wf_vertline_layer->setMode(QCPLayer::LayerMode::lmBuffered);

    m_wf_vertline = new QCPItemStraightLine(m_wf_plot);
    m_wf_vertline->setPen(QPen(Qt::red, 1));
    m_wf_vertline->point1->setCoords(0, 0);
    m_wf_vertline->point2->setCoords(0, 1);
    m_wf_vertline->setLayer(m_wf_vertline_layer);
    
    m_wf_show_vline_btn->setCheckable(true);
    m_wf_show_vline_btn->setChecked(true);

    m_wf_save_btn = new QPushButton();


    m_wf_widget_layout->addLayout(m_wf_btm_layout);

    m_wf_btm_layout->addWidget(m_wf_show_vline_btn);
    m_wf_btm_layout->addWidget(m_wf_save_btn, 1);

}

void Sonify::initSidePanel()
{
    m_side_panel->setFixedWidth(300);
    m_side_panel->setLayout(m_side_panel_layout);
    m_side_panel_layout->addWidget(m_sonify_btn, 0, 0, 1, 2);
    m_side_panel_layout->addWidget(m_play_btn, 1, 0);
    m_side_panel_layout->addWidget(m_reset_btn, 1, 1);
    m_side_panel_layout->addWidget(m_traverse_label, 2, 0);
    m_side_panel_layout->addWidget(m_traverse_combo, 2, 1);
    m_side_panel_layout->addWidget(m_num_samples_label, 3, 0);
    m_side_panel_layout->addWidget(m_num_samples_spinbox, 3, 1);

    QLabel *m_separator = new QLabel();
    m_side_panel_layout->addWidget(m_separator, 4, 0, 1, 1, Qt::AlignCenter);
}

void Sonify::initStatusbar()
{

    m_status_bar_layout->addWidget(m_statusbar_msg_label);
    m_status_bar_layout->addWidget(m_progress_bar);
    m_status_bar_layout->addWidget(m_stop_sonification_btn);
    m_status_bar_layout->addWidget(m_audio_progress_label);
    m_status_bar_layout->addWidget(m_duration_label);
    m_status_bar->setLayout(m_status_bar_layout);
}

void Sonify::initWidgets()
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
    m_audio_progress_label = new QLabel("");
    m_traverse_label = new QLabel("Traversal Mode: ");
    m_num_samples_label = new QLabel("Samples: ");
    m_num_samples_spinbox = new QSpinBox();
    m_num_samples_spinbox->setMinimum(1);
    m_num_samples_spinbox->setMaximum(4000);
    m_num_samples_spinbox->setValue(1024);
    m_splitter = new QSplitter();
    m_progress_bar = new QProgressBar();
    m_stop_sonification_btn = new QPushButton("Stop");

    m_status_bar = new QWidget();
    m_status_bar->setMaximumHeight(30);
    m_status_bar_layout = new QHBoxLayout();
    m_statusbar_msg_label = new QLabel();

    m_traverse_combo->addItem("Left to Right");
    m_traverse_combo->addItem("Right to Left");
    m_traverse_combo->addItem("Top to Bottom");
    m_traverse_combo->addItem("Bottom to Top");
    m_traverse_combo->addItem("Circle Outwards");
    m_traverse_combo->addItem("Circle Inwards");
    m_traverse_combo->addItem("Clockwise");
    m_traverse_combo->addItem("Anti-Clockwise");
    m_traverse_combo->addItem("Select Region");
    m_traverse_combo->addItem("Draw Path");
    m_traverse_combo->addItem("Random Paths");

    m_widget->setLayout(m_layout);

    m_layout->addWidget(m_splitter);
    m_splitter->addWidget(m_side_panel);
    m_splitter->addWidget(gv);

    m_layout->addWidget(m_status_bar);
    m_play_btn->setEnabled(false);
    m_reset_btn->setEnabled(false);
    m_progress_bar->setVisible(false);
    m_stop_sonification_btn->setVisible(false);

    m_sonify_btn->setEnabled(false);
    gv->setAlignment(Qt::AlignmentFlag::AlignCenter);
    this->setCentralWidget(m_widget);
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
    m_tools_menu = new QMenu("Tools");
    m_view_menu = new QMenu("View");
    m_about_menu = new QMenu("About");

    m_menu_bar->addMenu(m_file_menu);
    m_menu_bar->addMenu(m_tools_menu);
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

    m_isAudioPlaying = !m_isAudioPlaying;
    if (m_isAudioPlaying)
        Play();
    else
        Pause();
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
    connect(sonification, &Sonification::audioindex, gv, [&](int index) {
        gv->setAudioIndex(index);
    });

    connect(sonification, &Sonification::audioFinishedPlaying, gv, [&]() {
        emit gv->animationFinished();
    });

    connect(sonification, &Sonification::audioprogress, gv, [&](double location) {
        m_audio_progress_label->setText(QString::number(location / static_cast<double>(sonification->getSampleRate())));
        m_wf_vertline->point1->setCoords(location, 0);
        m_wf_vertline->point2->setCoords(location, 1);
        m_wf_vertline_layer->replot();
    });

    connect(m_stop_sonification_btn, &QPushButton::clicked, this, [&]() {
        sonification->stopSonification(true);
        m_progress_bar->setVisible(false);
        m_stop_sonification_btn->setVisible(false);
        setMsg("Sonification Stopped", 5);
    });

    connect(gv, &GV::animationFinished, this, [&]() {
        m_play_btn->setText("Play");
        m_isAudioPlaying = false;
        sonification->reset();
        m_traverse_combo->setEnabled(true);
    });

    connect(m_view__waveform, &QAction::triggered, this, &Sonify::viewWaveform);
    connect(sonification, &Sonification::sonificationStopped, this, [&]() {
        m_sonify_btn->setEnabled(true);
    });
    connect(sonification, &Sonification::sonificationDone, this, [&]() {
        gv->setDuration(sonification->getDuration());
        m_duration_label->setText("Duration: " + QString::number(sonification->getDuration()) + "s");
        m_sonify_btn->setEnabled(true);
        m_play_btn->setEnabled(true);
        m_reset_btn->setEnabled(true);
        m_progress_bar->setVisible(false);
        m_stop_sonification_btn->setVisible(false);
        gv->setTraverse(m_mode);
    });
    connect(sonification, &Sonification::sonificationProgress, this, [&](int progress) {
        m_progress_bar->setValue(progress);
    });

    connect(m_wf_save_btn, &QPushButton::clicked, this, [&]() {
        Pause();
    });

    connect(m_wf_show_vline_btn, &QPushButton::clicked, this, [&](bool state) {
        m_wf_show_vline = state;
        m_wf_vertline->setVisible(state);
    });
}

QVector<double> linspace(double start, double stop, int num) {
    QVector<double> result;
    result.reserve(num);
    double step = (stop - start) / (num - 1);
    for (int i = 0; i < num; ++i) {
        result.append(start + i * step);
    }
    return result;
}

void Sonify::viewWaveform(bool state)
{
    if (state)
    {
        m_wf_widget->show();
        /*auto data = readWAVFile("/home/neo/Downloads/gettysburg10.wav");*/

        auto data = sonification->getAudioData();

        QVector<double> x, y;
        auto size = data.size();
        x.resize(size);
        y.resize(size);

        for(int i=0; i < data.size(); i++)
        {
            x[i] = i;
            y[i] = static_cast<double>(data[i] / (double) 32767);
        }

        m_wf_plot->graph(0)->setData(x, y);
        m_wf_plot->xAxis->setRange(0, size);
        m_wf_plot->replot();
    }
    else
        m_wf_widget->close();

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

void Sonify::Open(QString filename)
{
    if (filename.isEmpty())
    {
        QFileDialog fd;
        QStringList files = fd.getOpenFileNames(this, "Open Image", nullptr, "Image Files (*.png *.peg *.pg)");

        if (files.empty())
            return;

        filename = files[0];
    }

    AskForResize(filename);
    m_sonify_btn->setEnabled(true);
}

void Sonify::doSonify()
{
    m_sonify_btn->setEnabled(false);
    sonification->stopSonification(false);
    m_progress_bar->reset();
    m_traverse_combo->setEnabled(false);
    m_num_samples_spinbox->setEnabled(false);
    sonification->setNumSamples(m_num_samples_spinbox->value());

    if (m_traverse_combo->currentText() == "Left to Right")
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::LEFT_TO_RIGHT;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == "Right to Left")
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::RIGHT_TO_LEFT;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == "Top to Bottom")
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::TOP_TO_BOTTOM;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == "Bottom to Top")
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::BOTTOM_TO_TOP;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == "Circle Outwards")
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::CIRCLE_OUTWARDS;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == "Circle Inwards")
    {
        m_stop_sonification_btn->setVisible(true);
        m_progress_bar->setVisible(true);
        m_mode = Traverse::CIRCLE_INWARDS;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == "Clockwise")
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::CLOCKWISE;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == "Anti-Clockwise")
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::ANTICLOCKWISE;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == "Draw Path")
    {
        gv->setDrawPathMode(true);
        connect(gv, &GV::drawPathFinished, this, [&]() {
            m_mode = Traverse::PATH;
            sonification->Sonify(m_pix, gv, m_mode);
        });
    }
}


void Sonify::Reset()
{
    m_play_btn->setText("Play");
    m_isAudioPlaying = false;
    m_wf_vertline->point1->setCoords(0, 0);
    m_wf_vertline->point2->setCoords(0, 0);
    m_wf_vertline_layer->replot();
    gv->reset();
    sonification->reset();
    m_audio_progress_label->setText("");
}

Sonify::~Sonify()
{}

void Sonify::CaptureWindow()
{
    /*pix.save("screen.png");*/
}

void Sonify::AskForResize(QString filename)
{
    QDialog *ask_widget = new QDialog();
    QGridLayout *ask_layout = new QGridLayout();

    m_pix = QPixmap(filename);

    ask_widget->setLayout(ask_layout);
    QString msgtext = QString("The input image (%1) has dimensions (%2,%3). Images with large dimensions tend be to slow during the sonification process and can result in longer waiting time"
    ).arg(filename).arg(m_pix.width()).arg(m_pix.height());
    QLabel *msg = new QLabel(msgtext);
    msg->setWordWrap(true);

    QSpinBox *input_img_width = new QSpinBox();
    QSpinBox *input_img_height = new QSpinBox();
    QPushButton *ok_btn = new QPushButton("Ok"),
                *keep_original_btn = new QPushButton("Ignore and Keep Original Dimension");
    QCheckBox *keep_aspect_ratio_cb = new QCheckBox();

    connect(ok_btn, &QPushButton::clicked, this, [&]() {

        auto width = input_img_width->text().toInt();
        auto height = input_img_height->text().toInt();

        if (keep_aspect_ratio_cb->isChecked())
            m_pix = m_pix.scaled(width, height, Qt::KeepAspectRatio);
        else
            m_pix = m_pix.scaled(width, height, Qt::IgnoreAspectRatio);
        gv->setPixmap(m_pix);
        ask_widget->accept();
    });

    connect(keep_original_btn, &QPushButton::clicked, this, [&]() {
        gv->setPixmap(m_pix);
        ask_widget->reject();
    });

    input_img_width->setRange(0, 5000);
    input_img_height->setRange(0, 5000);
    input_img_width->setValue(m_pix.width());
    input_img_height->setValue(m_pix.height());

    ask_layout->addWidget(msg, 0, 0, 1, 2);
    ask_layout->addWidget(new QLabel("Width"), 1, 0);
    ask_layout->addWidget(input_img_width, 1, 1);
    ask_layout->addWidget(new QLabel("Height"), 2, 0);
    ask_layout->addWidget(input_img_height, 2, 1);
    ask_layout->addWidget(new QLabel("Keep aspect ratio"), 3, 0);
    ask_layout->addWidget(keep_aspect_ratio_cb, 3, 1);
    ask_layout->addWidget(keep_original_btn, 4, 0);
    ask_layout->addWidget(ok_btn, 4, 1);

    ask_widget->exec();

}


void Sonify::Play()
{
    sonification->play();
    m_play_btn->setText("Pause");
    m_reset_btn->setEnabled(false);
    m_num_samples_spinbox->setEnabled(false);
    m_traverse_combo->setEnabled(false);
}

void Sonify::Pause()
{
    sonification->pause();
    m_play_btn->setText("Play");
    m_reset_btn->setEnabled(true);
    m_num_samples_spinbox->setEnabled(true);
    m_traverse_combo->setEnabled(true);
}
