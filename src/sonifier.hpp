#ifndef SONIFIER_HPP
#define SONIFIER_HPP

#include <qt6/QtGui/QPixmap>
#include <qt6/QtGui/QRgb>
#include <qt6/QtCore/QObject>
#include <qt6/QtCore/QVector>
#include "traverse.hpp"

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
    void setParameters(QPixmap &pix, Traverse t);
    void setParameters(QPixmap &pix, Traverse t, QVector<QPointF>);
    void setSampleRate(float SR);
    void setSamples(int nsamples);
    void LeftToRight();
    void RightToLeft();
    void TopToBottom();
    void BottomToTop();
    void CircleOutwards();
    void CircleInwards();
    void Clockwise();
    void AntiClockwise();
    void PathDrawn();
    void Sonify();
    void stopSonifying(bool state);

signals:
    void sonificationDone(QVector<short>);
    void sonificationProgress(int progress);

private:
    float m_SampleRate;
    int m_nsamples;
    Mapping *m_mapping = new Mapping();

    Traverse m_traverse;
    QPixmap m_pix;
    QVector<QPointF> m_pixpos;

    bool m_stop_sonifying = false;
};

#endif
