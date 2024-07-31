#ifndef TONEGENERATOR_HPP
#define TONEGENERATOR_HPP

#include "notes.hpp"
#include "qboxlayout.h"
#include <sndfile.h>
#include <qt6/QtCore/QVector>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QDialog>
#include <qt6/QtWidgets/QComboBox>
#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtWidgets/QDoubleSpinBox>
#include <qt6/QtGui/QCloseEvent>
#include <qt6/QtGui/QKeyEvent>
#include <qt6/QtGui/QColor>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtWidgets/QSplitter>
#include <qt6/QtWidgets/QProgressBar>
#include <qt6/QtWidgets/QColorDialog>
#include <cmath>
#include <SDL2/SDL.h>
#include "qcustomplot.h"

enum WaveType
{
    SINE,
    TRAINGULAR,
    SAWTOOTH,
    SQUARE
};

class ToneGenerator : public QDialog
{
    Q_OBJECT
public:
    ToneGenerator(QWidget *parent = nullptr);
    ~ToneGenerator();
    QVector<short> generateSineWave(double, double, double);
    QVector<short> generateSawtoothWave(double, double, double);
    QVector<short> generateTriangularWave(double, double, double);
    QVector<short> generateSquareWave(double, double, double);
    static void sdlAudioCallback(void* userdata, Uint8* stream, int len);

signals:
    void audioFinishedPlaying();
    void closed();
    void audioProgress(int);

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);

private:
    void plotWave(int index);
    /*Notes *m_notes = new Notes();*/
    QVBoxLayout *m_layout = new QVBoxLayout();
    QWidget *m_side_panel = new QWidget();
    QGridLayout *m_side_panel_layout = new QGridLayout();

    QPushButton *m_play_btn = new QPushButton("Play"),
                *m_stop_btn = new QPushButton("Stop"),
                *m_color_btn = new QPushButton();

    QLabel  *m_duration_label = new QLabel("Duration"),
            *m_freq_label = new QLabel("Frequency"),
            *m_amplitude_label = new QLabel("Amplitude"),
            *m_color_label = new QLabel("Wave color");

    QDoubleSpinBox  *m_amplitude_sb = new QDoubleSpinBox(),
                    *m_duration_sb = new QDoubleSpinBox(),
                    *m_freq_sb = new QDoubleSpinBox();

    QSplitter *m_splitter = new QSplitter();


    QVector<short> m_audioData;
    int m_nsamples = 1024;
    float m_sampleRate = 44100.0f;
    int m_audioOffset = 0;

    SDL_AudioSpec m_wavSpec;
    SDL_AudioDeviceID m_audioDevice;
    QComboBox *m_wave_type_combo = new QComboBox();
    QLabel *m_wave_type_label = new QLabel("Wave Type");

    bool m_playing = false;
    QCustomPlot *m_plot = new QCustomPlot();

    QStringList m_wavetype_labels = { "Sine", "Triangular", "Sawtooth", "Square" };

    WaveType m_wavetype = WaveType::SINE;
    QProgressBar *m_audio_progress = new QProgressBar();
};

#endif
