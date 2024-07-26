#ifndef SONIFICATION_HPP
#define SONIFICATION_HPP

#include <qt6/QtGui/QPixmap>
#include <sndfile.h>
#include <cmath>
#include "traverse.hpp"
#include "format.hpp"
#include <SDL2/SDL.h>
#include <qt6/QtCore/QtMath>
#include <qt6/QtCore/QDebug>
#include <qt6/QtCore/QObject>
#include <qt6/QtConcurrent/QtConcurrent>
#include <qt6/QtGui/QRgb>
#include "gv.hpp"
#include "sonifier.hpp"

class Sonification : public QObject
{

    Q_OBJECT
public:
    Sonification();
    void Sonify(QPixmap &pix, GV *gv, Traverse mode = Traverse::LEFT_TO_RIGHT);
    void setNumSamples(int nsamples);
    void play();
    void pause();
    void reset();
    bool save(QString filename, Format format = Format::WAV);
    double getDuration();
    int getNumSamples();
    ~Sonification();
    QVector<short> getAudioData();
    float getSampleRate();
    /*void SonifyPath(QPixmap &pix, QVector<QPointF> &pixelPos);*/

signals:
    void audioprogress(double);
    void audioindex(int);
    void sonificationDone();

private:

    double m_MapIntensityToFrequence(int intensity);
    void m_GenerateSound();
    bool m_GenerateWavFile(QString filename);

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

    const double M_PI2 = 6.28318530718;
    Mapping *mapping = new Mapping();

    Sonifier *sonifier = new Sonifier();
};

#endif
