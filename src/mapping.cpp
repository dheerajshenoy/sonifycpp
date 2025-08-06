/*
 *
 * This contains mapping functions that is used to map the pixels of image to
 * create sound. There is also a function for generating sine waves
 *
 *
 *
 */

#include "mapping.hpp"

#include <cmath>
#include <complex>

namespace
{
    short LinearMap(double inp_min, double inp_max, double out_min,
                    double out_max, double val)
    {
        return static_cast<short>(out_min
                                  + (val - inp_min) * (out_max - out_min)
                                        / (inp_max - inp_min));
    }

    short ExpMap(double inp_min, double inp_max, double out_min, double out_max,
                 double val)
    {
        double x_norm = (val - inp_min) / (inp_max - inp_min);
        double y_norm = std::pow(x_norm, 2);
        return static_cast<short>(y_norm * (out_max - out_min) + out_min);
    }

    short LogMap(double inp_min, double inp_max, double out_min, double out_max,
                 double val)
    {
        double x_norm = (val - inp_min) / (inp_max - inp_min);
        double y_norm = std::log(x_norm + 1) / std::log(10.0);
        return static_cast<short>(y_norm * (out_max - out_min) + out_min);
    }

    void generateSineWave(QVector<short> &vector, double _amplitude,
                          double frequency, double time,
                          float samplerate) noexcept
    {
        const unsigned int N = static_cast<unsigned int>(samplerate * time);
        if (N == 0)
            return;

        vector.resize(N);

        // clamp amplitude to avoid overflow
        double amp         = std::clamp(_amplitude, -1.0, 1.0);
        const double scale = amp * 32767.0;

        const double theta
            = 2.0 * M_PI * frequency / static_cast<double>(samplerate);

        // rotation complex number: e^{i θ}
        const std::complex<double> w(std::cos(theta), std::sin(theta));
        std::complex<double> z(0.0, 0.0);
        // start at angle zero: sin(0)=0, but we want first sample as sin(0),
        // so initialize z = (cos(0), sin(0)) = (1,0), then advance after using.
        z = std::complex<double>(1.0, 0.0);

        for (unsigned int i = 0; i < N; ++i)
        {
            vector[i] = static_cast<short>(scale * z.imag());
            z *= w; // advance phase
        }
    }

    QVector<short> generateSineWave(double _amplitude, double frequency,
                                    double time, float samplerate) noexcept
    {
        const unsigned int N = static_cast<unsigned int>(samplerate * time);
        if (N == 0)
            return {};

        QVector<short> fs;
        fs.resize(N);

        // clamp amplitude to avoid overflow
        double amp         = std::clamp(_amplitude, -1.0, 1.0);
        const double scale = amp * 32767.0;

        const double theta
            = 2.0 * M_PI * frequency / static_cast<double>(samplerate);
        // rotation complex number: e^{i θ}
        const std::complex<double> w(std::cos(theta), std::sin(theta));
        std::complex<double> z(0.0, 0.0);
        // start at angle zero: sin(0)=0, but we want first sample as sin(0),
        // so initialize z = (cos(0), sin(0)) = (1,0), then advance after using.
        z = std::complex<double>(1.0, 0.0);

        for (unsigned int i = 0; i < N; ++i)
        {
            double sample = z.imag(); // sin(current angle)
            fs[i]         = static_cast<short>(scale * sample);
            z *= w; // advance phase
        }

        return fs;
    }

    void applyEnvelope(QVector<short> &samples) noexcept
    {
        const int N       = static_cast<int>(samples.size());
        const int attack  = std::min(100, N / 10);
        const int release = std::min(100, N / 10);

        for (int i = 0; i < attack; ++i)
            samples[i] *= static_cast<float>(i) / attack;

        for (int i = 0; i < release; ++i)
            samples[N - 1 - i] *= static_cast<float>(i) / release;
    }

} // namespace

template <typename T>
QVector<T>
addTwoVectors(const QVector<T> &first, const QVector<T> &second) noexcept
{
    int maxSize = static_cast<int>(std::max(first.size(), second.size()));
    QVector<T> result(maxSize, T{});

    for (qsizetype i = 0; i < maxSize; ++i)
    {
        if (i < first.size())
        {
            result[i] += first[i];
        }
        if (i < second.size())
        {
            result[i] += second[i];
        }
    }

    return result;
}

template <typename T, typename... Args>
QVector<T>
addVectors(const QVector<T> &first, const QVector<T> &second,
           const Args &...args) noexcept
{
    QVector<T> result = addTwoVectors(first, second);

    if constexpr (sizeof...(args) > 0)
    {
        result = addVectors(result, args...);
    }

    return result;
}

double
Mapping::Hue2Freq(int H) noexcept
{
    constexpr double scale_freqs[]
        = {220.00, 246.94, 261.63, 293.66, 329.63, 349.23, 415.30};
    constexpr int thresholds[] = {26, 52, 78, 104, 128, 154, 180};
    auto note                  = scale_freqs[0];

    if (H <= thresholds[0])
        note = scale_freqs[0];

    else if (H > thresholds[0] && H <= thresholds[1])
        note = scale_freqs[1];

    else if (H > thresholds[1] && H <= thresholds[2])
        note = scale_freqs[2];

    else if (H > thresholds[2] && H <= thresholds[3])
        note = scale_freqs[3];

    else if (H > thresholds[3] && H <= thresholds[4])
        note = scale_freqs[4];

    else if (H > thresholds[4] && H <= thresholds[5])
        note = scale_freqs[5];

    else if (H > thresholds[5] && H <= thresholds[6])
        note = scale_freqs[6];

    else
        note = scale_freqs[0];

    return note;
}

QVector<short>
Mapping::Map__HSV(const QVector<PixelColumn> &pixelCol) noexcept
{
    QVector<short> fs;
    int N = static_cast<int>(pixelCol.size());
    fs.resize(N);
    PixelColumn p;
    QVector<short> wave;
    double f = 0;

    using MapFunc = short (*)(double, double, double, double, double);

    MapFunc mapper = nullptr;
    switch (m_freq_map)
    {
        case FreqMap::Linear:
            mapper = LinearMap;
            break;
        case FreqMap::Exp:
            mapper = ExpMap;
            break;
        case FreqMap::Log:
            mapper = LogMap;
            break;
    }

    for (int i = 0; i < N; i++)
    {
        auto hsv = QColor(pixelCol[i].pixel).toHsv();
        f += mapper(0, 360, m_freq_min, m_freq_max, hsv.hue())
             / static_cast<double>(N);
    }

    wave = generateSineWave(0.5, f, 1, m_samplerate);
    fs   = addVectors(fs, wave);
    return fs;
}

QVector<short>
Mapping::Map__Intensity(const QVector<PixelColumn> &pixelCol) noexcept
{
    QVector<short> fs;
    int N = static_cast<int>(pixelCol.size());
    fs.resize(N);
    QVector<short> wave;
    double f = 0.0;

    using MapFunc = short (*)(double, double, double, double, double);

    MapFunc mapper = nullptr;
    switch (m_freq_map)
    {
        case FreqMap::Linear:
            mapper = LinearMap;
            break;
        case FreqMap::Exp:
            mapper = ExpMap;
            break;
        case FreqMap::Log:
            mapper = LogMap;
            break;
    }

    for (int i = 0; i < N; i++)
    {
        float intensity = static_cast<float>(qGray(pixelCol[i].pixel)) / 255;
        f += mapper(0, 360, m_freq_min, m_freq_max, intensity)
             / static_cast<double>(N);
    }

    std::vector<double> list = {0, 0.2, 0.4, 0.6, 0.8, 1.0};
    /*auto s = list.at(rand() % list.size());*/
    wave = generateSineWave(0.5, f, 1, m_samplerate);
    fs   = addVectors(fs, wave);
    return fs;
}

QVector<short>
Mapping::Map__Orchestra(const QVector<PixelColumn> &pixelCol) noexcept
{
    const int N = static_cast<int>(pixelCol.size());
    QVector<short> wave, tone;
    wave.resize(N); // rough preallocation assuming 100 samples per tone

    double freq = 0.0, amp = 0.0;
    const double alpha = 0.1; // smoothing factor

    using MapFunc = short (*)(double, double, double, double, double);

    MapFunc mapper = nullptr;

    switch (m_freq_map)
    {
        case FreqMap::Linear:
            mapper = LinearMap;
            break;
        case FreqMap::Exp:
            mapper = ExpMap;
            break;
        case FreqMap::Log:
            mapper = LogMap;
            break;
    }

    for (int i = 0; i < N; ++i)
    {
        const float intensity = static_cast<float>(qGray(pixelCol[i].pixel));
        const double target_freq
            = mapper(0.0, 255.0, m_freq_min, m_freq_max, intensity);
        const double target_amp = intensity / 255.0;

        freq += alpha * (target_freq - freq);
        amp += alpha * (target_amp - amp);

        generateSineWave(tone, amp, freq, 0.05, m_samplerate);
        applyEnvelope(tone);
        wave += tone;
    }

    return wave;
}
