#include "sonification.hpp"
#include <cmath>
#define PI2 6.28318530718

Sonification::Sonification()
{

    m_NumSamples = 100;
    m_wavSpec.freq = 44100;
    m_wavSpec.format = AUDIO_S16LSB;
    m_wavSpec.channels = 1;
    m_wavSpec.samples = m_NumSamples;
    m_wavSpec.callback = sdlAudioCallback;
    m_wavSpec.userdata = this;
    m_val = 2 * M_PI * m_NumSamples / m_SampleRate;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return;
    }

    m_audioDevice = SDL_OpenAudioDevice(nullptr, 0, &m_wavSpec, nullptr, 0);
    if (m_audioDevice == 0) {
        fprintf(stderr, "Failed to open audio device: %s", SDL_GetError());
        return;
    }
}

Sonification::~Sonification()
{
    if (m_audioDevice) {
        SDL_CloseAudioDevice(m_audioDevice);
    }
    SDL_Quit();
}

// Function to sonify an `image` provided by QImage and in mode `mode`
void Sonification::Sonify(QPixmap &pix, Traverse mode)
{

    if (m_is_sonified) return;
    // Return if null
    if (pix.isNull()) return;
    
    switch(mode)
    {
    
        case Traverse::NORMAL:
            m_SonifyNormal(pix);
        break;

    }

    m_is_sonified = true;
}

QVector<double> Sonification::m_GenerateSineWave(double amplitude, double frequency, double time)
{
    QVector<double> fs;
    fs.resize(m_NumSamples);
    double val = m_val * frequency;
    for(int i=0; i < m_NumSamples; i++)
        fs[i] = amplitude * sin(val * i);
    return fs;
}

// Normal mode sonification helper function
// Vertical position to frequency, horizontal position to time
// brightness into amplitude
void Sonification::m_SonifyNormal(QPixmap &pix)
{
    QImage img = pix.toImage();
    QVector<double> temp;
    for(int x=0; x < img.width(); x++)
    {
        temp.clear();
        temp.resize(m_NumSamples);
        for(int y=0; y < img.height(); y++)
        {
            QRgb pixel = img.pixel(x, y);
            double intensity = qGray(pixel);
            auto sine = m_GenerateSineWave(intensity, y, x);
            fprintf(stderr, "%d", sine.length());
            fprintf(stderr, "%d", temp.length());
            temp = addVectors<double>(temp, sine);
        }

        for(int i=0; i < temp.size(); i++)
            m_audioData.push_back(temp.at(i));
    }

}

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


/*double Sonification::m_GenerateSineWave(double amplitude, double frequency, double time)*/
/*{*/
/*    return amplitude * std::sin(m_val * frequency * time);*/
/*}*/

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
        fprintf(stderr, "Error writing to file: Error %d", sf_error(sndfile));
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
    if (bytesToCopy < len) {
        memset(stream + bytesToCopy, 0, len - bytesToCopy);
        SDL_PauseAudioDevice(s->m_audioDevice, 1); // Stop audio playback
    }
}
