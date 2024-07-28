/*
*
* This contains mapping functions that is used to map the pixels of image to create sound.
* There is also a function for generating sine waves
*
*
*
*/
 
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
    /*if (x < 20)*/
    /*    x = 20;*/

    return generateSineWave(90 * intensity / 255, static_cast<double>(x * 20), static_cast<double>(y));
}

QVector<short> Mapping::Map2(QRgb pixel, int x, int y)
{
    QColor col = QColor(pixel).toHsv();
    QVector<short> res;

    double intensity = qGray(pixel);

    if (intensity < 40)
        return generateSineWave(0, 0, 0);
    if (intensity < 60)
    {
        auto s1 = generateSineWave(intensity, m_notes.getFrequency("C4"), 0);
        auto s2 = generateSineWave(intensity, m_notes.getFrequency("E4"), 0);
        auto s3 = generateSineWave(intensity, m_notes.getFrequency("G4"), 0);
        auto s4 = generateSineWave(intensity, m_notes.getFrequency("D4"), 0);
        return utils::addVectors(s1, s2, s3, s4);
    }

    if (intensity < 100)
    {
        auto s1 = generateSineWave(intensity, m_notes.getFrequency("C6"), 0);
        auto s2 = generateSineWave(intensity, m_notes.getFrequency("E6"), 0);
        auto s3 = generateSineWave(intensity, m_notes.getFrequency("G6"), 0);
        auto s4 = generateSineWave(intensity, m_notes.getFrequency("D6"), 0);
        return utils::addVectors(s1, s2, s3, s4);
    }

    auto s1 = generateSineWave(intensity, m_notes.getFrequency("G4"), 0);
    auto s2 = generateSineWave(intensity, m_notes.getFrequency("C4"), 0);
    auto s3 = generateSineWave(intensity, m_notes.getFrequency("D4"), 0);
    return utils::addVectors(s1, s2, s3);


}
QVector<short> Mapping::generateSineWave(double amplitude, double frequency, double time)
{
    QVector <short> fs;
    fs.resize(m_nsamples);
    double val = 2 * M_PI * frequency / m_samplerate;
    for(int i=0; i < m_nsamples; i++)
        fs[i] = amplitude * sin(val * i);
    return fs;
}
