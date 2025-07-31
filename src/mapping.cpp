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

QVector<short>
Mapping::Map1(const double &amplitude, const int &y, const int &x) noexcept
{
    QVector<short> fs;
    return fs;
}

/*
template<typename T, typename... Args>
QVector<T> addVectors(const QVector<T>& first, const QVector<T>& second, const
Args&... args)
{
    if (first.isEmpty())
        return second;

    if (second.isEmpty())
        return first;

    size_t size = first.size();
    QVector<T> result(size, T{});

    for (size_t i = 0; i < size; ++i) {
        result[i] = first[i] + second[i];
    }

    if constexpr (sizeof...(args) > 0) {
        result = addVectors<T>(result, args...);
    }

    return result;
}
*/

template <typename T>
QVector<T>
addTwoVectors(const QVector<T> &first, const QVector<T> &second) noexcept
{
    size_t maxSize = std::max(first.size(), second.size());
    QVector<T> result(maxSize, T{});

    for (size_t i = 0; i < maxSize; ++i)
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

void
Mapping::setSamples(const int &samples) noexcept
{
    m_nsamples = samples;
}

void
Mapping::setFreqMap(const FreqMap &f) noexcept
{
    m_freq_map = f;
}

void
Mapping::setSampleRate(const float &samplerate) noexcept
{
    m_samplerate = samplerate;
}

void
Mapping::setMinMax(const int &min, const int &max) noexcept
{
    m_freq_min = min;
    m_freq_max = max;
}

#include <cmath>
#include <complex>

QVector<short>
Mapping::generateSineWave(const double &_amplitude, const double &frequency,
                          const double &time) noexcept
{
    const unsigned int N = static_cast<unsigned int>(m_samplerate * time);
    if (N == 0)
        return {};

    QVector<short> fs;
    fs.resize(N);

    // clamp amplitude to avoid overflow
    double amp         = std::clamp(_amplitude, -1.0, 1.0);
    const double scale = amp * 32767.0;

    const double theta
        = 2.0 * M_PI * frequency / static_cast<double>(m_samplerate);
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

double
Mapping::Hue2Freq(const int &H) noexcept
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

short
Mapping::LinearMap(const double &inp_min, const double &inp_max,
                   const double &out_min, const double &out_max,
                   const double &val) noexcept
{
    return out_min
           + (val - inp_min) * (out_max - out_min) / (inp_max - inp_min);
}

short
Mapping::ExpMap(const double &inp_min, const double &inp_max,
                const double &out_min, const double &out_max,
                const double &val) noexcept
{
    double x_norm = (val - inp_min) / (inp_max - inp_min);
    double y_norm = std::pow(x_norm, 2);
    return y_norm * (out_max - out_min) + out_min;
}

short
Mapping::LogMap(const double &inp_min, const double &inp_max,
                const double &out_min, const double &out_max,
                const double &val) noexcept
{
    double x_norm = (val - inp_min) / (inp_max - inp_min);
    double y_norm = std::log(x_norm + 1) / std::log(10.0);
    return y_norm * (out_max - out_min) + out_min;
}

QVector<short>
Mapping::Map__HSV(const QVector<PixelColumn> &pixelCol) noexcept
{
    QVector<short> fs;
    int N = pixelCol.size();
    fs.resize(N);
    PixelColumn p;
    QVector<short> wave;
    double f = 0;

    switch (m_freq_map)
    {
        case FreqMap::Linear:
            for (int i = 0; i < N; i++)
            {
                p        = pixelCol[i];
                auto hsv = QColor(p.pixel).toHsv();
                f += LinearMap(0, 360, m_freq_min, m_freq_max, hsv.hue())
                     / static_cast<double>(N);
            }
            break;

        case FreqMap::Exp:
            for (int i = 0; i < N; i++)
            {
                p        = pixelCol[i];
                auto hsv = QColor(p.pixel).toHsv();
                f += ExpMap(0, 360, m_freq_min, m_freq_max, hsv.hue())
                     / static_cast<double>(N);
            }
            break;

        case FreqMap::Log:
            for (int i = 0; i < N; i++)
            {
                p        = pixelCol[i];
                auto hsv = QColor(p.pixel).toHsv();
                f += LogMap(0, 360, m_freq_min, m_freq_max, hsv.hue())
                     / static_cast<double>(N);
            }
            break;
    }

    std::vector<double> list = {0, 0.2, 0.4, 0.6, 0.8, 1.0};
    /*auto s = list.at(rand() % list.size());*/
    wave = generateSineWave(0.5, f, 1);
    fs   = addVectors(fs, wave);
    return fs;
}

QVector<short>
Mapping::Map__Intensity(const QVector<PixelColumn> &pixelCol) noexcept
{
    QVector<short> fs;
    int N = static_cast<int>(pixelCol.size());
    fs.resize(N);
    PixelColumn p;
    QVector<short> wave;
    double f = 0;

    switch (m_freq_map)
    {
        case FreqMap::Linear:
            for (int i = 0; i < N; i++)
            {
                p               = pixelCol[i];
                float intensity = static_cast<float>(qGray(p.pixel)) / 255;
                f += LinearMap(0, 360, m_freq_min, m_freq_max, intensity)
                     / static_cast<double>(N);
            }
            break;

        case FreqMap::Exp:
            for (int i = 0; i < N; i++)
            {
                p               = pixelCol[i];
                float intensity = static_cast<float>(qGray(p.pixel)) / 255;
                f += ExpMap(0, 360, m_freq_min, m_freq_max, intensity)
                     / static_cast<double>(N);
            }
            break;

        case FreqMap::Log:
            for (int i = 0; i < N; i++)
            {
                p               = pixelCol[i];
                float intensity = static_cast<float>(qGray(p.pixel)) / 255;
                f += LogMap(0, 360, m_freq_min, m_freq_max, intensity)
                     / static_cast<double>(N);
            }
            break;
    }

    std::vector<double> list = {0, 0.2, 0.4, 0.6, 0.8, 1.0};
    /*auto s = list.at(rand() % list.size());*/
    wave = generateSineWave(0.5, f, 1);
    fs   = addVectors(fs, wave);
    return fs;
}

QVector<short>
Mapping::add(const QVector<PixelColumn> &pixelCol) noexcept
{
    QVector<short> fs;
    int N = pixelCol.size();
    PixelColumn p;
    double intensity = 0;
    QVector<QVector<short>> waves;
    waves.resize(N);
    fs.resize(waves[0].size());
    fs.fill(0);

    QVector<double> freqs = {100.0, 200.0, 300.0, 400.0};

    for (int i = 0; i < N; i++)
    {
        p         = pixelCol[i];
        intensity = QColor(p.pixel).toHsv().hue() / 360.0;
        /*intensity = qGray(p.pixel) / 255.0;*/
        waves[i] = generateSineWave(intensity / N, 200.0f, 1);
    }
    /*intensity /= N;*/

    for (const auto &wave : waves)
        fs = addVectors(fs, wave);

    // auto wave1 = generateSineWave(intensity / 7, 440.0, 1);
    // auto wave2 = generateSineWave(intensity * 0.5 / 7, 880.0, 1);
    // auto wave3 = generateSineWave(intensity * 0.5 / 7, 1320.0, 1);
    // auto wave4 = generateSineWave(intensity * 0.0513 / 7, 1760.0, 1);
    // auto wave5 = generateSineWave(intensity * 0.045 / 7, 2200.0, 1);
    // auto wave6 = generateSineWave(intensity * 0.061 / 7, 2640.0, 1);
    // auto wave7 = generateSineWave(intensity * 0.0168 / 7, 3080.0, 1);
    // fs         = addVectors(wave1, wave2, wave3, wave4, wave5, wave6, wave7);
    return fs;
}
