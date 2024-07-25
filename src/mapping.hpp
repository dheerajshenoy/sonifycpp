#ifndef MAPPING_HPP
#define MAPPING_HPP

#include <qt6/QtCore/QVector>
#include <qt6/QtCore/QDebug>
#include <qt6/QtGui/QRgb>
#include <qt6/QtGui/QColor>

/*
* ####################################################################################################
*
* MAPPING FUNCTIONS
*
* ----------------------------------------------------------------------------------------------------
* Map1 = Convert to grayscale image then map (x, y) coordinate of the pixel to (time, frequency) and intensity of the pixel to
* amplitude
* 
* Map2 = take the (x, y) coordinate of the pixel 
*
*
*
*
*/

class Mapping
{

public:
    Mapping() {}
    ~Mapping() {}

    void setSampleRate(float &samplerate);
    void setSamples(int &samples);

    QVector<short> Map1(QRgb pixel, int x, int y);
    QVector<short> Map2(QRgb pixel, int x, int y);

private:
    QVector<short> generateSineWave(double amplitude, double frequency, double time);


    template <typename T>
    void addVectors(QVector<T> &res);

    template <typename T>
    void addVectors(QVector<T> &res, const QVector<T>& a);

    template <typename T>
    void addVectors(QVector<T> &res, QVector<T> &a, QVector<T> &b);

    template <typename T, typename... Args>
    void addVectors(QVector<T> &res, const Args&... vecs);
    
    double m_samplerate;
    int m_samples;

    QVector<short> fs;
};

#endif
