/*

*   This the class that manages the other classes that handle the GUI, sonification process and audio playback. This represents the main window
*
*/

#pragma once

#include <QStandardPaths>
#include <QMainWindow>
#include <QGuiApplication>
#include <QShortcut>
#include <QKeySequence>
#include <QApplication>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QPushButton>
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QtMultimedia/QWindowCapture>
#include <QSizePolicy>
#include <QCheckBox>
#include <QProgressBar>
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
#include "ImageEditorDialog.hpp"

enum PanelLocation
{
    LEFT = 0,
    RIGHT,
    TOP,
    BOTTOM,
};

class Sonify : public QMainWindow
{
    Q_OBJECT
public:
    Sonify(QWidget *parent = nullptr);
    void Open(QString filename = "") noexcept;

signals:
    void fileOpened();

private:

    void Play() noexcept;
    void ConvertToVideo() noexcept;
    void Pause() noexcept;
    void PlayAudio() noexcept;
    void Reset() noexcept;
    bool Save(const QString& filename = "") noexcept;
    void setMsg(QString&& msg, const int& s = -1) noexcept;
    void doSonify() noexcept;
    void initConfigDir() noexcept;
    void initConnections() noexcept;
    void initLuaBindings() noexcept;
    void initMenu() noexcept;
    void initWidgets() noexcept;
    void initStatusbar() noexcept;
    void initLeftPanel() noexcept;
    void initRightPanel() noexcept;
    void initTopPanel() noexcept;
    void initBottomPanel() noexcept;
    void initKeybinds() noexcept;
    void initIcons() noexcept;
    void CaptureWindow() noexcept;
    void AskForResize(const QString& filename) noexcept;
    void viewWaveform(const bool& state = false) noexcept;

    QWidget *m_widget = new QWidget();
    QSplitter *m_splitter = new QSplitter();

    QWidget *m_side_panel = new QWidget(),
            *m_top_panel = new QWidget();

    QGridLayout *m_side_panel_layout = new QGridLayout();
        
    QBoxLayout *m_top_panel_layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
    QVBoxLayout *m_layout = new QVBoxLayout();
    Sonification *sonification = new Sonification();
    QPushButton *m_play_btn = nullptr,
                *m_sonify_btn = nullptr,
                *m_reset_btn = nullptr;

    GV *gv = new GV();
    bool m_isAudioPlaying = false;

    QMenuBar *m_menu_bar = new QMenuBar();
    QMenu   *m_file_menu = nullptr,
            *m_audio_menu = nullptr,
            *m_tools_menu = nullptr,
            *m_view_menu = nullptr,
            *m_effects_menu = nullptr,
            *m_help_menu = nullptr;

    QAction *m_file__open = nullptr,
            *m_file__exit = nullptr;

    QAction *m_audio__save = nullptr;

    QAction *m_effects__reverb = nullptr,
            *m_effects__phaser = nullptr,
            *m_effects__distortion = nullptr,
            *m_effects__wah_wah = nullptr,
            *m_effects__filter = nullptr,
            *m_effects__pitch_shifter = nullptr;

    QAction *m_tools__tone_generator = nullptr,
            *m_tools__spectrum_analyzer = nullptr,
            *m_tools__pixel_analyzer = nullptr,
            *m_tools__waveform = nullptr,
            *m_tools__screen_record = nullptr,
            *m_tools__image_settings = nullptr;

    QAction *m_help__about = nullptr;
    
    QAction *m_view__panel = nullptr,
            *m_view__statusbar = nullptr,
            *m_view__menubar = nullptr;

    QComboBox   *m_traverse_combo = new QComboBox(),
                *m_mapping_combo = new QComboBox(); 

    QLabel *m_duration_label, *m_audio_progress_label, *m_traverse_label, *m_num_samples_label, *m_statusbar_msg_label,
            *m_min_freq_label, *m_max_freq_label, *m_mapping_label;

    QSpinBox    *m_num_samples_spinbox = new QSpinBox(),
                *m_min_freq_sb = new QSpinBox(),
                *m_max_freq_sb = new QSpinBox();

    QWidget *m_status_bar = new QWidget();
    QHBoxLayout *m_status_bar_layout = new QHBoxLayout();

    QPixmap m_pix;


    QProgressBar *m_progress_bar = new QProgressBar();
    QPushButton *m_stop_sonification_btn = nullptr;

    QLabel *m_input_img_height_label = nullptr, *m_input_img_width_label = nullptr;
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

    PanelLocation m_panel_location = PanelLocation::LEFT;
};
