#pragma once

#include <stdint.h>

typedef struct
{
    uint8_t r, g, b, a;
} RGBA;

typedef struct
{
    double h, s, v;
} HSV;

typedef struct
{
    RGBA rgba;
    int x, y;
} Pixel;
