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
