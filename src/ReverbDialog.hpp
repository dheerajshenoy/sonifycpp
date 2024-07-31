#ifndef REVERB_DIALOG_HPP
#define REVERB_DIALOG_HPP

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QGridLayout>

class ReverbDialog : public QDialog
{
    Q_OBJECT
public:
    ReverbDialog(QWidget *parent = nullptr);
    ~ReverbDialog();
    void setData(QVector<short> &audioData, int &sampleRate);
    QVector<short> reverbedOutput();

signals:
    void outputReady(QVector<short>);

private:

    void process();
    QGridLayout *m_layout = new QGridLayout();
    QDoubleSpinBox    *m_delay_sb = new QDoubleSpinBox(),
                *m_feedback_sb = new QDoubleSpinBox();
    QLabel *m_delay_label = new QLabel("Delay (ms)");
    QLabel *m_feedback_label = new QLabel("Feedback");


    QPushButton *m_reverb_btn = new QPushButton("Apply Reverb");

    QVector<short> m_audioData;
    int m_sampleRate;
};

#endif
