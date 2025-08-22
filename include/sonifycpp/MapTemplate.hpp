// File for creating custom mappings using shared objects
#pragma once

#include "pixel.hpp"
#include "utils.hpp"

#include <vector>

class MapTemplate
{
public:

    using FreqMapFunc = short (*)(double, double, double, double, double);

    virtual ~MapTemplate()                                         = default;
    virtual const char *name() const                               = 0;
    virtual std::vector<short> mapping(const std::vector<Pixel> &) = 0;

    inline void setMinFreq(int f) noexcept { _min_freq = f; }
    inline void setMaxFreq(int f) noexcept { _max_freq = f; }
    inline void setSampleRate(int f) noexcept { _sample_rate = f; }
    FreqMapFunc freq_map;

protected:

    int _min_freq{ 0 }, _max_freq{ 20000 }, _sample_rate{ 44100 };
};

// Factory function for plugins
extern "C"
{
    MapTemplate *create();
}
