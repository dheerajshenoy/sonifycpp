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
    QColor col = QColor(pixel).toHsv();
    QVector<short> res;

    // 0 to 360 : color
    auto h = col.hue();

    // 0 to 255 : shade
    auto s = col.saturation();

    // 0 to 255 : brightness
    auto v = col.value();

    QVector<short> c1;

    if (s < 50)
    {
        c1 = utils::addVectors(generateSineWave(v, h * m_notes.getFrequency("A4"), 0),
            generateSineWave(v, h * m_notes.getFrequency("B4"), 0));
    }
    else if ( s < 100)
    {
        c1 = utils::addVectors(generateSineWave(v, h * m_notes.getFrequency("C4"), 0),
            generateSineWave(v, h * m_notes.getFrequency("D4"), 0));
    }

    else if ( s < 150)
    {
        c1 = utils::addVectors(generateSineWave(v, h * m_notes.getFrequency("E4"), 0),
            generateSineWave(v, h * m_notes.getFrequency("F4"), 0));
    }

    else if ( s < 200)
    {
        c1 = utils::addVectors(generateSineWave(v, h * m_notes.getFrequency("A5"), 0),
            generateSineWave(v, h * m_notes.getFrequency("B5"), 0));
    }
    else
        c1 = generateSineWave(v, h * m_notes.getFrequency("C5"), 0);

    return c1;
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
