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
    double intensity = qGray(pixel) / 255.0;

    return generateSineWave(intensity, x, 1);
}


QVector<short> Mapping::Pentatonic(QRgb pixel, int x, int y)
{
    QColor col = QColor(pixel).toHsv();

    double intensity = qGray(pixel) / 255.0;


    /*if (intensity < 40)*/
    /*    return generateSineWave(0, 0, 0);*/

    auto s1 = generateSineWave(intensity, m_notes.getFrequency("C1"), 1);
    auto s2 = generateSineWave(intensity, m_notes.getFrequency("D2"), 1);
    /*auto s3 = generateSineWave(intensity, m_notes.getFrequency("E3"), 1);*/
    /*auto s4 = generateSineWave(intensity, m_notes.getFrequency("G4"), 1);*/
    /*auto s5 = generateSineWave(intensity, m_notes.getFrequency("A5"), 1);*/
    return utils::addVectors(s1, s2);

}

QVector<short> Mapping::Bells(QRgb pixel, int x, int y)
{
    QColor col = QColor(pixel).toHsv();

    double intensity = qGray(pixel) / 255.0;

    if (intensity < 150)
        return generateSineWave(0, 0, 0);

    auto s1 = generateSineWave(intensity, 814.0, 1);
    auto s2 = generateSineWave(intensity, 752.0, 1);
    auto s3 = generateSineWave(intensity, 600.0, 1);
    auto s4 = generateSineWave(intensity, 548.0, 1);
    auto s5 = generateSineWave(intensity, 400.0, 1);
    auto s6 = generateSineWave(intensity, 342.0, 1);
    auto s7 = generateSineWave(intensity, 238.0, 1);
    auto s8 = generateSineWave(intensity, 184.0, 1);
    auto s9 = generateSineWave(intensity, 112.0, 1);
    return utils::addVectors(s1, s2, s3, s4, s5, s6, s7, s8, s9);

}

QVector<short> Mapping::generateBellSound(double _amplitude, double frequency, double time)
{
    QVector<short> fs;
    int N = m_samplerate * time;
    fs.resize(N);
    double val = 2 * M_PI * frequency;
    for(int i=0; i < N; i++)
    {
        double t = static_cast<double>(i) / m_samplerate;
        double amplitude = std::exp(-3 * t) * _amplitude;
        double modulation = std::sin(2 * M_PI * 2 * t); // 2 Hz modulation frequency
        double sample = amplitude * std::sin(val * t + modulation);
        fs[i] = static_cast<short>(sample * 32767);
    }
    return fs;


}

QVector<short> Mapping::generateSineWave(double _amplitude, double frequency, double time)
{
    QVector<short> fs;
    int N = m_samplerate * time;
    fs.resize(N);
    int amplitude = _amplitude * 32767;
    double val = 2 * M_PI * frequency / m_samplerate;
    double angle = 0.0f;
    for(int i=0; i < N; i++)
    {
        fs[i] = amplitude * sin(angle);
        angle += val;
    }
    return fs;
}

QVector<short> Mapping::MapFull(QVector<PixelColumn> &pixelCol)
{

    QVector<short> fs;
    int N = pixelCol.size();
    fs.resize(N);
    PixelColumn p;

    for(int i=0; i < N; i++)
    {
        p = pixelCol[i];
        auto tmp = generateSineWave(qGray(p.pixel) / 255.0, p.y, 1);
        fs = utils::addVectors(fs, tmp);
    }

    return fs;
}

double Mapping::Hue2Freq(int H)
{
    double scale_freqs[] = { 220.00, 246.94 ,261.63, 293.66, 329.63, 349.23, 415.30 } ;
    int thresholds[] = { 26 , 52 , 78 , 104,  128 , 154 , 180 };
    auto note = scale_freqs[0];

    if (H <= thresholds[0])
        note = scale_freqs[0];

    else if(H > thresholds[0] && H <= thresholds[1])
        note = scale_freqs[1];

    else if(H > thresholds[1] && H <= thresholds[2])
        note = scale_freqs[2];

    else if(H > thresholds[2] && H <= thresholds[3])
        note = scale_freqs[3];

    else if(H > thresholds[3] && H <= thresholds[4])
        note = scale_freqs[4];

    else if(H > thresholds[4] && H <= thresholds[5])
        note = scale_freqs[5];

    else if (H > thresholds[5] && H <= thresholds[6])
        note = scale_freqs[6];

    else
        note = scale_freqs[0];

    return note;
}

QVector<short> Mapping::MapFull2(QVector<PixelColumn> &pixelCol)
{
    QVector<short> fs;
    int N = pixelCol.size();
    fs.resize(N);
    PixelColumn p;
    QVector<short> wave;
    double f=0;

    for(int i=0; i < N; i++)
    {
        p = pixelCol[i];
        f += p.y;
    }

    fs = generateBellSound(0.5, f, 1);
    return fs;
}
