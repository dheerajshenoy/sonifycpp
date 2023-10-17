#ifndef __SONIFY_HPP
#define __SONIFY_HPP


class Canvas;
class Drawer;
class StatusBar;
class Preferences;


class sonify : public QMainWindow
{
    public:
        sonify(QWidget *parent = nullptr);
        ~sonify();

    private:
        QWidget *mainWidget = new QWidget();
        QVBoxLayout *mainLayout = new QVBoxLayout();
        QMenuBar *menuBar = new QMenuBar();
        QMenu *action_file_open_recent = new QMenu("Open Recent");
        Canvas *canvas;
        Drawer *drawer;
        StatusBar *statusbar;
        std::ofstream _recentFiles;

        void _InitGUI();
        void _InitMenubar();
        void _InitToolbar();
        void _InitMainWidget();
        void _InitStatusbar();
        void _openImage(QString path = "");
        void _openPrefs();
        void Selection();
        void ReadRecentFiles();
        void addRecentFile(QString);

        QString _fileName;
        void Play();

        friend class StatusBar;
        friend class Drawer;
        friend class Canvas;
        friend class Preferences;
};

#include "statusbar.hpp"
#include "canvas.hpp"
#include "drawer.hpp"


#endif
