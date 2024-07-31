/*
*
*   Contains function for audio playback of the sonified data
*
*/

#include "sonification.hpp"
#include <cmath>

Sonification::Sonification()
{
    m_val = M_PI2 * m_NumSamples / m_SampleRate;

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        qDebug() << "SDL could not initialize! SDL_Error: " << SDL_GetError();
        return;
    }

    sonifier = new Sonifier();

    connect(sonifier, &Sonifier::sonificationDone, this, [&](QVector<short>audioData) {
        m_audioData = audioData;
        emit sonificationDone();
    });
    connect(sonifier, &Sonifier::sonificationProgress, this, [&](int progress) {
        emit sonificationProgress(progress);
    });
}

Sonification::~Sonification()
{
    if (m_audioDevice) {
        SDL_CloseAudioDevice(m_audioDevice);
    }
    SDL_Quit();

    if (m_thread->isRunning())
    {
        sonifier->stopSonifying(true);
        m_thread->quit();
        m_thread->wait();
    }
}

void Sonification::setNumSamples(int nsamples) noexcept
{
    m_NumSamples = nsamples; 
    m_val = M_PI2 * m_NumSamples / m_SampleRate;
}

// Function to sonify an `image` provided by QImage and in mode `mode`
void Sonification::Sonify(QPixmap &pix, GV *gv, Traverse mode)
{
    // Return if null
    if (pix.isNull()) return;

    m_traverse = mode;

    SDL_zero(m_wavSpec);
    m_wavSpec.samples = m_NumSamples;
    m_wavSpec.freq = m_SampleRate;
    m_wavSpec.format = AUDIO_S16LSB;
    m_wavSpec.channels = m_ChannelCount;
    m_wavSpec.callback = sdlAudioCallback;
    m_wavSpec.userdata = this;

    sonifier->setSampleRate(m_SampleRate);
    sonifier->setSamples(m_NumSamples);

    if (m_audioDevice)
        SDL_CloseAudioDevice(m_audioDevice);
    m_audioDevice = SDL_OpenAudioDevice(nullptr, 0, &m_wavSpec, nullptr, 0);
    if (m_audioDevice == 0) {
        qDebug() << "Failed to open audio device: " << SDL_GetError();
        return;
    }

    if (mode == Traverse::PATH)
    {
        sonifier->setParameters(pix, mode, gv->getPathDrawnPos());
    }
    else
        sonifier->setParameters(pix, mode);

    if (!m_thread)
    {
        m_thread = new QThread();
        connect(m_thread, &QThread::started, sonifier, &Sonifier::Sonify);
        connect(m_thread, &QThread::finished, this, [&]() {});
        connect(sonifier, &Sonifier::sonificationDone, this, [&]() {
            m_thread->quit();
        });
        sonifier->moveToThread(m_thread);
    }

    m_thread->start();
}

void Sonification::pause()
{
    SDL_PauseAudioDevice(m_audioDevice, 1);
}

void Sonification::play()
{
    SDL_PauseAudioDevice(m_audioDevice, 0);
}

void Sonification::reset()
{
    SDL_LockAudioDevice(m_audioDevice); // Lock the audio device to avoid race conditions
    m_audioOffset = 0; // Reset the audio offset
    SDL_UnlockAudioDevice(m_audioDevice); // Unlock the audio device
    SDL_PauseAudioDevice(m_audioDevice, 1);
}

// Save the audio data to file
bool Sonification::save(QString filename, Format f) noexcept
{
    switch(f)
    {
        case Format::WAV:
            if(m_GenerateWavFile(filename))
                return true;
            break;

        case Format::MP3:
            // TODO: Handle MP3 saving
            break;
    }

    return false;
}

// Helper function to generate WAV file
bool Sonification::m_GenerateWavFile(QString filename)
{
    SF_INFO sfinfo;
    sfinfo.samplerate = m_SampleRate;
    sfinfo.frames = getDuration();
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    sfinfo.channels = 1;

    SNDFILE *sndfile = sf_open(filename.toStdString().c_str(), SFM_WRITE, &sfinfo);

    if (!sndfile) {
        qDebug() << "Error writing to file: Error %d" << sf_error(sndfile);
        m_audioData.clear();
        return false;
    }

    sf_count_t count = sf_write_short(sndfile, m_audioData.data(), m_audioData.size());

    /*if (count != m_NumSamples)*/
    /*{*/
    /*    fprintf(stderr, "Error writing to file: Error %d", sf_error(sndfile));*/
    /*    sf_close(sndfile);*/
    /*    m_audioData.clear();*/
    /*    return false;*/
    /*}*/

    sf_close(sndfile);

    return true;
}

double Sonification::getDuration() noexcept
{
    return m_audioData.size() / m_SampleRate;
}

void Sonification::sdlAudioCallback(void* userdata, Uint8* _stream, int len)
{

    Sonification *s = static_cast<Sonification*>(userdata);

    if (s->m_audioData.size() == 0) return;

    Sint16 *stream = reinterpret_cast<Sint16*>(_stream);

    int bytesToCopy = std::min(static_cast<int>(s->m_audioData.size() * sizeof(short) - s->m_audioOffset), len);

    if (bytesToCopy == 0) {
        memset(stream + bytesToCopy, 0, len - bytesToCopy);
        SDL_PauseAudioDevice(s->m_audioDevice, 1); // Stop audio playback
        emit s->audioFinishedPlaying();
    }

    memcpy(stream, s->m_audioData.data() + s->m_audioOffset / sizeof(short), bytesToCopy);

    s->m_audioOffset += bytesToCopy;

    auto d = s->m_audioOffset / sizeof(short);
    emit s->audioprogress(d);
    emit s->audioindex(d / static_cast<double>(s->m_NumSamples));

}

int Sonification::getNumSamples() noexcept
{
    return m_NumSamples;
}

void Sonification::setAudioData(QVector<short> &audioData) noexcept
{
    m_audioData = audioData;
}

int& Sonification::getSampleRate() noexcept
{
    return m_SampleRate;
}

QVector<short>& Sonification::getAudioData() noexcept
{
    return m_audioData;
}

void Sonification::stopSonification(bool state) noexcept
{
    sonifier->stopSonifying(state);
    if (state)
    {
        if (m_thread->isRunning())
        {
            m_thread->quit();
            m_thread->wait();
        }
    }

    emit sonificationStopped();
}

