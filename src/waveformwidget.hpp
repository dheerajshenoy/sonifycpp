#ifndef WAVEFORMWIDGET_HPP
#define WAVEFORMWIDGET_HPP

#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QHBoxLayout>
#include <qt6/QtGui/QCloseEvent>
#include "qcustomplot.h"

class WaveformWidget : public QWidget
{
    Q_OBJECT
public:
    WaveformWidget(QWidget *parent = nullptr);
    ~WaveformWidget();
    void setData(QVector<short> &data);
    void setVertLinePosition(double);
    void resetVertLine();

protected:
    void closeEvent(QCloseEvent *e);

signals:
    void closed();

private:
    QPushButton *m_wf_show_vline_btn = new QPushButton("Show Position Line");
    QPushButton *m_wf_save_btn = new QPushButton("Save Figure");
    QVBoxLayout *m_wf_widget_layout = new QVBoxLayout();
    QHBoxLayout *m_wf_btm_layout = new QHBoxLayout();
    QCustomPlot *m_wf_plot = new QCustomPlot();
    QCPItemStraightLine *m_wf_vertline;
    QCPLayer *m_wf_vertline_layer;

    bool m_wf_show_vline = true;

};

#endif
