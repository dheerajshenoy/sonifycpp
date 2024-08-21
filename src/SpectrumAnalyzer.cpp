#include "SpectrumAnalyzer.hpp"

SpectrumAnalyzer::SpectrumAnalyzer(QWidget *parent) noexcept
    : QDialog(parent)
{
    
    this->setLayout(m_layout);
    m_layout->addWidget(m_plot);
    m_plot->addGraph();
    m_plot->plotLayout()->insertRow(0);
    m_plot->setInteractions(QCP::iSelectAxes | QCP::iRangeDrag | QCP::iRangeZoom);
    m_plot->plotLayout()->addElement(0, 0, new QCPTextElement(m_plot, "Frequency Analyzer"));
    m_plot->xAxis->setLabel("Frequency");
    m_plot->yAxis->setLabel("Amplitude");
}

void SpectrumAnalyzer::setData(const QVector<short> &data, const int &SampleRate) noexcept
{
   // Perform FFT
    int N = data.size();
    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    for (int i = 0; i < N; ++i) {
        in[i][0] = data[i]; // Real part
        in[i][1] = 0.0;        // Imaginary part
    }

    fftw_execute(plan);

    QVector<double> spectrum(N / 2);
    double maxAmp = 0.0;
    for (size_t i = 0; i < N / 2; ++i) {
        double amp = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]) / N;
        spectrum[i] = amp;
        if (amp > maxAmp)
            maxAmp = amp;
    }

    for(size_t i =0; i < N / 2; i++)
        spectrum[i] = spectrum.at(i) / maxAmp;

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    QVector<double> frequencies(N);
    for (size_t i = 0; i < N; ++i)
    {
        double freq = i * SampleRate / static_cast<double>(N);
        frequencies[i] = freq;
    }

    m_plot->graph(0)->setData(frequencies, spectrum);
    m_plot->xAxis->setRange(0, 22500);
    m_plot->yAxis->setRange(0, 1);
    m_plot->replot();
}

void SpectrumAnalyzer::closeEvent(QCloseEvent *e) noexcept
{
    emit closed();
}

void SpectrumAnalyzer::keyPressEvent(QKeyEvent *e) noexcept
{
    if (e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
}
