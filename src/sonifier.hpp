#ifndef SONIFIER_HPP
#define SONIFIER_HPP

#include <qt6/QtGui/QPixmap>
#include <qt6/QtGui/QPixmap>
#include <qt6/QtCore/QObject>
#include <qt6/QtCore/QVector>

#ifdef __linux__
    #include <alsa/asoundlib.h>
#elif __APPLE__
    #include <CoreAudio/CoreAudio.h>
#elif _WIN32
    #include <windows.h>
#endif
#include "mapping.hpp"
#include "utils.hpp"

class Sonifier : public QObject
{
    Q_OBJECT
public:
    Sonifier() {}
    ~Sonifier() {}
    void setSampleRate(float SR);
    void setSamples(int nsamples);
    void LeftToRight(QPixmap &pix, QVector<short> &audioData);
    void RightToLeft(QPixmap &pix, QVector<short> &audioData);
    void TopToBottom(QPixmap &pix, QVector<short> &audioData);
    void BottomToTop(QPixmap &pix, QVector<short> &audioData);
    void CircleOutwards(QPixmap &pix, QVector<short> &audioData);
    void CircleInwards(QPixmap &pix, QVector<short> &audioData);
    void Clockwise(QPixmap &pix, QVector<short> &audioData);
    void AntiClockwise(QPixmap &pix, QVector<short> &audioData);
    void PathDrawn(QVector<QPointF> &pixelPos, QPixmap &pix, QVector<short> &audioData);

signals:
    void sonificationDone();

private:
    float m_SampleRate;
    int m_nsamples;
    Mapping *m_mapping = new Mapping();


};

#endif
