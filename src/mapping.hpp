/*
 *
 *   Mapping Header File
 *
 *
 */

#pragma once

#include "freqmap.hpp"
#include "notes.hpp"
#include "pixelColumn.hpp"
#include "utils.hpp"

#include <QColor>
#include <QDebug>
#include <QRgb>
#include <QVector>

class Mapping
{

public:
    Mapping() {}
    ~Mapping() {}

    enum class Type
    {
        INTENSITY = 0,
        HSV
    };

    void setSampleRate(const float &samplerate) noexcept;
    void setSamples(const int &samples) noexcept;
    void setMinMax(const int &min, const int &max) noexcept;
    void setFreqMap(const FreqMap &f) noexcept;

    QVector<short> Map1(const double &, const int &, const int &) noexcept;
    QVector<short> Map__Intensity(const QVector<PixelColumn> &) noexcept;
    QVector<short> Map__HSV(const QVector<PixelColumn> &) noexcept;
    QVector<short> Map__Orchestra(const QVector<PixelColumn> &) noexcept;
    QVector<short> add(const QVector<PixelColumn> &pix) noexcept;

private:
    double Hue2Freq(const int &H) noexcept;

    short LinearMap(const double &inp_min, const double &inp_max,
                    const double &out_min, const double &out_max,
                    const double &val) noexcept;

    short ExpMap(const double &inp_min, const double &inp_max,
                 const double &out_min, const double &out_max,
                 const double &val) noexcept;

    short LogMap(const double &inp_min, const double &inp_max,
                 const double &out_min, const double &out_max,
                 const double &val) noexcept;
    QVector<short> generateSineWave(const double &amplitude,
                                    const double &frequency,
                                    const double &time) noexcept;

    double sineWave(const double &_amplitude, const double &frequency,
                    const double &time) noexcept;

    void applySimpleEnvelope(QVector<short> &buf, double attackFrac = 0.1,
                             double releaseFrac = 0.1) noexcept;

    double m_samplerate;
    int m_nsamples;
    Notes m_notes;

    int m_freq_min, m_freq_max;
    FreqMap m_freq_map = FreqMap::Linear;

    QVector<double> m_harmonics = {1.0, 0.8, 0.6, 0.4, 0.3, 0.2};
};
