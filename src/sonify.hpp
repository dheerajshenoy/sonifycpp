#ifndef __SONIFY_HPP
#define __SONIFY_HPP


class Canvas;
class Drawer;
class StatusBar;


class sonify : public QMainWindow
{
    public:
        sonify(QWidget *parent = nullptr);
        ~sonify();

    private:
        QWidget *mainWidget = new QWidget();
        QVBoxLayout *mainLayout = new QVBoxLayout();
        QMenuBar *menuBar = new QMenuBar();
        Canvas *canvas;
        Drawer *drawer;
        StatusBar *statusbar;

        void _InitGUI();
        void _InitMenubar();
        void _InitToolbar();
        void _InitMainWidget();
        void _InitStatusbar();
        void _openImage(QString path = "");
        void Selection();

        QString _fileName;
        void Play();

        friend class StatusBar;
        friend class Drawer;
        friend class Canvas;
};

#include "statusbar.hpp"
#include "canvas.hpp"
#include "drawer.hpp"


#endif
