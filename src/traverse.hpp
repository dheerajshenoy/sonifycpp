#pragma once

enum class Traverse : int
{
    LEFT_TO_RIGHT = 0,
    RIGHT_TO_LEFT,
    TOP_TO_BOTTOM,
    BOTTOM_TO_TOP,
    CIRCLE_OUTWARDS,
    CIRCLE_INWARDS,
    CLOCKWISE,
    ANTICLOCKWISE,
    PATH,
    REGION,
    INSPECT,
};
