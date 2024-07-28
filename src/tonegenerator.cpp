#include "tonegenerator.hpp"

ToneGenerator::ToneGenerator(QWidget *parent)
    : QDialog(parent)
{

    this->setLayout(m_layout);
    m_layout->addWidget(m_freq_label);
    m_layout->addWidget(m_freq_sb);
    m_layout->addWidget(m_duration_label);
    m_layout->addWidget(m_duration_sb);
    m_layout->addWidget(m_play_btn);

    m_freq_sb->setRange(20, 20000);
    m_duration_sb->setRange(1, 100);

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        qDebug() << "SDL could not initialize! SDL_Error: " << SDL_GetError();
        return;
    }

    SDL_zero(m_wavSpec);
    m_wavSpec.format = AUDIO_S16LSB;
    m_wavSpec.channels = 1;
    m_wavSpec.callback = sdlAudioCallback;
    m_wavSpec.userdata = this;

    m_audioDevice = SDL_OpenAudioDevice(nullptr, 0, &m_wavSpec, nullptr, 0);
    if (m_audioDevice == 0) {
        qDebug() << "Failed to open audio device: " << SDL_GetError();
        return;
    }

    connect(m_play_btn, &QPushButton::clicked, this, [&]() {
        m_play_btn->setEnabled(false);
        m_playing = true;
        m_audioOffset = 0;
        m_audioData = generateSineWave(400, m_freq_sb->text().toInt(), m_duration_sb->text().toInt());
        SDL_PauseAudioDevice(m_audioDevice, 0);
    });

    this->show();
}

void ToneGenerator::sdlAudioCallback(void *userData, Uint8* _stream, int _len)
{
    ToneGenerator* s = reinterpret_cast<ToneGenerator *>(userData);
    Sint16 *stream = reinterpret_cast<Sint16*>(_stream);
    int len = _len / 2;

    int bytesToCopy = std::min(static_cast<int>(s->m_audioData.size() * sizeof(short) - s->m_audioOffset), len);

    if (bytesToCopy == 0) {
        memset(stream + bytesToCopy, 0, len - bytesToCopy);
        SDL_PauseAudioDevice(s->m_audioDevice, 1); // Stop audio playback
        emit s->audioFinishedPlaying();
        s->m_play_btn->setEnabled(true);
        s->m_playing = false;
    }

    memcpy(stream, s->m_audioData.data() + s->m_audioOffset / sizeof(short), bytesToCopy);

    s->m_audioOffset += bytesToCopy;
}

QVector<short> ToneGenerator::generateSineWave(double amplitude, double frequency, double time)
{
    QVector<short> fs;
    fs.resize(m_sampleRate * time);
    double val = 2 * M_PI * frequency / m_sampleRate;
    for(int i=0; i < m_sampleRate * time; i++)
        fs[i] = amplitude * sin(val * i);
    return fs;
}

ToneGenerator::~ToneGenerator()
{}
