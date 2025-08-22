#include "tonegenerator.hpp"

#include "sonification.hpp"

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_surface.h>

ToneGenerator::ToneGenerator(QWidget *parent) noexcept : QDialog(parent)
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
    m_plot->setInteractions(QCP::Interaction::iRangeDrag |
                            QCP::Interaction::iRangeZoom);

    m_side_panel->setLayout(m_side_panel_layout);

    for (const auto &l : m_wavetype_labels)
        m_wave_type_combo->addItem(l);

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

    if (!SDL_Init(SDL_INIT_AUDIO))
    {
        qDebug() << "SDL could not initialize! SDL_Error: " << SDL_GetError();
        return;
    }

    SDL_zero(m_audioSpec);
    m_audioSpec.format   = SDL_AUDIO_S16;
    m_audioSpec.channels = 1;
    m_audioSpec.freq     = 44100;

    m_audioStream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_audioSpec,
        &ToneGenerator::audioCallback, this);

    if (!m_audioStream)
    {
        qDebug() << "Failed to open audio device stream: " << SDL_GetError();
        return;
    }

    connect(m_play_btn, &QPushButton::clicked, this, [&]()
    {
        m_play_btn->setVisible(false);
        m_stop_btn->setVisible(true);
        m_audio_progress->setVisible(true);
        m_play_btn->setEnabled(false);
        m_playing     = true;
        m_audioOffset = 0;
        switch (m_wavetype)
        {
            case WaveType::SINE:
                m_audioData = generateSineWave(m_amplitude_sb->text().toFloat(),
                                               m_freq_sb->text().toDouble(),
                                               m_duration_sb->text().toFloat());
                break;

            case WaveType::SAWTOOTH:
                m_audioData =
                    generateSawtoothWave(m_amplitude_sb->text().toFloat(),
                                         m_freq_sb->text().toDouble(),
                                         m_duration_sb->text().toFloat());
                break;

            case WaveType::SQUARE:
                m_audioData =
                    generateSquareWave(m_amplitude_sb->text().toFloat(),
                                       m_freq_sb->text().toDouble(),
                                       m_duration_sb->text().toFloat());
                break;

            case WaveType::TRAINGULAR:
                m_audioData =
                    generateTriangularWave(m_amplitude_sb->text().toFloat(),
                                           m_freq_sb->text().toDouble(),
                                           m_duration_sb->text().toFloat());
                break;
        }
        SDL_FlushAudioStream(m_audioStream);
        SDL_ResumeAudioStreamDevice(m_audioStream);
    });

    QColor color = QColor::fromString("#FF5000");
    m_plot->graph(0)->setPen(QPen(color, 1));
    QPalette palette = m_color_btn->palette();
    palette.setColor(QPalette::Button, color);
    m_color_btn->setPalette(palette);
    m_color_btn->update();
    m_plot->replot();

    m_color_btn->setAutoFillBackground(true);
    connect(m_color_btn, &QPushButton::clicked, this, [&]()
    {
        QColorDialog cd;
        QColor color     = cd.getColor();
        QPalette palette = m_color_btn->palette();
        palette.setColor(QPalette::Button, color);
        m_color_btn->setPalette(palette);
        m_color_btn->update();
        m_plot->graph(0)->setPen(QPen(color, 1));
        m_plot->replot();
    });

    this->show();

    connect(m_wave_type_combo, &QComboBox::currentIndexChanged, this,
            [&](int index)
    {
        switch (index)
        {
            case 0: m_wavetype = WaveType::SINE; break;

            case 1: m_wavetype = WaveType::TRAINGULAR; break;

            case 2: m_wavetype = WaveType::SAWTOOTH; break;

            case 3: m_wavetype = WaveType::SQUARE; break;
        }
        plotWave(m_wavetype);
    });

    connect(this, &ToneGenerator::audioFinishedPlaying, this, [&]()
    {
        m_play_btn->setEnabled(true);
        m_playing = false;
        m_audio_progress->setVisible(false);
        m_play_btn->setVisible(true);
        m_stop_btn->setVisible(false);
    });

    connect(m_stop_btn, &QPushButton::clicked, this, [&]()
    {
        SDL_PauseAudioStreamDevice(m_audioStream);
        m_audio_progress->setVisible(false);
        m_play_btn->setVisible(true);
        m_stop_btn->setVisible(false);
        m_play_btn->setEnabled(true);
    });

    connect(m_duration_sb, &QDoubleSpinBox::valueChanged, this,
            [&]() { plotWave(m_wavetype); });

    connect(m_freq_sb, &QDoubleSpinBox::valueChanged, this,
            [&]() { plotWave(m_wavetype); });

    connect(m_amplitude_sb, &QDoubleSpinBox::valueChanged, this,
            [&]() { plotWave(m_wavetype); });

    connect(this, &ToneGenerator::audioProgress, m_audio_progress,
            [&](int progress) { m_audio_progress->setValue(progress); });

    plotWave(WaveType::SINE);
}

ToneGenerator::~ToneGenerator()
{
    if (m_audioStream) SDL_PauseAudioStreamDevice(m_audioStream);
    SDL_Quit();
}

void
ToneGenerator::plotWave(const WaveType &wave) noexcept
{
    const float duration   = m_duration_sb->text().toFloat();
    const float sampleRate = 44100.0f;
    const float frequency  = m_freq_sb->text().toFloat();
    const double amplitude = m_amplitude_sb->text().toDouble();

    const int N = static_cast<int>(duration * sampleRate);
    QVector<double> x(N), y(N);

    const double step = 1.0 / sampleRate;

    switch (wave)
    {
        case WaveType::SINE:
            for (int i = 0; i < N; i++)
            {
                x[i] = i * step;
                y[i] = amplitude * std::sin(2 * M_PI * frequency * x.at(i));
            }
            break;

        case WaveType::SAWTOOTH:
            for (int i = 0; i < N; i++)
            {
                x[i]         = i * step;
                double phase = frequency * x.at(i);
                y[i] = amplitude * 2.0 * (phase - std::floor(phase + 0.5));
            }
            break;

        case WaveType::SQUARE:
            for (int i = 0; i < N; i++)
            {
                x[i] = i * step;
                y[i] = (std::sin(2 * M_PI * frequency * x.at(i)) >= 0)
                           ? amplitude
                           : -amplitude;
            }
            break;

        case WaveType::TRAINGULAR:
            for (int i = 0; i < N; i++)
            {
                x[i]         = i * step;
                double phase = frequency * x.at(i);
                y[i] =
                    amplitude *
                    (2.0 * std::abs(2.0 * (phase - std::floor(phase + 0.5))) -
                     1.0);
            }
            break;
    }

    m_plot->graph(0)->setData(x, y);
    m_plot->replot();
}

void SDLCALL
ToneGenerator::audioCallback(void *userdata, SDL_AudioStream *_stream,
                             int additional, int /* total */) noexcept
{
    /*
      `total` is how much data the audio stream is eating right now,
      additional_amount is how much more it needs than what it currently has
      queued (which might be zero!). You can supply any amount of data here; it
      will take what it needs and use the extra later. If you don't give it
      enough, it will take everything and then feed silence to the hardware for
      the rest. Ideally, though, we always give it what it needs and no extra,
      so we aren't buffering more than necessary.
     */
    ToneGenerator *s = static_cast<ToneGenerator *>(userdata);

    if (s->m_audioData.empty()) return;

    const int totalBytes =
        static_cast<int>(s->m_audioData.size() * sizeof(short));
    const int bytesRemaining = totalBytes - s->m_audioOffset;

    if (bytesRemaining <= 0)
    {
        // Audio is finished
        QMetaObject::invokeMethod(s, [s]() { emit s->audioFinishedPlaying(); },
                                  Qt::QueuedConnection);
        SDL_PauseAudioStreamDevice(s->m_audioStream);

        return;
    }

    int bytesToCopy = std::min(additional, bytesRemaining);

    SDL_PutAudioStreamData(
        _stream,
        reinterpret_cast<const uint8_t *>(s->m_audioData.data()) +
            s->m_audioOffset,
        bytesToCopy);

    s->m_audioOffset += bytesToCopy;

    // Optional progress reporting
    int progress = static_cast<int>(
        (s->m_audioOffset / static_cast<double>(totalBytes)) * 100);
    QMetaObject::invokeMethod(s, [s, progress]()
    { emit s->audioProgress(progress); }, Qt::QueuedConnection);
}

std::vector<short>
ToneGenerator::generateSineWave(double _amplitude, double frequency,
                                double time) noexcept
{
    const int N         = static_cast<int>(m_sampleRate * time);
    const int amplitude = static_cast<int>(_amplitude * 32767);
    const double val    = 2 * M_PI * frequency / m_sampleRate;
    double angle        = 0.0f;

    std::vector<short> fs(N);

    for (int i = 0; i < N; i++)
    {
        fs[i] = static_cast<short>(amplitude * sin(angle));
        angle += val;
    }
    return fs;
}

std::vector<short>
ToneGenerator::generateSquareWave(double _amplitude, double frequency,
                                  double time) noexcept
{
    const int N         = static_cast<int>(m_sampleRate * time);
    const int amplitude = static_cast<int>(_amplitude * 32767);

    std::vector<short> fs(N);

    const double phaseStep = 2.0 * M_PI * frequency / m_sampleRate;

    for (int i = 0; i < N; i++)
    {
        fs[i] = static_cast<short>(
            (std::sin(i * phaseStep) >= 0.0) ? amplitude : -amplitude);
    }
    return fs;
}

std::vector<short>
ToneGenerator::generateTriangularWave(double _amplitude, double frequency,
                                      double time) noexcept
{
    std::vector<short> fs;
    int N = static_cast<int>(m_sampleRate * time);
    fs.resize(N);
    int amplitude = static_cast<int>(_amplitude * 32767);
    for (int i = 0; i < N; i++)
    {
        fs[i] = static_cast<short>(
            amplitude * 2.0 *
                std::abs(2.0 *
                         ((i * frequency / static_cast<double>(m_sampleRate)) -
                          std::floor((i * frequency /
                                      static_cast<double>(m_sampleRate)) +
                                     0.5))) -
            1.0);
    }
    return fs;
}

std::vector<short>
ToneGenerator::generateSawtoothWave(double _amplitude, double frequency,
                                    double time) noexcept
{
    std::vector<short> fs;
    int N = static_cast<int>(m_sampleRate * time);
    fs.resize(N);
    int amplitude = static_cast<int>(_amplitude * 32767);
    for (int i = 0; i < N; i++)
    {
        fs[i] = static_cast<short>(
            amplitude * 2.0 *
            (i * frequency / static_cast<double>(m_sampleRate) -
             floor(0.5 + i * frequency / static_cast<double>(m_sampleRate))));
    }
    return fs;
}

void
ToneGenerator::closeEvent(QCloseEvent *e) noexcept
{
    emit closed();
}

void
ToneGenerator::keyPressEvent(QKeyEvent *e) noexcept
{
    if (e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
    else { /* minimize */ }
}
