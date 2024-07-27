#include "waveform_savedialog.hpp"

WaveformSaveDialog::WaveformSaveDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setLayout(m_layout);

    m_filename_label = new QLabel("File");
    m_filename_lineedit = new QLineEdit();

    m_layout->addWidget(m_filename_label, 0, 0);
    m_layout->addWidget(m_filename_lineedit, 0, 1);
    m_layout->addWidget(m_file_browse_btn, 0, 2);

    m_layout->addWidget(m_cancel_btn, 5, 0);
    m_layout->addWidget(m_save_btn, 5, 1);

}
