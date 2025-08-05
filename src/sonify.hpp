/*
This is the class that manages the other classes that handle the GUI,
sonification process and audio playback. This represents the main window
*/

#pragma once

#include "ImageEditorDialog.hpp"
#include "ReverbDialog.hpp"
#include "SpectrumAnalyzer.hpp"
#include "aboutdialog.hpp"
#include "argparse.hpp"
#include "config.hpp"
#include "gv.hpp"
#include "screenRecorder.hpp"
#include "sonification.hpp"
#include "statusbar.hpp"
#include "toml.hpp"
#include "tonegenerator.hpp"
#include "traverse.hpp"
#include "waveform_savedialog.hpp"
#include "waveformwidget.hpp"

#include <QApplication>
#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QShortcut>
#include <QSizePolicy>
#include <QSpinBox>
#include <QSplitter>
#include <QStandardPaths>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QtConcurrent/QtConcurrent>
#include <QtMultimedia/QWindowCapture>
#include <unordered_map>

class Sonify : public QMainWindow
{
    Q_OBJECT
public:
    Sonify(QWidget *parent = nullptr);
    void Open(QString filename = "") noexcept;
    void args(const argparse::ArgumentParser &args) noexcept;
    void Close() noexcept;

    enum class Location
    {
        LEFT = 0,
        RIGHT,
        TOP,
        BOTTOM,
    };

signals:
    void fileOpened();

private:
    void Play() noexcept;
    void ConvertToVideo() noexcept;
    void Pause() noexcept;
    void PlayAudio() noexcept;
    void Reset() noexcept;
    bool Save(const QString &filename = "") noexcept;
    void doSonify() noexcept;
    void initConfigFile() noexcept;
    void initConnections() noexcept;
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
    bool AskForResize(const QString &filename) noexcept;
    void viewWaveform(const bool &state = false) noexcept;
    void readConfigFile() noexcept;
    void sonificationDone() noexcept;
    void audioPlaybackDone() noexcept;

    QWidget *m_widget     = new QWidget();
    QSplitter *m_splitter = new QSplitter();

    QWidget *m_panel = new QWidget();

    QVBoxLayout *m_layout      = new QVBoxLayout();
    Sonification *sonification = new Sonification();
    QPushButton *m_play_btn = nullptr, *m_sonify_btn = nullptr,
                *m_reset_btn = nullptr;

    GV *m_gv              = new GV();
    bool m_isAudioPlaying = false;

    QMenuBar *m_menu_bar = new QMenuBar();
    QMenu *m_file_menu = nullptr, *m_audio_menu = nullptr,
          *m_tools_menu = nullptr, *m_view_menu = nullptr,
          *m_effects_menu = nullptr, *m_help_menu = nullptr;

    QAction *m_file__open = nullptr, *m_file__close = nullptr,
            *m_file__exit  = nullptr;
    QAction *m_audio__save = nullptr;

    QAction *m_effects__reverb = nullptr, *m_effects__phaser = nullptr,
            *m_effects__distortion = nullptr, *m_effects__wah_wah = nullptr,
            *m_effects__filter = nullptr, *m_effects__pitch_shifter = nullptr;

    QAction *m_tools__tone_generator    = nullptr,
            *m_tools__spectrum_analyzer = nullptr,
            *m_tools__pixel_analyzer = nullptr, *m_tools__waveform = nullptr,
            *m_tools__screen_record  = nullptr,
            *m_tools__image_settings = nullptr;

    QAction *m_help__about = nullptr;

    QAction *m_view__panel = nullptr, *m_view__statusbar = nullptr,
            *m_view__menubar = nullptr;

    QComboBox *m_traverse_combo      = new QComboBox(),
              *m_freq_mapping_combo  = new QComboBox(),
              *m_pixel_mapping_combo = new QComboBox();

    QSpinBox *m_num_samples_spinbox = new QSpinBox(),
             *m_min_freq_sb = new QSpinBox(), *m_max_freq_sb = new QSpinBox();

    Statusbar *m_status_bar = new Statusbar();

    QPixmap m_pix;

    QLabel *m_input_img_height_label = nullptr,
           *m_input_img_width_label  = nullptr;
    Traverse m_mode;

    WaveformWidget *m_wf_widget = nullptr;
    ScreenRecorder *m_recorder  = new ScreenRecorder();

    QStringList m_traversal_name_list;

    bool m_def_keep_aspect = false, m_def_ask_for_resize = true;

    int m_def_height = -1;
    int m_def_width  = -1;

    ToneGenerator *m_tg    = nullptr;
    SpectrumAnalyzer *m_sp = nullptr;
    QString m_config_file_path, m_config_dir;

    Config m_config;

    const std::unordered_map<std::string, Traverse> m_traverse_string_map{
        {"Left to Right", Traverse::LEFT_TO_RIGHT},
        {"Right to Left", Traverse::RIGHT_TO_LEFT},
        {"Top to Bottom", Traverse::TOP_TO_BOTTOM},
        {"Bottom to Top", Traverse::BOTTOM_TO_TOP},
        {"Circle Outwards", Traverse::CIRCLE_OUTWARDS},
        {"Circle Inwards", Traverse::CIRCLE_INWARDS},
        {"Clockwise", Traverse::CLOCKWISE},
        {"Anti-Clockwise", Traverse::ANTICLOCKWISE},
        {"Draw Path", Traverse::PATH},
        {"Inspect", Traverse::INSPECT},
        // Optional: include REGION if it's still used somewhere else
        {"Region", Traverse::REGION}};
};
