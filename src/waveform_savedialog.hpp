#pragma once

#include <QDialog>
#include <QLabel>
#include <QFileDialog>
#include <QGridLayout>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QWidget>
#include "qcustomplot.h"
#include "qlabel.h"

enum class SaveFormat : int
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
    WaveformSaveDialog(QWidget *parent = nullptr) noexcept; 
    void setPlot(QCustomPlot *c) noexcept;

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
