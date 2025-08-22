#include "sonify.hpp"

#include "ImageEditorDialog.hpp"
#include "freqmap.hpp"
#include "sonifycpp/utils.hpp"
#include "utils_internal.hpp"

#include <QDir>
#include <QProgressDialog>
#include <QStandardPaths>
#include <QtConcurrent/qtconcurrentrun.h>
#include <dlfcn.h>

Sonify::Sonify(QWidget *parent) : QMainWindow(parent)
{
    initWidgets();
    initStatusbar();
    initMenu();
    initConnections();
    initIcons();

    this->show();
    initConfigFile();
    loadMappingSharedObjects();
}

// Set the keybindings
void
Sonify::initKeybinds() noexcept
{
    QShortcut *kb_menubar = new QShortcut(QKeySequence("Ctrl+Shift+M"), this);
    QShortcut *kb_open    = new QShortcut(QKeySequence("Ctrl+O"), this);

    connect(kb_menubar, &QShortcut::activated,
            [&]() { m_menu_bar->setVisible(!m_menu_bar->isVisible()); });

    connect(kb_open, &QShortcut::activated, [&]() { Open(); });
}

// Read the configuration file, if it exists
void
Sonify::initConfigFile() noexcept
{
    m_config_dir =
        QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
        QDir::separator() + "sonifycpp";

    QDir dir = QDir(m_config_dir);
    if (!dir.exists()) dir.mkdir(dir.path());

    m_mappings_dir =
        QDir::cleanPath(m_config_dir + QDir::separator() + "mappings");

    m_config_file_path =
        QDir::cleanPath(m_config_dir + QDir::separator() + "config.toml");

    // Read from configuration toml file
    if (QFile(m_config_file_path).exists())
        readConfigFile();
    else
    {
        // Initialize defaults
        m_config = { .num_samples    = 512,
                     .min_freq       = 0,
                     .max_freq       = 20000,
                     .img_height     = 100,
                     .img_width      = 100,
                     .freq_range     = { 0, 20000 },
                     .traversal      = Traverse::LEFT_TO_RIGHT,
                     .sample_rate    = 44100.0f,
                     .panel_position = "top" };
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
    layout->addWidget(new QLabel("Traverse"), 2, 0);
    layout->addWidget(m_traverse_combo, 2, 1);
    layout->addWidget(new QLabel("Samples"), 3, 0);
    layout->addWidget(m_num_samples_spinbox, 3, 1);
    layout->addWidget(new QLabel("Min Freq"), 4, 0);
    layout->addWidget(m_min_freq_sb, 4, 1);
    layout->addWidget(new QLabel("Max Freq"), 5, 0);
    layout->addWidget(m_max_freq_sb, 5, 1);
    layout->addWidget(new QLabel("Mapping"), 6, 0);
    layout->addWidget(m_freq_mapping_combo, 6, 1);
    layout->addWidget(new QLabel("Pixel Mapping"), 7, 0);
    layout->addWidget(m_pixel_mapping_combo, 7, 1);

    m_splitter->addWidget(m_panel);
    m_splitter->addWidget(m_gv);
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
    layout->addWidget(new QLabel("Traverse"), 2, 0);
    layout->addWidget(m_traverse_combo, 2, 1);
    layout->addWidget(new QLabel("Samples"), 3, 0);
    layout->addWidget(m_num_samples_spinbox, 3, 1);
    layout->addWidget(new QLabel("Min Freq"), 4, 0);
    layout->addWidget(m_min_freq_sb, 4, 1);
    layout->addWidget(new QLabel("Max Freq"), 5, 0);
    layout->addWidget(m_max_freq_sb, 5, 1);
    layout->addWidget(new QLabel("Mapping"), 6, 0);
    layout->addWidget(m_freq_mapping_combo, 6, 1);

    m_splitter->addWidget(m_gv);
    m_splitter->addWidget(m_panel);
    m_layout->addWidget(m_splitter);
    m_layout->addWidget(m_status_bar);
    layout->setRowStretch(7, 1);
}

void
Sonify::initIcons() noexcept
{
    m_sonify_btn->setText("Sonify");
    m_play_btn->setText("Play");
    m_reset_btn->setText("Reset");
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
    layout->addWidget(new QLabel("Traverse"));
    layout->addWidget(m_traverse_combo);
    layout->addWidget(new QLabel("Samples"));
    layout->addWidget(m_num_samples_spinbox);
    layout->addWidget(new QLabel("Min Freq"));
    layout->addWidget(m_min_freq_sb);
    layout->addWidget(new QLabel("Max Freq"));
    layout->addWidget(m_max_freq_sb);
    layout->addWidget(new QLabel("Mapping"));
    layout->addWidget(m_freq_mapping_combo);
    layout->addWidget(new QLabel("Pixel Mapping"));
    layout->addWidget(m_pixel_mapping_combo);
    layout->addStretch(1);

    m_panel->setFixedHeight(40);
    m_layout->addWidget(m_panel);
    m_layout->addWidget(m_splitter);
    m_layout->addWidget(m_status_bar);
    m_splitter->addWidget(m_gv);
    m_layout->setStretchFactor(m_gv, 1);
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
    layout->addWidget(new QLabel("Traverse"));
    layout->addWidget(m_traverse_combo);
    layout->addWidget(new QLabel("Samples"));
    layout->addWidget(m_num_samples_spinbox);
    layout->addWidget(new QLabel("Min Freq"));
    layout->addWidget(m_min_freq_sb);
    layout->addWidget(new QLabel("Max Freq"));
    layout->addWidget(m_max_freq_sb);
    layout->addWidget(new QLabel("Mapping"));
    layout->addWidget(m_freq_mapping_combo);
    layout->addWidget(new QLabel("Pixel Mapping"));
    layout->addWidget(m_pixel_mapping_combo);

    layout->setSpacing(20);
    layout->addStretch(1);
    m_panel->setFixedHeight(40);
    m_layout->addWidget(m_splitter);
    m_layout->addWidget(m_panel);
    m_layout->addWidget(m_status_bar);
    m_splitter->addWidget(m_gv);
    m_layout->setStretchFactor(m_panel, 1);
}

void
Sonify::initStatusbar() noexcept
{
}

void
Sonify::initWidgets() noexcept
{
    m_sonify_btn = new QPushButton("Sonify");
    m_play_btn   = new QPushButton("Play");
    m_reset_btn  = new QPushButton("Reset");
    m_num_samples_spinbox->setMinimum(1);
    m_num_samples_spinbox->setMaximum(4000);
    m_num_samples_spinbox->setValue(m_config.num_samples);

    m_freq_mapping_combo->addItem("Linear");
    m_freq_mapping_combo->addItem("Logarithmic");
    m_freq_mapping_combo->addItem("Exponential");

    m_sonify_btn->setToolTip("Sonify");
    m_reset_btn->setToolTip("Reset");
    m_play_btn->setToolTip("Play");

    int r1 = m_config.freq_range.at(0);
    int r2 = m_config.freq_range.at(1);
    m_min_freq_sb->setRange(r1, r2);
    m_max_freq_sb->setRange(r1, r2);

    m_min_freq_sb->setValue(m_config.min_freq);
    m_max_freq_sb->setValue(m_config.max_freq);

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

    m_gv->setObjColor(QString::fromStdString(m_config.pointer_color));
    m_gv->setAlignment(Qt::AlignmentFlag::AlignCenter);
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

    m_widget->setMinimumSize(800, 600);
}

void
Sonify::initMenu() noexcept
{
    m_file_menu     = new QMenu("File");
    m_audio_menu    = new QMenu("Audio");
    m_tools_menu    = new QMenu("Tools");
    m_view_menu     = new QMenu("View");
    m_mappings_menu = new QMenu("Mappings");
    m_help_menu     = new QMenu("Help");

    m_menu_bar->addMenu(m_file_menu);
    m_menu_bar->addMenu(m_tools_menu);
    m_menu_bar->addMenu(m_view_menu);
    m_menu_bar->addMenu(m_audio_menu);
    m_menu_bar->addMenu(m_mappings_menu);
    m_menu_bar->addMenu(m_help_menu);

    m_mappings__reload     = new QAction("Reload Custom Mappings");
    m_mappings__list_pmaps = new QAction("List Pixel Mappings");

    m_mappings_menu->addActions({ m_mappings__reload, m_mappings__list_pmaps });

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
    connect(m_sonification, &Sonification::audioIndex, m_gv,
            &GV::setAudioIndex);

    connect(m_sonification, &Sonification::audioFinishedPlaying, this,
            &Sonify::audioPlaybackDone);

    connect(m_sonification, &Sonification::audioProgress, m_gv,
            [this](double location)
    {
        m_status_bar->setAudioProgressText(QString::number(
            location / static_cast<double>(m_sonification->sampleRate())));
        if (m_wf_widget)
            m_wf_widget->setVertLinePosition(
                location / static_cast<double>(m_sonification->sampleRate()));
    });

    connect(m_status_bar, &Statusbar::stopSonificationRequested, this,
            &Sonify::sonificationStopped);

    connect(m_gv, &GV::animationFinished, this, [this]()
    {
        m_play_btn->setText("Play");
        m_isAudioPlaying = false;
        m_sonification->reset();
        m_traverse_combo->setEnabled(true);
    });

    connect(m_tools__waveform, &QAction::triggered, this,
            &Sonify::viewWaveform);

    connect(m_tools__image_settings, &QAction::triggered, this, [this]()
    {
        ImageEditorDialog *e = new ImageEditorDialog(this);
        QPixmap pix          = m_gv->getPixmap();
        e->setPixmap(pix);
        e->setAttribute(Qt::WA_DeleteOnClose);
        connect(e, &ImageEditorDialog::optionsApplied, this,
                &Sonify::applyImageEdits);
        connect(e, &ImageEditorDialog::resetImage, this,
                [this]() { m_gv->setPixmap(m_pix); });
        e->open();
    });

    connect(m_mappings__reload, &QAction::triggered, this,
            &Sonify::loadMappingSharedObjects);

    connect(m_sonification, &Sonification::sonificationDone, this,
            &Sonify::sonificationDone);

    connect(m_sonification, &Sonification::sonificationProgress, this,
            [this](int progress)
    { m_status_bar->setSonificationProgress(progress); });

    connect(m_gv, &GV::dropFile, this,
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
            std::vector<short> data = m_sonification->audioData();
            float sr                = m_sonification->sampleRate();
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
            if (!m_recorder)
            {
                m_recorder = new ScreenRecorder();
                m_recorder->setGraphicsView(m_gv);
            }

            connect(m_recorder, &ScreenRecorder::finished, this,
                    []() { qDebug() << "Screen recording finished"; });
            m_recorder->Start();
        }
        else
        {
            if (m_recorder) m_recorder->Stop();
        }
    });

    connect(m_help__about, &QAction::triggered, this, [this]()
    {
        AboutDialog *aboutDialog = new AboutDialog(this);
        aboutDialog->open();
    });

    connect(m_effects__reverb, &QAction::triggered, this, [this]()
    {
        ReverbDialog *reverbDialog = new ReverbDialog(this);
        reverbDialog->setData(m_sonification->audioData(),
                              m_sonification->sampleRate());
        connect(reverbDialog, &ReverbDialog::outputReady, this,
                [&](std::vector<short> reverbedOutput)
        { m_sonification->setAudioData(reverbedOutput); });
        reverbDialog->open();
    });

    connect(m_view__menubar, &QAction::triggered, this,
            [this](bool state) { m_menu_bar->setVisible(state); });

    connect(m_view__statusbar, &QAction::triggered, this,
            [this](bool state) { m_status_bar->setVisible(state); });

    connect(m_view__panel, &QAction::triggered, this,
            [this](bool state) { m_panel->setVisible(!state); });
}

// This is used to set the state of the audio playback. Call
// Play() if we have to play the audio or call Pause() if we
// want to pause.
void
Sonify::PlayAudio() noexcept
{
    m_isAudioPlaying = !m_isAudioPlaying;
    if (m_isAudioPlaying)
        Play();
    else
        Pause();
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
        std::vector<short> data = m_sonification->audioData();
        m_wf_widget->setData(data, m_sonification->sampleRate());
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

        if (filename.isEmpty()) return false;

        return m_sonification->save(filename);
    }

    return false;
}

// Function that handles Opening an image. If filename is
// specified, image will be opened, otherwise a file dialog
// will ask for the file.
void
Sonify::Open(QString filename) noexcept
{
    if (filename.isEmpty())
    {
        filename = QFileDialog::getOpenFileName(
            this, "Open Image", nullptr, "Image Files (*.png *.jpeg *.jpg)");
        if (filename.isEmpty()) return;
    }

    if (filename.startsWith("~/"))
    {
        const QString home = QDir::homePath();
        filename           = home + filename.mid(1);
    }

    if (m_def_ask_for_resize)
    {
        if (!AskForResize(filename)) return;
    }
    else
    {
        QPixmap pix(filename);

        if (pix.isNull()) return;

        if (m_def_keep_aspect)
            pix = pix.scaled(m_def_width, m_def_height, Qt::KeepAspectRatio);
        else
            pix = pix.scaled(m_def_width, m_def_height, Qt::IgnoreAspectRatio);

        m_pix = pix;
    }

    m_gv->setPixmap(m_pix);
    m_sonify_btn->setEnabled(true);
    m_file__close->setEnabled(true);
    m_play_btn->setEnabled(false);
    m_reset_btn->setEnabled(false);
    emit fileOpened();
}

// Non-interactive version of the Open function
void
Sonify::open(QString filename, int w, int h, bool keepAspectRatio) noexcept
{
    if (filename.startsWith("~/"))
    {
        const QString home = QDir::homePath();
        filename           = home + filename.mid(1);
    }

    QPixmap pix(filename);

    if (pix.isNull()) return;

    if (keepAspectRatio)
        pix = pix.scaled(w, h, Qt::KeepAspectRatio);
    else
        pix = pix.scaled(w, h, Qt::IgnoreAspectRatio);

    m_pix = pix;

    m_gv->setPixmap(m_pix);
    m_sonify_btn->setEnabled(true);
    m_file__close->setEnabled(true);
    m_play_btn->setEnabled(false);
    m_reset_btn->setEnabled(false);
    emit fileOpened();
}

// Non-interactive version of the Open function. Opens the image as is
// with the default size and aspect ratio kept.
void
Sonify::open(QString filename) noexcept
{
    if (filename.startsWith("~/"))
    {
        const QString home = QDir::homePath();
        filename           = home + filename.mid(1);
    }

    QPixmap pix(filename);

    if (pix.isNull()) return;

    m_pix = pix;
    m_gv->setPixmap(m_pix);
    m_sonify_btn->setEnabled(true);
    m_file__close->setEnabled(true);
    m_play_btn->setEnabled(false);
    m_reset_btn->setEnabled(false);
    emit fileOpened();
}

void
Sonify::Close() noexcept
{
    m_gv->scene()->clear();
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
    m_sonification->setStopSonification(false);

    MapTemplate *t =
        m_sonification->pixelMappingClass(m_pixel_mapping_combo->currentText());

    if (!t)
    {
        QMessageBox::critical(
            this, "Error Sonifying",
            "Could not sonify because the mapping template is null!");
        return;
    }

    Sonifier::MapFunc mapFunc;
    const int min_freq = m_min_freq_sb->text().toInt();
    const int max_freq = m_max_freq_sb->text().toInt();
    m_sonification->setNumSamples(m_num_samples_spinbox->value());

    switch (m_freq_mapping_combo->currentIndex())
    {
        case 0: t->setFreqMap(utils::LinearMap); break;
        case 1: t->setFreqMap(utils::LogMap); break;
        case 2: t->setFreqMap(utils::ExpMap); break;
    }

    t->setMaxFreq(max_freq);
    t->setMinFreq(min_freq);

    mapFunc = [t](const std::vector<Pixel> &cols) { return t->mapping(cols); };

    m_status_bar->sonificationStart();

    const QString traverseComboText = m_traverse_combo->currentText();

    if (traverseComboText ==
        m_traversal_name_list[static_cast<int>(Traverse::LEFT_TO_RIGHT)])
    {
        m_mode = Traverse::LEFT_TO_RIGHT;
        m_status_bar->setTraversalModeText("Left to Right");
    }

    else if (traverseComboText ==
             m_traversal_name_list[static_cast<int>(Traverse::RIGHT_TO_LEFT)])
    {
        m_mode = Traverse::RIGHT_TO_LEFT;
        m_status_bar->setTraversalModeText("Right to Left");
    }

    else if (traverseComboText ==
             m_traversal_name_list[static_cast<int>(Traverse::TOP_TO_BOTTOM)])
    {
        m_mode = Traverse::TOP_TO_BOTTOM;
        m_status_bar->setTraversalModeText("Top to Bottom");
    }

    else if (traverseComboText ==
             m_traversal_name_list[static_cast<int>(Traverse::BOTTOM_TO_TOP)])
    {
        m_mode = Traverse::BOTTOM_TO_TOP;
        m_status_bar->setTraversalModeText("Bottom to Top");
    }

    else if (traverseComboText ==
             m_traversal_name_list[static_cast<int>(Traverse::CIRCLE_OUTWARDS)])
    {
        m_mode = Traverse::CIRCLE_OUTWARDS;
        m_status_bar->setTraversalModeText("Circle Outwards");
    }

    else if (traverseComboText ==
             m_traversal_name_list[static_cast<int>(Traverse::CIRCLE_INWARDS)])
    {
        m_mode = Traverse::CIRCLE_INWARDS;
        m_status_bar->setTraversalModeText("Circle Inwards");
    }

    else if (traverseComboText ==
             m_traversal_name_list[static_cast<int>(Traverse::CLOCKWISE)])
    {
        m_mode = Traverse::CLOCKWISE;
        m_status_bar->setTraversalModeText("Clockwise");
    }

    else if (traverseComboText ==
             m_traversal_name_list[static_cast<int>(Traverse::ANTICLOCKWISE)])
    {
        m_mode = Traverse::ANTICLOCKWISE;
        m_status_bar->setTraversalModeText("Anti-clockwise");
    }

    else if (traverseComboText ==
             m_traversal_name_list[static_cast<int>(Traverse::PATH)])
    {
        m_gv->clearDrawPath();
        m_gv->setDrawPathMode(true);
        connect(m_gv, &GV::drawPathFinished, this, [&]()
        {
            m_mode = Traverse::PATH;
            m_status_bar->setTraversalModeText("Path");
        });
    }

    else if (traverseComboText ==
             m_traversal_name_list[static_cast<int>(Traverse::INSPECT)])
    {
        /*gv->setPixelAnalyserMode(true);*/
        m_mode = Traverse::INSPECT;
        m_status_bar->setTraversalModeText("Inspect");
    }

    m_sonification->Sonify(m_pix, m_gv, mapFunc, m_mode, min_freq, max_freq);
    m_panel->setEnabled(false);
}

// Reset btn function
void
Sonify::Reset() noexcept
{
    /*m_play_btn->setText("Play");*/
    m_isAudioPlaying = false;

    m_gv->reset();
    if (m_wf_widget) m_wf_widget->resetVertLine();
    m_sonification->reset();
    m_status_bar->setAudioProgressText("");
}

void
Sonify::CaptureWindow() noexcept
{
    // TODO: Screen record
}

bool
Sonify::AskForResize(const QString &filename) noexcept
{
    // Load original pixmap early so we can restore if
    // needed
    QPixmap original_pix = QPixmap(filename);
    if (original_pix.isNull())
    {
        // optionally: show error message about failing to
        // load
        return false;
    }

    // Dialog setup
    QDialog dialog(this);
    dialog.setWindowTitle("Resize Input Image");
    dialog.setModal(true);
    QGridLayout *layout = new QGridLayout(&dialog);

    // Message
    QString msgtext = QString("The input image (%1) has dimensions "
                              "(%2, %3). Images with "
                              "large "
                              "dimensions tend to be slower during "
                              "sonification and can "
                              "increase wait time.")
                          .arg(filename)
                          .arg(original_pix.width())
                          .arg(original_pix.height());
    QLabel *msg = new QLabel(msgtext);
    msg->setWordWrap(true);
    layout->addWidget(msg, 0, 0, 1, 2);

    // Width/Height controls
    QLabel *width_label   = new QLabel("Width:");
    QLabel *height_label  = new QLabel("Height:");
    QSpinBox *width_spin  = new QSpinBox();
    QSpinBox *height_spin = new QSpinBox();
    width_spin->setRange(1, 5000);
    height_spin->setRange(1, 5000);
    width_spin->setToolTip("Target width for the resized image");
    height_spin->setToolTip("Target height for the resized image");

    // Initial values from defaults or original
    if (m_def_width > 0)
        width_spin->setValue(m_def_width);
    else
        width_spin->setValue(original_pix.width());

    if (m_def_height > 0)
        height_spin->setValue(m_def_height);
    else
        height_spin->setValue(original_pix.height());

    layout->addWidget(width_label, 1, 0);
    layout->addWidget(width_spin, 1, 1);
    layout->addWidget(height_label, 2, 0);
    layout->addWidget(height_spin, 2, 1);

    // Aspect ratio
    QCheckBox *keep_aspect = new QCheckBox("Keep aspect ratio");
    keep_aspect->setChecked(m_def_keep_aspect);
    keep_aspect->setToolTip("Maintain original aspect ratio when resizing");
    layout->addWidget(keep_aspect, 3, 0, 1, 2);

    // Synchronize width/height when aspect ratio is on
    const double original_ratio =
        original_pix.width() > 0
            ? static_cast<double>(original_pix.height()) / original_pix.width()
            : 1.0;
    auto sync_height = [&](int w)
    {
        if (keep_aspect->isChecked())
        {
            int new_h = static_cast<int>(std::round(w * original_ratio));
            height_spin->blockSignals(true);
            height_spin->setValue(new_h);
            height_spin->blockSignals(false);
        }
    };
    auto sync_width = [&](int h)
    {
        if (keep_aspect->isChecked() && original_ratio != 0.0)
        {
            int new_w = static_cast<int>(std::round(h / original_ratio));
            width_spin->blockSignals(true);
            width_spin->setValue(new_w);
            width_spin->blockSignals(false);
        }
    };

    QObject::connect(width_spin, &QSpinBox::valueChanged, &dialog,
                     [&](int v) { sync_height(v); });
    QObject::connect(height_spin, &QSpinBox::valueChanged, &dialog,
                     [&](int v) { sync_width(v); });

    // Buttons: OK / Ignore original / Cancel
    QDialogButtonBox *button_box =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QPushButton *ignore_btn =
        new QPushButton("Ignore and Keep Original Dimension");
    ignore_btn->setToolTip("Skip resizing and use the original image as-is");
    layout->addWidget(ignore_btn, 4, 0);
    layout->addWidget(button_box, 4, 1);

    // Keep aspect ratio toggle should re-sync if toggled
    QObject::connect(keep_aspect, &QCheckBox::toggled, &dialog,
                     [&](bool checked)
    {
        if (checked)
        {
            // enforce ratio based on current width
            sync_height(width_spin->value());
        }
    });

    // OK button enable/disable (prevent zero dim, though
    // ranges start at 1)
    QPushButton *ok_button = button_box->button(QDialogButtonBox::Ok);
    ok_button->setEnabled(true); // already safe due to spinbox ranges

    // Signal handling
    QObject::connect(ignore_btn, &QPushButton::clicked, &dialog, [&]()
    {
        m_pix = original_pix;
        if (m_gv) m_gv->setPixmap(m_pix);
        dialog.accept();
    });

    QObject::connect(button_box, &QDialogButtonBox::accepted, &dialog, [&]()
    {
        int w = width_spin->value();
        int h = height_spin->value();
        if (keep_aspect->isChecked())
            m_pix = original_pix.scaled(w, h, Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);
        else
            m_pix = original_pix.scaled(w, h, Qt::IgnoreAspectRatio,
                                        Qt::SmoothTransformation);

        if (m_gv) m_gv->setPixmap(m_pix);

        // Persist defaults for next time
        m_def_width       = w;
        m_def_height      = h;
        m_def_keep_aspect = keep_aspect->isChecked();

        dialog.accept();
    });
    QObject::connect(button_box, &QDialogButtonBox::rejected, &dialog,
                     [&]() { dialog.reject(); });

    // Execute modally
    auto dialogResult = dialog.exec();

    if (dialogResult == QDialog::Rejected) return false;

    return true;
}

// Function that handles 'play' state
void
Sonify::Play() noexcept
{
    m_sonification->play();
    m_play_btn->setText("Pause");
    enablePanelUIs(false);
    m_play_btn->setEnabled(true);
}

// Function that handles 'pause' state
void
Sonify::Pause() noexcept
{
    m_sonification->pause();
    m_play_btn->setText("Play");

    enablePanelUIs(true);
}

void
Sonify::readConfigFile() noexcept
{
}

void
Sonify::args(const argparse::ArgumentParser &args) noexcept
{
    if (auto filename = args.present("-i"))
    {
        if (filename) { Open(QString::fromStdString(*filename)); }
    }
}

// Function called when the sonification is done
void
Sonify::sonificationDone() noexcept
{
    m_panel->setEnabled(true);
    m_audio__save->setEnabled(true);
    m_tools__waveform->setEnabled(true);
    m_tools__spectrum_analyzer->setEnabled(true);
    m_status_bar->sonificationDone();
    m_gv->setDuration(m_sonification->duration());
    m_status_bar->setAudioDurationText(
        "Duration: " + QString::number(m_sonification->duration()) + " secs");

    m_gv->setTraverse(m_mode);
    m_play_btn->setEnabled(true);
    m_reset_btn->setEnabled(true);
}

void
Sonify::sonificationStopped() noexcept
{
    m_sonification->setStopSonification(true);
    m_status_bar->sonificationDone();
    m_status_bar->setMsg("Sonification Stopped", 5);
    m_panel->setEnabled(true);
}

void
Sonify::audioPlaybackDone() noexcept
{
    emit m_gv->animationFinished();
    enablePanelUIs(true);
}

void
Sonify::applyImageEdits(const ImageEditorDialog::ImageOptions &options) noexcept
{
    QImage img = m_pix.toImage();

    img = utils_internal::changeBrightness(img, options.Brightness);
    img = utils_internal::changeSaturation(img, options.Saturation);
    img = utils_internal::changeContrast(img, options.Contrast);
    img = utils_internal::changeGamma(img, options.Gamma);

    QTransform t;
    img = img.transformed(t.rotate(options.rotate));

    if (options.Grayscale) img = utils_internal::convertToGrayscale(img);

    if (options.Invert) img = utils_internal::invertColor(img);

    m_pix = QPixmap::fromImage(img);
    m_gv->setPixmap(m_pix);
}

void
Sonify::enablePanelUIs(bool state) noexcept
{
    m_num_samples_spinbox->setEnabled(state);
    m_sonify_btn->setEnabled(state);
    m_play_btn->setEnabled(state);
    m_reset_btn->setEnabled(state);
    m_max_freq_sb->setEnabled(state);
    m_min_freq_sb->setEnabled(state);
    m_freq_mapping_combo->setEnabled(state);
    m_pixel_mapping_combo->setEnabled(state);
    m_traverse_combo->setEnabled(state);
}

// Loads all the shared objects from the `mappings` directory
void
Sonify::loadMappingSharedObjects() noexcept
{
    // remove existing user defined mapping names from QComboBox
    QStringList customMappings = m_sonification->getPixelMappingNames();

    for (const auto &m : customMappings)
    {
        int index = m_pixel_mapping_combo->findText(m);
        if (index != -1) { m_pixel_mapping_combo->removeItem(index); }
    }

    m_sonification->clearCustomPixelMappings();

    initDefaultPixelMappings();

    const QDir dir(m_mappings_dir);
    if (!dir.exists()) { return; }
    const QStringList &sofiles = dir.entryList({ "*.so" }, QDir::Files);

    if (sofiles.empty()) return;

    const int numFiles = sofiles.size();
    QProgressDialog progress("Loading Mapping Shared Objects...", "Cancel", 0,
                             numFiles, this);
    progress.setMinimumDuration(0);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    for (int i = 0; i < numFiles; i++)
    {
        progress.setValue(i);
        if (progress.wasCanceled()) break;
        QCoreApplication::processEvents();
        loadSharedObject(dir.filePath(sofiles.at(i)));
    }

    progress.setValue(numFiles);
}

bool
Sonify::loadSharedObject(const QString &filename) noexcept
{
    void *handle = dlopen(filename.toStdString().c_str(), RTLD_LAZY);

    if (!handle)
    {
        qWarning() << dlerror() << "\n";
        return false;
    }

    auto create = reinterpret_cast<MapTemplate *(*)()>(dlsym(handle, "create"));
    if (!create)
    {
        qWarning() << dlerror() << "\n";
        dlclose(handle);
        return false;
    }

    MapTemplate *ptr = create();

    if (!ptr) return false;

    const std::string name(ptr->name());
    if (name == "Intensity" || name == "HSV")
    {
        QMessageBox::warning(
            this, "Mapping Name Conflict",
            "The name of the map seems to be conflicting with the built-in "
            "mapping name. This results in shadowing of the built-in mappings "
            "`Intensity` and/or `HSV`. Please rename the mappings to something "
            "different so as to not have this problem");
    }

    m_pixel_mapping_combo->addItem(ptr->name());

    m_sonification->addCustomPixelMapping({ handle, ptr });
    return true;
}

// Initialize default pixel mappings
void
Sonify::initDefaultPixelMappings() noexcept
{
    HSVMap *map1       = new HSVMap;
    IntensityMap *map2 = new IntensityMap;

    MapTemplate *maps[] = { map1, map2 };

    for (auto *m : maps)
    {
        m_sonification->addCustomPixelMapping({ nullptr, m });
        m_pixel_mapping_combo->addItem(m->name());
    }
}
