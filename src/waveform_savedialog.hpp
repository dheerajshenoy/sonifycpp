#ifndef WAVEFORM_SAVEDIALOG_HPP
#define WAVEFORM_SAVEDIALOG_HPP

#include <qt6/QtWidgets/QDialog>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtWidgets/QFileDialog>
#include <qt6/QtWidgets/QGridLayout>
#include <qt6/QtWidgets/QComboBox>
#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtWidgets/QSpinBox>
#include <qt6/QtWidgets/QLineEdit>
#include <qt6/QtWidgets/QWidget>
#include "qcustomplot.h"
#include "qlabel.h"

enum SaveFormat
{
    JPG,
    PNG,
    PDF,
    BMP,
    RASTER
};

class WaveformSaveDialog : public QDialog
{

public:
    WaveformSaveDialog(QWidget *parent = nullptr);
    ~WaveformSaveDialog(){}
    void setPlot(QCustomPlot *c);

private:
    QGridLayout *m_pdflayout = new QGridLayout(),
                *m_rastered_layout = new QGridLayout(),
                *m_imglayout = new QGridLayout();

    QVBoxLayout *m_layout = new QVBoxLayout();

    QWidget *m_pdf_widget = new QWidget(),
            *m_img_widget = new QWidget(),
            *m_rastered_widget = new QWidget();

    QLabel  *m_info_label = new QLabel("Please provide the filename with extension"),
            *m_width_label = new QLabel("Width"),
            *m_height_label = new QLabel("Height"),
            *m_dpi_label = new QLabel("DPI"),
            *m_scale_label = new QLabel("Scale"),
            *m_quality_label = new QLabel("Quality"),
            *m_resolution_label = new QLabel("Resolution"),
            *m_resolution_unit_label = new QLabel("Unit"),
            *m_filepath_label = new QLabel("File Path"),
            *m_filename_label = new QLabel("File Name");

    QSpinBox    *m_width_sb = new QSpinBox(),
                *m_height_sb = new QSpinBox(),
                *m_dpi_sb = new QSpinBox(),
                *m_scale_sb = new QSpinBox(),
                *m_quality_sb = new QSpinBox(),
                *m_resolution_sb = new QSpinBox();

    QLineEdit *m_filepath_lineedit = new QLineEdit(),
              *m_filename_lineedit = new QLineEdit();

    QPushButton *m_advanced_btn = new QPushButton(),
                *m_file_browse_btn = new QPushButton("Browse"),
                *m_save_btn = new QPushButton("Save"),
                *m_cancel_btn = new QPushButton("Cancel"),
                *m_filename_done_btn = new QPushButton("Ok");

    QCustomPlot *m_plot;
    QComboBox *m_resolution_unit_combo = new QComboBox();

    SaveFormat m_save_format;
};

#endif
