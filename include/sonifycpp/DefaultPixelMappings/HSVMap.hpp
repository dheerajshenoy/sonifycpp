#pragma once

#include "sonifycpp/MapTemplate.hpp"

class HSVMap : public MapTemplate
{
public:

    const char *name() const override { return "HSV"; }

    std::vector<short>
    mapping(const std::vector<Pixel> &pixelCol) noexcept override
    {
        std::vector<short> fs;
        int N = static_cast<int>(pixelCol.size());
        fs.resize(N);
        std::vector<short> wave;
        double f = 0;

        for (const auto &px : pixelCol)
        {
            const HSV hsv = utils::RGBtoHSV(px.rgba);
            f += freq_map(0, 360, _min_freq, _max_freq, hsv.h) /
                 static_cast<double>(N);
        }

        wave = utils::sineWave(0.5, f, 1, _sample_rate);
        fs   = utils::addVectors(fs, wave);
        return fs;
    }
};
