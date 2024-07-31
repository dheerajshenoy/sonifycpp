#ifndef SPECTRUM_ANALYZER_HPP
#define SPECTRUM_ANALYZER_HPP

#include <qt6/QtWidgets/QDialog>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtCore/QVector>
#include <qt6/QtCore/QObject>
#include <qt6/QtGui/QCloseEvent>
#include <qt6/QtGui/QKeyEvent>
#include "qcustomplot.h"
#include <fftw3.h>

class SpectrumAnalyzer : public QDialog
{
    Q_OBJECT
public:
    SpectrumAnalyzer(QWidget *parent = nullptr);
    ~SpectrumAnalyzer();
    void setData(QVector<short> &data, float &SampleRate);

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);

private:
    QVBoxLayout *m_layout = new QVBoxLayout();
    QCustomPlot *m_plot = new QCustomPlot();

};

#endif
