#include "waveformwidget.hpp"

WaveformWidget::WaveformWidget(QWidget *parent)
    : QWidget(parent)
{

    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    m_wf_widget_layout->addWidget(m_wf_plot);
    m_wf_plot->addGraph();
    m_wf_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    m_wf_plot->yAxis->setRange(-1, 1);

    m_wf_plot->addLayer("Line", 0, QCustomPlot::LayerInsertMode::limAbove);
    m_wf_plot->setCurrentLayer("Line");
    m_wf_vertline_layer = m_wf_plot->layer("Line");
    m_wf_vertline_layer->setMode(QCPLayer::LayerMode::lmBuffered);

    m_wf_vertline = new QCPItemStraightLine(m_wf_plot);
    m_wf_vertline->setPen(QPen(Qt::red, 1));
    m_wf_vertline->point1->setCoords(0, 0);
    m_wf_vertline->point2->setCoords(0, 1);
    m_wf_vertline->setLayer(m_wf_vertline_layer);

    m_wf_show_vline_btn->setCheckable(true);
    m_wf_show_vline_btn->setChecked(true);

    m_wf_save_btn = new QPushButton();
    m_wf_widget_layout->addLayout(m_wf_btm_layout);
    m_wf_btm_layout->addWidget(m_wf_show_vline_btn);
    m_wf_btm_layout->addWidget(m_wf_save_btn, 1);

    connect(m_wf_show_vline_btn, &QPushButton::clicked, this, [&](bool state) {
        m_wf_show_vline = state;
        m_wf_vertline->setVisible(state);
    });

    connect(m_wf_save_btn, &QPushButton::clicked, this, [&]() {
        /*Pause();*/
    });

    this->setLayout(m_wf_widget_layout);
}

void WaveformWidget::setData(QVector<short> &data)
{

    QVector<double> x, y;
    auto size = data.size();
    x.resize(size);
    y.resize(size);

    for(int i=0; i < data.size(); i++)
    {
        x[i] = i;
        y[i] = static_cast<double>(data[i] / (double) 32767);
    }

    m_wf_plot->graph(0)->setData(x, y);
    m_wf_plot->xAxis->setRange(0, size);
    m_wf_plot->replot();

}

void WaveformWidget::closeEvent(QCloseEvent *e)
{
    emit closed();
}

void WaveformWidget::setVertLinePosition(double location)
{
    m_wf_vertline->point1->setCoords(location, 0);
    m_wf_vertline->point2->setCoords(location, 1);
    m_wf_vertline_layer->replot();
}

WaveformWidget::~WaveformWidget()
{}

void WaveformWidget::resetVertLine()
{
    m_wf_vertline->point1->setCoords(0, 0);
    m_wf_vertline->point2->setCoords(0, 0);
    m_wf_vertline_layer->replot();
}
