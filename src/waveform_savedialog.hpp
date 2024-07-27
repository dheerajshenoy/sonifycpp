#ifndef WAVEFORM_SAVEDIALOG_HPP
#define WAVEFORM_SAVEDIALOG_HPP

#include <qt6/QtWidgets/QDialog>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtWidgets/QFileDialog>
#include <qt6/QtWidgets/QGridLayout>
#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtWidgets/QSpinBox>
#include <qt6/QtWidgets/QLineEdit>
#include <qt6/QtWidgets/QWidget>

class WaveformSaveDialog : public QDialog
{

public:
    WaveformSaveDialog(QWidget *parent = nullptr);
    ~WaveformSaveDialog(){}

private:
    QGridLayout *m_layout = new QGridLayout();
    QLabel *m_width_label, *m_height_label, *m_dpi_label, *m_scale_label, *m_quality_label,
            *m_filename_label;
    QSpinBox *m_width_sb, m_height_sb, m_dpi_sb, m_scale_sb, m_quality_sb;
    QLineEdit *m_filename_lineedit;
    QPushButton *m_advanced_btn = new QPushButton(),
                *m_file_browse_btn = new QPushButton("Browse"),
                *m_save_btn = new QPushButton("Save"),
                *m_cancel_btn = new QPushButton("Cancel");
};

#endif
