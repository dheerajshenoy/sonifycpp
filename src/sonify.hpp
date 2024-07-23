#include <qt6/QtWidgets/QMainWindow>
#include <qt6/QtWidgets/QApplication>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QHBoxLayout>
#include <qt6/QtWidgets/QWidget>
#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtWidgets/QLabel>
#include "sonification.hpp"
#include "gv.hpp"

class Sonify : public QMainWindow
{

public:
    Sonify(QWidget *parent = nullptr);
    void initGUI();
    void initConnections();
    void PlayAudio();
    void Reset();
    void doSonify();
    ~Sonify();

private:
    QWidget *m_widget = nullptr;
    QVBoxLayout *m_layout = nullptr;
    Sonification *sonification = new Sonification();
    QPushButton *m_play_btn = nullptr,
                *m_sonify_btn = nullptr,
                *m_reset_btn = nullptr;
    GV *gv = new GV();
    bool m_isAudioPlaying = false;
};
