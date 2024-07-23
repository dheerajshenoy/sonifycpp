#include "sonification.hpp"
#include <cmath>
#define PI2 6.28318530718

Sonification::Sonification()
{
    m_val = 2 * M_PI / m_SampleRate;

    m_wavSpec.freq = 44100;
    m_wavSpec.format = AUDIO_S16LSB;
    m_wavSpec.channels = 1;
    m_wavSpec.samples = 1024;
    m_wavSpec.callback = sdlAudioCallback;
    m_wavSpec.userdata = this;

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

// Normal mode sonification helper function
// Vertical position to frequency, horizontal position to time
// brightness into amplitude
void Sonification::m_SonifyNormal(QPixmap &pix)
{
    QImage img = pix.toImage();
    double f = 0;
    for(int x=0; x < img.width(); x ++)
    {
        f = 0;
        for(int y=0; y < img.height(); y++)
        {
            QRgb pixel = img.pixel(x, y);
            double intensity = qGray(pixel);
            /*f += m_MapIntensityToFrequence(intensity);*/
            f += m_GenerateSineWave(intensity, y, x);
            m_audioData.push_back(f);
        }
    }
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
}


double Sonification::m_GenerateSineWave(double amplitude, double frequency, double time)
{
    return amplitude * std::sin(m_val * frequency * time);
}

double Sonification::m_MapIntensityToFrequence(int intensity)
{
    return 200.0 + (intensity * 800.0 / 255.0);
}

void Sonification::m_GenerateWavFile(QString filename)
{
    SF_INFO sfinfo;
    sfinfo.samplerate = m_SampleRate;
    m_NumSamples = m_audioData.size();
    sfinfo.frames = m_NumSamples;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    sfinfo.channels = 1;
    /*m_NumSamples = m_SampleRate * img.width();*/

    SNDFILE *sndfile = sf_open(filename.toStdString().c_str(), SFM_WRITE, &sfinfo);

    if (!sndfile) {
        fprintf(stderr, "Error writing to file: Error %d", sf_error(sndfile));
        m_audioData.clear();
    }

    sf_count_t count = sf_write_short(sndfile, m_audioData.data(), m_audioData.size());

    if (count != m_NumSamples)
    {
        fprintf(stderr, "Error writing to file: Error %d", sf_error(sndfile));
        sf_close(sndfile);
        m_audioData.clear();
    }

    sf_close(sndfile);

    m_audioData.clear();
}

double Sonification::getDuration()
{
    return static_cast<double>(static_cast<int>(m_audioData.size()) / m_SampleRate);
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
