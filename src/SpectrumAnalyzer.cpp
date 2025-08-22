#include "SpectrumAnalyzer.hpp"

SpectrumAnalyzer::SpectrumAnalyzer(QWidget *parent) noexcept : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();
    setAttribute(Qt::WA_DeleteOnClose);
    this->setLayout(layout);
    layout->addWidget(m_plot);
    m_plot->addGraph();
    m_plot->plotLayout()->insertRow(0);
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_plot->plotLayout()->addElement(
        0, 0, new QCPTextElement(m_plot, "Frequency Analyzer"));
    m_plot->xAxis->setLabel("Frequency");
    m_plot->yAxis->setLabel("Amplitude");
    m_plot->setNotAntialiasedElement(QCP::aeAll);
    // m_plot->setOpenGl(true);

    auto graph = m_plot->graph(0);
    m_plot->xAxis->setRange(0, MAX_FREQ);
    m_plot->yAxis->setRange(MIN_DB, 0);

    setMinimumSize(400, 400);

    // Initialize with default size
    initializeFFTW(DEFAULT_FFT_SIZE);
}

SpectrumAnalyzer::~SpectrumAnalyzer() noexcept
{
    cleanupFFTW();
}

void
SpectrumAnalyzer::setData(const std::vector<short> &data,
                          int sampleRate) noexcept
{
    if (data.empty()) return;

    // Update sample rate if changed
    // if (m_sampleRate != sampleRate)
    // {
    //     m_sampleRate = sampleRate;
    //     // Recalculate frequencies
    //     const double freqStep = static_cast<double>(sampleRate) / m_fftSize;
    //     for (int i = 0; i < m_fftSize / 2; ++i)
    //     {
    //         m_frequencies[i] = i * freqStep;
    //     }
    // }

    // Resize FFT if needed (prefer power of 2)
    int newSize = 1;
    while (newSize < static_cast<int>(data.size()))
        newSize <<= 1;
    newSize = std::min(newSize, 8192); // Cap at 8192 for real-time performance

    if (newSize != m_fftSize) { initializeFFTW(newSize); }

    // Apply windowing
    applyWindow(data);

    // Execute FFT
    fftw_execute(m_plan);

    // Calculate magnitude spectrum with logarithmic scaling
    double maxAmp      = 1e-10; // Prevent log(0)
    const int halfSize = m_fftSize / 2;

    for (int i = 0; i < halfSize; ++i)
    {
        const double real      = m_out[i][0];
        const double imag      = m_out[i][1];
        const double magnitude = std::sqrt(real * real + imag * imag);
        m_spectrum[i]          = magnitude;
        maxAmp                 = std::max(maxAmp, magnitude);
    }

    // Convert to dB scale
    const double maxAmpInv = 1.0 / maxAmp;
    for (int i = 0; i < halfSize; ++i)
    {
        const double normalized = m_spectrum[i] * maxAmpInv;
        m_spectrum[i]           = 20.0 * std::log10(std::max(normalized, 1e-8));
    }

    m_plot->graph(0)->setData(m_frequencies, m_spectrum);
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

void
SpectrumAnalyzer::closeEvent(QCloseEvent *e) noexcept
{
    emit closed();
}

void
SpectrumAnalyzer::initializeFFTW(int size) noexcept
{
    if (m_fftSize == size && m_plan) return; // Already initialized

    cleanupFFTW();

    m_fftSize = size;

    // Allocate aligned memory for better performance
    m_in  = fftw_alloc_complex(size);
    m_out = fftw_alloc_complex(size);

    // Create optimized plan (FFTW_MEASURE for better performance in long run)
    m_plan = fftw_plan_dft_1d(size, m_in, m_out, FFTW_FORWARD, FFTW_ESTIMATE);

    // Pre-allocate vectors
    m_spectrum.resize(size / 2);
    m_frequencies.resize(size / 2);
    m_window.resize(size);

    // Pre-calculate Hann window (better than rectangular)
    for (int i = 0; i < size; ++i)
    {
        m_window[i] = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / (size - 1)));
    }

    // Pre-calculate frequencies
    const double freqStep = static_cast<double>(m_sampleRate) / size;
    for (int i = 0; i < size / 2; ++i)
    {
        m_frequencies[i] = i * freqStep;
    }
}

void
SpectrumAnalyzer::cleanupFFTW() noexcept
{
    if (m_plan)
    {
        fftw_destroy_plan(m_plan);
        m_plan = nullptr;
    }
    if (m_in)
    {
        fftw_free(m_in);
        m_in = nullptr;
    }
    if (m_out)
    {
        fftw_free(m_out);
        m_out = nullptr;
    }
    m_fftSize = 0;
}

void
SpectrumAnalyzer::applyWindow(const std::vector<short> &data) noexcept
{
    const int size = std::min(static_cast<int>(data.size()), m_fftSize);

    // Apply windowing and convert to complex
    for (int i = 0; i < size; ++i)
    {
        m_in[i][0] = data[i] * m_window[i]; // Real part with window
        m_in[i][1] = 0.0;                   // Imaginary part
    }

    // Zero-pad if necessary
    for (int i = size; i < m_fftSize; ++i)
    {
        m_in[i][0] = 0.0;
        m_in[i][1] = 0.0;
    }
}
