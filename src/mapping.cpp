#include "mapping.hpp"
#include <cmath>


void Mapping::setSamples(int &samples)
{
    m_samples = samples;
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
    QVector<short> res;
    addVectors(res, r, g, b);
    return res;
}

QVector<short> Mapping::generateSineWave(double amplitude, double frequency, double time)
{
    QVector<int> d1 = { 1, 2, 3 };
    QVector<int> d2 = { 1, 2, 3 };
    QVector<int> d3 = { 1, 2, 3 };

    QVector<int> res;
    addVectors(res, d1, d2, d3);
    qDebug() << res;
    exit(0);
    fs.resize(m_samples);
    double val = 2 * M_PI * frequency * m_samples / m_samplerate;
    for(int i=0; i < m_samples; i++)
        fs[i] = amplitude * sin(val * i);
    return fs;
}

template <typename T>
void Mapping::addVectors(QVector<T> &res, QVector<T> &a, QVector<T> &b)
{
    if (a.length() != b.length()) return;

    for(int i=0; i < a.length(); i++)
    {
        res[i] = a[i] + b[i];
    }
}

template <typename T>
void Mapping::addVectors(QVector<T> &res)
{
}

template <typename T>
void Mapping::addVectors(QVector<T> &res, const QVector<T>& a) {
    res = a;
}

// Recursive case for multiple vectors
template <typename T, typename... Args>
void Mapping::addVectors(QVector<T> &res, const Args&... vecs)
{
    addVectors(res, vecs...);
}

