#include <qt6/QtGui/QPixmap>
#include <sndfile.h>
#include <cmath>
#include "traverse.hpp"
#include "format.hpp"
#include <SDL2/SDL.h>
#include <qt6/QtCore/QtMath>
#include <qt6/QtGui/QRgb>

class Sonification
{

public:
    Sonification();
    void Sonify(QPixmap &pix, Traverse mode = Traverse::LEFT_TO_RIGHT);
    void setNumSamples(int nsamples);
    void play();
    void pause();
    void reset();
    bool save(QString filename, Format format = Format::WAV);
    double getDuration();
    ~Sonification();

private:

    void m_Sonify_LeftToRight(QPixmap &pix);
    void m_Sonify_RightToLeft(QPixmap &pix);
    void m_Sonify_TopToBottom(QPixmap &pix);
    void m_Sonify_BottomToTop(QPixmap &pix);
    void m_Sonify_CircleInwards(QPixmap &pix);
    void m_Sonify_CircleOutwards(QPixmap &pix);


    double m_MapIntensityToFrequence(int intensity);
    void m_GenerateSound();
    bool m_GenerateWavFile(QString filename);
    QVector<double> m_GenerateSineWave(double amplitude, double frequency, double time);

    template <typename T>
    QVector<T> addVectors(QVector<T> &, QVector<T> &);

    static void sdlAudioCallback(void* userdata, Uint8* stream, int len);

    float m_SampleRate = 44100.0f;
    int m_ChannelCount = 1;

    double m_Duration;

    int m_NumSamples = 1024;

    QVector<short> m_audioData;

    double m_val;
    SDL_AudioSpec m_wavSpec;
    SDL_AudioDeviceID m_audioDevice;
    size_t m_audioOffset = 0;

    Traverse m_traverse = Traverse::LEFT_TO_RIGHT;

};
