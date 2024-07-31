/*
*
*   This the class that manages the other classes that handle the GUI, sonification process and audio playback. This represents the main window
*
*/

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
#include <qt6/QtCore/QStandardPaths>
#include <qt6/QtCore/QDir>
#include <qt6/QtCore/QDebug>
#include <qt6/QtCore/QThread>
#include <qt6/QtConcurrent/QtConcurrent>
#include <qt6/QtMultimedia/QWindowCapture>
#include <qt6/QtWidgets/QSizePolicy>
#include <qt6/QtWidgets/QCheckBox>
#include <qt6/QtWidgets/QProgressBar>
#include "sonification.hpp"
#include "gv.hpp"
#include "waveform_savedialog.hpp"
#include "tonegenerator.hpp"
#include "waveformwidget.hpp"
#include "aboutdialog.hpp"
#include "screenRecorder.hpp"
#include "SpectrumAnalyzer.hpp"
#include "sol/sol.hpp"
#include "ReverbDialog.hpp"

/*#include <opencv4/opencv2/opencv.hpp>*/
/**/
/**/
/*extern "C" {*/
/*#include <libavcodec/avcodec.h>*/
/*#include <libavformat/avformat.h>*/
/*#include <libavutil/time.h>*/
/*#include <libswscale/swscale.h>*/
/*}*/

class Sonify : public QMainWindow
{

public:
    Sonify(QWidget *parent = nullptr);
    ~Sonify();
    void Open(QString filename = "");

private:

    void Play();
    void ConvertToVideo();
    void Pause();
    void PlayAudio();
    void Reset();
    bool Save(QString filename = "");
    void setMsg(QString msg, int s = -1);
    void doSonify();
    void initConfigDir();
    void initConnections();
    void initLuaBindings();
    void initMenu();
    void initWidgets();
    void initStatusbar();
    void initSidePanel();
    void CaptureWindow();
    void AskForResize(QString filename);
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
    QMenu   *m_file_menu,
            *m_audio_menu,
            *m_tools_menu,
            *m_view_menu,
            *m_effects_menu,
            *m_help_menu;

    QAction *m_file__open,
            *m_file__exit;

    QAction *m_audio__save;

    QAction *m_effects__reverb,
            *m_effects__phaser,
            *m_effects__distortion,
            *m_effects__wah_wah,
            *m_effects__filter,
            *m_effects__pitch_shifter;

    QAction *m_tools__tone_generator,
            *m_tools__spectrum_analyzer,
            *m_tools__waveform,
            *m_tools__screen_record;

    QAction *m_view__waveform;

    QAction *m_help__about;

    QComboBox *m_traverse_combo;

    QLabel *m_duration_label, *m_audio_progress_label, *m_traverse_label, *m_num_samples_label, *m_statusbar_msg_label,
            *m_min_freq_label, *m_max_freq_label;

    QSpinBox *m_num_samples_spinbox, *m_min_freq_sb, *m_max_freq_sb;

    QWidget *m_status_bar;
    QHBoxLayout *m_status_bar_layout;

    QPixmap m_pix;

    void viewWaveform(bool state = false);

    QProgressBar *m_progress_bar;
    QPushButton *m_stop_sonification_btn;

    QLabel *m_input_img_height_label, *m_input_img_width_label;
    Traverse m_mode;

    WaveformWidget *m_wf_widget = nullptr;
    ScreenRecorder *m_recorder = new ScreenRecorder();

    QStringList m_traversal_name_list;

    bool    m_def_keep_aspect = false,
            m_def_ask_for_resize = true;
    int m_def_height = -1;
    int m_def_width = -1;

    ToneGenerator *m_tg = nullptr;
    SpectrumAnalyzer *m_sp = nullptr;
    QString m_script_file_path, m_config_dir;

    sol::state m_lua_state;

};


#endif
