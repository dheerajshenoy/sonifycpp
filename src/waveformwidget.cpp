#include "waveformwidget.hpp"

#include <qinputdialog.h>

WaveformWidget::WaveformWidget(QWidget *parent) noexcept : QWidget(parent)
{

    // this->setWindowModality(Qt::WindowModality::ApplicationModal);
    m_wf_widget_layout->addWidget(m_wf_plot);
    m_wf_plot->addGraph();
    m_wf_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom
                               | QCP::iSelectPlottables);
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

    m_wf_widget_layout->addLayout(m_wf_btm_layout);
    m_wf_btm_layout->addWidget(m_wf_show_vline_btn);
    m_wf_btm_layout->addWidget(m_wf_save_btn);
    m_wf_btm_layout->addStretch(1);

    connect(m_wf_show_vline_btn, &QPushButton::clicked, this, [&](bool state)
    {
        m_wf_show_vline = state;
        m_wf_vertline->setVisible(state);
        m_wf_plot->update();
    });

    connect(m_wf_save_btn, &QPushButton::clicked, this,
            &WaveformWidget::savePlot);

    this->setLayout(m_wf_widget_layout);
}

void
WaveformWidget::setData(const QVector<short> &data, float sampleRate) noexcept
{

    QVector<double> x, y;
    int size = static_cast<int>(data.size());
    x.resize(size);
    y.resize(size);

    double invRate = 1.0 / sampleRate;

    for (int i = 0; i < size; i++)
    {
        x[i] = i * invRate; // time in seconds
        // normalize to [-1, 1]: handle asymmetric range
        if (data[i] < 0)
            y[i] = static_cast<double>(data[i] / 32768.0);
        else
            y[i] = static_cast<double>(data[i] / 32767.0);
    }

    m_wf_plot->graph(0)->data()->clear();
    m_wf_plot->graph(0)->setData(x, y);
    m_wf_plot->xAxis->setRange(0.0, x.back());
    m_wf_plot->replot();
}

void
WaveformWidget::closeEvent(QCloseEvent *e) noexcept
{
    emit closed();
}

void
WaveformWidget::setVertLinePosition(const double &location) noexcept
{
    m_wf_vertline->point1->setCoords(location, 0);
    m_wf_vertline->point2->setCoords(location, 1);
    m_wf_vertline_layer->replot();
}

void
WaveformWidget::resetVertLine() noexcept
{
    m_wf_vertline->point1->setCoords(0, 0);
    m_wf_vertline->point2->setCoords(0, 0);
    m_wf_vertline_layer->replot();
}

void
WaveformWidget::savePlot() noexcept
{
    // TODO: Custom Dialog
    QString path = QFileDialog::getSaveFileName(
        this, "Save Plot", QString(),
        "Images (*.png *.jpg *.jpeg *.bmp);;PDF (*.pdf);;All Files(*)");
    if (!path.isEmpty())
    {
        QFileInfo info(path);
        QString ext = info.suffix();
        if (ext == "png")
            m_wf_plot->savePng(path);
        else if (ext == "pdf")
            m_wf_plot->savePdf(path);
        else if (ext == "jpeg" || ext == "jpg")
            m_wf_plot->saveJpg(path);
        else if (ext == "bmp")
            m_wf_plot->saveBmp(path);
        else
            m_wf_plot->savePdf(path);
        // else
        // m_wf_plot->saveRastered(const QString &fileName, int width, int
        // height, double scale, const char *format)
    }
}
