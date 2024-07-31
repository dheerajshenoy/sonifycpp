#ifndef PIXEL_COLUMN_HPP
#define PIXEL_COLUMN_HPP

#include <qt6/QtCore/QVector>
#include <qt6/QtGui/QRgb>

struct PixelColumn
{
    QRgb pixel;
    int x, y;
};

#endif
