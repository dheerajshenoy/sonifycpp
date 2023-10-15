#ifndef __SONIFY_CPP
#define __SONIFY_CPP

#include "sonify.hpp"
#include "playaudio.hpp"

sonify::sonify(QWidget *parent)
    : QMainWindow(parent)
{
    _InitGUI();
    _InitMenubar();
    _InitToolbar();
    _InitMainWidget();
    _InitStatusbar();
}

void sonify::Play()
{
    PlayAudio *p = new PlayAudio();
    p->start();
}

void sonify::_InitGUI()
{
    drawer = new Drawer(this);
    canvas = new Canvas(this);
    statusbar = new StatusBar(this);
}

/* Menubar Init function */
void sonify::_InitMenubar()
{

    // Menu
    QMenuBar *menuBar = new QMenuBar();
    QMenu *menu_file = new QMenu("File");
    QMenu *menu_edit = new QMenu("Edit");
    QMenu *menu_view = new QMenu("View");
    QMenu *menu_help = new QMenu("Help");

    menuBar->addMenu(menu_file);
    menuBar->addMenu(menu_edit);
    menuBar->addMenu(menu_view);
    menuBar->addMenu(menu_help);
    
    /* File menu items */
    
    // Open
    QAction *action_file_open = new QAction("Open");
    action_file_open->setIcon(QIcon(":/icons/open.png"));
    connect(action_file_open, &QAction::triggered, this, [=]() { this->_openImage(); });

    // Open Recent
    QMenu *action_file_open_recent = new QMenu("Open Recent");
    action_file_open_recent->setIcon(QIcon(":/icons/open.png"));

    // TODO: Handle open recent files

    // Exit
    QAction *action_file_exit = new QAction(QIcon(":icons/exit.png"), "Exit");
    connect(action_file_exit, &QAction::triggered, this, [=]() { QApplication::quit(); });

    menu_file->addAction(action_file_open);
    menu_file->addMenu(action_file_open_recent);
    menu_file->addAction(action_file_exit);

    /* Edit Menu */

    QAction *action_edit_pref = new QAction(QIcon(":icons/prefs.png"), "Preferences");

    menu_edit->addAction(action_edit_pref);

    /* View Menu */

    QMenu *action_view_overlay = new QMenu("Overlay");
    action_view_overlay->setIcon(QIcon(":/icons/overlay.png"));

    menu_view->addMenu(action_view_overlay);

    QAction *action_view_overlay_line = new QAction(QIcon(":/icons/"), "Line");
    
    action_view_overlay->addAction(action_view_overlay_line);

    /* Help Menu */

    QAction *action_help_about_sonify = new QAction("About Sonify++");

    menu_help->addAction(action_help_about_sonify);

    this->setMenuBar(menuBar);
}

void sonify::_InitToolbar()
{
    QToolBar *_toolbar = new QToolBar();

    QAction* toolbar_open_action = _toolbar->addAction(QIcon(":/icons/open.png"), "Open");
    connect(toolbar_open_action, &QAction::triggered, this, [=]() { this->_openImage(); });

    QAction *toolbar_selection_action = _toolbar->addAction(QIcon(":/icons/selection.png"), "Selection");
    connect(toolbar_selection_action, &QAction::triggered, this, [=]() { this->Selection(); });
    
    this->mainLayout->addWidget(_toolbar);
}

void sonify::_InitMainWidget()
{
    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);
    this->setContentsMargins(0, 0, 0, 0);

    QSplitter *mainSplitter = new QSplitter();
    mainSplitter->setContentsMargins(0, 0, 0, 0);
    this->mainLayout->addWidget(mainSplitter);
        
    mainSplitter->addWidget(this->drawer);
    mainSplitter->addWidget(this->canvas);
}

void sonify::_InitStatusbar()
{
    statusbar->setMaximumHeight(40);
    this->mainLayout->addWidget(statusbar);
}

void sonify::_openImage(QString path)
{
    if (path.isEmpty())
    {
        QFileDialog fd(this, "Open Image", "",
                "Image Files (*.jpg *.png)");
        if (fd.exec())
        {
            statusbar->Msg("Image loaded", 5);
            this->_fileName = fd.selectedFiles()[0];
            canvas->SetImage(_fileName);
            statusbar->SetFile(_fileName);
        }
        else
        {
            statusbar->Msg("Could not open file!", 5);
            // TODO: Handle file open error
        }
    }
    else
    {
        
    }
}

void sonify::Selection()
{

}

sonify::~sonify() {}

#endif
