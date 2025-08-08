/*
 *
 *   Mapping Header File
 *
 *
 */

#pragma once

#include "freqmap.hpp"
#include "pixel.hpp"
#include "utils.hpp"

#include <QColor>
#include <QDebug>
#include <QRgb>

class Mapping {

public:
    Mapping() {}
    ~Mapping() {}

    inline void setSamples(int samples) noexcept {
        m_nsamples = samples;
    }

    inline void setFreqMap(const FreqMap &f) noexcept {
        m_freq_map = f;
    }

    inline void setSampleRate(float samplerate) noexcept {
        m_samplerate = samplerate;
    }

    inline void setMinMax(int min, int max) noexcept {
        m_freq_min = min;
        m_freq_max = max;
    }

    std::vector<short> Map__Intensity(const std::vector<Pixel> &) noexcept;
    std::vector<short> Map__HSV(const std::vector<Pixel> &) noexcept;
    std::vector<short> Map__Orchestra(const std::vector<Pixel> &) noexcept;
    std::vector<short> add(const std::vector<Pixel> &pix) noexcept;

private:
    float m_samplerate;
    int m_nsamples;

    int m_freq_min, m_freq_max;
    FreqMap m_freq_map = FreqMap::Linear;

    std::vector<double> m_harmonics = {1.0, 0.8, 0.6, 0.4, 0.3, 0.2};
};
