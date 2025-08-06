#pragma once

#include <QColor>
#include <QImage>
#include <QtMath>

namespace Utils
{
    inline QImage changeGamma(const QImage &input, int gammaValue)
    {
        QImage image = input.convertToFormat(QImage::Format_RGB32);
        double gamma = gammaValue / 100.0;

        for (int y = 0; y < image.height(); ++y)
        {
            QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
            {
                QColor c(line[x]);
                int r = qBound(0, int(qPow(c.red() / 255.0, gamma) * 255), 255);
                int g
                    = qBound(0, int(qPow(c.green() / 255.0, gamma) * 255), 255);
                int b
                    = qBound(0, int(qPow(c.blue() / 255.0, gamma) * 255), 255);
                line[x] = qRgb(r, g, b);
            }
        }

        return image;
    }

    inline QImage changeBrightness(const QImage &input, int delta)
    {
        QImage image = input.convertToFormat(QImage::Format_RGB32);

        for (int y = 0; y < image.height(); ++y)
        {
            QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
            {
                QColor c(line[x]);
                int r   = qBound(0, c.red() + delta, 255);
                int g   = qBound(0, c.green() + delta, 255);
                int b   = qBound(0, c.blue() + delta, 255);
                line[x] = qRgb(r, g, b);
            }
        }

        return image;
    }

    inline QImage changeSaturation(const QImage &input, int delta)
    {
        QImage image = input.convertToFormat(QImage::Format_RGB32);

        for (int y = 0; y < image.height(); ++y)
        {
            QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
            {
                QColor c(line[x]);
                int h, s, v;
                c.getHsv(&h, &s, &v);
                s = qBound(0, s + delta, 255);
                c.setHsv(h, s, v);
                line[x] = c.rgb();
            }
        }

        return image;
    }

    inline QImage invertColor(const QImage &input)
    {
        QImage image = input.convertToFormat(QImage::Format_RGB32);

        for (int y = 0; y < image.height(); ++y)
        {
            QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
            {
                QColor c(line[x]);
                line[x] = qRgb(255 - c.red(), 255 - c.green(), 255 - c.blue());
            }
        }

        return image;
    }

    inline QImage convertToGrayscale(const QImage &input)
    {
        QImage image = input.convertToFormat(QImage::Format_RGB32);

        for (int y = 0; y < image.height(); ++y)
        {
            QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
            {
                QColor c(line[x]);
                int gray = qGray(c.rgb());
                line[x]  = qRgb(gray, gray, gray);
            }
        }

        return image;
    }

    inline QImage changeContrast(const QImage &input, int contrastValue)
    {
        QImage image = input.convertToFormat(QImage::Format_RGB32);

        double factor = (259.0 * (contrastValue + 255.0))
                        / (255.0 * (259.0 - contrastValue));

        for (int y = 0; y < image.height(); ++y)
        {
            QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x)
            {
                QColor c(line[x]);
                int r   = qBound(0, int(factor * (c.red() - 128) + 128), 255);
                int g   = qBound(0, int(factor * (c.green() - 128) + 128), 255);
                int b   = qBound(0, int(factor * (c.blue() - 128) + 128), 255);
                line[x] = qRgb(r, g, b);
            }
        }

        return image;
    }

} // namespace Utils
