#pragma once

#include <QPixmap>
#include <sndfile.h>
#include <cmath>
#include "traverse.hpp"
#include "format.hpp"
#include <SDL2/SDL.h>
#include <QtMath>
#include <QDebug>
#include <QObject>
#include <QThreadPool>
#include <QThread>
#include <QRgb>
#include "gv.hpp"
#include "sonifier.hpp"

class Sonification : public QObject
{

    Q_OBJECT
public:
    Sonification() noexcept;
    ~Sonification() noexcept;
    void Sonify(const QPixmap &pix, GV *gv, const Traverse& mode = Traverse::LEFT_TO_RIGHT,
                const int& min = 20, const int& max = 20000) noexcept;
    void setNumSamples(const int& nsamples) noexcept;
    void play() noexcept;
    void pause() noexcept;
    void reset() noexcept;
    bool save(const QString& filename, const Format& format = Format::WAV) noexcept;
    double getDuration() noexcept;
    int getNumSamples() noexcept;
    QVector<short>& getAudioData() noexcept;
    void stopSonification(const bool& state) noexcept;
    int& getSampleRate() noexcept;
    void AddReverb() noexcept;
    void setAudioData(const QVector<short>& data) noexcept;
    void setFreqMap(const FreqMap& f) noexcept;

signals:
    void audioprogress(double);
    void audioindex(int);
    void sonificationDone();
    void audioFinishedPlaying();
    void sonificationProgress(int);
    void sonificationStopped();

private:
    void m_GenerateSound() noexcept;
    bool m_GenerateWavFile(const QString& filename) noexcept;
    QVector<short> applyReverb(const int& delayTimeMs, const float& feedback) noexcept;

    template <typename T>
    QVector<T> addVectors(QVector<T> &, QVector<T> &) noexcept;
    static void sdlAudioCallback(void* userdata, Uint8* stream, int len) noexcept;

    int m_SampleRate = 44100;
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
    Sonifier *sonifier = nullptr;
    QThread *m_thread = nullptr;
};
