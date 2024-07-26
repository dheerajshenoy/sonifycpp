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
    return generateSineWave(intensity, static_cast<double>(x), static_cast<double>(y));
}

QVector<short> Mapping::Map2(QRgb pixel, int x, int y)
{
    QColor col = QColor(pixel);
    double intensity = qGray(col.rgb());
    auto r = generateSineWave(intensity, col.red(), 0);
    auto g = generateSineWave(intensity, col.green(), 0);
    auto b = generateSineWave(intensity, col.blue(), 0);
    return utils::addVectors(r, g, b);
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
