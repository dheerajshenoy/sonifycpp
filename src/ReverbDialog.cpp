#include "ReverbDialog.hpp"

ReverbDialog::ReverbDialog(QWidget *parent) noexcept : QDialog(parent)
{

    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    m_delay_sb->setRange(0, 10000);
    m_feedback_sb->setRange(0, 1000);
    m_layout->addWidget(m_delay_label, 0, 0);
    m_layout->addWidget(m_delay_sb, 0, 1);
    m_layout->addWidget(m_feedback_label, 1, 0);
    m_layout->addWidget(m_feedback_sb, 1, 1);
    m_layout->addWidget(m_reverb_btn, 2, 0, 1, 2);
    this->setLayout(m_layout);

    connect(m_reverb_btn, &QPushButton::clicked, this, &ReverbDialog::process);
}

void
ReverbDialog::setData(const std::vector<short> &audioData,
                      int sampleRate) noexcept
{
    m_audioData  = audioData;
    m_sampleRate = sampleRate;
}

void
ReverbDialog::process() noexcept
{
    auto delay    = m_delay_sb->text().toDouble();
    auto feedback = m_feedback_sb->text().toDouble();

    if (delay == 0.0f && feedback == 0.0f) return;
    if (m_audioData.size() == 0 || m_sampleRate == 0) return;

    // Calculate delay size in samples
    int delaySize = (m_sampleRate * delay) / 1000;

    // Create a delay line (circular buffer)
    std::vector<float> delayLine(delaySize, 0.0f);
    std::vector<short> output(m_audioData.size());

    // Variables for delay line management
    int index = 0;

    for (int i = 0; i < m_audioData.size(); ++i)
    {
        float inSample      = static_cast<float>(m_audioData.at(i));
        float delayedSample = delayLine.at(index);
        float outSample     = inSample + delayedSample * feedback;

        // Clip output to the range of short
        output[i] = std::clamp(static_cast<int>(outSample), SHRT_MIN, SHRT_MAX);

        // Update the delay line
        delayLine[index] = outSample;
        index            = (index + 1) % delaySize; // Circular buffer
    }

    emit outputReady(output);
}
