#pragma once

#include "sonifycpp/pixel.hpp"

#include <QImage>
#include <QString>
#include <QStringView>
#include <vector>

namespace utils_internal
{

    // ------- Image processing -------
    QImage changeGamma(const QImage &image, int gamma) noexcept;
    QImage changeBrightness(const QImage &image, int delta) noexcept;
    QImage changeSaturation(const QImage &image, int delta) noexcept;
    QImage invertColor(const QImage &image) noexcept;
    QImage convertToGrayscale(const QImage &image) noexcept;
    QImage changeContrast(const QImage &image, int delta) noexcept;

    std::vector<short> readWAVFile(const QString &path) noexcept;

    // Helper function to generate WAV file
    bool generateWavFile(const QString &filename, int sampleRate, int duration,
                         const std::vector<short> &data) noexcept;

    inline int intensity(QRgb pixel) noexcept
    {
        return qGray(pixel);
    }

    inline QRgb toQRgb(const Pixel &p) noexcept
    {
        return qRgba(p.rgba.r, p.rgba.g, p.rgba.b, p.rgba.a);
    }

    inline Pixel fromQRgb(const QRgb &qrgb, int x, int y) noexcept
    {
        return { RGBA{
                     static_cast<std::uint8_t>(qRed(qrgb)),
                     static_cast<std::uint8_t>(qGreen(qrgb)),
                     static_cast<std::uint8_t>(qBlue(qrgb)),
                     static_cast<std::uint8_t>(qAlpha(qrgb)),
                 },
                 x, y };
    }

} // namespace utils_internal
