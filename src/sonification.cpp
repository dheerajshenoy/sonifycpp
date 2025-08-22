// Contains function for audio playback of the sonified data

#include "sonification.hpp"

#include "utils_internal.hpp"

#include <SDL3/SDL_audio.h>
#include <algorithm>
#include <dlfcn.h>
#include <immintrin.h>

// Constructor
Sonification::Sonification() noexcept
{
    m_val = M_PI2 * m_NumSamples / m_SampleRate;

    if (!SDL_Init(SDL_INIT_AUDIO))
    {
        qFatal() << "SDL could not initialize! SDL_Error: " << SDL_GetError();
        return;
    }

    m_sonifier = new Sonifier();

    connect(m_sonifier, &Sonifier::sonificationDone, this,
            [&](std::vector<short> audioData)
    {
        m_audioData = audioData;
        emit sonificationDone();
    });

    connect(m_sonifier, &Sonifier::sonificationProgress, this,
            [&](int progress) { emit sonificationProgress(progress); });
}

// Destructor
Sonification::~Sonification() noexcept
{

    SDL_PauseAudioStreamDevice(m_audioStream);
    SDL_DestroyAudioStream(m_audioStream);
    SDL_Quit();

    if (m_thread && m_thread->isRunning())
    {
        m_sonifier->stopSonifying(true);
        m_thread->quit();
        m_thread->wait();
    }

    deleteCustomMappings();
}

// Function to sonify an `image` provided by QImage and in mode `mode`
void
Sonification::Sonify(const QPixmap &pix, GV *gv,
                     const Sonifier::MapFunc &mapFunc, Traverse mode, int min,
                     int max) noexcept
{
    // Return if null
    if (pix.isNull()) return;

    m_traverse = mode;

    SDL_zero(m_audioSpec);

    m_audioSpec.freq     = m_SampleRate;
    m_audioSpec.format   = SDL_AUDIO_S16;
    m_audioSpec.channels = m_ChannelCount;

    m_sonifier->setMapFunc(mapFunc);
    m_sonifier->setSampleRate(m_SampleRate);
    m_sonifier->setSamples(m_NumSamples);
    m_sonifier->setMinMax(min, max);

    m_audioStream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_audioSpec, audioCallback, this);

    if (mode == Traverse::PATH)
        m_sonifier->setParameters(pix, mode, gv->getPathDrawnPos());
    else if (mode == Traverse::INSPECT)
    {
        // TODO: Implement INSPECT mode
    }
    else
        m_sonifier->setParameters(pix, mode);

    if (!m_thread)
    {
        m_thread = new QThread();
        connect(m_thread, &QThread::started, m_sonifier, &Sonifier::Sonify);
        connect(m_thread, &QThread::finished, this, [&]() {});
        connect(m_sonifier, &Sonifier::sonificationDone, this,
                [&]() { m_thread->quit(); });
        m_sonifier->moveToThread(m_thread);
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
            if (utils_internal::generateWavFile(filename, m_SampleRate,
                                                duration(), m_audioData))
                return true;
            break;

        case Format::MP3:
            // TODO: Implement MP3 saving
            break;
    }

    return false;
}

double
Sonification::duration() noexcept
{
    return static_cast<double>(m_audioData.size()) / m_SampleRate;
}

void SDLCALL
Sonification::audioCallback(void *userdata, SDL_AudioStream *_stream,
                            int additional, int /* total */) noexcept
{
    Sonification *s = static_cast<Sonification *>(userdata);

    if (s->m_audioData.empty()) return;

    const int bytesPerSample = sizeof(short);
    const int totalBytes =
        static_cast<int>(s->m_audioData.size() * bytesPerSample);
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
        reinterpret_cast<const uint8_t *>(s->m_audioData.data()) +
            s->m_audioOffset,
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
    m_sonifier->stopSonifying(state);
    if (state)
    {
        if (m_thread && m_thread->isRunning())
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
    m_sonifier->setFreqMap(f);
}

void
Sonification::clear() noexcept
{
    SDL_PauseAudioStreamDevice(m_audioStream);
    SDL_ClearAudioStream(m_audioStream);
    m_audioData.clear();
    m_audioOffset = 0;
}

// Return the MapFunc class associated with map `mapName`
MapTemplate *
Sonification::mappingClass(const char *mapName) const noexcept
{
    auto it = std::find_if(m_custom_mappings.cbegin(), m_custom_mappings.cend(),
                           [&mapName](const PluginInstance &p)
    { return p.ptr->name() == mapName; });

    if (it != m_custom_mappings.end()) { return it->ptr; }

    return nullptr;
}

void
Sonification::deleteCustomMappings() noexcept
{
    if (!m_custom_mappings.empty())
    {
        for (const auto &m : m_custom_mappings)
        {
            delete m.ptr;
            dlclose(m.handle);
        }
    }
}
