#pragma once

#include "qcustomplot.h"

#include <QCloseEvent>
#include <QDialog>
#include <QKeyEvent>
#include <QObject>
#include <QVBoxLayout>
#include <fftw3.h>

class SpectrumAnalyzer : public QDialog {
    Q_OBJECT
public:
    SpectrumAnalyzer(QWidget *parent = nullptr) noexcept;
    void setData(const std::vector<short> &data,
                 const int &SampleRate) noexcept;

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent *e) noexcept;
    void keyPressEvent(QKeyEvent *e) noexcept;

private:
    QVBoxLayout *m_layout = new QVBoxLayout();
    QCustomPlot *m_plot   = new QCustomPlot();
};
