#pragma once

#include "qcustomplot.h"

#include <QCloseEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QVBoxLayout>

class WaveformWidget : public QWidget
{
    Q_OBJECT
public:
    WaveformWidget(QWidget *parent = nullptr) noexcept;
    void setData(const QVector<short> &data, float sampleRate) noexcept;
    void setVertLinePosition(const double &) noexcept;
    void resetVertLine() noexcept;

protected:
    void closeEvent(QCloseEvent *e) noexcept;

signals:
    void closed();

private:
    QPushButton *m_wf_show_vline_btn   = new QPushButton("Show Position Line");
    QPushButton *m_wf_save_btn         = new QPushButton("Save Figure");
    QVBoxLayout *m_wf_widget_layout    = new QVBoxLayout();
    QHBoxLayout *m_wf_btm_layout       = new QHBoxLayout();
    QCustomPlot *m_wf_plot             = new QCustomPlot();
    QCPItemStraightLine *m_wf_vertline = nullptr;
    QCPLayer *m_wf_vertline_layer      = nullptr;

    void savePlot() noexcept;

    bool m_wf_show_vline = true;
};
