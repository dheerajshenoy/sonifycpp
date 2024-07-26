#include "sonification.hpp"
#include <cmath>

Sonification::Sonification()
{

    m_val = M_PI2 * m_NumSamples / m_SampleRate;
    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        qDebug() << "SDL could not initialize! SDL_Error: " << SDL_GetError();
        return;
    }

    connect(sonifier, &Sonifier::sonificationDone, this, [&]() { emit sonificationDone(); });

}

Sonification::~Sonification()
{
    if (m_audioDevice) {
        SDL_CloseAudioDevice(m_audioDevice);
    }
    SDL_Quit();
}

void Sonification::setNumSamples(int nsamples)
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

    gv->setTraverse(mode);
    switch(mode)
    {
    
        case Traverse::LEFT_TO_RIGHT:
            sonifier->LeftToRight(pix, m_audioData);
            break;

        case Traverse::RIGHT_TO_LEFT:
            sonifier->RightToLeft(pix, m_audioData);
            break;

        case Traverse::TOP_TO_BOTTOM:
            sonifier->TopToBottom(pix, m_audioData);
            break;

        case Traverse::BOTTOM_TO_TOP:
            sonifier->BottomToTop(pix, m_audioData);
            break;

        case Traverse::CIRCLE_INWARDS:
            sonifier->CircleInwards(pix, m_audioData);
            break;

        case Traverse::CIRCLE_OUTWARDS:
            sonifier->CircleOutwards(pix, m_audioData);
            break;

        case Traverse::CLOCKWISE:
            sonifier->Clockwise(pix, m_audioData);
            break;

        case Traverse::ANTICLOCKWISE:
            sonifier->AntiClockwise(pix, m_audioData);
            break;

        case Traverse::PATH:
            break;
    }

}

/**/
/*void Sonification::SonifyPath(QPixmap &pix, QVector<QPointF> &pixelPos)*/
/*{*/
/*    if (!m_audioData.isEmpty())*/
/*        m_audioData.clear();*/
/*    QImage img = pix.toImage();*/
/*    for(int i=0; i < pixelPos.size(); i++)*/
/*    {*/
/*        auto pixelpos = pixelPos[i];*/
/*        auto x = pixelpos.x();*/
/*        auto y = pixelpos.y();*/
/*        int intensity = qGray(img.pixel(x, y));*/
/*        auto sine = m_GenerateSineWave(intensity * 1000, y, x);*/
/*        for(int j=0; j < sine.size(); j++)*/
/*            m_audioData.push_back(sine[j]);*/
/*    }*/
/**/
/*    SDL_zero(m_wavSpec);*/
/*    m_wavSpec.samples = m_NumSamples;*/
/*    m_wavSpec.freq = m_SampleRate;*/
/*    m_wavSpec.format = AUDIO_S16LSB;*/
/*    m_wavSpec.channels = m_ChannelCount;*/
/*    m_wavSpec.callback = sdlAudioCallback;*/
/*    m_wavSpec.userdata = this;*/
/**/
/*    sonifier->setSampleRate(m_SampleRate);*/
/*    sonifier->setSamples(m_NumSamples);*/
/**/
/*    if (m_audioDevice)*/
/*        SDL_CloseAudioDevice(m_audioDevice);*/
/*    m_audioDevice = SDL_OpenAudioDevice(nullptr, 0, &m_wavSpec, nullptr, 0);*/
/*    if (m_audioDevice == 0) {*/
/*        qDebug() << "Failed to open audio device: " << SDL_GetError();*/
/*        return;*/
/*    }*/
/*}*/

template <typename T>
QVector<T> Sonification::addVectors(QVector<T> &a, QVector<T> &b)
{
    if (a.length() != b.length()) return {};
    QVector<T> res;
    res.resize(a.length());

    for(int i=0; i < a.length(); i++)
    {
        res[i] = a[i] + b[i];
    }
    return res;
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



double Sonification::m_MapIntensityToFrequence(int intensity)
{
    return 200.0 + (intensity * 800.0 / 255.0);
}

bool Sonification::save(QString filename, Format f)
{
    switch(f)
    {
        case Format::WAV:
            if(m_GenerateWavFile(filename))
                return true;
            break;

        case Format::MP3:
            break;
    }

    return false;
}

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

double Sonification::getDuration()
{
    return m_audioData.size() / m_SampleRate;
}

void Sonification::sdlAudioCallback(void* userdata, Uint8* stream, int len)
{
    Sonification *s = static_cast<Sonification*>(userdata);

    int bytesToCopy = std::min(static_cast<int>(s->m_audioData.size() * sizeof(short) - s->m_audioOffset), len);
    
    memcpy(stream, s->m_audioData.data() + s->m_audioOffset / sizeof(short), bytesToCopy);

    s->m_audioOffset += bytesToCopy;
    emit s->audioprogress(static_cast<double>(s->m_audioOffset / (s->m_SampleRate * sizeof(short))));
    emit s->audioindex(static_cast<int>(s->m_audioOffset / (sizeof(short))));

    if (bytesToCopy < len) {
        memset(stream + bytesToCopy, 0, len - bytesToCopy);
        SDL_PauseAudioDevice(s->m_audioDevice, 1); // Stop audio playback
    }

    /*if (s->m_audioOffset >= s->m_audioData.size() * sizeof(short)) {*/
    /*    s->m_audioOffset = 0;*/
    /*    SDL_PauseAudioDevice(s->m_audioDevice, 1);*/
    /*}*/
}

int Sonification::getNumSamples()
{
    return m_NumSamples;
}

float Sonification::getSampleRate()
{
    return m_SampleRate;
}

QVector<short> Sonification::getAudioData()
{
    return m_audioData;
}
