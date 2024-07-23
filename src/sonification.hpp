#include <qt6/QtGui/QPixmap>
#include <sndfile.h>
#include <cmath>
#include "traverse.hpp"
#include "format.hpp"
#include <SDL2/SDL.h>

class Sonification
{

public:
    Sonification();
    void Sonify(QPixmap &pix, Traverse mode = Traverse::NORMAL);
    void play();
    void pause();
    void reset();
    bool save(QString filename, Format format = Format::WAV);
    double getDuration();
    ~Sonification();

private:

    void m_SonifyNormal(QPixmap &pix);
    double m_MapIntensityToFrequence(int intensity);
    void m_GenerateSound();
    bool m_GenerateWavFile(QString filename);
    QVector<double> m_GenerateSineWave(double amplitude, double frequency, double time);

    template <typename T>
    QVector<T> addVectors(QVector<T> &, QVector<T> &);

    static void sdlAudioCallback(void* userdata, Uint8* stream, int len);

    float m_SampleRate = 44100.0f;
    int m_ChannelCount = 2;

    int m_Duration, m_NumSamples;

    QVector<short> m_audioData;

    double m_val;


    SDL_AudioSpec m_wavSpec;
    SDL_AudioDeviceID m_audioDevice;
    size_t m_audioOffset = 0;

    bool m_is_sonified = false;
};
