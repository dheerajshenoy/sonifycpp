#include "aboutdialog.hpp"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setLayout(m_layout);
    m_layout->addWidget(m_name_label);

    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    m_name_label->setFont(QFont("ShureTechMono Nerd Font Mono", 18));
    m_name_label->setAlignment(Qt::AlignmentFlag::AlignCenter);
    m_layout->addWidget(m_done_btn);

    connect(m_done_btn, &QPushButton::clicked, this, [&]() {
        this->close();
    });

    m_layout->addWidget(m_info_label);
    m_layout->addWidget(m_git_btn);

    connect(m_git_btn, &QPushButton::clicked, this, [&]() {
    });

}

AboutDialog::~AboutDialog()
{}
