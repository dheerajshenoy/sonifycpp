#include "waveform_savedialog.hpp"

WaveformSaveDialog::WaveformSaveDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setLayout(m_layout);

    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    m_resolution_unit_combo->addItem("Dots per Inch");
    m_resolution_unit_combo->addItem("Dots per Meter");
    m_resolution_unit_combo->addItem("Dots per Centimeter");

    m_filepath_label->setText("Browse for file path");
    m_filepath_lineedit->setEnabled(false);

    m_layout->addWidget(m_filepath_label);
    m_layout->addWidget(m_filepath_lineedit);
    m_layout->addWidget(m_file_browse_btn);

    m_layout->addWidget(m_filename_label);
    m_layout->addWidget(m_filename_lineedit);
    m_layout->addWidget(m_filename_done_btn);

    m_layout->addWidget(m_img_widget);
    m_layout->addWidget(m_pdf_widget);
    m_layout->addWidget(m_rastered_widget);

    m_layout->addWidget(m_cancel_btn);
    m_layout->addWidget(m_save_btn);

    m_img_widget->setVisible(false);
    m_pdf_widget->setVisible(false);
    m_rastered_widget->setVisible(false);

    m_imglayout->addWidget(m_width_label, 0, 0);
    m_imglayout->addWidget(m_width_sb, 0, 1);
    m_imglayout->addWidget(m_height_label, 1, 0);
    m_imglayout->addWidget(m_height_sb, 1, 1);

    m_imglayout->addWidget(m_scale_label, 2, 0);
    m_imglayout->addWidget(m_scale_sb, 2, 1);
    m_imglayout->addWidget(m_quality_label, 2, 2);
    m_imglayout->addWidget(m_quality_sb, 2, 3);

    m_imglayout->addWidget(m_resolution_label, 3, 0);
    m_imglayout->addWidget(m_resolution_sb, 3, 1);
    m_imglayout->addWidget(m_resolution_unit_label, 3, 2);
    m_imglayout->addWidget(m_resolution_unit_combo, 3, 3);

    m_img_widget->setLayout(m_imglayout);
    m_pdf_widget->setLayout(m_pdflayout);
    m_rastered_widget->setLayout(m_rastered_layout);

    int ok_row, ok_column, ok_rowspan, ok_colspan;

    connect(m_filename_done_btn, &QPushButton::clicked, this, [&]() {
        m_img_widget->setVisible(false);
        m_pdf_widget->setVisible(false);
        m_rastered_widget->setVisible(false);

        if (m_filename_lineedit->text().isEmpty())
        {
            return;
        }

        QString ext = QFileInfo(m_filename_lineedit->text()).completeSuffix();

        // TODO: Add some message that no extension found for the filename
        if (ext.isEmpty())
        {
            return;
        }
        
        if (ext == "jpeg" || ext == "jpg" || ext == "png")
        {
            m_save_format = SaveFormat::JPG;
            m_img_widget->setVisible(true);
        }

        else if (ext == "pdf")
        {
            m_save_format = SaveFormat::PDF;
            m_pdf_widget->setVisible(true);
        }

        else
        {
            m_img_widget->setVisible(false);
            m_pdf_widget->setVisible(false);
            m_rastered_widget->setVisible(false);
        }


    });

    connect(m_save_btn, &QPushButton::clicked, this, [&]() {
        QString filename = m_filename_lineedit->text();
        int width = m_width_sb->text().toInt();
        int height = m_height_sb->text().toInt();
        double scale = m_scale_sb->text().toFloat();
        int quality = m_quality_sb->text().toInt();
        int resolution = m_resolution_sb->text().toInt();

        QString resunit_text = m_resolution_unit_combo->currentText();
        QCP::ResolutionUnit resunit;

        if (resunit_text == "Dots per Inch")
            resunit = QCP::ResolutionUnit::ruDotsPerInch;

        else if (resunit_text == "Dots per Meter")
            resunit = QCP::ResolutionUnit::ruDotsPerMeter;

        else if (resunit_text == "Dots per Centimeter")
            resunit = QCP::ResolutionUnit::ruDotsPerCentimeter;

    });

    connect(m_cancel_btn, &QPushButton::clicked, this, [&]() {
        this->close();
    });

    connect(m_file_browse_btn, &QPushButton::clicked, this, [&]() {
        QFileDialog fd;
        fd.setFileMode(QFileDialog::FileMode::Directory);
        fd.setViewMode(QFileDialog::ViewMode::List);

        if (fd.exec())
        {
            QString dir = fd.selectedFiles()[0];
            m_filepath_lineedit->setText(dir);
        }
    });

}
        

void WaveformSaveDialog::setPlot(QCustomPlot *c)
{
    m_plot = c;
}
