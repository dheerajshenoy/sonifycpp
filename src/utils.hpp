#pragma once

#include <QColor>
#include <QImage>
#include <QVector>
#include <QtMath>

namespace Utils
{

    static QImage changeGamma(QImage &image, int _value, int h, int w)
    {
        double value = _value / 100.0;
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                QColor color = image.pixelColor(x, y);

                int red = qBound(
                    0, static_cast<int>(qPow(color.red() / 255.0, value) * 255),
                    255);
                int green = qBound(
                    0,
                    static_cast<int>(qPow(color.green() / 255.0, value) * 255),
                    255);
                int blue = qBound(
                    0,
                    static_cast<int>(qPow(color.blue() / 255.0, value) * 255),
                    255);

                image.setPixelColor(x, y, QColor(red, green, blue));
            }
        }
        return image;
    }

    static QImage changeBrightness(QImage &image, int value, int h, int w)
    {
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                QColor color = image.pixelColor(x, y);
                int red      = color.red() + value;
                int green    = color.green() + value;
                int blue     = color.blue() + value;

                // Clamp values to the 0-255 range
                red   = qBound(0, red, 255);
                green = qBound(0, green, 255);
                blue  = qBound(0, blue, 255);

                image.setPixelColor(x, y, QColor(red, green, blue));
            }
        }
        return image;
    }

    static QImage changeSaturation(QImage &image, int value, int h, int w)
    {
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                QColor color = image.pixelColor(x, y);
                int h, s, v;
                color.getHsv(&h, &s, &v);

                s = qBound(0, s + value,
                           255); // Adjust saturation and clamp to [0, 255]

                color.setHsv(h, s, v);
                image.setPixelColor(x, y, color);
            }
        }
        return image;
    }

    static QImage invertColor(QImage &img, int h, int w)
    {
        for (int i = 0; i < w; i++)
            for (int j = 0; j < h; j++)
            {
                QColor color = img.pixelColor(i, j);
                img.setPixelColor(i, j,
                                  QColor(255 - color.red(), 255 - color.green(),
                                         255 - color.blue()));
            }

        return img;
    }

    static QImage convertToGrayscale(QImage &img, int h, int w)
    {
        for (int i = 0; i < w; i++)
            for (int j = 0; j < h; j++)
            {
                QColor color = img.pixelColor(i, j);
                int gray     = qGray(color.rgb());
                img.setPixelColor(i, j, QColor(gray, gray, gray));
            }
        return img;
    }

    // contrast factor 0.0 to 2.0
    static QImage changeContrast(QImage &image, int _value, int h, int w)
    {
        double value = (259.0 * (_value + 255.0)) / (255.0 * (259.0 - _value));
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                QColor color = image.pixelColor(x, y);

                int red = qBound(
                    0, static_cast<int>((value * (color.red() - 128)) + 128),
                    255);
                int green = qBound(
                    0, static_cast<int>((value * (color.green() - 128)) + 128),
                    255);
                int blue = qBound(
                    0, static_cast<int>((value * (color.blue() - 128)) + 128),
                    255);

                image.setPixelColor(x, y, QColor(red, green, blue));
            }
        }

        return image;
    }

} // namespace Utils
