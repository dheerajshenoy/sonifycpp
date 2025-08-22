/*
 *
 * This contains mapping functions that is used to map the pixels of image to
 * create sound. There is also a function for generating sine waves
 *
 *
 *
 */

#include "mapping.hpp"

#include "sonifycpp/utils.hpp"
#include "utils_internal.hpp"

std::vector<short>
Mapping::Map__HSV(const std::vector<Pixel> &pixelCol) noexcept
{
    std::vector<short> fs;
    int N = static_cast<int>(pixelCol.size());
    fs.resize(N);
    std::vector<short> wave;
    double f = 0;

    using MapFunc = short (*)(double, double, double, double, double);

    MapFunc mapper = nullptr;
    switch (m_freq_map)
    {
        case FreqMap::Linear: mapper = utils::LinearMap; break;
        case FreqMap::Exp: mapper = utils::ExpMap; break;
        case FreqMap::Log: mapper = utils::LogMap; break;
    }

    for (int i = 0; i < N; i++)
    {
        QRgb rgb = utils_internal::toQRgb(pixelCol[i]);
        auto hsv = QColor(rgb).toHsv();
        f += mapper(0, 360, m_freq_min, m_freq_max, hsv.hue()) /
             static_cast<double>(N);
    }

    wave = utils::sineWave(0.5, f, 1, m_samplerate);
    fs   = utils::addVectors(fs, wave);
    return fs;
}

std::vector<short>
Mapping::Map__Intensity(const std::vector<Pixel> &pixelCol) noexcept
{
    const int N = static_cast<int>(pixelCol.size());
    if (N == 0) return {};

    using MapFunc  = short (*)(double, double, double, double, double);
    MapFunc mapper = nullptr;
    switch (m_freq_map)
    {
        case FreqMap::Linear: mapper = utils::LinearMap; break;
        case FreqMap::Exp: mapper = utils::ExpMap; break;
        case FreqMap::Log: mapper = utils::LogMap; break;
    }

    if (!mapper) return {};

    const double amplitude     = 0.5;
    const double duration      = 0.05; // seconds
    const int samples_per_wave = static_cast<int>(m_samplerate * duration);

    std::vector<short> result(samples_per_wave, 0);
    std::vector<short> wave;
    wave.reserve(samples_per_wave);

    for (const auto &px : pixelCol)
    {
        float intensity = static_cast<float>(qGray(utils_internal::toQRgb(px)));
        double freq     = mapper(0, 255, m_freq_min, m_freq_max, intensity);

        wave.clear();
        utils::generateSineWave(wave, amplitude, freq, duration, m_samplerate);
        utils::applyFadeInOut(
            wave); // optional but recommended for audio quality
        result = utils::addVectors(result, wave);
    }

    // Optional: normalize to prevent clipping
    utils::normalizeWave(result);
    return result;
}

std::vector<short>
Mapping::Map__Orchestra(const std::vector<Pixel> &pixelCol) noexcept
{
    const int N = static_cast<int>(pixelCol.size());
    std::vector<short> wave, tone;
    wave.resize(N); // rough preallocation assuming 100 samples per tone

    double freq = 0.0, amp = 0.0;
    const double alpha = 0.1; // smoothing factor

    using MapFunc = short (*)(double, double, double, double, double);

    MapFunc mapper = nullptr;

    switch (m_freq_map)
    {
        case FreqMap::Linear: mapper = utils::LinearMap; break;
        case FreqMap::Exp: mapper = utils::ExpMap; break;
        case FreqMap::Log: mapper = utils::LogMap; break;
    }

    for (int i = 0; i < N; ++i)
    {
        const QRgb rgb        = utils_internal::toQRgb(pixelCol[i]);
        const float intensity = static_cast<float>(qGray(rgb));
        const double target_freq =
            mapper(0.0, 255.0, m_freq_min, m_freq_max, intensity);
        const double target_amp = intensity / 255.0;

        freq += alpha * (target_freq - freq);
        amp += alpha * (target_amp - amp);

        utils::generateSineWave(tone, amp, freq, 0.05, m_samplerate);
        utils::applyEnvelope(tone);
        wave.insert(wave.end(), tone.cbegin(), tone.cend());
    }

    return wave;
}
