#ifndef __SONIFY_CPP
#define __SONIFY_CPP

#include "sonify.hpp"
#include "preferences.hpp"

sonify::sonify(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("sonify");
    _InitGUI();
    _InitMenubar();
    _InitToolbar();
    _InitMainWidget();
    _InitStatusbar();
}

void sonify::Play()
{
    canvas->Play();
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
    action_file_open_recent->setIcon(QIcon(":/icons/open.png"));

    ReadRecentFiles();


    // Exit
    QAction *action_file_exit = new QAction(QIcon(":icons/exit.png"), "Exit");
    connect(action_file_exit, &QAction::triggered, this, [&]() { QApplication::quit(); });

    menu_file->addAction(action_file_open);
    menu_file->addMenu(action_file_open_recent);
    menu_file->addAction(action_file_exit);

    /* Edit Menu */

    QAction *action_edit_pref = new QAction(QIcon(":icons/prefs.png"), "Preferences");
    connect(action_edit_pref, &QAction::triggered, this, [&]() { this->_openPrefs(); });
    menu_edit->addAction(action_edit_pref);

    /* View Menu */

    QMenu *action_view_overlay = new QMenu("Overlay");
    action_view_overlay->setIcon(QIcon(":/icons/overlay.png"));

    menu_view->addMenu(action_view_overlay);

    QAction *action_view_overlay_line = new QAction(QIcon(":/icons/"), "Line");
    action_view_overlay_line->setCheckable(true);
    action_view_overlay_line->setChecked(true);
    
    QAction *action_view_overlay_wave = new QAction(QIcon(":/icons/"), "Wave");
    action_view_overlay_wave->setCheckable(true);
    action_view_overlay_wave->setChecked(true);

    action_view_overlay->addAction(action_view_overlay_line);
    action_view_overlay->addAction(action_view_overlay_wave);
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
            drawer->btn_sonify->setEnabled(true);
            
            addRecentFile(_fileName);
        }
        else
        {
            statusbar->Msg("Could not open file!", 5);
            // TODO: Handle file open error
        }
    }
    else
    {
        if(path != "" and QFile::exists(path))
        {
            statusbar->Msg("Image loaded", 5);
            this->_fileName = path;
            canvas->SetImage(_fileName);
            statusbar->SetFile(_fileName);
            drawer->btn_sonify->setEnabled(true);
            
            addRecentFile(_fileName);
        }
        else
        {
            statusbar->Msg("Could not open file!", 5);
            // TODO: Handle file open error
        }
    }
}

void sonify::ReadRecentFiles()
{
    std::ifstream file;
    file.open("recentfiles.txt", std::ios_base::in);
    std::string line;
    QString name;
    for(; getline(file, line); )
    {
        name = QString::fromStdString(line);
        QAction *action = new QAction(name);
        connect(action, &QAction::triggered, this, [=]() { this->_openImage(name); });
        action_file_open_recent->addAction(action);
    }
}

void sonify::addRecentFile(QString filePath)
{
    _recentFiles.open("recentfiles.txt", std::ios_base::app);
    _recentFiles << filePath.toStdString() << "\n";
}

void sonify::_openPrefs()
{
    // static Preferences prefs;
    // prefs.show();
}

void sonify::Selection()
{

}

sonify::~sonify() {}

#endif
