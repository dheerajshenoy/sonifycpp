#pragma once

#include "notes.hpp"
#include "qboxlayout.h"
#include "qcustomplot.h"

#include <QCloseEvent>
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QKeyEvent>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <cmath>
#include <sndfile.h>

enum class WaveType : int { SINE = 0, TRAINGULAR, SAWTOOTH, SQUARE };

class ToneGenerator : public QDialog {
    Q_OBJECT
public:
    ToneGenerator(QWidget *parent = nullptr) noexcept;
    ~ToneGenerator();
    std::vector<short> generateSineWave(double, double, double) noexcept;
    std::vector<short> generateSawtoothWave(double, double, double) noexcept;
    std::vector<short> generateTriangularWave(double, double, double) noexcept;
    std::vector<short> generateSquareWave(double, double, double) noexcept;

    static void audioCallback(void *userdata,
                              SDL_AudioStream *stream,
                              int additional_amount,
                              int total_amount) noexcept;

signals:
    void audioFinishedPlaying();
    void closed();
    void audioProgress(int);

protected:
    void closeEvent(QCloseEvent *e) noexcept;
    void keyPressEvent(QKeyEvent *e) noexcept;

private:
    void plotWave(const WaveType &) noexcept;
    /*Notes *m_notes = new Notes();*/
    QVBoxLayout *m_layout            = new QVBoxLayout();
    QWidget *m_side_panel            = new QWidget();
    QGridLayout *m_side_panel_layout = new QGridLayout();

    QPushButton *m_play_btn  = new QPushButton("Play"),
                *m_stop_btn  = new QPushButton("Stop"),
                *m_color_btn = new QPushButton();

    QLabel *m_duration_label  = new QLabel("Duration"),
           *m_freq_label      = new QLabel("Frequency"),
           *m_amplitude_label = new QLabel("Amplitude"),
           *m_color_label     = new QLabel("Wave color");

    QDoubleSpinBox *m_amplitude_sb = new QDoubleSpinBox(),
                   *m_duration_sb  = new QDoubleSpinBox(),
                   *m_freq_sb      = new QDoubleSpinBox();

    QSplitter *m_splitter = new QSplitter();

    std::vector<short> m_audioData;
    int m_nsamples     = 1024;
    float m_sampleRate = 44100.0f;
    int m_audioOffset  = 0;

    SDL_AudioSpec m_audioSpec;
    SDL_AudioStream *m_audioStream;

    QComboBox *m_wave_type_combo = new QComboBox();
    QLabel *m_wave_type_label    = new QLabel("Wave Type");

    bool m_playing      = false;
    QCustomPlot *m_plot = new QCustomPlot();

    QStringList m_wavetype_labels = {
        "Sine", "Triangular", "Sawtooth", "Square"};

    WaveType m_wavetype            = WaveType::SINE;
    QProgressBar *m_audio_progress = new QProgressBar();
};
