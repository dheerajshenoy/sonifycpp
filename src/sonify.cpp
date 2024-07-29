#include "sonify.hpp"

// Read in a WAV file. This was used just to check if the Waveform visualizer worked
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
    initConfigDir();
    initWidgets();
    initStatusbar();
    initSidePanel();
    initMenu();
    initConnections();
    this->show();
    m_recorder->setGraphicsView(gv);

    initLuaBindings();

}

int MyFunc(lua_State *s)
{
    int a = lua_tointeger(s, 1);
    int b = lua_tointeger(s, 2);

    int result = a + b;

    lua_pushinteger(s, result);

    return 1;
}

void Sonify::initConfigDir()
{
    m_config_dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir = QDir(m_config_dir);
    if (!dir.exists())
        dir.mkdir(dir.path());

    m_script_file_path = QDir::cleanPath(m_config_dir + QDir::separator() + "script.lua");
}

// Handle lua script file content
void Sonify::initLuaBindings()
{

    QFile file = QFile(m_script_file_path);
    if(!file.exists()) return; // If no script file, don't load lua

    luaopen_base(m_lua_state);

    if (checkLua(m_lua_state, luaL_dofile(m_lua_state, m_script_file_path.toStdString().c_str())).status)
    {
        // Read defaults

        lua_getglobal(m_lua_state, "Defaults");

        if (lua_istable(m_lua_state, -1))
        {
            lua_pushstring(m_lua_state, "object_color");
            lua_gettable(m_lua_state, -2);
            if (lua_isstring(m_lua_state, -1))
            {
                gv->setObjColor(lua_tostring(m_lua_state, -1));
            }
            lua_pop(m_lua_state, 1);

            // Default sample rate

            lua_pushstring(m_lua_state, "samples");
            lua_gettable(m_lua_state, -2);
            if (lua_isnumber(m_lua_state, -1))
            {
                m_num_samples_spinbox->setValue(static_cast<int>(lua_tonumber(m_lua_state, -1)));
            }
            lua_pop(m_lua_state, 1);

            lua_pushstring(m_lua_state, "traversal");
            lua_gettable(m_lua_state, -2);
            if (lua_isnumber(m_lua_state, -1))
            {
                int index = static_cast<int>(lua_tonumber(m_lua_state, -1));
                if (index <= m_traverse_combo->count() && index > 0)
                    m_traverse_combo->setCurrentIndex(index - 1);
            }
            lua_pop(m_lua_state, 1);

            // get default resolution if any
            lua_pushstring(m_lua_state, "resolution");
            lua_gettable(m_lua_state, -2);

            if (lua_istable(m_lua_state, -1))
            {

                lua_pushstring(m_lua_state, "height");
                lua_gettable(m_lua_state, -2);

                if (lua_isnumber(m_lua_state, -1))
                    m_def_height = lua_tonumber(m_lua_state, -1);

                lua_pop(m_lua_state, 1);

                lua_pushstring(m_lua_state, "width");
                lua_gettable(m_lua_state, -2);

                if (lua_isnumber(m_lua_state, -1))
                    m_def_width = lua_tonumber(m_lua_state, -1);

                lua_pop(m_lua_state, 1);

                lua_pushstring(m_lua_state, "keep_aspect");
                lua_gettable(m_lua_state, -2);

                if (lua_isboolean(m_lua_state, -1))
                    m_def_keep_aspect = lua_toboolean(m_lua_state, -1);

                lua_pop(m_lua_state, 1);

                lua_pushstring(m_lua_state, "ask_for_resize");
                lua_gettable(m_lua_state, -2);

                if (lua_isboolean(m_lua_state, -1))
                    m_def_ask_for_resize = lua_toboolean(m_lua_state, -1);

            }

            lua_pop(m_lua_state, 1);
        }

        lua_pop(m_lua_state, 1);

        // Get all the mapping functions defined (if any)
        lua_getglobal(m_lua_state, "Maps");

        if (lua_istable(m_lua_state, -1))
        {
            // Iterate through the maps table
            lua_pushnil(m_lua_state);  // First key
            while (lua_next(m_lua_state, -2) != 0)
            {
                // Now 'key' is at index -2 and 'value' at index -1
                // Check if value is a table
                if (lua_istable(m_lua_state, -1))
                {

                    // Get the 'name' field from the table
                    lua_getfield(m_lua_state, -1, "name");
                    const char* name = lua_tostring(m_lua_state, -1);
                    qDebug() << "Name: " << name;
                    lua_pop(m_lua_state, 1);  // Remove 'name' value

                    // Get the 'func' field from the table
                    lua_getfield(m_lua_state, -1, "func");
                    if (lua_isfunction(m_lua_state, -1))
                    {
                        if (lua_pcall(m_lua_state, 0, 0, 0) != LUA_OK)
                        {
                            qDebug() << "CANNOT CALL FUNCTION" << lua_tostring(m_lua_state, -1);
                            lua_pop(m_lua_state, 1);
                        }

                        else {
                            lua_pop(m_lua_state, 1);
                        }
                    }
                }
                /*lua_pop(m_lua_state, 1);  // Remove value, keep key for next iteration*/
            }
        }
    }

}

LuaError Sonify::checkLua(lua_State *s, int r)
{
    LuaError l;
    if (r != LUA_OK)
    {
        l.status = false;
        l.errmsg = lua_tostring(s, -1);
    }

    return l;
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
    m_splitter = new QSplitter();
    m_progress_bar = new QProgressBar();
    m_stop_sonification_btn = new QPushButton("Stop");

    m_status_bar = new QWidget();
    m_status_bar->setMaximumHeight(30);
    m_status_bar_layout = new QHBoxLayout();
    m_statusbar_msg_label = new QLabel();

    m_traversal_name_list = {
        "Left to Right",
        "Right to Left",
        "Top to Bottom",
        "Bottom to Top",
        "Circle Outwards",
        "Circle Inwards",
        "Clockwise",
        "Anti-Clockwise",
        "Draw Path",
        "Select Region",
    };

    for(const QString &t : m_traversal_name_list)
    {
        m_traverse_combo->addItem(t);
    }

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

void Sonify::initMenu()
{
    m_menu_bar = new QMenuBar();

    m_file_menu = new QMenu("File");
    m_audio_menu = new QMenu("Audio");
    m_tools_menu = new QMenu("Tools");
    m_view_menu = new QMenu("View");
    m_help_menu = new QMenu("Help");

    m_menu_bar->addMenu(m_file_menu);
    m_menu_bar->addMenu(m_tools_menu);
    m_menu_bar->addMenu(m_view_menu);
    m_menu_bar->addMenu(m_audio_menu);
    m_menu_bar->addMenu(m_help_menu);

    m_file__open = new QAction("Open");
    m_file__exit = new QAction("Exit");
    m_audio__save = new QAction("Save");

    m_audio__save->setEnabled(false);

    m_file_menu->addAction(m_file__open);
    m_file_menu->addAction(m_file__exit);

    m_tools__spectrum_analyzer = new QAction("Spectrum Analyzer");
    m_tools__spectrum_analyzer->setCheckable(true);
    m_tools__spectrum_analyzer->setEnabled(false);

    m_tools__tone_generator = new QAction("Tone Generator");
    m_tools__tone_generator->setCheckable(true);
    m_tools__screen_record = new QAction("Screen Record");
    m_tools__screen_record->setCheckable(true);
    m_tools__waveform = new QAction("Waveform");
    m_tools__waveform->setCheckable(true);
    m_tools__waveform->setEnabled(false);

    m_tools_menu->addAction(m_tools__tone_generator);
    m_tools_menu->addAction(m_tools__spectrum_analyzer);
    m_tools_menu->addAction(m_tools__screen_record);
    m_tools_menu->addAction(m_tools__waveform);

    m_audio_menu->addAction(m_audio__save);

    m_help__about = new QAction("About");
    m_help_menu->addAction(m_help__about);

    this->setMenuBar(m_menu_bar);
}

// This is used to show statusbar message for specified seconds `s` or indefinetely if set equal to -1
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
        /*m_wf_widget->setVertLinePosition(location);*/
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

    connect(m_tools__waveform, &QAction::triggered, this, &Sonify::viewWaveform);

    connect(sonification, &Sonification::sonificationStopped, this, [&]() {
        m_sonify_btn->setEnabled(true);
    });

    connect(sonification, &Sonification::sonificationDone, this, [&]() {
        m_audio__save->setEnabled(true);
        m_tools__waveform->setEnabled(true);
        m_tools__spectrum_analyzer->setEnabled(true);
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

    connect(gv, &GV::dropFile, this, [&](QString droppedFilePath) {
        Open(droppedFilePath);
    });

    connect(m_tools__spectrum_analyzer, &QAction::triggered, this, [&](bool state) {
        if (state)
        {
            m_sp = new SpectrumAnalyzer(this);
            connect(m_sp, &SpectrumAnalyzer::closed, this, [&]() {
                m_sp->close();
                delete m_sp;
                m_sp = nullptr;
                m_tools__spectrum_analyzer->setChecked(false);
            });
            auto data = sonification->getAudioData();
            auto sr = sonification->getSampleRate();
            m_sp->setData(data, sr);
            m_sp->exec();
        }
    });

    connect(m_tools__tone_generator, &QAction::triggered, this, [&](bool state) {
        if (state)
        {
            if (!m_tg)
            {
                m_tg = new ToneGenerator(this);
                connect(m_tg, &ToneGenerator::closed, this, [&]() {
                    m_tools__tone_generator->setChecked(false);
                    m_tg->close();
                    disconnect(m_tg, 0, 0, 0);
                    delete m_tg;
                    m_tg = nullptr;
                });
                m_tg->exec();
            }
        }
    });

    connect(m_tools__screen_record, &QAction::triggered, this, [&](bool state)
    {
        if (state)
        {
            connect(m_recorder, &ScreenRecorder::finished, this, [&]() {
            qDebug() << "Screen recording finished";
            });
            m_recorder->Start();
        }
        else
            m_recorder->Stop();
    });

    connect(m_help__about, &QAction::triggered, this, [&]() {
        AboutDialog *aboutDialog = new AboutDialog(this);
        aboutDialog->exec();
    });
}


// This is used to set the state of the audio playback. Call Play() if we have to play the audio
// or call Pause() if we want to pause.
void Sonify::PlayAudio()
{

    m_isAudioPlaying = !m_isAudioPlaying;
    if (m_isAudioPlaying)
        Play();
    else
        Pause();
}

// Function that is similar to numpy linspace.
QVector<double> linspace(double start, double stop, int num) {
    QVector<double> result;
    result.reserve(num);
    double step = (stop - start) / (num - 1);
    for (int i = 0; i < num; ++i) {
        result.append(start + i * step);
    }
    return result;
}

// Function that handles the waveform widget viewing
void Sonify::viewWaveform(bool state)
{
    if (state)
    {
        if (!m_wf_widget)
        {
            m_wf_widget = new WaveformWidget();
            connect(m_wf_widget, &WaveformWidget::closed, this, [&]() {
                m_tools__tone_generator->setChecked(false);
            });
        }
        m_wf_widget->show();
        auto data = readWAVFile("/home/neo/Downloads/gettysburg10.wav");
        /*auto data = sonification->getAudioData();*/

        m_wf_widget->setData(data);
    }
    else
        m_wf_widget->close();
}

// Function that handles saving the WAV audio file
bool Sonify::Save(QString filename)
{
    if (filename.isEmpty())
    {
        QFileDialog fd;
        QString filename = fd.getSaveFileName(this, "Save Audio", nullptr, "Audio Files (*.wav)");

        if (filename.isEmpty()) return false;

        return sonification->save(filename);
    }

    return false;
}

// Function that handles Opening an image. If filename is specified, image will be opened, otherwise a file dialog will ask for the file.
void Sonify::Open(QString filename)
{

    if (filename.isEmpty())
    {
        QFileDialog fd;
        QStringList files = fd.getOpenFileNames(this, "Open Image", nullptr, "Image Files (*.png *.jpeg *.jpg)");

        if (files.empty())
            return;

        filename = files[0];
    }

    if (m_def_ask_for_resize)
        AskForResize(filename);
    else {
        m_pix = QPixmap(filename);
        if (m_def_keep_aspect)
            m_pix = m_pix.scaled(m_def_width, m_def_height, Qt::KeepAspectRatio);
        else
            m_pix = m_pix.scaled(m_def_width, m_def_height, Qt::IgnoreAspectRatio);
    }

    gv->setPixmap(m_pix);
    m_sonify_btn->setEnabled(true);
}

// Function that handles action of sonification
void Sonify::doSonify()
{
    m_sonify_btn->setEnabled(false);
    sonification->stopSonification(false);
    m_progress_bar->reset();
    m_traverse_combo->setEnabled(false);
    m_num_samples_spinbox->setEnabled(false);
    sonification->setNumSamples(m_num_samples_spinbox->value());

    if (m_traverse_combo->currentText() == m_traversal_name_list[Traverse::LEFT_TO_RIGHT])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::LEFT_TO_RIGHT;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == m_traversal_name_list[Traverse::RIGHT_TO_LEFT])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::RIGHT_TO_LEFT;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == m_traversal_name_list[Traverse::TOP_TO_BOTTOM])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::TOP_TO_BOTTOM;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == m_traversal_name_list[Traverse::BOTTOM_TO_TOP])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::BOTTOM_TO_TOP;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == m_traversal_name_list[Traverse::CIRCLE_OUTWARDS])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::CIRCLE_OUTWARDS;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == m_traversal_name_list[Traverse::CIRCLE_INWARDS])
    {
        m_stop_sonification_btn->setVisible(true);
        m_progress_bar->setVisible(true);
        m_mode = Traverse::CIRCLE_INWARDS;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == m_traversal_name_list[Traverse::CLOCKWISE])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::CLOCKWISE;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == m_traversal_name_list[Traverse::ANTICLOCKWISE])
    {
        m_progress_bar->setVisible(true);
        m_stop_sonification_btn->setVisible(true);
        m_mode = Traverse::ANTICLOCKWISE;
        sonification->Sonify(m_pix, gv, m_mode);
    }

    else if (m_traverse_combo->currentText() == m_traversal_name_list[Traverse::PATH])
    {
        gv->setDrawPathMode(true);
        connect(gv, &GV::drawPathFinished, this, [&]() {
            m_mode = Traverse::PATH;
            sonification->Sonify(m_pix, gv, m_mode);
        });
    }
}

// Reset btn function
void Sonify::Reset()
{
    m_play_btn->setText("Play");
    m_isAudioPlaying = false;
    gv->reset();
    m_wf_widget->resetVertLine();
    sonification->reset();
    m_audio_progress_label->setText("");
}

Sonify::~Sonify()
{}

// TODO: Screen record
void Sonify::CaptureWindow()
{
}

// Function to ask for image resize when opening
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

    keep_aspect_ratio_cb->setChecked(m_def_keep_aspect);

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

    ask_widget->exec();

}

// Function that handles 'play' state
void Sonify::Play()
{
    sonification->play();
    m_play_btn->setText("Pause");
    m_reset_btn->setEnabled(false);
    m_num_samples_spinbox->setEnabled(false);
    m_traverse_combo->setEnabled(false);
}

// Function that handles 'pause' state
void Sonify::Pause()
{

    sonification->pause();
    m_play_btn->setText("Play");
    m_reset_btn->setEnabled(true);
    m_num_samples_spinbox->setEnabled(true);
    m_traverse_combo->setEnabled(true);

}

/*void Sonify::ConvertToVideo()*/
/*{*/
/*    std::vector<std::string> imagePaths = {*/
/*        "temp/0.png",*/
/*        "temp/1.png",*/
/*        "temp/2.png"*/
/*        // Add more image paths as needed*/
/*    };*/
/**/
/*    std::vector<cv::Mat> images;*/
/**/
/*    for (const auto& path : imagePaths) {*/
/*        cv::Mat img = cv::imread(path);*/
/*        if (img.empty()) {*/
/*            std::cerr << "Could not read image: " << path << std::endl;*/
/*            continue;*/
/*        }*/
/*        images.push_back(img);*/
/*    }*/
/**/
/*    if (images.empty()) {*/
/*        std::cerr << "No images were loaded." << std::endl;*/
/*        return;*/
/*    }*/
/**/
/*    // Define the codec and create VideoWriter object*/
/*    cv::VideoWriter video("output.avi", cv::VideoWriter::fourcc('M','J','P','G'), 10, cv::Size(images[0].cols, images[0].rows));*/
/**/
/*    if (!video.isOpened()) {*/
/*        std::cerr << "Could not open the output video file for write." << std::endl;*/
/*        return;*/
/*    }*/
/**/
/*    for (const auto& img : images) {*/
/*        video.write(img);*/
/*    }*/
/**/
/*    video.release();*/
/*    std::cout << "Video created successfully!" << std::endl;*/
/*}*/
