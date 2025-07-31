// Contains function for audio playback of the sonified data

#include "sonification.hpp"

#include <SDL3/SDL_audio.h>
#include <cmath>

// Constructor
Sonification::Sonification() noexcept
{
    m_val = M_PI2 * m_NumSamples / m_SampleRate;

    if (!SDL_Init(SDL_INIT_AUDIO))
    {
        qDebug() << "SDL could not initialize! SDL_Error: " << SDL_GetError();
        return;
    }

    sonifier = new Sonifier();

    connect(sonifier, &Sonifier::sonificationDone, this,
            [&](QVector<short> audioData)
    {
        m_audioData = audioData;
        emit sonificationDone();
    });

    connect(sonifier, &Sonifier::sonificationProgress, this,
            [&](int progress) { emit sonificationProgress(progress); });
}

// Destructor
Sonification::~Sonification() noexcept
{
    if (m_audioDevice)
    {
        SDL_CloseAudioDevice(m_audioDevice);
    }

    SDL_Quit();

    if (m_thread && m_thread->isRunning())
    {
        sonifier->stopSonifying(true);
        m_thread->quit();
        m_thread->wait();
    }
}

// Function to sonify an `image` provided by QImage and in mode `mode`
void
Sonification::Sonify(const QPixmap &pix, GV *gv, Traverse mode, int min,
                     int max) noexcept
{
    // Return if null
    if (pix.isNull())
        return;

    m_traverse = mode;

    SDL_zero(m_audioSpec);

    m_audioSpec.freq     = m_SampleRate;
    m_audioSpec.format   = SDL_AUDIO_S16;
    m_audioSpec.channels = m_ChannelCount;

    sonifier->setSampleRate(m_SampleRate);
    sonifier->setSamples(m_NumSamples);
    sonifier->setMinMax(min, max);

    m_audioStream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_audioSpec, audioCallback, this);

    if (mode == Traverse::PATH)
        sonifier->setParameters(pix, mode, gv->getPathDrawnPos());
    else if (mode == Traverse::INSPECT)
    {
        // TODO: Implement INSPECT mode
    }
    else
        sonifier->setParameters(pix, mode);

    if (!m_thread)
    {
        m_thread = new QThread();
        connect(m_thread, &QThread::started, sonifier, &Sonifier::Sonify);
        connect(m_thread, &QThread::finished, this, [&]() {});
        connect(sonifier, &Sonifier::sonificationDone, this,
                [&]() { m_thread->quit(); });
        sonifier->moveToThread(m_thread);
    }

    m_thread->start();
}

void
Sonification::pause() noexcept
{
    SDL_PauseAudioStreamDevice(m_audioStream);
}

void
Sonification::play() noexcept
{
    SDL_ResumeAudioStreamDevice(m_audioStream);
}

void
Sonification::reset() noexcept
{
    m_audioOffset = 0; // Reset the audio offset
    SDL_PauseAudioStreamDevice(m_audioStream);
}

// Save the audio data to file
bool
Sonification::save(const QString &filename, Format f) noexcept
{
    switch (f)
    {
        case Format::WAV:
            if (m_GenerateWavFile(filename))
                return true;
            break;

        case Format::MP3:
            break;
    }

    return false;
}

// Helper function to generate WAV file
bool
Sonification::m_GenerateWavFile(const QString &filename) noexcept
{
    SF_INFO sfinfo;
    sfinfo.samplerate = m_SampleRate;
    sfinfo.frames     = this->duration();
    sfinfo.format     = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    sfinfo.channels   = 1;

    SNDFILE *sndfile
        = sf_open(filename.toStdString().c_str(), SFM_WRITE, &sfinfo);

    if (!sndfile)
    {
        qDebug() << "Error writing to file: Error %d" << sf_error(sndfile);
        m_audioData.clear();
        return false;
    }

    sf_count_t count
        = sf_write_short(sndfile, m_audioData.data(), m_audioData.size());

    /*if (count != m_NumSamples)*/
    /*{*/
    /*    fprintf(stderr, "Error writing to file: Error %d",
     * sf_error(sndfile));*/
    /*    sf_close(sndfile);*/
    /*    m_audioData.clear();*/
    /*    return false;*/
    /*}*/

    sf_close(sndfile);

    return true;
}

double
Sonification::duration() noexcept
{
    return static_cast<double>(m_audioData.size()) / m_SampleRate;
}

void SDLCALL
Sonification::audioCallback(void *userdata, SDL_AudioStream *_stream,
                            int additional, int total) noexcept
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
    Sonification *s = static_cast<Sonification *>(userdata);

    if (s->m_audioData.empty())
        return;

    const int bytesPerSample = sizeof(short);
    const int totalBytes
        = static_cast<int>(s->m_audioData.size() * bytesPerSample);
    const int bytesRemaining = totalBytes - s->m_audioOffset;

    if (bytesRemaining <= 0)
    {
        // Audio is finished
        QMetaObject::invokeMethod(s, [s]() { emit s->audioFinishedPlaying(); },
                                  Qt::QueuedConnection);
        return;
    }

    if (bytesRemaining <= 0)
    {
        SDL_PauseAudioStreamDevice(s->m_audioStream);
        emit s->audioFinishedPlaying();
        return;
    }

    int bytesToCopy = std::min(additional, bytesRemaining);

    SDL_PutAudioStreamData(
        _stream,
        reinterpret_cast<const uint8_t *>(s->m_audioData.data())
            + s->m_audioOffset,
        bytesToCopy);

    s->m_audioOffset += bytesToCopy;

    int sampleIndex = s->m_audioOffset / (bytesPerSample * s->m_NumSamples);

    // Optional progress reporting
    int progress = static_cast<int>(s->m_audioOffset / bytesPerSample);
    QMetaObject::invokeMethod(s, [s, sampleIndex, progress]()
    {
        emit s->audioIndex(sampleIndex);
        emit s->audioProgress(progress);
    }, Qt::QueuedConnection);
}

void
Sonification::stopSonification(bool state) noexcept
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

void
Sonification::setFreqMap(FreqMap f) noexcept
{
    sonifier->setFreqMap(f);
}
