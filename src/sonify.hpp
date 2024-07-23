#include <qt6/QtWidgets/QMainWindow>
#include <qt6/QtWidgets/QApplication>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QFileDialog>
#include <qt6/QtWidgets/QMenu>
#include <qt6/QtWidgets/QMenuBar>
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
    void initConnections();
    void initMenu();
    void PlayAudio();
    void Reset();
    void doSonify();
    bool Open(QString filename = "");
    ~Sonify();

private:
    QWidget *m_widget;
    QVBoxLayout *m_layout;
    Sonification *sonification = new Sonification();
    QPushButton *m_play_btn,
                *m_sonify_btn,
                *m_reset_btn;
    GV *gv = new GV();
    bool m_isAudioPlaying = false;

    QMenuBar *m_menu_bar;
    QMenu *m_file_menu,
          *m_about_menu;

    QAction *m_file__open,
            *m_file__exit;

    QPixmap m_pix;
};
