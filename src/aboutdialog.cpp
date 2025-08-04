#include "aboutdialog.hpp"

#include <QDesktopServices>
#include <qdesktopservices.h>

AboutDialog::AboutDialog(QWidget *parent) noexcept : QDialog(parent)
{
    QVBoxLayout *layout   = new QVBoxLayout();
    QLabel *name_label    = new QLabel("SonifyCPP");
    QPushButton *done_btn = new QPushButton("Done");
    QLabel *info_label    = new QLabel("Convert images to audio");
    QPushButton *git_btn  = new QPushButton("Git");

    this->setLayout(layout);
    layout->addWidget(name_label);

    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    name_label->setFont(QFont("ShureTechMono Nerd Font Mono", 18));
    name_label->setAlignment(Qt::AlignmentFlag::AlignCenter);
    layout->addWidget(done_btn);

    connect(done_btn, &QPushButton::clicked, this, [&]() { this->close(); });

    layout->addWidget(info_label);
    layout->addWidget(git_btn);

    connect(git_btn, &QPushButton::clicked, this, [&]()
    {
        QDesktopServices::openUrl(
            QUrl("https://github.com/dheerajshenoy/sonifycpp"));
    });
}
