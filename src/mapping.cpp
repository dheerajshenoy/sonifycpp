#include "mapping.hpp"
#include <cmath>


void Mapping::setSamples(int &samples)
{
    m_nsamples = samples;
}

void Mapping::setSampleRate(float &samplerate)
{
    m_samplerate = samplerate;
}

QVector<short> Mapping::Map1(QRgb pixel, int x, int y)
{
    double intensity = static_cast<double>(qGray(pixel));
    if (x < 20)
        x = 20;

    return generateSineWave(90 * intensity / 255, static_cast<double>(x), static_cast<double>(y));
}

QVector<short> Mapping::Map2(QRgb pixel, int x, int y)
{
    QColor col = QColor(pixel).toHsv();
    QVector<short> res;

    float frequencies[] = {440.0, 660.0, 880.0, 1100.0};
    float decayRates[] = {0.001, 0.002, 0.003, 0.004};

    double intensity = static_cast<double>(qGray(pixel));
    auto s1 = generateSineWave(intensity * std::exp(-decayRates[0] * 0.5), frequencies[0], static_cast<double>(y));
    auto s2 = generateSineWave(intensity * std::exp(-decayRates[1] * 0.5), frequencies[1], static_cast<double>(y));
    auto s3 = generateSineWave(intensity * std::exp(-decayRates[2] * 0.5), frequencies[2], static_cast<double>(y));
    auto s4 = generateSineWave(intensity * std::exp(-decayRates[3] * 0.5), frequencies[3], static_cast<double>(y));
    return utils::addVectors(s1, s2, s3, s4);
}

QVector<short> Mapping::generateSineWave(double amplitude, double frequency, double time)
{
    QVector <short> fs;
    fs.resize(m_nsamples);
    double val = 2 * M_PI * frequency * m_nsamples / m_samplerate;
    for(int i=0; i < m_nsamples; i++)
        fs[i] = amplitude * sin(val * i);
    return fs;
}
