#ifndef TONEGENERATOR_HPP
#define TONEGENERATOR_HPP

#include "notes.hpp"
#include "qboxlayout.h"
#include <sndfile.h>
#include <qt6/QtCore/QVector>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QDialog>
#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtWidgets/QSpinBox>
#include <qt6/QtWidgets/QLabel>
#include <cmath>
#include <SDL2/SDL.h>

class ToneGenerator : public QDialog
{
    Q_OBJECT
public:
    ToneGenerator(QWidget *parent = nullptr);
    ~ToneGenerator();
    QVector<short> generateSineWave(double, double, double);
    static void sdlAudioCallback(void* userdata, Uint8* stream, int len);

signals:
    void audioFinishedPlaying();

private:
    /*Notes *m_notes = new Notes();*/
    QVBoxLayout *m_layout = new QVBoxLayout();
    QPushButton *m_play_btn = new QPushButton("Play");
    QLabel  *m_duration_label = new QLabel("Duration"),
            *m_freq_label = new QLabel("Frequency");
    QSpinBox    *m_duration_sb = new QSpinBox(),
                *m_freq_sb = new QSpinBox();

    QVector<short> m_audioData;
    int m_nsamples = 1024;
    float m_sampleRate = 44100.0f;
    int m_audioOffset = 0;

    SDL_AudioSpec m_wavSpec;
    SDL_AudioDeviceID m_audioDevice;

    bool m_playing = false;
};

#endif
