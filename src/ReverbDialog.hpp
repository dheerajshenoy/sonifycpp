#pragma once

#include <QDialog>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

class ReverbDialog : public QDialog {
    Q_OBJECT
public:
    ReverbDialog(QWidget *parent = nullptr) noexcept;
    void setData(const std::vector<short> &audioData, int sampleRate) noexcept;
    std::vector<short> reverbedOutput() noexcept;

signals:
    void outputReady(std::vector<short>);

private:
    void process() noexcept;
    QGridLayout *m_layout         = new QGridLayout();
    QDoubleSpinBox *m_delay_sb    = new QDoubleSpinBox(),
                   *m_feedback_sb = new QDoubleSpinBox();
    QLabel *m_delay_label         = new QLabel("Delay (ms)");
    QLabel *m_feedback_label      = new QLabel("Feedback");
    QPushButton *m_reverb_btn     = new QPushButton("Apply Reverb");

    std::vector<short> m_audioData;
    int m_sampleRate;
};
