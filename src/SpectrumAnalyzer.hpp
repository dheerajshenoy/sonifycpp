#pragma once

#include "qcustomplot.h"

#include <QCloseEvent>
#include <QKeyEvent>
#include <QObject>
#include <QVBoxLayout>
#include <QWidget>
#include <fftw3.h>

class SpectrumAnalyzer : public QDialog
{
    Q_OBJECT
public:

    SpectrumAnalyzer(QWidget *parent = nullptr) noexcept;
    ~SpectrumAnalyzer() noexcept;

    void setData(const std::vector<short> &data, int SampleRate) noexcept;

signals:
    void closed();

protected:

    void closeEvent(QCloseEvent *e) noexcept override;

private:

    QCustomPlot *m_plot{ new QCustomPlot() };

    // FFTW optimization - reuse plans and buffers
    fftw_plan m_plan    = nullptr;
    fftw_complex *m_in  = nullptr;
    fftw_complex *m_out = nullptr;
    int m_fftSize       = 0;

    // Pre-allocated buffers to avoid memory allocation
    QVector<double> m_spectrum;
    QVector<double> m_frequencies;
    QVector<double> m_window; // Windowing function

    // Cached values
    int m_sampleRate = 44100;

    void initializeFFTW(int size) noexcept;
    void cleanupFFTW() noexcept;
    void applyWindow(const std::vector<short> &data) noexcept;

    static constexpr int DEFAULT_FFT_SIZE = 2048; // Power of 2 for efficiency
    static constexpr double MIN_DB        = -80.0;
    static constexpr double MAX_FREQ      = 22050.0;
};
