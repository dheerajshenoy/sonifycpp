#pragma once

#include <QImage>
#include <QString>
#include <QStringView>
#include <vector>

namespace utils
{

    // ------- Image processing -------
    QImage changeGamma(const QImage &image, int gamma) noexcept;
    QImage changeBrightness(const QImage &image, int delta) noexcept;
    QImage changeSaturation(const QImage &image, int delta) noexcept;
    QImage invertColor(const QImage &image) noexcept;
    QImage convertToGrayscale(const QImage &image) noexcept;
    QImage changeContrast(const QImage &image, int delta) noexcept;

    // ------- Math and mapping -------
    short LinearMap(double value, double in_min, double in_max, double out_min,
                    double out_max) noexcept;

    short ExpMap(double value, double in_min, double in_max, double out_min,
                 double out_max) noexcept;

    short LogMap(double value, double in_min, double in_max, double out_min,
                 double out_max) noexcept;

    // ------- Signal generation -------
    void generateSineWave(std::vector<short> &buffer, double amplitude,
                          double frequency, double time,
                          float samplerate) noexcept;

    std::vector<short> sineWave(double _amplitude, double frequency,
                                double time, float samplerate) noexcept;

    void applyEnvelope(std::vector<short> &samples) noexcept;

    void normalizeWave(std::vector<short> &wave) noexcept;

    void applyFadeInOut(std::vector<short> &wave, int fade = 10) noexcept;

    // ------- Utility -------
    double Hue2Freq(double hue) noexcept;

    int intensity(QRgb pixel) noexcept;

    std::vector<double> linspace(double start, double end, int num) noexcept;

    std::vector<short> readWAVFile(const QString &path) noexcept;

    // ------- Vector math (templates) -------
    template <typename T>
    std::vector<T> addTwoVectors(const std::vector<T> &first,
                                 const std::vector<T> &second) noexcept
    {
        std::vector<T> result;
        result.reserve(first.size());
        for (size_t i = 0; i < first.size(); ++i)
            result.push_back(first[i] + second[i]);
        return result;
    }

    // Base case: just return one vector
    template <typename T>
    std::vector<T> addVectors(const std::vector<T> &v) noexcept
    {
        return v;
    }

    template <typename T, typename... Args>
    std::vector<T> addVectors(const std::vector<T> &first,
                              const std::vector<T> &second,
                              const Args &...args) noexcept
    {
        return addVectors(addTwoVectors(first, second), args...);
    }

    // Helper function to generate WAV file
    bool generateWavFile(const QString &filename, int sampleRate, int duration,
                         const std::vector<short> &data) noexcept;

} // namespace utils
