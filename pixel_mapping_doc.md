# SonifyCPP Custom Mapping Plugin Documentation

This guide explains how to create custom mapping plugins for SonifyCPP using the `MapTemplate` interface.

> [!NOTE]
> This section assumes that you have some knowledge of C++

## Overview

SonifyCPP allows users to create audio mappings from pixel data by writing shared library plugins (.so). Each plugin must implement the abstract MapTemplate interface, which defines:

`name()` – a descriptive name for the mapping

`mapping()` – the core function that converts a vector of Pixel into audio frequencies

Plugins are loaded dynamically at runtime using `dlopen` and `dlsym` functions from `<dlfcn.h>` C standard library.

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

## Creating a Custom Plugin

Derive from `MapTemplate` (defined in <sonifycpp/MapTemplate.hpp>)

Implement `name()` - descriptive name for your mapping

Implement `mapping()` - convert std::vector<Pixel> into std::vector<short> audio data

Export factory function: `create()`

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
g++ -fPIC -shared MyMapper.cpp -I/path/to/sonifycpp/include -o MyMapper.so
```

`-fPIC` – required for shared objects
`-shared` – build as a .so file

# Notes for Plugin Authors

> [!NOTE]
>
> Do not change _min_freq, _max_freq, _sample_rate, or freq_map.
>
> Focus on the mapping logic in mapping().
>
> The host ensures consistent frequency ranges and mapping functions.
