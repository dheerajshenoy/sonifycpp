#pragma once

#include "traverse.hpp"

#include <array>
#include <string>

struct Config
{
    int num_samples{ 1024 };
    int min_freq{ 0 };
    int max_freq{ 20000 };
    int img_height{ 100 };
    int img_width{ 100 };
    std::array<int, 2> freq_range{ 0, 20000 };
    Traverse traversal{ Traverse::LEFT_TO_RIGHT };
    float sample_rate{ 44100.0f };
    std::string panel_position{ "left" };
    std::string pointer_color{ "#FF5000" };
};
