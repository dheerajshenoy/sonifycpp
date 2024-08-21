#include "tonegenerator.hpp"

ToneGenerator::ToneGenerator(QWidget *parent) noexcept
    : QDialog(parent)
{

    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    this->setLayout(m_layout);
    m_layout->addWidget(m_splitter);
    m_splitter->addWidget(m_side_panel);
    m_splitter->addWidget(m_plot);

    this->setMinimumSize(QSize(720, 480));

    m_splitter->setStretchFactor(1, 1);

    m_plot->addGraph();
    m_plot->yAxis->setRange(-2, 2);
    m_plot->replot();
    m_plot->setInteractions(QCP::Interaction::iRangeDrag | QCP::Interaction::iRangeZoom);

    m_side_panel->setLayout(m_side_panel_layout);

    for(int i=0; i < m_wavetype_labels.size(); i++)
        m_wave_type_combo->addItem(m_wavetype_labels[i]);

    m_side_panel_layout->addWidget(m_wave_type_label, 0, 0);
    m_side_panel_layout->addWidget(m_wave_type_combo, 0, 1);
    m_side_panel_layout->addWidget(m_freq_label, 1, 0);
    m_side_panel_layout->addWidget(m_freq_sb, 1, 1);
    m_side_panel_layout->addWidget(m_amplitude_label, 2, 0);
    m_side_panel_layout->addWidget(m_amplitude_sb, 2, 1);
    m_side_panel_layout->addWidget(m_duration_label, 3, 0);
    m_side_panel_layout->addWidget(m_duration_sb, 3, 1);
    m_side_panel_layout->addWidget(m_color_label, 4, 0);
    m_side_panel_layout->addWidget(m_color_btn, 4, 1);
    m_side_panel_layout->addWidget(m_play_btn, 6, 0, 1, 2);
    m_side_panel_layout->addWidget(m_stop_btn, 6, 0, 1, 2);
    m_side_panel_layout->addWidget(m_audio_progress, 7, 0, 1, 2);

    m_color_btn->setFixedSize(QSize(50, 10));
    m_stop_btn->setVisible(false);

    m_audio_progress->setVisible(false);
    QLabel *separator = new QLabel();
    separator->setAlignment(Qt::AlignmentFlag::AlignCenter);
    m_side_panel_layout->addWidget(separator, 8, 0);

    m_amplitude_sb->setRange(0, 1);
    m_amplitude_sb->setSingleStep(0.1);
    m_amplitude_sb->setValue(1.0);
    m_amplitude_sb->setDecimals(2);

    m_freq_sb->setRange(1, 40000);
    m_freq_sb->setValue(1.0);
    m_duration_sb->setRange(1, 100);
    m_duration_sb->setValue(10);


    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        qDebug() << "SDL could not initialize! SDL_Error: " << SDL_GetError();
        return;
    }

    SDL_zero(m_wavSpec);
    m_wavSpec.format = AUDIO_S16LSB;
    m_wavSpec.channels = 1;
    m_wavSpec.callback = sdlAudioCallback;
    m_wavSpec.userdata = this;
    m_wavSpec.samples = 1024;
    m_wavSpec.freq = 44100;

    m_audioDevice = SDL_OpenAudioDevice(nullptr, 0, &m_wavSpec, nullptr, 0);
    if (m_audioDevice == 0) {
        qDebug() << "Failed to open audio device: " << SDL_GetError();
        return;
    }

    connect(m_play_btn, &QPushButton::clicked, this, [&]() {
        m_play_btn->setVisible(false);
        m_stop_btn->setVisible(true);
        m_audio_progress->setVisible(true);
        m_play_btn->setEnabled(false);
        m_playing = true;
        m_audioOffset = 0;
        switch(m_wavetype)
        {
            case WaveType::SINE:
                m_audioData = generateSineWave(m_amplitude_sb->text().toFloat(), m_freq_sb->text().toDouble(), m_duration_sb->text().toFloat());
            break;

            case WaveType::SAWTOOTH:
                m_audioData = generateSawtoothWave(m_amplitude_sb->text().toFloat(), m_freq_sb->text().toDouble(), m_duration_sb->text().toFloat());
            break;

            case WaveType::SQUARE:
                m_audioData = generateSquareWave(m_amplitude_sb->text().toFloat(), m_freq_sb->text().toDouble(), m_duration_sb->text().toFloat());
            break;

            case WaveType::TRAINGULAR:
                m_audioData = generateTriangularWave(m_amplitude_sb->text().toFloat(), m_freq_sb->text().toDouble(), m_duration_sb->text().toFloat());
            break;
        }
        SDL_PauseAudioDevice(m_audioDevice, 0);
    });

    QColor color = QColor::fromString("#FF5000");
    m_plot->graph(0)->setPen(QPen(color, 1));
    QPalette palette = m_color_btn->palette();
    palette.setColor(QPalette::Button, color);
    m_color_btn->setPalette(palette);
    m_color_btn->update();
    m_plot->replot();

    m_color_btn->setAutoFillBackground(true);
    connect(m_color_btn, &QPushButton::clicked, this, [&]() {
        QColorDialog cd;
        QColor color = cd.getColor();
        QPalette palette = m_color_btn->palette();
        palette.setColor(QPalette::Button, color);
        m_color_btn->setPalette(palette);
        m_color_btn->update();
        m_plot->graph(0)->setPen(QPen(color, 1));
        m_plot->replot();

    });

    this->show();

    connect(m_wave_type_combo, &QComboBox::currentIndexChanged, this, [&](int index) {
        switch(index)
        {
            case 0:
                m_wavetype = WaveType::SINE;
            break;

            case 1:
                m_wavetype = WaveType::TRAINGULAR;
            break;

            case 2:
                m_wavetype = WaveType::SAWTOOTH;
            break;

            case 3:
                m_wavetype = WaveType::SQUARE;
            break;
        }
        plotWave(m_wavetype);
    });

    connect(this, &ToneGenerator::audioFinishedPlaying, this, [&]() {
        m_play_btn->setEnabled(true);
        m_playing = false;
        m_audio_progress->setVisible(false);
        m_play_btn->setVisible(true);
        m_stop_btn->setVisible(false);
    });

    connect(m_stop_btn, &QPushButton::clicked, this, [&]() {
        SDL_PauseAudioDevice(m_audioDevice, 1);
        m_audio_progress->setVisible(false);
        m_play_btn->setVisible(true);
        m_stop_btn->setVisible(false);
        m_play_btn->setEnabled(true);
    });

    connect(m_duration_sb, &QDoubleSpinBox::valueChanged, this, [&]() {
        plotWave(m_wavetype);
    });

    connect(m_freq_sb, &QDoubleSpinBox::valueChanged, this, [&]() {
        plotWave(m_wavetype);
    });

    connect(m_amplitude_sb, &QDoubleSpinBox::valueChanged, this, [&]() {
        plotWave(m_wavetype);
    });


    connect(this, &ToneGenerator::audioProgress, m_audio_progress, [&](int progress) {
        m_audio_progress->setValue(progress);
    });

    plotWave(WaveType::SINE);
}

void ToneGenerator::plotWave(const WaveType &wave) noexcept
{
    QVector<double> x, y;
    auto duration = m_duration_sb->text().toFloat();
    int sampleRate = 44100;
    x.resize(duration * sampleRate);
    y.resize(duration * sampleRate);
    auto frequency = m_freq_sb->text().toFloat();
    auto amplitude = m_amplitude_sb->text().toDouble();
    switch(wave)
    {
        case WaveType::SINE:
            for(int i=0; i < x.size(); i++)
            {
                x[i] = i / static_cast<double>(sampleRate);
                y[i] = amplitude * qSin(x[i] * frequency * 2 * M_PI);
            }

            break;
        case WaveType::SAWTOOTH:
            for(int i=0; i < x.size(); i++)
            {
                x[i] = i / static_cast<double>(sampleRate);
                y[i] = amplitude * 2.0 * (i * frequency / static_cast<double>(sampleRate) - floor(0.5 + i * frequency / static_cast<double>(sampleRate)));
            }
        break;

        case WaveType::SQUARE:
            for(int i=0; i < x.size(); i++)
            {
                x[i] = i / static_cast<double>(sampleRate);
                y[i] = (std::sin(2 * M_PI * frequency * x[i]) >= 0) ? amplitude : -amplitude;
            }
        break;

        case WaveType::TRAINGULAR:
            for(int i=0; i < x.size(); i++)
            {
                x[i] = i / static_cast<double>(sampleRate);
                y[i] = amplitude * 2.0 * std::abs(2.0 * ((i * frequency / static_cast<double>(sampleRate)) - std::floor((i * frequency / static_cast<double>(sampleRate)) + 0.5))) - amplitude;
            }
        break;

    }
    m_plot->graph(0)->setData(x, y);
    m_plot->replot();
}

void ToneGenerator::sdlAudioCallback(void *userData, Uint8* _stream, int len) noexcept
{
    ToneGenerator* s = reinterpret_cast<ToneGenerator *>(userData);
    Sint16 *stream = reinterpret_cast<Sint16*>(_stream);

    int bytesToCopy = std::min(static_cast<int>(s->m_audioData.size() * sizeof(short) - s->m_audioOffset), len);

    if (bytesToCopy == 0) {
        memset(stream + bytesToCopy, 0, len - bytesToCopy);
        SDL_PauseAudioDevice(s->m_audioDevice, 1); // Stop audio playback
        emit s->audioFinishedPlaying();
    }

    memcpy(stream, s->m_audioData.data() + s->m_audioOffset / sizeof(short), bytesToCopy);

    s->m_audioOffset += bytesToCopy;
    int progress = static_cast<int>((s->m_audioOffset / static_cast<double>(s->m_audioData.size() * sizeof(short))) * 100);
    emit s->audioProgress(progress);
}

QVector<short> ToneGenerator::generateSineWave(const double& _amplitude, const double& frequency,
                                               const double& time) noexcept
{
    QVector<short> fs;
    int N = m_sampleRate * time;
    fs.resize(N);
    int amplitude = _amplitude * 32767;
    double val = 2 * M_PI * frequency / m_sampleRate;
    double angle = 0.0f;
    for(int i=0; i < N; i++)
    {
        fs[i] = amplitude * sin(angle);
        angle += val;
    }
    return fs;
}

QVector<short> ToneGenerator::generateSquareWave(const double& _amplitude, const double& frequency,
                                                 const double& time) noexcept
{
    QVector<short> fs;
    int N = m_sampleRate * time;
    fs.resize(N);
    int amplitude = _amplitude * 32767;
    for(int i=0; i < N; i++)
    {
        fs[i] = amplitude * (std::sin(2 * M_PI * frequency * i / m_sampleRate) >= 0) ? 1.0 : -1.0;
    }
    return fs;
}

QVector<short> ToneGenerator::generateTriangularWave(const double& _amplitude, const double& frequency,
                                                     const double& time) noexcept
{
    QVector<short> fs;
    int N = m_sampleRate * time;
    fs.resize(N);
    int amplitude = _amplitude * 32767;
    for(int i=0; i < N; i++)
    {
        fs[i] = amplitude * 2.0 * std::abs(2.0 * ((i * frequency / static_cast<double>(m_sampleRate)) - std::floor((i * frequency / static_cast<double>(m_sampleRate)) + 0.5))) - 1.0; 
    }
    return fs;
}

QVector<short> ToneGenerator::generateSawtoothWave(const double& _amplitude, const double& frequency,
                                                   const double& time) noexcept
{
    QVector<short> fs;
    int N = m_sampleRate * time;
    fs.resize(N);
    int amplitude = _amplitude * 32767;
    for(int i=0; i < N; i++)
    {
        fs[i] = amplitude * 2.0 * (i * frequency / static_cast<double>(m_sampleRate) - floor(0.5 + i * frequency / static_cast<double>(m_sampleRate)));
    }
    return fs;
}

ToneGenerator::~ToneGenerator()
{
    if (m_audioDevice) {
        SDL_CloseAudioDevice(m_audioDevice);
    }
    SDL_Quit();
}

void ToneGenerator::closeEvent(QCloseEvent *e) noexcept
{
    emit closed();
}

void ToneGenerator::keyPressEvent(QKeyEvent *e) noexcept
{
    if(e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
    else {/* minimize */}
}
