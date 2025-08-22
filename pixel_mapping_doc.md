# SonifyCPP Custom Mapping Plugin Documentation

This guide explains how to create custom mapping plugins for SonifyCPP using the `MapTemplate` interface.

> [!WARNING]
> This section assumes that you have good knowledge of C++
> If not, you can check the below resources/topics:
>
> Class Inheritance
> Shared object file generation
> [Microsoft C++ class Inheritance](https://learn.microsoft.com/en-us/cpp/cpp/inheritance-cpp?view=msvc-170)

## Overview

SonifyCPP allows users to create audio mappings from pixel data by writing shared library plugins (.so). Each plugin must implement the abstract MapTemplate interface, which defines:

`name()` – a descriptive name for the mapping

`mapping()` – the core function that converts a vector of Pixel into audio frequencies

> [!NOTE]
> The mappings file should be named `<name>.so`
>
> Mappings should be inside `~/.config/sonifycpp/mappings/`

## Base Class `MapTemplate`

```cpp
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

    // These are automatically set by the host; do not modify in your plugin
    inline void setMinFreq(int f) noexcept { _min_freq = f; }
    inline void setMaxFreq(int f) noexcept { _max_freq = f; }
    inline void setSampleRate(int f) noexcept { _sample_rate = f; }


protected:
    FreqMapFunc freq_map;

    int _min_freq{ 0 }, _max_freq{ 20000 }, _sample_rate{ 44100 };
};

// Factory function for plugins
extern "C"
{
    MapTemplate *create();
}
```

### Key Points

- `freq_map` – a function pointer used to map values to frequencies

- `_min_freq`, `_max_freq`, `_sample_rate` – parameters from the UI

> [!NOTE]
> Do not change `_min_freq`, `_max_freq`, `_sample_rate`, or `freq_map`.


## Creating a Custom Plugin

1. Derive from `MapTemplate` (defined in <sonifycpp/MapTemplate.hpp>)
2. Implement `name()` - descriptive name for your mapping
3. Implement `mapping()` - convert std::vector<Pixel> into std::vector<short> audio data
4. Export factory function: `create()`

Example:

```cpp
#include <sonifycpp/MapTemplate.hpp>
#include <sonifycpp/pixel.hpp>
#include <sonifycpp/utils.hpp>

class MyMapper : public MapTemplate {
public:
    const char* name() const override {
        return "My Custom Mapping";
    }

    std::vector<short> mapping(const std::vector<Pixel>& pixelCol) override {
        std::vector<short> fs(pixelCol.size(), 0);
        double f = 0;

        for (const auto &px : pixelCol) {
            HSV hsv = utils::RGBtoHSV(px.rgba);
            f += freq_map(0, 360, _min_freq, _max_freq, hsv.h) / pixelCol.size();
        }

        auto wave = utils::sineWave(0.5, f, 1, _sample_rate);
        fs = utils::addVectors(fs, wave);
        return fs;
    }
};

// Factory functions for dynamic loading
extern "C" MapTemplate* create() { return new MyMapper(); }
```

## Compiling the Plugin

Compile your plugin as a shared object (`.so`):

```bash
g++ -fPIC -shared mapper.cpp -o mapper.so
```

`-fPIC` – required for shared objects

`-shared` – build as a .so file

## Convenience Functions (utils namespace)

SonifyCPP provides a `utils` namespace (defined in <sonifycpp/utils.hpp>) with helpful functions to simplify plugin development:

### Math / Mapping Functions

`LinearMap(value, in_min, in_max, out_min, out_max)` – linear scaling

`ExpMap(value, in_min, in_max, out_min, out_max)` – exponential scaling

`LogMap(value, in_min, in_max, out_min, out_max)` – logarithmic scaling

### Signal Generation

`sineWave(amplitude, frequency, time, samplerate)` – generate a sine wave

`generateSineWave(buffer, amplitude, frequency, time, samplerate)` – fill an existing buffer

`applyEnvelope(samples)` – apply amplitude envelope

`normalizeWave(wave)` – normalize wave to max amplitude

`applyFadeInOut(wave, fade)` – smooth fade in/out

### Utility Functions

`Hue2Freq(hue)` – convert hue (0–360) to frequency

`linspace(start, end, num)` – generate linearly spaced numbers

`RGBtoHSV(rgba)` – convert RGBA color to HSV

### Vector Utilities

`addTwoVectors(first, second)` – element-wise addition of two vectors

`addVectors(v1, v2, v3, ...)` – element-wise addition of multiple vectors (variadic template)
