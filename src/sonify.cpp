#include "sonify.hpp"

#include <qboxlayout.h>

Sonify::Sonify(QWidget *parent) : QMainWindow(parent)
{
    initConfigFile();
    initWidgets();
    initStatusbar();
    initMenu();
    initConnections();
    initIcons();

    this->show();
    m_recorder->setGraphicsView(gv);
}

// Set the keybindings
void
Sonify::initKeybinds() noexcept
{
    QShortcut *kb_menubar = new QShortcut(QKeySequence("Ctrl+M"), this);
    QShortcut *kb_open    = new QShortcut(QKeySequence("Ctrl+O"), this);

    connect(kb_menubar, &QShortcut::activated,
            [&]() { m_menu_bar->setVisible(!m_menu_bar->isVisible()); });

    connect(kb_open, &QShortcut::activated, [&]() { Open(); });
}

// Read the configuration file, if it exists
void
Sonify::initConfigFile() noexcept
{
    m_config_dir
        = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir = QDir(m_config_dir);

    if (!dir.exists())
        dir.mkdir(dir.path());

    m_config_file_path
        = QDir::cleanPath(m_config_dir + QDir::separator() + "config.toml");

    // Read from configuration toml file
    if (QFile(m_config_file_path).exists())
        readConfigFile();
    else
    {
        // Initialize defaults
        m_config = {.num_samples    = 512,
                    .min_freq       = 0,
                    .max_freq       = 20000,
                    .img_height     = 100,
                    .img_width      = 100,
                    .freq_range     = {0, 20000},
                    .traversal      = Traverse::LEFT_TO_RIGHT,
                    .sample_rate    = 44100.0f,
                    .panel_position = "top"};
    }
}

void
Sonify::initLeftPanel() noexcept
{
    QGridLayout *layout = new QGridLayout();
    m_panel->setFixedWidth(300);
    m_panel->setLayout(layout);
    layout->addWidget(m_sonify_btn, 0, 0, 1, 2);
    layout->addWidget(m_play_btn, 1, 0);
    layout->addWidget(m_reset_btn, 1, 1);
    layout->addWidget(m_traverse_label, 2, 0);
    layout->addWidget(m_traverse_combo, 2, 1);
    layout->addWidget(m_num_samples_label, 3, 0);
    layout->addWidget(m_num_samples_spinbox, 3, 1);
    layout->addWidget(m_min_freq_label, 4, 0);
    layout->addWidget(m_min_freq_sb, 4, 1);
    layout->addWidget(m_max_freq_label, 5, 0);
    layout->addWidget(m_max_freq_sb, 5, 1);
    layout->addWidget(m_mapping_label, 6, 0);
    layout->addWidget(m_freq_mapping_combo, 6, 1);
    layout->addWidget(new QLabel("Pixel Mapping"), 7, 0);
    layout->addWidget(m_pixel_mapping_combo, 7, 1);

    m_splitter->addWidget(m_panel);
    m_splitter->addWidget(gv);
    m_layout->addWidget(m_splitter);
    m_layout->addWidget(m_status_bar);

    QLabel *m_separator = new QLabel();
    layout->addWidget(m_separator, 8, 0, 1, 1, Qt::AlignCenter);
}

void
Sonify::initRightPanel() noexcept
{
    QGridLayout *layout = new QGridLayout();
    m_panel->setFixedWidth(300);
    m_panel->setLayout(layout);
    layout->addWidget(m_sonify_btn, 0, 0, 1, 2);
    layout->addWidget(m_play_btn, 1, 0);
    layout->addWidget(m_reset_btn, 1, 1);
    layout->addWidget(m_traverse_label, 2, 0);
    layout->addWidget(m_traverse_combo, 2, 1);
    layout->addWidget(m_num_samples_label, 3, 0);
    layout->addWidget(m_num_samples_spinbox, 3, 1);
    layout->addWidget(m_min_freq_label, 4, 0);
    layout->addWidget(m_min_freq_sb, 4, 1);
    layout->addWidget(m_max_freq_label, 5, 0);
    layout->addWidget(m_max_freq_sb, 5, 1);
    layout->addWidget(m_mapping_label, 6, 0);
    layout->addWidget(m_freq_mapping_combo, 6, 1);

    m_splitter->addWidget(gv);
    m_splitter->addWidget(m_panel);
    m_layout->addWidget(m_splitter);
    m_layout->addWidget(m_status_bar);
    layout->setRowStretch(7, 1);
}

void
Sonify::initIcons() noexcept
{
    m_sonify_btn->setText("");
    m_sonify_btn->setIcon(QIcon(":/icons/sonify.svg"));
    m_play_btn->setText("");
    m_play_btn->setIcon(QIcon(":/icons/play-button.svg"));
    m_reset_btn->setText("");
    m_reset_btn->setIcon(QIcon(":/icons/stop.svg"));
    m_file__open->setIcon(QIcon(":/icons/open-file.svg"));
    m_file__close->setIcon(QIcon(":/icons/close-file.svg"));
    m_file__exit->setIcon(QIcon(":/icons/exit.svg"));
    m_tools__waveform->setIcon(QIcon(":/icons/waveform.svg"));
    m_tools__screen_record->setIcon(QIcon(":/icons/screen-record.svg"));
    m_tools__tone_generator->setIcon(QIcon(":/icons/note.svg"));
    m_tools__spectrum_analyzer->setIcon(QIcon(":/icons/tuning-fork.svg"));
    m_effects__wah_wah->setIcon(QIcon(":/icons/wah-wah.svg"));
    m_effects__distortion->setIcon(QIcon(":/icons/distortion.svg"));
}

void
Sonify::initTopPanel() noexcept
{
    QHBoxLayout *layout = new QHBoxLayout();
    m_panel->setLayout(layout);
    layout->addWidget(m_sonify_btn);
    layout->addWidget(m_play_btn);
    layout->addWidget(m_reset_btn);
    layout->addWidget(m_traverse_label);
    layout->addWidget(m_traverse_combo);
    layout->addWidget(m_num_samples_label);
    layout->addWidget(m_num_samples_spinbox);
    layout->addWidget(m_min_freq_label);
    layout->addWidget(m_min_freq_sb);
    layout->addWidget(m_max_freq_label);
    layout->addWidget(m_max_freq_sb);
    layout->addWidget(m_mapping_label);
    layout->addWidget(m_freq_mapping_combo);
    layout->addWidget(new QLabel("Pixel Mapping"));
    layout->addWidget(m_pixel_mapping_combo);
    layout->addStretch(1);

    m_panel->setFixedHeight(40);
    m_layout->addWidget(m_panel);
    m_layout->addWidget(m_splitter);
    m_layout->addWidget(m_status_bar);
    m_splitter->addWidget(gv);
    m_layout->setStretchFactor(gv, 1);
    layout->setSpacing(20);

    m_panel->setEnabled(false);
}

void
Sonify::initBottomPanel() noexcept
{
    m_panel             = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();
    m_panel->setLayout(layout);
    layout->addWidget(m_sonify_btn);
    layout->addWidget(m_play_btn);
    layout->addWidget(m_reset_btn);
    layout->addWidget(m_traverse_label);
    layout->addWidget(m_traverse_combo);
    layout->addWidget(m_num_samples_label);
    layout->addWidget(m_num_samples_spinbox);
    layout->addWidget(m_min_freq_label);
    layout->addWidget(m_min_freq_sb);
    layout->addWidget(m_max_freq_label);
    layout->addWidget(m_max_freq_sb);
    layout->addWidget(m_mapping_label);
    layout->addWidget(m_freq_mapping_combo);
    layout->addWidget(new QLabel("Pixel Mapping"));
    layout->addWidget(m_pixel_mapping_combo);

    layout->setSpacing(20);
    layout->addStretch(1);
    m_panel->setFixedHeight(40);
    m_layout->addWidget(m_splitter);
    m_layout->addWidget(m_panel);
    m_layout->addWidget(m_status_bar);
    m_splitter->addWidget(gv);
    m_layout->setStretchFactor(m_panel, 1);
}

void
Sonify::initStatusbar() noexcept
{
    m_status_bar_layout->addWidget(m_statusbar_msg_label);
    m_status_bar_layout->addWidget(m_progress_bar);
    m_status_bar_layout->addWidget(m_stop_sonification_btn);
    m_status_bar_layout->addWidget(m_audio_progress_label);
    m_status_bar_layout->addWidget(m_duration_label);
    m_status_bar->setLayout(m_status_bar_layout);
}

void
Sonify::initWidgets() noexcept
{
    m_statusbar_msg_label  = new QLabel();
    m_sonify_btn           = new QPushButton("Sonify");
    m_play_btn             = new QPushButton("Play");
    m_reset_btn            = new QPushButton("Reset");
    m_duration_label       = new QLabel("Duration: ");
    m_mapping_label        = new QLabel("Map: ");
    m_audio_progress_label = new QLabel("");
    m_traverse_label       = new QLabel("Traversal Mode: ");
    m_num_samples_label    = new QLabel("Samples: ");
    m_num_samples_spinbox->setMinimum(1);
    m_num_samples_spinbox->setMaximum(4000);
    m_num_samples_spinbox->setValue(m_config.num_samples);
    m_duration_label->setVisible(false);

    m_freq_mapping_combo->addItem("Linear");
    m_freq_mapping_combo->addItem("Logarithmic");
    m_freq_mapping_combo->addItem("Exponential");

    m_pixel_mapping_combo->addItem("Intensity");
    m_pixel_mapping_combo->addItem("HSV");

    m_sonify_btn->setToolTip("Sonify");
    m_reset_btn->setToolTip("Reset");
    m_play_btn->setToolTip("Play");

    int r1 = m_config.freq_range.at(0);
    int r2 = m_config.freq_range.at(1);
    m_min_freq_sb->setRange(r1, r2);
    m_max_freq_sb->setRange(r1, r2);

    m_min_freq_sb->setValue(m_config.min_freq);
    m_max_freq_sb->setValue(m_config.max_freq);

    m_min_freq_label        = new QLabel("Min Freq");
    m_max_freq_label        = new QLabel("Max Freq");
    m_stop_sonification_btn = new QPushButton("Stop");

    m_status_bar->setMaximumHeight(30);

    m_traversal_name_list = {
        "Left to Right",   "Right to Left",  "Top to Bottom", "Bottom to Top",
        "Circle Outwards", "Circle Inwards", "Clockwise",     "Anti-Clockwise",
        "Draw Path",       "Inspect",
    };

    for (const QString &t : m_traversal_name_list)
        m_traverse_combo->addItem(t);

    int index = static_cast<int>(m_config.traversal);
    m_traverse_combo->setCurrentIndex(index);
    m_widget->setLayout(m_layout);
    m_splitter->setStretchFactor(1, 1);
    m_progress_bar->setVisible(false);
    m_stop_sonification_btn->setVisible(false);

    gv->setAlignment(Qt::AlignmentFlag::AlignCenter);
    this->setCentralWidget(m_widget);

    std::string pp = m_config.panel_position;

    if (pp == "top")
        initTopPanel();
    else if (pp == "left")
        initLeftPanel();
    else if (pp == "right")
        initRightPanel();
    else if (pp == "bottom")
        initBottomPanel();
    else
        initLeftPanel();

    m_panel->setEnabled(false);
}

void
Sonify::initMenu() noexcept
{
    m_file_menu  = new QMenu("File");
    m_audio_menu = new QMenu("Audio");
    m_tools_menu = new QMenu("Tools");
    m_view_menu  = new QMenu("View");
    m_help_menu  = new QMenu("Help");

    m_menu_bar->addMenu(m_file_menu);
    m_menu_bar->addMenu(m_tools_menu);
    m_menu_bar->addMenu(m_view_menu);
    m_menu_bar->addMenu(m_audio_menu);
    m_menu_bar->addMenu(m_help_menu);

    m_file__open  = new QAction("Open");
    m_file__close = new QAction("Close");
    m_file__close->setEnabled(false);
    m_file__exit  = new QAction("Exit");
    m_audio__save = new QAction("Save");

    m_audio__save->setEnabled(false);
    m_effects_menu = new QMenu("Effects");

    m_audio_menu->addMenu(m_effects_menu);
    m_file_menu->addAction(m_file__open);
    m_file_menu->addAction(m_file__close);
    m_file_menu->addAction(m_file__exit);

    m_tools__spectrum_analyzer = new QAction("Spectrum Analyzer");
    m_tools__spectrum_analyzer->setCheckable(true);
    m_tools__spectrum_analyzer->setEnabled(false);

    m_tools__pixel_analyzer = new QAction("Pixel Analyzer");
    m_tools__pixel_analyzer->setCheckable(true);
    m_tools__pixel_analyzer->setChecked(false);
    m_tools__pixel_analyzer->setEnabled(false);

    m_tools__tone_generator = new QAction("Tone Generator");
    m_tools__tone_generator->setCheckable(true);
    m_tools__screen_record = new QAction("Screen Record");
    m_tools__screen_record->setCheckable(true);
    m_tools__waveform = new QAction("Waveform");
    m_tools__waveform->setCheckable(true);
    m_tools__waveform->setEnabled(false);
    m_tools__image_settings = new QAction("Image Settings");
    m_tools__image_settings->setIcon(QIcon(":/icons/image.svg"));
    m_tools__image_settings->setEnabled(false);

    m_tools_menu->addAction(m_tools__tone_generator);
    m_tools_menu->addAction(m_tools__spectrum_analyzer);
    m_tools_menu->addAction(m_tools__screen_record);
    m_tools_menu->addAction(m_tools__waveform);
    m_tools_menu->addAction(m_tools__image_settings);

    m_audio_menu->addAction(m_audio__save);

    m_view__panel     = new QAction("Panel");
    m_view__statusbar = new QAction("Statusbar");
    m_view__menubar   = new QAction("Menubar");

    m_view__panel->setCheckable(true);
    m_view__statusbar->setCheckable(true);
    m_view__menubar->setCheckable(true);

    m_view__menubar->setChecked(true);
    m_view__panel->setChecked(true);
    m_view__statusbar->setChecked(true);

    m_view_menu->addAction(m_view__panel);
    m_view_menu->addAction(m_view__statusbar);
    m_view_menu->addAction(m_view__menubar);

    m_effects__reverb        = new QAction("Reverb");
    m_effects__phaser        = new QAction("Phaser");
    m_effects__distortion    = new QAction("Distortion");
    m_effects__wah_wah       = new QAction("Wah-Wah");
    m_effects__filter        = new QAction("Filter");
    m_effects__pitch_shifter = new QAction("Pitch Shifter");

    m_effects_menu->addAction(m_effects__reverb);
    m_effects_menu->addAction(m_effects__phaser);
    m_effects_menu->addAction(m_effects__distortion);
    m_effects_menu->addAction(m_effects__wah_wah);
    m_effects_menu->addAction(m_effects__pitch_shifter);

    m_effects_menu->setEnabled(false);

    m_help__about = new QAction("About");
    m_help_menu->addAction(m_help__about);

    this->setMenuBar(m_menu_bar);
}

// This is used to show statusbar message for specified seconds `s` or
// indefinetely if set equal to -1
void
Sonify::setMsg(QString &&msg, const int &s) noexcept
{
    if (!msg.isEmpty())
    {
        if (s > 0)
        {
            m_statusbar_msg_label->setText(msg);

            QTimer::singleShot(s * 1000,
                               [&]() { m_statusbar_msg_label->clear(); });

            return;
        }
        else
        {
            m_statusbar_msg_label->setText(msg);
        }
    }
}

void
Sonify::initConnections() noexcept
{
    connect(m_sonify_btn, &QPushButton::clicked, this, &Sonify::doSonify);
    connect(m_play_btn, &QPushButton::clicked, this, &Sonify::PlayAudio);
    connect(m_reset_btn, &QPushButton::clicked, this, &Sonify::Reset);
    connect(m_file__exit, &QAction::triggered, this,
            []() { QApplication::exit(); });

    connect(this, &Sonify::fileOpened, this, [this]()
    {
        m_tools__image_settings->setEnabled(true);
        m_panel->setEnabled(true);
    });

    connect(m_file__close, &QAction::triggered, this, &Sonify::Close);
    connect(m_file__open, &QAction::triggered, this, [&]() { Open(); });
    connect(m_audio__save, &QAction::triggered, this, [&]() { Save(); });
    connect(sonification, &Sonification::audioIndex, gv, &GV::setAudioIndex);

    connect(sonification, &Sonification::audioFinishedPlaying, gv, [this]()
    {
        m_reset_btn->setEnabled(true);
        emit gv->animationFinished();
    });

    connect(sonification, &Sonification::audioProgress, gv,
            [this](double location)
    {
        m_audio_progress_label->setText(QString::number(
            location / static_cast<double>(sonification->sampleRate())));
        if (m_wf_widget)
            m_wf_widget->setVertLinePosition(
                location / static_cast<double>(sonification->sampleRate()));
    });

    connect(m_stop_sonification_btn, &QPushButton::clicked, this, [this]()
    {
        sonification->stopSonification(true);
        m_progress_bar->setVisible(false);
        m_stop_sonification_btn->setVisible(false);
        m_num_samples_spinbox->setEnabled(true);
        m_traverse_combo->setEnabled(true);
        setMsg("Sonification Stopped", 5);
    });

    connect(gv, &GV::animationFinished, this, [this]()
    {
        m_play_btn->setIcon(QIcon(":/icons/play-button.svg"));
        /*m_play_btn->setText("Play");*/
        m_isAudioPlaying = false;
        sonification->reset();
        m_traverse_combo->setEnabled(true);
    });

    connect(m_tools__waveform, &QAction::triggered, this,
            &Sonify::viewWaveform);

    connect(m_tools__image_settings, &QAction::triggered, this, [this]()
    {
        ImageEditorDialog *e = new ImageEditorDialog(this);
        auto pix             = gv->getPixmap();
        e->setPixmap(pix);
        connect(e, &ImageEditorDialog::optionsApplied, this,
                [this, &pix](ImageOptions options)
        {
            QImage img = m_pix.toImage();
            img        = Utils::changeBrightness(img, options.Brightness,
                                                 m_pix.height(), m_pix.width());
            img        = Utils::changeSaturation(img, options.Saturation,
                                                 m_pix.height(), m_pix.width());
            img = Utils::changeContrast(img, options.Contrast, m_pix.height(),
                                        m_pix.width());
            img = Utils::changeGamma(img, options.Gamma, m_pix.height(),
                                     m_pix.width());
            if (options.Grayscale)
                img = Utils::convertToGrayscale(img, m_pix.height(),
                                                m_pix.width());
            if (options.Invert)
                img = Utils::invertColor(img, m_pix.height(), m_pix.width());
            pix = QPixmap::fromImage(img);
            gv->setPixmap(pix);
        });
        connect(e, &ImageEditorDialog::resetImage, this,
                [this]() { gv->setPixmap(m_pix); });
        e->setAttribute(Qt::WA_DeleteOnClose);
        e->open();
    });

    connect(sonification, &Sonification::sonificationStopped, this,
            [this]() { m_sonify_btn->setEnabled(true); });

    connect(sonification, &Sonification::sonificationDone, this, [this]()
    {
        m_panel->setEnabled(true);
        m_audio__save->setEnabled(true);
        m_tools__waveform->setEnabled(true);
        m_tools__spectrum_analyzer->setEnabled(true);
        m_duration_label->setVisible(true);
        gv->setDuration(sonification->duration());
        m_duration_label->setText(
            "Duration: " + QString::number(sonification->duration()) + "s");
        m_sonify_btn->setEnabled(true);
        m_play_btn->setEnabled(true);
        m_reset_btn->setEnabled(true);
        m_progress_bar->setVisible(false);
        m_stop_sonification_btn->setVisible(false);
        gv->setTraverse(m_mode);
    });

    connect(sonification, &Sonification::sonificationProgress, this,
            [this](int progress) { m_progress_bar->setValue(progress); });

    connect(gv, &GV::dropFile, this,
            [this](QString droppedFilePath) { Open(droppedFilePath); });

    connect(m_tools__spectrum_analyzer, &QAction::triggered, this,
            [this](bool state)
    {
        if (state)
        {
            m_sp = new SpectrumAnalyzer(this);
            connect(m_sp, &SpectrumAnalyzer::closed, this, [this]()
            {
                m_sp->close();
                delete m_sp;
                m_sp = nullptr;
                m_tools__spectrum_analyzer->setChecked(false);
            });
            auto data = sonification->audioData();
            auto sr   = sonification->sampleRate();
            m_sp->setData(data, sr);
            m_sp->open();
        }
    });

    connect(m_tools__tone_generator, &QAction::triggered, this,
            [this](bool state)
    {
        if (state)
        {
            if (!m_tg)
            {
                m_tg = new ToneGenerator(this);
                connect(m_tg, &ToneGenerator::closed, this, [this]()
                {
                    m_tools__tone_generator->setChecked(false);
                    m_tg->close();
                    disconnect(m_tg, 0, 0, 0);
                    delete m_tg;
                    m_tg = nullptr;
                });
                m_tg->open();
            }
        }
    });

    connect(m_tools__screen_record, &QAction::triggered, this,
            [this](bool state)
    {
        if (state)
        {
            connect(m_recorder, &ScreenRecorder::finished, this,
                    [this]() { qDebug() << "Screen recording finished"; });
            m_recorder->Start();
        }
        else
            m_recorder->Stop();
    });

    connect(m_help__about, &QAction::triggered, this, [this]()
    {
        AboutDialog *aboutDialog = new AboutDialog(this);
        aboutDialog->open();
    });

    connect(m_effects__reverb, &QAction::triggered, this, [this]()
    {
        ReverbDialog *reverbDialog = new ReverbDialog(this);
        reverbDialog->setData(sonification->audioData(),
                              sonification->sampleRate());
        connect(reverbDialog, &ReverbDialog::outputReady, this,
                [&](QVector<short> reverbedOutput)
        { sonification->setAudioData(reverbedOutput); });
        reverbDialog->open();
    });

    connect(m_view__menubar, &QAction::triggered, this,
            [this](bool state) { m_menu_bar->setVisible(state); });

    connect(m_view__statusbar, &QAction::triggered, this,
            [this](bool state) { m_status_bar->setVisible(state); });

    connect(m_view__panel, &QAction::triggered, this,
            [this](bool state) { m_panel->setVisible(!state); });
}

// This is used to set the state of the audio playback. Call Play() if we
// have to play the audio or call Pause() if we want to pause.
void
Sonify::PlayAudio() noexcept
{
    m_isAudioPlaying = !m_isAudioPlaying;
    if (m_isAudioPlaying)
        Play();
    else
        Pause();
    qDebug() << m_isAudioPlaying;
}

// Function that is similar to numpy linspace.
QVector<double>
linspace(double start, double stop, int num) noexcept
{
    QVector<double> result;
    result.reserve(num);
    double step = (stop - start) / (num - 1);
    for (int i = 0; i < num; ++i)
    {
        result.append(start + i * step);
    }
    return result;
}

// Function that handles the waveform widget viewing
void
Sonify::viewWaveform(const bool &state) noexcept
{
    if (state)
    {
        if (!m_wf_widget)
        {
            m_wf_widget = new WaveformWidget();
            connect(m_wf_widget, &WaveformWidget::closed, this, [&]()
            {
                m_tools__waveform->setChecked(false);
                disconnect(m_wf_widget, 0, 0, 0);
                m_wf_widget->close();
                delete m_wf_widget;
                m_wf_widget = nullptr;
            });
        }
        auto data = sonification->audioData();
        m_wf_widget->setData(data, sonification->sampleRate());
        m_wf_widget->show();
    }
    else
        m_wf_widget->close();
}

// Function that handles saving the WAV audio file
bool
Sonify::Save(const QString &filename) noexcept
{
    if (filename.isEmpty())
    {
        QFileDialog fd;
        QString filename = fd.getSaveFileName(this, "Save Audio", nullptr,
                                              "Audio Files (*.wav)");

        if (filename.isEmpty())
            return false;

        return sonification->save(filename);
    }

    return false;
}

// Function that handles Opening an image. If filename is specified, image
// will be opened, otherwise a file dialog will ask for the file.
void
Sonify::Open(QString filename) noexcept
{

    if (filename.isEmpty())
    {
        QFileDialog fd;
        QStringList files = fd.getOpenFileNames(
            this, "Open Image", nullptr, "Image Files (*.png *.jpeg *.jpg)");

        if (files.empty())
            return;

        filename = files[0];
    }

    if (m_def_ask_for_resize)
        AskForResize(filename);
    else
    {
        m_pix = QPixmap(filename);
        if (m_def_keep_aspect)
            m_pix
                = m_pix.scaled(m_def_width, m_def_height, Qt::KeepAspectRatio);
        else
            m_pix = m_pix.scaled(m_def_width, m_def_height,
                                 Qt::IgnoreAspectRatio);
    }

    gv->setPixmap(m_pix);
    m_sonify_btn->setEnabled(true);
    m_file__close->setEnabled(true);
    // sonification->clear();
    m_play_btn->setEnabled(false);
    m_reset_btn->setEnabled(false);
    emit fileOpened();
}

void
Sonify::Close() noexcept
{
    gv->scene()->clear();
    m_tools__image_settings->setEnabled(false);
    m_tools__pixel_analyzer->setEnabled(false);
    m_tools__spectrum_analyzer->setEnabled(false);
    m_tools__waveform->setEnabled(false);
    m_panel->setEnabled(false);
    m_effects_menu->setEnabled(false);
}

// Function that handles action of sonification
void
Sonify::doSonify() noexcept
{
    sonification->stopSonification(false);
    m_progress_bar->reset();
    m_panel->setEnabled(false);
    sonification->setNumSamples(m_num_samples_spinbox->value());

    switch (m_freq_mapping_combo->currentIndex())
    {

        case 0:
            sonification->setFreqMap(FreqMap::Linear);
            break;

        case 1:
            sonification->setFreqMap(FreqMap::Log);
            break;

        case 2:
            sonification->setFreqMap(FreqMap::Exp);
            break;
    }

    auto min_freq = m_min_freq_sb->text().toInt();
    auto max_freq = m_max_freq_sb->text().toInt();

    Sonifier::MapFunc mapFunc;
    switch (m_pixel_mapping_combo->currentIndex())
    {
        case 0:
            mapFunc = [this](const QVector<PixelColumn> &cols)
            {
                return sonification->sonifier()->mapping()->Map__Intensity(
                    cols);
            };
            break;

        case 1:
            mapFunc = [this](const QVector<PixelColumn> &cols)
            {
                return sonification->sonifier()->mapping()->Map__HSV(cols);
            };
            break;
    }

    if (m_traverse_combo->currentText()
        == m_traversal_name_list[static_cast<int>(Traverse::LEFT_TO_RIGHT)])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::LEFT_TO_RIGHT;
        sonification->Sonify(m_pix, gv, mapFunc, m_mode, min_freq, max_freq);
    }

    else if (m_traverse_combo->currentText()
             == m_traversal_name_list[static_cast<int>(
                 Traverse::RIGHT_TO_LEFT)])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::RIGHT_TO_LEFT;
        sonification->Sonify(m_pix, gv, mapFunc, m_mode, min_freq, max_freq);
    }

    else if (m_traverse_combo->currentText()
             == m_traversal_name_list[static_cast<int>(
                 Traverse::TOP_TO_BOTTOM)])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::TOP_TO_BOTTOM;
        sonification->Sonify(m_pix, gv, mapFunc, m_mode, min_freq, max_freq);
    }

    else if (m_traverse_combo->currentText()
             == m_traversal_name_list[static_cast<int>(
                 Traverse::BOTTOM_TO_TOP)])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::BOTTOM_TO_TOP;
        sonification->Sonify(m_pix, gv, mapFunc, m_mode, min_freq, max_freq);
    }

    else if (m_traverse_combo->currentText()
             == m_traversal_name_list[static_cast<int>(
                 Traverse::CIRCLE_OUTWARDS)])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::CIRCLE_OUTWARDS;
        sonification->Sonify(m_pix, gv, mapFunc, m_mode, min_freq, max_freq);
    }

    else if (m_traverse_combo->currentText()
             == m_traversal_name_list[static_cast<int>(
                 Traverse::CIRCLE_INWARDS)])
    {
        m_stop_sonification_btn->setVisible(true);
        m_progress_bar->setVisible(true);
        m_mode = Traverse::CIRCLE_INWARDS;
        sonification->Sonify(m_pix, gv, mapFunc, m_mode, min_freq, max_freq);
    }

    else if (m_traverse_combo->currentText()
             == m_traversal_name_list[static_cast<int>(Traverse::CLOCKWISE)])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::CLOCKWISE;
        sonification->Sonify(m_pix, gv, mapFunc, m_mode, min_freq, max_freq);
    }

    else if (m_traverse_combo->currentText()
             == m_traversal_name_list[static_cast<int>(
                 Traverse::ANTICLOCKWISE)])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::ANTICLOCKWISE;
        sonification->Sonify(m_pix, gv, mapFunc, m_mode, min_freq, max_freq);
    }

    else if (m_traverse_combo->currentText()
             == m_traversal_name_list[static_cast<int>(Traverse::PATH)])
    {
        gv->clearDrawPath();
        gv->setDrawPathMode(true);
        connect(gv, &GV::drawPathFinished, this, [&]()
        {
            m_mode = Traverse::PATH;
            sonification->Sonify(m_pix, gv, mapFunc, m_mode, min_freq,
                                 max_freq);
        });
    }

    else if (m_traverse_combo->currentText()
             == m_traversal_name_list[static_cast<int>(Traverse::INSPECT)])
    {
        /*gv->setPixelAnalyserMode(true);*/
        m_mode = Traverse::INSPECT;
        sonification->Sonify(m_pix, gv, mapFunc, m_mode, min_freq, max_freq);
    }
}

// Reset btn function
void
Sonify::Reset() noexcept
{
    /*m_play_btn->setText("Play");*/
    m_isAudioPlaying = false;

    gv->reset();
    if (m_wf_widget)
        m_wf_widget->resetVertLine();
    sonification->reset();
    m_audio_progress_label->setText("");
    m_num_samples_spinbox->setEnabled(true);
}

// TODO: Screen record
void
Sonify::CaptureWindow() noexcept
{
}

// Function to ask for image resize when opening
void
Sonify::AskForResize(const QString &filename) noexcept
{
    QDialog *ask_widget     = new QDialog(this);
    QGridLayout *ask_layout = new QGridLayout();

    m_pix = QPixmap(filename);

    ask_widget->setLayout(ask_layout);
    QString msgtext
        = QString("The input image (%1) has dimensions (%2,%3). Images with "
                  "large dimensions tend be to slow during the sonification "
                  "process and can result in longer waiting time")
              .arg(filename)
              .arg(m_pix.width())
              .arg(m_pix.height());
    QLabel *msg = new QLabel(msgtext);
    msg->setWordWrap(true);

    QSpinBox *input_img_width  = new QSpinBox();
    QSpinBox *input_img_height = new QSpinBox();
    QPushButton *ok_btn        = new QPushButton("Ok"),
                *keep_original_btn
                = new QPushButton("Ignore and Keep Original Dimension");
    QCheckBox *keep_aspect_ratio_cb = new QCheckBox();

    keep_aspect_ratio_cb->setChecked(m_def_keep_aspect);

    connect(ok_btn, &QPushButton::clicked, this,
            [input_img_height, input_img_width, keep_aspect_ratio_cb,
             ask_widget, this]()
    {
        auto width  = input_img_width->text().toInt();
        auto height = input_img_height->text().toInt();

        if (keep_aspect_ratio_cb->isChecked())
            m_pix = m_pix.scaled(width, height, Qt::KeepAspectRatio);
        else
            m_pix = m_pix.scaled(width, height, Qt::IgnoreAspectRatio);
        gv->setPixmap(m_pix);
        ask_widget->accept();
    });

    connect(keep_original_btn, &QPushButton::clicked, this, [&]()
    {
        gv->setPixmap(m_pix);
        ask_widget->reject();
    });

    input_img_width->setRange(0, 5000);
    input_img_height->setRange(0, 5000);

    if (m_def_width > -1)
        input_img_width->setValue(m_def_width);
    else
        input_img_width->setValue(m_pix.width());

    if (m_def_height > -1)
        input_img_height->setValue(m_def_height);
    else
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

    ask_widget->open();
}

// Function that handles 'play' state
void
Sonify::Play() noexcept
{
    sonification->play();
    /*m_play_btn->setText("Pause");*/
    m_play_btn->setIcon(QIcon(":/icons/pause-button.svg"));
    m_reset_btn->setEnabled(false);
    m_num_samples_spinbox->setEnabled(false);
    m_traverse_combo->setEnabled(false);
}

// Function that handles 'pause' state
void
Sonify::Pause() noexcept
{
    sonification->pause();
    /*m_play_btn->setText("Play");*/
    m_play_btn->setIcon(QIcon(":/icons/play-button.svg"));
    m_reset_btn->setEnabled(true);
    m_num_samples_spinbox->setEnabled(true);
    m_traverse_combo->setEnabled(true);
}

// Read in a WAV file. This was used just to check if the Waveforms
// visualizer worked
QVector<short>
readWAVFile(const QString filename) noexcept
{
    SF_INFO sfinfo;

    SNDFILE *file = sf_open(filename.toStdString().c_str(), SFM_READ, &sfinfo);

    if (!file)
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

void
Sonify::readConfigFile() noexcept
{
    auto toml = toml::parse_file(m_config_file_path.toStdString());

    auto defaults = toml["defaults"];
    auto ui       = toml["ui"];

    int num_samples               = defaults["num_samples"].value_or(256);
    float sample_rate             = defaults["sample_rate"].value_or(44100.0f);
    int min_freq                  = defaults["min_freq"].value_or(0);
    int max_freq                  = defaults["max_freq"].value_or(20000);
    int img_height                = defaults["img_height"].value_or(-1);
    int img_width                 = defaults["img_width"].value_or(-1);
    toml::array *freq_range_array = defaults["freq_range"].as_array();

    std::array<int, 2> freq_range;
    if (!freq_range_array || freq_range_array->size() != 2)
        freq_range = {0, 20000};

    freq_range = {static_cast<int>((*freq_range_array)[0].value_or(0)),
                  static_cast<int>((*freq_range_array)[1].value_or(0))};

    std::string traverse = defaults["traversal"].value_or("left-to-right");
    Traverse traversal;
    if (m_traverse_string_map.contains(traverse))
        traversal = m_traverse_string_map.at(traverse);
    else
        traversal = Traverse::LEFT_TO_RIGHT;
    std::string panel_position = ui["panel_position"].value_or("top");

    m_config = {.num_samples    = num_samples,
                .min_freq       = min_freq,
                .max_freq       = max_freq,
                .img_height     = img_height,
                .img_width      = img_width,
                .freq_range     = freq_range,
                .traversal      = traversal,
                .sample_rate    = sample_rate,
                .panel_position = panel_position};
}
