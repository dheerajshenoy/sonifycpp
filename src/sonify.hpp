#ifndef SONIFY_HPP
#define SONIFY_HPP

#include <qt6/QtWidgets/QMainWindow>
#include <qt6/QtGui/QGuiApplication>
#include <qt6/QtWidgets/QApplication>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QFileDialog>
#include <qt6/QtWidgets/QMenu>
#include <qt6/QtWidgets/QMenuBar>
#include <qt6/QtWidgets/QHBoxLayout>
#include <qt6/QtWidgets/QGridLayout>
#include <qt6/QtWidgets/QWidget>
#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtWidgets/QSplitter>
#include <qt6/QtWidgets/QComboBox>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtWidgets/QSpinBox>
#include <qt6/QtCore/QTimer>
#include <qt6/QtConcurrent/QtConcurrent>
#include <qt6/QtMultimedia/QWindowCapture>
#include <qt6/QtWidgets/QSizePolicy>
#include <qt6/QtWidgets/QProgressBar>
#include "sonification.hpp"
#include "gv.hpp"
#include "qcustomplot.h"

class Sonify : public QMainWindow
{

public:
    Sonify(QWidget *parent = nullptr);
    ~Sonify();

private:
    void PlayAudio();
    void Reset();
    bool Open(QString filename = "");
    bool Save(QString filename = "");
    void setMsg(QString msg, int s = -1);
    void doSonify();
    void initConnections();
    void initMenu();
    void initWidgets();
    void initStatusbar();
    void initSidePanel();
    void CaptureWindow();
    QWidget *m_widget;
    QSplitter *m_splitter;
    QWidget *m_side_panel;
    QGridLayout *m_side_panel_layout = new QGridLayout();
    QVBoxLayout *m_layout;
    Sonification *sonification = new Sonification();
    QPushButton *m_play_btn,
                *m_sonify_btn,
                *m_reset_btn;
    GV *gv = new GV();
    bool m_isAudioPlaying = false;

    QMenuBar *m_menu_bar;
    QMenu *m_file_menu,
          *m_audio_menu,
          *m_tools_menu,
          *m_view_menu,
          *m_about_menu;

    QAction *m_file__open,
            *m_file__exit;
    QAction *m_audio__save;
    QAction *m_view__waveform;

    QComboBox *m_traverse_combo;

    QLabel *m_duration_label, *m_audio_progress_label, *m_traverse_label, *m_num_samples_label, *m_statusbar_msg_label;
    QSpinBox *m_num_samples_spinbox;

    QWidget *m_status_bar;
    QHBoxLayout *m_status_bar_layout;

    QPixmap m_pix;

    void viewWaveform(bool state = false);
    QCustomPlot *waveformplot = new QCustomPlot();

    QProgressBar *m_progress_bar;
    QPushButton *m_stop_sonification_btn;

    QLineEdit *m_input_img_height, *m_input_img_width;
    QLabel *m_input_img_height_label, *m_input_img_width_label;
};

#endif
