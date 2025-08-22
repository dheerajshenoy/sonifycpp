#pragma once

#include "format.hpp"
#include "gv.hpp"
#include "sonifier.hpp"
#include "traverse.hpp"

#include <QDebug>
#include <QObject>
#include <QPixmap>
#include <QRgb>
#include <QThread>
#include <QThreadPool>
#include <QtMath>
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <cmath>

class Sonification : public QObject
{
    Q_OBJECT
public:

    Sonification() noexcept;
    ~Sonification() noexcept;
    void Sonify(const QPixmap &pix, GV *gv, const Sonifier::MapFunc &mapFunc,
                Traverse mode = Traverse::LEFT_TO_RIGHT, int min = 20,
                int max = 20000) noexcept;

    inline void setNumSamples(int nsamples) noexcept
    {
        m_NumSamples = nsamples;
        m_val        = M_PI2 * m_NumSamples / m_SampleRate;
    }

    inline void setAudioOffset(size_t offset) noexcept
    {
        m_audioOffset = offset;
    }

    struct PluginInstance
    {
        void *handle;
        MapTemplate *ptr;
    };

    inline Sonifier *sonifier() const noexcept { return m_sonifier; }

    double duration() noexcept;
    void play() noexcept;
    void pause() noexcept;
    void reset() noexcept;
    void clear() noexcept;
    bool save(const QString &filename, Format format = Format::WAV) noexcept;

    inline int numSamples() const noexcept { return m_NumSamples; }

    inline std::vector<short> &audioData() noexcept { return m_audioData; }

    inline size_t audioOffset() const noexcept { return m_audioOffset; }

    void stopSonification(bool state) noexcept;

    inline float sampleRate() const noexcept { return m_SampleRate; }

    void AddReverb() noexcept;

    inline void setAudioData(const std::vector<short> &data) noexcept
    {
        m_audioData = data;
    }

    // Adds custom mapping
    inline void addPluginInstance(const PluginInstance &p) noexcept
    {
        m_custom_mappings.push_back(p);
    }

    void setFreqMap(FreqMap f) noexcept;

    MapTemplate *mappingClass(const QString &mapName) const noexcept;

signals:
    void audioProgress(double);
    void audioIndex(int);
    void sonificationDone();
    void audioFinishedPlaying();
    void sonificationProgress(int);
    void sonificationStopped();

private:

    void m_GenerateSound() noexcept;
    std::vector<short> applyReverb(int delayTimeMs, float feedback) noexcept;

    static void audioCallback(void *userdata, SDL_AudioStream *stream,
                              int additional_amount, int total_amount) noexcept;

    float m_SampleRate{ 44100.0f };
    int m_ChannelCount{ 1 };
    double m_Duration;
    int m_NumSamples{ 1024 };
    std::vector<short> m_audioData;
    double m_val;

    SDL_AudioSpec m_audioSpec;
    SDL_AudioStream *m_audioStream;

    size_t m_audioOffset{ 0 };
    Traverse m_traverse{ Traverse::LEFT_TO_RIGHT };
    const double M_PI2{ 6.28318530718 };
    Mapping *mapping{ new Mapping() };
    Sonifier *m_sonifier{ nullptr };
    QThread *m_thread{ nullptr };
    std::vector<PluginInstance> m_custom_mappings;
};
