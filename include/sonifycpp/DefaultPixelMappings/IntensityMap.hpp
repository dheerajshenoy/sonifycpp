#pragma once

#include "sonifycpp/MapTemplate.hpp"
#include "sonifycpp/utils.hpp"

class IntensityMap : public MapTemplate
{
public:

    const char *name() const override { return "Intensity"; }

    std::vector<short>
    mapping(const std::vector<Pixel> &pixelCol) noexcept override
    {
        const int N = static_cast<int>(pixelCol.size());
        if (N == 0) return {};

        const double amplitude     = 0.25;
        const double duration      = 0.1; // seconds
        const int samples_per_wave = static_cast<int>(_sample_rate * duration);

        std::vector<short> result(samples_per_wave, 0);
        std::vector<short> wave;
        wave.reserve(samples_per_wave);

        for (const auto &px : pixelCol)
        {
            const double intensity = utils::intensity(px.rgba) / 255.0;
            const double freq =
                freq_map(0, 1000, _min_freq, _max_freq, intensity);

            utils::generateSineWave(wave, amplitude, freq, duration,
                                    _sample_rate);
            utils::applyFadeInOut(wave);
            result = utils::addVectors(result, wave);
        }

        // Optional: normalize to prevent clipping
        // utils::normalizeWave(result);
        return result;
    }
};
