#ifndef MAPPING_HPP
#define MAPPING_HPP

#include <qt6/QtCore/QVector>
#include <qt6/QtCore/QDebug>
#include <qt6/QtGui/QRgb>
#include <qt6/QtGui/QColor>
#include "utils.hpp"
#include "notes.hpp"

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


    double m_samplerate;
    int m_nsamples;
    Notes m_notes;
};

#endif
